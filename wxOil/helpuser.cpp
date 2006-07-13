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
/*

	helpuser.cpp

	Routines to invoke the platform's help system for a given Camelot object or
	topic reference.

*/

#include "camtypes.h"

//#include <io.h>			// for FileExists
#include <stdlib.h>
#include "camelot.h"
//#include "mainfrm.h"
//#include "dialogop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errorbox.h"

#include "helpuser.h"
#include "helptabs.h"
//#include "xshelpid.h"			// for the _R(IDH_TEST_PAGE)
#include "product.h"			// for the PRODUCT_BASENAME
//#include "resdll.h"
//#include "helpdownload.h"

//#include "simon.h"				// for the _R(IDS_HELP) button label string

//#include <htmlhelp.h>			// for html help
#include "keypress.h"


// Code version.
DECLARE_SOURCE("$Revision$");


// We need to use the correct filename parsing functions, depending on whether we are
// a C/ASCII/ANSI or a Unicode build.  Note that under Win32 all exported functions
// are specified in the ANSI character set, even in Unicode builds.
#undef SPLITPATH
#undef MAKEPATH
#undef GETSHORTPATHNAME

#ifdef UNICODE
	// Wide-character versions.
	#define SPLITPATH			(_wsplitpath)
	#define MAKEPATH			(_wmakepath)
	#define GETSHORTPATHNAME	("GetShortPathNameW")
#else
	// C/ASCII/ANSI versions.
	#define SPLITPATH			(_tsplitpath)
	#define MAKEPATH			(_tmakepath)
	#define GETSHORTPATHNAME	("GetShortPathNameA")
#endif


// DialogOps derived from this class are "tabbed", and require special processing.
#undef  TABBED_DIALOG_CLASS
#define TABBED_DIALOG_CLASS		(CC_RUNTIME_CLASS(DialogTabOp))


// This is the window class name for push buttons.
#undef  BUTTON_CLASS
#define BUTTON_CLASS			(TEXT("BUTTON"))


// This determines the maximum time, in seconds, the app will wait on shutdown for the
// NT launcher to pass a quit message to the 16-bit help engine on Windows NT.
#define NTLAUNCHER_TIMEOUT		(3)



///////////////////////////////////////////////////////////////////////////////////////////
//	Implementation.

// These store the full paths to the help-file
static String_256		achzHelpPath;

PORTNOTE("help", "Help function unimplemented!")
#if !defined(EXCLUDE_FROM_XARALX)
static TCHAR achzMoviesHelpPath[_MAX_PATH];
// static TCHAR achzStubPath[_MAX_PATH];

#ifdef STANDALONE
static TCHAR achzSpecPath[_MAX_PATH];
#endif

// This holds a handle to our message filter hook that traps F1 being pressed in dialogs.
static HHOOK hF1Hook = NULL;
#endif

// This is set to TRUE if we use the help at all.  It reminds us to shut the help engine
// down when we quit.
static BOOL fHaveUsedHelp = FALSE;

#ifdef STANDALONE
static BOOL fHaveUsedSpecHelp = FALSE;
#endif

// This remembers that last format message resource ID passed to MakeMsg.  If we have to
// provide help on an error box with a "error ID" of zero, we use this value instead,
// hoping that it contains the ID of the string that was used to format the message.
static UINT32 nNextMessageHelpContext = 0;

// This flag prevents a bad-install error message being shown repeatedly.
// static BOOL fNoNTLauncherMsgShown = FALSE;



/********************************************************************************************
>	static DWORD LookupDialogTopic(LPCTSTR lpcszLookupClass, UINT32 nPageResID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		lpcszLookupClass			the run-time class name of the dialog to find
											on-line help about. Can be null - in which case
											we just check the PageID
				nPageResID					the resource ID of the current page, if we
											want help on a 'tabbed' dialog
	Returns:	The index ID of the appropriate help topic, or zero if there isn't one.
	Purpose:	Private helper function that finds the help topic associated with "named"
				dialogs, ie. those dialogs that are managed by a specialised C++ class, not
				error or warning dialogs.
	SeeAlso:	HelpUser
********************************************************************************************/

static DWORD LookupDialogTopic(LPCTSTR lpcszLookupClass, UINT32 nPageResID)
{
	// Check we're not being passed junk.
//	ERROR3IF(lpcszLookupClass == NULL, "No dialog class in LookupDialogTopic");
		
	// Scan through the whole table.
	for (DialogHelpTuple* ptpl = atplDialogTopics;
		 ptpl->lpcszDialogClass != NULL;
		 ptpl++)
	{
		// If the class-names & page ID's match then return the associated topic index.
		if ((lpcszLookupClass == NULL || ::camStricmp(ptpl->lpcszDialogClass, lpcszLookupClass) == 0) &&
			ptpl->nPageResID == nPageResID)
		{
			return ptpl->dwTopicIndex;
		}
	}

	// Couldn't find such a dialog class.
	return 0;
}



/********************************************************************************************
>	static DWORD LookupOperationTopic(LPCTSTR lpcszOpName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/95
	Inputs:		lpcszOpName				the OPTOKEN value of the Operation to look up help
										for
	Returns:	The index ID of the appropriate help topic, or zero if there isn't one.
	Purpose:	Private helper function that finds the help topic associated with
				Camelot operations, eg. those things exposed in menus!
	SeeAlso:	HelpUser
********************************************************************************************/

#if !defined(EXCLUDE_FROM_XARALX)
static DWORD LookupOperationTopic(LPCTSTR lpcszOpName)
{
PORTNOTETRACE("help", "Help function unimplemented!");
	// Check for junk.
	ERROR3IF(lpcszOpName == NULL, "No valid OpToken in LookupOperationTopic");

	// Scan through the whole table.
	for (OpHelpTuple* ptpl = atplOpTopics;
		 ptpl->lpcszToken != NULL;
		 ptpl++)
	{
		// If the token values match then we've found our help topic.
		if (::camStricmp(ptpl->lpcszToken, lpcszOpName) == 0)
		{
			return ptpl->dwTopicIndex;
		}
	}

	// No help topic, so sorry.
	return 0;
}
#endif



/********************************************************************************************
>	static DWORD LookupMessageTopic(UINT32 nMessageID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		nMessageID			the resource identifier of the message displayed in an
									error or warning dialog
	Returns:	The index ID of the appropriate help topic, or zero if there isn't one.
	Purpose:	Private helper function that finds the help topic associated with the given
				message, generally diaplayed in an error or warning dialog.
	SeeAlso:	HelpUser
********************************************************************************************/

#if !defined(EXCLUDE_FROM_XARALX)
static DWORD LookupMessageTopic(UINT32 nMessageID)
{
PORTNOTETRACE("help", "Help function unimplemented!");
#ifndef STANDALONE
	// If we have no message ID then use the last one passed to MakeMsg.
	if (nMessageID == 0) nMessageID = nNextMessageHelpContext;

	// Start at the beginning of the table, naturally . . .
	MsgHelpTuple* ptpl = atplMsgTopics;
	DWORD dwHelpTopic;

	// The table is terminated by a zero topic index.
	while ((dwHelpTopic = *ptpl++) != 0)
	{
		// Each sub-table is terminated by a zero message ID.
		while (*ptpl != 0)
		{
			// If one of the following matches the message then return the help topic.
			if (*ptpl++ == nMessageID) return dwHelpTopic;
		}

		// Skip over the terminating message ID.
		ptpl++;
	}
#endif
	// No help topic for this message, so sorry.
	return 0;
}
#endif


/********************************************************************************************
>	static BOOL FakeHelpButtonClick(HWND hwndDialog)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95
	Inputs:		hwndDialog			the dialog box to fake a help button click within
	Returns:	TRUE if the fake click was possible, FALSE otherwise (maybe the dialog
				doesn't have a help button?)
	Purpose:	Searches through the child windows of the given dialog, looking for a push
				button with the label "Help".  If it finds one then the routine posts a
				"faked" click message for the button, so help is invoked.
	SeeAlso:	F1HookProc
********************************************************************************************/

PORTNOTE("help", "Help function unimplemented!")
#if !defined(EXCLUDE_FROM_XARALX)
static BOOL FakeHelpButtonClick(HWND hwndDialog)
{
	// Make sure we've been passed a genuine window handle.
	if (!::IsWindow(hwndDialog)) return FALSE;

	// Get the text of a "help" button.
	String_256 strHelpLabel(_R(IDS_HELP));

	// Search through all child windows of the given dialog, looking for a push-button
	// with the label "Help".
	for (HWND hwndChild = ::GetWindow(hwndDialog, GW_CHILD);
		 hwndChild != NULL;
		 hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT))
	{
		// Find out the window class of the child window.
		TCHAR achzClassName[256];
		::GetClassName(hwndChild, achzClassName, 256);

		// If it is a button then examine the label.
		if (camStricmp(achzClassName, BUTTON_CLASS) == 0)
		{
			// It is a button.  Get its label.
			TCHAR achzButtonLabel[256];
			::GetWindowText(hwndChild, achzButtonLabel, 256);
			
			// Does its label match the "help" label?
			if (camStricmp(achzButtonLabel, strHelpLabel) == 0)
			{
				// It does!  Fake a click on it and return.
				return ::PostMessage(hwndDialog,
									 WM_COMMAND,
							  		 (WPARAM) MAKEWPARAM(::GetDlgCtrlID(hwndChild), BN_CLICKED),
							  		 (LPARAM) hwndChild);
			}
		}
	}

	// No help button found, can't do it.
	return FALSE;
}



/********************************************************************************************
>	static LRESULT CALLBACK EXPORT F1HookProc(INT32 nCode, WPARAM wParam, LPARAM lParam)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95
	Inputs:		nCode			where the message was generated, eg. a menu, or a dialog
				wParam			(ignored)
				lParam			contains a pointer to a MSG structure describing the event
	Returns:	Whatever the next hook in the chain returns.
	Purpose:	Called when a message is generated within the app.  Checks if the message
				was from a dialog box, and if so checks if it is the F1 key being pressed.
				If it is then we work out which dialog has the "focus" and simulate its
				help button being clicked.
	SeeAlso:	InitUserHelp; DeInitUserHelp
********************************************************************************************/

static LRESULT CALLBACK EXPORT F1HookProc(INT32 nCode, WPARAM wParam, LPARAM lParam)
{
	// Check if it's the F1 key going down within a dialog box . . .
	LPMSG lpmsg = (LPMSG) lParam;
	BOOL fHandleOK = ::IsWindow(lpmsg->hwnd);
	if (nCode == MSGF_DIALOGBOX && lpmsg->message == WM_KEYDOWN && lpmsg->wParam == CAMKEY(F1))
	{
		// Simulate the help button within the dialog being clicked, if there is one.
		// Empirically I have discovered that the window handle contained within the
		// message is that of the first child window in the dialog.
		if (fHandleOK) FakeHelpButtonClick(::GetParent(lpmsg->hwnd));
	}

	// Pass this event on to the next hook proc if it's valid.
	return (fHandleOK) ? ::CallNextHookEx(hF1Hook, nCode, wParam, lParam) : 0;
}
#endif


/********************************************************************************************
>	static BOOL RunOilHelp(LPCTSTR lpcszHelpPath, UINT32 nCommand, DWORD dwData)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		lpcszHelpPath		string containing the path to the helpfile to display.
				nCommand			the command identifier to pass to the help engine, eg
									HELP_CONTEXT or HELP_CONTENTS.
				dwData				extra data to pass to the help engine, eg. the topic
									index, or zero.
	Returns:	TRUE if the help engine was run successfully, FALSE otherwise.
	Purpose:	Private helper function that runs the system's help engine, whatever that
				might be, passing the given command.  On Windows this is WINHELP.EXE or
				WINHLP32.EXE, on RISCOS it's get-the-manual-out time . . .
	SeeAlso:	HelpUser
********************************************************************************************/

static BOOL RunOilHelp(LPCTSTR lpcszHelpPath, UINT32 nCommand, DWORD dwData)
{
#if !defined(EXCLUDE_FROM_XARALX)
	// Firing up the help engine can take a while, so show an hour-glass cursor.
	HCURSOR hOldCursor;
	HCURSOR hBusyCursor = ::LoadCursor(NULL, _R(IDC_WAIT));
	if (hBusyCursor != NULL) hOldCursor = ::SetCursor(hBusyCursor);

	// We always pass the handle of the main-frame window as the "owning" window.
	HWND hwndInvoker = GetMainFrame()->GetSafeHwnd();
#endif

	// If we are running on a pure 32-bit platform such as Windows NT then we must fake
	// the WinHelp() function, as we want the 16-bit engine to be run, not WINHLP32.EXE.
	// The current help-file build includes references to 16-bit DLLs that seem to be
	// almost too much for the 32-bit engine.  We don't have to do this if we aren't
	// running the engine to browse a file, but simply issuing a command, such as
	// HELP_HELPONHELP.
	BOOL fOk;
	
		// Running under Chicago or Win32s, or the helpfile parameter is NULL, so run the
		// "normal" engine.  Here we should really "thunk" (translate) the 32-bit HWND
		// to a 16-bit HWND, I think?
//		TRACEUSER( "JustinF", _T("Help being invoked normally\n"));


		fOk = TRUE;
		if(nCommand != HELP_QUIT)
		{
			wxString strHelpFileName(lpcszHelpPath);
			wxString Context = _T("");
			if (dwData)
			{
				Context = PCTSTR(String_256( dwData ));
				if (_T("") != Context)
					// so that the string table contains a bit less, the ::/xarax/ and .htm
					// are here, because they are common to every HTML help path.
					strHelpFileName += _T("xarax/") + Context + _T(".htm");
			}

			CCamApp::LaunchWebBrowser( strHelpFileName );
		}

#if !defined(EXCLUDE_FROM_XARALX)
	// Get some slightly useful information if it all goes horribly wrong.
#ifdef _DEBUG
	if (!fOk) TRACEUSER( "Ollie", _T("RunOilHelp failed - last error: %lu\n"),
										(UINT32) ::GetLastError());
#endif

	// Undo the hour-glass cursor and return a success code.
	if (hBusyCursor != NULL) ::SetCursor(hOldCursor);
#endif

	return fOk;
}



/********************************************************************************************
>	BOOL MakeShortPath(LPTSTR lpszPath, size_t cchMaxLen)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/95
	Inputs:		lpszPath		the full path to shorten.  The shortened version will be
								"outputed" into this same buffer.
				cchMaxLen		the size of the above buffer
	Outputs:	lpszPath		contains the shortened path
	Returns:	TRUE if successful, FALSE otherwise.
	Purpose:	Special function to work around an ommission in Win32s, which does not
				include the GetShortPathName function, even though Win32s is supposed to
				export ALL Win32 functions.  This version does nothing under Win32s, which
				shortens pathnames automatically (?), under other versions of Windows it
				dynamically links to the function in the kernel32 library, thus avoiding
				an explicit link to the function which would prevent the app loading.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL MakeShortPath(LPTSTR lpszPath, size_t cchMaxLen)
{
PORTNOTETRACE("help", "Help function unimplemented!");
#if !defined(EXCLUDE_FROM_XARALX)
	// Under Win32s we don't want to do this, because (i) Win32s does it anyway;
	// and (ii) the bloody GetShortPathName function isn't supported because they
	// forgot it (a serious bug, Mr Gates!)
	if (IsWin32s() && !IsWin32c()) return TRUE;

	// Now we have some aggro.  Because GetShortPathName isn't recognised as a Win32
	// function by Win32s, we must NOT have any explicit references to it in the program,
	// or the app won't load under Win32s.  Instead, we will try to dynamically link to it.
	HMODULE hlib = ::GetModuleHandle(TEXT("KERNEL32"));
	if (hlib == NULL)
	{
		TRACEUSER( "Ollie", _T("MakeShortPath: GetModuleHandle failed (error: %lu)\n"),
								(UINT32) ::GetLastError());
		return FALSE;
	}

	// OK, we've loaded the relevant library.  Now try to link to the function it contains.
	typedef DWORD (WINAPI *GSPNFUNC)(LPCTSTR, LPTSTR, DWORD);
	GSPNFUNC lpfn = (GSPNFUNC) ::GetProcAddress(hlib, GETSHORTPATHNAME);
	if (lpfn == NULL)
	{
		TRACEUSER( "Ollie", _T("MakeShortPath: GetProcAddress failed (error: %lu)\n"),
								(UINT32) ::GetLastError());
		return FALSE;
	}

	// Now call the GetShortPathName function and return a success code.
	DWORD dwResult = (*lpfn)(lpszPath, lpszPath, cchMaxLen);
	return dwResult != 0 && dwResult < cchMaxLen;
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	static BOOL ShowHelp(UINT32 nCommand, DWORD dwData)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/95
	Inputs:		nCommand			the command to pass to the help engine, eg. HELP_CONTENTS
	Returns:	TRUE if successful.
	Purpose:	Called to display the given topic in the Xara Studio general help file.
				Makes a note if help is run successfully that we have to shut it down when
				we quit.
********************************************************************************************/

static BOOL ShowHelp(UINT32 nCommand, DWORD dwData)
{
	BOOL fResult = RunOilHelp(achzHelpPath, nCommand, dwData);
	if (!fHaveUsedHelp && fResult) fHaveUsedHelp = TRUE;
	return fResult;
}




///////////////////////////////////////////////////////////////////////////////////////////
//	Interface.

/********************************************************************************************
>	BOOL HelpUser(const DialogOp& DlgOp)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		DlgOp				reference to a (constant) dialog operation to display
									help about
	Returns:	TRUE if help was successfully provided, FALSE if not.
	Purpose:	Public function that finds the help topic associated with the given dialog
				operation and runs the system help engine to display the topic.
	SeeAlso:	RunOilHelp; LookupDialogTopic
********************************************************************************************/

BOOL HelpUser(const DialogOp& DlgOp)
{
	// Work out which page within the dialog is visible, if appropriate.  We begin by
	// assuming this isn't relevant.
	UINT32 nPageResID = 0;
	if (DlgOp.IsKindOf(TABBED_DIALOG_CLASS))
	{
		// Work out which is the active (top-most) page within the tabbed dialog.
		wxWindow* pWnd = (wxWindow*) DlgOp.WindowID;
		if (pWnd != NULL)
		{
			// We got the window handle, get its MFC CWnd analogue.
			wxBookCtrlBase* pSheet = DialogManager::GetBookControl( pWnd );
			if (pSheet != NULL)
			{
				// Ask it for the resource ID of its currently active page.
				nPageResID = (UINT32) pSheet->GetCurrentPage()->GetId();
			}
#ifdef _DEBUG
			else
			{
				TRACEUSER( "Ollie", _T("Null OurPropSheet pointer in HelpUser\n"));
			}
#endif
		}
#ifdef _DEBUG
		else
		{
			TRACEUSER( "Ollie", _T("Null DialogOp window handle in HelpUser\n"));
		}
#endif
	}

	// Get the run-time class name etc of the dialog.
	LPCTSTR lpcszDialogClass = DlgOp.GetRuntimeClass()->m_lpszClassName;

	// Look-up the class name/page ID in our list of help topics.  If we can't find it
	// then we return a failure code.
	DWORD dwHelpIndex = LookupDialogTopic(lpcszDialogClass,	nPageResID);
	if (dwHelpIndex == 0)
	{
		TRACEUSER( "Ollie", _T("Can't find help topic for %s dialog (page ID %lu)\n"),
								(LPTSTR) lpcszDialogClass, (UINT32) nPageResID);
		return FALSE;
	}

	// Show this topic in the help system and return a success code.
	return ShowHelp(HELP_CONTEXT, dwHelpIndex);
}



/********************************************************************************************
>	BOOL HelpUser(const CDialog& dlg)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		dlg	  		the MFC dialog that requires help
	Returns:	TRUE if help was successfully provided, FALSE if not.
	Purpose:	Public function that finds the help topic associated with the given MFC
				dialog and runs the system help engine to display the topic.
	SeeAlso:	RunOilHelp; LookupDialogTopic
********************************************************************************************/

BOOL HelpUser(const CDialog& dlg)
{
PORTNOTETRACE("help", "Help function unimplemented!");
#if !defined(EXCLUDE_FROM_XARALX)
	// Look-up the class name/page ID in our list of help topics.  If we can't find it
	// then we return a failure code.
	DWORD dwHelpIndex = LookupDialogTopic(dlg.GetRuntimeClass()->m_lpszClassName, 0);
	if (dwHelpIndex == 0)
	{
		TRACEUSER( "Ollie", _T("Can't find help topic for the %s MFC dialog\n"),
								(LPCTSTR) dlg.GetRuntimeClass()->m_lpszClassName);
		return FALSE;
	}

	// Show this topic in the help system and return a success code.
	return ShowHelp(HELP_CONTEXT, dwHelpIndex);
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	BOOL HelpUser(const OpDescriptor& opdesc)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		opdesc	  		the OpDescriptor of the Operation to provide help for
	Returns:	TRUE if help was successfully provided, FALSE if not.
	Purpose:	Public function that finds the help topic associated with the given
				OpDescriptor and runs the system help engine to display the topic.
	SeeAlso:	RunOilHelp; LookupOperationTopic
********************************************************************************************/

BOOL HelpUser(const OpDescriptor& opdesc)
{
PORTNOTETRACE("help", "Help function unimplemented!");
#if !defined(EXCLUDE_FROM_XARALX)
	// Check if a help topic is already recorded within the OpDescriptor.  If it isn't
	// then we will have to look it up instead.
/*
	//  NB. all the help topic ID's within the OpDescriptors are wrong, so ignore them!
	DWORD dwHelpIndex = ((OpDescriptor&) opdesc).GetHelpId();
	if (dwHelpIndex == 0) dwHelpIndex = LookupOperationTopic(opdesc.Token);
*/
	DWORD dwHelpIndex = LookupOperationTopic(opdesc.Token);
	if (dwHelpIndex == 0)
	{
		TRACEUSER( "Ollie", _T("Can't find help topic for the %s OpDescriptor\n"),
								(LPCTSTR) opdesc.Token);
		return FALSE;
	}

	// Show the topic we have found.
	return ShowHelp(HELP_CONTEXT, dwHelpIndex);
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	BOOL HelpUser(UINT32 nMessageID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		nMessageID			the resource identifier of the message to display
									help about
	Returns:	TRUE if help was successfully provided, FALSE otherwise
	Purpose:	Public function that finds the help topic associated with the given
				message and runs the system help engine to display the topic.
	SeeAlso:	RunOilHelp; LookupMessageTopic
********************************************************************************************/

BOOL HelpUser(UINT32 nMessageID)
{
PORTNOTETRACE("help", "Help function unimplemented!");
#if !defined(EXCLUDE_FROM_XARALX)
	// Look-up the message ID in our list of help topics.  If we can't find it
	// then return a failure code.
	DWORD dwHelpIndex = LookupMessageTopic(nMessageID);
	if (dwHelpIndex == 0)
	{
		TRACEUSER( "Ollie", _T("Can't find help topic for message 0x%lX\n"),
								(UINT32) nMessageID);
		return FALSE;
	}

	// Show this topic in the help system and return a success code.
	return ShowHelp(HELP_CONTEXT, dwHelpIndex);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL HelpUserPropertyPage(UINT32 PageID);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>	
	Created:	7/5/97
	Inputs:		PageID		ResourceID of the currently active page
	Returns:	TRUE if help was successfully provided, FALSE otherwise
	Purpose:	Public function that finds the help topic associated with the given
				property page. This is required due to the new way the tabbed dialogs work
				(via MFC).

********************************************************************************************/

BOOL HelpUserPropertyPage(UINT32 PageID)
{
PORTNOTETRACE("help", "Help function unimplemented!");
#if !defined(EXCLUDE_FROM_XARALX)
	DWORD dwHelpIndex = LookupDialogTopic(NULL, PageID);
	return (HelpUserTopic(dwHelpIndex));
#else
	return FALSE;
#endif
}

/********************************************************************************************

  >	BOOL HelpUserTopic(DWORD dwHelpIndex)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		nMessageID			the resource identifier of the message to display
									help about
	Returns:	TRUE if help was successfully provided, FALSE otherwise
	Purpose:	Public function that finds the help topic associated with the given
				message and runs the system help engine to display the topic.

********************************************************************************************/

BOOL HelpUserTopic(DWORD dwHelpIndex)
{
PORTNOTETRACE("help", "Help function unimplemented!");
#if !defined(EXCLUDE_FROM_XARALX)
	if (dwHelpIndex == 0)
		return FALSE;
	
	// Show this topic in the help system and return a success code.
	return ShowHelp(HELP_CONTEXT, dwHelpIndex);
#else
	return FALSE;
#endif
}


/********************************************************************************************
>	BOOL _HelpUser(LPCTSTR lpcszClassName, UINT32 nSubTopic = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/95
	Inputs:		lpcszClassName				the class name of the object to run help for
				nSubTopic					which sub-topic for the object to display
											(by default, no sub-topic)
	Returns:	TRUE if the help system is started successfully.
	Purpose:	Public function that finds the help topic associated with the given
				object and runs the system help engine to display it.  This is a
				"bodgey" direct-access function for invoking help in those tricky cases,
				such as the print-setup dialog, that do not have any Camelot object
				directly associated with them.
	SeeAlso:	HelpUser; LookupDialogTopic; RunOilHelp
********************************************************************************************/

BOOL _HelpUser(LPCTSTR lpcszClassName, UINT32 nSubTopic /* = 0 */)
{
PORTNOTETRACE("help", "Help function unimplemented!");
#if !defined(EXCLUDE_FROM_XARALX)
	DWORD dwHelpIndex = LookupDialogTopic(lpcszClassName, nSubTopic);
	if (dwHelpIndex == 0)
	{
		TRACEUSER( "Ollie", _T("Can't find raw help topic for %s (sub %lu)\n"),
									lpcszClassName, nSubTopic);
		return FALSE;
	}

	return ShowHelp(HELP_CONTEXT, dwHelpIndex);
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	BOOL HelpContents()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Returns:	TRUE if successful.
	Purpose:	Runs the system help engine, displaying the "Contents" page.
	Errors:		-
	SeeAlso:	HelpUser; HelpSearch; HelpUsingHelp
********************************************************************************************/

BOOL HelpContents()
{

PORTNOTETRACE("help", "Help function unimplemented!");
#if !defined(EXCLUDE_FROM_XARALX)
#ifndef STANDALONE

#if (_MFC_VER <= 0x300)
	#define HELP_FINDER		0xB
#endif // MFC_VER

	// If F1 was pressed, popup help in the context of the current tool. Use the tool's OpToken (of 
	// the form "TOOL<ToolId>") to look-up its help page. Fixes #10489
	if (KeyPress::IsKeyPressed(CAMKEY(F1)))
	{
		String OpToken;
		wsprintf(OpToken, "TOOL%u", (Tool::GetCurrentID()));
		DWORD dwHelpIndex = LookupOperationTopic(OpToken);
		return ShowHelp(HELP_CONTEXT, dwHelpIndex);
	}

	return ShowHelp(HELP_FINDER, 0);

#else // STANDALONE
	// On the viewer go directly to the contents page on all OS's
	#ifdef WEBSTER
		return ShowHelp(HELP_CONTEXT, _R(IDH_Contents));
	#else //WEBSTER
		return ShowHelp(HELP_CONTEXT, _R(IDH_Misc_Contents));
	#endif // WEBSTER
#endif // STANDALONE
#else
	return TRUE;
#endif // EXCLUDE_FROM_XARALX

}



/********************************************************************************************
>	BOOL HelpUsingHelp()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Returns:	TRUE if successful.
	Purpose:	Runs the system help engine, displaying the system's instructions for
				using it.
	SeeAlso:	HelpUser; HelpContents; HelpSearch
********************************************************************************************/

BOOL HelpUsingHelp()
{
PORTNOTETRACE("help", "Help function unimplemented!");
#if !defined(EXCLUDE_FROM_XARALX)
	return RunOilHelp(NULL, HELP_HELPONHELP, 0);
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	BOOL HelpUsingTools()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Returns:	TRUE if successful.
	Purpose:	Runs the system help engine, displaying the help on tools.
	SeeAlso:	HelpUser; HelpContents; HelpSearch
********************************************************************************************/

BOOL HelpUsingTools()
{
	return ShowHelp(HELP_CONTEXT, _R(IDH_Misc_Tools));
}



/********************************************************************************************
>	BOOL HelpUsingGalleries()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Returns:	TRUE if successful.
	Purpose:	Runs the system help engine, displaying the help on galleries.
	SeeAlso:	HelpUser; HelpContents; HelpSearch
********************************************************************************************/

BOOL HelpUsingGalleries()
{
	return ShowHelp(HELP_CONTEXT, _R(IDH_Overview_Galleries));
}



/********************************************************************************************
>	BOOL HelpOnlineDemos()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Returns:	TRUE if successful.
	Purpose:	Runs the system help engine, displaying the on-line demos help page.
	SeeAlso:	HelpUser; HelpContents; HelpSearch
********************************************************************************************/

BOOL HelpOnlineDemos()
{
PORTNOTETRACE("help", "Help function unimplemented!");
#if !defined(EXCLUDE_FROM_XARALX)
	return RunOilHelp(achzMoviesHelpPath, 0, 0);
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	BOOL HelpTechnicalSupport()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/95
	Returns:	TRUE if successful.
	Purpose:	Runs the system help engine, displaying the tech support help page.
	SeeAlso:	HelpUser; HelpContents; HelpSearch
*******************************************************************************************/

BOOL HelpTechnicalSupport()
{
PORTNOTETRACE("help", "Help function unimplemented!");
#if !defined(EXCLUDE_FROM_XARALX)
#ifdef WEBSTER
	return FALSE;
#else
	return ShowHelp(HELP_CONTEXT, _R(IDH_Misc_Tech_Support));
#endif
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	BOOL CanHelpUser(UINT32 nMessageID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		nMessageID			the resource identifier of the message that is being
									enquired about
	Returns:	TRUE if a help topic descrbing this message exists, FALSE otherwise.
	Purpose:	Public function to query whether a help topic exists for a given message.
				Depending on this the caller may display a help button in a message box,
				or may not etc.
	SeeAlso:	LookupMessageTopic; InformGeneral
********************************************************************************************/

BOOL CanHelpUser(UINT32 nMessageID)
{
PORTNOTETRACE("help", "Help function unimplemented!");
#if !defined(EXCLUDE_FROM_XARALX)
	// Just return TRUE if a help topic for the message exists.
	return LookupMessageTopic(nMessageID) != 0;
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	void SetNextMsgHelpContext(UINT32 nMessageID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		nMessageID				the last message format string passed to MakeMsg
	Purpose:	Remembers the last format string passed to MakeMsg.  If a call is made to
				InformWarning, InformError etc and no string resource ID is passed then we
				have a problem using the string resource as an index into the help file.
				This remembers that last one, which we use if the "error ID" is zero.
	SeeAlso:	StringBase::MakeMsg
********************************************************************************************/

void SetNextMsgHelpContext(UINT32 nMessageID)
{
	nNextMessageHelpContext = nMessageID;
	//TRACEUSER( "Andy", _T("\tSetNextMsgHelpContext(%d)\n"), nMessageID);
}



/********************************************************************************************
>	UINT32 GetNextMsgHelpContext()

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/00
	Inputs:		none
	Returns:	help context
	Purpose:	Gets the last help context set by SetNextMsgHelpContext
	SeeAlso:	used by ScreenView::OnActivateView
********************************************************************************************/

UINT32 GetNextMsgHelpContext()
{
	return nNextMessageHelpContext;
}



/********************************************************************************************
>	BOOL InitUserHelp()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Returns:	TRUE if successful.
	Purpose:	Initialises the help system.  This should generally be called before just
				about everything else, so help is available for startup problems.
	SeeAlso:	DeInitUserHelp
********************************************************************************************/

// Forward references to helper functions
BOOL InitHelpPath(BOOL bMainHelp);
BOOL HelpFileExists(LPTSTR FileName);

BOOL InitUserHelp()
{
#if !defined(EXCLUDE_FROM_XARALX)
	// If we have any deferred file copies, deal with them now...
	HelpDownloadOp::DoDeferredFileCopy();
#endif

	// init main help file:
	BOOL bOK = InitHelpPath(TRUE);

	// init movies help file
	bOK = bOK && InitHelpPath(FALSE);

#if !defined(EXCLUDE_FROM_XARALX)
	// Install a message filter hook to trap F1 being pressed in dialogs etc.  We don't need
	// one for menus as our "custom menu code" (ha ha ha) does this already.
	hF1Hook = ::SetWindowsHookEx(WH_MSGFILTER, F1HookProc, NULL, ::GetCurrentThreadId());
	ERROR3IF(hF1Hook == NULL, "Couldn't set F1 key hook in InitUserHelp");

	// Success!?!
	return hF1Hook != NULL;
#else
	return bOK;
#endif
}


// --------------------------------------------------------------------------------
BOOL InitHelpPath(BOOL bMainHelp)
{
	if( !bMainHelp )
	{
		PORTNOTETRACE("help", "InitHelpPath does not support Movie path yet");
	}

	// Get the locale id
	wxString	strLocale( setlocale( LC_MESSAGES, NULL ), wxConvUTF8 );
	INT32		ordSep = strLocale.Find( _T('_' ) );
	if ( -1 != ordSep )
		strLocale = strLocale.Left( ordSep );
	TRACEUSER( "jlh92", _T("Locale = %s\n"), PCTSTR(strLocale) );
	
	// Locale C is considered a synonym for en
	if( strLocale == _T("C") )
		strLocale = _T("en");

	// Check the help dir exists, if not bomb out
	wxString	strHelpPath( (PCTSTR)CCamApp::GetResourceDirectory() );
	strHelpPath += _T("/doc/");
	TRACEUSER( "jlh92", _T("Using filter discovery directory \"%s\"\n"), PCTSTR(strHelpPath + strLocale) );
	if( wxDir::Exists( strHelpPath + strLocale ) )
		strHelpPath += strLocale + _T("/");
	else
	{
		if( wxDir::Exists( strHelpPath + _T("en") ) )
			strHelpPath += _T("en/");
#if defined(_DEBUG)
		else
		{
			// We'll try default location under debug to make life easier
			strHelpPath = _T("/usr/share/xaralx/doc/en/");
			TRACEUSER( "jlh92", _T("Try = \"%s\"\n"), PCTSTR(strHelpPath) );
		}
#endif
	}

	achzHelpPath = strHelpPath;
	PORTNOTE( "help", "We should check and report non-existant help directory at some point" )
	return TRUE; // wxDir::Exists( strHelpPath );
}


/********************************************************************************************
>	BOOL DeInitUserHelp()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Returns:	TRUE if successful.
	Purpose:	Shuts down the help system.  This should be called as late as possible, so
				help is available for app shut-down problems.  Note that MFC will call
				WinHelp(HELP_QUIT) for us when the main frame window is destroyed.
	SeeAlso:	InitUserHelp
********************************************************************************************/

BOOL DeInitUserHelp()
{
PORTNOTETRACE("help", "Help function unimplemented!");
#if !defined(EXCLUDE_FROM_XARALX)
	// Tell the help engine we've finished using the help file(s).
	// NB. KNWON BUG: this doesn't do the job on Windows NT, leaving the helpfiles open.
	// As yet I have no idea why - it is as per the docs and works on Win32s & Chicago.
	if (fHaveUsedHelp)
	{
		TRACEUSER( "Ollie", _T("Closing general helpfile\n"));
		ShowHelp(HELP_QUIT, 0);
	}

#ifdef STANDALONE
	// Same if we ran the special helpfile.
	if (fHaveUsedSpecHelp)
	{
		TRACEUSER( "Ollie", _T("Closing special helpfile\n"));
		RunOilHelp(achzSpecPath, HELP_QUIT, 0);
	}
#endif

	// Remove our F1 message hook.
	return hF1Hook == NULL || ::UnhookWindowsHookEx(hF1Hook);
#else
	return TRUE;
#endif
}



/********************************************************************************************
>	BOOL ShowHelpSpec()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/95
	Returns:	TRUE if successful.
	Purpose:	Used by the viewer to display the Xara Studio special help file.  Makes a
				note if help is run successfully that we have to shut it down when we
				quit.
********************************************************************************************/

#ifdef STANDALONE

BOOL ShowHelpSpec()
{
PORTNOTETRACE("help", "Help function unimplemented!");
#if !defined(EXCLUDE_FROM_XARALX)
	BOOL fResult = RunOilHelp(achzSpecPath, HELP_CONTENTS, 0);
	if (!fHaveUsedSpecHelp && fResult) fHaveUsedSpecHelp = TRUE;
	return fResult;
#else
	return FALSE;
#endif
}

#endif	// STANDALONE
