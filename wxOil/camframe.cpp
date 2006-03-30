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
 * This is the top-level window of the application.
 */

#include "camtypes.h"

#include "camelot.h"
#include "camframe.h"
#include "camresource.h"

//#include "Res/tbbitmap.h"

#include "wrkrect.h"

#include "menuops.h"
#include "qualops.h"
#include "zoomops.h"
#include "docview.h"
#include "speedtst.h"
#include "oilmenus.h"

#include "blobby.h"
#include "dbugtree.h"
#include "stdbars.h"
#include "rendwnd.h"
#include "ccolbar.h"
#include "dragmgr.h"

#if defined(USE_WXAUI)

class CamFrameManager : public wxFrameManager
{
private:
	DECLARE_CLASS( CamFrameManager )

public:
	CamFrameManager(wxFrame* frame = NULL, unsigned int /*TYPENOTE: Correct*/ flags = wxAUI_MGR_DEFAULT)
		   : wxFrameManager(frame, flags)
	{
	}

	void OnSize(wxSizeEvent &event)
	{
		if (GetMainFrame())
			GetMainFrame()->UpdateWndSize();
		event.Skip();
	}
	void OnMove(wxMoveEvent &event)
	{
		if (GetMainFrame())
			GetMainFrame()->UpdateWndSize();
		event.Skip();
	}

	DECLARE_EVENT_TABLE()
};

IMPLEMENT_CLASS( CamFrameManager, wxFrameManager )
BEGIN_EVENT_TABLE( CamFrameManager, wxFrameManager )
	EVT_SIZE(CamFrameManager::OnSize)
	EVT_MOVE(CamFrameManager::OnMove)
END_EVENT_TABLE()

#endif

CCamFrame *CCamFrame::m_pMainFrame = NULL;

/***************************************************************************************************************************/

IMPLEMENT_CLASS( CCamFrame, wxDocMDIParentFrame )
BEGIN_EVENT_TABLE( CCamFrame, wxDocMDIParentFrame )
	EVT_LEFT_UP(			CCamFrame::OnLButtonUp )
	EVT_RIGHT_UP(			CCamFrame::OnRButtonUp )
	EVT_MOTION(				CCamFrame::OnMouseMove )

	EVT_MENU_RANGE( AUTO_MENU_ID_MIN, AUTO_MENU_ID_MAX, CCamFrame::OnMenuCommand			)

	EVT_MENU( _R(DOCVIEW_ZOOMIN),					CCamFrame::OnZoomIn						)
	EVT_MENU( _R(DOCVIEW_ZOOMOUT),					CCamFrame::OnZoomOut					)
	EVT_MENU( _R(DOCVIEW_ZOOMDRAWING),				CCamFrame::OnZoomDrawing				)
	EVT_MENU( _R(DOCVIEW_ZOOMSPREAD),				CCamFrame::OnZoomPage					)
	EVT_MENU( _R(DOCVIEW_ZOOMPREVIOUS),				CCamFrame::OnZoomPrevious				)
	EVT_MENU( _R(DOCVIEW_QUALITYOUTLINE),			CCamFrame::OnQualityOutline				)
	EVT_MENU( _R(DOCVIEW_QUALITYSIMPLE),			CCamFrame::OnQualitySimple				)
	EVT_MENU( _R(DOCVIEW_QUALITYNORMAL),			CCamFrame::OnQualityNormal				)
	EVT_MENU( _R(DOCVIEW_QUALITYANTIALIASED),		CCamFrame::OnQualityAntialiased			)
	EVT_MENU( _R(DOCVIEW_BACKGROUNDRENDER),			CCamFrame::OnBackgroundRender			)
	EVT_MENU( _R(DOCVIEW_CACHING),					CCamFrame::OnCaching					)
	EVT_MENU( _R(DOCVIEW_DOUBLEBUFFER),				CCamFrame::OnDoubleBuffer				)
	EVT_MENU( _R(DOCVIEW_TIMEREDRAW),				CCamFrame::OnTimeRedraw					)
//#if defined(_DEBUG)
	EVT_MENU( _R(DOCVIEW_BLOBBYDLG),				CCamFrame::OnBlobbyDlg					)
	EVT_MENU( _R(DOCVIEW_BLOBBYBAR),				CCamFrame::OnBlobbyBar					)
	EVT_MENU( _R(DEBUG_TREEVIEWDLG),				CCamFrame::OnTreeView					)
//#endif
	// Using the dialog IDs here as menu IDs is a bodge, but should work
	EVT_MENU( _R(IDD_BUTTBAR_GENERAL),				CCamFrame::OnStandardBar				)
	EVT_MENU( _R(IDD_BUTTBAR_IMAGESETTING),			CCamFrame::OnStandardBar				)
	EVT_MENU( _R(IDD_BUTTBAR_WINDOW),				CCamFrame::OnStandardBar				)
	EVT_MENU( _R(IDD_BUTTBAR_ARRANGE),				CCamFrame::OnStandardBar				)
	EVT_MENU( _R(IDD_BUTTBAR_EDIT),					CCamFrame::OnStandardBar				)
	EVT_MENU( _R(IDD_BUTTBAR_FILE),					CCamFrame::OnStandardBar				)
	EVT_MENU( _R(IDD_BUTTBAR_GALLERIES),			CCamFrame::OnStandardBar				)
	EVT_MENU( _R(IDD_BUTTBAR_STANDARD),				CCamFrame::OnStandardBar				)
	EVT_MENU( _R(IDD_BUTTBAR_TOOLBAR),				CCamFrame::OnStandardBar				)
	EVT_MENU( _R(IDD_BUTTBAR_ANIMATION),			CCamFrame::OnStandardBar				)
	EVT_MENU( _R(IDD_BUTTBAR_FEATHER),				CCamFrame::OnStandardBar				)
	
	EVT_MENU_OPEN(									CCamFrame::OnInitMenuPopup				)

	EVT_UPDATE_UI( _R(IDD_BUTTBAR_GENERAL),			CCamFrame::OnUpdateStandardBar			)
	EVT_UPDATE_UI( _R(IDD_BUTTBAR_IMAGESETTING),	CCamFrame::OnUpdateStandardBar			)
	EVT_UPDATE_UI( _R(IDD_BUTTBAR_WINDOW),			CCamFrame::OnUpdateStandardBar			)
	EVT_UPDATE_UI( _R(IDD_BUTTBAR_ARRANGE),			CCamFrame::OnUpdateStandardBar			)
	EVT_UPDATE_UI( _R(IDD_BUTTBAR_EDIT),			CCamFrame::OnUpdateStandardBar			)
	EVT_UPDATE_UI( _R(IDD_BUTTBAR_FILE),			CCamFrame::OnUpdateStandardBar			)
	EVT_UPDATE_UI( _R(IDD_BUTTBAR_GALLERIES),		CCamFrame::OnUpdateStandardBar			)
	EVT_UPDATE_UI( _R(IDD_BUTTBAR_STANDARD),		CCamFrame::OnUpdateStandardBar			)
	EVT_UPDATE_UI( _R(IDD_BUTTBAR_TOOLBAR),			CCamFrame::OnUpdateStandardBar			)
	EVT_UPDATE_UI( _R(IDD_BUTTBAR_ANIMATION),		CCamFrame::OnUpdateStandardBar			)
	EVT_UPDATE_UI( _R(IDD_BUTTBAR_FEATHER),			CCamFrame::OnUpdateStandardBar			)

	EVT_UPDATE_UI( _R(DOCVIEW_ZOOMPREVIOUS),		CCamFrame::OnUpdateZoomPrevious			)
	EVT_UPDATE_UI( _R(DOCVIEW_QUALITYOUTLINE),		CCamFrame::OnUpdateQualityOutline		)
	EVT_UPDATE_UI( _R(DOCVIEW_QUALITYSIMPLE),		CCamFrame::OnUpdateQualitySimple		)
	EVT_UPDATE_UI( _R(DOCVIEW_QUALITYNORMAL),		CCamFrame::OnUpdateQualityNormal		)
	EVT_UPDATE_UI( _R(DOCVIEW_QUALITYANTIALIASED),	CCamFrame::OnUpdateQualityAntialiased	)
	EVT_UPDATE_UI( _R(DOCVIEW_BACKGROUNDRENDER),	CCamFrame::OnUpdateBackgroundRender		)
	EVT_UPDATE_UI( _R(DOCVIEW_CACHING),				CCamFrame::OnUpdateCaching				)
	EVT_UPDATE_UI( _R(DOCVIEW_DOUBLEBUFFER),		CCamFrame::OnUpdateDoubleBuffer			)
#if !defined(USE_WXAUI)
	EVT_SIZE(CCamFrame::OnSize)
	EVT_MOVE(CCamFrame::OnMove)
#endif

	EVT_CLOSE(CCamFrame::OnCloseWindow)
END_EVENT_TABLE()

/***************************************************************************************************************************/

CCamFrame::CCamFrame( wxDocManager *manager, wxFrame *frame, const wxString& title, const wxPoint& pos, 
		      const wxSize& size, /*TYPENOTE: Correct*/ long type ) : 
		wxDocMDIParentFrame( manager, frame, wxID_ANY, title, pos, size, type, wxT("CCamFrame") ),
		m_ChangingViewMode( false )
{
	wxASSERT( NULL == m_pMainFrame );

	m_pMainFrame = this;
	m_editMenu = (wxMenu *)NULL;

#if defined(USE_WXAUI)
	m_pFrameManager = NULL;
#endif
	m_pCaptureWnd = NULL;
	m_pStatusBar = NULL;

	Create( frame, wxID_ANY, title, pos, size, type );
	
	wxBitmap			bitmapTmp;
	CamResource::LoadwxBitmap( bitmapTmp, _T("camelot.png") );
	m_iconAppIcon.CopyFromBitmap( bitmapTmp );
	SetIcon( m_iconAppIcon );
}

/***************************************************************************************************************************/

CCamFrame::~CCamFrame()
{
#if defined(USE_WXAUI)
	if (m_pFrameManager)
	{
		m_pFrameManager->UnInit();
		delete m_pFrameManager;
	}
#endif

	m_pMainFrame = NULL;
}

/***************************************************************************************************************************/
/*
 * Segment a toolbar image strip into a image list
 * @param lstImage		The ImageList to fill
 * @param str			The file name of the image file that contains the icons
 * @param cPX			The width the of icons (if 0, code will attempt to guess width)
 * @return true if successful, else false
 **/
#if 0
static bool BuildImageList( std::vector<wxBitmap> &lstImage,  const TCHAR * str, unsigned cPx = 0 )
{
	wxImage	imageStandard;
	CamResource::LoadwxImage(imageStandard, str );

	if( !imageStandard.Ok() )
		return false;

	unsigned cpxWidth  = imageStandard.GetWidth();
	unsigned cpxHeight = imageStandard.GetHeight();
	
	// Find width of icons (if not known)
	INT32					ord = 0;
	if( 0 == cPx )
	{
		for( cPx = 1; cPx < cpxWidth; ++cPx )
		{
			if( 0 != imageStandard.GetAlpha( cPx, 1 ) )
				break;
		}
		++cPx;

		++ord;
	}

	// Split icons into an imagelist
	INT32					cImage = cpxWidth / cPx;
	for( ; ord < cImage; ++ord )
	{
		wxImage			image( imageStandard.GetSubImage( wxRect( ord * cPx, 0, cPx, cpxHeight ) ) );

		// GetSubImage doesn't copy alpha, so we do it ourselves
		image.SetAlpha( NULL, false );
		wxByte		   *pSrc = imageStandard.GetAlpha() + ( ord * cPx );
		wxByte		   *pDst = image.GetAlpha();

		for( unsigned posY = 0; posY < cpxHeight; ++posY, pSrc += cpxWidth, pDst += cPx )
		{
			memcpy( pDst, pSrc, cPx );
		}

		lstImage.push_back( image );
	}

	return true;
}
#endif

/***************************************************************************************************************************/

bool CCamFrame::Create( wxWindow *pParent, wxWindowID id, const wxString &caption, const wxPoint &pos, 
			const wxSize &size, /* TYPENOTE: Correct */ long style )
{
#if defined(USE_WXAUI)
	m_pFrameManager = new CamFrameManager;
	if (!m_pFrameManager) return FALSE;
	m_pFrameManager->SetFrame(this);
	
	m_pFrameManager->SetFlags(m_pFrameManager->GetFlags() |
							wxAUI_MGR_ALLOW_FLOATING
#if defined(__WXMSW__)
							 | wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_TRANSPARENT_HINT_FADE
#endif
							);
#endif
	return TRUE;
}

BOOL CCamFrame::CreateToolbars()
{
	// Create the status bar
	m_pStatusBar = CreateStatusBar();
	String_256	strStatus( _R(IDS_NO_OBJECTS_SEL) );
	m_pStatusBar->SetStatusText( strStatus );
	
	CColourBar* pColourBar = new CColourBar();
	if (pColourBar)
	{
		pColourBar->Create(this, 12345);

		wxPaneInfo paneinfo;
		paneinfo.BottomDockable().Bottom();
		paneinfo.CaptionVisible(false);
		m_pFrameManager->AddPane(pColourBar, paneinfo);

		m_pFrameManager->Update();
	}

	// Enable alpha on XP true-color
#if defined(__WXMSW__)
	if( wxTheApp->GetComCtl32Version() >= 600 && ::wxDisplayDepth() >= 32 )
	{
		wxSystemOptions::SetOption( wxT("msw.remap"), 2 );
	}
#endif	

	wxCommandEvent sbe(wxEVT_COMMAND_MENU_SELECTED, _R(IDD_BUTTBAR_STANDARD));
	OnStandardBar(sbe);

	wxCommandEvent tbe(wxEVT_COMMAND_MENU_SELECTED, _R(IDD_BUTTBAR_TOOLBAR));
	OnStandardBar(tbe);

	return TRUE;
}

void CCamFrame::UpdateFrameManager()
{
#if defined(USE_WXAUI)
	if (m_pFrameManager)
		m_pFrameManager->Update();
#endif
}

/***************************************************************************************************************************/

bool CCamFrame::Destroy()
{
	// Before anything else let's set Document::Current and DocView::Current properly...
	// This "event" doesn't refer to any particular doc or view so we'll set them both NULL
	Document::SetNoCurrent();
	DocView::SetNoCurrent();

	// Update the window position and state preferences
	// This doesn't work as well as in Xtreme due to the lack of GetWindowPlacement
	// GetRect returns the current size of the window even when maximised or iconised
	// so some effort will be needed to correctly save the normal position
	bool bMin = IsIconized();
	bool bMax = IsMaximized();
	wxSize ScreenSize = wxGetDisplaySize();
	CCamApp::MainWndPosString._MakeMsg(
			_T("#1%d #2%d #3%d #4%d #5%d #6%d"),
			m_WndRect.x,
			m_WndRect.y,
			m_WndRect.width,
			m_WndRect.height,
			ScreenSize.x,
			ScreenSize.y
				);
	CCamApp::MainWndMaximized = bMax ? TRUE : FALSE;
	CCamApp::MainWndMinimized = bMin ? TRUE : FALSE;

PORTNOTE("other","Removed WindowPlacement saving")
#if !defined(EXCLUDE_FROM_XARALX)
#ifndef EXCLUDE_FROM_RALPH
	// lets write out our non-maximised window position to the .ini file
	WINDOWPLACEMENT wPlace;
	wPlace.length = sizeof(WINDOWPLACEMENT);
	if (GetWindowPlacement( &wPlace ))
	{
		
		// we must save last normal position if in full screen mode 
		if (FullScreenMode)
		{
			wPlace = NormalModePlace;
		}

		CCamApp::MainWndPosString._MakeMsg(
				"#1%d #2%d #3%d #4%d #5%d #6%d",
				wPlace.rcNormalPosition.left,
				wPlace.rcNormalPosition.top,
				wPlace.rcNormalPosition.right - wPlace.rcNormalPosition.left,
				wPlace.rcNormalPosition.bottom - wPlace.rcNormalPosition.top,
				GetSystemMetrics( SM_CXSCREEN ),
				GetSystemMetrics( SM_CYSCREEN )
					);
		CCamApp::MainWndMaximized = (wPlace.showCmd == SW_SHOWMAXIMIZED) ? TRUE : FALSE;
		CCamApp::MainWndMinimized = (wPlace.showCmd == SW_SHOWMINIMIZED) ? TRUE : FALSE;
	}
	else
	{
		CCamApp::MainWndPosString = "";							// so uses default next time
		CCamApp::MainWndMaximized = FALSE;
		CCamApp::MainWndMinimized = FALSE;

	}
#endif // EXCLUDE_FROM_RALPH
#endif

	// Make sure that the currently selected tool is "de-initialised" before the
	// main window is destroyed, or it will try to use a dead info bar.
	// NB. Docs say that this function can be safely called more than once.
	Tool::DeinitTools();

   	// DeathMsg is the last message sent before camelot shuts down
	BROADCAST_TO_ALL(DeathMsg());

PORTNOTE("other","Removed ExternalClipboard usage")
#if !defined(EXCLUDE_FROM_XARALX)
#ifndef EXCLUDE_FROM_RALPH
	// And make sure we're no longer on the clipboard viewer chain
	ChangeClipboardChain(NextClipboardViewer);
#endif
#endif

	return wxDocMDIParentFrame::Destroy();
}

/***************************************************************************************************************************/

CCamFrame *GetMainFrame(void)
{
	return CCamFrame::m_pMainFrame;
}

/***************************************************************************************************************************/


void CCamFrame::StartDragManagerDrag(CaptureWnd* pWnd)
{
	TRACEUSER("Gerry", _T("StartDragManagerDrag"));
	// Vector mouse capture related stuff to this "window"

	if (HasCapture())
	{
		TRACEUSER("Gerry", _T("Already got capture"));
	}

	CaptureMouse();

	if (HasCapture())
	{
		TRACEUSER("Gerry", _T("Got capture"));
		m_pCaptureWnd = pWnd;
	}
}


void CCamFrame::EndDragManagerDrag(CaptureWnd* pWnd)
{
	TRACEUSER("Gerry", _T("EndDragManagerDrag"));
	if (m_pCaptureWnd)
	{
		ERROR3IF(m_pCaptureWnd != pWnd, "EndDrag different window to StartDrag");	

		m_pCaptureWnd = NULL;
		if (HasCapture())
		{
			ReleaseMouse();
			if (HasCapture())
			{
				TRACEUSER("Gerry", _T("Still got capture"));
			}
			else
			{
				TRACEUSER("Gerry", _T("Capture released"));
			}

		}
		else
		{
			TRACEUSER("Gerry", _T("Haven't got capture"));
		}
	}
	else
	{
		TRACEUSER("Gerry", _T("No CaptureWnd"));
	}
}


void CCamFrame::OnLButtonUp(wxMouseEvent& event)
{
	TRACEUSER("Gerry", _T("CCamFrame::OnLButtonUp"));
	if (m_pCaptureWnd)
		m_pCaptureWnd->OnLButtonUp(event);
	else
		event.Skip();
}

void CCamFrame::OnRButtonUp(wxMouseEvent& event)
{
	TRACEUSER("Gerry", _T("CCamFrame::OnRButtonUp"));
	if (m_pCaptureWnd)
		m_pCaptureWnd->OnRButtonUp(event);
	else
		event.Skip();
}

void CCamFrame::OnMouseMove(wxMouseEvent& event)
{
	if (m_pCaptureWnd)
		m_pCaptureWnd->OnMouseMove(event);
	else
		event.Skip();
}



void CCamFrame::UpdateWndSize()
{
	if (!IsIconized() && !IsMaximized())
	{
		m_WndRect = GetRect();
	}
}


#if !defined(USE_WXAUI)
void CCamFrame::OnSize(wxSizeEvent &event)
{
	// We want to remember the size of the window if we are not maximised or minimised
	if (!IsIconized() && !IsMaximized())
		m_WndRect = GetRect();

	// We want to skip the event even when we do have a FrameManager
	// The FrameManager actually gets the OnSize before us so no need to pass it on
//	event.Skip();
}

void CCamFrame::OnMove(wxMoveEvent &event)
{
	// We want to remember the size of the window if we are not maximised or minimised
	if (!IsIconized() && !IsMaximized())
		m_WndRect = GetRect();

	// We want to skip the event even when we do have a FrameManager
	event.Skip();
}
#endif

void CCamFrame::OnCloseWindow(wxCloseEvent& event)
{
	Camelot.ShuttingDown(TRUE);

	if (m_docManager->Clear(!event.CanVeto()))
	{
PORTNOTE("other","Removed ExternalClipboard usage")
#if !defined(EXCLUDE_FROM_XARALX)
		// Put any stuff the user wants to keep on the clipboard before we depart.
		// We want this to happen before the window closes and progress displays are lost.
		ExternalClipboard::PrepareForShutdown();
#endif
		// We must DeInit the dialog manager before any windows are destroyed. 
		// This needs to be here because Popup windows (Dialogs/Floating bars) 
		// seem to be destroyed before the MainFrame receives an OnDestroy message
		DialogManager::DeInit(); 

		this->Destroy();
	}
	else
	{
		// We cancelled for some reason.
		Camelot.ShuttingDown(FALSE);

		event.Veto();
	}
}



/***************************************************************************************************************************/

void CCamFrame::OnMenuCommand( wxCommandEvent& event )
{
	// Before anything else let's set Document::Current and DocView::Current properly...
	// Commands which make it up to the main frame will either work on no document or the
	// selected document. So the most useful thing to do is to set Current to Selected.
	Document::GetSelected()->SetCurrent();
	DocView::GetSelected()->SetCurrent();

#if !defined(EXCLUDE_FROM_XARALX)
#if (_OLE_VER < 0x200) && (_MSVC_VER < 0x300)
	if (COleClientItem::InWaitForRelease())
	{
		AfxMessageBox(_R(IDP_BUSY));
		return TRUE;        // handled
	}
#endif
#endif

	// Process Menu Commands
	if( IsMenuCommand( event ) )
	{   
		if( PerformMenuCommand( event ) )
			return;
    }

#if !defined(EXCLUDE_FROM_XARALX)
	// If we get to here then F1 has been pressed over a menu (see comment below).
	if (LOWORD(wParam) == _R(ID_HELP))
	{
		// Apparently this occurs when F1 is pressed at "non-menu times"?!?
		if (HIWORD(wParam))
		{
			TRACEUSER( "Marc", _T("Processing _R(ID_HELP): wParam = 0x%lX, lParam = 0x%lX\n"),
									(UINT32) wParam, (UINT32) lParam);
			return TRUE;
		}

		// If we have a valid menu item then run the help engine for it.
		if (MenuBeingTracked != NULL)
		{
			OpDescriptor* pOpDesc = MenuBeingTracked->GetOpDescriptor();
			ERROR3IF(pOpDesc == NULL, "Null OpDescriptor* in CMainFrame::OnCommand");
			if (pOpDesc != NULL) HelpUser(*pOpDesc);

			// Bug fix: send ourselves a WM_CANCELMODE message here, so that further menu
			// processing is cancelled.  Otherwise it is possible for the F1 to run the
			// menu item, as well as explaining it.
			SendMessage(WM_CANCELMODE);
			return TRUE;
		}
	}
#endif

	// For everything else, pass to the base class for handling.
	event.Skip();
}


/***************************************************************************************************************************/
//
// These event handlers are probably temporary. They are here just
// to get the program up and running and to add some functionality.
//

void CCamFrame::OnZoomIn( wxCommandEvent& WXUNUSED(event) )
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpZoomIn)); 
	if ( pOpDesc )
		pOpDesc->Invoke();
}

void CCamFrame::OnZoomOut( wxCommandEvent& WXUNUSED(event) )
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpZoomOut)); 
	if ( pOpDesc )
		pOpDesc->Invoke();
}

void CCamFrame::OnZoomDrawing( wxCommandEvent& WXUNUSED(event) )
{
	OpZoomDescriptor::FakeInvoke(OPTOKEN_ZOOMDRAWING);
}
 
void CCamFrame::OnZoomPage( wxCommandEvent& WXUNUSED(event) )
{
	OpZoomDescriptor::FakeInvoke(OPTOKEN_ZOOMSPREAD);
}
 
void CCamFrame::OnZoomPrevious( wxCommandEvent& WXUNUSED(event) )
{
	OpZoomDescriptor::FakeInvoke(OPTOKEN_ZOOMPREV);
}

void CCamFrame::OnUpdateZoomPrevious( wxUpdateUIEvent& event )
{
	OpZoomDescriptor* pOpDesc = (OpZoomDescriptor*)OpZoomDescriptor::FindOpDescriptor(OPTOKEN_ZOOMPREV); 
	event.Enable(pOpDesc && pOpDesc->IsAvailable());
//	event.Enable(!OpZoomPrevZoomDescriptor::GetState(NULL,NULL).Greyed);
}

/***************************************************************************************************************************/
//
// These event handlers are probably temporary. They are here just
// to get the program up and running and to add some functionality.
//

void CCamFrame::OnQualityOutline( wxCommandEvent& WXUNUSED(event) )
{
	Document::GetSelected()->SetCurrent();
	DocView::GetSelected()->SetCurrent();
	OpDescriptor* pOpDesc = (OpDescriptor*)OpDescriptor::FindOpDescriptor( OPTOKEN_QUALITYOUTLINE );
	if ( pOpDesc )
		pOpDesc->Invoke();
}

void CCamFrame::OnQualitySimple( wxCommandEvent& WXUNUSED(event) )
{
	Document::GetSelected()->SetCurrent();
	DocView::GetSelected()->SetCurrent();
	OpDescriptor* pOpDesc = (OpDescriptor*)OpDescriptor::FindOpDescriptor( OPTOKEN_QUALITYSIMPLE );
	if ( pOpDesc )
		pOpDesc->Invoke();
}

void CCamFrame::OnQualityNormal( wxCommandEvent& WXUNUSED(event) )
{
	Document::GetSelected()->SetCurrent();
	DocView::GetSelected()->SetCurrent();
	OpDescriptor* pOpDesc = (OpDescriptor*)OpDescriptor::FindOpDescriptor( OPTOKEN_QUALITYNORMAL );
	if ( pOpDesc )
		pOpDesc->Invoke();
}

void CCamFrame::OnQualityAntialiased( wxCommandEvent& WXUNUSED(event) )
{
	Document::GetSelected()->SetCurrent();
	DocView::GetSelected()->SetCurrent();
	OpDescriptor* pOpDesc = (OpDescriptor*)OpDescriptor::FindOpDescriptor( OPTOKEN_QUALITYANTIALIASED );
	if ( pOpDesc )
		pOpDesc->Invoke();
}

void CCamFrame::OnUpdateQualitySimple( wxUpdateUIEvent& event )
{
	const DocView *pDocView = DocView::GetSelected();
	event.Check(pDocView && pDocView->RenderQuality.GetQuality()==QUALITY_LEVEL_SIMPLE);
}

void CCamFrame::OnUpdateQualityOutline( wxUpdateUIEvent& event )
{
	const DocView *pDocView = DocView::GetSelected();
	event.Check(pDocView && pDocView->RenderQuality.GetQuality()==QUALITY_LEVEL_OUTLINE);
}

void CCamFrame::OnUpdateQualityNormal( wxUpdateUIEvent& event )
{
	const DocView *pDocView = DocView::GetSelected();
	event.Check(pDocView && pDocView->RenderQuality.GetQuality()==QUALITY_LEVEL_NORMAL);
}

void CCamFrame::OnUpdateQualityAntialiased( wxUpdateUIEvent& event )
{
	const DocView *pDocView = DocView::GetSelected();
	event.Check(pDocView && pDocView->RenderQuality.GetQuality()==QUALITY_LEVEL_ANTI);
}

/***************************************************************************************************************************/
//
// These event handlers are probably temporary. They are here just
// to get the program up and running and to add some functionality.
//

void CCamFrame::OnBackgroundRender( wxCommandEvent& WXUNUSED(event) )
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpToggleFore)); 
	if ( pOpDesc )
		pOpDesc->Invoke();
}

void CCamFrame::OnUpdateBackgroundRender( wxUpdateUIEvent& event )
{
	event.Check(FALSE != OpToggleFore::GetState(NULL,NULL).Ticked);
}

/***************************************************************************************************************************/
//
// These event handlers are probably temporary. They are here just
// to get the program up and running and to add some functionality.
//

void CCamFrame::OnCaching( wxCommandEvent& WXUNUSED(event) )
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpClearCache)); 
	if ( pOpDesc )
		pOpDesc->Invoke();
}

void CCamFrame::OnUpdateCaching( wxUpdateUIEvent& event )
{
	event.Check(FALSE != OpClearCache::GetState(NULL,NULL).Ticked);
}

// We should really do an Op for this
void CCamFrame::OnDoubleBuffer( wxCommandEvent& event )
{
	CRenderWnd::SetDoubleBuffer(event.IsChecked());
}

void CCamFrame::OnUpdateDoubleBuffer( wxUpdateUIEvent& event )
{
	event.Check(FALSE != CRenderWnd::GetDoubleBuffer());
}


/***************************************************************************************************************************/
//
// These event handlers are probably temporary. They are here just
// to get the program up and running and to add some functionality.
//

void CCamFrame::OnTimeRedraw( wxCommandEvent& WXUNUSED(event) )
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpTimeDraw)); 
	if ( pOpDesc )
		pOpDesc->Invoke();
}

/***************************************************************************************************************************/
//
// These event handlers are probably temporary. They are here just
// to get the program up and running and to add some functionality.
//

//#if defined(_DEBUG)
void CCamFrame::OnBlobbyDlg( wxCommandEvent& WXUNUSED(event) )
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(BlobbyDlg)); 
	if ( pOpDesc )
		pOpDesc->Invoke();
}

void CCamFrame::OnBlobbyBar( wxCommandEvent& WXUNUSED(event) )
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(BlobbyBar)); 
	if ( pOpDesc )
		pOpDesc->Invoke();
}

void CCamFrame::OnTreeView( wxCommandEvent& WXUNUSED(event) )
{
//	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(DebugTreeDlg));
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_DEBUGTREEDLG);
	if ( pOpDesc )
		pOpDesc->Invoke();
}

void CCamFrame::OnStandardBar( wxCommandEvent& event)
{
	// We can't find by Runtime Class as there are multiple OpDescriptors with the same Class
	// So get the resource ID (which also happens to be the dialog ID to use
	ResourceID r = (ResourceID)(event.GetId());
	// Find the Op Descriptor by name
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor((TCHAR *)(CamResource::GetObjectName(r))); 
	// Stick the resource ID in he Op Param
//	OpParam Param(CUniversalParam((INT32)r), CUniversalParam(0));
	pOpDesc->Invoke();
}

void CCamFrame::OnInitMenuPopup( wxMenuEvent& event )
{
	// Before anything else let's set Document::Current and DocView::Current properly...
	// Menus will either work on no document or the selected document. So the most useful
	// thing to do is to set Current to Selected.
	Document::GetSelected()->SetCurrent();
	DocView::GetSelected()->SetCurrent();

PORTNOTE( "other", "Removed usage of bSysMent from OnInitMenuPopup" )
#if !defined(EXCLUDE_FROM_XARALX)
	if (bSysMenu)
	{
		CMDIFrameWnd::OnInitMenuPopup(pMenu, n, bSysMenu);
		// bug fix 2996 
		if(FullScreenMode)
		{
			pMenu->EnableMenuItem(SC_MOVE,MF_GRAYED);
			pMenu->EnableMenuItem(SC_SIZE,MF_GRAYED);
			pMenu->EnableMenuItem(SC_MAXIMIZE,MF_GRAYED);
		}
	}
	else
#endif
	if( !UpdateMenu( event.GetMenu() ) )
		event.Skip();
}

void CCamFrame::OnUpdateStandardBar( wxUpdateUIEvent& event)
{
	// We can't find by Runtime Class as there are multiple OpDescriptors with the same Class
	// So get the resource ID (which also happens to be the dialog ID to use
	ResourceID r = (ResourceID)(event.GetId());
	// Find the Op Descriptor by name
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor((TCHAR *)(CamResource::GetObjectName(r))); 
	// Stick the resource ID in he Op Param
//	OpParam Param(CUniversalParam((INT32)r), CUniversalParam(0));
	OpParam Param((INT32)r, (INT32)0);
	String_256 disable;
	OpState opst;
	if ( pOpDesc) opst = pOpDesc->GetOpsState(&disable,&Param);
	event.Enable(pOpDesc && !opst.Greyed);
	event.Check(FALSE != opst.Ticked);
}

//#endif

/***************************************************************************************************************************/





