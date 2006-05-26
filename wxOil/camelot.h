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
#if !defined(_CAMELOT_H_)
#define _CAMELOT_H_

#if !defined(EXCLUDE_FROM_XARLIB)

#include "dlgmgr.h"     

class CCamFrame;

class CCamApp : public wxApp
{
public:
	CCamApp();

	bool OnInit();
	INT32 OnExit();
	void OnAppExit();

    wxMDIChildFrame *CreateChildFrame( wxDocument *doc, wxView *view );

	// Functions required to enable/disable the systems functionality.   
	static void DisableSystem(); 
    static void EnableSystem();
	static inline bool IsDisabled() {return s_bIsDisabled;}

    // Display our about box             
	static void DoAboutBox();
	
    // Used to access the dialog manager             
    static DialogManager *GetDlgManager();

	// Access the DocManager for templates
	wxDocManager *GetDocumentManager() const
	{
		return m_docManager.get();
	}
	
	bool OnRecentFile( INT32 RecentFileNumber );
	bool GetRecentFileText( INT32 Index, String_256* pszText );

    void OnIdle		( wxIdleEvent  &event );
	void OnTimer	( wxTimerEvent &event );

	bool HandleKeyPress( wxKeyEvent &event );

	void OnHelpIndex();

	void GiveActiveCanvasFocus();
	
	int /*TYPENOTE: Correct*/ FilterEvent( wxEvent& event );

	void OnFatalException();

	static BOOL LaunchWebBrowser(const wxString& strUrl);

private:
	INT32 RunFalseMainLoop();

protected:
	std::auto_ptr<wxDocManager> m_docManager;
	CCamFrame			   *m_pMainFrame;

	static bool			s_bIsDisabled;

	static wxString		m_strResourcePath;

	wxTimer				m_Timer;

private:
	static BOOL LaunchBrowserApp(const wxString& strAppName, wxString strCommand);

	static DialogManager m_DlgMgr; 		// The dialog manager handles all oily dialog functions

	static BOOL InInitOrDeInit;

	DECLARE_EVENT_TABLE()

public:
	// Window
	static BOOL MainWndMaximized;					// user maximized preference
	static BOOL MainWndMinimized;					// user maximized preference
	static String_256 MainWndPosString;				// user window position

	static BOOL bFirstRun;							// TRUE if this is the first time the app has been run
													// (first time with this set of preferences)
	BOOL allowRegistryWrite;						// if a copy of camelot is open, and we try do load another one,
													// and exit (we prevent this), then we don't wan't to write to
													// the registry !!!!
	static void NeedMoreIdles() { ::wxWakeUpIdle(); }
};

DECLARE_APP( CCamApp )

#endif

// Used to determine target platform at runtime.
#if defined(__WXMSW__)
extern BOOL WinArray[];
enum Winver { Winver32s=0, Winver32c=1 };
inline BOOL IsWin32s()	{	return WinArray[Winver32s];		}
inline BOOL IsWin32c()	{	return WinArray[Winver32c];		}
inline BOOL IsWin32NT()	{	return !WinArray[Winver32s];	}

inline BOOL IsWindows95() { return IsWin32s() && IsWin32c();  		}
inline BOOL IsWindows31() { return IsWin32s() && !IsWin32c(); 		}
inline BOOL IsWindowsNT() { return !IsWindows95() && !IsWindows31(); }

// Matt 16/01/2001 - Added so that we can detect NT5/Win2000
inline BOOL IsWindows2000()
{
	DWORD dwVersion = GetVersion();
	return ((dwVersion < 0x80000000) && ((DWORD)(LOBYTE(LOWORD(dwVersion))) == 5));
}

#endif

#endif //_CAMELOT_H_
