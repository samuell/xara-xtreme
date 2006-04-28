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
// The operations for importing and exporting using the filter system.
// These used to be are OILy as they used to invoke common dialogs etc
// and hence used to be in OilFltrs.h/cpp
// Moved to new kernel based impexpop.h/cpp 4/1/96 as there is now a class, BaseFileDialog
// and derived classes which handle all of the OILy based common dialog functionality

#include "camtypes.h"

#include "camframe.h"
#include "camelot.h"
#include "pathname.h"
#include "document.h"
#include "docview.h"
#include "app.h"
#include "errors.h"
//#include "tim.h"
#include "fixmem.h"
#include "spread.h"
#include "trans2d.h"
#include "fillattr.h"
#include "attrmgr.h"
//#include "andy.h"			// for ID?_OVERWRITE, _R(IDE_FORMATNOTSUPPORTED)
//#include "nev.h"			// _R(IDN_USER_CANCELLED), _R(IDN_FILTERNOTPRESENT), _R(IDS_OPMENUIMPORT_ONE), _R(IDS_OPMENUEXPORT_ONE)
#include "optsmsgs.h"		// options messages IMPORTWITHLAYERS
#include "bubbleid.h"
//#include "simon.h"

#include "impexpop.h"
#include "filedlgs.h"		// for ImportFileDialog & ExportFileDialog

#include "helpuser.h"
//#include "barsdlgs.h"		// for the import/export bar controls

#include "bitfilt.h"		// BaseBitmapFilter
//#include "cameleps.h"
#include "oilfltrs.h"
//#include "will3.h"
#include "bmpsdlg.h"
#include "exjpeg.h"

#include "fileutil.h"

//#include "resimmap.h"		// For _R(IDS_HTMLIMPORT_FILEDOWNLOAD)
#include "webaddr.h"		// For class WebAddress

#include "opbevel.h"

#include "bmapprev.h"

#if XAR_TREE_DIALOG
#include "cxftree.h"
#endif

#if _DEBUG
//#include "cmxtree.h"
#endif

//Webster-Ranbirr-12\11\96
#ifdef WEBSTER
#define FILTER_JPEG 2
#define TWENTY_FOUR 24
#define ONE			1
#endif //webster

// generic import filter
//#include "gendwnd.h"
//#include "webster.h"

#include "ngsentry.h"
#include "ngscan.h"
//#include "justin3.h"

#include "filtimag.h"	// The imagemap filter class
//#include "desnotes.h"	// Dream weaver integration
//#include "sliceres.h"	// For _R(IDS_NOTES_WARNING)

CC_IMPLEMENT_DYNCREATE(OpMenuImport, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpMenuExport, SelOperation)
CC_IMPLEMENT_DYNAMIC(BitmapExportParam, OpParam)
CC_IMPLEMENT_DYNCREATE(OpDroppedFile, SelOperation)


/********************************************************************************************

	Preference:	DefaultImportFilter
	Section:	Filters
	Range:		A filter ID.
	Purpose:	The unique ID of the filter to default to when importing files.
	SeeAlso:	DefaultImportFilterPath; DefaultExportFilter; DefaultExportFilterPath

********************************************************************************************/ 

/********************************************************************************************

	Preference:	DefaultImportFilterPath
	Section:	Filters
	Range:		A pathname.
	Purpose:	The path to default to when opening the Import File dialog.
	SeeAlso:	DefaultImportFilter; DefaultExportFilter; DefaultExportFilterPath

********************************************************************************************/ 

/********************************************************************************************

	Preference:	DefaultExportFilter
	Section:	Filters
	Range:		A filter ID.
	Purpose:	The unique ID of the filter to default to when exporting files.
	SeeAlso:	DefaultExportFilterPath; DefaultImportFilter; DefaultImportFilterPath

********************************************************************************************/ 

/********************************************************************************************

	Preference:	DefaultExportFilterPath
	Section:	Filters
	Range:		A pathname.
	Purpose:	The path to default to when opening the Export File dialog.
	SeeAlso:	DefaultExportFilter; DefaultImportFilter; DefaultImportFilterPath

********************************************************************************************/ 


// The most recently selected import filter
UINT32 OpMenuImport::SelectedFilterID = FILTERID_GENERIC;
INT32  OpMenuImport::SelectedFilter 	= -1;

// String to hold the default path for both import and export filter dialogs.
String_256 OpMenuImport::DefaultImportFilterPath;
String_256 OpMenuExport::DefaultExportFilterPath;



/********************************************************************************************

>	BOOL OpMenuImport::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Declares op descriptors for Acorn Draw file reading.
	Errors:		Returns FALSE on failure.

********************************************************************************************/

BOOL OpMenuImport::Init()
{
	// Derived off SelectionOperation which does not take 15 parameters.
	// (Misses out trhe vertical counterpart parameter)
	BOOL InitOK;
	InitOK = RegisterOpDescriptor(	0, 							// Tool ID
									_R(IDT_IMPORT),					// String resource ID
									CC_RUNTIME_CLASS(OpMenuImport), 	// Runtime class
									OPTOKEN_MENUIMPORT,			// Token string
									GetState,					// GetState function
									0,							// help ID
									_R(IDBBL_FILEIMPORT),			// bubble help
									_R(IDD_BARCONTROLSTORE),		// resource ID
									_R(IDC_FILEIMPORT),				// control ID
									SYSTEMBAR_FILE,				// Bar ID
									TRUE,						// Recieve system messages
									FALSE,						// Smart duplicate operation
									TRUE,						// Clean operation
									_R(IDS_OPMENUIMPORT_ONE),		// String for one copy only error
									DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC // Auto state flags
 								);
	if (!InitOK) return FALSE; 


	Camelot.DeclareSection(_T("Filters"), 5);
	Camelot.DeclarePref(_T("Filters"), _T("DefaultImportFilter"), &SelectedFilterID);
	Camelot.DeclarePref(_T("Filters"), _T("DefaultImportFilterPath"), &DefaultImportFilterPath);

PORTNOTE("other", "Removed OpGenericDownload" )
#if 0
	if (!OpGenericDownload::Init())
		return FALSE;
#endif
	
	return TRUE;
}

/********************************************************************************************

>	void OpMenuImport::Do(OpDescriptor*)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Imports a file into the current document.
	Errors:		Various. Get reported to the user.

********************************************************************************************/

void OpMenuImport::Do(OpDescriptor*)
{
	// zero is a bad state for this to be in so ensure its not in that state
	if (SelectedFilter == 0)
		SelectedFilter = -1;

	// Get the list of import filters
	FilterString = BaseFileDialog::BuildFilterString(TRUE, SelectedFilterID, &SelectedFilter);
	if (FilterString == 0)
	{
		// Unable to build filter string - report error and finish operation.
		InformError();
		FailAndExecute();
		// Finished the operation
		End();
		return;
	}

	// Create the dialog box
	ImportFileDialog FDialog(FilterString);
	
	FDialog.SetTitle(_R(IDT_IMPORT_TITLE));

	// Select the desired filter, if the default i.e. -1 then set 1 as this is the first valid index.
	if (SelectedFilter != -1)
		FDialog.SetSelectedFilterIndex(SelectedFilter);
	else
		FDialog.SetSelectedFilterIndex(1);

	// Select the desired path
	if (DefaultImportFilterPath.Length() > 0)
		FDialog.SetInitialDirectory(DefaultImportFilterPath);

	BOOL LayersImport = Filter::ImportWithLayers;

	// 'Do' the dialog and get that path/filename that we so desire
	BOOL DlgResult = FDialog.OpenAndGetFileName();

	// Check if the import with layers button has been changed and if so send a message
	// to warn other users e.g. options dialog box
	if (LayersImport != Filter::ImportWithLayers)
		BROADCAST_TO_ALL(OptionsChangingMsg(OptionsChangingMsg::IMPORTWITHLAYERS)); 

	if (!DlgResult)
	{
		// Get rid of the filter string buffer
		CCFree(FilterString);
		FilterString = 0;

		// User aborted - exit operation quietly.
		FailAndExecute();
		End();
		return;
	}

	// Remember the filter for next time.
	SelectedFilter = FDialog.GetSelectedFilterIndex();
	TRACEUSER( "luke", _T("Looking for ordinal %d"), SelectedFilter );

	// Get the filename, ensuring that the path is valid
	PathName Path;
	if (!FDialog.GetChosenFileName(&Path))
	{
		InformError();
		// Get rid of the filter string buffer
		CCFree(FilterString);
		FilterString = 0;
		FailAndExecute(); 
		End();
		return;
	}

	// Extract directory name (minus terminating backslash) and remember for next time.
	DefaultImportFilterPath = Path.GetLocation(FALSE);
			
	// Find the filter that the user chose
	Filter *pFilter = Filter::GetFirst();
	while (pFilter != 0)
	{
		if( NULL != pFilter->pOILFilter )
		{
PORTNOTE("other", "Removed CamelotEPSFilter check" )
#if !defined(EXCLUDE_FROM_XARALX)
			if( ( pFilter->GetFlags().CanImport && !IS_A( pFilter, CamelotEPSFilter ) ) &&
#else
			if( pFilter->GetFlags().CanImport &&
#endif
				pFilter->pOILFilter->Position == SelectedFilter )
			{
				TRACEUSER( "luke", _T("%s is THE filter (%d)"),
					(TCHAR*)pFilter->FilterName, pFilter->pOILFilter->Position );

				// This is the filter!
				break;
			}

			TRACEUSER( "luke", _T("%s is not selected (ord=%d)"),
				(TCHAR*)pFilter->FilterName, pFilter->pOILFilter->Position );
		}
		else
			TRACEUSER( "luke", _T("Can't handle NULL pFilter->pOILFilter for %s"),
				(TCHAR*)pFilter->FilterName );
		
		// Try the next filter
		pFilter = Filter::GetNext(pFilter);
	}
	
	// Get rid of the filter string buffer
	CCFree(FilterString);
	FilterString = 0;

	if (pFilter == 0)
	{
		InformError(_R(IDT_CANT_FIND_FILTER));
		FailAndExecute(); 
	}
	else
	{
		SelectedFilterID = pFilter->FilterID;

		String_256 URL = Path.GetPath();
		
		TRACEUSER( "luke", _T("Path = %s\n"), (TCHAR*)URL );

		//Graham 14/7/97: Get the current view
		DocView* pDocView=DocView::GetCurrent();

		//And find its centre import position
		ImportPosition posToUse=pDocView->GetCentreImportPosition();
			
		// call the Import function
		DoImport(Path, pFilter, &URL, &posToUse);
	}
}



/********************************************************************************************

>	void OpMenuImport::DoImport(PathName Path, Filter* pFilter, String_256* URL, ImportPosition* pImportPosition)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com> gascoin
	Created:	07/05/97
	Inputs:		Path - local file
				pFilter - pointer to a filter
				URL - original URL of the imported file
				pImportPosition - position to import to (added by Graham 14/7/97)
	Purpose:	import function

********************************************************************************************/

void OpMenuImport::DoImport(PathName Path, Filter* pFilter, String_256* URL, ImportPosition* pImportPosition)
{
	// If this is not a filter family, check for compatibility before asking
	// filter to load the file.
	// This means the user has chosen an explicit filter to handle the import
	if (!pFilter->IS_KIND_OF(FilterFamily))
	{
		UINT32 Size = 1024;
		size_t FileSize;
		ADDR FilterBuf = pFilter->LoadInitialSegment(Path, &Size, &FileSize);

		// If there has been a problem in the load initial segment then fail now.
		if (FilterBuf == 0)
		{
			// Tell the user about the problem and get out now while the goings good 
			InformError();
			FailAndExecute(); 
			End();
			return;
		}

		// Inform any filters that we are about to do a HowCompatible call.
		// This would allow a set of filters which have common functionality hidden in a
		// filter that cannot import and cannot export handle this call and hence set
		// itself up. This would allow it to maybe cache a result which should only be
		// checked by the first filter in the group. 
		pFilter->PreHowCompatible();
			
		// Change this to be less than 8 as the filters like the Accusoft forms return
		// 8 and 9 to make sure that they are last in the chain.
		if (pFilter->HowCompatible(Path, FilterBuf, Size, FileSize) < 8)
		{
			// Not 100% happy with this file - ask for confirmation.
			ErrorInfo Question;
			Question.ErrorMsg = _R(IDT_IMPQUERY_NOTSURE);
			Question.Button[0] = _R(IDB_IMPQUERY_IMPORT);
			Question.Button[1] = _R(IDB_IMPQUERY_DONTIMPORT);

			if (UINT32(AskQuestion(&Question)) != _R(IDB_IMPQUERY_IMPORT))
			{
				// User asked for this to be cancelled.
				TRACEUSER( "Tim", _T("Filter compatibility was less than 10\n"));

				// Close the file, report the abort and finish.
				CCFree(FilterBuf);
				FailAndExecute();
				InformMessage(_R(IDT_IMP_USERABORT));
				End();
				return;
			}
		}

		// Get rid of initial file header
		CCFree(FilterBuf);
	}

	// Start the selection operation
	if (DoStartSelOp(FALSE)) // We don't want to save the end selection status because 
							 // it will be restored automatically when the operation is 
							 // redone
	{
		// First off, we have to try and open the file
		CCDiskFile DiskFile(1024, FALSE, TRUE);

		// Get pointer to current doc 'cos we'll need it several times...
		Document* pCurDoc = Document::GetCurrent();

		try
		{
			if (!DiskFile.open(Path, ios::in | ios::binary))
			{
				// Failed to open the file...
				TRACEUSER( "Rik", _T("Failed to open file in MenuImport Do"));
				InformError(_R(IDT_IMPORT_NOTFOUND));
				FailAndExecute();
				End();
				return;
			}

			// Import the file and see if Something went a bit wrong - tell the user what it was.
			if (!pFilter->DoImport(this, &DiskFile, pCurDoc, FALSE, pImportPosition, 0, 0, URL))
			{
				// Only tell them if not special user cancelled error message
				if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
					InformError();
				else
					Error::ClearError();	// otherwise remove the error so it won't get reported
				FailAndExecute();
				End();

				// Close the file again
				if (DiskFile.isOpen())
					DiskFile.close();

				return;
			}

			// close the file
			if (DiskFile.isOpen())
				DiskFile.close();
	
		}

		// See if there was a file io error
		catch(CFileException e)
		{
			// Report the error if no one else did
			if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
				InformError();
			else
				Error::ClearError();	// otherwise remove the error so it won't get reported

			// Make sure that the file is closed
			try
			{
				if (DiskFile.isOpen())
					DiskFile.close();
			}
			catch(CFileException e)
			{
				// Failed to close the file - not much we can do about it really
			}

			// and fail
			FailAndExecute();
		}
	}

	// grab the focus
	GetMainFrame()->SetFocus();
	// Finished the operation
	End();
}


/********************************************************************************************

>	void OpMenuImport::EndImport()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/05/97
	Purpose:	End the operation. It's called by GenericDownloadOp::OnDownloadFail()

********************************************************************************************/

void OpMenuImport::EndImport()
{
	FailAndExecute();
	End();
}


/********************************************************************************************

>	BOOL OpMenuImport::OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/05/97
	Inputs:		pChangingDoc -
				State -
	Returns:	
	Purpose:	End (abort) the operation when for instance the current document has been destroyed
				while downloading.
	Errors:		

********************************************************************************************/

BOOL OpMenuImport::OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State)
{
	if (State == DocChangingMsg::ABOUTTODIE && pChangingDoc == pOurDoc)
	{
		EndImport();
	}

	return TRUE;
}


/********************************************************************************************

>	OpMenuImport::OpMenuImport()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Constructor for OpMenuImport operation.

********************************************************************************************/

OpMenuImport::OpMenuImport()
{
	OpFlags.HasOwnTimeIndicator = TRUE; // The OpMenuImport op has its own time indicator
}



/********************************************************************************************

>	OpState OpMenuImport::GetState(String_256*, OpDescriptor*)

	Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	This item is always available, so long as a document is visible.

********************************************************************************************/

OpState OpMenuImport::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	return OpSt;
}

INT32 OpMenuExport::SelectedFilter = -1;
INT32 OpMenuExport::SelectedBitmapFilter = -1;

// There was an #ifdef WEBSTER directive here that specified the initial file type for the
// export menu. I've removed this, but should a new Webster ever be needed, the initial type
// was:
//
// UINT32 OpMenuExport::SelectedFilterID = FILTERID_WEB;
// UINT32 OpMenuExport::DefaultBitmapFilterID = FILTERID_BMP;
//
// The WEBSTER directive was being triggered by the full build of Camelot.
UINT32 OpMenuExport::SelectedFilterID = FILTERID_EXPORT_JPEG;
UINT32 OpMenuExport::DefaultBitmapFilterID = FILTERID_EXPORT_JPEG;

List * OpMenuExport::m_pSelNodeList = 0;

/********************************************************************************************

>	BOOL OpMenuExport::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/03/94
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Declares op descriptor for export operation.
	Errors:		Returns FALSE on failure.

********************************************************************************************/

BOOL OpMenuExport::Init()
{
	// derived off operation so takes 16 parameters including the vertical counterpart
	BOOL InitOK;
	InitOK = RegisterOpDescriptor(	0, 							// Tool ID
									_R(IDT_EXPORT),					// String resource ID
									CC_RUNTIME_CLASS(OpMenuExport), 	// Runtime class
									OPTOKEN_MENUEXPORT,			// Token string
									GetState,					// GetState function
									0,							// help ID
									_R(IDBBL_FILEEXPORT),			// bubble help
									_R(IDD_BARCONTROLSTORE),		// resource ID
									_R(IDC_FILEEXPORT),				// control ID
									SYSTEMBAR_FILE,				// Bar ID
									TRUE,						// Recieve system messages
									FALSE,						// Smart duplicate operation
									FALSE,						// Clean operation
//									0,							// No vertical counterpart
									_R(IDS_OPMENUEXPORT_ONE),		// String for one copy only error
									DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC // Auto state flags
 								);
	if (!InitOK) return FALSE; 

	Camelot.DeclareSection(_T("Filters"), 5);
	Camelot.DeclarePref(_T("Filters"), _T("DefaultExportFilter"), &SelectedFilterID);
	Camelot.DeclarePref(_T("Filters"), _T("DefaultExportFilterPath"), &DefaultExportFilterPath);
	Camelot.DeclarePref(_T("Filters"), _T("DefaultBitmapFilter"), &DefaultBitmapFilterID);

	return TRUE;
}



/********************************************************************************************

>	void OpMenuExport::Do(OpDescriptor*)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/03/94
	Purpose:	Exports a file from the current document.
				Most of the work is now done by the DoWithParam.
	Errors:		Various. Get reported to the user.

********************************************************************************************/

void OpMenuExport::Do(OpDescriptor* OpDesc)
{
	DoWithParam(OpDesc, 0);
}



/********************************************************************************************

>	static void OpMenuExport::SaveSelection()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> / JustinF
	Created:	9/1/2000
	Purpose:	Saves the selection so it can be restored
	
********************************************************************************************/
void OpMenuExport::SaveSelection()
{
	if (m_pSelNodeList)
	{
		m_pSelNodeList->DeleteAll();
		delete m_pSelNodeList;
		m_pSelNodeList = 0;
	}

	SelRange* pRng = GetApplication()->FindSelection();
	if (pRng == 0)
	{
		ERROR3("No SelRange in OpMenuExport::SaveSelection");
		return;
	}
	
	pRng->SetGag(TRUE);
	m_pSelNodeList = pRng->MakeListOfNodes(FALSE);
}



/********************************************************************************************
>	static void OpMenuExport::NormaliseSelection()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/2000
	Purpose:	Fixes-up the selection for compound objects.
********************************************************************************************/
void OpMenuExport::NormaliseSelection()
{
	// run through the range selecting all PromoteToParent nodes
	SelRange* pRng = GetApplication()->FindSelection();
	if (pRng == 0) 
	{
		ERROR3("No range in OpMenuExport::NormaliseSelection");
		return;
	}

	for (Node* pNode = pRng->FindFirst(FALSE);
		 pNode != 0;
		 pNode = pRng->FindNext(pNode, FALSE))
			if (pNode->IsAnObject())
				for (Node* pParent = pNode->FindParent();
					 pParent != 0 && pParent->IsAnObject();
					 pParent = pParent->FindParent())
						if (pParent->ShouldITransformWithChildren())
						{
							pParent->SetSelected(FALSE);
							pParent->SetSelected(TRUE);
						}					

	pRng->Update();
}



/********************************************************************************************

>	static void OpMenuExport::RestoreSelection()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> / JustinF
	Created:	9/1/2000
	Inputs:		
	Purpose:	Restores the selection given in m_pSelNodeList
	
********************************************************************************************/
void OpMenuExport::RestoreSelection()
{
	SelRange* pRng = GetApplication()->FindSelection();
	if (pRng == 0) 
	{
		ERROR3("No range in OpMenuExport::RestoreSelection");
		return;
	}

	if (pRng->IsSelRangeGagged()) pRng->SetGag(FALSE);
	NodeRenderableInk::DeselectAll(FALSE, FALSE);

	if (!m_pSelNodeList)
	{
		ERROR3("Sel node list not set up");
		return;
	}

	for (NodeListItem* pItem = (NodeListItem*) m_pSelNodeList->GetHead();
		 pItem != 0;
		 pItem = (NodeListItem*) m_pSelNodeList->GetNext(pItem))
			if (pItem->pNode != 0) pItem->pNode->SetSelected(TRUE);

	m_pSelNodeList->DeleteAll();
	delete m_pSelNodeList;
	m_pSelNodeList = 0;

	RangeControl rg = pRng->GetRangeControlFlags();
	rg.PromoteToParent = FALSE;
	pRng->Range::SetRangeControl(rg);
	pRng->Update();
//	pRng->Update(TRUE);
}

void RemoveUneededReportedErrors()
{
	// clear silly error before End()ing the op
	// Only tell them if not special user cancelled error message
	if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED) && Error::GetErrorNumber() != 0)
		InformError();
	else
		Error::ClearError();	// otherwise remove the error so it won't get reported
}

/********************************************************************************************
>	void OpMenuExport::DoWithParam(OpDescriptor*, OpParam* pParam)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/04/95
	Inputs:		pParam	if 0 then assumed to be a normal export using any filter
						if not 0 then assumed to be a KernelBitmap* and wanting to export
						as a bitmap.
	Purpose:	Exports a file from the current document.
	Errors:		Various. Get reported to the user.
********************************************************************************************/

void OpMenuExport::DoWithParam(OpDescriptor*, OpParam* pParam)
{
	// DMc - need to do this so that compound nodes are treated appropriately
	SaveSelection();
	NormaliseSelection();

//Webster-Ranbirr-12\11\96
#ifdef WEBSTER
	// Temp. variable to store the filter type.
	INT32 BitmapFilter = 0;
#endif //webster

	// zero is a bad state for this to be in so ensure its not in that state
	if (SelectedFilter == 0) SelectedFilter = -1;
	if (SelectedBitmapFilter == 0) SelectedBitmapFilter = -1;
	
	ERROR3IF(pParam != 0 && !pParam->IS_KIND_OF(BitmapExportParam),
		"OpMenuExport::DoWithParam: OpParam passed is not a BitmapExportParam");

	// Check whether a parameter has been specified 
	UINT32 Bpp = 0;
	BitmapExportParam* pExportParam = (BitmapExportParam*) pParam;
	if (pExportParam != 0)
	{
		// At present, this is only used to export the specifed bitmap so use a list
		// of bitmap filters. We need to limit the bitmap filters to ones that can support
		// the colour depth of the specified bitmap.
		// Recover the bitmap pointer from the parameter passed in
		KernelBitmap* pTheBitmap;
		if (pExportParam->GetBitmapCount() == 0 ||
			(pTheBitmap = pExportParam->GetBitmap(0)) == 0)
		{
			ERROR2RAW("OpMenuExport::DoWithParam: no bitmaps, or null pointer");
			FailAndExecute();
			RestoreSelection();
			End();
			return;
		}

		// If it is a 32 bpp bitmap then we do a conversion to 24. So really we should ask
		// the filters whether they like 24bpp files. Otherwise, just the BMP filter will say
		// yes.
		Bpp = pTheBitmap->GetBPP();
//		if (Bpp == 32) Bpp = 24;

	//Webster-Ranbirr-12\11\96
/*	#ifndef WEBSTER
		FilterString = BaseFileDialog::BuildFilterString(
								FALSE,
								DefaultBitmapFilterID,
								&SelectedBitmapFilter,
								CC_RUNTIME_CLASS(BitmapFilter),
								Bpp,
								pExportParam->GetBitmapCount());
	#else
*/		// Save a reference to the last selected Filter, in case Cancel is entered.
		//BitmapFilter = SelectedBitmapFilter;
			
		// Is this bitmap a compressed JPEG?
		BitmapSource* pSource = 0;
		BaseBitmapFilter* pDummyFilter;

		BOOL OriginalSourcePresent = pTheBitmap->GetOriginalSource(&pSource,&pDummyFilter); 
		if (OriginalSourcePresent)
		{
			DefaultBitmapFilterID = FILTERID_EXPORT_JPEG;
			SelectedBitmapFilter = 13; //FILTER_JPEG;
		}
		else
		{

		}

		FilterString = BaseFileDialog::BuildFilterString(
								FALSE,
								DefaultBitmapFilterID,
								&SelectedBitmapFilter,
								CC_RUNTIME_CLASS(BitmapFilter),
								Bpp,
								pExportParam->GetBitmapCount());
//	#endif //webster
	}
	else
	{
		// Get the list of export filters using all filters available
		FilterString = BaseFileDialog::BuildFilterString(
								FALSE,
								SelectedFilterID,
								&SelectedFilter);
	}

	if (FilterString == 0)
	{
		// Unable to build filter string - report error and finish operation.
		InformError();
		FailAndExecute();

		// Finished the operation
		RestoreSelection();
		End();
		return;
	}

	// Create the dialog box
	ExportFileDialog FDialog(FilterString);
	INT32 TheSelectedFilter = -1;
	String_256 DefaultName;
	
	FDialog.SetTitle(_R(IDT_EXPORT_TITLE));

	// Note this ptr for use in JPEG export.
//	KernelBitmap* pBitmap = pExportParam->GetBitmap(0);
//	JPEGExportOptions::SetKernelBitmap(pBitmap);
	
	if (pExportParam == 0)
		TheSelectedFilter = SelectedFilter;		   	// Using all available filters
	else
	{
		// Exporting as a bitmap so fill in the name of the bitmap
		KernelBitmap* pTheBitmap = pExportParam->GetBitmap(0);

		if (pTheBitmap->ActualBitmap)
			DefaultName = pTheBitmap->ActualBitmap->GetName();
		else
			SetBitmapName(pTheBitmap, &DefaultName, pExportParam);
		
		// Pass this name onto the dialog box
		FDialog.SetDefaultFileName(DefaultName);
		FDialog.ExportABitmap(TRUE, pTheBitmap);

		// Now work out the selected index, if there is one
		TheSelectedFilter = SelectedBitmapFilter;
	}

	// Select the desired filter, if the default i.e. -1 then set 1 as this is the first valid index.
	FDialog.SetSelectedFilterIndex(TheSelectedFilter != -1 ? TheSelectedFilter : 1);

	// Select the desired path
	if (DefaultExportFilterPath.Length() > 0)
		FDialog.SetInitialDirectory(DefaultExportFilterPath);

	// 'Do' the dialog and get that filename that we require
	BOOL DlgResult = FDialog.OpenAndGetFileName();
	if (!DlgResult)
	{
	//Webster-Ranbirr-12\11\96	
	#ifdef WEBSTER
		// If cancel is entered then ensure that our filter 
		// is set to the previous selected filter.
		SelectedBitmapFilter = BitmapFilter; 
	#endif //webster

		CCFree(FilterString);
		FilterString = 0;
		RestoreSelection();
		End();
		return;
		
	}

	// Remember the filter for next time.
	TheSelectedFilter = FDialog.GetSelectedFilterIndex();
	if (pExportParam == 0)
		SelectedFilter = TheSelectedFilter; 		// Using all available filters
	else
		SelectedBitmapFilter = TheSelectedFilter; 	// Exporting as a bitmap only

	// Get the filename.
	PathName Path;
	FDialog.GetChosenFileName(&Path);

	// Find the filter that the user chose
	Filter* pFilter;
	for (pFilter = Filter::GetFirst();
		 pFilter != 0;
		 pFilter = Filter::GetNext(pFilter))
	{
		if( NULL == pFilter->pOILFilter )
			continue;

		if (pFilter->GetFlags().CanExport && 
			pFilter->pOILFilter->Position == TheSelectedFilter)
				// This is the filter!
				break;
	}

	if (pFilter == 0)
	{
		InformError(_R(IDT_CANT_FIND_FILTER));

		// Get rid of the filter string buffer
		CCFree(FilterString);
		FilterString = 0;
		RestoreSelection();
		FailAndExecute(); 
		End();
		return;
	}

	// This is now taken care of in FileDlgs.cpp on the ok as it then checks if the file
	// exists or not. If we do it here then this does not happen.
	// Unfortunately, chnages made to the string in the dialog do not filter through
	// and so we must fix it up here as well.
	// Always make sure that the filter's default extension is on if the user has not
	// specified an extension.
	// Get the OILFilter class to check the extension for the selected or default filter
	// matches
	if (!pFilter->pOILFilter->DoesExtensionOfPathNameMatch(&Path))
	{	
		// Extension is either blank or does not match the one supplied by the filter
		// Ask the OILFilter class to fix it for us
		pFilter->pOILFilter->FixExtensionOfPathName(&Path);
	}

	// Ensure that the path is valid
	if (!Path.IsValid())
	{
		InformError();

		// Get rid of the filter string buffer
		CCFree(FilterString);
		FilterString = 0;
		FailAndExecute(); 
		RestoreSelection();
		End();
		return;
	}

PORTNOTE("other", "Removed BmapPrevDlg usage" )
#if !defined(EXCLUDE_FROM_XARALX)
	// Need to set the path for the export dialog (used for image map stuff etc)
	BmapPrevDlg::m_pthExport = Path;
#endif
	
	// Extract directory name (minus terminating backslash) and remember for next time.
	DefaultExportFilterPath = Path.GetLocation(FALSE);
			
	UINT32 TheSelectedFilterID = pFilter->FilterID;
	if (pExportParam == 0)
		SelectedFilterID = TheSelectedFilterID;			// Using all available filters
	else	
		DefaultBitmapFilterID = TheSelectedFilterID;	// Exporting as a bitmap only

	// If we are exporting using an EPS based filter and not the CamelotEPS or NativeEPS
	// filters then must warn the user that the EPS data does not save all info and question
	// whether they want to continue or not.
	// This used to use IS_KIND_OF VectorFilter before 29/4/96 but of course this
	// really wants to be for all the eps based filters and not all vector filters
	// Graeme (11-4-00) - Added FILTERID_AIEPS to stop the warnings when doing an
	// AI format export. After all, the AIEPS filter is probably better specified
	// these days than the other EPS filters.
	if (TheSelectedFilterID != FILTERID_CAMELOT_EPS &&
		TheSelectedFilterID != FILTERID_NATIVE_EPS &&
		TheSelectedFilterID != FILTERID_AIEPS
PORTNOTE("other", "Removed EPSFilter usage" )
#if !defined(EXCLUDE_FROM_XARALX)
		&& pFilter->IS_KIND_OF ( EPSFilter )
#endif
		)
	{
		// Warn the user that they are not exporting in Camelot EPS or native
		// format, and get them to confirm that this is what they want to do.
		// (We only do this for vector formats)
		ErrorInfo Info;
		Info.ErrorMsg = _R(IDT_EXPQUERY_NOTSURE);
		Info.Button[0] = _R(IDB_EXPQUERY_EXPORT);
		Info.Button[1] = _R(IDB_EXPQUERY_DONTEXPORT);
		if (UINT32(AskQuestion(&Info)) == _R(IDB_EXPQUERY_DONTEXPORT))
		{
			// User decided not to export - abort.
			CCFree(FilterString);
			FilterString = 0;
			FailAndExecute();
			RestoreSelection();
			End();
			return;
		}
	}

	BOOL fExportedOk = FALSE;

///////////////////////////////////////////////////////////////////////////////
	// for the future "Options" button handling

	// get the export options
	// what we need is for the export dlg to appear and give the user the chance
	// to fill in the export options
	// SMFIX

	BitmapExportOptions* pOptions = NULL;
	if (pFilter->IS_KIND_OF(BaseBitmapFilter)) 
	{
		if (BmapPrevDlg::m_pExportOptions)
		{
			delete BmapPrevDlg::m_pExportOptions;
			BmapPrevDlg::m_pExportOptions = 0;
		}
		
		// only if it is a raster type and we are not saving from the gallery
		if (pExportParam == 0)
		{
			// set up with default attrib for this filter
			// and get the user to edit these default params in the export dlg (thats the FALSE param)
PORTNOTE("other", "Use SetUpExportOptions to get defaults only and not do dialog" )
			((BaseBitmapFilter*) pFilter)->SetUpExportOptions(&pOptions, FALSE);
//			((BaseBitmapFilter*) pFilter)->SetUpExportOptions(&pOptions, TRUE);
			// the dlg has been up and the user may have the graphic type
			// ask the dlg for the type that it used
			if (BmapPrevDlg::m_pExportOptions)
				pOptions = BmapPrevDlg::m_pExportOptions;
			// take responsibility for the export options away from the bmp preview dlg
			BmapPrevDlg::m_pExportOptions = 0;
			// the filter we want to export with will change too if the export options have changed type
			if (pOptions)
			{
				Filter * pOldFilter = pFilter;
				pFilter = pOptions->FindBitmapFilterForTheseExportOptions();
				// change the export extent if we have changed filters
				if (pFilter && pFilter != pOldFilter)
				{
					// set the path extention
					Path.SetType(BmapPrevDlg::m_pthExport.GetType());
				}
				else pFilter = pOldFilter;
			}

			if (!pOptions || !pFilter)
			{
				// User decided not to export - abort.
				RemoveUneededReportedErrors();
				CCFree(FilterString);
				FilterString = 0;
				FailAndExecute();
				RestoreSelection();
				End();
				return;
			}

			// do the odd details that a filter may need setting before it is ready to go (sjk 5/12/00)
			((BaseBitmapFilter*) pFilter)->PostGetExportOptions(pOptions);
		}
	}

	BOOL bFinished = FALSE;

	// check if we can simply rename the temp file created during preview
	if (pOptions != 0 && pOptions->HasTempFile() && !pOptions->GetSeparateLayerFiles())
	{
		// get the temp file name
		PathName TempPath = pOptions->GetPathName();
		String_256 FinalName = Path.GetPath();
		String_256 TempName = TempPath.GetPath();

		// delete the empty file, if one was created, and try to rename the file
		FileUtil::DeleteFile(&Path);
		
#if defined(__WXGTK__) && FALSE != wxUSE_UNICODE
		char pszTempName[256];
		char pszFinalName[256];
		wcstombs( pszTempName, TempName, 256 );
		wcstombs( pszFinalName, FinalName, 256 );		
#else
		char* pszTempName = TempName;
		char* pszFinalName = FinalName;
#endif		
		
		bFinished = !rename(pszTempName, pszFinalName);

		if (bFinished)
		{
			fExportedOk = TRUE;
			// Only tell them if not special user cancelled error message
			RemoveUneededReportedErrors();
		}
	}

	// Set flag telling exporter that we are NOT exporting a temporary file!
	if(pOptions)
		pOptions->SetTempFileFlag(FALSE);

	// Now export the image map if required
	if (pOptions && pOptions->GetSupportsImageMap() && pFilter->IS_KIND_OF(BaseBitmapFilter))
		static_cast<BaseBitmapFilter *>(pFilter)->ExportImagemap(this, &Path, GetWorkingDoc());

	if (pOptions != NULL)
	{
		//And if we should be exporting an HTML tag to the clipboard
		if (pOptions->ShouldPutHTMLTagOnClipboard() && pFilter->IS_KIND_OF(BaseBitmapFilter))
			static_cast<BaseBitmapFilter *>(pFilter)->ExportHTMLTag(&Path);

PORTNOTE("other", "Removed DreamWeaver DesignNote code" )
#if 0
		// Do we want to make the extra dream weaver design notes file?
		if (pOptions->GetCanUseDesignNotes() && UsesDesignNotes())
		{
			if( !lstrcmpi( Path.GetType(), _T("gif") ) || !lstrcmpi( Path.GetType(), _T("jpg") ) || 
				!lstrcmpi( Path.GetType(), _T("bmp") ) || !lstrcmpi( Path.GetType(), _T("png") ) )
			{
				wxString DocName( (TCHAR *)Document::GetCurrent()->GetLocation() );	
				DocName += _T("\\");
				DocName += (TCHAR *)Document::GetCurrent()->GetPathName();

				if (DocName != _T("\\") )
				{
					wxString GraphicFile( (const TCHAR *)Path.GetPath() );

					// create or modify a .mno forward source file
					CreateDesignNoteWithForwardSource(GraphicFile, DocName);
				}
				else
				{
					InformWarning (_R(IDS_NOTES_WARNING));
				}
			}
		}
#endif
	}

	if (!bFinished)
	{
		// either no temp file, or failed to rename it, do normal export

///////////////////////////////////////////////////////////////////////////////

	// First off, we have to try and open the file
	CCDiskFile theDiskFile(1024, FALSE, TRUE);
	try
	{
		/* File now opened up, if not already opened inside the DoExport functions in the filters
		// themselves. This means this is done after the export options and so the file is not trashed
		// until we start doing something to it.
		// Open up the file. This will create a zero length file just to make sure that it
		// is possible, i.e. we have write access.
		// Opened up here so that the filters do not need to know about opening the files and
		// closing them and so the preview filter can easily be used to append data into the
		// middle of the native and eps files.
		// Added 27/9/95 ios::trunc as exporting a bitmap or eps file to the same file twice
		// appended the data to the end of the file on Windows 95.
		if (!theDiskFile.open(Path, ios::in | ios::out | ios::binary | ios::trunc))
		{
			// Failed to open the file...
			InformError(_R(IDT_EXPORT_NOTFOUND));
			FailAndExecute();
			End();
			return;
		} */

		// Do we want a Preview Bitmap?
		if (pFilter->CanIncludePreviewBmp()) pFilter->IncludePreviewBmp(TRUE);

		// Export the file
		if (pExportParam != 0)
		{
			ERROR3IF(!pFilter->IS_KIND_OF(BaseBitmapFilter),
										"OpMenuExport::not a bitmap filter");
			BaseBitmapFilter* pBitmapFilter = (BaseBitmapFilter*) pFilter;

			if (pExportParam->GetBitmapCount() != 1)
				fExportedOk = pBitmapFilter->DoExportBitmaps(
									this, 				// Export bitmaps
									&theDiskFile,
									&Path,
									pExportParam);
			else
			{
///////////////////////////////////////////////////////////////////////////////
	// for the future "Options" button handling

				if (pFilter->IS_KIND_OF(BaseBitmapFilter) && pOptions)
					fExportedOk = 
						pBitmapFilter->DoExportBitmapWithOptions(
										this,			// Export bitmap
										&theDiskFile, 
										&Path,
										pExportParam->GetBitmap(0),
										pOptions);
				else
///////////////////////////////////////////////////////////////////////////////

				fExportedOk = pBitmapFilter->DoExportBitmap(
									this,				// Export bitmap
									&theDiskFile,
									&Path,
									pExportParam->GetBitmap(0)); 

			}
		}
		else
		{
///////////////////////////////////////////////////////////////////////////////
	// for the future "Options" button handling

			if (pFilter->IS_KIND_OF(BaseBitmapFilter) && pOptions/*&& CCamApp::IsNewWindowsUI()*/)
				fExportedOk = ((BaseBitmapFilter*) pFilter)->DoExportWithOptions(
												this,		// Export drawing
												&theDiskFile, 
												&Path, 
												GetWorkingDoc(), 
												pOptions);	
			else
				fExportedOk = pFilter->DoExport(this, 		// Export drawing
											&theDiskFile, 
											&Path, 
											GetWorkingDoc());
		}

		if (!fExportedOk)
		{
			if (Error::GetErrorNumber() == _R(IDN_USER_CANCELLED))
				// Suppress the error if it was the 'user has cancelled one'.
			 	Error::ClearError();
			else
			{
				// Something went a bit wrong - tell the user what it was.
				InformError();
				pFilter->DeleteExportFile(&theDiskFile);
			}
		}

		// We had better tell the filter we no longer want Previews
		if (pFilter->CanIncludePreviewBmp()) pFilter->IncludePreviewBmp(FALSE);

		// close the file
		if (theDiskFile.isOpen()) theDiskFile.close();
	}

	// See if there was a file io error
	catch( CFileException e )
	{
		// Report the error if no one else did
		RemoveUneededReportedErrors();

		// Make sure that the file is closed and deleted
		try
		{
			// First try and delete it (tries to close it first) and
			// double check to make sure it is closed.
			if (pFilter) pFilter->DeleteExportFile(&theDiskFile);
			if (theDiskFile.isOpen()) theDiskFile.close();
		}
		catch(CFileException e)
		{
			// Failed to close the file - not much we can do about it really
			TRACE( _T("Failed to close file\n"));
		}

		fExportedOk = FALSE;
	}

///////////////////////////////////////////////////////////////////////////////
	// for the future "Options" button handling
	}
///////////////////////////////////////////////////////////////////////////////

	// Get rid of the filter string buffer and restore the original selection.
	CCFree(FilterString);
	FilterString = 0;
	RestoreSelection();

	if (!fExportedOk)
	{
		// delete the export options
		if (pOptions)
			delete pOptions;

		FailAndExecute();
		End();
		return;
	}

	// TODO: did we export only the selection?
	BOOL fExportedSelection = FALSE;
	if (pFilter->IS_KIND_OF(BaseBitmapFilter) && pParam == 0 && pOptions)
	{
		fExportedSelection = (pOptions->GetSelectionType() == SELECTION);
	}

	// Make named export sets from the user's exported selections.
	if (fExportedSelection)
		CreateNamedSet(pFilter, Path);		// this will call Fail or Succeed
	else
		SucceedAndDiscard();

	// delete the export options
	if (pOptions)
		delete pOptions;

	End();
}



/********************************************************************************************
>	void OpMenuExport::CreateNamedSet(Filter* pFilter, const PathName& pthTarget)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/96
	Inputs:		The context of the just-performed export of the selected objects.
	Purpose:	Creates a named export set from the selected objects and the export
				information passed to it.
********************************************************************************************/

void OpMenuExport::CreateNamedSet(Filter* pFilter, const PathName& pthTarget)
{
PORTNOTETRACE("other", "CreateNamedSet - Do nothing - Use NgScan");
#if !defined(EXCLUDE_FROM_XARALX)
	// Try to create a named set for them based on the filename chosen.
	String_256 strName(pthTarget.GetFileName(FALSE));
	NodeSetSentinel* pSentry = GetWorkingDoc()->GetSetSentinel();
	NodeSetProperty* pSet = pSentry->FindPropertyNode(strName);

	// This code used to first check if a set of that the name & path existed with the
	// same type, then if it could find such a set it would reuse it rather than creating
	// a new set (w/ a unique name).  I have changed this so the type of the export does
	// not matter any more (Jonathan 2 January 2001).
	BOOL fMustCreateNewSet = TRUE;
	if (pSet != 0)
	{
		NamedExportProp* pProp =
					(NamedExportProp*) pSet->GetProperty(NamedExportProp::nIndex);
		if (/* pProp->GetFilter() != pFilter || */
			pProp->GetPath().GetLocation()		!=	pthTarget.GetLocation() ||
			pProp->GetPath().GetFileName(FALSE)	!=	pthTarget.GetFileName(FALSE))
		{
			// Make the set name unique by appending ascending numerals.
			String_256 strBase = strName;
			INT32 nAppend = 2;
			while (pSentry->GetNameAttr(strName) != 0)
				strName.MakeMsg(_R(IDS_NAMEDLG_UNIQUE_MASK), &strBase, nAppend++);
		}
		else
			// Same name & same path - it must be the existing set that is
			// being referred to so don't make a new one, just update the properties of
			// the one that exists (used to include filter type when checking to see if
			// we can reuse the existing name but not any more).
			fMustCreateNewSet = FALSE;
	}

	// If necessary, create a new named set of the selected objects.
	if (fMustCreateNewSet)
	{
		TRACEUSER( "JustinF", _T("Creating export set %s\n"), (LPCTSTR) strName);

		// This is taken from the implementation of OpApplyNameToSel.
		AllowOpScan aosSel(this, &theSelectedObjects),
					aosSentinel(this, &theSetSentinel);

		if (!aosSel.Scan() ||
			!aosSentinel.Scan() ||
			!DoStartSelOp(FALSE, FALSE) ||
			!ApplySingleScan(this, &theSelectedObjects, strName).Scan() ||
			!ApplySingleScan(this, &theSetSentinel, strName).Scan() ||
			!ApplyPropScan(this, strName).Scan() ||
			!aosSel.Update() ||
			!aosSentinel.Update() ||
			(pSet = pSentry->FindPropertyNode(strName)) == 0)
		{
			TRACEUSER( "JustinF", _T("OpMenuExport::CreateNamedSet: naming scan failed\n"));
			FailAndExecute();
			return;
		}
	}

	// Remember the export filter and the path to the target.
	NamedExportProp* pProp =
				(NamedExportProp*) pSet->GetProperty(NamedExportProp::nIndex);
	ERROR3IF(pProp == 0, "OpMenuExport::CreateNamedSet: no export property");
	
	pProp->SetFilter(pFilter);
	pProp->SetPath(pthTarget);
	pProp->SetDirty(); // Since the user has picked the graphic type it has been toutched and so in NOT a virgin

	// Remember the options associated with the filter.
	// TODO: fix this one day so that all filters are supported.
	ERROR3IF(!pFilter->IS_KIND_OF(BaseBitmapFilter),
				"OpMenuExport::CreateNamedSet: not a bitmap filter");
	BitmapExportOptions* pOptions =
				((BaseBitmapFilter*) pFilter)->GetBitmapExportOptions();
	pProp->SetOptions(pOptions);
#endif
}



/********************************************************************************************
>	BOOL OpMenuExport::SetBitmapName(KernelBitmap* pTheBitmap, String_256 * DefaultName, 
									BitmapExportParam* pExportParam = 0)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/96
	Inputs:		pTheBitmap	the bitmap that we are interested in
				pExportParam	the BitmapExportParameters we're using to export this bitmap
	Outputs:	DefaultName	name to fill in
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Interegates the bitmap for its name and dumps this into the given string.
				If there is an extension on the bitmap name then it will fix this to be
				correct for the filter that will be selected. If there is no extension then
				it will just append the correct extension onto the name.
********************************************************************************************/

BOOL OpMenuExport::SetBitmapName(KernelBitmap * pTheBitmap, String_256 * DefaultName, 
							BitmapExportParam* pExportParam)
{
	OILBitmap* pOilBitmap = pTheBitmap->ActualBitmap;
	if (pOilBitmap)
	{
		*DefaultName = pOilBitmap->GetName();
		// We must add the extension on for the selected or default filter
		// Only add the extension if none is present already.
		// If an extension is present then check that it corresponds to the selected or default filter
		PathName TempPath(*DefaultName);
		String_32 Extension = TempPath.GetType();
		
		// Find the filter that the user chose
		Filter *pFilter = Filter::GetFirst();
		while (pFilter != 0)
		{
			if ((pFilter->GetFlags().CanExport) && 
				(pFilter->FilterID == DefaultBitmapFilterID))
				// This is the filter!
				break;

			// Try the next filter
			pFilter = Filter::GetNext(pFilter);
		}
		
		if (pFilter && pFilter->pOILFilter)
		{
			// Get the OILFilter class to check the extension for the selected or default filter
			// matches
			if (!pFilter->pOILFilter->DoesExtensionOfPathNameMatch(&TempPath))
			{	
				// Extension is either blank or does not match the one supplied by the filter
				// Ask the OILFilter class to fix it for us
				pFilter->pOILFilter->FixExtensionOfPathName(&TempPath);
				// Now copy that back into the name
				*DefaultName = TempPath.GetPath();
			}
			// If this was imported from a multi-image file
			// then we want to either bin the number or tag it on the end
			// Actually this whole practice is rather dubious since file names may
			// contain the search string. Wait for a proper fix to import bitmap!
			String_256 fileName = TempPath.GetFileName(FALSE);	// without extension
			// Search for the image index and throw it away...
			String_32 searchString;
			searchString.MakeMsg(_R(IDS_BITMAP_NAME), "","");
			INT32 index = fileName.Sub(searchString);
			if (index > 0)		// it had a number...probably
			{
				if (pExportParam != 0 && pExportParam->GetBitmapCount() > 1)	
				{
					// multiple bitmaps - throw away any numbering
					// oops...keeps the LHS (ignores #1 position)
					fileName.Left(&fileName, index);	
				}
				else				// single bitmap - tag the number on the end, no spaces
				{
					fileName.Remove(index, searchString.Length());
				}
				TempPath.SetFileName(fileName);
				*DefaultName = TempPath.GetPath();
			}
		}
	}

	return TRUE; 
}



/********************************************************************************************

>	OpState OpMenuExport::GetState(String_256*, OpDescriptor*)

	Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/03/94
	Purpose:	This item is always available, so long as a document is visible.

********************************************************************************************/

OpState OpMenuExport::GetState(String_256 * pUIDescription, OpDescriptor * pOpDesc)
{
//Removed - Ranbir. 
//Fixes a bug.
//We cannot set the Greyed.State to TRUE, as the Save button in the bitmap gallery 
//also relies on this getstate function. 
/*
#ifdef WEBSTER
	// In Webster, we will grey out the Export option if there are no visible objects on the page
	OpState OpSt;
	String_256 DisableReason;

	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		DocRect VisibleRect = pSpread->GetPageVisibleBounds();
		BOOL Empty = VisibleRect.IsEmpty();
		if (!Empty)
		{
			// There's a visible rect, so there must be exportable objects, so don't grey out
			OpSt.Greyed = FALSE;
		}
		else
		{
			// No visible objects, so grey it out and say why
			OpSt.Greyed = TRUE; 
			// Load reason why operation is disabled
			DisableReason = String_256(_R(IDS_NO_VISIBLE_OBJECTS));
			if (pUIDescription)
	   			*pUIDescription = DisableReason;	        
		}			
	}
	else 
	{
		// No selected spread, so we are greyed
		OpSt.Greyed = TRUE;

		// Load reason why operation is disabled
		DisableReason = String_256(_R(IDS_NO_DOC));
		if (pUIDescription)
		   	*pUIDescription = DisableReason;	     
	}

 	return OpSt;   

#else
*/

	OpState OpSt;

	return OpSt;
//#endif // WEBSTER
}





/********************************************************************************************

>	BOOL OpDroppedFile::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/03/94
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Declares op descriptor for drag'n'drop file operation.
	Errors:		Returns FALSE on failure.

********************************************************************************************/

BOOL OpDroppedFile::Init()
{
	if (!RegisterOpDescriptor(0,
							 _R(IDT_IMPORT),
							 CC_RUNTIME_CLASS(OpDroppedFile),
							 OPTOKEN_DROPPEDFILE,
							 GetState,
							 0,					// help ID 
							 0,
							 0))				// bitmap ID 
		return FALSE; 

	return TRUE;
}



/********************************************************************************************

>	void OpDroppedFile::DoWithParam(OpDescriptor*, OpParam* pOpParam)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/94
	Inputs:		pOpDesc - the OpDescriptor for this operation
				pOpParam - parameters for this operations; actually an HDROP handle.
	Purpose:	Import the files dropped onto the specified document.

********************************************************************************************/

#define PATH_BUF_SIZE (256)

void OpDroppedFile::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
	// Get the handle for this dropfiles action...
PORTNOTE("other", "Removed OpGenericDownload" )
#if 0
	HDROP hDrop = (HDROP) pOpParam;

#if XAR_TREE_DIALOG
	CXFTreeDlg* pCXFTreeDlg = CXFTreeDlg::GetCurrentCXFTreeDlg();
	if (pCXFTreeDlg != 0)
	{
		char PathBuf[PATH_BUF_SIZE];
		PathBuf[0] = 0;

		// Get the next filename
		UINT32 cBytes = DragQueryFile(hDrop, 0, PathBuf, PATH_BUF_SIZE);

		pCXFTreeDlg->ShowFile(PathBuf);

		End();
		return;
	}
#endif
#if _DEBUG
	CMXTreeDlg* pCMXTreeDlg = CMXTreeDlg::GetCurrentCMXTreeDlg();
	if (pCMXTreeDlg != 0)
	{
		char PathBuf[PATH_BUF_SIZE];
		PathBuf[0] = 0;

		// Get the next filename
		UINT32 cBytes = DragQueryFile(hDrop, 0, PathBuf, PATH_BUF_SIZE);

		pCMXTreeDlg->ShowFile(PathBuf);

		End();
		return;
	}
#endif

	// Make sure we have a generic filter!
	GenericFilter *pGenericFilter = Filter::GetGenericFilter();

	if (pGenericFilter == 0)
	{
		InformError(_R(IDT_CANT_FIND_FILTER));
		FailAndExecute(); 
		End();
		return;
	}

	// Get me a DocView!
	DocView *pDocView = DocView::GetCurrent();
	// Sanity checks...
	if (pDocView == 0)
	{
		ERROR3("No current DocView while procesing drag'n'drop!");
		// give the user a random error, should never happen but just in case give them some
		// indication that something has gone wrong.
		InformError(_R(IDT_CANT_FIND_FILTER));
		FailAndExecute(); 
		End();
		// Exit cleanly
		return;
	}

	// Find out where the files were dropped...
	WinCoord DropPoint;
	DragQueryPoint(hDrop, &DropPoint);

	// Turn this into Oil coordinates...
	OilCoord OilPos = DropPoint.ToOil(pDocView);

	// ...find the spread in which the click happened...
	ImportPosition Pos;

	Pos.pSpread = pDocView->FindEnclosingSpread(OilPos);
	if (Pos.pSpread == 0)
	{
		ERROR3("Could not find spread in which the drag'n'drop happened");
		// give the user a random error, should never happen but just in case give them some
		// indication that something has gone wrong.
		InformError(_R(IDT_CANT_FIND_FILTER));
		FailAndExecute(); 
		End();
		return; // Exit reasonably nicely
	}

	// First of all convert the OilCoord into device coords
	Pos.Position = OilPos.ToDoc(Pos.pSpread, pDocView);
	
	// Translate the coord to spread coords
	Pos.pSpread->DocCoordToSpreadCoord(&Pos.Position);

	// Find out how many files were dropped
	UINT32 cFiles = DragQueryFile(hDrop, (UINT32) -1, 0, 0);

	// Load each file in turn...
	for (UINT32 iFile = 0; iFile < cFiles; iFile++)
	{
		char PathBuf[PATH_BUF_SIZE];
		PathBuf[0] = 0;

		// Get the next filename
		UINT32 cBytes = DragQueryFile(hDrop, iFile, PathBuf, PATH_BUF_SIZE);

		// Check it's not too long
		if (cBytes > (PATH_BUF_SIZE - 5))
		{
			// Too long - inform user and try the next file.
			Error::SetError(_R(IDT_FILENAME_TOO_LONG));
			InformError();
		}
		else
		{
			// Import this file...
			PathName Path(PathBuf);
			// Ensure that the path is valid
			if ( !Path.IsValid() )
			{
				// Pathname specified is invalid
				InformError();
				FailAndExecute(); 
				End();
				return;
			}

			// First off, we have to try and open the file
			CCDiskFile DiskFile(1024, FALSE, TRUE);

			// Get pointer to current doc 'cos we'll need it several times...
			Document* pCurDoc = Document::GetCurrent();

			try
			{
				if (!DiskFile.open(Path, ios::in | ios::binary))
				{
					// Failed to open the file...
					TRACEUSER( "Rik", _T("Failed to open file in MenuImport Do"));
					InformError(_R(IDT_IMPORT_NOTFOUND));
					FailAndExecute();
					End();
					return;
				}

				// Import the file and see if Something went a bit wrong - tell the user what it was.
				if (!pGenericFilter->DoImport(this, &DiskFile, pCurDoc, FALSE, &Pos))
				{
					// Something went a bit wrong - tell the user what it was.
					// Only tell them if not special user cancelled error message
					if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
						InformError();
					else
						Error::ClearError();	// otherwise remove the error so it won't get reported
					FailAndExecute(); 
					End();

					// Close the file again
					if (DiskFile.isOpen())
						DiskFile.close();

					return;
				}

				// Close the file again
				if (DiskFile.isOpen())
					DiskFile.close();

			}

			// See if there was a file io error
			catch(CFileException e)
			{
				// Report the error if no one else did
				if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
					InformError();
				else
					Error::ClearError();	// otherwise remove the error so it won't get reported

				// Make sure that the file is closed
				try
				{
					if (DiskFile.isOpen())
						DiskFile.close();
				}
				catch(CFileException e)
				{
					// Failed to close the file - not much we can do about it really
				}

				// and fail
				FailAndExecute();

			}
		}
	}
#endif
	
	// Finished the operation
	End();

	return;
}



/********************************************************************************************

>	OpState OpDroppedFile::GetState(String_256*, OpDescriptor*)

	Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/03/94
	Purpose:	This item is always available, so long as a document is visible.

********************************************************************************************/

OpState OpDroppedFile::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	return OpSt;
}



/********************************************************************************************

  >	BitmapExportParam::BitmapExportParam()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/97
	Purpose:	Default Constructor

********************************************************************************************/

BitmapExportParam::BitmapExportParam(): OpParam(0, 0)
{
	m_pBmpDlgParam = 0; 
}

/********************************************************************************************
>	BitmapExportParam::BitmapExportParam(UINT32 Count, KernelBitmap** pBitmap)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		Count - The number of indexes in the array
				pBitmap - pointer to an array of pointers to kernel bitmaps
	Outputs:	-
	Returns:	-
	Purpose:	Used to pass around lists of bitmaps.  Note that deleting the bitmap array
				is the responsibility of the caller.
********************************************************************************************/
BitmapExportParam::BitmapExportParam(UINT32 Count, KernelBitmap** pBitmap, BmpDlgParam* pParam) :
	OpParam((INT32)Count, (void *)pBitmap)
{
	ERROR3IF(Count==0, "Zero bitmap count, that's not right");
	ERROR3IF(pBitmap==0, "0 bitmap array");
	ERROR3IF(!(*pBitmap)->IS_KIND_OF(KernelBitmap), "This dosen't seem to be an array of kernel bitmaps");

	// Save a reference to the BmpDlgParam, for use in BmpPrefsDlg.
	SetBmpDlgParam(pParam);
}



/********************************************************************************************
>	virtual UINT32 BitmapExportParam::GetBitmapCount()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		-
	Outputs:	-
	Returns:	The number of bitmaps in the bitmap array.  (The array runs 0..Count-1)
	Purpose:	For getting the number of bitmaps in the array!
********************************************************************************************/
UINT32 BitmapExportParam::GetBitmapCount()
{
	return (UINT32)Param1;
}



/********************************************************************************************
>	virtual KernelBitmap* BitmapExportParam::GetBitmap(UINT32 Index)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		Index - the index of the required bitmap
	Outputs:	-
	Returns:	Pointer to the nth bitmap, 0 if error
	Purpose:	For safely getting a pointer to a bitmap in the bitmap array
********************************************************************************************/
KernelBitmap* BitmapExportParam::GetBitmap(UINT32 Index)
{
	ERROR2IF(Index>=(UINT32)Param1, 0, "Index out of bounds");
	ERROR2IF(Param2==0, 0, "0 array");

	ERROR3IF( !( ( (KernelBitmap**)(void *)Param2 )[Index])->IS_KIND_OF(KernelBitmap), "This dosen't seem to be a pointer to a kernel bitmap");
	return( (KernelBitmap**)(void *)Param2 )[Index];
}


/********************************************************************************************
>	void BitmapExportParam::SetBmpDlgParam(BmpDlgParam* pParam)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/96
	Inputs:		pParam - Allows the transfer of bitmap info.
	Outputs:	-
	Returns:	-
	Purpose:	-
********************************************************************************************/
BOOL BitmapExportParam::SetBmpDlgParam(BmpDlgParam* pParam)
{
	ERROR2IF(pParam == 0,FALSE, "pParam is 0");
	m_pBmpDlgParam = pParam;
	return TRUE;
}


