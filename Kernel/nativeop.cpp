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
// The Native Save/Load operations

/*
*/

#include "camtypes.h"
#include "nativeop.h"
#include "filters.h"
#include "native.h"		// proper native filter designed by us
#include "nativeps.h"	// old native filter based on eps, used for version 1.1  
#include "oilfltrs.h"
//#include "tim.h"
//#include "nev.h"
//#include "rik.h"
#include "document.h"
#include "filedlgs.h"
//#include "oilfiles.h"
#include <stdlib.h>
#include <errno.h>
//#include "barsdlgs.h"
#include "webop.h"
//#include "filtrres.h"	// New native filter strings
#include "fixmem.h"		// CCFree
#include "bubbleid.h"

#include "camdoc.h"		// for CCamDoc::GetTemplateFilename
//#include "justin3.h"	// for _R(IDS_SAVE_AS_DEFAULT_EMBEDDED)
#include "app.h"		// GetApplication
#include "sgliboil.h"	// for SGLibOil::FileExists
//#include "resimmap.h"	// for various resources

#include "menuops.h"

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNCREATE(OpMenuLoad, SelOperation)
#ifdef DO_EXPORT
	CC_IMPLEMENT_DYNCREATE(OpMenuSave, Operation)
	CC_IMPLEMENT_DYNCREATE(OpSaveAsNativeV1, Operation)
#endif

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



/********************************************************************************************
>	BOOL InvokeNativeFileOp(const TCHAR* pOpName, CCLexFile* pFile, UINT32 nPrefFilter)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/96
	Inputs:		pOpName		-		the OPTOKEN of the Operation to invoke on pFile.
				pFile		-		pointer to the CCLexFile to use for this operation.
				nPrefFilter	-		the filter to use to import the native file, usually
									FILTERID_GENERIC.  Set to -1 to use the last known
									filter the user selected in a file dialog.
	Returns:	TRUE if the Operation is performed successfully, FALSE if it can't be
				found or performed.
	Purpose:	Shorthand for invoking the native file Operations.
	SeeAlso:	OpMenuLoad; OpMenuSave; CCFile; Operation; OpDescriptor; OpParam
********************************************************************************************/

BOOL InvokeNativeFileOp(const TCHAR* pOpName, CCLexFile* pFile, UINT32 nPrefFilter)
{
	// Find the requested Operation.
	OpDescriptor* pOp = OpDescriptor::FindOpDescriptor((TCHAR*) pOpName);
	if (pOp == 0)
	{
		ERROR3("Can't find OpDescriptor in InvokeNativeFileOp");
		return FALSE;
	}

	// Build the parameter block.
	NativeFileOpParams pm;
	pm.pFile = pFile;
	pm.fStatus = FALSE;
	pm.nPrefFilter = nPrefFilter;

	// Invoke it with the given parameters, returning its status.
	OpParam				param( &pm, INT32(0) );
	pOp->Invoke( &param );
	return pm.fStatus;
}



/********************************************************************************************

>	static BOOL OpMenuLoad::Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	TRUE if it worked, FALSE if not.
	Purpose:	Registers the opdescriptor for the Native Load operation.

********************************************************************************************/

BOOL OpMenuLoad::Init()
{
	// Register this operations descriptor
	return RegisterOpDescriptor(0, _R(IDT_IMPORT), CC_RUNTIME_CLASS(OpMenuLoad),
								OPTOKEN_NATIVELOAD, GetState,
								0, _R(IDBBL_IMPORTFILEOP), 0,0,SYSTEMBAR_ILLEGAL,
								TRUE, FALSE, TRUE);

	return false;
}



/********************************************************************************************

>	OpState OpMenuLoad::GetState(String_256*, OpDescriptor*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	Yet another annoying OpState function that does nothing

********************************************************************************************/

OpState OpMenuLoad::GetState(String_256*, OpDescriptor*)
{
	// Dunno
	OpState Blobby;
	return Blobby;
}



/********************************************************************************************
>	void OpMenuLoad::DoWithParam(OpDescriptor*, OpParam* pOpParam)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/96
	Inputs:		OpDescriptor - Not Used.
				pOpParam->Param1 - a pointer to the NativeFileOpParams for this invokation.
	Purpose:	Loads a Camelot Native EPS file.
********************************************************************************************/

void OpMenuLoad::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
	// Extract the parameters.
	NativeFileOpParams* ppm = (NativeFileOpParams*)(void *)(pOpParam->Param1);

	// Try to load the file.
	ERROR3IF(!ppm->pFile, "Null CCLexFile* in OpMenuLoad::DoWithParam");
	ppm->fStatus = ppm->pFile && LoadFile(ppm->pFile, ppm->nPrefFilter);
	if (!ppm->fStatus) FailAndExecute();

	// Before we call End() we must ensure that CurrentDoc == SelectedDoc
	Document::GetSelected()->SetCurrent();
	End();
}



/********************************************************************************************
>	BOOL OpMenuLoad::LoadFile(CCLexFile* pFileToLoad, UINT32 nPrefFilter)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & JustinF
	Created:	3/1/95
	Inputs:		pFileToLoad - the file to load from.
				nPreFilter  - the ID of the filter to use
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Tries to load the file specified. If it is not a Camelot file then it
				should report errors etc. Most of this function is actually looking for
				the Native Load/Save filter to try and load.
	SeeAlso:	OpMenuLoad; RecentFileOp
********************************************************************************************/

BOOL OpMenuLoad::LoadFile(CCLexFile* pFileToLoad, UINT32 nPrefFilter)
{
	// Make sure we have a valid file to load.
/*	TRACEUSER( "JustinF", _T("In OpMenuLoad::LoadFile(%p, %u)\n"),
				(LPVOID) pFileToLoad, nPrefFilter);
*/	ERROR3IF(!pFileToLoad, "Null CCLexFile* in OpMenuLoad::LoadFile");
	
	// Find out the position of the filter selected by the user in the open dialog
	INT32 SelectedPos = 0;

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	SelectedPos = BaseFileDialog::SelectedFilter;
#endif

	// Go get the first filter in the list
	Filter* pFilter = Filter::GetFirst();

	// Do we know which filter was used? (we know nothing about things in the recent file
	// list).  If we don't, then use the preferred one, by default the generic filter.
	if (nPrefFilter != FILTERID_USERCHOICE || SelectedPos == 0)
	{
		// We know nothing. We will have to go and have a look at all the possibles
		// We will find the Filter Family and ask it to try and load the file.
		UINT32 nID = (nPrefFilter != FILTERID_USERCHOICE) ? nPrefFilter : FILTERID_GENERIC;
		while (pFilter != NULL && pFilter->FilterID != nID)
		{
			// Try the next filter
			pFilter = Filter::GetNext(pFilter);
		}
	}
	else
	{
		// We know which type of filter the user had selected in the file dialog
		// Find the filter that the user chose.
		while (pFilter != NULL)
		{
			// This is the filter?
			if (pFilter->GetFlags().CanImport &&
				pFilter->pOILFilter->Position == SelectedPos)
					break;

			// Try the next filter
			pFilter = Filter::GetNext(pFilter);
		}
	}
	
	// Check that the Filter existed
	if (pFilter == NULL)
	{
		// It did not...
		InformError(_R(IDT_CANT_FIND_FILTER));
		return FALSE;
	}

	// Get pointer to current doc 'cos we'll need it several times...
	Document* pCurDoc = Document::GetCurrent();

	// If this is not a filter family, check for compatibility before asking
	// filter to load the file.
	// This means the user has chosen an explicit filter to handle the import
	PathName Path = pFileToLoad->GetPathName();
	String_256 FilePath = Path.GetPath();
	// FilePath will be null if a pathname is not valid
	if (!pFilter->IS_KIND_OF(FilterFamily) && !FilePath.IsEmpty())
	{
		UINT32		Size = 1024;
		size_t		FileSize;
		ADDR		FilterBuf = pFilter->LoadInitialSegment(Path, &Size, &FileSize);

		// If there has been a problem in the load initial segment then fail now.
		if (FilterBuf == NULL)
		{
			// Tell the user about the problem and get out now while the goings good 
			InformError();
			return FALSE;
		}

		// Inform any filters that we are about to do a HowCompatible call.
		// This would allow a set of filters which have common functionality hidden in a
		// filter that cannot import and cannot export handle this call and hence set
		// itself up. This would allow it to maybe cache a result which should only be
		// checked by the first filter in the group. 
		pFilter->PreHowCompatible();
		
		// Change this to be less than 8 as the filters like the Accusoft forms return
		// 8 and 9 to make sure that they are last in the chain.
		if (pFilter->HowCompatible(Path, FilterBuf, Size, UINT32(FileSize)) < 8)
		{
			// Not 100% happy with this file - ask for confirmation.
			ErrorInfo Question;
			Question.ErrorMsg = _R(IDW_OPENQUERY_NOTSURE);
			Question.Button[0] = _R(IDB_OPENQUERY_OPEN);
			Question.Button[1] = _R(IDB_OPENQUERY_DONTOPEN);

			if ((ResourceID)AskQuestion(&Question) != _R(IDB_OPENQUERY_OPEN))
			{
				// User asked for this to be cancelled.
				TRACEUSER( "Tim", _T("Filter compatibility was less than 10\n"));

				// Close the file, report the abort and finish.
				CCFree(FilterBuf);
				//InformMessage(_R(IDT_IMP_USERABORT));
				return FALSE;
			}
		}

		// Get rid of initial file header
		CCFree(FilterBuf);
	}

	// we have to try and open the file
	try
	{
		// Found the Filter, so ask it to import the file please
		if (!pFilter->DoImport(this, pFileToLoad, pCurDoc))
		{
			// Something went a bit wrong - tell the user what it was.
			// Only tell them if not special user cancelled error message
			if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
			{
				// Only supress the error if not the special user abort error
				// ***** For now use the native EPS filter
				if (pFilter->FilterID == FILTERID_NATIVE_EPS &&
					Error::GetErrorNumber() != _R(IDT_IMPORT_USERABORT))
				{
					Error::ClearError();
					InformError(_R(IDS_ERRORINARTFILE));
				}
				else
				{
					// Tell the user what the problem was
					InformError();
					wxMessageDialog dlg(
						NULL,
						_T( "Xara LX failed to load the design.\n\n")
						_T( "This is an early demonstration version of the program which does ")
						_T( "not yet support all of the data types that can appear in XAR designs."),
						_T("Load failed"),
						wxOK
					);
					dlg.ShowModal() ;
				}
			}
			else
			{
				// otherwise remove the error so it won't get reported
				Error::ClearError();
			}

			// and fail
			return FALSE;
		}

	}
	// See if there was a file io errir
	catch( CFileException )
	{
		// Report the error if no one else did, otherwise clear it.
		if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
			InformError();
		else
			Error::ClearError();

		// and fail
		return FALSE;
	}

	// Success.
	return TRUE;
}



#ifdef DO_EXPORT

/********************************************************************************************

>	void OpMenuSave::DoWithParam(OpDescriptor*, OpParam* pOpParam)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Inputs:		OpDescriptor - Not Used
				pOpParam->Param1 - A TCHAR* pointer to the filename of the file to save
				pOpParam->Param2 - Should be 0
	Purpose:	Saves the document into the file specified using the Native EPS filter

********************************************************************************************/

void OpMenuSave::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
	// Extract the parameters.
	NativeFileOpParams* ppm = (NativeFileOpParams*) (void *) (pOpParam->Param1);

	// Try to save the file.
	ERROR3IF(!ppm->pFile, "Null CCLexFile* in OpMenuSave::DoWithParam");
	ppm->fStatus = ppm->pFile && FindFilterAndSave(ppm->pFile, ppm->nPrefFilter);
	End();
}



/********************************************************************************************

>	virtual void OpMenuSave::Do(OpDescriptor* pOpDesc)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		pOpDesc - the OpDescriptor that called this op
	Purpose:	Saves the Current Doc as the default document

********************************************************************************************/

void OpMenuSave::Do(OpDescriptor* pOpDesc)
{
	//First get the selected document
	Document* pdocToSave=Document::GetSelected();
	CCamDoc* pccamdocToSave = pdocToSave->GetOilDoc();

	if (pdocToSave==NULL || pccamdocToSave==NULL)
	{
		ERROR2RAW("No default document!");
		return;
	}

	//And we'll need a pointer to the application
	Application* pCamelot=GetApplication();

	//And put the default templates path in the dialog
	PathName pathToPutInDialog=pCamelot->GetTemplatesPath();
	
	//Now create the dialog
	SaveTemplateDialog dialogToDisplay(pathToPutInDialog);
		
	//And show it
	if (dialogToDisplay.ShowModal() == wxID_OK)
	{
		//Then get the path they specified, using this amazingly bad, confusing and
		//undocumented file dialog code

		//The "CString" reference should ideally go in Winoil
		PathName pathToSaveTo;
		dialogToDisplay.GetChosenFileName(&pathToSaveTo);

		wxString cstrPathToSaveTo=pathToSaveTo.GetPath(FALSE);
		dialogToDisplay.AppendExtension(&cstrPathToSaveTo);

		String_256 strPathToSaveTo=cstrPathToSaveTo;
		pathToSaveTo=strPathToSaveTo;
						
		// Create the save file.
		CCDiskFile file(pathToSaveTo, ios::out | ios::binary | ios::trunc);

		// First find the filter.
		Filter *pFilter = FindFilter ( FILTERID_NATIVE );

		// Tell it to save attributes.
		pFilter->SetSaveAttributes ( TRUE );

		// Then save the image to the file.
		Save ( pFilter, &file );
		
		//Now, if we should make that path the default path
		if (SaveTemplateDialog::m_fUseAsDefault)
		{
			if (pdocToSave->IsAnimated())
			{
				DocOps::ms_strDefaultAnimationTemplate=strPathToSaveTo;
			}
			else
			{
				DocOps::ms_strDefaultDrawingTemplate=strPathToSaveTo;
			}
		}

		if (SaveTemplateDialog::m_fDefaultTemplatesFolder)
		{
			String_256 strDefaultPath=pathToSaveTo.GetLocation(TRUE);
			pCamelot->SetTemplatesPath(strDefaultPath);
		}
	}

	// Finished the operation
	End();
}



/********************************************************************************************

>	virtual UINT32 OpMenuSave::GetSearchFilterId()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/7/96
	Inputs:		-
	Returns:	The id of the filter to search for.
	Purpose:	To allow different ops to just specify different filters and reuse
				the same operation code to save out the file in different file
				formats.

********************************************************************************************/

UINT32 OpMenuSave::GetSearchFilterId()
{
//#ifdef NEW_NATIVE_FILTER
	// Use the new native filter
	return FILTERID_NATIVE;
//#else
//	return FILTERID_NATIVE_EPS;
//#endif
}



/********************************************************************************************

>	virtual BOOL OpMenuSave::FixFileType()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if fixing is required, FALSE otherwise
	Purpose:	To determine whether this operation requires the file type or extension on
				the specified file path to be correct and match the	filter we are about to use.
				This baseclass version says no as the old native save operation did not do
				it.

********************************************************************************************/

BOOL OpMenuSave::FixFileType()
{
//#ifdef NEW_NATIVE_FILTER
//	// Say we do want it in this baseclass version
//	return TRUE;
//#else
	// Say we do not want it in this baseclass version
	return FALSE;
//#endif
}



/********************************************************************************************

>	virtual BOOL OpMenuSave::EnsureFileTypeCorrectId(Filter*, PathName& Path)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/7/96
	Inputs:		pFilter - The filter to save with
				Path - The pathname of the file to save to
	Returns:	TRUE if it worked, FALSE if there was an error
	Purpose:	To ensure that the file type or extension on the specified file path is
				correct and matches the	filter we are about to use.

********************************************************************************************/

BOOL OpMenuSave::EnsureFileTypeCorrectId(Filter* pFilter, PathName& Path)
{
	if (pFilter && pFilter->pOILFilter)
	{
		// Get the OILFilter class to check the extension for the selected or default filter
		// matches
		if (!pFilter->pOILFilter->DoesExtensionOfPathNameMatch(&Path))
		{	
			// Extension is either blank or does not match the one supplied by the filter
			// Ask the OILFilter class to fix it for us
			pFilter->pOILFilter->FixExtensionOfPathName(&Path);
		}
	}

	return TRUE;
}



/********************************************************************************************

>	virtual BOOL OpMenuSave::FindFilterAndSave ( CCLexFile* pFile, UINT32 nPrefFilter )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> and Graeme
	Created:	31/3/95
	Inputs:		pFile			- the file to save into
				nPrefFilter		- the filter to use, or FILTERID_USERCHOICE if this
								  Operation's default GetSearchFilterId filter should be
								  used.
	Returns:	TRUE if it worked, FALSE if not. FailAndExecute will have been called if
				FALSE is returned.
	Purpose:	This function finds the appropriate filter to save with and saves the 
				current doc into the file specified. This version is a wrapper around two
				methods, FindFilter () and Save (), which allows for a bit more flexibility
				in how the file output is handled.

********************************************************************************************/

BOOL OpMenuSave::FindFilterAndSave ( CCLexFile* pFile, UINT32 nPrefFilter )
{
	// Find the appropriate filter.
	Filter *pFilter = FindFilter ( nPrefFilter );

	// And save the file using it.
	return Save ( pFilter, pFile );
}

/********************************************************************************************

>	virtual Filter* OpMenuSave::FindFilter ( UINT32 nPrefFilter )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> and Graeme
	Created:	14/3/00
	Inputs:		nPrefFilter		- the filter to use, or FILTERID_USERCHOICE if this
								  Operation's default GetSearchFilterId filter should be
								  used.
	Returns:	A pointer to a filter if successful, NULL otherwise.
	Purpose:	Finds the appropriate filter type for the export being done.

********************************************************************************************/

Filter* OpMenuSave::FindFilter ( UINT32 nPrefFilter )
{
	UINT32 SearchFilterId = ( nPrefFilter != FILTERID_USERCHOICE ) ? nPrefFilter
																 : GetSearchFilterId ();
	Filter* pFilter = Filter::GetFirst ();

	while ( pFilter != NULL && pFilter->FilterID != SearchFilterId )
	{
		// Try the next filter
		pFilter = Filter::GetNext ( pFilter );
	}

	// Make sure that we found the Native Filter
	if ( pFilter == NULL )
	{
		InformError ( _R(IDT_CANT_FIND_FILTER) );
		FailAndExecute (); 
	}

	return pFilter;
}

/********************************************************************************************

>	virtual BOOL OpMenuSave::Save ( Filter *pFilter, CCLexFile *pFile )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> and Graeme
	Created:	14/3/00
	Inputs:		pFilter	- A pointer to an output filter.
				pFile	- A pointer to the output file.
	Returns:	TRUE if the file was successfully exported, FALSE if there was a problem.
	Purpose:	Finds the appropriate filter type for the export being done.

********************************************************************************************/

BOOL OpMenuSave::Save ( Filter *pFilter, CCLexFile *pFile )
{
	// Check that the extension is ok according to this filter
	// But only if the operation requires it
	if (FixFileType())
	{
		PathName pth = pFile->GetPathName();
		if (pth.IsValid()) EnsureFileTypeCorrectId(pFilter, pth);
	}

	// open the file and export into it
	if (!SaveSpecificFile(pFilter, pFile))
	{
		FailAndExecute();
		return FALSE;
	}

	// Success.
	return TRUE;
}



/********************************************************************************************

>	virtual BOOL OpMenuSave::SaveSpecificFile(Filter* pFilter, CCLexFile* pFile)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		pFilter - The filter to save with
				pFile	- The file to save to
	Returns:	TRUE if it worked, FALSE if there was an error
	Purpose:	Opens the file and calls the DoExport member of the filter, before closing
				the file again. If everything works as planned it returns TRUE

********************************************************************************************/

BOOL OpMenuSave::SaveSpecificFile(Filter* pFilter, CCLexFile* pFile)
{
	ERROR2IF(pFilter == NULL,FALSE,"OpMenuSave::SaveSpecificFile null filter specified");

	// First off, we have to try and open the file
	BOOL AllOK = TRUE;
//	TRY
	{
		// Tell the filter we would like a Preview Bitmap please
		if (pFilter->CanIncludePreviewBmp()) pFilter->IncludePreviewBmp(TRUE);

		// Try and export the file
		PathName pth = pFile->GetPathName();

		if (!pFilter->DoExport(this, pFile, &pth, Document::GetCurrent(), FALSE))
		{
			// Something went a bit wrong - tell the user what it was.
			// Supress the error if it was the 'user has cancelled one'
			if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
			{	
				// Clean up after ourselves
				InformError();
			}
			else
				Error::ClearError();	// otherwise remove the error so it won't get reported

			// Set the error
			AllOK = FALSE;
		}
	}
#if 0
	// See if there was a file io error
	CATCH(CFileException, e)
	{
		// Report the error if no one else did
		if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
		{
			InformError();
		}
		else
			Error::ClearError();

		// and fail
		return FALSE;
	}
	END_CATCH
#endif
	// Tell the filter we would NOT like a Preview Bitmap ready for next time
	if (pFilter->CanIncludePreviewBmp()) pFilter->IncludePreviewBmp(FALSE);

	// return
	return AllOK;
}



/********************************************************************************************

>	BOOL OpMenuSave::Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Registers the OpDescriptor for the Save Operation.

********************************************************************************************/

BOOL OpMenuSave::Init()
{
	BOOL InitOK = TRUE;

	// Register the save / save as... version
//	InitOK = RegisterOpDescriptor(
//									0, 							// Tool ID
//									_R(IDT_IMPORT),					// String resource ID
//									CC_RUNTIME_CLASS(OpMenuSave), 	// Runtime class
//									OPTOKEN_NATIVESAVE,			// Token string
//									GetState,					// GetState function
//									0,							// help ID
//									_R(IDBBL_FILESAVEOP),			// bubble help
//									_R(IDD_BARCONTROLSTORE),		// resource ID
//									_R(IDC_BTN_FILESAVE),			// control ID
//									SYSTEMBAR_FILE,				// Bar ID
//									TRUE,						// Recieve system messages
//									FALSE,						// Smart duplicate operation
//									TRUE,						// Clean operation
//									0,							// No vertical counterpart
//									0,							// String for one copy only error
//									(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
// 								);
//	if (!InitOK) return FALSE; 
	if (!RegisterOpDescriptor(0, _R(IDT_IMPORT), CC_RUNTIME_CLASS(OpMenuSave),
							 OPTOKEN_NATIVESAVE, GetState,
							 0, _R(IDBBL_IMPORTFILEOP), 0))
		return FALSE;

	// Register the Save As Default version
//	if (!RegisterOpDescriptor(0, _R(IDT_SAVEASDEFAULT), CC_RUNTIME_CLASS(OpMenuSave),
//							 OPTOKEN_SAVEASDEFAULT, GetState,
//							 0, _R(IDBBL_IMPORTFILEOP), 0))
//		return FALSE; 
	InitOK = RegisterOpDescriptor(
									0, 							// Tool ID
									_R(IDT_SAVEASDEFAULT),			// String resource ID
									CC_RUNTIME_CLASS(OpMenuSave), 	// Runtime class
									OPTOKEN_SAVEASDEFAULT,		// Token string
									GetState,					// GetState function
									0,							// help ID
									_R(IDBBL_FILESAVETEMPLATE),		// bubble help
									_R(IDD_BARCONTROLSTORE),		// resource ID
									_R(IDC_FILESAVETEMPLATE),		// control ID
									SYSTEMBAR_FILE,				// Bar ID
									TRUE,						// Recieve system messages
									FALSE,						// Smart duplicate operation
									TRUE,						// Clean operation
									0,							// No vertical counterpart
									0,							// String for one copy only error
									(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
 								);
	if (!InitOK) return FALSE; 

#ifdef NEW_NATIVE_FILTER
	// Only used by batching at present
	if (!OpSaveAsWeb::Init()) return FALSE;
#endif
	// Used by the new saving code
	if (!OpSaveAsNativeV2::Init()) return FALSE;
	if (!OpSaveAsNativeV1::Init()) return FALSE;

	// All worked
	return TRUE;
}




/********************************************************************************************

>	OpState OpMenuSave::GetState(String_256*, OpDescriptor*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	Another empty OpState function

********************************************************************************************/

OpState OpMenuSave::GetState( String_256* pUIDescription, OpDescriptor* pDesc )
{
	OpState OpSt;

	return OpSt;
}



/********************************************************************************************
/ ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/
********************************************************************************************/

/********************************************************************************************

>	virtual UINT32 OpSaveAsNativeV1::GetSearchFilterId()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		-
	Returns:	The id of the filter to search for.
	Purpose:	To allow different ops to just specify different filters and reuse
				the same operation code to save out the file in different file
				formats.

********************************************************************************************/

UINT32 OpSaveAsNativeV1::GetSearchFilterId()
{
	return FILTERID_NATIVE_EPS;
}



/********************************************************************************************

>	virtual BOOL OpSaveAsNativeV1::FixFileType()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if fixing is required, FALSE otherwise
	Purpose:	To determine whether this operation requires the file type or extension on
				the specified file path to be correct and match the	filter we are about to use.
				This baseclass version says no as the old native save operation did not do
				it.

********************************************************************************************/

BOOL OpSaveAsNativeV1::FixFileType()
{
	// Say we do want it in this baseclass version
	return TRUE;
}



/********************************************************************************************

>	BOOL OpSaveAsNativeV1::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Registers the OpDescriptor for the Save Operation.

********************************************************************************************/

BOOL OpSaveAsNativeV1::Init()
{
	BOOL InitOK = TRUE;

	// Register the save as web op desciptor
	InitOK = RegisterOpDescriptor(
									0, 							// Tool ID
									_R(IDS_SAVEASNATIVEV1),			// String resource ID
									CC_RUNTIME_CLASS(OpSaveAsNativeV1), 	// Runtime class
									OPTOKEN_SAVEASNATIVEV1,		// Token string
									GetState,					// GetState function
									0,							// help ID
									_R(IDBBL_SAVEASNATIVEV1),		// bubble help
									0, //_R(IDD_BARCONTROLSTORE),		// resource ID
									0, //_R(IDC_FILESAVETEMPLATE),		// control ID
									SYSTEMBAR_ILLEGAL,			// Bar ID
									TRUE,						// Recieve system messages
									FALSE,						// Smart duplicate operation
									TRUE,						// Clean operation
									0,							// No vertical counterpart
									0,							// String for one copy only error
									(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
 								);
	if (!InitOK) return FALSE; 

	// All worked
	return TRUE;
}



/********************************************************************************************

>	OpState OpSaveAsNativeV1::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Purpose:	The OpState function

********************************************************************************************/

OpState OpSaveAsNativeV1::GetState(String_256 * pUIDescription, OpDescriptor*)
{
	OpState OpSt;

	return OpSt;
}

#endif // DO_EXPORT
