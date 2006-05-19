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
// Custom File Open and Save Dialogs

#include "camtypes.h"
#include "camelot.h"		// IsNewWindowsUI

#include "filedlgs.h"
#include "app.h"
#include "camframe.h"
//#include "rik.h"
//#include "andy.h"
//#include "mario.h"			// for _R(IDE_NOMORE_MEMORY)
//#include "nev.h"			// for _R(IDN_IMPORTBUTTON) etc
//#include "resource.h"
#include "oilfltrs.h"
#include "fixmem.h"
#include "native.h"			// The new designed native filter, used for v2
#include "nativeps.h"		// The old style EPS native filter, used in v1.1
//#include "custmsg.h"
#include "helpuser.h"
#include "camelot.h"
#include "filters.h"
#include "bmapprev.h"
#include "bmpfiltr.h"

#include "palman.h"

// WEBSTER - markn 28/1/97
//#include "tim.h"
#include "webparam.h"
#include "xarprefs.h"
#include "webprefs.h"
#include "webfiltr.h"

//#include "richard2.h"		// For resource strings

#include "ctrlhelp.h"
#include "bitfilt.h"		// BaseBitmapFilter

//#include "xardlg.h"			// For the base dialogue for the web dialogue.
//#include "webdlg.h"			// Additional controls in _R(IDD_SAVEWEBOPTS)
//#include "filtrres.h"		// _R(IDS_FILTEREXT_WEB)
//#include "reshlpid.h"       // _R(ID_CC_HELP_BUTTON) 

// New for preview bitmaps
#include "dlgcol.h"			// DialogColourInfo
#include "dlgview.h"		// DialogView
#include "docview.h"		// DocView
#include "osrndrgn.h"		// OSRenderRegion
#include "bitmap.h"			// KernelBitmap
#include "nodebmp.h"		// NodeBitmap
#include "bitmpinf.h"		// BitmapInfo
#include "oilbitmap.h"		// CWxBitmap
#ifndef WEBSTER
//#include "extfilts.h"		// AccusoftFilters
#endif //WEBSTER
//#include "cmxifltr.h"		// CMXImportFilter
#include "webaddr.h"		// Olivier 09/05/97, use of the WebAddress class
#include "aprps.h"				
#include "exjpeg.h"
#include "ngprop.h"			// for NamedExportProp::m_fApplyNotExport
//#include "bmpreres.h"		// for _R(IDS_PREVIEW_APPLY)

//#include "resimmap.h"		// for new templates stuff
#include "menuops.h"
#include "progress.h"

#include "qualattr.h"
//#include <shfolder.h> // For SHGetFolderPath

void FixFPControlRegister();

// the longest name we can display as a path, before going down to the basename
#define	MAX_ALLOWED_NAME_LENGTH	60

#define NumberOfNames 60

const DWORD FCIDM_SHVIEW_THUMBNAIL = 0x702D;

String_256 BaseFileDialog::DefaultSaveFilePath = TEXT("");
String_256 BaseFileDialog::DefaultOpenFilePath = TEXT("");
INT32        BaseFileDialog::SelectedFilter = FILTERID_VECTOR;	// changed from FILTERID_GENERIC
BOOL 	   BaseFileDialog::MakeBakFiles = FALSE;
BOOL 	   BaseFileDialog::ShowPreviewBitmap = TRUE;
BOOL 	   BaseFileDialog::ThumbNailMode = FALSE;

BOOL 	   SaveTemplateDialog::m_fUseAsDefault = TRUE;
BOOL 	   SaveTemplateDialog::m_fDefaultTemplatesFolder = TRUE;


// This points at the current running file dialog.  NB. this will only allow one file
// dialog to be open at any one time!
BaseFileDialog* BaseFileDialog::m_pCurrentFileDialog = NULL;

// Run-time type enquiry mechanisms.  These are required by the on-line help system.
IMPLEMENT_CLASS(BaseFileDialog, 	wxFileDialog);
IMPLEMENT_CLASS(OpenFileDialog, 	BaseFileDialog);
IMPLEMENT_CLASS(SaveFileDialog, 	BaseFileDialog);
IMPLEMENT_CLASS(SaveTemplateDialog, SaveFileDialog);
IMPLEMENT_CLASS(ImportFileDialog,	BaseFileDialog);
IMPLEMENT_CLASS(ExportFileDialog,	BaseFileDialog);
IMPLEMENT_CLASS(GIFExportFileDialog,ExportFileDialog);

PORTNOTE("other", "Removed MFC MESSAGE_MAPs" )
#if !defined(EXCLUDE_FROM_XARALX)
// Need to ensure we get redraw messages back from the dialog box
// so that we can redraw the preview bitmap
BEGIN_MESSAGE_MAP(OpenFileDialog, BaseFileDialog)
	ON_BN_CLICKED(_R(ID_CC_HELP_BUTTON), BaseFileDialog::OnHelp)		// Windows 95 form
	ON_BN_CLICKED(_R(IDC_SHOWPREVIEW), BaseFileDialog::OnShowPreviewBitmap)		// Windows 95 form
	ON_MESSAGE(WM_REDRAW, OnRedrawMessage)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// Map the buttons on the save dialog to the web export options handling functions
// Need two forms for the Windows 95 UI and Old Windows UI types.
// This code is all negated now and so is disabled.
// But now we have a preview bitmap on the dialog box
BEGIN_MESSAGE_MAP(SaveFileDialog, BaseFileDialog)
	ON_BN_CLICKED(_R(ID_CC_HELP_BUTTON), BaseFileDialog::OnHelp)		// Windows 95 form
	ON_BN_CLICKED(_R(IDC_SHOWPREVIEW), BaseFileDialog::OnShowPreviewBitmap)		// Windows 95 form
	// WEBSTER - markn 28/1/97
	ON_BN_CLICKED(_R(IDC_FILEDLG_OPTIONS), BaseFileDialog::OnOptions)			// Windows 95 form
	ON_MESSAGE(WM_REDRAW, OnRedrawMessage)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(SaveTemplateDialog, BaseFileDialog)
	ON_BN_CLICKED(_R(ID_CC_HELP_BUTTON), BaseFileDialog::OnHelp)		// Windows 95 form
	ON_BN_CLICKED(_R(IDC_SHOWPREVIEW), BaseFileDialog::OnShowPreviewBitmap)		// Windows 95 form
	ON_BN_CLICKED(_R(IDC_USEASDEFAULT), SaveTemplateDialog::OnUseAsDefault)		// Windows 95 form
	ON_MESSAGE(WM_REDRAW, OnRedrawMessage)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// Need to ensure we get redraw messages back from the dialog box
// so that we can redraw the preview bitmap
BEGIN_MESSAGE_MAP(ImportFileDialog, BaseFileDialog)
	ON_BN_CLICKED(_R(ID_CC_HELP_BUTTON), BaseFileDialog::OnHelp)		// Windows 95 form
	ON_BN_CLICKED(_R(IDC_SHOWPREVIEW), BaseFileDialog::OnShowPreviewBitmap)		// Windows 95 form
	ON_COMMAND(IDOK, OnOK)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_MESSAGE(WM_REDRAW, OnRedrawMessage)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(ExportFileDialog, BaseFileDialog)
	ON_BN_CLICKED(_R(ID_CC_HELP_BUTTON), BaseFileDialog::OnHelp)			// Windows 95 form
	ON_BN_CLICKED(_R(IDC_FILEDLG_OPTIONS), BaseFileDialog::OnOptions)
	ON_BN_CLICKED(_R(IDC_SHOWPREVIEW), BaseFileDialog::OnShowPreviewBitmap)	// Windows 95 form
	ON_BN_CLICKED(_R(IDC_AUTO_CHECK), OnAutoPreview)						// Windows 95 form
	ON_MESSAGE(WM_REDRAW, OnRedrawMessage)
	ON_WM_SIZE()
END_MESSAGE_MAP()
#endif

/********************************************************************************************
>	BaseFileDialog::BaseFileDialog(BOOL IsFileOpen, WORD Flags, LPCSTR lpszFilter=NULL,
				wxWindow* pParentWnd=NULL)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & JustinF
	Created:	26/1/95
	Inputs:		IsFileOpen - TRUE if this is a File Open dialog, FALSE if it is a Save Dialog
				Flags - The flags you want set in the CFileDialog.
				lpszFilter - The Filter String
				pParentWnd - The owner of this window (by default the main frame window)
	Purpose:	Creates a base class file dialog, installing a hook etc.

Technical Notes:
We set the OFN_EXPLORER on Windows 95 only. Really this should include the new shell on NT
when we work with it. If you set this flag on NT without the new shell, then you get a useful
not enough memory or similar error and the dialog box fails to open. At present, the user will
see nothing as we don't seem to cope with errors.
We need to set the flag as some functions e.g. ExportFileDialog::OnTypeChange() will only
work with it set. Used to fix the filenames up when switching to a new filter.
********************************************************************************************/

// Visual C (MFC) 4 defines this constant, but Visual C 2 does not
#ifndef OFN_EXPLORER
#define OFN_EXPLORER 0
#endif

BaseFileDialog::BaseFileDialog(BOOL IsFileOpen, DWORD dwFlags, LPCTSTR lpszFilter, wxWindow* pParentWnd)
  :	wxFileDialog( (pParentWnd != NULL) ? pParentWnd : pParentWnd = GetMainFrame(), _T("Choose a file"),
		_T(""), _T(""), lpszFilter, dwFlags | ( IsFileOpen ? wxOPEN : wxSAVE) )
// 	IsFileOpen, NULL, NULL, dwFlags | ((CCamApp::IsNewWindowsUI()) ? OFN_EXPLORER : OFN_SHOWHELP) | OFN_ENABLEHOOK,
//	lpszFilter, )
{
PORTNOTE("other", "Removed MFC junk" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Set the address of the message hook procedure.
	m_ofn.lpfnHook = (LPOFNHOOKPROC) HookProc;

	// Unfortunately, MFC doesn't set the hwndOwner member of m_ofn . . .
	m_ofn.hwndOwner = (pParentWnd != NULL) ? pParentWnd->m_hWnd : NULL;

	// We haven't called the explorer style init function by default
	ExplorerInited = FALSE;
#endif
	
	// Remember a pointer to this file dialog.
	m_pCurrentFileDialog = this;

	// Set the preview bitmap to be NULL
	pBitmapToUse = NULL;

	// Added by Craig Hamilton 20/9/00.
	m_previousPathName = _T("");
	// End added.
}


/********************************************************************************************

>	BaseFileDialog::~BaseFileDialog()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/96
	Returns:	-
	Purpose:	Standard destructor. Need to clean up the preview bitmap, if present.

********************************************************************************************/

BaseFileDialog::~BaseFileDialog()
{
	// Must clean up our preview bitmap, if present
	if (pBitmapToUse != NULL)
	{
		delete pBitmapToUse;
		pBitmapToUse = NULL;
	}
	m_pCurrentFileDialog = NULL;
}


PORTNOTE("other", "Removed MFC type Hooks" )
#if !defined(EXCLUDE_FROM_XARALX)
/********************************************************************************************
>	static UINT32 CALLBACK BaseFileDialog::HookProc(wxWindow* hwnd, UINT32 nMsg,
												  WPARAM wParam, LPARAM lParam)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		(the usual message proc paramters, see Win32 SDK documentation)
	Returns:	FALSE if the dialog library should process this message, TRUE if it
				should ignore it.
	Purpose:	Called by the common dialog library when it has a message to process.
				Calls the virtual OnHookMsg function.
	SeeAlso:	BaseFileDialog::OnHookMsg
********************************************************************************************/

UINT32 CALLBACK BaseFileDialog::HookProc(wxWindow* hwnd, UINT32 nMsg, WPARAM wParam, LPARAM lParam)
{
	// If there is no current file dialog then just say nothing and do nothing
	// This MAY happen if we are bodging the web filename file dialog which is to get
	// round having double file dialogs on screen, which is BAD!
	if (m_pCurrentFileDialog == NULL) // || m_pCurrentFileDialog->hwnd == NULL)
		return FALSE;

	return m_pCurrentFileDialog->OnHookMsg(hwnd, nMsg, wParam, lParam);
}



/********************************************************************************************
>	virtual UINT32 BaseFileDialog::OnHookMsg(wxWindow* hwnd, UINT32 nMsg, WPARAM wParam, LPARAM lParam)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		(the usual message proc parameters)
	Returns:	FALSE if the dialog library should process this message, TRUE if it
				should ignore it.
	Purpose:	Called by the common dialog library when it has a message to process.
				This function checks if the help button has been clicked, and runs the
				help engine if it has.
	SeeAlso:	BaseFileDialog::HookProc
********************************************************************************************/

UINT32 BaseFileDialog::OnHookMsg(wxWindow* hwnd, UINT32 nMsg, WPARAM wParam, LPARAM lParam)
{
	// Check for a help message.
	if (nMsg == WM_COMMAND && LOWORD(wParam) == pshHelp)
	{
		// Run the help engine for this dialog and don't bother processing this message
		// any further, we've done it.
		HelpUser(*this);
		return TRUE;
	}

	// Allow explorer style dialog boxes to set themselves up
	if (nMsg == WM_INITDIALOG && CCamApp::IsNewWindowsUI())
	{
		// Only call this if we are the new explorer dialog boxes
		return ExplorerInitDialog(hwnd);
	}

	// WM_NOTIFY notification messages indicates actions taken by the user in the dialog box.
	// The lParam parameter for each WM_NOTIFY message is the address of a OFNOTIFY structure
	// that defines the action. The code member in the header for the OFNOTIFY structure
	// contains the notification values
	if (nMsg == WM_NOTIFY && lParam != NULL)
	{
		OFNOTIFY * pNotify = (OFNOTIFY*)lParam;
		// This is used to inform explorer style dialogs of actions
		if (CCamApp::IsNewWindowsUI())
			return ExplorerFileHookNotify(hwnd, pNotify);
	}

	//TODO: change 'File name:' to 'File name (or URL):' (WARNING: change _R(ID_OF_FILENAME) with the correct ID)
	//SetDlgItemText(hwnd, _R(ID_OF_FILENAME), (TCHAR *)String_256("File name (or URL):"));

#ifndef RALPH
	// By default let MFC handle all other messages in its own hook procedure.
	return _AfxCommDlgProc(hwnd, nMsg, wParam, lParam);
#else
	// Why doesn't this work with _AFXDLL defined ?
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL BaseFileDialog::ExplorerInitDialog(wxWindow* hDlg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/7/96
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Should be called when the explorer style dialog boxes need setting up.
				We can take this opportunity to add in any extra controls that we require.
				Baseclass version.
	SeeAlso:	BaseFileDialog::OnHookMsg; BaseFileDialog::HookProc;

********************************************************************************************/

BOOL BaseFileDialog::ExplorerInitDialog(wxWindow* hDlg)
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL BaseFileDialog::ExplorerFileHookNotify(wxWindow* hDlg, LPOFNOTIFY pofn)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Inputs:		hDlg - wxWindow* of the dialog box.
 				pofn - pointer to notify structure
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Should be called when actions happen on the explorer style dialog boxes.
				We can take this opportunity to handle these actions as required.
				Baseclass version.
	SeeAlso:	BaseFileDialog::OnHookMsg; BaseFileDialog::HookProc;

********************************************************************************************/

BOOL BaseFileDialog::ExplorerFileHookNotify(wxWindow* hDlg, LPOFNOTIFY pofn)
{
	return TRUE;
}

/********************************************************************************************

>	virtual UINT32 BaseFileDialog::OnFileTypeChange()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/96
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Should be called when the user has selected a new file type from the
				"Files of type" combo box. We might need to do things like fix the file
				extension for the currently selected save/export filter.
				Baseclass version.
	SeeAlso:	BaseFileDialog::OnHookMsg; BaseFileDialog::HookProc;

********************************************************************************************/

UINT32 BaseFileDialog::OnFileTypeChange()
{
	return TRUE;
}
#endif

/********************************************************************************************

>	BOOL BaseFileDialog::Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/95
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Declares some preferences for the Open and Save dialogs

********************************************************************************************/

BOOL BaseFileDialog::Init()
{
	// Set some defaults
	String_256 Blank("");
	DefaultSaveFilePath = Blank;
	DefaultOpenFilePath = Blank;

	// declare prefs for the open and save dialogs
	Camelot.DeclareSection(_T("Filters"), 6);
	Camelot.DeclarePref(_T("Filters"), _T("DefaultSaveFilePath"), &DefaultSaveFilePath);
	Camelot.DeclarePref(_T("Filters"), _T("DefaultOpenFilePath"), &DefaultOpenFilePath);
	Camelot.DeclarePref(_T("Filters"), _T("OpenFileFilter"), &SelectedFilter);
	Camelot.DeclarePref(_T("Filters"), _T("MakeBakFiles"), &MakeBakFiles);
	Camelot.DeclarePref(_T("Filters"), _T("ShowPreviewBitmap"), &ShowPreviewBitmap);
	Camelot.DeclarePref(_T("Filters"), _T("ThumbNailMode"), &ThumbNailMode);


	return TRUE;
}



/********************************************************************************************

>	BOOL BaseFileDialog::PrepareDialog()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Does anything the dialogs want doing after they have been created, but before
				they are displayed.

********************************************************************************************/

BOOL BaseFileDialog::PrepareDialog()
{
	// Nothing to do in this base class . . .
	return TRUE;
}


/********************************************************************************************

>	static char *BaseFileDialog::BuildFilterString(BOOL GetImport,
							   	   				   UINT32 SelectedFilterID,
												   INT32 *SelectedFilter,
							      				   CCRuntimeClass *pTypeOfFilter = GenericFilter,
							      				   UINT32 Bpp = 0)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/94
	Inputs:		GetImport - TRUE => Build list of Import filters
						   FALSE => Build list of Export filters
				SelectedFilterID - the ID of the last filter that was used by the
								   calling Operation.
				pTypeOfFilter	 - optional type of filter to use. Defaults to GenericFilter
								   for all.
				Bpp				 - if only bitmap filters required then check this bpp against
								   the ones that the filter supports 
	Outputs:	SelectedFilter - the position in the list box of the selected filter
								 identified by SelectedFilterID, or unchanged if it wasn't 
								 found.
	Returns:	The filter string.
	Purpose:	Build a filter string suitable for use by Windows' common dialog 
				"File open" or "File save" boxes.
				This is mainly for the import and export dialog boxes.
				Used to be in OilFiltrs.cpp until moved to BaseFileDialog 5/1/96.
	Errors:		Out of memory
	SeeAlso:	MetaFileFilter;
	SeeAlso:	OpMenuExport; OpMenuImport;

********************************************************************************************/

TCHAR *BaseFileDialog::BuildFilterString(BOOL GetImport,
									    UINT32 SelectedFilterID,
									    INT32 *SelectedFilter,
									    CCRuntimeClass *pTypeOfFilter,
									    UINT32 Bpp,
										UINT32 BitmapCount)
{
	// Find out how much space is needed
	UINT32 FilterSize = 0;

	// Used to load in the resource strings for the filter extension descriptions.
	String_256 FilterExtensions;

	Filter *pFilter = Filter::GetFirst();
	BOOL FilterCanCope = TRUE;
	while (pFilter != NULL)
	{
		// If we are filtering for bitmap filters then check that if a Bpp has been
		// specified that the bitmap filter can cope with that.
		FilterFlags Flags = pFilter->GetFlags();
		if (pTypeOfFilter==CC_RUNTIME_CLASS(BitmapFilter) && pFilter->IsKindOf(pTypeOfFilter))
		{
			BaseBitmapFilter* pBitmapFilter = (BaseBitmapFilter*)pFilter;
			if (pBitmapFilter->IsThisBppOk(Bpp))
				FilterCanCope = TRUE;	
			else
				FilterCanCope = FALSE;

			if (BitmapCount>1 && !Flags.CanExportMultipleImages)
				FilterCanCope = FALSE;
		}
		else
			FilterCanCope = TRUE;
				
		// little bit of a bodge here to stop the CamelotEPSFilter from appearing in the
		// import list - you can't hide it from just one list. It must be availiable though
		// to give it's nice little error message. Other instances of this below.
		if (
PORTNOTE("other", "Removed CamelotEPSFilter check" )
#if !defined(EXCLUDE_FROM_XARALX)
			( ( GetImport && Flags.CanImport && !IS_A( pFilter, CamelotEPSFilter ) )
#else
			( ( GetImport && Flags.CanImport )
#endif
				|| (!GetImport && Flags.CanExport)) && NULL != pFilter->pOILFilter &&
				Flags.ShowFilter && (pFilter->IsKindOf(pTypeOfFilter)) && FilterCanCope
		   )
		{
			// Add the space required to put this into the filter string
			// (add 1 for the separating '|' character)
			FilterSize += camStrlen(pFilter->pOILFilter->ConstructFilterString(BitmapCount)) + 1;
		}

		// Try the next filter
		pFilter = Filter::GetNext(pFilter);
	}

	// If we have found no valid filters then we must set an error and return NULL
	// as otherwise we are in an illegal situation
	if (FilterSize == 0)
	{
		Error::SetError(_R(IDE_FORMATNOTSUPPORTED));
		return NULL;
	}
	
	
	// Add two for the final "|" and the 0 terminator.
	FilterSize += 2;

	// Try to get this string
	TCHAR *FilterString = (TCHAR *) CCMalloc(FilterSize * sizeof(TCHAR));
	if (FilterString==NULL)
	{
		// Error state already set by CCMalloc
		return NULL;
	}
	
	// Construct the string, and set up the position numbers for the filters.
	UINT32 Position = 0;
	FilterString[0] = 0;
	pFilter = Filter::GetFirst();

	while (pFilter != NULL)
	{
		// If we are filtering for bitmap filters then check that if a Bpp has been
		// specified that the bitmap filter can cope with that.
		FilterFlags Flags = pFilter->GetFlags();
		if (pTypeOfFilter==CC_RUNTIME_CLASS(BitmapFilter) && pFilter->IsKindOf(pTypeOfFilter))
		{
			BaseBitmapFilter* pBitmapFilter = (BaseBitmapFilter*)pFilter;
			if (pBitmapFilter->IsThisBppOk(Bpp))
			{
				// Webster - RanbirR - 25\02\97  
				// If this is a single bitmap and the the Bpp is 24, 
				// then we do not want to dipsplay *.gif in the export filter List.
				// Only allow the format for Animated GIF's (even though they do not 
				// support 24 colour bitmaps.) We later convert to an 8 Bpp. 
				// This fixes an animated GIF export bug in Camelot.
				// At present we error if the first bitmap in our animated gif is 24Bpp.
PORTNOTE("filter", "Removed use fo GIFFilter")
#ifndef EXCLUDE_FROM_XARALX
				if (pFilter->FilterID ==  FILTERID_TI_GIF && Bpp == 24 && BitmapCount == 1)
					FilterCanCope = FALSE;

				else
#endif
					FilterCanCope = TRUE;
			}
			else
				FilterCanCope = FALSE;

			if (BitmapCount>1 && !Flags.CanExportMultipleImages)
				FilterCanCope = FALSE;
		}
		else
			FilterCanCope = TRUE;

		if( NULL != pFilter->pOILFilter )
		{
			if (
PORTNOTE("other", "Removed CamelotEPSFilter check" )
#if !defined(EXCLUDE_FROM_XARALX)
				( ( GetImport && Flags.CanImport && !IS_A( pFilter, CamelotEPSFilter ) )
#else
				( ( GetImport && Flags.CanImport )
#endif
					|| (!GetImport && Flags.CanExport)) &&
					Flags.ShowFilter && (pFilter->IsKindOf(pTypeOfFilter)) && FilterCanCope
			   )
			{
				// Add this filter into the list
				camStrcat(FilterString, pFilter->pOILFilter->ConstructFilterString(BitmapCount));
				camStrcat(FilterString, _T("|"));
				pFilter->pOILFilter->Position = Position;

				// Is this the filter we used last?
				if ((*SelectedFilter == -1) && (pFilter->FilterID == SelectedFilterID))
					// Yes - remember this.
					*SelectedFilter = Position;

				Position++;
			}
			else
				// Make sure we don't try this one later
				pFilter->pOILFilter->Position = -1;
		}
		
		// Try the next filter
		pFilter = Filter::GetNext(pFilter);
	}

	// Terminate the string
	camStrcat(FilterString, _T("|"));

	// All ok
	return FilterString;
}


/********************************************************************************************

>	virtual INT32 BaseFileDialog::GetSelectedFilterIndex()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/96
	Returns:	The index of the selected filter
	Purpose:	Allows access in a kernelly fashion to the selected filter index on
				the common file dialog box.

********************************************************************************************/

INT32 BaseFileDialog::GetSelectedFilterIndex()
{
	return GetFilterIndex();
}

/********************************************************************************************

>	virtual INT32 BaseFileDialog::SetSelectedFilterIndex(INT32 NewIndex)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/96
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Set the desired index for the filter that is going to be selected on the 
				Common file dialog box.

********************************************************************************************/

INT32 BaseFileDialog::SetSelectedFilterIndex(INT32 NewIndex)
{
	INT32 Old = GetFilterIndex();
	SetFilterIndex( NewIndex );
	TRACEUSER( "Neville", _T("SetSelectedFilterIndex - new %d (old = %d)\n"), NewIndex, Old);
	return Old;
}

/********************************************************************************************

>	virtual BOOL BaseFileDialog::SetTitle(INT32 TitleId)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/96
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Allows the title of the common file dialog box to be set up	

********************************************************************************************/

BOOL BaseFileDialog::SetTitle(INT32 TitleId) 
{
	if (SmartLoadString(0, TitleId, Title, 128))
		SetName( Title );
	
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL BaseFileDialog::SetInitialDirectory(const String_256& pDir)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/96
	Inputs:		pDir	String containing the pathname to use
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Sets the directory which the common file dialog box will open looking at.

********************************************************************************************/

BOOL BaseFileDialog::SetInitialDirectory(const String_256& strDir)
{
	SetDirectory( PCTSTR(strDir) );
	
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL BaseFileDialog::SetDefaultFileName(const String_256& Name)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/96
	Inputs:		Name	name of the file to be used by default
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Sets the name and length of the name of the default file to be used in the 
				common file dialog box

********************************************************************************************/

BOOL BaseFileDialog::SetDefaultFileName(String_256& Name)
{
	SetFilename( PCTSTR(Name) );
		
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL BaseFileDialog::GetChosenFileName(PathName * pName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/96
	Outputs:	The pathname that the user chose.
	Returns:	TRUE if it worked and patname is valid, FALSE if not
	Purpose:	Gets the pathname of the file that was chosen by the user. It will return
				the vailidity of the pathname to the caller. Should be valid as the file
				dialog box should filter out problem pathnames.

********************************************************************************************/

BOOL BaseFileDialog::GetChosenFileName(PathName * pName)
{
	TRACEUSER( "luke", _T("FileName is %s\n"), GetPath().c_str() );

	pName->SetPathName( GetPath() );
	return pName->IsValid();
}

/********************************************************************************************

>	virtual Filter * BaseFileDialog::GetSelectedExportFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/96
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	

********************************************************************************************/

Filter * BaseFileDialog::GetSelectedExportFilter(INT32 TheSelectedFilter)
{
	// Find the filter that the user has chosen
	Filter *pFilter = Filter::GetFirst();
	while (pFilter != NULL)
	{
		if ((pFilter->GetFlags().CanExport) && 
			(pFilter->pOILFilter->Position == TheSelectedFilter))
			// This is the filter!
			break;

		// Try the next filter
		pFilter = Filter::GetNext(pFilter);
	}

	return pFilter;
}

/********************************************************************************************

>	void BaseFileDialog::SetSelectedExportFilter(UINT32 uiFilterID)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/97
	Returns:	-
	Purpose:	Sets the selected export filter to the ID given

********************************************************************************************/

void BaseFileDialog::SetSelectedExportFilter(UINT32 uiFilterID)
{
	//Search through the filters until we find the one we want
	Filter* pFilter = Filter::GetFirst();
	while (pFilter != NULL)
	{
		if (pFilter->FilterID == uiFilterID)
		{
			//We've found it
			//So set this as the selected filter
			SetSelectedFilterIndex( pFilter->pOILFilter->Position );
			return;
		}
		pFilter = Filter::GetNext(pFilter);
	}

	ERROR3("BaseFileDialog::SetSelectedExportFilter - couldn't find the Filter ID");
}


/********************************************************************************************

>	virtual BOOL BaseFileDialog::OpenAndGetFileName()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/96
	Returns:	True if the user pressed ok on the dialog box, False otherwise
	Purpose:	Opens the common file dialog box up, waits for the user to gives us the
				filename that they wish to use and either press ok or cancel. 

********************************************************************************************/

BOOL BaseFileDialog::OpenAndGetFileName()
{
PORTNOTE("other", "Removed ControlHelper usage" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Keep Control Helper system informed
	ControlHelper::InformModalDialogOpened();
#endif
	
	// 'Do' the dialog
	INT32 DlgResult = ShowModal();

PORTNOTE("other", "Removed ControlHelper usage" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Keep Control Helper system informed
	ControlHelper::InformModalDialogClosed();
#endif
	
	return (DlgResult == wxID_OK);
}

/********************************************************************************************

>	BOOL BaseFileDialog::IsValidFilename()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/95
	Returns:	TRUE if the current filename is Acceptable, FALSE if not
	Purpose:	Makes sure that the filename picked is actually OK. If there is another
				file of the same name, it checks to see if the user wants to replace it.
				Moved from SaveFileDialog 9/1/96
********************************************************************************************/

BOOL BaseFileDialog::IsValidFilename()
{
	// Get the pathname
	wxString FullPath = GetFilename();
	AppendExtension(&FullPath);

	// Webster - RanbirR 17/02/97
	// Save the file name with our Filter extension, for use in our Message Box.
#ifdef WEBSTER
	wxString FileName(FullPath);
#endif //webster

	// if doesn't exist then thats fine
	if( wxFile::Exists( FullPath ) )
	{
		//	WEBSTER-ranbirr-27/03/97
		// Remove the extension from the file. Webster - RanbirR 11\02\97.
	#ifdef WEBSTER
		RemoveFileExtension(&FileName);
	#endif //webster
		return TRUE;
	}

	// Added for Webster - Ranbir 11\02\97	
	// Removes the exstion from a file name.
#ifdef WEBSTER
	RemoveFileExtension(&FileName);
#endif //webster

	// Get the filename into a Path object
#ifndef WEBSTER
	String_256 KernelFilename(FullPath);
	PathName FileMessage(KernelFilename);
	KernelFilename =  FileMessage.GetTruncatedPath(50);
#endif //webster
	
	// Webster - RanbirR 17/02/97 
	// Use "FileName", since it contains the Filter extension.
#ifdef WEBSTER
	String_256 KernelFilename(FileName);
	PathName FileMessage(KernelFilename);
	KernelFilename =  FileMessage.GetTruncatedPath(50);
#endif //webster

	const TCHAR* Name;
	Name = (TCHAR*)KernelFilename;

	String_256 strPrompt( _R(IDM_OVERWRITE) );

	// fill in the %s field with the filename
	TCHAR ErrorMsg[256];
	camSnprintf( ErrorMsg, sizeof(ErrorMsg) / sizeof(ErrorMsg[0]), strPrompt, 
		(const TCHAR*)Name );
	Error::SetError( 0, ErrorMsg, 0 );
	SetNextMsgHelpContext(_R(IDM_OVERWRITE));

	// then ask the user
	ErrorInfo Info;
	Info.Button[0] = _R(IDB_OVERWRITE);
	Info.Button[1] = _R(IDS_CANCEL);
	
	// This makes the cancel button the default one in this situation
	// as defaulting to replace is bad.
	Info.OK = Info.Cancel = 2;

	UINT32	idResult = AskQuestion( &Info );

	// if they cancel, then the name is not acceptable to them
	if( _R(IDS_CANCEL) == idResult )
		return FALSE;

	// user was quite keen on the replace file idea.
	// delete the file we're replacing so rampant filters don't get upset
	// Removed 2/1/96 as the export filters should be using the new DeleteExportFile function
	// Problems with this approach as if the exporter pops up an expotr options dialog box then
	// most people would expect a cancel on this not to vape the file.
	//_unlink(FullPath);

	return TRUE;
}

/********************************************************************************************

>	void BaseFileDialog::AppendExtension(wxString* pFilename)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/96
	Inputs:		pFilename - the filename to append an extension to
	Outputs:	pFilename - the input filename with the .art extension on the end (if needed)
	Purpose:	Given a filename, this function should attach the appropriate file extension
				to the to it. This baseclass version does nothing

********************************************************************************************/

void BaseFileDialog::AppendExtension(wxString* pFilename)
{
 	// baseclass version so do nothing
}

/********************************************************************************************

>	void BaseFileDialog::RemoveFileExtension(wxString* pFilename)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/02/97
	Inputs:		pName - The filename from which the extension will be removed.
	Outputs:	pName - The input filename with the extension removed.
	
********************************************************************************************/
//	WEBSTER-ranbirr-27/03/97
#ifdef WEBSTER
BOOL  BaseFileDialog::RemoveFileExtension(wxString* pName)
{
	// Is this a valid file name
	if ((pName == NULL) || (pName->IsEmpty()))
		return FALSE;

	// Does the filename have an extension? 
	INT32 i = pName->GetLength() - 1;
	INT32 ExtensionLen = 0;

	// look for the dot char.
	while ((i >= 0) && (pName->GetAt(i) != '.'))
	{
		i--;
		ExtensionLen++;
	}
	
	// Is it a valid extension?
	if (ExtensionLen <= 3 && ExtensionLen > 0 && i > 0)
	{
		// Remove the extension.
		*pName = pName->Left(i);
		return TRUE;
	}
	else
		return FALSE;
}
#endif //webster


/********************************************************************************************
***		New help button handling code
********************************************************************************************/

/********************************************************************************************

>	afx_msg void BaseFileDialog::OnHelp()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Returns:	-
	Purpose:	Handle the clicks on the help button.
				This is the Windows 95 UI form of the function.

********************************************************************************************/

PORTNOTE("other", "Removed MFC type Hooks" )
#if !defined(EXCLUDE_FROM_XARALX)
afx_msg void BaseFileDialog::OnHelp()
{
	// Run the help engine for this dialog and don't bother processing this message
	// any further, we've done it.
	HelpUser(*this);
	return;
}

/********************************************************************************************
***		Preview bitmap handling code
********************************************************************************************/

/********************************************************************************************

>	afx_msg void BaseFileDialog::OnShowPreviewBitmap()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/96
	Returns:	-
	Purpose:	Handle the clicks on the buttons which offer the show preview bitmap.
				This is the Windows 95 UI form of the function.

********************************************************************************************/

afx_msg void BaseFileDialog::OnShowPreviewBitmap()
{
	if (!CCamApp::IsNewWindowsUI())
		return;

	// Do exactly the same as the file name change code does
	OnFileNameChange();
}

/********************************************************************************************

>	afx_msg void BaseFileDialog::OnOptions()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/97
	Returns:	-
	Purpose:	Implements the options button installed for WEBSTER
				Base class does nowt

********************************************************************************************/

afx_msg void BaseFileDialog::OnOptions()
{
	// WEBSTER - markn 28/1/97
	HandleOptions();
}

/********************************************************************************************

>	virtual BOOL BaseFileDialog::HandleOptions()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/97
	Returns:	-
	Purpose:	Implements the options button installed for WEBSTER
				Base class does nowt

********************************************************************************************/

BOOL BaseFileDialog::HandleOptions()
{
	return TRUE;
}

/********************************************************************************************

>	void BaseFileDialog::OnFileNameChange()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/7/96
	Returns:	-
	Purpose:	Should be called when the user has selected a new file in the file list.
				We might need to do things like remove the preview bitmap from the file
				and display it.
				Only called back if OFN_EXPLORER is set and you can only do this on Windows 95
				otherwise a dialog is not popped up.

	SeeAlso:	BaseFileDialog::OnHookMsg; BaseFileDialog::HookProc;
	SeeAlso:	ExportFileDialog::OnLBSelChangedNotify;

********************************************************************************************/

void BaseFileDialog::OnFileNameChange()
{
#if _MFC_VER >= 0x400
	// Remove code completely if using less than MFC4.0 as calls like SetControlText
	// will not be present and this function will not work
	// Just in case Windows NT 3 decides to call this we'd better stop it as otherwise things
	// will be happening more than they should. This is the method used on Windows 95 or NT 4.
	if (!CCamApp::IsNewWindowsUI())
		return;

	// Graeme (1/12/00) - We need to update ShowPreviewBitmap here. This updates the registry
	// settings, and enables the dialogue to remember the show preview bitmap settings. The
	// single equals sign is intentional, as I want to perform an assignment, rather than an
	// equality operation.
	if ( ShowPreviewBitmap = IsDlgButtonChecked ( _R(IDC_SHOWPREVIEW) ) )
	{
		// Get the filename which the user has entered in the field.
		wxString FileName = GetFileName();

		// Added by Craig Hamilton 20/9/00.
		// This fixes the problem where a user drags a file over folders within the import
		// dialog causing the preview to be redone. If the previously selected file is the
		// same as the current then there is no need to regenerate a new preview.
		wxString pathName = GetPathName();
		if(m_previousPathName != pathName)
		{
			// Reinitialise. This is the only place this happens.
			m_previousPathName = pathName;

			// If its empty then return immediately
			if (FileName.IsEmpty())
				return;

			PathName Path(FileName.GetBuffer(256));
			FileName.ReleaseBuffer();
			if (!Path.IsValid())
			{
				// There was a problem e.g. user has typed f:
				// Magically a bad pathname sets an error so clear it!
				Error::ClearError();
				return;
			}

			BOOL Previous = (pBitmapToUse != NULL);

			BOOL ok = GetPreviewBitmapFromFile(&Path);

			BOOL Now = (pBitmapToUse != NULL);
			
			// If we have changed state or read a new preview bitmap ok then force a redraw
			if ((!Previous && Now) || (Previous && !Now) || ok)
			{
				// Force a redraw of the preview bitmap icon, as we have changed state
				wxWindow* Icon = FindWindow( _R(IDC_PREVIEW));
				::InvalidateRect(Icon, NULL, TRUE);
			}
		}
		// End added.
	}
	else
	{
		// Delete the present bitmap so we show nothing
		if (pBitmapToUse != NULL)
		{
			delete pBitmapToUse;
			pBitmapToUse = NULL;

			// Force a redraw of the preview bitmap icon, as we have changed state
			wxWindow* Icon = FindWindow( _R(IDC_PREVIEW));
			::InvalidateRect(Icon, NULL, TRUE);
		}
	}

#endif
	return;
}

/********************************************************************************************

>	BOOL BaseFileDialog::GetPreviewBitmapFromFile(PathName * pPath)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/96
	Returns:	-
	Purpose:	Rips a preview bitmap from the specified file, if appropriate. The class
				variable pBitmapToUse will contain the bitmap or will contain NULL if no
				bitmap is found.
	SeeAlso:	OpenFileDialog::OnFileNameChange;

********************************************************************************************/

BOOL BaseFileDialog::GetPreviewBitmapFromFile(PathName * pPath)
{
    CCDiskFile		File		( 1024, FALSE, TRUE );
	String_256		Type		( pPath->GetType() );
	KernelBitmap	*pThumbnail	= NULL;
	BOOL			ReadOK		= FALSE;

	// If there is a preview bitmap in use, delete it first.
	if ( pBitmapToUse != NULL )
	{
		delete pBitmapToUse;
		pBitmapToUse = NULL;
	}

	// Get lowercase filetype
	Type.toLower();

	// Bodge to speed galleries up for art files - doesn't check or use external previews
	if ((Type.Sub((String_8)"cdr") != -1) ||
		(Type.Sub((String_8)"cmx") != -1) )
	{
		// Use the RIFF file import code.
		ReadOK = ReadRIFFPreview ( File, pPath );
	}

	// Read in a native file.
	else if ( !Type.Sub ( ( String_8 ) "art" ) ||
			  !Type.Sub ( ( String_8 ) "xar" ) ||
			  !Type.Sub ( ( String_8 ) "cxn" ) ||
			  !Type.Sub ( ( String_8 ) "cxw" ) ||
			  !Type.Sub ( ( String_8 ) "web" ) )
	{
		// Use the native file format import code.
		ReadOK = ReadNativePreview ( File, pPath );
	}

	// Try a PNG.
	else if ( !Type.Sub ( ( String_8 ) "png" ) )
	{
		// Use the PNG import code to load the bitmap up.
		ReadOK = ReadBitmapPreview ( File, pPath, FILTERID_PREVIEW_PNG );
	}

	// Try a BMP.
	else if ( !Type.Sub ( ( String_8 ) "bmp" ) )
	{
		// Use the BMP import code to load the bitmap up.
		ReadOK = ReadBitmapPreview ( File, pPath, FILTERID_PREVIEW_BMP );
	}

	// Try a GIF.
	else if ( !Type.Sub ( ( String_8 ) "gif" ) )
	{
		// Use the GIF import code to load the bitmap up.
		ReadOK = ReadBitmapPreview ( File, pPath, FILTERID_PREVIEW_GIF );
	}

	// Try a JPEG.
	else if ( !Type.Sub ( ( String_8 ) "jpg" ) || !Type.Sub ( ( String_8 ) "jpeg" ) )
	{
		// Use the JPEG import code to load the bitmap up.
		ReadOK = ReadBitmapPreview ( File, pPath, FILTERID_IMPORT_JPEG );
	}

	// Try a TIFF.
	else if ( !Type.Sub ( ( String_8 ) "tif" ) || !Type.Sub ( ( String_8 ) "tiff" ) )
	{
		// Use the JPEG import code to load the bitmap up.
		ReadOK = ReadBitmapPreview ( File, pPath, FILTERID_PREVIEW_TIFF );
	}

	// Clear any errors we encountered
	Error::ClearError();	

	return ReadOK;
}

/********************************************************************************************

>	BOOL BaseFileDialog::OpenFileForPreview ( CCDiskFile	&File,
											  PathName		*pPath )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/00
	Inputs:		&File	- The file containing the preview bitmap.
				pPath	- The pathname of the file.
	Returns:	TRUE	- The file was opened OK.
				FALSE	- An error occured.
	Purpose:	Sets the file up so that the preview image can be read.

********************************************************************************************/

BOOL BaseFileDialog::OpenFileForPreview ( CCDiskFile	&File,
										  PathName		*pPath )
{
	// Don't error, just set exceptions...
	BOOL Found = FALSE;

	// Open file and check if it exists at the same time
	TRY
	{
		Found = File.open(*pPath, ios::in | ios::binary | ios::nocreate);
	}		
	CATCH( CFileException, e)
	{
		Found = FALSE;
		Error::ClearError();
	}
	END_CATCH

	return Found;
}

/********************************************************************************************

>	BOOL BaseFileDialog::ReadRIFFPreview ( CCDiskFile	&File,
										   PathName		*pPath )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/00
	Inputs:		&File		- The file containing the preview bitmap.
				*pPath		- The pathname giving the location of the file.
	Returns:	TRUE		- The preview bitmap was created successfully.
				FALSE		- An error occurred.
	Purpose:	Reads in the RIFF chunk containing the preview.

********************************************************************************************/

BOOL BaseFileDialog::ReadRIFFPreview ( CCDiskFile	&File,
									   PathName		*pPath )
{
	// Set up the return variable.
	BOOL Result = FALSE;
	
	// Attempt to open the file, and read in the preview from the CMX or CDR file.
	if ( OpenFileForPreview ( File, pPath ) &&
		 AccusoftFilters::GetVersionNumber() > 0 &&
		 CMXImportFilter::SkipToPreviewBitmap ( &File ) )
	{
		WinBitmap *pThumbnail = new WinBitmap;

		// Attempt to read in the bitmap.
		if ( !AccusoftFilters::ReadFromFile ( &File, &pThumbnail->BMInfo,
											  &pThumbnail->BMBytes, TRUE, NULL,
											  NULL, TRUE ) )
		{
			// It failed to load, so clean up the bitmap.
			delete pThumbnail;
			pThumbnail = NULL;
		}

		// Create a kernel bitmap.
		else if ( pThumbnail != NULL &&
				  ( pBitmapToUse = new KernelBitmap ( pThumbnail, TRUE ) ) != NULL )
		{
			// It worked.
			Result = TRUE;
		}
	}

	// Return whether it worked or not.
	return Result;
}

/********************************************************************************************

>	BOOL BaseFileDialog::ReadNativePreview ( CCDiskFile	&File,
											 PathName	*pPath )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/00
	Inputs:		&File		- The file containing the preview bitmap.
				*pPath		- The pathname giving the location of the file.
	Returns:	TRUE		- The preview bitmap was created successfully.
				FALSE		- An error occurred.
	Purpose:	Reads in the native file chunk containing the preview.

********************************************************************************************/

BOOL BaseFileDialog::ReadNativePreview ( CCDiskFile	&File,
										 PathName	*pPath )
{
	// Now that we have had the decision to make .cxns become .xars, we must check to see if
	// its a new native file before just doing a straight Accusoft read. This will have the
	// added side effect of ensuring Accusoft do not see the native format files, without
	// our helping hand as to where they can look. Otherwise, they are likely to access
	// violate.
	BOOL	Result		= FALSE;
	BOOL	IsNewFormat	= FALSE;
	UINT32	FilterId	= FILTERID_NONE;

	// Try to open the file up to get the preview image first!
	if ( !OpenFileForPreview ( File, pPath ) )
	{
		return FALSE;
	}

	// In the process it will position the file at the start of the preview bitmap image
	// and get the image type
	// Use the special version of the function to read in bitmap which uses the present
	// file offset as the place that the image is stored.
	if ( BaseCamelotFilter::SkipToPreviewBitmap ( &File, &IsNewFormat, &FilterId ) &&
		 ( FilterId != FILTERID_NONE ) )
	{
		// Load in the bitmap starting from the present point in the file		
		Filter* pFilter = Filter::GetFirst();

		while ((pFilter != NULL) && (pFilter->FilterID != FilterId))
		{
			// Try the next filter
			pFilter = Filter::GetNext(pFilter);
		}

		if ( pFilter != NULL )
		{
			// Ensure that it's a bitmap filter.
			ERROR3IF ( !pFilter->IS_KIND_OF ( BaseBitmapFilter ),
						"Preview filter is not a BaseBitmapFilter\n" );

			// It's necessary to cast the filter to be a base bitmap filter, so
			// that the bitmap loading code is exposed.
			BaseBitmapFilter *pBMPFilter = static_cast<BaseBitmapFilter*> ( pFilter );

			// Attempt to read the preview in from the file.
			if ( pBMPFilter->ImportBitmap ( &File, &pBitmapToUse ) )
			{
				// It worked!
				Result = TRUE;
			}
			else
			{
				// Import failed! Delete the bitmap.
				delete pBitmapToUse;
				pBitmapToUse = NULL;
			}

			// I need to manually shutdown the loading bar.
			EndSlowJob ();
		}
	}

	// Try to use another method.
	else if ( !IsNewFormat && AccusoftFilters::GetVersionNumber() > 0 )
	{
		// Old format had a preview as the first thing in the file so
		// Just do a straight load from the start of the file
		// But only if not a new format file as otherwise Accusoft does its usual lets load
		// something spappy.

		// Create a new bitmap record.
		WinBitmap *pBMP = new WinBitmap;
		
		// Read in and create the new bitmap.
		if ( pBMP != NULL && 
			 AccusoftFilters::ReadFromFile ( &File, &pBMP->BMInfo, &pBMP->BMBytes, TRUE ) &&
			 ( pBitmapToUse = new KernelBitmap ( pBMP, TRUE ) ) != NULL )
		{
			// It worked!
			Result = TRUE;
		}

		// The bitmap wasn't loaded.
		else
		{
			// Import failed! Delete the bitmap.
			delete pBMP;
		}
	}

	// Return a pointer to the bitmap loaded from the file.
	return Result;
}

/********************************************************************************************

>	BOOL BaseFileDialog::ReadBitmapPreview ( CCDiskFile	&File,
											 PathName	*pPath,
											 UINT32		FilterID )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/00
	Inputs:		&File		- The file containing the preview bitmap.
				pPath		- The pathname of the file.
				FilterID	- The filter to use.
	Returns:	TRUE		- The preview bitmap was created successfully.
				FALSE		- An error occurred.
	Purpose:	Creates a preview image from the contents of a bitmap file.

********************************************************************************************/

BOOL BaseFileDialog::ReadBitmapPreview ( CCDiskFile	&File,
										 PathName	*pPath,
										 UINT32		FilterID )
{
	// Set up the default return value.
	BOOL Result = FALSE;

	// Try to open the file.
	if ( OpenFileForPreview ( File, pPath ) )
	{
		// Find the first filter in the filter list.
		Filter* pFilter = Filter::GetFirst ();

		// Scan through the list of filters until the correct one is found.
		while ( ( pFilter != NULL ) && ( pFilter->FilterID != FilterID ) )
		{
			// Try the next filter
			pFilter = Filter::GetNext ( pFilter );
		}

		// If a filter has been found, load up its contents. The IS_KIND_OF is a sanity
		// check to ensure that the SetPreviewBitmap isn't called on (e.g.) a vector
		// filter.
		if ( pFilter != NULL && pFilter->IS_KIND_OF ( BaseBitmapFilter ) )
		{
			// It's necessary to cast the filter to be a base bitmap filter, so
			// that the bitmap loading code is exposed.
			BaseBitmapFilter *pBMPFilter = static_cast<BaseBitmapFilter*> ( pFilter );

			// Tell the filter that it *IS* a preview bitmap.
			pBMPFilter->SetPreviewBitmap ( TRUE );

			// Attempt to read the preview in from the file.
			if ( pBMPFilter->ImportBitmap ( &File, &pBitmapToUse ) )
			{
				// It didn't work.
				Result = TRUE;
			}

			// Something went wrong with the input, so clean up the bitmap.
			else
			{
				// Delete the bitmap.
				delete pBitmapToUse;
				pBitmapToUse = NULL;
			}

			// I need to manually shutdown the loading bar.
			EndSlowJob ();
		}
	}

	// Return to the main loop.
	return Result;
}

/********************************************************************************************

>	afx_msg LRESULT BaseFileDialog::OnRedrawMessage(WPARAM wParam, LPARAM lParam)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/3/95
	Inputs:		wParam - The Windows wParam parameter
				lParam - The Windows lParam parameter
	Returns:	TRUE
	Purpose:	Handles the Redraw control wanting to be drawn. This control is used to
				display the Preview bitmaps from selected files in the file list. When
				we get this message we will need to go and find the bitmap from the
				file and display it. The message is sent up by one of our cc_DialogRedraw
				custom controls.

********************************************************************************************/

afx_msg LRESULT BaseFileDialog::OnRedrawMessage(WPARAM wParam, LPARAM lParam)
{
//	TRACEUSER( "Neville", _T("Draw the Blank version of the control now, as we don't have a filename\n"));

 	// Do nothing if not an explorer type dialog box
	if (!CCamApp::IsNewWindowsUI())
		return TRUE;

	// Find out about the paint message and fill in the details in the kernel message
	RedrawInfo* pInfo = (RedrawInfo*) lParam;
	ReDrawInfoType ExtraInfo;

	ExtraInfo.pMousePos = NULL;		// No mouse position info for redraw events
	
	// Build a CC dc out of it for rendering to the screen
	// Get a MFC CDC to put the DC in
	CCDC MyDc(RENDERTYPE_SCREEN);
	MyDc.Attach(pInfo->PaintInfo.hdc);
	ExtraInfo.pDC = &MyDc;

	// The devices DPI
	ExtraInfo.Dpi = MyDc.GetDeviceCaps(LOGPIXELSY);
	
	// How big the window is
	RECT WindowSize;
	if (::GetClientRect((wxWindow*)wParam, &WindowSize))
	{
		ExtraInfo.dx = (((INT32)WindowSize.right)*72000) / ExtraInfo.Dpi;
		ExtraInfo.dy = (((INT32)WindowSize.bottom)*72000) / ExtraInfo.Dpi;
	}

	// Work out the size of the invalidated region
	RECT* ClipRect = &pInfo->PaintInfo.rcPaint;
	DocRect DocClipRect;

	// Convert to millipoints, Also need to flip the y coords to get a
	// rectangle in with the origin in the bottom left.
	DocClipRect.lo.x = (ClipRect->left * 72000) / ExtraInfo.Dpi;
	DocClipRect.lo.y = ExtraInfo.dy - ((ClipRect->bottom * 72000) / ExtraInfo.Dpi);

	DocClipRect.hi.x = (ClipRect->right * 72000) / ExtraInfo.Dpi;
	DocClipRect.hi.y = ExtraInfo.dy - ((ClipRect->top * 72000) / ExtraInfo.Dpi);

	// Set the pointer in the extra info structure
	ExtraInfo.pClipRect = &DocClipRect;

	// Go get a render region
	DocRect VirtualSize(0, 0, ExtraInfo.dx, ExtraInfo.dy);
	RenderRegion* pRender = CreateOSRenderRegion(&VirtualSize, &ExtraInfo);
	if (pRender!=NULL)
	{
		// Code stolen from ColourEditDlg::RenderControl
		DialogColourInfo RedrawColours;		// Get a supplier for default dlg colours
		INT32 PixelSize = 72000 / ExtraInfo.Dpi;		// Size of output pixel in millipoints

		// Render the attributes and then a rectangle
		pRender->SaveContext();

		// pBitmapToUse stores the preview bitmap of the current file
		if (pBitmapToUse)
		{
			// bring the left hand edge in one pixel.
			VirtualSize.lox += PixelSize;

			// Using this method we don't get flicker on the backdrop as we draw the background rectangle
			// followed by the white new background
			// Draw single pixel black line left and top and single pixel background colour 1 pixel in
			pRender->SetLineWidth(0);
			pRender->SetFillColour(RedrawColours.DialogBack());
			pRender->SetLineColour(COLOUR_BLACK); //RedrawColours.ButtonShadow());
			pRender->DrawLine(DocCoord(VirtualSize.lo.x, VirtualSize.hi.y), VirtualSize.hi);	// Top Left to Top Right
			pRender->DrawLine(DocCoord(VirtualSize.lo.x, VirtualSize.hi.y), VirtualSize.lo);	// Top Left to Bottom Left
			pRender->SetLineColour(RedrawColours.DialogBack());
			pRender->DrawLine(DocCoord(VirtualSize.lo.x + PixelSize, VirtualSize.hi.y - PixelSize),
							  DocCoord(VirtualSize.hi.x, VirtualSize.hi.y - PixelSize));		// Top Left to Top Right
			pRender->DrawLine(DocCoord(VirtualSize.lo.x + PixelSize, VirtualSize.lo.y),
							  DocCoord(VirtualSize.lo.x + PixelSize, VirtualSize.hi.y - PixelSize)); // Bottom Left to Top Left

			// Draw single pixel highlight line right and bottom and single pixel background colour 1 pixel in
			pRender->SetLineColour(RedrawColours.ButtonHighlight());
			pRender->DrawLine(DocCoord(VirtualSize.hi.x, VirtualSize.hi.y - PixelSize),
							  DocCoord(VirtualSize.hi.x, VirtualSize.lo.y));
			pRender->DrawLine(DocCoord(VirtualSize.hi.x - PixelSize, VirtualSize.lo.y + PixelSize),
							  DocCoord(VirtualSize.lo.x + PixelSize, VirtualSize.lo.y + PixelSize));

			pRender->SetLineColour(RedrawColours.DialogBack());
			pRender->DrawLine(DocCoord(VirtualSize.hi.x - PixelSize, VirtualSize.hi.y - PixelSize),
							  DocCoord(VirtualSize.hi.x - PixelSize, VirtualSize.lo.y + 2 * PixelSize));
			pRender->DrawLine(DocCoord(VirtualSize.hi.x - PixelSize, VirtualSize.lo.y + 2 * PixelSize),
							  DocCoord(VirtualSize.lo.x + PixelSize, VirtualSize.lo.y + 2 * PixelSize));
			
			// put the left hand edge back again
			VirtualSize.lox -= PixelSize;

			// And deflate the rect by 2 pixels
			VirtualSize.Inflate(-PixelSize * 2);

			// Code stolen from BfxDlg::RenderBitmap
			pRender->SetLineColour(COLOUR_TRANS);

			// Draw a rectangle to fill in the background - Fill with Dialogue Background colour
			pRender->SetFillColour(COLOUR_WHITE);
			pRender->DrawRect(&VirtualSize);

			NodeBitmap NB;
			NodeBitmap * pNodeBitmap = &NB;
		
			if (!((pNodeBitmap == NULL) || (!pNodeBitmap->SetUpPath(12,12))))
			{
				// Get a new bitmap object for this node.
				pNodeBitmap->GetBitmapRef()->SetBitmap(pBitmapToUse);
						
				ENSURE(pNodeBitmap->GetBitmap()->ActualBitmap != NULL, "No bitmap object found!");
		
				DocRect BitmapSize(VirtualSize);
				BitmapInfo BMInfo;
				pBitmapToUse->ActualBitmap->GetInfo(&BMInfo);
				if (BMInfo.PixelWidth==0) BMInfo.PixelWidth=1;
				if (BMInfo.PixelHeight==0) BMInfo.PixelHeight=1;

				double BMAspect = ((double)(BMInfo.PixelWidth))/((double)(BMInfo.PixelHeight));
				double RedrawAspect = ((double)(ExtraInfo.dx))/((double)(ExtraInfo.dy));

				if (BMAspect>RedrawAspect)
				{
					// background colour at top and bottom
					INT32 NewHeight = (INT32)( 0.5 + ((double)(VirtualSize.Width()))/BMAspect);
					if (NewHeight<1) NewHeight = 1;
					BitmapSize.loy = (VirtualSize.hiy + VirtualSize.loy - NewHeight) / 2;
					BitmapSize.hiy = BitmapSize.loy + NewHeight;
				}
				else
				{
				 	// background colour on left and right
					INT32 NewWidth = (INT32)( 0.5 + ((double)(VirtualSize.Height()))*BMAspect);
					if (NewWidth<1) NewWidth = 1;
					BitmapSize.lox = (VirtualSize.hix + VirtualSize.lox - NewWidth) / 2;
					BitmapSize.hix = BitmapSize.lox + NewWidth;
				}

				// And set this in our bitmap node
				pNodeBitmap->CreateShape(BitmapSize);

				QualityAttribute* pQualAttr = new QualityAttribute;
				if (pQualAttr != NULL)
				{
					// If all that worked then turn on anti-aliasing when drawing the bitmap in preview.
					pQualAttr->QualityValue = Quality::QualityMax;
					pRender->SetQuality(pQualAttr, TRUE);
				}

				pNodeBitmap->Render(pRender);
			}
		}
		else
		{
			// make the right hand side out of the box so that the right black line does not show
			VirtualSize.hix += PixelSize;

			// Draw a backgound making sure we blat anything previously there wiht the dialog
			// background colour
			pRender->SetLineWidth(0);
			pRender->SetFillColour(RedrawColours.DialogBack());
			pRender->SetLineColour(COLOUR_BLACK); //RedrawColours.ButtonShadow());
			pRender->DrawRect(&VirtualSize);
			pRender->DrawLine(DocCoord(VirtualSize.lo.x, VirtualSize.hi.y), VirtualSize.hi);

			pRender->SetLineWidth(0);
			pRender->SetLineColour(RedrawColours.ButtonHighlight());
			pRender->DrawLine(DocCoord(VirtualSize.hi.x - PixelSize, VirtualSize.hi.y - PixelSize), DocCoord(VirtualSize.hi.x - PixelSize, VirtualSize.lo.y));
			pRender->DrawLine(DocCoord(VirtualSize.hi.x - PixelSize, VirtualSize.lo.y + PixelSize), DocCoord(VirtualSize.lo.x + PixelSize, VirtualSize.lo.y + PixelSize)); // Bottom Right to Bottom Left

			// bring the right hand side back in
			VirtualSize.hix -= PixelSize;
			// bring the bottom up
			VirtualSize.loy += PixelSize;

			// Draw the no bitmap present cross i.e. vertical
			// And Deflate the rect by 1 pixels so we draw to the outer lines
			VirtualSize.Inflate(-PixelSize);
			INT32 Width = VirtualSize.Width();
			INT32 Height = VirtualSize.Height();
			pRender->SetLineWidth(0);
			pRender->SetLineColour(COLOUR_BLACK);
			pRender->DrawLine(DocCoord(VirtualSize.lo.x + Width/2, VirtualSize.lo.y),
							  DocCoord(VirtualSize.lo.x + Width/2, VirtualSize.hi.y));
			pRender->DrawLine(DocCoord(VirtualSize.lo.x, VirtualSize.lo.y + Height/2),
							  DocCoord(VirtualSize.hi.x, VirtualSize.lo.y + Height/2));
		}

		pRender->RestoreContext();

		// Get rid of the render region
		DestroyOSRenderRegion(pRender);
	}

	// Give back the dc
	MyDc.Detach();

	return TRUE;
}
/********************************************************************************************

>	BOOL BaseFileDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)

	Author:		Alex
	Created:	06/11/2003
	Inputs:		wParam - The Windows wParam parameter
				lParam - The Windows lParam parameter
	Returns:	TRUE
	Purpose:	Used to copy the OPENFILNAMEEX values into the
				standard OPENFILENAME member variable. This is
				to support XP-style common file dialogs - the
				places bar on the left hand side of the dialog.

********************************************************************************************/

#ifdef XPDialog
BOOL BaseFileDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	memcpy(&m_ofn, &m_ofnEx, sizeof(m_ofn));
	m_ofn.lStructSize = sizeof(m_ofn);

	return CFileDialog::OnNotify(wParam, lParam, pResult);
}
#endif

/********************************************************************************************

>	INT32 BaseFileDialog::DoModal()

	Author:		Alex
	Created:	06/11/2003
	Inputs:		
	Returns:	
	Purpose:	Overrides the CFileDialog::DoModal() method
				so that XP-style common file dialogs can be
				supported.
				Used to copy the OPENFILNAMEEX values into the
				standard OPENFILENAME member variable. This is
				to support XP-style common file dialogs - the
				places bar on the left hand side of the dialog.

********************************************************************************************/

#ifdef XPDialog
INT32 BaseFileDialog::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_ofn.Flags & OFN_ENABLEHOOK);
	ASSERT(m_ofn.lpfnHook != NULL); // can still be a user hook

	// XP-style dialogs:
//	m_ofn.Flags &= ~OFN_ENABLEHOOK;
	
	// zero out the file buffer for consistent parsing later
	ASSERT(AfxIsValidAddress(m_ofn.lpstrFile, m_ofn.nMaxFile));
	DWORD nOffset = camStrlen(m_ofn.lpstrFile)+1;
	ASSERT(nOffset <= m_ofn.nMaxFile);
	memset(m_ofn.lpstrFile+nOffset, 0, (m_ofn.nMaxFile-nOffset)*sizeof(TCHAR));

	// WINBUG: This is a special case for the file open/save dialog,
	//  which sometimes pumps while it is coming up but before it has
	//  disabled the main window.
	wxWindow* hWndFocus = ::GetFocus();
	BOOL bEnableParent = FALSE;
	m_ofn.hwndOwner = PreModal();
	AfxUnhookWindowCreate();
	if (m_ofn.hwndOwner != NULL && ::IsWindowEnabled(m_ofn.hwndOwner))
	{
		bEnableParent = TRUE;
		::EnableWindow(m_ofn.hwndOwner, FALSE);
	}

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	ASSERT(pThreadState->m_pAlternateWndInit == NULL);

	if (m_ofn.Flags & OFN_EXPLORER)
		pThreadState->m_pAlternateWndInit = this;
	else
		AfxHookWindowCreate(this);

	// TO ALLOW XP-style:
//#ifdef XPDialog
	memset(&m_ofnEx, 0, sizeof(m_ofnEx));
	memcpy(&m_ofnEx, &m_ofn, sizeof(m_ofn));
	DWORD dwWinMajor = (DWORD)(LOBYTE(LOWORD(::GetVersion())));
	if ((BYTE)dwWinMajor >= 5)
	   m_ofnEx.lStructSize = sizeof(m_ofnEx);
	// ENDS

	INT32 nResult;
	if (m_bOpenFileDialog)
		nResult = ::GetOpenFileName((OPENFILENAME*)&m_ofnEx);	// changed to ofnEx
	else
		nResult = ::GetSaveFileName((OPENFILENAME*)&m_ofnEx);	// changed to ofnEx

	// MORE
	memcpy(&m_ofn, &m_ofnEx, sizeof(m_ofn));
	m_ofn.lStructSize = sizeof(m_ofn);
	// ENDS
/*#else
	INT32 nResult;
	if (m_bOpenFileDialog)
		nResult = ::GetOpenFileName(&m_ofn);
	else
		nResult = ::GetSaveFileName(&m_ofn);
#endif*/

	if (nResult)
		ASSERT(pThreadState->m_pAlternateWndInit == NULL);
	pThreadState->m_pAlternateWndInit = NULL;

	// WINBUG: Second part of special case for file open/save dialog.
	if (bEnableParent)
//#ifdef XPDialog
		::EnableWindow(m_ofnEx.hwndOwner, TRUE);	// changed to ofnEx
/*#else
		::EnableWindow(m_ofn.hwndOwner, TRUE);
#endif*/
	if (::IsWindow(hWndFocus))
		::SetFocus(hWndFocus);

	PostModal();
	return nResult ? nResult : IDCANCEL;
}
#endif


/****************************************************************************

>	void BaseFileDialog::OnSize(UINT32 nType, INT32 cx, INT32 cy)

	Author:		Alex
	Created:	12/02/2004

	Inputs:		nType		- 
				cx			- 
				cy			- 
	Purpose:	

****************************************************************************/

void BaseFileDialog::OnSize(UINT32 nType, INT32 cx, INT32 cy)
{
	wxFileDialog::OnSize(nType, cx, cy);
	
	// move the darn Help button!!
	CWnd *wndPreview = GetDlgItem(_R(IDC_PREVIEW));
	CWnd *wndHelpButton = GetDlgItem(_R(ID_CC_HELP_BUTTON));
	CWnd *wndDlg = GetParent();
	CWnd *wndCancelButton = wndDlg->GetDlgItem(IDCANCEL);
	if(wndPreview && wndHelpButton)
	{
		wxRect rPreview, rCancel, rHelp;
		wndCancelButton->GetWindowRect(&rCancel);
		ScreenToClient(&rCancel);
		wndPreview->GetWindowRect(&rPreview);
		ScreenToClient(&rPreview);
		wndHelpButton->GetWindowRect(&rHelp);
		ScreenToClient(&rHelp);

		wndHelpButton->SetWindowPos(NULL, rPreview.left, rCancel.top, 0, 0, SWP_NOSIZE);	// SWP_NOSIZE to keep the current button size
	}

}


/****************************************************************************

>	void BaseFileDialog::OnPaletteChanged(wxWindow* pWindow)

	Author:		Alex
	Creation:	03/03/2004
	Purpose:	update colours in file dialogs when the palette changes

****************************************************************************/

void BaseFileDialog::OnPaletteChanged(wxWindow* pFocusWnd)
{
	if (pFocusWnd != this)
	{
		// Select in our palette, in the background
		CPalette* pMainPal = PaletteManager::GetPalette();
		HPALETTE hMainPal = HPALETTE(pMainPal);

		HDC hDC = ::GetDC(m_hWnd);
		HPALETTE OldPal = ::SelectPalette(hDC, hMainPal, TRUE);
		UINT32 NumColoursChanged = ::RealizePalette(hDC);
		
		// If colours have changed, redraw the entire window
//		if (NumColoursChanged > 0)
			// For some reason, NumColoursChanged was always 0 in my testing.
			// I don't think it's a problem to redraw whenever this method
			// is called. But just do it when in 256 colour mode
			INT32 ScreenDepth = GetDeviceCaps(hDC, BITSPIXEL);
			if(ScreenDepth <= 8)
				InvalidateRect(0);

		// And select the previous DC palette into the DC
		::SelectPalette(hDC, OldPal, TRUE);
		::RealizePalette(hDC);
		::ReleaseDC(m_hWnd, hDC);
	}
}

/****************************************************************************

>	void BaseFileDialog::OnFolderChange()

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Creation:	25/01/2005
	Purpose:	Read preference and apply thumbnail mode if required.

****************************************************************************/
void BaseFileDialog::OnFolderChange()
{
	if(ThumbNailMode)
	{
		wxWindow* view = FindWindowEx(GetParent()->m_hWnd, 0, "SHELLDLL_DefView", 0);
		::SendMessage(view, WM_COMMAND, FCIDM_SHVIEW_THUMBNAIL, 0);
	}
}
#endif

/********************************************************************************************

>	RenderRegion* CreateOSRenderRegion(DocRect* pRequiredSize,
									   ReDrawInfoType* ExtraInfo,
									   BOOL UseSelViewColContext = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Copied from Rik/Jason code in DialogOp::CreateGRenderRegion)
	Created:	17/7/96
	
	Inputs:		pRequiredSize - The size that you want the effective area of the window to
				be. Measured in Millipoints. eg. If you want your control to appear to be
				1 inch square (whatever the actual size of it is), pass in a DocRect that
				looks a bit like this ... DocRect(0,0,72000,72000)
				ExtraInfo - The info about the region, as supplied by the dialog manager
	Returns:	Render Region. This returns a pointer to a render region, or NULL if it
				failed to create one.

	Purpose:	Simplifies the creation of a render region as used in a dialog box. It
				will create a render region, scaled to the correct size etc and deal with
				all the Creation, initialisation etc of the region. StartRender is also
				called, so if this function does not return NULL you are able to start
				drawing into it. You MUST call DestroyOSRenderRegion when you have draw
				everything that you need to as this calls StopRender (which does the blit
				to the screen) as well as deleting the render region for you

	Scope:		Global

	Notes:		Use GRenderRegions when you want fast redraw, grad fills, and transparency.
				If only simple rendering is required, think about perhaps using an
				OSRenderRegion instead.

	SeeAlso:	DialogOp::DestroyOSRenderRegion
	SeeAlso:	DialogOp::CreateGRenderRegion

********************************************************************************************/

RenderRegion* CreateOSRenderRegion(DocRect* pRequiredSize, ReDrawInfoType* ExtraInfo,
								   BOOL UseSelViewColContext)
{
	// Make a new dialog view
	DialogView *pDialogView = new DialogView;
	if (pDialogView == NULL)
		// Error - return failure.
		return NULL;

	// Initialise the DialogView
	if (!pDialogView->Init())
	{
		// Error - return failure.
		delete pDialogView;
		return NULL;
	}

	// Pixelise the rectangle
	pRequiredSize->lo.Pixelise(pDialogView);
	pRequiredSize->hi.Pixelise(pDialogView);
	DocRect ClipRect = *pRequiredSize;

	// Get some default params for the render region
	FIXED16 Scale(1);

	// Ok, go and scale things
	INT32 ReqDx = ClipRect.Width();
	INT32 ReqDy = ClipRect.Height();

	// Work out the scale factors
	FIXED16 XScale = FIXED16(double(ExtraInfo->dx) / double(ReqDx));
	FIXED16 YScale = FIXED16(double(ExtraInfo->dy) / double(ReqDy));

	// Build the matricies
	// One to shift everything to the correct side of the X axis
	Matrix Translate(0, -ExtraInfo->dy);

	// One to scale everything into the window
	Matrix ScaleIt(XScale, YScale);

	// One to translate everything to the origin
	Matrix ToOrigin(-ClipRect.lo.x, -ClipRect.lo.y);

	// Combine them all
	ToOrigin *= ScaleIt;
	ToOrigin *= Translate;

	// If the caller wants to use the same colour separation options (ColourPlate) as the
	// currently selected view, then copy the ColourPlate across to the new DialogView
	if (UseSelViewColContext && DocView::GetSelected() != NULL)
		pDialogView->SetColourPlate(DocView::GetSelected()->GetColourPlate());

	// Make a render region for the screen, asking specifically for an OSRenderRegion,
	// and none of this sneaky & cunning diverting it back into being a GRenderRegion!
//	RenderRegion* pRender = OSRenderRegion::Create(ClipRect, ToOrigin, Scale,
//													RENDERTYPE_SCREEN, NULL, TRUE);
	RenderRegion* pRender = OSRenderRegion::Create(ClipRect, ToOrigin, Scale,
													RENDERTYPE_SCREEN, NULL, FALSE, TRUE);
	// Penultimate param FALSE so that a GDRAW render region is used...
	// Last param TRUE so that the render bitmap is forced into 32BPP mode
	// (this fixes problems with palettes when drawing the preview in 256 colour mode)

	if (pRender != NULL)
	{
		// Try and create the bitmap etc
		if (pRender->AttachDevice(pDialogView, ExtraInfo->pDC, NULL))
		{
			// Try and start the render region
			if (pRender->StartRender())
			{
				return pRender;
			}
		}

		// Failed to attach and start the render region so free up the region
		delete pRender;
	}

	// Something went wrong, fail
	delete pDialogView;
	return NULL;
}


/********************************************************************************************

>	BOOL DestroyGRenderRegion(RenderRegion* pRender)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Copied from Rik/Jason code in DialogOp::CreateGRenderRegion)
	Created:	17/7/96

	Inputs:		A pointer to a render region return by CreateOSRenderRegion
	Returns:	TRUE if it worked, FALSE if the render region passed in was bad

	Purpose:	Deinitialises the render region and deletes it.

	Scope:		Global

	Errors:		ERROR2 error if pRender is NULL

	SeeAlso:	DialogOp::CreateGRenderRegion
	SeeAlso:	DialogOp::CreateOSRenderRegion

********************************************************************************************/

BOOL DestroyOSRenderRegion(RenderRegion* pRender)
{
	// Test for preconditions
	ERROR2IF(pRender==NULL, FALSE, "DestroyOSRenderRegion was passed a NULL render region");

	// Blit to the screen
	pRender->StopRender();

	// and delete the render region and its (dialog) view
	// these are deleted in this order because the RenderRegion destructor may
	// call functions on its RenderView
	View* pView = pRender->GetRenderView();
	delete pRender;
	delete pView;

	// It worked
	return TRUE;
}


/********************************************************************************************
**** 	End of BaseFileDialog class														*****
********************************************************************************************/



/********************************************************************************************

>	OpenFileDialog::OpenFileDialog(LPCTSTR FilterString)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Purpose:	Constructs a Open File Dialog. It sets the File Must Exist and Hide Read Only
				Flags. Call PrepareDialog() then DoModal() to use.

********************************************************************************************/

OpenFileDialog::OpenFileDialog(LPCTSTR FilterString)
  : BaseFileDialog( TRUE, wxFILE_MUST_EXIST | wxHIDE_READONLY, FilterString )
{
	// Set some defaults
	NativeFilterPos = 0;

PORTNOTE("other", "Deactivate template usage" )
#if !defined(EXCLUDE_FROM_XARALX)
	if (CCamApp::IsNewWindowsUI())
	{
		// Use our template to do the ammendment and addition of controls to the dialog box
		m_ofn.lpTemplateName = MAKEINTRESOURCE(_R(IDD_PREVIEWBITMAP));
	}
	else
		m_ofn.lpTemplateName = NULL;
#endif
	
	// Set the preview bitmap to be NULL
	pBitmapToUse = NULL;
}

/********************************************************************************************

>	BOOL OpenFileDialog::OnInitDialog()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/95
	Returns:	TRUE => normal window focus is required.
	Purpose:	Changes a few bits and bobs about in the Save Dialog
	SeeAlso:	SaveFileDialog

********************************************************************************************/

void OpenFileDialog::OnInitDialog( wxInitDialogEvent& event )
{
	// Set up the string we require
	String_256 NewText(_R(IDS_OPENBUTTON));
	const TCHAR* NewTextStr = NewText;

	// Get the handle of the ok button.
	wxWindow* OkButton = FindWindow( wxID_OK );

	// Now replace the 'ok' by 'Save' and return
	OkButton->SetLabel( NewTextStr );

PORTNOTE("other", "Remove code to make filter combo-deeper" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Get the handle of the filter list box.
	wxWindow* ComboBox = FindWindow( cmb1 );

	// Find out where the list box is.
	WINDOWPLACEMENT Placement;
	Placement.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(ComboBox, &Placement);

	// Make the list box deeper than usual
	::MoveWindow(ComboBox,
				 Placement.rcNormalPosition.left,
				 Placement.rcNormalPosition.top,
				 Placement.rcNormalPosition.right - Placement.rcNormalPosition.left,
			     (Placement.rcNormalPosition.bottom - Placement.rcNormalPosition.top) * 5,
			     TRUE);
#endif
 	
 	// Let the base class do its thang . . .
 	return BaseFileDialog::OnInitDialog( event );
}

/********************************************************************************************

>	virtual BOOL OpenFileDialog::ExplorerInitDialog(wxWindow* hDlg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/7/96
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Should be called when the explorer style dialog boxes need setting up.
				We can take this opportunity to add in any extra controls that we require.
	SeeAlso:	BaseFileDialog::ExplorerInitDialog; BaseFileDialog::OnHookMsg; BaseFileDialog::HookProc;

********************************************************************************************/

BOOL OpenFileDialog::ExplorerInitDialog(wxWindow* hDlg)
{
PORTNOTE("other", "Remove MFC code" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Do nothing of not on explorer type dialog boxes
	if (!CCamApp::IsNewWindowsUI())
		return TRUE;


	// Set up and initialise the new buttons and controls

	// Give it the correct state according to the preference.
	wxCheckBox*	pCheck = (wxCheckBox*)FindWindow( _R(IDC_SHOWPREVIEW));
	pCheck->SetValue( BaseFileDialog::ShowPreviewBitmap != FALSE );

	wxWindow*	Icon = FindWindow( _R(IDC_PREVIEW));
	Icon->EnableWindow(TRUE);

	// WEBSTER - markn 28/1/97
	Icon = FindWindow( _R(IDC_FILEDLG_OPTIONS));
	Icon->Show( false );

	//Graham 27/10/97: And the "use as default" button
	Icon = FindWindow( _R(IDC_USEASDEFAULT));
	Icon->ShowWindow( false );
	
	// Move the Preview box down a little, so that it lines up with the top of the List box
	wxWindow*	wndDlg = GetParent();
	wxWindow*	pWndPreview = FindWindow( _R(IDC_PREVIEW) );
	wxRect		rPreview, rList;
	pWndPreview->GetWindowRect(&rPreview);
	ScreenToClient(rPreview);
	wxWindow*	pWndList = wndDlg->GetDlgItem(lst1);	// lst1 is the ID for the main list box
	pWndList->GetWindowRect(&rList);
	ScreenToClient(rList);
	pWndPreview->SetWindowPos(NULL, rPreview.GetLeft(), rList.GetTop(), 
		rPreview.GetWidth(), rPreview.GetHeight(), 0);

	ExplorerInited = TRUE;
#endif

	return TRUE;
}


/********************************************************************************************

>	BOOL OpenFileDialog::PrepareDialog()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Does anything the dialogs want doing after they have been created, but before
				they are displayed

********************************************************************************************/

BOOL OpenFileDialog::PrepareDialog()
{
	// Set the dialogs Title
	if (SmartLoadString(0, _R(IDS_OPENTITLE), Title, 128))
		wxFileDialog::SetTitle( Title );

	// Select the desired path
	if (DefaultOpenFilePath.Length() <= 0)
		DefaultOpenFilePath = wxStandardPaths::Get().GetUserConfigDir();

	SetDirectory( DefaultOpenFilePath );

	// Choose the initial filter
	SetFilterIndex( SelectedFilter );

	// See if the base class wants to do anything
	return BaseFileDialog::PrepareDialog();
}

/********************************************************************************************

>	void OpenFileDialog::SetDefaultPath(const String_256& NewPath)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		NewPath - The path to use as the default
	Purpose:	Sets the default path. The next open dialog will default to this path

********************************************************************************************/

void OpenFileDialog::SetDefaultPath(const String_256& NewPath)
{
	// Set the default path
	DefaultOpenFilePath = NewPath;
}

/********************************************************************************************

>	virtual void OpenFileDialog::DoDataExchange(CDataExchange* pDX)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/96
	Inputs:		(a standard MFC object)
	Purpose:	Maintains the state of the "Show preview bitmap" check-box in the open file dialog.

********************************************************************************************/

PORTNOTE("other", "Removed MFC junk" )
#if !defined(EXCLUDE_FROM_XARALX)
void OpenFileDialog::DoDataExchange(CDataExchange* pDX)
{
    BaseFileDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMyDialog)
	if (CCamApp::IsNewWindowsUI())
	{
		DDX_Check(pDX, _R(IDC_SHOWPREVIEW), BaseFileDialog::ShowPreviewBitmap);
	}
    //}}AFX_DATA_MAP
}
#endif


/********************************************************************************************

>	char* OpenFileDialog::BuildFilterString(INT32* NativeFilterPos)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Outputs:	NativeFilterPos - The position in the list of the native filter
	Returns:	NULL if there was not enough memory, or a pointer to a CCMalloced bit
				of memory with a string in it in the form that dialogs like
	Purpose:	Builds the string that is passed to the dialog that lists all the file
				types.

********************************************************************************************/

TCHAR* OpenFileDialog::BuildFilterString(INT32* NativeFilterPos)
{
	// Find out how much space is needed
	UINT32 FilterSize = 0;
	*NativeFilterPos = 0;

	// Used to load in the resource strings for the filter extension descriptions.
	String_256 FilterExtensions;

	Filter *pFilter = Filter::GetFirst();
	while (pFilter != NULL)
	{
		FilterFlags Flags = pFilter->GetFlags();

		// little bit of a bodge here to stop the CamelotEPSFilter from appearing in the
		// import list - you can't hide it from just one list. It must be availiable though
		// to give it's nice little error message. Other instances of this below.
PORTNOTE("other", "Removed CamelotEPSFilter check" )
#if !defined(EXCLUDE_FROM_XARALX)
		if (Flags.CanImport && Flags.ShowFilter && !IS_A(pFilter, CamelotEPSFilter))
#else
		if (Flags.CanImport && Flags.ShowFilter )
#endif
		{
			// Add the space required to put this into the filter string
			// (add 1 for the separating '|' character)
			FilterSize += camStrlen(pFilter->pOILFilter->ConstructFilterString()) + 1;
		}

		// Try the next filter
		pFilter = Filter::GetNext(pFilter);
	}

	// Add two for the final "|" and the 0 terminator.
	FilterSize += 2;

	// Try to get this string
	TCHAR*	FilterString = (TCHAR*)CCMalloc(FilterSize);
	if (FilterString==NULL)
		// Error state already set by CCMalloc
		return NULL;

	// Construct the string, and set up the position numbers for the filters.
	UINT32 Position = 0;
	FilterString[0] = 0;
	pFilter = Filter::GetFirst();

	while (pFilter != NULL)
	{
		FilterFlags Flags = pFilter->GetFlags();

PORTNOTE("other", "Removed CamelotEPSFilter check" )
#if !defined(EXCLUDE_FROM_XARALX)
		if (Flags.CanImport && Flags.ShowFilter && !IS_A(pFilter, CamelotEPSFilter))
#else
		if (Flags.CanImport && Flags.ShowFilter )
#endif
		{
			// Add this filter into the list
			camStrcat(FilterString, pFilter->pOILFilter->ConstructFilterString());
			camStrcat(FilterString, _T("|"));
			pFilter->pOILFilter->Position = Position;

			// see if it is the native filter
PORTNOTE("other", "Removed CamelotEPSFilter check" )
#if !defined(EXCLUDE_FROM_XARALX)
			if (IS_A(pFilter, CamelotNativeEPSFilter))
#else
			if( false )
#endif
			{
				*NativeFilterPos = Position;
			}

			// increment
			Position++;
		}
		else
			// Make sure we don't try this one later
			pFilter->pOILFilter->Position = -1;

		// Try the next filter
		pFilter = Filter::GetNext(pFilter);
	}

	// Terminate the string
	camStrcat(FilterString, _T("|"));

	// All ok
	return FilterString;
}



/********************************************************************************************

>	SaveFileDialog::SaveFileDialog(LPCTSTR FilterString, String_256 DefaultPath,
					String_256 DefaultName)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		FilterString - The string of available filters
				DefPath - The Default Path for the file
				DefName - The Default FileName for the file
	Purpose:	Constructs a Save File Dialog. It sets the File Must Exist and Hide Read Only
				Flags. Call PrepareDialog() then DoModal() to use.

********************************************************************************************/

SaveFileDialog::SaveFileDialog(LPCTSTR FilterString, const String_256& DefPath, const String_256& DefName)
  : BaseFileDialog( FALSE, wxFILE_MUST_EXIST | wxHIDE_READONLY, FilterString )
//  : BaseFileDialog(FALSE, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FilterString)
{
	// Make a note of this lot
	DefaultName = DefName;
	DefaultPath = DefPath;
	DocName = DefName;

PORTNOTE("other", "Removed OFN_OVERWRITEPROMPT - not sure if needed" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Under Windows 95 the OnFileNameOK system does not work, so use the default one
	// Disable for VC4.0/MFC 4.0 as they have fixed it.
#if _MFC_VER < 0x400
	if (IsWin32c())
		m_ofn.Flags |= OFN_OVERWRITEPROMPT;
#endif
#endif
}


/********************************************************************************************

>	SaveFileDialog::SaveFileDialog(LPCTSTR FilterString, String_256& DefPath,
									String_256& DefName, String_256& DocumentName)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		FilterString - The string of available filters
				DefPath - The Default Path for the file
				DefName - The Default FileName for the file
				DocumentName - The name of the document to put in the dialog title
	Purpose:	Constructs a Save File Dialog. It sets the File Must Exist and Hide Read Only
				Flags. Call PrepareDialog() then DoModal() to use.

********************************************************************************************/

SaveFileDialog::SaveFileDialog(LPCTSTR FilterString, const String_256& DefPath, const String_256& DefName,
	const String_256& DocumentName)
PORTNOTE("other", "Disabled template stuff" )
//	: BaseFileDialog( FALSE, ((CCamApp::IsNewWindowsUI()) ? OFN_ENABLETEMPLATE : 0) | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FilterString)
	: BaseFileDialog( FALSE, wxFILE_MUST_EXIST | wxHIDE_READONLY, FilterString )
{
	// Make a note of this lot
	DefaultName = DefName;
	DefaultPath = DefPath;
	DocName = DocumentName;

PORTNOTE("other", "Disabled template stuff" )
#if !defined(EXCLUDE_FROM_XARALX)
	if (CCamApp::IsNewWindowsUI())
	{
		// Use our template to do the ammendment and addition of controls to the dialog box
		m_ofn.lpTemplateName = MAKEINTRESOURCE(_R(IDD_PREVIEWBITMAP));
	}
	else
		m_ofn.lpTemplateName = NULL;
#endif
	
	// Set the preview bitmap to be NULL
	pBitmapToUse = NULL;
}


/********************************************************************************************

>	BOOL SaveFileDialog::OnInitDialog()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/95
	Returns:	TRUE => normal window focus is required.
	Purpose:	Changes a few bits and bobs about in the Save Dialog
	SeeAlso:	SaveFileDialog

********************************************************************************************/

BOOL SaveFileDialog::OnInitDialog()
{
	// Set up the string we require
	String_256		NewText(_R(IDS_SAVEBUTTON));
	const TCHAR*	NewTextStr = NewText;

	// Get the handle of the ok button.
	wxButton*		OkButton = (wxButton*)FindWindow( wxID_OK );

	// Now replace the 'ok' by 'Save' and return
	OkButton->SetLabel( NewTextStr );

PORTNOTE( "other", "Remove code to add compression check" )
#if !defined(EXCLUDE_FROM_XARALX) && defined(_DEBUG)
	// Move the bottom of the dialog down...
	WINDOWPLACEMENT Placement;
	Placement.length = sizeof(Placement);
	::GetWindowPlacement(GetSafeHwnd(), &Placement);
	INT32 Height = Placement.rcNormalPosition.bottom - Placement.rcNormalPosition.top;
	if (!CCamApp::IsNewWindowsUI())
	{
		// Only resize the window on older UI i.e. NT 3 and Win 3 as handled differently
		// on Windows 95 UI forms.
		::MoveWindow(GetSafeHwnd(),
					 Placement.rcNormalPosition.left,
					 Placement.rcNormalPosition.top,
					 Placement.rcNormalPosition.right - Placement.rcNormalPosition.left,
					 Height + 20,
					 TRUE);
	}
	
	// ...and add a check box for the compression on/off preference.
	wxRect ButtonPos(10, Height - 35, 110, Height - 15);

	CompressButton.Create((TCHAR *)String_32(_R(IDS_FILEDLGS_COMPRESS_FILE)),
						WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 
						ButtonPos, this, _R(IDC_COMPRESS_BUTTON));
	
	// Give it the correct state according to the preference.
	CompressButton.SetCheck(CamelotNativeEPSFilter::CompressNative != FALSE);
#endif

	return TRUE;
}

// Maximum truncated length of the web filename that will fit in the display icon
const INT32 MaxWebName = 40;

/********************************************************************************************

>	virtual BOOL SaveFileDialog::ExplorerInitDialog(wxWindow* hDlg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/7/96
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Should be called when the explorer style dialog boxes need setting up.
				We can take this opportunity to add in any extra controls that we require.
	SeeAlso:	BaseFileDialog::ExplorerInitDialog; BaseFileDialog::OnHookMsg; BaseFileDialog::HookProc;

********************************************************************************************/

BOOL SaveFileDialog::ExplorerInitDialog(wxWindow* hDlg)
{
PORTNOTE("other", "Remove MFC code" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Do nothing of not on explorer type dialog boxes
	if (!CCamApp::IsNewWindowsUI())
		return TRUE;

	// Set up and initialise the new buttons and controls

	// Give it the correct state according to the preference.
	CheckDlgButton(_R(IDC_SHOWPREVIEW), BaseFileDialog::ShowPreviewBitmap != FALSE);

	wxWindow* Icon = FindWindow( _R(IDC_PREVIEW));
	::EnableWindow(Icon, TRUE);

	// WEBSTER - markn 28/1/97
	Icon = FindWindow( _R(IDC_FILEDLG_OPTIONS));
#ifndef WEBSTER
	::ShowWindow(Icon, SW_HIDE);
#else
	::ShowWindow(Icon, SW_SHOWNORMAL);
	::EnableWindow(Icon, TRUE);
#endif // WEBSTER

	//Graham 27/10/97: And the "use as default" button
	Icon = FindWindow( _R(IDC_USEASDEFAULT));

	::ShowWindow(Icon, SW_HIDE);
	// Move the Preview box down a little, so that it lines up with the List box
	CWnd *wndDlg = GetParent();
	wxWindow* pWndPreview = GetDlgItem(_R(IDC_PREVIEW));
	wxRect rPreview, rList;
	pWndPreview->GetWindowRect(&rPreview);
	ScreenToClient(rPreview);
	wxWindow* pWndList = wndDlg->GetDlgItem(lst1);	// lst1 is the ID for the main list box
	pWndList->GetWindowRect(&rList);
	ScreenToClient(rList);
	pWndPreview->SetWindowPos(NULL, rPreview.left, rList.top, rPreview.Width(), rPreview.Height(), 0);

	ExplorerInited = TRUE;
#endif

	return TRUE;
}

/********************************************************************************************
>	virtual void SaveFileDialog::DoDataExchange(CDataExchange* pDX)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/95
	Inputs:		(a standard MFC object)
	Purpose:	Gets the state of the "Compress file" check-box in the save file dialog.
********************************************************************************************/

PORTNOTE("other", "Removed MFC junk" )
#if !defined(EXCLUDE_FROM_XARALX)
void SaveFileDialog::DoDataExchange(CDataExchange* pDX)
{
    BaseFileDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMyDialog)
#ifdef _DEBUG
	DDX_Check(pDX, _R(IDC_COMPRESS_BUTTON), CamelotNativeEPSFilter::CompressNative);
#endif
//	DDX_Check(pDX, _R(IDC_SAVEASWEB_BUTTON), SaveWebFile); //BaseCamelotFilter::ExportWebFile);
	if (CCamApp::IsNewWindowsUI())
	{
		DDX_Check(pDX, _R(IDC_SHOWPREVIEW), BaseFileDialog::ShowPreviewBitmap);
	}
    //}}AFX_DATA_MAP
}
#endif

/********************************************************************************************

>	BOOL SaveFileDialog::PrepareDialog()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Does anything the dialogs want doing after they have been created, but before
				they are displayed

********************************************************************************************/

BOOL SaveFileDialog::PrepareDialog()
{
	// Set the title of the dialog, depending on what the document name is
	if (!DocName.IsEmpty())
	{
		// Get a string to build the document title in
		String_256 DialogTitle;

		// make a title for the dialog, using the default name for the document
		// ie it will come out a bit like this... Save MyDoc.art As
		if (DialogTitle.MakeMsg(_R(IDS_SAVENAMEDTITLE), &DocName)!=0)
		{
			// OK, we have the title in a string 256, so put it into a normal string
			UINT32 CopyLength = DialogTitle.Length() + 1;
			if (CopyLength > TITLE_SIZE)
			{
				CopyLength = TITLE_SIZE - 1;
			}
			camStrncpy( Title, (const TCHAR*)(DialogTitle), CopyLength);
			Title[CopyLength] = TEXT('\0');
		
			// Change the dialogs data structure to use the new title string
			wxFileDialog::SetTitle( Title );
		}
	}
	else
	{
		// We have no title for the document, so just use 'Save As' instead
		if (SmartLoadString(0, _R(IDS_SAVETITLE), Title, 128))
			wxFileDialog::SetTitle( Title );
	}

	// Select the desired path, use the one passed in to the constructor is available
	if (!DefaultPath.IsEmpty())
	{
		// Set the default path
		SetDirectory( DefaultPath );
	}
	else
	{
		// or use the global default path
		if( DefaultSaveFilePath.Length() == 0 )
			// use the My Documents folder
			DefaultOpenFilePath = wxStandardPaths::Get().GetUserConfigDir();

		SetDirectory( DefaultSaveFilePath );
	}

	// Do the same for the filename
	if (!DefaultName.IsEmpty())
		SetFilename( DefaultName );

	return TRUE;
}



/********************************************************************************************

>	void SaveFileDialog::SetDefaultPath(const String_256& NewPath)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		NewPath - The path to use as the default
	Purpose:	Sets the default path. The next Save As dialog will default to this path

********************************************************************************************/

void SaveFileDialog::SetDefaultPath(const String_256& NewPath)
{
	// Set the default path
	DefaultSaveFilePath = NewPath;
}


/********************************************************************************************

>	BOOL SaveFileDialog::OnFileNameOK()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/94
	Returns:	TRUE if we didn't like it, FALSE if it was OK. (This is contrary to the MFC
				docs, but they seem to be wrong).
	Purpose:	So we can do our own 'that file already exists' check, as the standard one
				used Y/N and that is officially forbidden (despite the fact that 99.999%
				of Windows programs use it). Its an MFC overridable.
	SeeAlso:	ExportFileDialog

********************************************************************************************/

BOOL SaveFileDialog::OnFileNameOK()
{
	// ask the new function if the name is OK
	if (IsValidFilename())
		return FALSE;
	else
		return TRUE;
}



/********************************************************************************************

>	void SaveFileDialog::AppendExtension(wxString* pFilename)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/95
	Inputs:		pFilename - the filename to append an extension to
	Outputs:	pFilename - the input filename with the .art extension on the end (if needed)
	Purpose:	Given a filename, this function will attach the .art file extension to the
				to it

********************************************************************************************/

void SaveFileDialog::AppendExtension(wxString* pFilename)
{
	// Make sure we were not passed rubbish
	if ((pFilename==NULL) || (pFilename->IsEmpty()))
		return;

	TRACEUSER( "Rik", _T("Append Extension in - %s\n"), (const TCHAR*)(*pFilename));

	// First we will see if this filename has an extension already
	INT32	ExtensionLen = pFilename->length() - pFilename->Find( '.', true );
	INT32	i = pFilename->Find( '.', true );

	// See if we found a valid extension
	if( ExtensionLen > 3 || i < 0 )
	{
		// No extension as yet, so add one in
		String_32 NewExtension(_R(IDS_DEFAULT_EXTENSION));
		*pFilename += wxString( _T(".") );
		*pFilename += wxString( (const TCHAR *)NewExtension );
	}

	TRACEUSER( "Rik", _T("Append Extension out- %s\n\n"), (const TCHAR*)(*pFilename) );
}

/********************************************************************************************

>	BOOL SaveFileDialog::HandleOptions()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/97
	Returns:	-
	Purpose:	Implements the options button installed for WEBSTER
				Save dlg opens up the .web save options

********************************************************************************************/

PORTNOTE("other", "Removed MFC type Hooks" )
#if !defined(EXCLUDE_FROM_XARALX)
BOOL SaveFileDialog::HandleOptions()
{
#ifndef WEBSTER
	return BaseFileDialog::HandleOptions();
#else
	if (CCamApp::IsNewWindowsUI())
	{
		CamelotWebFilter* pWebFilter = NULL;
		{
			// Find the web filter
			Filter* pFilter = Filter::GetFirst();
			while (pFilter != NULL && pFilter->FilterID != FILTERID_WEB)
				pFilter = Filter::GetNext(pFilter);		// Try the next filter

			if (pFilter != NULL && IS_A(pFilter,CamelotWebFilter))
				pWebFilter = (CamelotWebFilter*)pFilter;
		}

		// Make sure that we found the web Filter
		if (pWebFilter == NULL)
		{
			InformError(_R(IDT_CANT_FIND_FILTER));
			return FALSE;
		}

		WebPrefsDlgParam Prefs;

		Prefs.SetSelType(pWebFilter->GetSelType());

		// Use the preference option in the baseclass camelot filter;
		Prefs.SetBmpCompression(			pWebFilter->GetBitmapCompression());
		Prefs.SetExportPreviewBitmap(		pWebFilter->GetPreviewBitmapExport());
		Prefs.SetConvertTextToOutlines(		pWebFilter->GetConvertTextToOutlines());
		Prefs.SetConvertBlendsToOutlines(	pWebFilter->GetConvertBlendsToOutlines());
		Prefs.SetHTMLToClipboard(	pWebFilter->ShouldExportHTMLTag());
		
		// This is ok as we are using a modal dialog box
		BOOL Ok = WebPrefsDlg::InvokeDialog(&Prefs);

		// If ok, and the OK button was clicked to close the dlg
		if (OK && Prefs.GetWebOk())
		{
			// Set up some returned variables as the user has oked them
			pWebFilter->SetBitmapCompression(Prefs.GetBmpCompression());
			pWebFilter->SetPreviewBitmapExport(Prefs.GetExportPreviewBitmap());
			pWebFilter->SetConvertTextToOutlines(Prefs.GetConvertTextToOutlines());
			pWebFilter->SetConvertBlendsToOutlines(Prefs.GetConvertBlendsToOutlines());
			pWebFilter->SetRemoveInvisibleLayers(TRUE);
			pWebFilter->SetRemoveUnusedColours(FALSE);
			pWebFilter->SetMinimalWebFormat(FALSE);
			pWebFilter->SetSelType(Prefs.GetSelType());
			pWebFilter->SetHTMLToClipboard(Prefs.GetHTMLToClipboard());
		}
	}

	return TRUE;
#endif // WEBSTER
}
#endif

/********************************************************************************************
>	ImportFileDialog::ImportFileDialog(LPCTSTR lpszFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		lpszFilter			which file extensions to display/allow
	Purpose:	Constructs a file import dialog.

Graham 6/6/97: Took out the OFN_NOVALIDATE flag. Olivier had set this to allow
URLs to be typed into the "File name" field, but it was stopping wildcard searches
(e.g. *.gif) from working
********************************************************************************************/

ImportFileDialog::ImportFileDialog(LPCTSTR lpszFilter)
//  :	BaseFileDialog(TRUE, ((CCamApp::IsNewWindowsUI()) ? OFN_ENABLETEMPLATE : 0) | 
//			OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, lpszFilter, NULL)
	: BaseFileDialog( TRUE, wxFILE_MUST_EXIST | wxHIDE_READONLY, lpszFilter, NULL )
{
PORTNOTE("other", "Disabled template stuff" )
#if !defined(EXCLUDE_FROM_XARALX)
	if (CCamApp::IsNewWindowsUI())
	{
		// Use our template to do the ammendment and addition of controls to the dialog box
		m_ofn.lpTemplateName = MAKEINTRESOURCE(_R(IDD_PREVIEWBITMAP));
	}
	else
		m_ofn.lpTemplateName = NULL;
#endif
	
	// Set the preview bitmap to be NULL
	pBitmapToUse = NULL;
}



/********************************************************************************************
>	BOOL ImportFileDialog::OnInitDialog()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/94
	Returns:	TRUE => normal window focus is required.
	Purpose:	Make the drop-down filter box bigger than usual.
	SeeAlso:	ImportFileDialog
********************************************************************************************/

BOOL ImportFileDialog::OnInitDialog( wxInitDialogEvent& event )
{
	// Set up the string we require
	String_256 NewText(_R(IDN_IMPORTBUTTON));
	const TCHAR*	NewTextStr = NewText;

	// Get the handle of the ok button.
	wxButton*		OkButton = dynamic_cast<wxButton*>( FindWindow( wxID_OK ) );

	// Now replace the 'ok' by 'import'
	OkButton->SetLabel( NewTextStr ); 

PORTNOTE("other", "Remove code to re-arrange ImportFileDialog" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Get the handle of the filter list box.
	wxComboBox*		ComboBox = dynamic_cast<wxComboBox*>( FindWindow( cmb1 ) );

	// Find out where the list box is.
	WINDOWPLACEMENT Placement;
	Placement.length = sizeof(Placement);
	::GetWindowPlacement(ComboBox, &Placement);

	// Make the list box deeper than usual
	::MoveWindow(ComboBox,
				 Placement.rcNormalPosition.left,
				 Placement.rcNormalPosition.top,
				 Placement.rcNormalPosition.right - Placement.rcNormalPosition.left,
			     (Placement.rcNormalPosition.bottom - Placement.rcNormalPosition.top) * 5,
			     TRUE);

	// Move the bottom of the dialog down...
	Placement.length = sizeof(Placement);
	::GetWindowPlacement(GetSafeHwnd(), &Placement);
	INT32 Height = Placement.rcNormalPosition.bottom - Placement.rcNormalPosition.top;

	// ...and add a check box for the layers on/off preference.
	// We need to place the extra control differently if we are in Win 95
	// as the file dialog is a different size and shape. It also behaves differently,
	// adjusting its size to fit the controls etc. Much smarter.
	wxRect ButtonPos;
	if (CCamApp::IsNewWindowsUI()) //IsWin32c())
	{
		// Its windows 95, so use these settings
		ButtonPos = wxRect(10, 235, 160, 255);
		if (IsWin32NT())
		{
			// NT 4 seems to open the dialog box too narrow in height so the switch is hidden
			// Windows 95 shows the switch.
			::MoveWindow(GetSafeHwnd(),
				 Placement.rcNormalPosition.left,
				 Placement.rcNormalPosition.top,
				 Placement.rcNormalPosition.right - Placement.rcNormalPosition.left,
				 Height + 20,
				 TRUE);
		}
	}
	else
	{
		// Its not Windows 95, so use these settings.
		// Note, we have to expand the dialog window here as well.
		ButtonPos = wxRect(10, Height-35, 160, Height-15);
		::MoveWindow(GetSafeHwnd(),
					 Placement.rcNormalPosition.left,
					 Placement.rcNormalPosition.top,
					 Placement.rcNormalPosition.right - Placement.rcNormalPosition.left,
				     Height + 20,
				     TRUE);
	}

	// Get the text for the button - default to something useful
	// so that if SmartLoadString fails we still have something
	// even if it is in the wrong language
	char ButtonText[128] = "Import with Layers"; // Fallback - don't resourceify
	SmartLoadString(0, _R(IDS_IMPORTWITHLAYERS), ButtonText, 128);
	LayersButton.Create(ButtonText, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
						ButtonPos, this, _R(IDC_LAYERS_BUTTON));

	// ..and make it use the same font as the rest of the dialog
	HFONT hFont = (HFONT) SendMessage(WM_GETFONT);
	LayersButton.SendMessage(WM_SETFONT, (WPARAM) hFont, MAKELPARAM(FALSE, 0));

	// Give it the correct state according to the preference.
	LayersButton.SetCheck(Filter::ImportWithLayers != FALSE);
#endif

	// Leave the rest up to the base class . . .
	BaseFileDialog::OnInitDialog( event );
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL ImportFileDialog::ExplorerInitDialog(wxWindow* hDlg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/96
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Should be called when the explorer style dialog boxes need setting up.
				We can take this opportunity to add in any extra controls that we require.
	SeeAlso:	BaseFileDialog::ExplorerInitDialog; BaseFileDialog::OnHookMsg; BaseFileDialog::HookProc;

********************************************************************************************/

BOOL ImportFileDialog::ExplorerInitDialog(wxWindow* hDlg)
{
PORTNOTE("other", "Remove MFC code" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Do nothing of not on explorer type dialog boxes
	if (!CCamApp::IsNewWindowsUI())
		return TRUE;

	// Set up and initialise the new buttons and controls

	// Give it the correct state according to the preference.
	CheckDlgButton(_R(IDC_SHOWPREVIEW), BaseFileDialog::ShowPreviewBitmap != FALSE);

	wxWindow* Icon = FindWindow( _R(IDC_PREVIEW));
	::EnableWindow(Icon, TRUE);

	// WEBSTER - markn 28/1/97
	Icon = FindWindow( _R(IDC_FILEDLG_OPTIONS));
	::ShowWindow(Icon, SW_HIDE);

	//Graham 27/10/97: And the "use as default" button
	Icon = FindWindow( _R(IDC_USEASDEFAULT));
	::ShowWindow(Icon, SW_HIDE);

	//And the "default templates folder" button
	//Icon = FindWindow( _R(IDC_DEFAULTTEMPLATESFOLDER));
	//::ShowWindow(Icon, SW_HIDE);

	// Move the Preview box down a little, so that it lines up with the List box
	CWnd *wndDlg = GetParent();
	wxWindow* pWndPreview = GetDlgItem(_R(IDC_PREVIEW));
	wxRect rPreview, rList;
	pWndPreview->GetWindowRect(&rPreview);
	ScreenToClient(rPreview);
	wxWindow* pWndList = wndDlg->GetDlgItem(lst1);	// lst1 is the ID for the main list box
	pWndList->GetWindowRect(&rList);
	ScreenToClient(rList);
	pWndPreview->SetWindowPos(NULL, rPreview.left, rList.top, rPreview.Width(), rPreview.Height(), 0);

	ExplorerInited = TRUE;
#endif

	return TRUE;
}

/********************************************************************************************
>	virtual void ImportFileDialog::DoDataExchange(CDataExchange* pDX)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		(a standard MFC object)
	Purpose:	Gets the state of the "Import Layers" check-box in the import file dialog.
********************************************************************************************/

PORTNOTE("other", "Removed MFC junk" )
#if !defined(EXCLUDE_FROM_XARALX)
void ImportFileDialog::DoDataExchange(CDataExchange* pDX)
{
    BaseFileDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMyDialog)
	DDX_Check(pDX, _R(IDC_LAYERS_BUTTON), Filter::ImportWithLayers);
	if (CCamApp::IsNewWindowsUI())
	{
		DDX_Check(pDX, _R(IDC_SHOWPREVIEW), BaseFileDialog::ShowPreviewBitmap);
	}
    //}}AFX_DATA_MAP
}

void ImportFileDialog::OnLButtonDblClk( wxMouseEvent& event ) 
{
	CDialog::OnLButtonDblClk(nFlags, point);
}
#endif


/********************************************************************************************
>	ExportFileDialog::ExportFileDialog(LPCTSTR lpszFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		lpszFilter			which file extensions to display/allow
	Purpose:	Contructs an export file dialog.
********************************************************************************************/

ExportFileDialog::ExportFileDialog(LPCTSTR lpszFilter)
//	: BaseFileDialog(FALSE, ((CCamApp::IsNewWindowsUI()) ? OFN_ENABLETEMPLATE : 0) | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, lpszFilter, NULL)
	: BaseFileDialog( FALSE, 0, lpszFilter, NULL )
{
PORTNOTE("other", "Disabled template stuff" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Under Windows 95 the OnFileNameOK system does not work, so use the default one
	// Disable for VC4.0/MFC 4.0 as they have fixed it.
#if _MFC_VER < 0x400
	if (IsWin32c())
		m_ofn.Flags |= OFN_OVERWRITEPROMPT;
#endif

	if (CCamApp::IsNewWindowsUI())
	{
		// Use our template to do the ammendment and addition of controls to the dialog box
		m_ofn.lpTemplateName = MAKEINTRESOURCE(_R(IDD_PREVIEWBITMAP));
	}
	else
		m_ofn.lpTemplateName = NULL;
#endif
	
	// Set the preview bitmap to be NULL
	pBitmapToUse = NULL;	

	m_bExportABitmap = FALSE;

#if FALSE
	// for the future "Options" button handling

	// set the options to be NULL
	m_pOptions = NULL;
#endif
}



/********************************************************************************************
>	BOOL ExportFileDialog::OnInitDialog()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/03/94
	Returns:	TRUE => normal window focus is required.
	Purpose:	Make the drop-down filter box bigger than usual.
	SeeAlso:	ExportFileDialog
********************************************************************************************/

BOOL ExportFileDialog::OnInitDialog( wxInitDialogEvent& event )
{
	// Set up the string we require
	String_256 NewText(_R(IDN_EXPORTBUTTON));

	// Get the handle of the ok button.
	wxButton*	OkButton = dynamic_cast<wxButton *>( FindWindow( wxID_OK ) );

	// Now replace the 'ok' by 'export'
	OkButton->SetLabel( (LPCTSTR)NewText );

PORTNOTE("other", "Remove code to re-arrange ImportFileDialog" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Get the handle of the filter list box.
	wxComboBox*	ComboBox = dynamic_cast<wxComboBox *>( FindWindow( cmb1 ) );

	// Find out where the list box is.
	WINDOWPLACEMENT Placement;
	Placement.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(ComboBox, &Placement);

	// Make the list box deeper than usual
	::MoveWindow(ComboBox,
				 Placement.rcNormalPosition.left,
				 Placement.rcNormalPosition.top,
				 Placement.rcNormalPosition.right - Placement.rcNormalPosition.left,
			     (Placement.rcNormalPosition.bottom - Placement.rcNormalPosition.top) * 5,
			     TRUE);
#endif

	// Let the base class do the rest . . .
	BaseFileDialog::OnInitDialog( event );
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL ExportFileDialog::ExplorerInitDialog(wxWindow* hDlg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/7/96
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Should be called when the explorer style dialog boxes need setting up.
				We can take this opportunity to add in any extra controls that we require.
	SeeAlso:	BaseFileDialog::ExplorerInitDialog; BaseFileDialog::OnHookMsg; BaseFileDialog::HookProc;

********************************************************************************************/

BOOL ExportFileDialog::ExplorerInitDialog(wxWindow* hDlg)
{
PORTNOTE("other", "Remove MFC code" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Do nothing of not on explorer type dialog boxes
	if (!CCamApp::IsNewWindowsUI())
		return TRUE;

	// Replace the 'OK' button by 'Export' or 'Apply'.
/*	::SetWindowText(FindWindow( IDOK),
					(LPCSTR) String(NamedExportProp::m_fApplyNotExport
											? _R(IDS_PREVIEW_APPLY) : _R(IDN_EXPORTBUTTON)));
*/
	// Actually stand a chance of replacing the 'OK' button by 'Export' or 'Apply'...
	SetControlText(IDOK, (LPCSTR) String(NamedExportProp::m_fApplyNotExport ? _R(IDS_PREVIEW_APPLY) : _R(IDN_EXPORTBUTTON)));

	// Give it the correct state according to the preference.
	CheckDlgButton(_R(IDC_SHOWPREVIEW), BaseFileDialog::ShowPreviewBitmap != FALSE);

	wxWindow* Icon = FindWindow( _R(IDC_PREVIEW));
	::EnableWindow(Icon, TRUE);

	// WEBSTER - markn 28/1/97
	Icon = FindWindow( _R(IDC_FILEDLG_OPTIONS));
	// Grey the options button.
	::EnableWindow(Icon, FALSE);
	::ShowWindow(Icon, SW_HIDE);

	//Graham 27/10/97: And the "use as default" button
	Icon = FindWindow( _R(IDC_USEASDEFAULT));
	::ShowWindow(Icon, SW_HIDE);

	//And the "default templates folder" button
	//Icon = FindWindow( _R(IDC_DEFAULTTEMPLATESFOLDER));
	//::ShowWindow(Icon, SW_HIDE);

  	// initialise the state of the Options button
	SetStateOnFilterChange();

	// Move the Preview box down a little, so that it lines up with the List box
	CWnd *wndDlg = GetParent();
	wxWindow* pWndPreview = GetDlgItem(_R(IDC_PREVIEW));
	wxRect rPreview, rList;
	pWndPreview->GetWindowRect(&rPreview);
	ScreenToClient(rPreview);
	wxWindow* pWndList = wndDlg->GetDlgItem(lst1);	// lst1 is the ID for the main list box
	pWndList->GetWindowRect(&rList);
	ScreenToClient(rList);
	pWndPreview->SetWindowPos(NULL, rPreview.left, rList.top, rPreview.Width(), rPreview.Height(), 0);

	// set the state of the Auto-preview check-box
//J	::CheckDlgButton(GetSafeHwnd(), _R(IDC_AUTO_CHECK), BmapPrevDlg::m_bDoAutoPreview);
	return ExplorerInited = TRUE;
#endif

	return TRUE;
}

/********************************************************************************************

>	virtual void ExportFileDialog::DoDataExchange(CDataExchange* pDX)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/96
	Inputs:		(a standard MFC object)
	Purpose:	Handles data exchange messages for the export file dialog.

********************************************************************************************/

PORTNOTE("other", "Removed MFC junk" )
#if !defined(EXCLUDE_FROM_XARALX)
void ExportFileDialog::DoDataExchange(CDataExchange* pDX)
{
    BaseFileDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMyDialog)
	//DDX_Check(pDX, _R(IDC_LAYERS_BUTTON), Filter::ImportWithLayers);
	if (CCamApp::IsNewWindowsUI())
	{
		DDX_Check(pDX, _R(IDC_SHOWPREVIEW), BaseFileDialog::ShowPreviewBitmap);
	}
    //}}AFX_DATA_MAP
}
#endif				   

/********************************************************************************************

>	BOOL ExportFileDialog::OnFileNameOK()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/94
	Returns:	TRUE if we didn't like it, FALSE if it was OK. (This is contrary to the MFC
				docs, but they seem to be wrong).
	Purpose:	So we can do our own 'that file already exists' check, as the standard one
				used Y/N and that is officially forbidden (despite the fact that 99.999%
				of Windows programs use it). Its an MFC overridable.
	SeeAlso:	ExportFileDialog
********************************************************************************************/

BOOL ExportFileDialog::OnFileNameOK()
{
	// ask the new function if the name is OK
	if (IsValidFilename())
		return FALSE;
	else
		return TRUE;
}

/********************************************************************************************

>	void ExportFileDialog::AppendExtension(wxString* pFilename)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/95
	Inputs:		pFilename - the filename to append an extension to
	Outputs:	pFilename - the input filename with the .art extension on the end (if needed)
	Purpose:	Given a filename, this function will attach the appropriate file extension
				according to the chosen filter to it.
				Based on the function SaveFileDialog::AppendExtension but needs to append the
				selected filer name rather than an explicit .art.
				
				Ideally should do this when the user selects a new export filter type on the
				list and when the box is first brought up but alledgedly trapping the list
				clicks is difficult.
				
	SeeAlso:	SaveFileDialog::AppendExtension;

********************************************************************************************/

void ExportFileDialog::AppendExtension(wxString* pFilename)
{
	// Use the new routine in its default state i.e. only add extension if not present already
	AddExtension(pFilename);
	return;	
}

/********************************************************************************************

>	void ExportFileDialog::AddExtension(wxString* pFilename, BOOL Replace = FALSE, BOOL AskAboutReplace = FALSE,
										UINT32 Selection = 0)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/96
	Inputs:		pFilename		- the filename to append an extension to
				Replace			- True if want when an extension is present, it will be replaced
								- False only adds extensions when none are present
				AskAboutReplace	- If extension does not match the filter the ask about replacing it
				Selection		- Non-zero means that this is the selected item
	Outputs:	pFilename		- the input filename with the .art extension on the end (if needed)
	Purpose:	Given a filename, this function will attach the appropriate file extension
				according to the chosen filter to it.
				Based on the function SaveFileDialog::AppendExtension but needs to append the
				selected filer name rather than an explicit .art.
				
				Ideally should do this when the user selects a new export filter type on the
				list and when the box is first brought up but alledgedly trapping the list
				clicks is difficult.
				
	SeeAlso:	SaveFileDialog::AppendExtension;

********************************************************************************************/

void ExportFileDialog::AddExtension(wxString* pFilename, BOOL Replace, BOOL AskAboutReplace, UINT32 Selection)
{
	// Make sure we were not passed rubbish
	if ((pFilename==NULL) || (pFilename->IsEmpty()))
		return;

	TRACEUSER( "Neville", _T("Add Extension in - %s\n"), (PCTSTR)*pFilename );

	// First we will see if this filename has an extension already
	INT32	ExtensionLen = pFilename->length() - pFilename->Find( '.', true );
	INT32	i = pFilename->Find( '.', true );

	// If a selection has been passed in then use that, otherwise use the one stored in the
	// class variable.
	INT32 TheSelectedFilter = 0;
	if (Selection)
		TheSelectedFilter = Selection;
	else
		TheSelectedFilter = GetSelectedFilterIndex(); // could use m_ofn.nFilterIndex but beware bodge!

TRACEUSER( "Neville", _T("TheSelectedFilter - %d\n"), TheSelectedFilter);
	// Find the filter that the user has chosen
	Filter *pFilter = GetSelectedExportFilter(TheSelectedFilter);

	// If no filter was found then return immediately
	if (pFilter == NULL)
		return;

	String_32 ExtStr = pFilter->pOILFilter->FilterExt;
	// Filters might have multiple extensions listed separated by commas
	// so use first e.g. AI eps filter has ai and eps.
	// Work out the position of the first separator character, - 1 if none
	String_8	SepMark = _T(",");
	INT32		Position = ExtStr.Sub( SepMark );
	
	// If comma separator was found use up to the separator 
	if (Position > 0)
		ExtStr.Left(&ExtStr, Position);

	// See if we found a valid extension
	if ((ExtensionLen > 3) || (i < 0))
	{
		// No valid extension as yet so add one
		*pFilename += wxString( _T(".") );
		*pFilename += (TCHAR*)ExtStr;
	}
	else if (ExtensionLen <= 3 && ExtensionLen > 0 && i > 0)
	{
		// FLag we don't overwrite by default
		BOOL OverwriteExtension = FALSE;

		// Only if wanting to ask and not force the replacement do we need to
		// check the extension and ask the user if they want to correct it
		if (AskAboutReplace && !Replace)
		{
			// Valid extension found so check that it is correct
			String_32	FoundExtStr( _T("") );
			TCHAR*		FoundExt = (TCHAR *)FoundExtStr;
			INT32		j;
			for( j = 0; j < ExtensionLen; j++ )
				FoundExt[j] = pFilename->operator[](i + 1 + j);

 			// Stuff a terminator on the end
 			FoundExt[j] = '\0';
			// Ensure in lower case
			FoundExtStr.toLower();

			if (FoundExtStr != ExtStr)
			{
				// Wrong extension present so warn the user
		 		String_256 Msg;
		 		Msg.MakeMsg(_R(IDW_REPLACEEXTENSION), (TCHAR *)FoundExtStr, (TCHAR *)ExtStr);
				Error::SetError(0, Msg, 0);
				INT32 ButtonPressed = InformWarning(0, _R(IDS_CORRECT), _R(IDS_DONTCORRECT));
				Error::ClearError();

				// If the user presses 'Correct' then go and fix the extension to what
				// it should be 
				if(ButtonPressed == 1)
				{
					OverwriteExtension = TRUE;
				}
			}
		}
		else if (Replace)
		{
			// Flag that we require an overwrite
			OverwriteExtension = TRUE;
		}
		
		if (OverwriteExtension)
		{
			// Hack off the current extension
			*pFilename = pFilename->Left(i);
			
			// Add in the new one
			*pFilename += wxString( _T(".") );
			*pFilename += (TCHAR*)ExtStr;
		}
	}

	TRACEUSER( "Neville", _T("Add Extension out- %s\n\n"), PCTSTR(*pFilename) );
}




/************************************************************************************************************

>	BOOL ExportFileDialog::HandleOptions()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/07/97
	Returns:	-
	Purpose:	Implements the options button installed for WEBSTER								 
	
**************************************************************************************************************/

#if FALSE
	// for the future "Options" button handling

BOOL ExportFileDialog::HandleOptions()
{
	BOOL Ok = TRUE;

	// get the filter index
	INT32 TheSelectedFilter = GetSelectedFilterIndex(); // could use m_ofn.nFilterIndex but beware bodge!

	// Find the filter that the user has chosen
	Filter *pFilter = GetSelectedExportFilter(TheSelectedFilter);

	// check for bitmap filter
	if (pFilter->IS_KIND_OF(BaseBitmapFilter))
	{
		BaseBitmapFilter *pBmpFilter = (BaseBitmapFilter *)pFilter;

		// delete the old options
		if (m_pOptions != NULL)
			delete m_pOptions;
		m_pOptions = NULL;

		// set up our export options
		if (m_bExportABitmap)
			Ok = pBmpFilter->SetUpExportBitmapOptions(&m_pOptions, m_pBitmapToExport);
		else
			Ok = pBmpFilter->SetUpExportOptions(&m_pOptions);

		if (Ok)
		{
			// check for valid options
			Ok = m_pOptions->IsValid();		
		}
	}
	else
		Ok = FALSE;

	// if everything went ok set the options to be the defaults
	if (Ok)
		m_pOptions->SetAsDefaults();

	// Everything ok.
	return Ok;
}

#endif


/************************************************************************************************************

>	BOOL ExportFileDialog::SetStateOnFilterChange()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/07/97
	Returns:	The state of the Options button
	Purpose:	Finds out whether to grey the Options button and/or the Auto-preview checkbox
	
**************************************************************************************************************/
BOOL ExportFileDialog::SetStateOnFilterChange()
{
	// get the filter index
	INT32 TheSelectedFilter = GetSelectedFilterIndex(); // could use m_ofn.nFilterIndex but beware bodge!

	// Find the filter that the user has chosen
	Filter *pFilter = GetSelectedExportFilter(TheSelectedFilter);

	BOOL State; // the state of the options button

PORTNOTE("other", "Removed BMPFilter usage" )
	// enable the Options button, but only if a bitmap filter 
	//and not the BMP filter if exporting a bitmap
	if ((pFilter != NULL) && pFilter->IS_KIND_OF(BaseBitmapFilter) && 
		(!m_bExportABitmap /*|| !IS_A(pFilter, BMPFilter)*/ ) )
		State = TRUE; // the state of the checkbox
	else
		State = FALSE;
	
//	wxWindow* Icon;
#if FALSE
	// for the future "Options" button handling

	// enable the Options button
	Icon = FindWindow( _R(IDC_FILEDLG_OPTIONS));
	::EnableWindow(Icon, State);

	// delete the options created for the last filter
	if (m_pOptions != NULL)
		delete m_pOptions;
	m_pOptions = NULL;
#endif

	// set the state the "Auto-preview" checkbox
//	Icon = FindWindow( _R(IDC_AUTO_CHECK));

	// display it first
//	::ShowWindow(Icon, SW_SHOW);

	// when exporting a bitmap we only have auto-preview on Options if using the JPEG filter
//	BOOL AutoPreviewState = State && (!m_bExportABitmap || 
//									  (pFilter != NULL) && (IS_A(pFilter,JPEGExportFilter))); 
//	::EnableWindow(Icon, AutoPreviewState);

	return State;
}

/********************************************************************************************

>	void ExportFileDialog::OnTypeChange()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/96
	Returns:	-
	Purpose:	Should be called when the user has selected a new file type from the
				"Files of type" combo box. We might need to do things like fix the file
				extension for the currently selected save/export filter.
				Only called back if OFN_EXPLORER is set and you can only do this on Windows 95
				otherwise a dialog is not popped up.

	SeeAlso:	BaseFileDialog::OnHookMsg; BaseFileDialog::HookProc;
	SeeAlso:	ExportFileDialog::OnLBSelChangedNotify;

********************************************************************************************/

void ExportFileDialog::OnTypeChange()
{
#if _MFC_VER >= 0x400
	// Remove code completely if using less than MFC4.0 as calls like SetControlText
	// will not be present and this function will not work
	// Just in case Windows NT 3 decides to call this we'd better stop it as otherwise things
	// will be happening more than they should. This is the method used on Windows 95 or NT 4.
	if (!CCamApp::IsNewWindowsUI())
		return;

	// set the state of the Options button and the Auto-preview checkbox
	SetStateOnFilterChange();

	// Get the filename which the user has entered in the field
	wxString FileName = GetFileName();
	// If its empty then return immediately
	if (FileName.IsEmpty())
		return;
	
	// Add in the selection for the newly selected filter forcing an overwrite of any extension
	// already present
	AddExtension(&FileName, TRUE);

	// Try and put this back into the edit box containing the filename
	if (!FileName.IsEmpty())
	{
		// The following call ensures on NT. 
		SetControlText(edt1, (LPCSTR)(FileName.GetBuffer(1))); // 1 = min buffer size!
	}

#endif
	return;
}
	
/********************************************************************************************

>	virtual void ExportFileDialog::OnLBSelChangedNotify(UINT32 nIDBox, UINT32 iCurSel, UINT32 nCode)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/96
	Inputs:		nIDBox	The ID of the list box or combo box in which the selection occurred.
				iCurSel	The index of the current selection.
				nCode	The control notification code. Values are:
					 CD_LBSELCHANGE	Specifies iCurSel is the selected item in a single-selection list box.
					 CD_LBSELSUB		Specifies that iCurSel is no longer selected in a multiselection list box.
					 CD_LBSELADD		Specifies that iCurSel is selected in a multiselection list box.
					 CD_LBSELNOITEMS	Specifies that no selection exists in a multiselection list box.
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Should be called when the user has selected a new file type from the
				"Files of type" combo box. We might need to do things like fix the file
				extension for the currently selected save/export filter.
				This should be called on all OS's, so we must disable it in Windows 95. This turns
				out not to be the case as it does not seem to get called on Windows 95, probably
				due to the ExportFileDialog::OnTypeChange() override.
				It is called on initing the dialog box but shouldn't cause a problem.
				Filenames of the sort *.gif are automatically fixed by the OS. As soon as you
				type a real name in though, they are no longer fixed!
				
	SeeAlso:	BaseFileDialog::OnHookMsg; BaseFileDialog::HookProc;
	SeeAlso:	ExportFileDialog::OnTypeChange();

********************************************************************************************/

void ExportFileDialog::OnLBSelChangedNotify(UINT32 nIDBox, UINT32 iCurSel, UINT32 nCode)
{
PORTNOTE("other", "Removed MFC junk" )
#if !defined(EXCLUDE_FROM_XARALX)
	// If its Windows 95 or NT 4 then use the proper method of ExportFileDialog::OnTypeChange()
	// which is not called on Windows NT 3
	// It would appear that this does not get called at all on Windows 95!
	if (CCamApp::IsNewWindowsUI())
		return;
	
	// Combo box = filter type box
	if (nIDBox == cmb1 && nCode == CD_LBSELCHANGE)
	{
		// Find out the Path name of the newly selected file
		TRACEUSER( "Neville", _T("Selection Changed - New selection = (%i)\n"), iCurSel);
		
		// Try and put this back into the edit box containing the filename
		wxWindow* hFileNameBox = FindWindow( edt1);

		if (hFileNameBox)
		{
			// Get the current filename from the edit field
			//wxString FullPath = GetPathName(); // This doesn't work and returns zero
			String_256 StrVal;
			::GetWindowText(hFileNameBox, (LPSTR)(TCHAR*)StrVal, StrVal.MaxLength());

			// List boxes go from 1..n rather than 0..n	
			INT32 TheSelectedFilter = iCurSel + 1;
			
			// Find the filter that the user has chosen
			Filter *pFilter = GetSelectedExportFilter(TheSelectedFilter);

			if (!StrVal.IsEmpty() && pFilter && pFilter->pOILFilter)
			{
				PathName TempPath(StrVal);
				if (!TempPath.IsValid())
				{
					// There was a problem e.g. user has typed f:
					// Magically a bad pathname sets an error so clear it!
					Error::ClearError();
					return;
				}

				// Get the OILFilter class to check the extension for the selected or default filter
				// matches
				if (!pFilter->pOILFilter->DoesExtensionOfPathNameMatch(&TempPath))
				{	
					// Extension is either blank or does not match the one supplied by the filter
					// Ask the OILFilter class to fix it for us
					pFilter->pOILFilter->FixExtensionOfPathName(&TempPath);
					// Now copy that back into the name
					StrVal = TempPath.GetPath();

					// Try and put this back into the edit box containing the filename
					::SetWindowText(hFileNameBox, (TCHAR*)(StrVal));
				}
			}
		}
	}
#endif
}

/********************************************************************************************

>	BOOL ExportFileDialog::IsValidFilename()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/97
	Returns:	TRUE if the current filename is Acceptable, FALSE if not
	Purpose:	Check to see if the file name is OK.

				If the file already exists, then we check with the *currently
				selected filter* to see if it wants to accept an existing file.

				Most filters will put up a message to ask the user whether they
				want to replace the file. However, the ImagemapFilter will
				also give the user the option to intelligently insert an imagemap
				into the HTML file.

  SeeAlso:		BaseFileDialog::IsValidFilename
********************************************************************************************/

BOOL ExportFileDialog::IsValidFilename()
{
	// Get the pathname
	wxString	FullPath = GetPath();
	AppendExtension(&FullPath);

	// if doesn't exist then thats fine
	if( !wxFile::Exists( FullPath ) )
	{
		return TRUE;
	}

	// But it does exist.
	// So we need to get the currently selected filter to warn the user
	// So get the currently selected filter
	Filter*		pFilter = GetSelectedExportFilter(GetSelectedFilterIndex());

	// Check it's there
	ERROR2IF(pFilter==NULL, FALSE, "ExportFileDialog::IsValidFilename - filter does not exist");

	// And ask that filter if it wants to accept an existing file
	String_256	strFullPath(FullPath);
	PathName	pthFullPath(strFullPath);

	return pFilter->WillAcceptExistingFile(pthFullPath);
}


/********************************************************************************************

>	afx_msg void ExportFileDialog::OnAutoPreview()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/97
	Returns:	-
	Purpose:	Should be called when the user has selected a new file type from the
				"Files of type" combo box. We might need to do things like fix the file
				extension for the currently selected save/export filter.
				Only called back if OFN_EXPLORER is set and you can only do this on Windows 95
				otherwise a dialog is not popped up.

	SeeAlso:	BaseFileDialog::OnHookMsg; BaseFileDialog::HookProc;
	SeeAlso:	ExportFileDialog::OnLBSelChangedNotify;

********************************************************************************************/

PORTNOTE("other", "Removed MFC junk" )
#if !defined(EXCLUDE_FROM_XARALX)
afx_msg void ExportFileDialog::OnAutoPreview()
{
//J	BmapPrevDlg::m_bDoAutoPreview =	IsDlgButtonChecked(_R(IDC_AUTO_CHECK));
}
#endif
	
	
/************************************************************************************************************

>	BOOL ExportFileDialog::HandleOptions()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/07/97
	Returns:	-
	Purpose:	Implements the options button installed for WEBSTER								 
				This opens the GIF Animation properties tabs dialog with the Browser PReview tab selected.

**************************************************************************************************************/
BOOL ExportFileDialog::HandleOptions()
{
PORTNOTETRACE("other", "ExportFileDialog::HandleOptions - Do nothing - ControlHelper, GIFAnimationPropertyTabsDlg" );
#if !defined(EXCLUDE_FROM_XARALX)
	// Get a ptr to the correct op. 
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_EXPORTGIFANIMTABSDLG);

	if (pOpDesc != NULL)
	{
		// If the animation properties dialog is open, close it.
		if (GIFAnimationPropertyTabsDlg::GetGIFAnimationPropertyTabsDlg())
		{
			// Get a ptr to the dlg.
			GIFAnimationPropertyTabsDlg* pDlg = GIFAnimationPropertyTabsDlg::GetGIFAnimationPropertyTabsDlg();

			// Ensure it is a valid ptr.
			if(pDlg)
			{
				// Close the dialog.
				pDlg->Close();		
				pDlg->End();			
			}
		}
	
		// Select the "Browser Preview tab".
		GIFAnimationPropertyTabsDlg::SetPageToOpen(0);

		// Hide all other tabs. This flag is set back to FALSE in the destructor of AnimExOptns.
		GIFAnimationPropertyTabs::SetBrowserPreviewTab(TRUE);

		// This ensures that the dialog process works correctly.
		ControlHelper::InformModalDialogOpened();
		
		// Open the dialog.
		pOpDesc->Invoke();

		// Get a ptr to the dialog.
		GIFAnimationPropertyTabsDlg* pDlg = GIFAnimationPropertyTabsDlg::GetGIFAnimationPropertyTabsDlg();

		// Ensure the ptr is valid.
		if (pDlg)
		{
			// Send our DIM_CANCEL message.
			BROADCAST_TO_CLASS( DialogMsg( pDlg->WindowID, DIM_CANCEL, 0) ,DialogOp );
		}

		// The dialog has closed.
		ControlHelper::InformModalDialogClosed();
	}
#endif

	// Everything ok.
	return TRUE;
}


/************************************************************************************************************

>	BOOL GIFExportFileDialog::GIFExportFileDialog

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/07/97
	Returns:	-
	Purpose:	Implements the options button installed for WEBSTER								 
				This opens the GIF Animation properties tabs dialog.

**************************************************************************************************************/

GIFExportFileDialog::GIFExportFileDialog(LPCTSTR lpszFilter):ExportFileDialog(lpszFilter)
{
	// Base class does all the work.
}

/************************************************************************************************************

>	BOOL GIFExportFileDialog::HandleOptions()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/07/97
	Returns:	-
	Purpose:	Implements the options button installed for WEBSTER								 
				This opens the GIF Animation properties tabs dialog.

**************************************************************************************************************/

BOOL GIFExportFileDialog::HandleOptions()
{
PORTNOTETRACE("other", "GIFExportFileDialog::HandleOptions - Do nothing - Use NgScan" );
#if !defined(EXCLUDE_FROM_XARALX)
	// Get a ptr to the correct op. 
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_EXPORTGIFANIMTABSDLG);

	if (pOpDesc != NULL)
	{
		// If the animation properties dialog is open, close it.
		if (GIFAnimationPropertyTabsDlg::GetGIFAnimationPropertyTabsDlg())
		{
			// Get a ptr to the dlg.
			GIFAnimationPropertyTabsDlg* pDlg = GIFAnimationPropertyTabsDlg::GetGIFAnimationPropertyTabsDlg();

			// Ensure it is a valid ptr.
			if(pDlg)
			{
				// Close the dialog.
				pDlg->Close();		
				pDlg->End();			
			}
		}
	
		// Select the "Animation colours tab".
		GIFAnimationPropertyTabsDlg::SetPageToOpen(0);

		// Hide the Frame propertiestab. This flag is set back to TRUE in the destructor of AnimExOptns.
		GIFAnimationPropertyTabs::SetIncludeFramePropertiesTab(FALSE);

		// This ensures that the dialog process works correctly.
		ControlHelper::InformModalDialogOpened();

		// Open the dialog.
		pOpDesc->Invoke();

		// Get a ptr to the dialog.
		GIFAnimationPropertyTabsDlg* pDlg = GIFAnimationPropertyTabsDlg::GetGIFAnimationPropertyTabsDlg();

		// Ensure the ptr is valid.
		if (pDlg)
		{
			// Send our DIM_CANCEL message.
			BROADCAST_TO_CLASS( DialogMsg( pDlg->WindowID, DIM_CANCEL, 0 ), DialogOp );
		}

		// The dialog has closed.
		ControlHelper::InformModalDialogClosed();
	}
#endif
		
	// Everything ok.
	return TRUE;
}

/***********************************************************************************************************

>	virtual BOOL GIFExportFileDialog::ExplorerInitDialog(wxWindow* hDlg)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/07/97
	Returns:	TRUE if it worked else FALSE.
	Purpose:	Should be called when the explorer style dialog boxes need setting up.
			

************************************************************************************************************/
BOOL GIFExportFileDialog::ExplorerInitDialog(wxWindow* hDlg)
{
PORTNOTE("other", "Remove MFC code" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Do nothing of not on explorer type dialog boxes
	if (!CCamApp::IsNewWindowsUI())
		return TRUE;

	// Set up and initialise any buttons and controls.

	//The "show preview"
	CheckDlgButton(_R(IDC_SHOWPREVIEW), BaseFileDialog::ShowPreviewBitmap != FALSE);
	
	// Get a handle to the options button.
	wxWindow* Icon = FindWindow( _R(IDC_FILEDLG_OPTIONS));
	
	// Ungrey the options button.
	::EnableWindow(Icon, TRUE);	

	//Graham 27/10/97: And the "use as default" button
	Icon = FindWindow( _R(IDC_USEASDEFAULT));
	::ShowWindow(Icon, SW_HIDE);
#endif
	
	// Everything OK.
	return TRUE;
}

/********************************************************************************************

>	SaveTemplateDialog::SaveTemplateDialog(PathName& pathToShowInDialog)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/97
	Inputs:		pathToShowIngDialog		The path to show in the dialog
	Purpose:	Contructor

********************************************************************************************/

SaveTemplateDialog::SaveTemplateDialog(PathName& pathToShowInDialog)
	: SaveFileDialog( PCTSTR( String_256( _R(IDS_NEWTEMPLATES_FILTER) ) ) , 
		String_256(pathToShowInDialog.GetLocation(TRUE)),
		String_256(pathToShowInDialog.GetFileName(TRUE)), String_256() )

{
	m_fUseAsDefault=FALSE;
	m_fDefaultTemplatesFolder=TRUE;
	
	PrepareDialog();
}


/********************************************************************************************

>	virtual BOOL SaveTemplateDialog::ExplorerInitDialog(wxWindow* hDlg)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/97
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Should be called when the explorer style dialog boxes need setting up.
				We can take this opportunity to add in any extra controls that we require.
	SeeAlso:	BaseFileDialog::ExplorerInitDialog; BaseFileDialog::OnHookMsg; BaseFileDialog::HookProc;

********************************************************************************************/

BOOL SaveTemplateDialog::ExplorerInitDialog(wxWindow* hDlg)
{
PORTNOTE("other", "Remove MFC code" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Do nothing if not on explorer type dialog boxes
	if (!CCamApp::IsNewWindowsUI())
		return TRUE;

	// Set up and initialise the new buttons and controls

	//The "show preview"
	CheckDlgButton(_R(IDC_SHOWPREVIEW), BaseFileDialog::ShowPreviewBitmap != FALSE);

	//The "Use for all new drawings"
	CheckDlgButton(_R(IDC_USEASDEFAULT), FALSE);

	//And show the preview window	
	wxWindow* Icon = FindWindow( _R(IDC_PREVIEW));
	::EnableWindow(Icon, TRUE);

	//But hide the options button
	Icon = FindWindow( _R(IDC_FILEDLG_OPTIONS));
	::ShowWindow(Icon, SW_HIDE);

	// Move the Preview box down a little, so that it lines up with the List box
	wxWindow*	wndDlg = GetParent();
	wxWindow*	pWndPreview = GetDlgItem(_R(IDC_PREVIEW));
	wxRect		rPreview, rList;
	pWndPreview->GetWindowRect(&rPreview);
	ScreenToClient(rPreview);
	wxWindow*	pWndList = wndDlg->GetDlgItem(lst1);	// lst1 is the ID for the main list box
	pWndList->GetWindowRect(&rList);
	ScreenToClient(rList);
	pWndPreview->SetWindowPos(NULL, rPreview.GetLeft(), rList.GetTop(), rPreview.GetWidth(), rPreview.GetHeight(), 0);

	ExplorerInited = TRUE;
#endif

	return TRUE;
}

/********************************************************************************************
>	virtual void SaveFileDialog::DoDataExchange(CDataExchange* pDX)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/95
	Inputs:		(a standard MFC object)
	Purpose:	Data exchange
********************************************************************************************/

PORTNOTE("other", "Removed MFC junk" )
#if !defined(EXCLUDE_FROM_XARALX)
void SaveTemplateDialog::DoDataExchange(CDataExchange* pDX)
{
    BaseFileDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMyDialog)
#ifdef _DEBUG
	DDX_Check(pDX, _R(IDC_COMPRESS_BUTTON), CamelotNativeEPSFilter::CompressNative);
#endif
//	DDX_Check(pDX, _R(IDC_SAVEASWEB_BUTTON), SaveWebFile); //BaseCamelotFilter::ExportWebFile);
	if (CCamApp::IsNewWindowsUI())
	{
		DDX_Check(pDX, _R(IDC_SHOWPREVIEW), BaseFileDialog::ShowPreviewBitmap);
		DDX_Check(pDX, _R(IDC_USEASDEFAULT), SaveTemplateDialog::m_fUseAsDefault);
		//DDX_Check(pDX, _R(IDC_DEFAULTTEMPLATESFOLDER), SaveTemplateDialog::m_fDefaultTemplatesFolder);
	}
    //}}AFX_DATA_MAP
}
#endif

/********************************************************************************************

>	BOOL SaveTemplateDialog::PrepareDialog()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/97
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Does anything the dialogs want doing after they have been created, but before
				they are displayed

********************************************************************************************/

BOOL SaveTemplateDialog::PrepareDialog()
{
	// Set the title of the dialog
	m_strTitleOfDialog = String_256(_R(IDS_NEWTEMPLATES_TITLEOFDIALOG));

	wxFileDialog::SetTitle( (TCHAR*)m_strTitleOfDialog );

	// Select the desired path
	if (!DefaultPath.IsEmpty())
	{
		// Set the default path
		SetDirectory( DefaultPath );
	}
	else
	{
		// or use the global default path
		if (DefaultSaveFilePath.Length() > 0)
			SetDirectory( DefaultSaveFilePath );
	}

	// Do the same for the filename
	if (!DefaultName.IsEmpty())
		SetFilename( DefaultName );

	// See if the base class wants to do anything
	return TRUE;
}

/********************************************************************************************

>	void SaveTemplateDialog::OnUseAsDefault()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/97
	Returns:	-
	Purpose:	Called when the "Use As Default" switch is clicked on. This function
				gets the value of the switch and puts it into our member variable.

				(It seems that normal Data Exchange methods don't work if the control
				is on an extension to the dialog)

********************************************************************************************/

void SaveTemplateDialog::OnUseAsDefault()
{
	m_fUseAsDefault = dynamic_cast<wxCheckBox*>( FindWindow( _R(IDC_USEASDEFAULT) ) )->IsChecked();
}

/********************************************************************************************

>	void SaveTemplateDialog::OnDefaultTemplatesFolder()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/97
	Returns:	-
	Purpose:	Called when the "Default templates folder" switch is clicked on. This function
				gets the value of the switch and puts it into our member variable.

				(It seems that normal Data Exchange methods don't work if the control
				is on an extension to the dialog)

********************************************************************************************/

void SaveTemplateDialog::OnDefaultTemplatesFolder()
{
	// Do nothing.
}


/********************************************************************************************

>	void SaveTemplateDialog::OnFileNameChange()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/97
	Returns:	-
	Purpose:	Called when the user selects a new file. This checks the path of the file
				against the default template path of the document type. If the paths
				are the same, we put the "Use As Default" switch on.
		
********************************************************************************************/

PORTNOTE("other", "Removed MFC junk" )
#if !defined(EXCLUDE_FROM_XARALX)
void SaveTemplateDialog::OnFileNameChange()
{
#if _MFC_VER >= 0x400
	if (!CCamApp::IsNewWindowsUI())
		return;

	//Get the new filename
	wxString cstrNameOfNewFile = GetPathName();
		
	// If its empty then return immediately
	if (cstrNameOfNewFile.IsEmpty())
		return;

	PathName pathOfNewFile(cstrNameOfNewFile.GetBuffer(256));
	
	cstrNameOfNewFile.ReleaseBuffer();
	
	if (!pathOfNewFile.IsValid())
	{
		// There was a problem e.g. user has typed f:
		// Magically a bad pathname sets an error so clear it!
		Error::ClearError();
		return;
	}

#endif

	BaseFileDialog::OnFileNameChange();

	return;
}
#endif
