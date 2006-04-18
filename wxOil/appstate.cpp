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


	appstate.cpp

	This module saves the state of the application when it is closed, and restores it the next
	time it is run.
*/

#include "camtypes.h"
#include "errors.h"
//#include "mario.h"
#include "app.h"
#include "camelot.h"
#include "document.h"
#include "docview.h"
#include "camdoc.h"
#include "ccmdikid.h"
#include "scrcamvw.h"
#include "vstate.h"
#include "rulers.h"
#include "filedlgs.h"
#include "registry.h"
#include "statline.h"
//#include "justin.h"


DECLARE_SOURCE("$Revision$");


// This preference controls whether we restore docs etc on startup.
BOOL fRestoreWorkspaceOnStartup = FALSE;

// This key holds the place in the registry to load and save the workspace.
HKEY hWorkspaceRegKey = 0;

// If TRUE then ScreenView::OnInitialUpdate *won't* automatically maximise new views.
BOOL fDontMaximiseViews = FALSE;



/********************************************************************************************
>	static void MakeKeyFromViewState(HKEY hViewKey, ScreenCamView* pCamView)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/95
	Inputs:		hViewKey		the key to store the ViewState's data
				pCamView		the view to store
	Purpose:	Saves the data of the given view under the given key.
	SeeAlso:	SaveAppWindowState; MakeViewStateFromKey
********************************************************************************************/

static void MakeKeyFromView(HKEY hViewKey, ScreenCamView* pCamView)
{
	// Get the ViewState & DocView associated with the given ScreenCamView.
	ERROR3IF(pCamView == NULL, "Null ScreenCamView* in MakeKeyFromView\n");
	ViewState* pvs = pCamView->GetViewState();
	ERROR3IF(pvs == NULL, "Null ViewState* in MakeKeyFromView\n");
	DocView* pdv = pCamView->GetDocViewPtr();
	ERROR3IF(pdv == NULL, "Null DocView* in MakeKeyFromView\n");
	
	// First, its frame window's position.
	SetRegValue(hViewKey, TEXT("FrameLoX"), REG_DWORD, &pvs->ViewRect.lo.x, sizeof(DWORD));
	SetRegValue(hViewKey, TEXT("FrameLoY"), REG_DWORD, &pvs->ViewRect.lo.y, sizeof(DWORD));
	SetRegValue(hViewKey, TEXT("FrameHiX"), REG_DWORD, &pvs->ViewRect.hi.x, sizeof(DWORD));
	SetRegValue(hViewKey, TEXT("FrameHiY"), REG_DWORD, &pvs->ViewRect.hi.y, sizeof(DWORD));

	// Next, its icon position.
	SetRegValue(hViewKey, TEXT("IconX"), REG_DWORD, &pvs->IconPoint.x, sizeof(DWORD));
	SetRegValue(hViewKey, TEXT("IconY"), REG_DWORD, &pvs->IconPoint.y, sizeof(DWORD));

	// Next, its scale factor.
	DWORD dwScale = (DWORD) pvs->ViewScale.GetRawLong();
	SetRegValue(hViewKey, TEXT("Scale"), REG_DWORD, &dwScale, sizeof(DWORD));

	// Next, its minimise/maximise state etc.
	DWORD dwFlags = (pdv->ViewFlags.GuidesShow		? 0x2000 : 0) |	// these in the DocView
					(pdv->ViewFlags.GuidesSnap      ? 0x1000 : 0) |
					(pdv->ViewFlags.LogicalView     ? 0x0800 : 0) |
					(pdv->ViewFlags.PrintBorderShow ? 0x0400 : 0) |
					(pdv->ViewFlags.MagObjectsSnap  ? 0x0200 : 0) |
					(pdv->ViewFlags.ObjectsSnap		? 0x0100 : 0) |
					(pdv->ViewFlags.GridSnap		? 0x0080 : 0) |
					(pdv->ViewFlags.GridShow		? 0x0040 : 0) |

					(pvs->RulersVisible 	 		? 0x0020 : 0) |	// these in the ViewState
					(pvs->ScrollersVisible 			? 0x0010 : 0) |
					(pvs->AlwaysOnTop 	 			? 0x0008 : 0) |
					(pvs->IsMinimised 	 			? 0x0004 : 0) |
					(pvs->IsMaximised 	 			? 0x0002 : 0) |

					(pCamView->IsTopmost() 			? 0x0001 : 0);	// maintained by MFC

	SetRegValue(hViewKey, TEXT("Flags"), REG_DWORD, &dwFlags, sizeof(DWORD));

	// Next, the scroll position, which unfortunately is two 64-bit quantities.
	UINT32 nLow;
	INT32  nHigh;
	WorkCoord wcScroll = pvs->GetScrollPos(TRUE);

	// Save the x ordinate as a low-high pair of 32-bit numbers.
	wcScroll.x.GetHighLow(&nHigh, &nLow);
	SetRegValue(hViewKey, TEXT("ScrollLoX"), REG_DWORD, &nLow,  sizeof(DWORD));
	SetRegValue(hViewKey, TEXT("ScrollHiX"), REG_DWORD, &nHigh, sizeof(DWORD));

	// Save the y ordinate as a low-high pair of 32-bit numbers.
	wcScroll.y.GetHighLow(&nHigh, &nLow);
	SetRegValue(hViewKey, TEXT("ScrollLoY"), REG_DWORD, &nLow,  sizeof(DWORD));
	SetRegValue(hViewKey, TEXT("ScrollHiY"), REG_DWORD, &nHigh, sizeof(DWORD));

	// Save the z-ordering.
	SetRegValue(hViewKey, TEXT("zOrder"), REG_DWORD, &(pvs->zPos), sizeof(DWORD));
}



/********************************************************************************************
>	static ViewState* MakeViewStateFromKey(HKEY hViewKey, BOOL* pfHadFocus)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/95
	Inputs:		hViewKey		the registry key containing view details
				pfHadFocus		points to a flag that will be set to TRUE if the ViewState
								had the focus when it was saved out, FALSE otherwise
	Returns:	A pointer to a ViewState object representing the data in the key
	Purpose:	Makes a ViewState object from the view data in the given registry key.
	SeeAlso:	LoadAppWindowState; MakeKeyFromViewState
********************************************************************************************/

static ViewState* MakeViewStateFromKey(HKEY hViewKey, BOOL* pfHadFocus)
{
	// Get the position of the view etc and build a ViewState for it.
	ERROR3IF(!pfHadFocus, "No BOOL output parameter in MakeViewStateFromKey");
	ViewState* pvs = new ViewState;
	if (pvs == NULL)
	{
		TRACEUSER( "JustinF", _T("Couldn't allocate a ViewState in MakeViewStateFromKey\n"));
		return NULL;
	}

	// Read in the positional data.
	pvs->ViewRect.lo.x = (INT32) GetRegDword(hViewKey, TEXT("FrameLoX"));
	pvs->ViewRect.lo.y = (INT32) GetRegDword(hViewKey, TEXT("FrameLoY"));
	pvs->ViewRect.hi.x = (INT32) GetRegDword(hViewKey, TEXT("FrameHiX"));
	pvs->ViewRect.hi.y = (INT32) GetRegDword(hViewKey, TEXT("FrameHiY"));
	pvs->IconPoint.x   = (INT32) GetRegDword(hViewKey, TEXT("IconX"));
	pvs->IconPoint.y   = (INT32) GetRegDword(hViewKey, TEXT("IconY"));

	// Dirty trick to read in the fixed-point scale.
	INT32 nScale = (INT32) GetRegDword(hViewKey, TEXT("Scale"));
	pvs->ViewScale = FIXED16::FromRawLong(nScale);

	// Next, its flags etc.
	DWORD dwFlags = GetRegDword(hViewKey, TEXT("Flags"));
	pvs->RulersVisible 	  = (dwFlags & 0x0020) ? TRUE : FALSE;
	pvs->ScrollersVisible = (dwFlags & 0x0010) ? TRUE : FALSE;
	pvs->AlwaysOnTop 	  = (dwFlags & 0x0008) ? TRUE : FALSE;
	pvs->IsMinimised 	  = (dwFlags & 0x0004) ? TRUE : FALSE;

	// Don't restore any as maximised - it just doesn't work with the MDI client window.
	pvs->IsMaximised 	  = /* (dwFlags & 0x0002) ? TRUE : */ FALSE;
	
	// Did this view have the focus when it was saved out?
	*pfHadFocus			  = (dwFlags & 0x0001) ? TRUE : FALSE;

	// That deals with the flags stored in the ViewState.  Set the remainder, which are stored
	// in the associated DocView, in a special place where ScreenCamView::OnInitialUpdate
	// will set them properly.
	pvs->_GridShow 		  = (dwFlags & 0x0040) ? TRUE : FALSE;
	pvs->_GridSnap  	  = (dwFlags & 0x0080) ? TRUE : FALSE;
	pvs->_ObjectsSnap 	  = (dwFlags & 0x0100) ? TRUE : FALSE;
	pvs->_MagObjectsSnap  = (dwFlags & 0x0200) ? TRUE : FALSE;
	pvs->_PrintBorderShow = (dwFlags & 0x0400) ? TRUE : FALSE;
	pvs->_LogicalView 	  = (dwFlags & 0x0800) ? TRUE : FALSE;
	pvs->_GuidesSnap 	  = (dwFlags & 0x1000) ? TRUE : FALSE;
	pvs->_GuidesShow 	  = (dwFlags & 0x2000) ? TRUE : FALSE;

	// Make sure this is marked as a "loaded" view, so that the view system won't override
	// the settings here with default values.
	pvs->IsNewView   	  = FALSE;

	// Next, the scroll position.
	UINT32 xlow  = GetRegDword(hViewKey, TEXT("ScrollLoX"));
	UINT32 ylow  = GetRegDword(hViewKey, TEXT("ScrollLoY"));
	INT32  xhigh	= (INT32) GetRegDword(hViewKey, TEXT("ScrollHiX"));
	INT32  yhigh	= (INT32) GetRegDword(hViewKey, TEXT("ScrollHiY"));

	// Build a WorkCoord.
	WorkCoord wc;
	wc.x.SetHighLow(xhigh, xlow);
	wc.y.SetHighLow(yhigh, ylow);
	pvs->SetScrollPos(wc, FALSE);

	// Finally, the z-order.
	pvs->zPos = GetRegDword(hViewKey, TEXT("zOrder"));

	// Return the ViewState.
	return pvs;
}



/********************************************************************************************
>	BOOL SaveAppWindowState(BOOL fAutoRestart)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95
	Inputss:	fAutoRestart		if TRUE then the registry is set to automatically
									restart Camelot the next time the user logs on
	Returns:	TRUE if successful.
	Purpose:	Saves the document/view window state into the registry/custom .INI file.
	SeeAlso:	LoadAppWindowState
********************************************************************************************/

BOOL SaveAppWindowState(BOOL fAutoRestart)
{
	// Don't do this under Win32s.
	if (IsWin32s() && !IsWin32c()) return FALSE;

	// Don't allow a call twice (now we have two document templates, each will try to call
	// this, so ignore second calls).
	static BOOL fHasBeenCalled = FALSE;
	if (fHasBeenCalled) return TRUE;
	fHasBeenCalled = TRUE;

	// If there's a status line then set it.
	CMainFrame* pMainWnd = GetMainFrame();
	if (pMainWnd != NULL)
	{
		pMainWnd->UpdateStatusBarText(&String_256(_R(IDS_SAVING_WORKSPACE_MSG)), FALSE);
	}

	// First open/create a new sub-key under the app-state key for holding window positions.
	HKEY hMDIKey = CreateRegKey(hAppStateRegKey, TEXT("Workspace\\MDI"));
	if (hMDIKey == NULL)
	{
		ERROR3("Can't get an MDI key in SaveAppWindowState");
		return FALSE;
	}
	
	// Get the 'selected' document, if any.
	Document* pKernelDoc = Document::GetSelected();
	if (pKernelDoc == NULL)
	{
		// There's no docs open to bother with.
		TRACEUSER( "JustinF", _T("No docs open so nothing to do in SaveAppWindowState\n"));

		// Write how many docs and views there are.
		DWORD n = 0;
		SetRegValue(hMDIKey, TEXT("OpenDocCount"), REG_DWORD, &n, sizeof(DWORD));
		SetRegValue(hMDIKey, TEXT("TotalViewCount"), REG_DWORD, &n, sizeof(DWORD));
		CloseRegKey(hMDIKey);
		return TRUE;
	}
	
	// This counts the total number of open docs & views.
	DWORD nDocs = 0;
	INT32 nTotalViewCount = 0;

	// Loop over each document template.
	POSITION posTmp = AfxGetApp()->GetFirstDocTemplatePosition();
	while (posTmp)
	{
		// Get the next doc template.
		CDocTemplate* pTemplate = AfxGetApp()->GetNextDocTemplate(posTmp);

		// Iterate through all the open documents, counting them.
		POSITION posDoc = pTemplate->GetFirstDocPosition();
		while (posDoc)
		{
			// Get this document, skipping any that aren't acting as a server (or hidden).
			CCamDoc* pDoc = (CCamDoc*) pTemplate->GetNextDoc(posDoc);
		#if (_OLE_VER >= 0x200)
			if (pDoc->IsEmbedded() || pDoc->GetKernelDoc()->IsAHiddenDoc()) continue;
		#endif

			// Create a new subkey for this doc.
			TCHAR szDocID[32];
			::wsprintf(szDocID, TEXT("Doc %lu"), ++nDocs);
			HKEY hDocKey = CreateRegKey(hMDIKey, szDocID);
			if (hDocKey == NULL)
			{
				ERROR3("Can't get a doc sub-key in SaveAppWindowState");
				CloseRegKey(hMDIKey);
				return FALSE;
			}
			
			// Iterate through each view on the doc, saving it's state, and counting them.
			DWORD nViews = 0;
			POSITION posView = pDoc->GetFirstViewPosition();
			while (posView)
			{
				// Get this view.
				ScreenCamView* pView = (ScreenCamView*) pDoc->GetNextView(posView);

				// Create a new subkey for this view.
				TCHAR szViewID[32];
				::wsprintf(szViewID, TEXT("View %lu"), ++nViews);
				HKEY hViewKey = CreateRegKey(hDocKey, szViewID);
				if (hViewKey == NULL)
				{
					ERROR3("Can't get a view sub-key in SaveAppWindowState");
					CloseRegKey(hDocKey);
					CloseRegKey(hMDIKey);
					return FALSE;
				}

				// Write the view's state into the key.
				MakeKeyFromView(hViewKey, pView);
				CloseRegKey(hViewKey);

				// COunt this view.
				nTotalViewCount++;
			}

			// Save the state of the doc in the subkey.
			CString strDocPath = pDoc->GetPathName();
			if (strDocPath.IsEmpty())
			{
				// Aha!  There is no path name, which means that either (i) the doc is untitled
				// or (ii) the doc is based on a non-native import, eg. a JPEG, or (iii) the doc is
				// as unsaved copy.  See if it has an 'original' path-name saved by the importer, if
				// so then save that out.  Otherwise save out a null string, signifying an untitled doc.
				strDocPath = pDoc->GetOriginalPath();
				if (strDocPath.IsEmpty()) strDocPath = TEXT("");		// untitled
			}
			
		//		TRACEUSER( "JustinF", _T("SaveAppWindowState: path to doc is %s\n"), (LPCTSTR) strDocPath);
			SetRegValue(hDocKey, TEXT("FilePath"), REG_SZ,
						(LPCTSTR) strDocPath, strDocPath.GetLength() + 1);
			SetRegValue(hDocKey, TEXT("AttachedViewCount"), REG_DWORD, &nViews, sizeof(DWORD));
			CloseRegKey(hDocKey);
		}
	}

	// Write how many docs and views there are.
	SetRegValue(hMDIKey, TEXT("OpenDocCount"), REG_DWORD, &nDocs, sizeof(DWORD));
	SetRegValue(hMDIKey, TEXT("TotalViewCount"), REG_DWORD, &nTotalViewCount, sizeof(DWORD));
	
	CloseRegKey(hMDIKey);


/*************************************************** CCamApp::InitInstance does this for now!
	// Next, get and save the position of the main window.
	WINDOWPLACEMENT wp;
	wp.length = sizeof(wp);
	if (!GetMainFrame()->GetWindowPlacement(&wp))
	{
		ERROR3("Can't get main frame window placement info");
		return FALSE;
	}

	// Save the icon, normal, and maximised positions.
	SetRegValue(hDocKey, TEXT("FrameLoX"), REG_DWORD, &wp.rcNormalPosition.left,   sizeof(DWORD));
	SetRegValue(hDocKey, TEXT("FrameLoY"), REG_DWORD, &wp.rcNormalPosition.top,    sizeof(DWORD));
	SetRegValue(hDocKey, TEXT("FrameHiX"), REG_DWORD, &wp.rcNormalPosition.right,  sizeof(DWORD));
	SetRegValue(hDocKey, TEXT("FrameHiY"), REG_DWORD, &wp.rcNormalPosition.bottom, sizeof(DWORD));
	SetRegValue(hDocKey, TEXT("MaxiX"),    REG_DWORD, &wp.ptMaxPosition.x, 		   sizeof(DWORD));
	SetRegValue(hDocKey, TEXT("MaxiY"),    REG_DWORD, &wp.ptMaxPosition.y, 		   sizeof(DWORD));
	SetRegValue(hDocKey, TEXT("IconX"),    REG_DWORD, &wp.ptMinPosition.x, 		   sizeof(DWORD));
	SetRegValue(hDocKey, TEXT("IconY"),    REG_DWORD, &wp.ptMinPosition.y, 		   sizeof(DWORD));

	// Save the window's state.
	SetRegValue(hDocKey, TEXT(""), REG_DWORD, &wp.showCmd, sizeof(DWORD));
*/

	// Finally, if we are supposed to restart automatically . . .
	if (fAutoRestart)
	{
		// . . . we write a command-line to run under a special key.
		HKEY hRunOnceKey = CreateRegKey(HKEY_CURRENT_USER,
								TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce"));
		if (hRunOnceKey == NULL)
		{
			ERROR3("Can't get a RunOnce key in SaveAppWindowState");
			return FALSE;
		}

		// Windows will run this command-line on startup.
		TCHAR szAutoCmd[_MAX_PATH * 2];
		::wsprintf(szAutoCmd, TEXT("%s /Restore"), (LPCTSTR) szCamelotExePath);
		SetRegValue(hRunOnceKey, szCamelotAppRegistryID, REG_SZ,
					szAutoCmd, ::camStrlen(szAutoCmd));
		CloseRegKey(hRunOnceKey);

		// If there's a status line then set it.
		TRACEUSER( "JustinF", _T("\nSee you later . . .\n\n"));
		StatusLine::SetPrefix(String_256(_R(IDS_SEE_YOU_LATER_MSG)));
	}
	
	// Success!
	return TRUE;
}



/********************************************************************************************
>	BOOL LoadAppWindowState(CWnd* pSplashBox)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95
	Inputs:		pSplashBox		pointer to the splash box window displayed on program
								startup.  This is repeatedly put to the front of the
								z-order whenever a view window is created.
	Returns:	TRUE if there were windows to restore, FALSE if there weren't or it all
				went wrong.
	Purpose:	Loads the document/view window state from the registry/custom .INI file.
	SeeAlso:	SaveAppWindowState
********************************************************************************************/

BOOL LoadAppWindowState(CWnd* pSplashBox)
{
	// Check if we want to restore the workspace.
	if (!fRestoreWorkspaceOnStartup || (IsWin32s() && !IsWin32c())) return FALSE;

	// First open/create a new sub-key under the app-state key for holding window positions.
	HKEY hWorkspaceKey = CreateRegKey(hAppStateRegKey, TEXT("Workspace"));
	if (hWorkspaceKey == NULL)
	{
		ERROR3("Can't get a workspace key in LoadAppWindowState");
		return FALSE;
	}

	HKEY hMDIKey = CreateRegKey(hWorkspaceKey, TEXT("MDI"));
	if (hMDIKey == NULL)
	{
		ERROR3("Can't get an MDI key in LoadAppWindowState");
		CloseRegKey(hWorkspaceKey);
		return FALSE;
	}

	// If there's a status line then set it.
	StatusLine::SetPrefix(String_64(_R(IDS_LOADING_WORKSPACE_MSG)));

	// We really DON'T know anything about any filters to use . . .
	BaseFileDialog::SelectedFilter = 0;

	// Read in the number of documents & views.
	INT32 nTotalDocs = GetRegDword(hMDIKey, TEXT("OpenDocCount"));
	INT32 nTotalViews = GetRegDword(hMDIKey, TEXT("TotalViewCount"));
//	TRACEUSER( "JustinF", _T("Restoring %d docs and %d views\n"), nTotalDocs, nTotalViews);

	// Maybe prevent maximising views.
	fDontMaximiseViews = (nTotalViews > 1);

	// Allocate an array to hold the address of each view frame, indexed by their z-order.
	CCamMDIChild** pzOrder = new CCamMDIChild*[nTotalViews];
	ERRORIF(pzOrder == NULL, FALSE, _R(IDE_NOMORE_MEMORY));
	memset(pzOrder, (INT32) NULL, nTotalViews * sizeof(CCamMDIChild*));

	// Find out how many open docs are stored away and loop over the required number of docs.  As
	// we load each view we will see if it had the focus, and remember which one had it.
	CCamMDIChild* pFocusFrame = NULL;
	BOOL fAtLeastOneLoaded = FALSE;
	for (DWORD nDoc = 1; nDoc <= (DWORD) nTotalDocs; nDoc++)
	{
//		TRACEUSER( "JustinF", _T("restoring doc %ld\n"), (UINT32) nDoc);

		// Get the sub-key for the doc.
		TCHAR szDocID[32];
		::wsprintf(szDocID, TEXT("Doc %ld"), nDoc);
		HKEY hDocKey = OpenRegKey(hMDIKey, szDocID);
		if (hDocKey == NULL)
		{
			TRACEUSER( "JustinF", _T("Can't find %s sub-key in LoadAppWindowState\n"), (LPCTSTR) szDocID);
			continue;
		}

		// Get the file-path, if any.
		String_256 strPath;
		if (!GetRegString(hDocKey, TEXT("FilePath"), &strPath))
		{
			TRACEUSER( "JustinF", _T("Can't get file-path in LoadAppWindowState\n"));
			continue;
		}

		// This code is taken from CWinApp::OnFileNew in MFC\SRC\APPDLG.CPP.
		// Get the app's document template.
#if (_MFC_VER < 0x400)
		// MFC2/3 compatible code
		CPtrList& templateList = AfxGetApp()->m_templateList;
		ERROR3IF(templateList.GetCount() > 1, "Too many doc templates in LoadAppWindowState");
		CDocTemplate* pTemplate = (CDocTemplate*) templateList.GetHead();
#else
		// MFC4 compatible code
		POSITION Pos = AfxGetApp()->GetFirstDocTemplatePosition();
		CDocTemplate* pTemplate = AfxGetApp()->GetNextDocTemplate(Pos);
#endif
		ERROR3IF(pTemplate == NULL, "No doc template in LoadAppWindowState");
		ERROR3IF(!pTemplate->IsKindOf(RUNTIME_CLASS(CDocTemplate)),
					"Not a doc template in LoadAppWindowsState");

		// Find out how many more views and create each view at the given position.
		CCamDoc* pDoc;
		CCamMDIChild* pFrame;
		DWORD nViewCount = GetRegDword(hDocKey, TEXT("AttachedViewCount"));
		for (DWORD nView = 1; nView <= nViewCount; nView++)
		{
//			TRACEUSER( "JustinF", _T("\trestoring view %ld\n"), (UINT32) nView);

			// Get the sub-key for the view.
			TCHAR szViewID[32];
			::wsprintf(szViewID, TEXT("View %lu"), nView);
			HKEY hViewKey = OpenRegKey(hDocKey, szViewID);
			if (hViewKey == NULL)
			{
				TRACEUSER( "JustinF", _T("\tCan't find %s sub-key in LoadAppWindowState\n"),
									 (LPCTSTR) szViewID);
				continue;
			}

			// Make a ViewState if we can.
			BOOL fHadFocus;
			ViewState* pvs = MakeViewStateFromKey(hViewKey, &fHadFocus);
			if (pvs == NULL)
			{
				delete[] pzOrder;

				CloseRegKey(hViewKey);
				CloseRegKey(hMDIKey);
				CloseRegKey(hWorkspaceKey);

				StatusLine::SetDefaultPrefix();
				fDontMaximiseViews = FALSE;
				ERROR(_R(IDE_NOMORE_MEMORY), FALSE);
			}

			// OK, now we've finished with the view delete it from the registry.
			DeleteRegKey(hDocKey, szViewID);

			// Save the z-order position in it, cos as soon as we create the view this will be trashed,
			// and whether it had the focus..
			INT32 zPos = pvs->zPos;

			// Set it to be the ViewState to use for subsequent creation.
			ScreenCamView::SetReadyMadeViewState(pvs);

			// Create the view.  If the view is the first onto a doc then create the doc
			// as well.
			if (nView == 1)
			{
				// Prevent the normal doc importer from moving the view onto the doc etc.
				Document::ShouldRestoreViewOnImport(FALSE);

				// First view, so create/open the doc.
				if (strPath.IsEmpty())
				{
					// If returns NULL, the user has already been alerted.  Note that this will create
					// a single view window already attached to the doc.
					pDoc = (CCamDoc*) pTemplate->OpenDocumentFile(NULL);
				}
				else
				{
					// We have a path to the doc, so open it.
					pDoc = (CCamDoc*) AfxGetApp()->OpenDocumentFile(strPath);
					((CCamApp*) AfxGetApp())->MakeDocumentNative(pDoc, &PathName(strPath));
				}

				// Reset this to the default setting.
				Document::ShouldRestoreViewOnImport(TRUE);
				
				// Succeeded in creating the doc?
				if (pDoc != NULL)
				{
					// Yes, so get the address of the view's frame and make it active.
					POSITION pos = pDoc->GetFirstViewPosition();
					pFrame = (CCamMDIChild*) (pDoc->GetNextView(pos)->GetParentFrame());
					pFrame->MDIActivate();

					// Sort out the extension of the doc etc.
					fAtLeastOneLoaded = TRUE;
					pDoc->SetOriginalPath(strPath);

//					TRACEUSER( "JustinF", _T("\t\tdoc is %s\n"),
//										 (LPCTSTR) ((strPath.IsEmpty()) ? TEXT("new") : strPath));
				}
				else
				{
					// Failed to create initial doc, don't make any views.
					TRACEUSER( "JustinF", _T("\t\tFailed to create doc %lu in LoadAppWindowState\n"),
											(UINT32) nDoc);
					ScreenCamView::SetReadyMadeViewState(NULL);
					delete pvs;
					break;
				}
			}
			else
			{
				// Not the first view, so make a new view onto the doc.
				pFrame = (CCamMDIChild*) pTemplate->CreateNewFrame(pDoc, pFrame);
				if (pFrame == NULL)
				{
					TRACEUSER( "JustinF", _T("\t\tCouldn't create frame in LoadAppWindowState\n"));
					continue;
				}

				// Draw and activate the view.
				pTemplate->InitialUpdateFrame(pFrame, pDoc, TRUE);
				pFrame->MDIActivate();
			}

			// If a splashbox is up then reset it to the front of the z order.
			if (pSplashBox != NULL) pSplashBox->SetActiveWindow();
		
			// Save the view frame's address in the appropriate z-order slot.
			ERROR3IF(zPos >= nTotalViews, "Out of range zPos in LoadAppWindowState");
			if (zPos < nTotalViews) pzOrder[zPos] = pFrame;
			if (fHadFocus) pFocusFrame = pFrame;

			// Argh!! Murder! I can't get the view titles to update properly, no matter what
			// I do!  So I'm going to cheat here and if necessary append the view number to
			// the frame title.
			if (nViewCount == 1)
			{
				pFrame->m_nWindow = 0;
			}
			else
			{
				TCHAR szTitle[_MAX_PATH + 32], szNewTitle[_MAX_PATH + 32];
				pFrame->GetWindowText(szTitle, sizeof(szTitle) / sizeof(szTitle[0]));
				::wsprintf(szNewTitle, TEXT("%s:%lu"), (LPCTSTR) szTitle, (UINT32) nView);
				pFrame->SetWindowText(szNewTitle);
				pFrame->m_nWindow = nView;
			}
		}

		// OK, now we've finished with the doc delete its sub-key.
		CloseRegKey(hDocKey);
		DeleteRegKey(hMDIKey, szDocID);
	}

	// Restore this flag.
	fDontMaximiseViews = FALSE;

	// Check if we had a cataclysmic failure.
	if (!fAtLeastOneLoaded)
	{
		TRACEUSER( "JustinF", _T("Couldn't restore even one doc - wiping MDI key\n"));
		DeleteRegKey(hWorkspaceKey, TEXT("MDI"));
		CloseRegKey(hWorkspaceKey);
		delete[] pzOrder;

		StatusLine::SetDefaultPrefix();
		return FALSE;
	}
	
	// All done with the registry.
	CloseRegKey(hMDIKey);
	CloseRegKey(hWorkspaceKey);

	// OK, now we've finished loading all the docs and creating all their views, set their
	// z-order.
//	TRACEUSER( "JustinF", _T("Setting position of views: "));
	CWnd* pwndLast = (CWnd*) &CWnd::wndTop;
	for (INT32 i = 0; i < nTotalViews; i++)
	{
		CCamMDIChild* pFrame = pzOrder[i];
		if (pFrame != NULL)
		{
//			TRACEUSER( "JustinF", _T(" %d"), i + 1);
			pFrame->SetWindowPos(pwndLast, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
			pwndLast = pFrame;
		}
	}

//	TRACEUSER( "JustinF", _T("\n"));

	// Finally, set the focus to the appropriate view, if we found one.
	if (pFocusFrame != NULL) pFocusFrame->MDIActivate();

	// Success!
	delete[] pzOrder;
	StatusLine::SetDefaultPrefix();
	return TRUE;
}



/********************************************************************************************
>	BOOL InitAppState()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Returns:	TRUE if successfully initialises the app-state system.
	Purpose:	Initialises the app-state system, reading prefs.
	SeeAlso:	LoadAppWindowState
********************************************************************************************/

BOOL InitAppState()
{
	// First, declare this pref.
	ERROR2IF(!GetApplication()->DeclareSection(TEXT("Workspace"), 5) ||
			 !GetApplication()->DeclarePref(TEXT("Workspace"), TEXT("RestoreOnStartup"),
							 			    &fRestoreWorkspaceOnStartup, FALSE, TRUE),
			 FALSE,
			 _R(IDE_BAD_INI_FILE));

//	TRACEUSER( "JustinF", _T("%s restore workspace on startup\n"),
//			  (LPCTSTR) ((fRestoreWorkspaceOnStartup) ? TEXT("Will") : TEXT("Won't")));

	// Success!
	return TRUE;
}
