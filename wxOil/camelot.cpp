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
/***************************************************************************************************************************/
//
// camelot.cpp - main application class derived from wxApp
//
/***************************************************************************************************************************/

#include "camtypes.h"

#include "camdoc.h"
#include "docview.h"
#include "camview.h"
#include "camframe.h"
//#include "Res/UKEnglish/stringtbl.h"
#include "kernel.h"
#include "grndrgn.h"
#include "oilmenus.h"

#if !defined(__WXMSW__)
#include "gdraw.h"
#endif

#include "cversion.h"
#include "camelot.h"

#include "keypress.h"
#include "ccdc.h"
#include "camprofile.h"
#include "dlgevt.h"
#include "cartprov.h"
#include "ctrllist.h"
#include "cartctl.h"
#include "xmlutils.h"
#include "camplatform.h"

//
// Define FILELIST for recent file list on file menu.
// Note that this currently seems to be rather unreliable.
//
//#if defined(_DEBUG)
//#define FILELIST
//#endif

//
// Define XARA_MENUGEN to use Xtreme style menu creation.
// Note that this currently doesn't have full functionality
// but it's getting there
//#if defined(_DEBUG)
#define XARA_MENUGEN
//#endif


/********************************************************************************************

	Preference:	MainWndMax
	Section:	Windows
	Range:		0 (FALSE) or 1 (TRUE)
	Purpose:	If non-zero then main window defaults to maximized, else normal position.
				This default will be overridden if the user explicitly asked for a mode
				when starting the app (e.g. minimized). If the user exits while maximized,
				the flag will be set ready for next time.

********************************************************************************************/
BOOL CCamApp::MainWndMaximized = FALSE;

/********************************************************************************************

	Preference:	MainWndMin
	Section:	Windows
	Range:		0 (FALSE) or 1 (TRUE)
	Purpose:	If non-zero then main window defaults to minimized, else normal position.
				This default will be overridden if the user explicitly asked for a mode
				when starting the app (e.g. minimized). If the user exits while minimized,
				the flag will be set ready for next time.

********************************************************************************************/
BOOL CCamApp::MainWndMinimized = FALSE;

/********************************************************************************************

	Preference:	MainWndPos
	Section:	Windows
	Range:		Six numbers, separated with spaces, which are x y w h sx sy
	Purpose:	Remembers users last 'normal' window position for them. Also remembers the
				screen size so can be scaled if next time runs in different screen mode.

********************************************************************************************/
String_256 CCamApp::MainWndPosString;

/***************************************************************************************************************************/
//
// Timer used for background redraw.
// For now we have gone back to using the idle system for redraw as it does seem to work
// The background redraw may be changed to use a timer or some other method
//

#define CAM_TIMER_ID		 42
#define CAM_TIMER_FREQUENCY	100

/***************************************************************************************************************************/

IMPLEMENT_APP( CCamApp )
BEGIN_EVENT_TABLE( CCamApp, wxApp )
	EVT_IDLE( CCamApp::OnIdle )
//	EVT_TIMER( CAM_TIMER_ID, CCamApp::OnTimer )
END_EVENT_TABLE()

DialogManager			CCamApp::m_DlgMgr;
bool					CCamApp::s_bIsDisabled = false; // Initially system is not disabled.
wxString				CCamApp::m_strResourcePath;


/***************************************************************************************************************************/

CCamApp::CCamApp()
{
}

/*********************************************************************************************
>	int TYPENOTE: Correct CCamApp::FilterEvent( wxEvent& event )

	Author:		Luke_Hart (Xara Group Ltd) <LukeH@xara.com>
	Created:	09/05/06
	Inputs:		A reference to the event to filter
	Outputs:	-
	Returns:	-
	Purpose:	This function is used to collect the key press information for handling by
				applictaion. It passes event unmolested for controls that are allowed focus and
				controls within modal dialogs. It also has code to make sure that key events
				that are passed on aren't checked again, based on the event timestamp. All other 
				keys are passed into application keypress handling code.
	Errors:		-
	Scope:	    Protected
	SeeAlso:	The focus handling document

**********************************************************************************************/ 
int /*TYPENOTE: Correct*/ CCamApp::FilterEvent( wxEvent& event )
{
	static INT32	lLastTimeStamp = 0;

	if( event.GetEventType() == wxEVT_CHAR )
	{
		wxObject* pEventObject = event.GetEventObject();
		if (pEventObject)
		{
			TRACEUSER( "jlh92", _T("KeyEvent 4 %s CH\n"), 
				((wxWindow*)pEventObject)->GetClassInfo()->GetClassName() );
		}
	}

	if( event.GetEventType() == wxEVT_KEY_DOWN ||
		event.GetEventType() == wxEVT_KEY_UP )
	{
		// Use timestamp to detect events which are passing
		// down the chain (which we've tested)
		if( lLastTimeStamp == event.GetTimestamp() )
			return -1;
		lLastTimeStamp = event.GetTimestamp();

		wxObject* pEventObject = event.GetEventObject();
		TRACEUSER( "jlh92", _T("KeyEvent 4 %s %s\n"), 
			((wxWindow*)pEventObject)->GetClassInfo()->GetClassName(),
			event.GetEventType() == wxEVT_KEY_DOWN ? _T("KD") : _T("KU") );
		
		// Is the object allowed to recieve keys? 
		wxClassInfo* pClassInfo = pEventObject->GetClassInfo();
		if( pClassInfo->IsKindOf( CLASSINFO(wxTextCtrl) ) ||
			pClassInfo->IsKindOf( CLASSINFO(wxComboBox) ) ||
			pClassInfo->IsKindOf( CLASSINFO(wxOwnerDrawnComboBox) ) ||
			pClassInfo->IsKindOf( CLASSINFO(wxComboControl) )
			)
		{
			TRACEUSER( "jlh92", _T("Control gets keys") );
			// Yes, pass on as usual
			return -1;
		}

		// Scan down ancestors looking for either wxPanels (always non-modal) and
		// wxDailogs (can be modal, so we check)
		wxWindow *pWnd = (wxWindow*)pEventObject;
		while( NULL != pWnd && !pWnd->IsKindOf( CLASSINFO(wxPanel) ) )
		{
			// Dialogs may-be modal so check
			if( pWnd->IsKindOf( CLASSINFO(wxDialog) ) )
			{
				// Pass event down chain if modal
				if( ((wxDialog*)pWnd)->IsModal() )
				{
					TRACEUSER( "jlh92", _T("Modal dialog\n") );
					return -1;
				}

				// Non-modal dialog so do focus handling
				break;
			}

			pWnd = pWnd->GetParent();
		}

		// Make sure the kernel knows which view/doc the event applies to, if any.
		if( NULL != Document::GetSelected() )
			Document::GetSelected()->SetCurrent();
		if( NULL != DocView::GetSelected() )
			DocView::GetSelected()->SetCurrent();

		TRACEUSER( "jlh92", _T("Handled!\n") );

		// Process keyboard messages (and mark event as handled)
		if( !CCamFrame::GetMainFrame()->IsIconized() && KeyPress::TranslateMessage( (wxKeyEvent*)&event ) )
			return true;
	}
	
	return -1;
}

/***************************************************************************************************************************/
//
// Initialisation.
//

static bool GiveFocusToFocusableOffspring( wxWindow* pWnd )
{
	TRACEUSER( "jlh92", _T("GF2FO class %s\n"), pWnd->GetClassInfo()->GetClassName() );

	// Can we give focus to passed window. Yes, give focus
	// and return happy
	if( pWnd->AcceptsFocus() )
	{
		TRACEUSER( "jlh92", _T("Focused!\n") );
		pWnd->SetFocus();
		return true;
	}

	// No, lets try the children then
	wxWindowList&		lstChild = pWnd->GetChildren();
	wxWindowListNode*	pNode = lstChild.GetFirst();
	while( NULL != pNode )
	{
		if( GiveFocusToFocusableOffspring( pNode->GetData() ) )
			return true;

		pNode = pNode->GetNext();
	}
	
	return false;
}

bool CCamApp::OnInit()
{
	//
	// Parse command line. We do this early so we get flags which
	// are useful for init, such as -u
	//
	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{

#if defined(_DEBUG)
		{ wxCMD_LINE_OPTION, _T("u"), _T("user"), _T("set username for debug tracing") },
		{ wxCMD_LINE_SWITCH, _T("m"), _T("memorycheck"), _T("check memory") },
		{ wxCMD_LINE_OPTION, _T("l"), _T("listdebug"), _T("list debug level") , wxCMD_LINE_VAL_NUMBER },
#endif
		{ wxCMD_LINE_SWITCH, _T("v"), _T("version"),	_T("Display the version information") },
		{ wxCMD_LINE_OPTION, _T("r"), _T("resource"),	_T("resource directory") },
		{ wxCMD_LINE_PARAM, NULL, NULL, _T("input file"), wxCMD_LINE_VAL_STRING, 
											wxCMD_LINE_PARAM_OPTIONAL|wxCMD_LINE_PARAM_MULTIPLE },
		{ wxCMD_LINE_NONE }
	};
	wxCmdLineParser parser(argc,argv);
	parser.SetDesc(cmdLineDesc);
	parser.Parse();

	wxString ResourceDir = _T("");
	if (parser.Found(_T("r"), &ResourceDir))
	{
		if (!ResourceDir.IsEmpty())
		{
			CamResource::SetResourceFilePath(ResourceDir);
		}
	}

	if( parser.Found( _T("v") ) )
	{
		wxString			strMessage;

#if defined(__WXMSW__) || defined(__WXMAC__)
		strMessage = wxString::Format( wxT("Xara LX\nVersion: %s\nCDraw Version: %d.%03d\n"), 
			g_pszAppVersion, HIWORD(GDraw_GetVersion()), LOWORD(GDraw_GetVersion()) );
#else
#if FALSE == wxUSE_UNICODE
		TCHAR*			pszCDrawVer = GDraw_GetSvnVersion();
#else
		TCHAR			pszCDrawVer[32];
		camMbstowcs( pszCDrawVer, GDraw_GetSvnVersion(), 31 );
#endif
		strMessage = wxString::Format( wxT("Xara LX\nVersion: %s (%s)\nCDraw Version: %d.%03d (%s)\nBuild date: %s\n"), 
			g_pszAppVersion, g_pszSvnVersion, HIWORD(GDraw_GetVersion()), LOWORD(GDraw_GetVersion()), pszCDrawVer, CAMELOT_BUILD_DATE );
#endif

		camPrintf( strMessage.c_str() );
		
		return false;
	}
	
#if defined(_DEBUG)
	if (parser.Found(_T("m"))) SimpleCCObject::CheckMemoryFlag=1;

	long listlevel; // TYPENOTE: Correct - wxWidgets doesn't know about INT32 etc.
	if (parser.Found(_T("l"), &listlevel))
	{
		List::ListDebugLevel = listlevel;
	}

	// Set up the username for tracing
	// We should default this to the environment setting for (say) LOGNAME
	wxString Username = _T("");

	// Overwrite with LOGNAME if it is set
	wxGetEnv(_T("LOGNAME"), &Username);

	// Overwrite with -u option if it is set
	parser.Found(_T("u"), &Username);

	Error::SetUserName(Username);

	if (Username==_T(""))
	{
		TRACEUSER("ALL", _T("No user specific trace output\n"));
	}
	else
	{
//		TRACEUSER("ALL",_T("Tracing output where user is %s\n"),(const char *)Username.mb_str(wxConvUTF8));
		TRACEUSER("ALL",_T("Tracing output where user is %s\n"),(const TCHAR *)Username.c_str());
	}

	TRACEUSER("ALL",_T("Memory debugging %d, List debugging %d\n"), SimpleCCObject::CheckMemoryFlag, List::ListDebugLevel);

#endif

	// Register the image handler which loads CURs (used for Cursors, obviously)
	wxImage::AddHandler( new wxCURHandler );

	// Register the image handler which loads PNGs (used for TBs)
	wxImage::AddHandler( new wxPNGHandler );

	// Init the BinReloc stuff (we don't really care if this fails, since it
	// fails safe)
	BrInitError	error;
	br_init( &error );
	
	// Useful debug tracing enablers, left here for next debug session...
//	wxLog::AddTraceMask( _T("focus") );
//	wxLog::AddTraceMask( _T("keyevent") );

	// Initialise the MonotonicTime class
	MonotonicTime::Init();

	// We initialize profiling here to allow the rest of the app to be profiled
	CamProfile::Init();
	// Don't start profiling until Alex has finished the code!
	CamProfile::ActivateProfiling(TRUE);
	// Indicate time from now on should be assigned to "OTHER"
	CamProfile::AtBase(CAMPROFILE_OTHER);

	TRACET(_T("CCamApp::OnInit first available time to trace"));

	// Initialize resources system
	if (!CamResource::Init()) return FALSE;
	// Initialize the art provider - needed for dialogs
	wxPlatformDependent::Init(CLASSINFO(CamPlatformDependent));
	if (!CamArtProvider::Init()) return FALSE;
	// We need this pretty early so we can handle ERROR boxes etc
	if (!DialogEventHandler::Init()) return FALSE;
	if (!ControlList::Init()) return FALSE;

	TRACET(_T("CCamApp::Calling Camelot.Init"));

	// Initialise the kernel application object & Prefs
 	if( !Camelot.Init() )
		return false;

	TRACET(_T("CCamApp::Calling InitKernel"));
	// then initialise the kernel (and almost everything else)	
	if( !InitKernel() )
		return false;

	TRACET(_T("CCamApp::Calling GRenderRegion::Init"));
	if (!GRenderRegion::Init(true))
		return false;

	// Declare the prefs for the window size and state
	if (Camelot.DeclareSection(TEXT("Windows"), 10))
	{
		Camelot.DeclarePref(NULL, TEXT("MainWndMax"), &MainWndMaximized, 0, 1);
		Camelot.DeclarePref(NULL, TEXT("MainWndMin"), &MainWndMinimized, 0, 1);
		Camelot.DeclarePref(NULL, TEXT("MainWndPos"), &MainWndPosString);
	}

	TRACET(_T("CCamApp::Making Doc Manager"));
	
	// Create the document manager and register our doc template
	m_docManager = std::auto_ptr<wxDocManager>( new wxDocManager() );

	wxDocTemplate	   *pDocTemplate;
	pDocTemplate = new wxDocTemplate(
		m_docManager.get(), wxT("Xara"), wxT("*.xar;*.web"), wxT(""), wxT("xar"), wxT("Xara document"), 
		wxT("Text View"),
		CLASSINFO(CCamDoc),
		CLASSINFO(CCamView) );
//	pDocTemplate = new wxDocTemplate(
//		m_docManager.get(), wxT("Xara"), wxT("*.web"), wxT(""), wxT("web"), wxT("Xara document"), 
//		wxT("Text View"),
//		CLASSINFO(CCamDoc),
//		CLASSINFO(ScreenCamView) );

#if defined(FILELIST)
	m_docManager->FileHistoryLoad(Preferences::GetOilPrefs());
#endif

	// Set idles to only get sent to windows that want them
	wxIdleEvent::SetMode(wxIDLE_PROCESS_SPECIFIED); 

	// Create the Frame window
	//
	// get user preference info for main window
	wxSize ScreenSize = ::wxGetDisplaySize();
	wxRect WndRect;
	wxSize OldScreenSize;
	if ((camSscanf(MainWndPosString, _T("%d %d %d %d %d %d"),
			&WndRect.x, &WndRect.y, &WndRect.width, &WndRect.height, &OldScreenSize.x, &OldScreenSize.y) == 6) &&
			(WndRect.width > 0) && (WndRect.height > 0))
	{
		// numbers scanned OK, scale them to fit current screen size
		if (OldScreenSize.x != ScreenSize.x)
		{
			// if screen size different from last time, them scale them
			WndRect.x = MulDiv(WndRect.x, ScreenSize.x, OldScreenSize.x);
			WndRect.width = MulDiv(WndRect.width, ScreenSize.x, OldScreenSize.x);
		}
		if (OldScreenSize.y != ScreenSize.y)
		{
			WndRect.y = MulDiv(WndRect.y, ScreenSize.y, OldScreenSize.y);
			WndRect.height = MulDiv(WndRect.height, ScreenSize.y, OldScreenSize.y);
		}
	}
	else
	{
		// make up a sensible default position for the window, inset five percent
		// in x and 10 percent in y to avoid taskbars on linux
		WndRect.x = ScreenSize.x / 20;
		WndRect.width = ScreenSize.x - WndRect.x * 2;
		WndRect.y = ScreenSize.y / 10;
		WndRect.height = ScreenSize.y - WndRect.y * 2;
	}

	TRACET(_T("CCamApp::Making Frame Window"));
	m_pMainFrame = new CCamFrame( m_docManager.get(), (wxFrame *)NULL, wxT("Xara LX"), 
		WndRect.GetPosition(), WndRect.GetSize(), wxDEFAULT_FRAME_STYLE);

#if !defined(XARA_MENUGEN)
	TRACET(_T("CCamApp::Making Menu structure"));
	//
	// Build the MDI style file menu
	// This is just a simple menu. The full menu is created
	// after loading a document.
	//
	wxMenu* pFileMenu = new wxMenu;

	pFileMenu->Append( wxID_NEW,  wxT("&New...") );
	pFileMenu->Append( wxID_OPEN, wxT("&Open...") );

#if defined(FILELIST)
	wxMenu* pRecentFilesMenu = new wxMenu;
	m_docManager->FileHistoryUseMenu(pRecentFilesMenu);
	m_docManager->FileHistoryAddFilesToMenu(pRecentFilesMenu);

	pFileMenu->AppendSeparator();
	pFileMenu->Append( -1, wxT("&Recent files"),pRecentFilesMenu );
#endif //defiend(FILELIST)

	pFileMenu->AppendSeparator();
	pFileMenu->Append( wxID_EXIT, wxT("E&xit") );

	// Build help menu
	wxMenu			   *pHelpMenu = new wxMenu;
	pHelpMenu->Append( _R(DOCVIEW_ABOUT), wxT("&About...") );

	// Place menus into the menu bar
	wxMenuBar		   *pMenuBar = new wxMenuBar;
	pMenuBar->Append( pFileMenu, wxT("File") );
	pMenuBar->Append( pHelpMenu, wxT("Help") );

#ifdef __WXMAC__
	wxMenuBar::MacSetCommonMenuBar( pMenuBar );
#endif //def __WXMAC__

	// Set up menu bar...
	m_pMainFrame->SetMenuBar( pMenuBar );

#else

#ifdef __WXMAC__
	wxMenuBar::MacSetCommonMenuBar( WinMDIMenu );
#endif //def __WXMAC__

	// Set up menu bar...
	m_pMainFrame->SetMenuBar( WinMDIMenu );
	WinMDIMenu->ClearAccelTable();

#endif

	// We don't want auto-accelerators, this is done using our hotkeys
	// I don't know how to do this yet, but I'm looking....
//	wxAcceleratorTable* pAccelTable = pMenuBar->GetAccelTable();
//	*pAccelTable = wxAcceleratorTable();

	TRACET(_T("CCamApp::Init Setting mainframe as top window"));

	m_pMainFrame->CreateToolbars();
	m_pMainFrame->UpdateFrameManager();

	// Show the main frame window
	// make maximized if thats what we were last time, unless overridden by user
	if (MainWndMaximized)
	{
		m_pMainFrame->Maximize();
	}
	if (MainWndMinimized)
	{
		m_pMainFrame->Iconize();
	}
	SetTopWindow( m_pMainFrame );
	m_pMainFrame->Show( true );

#ifndef EXCLUDE_FROM_XARALX
	m_pMainFrame->CacheNormalPlaceMode();
	m_pMainFrame->CheckFullScreenMode();

	// if the splash box is up, draw our window behind it (will be a big pending WM_PAINT)
	// IT MUST NOT BE ACTIVE else palette problems
	if (AfxOleGetUserCtrl())
	{
		if (m_nCmdShow == SW_SHOWMINIMIZED || m_nCmdShow == SW_SHOWMINNOACTIVE)
		{
			pMainFrame->ShowWindow(SW_SHOWMINNOACTIVE);
		}
		else
		{
			if (m_nCmdShow == SW_SHOWMAXIMIZED)
				m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
			else
				pMainFrame->ShowWindow(SW_SHOWNOACTIVATE);

			// the ShowWindow() above has made the main window active so make the splash box active, else
			// we get nasty palette problems later on
			pSplashbox->SetActiveWindow();
		}
	}
#endif

	Tool::SelectFirstTool();

	// Go through the command line and load documents
	TRACET(_T("CCamApp::Init Loading docs (if any) from command line"));

	if( 0 == parser.GetParamCount() )
		m_docManager->CreateDocument( _T(""), wxDOC_NEW );
	else
	{
		for ( UINT32 i=0 ; i<parser.GetParamCount() ; i++ )
			m_docManager->CreateDocument(parser.GetParam(i),wxDOC_SILENT);
	}

	// Remove the splash screen
	CamResource::DoneInit();

	CXMLUtils::Initialise();

	// Give focus to any child that will take it, parent can't have it since
	// it's a frame (see gtk_widget_grab_focus)
	GiveFocusToFocusableOffspring( m_pMainFrame );
	
	// Create timer used for background rendering.
//	m_Timer.SetOwner(this,CAM_TIMER_ID);
//	m_Timer.Start(CAM_TIMER_FREQUENCY);

	return true;
}

/***************************************************************************************************************************/
//
// Deinitialisation.
//

INT32 CCamApp::OnExit( void )
{
	// We can no longer stop the closedown, so flag this fact
	Camelot.ShuttingDown(TRUE);

	// Rendering is back on idle events for now as it actually works
//	m_Timer.Stop();

#if defined(FILELIST)
	{
		m_docManager->FileHistorySave(Preferences::GetOilPrefs());
	}
#endif
	m_docManager = std::auto_ptr<wxDocManager>( NULL );

	// Now deinit everything

	CXMLUtils::DeInitialise();

	DeinitKernel();

#if CUSTOM_INONE == 2
	DLLControlClass::DeinitAllControls();
#endif

	GRenderRegion::DeInit();						// tidy up GDraw

PORTNOTE("other","Removed GDI+, filelist and profilename support")
#ifndef EXCLUDE_FROM_XARALX
	Gdiplus::GdiplusShutdown(gdiplusToken);

	// Get rid of the recent file list
	if (pFileList!=NULL)
	{
		// Write out the current file list to the ini file
		pFileList->WriteList();

		// then delete the list
		delete pFileList;
	}

	// Finished with preferences/registry so remove the allocated ProfileName. Was not here before 18/2/97
	if (m_pszProfileName != NULL)
    {
		free((void *) m_pszProfileName);
		m_pszProfileName = NULL;
    }
#endif

	ControlList::DeInit();
	DialogEventHandler::DeInit();
	// Kill of art provider
	CamArtProvider::DeInit();
	wxPlatformDependent::DeInit();
	// Kill of resources
	CamResource::DeInit();

	// Kill off profiling
	CamProfile::ActivateProfiling(FALSE);

#if DUMP_MEM_AT_END
	MemoryState.DumpAllObjectsSince();
#endif

	// We want a memory dump on Debug builds only
#ifdef _DEBUG
	SimpleCCObject::MemoryDump();
	DumpCCMallocTrace();
#endif
	
PORTNOTE("other","Removed 3D, Extras and UserHelp support")
#ifndef EXCLUDE_FROM_XARALX
	// For fabby 3D
	if (Is3dWanted())
		pCtl3dUnregister(AfxGetInstanceHandle()); 

	OILModule::DeinitExtras();							// free all DLLs (e.g. ctl3d)

	// Last of all, deinit the help system now that there's very little else to go wrong.
	DeInitUserHelp();
#endif

	return wxApp::OnExit();
}


void CCamApp::OnAppExit()
{
PORTNOTE("other","Removed multi-instance flag stuff")
#ifndef EXCLUDE_FROM_XARALX
	// remove the flag that prevents camelot starting more than once; BUT only if were
	// the camelot that set it!

	HWND mainWindow = m_pMainWnd->m_hWnd;
	
	if (mainWindow)
	{
		BOOL* ret = (BOOL*) RemoveProp (m_pMainWnd->m_hWnd, "Xara X sfs");

		if (ret) delete (ret);
	}
#endif	

	// Now close the main frame
	m_pMainFrame->Close();
}


/********************************************************************************************

>	void CCamApp::OnRecentFile(INT32 RecentFileNumber)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95
	Inputs:		RecentFileNumber - The number of the file in the Recent File list to open
	Returns:	TRUE if it loaded the file ok, FALSE if it failed
	Purpose:	Loads in one of the recently opened documents that are in the 'Recent File
				List' near the bottom of the File Menu.

********************************************************************************************/

bool CCamApp::OnRecentFile(INT32 RecentFileNumber)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Go and Find out about the File Number (m_pRecentFileList)
	if (pFileList!=NULL)
	{
		if (pFileList->GetSize() > RecentFileNumber)
		{
			// Go get the requested file out of the file list
			CString FileName;
			FileName = pFileList->GetFileName(RecentFileNumber);

			// We do not know what type of filter to use for this file, so get it to try them all
			OpenFileDialog::SelectedFilter = 0;

			// Get the App to open the File
			if (!FileName.IsEmpty())
			{
				// See if we got a document out of the deal
				CDocument* pDoc = OpenDocumentFile(FileName);
				if (pDoc!=NULL)
				{
					// Get the filename as a proper path
					PathName Path((const char*)FileName);

					// Make sure that the files name is sensible
					MakeDocumentNative(pDoc, &Path);

					// add it to the recent file list
					// This actually moves it to the top of the list
					AddToRecentFileList((const char*)FileName);

					return TRUE;
				}
			}
		}
	}
#endif

	// failed
	return false;
}



/********************************************************************************************

>	BOOL CCamApp::GetRecentFileText(INT32 Index, String_256* Text)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95
	Inputs:		Index - The index into the recent file list that we are interested in
	Outputs:	Text - the file name of the recent file. This will be abbreviated it is
				too long. This param is only changed if this function returns TRUE.
	Returns:	TRUE if it found and entry, FALSE if it did not
	Purpose:	Gets the text suitable for the menu text for a given recent file. If there
				is an available file the Text param will hold the abbreviated filename
				(eg. c:\...\blobby.art). If the function returns FALSE, then the Text param
				is left alone.

********************************************************************************************/

bool CCamApp::GetRecentFileText(INT32 Index, String_256* pszText)
{
	wxFileHistory*		pFileHist = m_docManager->GetFileHistory();

	if( Index < INT32(pFileHist->GetCount()) )
	{
		wxFileName	deconstruct( pFileHist->GetHistoryFile( Index ) );
		*pszText = deconstruct.GetFullName();
		return true;
	}

	// Failed to find an entry for this item
	return false;	
}


void CCamApp::DoAboutBox()
{
	wxString			strMessage;

#if defined(__WXMSW__) || defined(__WXMAC__)
	strMessage = wxString::Format( wxT("Xara LX\nVersion: %s\nCDraw Version: %d.%03d\nUsage: XaraLX.exe [xar-file...]"), 
		g_pszAppVersion, HIWORD(GDraw_GetVersion()), LOWORD(GDraw_GetVersion()) );
#else
#if FALSE == wxUSE_UNICODE
	TCHAR*			pszCDrawVer = GDraw_GetSvnVersion();
#else
	TCHAR			pszCDrawVer[32];
	camMbstowcs( pszCDrawVer, GDraw_GetSvnVersion(), 31 );
#endif
	strMessage = wxString::Format( wxT("Xara LX\nVersion: %s (%s)\nCDraw Version: %d.%03d (%s)\nBuild date: %s\nBuilt against: " wxVERSION_STRING "\n" /*"wxWidgets linked to %s\n" */ "Usage: XaraLX [xar-file...]"), 
		g_pszAppVersion, g_pszSvnVersion, HIWORD(GDraw_GetVersion()), LOWORD(GDraw_GetVersion()), pszCDrawVer, CAMELOT_BUILD_DATE /*,_T("Unknown")*/ );
#endif

	(void)wxMessageBox( strMessage, wxT("About Xara LX") );
}


void CCamApp::GiveActiveCanvasFocus()
{
	CCamView* pView = dynamic_cast<CCamView*>( m_docManager->GetCurrentView() );
	if( NULL != pView )
		pView->Activate( true );
}

/***************************************************************************************************************************/
//
// The following is class is needed to ensure that the file menu is removed
// from the file histroy menu list.
//
class CCamDocMDIChildFrame : public wxDocMDIChildFrame
{
public :
    CCamDocMDIChildFrame(wxDocument *doc, wxView *view, wxMDIParentFrame *frame, wxWindowID id,
        const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
	/* TYPENOTE: Correct */ long type = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT("frame"))
		: wxDocMDIChildFrame(doc,view,frame,id,title,pos,size,type,name)
	{ }

    virtual bool Destroy()
	{ 
#if defined(FILELIST)
		wxDocManager* pDocMan = wxGetApp().GetDocumentManager();
		if ( pDocMan && GetMenuBar() && GetMenuBar()->GetMenu(0) )
			pDocMan->FileHistoryRemoveMenu(GetMenuBar()->GetMenu(0));
#endif
		return wxDocMDIChildFrame::Destroy(); 
	}
};

/***************************************************************************************************************************/
/*
 * Centralised code for creating a document frame.
 * Called from view.cpp, when a view is created.
 */

wxMDIChildFrame *CCamApp::CreateChildFrame(wxDocument *doc, wxView *view)
{
	//// Make a child frame
	CCamDocMDIChildFrame *subframe = 
		new CCamDocMDIChildFrame(doc, view, GetMainFrame(), wxID_ANY, _T("Child Frame"), 
			wxPoint(10, 10), wxSize(600, 450),
			wxDEFAULT_FRAME_STYLE |
			wxNO_FULL_REPAINT_ON_RESIZE);

#ifdef __WXMSW__
	subframe->SetIcon(wxString(_T("chart")));
#endif
#ifdef __X__
	subframe->SetIcon(wxIcon(_T("doc.xbm")));
#endif

#if !defined(XARA_MENUGEN)
	//// Make a menubar
	wxMenu*    file_menu = new wxMenu;
	wxMenu*    edit_menu = new wxMenu;
	wxMenu*    zoom_menu = new wxMenu;
	wxMenu* quality_menu = new wxMenu;
	wxMenu*   other_menu = new wxMenu;
	wxMenu*    help_menu = new wxMenu;

	file_menu->Append( wxID_NEW,						wxT("&New..."			) );
	file_menu->Append( wxID_OPEN,						wxT("&Open..."			) );
	file_menu->Append( wxID_CLOSE,						wxT("&Close"			) );
	file_menu->Append( wxID_SAVE,						wxT("&Save"				) );
	file_menu->Append( wxID_SAVEAS,						wxT("Save &As..."		) );

#if defined(FILELIST)
	wxMenu* pRecentFilesMenu = new wxMenu;
	m_docManager->FileHistoryUseMenu(pRecentFilesMenu);
	m_docManager->FileHistoryAddFilesToMenu(pRecentFilesMenu);

	file_menu->AppendSeparator();
	file_menu->Append( -1,								wxT("&Recent files"),	pRecentFilesMenu );
#endif

	file_menu->AppendSeparator();
	file_menu->Append( wxID_PRINT,						wxT("&Print..."			"\tCtrl+P") );
	file_menu->Append( wxID_PRINT_SETUP,				wxT("Print &Setup..."	) );
	file_menu->Append( wxID_PREVIEW,					wxT("Print Pre&view"	) );
	file_menu->AppendSeparator();
	file_menu->Append( wxID_EXIT,						wxT("E&xit"				) );

	edit_menu->Append( wxID_UNDO,						wxT("&Undo"				) );
	edit_menu->Append( wxID_REDO,						wxT("&Redo"				) );
//	edit_menu->AppendSeparator();
//	edit_menu->Append( DOCVIEW_CUT, _T("&Cut last segment") );
	if( NULL != doc->GetCommandProcessor() )
		doc->GetCommandProcessor()->SetEditMenu( edit_menu );

	zoom_menu->Append( _R(DOCVIEW_ZOOMIN),				wxT("Zoom &in"			) );
	zoom_menu->Append( _R(DOCVIEW_ZOOMOUT),				wxT("Zoom &out"			) );
	zoom_menu->Append( _R(DOCVIEW_ZOOMDRAWING), 		wxT("Zoom to &drawing"  ) );
	zoom_menu->Append( _R(DOCVIEW_ZOOMSPREAD),			wxT("Zoom to &page"		) );
	zoom_menu->Append( _R(DOCVIEW_ZOOMPREVIOUS),		wxT("Previous &zoom"	) );

	quality_menu->Append( _R(DOCVIEW_QUALITYANTIALIASED),	wxT("&Antialiased"	),	wxT(""),wxITEM_RADIO );
	quality_menu->Append( _R(DOCVIEW_QUALITYNORMAL),		wxT("&Normal"		),	wxT(""),wxITEM_RADIO );
	quality_menu->Append( _R(DOCVIEW_QUALITYSIMPLE),		wxT("&Simple"		),	wxT(""),wxITEM_RADIO );
	quality_menu->Append( _R(DOCVIEW_QUALITYOUTLINE),		wxT("&Outline"		),	wxT(""),wxITEM_RADIO );

	other_menu->Append( _R(DOCVIEW_BACKGROUNDRENDER),		wxT("&Background render"),	wxT(""),wxITEM_CHECK );
	other_menu->Append( _R(DOCVIEW_CACHING),				wxT("&Caching"),				wxT(""),wxITEM_CHECK );
#ifdef __WXGTK__
	other_menu->Append( _R(DOCVIEW_DOUBLEBUFFER),			wxT("&Double buffer\tHmmm"),		wxT(""),wxITEM_CHECK );
#endif
//#if _DEBUG
	other_menu->Append( _R(DOCVIEW_TIMEREDRAW),				wxT("&Time redraw"		"\tCtrl+Shift+T") );
//#endif
//#if _DEBUG
	other_menu->Append( _R(DOCVIEW_BLOBBYDLG),				wxT("B&lobby Dialog"	"\tHmmm") );
	other_menu->Append( _R(DOCVIEW_BLOBBYBAR),				wxT("Blobb&y Bar"		"\tHmmm") );
	other_menu->Append( _R(DEBUG_TREEVIEWDLG),				wxT("Document Tree"		"\tHmmm") );
//#endif


	other_menu->Append( _R(IDD_BUTTBAR_STATUS),				_T("Status Bar"			"\tHmmm"), wxT(""),wxITEM_CHECK  );
	other_menu->Append( _R(IDD_BUTTBAR_GENERAL),			_T("General Bar"		"\tHmmm"), wxT(""),wxITEM_CHECK );
	other_menu->Append( _R(IDD_BUTTBAR_IMAGESETTING),		_T("ImageSetting Bar"	"\tHmmm"), wxT(""),wxITEM_CHECK  );
	other_menu->Append( _R(IDD_BUTTBAR_WINDOW),				_T("Window Bar"			"\tHmmm"), wxT(""),wxITEM_CHECK  );
	other_menu->Append( _R(IDD_BUTTBAR_ARRANGE),			_T("Arrange Bar"		"\tHmmm"), wxT(""),wxITEM_CHECK  );
	other_menu->Append( _R(IDD_BUTTBAR_EDIT),				_T("Edit Bar"			"\tHmmm"), wxT(""),wxITEM_CHECK  );
	other_menu->Append( _R(IDD_BUTTBAR_FILE),				_T("File Bar"			"\tHmmm"), wxT(""),wxITEM_CHECK  );
	other_menu->Append( _R(IDD_BUTTBAR_GALLERIES),			_T("Galleries Bar"		"\tHmmm"), wxT(""),wxITEM_CHECK  );
	other_menu->Append( _R(IDD_BUTTBAR_STANDARD),			_T("Standard Bar"		"\tHmmm"), wxT(""),wxITEM_CHECK  );
	other_menu->Append( _R(IDD_BUTTBAR_TOOLBAR),			_T("Toolbar Bar"		"\tHmmm"), wxT(""),wxITEM_CHECK  );
	other_menu->Append( _R(IDD_BUTTBAR_ANIMATION),			_T("Animation Bar"		"\tHmmm"), wxT(""),wxITEM_CHECK  );
	other_menu->Append( _R(IDD_BUTTBAR_FEATHER),			_T("Feather Bar"		"\tHmmm"), wxT(""),wxITEM_CHECK  );


	help_menu->Append( _R(DOCVIEW_ABOUT),					wxT("&About..."			"\tF1"));

	wxMenuBar* menu_bar = new wxMenuBar;

	menu_bar->Append(	file_menu, _T("File"	));
	menu_bar->Append(   edit_menu, _T("Edit"	));
	menu_bar->Append(   zoom_menu, _T("Zoom"	));
	menu_bar->Append(quality_menu, _T("Quality"));
	menu_bar->Append(  other_menu, _T("Other"	));
	menu_bar->Append(	help_menu, _T("Help"	));

	// Associate the menu bar with the frame
	subframe->SetMenuBar(menu_bar);
#endif

	// Luke: This seems to be needed to enable accelerator on GTK
	// Gavin: No, this is a bad idea. It mucks up the creation of the
	// "Window" submenu, and perhaps other things as well.
//	subframe->SetFocus();

#if WX_ACCEL
	wxAcceleratorEntry entries[1];
	entries[ 0].Set(wxACCEL_CTRL|wxACCEL_SHIFT,	'T',	_R(DOCVIEW_TIMEREDRAW)			);
/*	//
	// Not necessary since the accelerator is automatically
	// built from the menu accelerator text.
	//
	entries[ 1].Set(wxNORMAL,					'=',	_R(DOCVIEW_ZOOMIN)				);
	entries[ 2].Set(wxNORMAL,					'+',	_R(DOCVIEW_ZOOMIN)				);
	entries[ 3].Set(wxNORMAL,					'-',	_R(DOCVIEW_ZOOMOUT)				);
	entries[ 4].Set(wxNORMAL,					'D',	_R(DOCVIEW_ZOOMDRAWING)			);
	entries[ 5].Set(wxNORMAL,					'P',	_R(DOCVIEW_ZOOMSPREAD)			);
	entries[ 6].Set(wxNORMAL,					'Z',	_R(DOCVIEW_ZOOMPREVIOUS)		);
	entries[ 7].Set(wxNORMAL,					'1',	_R(DOCVIEW_QUALITYOUTLINE)		);
	entries[ 8].Set(wxNORMAL,					'2',	_R(DOCVIEW_QUALITYSIMPLE)		);
	entries[ 9].Set(wxNORMAL,					'3',	_R(DOCVIEW_QUALITYNORMAL)		);
	entries[10].Set(wxNORMAL,					'4',	_R(DOCVIEW_QUALITYANTIALIASED)	);
	entries[11].Set(wxNORMAL,					'B',	_R(DOCVIEW_BACKGROUNDRENDER)	);
	entries[12].Set(wxNORMAL,					'B',	_R(DOCVIEW_CACHING)				);
*/	wxAcceleratorTable accel(1,entries);
	subframe->SetAcceleratorTable(accel);
#else
	// We don't want auto-accelerators, this is done using our hotkeys
	// I don't know how to do this yet, but I'm looking....
#endif

	return subframe;
}

/********************************************************************************************

>	static DialogManager* CCampApp::GetDlgManager()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/93
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the dialog manager
	Purpose:	For obtaining a pointer to the dialog manager
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
DialogManager *CCamApp::GetDlgManager()
{
	return( &m_DlgMgr ); 
}

// Functions to Enable/Disable system functionality 

/********************************************************************************************

>	void CCamApp::DisableSystem(void)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function sets a flag which disables all rendering and processing of user 
				messages in the system.  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CCamApp::DisableSystem()
{
// RALPH
#ifdef RALPH
	return;
#endif

	// Disable camelot's main window. This disables the menu bar, button bar etc. 
	// It does not disable the tool bar however. This will be disabled in the tool bar code 
	// by testing the DisableSystem flag. 
	wxApp			   *pWinApp;
	wxWindow		   *pWnd;

	if ( ( pWinApp = &AfxGetApp() ) != NULL )		// Note assignment using = not == on both lines
		if ( ( pWnd = pWinApp->GetTopWindow() ) !=NULL )
			pWnd->Enable(FALSE);					// Only perform operation if no pointers are NULL
	s_bIsDisabled = true; // The flags value will be tested in the rendering and tool bar
						  // code etc. When TRUE functionality will be disabled.  	
}

/********************************************************************************************

>	void CCamApp::EnableSystem(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function restores system functionality. 
	Errors:		An ENSURE failure will occur if the DisableSys flag was not set when this 
				function was called. 
	SeeAlso:	CCamApp::DisableSystem

********************************************************************************************/

void CCamApp::EnableSystem()  
{
	wxApp			   *pWinApp;
	wxWindow		   *pWnd;

	ENSURE( s_bIsDisabled, "Trying to enable system which is already enabled" );
	// Enable Camelot's main window. 
	if ( ( pWinApp = &AfxGetApp() ) != NULL )		// Note assignment using = not == on both lines
		if ( ( pWnd = pWinApp->GetTopWindow() ) !=NULL )
			pWnd->Enable(TRUE);						// Only perform operation if no pointers are NULL
	s_bIsDisabled = false; // Enable system functionality
}

#if defined(__WXMSW__)

// each element in here corresponds to something in the Winver enumerated type, used
// in the inline functions in camelot.h
BOOL					WinArray[2];

#endif


/*******************************************************************************************/

void CCamApp::OnIdle( wxIdleEvent &event )
{
//	TRACEUSER("Gerry", _T("CCamApp::OnIdle\n"));

	if ( IsDisabled() )
	{
//		TRACEUSER("Gerry", _T("Disabled - Is this a bad thing????????\n"));
		event.Skip();
		return;
	}

	// First thing we will clean up any "temporary" DCs
	CCDC::CleanUpDCs();

	ControlList::Idle();

	if ( Camelot.OnIdle(true) )
	{
		event.RequestMore();
	}

//	TRACEUSER("Gerry", _T("MoreRequested = %s\n"), event.MoreRequested() ? _T("true") : _T("false"));

	// We *MUST* call Skip() here or you can't exit the program!
	event.Skip();
}

/*******************************************************************************************/

void CCamApp::OnTimer( wxTimerEvent& WXUNUSED(event) )
{
PORTNOTE("other","CCamApp::OnTimer - needs completing")
	//
	// This should also exit the while loop if there are messages in the
	// message queue.
	//
	while ( Camelot.ServiceRendering() )
		;
}

/*******************************************************************************************/

void CCamApp::OnHelpIndex()
{
	// Get the locale id
	wxString	strLocale( setlocale( LC_MESSAGES, NULL ), wxConvUTF8 );
	INT32			ordSep = strLocale.Find( _T('_' ) );
	if( -1 != ordSep )
		strLocale = strLocale.Left( ordSep );
	TRACEUSER( "jlh92", _T("Locale = %s\n"), PCTSTR(strLocale) );
	
	// Locale C is considered a synonym for en
	if( strLocale == _T("C") )
		strLocale = _T("en");

	// Check the help dir exists, if not bomb out
	PSTR		pszDataPath = br_find_data_dir( "/usr/share" );
	wxString	strHelpPath( pszDataPath, wxConvUTF8 );
	free( pszDataPath );
	strHelpPath += _T("/xaralx/doc/");
	TRACEUSER( "jlh92", _T("Using filter discovery directory \"%s\"\n"), PCTSTR(strHelpPath + strLocale) );
	if( wxDir::Exists( strHelpPath + strLocale ) )
		strHelpPath += strLocale + _T("/");
	else
	if( wxDir::Exists( strHelpPath + _T("en") ) )
		strHelpPath += strLocale + _T("en/");
	else
	{
		// We'll try default location under debug to make life easier
#if defined(_DEBUG)
		strHelpPath = _T("/usr/share/xaralx/doc/en/");
		TRACEUSER( "jlh92", _T("Try = \"%s\"\n"), PCTSTR(strHelpPath) );
		if( !wxDir::Exists( strHelpPath ) )
#endif
		{
			ERROR1RAW( _R(IDS_MISSING_HELPDIR) );
			return;
		}
	}

	// Build full path
	wxString	strUrl;
	strUrl += strHelpPath;
	strUrl += _T("xaralx.htm");
	if( !wxFile::Exists( strUrl ) )
	{
		ERROR1RAW( _R(IDS_MISSING_HELPINDEX) );
		return;
	}

	// Build the complete URL and launch browser
	strUrl.Prepend( _T("file://") );
	wxLaunchDefaultBrowser( strUrl );
}

/***************************************************************************************************************************/


////////////////////////////////////////////////////////////////////////////////

#if 0

//
// This code needs to be put into wx\common\menucmn.cpp
//
// The accelerators of menu strings are decoded and accelerators are
// automatically built from them. This works well most of the time
// but numeric keypad accelerators must be added in a rather ugly 
// format such as "Ctrl+KP_ADD". This code enables them to be included
// in a format such as "Ctrl+Num +", ie Num (and Numpad) are used as 
// modifiers in the same way as Shift, Ctrl and Alt.
//
// Could probably do with further improvement.
//

static inline bool CompareAccelString(const wxString& str, const wxChar *accel)
{
#if wxUSE_INTL
    return str == accel || str == wxGetTranslation(accel);
#else
    return str == accel;
#endif
}

 wxAcceleratorEntry *wxGetAccelFromString(const wxString& label)
{
    // wxPrintf( wxT("label %s\n"), label.c_str() );

    // check for accelerators: they are given after '\t'
    INT32 posTab = label.Find(wxT('\t'));
    if ( posTab != wxNOT_FOUND ) {
        // parse the accelerator string
        INT32 keyCode = 0;
        INT32 accelFlags = wxACCEL_NORMAL;
        wxString current;
		bool bNumPad = false;
        for ( size_t n = (size_t)posTab + 1; n < label.Len(); n++ ) {
            if ( (label[n] == '+') || (label[n] == '-') ) {
                if ( CompareAccelString(current, wxTRANSLATE("ctrl")) )
                    accelFlags |= wxACCEL_CTRL;
                else if ( CompareAccelString(current, wxTRANSLATE("alt")) )
                    accelFlags |= wxACCEL_ALT;
                else if ( CompareAccelString(current, wxTRANSLATE("shift")) )
                    accelFlags |= wxACCEL_SHIFT;
                else if ( CompareAccelString(current, wxTRANSLATE("numpad")) ||
						  CompareAccelString(current, wxTRANSLATE("num")) )
                    bNumPad = true;
                else {
                    // we may have "Ctrl-+", for example, but we still want to
                    // catch typos like "Crtl-A" so only give the warning if we
                    // have something before the current '+' or '-', else take
                    // it as a literal symbol
                    if ( current.empty() )
                    {
                        current += label[n];

                        // skip clearing it below
                        continue;
                    }
                    else
                    {
                        wxLogDebug(wxT("Unknown accel modifier: '%s'"),
                                   current.c_str());
                    }
                }

                current.clear();
            }
            else {
                current += (wxChar) wxTolower(label[n]);
            }
        }

        if ( current.empty() ) {
            wxLogDebug(wxT("No accel key found, accel string ignored."));
        }
        else {
            if ( current.Len() == 1 && !bNumPad ) {
                // it's a letter
                keyCode = current[0U];

                // Only call wxToupper if control, alt, or shift is held down,
                // otherwise lower case accelerators won't work.
                if (accelFlags != wxACCEL_NORMAL) {
                    keyCode = wxToupper(keyCode);
                }
            }
            else {
                // is it a function key?
                if ( current[0U] == 'f' && wxIsdigit(current[1U]) &&
                     (current.Len() == 2 ||
                     (current.Len() == 3 && wxIsdigit(current[2U]))) ) {
                    keyCode = CAMKEY(F1) + wxAtoi(current.c_str() + 1) - 1;
                }
                else {
                    // several special cases
                    current.MakeUpper();
                    if ( current == wxT("DEL") )
                        keyCode = CAMKEY(DELETE);
                    else if ( current == wxT("DELETE") )
                        keyCode = CAMKEY(DELETE);
                    else if ( current == wxT("BACK") )
                        keyCode = CAMKEY(BACK);
                    else if ( current == wxT("INS") )
                        keyCode = CAMKEY(INSERT);
                    else if ( current == wxT("INSERT") )
                        keyCode = CAMKEY(INSERT);
                    else if ( current == wxT("ENTER") || current == wxT("RETURN") )
                        keyCode = CAMKEY(RETURN);
                    else if ( current == wxT("PGUP") )
                        keyCode = CAMKEY(PRIOR);
                    else if ( current == wxT("PGDN") )
                        keyCode = CAMKEY(NEXT);
                    else if ( current == wxT("LEFT") )
                        keyCode = CAMKEY(LEFT);
                    else if ( current == wxT("RIGHT") )
                        keyCode = CAMKEY(RIGHT);
                    else if ( current == wxT("UP") )
                        keyCode = CAMKEY(UP);
                    else if ( current == wxT("DOWN") )
                        keyCode = CAMKEY(DOWN);
                    else if ( current == wxT("HOME") )
                        keyCode = CAMKEY(HOME);
                    else if ( current == wxT("END") )
                        keyCode = CAMKEY(END);
                    else if ( current == wxT("SPACE") )
                        keyCode = CAMKEY(SPACE);
                    else if ( current == wxT("TAB") )
                        keyCode = CAMKEY(TAB);
                    else if ( current == wxT("ESC") || current == wxT("ESCAPE") )
                        keyCode = CAMKEY(ESCAPE);
                    else if ( current == wxT("CANCEL") )
                        keyCode = CAMKEY(CANCEL);
                    else if ( current == wxT("CLEAR") )
                        keyCode = CAMKEY(CLEAR);
                    else if ( current == wxT("MENU") )
                        keyCode = CAMKEY(MENU);
                    else if ( current == wxT("PAUSE") )
                        keyCode = CAMKEY(PAUSE);
                    else if ( current == wxT("CAPITAL") )
                        keyCode = CAMKEY(CAPITAL);
                    else if ( current == wxT("SELECT") )
                        keyCode = CAMKEY(SELECT);
                    else if ( current == wxT("PRINT") )
                        keyCode = CAMKEY(PRINT);
                    else if ( current == wxT("EXECUTE") )
                        keyCode = CAMKEY(EXECUTE);
                    else if ( current == wxT("SNAPSHOT") )
                        keyCode = CAMKEY(SNAPSHOT);
                    else if ( current == wxT("HELP") )
                        keyCode = CAMKEY(HELP);
                    else if ( current == wxT("ADD") )
                        keyCode = CAMKEY(ADD);
                    else if ( current == wxT("SEPARATOR") )
                        keyCode = CAMKEY(SEPARATOR);
                    else if ( current == wxT("SUBTRACT") )
                        keyCode = CAMKEY(SUBTRACT);
                    else if ( current == wxT("DECIMAL") )
                        keyCode = CAMKEY(DECIMAL);
                    else if ( current == wxT("DIVIDE") )
                        keyCode = CAMKEY(DIVIDE);
                    else if ( current == wxT("NUM_LOCK") )
                        keyCode = CAMKEY(NUMLOCK);
                    else if ( current == wxT("SCROLL_LOCK") )
                        keyCode = CAMKEY(SCROLL);
                    else if ( current == wxT("PAGEUP") )
                        keyCode = CAMKEY(PAGEUP);
                    else if ( current == wxT("PAGEDOWN") )
                        keyCode = CAMKEY(PAGEDOWN);
                    else if ( current == wxT("KP_SPACE") )
                        keyCode = CAMKEY(NUMPAD_SPACE);
                    else if ( current == wxT("KP_TAB") )
                        keyCode = CAMKEY(NUMPAD_TAB);
                    else if ( current == wxT("KP_ENTER") )
                        keyCode = CAMKEY(NUMPAD_ENTER);
                    else if ( current == wxT("KP_HOME") )
                        keyCode = CAMKEY(NUMPAD_HOME);
                    else if ( current == wxT("KP_LEFT") )
                        keyCode = CAMKEY(NUMPAD_LEFT);
                    else if ( current == wxT("KP_UP") )
                        keyCode = CAMKEY(NUMPAD_UP);
                    else if ( current == wxT("KP_RIGHT") )
                        keyCode = CAMKEY(NUMPAD_RIGHT);
                    else if ( current == wxT("KP_DOWN") )
                        keyCode = CAMKEY(NUMPAD_DOWN);
                    else if ( current == wxT("KP_PRIOR") )
                        keyCode = CAMKEY(NUMPAD_PRIOR);
                    else if ( current == wxT("KP_PAGEUP") )
                        keyCode = CAMKEY(NUMPAD_PAGEUP);
                    else if ( current == wxT("KP_NEXT;") )
                        keyCode = CAMKEY(NUMPAD_NEXT);
                    else if ( current == wxT("KP_PAGEDOWN") )
                        keyCode = CAMKEY(NUMPAD_PAGEDOWN);
                    else if ( current == wxT("KP_END") )
                        keyCode = CAMKEY(NUMPAD_END);
                    else if ( current == wxT("KP_BEGIN") )
                        keyCode = CAMKEY(NUMPAD_BEGIN);
                    else if ( current == wxT("KP_INSERT") )
                        keyCode = CAMKEY(NUMPAD_INSERT);
                    else if ( current == wxT("KP_DELETE") )
                        keyCode = CAMKEY(NUMPAD_DELETE);
                    else if ( current == wxT("KP_EQUAL") )
                        keyCode = CAMKEY(NUMPAD_EQUAL);
                    else if ( current == wxT("KP_MULTIPLY") ||
							  current == wxT("*") && bNumPad )
                        keyCode = CAMKEY(NUMPAD_MULTIPLY);
                    else if ( current == wxT("KP_ADD") ||
							  current == wxT("+") && bNumPad )
                        keyCode = CAMKEY(NUMPAD_ADD);
                    else if ( current == wxT("KP_SEPARATOR") )
                        keyCode = CAMKEY(NUMPAD_SEPARATOR);
                    else if ( current == wxT("KP_SUBTRACT") ||
							  current == wxT("-") && bNumPad )
                        keyCode = CAMKEY(NUMPAD_SUBTRACT);
                    else if ( current == wxT("KP_DECIMAL") ||
							  current == wxT(".") && bNumPad )
                        keyCode = CAMKEY(NUMPAD_DECIMAL);
                    else if ( current == wxT("KP_DIVIDE") ||
							  current == wxT("/") && bNumPad )
                        keyCode = CAMKEY(NUMPAD_DIVIDE);
                    else if ( current == wxT("WINDOWS_LEFT") )
                        keyCode = CAMKEY(WINDOWS_LEFT);
                    else if ( current == wxT("WINDOWS_RIGHT") )
                        keyCode = CAMKEY(WINDOWS_RIGHT);
                    else if ( current == wxT("WINDOWS_MENU") )
                        keyCode = CAMKEY(WINDOWS_MENU);
                    else if ( current == wxT("COMMAND") )
                        keyCode = CAMKEY(COMMAND);
                    else if ( bNumPad && current.Len()==1 && wxIsdigit(current[0U]) )
                        keyCode = CAMKEY(NUMPAD0) + wxAtoi(current.c_str());
                    else if ( current.Left(3) == wxT("KP_") && wxIsdigit(current[3U]) )
                        keyCode = CAMKEY(NUMPAD0) + wxAtoi(current.c_str() + 3);
                    else if ( current.Left(7) == wxT("SPECIAL") && wxIsdigit(current[7U]) )
                        keyCode = CAMKEY(SPECIAL1) + wxAtoi(current.c_str() + 7) - 1;
                    else
                    {
                        wxLogDebug(wxT("Unrecognized accel key '%s', accel string ignored."),
                                   current.c_str());
                        return NULL;
                    }
                }
            }
        }

        if ( keyCode ) {
            // we do have something
            return new wxAcceleratorEntry(accelFlags, keyCode);
        }
    }

    return (wxAcceleratorEntry *)NULL;
}

#endif

////////////////////////////////////////////////////////////////////////////////
