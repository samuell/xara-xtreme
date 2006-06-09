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
// SGLibOil.cpp - Library OIL code

#include "camtypes.h"
#include "sgliboil.h"

//#include "app.h"		// For GetApplication() - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "galstr.h"
//#include "pathname.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "sginit.h"
//#include "sglcart.h"	// This .cpp file's corresponding header
//#include "sglib.h"
//#include "sgfonts.h"
//#include "sgscan.h"		// Generate code with hourglass...
//#include "sgscanf.h"		// Generate code with hourglass...
//#include "thumb.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ccdc.h"		// For render-into-dialogue support
//#include "fillval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "grnddib.h"
//#include "galres.h"
//#include "richard.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "oilfltrs.h"
//#include "tim.h"
//#include "nev.h"
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "selop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "filedlgs.h"
//#include "scrcamvw.h"	// PageDropInfo
//#include "resource.h"	// _R(IDS_CANCEL)
#include "dlgmgr.h"
//#include "bitmap.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "oilbitmap.h"
//#include "dibutil.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "reshlpid.h"
#include "progress.h"
#include "helpuser.h"
//#include "reshlpid.h"
//#include "sgindgen.h"
//#include "ctrlhelp.h"
//#include "richard2.h"
//#include "richard3.h"
#include "camelot.h"
#include "camdoc.h"
#include "unicdman.h"
//#include "fonts.h"
//#include "camfiltr.h"	// BaseCamelotFilter - version 2 native filter - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"	// TAG_DEFINEBITMAP_****
#include "cxfile.h"		// CXF_UNKNOWN_SIZE
//#include "cxfrech.h"	// CXaraFileRecordHandler - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "expcol.h"		// ExportedColours handling class
//#include "cxfrec.h"		// CXaraFileRecord handler - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "webster.h"
#include "pathnmex.h"
//#include "docview.h"		// For DocView::GetCentreImportPosition - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "backgrnd.h"	// OpBackground
#include "nodebmp.h"	// OpCreateBitmapNode
#include "product.h"	// PROGRAM_NAME macro

//#include <io.h>			// for FileExists
//#include <stdio.h>
//#include <commdlg.h>	// For GetLibPath
//#include <dlgs.h>
//#include <stdlib.h>
//#include <mmsystem.h>

//#include <sys\stat.h>	// FileModified

#if defined(__WXMSW__)
#include <shfolder.h>	// For SHGetFolderPath
#endif

extern void FixFPControlRegister(void);

// Implement the dynamic class bits...
PORTNOTE("dialog","Removed OpClipartImport")
#ifndef EXCLUDE_FROM_XARALX
CC_IMPLEMENT_DYNCREATE(OpClipartImport, SelOperation)
#endif
PORTNOTE("dialog","Removed OpBitmapImport")
#ifndef EXCLUDE_FROM_XARALX
CC_IMPLEMENT_DYNCREATE(OpBitmapImport, Operation)
#endif
CC_IMPLEMENT_DYNCREATE(SGLibOil, CCObject)

// This line mustn't go before any CC_IMPLEMENT_... macros
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

extern BOOL getDrive(const TCHAR**, TCHAR* drive);
extern BOOL getNetDrive(const TCHAR**, TCHAR* drive, BOOL *BadCharacter);

TCHAR *SGLibOil::TmpSndName = NULL;

// Define or undefine this to enable / disable the checking of network drive serial numbers
// for the inhouse 'FindCD' code... It's safe to leave in in release versions because it checks
// serial numbers of drives, which are never the same... The code's never called if they aren't
// connected to a network, or don't have their network mounts assigned to drive letters...
#define SCANFORXARAMOUNT

#if defined(__WXMSW__)
const TCHAR				chPathSep = _T('\\');
#else
const TCHAR				chPathSep = _T('/');
#endif

PORTNOTE("dialog","Removed OpClipartImport impl.")
#ifndef EXCLUDE_FROM_XARALX

// Constructor for ClipartImportParam,
// Makes sure the parameter defaults are sensible
ClipartImportParam::ClipartImportParam()
{
	File 		= NULL;
	Import 		= FALSE;
	Result 		= FALSE;
	DropInfo 	= NULL;
}

////////////////////////// OpClipartImport class ... /////////////////////////////////////////

/******************************************************************************************

>	OpClipartImport::OpClipartImport() : UndoableOperation()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpClipartImport constructor (Creates an undoable operation)
	SeeAlso:

******************************************************************************************/

OpClipartImport::OpClipartImport() : SelOperation()
{
	OpFlags.HasOwnTimeIndicator = TRUE; // The OpMenuImport op has its own time indicator
}       

/********************************************************************************************

>	OpClipartImport::~OpClipartImport() 

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpClipartImport destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpClipartImport::~OpClipartImport()
{
}

/******************************************************************************************

>	BOOL OpClipartImport::Init()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for an ClipartImport operation
	SeeAlso:

******************************************************************************************/

BOOL OpClipartImport::Init()
{  
	OpBitmapImport::Init();
	OpAsynchBitmapImport::Init();

	return RegisterOpDescriptor(
		0,								// Tool ID
 		_R(IDS_OPCLIPARTIMPORT),				// String resource ID
		CC_RUNTIME_CLASS(OpClipartImport),	// Runtime class
 		OPTOKEN_OPCLIPARTIMPORT,			// Token string
 		OpClipartImport::GetState,			// GetState function
		0,								// Help ID
		0,								// Bubble ID
		0,								// Resource ID
		0								// Control ID
//	needs	'GREY_WHEN_NO_CURRENT_DOC'
	);
}   

/******************************************************************************************

>	OpState	OpClipartImport::GetState(String_256* pString, OpDescriptor* pOpDesc)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Purpose:	Returns the OpState of the OpClipartImport operation.
	SeeAlso:

******************************************************************************************/

OpState	OpClipartImport::GetState(String_256* pString, OpDescriptor* pOpDesc)
{
	//OpState OpSt;
	OpState OpSt;

	return(OpSt);
}

/********************************************************************************************

>	void OpClipartImport::Do(OpDescriptor*)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		OpClipartImport (unused)
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		Always fails because this Do doesn't.
	SeeAlso:	-

********************************************************************************************/
			
void OpClipartImport::Do(OpDescriptor *NotUsed)
{
	ERROR3("OpClipartImport does not provide a Do() function - Use DoWithParam");
	End();
}

/******************************************************************************************
>	void OpClipartImport::DoWithParam(OpDescriptor* pOp, OpParam* pAlignParam)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com> (well, most of the code taken from tim's OpMenuImport)
	Created:	20/3/95
	Inputs:		pOp - OpDescriptor as for all Do() functions
				Param - points to a handy ClipartImport structure which says which
					file to import, etc...
	Outputs:	-
	Returns:	-
	Purpose:	Performs a clipart import / open depending on the parameters...
				Imports overlay the clipart on the existing document,
				Opens create a new page and import to that.

	SeeAlso:
******************************************************************************************/

void OpClipartImport::DoWithParam(OpDescriptor* pOp, OpParam* pClipartImportParam)
{
	ClipartImportParam *pInfo = (ClipartImportParam *) pClipartImportParam;

	if(pInfo == NULL)
	{
		ERROR3("OpClipartImport called with NULL info pointer");
		FailAndExecute(); 
		End();
		return;
	}
	else
	{
		if(pInfo->File == NULL)
		{
			ERROR3("OpClipartImport called with NULL file pointer");
			FailAndExecute(); 
			End();
			return;
		}
	}

	PathName Path = *(pInfo->File);

	if(!Path.IsValid())
	{
		ERROR3("Filename is invalid");
		//InformError(_R(IDT_IMPORT_NOTFOUND));
		FailAndExecute();
		End();
		return;
	}

	if(!pInfo->Import)
	{
		// Open / Load a file
		CCamApp		   *pApp = AfxGetApp();
		BaseFileDialog::SelectedFilter = 0;
		
		String_256 OpenPath((const TCHAR *)Path.GetPath());
//		Error::ClearError();

		// Open a document
		CCamDoc* pDoc = (CCamDoc*) pApp->OpenDocumentFile((TCHAR *)OpenPath);

		// Force the Ops 'Working Doc' to the selected one, in case the open
		// document thingy deleted the existing docs.
		if(pDoc != NULL) pOurDoc = Document::GetSelected();
		
		Document* pCurDoc = pDoc->GetKernelDoc();

		// And redraw the imported document
		//if(pCurDoc) pCurDoc->ForceRedraw();

	}
	else
	{
		// Can't import if no selected document (!)
		if(Document::GetSelected() == NULL)
		{
			ERROR3("No selected document");
			//InformError(_R(IDT_IMPORT_NOTFOUND));
			FailAndExecute();
			End();
			return;
		}
			
		// Import a file
		UINT32 SelFilter = FILTERID_GENERIC;
		BOOL LayersImport = FALSE;
		
		// Find the filter that the user chose
		Filter *pFilter = Filter::GetFirst();

		if(pFilter == NULL)
			return;

		while (pFilter != NULL)
		{
			if ((pFilter->GetFlags().CanImport) && (pFilter->FilterID == SelFilter))
				// This is the filter!
				break;

			// Try the next filter
			pFilter = Filter::GetNext(pFilter);
		}

		if (pFilter == NULL)
		{
			InformError(_R(IDT_CANT_FIND_FILTER));
			FailAndExecute(); 
			End();
			return;
		}
		else
		{
			// If this is not a filter family, check for compatibility before asking
			// filter to load the file.
			// This means the user has chosen an explicit filter to handle the import
			if (!pFilter->IS_KIND_OF(FilterFamily))
			{
				UINT32 Size = 1024;
				INT32 FileSize;
				ADDR FilterBuf = pFilter->LoadInitialSegment(Path, &Size, &FileSize);

				// If there has been a problem in the load initial segment then fail now.
				if (FilterBuf == NULL)
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
		
				// Change this to be less than 9 as the built in BMP filter returns 8 if it
				// is not too sure about the file and would like the AccusoftBMp filter to 
				// load the file if it is present.  
				if (pFilter->HowCompatible(Path, FilterBuf, Size, FileSize) < 9)
				{
					// Not 100% happy with this file - ask for confirmation.
					ErrorInfo Question;
					Question.ErrorMsg = _R(IDT_IMPQUERY_NOTSURE);
					Question.Button[0] = _R(IDB_IMPQUERY_IMPORT);
					Question.Button[1] = _R(IDB_IMPQUERY_DONTIMPORT);

					if (AskQuestion(&Question) != _R(IDB_IMPQUERY_IMPORT))
					{
						// User asked for this to be cancelled.
						TRACEUSER( "Richard", wxT("Filter compatibility was less than 10\n") );

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
				//Document* pCurDoc = Document::GetCurrent();
				Document* pCurDoc;
				ImportPosition Pos;

				if (pInfo->DropInfo)
				{
					// A specfic Import Position has been specified.
					// Get the document, spread, and position to import at.
				 	pCurDoc = pInfo->DropInfo->pDoc;

					Pos.Position 	= pInfo->DropInfo->DropPos;
					Pos.pSpread 	= pInfo->DropInfo->pSpread;
				}
				else
				{
					pCurDoc = Document::GetSelected();
				}

				if(pCurDoc == NULL)
				{
					ERROR3("OpClipartImport::DoWithParam retrieved NULL selected doc");
					return;
				}

				TRY
				{
					if (!DiskFile.open(Path, ios::in | ios::binary))
					{
						// Failed to open the file...
						TRACEUSER( "Richard", wxT("Failed to open file in ClipartImport Do") );
						InformError(_R(IDT_IMPORT_NOTFOUND));
						FailAndExecute();
						End();
						return;
					}

					// Import the file and see if Something went a bit wrong - tell the user what it was.
					ImportPosition* ImportPos;
					if (pInfo->DropInfo)
						ImportPos = &Pos;	// Import at a specific point.
					else
					{
						//Graham 15/7/97
						//Otherwise, put the imported object in the centre of the view
						//So get the current view
						DocView* pDocView=DocView::GetCurrent();

						//And find its centre import position
						Pos=pDocView->GetCentreImportPosition();

						ImportPos=&Pos;
					}



					if (!pFilter->DoImport(this, &DiskFile, pCurDoc, FALSE, ImportPos))
					{
						// Only tell them if not special user cancelled error message
						if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
							InformError();
						else
							Error::ClearError();	// otherwise remove the error so it won't get reported
						FailAndExecute();
						End();

						// close the file
						if (DiskFile.isOpen())
							DiskFile.close();

						return;
					}

					// close the file
					if (DiskFile.isOpen())
						DiskFile.close();

				}

				// See if there was a file io error
				CATCH(CFileException, e)
				{
					// Report the error if no one else did
					if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
						InformError();
					else
						Error::ClearError();	// otherwise remove the error so it won't get reported

					// Make sure that the file is closed
					TRY
					{
						if (DiskFile.isOpen())
							DiskFile.close();
					}
					CATCH(CFileException, e)
					{
						// Failed to close the file - not much we can do about it really
					}
					END_CATCH

					// and fail
					FailAndExecute();
					End();
					return;
				}
				END_CATCH
			}
		}
	}
		
	// grab the focus
	GetMainFrame()->SetActiveWindow();
	// Finished the operation
	End();
}






// Constructor for BitmapImportParam,
// Makes sure the parameter defaults are sensible
BitmapImportParam::BitmapImportParam()
{
	File 			= NULL;
	Result 			= FALSE;

	pTargetDoc		= 0;
	TagObjectToFill	= 0;
	TagObjectValid	= false;
	pObject			= 0;
	pObjectValid	= false;
}

////////////////////////// OpBitmapImport class ... /////////////////////////////////////////

/******************************************************************************************

>	OpBitmapImport::OpBitmapImport() : UndoableOperation()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpBitmapImport constructor (Creates an undoable operation)
	SeeAlso:

******************************************************************************************/

OpBitmapImport::OpBitmapImport() : Operation()
{
	OpFlags.HasOwnTimeIndicator = TRUE; // The OpMenuImport op has its own time indicator
}       

/********************************************************************************************

>	OpBitmapImport::~OpBitmapImport() 

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpBitmapImport destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpBitmapImport::~OpBitmapImport()
{
}

/******************************************************************************************

>	BOOL OpAlign::Init()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for an BitmapImport operation
	SeeAlso:

******************************************************************************************/

BOOL OpBitmapImport::Init()
{
	return RegisterOpDescriptor(
		0,								// Tool ID
 		_R(IDS_OPCLIPARTIMPORT),				// String resource ID
		CC_RUNTIME_CLASS(OpBitmapImport),	// Runtime class
 		OPTOKEN_OPBITMAPIMPORT,			// Token string
 		OpBitmapImport::GetState,			// GetState function
		0,								// Help ID
		0,								// Bubble ID
		0,								// Resource ID
		0								// Control ID
//	needs	'GREY_WHEN_NO_CURRENT_DOC'
	);
}   

/******************************************************************************************

>	OpState	OpBitmapImport::GetState(String_256* pString, OpDescriptor* pOpDesc)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Purpose:	Returns the OpState of the OpBitmapImport operation.
	SeeAlso:

******************************************************************************************/

OpState	OpBitmapImport::GetState(String_256* pString, OpDescriptor* pOpDesc)
{
	//OpState OpSt;
	OpState OpSt;

	return(OpSt);
}

/********************************************************************************************

>	void OpBitmapImport::Do(OpDescriptor*)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		OpBitmapImport (unused)
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		Always fails because this Do doesn't.
	SeeAlso:	-

********************************************************************************************/
			
void OpBitmapImport::Do(OpDescriptor *NotUsed)
{
	ERROR3("OpBitmapImport does not provide a Do() function - Use DoWithParam");
	End();
}

/******************************************************************************************
>	void OpBitmapImport::DoWithParam(OpDescriptor* pOp, OpParam* pAlignParam)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/95
	Inputs:		pOp - OpDescriptor as for all Do() functions
				Param - points to a handy BitmapImport structure which says which
					file to import, etc...
	Outputs:	-
	Returns:	-
	Purpose:	Performs a Bitmap import.
				Returns a pointer to the imported bitmap, without creating any objects.

	SeeAlso:
******************************************************************************************/

void OpBitmapImport::DoWithParam(OpDescriptor* pOp, OpParam* pBitmapImportParam)
{
	BitmapImportParam *pInfo = (BitmapImportParam *) pBitmapImportParam;
	KernelBitmap* pBitmap = 0;

	if(pInfo == NULL)
	{
		ERROR3("OpBitmapImport called with NULL info pointer");
		FailAndExecute(); 
		End();
		return;
	}
	else
	{
		if(pInfo->File == NULL)
		{
			ERROR3("OpBitmapImport called with NULL file pointer");
			FailAndExecute(); 
			End();
			return;
		}
	}

	PathName Path = *(pInfo->File);

	if(!Path.IsValid())
	{
		ERROR3("Filename is invalid");
		//InformError(_R(IDT_IMPORT_NOTFOUND));
		FailAndExecute();
		End();
		return;
	}

	// Import a file
	UINT32 SelFilter = FILTERID_GENERIC;
	BOOL LayersImport = FALSE;
	
	// Find the filter that the user chose
	Filter *pFilter = Filter::GetFirst();

	if(pFilter == NULL)
		return;

	while (pFilter != NULL)
	{
		if ((pFilter->GetFlags().CanImport) && (pFilter->FilterID == SelFilter))
			// This is the filter!
			break;

		// Try the next filter
		pFilter = Filter::GetNext(pFilter);
	}

	if (pFilter == NULL)
	{
		InformError(_R(IDT_CANT_FIND_FILTER));
		FailAndExecute(); 
		End();
		return;
	}
	else
	{
		// If this is not a filter family, check for compatibility before asking
		// filter to load the file.
		// This means the user has chosen an explicit filter to handle the import
		if (!pFilter->IS_KIND_OF(FilterFamily))
		{
			UINT32 Size = 1024;
			INT32 FileSize;
			ADDR FilterBuf = pFilter->LoadInitialSegment(Path, &Size, &FileSize);

			// If there has been a problem in the load initial segment then fail now.
			if (FilterBuf == NULL)
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
	
			// Change this to be less than 9 as the built in BMP filter returns 8 if it
			// is not too sure about the file and would like the AccusoftBMp filter to 
			// load the file if it is present.  
			if (pFilter->HowCompatible(Path, FilterBuf, Size, FileSize) < 9)
			{
				// Not 100% happy with this file - ask for confirmation.
				ErrorInfo Question;
				Question.ErrorMsg = _R(IDT_IMPQUERY_NOTSURE);
				Question.Button[0] = _R(IDB_IMPQUERY_IMPORT);
				Question.Button[1] = _R(IDB_IMPQUERY_DONTIMPORT);

				if (AskQuestion(&Question) != _R(IDB_IMPQUERY_IMPORT))
				{
					// User asked for this to be cancelled.
					TRACEUSER( "Richard", wxT("Filter compatibility was less than 10\n") );

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

		// First off, we have to try and open the file
		CCDiskFile DiskFile(1024, FALSE, TRUE);

		TRY
		{
			if (!DiskFile.open(Path, ios::in | ios::binary))
			{
				// Failed to open the file...
				TRACEUSER( "Richard", wxT("Failed to open file in BitmapImport Do") );
				InformError(_R(IDT_IMPORT_NOTFOUND));
				FailAndExecute();
				End();
				return;
			}

			// Import the file and see if Something went a bit wrong - tell the user what it was.
			if (!pFilter->ImportBitmap(&DiskFile, &pBitmap))
			{
				// Only tell them if not special user cancelled error message
				if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
					InformError();
				else
					Error::ClearError();	// otherwise remove the error so it won't get reported
				FailAndExecute();
				End();

				return;
			}

			// close the file
			if (DiskFile.isOpen())
				DiskFile.close();
		}

		// See if there was a file io error
		CATCH(CFileException, e)
		{
			// Report the error if no one else did
			if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
				InformError();
			else
				Error::ClearError();	// otherwise remove the error so it won't get reported

			// Make sure that the file is closed
			TRY
			{
				if (DiskFile.isOpen())
					DiskFile.close();
			}
			CATCH(CFileException, e)
			{
				// Failed to close the file - not much we can do about it really
			}
			END_CATCH

			// and fail
			FailAndExecute();
			End();
			return;
		}
		END_CATCH
	}

	// Should really check the return value of ApplyFill(...)
	ApplyFill(pInfo, pBitmap);

	// grab the focus
	GetMainFrame()->SetActiveWindow();

	// Finished the operation
	End();
}

bool OpBitmapImport::ApplyFill(BitmapImportParam *pInfo, KernelBitmap* pBitmap)
{
	// Check the document pointer points to the current document
	if (Document::GetCurrent() != pInfo->pTargetDoc || pInfo->pTargetDoc == 0)
	{
		// At the moment we cannot apply a fill to an object that is not in
		// the current document (the operations used in this function appear to
		// add the object to one documents tree and render the object on the other
		// document).
		ERROR3("Cannot apply fill when active document has changed");
		return false;
	}

	// Check the spread is valid
	if (!Document::SpreadBelongsToDoc(pInfo->pTargetDoc, pInfo->pSpread))
	{
		ERROR3("Trying to import a bitmap fill into a non-existent spread");
		return false;
	}

	// Everything looks OK, lets do the fill
	switch(pInfo->FillType)
	{
		// If you need to change ApplyToObject, you may need to change ApplyToObjectAsTrans
		case ApplyToObject:
		{
			NodeRenderableInk* pObjectToUse;
			if (pInfo->TagObjectValid)
			{
				Node *pTreeRoot = pInfo->pTargetDoc->GetFirstNode()->FindFirstDepthFirst();
				if (pTreeRoot) pTreeRoot = pTreeRoot->FindNext();
				Node *pNode = pTreeRoot->FindFirstDepthFirst();
				while ((pNode != NULL) && (pNode->GetTag() != pInfo->TagObjectToFill))
					pNode = pNode->FindNextDepthFirst(pTreeRoot);

				if (pNode == 0)
					return false;

				if (pNode->IsAnObject())
					pObjectToUse = static_cast<NodeRenderableInk *>(pNode);	// should use dynamic_cast but no RTTI available
				else
				{
					ERROR3("It has the tag we saved but its type has changed.  This is odd");
					return false;
				}
			}

			AttrBitmapColourFill* Attrib = new AttrBitmapColourFill;
			if (Attrib == 0)
				return false;

			Attrib->AttachBitmap(pBitmap);

			if (pInfo->TagObjectValid)
				AttributeManager::ApplyAttribToNode(pObjectToUse, Attrib);
			else if (pInfo->pObjectValid)
				AttributeManager::ApplyAttribToNode(pInfo->pObject, Attrib);
			else
				AttributeManager::AttributeSelected(0, Attrib);

			return true;
		}
		// If you need to change ApplyToObjectAsTransp, you may need to change ApplyToObject
		case ApplyToObjectAsTransp:
		{
			NodeRenderableInk* pObjectToUse;

			if (pInfo->TagObjectValid)
			{
				Node *pTreeRoot = pInfo->pTargetDoc->GetFirstNode()->FindFirstDepthFirst();
				if (pTreeRoot) pTreeRoot = pTreeRoot->FindNext();
				Node *pNode = pTreeRoot->FindFirstDepthFirst();
				while ((pNode != NULL) && (pNode->GetTag() != pInfo->TagObjectToFill))
					pNode = pNode->FindNextDepthFirst(pTreeRoot);

				if (pNode == 0)
					return false;

				if (pNode->IsAnObject())
					pObjectToUse = static_cast<NodeRenderableInk *>(pNode);	// should use dynamic_cast but no RTTI available
				else
				{
					ERROR3("It has the tag we saved but its type has changed.  This is odd");
					return false;
				}
			}

			pBitmap = NodeBitmap::CheckGreyscaleBitmap(pBitmap, _R(IDS_BMPGAL_MAKEGREY), _R(IDS_BMPGAL_DOGREY));

			AttributeManager::HaveAskedAboutContoneColours	= FALSE;
			AttributeManager::UserCancelledContoneColours	= FALSE;

			if (pBitmap == 0)
				return false;

			// Fill colour selected so create a fill colour attribute
			NodeAttribute *Attrib = new AttrBitmapTranspFill;
			if (Attrib == 0)
				return false;

			(static_cast<AttrBitmapTranspFill *>(Attrib))->AttachBitmap(pBitmap);

			if (pInfo->TagObjectValid)
				AttributeManager::ApplyAttribToNode(pObjectToUse, Attrib);
			else if (pInfo->pObjectValid)
				AttributeManager::ApplyAttribToNode(pInfo->pObject, Attrib);
			else
				AttributeManager::AttributeSelected(0, Attrib);

			return true;
		}
		case SetBackground:
		{
			OpBackgroundParam Param;
			Param.pBitmap	= pBitmap;
			Param.pDoc		= pInfo->pTargetDoc;			
			Param.pSpread	= pInfo->pSpread;
			
			// Obtain a pointer to the op descriptor for the create operation 
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BACKGROUND);

			// Invoke the operation, passing in our parameters
			pOpDesc->Invoke(&Param);
			return true;
		}
		case AddToPage:
		{
			PageDropInfo DropInfo;

			DropInfo.pDoc		= pInfo->pTargetDoc;
			DropInfo.pSpread	= pInfo->pSpread;
			DropInfo.DropPos	= pInfo->DropPos;

			// Obtain a pointer to the op descriptor for the create operation 
			OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpCreateNodeBitmap));

			// Invoke the operation, passing DocView and Pos as parameters
			OpDesc->Invoke(&OpParam((INT32)pBitmap,(INT32)&DropInfo));
			return true;
		}
		default:
		{
			ERROR3("Unknown fill type");
			return false;
		}
	}
}

#endif // wxCamelot port

/***********************************************************************************************
>	static BOOL SGLibOil::TidyUp(void)

	Purpose: Called by camelot de-init routines
***********************************************************************************************/

BOOL SGLibOil::TidyUp(void)
{
	if(TmpSndName != NULL)
	{
		PathName TheFile(TmpSndName);
		FileDelete(&TheFile);
		TmpSndName = NULL;
	}
	return TRUE;
}

/***********************************************************************************************

>	static BOOL SGLibOil::FileCopy(PathName *Source, PathName *Destination)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Inputs:		Source - Source path to grab file from
				Destination - Destination path to copy file to
	Returns:	Returns true if all was hunky dorey
	Purpose:	Copies a file from source to destination.
	Notes:		This function was deemed necessary as actually doing this sort of
				thing in windows 3.1 is completely nasty, and completely incompatible
				with NT's way...
		  
***********************************************************************************************/

BOOL SGLibOil::FileCopy(PathName *Source, PathName *Destination)
{
	PORTNOTETRACE("dialog","SGLibOil::FileCopy - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if(Source == NULL || Destination == NULL)
	{
		ERROR3("SGLibOil::FileCopy Source or Destination files should not be null");
		return FALSE;
	}

	if(!Destination->IsValid() || !Source->IsValid())
	{
		ERROR3("SGLibOil::FileCopy given invalid path");
		return FALSE;
	}

	if(FileExists(Destination))
	{
		ERROR3("SGLibOil::FileCopy destination file already exists");
		return FALSE;
	}

	if(!FileExists(Source))
	{
		ERROR3("SGLibOil::FileCopy source file doesn't exist");
		return FALSE;
	}

// *************************

//	return CopyFile((TCHAR *)Source->GetPath(), (TCHAR *)Destination->GetPath(), FALSE);

// *************************

	INT32 Size = SGLibOil::FileSize(Source);
	void *Buffer = CCMalloc(Size + 4);

	if(Buffer != NULL)
	{
		CCDiskFile File(1024, FALSE, TRUE);
 
		// Open file and check if it exists at the same time
		try
		{
			// Read file into the buffer
			File.open( Source->GetPath(), ios::in | ios::binary );
			File.read( Buffer, Size );
			if(File.isOpen())
				File.close();

			// Write file from buffer into output file
			File.open(Destination->GetPath(), ios::out | ios::binary);
			File.write(Buffer, Size);
			if(File.isOpen())
				File.close();

			CCFree(Buffer);

			// File copied ok...
			return TRUE;
		}		
		catch( CFileException )
		{
			// File not copied ok... Clear the errors and fall though to alternative low-
			// memory file copy... (this should be very rare !)
			Error::ClearError();
		}

		CCFree(Buffer);
	}

// *************************

	// Not enough memory available, do the copy the old, slow, byte by byte (ANSI) way...

	FILE			   *FPin  = NULL;
	FILE			   *FPout = NULL;

#if defined(__WXMSW__)
	FPin  = _tfopen((const TCHAR *)Source->GetPath(), "rb");
	FPout = _tfopen((const TCHAR *)Destination->GetPath(), "wb");
#else
	PSTR				pszSrc, pszDst;
	{
		size_t			cchTmp = camWcstombs( NULL, Source->GetPath(), 0 ) + 1;
		pszSrc = PSTR( alloca( cchTmp ) );
		camWcstombs( pszSrc, Source->GetPath(), cchTmp );
	}
	{
		size_t			cchTmp = camWcstombs( NULL, Destination->GetPath(), 0 ) + 1;
		pszDst = PSTR( alloca( cchTmp ) );
		camWcstombs( pszDst, Destination->GetPath(), cchTmp );
	}

	FPin  = fopen( pszSrc, "rb" );
	FPout = fopen( pszDst, "wb" );
#endif
	
	if(FPin != NULL || FPout != NULL)
	{
		TCHAR C;
		while(!feof(FPin))
		{
			C = _fgettc(FPin);
			if(!feof(FPin))	_fputtc(C, FPout);
		}						
		fclose(FPin);
		fclose(FPout);
		return TRUE;
	}

	if(FPin != NULL) fclose(FPin);
	if(FPout != NULL) fclose(FPout);
#endif
	return FALSE;
}

/***********************************************************************************************

>	static BOOL SGLibOil::FileExists(PathName *FileName)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/3/95
	Inputs:		File - Pointer to path and filename of file
	Returns:	Returns true if the specified file exists
	Purpose:	To find out whether a file actually exists.
	Notes:

***********************************************************************************************/

BOOL SGLibOil::FileExists(PathName *FileName)
{
	return wxFile::Exists( PCTSTR(FileName->GetPath()) );

#if 0
	CCDiskFile File;
	BOOL OldTEState = File.SetThrowExceptions(FALSE);
	BOOL OldREState = File.SetReportErrors(FALSE);
	BOOL Found = FALSE;
	 
	// Open file - returns false if there was an error (file doesn't exist)
	Found = File.open(FileName->GetPath(), ios::in | ios::binary | ios::nocreate);
	if(Found) File.close();
		
	File.SetThrowExceptions(OldTEState);
	File.SetReportErrors(OldREState);
 	Error::ClearError();

	return Found;
#endif
}

/***********************************************************************************************

>	static BOOL SGLibOil::DirExists(PathName *FileName)

	Author:		Luke Hart (Xara Group Ltd) <lukeh@xara.com>
	Created:	21/05/06
	Inputs:		File - Pointer to path and filename of dir
	Returns:	Returns true if the specified dir exists
	Purpose:	To find out whether a dir actually exists.
	Notes:

***********************************************************************************************/

BOOL SGLibOil::DirExists( const PathName& FileName )
{
	return wxDir::Exists( PCTSTR(FileName.GetPath()) );

#if 0
	CCDiskFile File;
	BOOL OldTEState = File.SetThrowExceptions(FALSE);
	BOOL OldREState = File.SetReportErrors(FALSE);
	BOOL Found = FALSE;
	 
	// Open file - returns false if there was an error (file doesn't exist)
	Found = File.open(FileName->GetPath(), ios::in | ios::binary | ios::nocreate);
	if(Found) File.close();
		
	File.SetThrowExceptions(OldTEState);
	File.SetReportErrors(OldREState);
 	Error::ClearError();

	return Found;
#endif
}

/***********************************************************************************************

>	static INT32 SGLibOil::FileSize(PathName *FileName)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Inputs:		File - Pointer to path and filename of file
	Returns:	Returns true if the specified file exists
	Purpose:	To find out whether a file actually exists.
	Notes:

***********************************************************************************************/

INT32 SGLibOil::FileSize(PathName *FileName)
{
	PORTNOTETRACE("dialog","SGLibOil::FileSize - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(FileName == NULL, "SGLibOil::FileSize Filename should not be null");
	ERROR3IF(!FileName->IsValid(), "SGLibOil::FileSize given invalid path");

	INT32 FileSize = 0;

	// Unfortunately MFC seems to have a nasty bug which is less than useful
	// WRT reading file sizes of old files... We resort to good old c.
	FILE *FP = NULL;
	FP = _tfopen((const TCHAR *)FileName->GetPath(), "rb");
	if(FP != NULL)
	{
		fseek(FP,0,SEEK_END);
		FileSize = ftell(FP);
		fclose(FP);
	}

	return FileSize;
#else	
	return 1;
#endif
}

/***********************************************************************************************

>	static BOOL SGLibOil::FileDelete(PathName *FileName)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/95
	Inputs:		File - Pointer to path and filename of file
	Returns:	Returns true if the specified file was deleted ok
	Purpose:	Delete a file - scarry !
	Notes:

***********************************************************************************************/

BOOL SGLibOil::FileDelete(PathName *FileName)
{
	PORTNOTETRACE("dialog","SGLibOil::FileDelete - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(FileName == NULL || !FileName->IsValid(), "SGLibOil::FileDelete given a dodgy FileName");

	// Check if it exists, if so, kill it...
	if( _taccess( (const TCHAR *)FileName->GetPath(), ios::in ) == 0 )
	{
//		RemoveFile((TCHAR *)FileName->GetPath(), cmoVital | cmoForce);
		if(_tremove((const TCHAR *)FileName->GetPath()) == 0)
			return TRUE;
	}
#endif
	return FALSE;
}
/***********************************************************************************************

>	static UINT32 SGLibOil::FileModified(PathName *FileName);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		FileName - Pointer to path and filename of file
	Returns:	Returns a numeric value relating the last time the file was modified.
				Old files will always return a lesser number than new ones.
	Purpose:	Return numeric form of last modified time / date... for a file

***********************************************************************************************/

UINT32 SGLibOil::FileModified(PathName *FileName)
{
	PORTNOTETRACE("dialog","SGLibOil::FileModified - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(FileName == NULL, "SGLibOil::FileModified given a NULL FileName");
	ERROR3IF(!FileName->IsValid(), "SGLibOil::FileModified given invalid path");

	struct _tstat buf;

	INT32 Result = _tstat((const TCHAR *)FileName->GetPath(), &buf);

	if(Result != 0)
		return 0;

	// some files seem to have dates of -1, which is a long time in the future, or basically
	// random... We assume they haven't been modified recently...
	if(buf.st_mtime == -1)
		return 0;

	return (UINT32)buf.st_mtime;
#else	
	return 1;
#endif
}

/***********************************************************************************************

>	static BOOL SGLibOil::LocateCDROMDrive(LibraryGallery *LG, SGLibType LibType, String_256 *DriveName,
									BOOL *OnXaraNet, String_256 *XaraMount, BOOL AdjustPressed = FALSE);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		LG		- parent library gallery for function
				LibType - the type of library we're looking for
				AdjustPressed - Is 'adjust' held down ?
	Outputs:	DriveName will return a string such as "D:\" for the last valid CD drive it
				found with any of the magic directories present.

				OnXaraNet will return TRUE if we found a camelot CD mount was point
				XaraMount will return the path of the dummy CD -> "g:\CD\"

	Returns:	Returns true if a CD Drive or Xara mount was found
	Purpose:	Looks for the Xara CD. At present we go through all their CDROM drives checking
				for the existance of certain directories on the CDs... We also check the various
				net points for the Xara CD test library...

	Notes:		The internal network drive matching code uses the serial number of the
				drive itself. If we change the Xara CamelotCD mount point we'll need to
				change the serial number stuff below...

***********************************************************************************************/

BOOL SGLibOil::LocateCDROMDrive(LibraryGallery *LG, SGLibType LibType, String_256 *DriveName,
									BOOL *OnXaraNet, String_256 *XaraMount, BOOL AdjustPressed)
{
#if !defined(__WXMSW__)
	TRACE( _T("Warning - SGLibOil::FileModified called") );
	return FALSE;
#else
	if(!LG || !DriveName || !OnXaraNet || !XaraMount)
	{
		ERROR3("SGLibOil::LocateCDROMDrive given null input stuff");
		return FALSE;
	}

	BOOL FoundCD = FALSE;
	BOOL FoundXara = FALSE;
	BOOL FoundProgramMount = FALSE;

	*OnXaraNet = FALSE;

	DWORD dwTemp;	
	const DWORD DriveStringBufferSize = 20480;
	TCHAR DriveStringBuffer[DriveStringBufferSize];
	TCHAR *lpszDriveStringBuffer = (TCHAR *)DriveStringBuffer;
	TCHAR RootPathName[] = { '?', ':', chPathSep, 0 };

	String_64 SlowJob(_R(IDS_LIBRARIES_FINDING_CD));
	BeginSlowJob(('z' - 'a'), FALSE, &SlowJob);

	DWORD dwDriveMask=GetLogicalDrives();
	DWORD dwRC=GetLogicalDriveStrings(DriveStringBufferSize, lpszDriveStringBuffer);

	//ERROR3IF(dwRC == 0, "SGLibOil::LocateCDROMDrive - RC == 0 (no worries, we can default to plan b !)");
	ERROR3IF(dwDriveMask == 0, "SGLibOil::LocateCDROMDrive - DriveMask == 0 - this means you haven't got any drives at all on your system !!!");

	if(dwRC == 0)
	{
		memset(DriveStringBuffer, 0, DriveStringBufferSize);
		INT32 i = 0;
		for(*RootPathName='a'; *RootPathName<='z'; (*RootPathName)++)
		{
			DriveStringBuffer[i] = *RootPathName;
			DriveStringBuffer[i + 1] = ':';
			DriveStringBuffer[i + 2] = chPathSep;
			DriveStringBuffer[i + 3] = 0;
			i += 4;				
		}
		
		dwRC = i;
		dwDriveMask = 0xffffffff;
	}

	if(dwRC != 0 && dwDriveMask != 0)
	{
		// Loop through all the drives
		for(*RootPathName='a'; *RootPathName<='z'; (*RootPathName)++)
		{
			if(!ContinueSlowJob(*RootPathName - 'a'))
			{
				*RootPathName = 'z';
			}
			else
			{
			    dwTemp=dwDriveMask & 1;
			    dwDriveMask >>= 1;
			    if (dwTemp)
				{
					// finds out what sort of drive this is
					dwRC=GetDriveType(RootPathName);

					switch (dwRC)
			        {
			        	case DRIVE_REMOTE:
#ifdef SCANFORXARAMOUNT
							{
								TRACEUSER( "Richard", wxT("The drive type for %s is remote\n"), lpszDriveStringBuffer);

								// See if we're on the Xara Net or not...
								String_256 FileSystemName;
								String_256 VolumeName;
								DWORD SerialNumber = 0;
								BOOL bRC;
					
								bRC=GetVolumeInformation(RootPathName,
			                    		(TCHAR *)VolumeName,
			                    		255,
			                    		&SerialNumber,
			                    		NULL,
			                    		NULL,
			                    		(TCHAR *)FileSystemName,
			                    		255);
						
								if(bRC)
								{
									TRACEUSER( "Richard", wxT("%d - %s - %s\n"), SerialNumber, (TCHAR *)VolumeName, (TCHAR *)FileSystemName);						

									BOOL FoundAPossibleMount = FALSE;

									// Programs mount
									if(    (SerialNumber 	== (DWORD)0x589c1e6f)
										&& (VolumeName   	== (String_256)"")
										&& (FileSystemName 	== (String_256)"NTFS") )
									{
										FoundProgramMount = TRUE;
									}

									if(!AdjustPressed)
									{
										// \\jimpc\corelxra
										if(	   (SerialNumber 	== 0x6863b85e)
											&& (VolumeName   	== (String_256)"CORELXARA")
											&& (FileSystemName 	== (String_256)"CDFS") )
										{
											FoundAPossibleMount = TRUE;
										}
									}

									// \\earth\materials
									if(AdjustPressed)
									{
										if(	   (SerialNumber 	== 0xec29b7f6)
											&& (VolumeName   	== (String_256)"")
											&& (FileSystemName 	== (String_256)"NTFS") )
										{
											FoundAPossibleMount = TRUE;
										}
									}

									if(FoundAPossibleMount)
									{
										// OK, but are we the proper drive ?
										String_256 CDPath(lpszDriveStringBuffer);
										String_256 CheckPath(lpszDriveStringBuffer);
									
										if(AdjustPressed)
										{
											switch(LibType)
											{
												case SGLib_ClipArt:
												case SGLib_ClipArt_WebThemes:
													CheckPath += TEXT("Graphics\\XaraSt~1");
													break;
												case SGLib_Font:
													CheckPath += TEXT("Fonts");
													break;
												case SGLib_Texture:
													CheckPath += TEXT("Graphics\\Fills");
													break;
												default:
													CheckPath += TEXT("Fills\\Fabric");
													break;
											}
										}
										else
											CheckPath += TEXT("Fills\\Fabric");

										if(_taccess((TCHAR *)CheckPath, ios::in ) == 0)
										{
											String_256 Drive(CDPath);
											SGLibOil::AppendSlashIfNotPresent(&Drive);
											*XaraMount = Drive;
											*OnXaraNet = TRUE;
											FoundXara = TRUE;
										}
									}
								}
								else
								{
									DWORD Problem = GetLastError();
									TRACEUSER( "Richard", wxT("GetVolumeInformation returned %d error"), Problem);
								}
							}
#endif
				            break;

			        	case DRIVE_CDROM:
							// Since autochangers seem to take a seemingly endless amount of time to swap through
							// each disk, so I've added this check to see if we've already found the CD or not...
							if(!FoundCD)
							{
								String_256 CDPath(lpszDriveStringBuffer);
								String_256 LibDirName;							
PORTNOTE("dialog","Removed LibraryGallery usage")
#ifndef EXCLUDE_FROM_XARALX
								LG->GetLibraryDirectoryName(&LibDirName);
#endif
								CDPath += LibDirName;

								if( 0 == _taccess( (TCHAR *)CDPath, ios::in ) )
								{				
									FoundCD = TRUE;
									*DriveName = lpszDriveStringBuffer;
								}

								// Bodge for Viewer CD, which contains a hidden Fills directory
								// If this ever moves, the below code will need to be changed
								if(!FoundCD && LibType == SGLib_Texture)
								{
									CDPath = (String_256)lpszDriveStringBuffer;
									CDPath += TEXT("Clipart\\Clipart\\");
									CDPath += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
									CDPath += TEXT("\\");
									CDPath += String_16(_R(IDS_LIBRARIES_FILLS_DIRNAME));
									if(_taccess((TCHAR *)CDPath, ios::in ) == 0)
									{				
										FoundCD = TRUE;
										*DriveName = lpszDriveStringBuffer;
										*DriveName += TEXT("Clipart\\Clipart\\");
										*DriveName += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
										*DriveName += TEXT("\\");
									}									
								}
							}
			            	break;

				        case DRIVE_REMOVABLE:
							TRACEUSER( "Richard", wxT("The drive type for %s is removable\n"), lpszDriveStringBuffer);
			            	break;
	
				        case DRIVE_FIXED:
							TRACEUSER( "Richard", wxT("The drive type for %s is fixed\n"), lpszDriveStringBuffer);
							// Maybe we should be recursing through their hard drive looking for XaraInfo
							// directories ? v2 probably...
			            	break;

				        case DRIVE_RAMDISK:
							TRACEUSER( "Richard", wxT("The drive type for %s is ramdisk\n"), lpszDriveStringBuffer);
			            	break;

				        default:
				            break;
			    	}

					// Advance to next valid drive in system
					lpszDriveStringBuffer = lpszDriveStringBuffer + camStrlen(lpszDriveStringBuffer) + 1;
			    }
			}
		}
	}

	EndSlowJob();

#ifdef _DEBUG
	if(!FoundCD && !FoundXara && FoundProgramMount)
	{
		String_256 XaraMsg;
		XaraMsg = "You're connected to the Xara Net, but not the CamelotCD mount, so can't 'find' the library. ";
		ERROR3(XaraMsg);
	}
#endif

	return (FoundCD || FoundXara);
#endif
}

/***********************************************************************************************

>	UINT32 CALLBACK SGLibOil::FileHook(HWND hDlg, UINT32 Msg, UINT32 wParam, INT32 lParam)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/4/95

	Inputs:		hDlg - HWND of the dialog box.
 				iMsg - UINT32 message number.
 				wParam - UINT32 parameter.
				lParam - INT32 parameter.
 
	Returns:	Non-Zero or Zero, indicating if the common dialog should
                SKIP the message (non-zero) or not (zero).  Don't confuse
                with typical dialog box return values.
 
	Purpose:	Processes messages from the common File Open dialog.
				Used to kill the get path dbox when OK is clicked.					
				Used for handling 'Generate' clicks.
	Notes:		

***********************************************************************************************/

UINT32 CALLBACK SGLibOil::FileHook( wxWindow *pDlg, UINT32 Msg, UINT32 wParam, INT32 lParam )
{
	PORTNOTETRACE("dialog","SGLibOil::FileHook - do nothing - window CB");
#ifndef EXCLUDE_FROM_XARALX
	switch(Msg)
	{
	 	case WM_INITDIALOG:
			{			
#ifdef STANDALONE
			// Remove the Help button on standalone builds...
			HWND hGadget = GetDlgItem((HWND)hDlg, (INT32)_R(IDC_LIBPATH_HELP_BUTTON));
			ShowWindow(hGadget, SW_HIDE);
#endif
		
			// We need to apply the correct font if we are in DBCS land
			if (UnicodeManager::IsDBCSOS())
				FontFactory::ApplyFontToWindow(hDlg, STOCKFONT_DIALOG);
			}
			return TRUE;

	 	case WM_COMMAND:

			switch(wParam)
			{
				case _R(IDC_LIBPATH_HELP_BUTTON):
					// Special check for the help button, and a bodgy way of invoking help for
					// this dialog, which is a generic CFileDialog type with a custom template,
					// and thus has no unique class-name.
					_HelpUser(TEXT("SGalleryAddFolderDlg"));
					return TRUE;

				case IDOK:
					// Close the dbox when Add pressed
					PostMessage(GetParent(hDlg), WM_COMMAND, IDOK, (INT32)FALSE);
					return FALSE;

				case _R(IDC_LIBPATH_GENERATE):
					// Since there's no way of getting the full pathname here, we simulate
					// a click on 'OK' to close the dbox, and set a flag saying we've clicked
					// 'Generate' instead, so the code after the DoModal can get the proper
					// path, generate the index, and re-open the dbox. Talk about a bodge !			
					SGLibOil::GenerateButtonClicked = TRUE;
					PostMessage(hDlg, WM_COMMAND, IDOK, (INT32)FALSE);
					return FALSE;

				case _R(IDC_LIBPATH_SEARCH):
					// Scan (Search) for some clipart
					{
						if(!SGLibOil::ParentLibraryGallery->ScanForLocation(SGLibOil::LibType,
																			&SGLibOil::SearchDirectory))
						{
							// Keep the dialog open - don't change the path
							INT32 ButtonPressed = InformWarning(_R(IDS_LIBRARY_BROWSING_CANT_FIND_LIBRARY), _R(IDS_OK));
							Error::ClearError();
							return FALSE;
						}

						DirPath Path(SGLibOil::SearchDirectory);
						if(!Path.IsValid())
						{
							ERROR3("Scanned to path is invalid");
							// Keep the dialog open - don't change the path
							return FALSE;
						}

						// Remember the pathname and which button was clicked so we can update the dialog
						SGLibOil::SearchButtonClicked = TRUE;

						PostMessage(hDlg, WM_COMMAND, IDOK, (INT32)FALSE);
						return FALSE;
					}

				default:
					break;
			}
			break;

		default:
			return FALSE;
	}
#endif
    return FALSE;
}

/***********************************************************************************************

>	UINT32 CALLBACK SGLibOil::ExplorerFileHook(HWND hDlg, UINT32 Msg, UINT32 wParam, INT32 lParam)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/96

	Inputs:		hDlg - HWND of the dialog box.
 				iMsg - UINT32 message number.
 				wParam - UINT32 parameter.
				lParam - INT32 parameter.
 
	Returns:	Non-Zero or Zero, indicating if the common dialog should
                SKIP the message (non-zero) or not (zero).  Don't confuse
                with typical dialog box return values.
 
	Purpose:	Processes messages from the Explorer version of the common File Open dialog.
				Used to kill the get path dbox when OK is clicked, handling 'Generate' clicks,
				and re-arranging the window's items...

***********************************************************************************************/

#if _MFC_VER >= 0x400 
UINT32 CALLBACK SGLibOil::ExplorerFileHook(HWND hDlg, UINT32 Msg, UINT32 wParam, INT32 lParam)
{
//	ERROR3("ExplorerFileHook called");
	static char szstr2[256];

	switch(Msg)
	{
	 	case WM_INITDIALOG:
		{			
#ifdef STANDALONE
			// Remove the Help button on standalone builds...
			HWND hGadget = GetDlgItem((HWND)hDlg, (INT32)_R(IDC_LIBPATH_HELP_BUTTON));
			ShowWindow(hGadget, SW_HIDE);
#endif
			CommDlg_OpenSave_HideControl(GetParent(hDlg), edt1); // name of current file
			CommDlg_OpenSave_HideControl(GetParent(hDlg), stc3); // label for edt1
			CommDlg_OpenSave_HideControl(GetParent(hDlg), cmb1); // file-types combo
			CommDlg_OpenSave_HideControl(GetParent(hDlg), stc2); // label for cmb1
			CommDlg_OpenSave_HideControl(GetParent(hDlg), chx1); // read-only checkbox

			CommDlg_OpenSave_HideControl(GetParent(hDlg), IDOK);
			CommDlg_OpenSave_HideControl(GetParent(hDlg), IDCANCEL);

			// drop down at top...
			//CommDlg_OpenSave_HideControl(GetParent(hDlg), cmb2);
			//HWND hGadget2 = GetDlgItem((HWND)GetParent(hDlg), (INT32)cmb2);
			//::MoveWindow(hGadget2, 6, 32+20, 90+50, 68, TRUE);

			CommDlg_OpenSave_HideControl(hDlg, _R(IDC_LIBPATH_ISLIBRARY));
			HWND hGadget2 = GetDlgItem((HWND)hDlg, (INT32)_R(IDC_LIBPATH_ISLIBRARY));
			::MoveWindow(hGadget2, 212, 113+200, 200, 12, TRUE);

			// We need to apply the correct font if we are in DBCS land
			if (UnicodeManager::IsDBCSOS())
				FontFactory::ApplyFontToWindow(hDlg, STOCKFONT_DIALOG);

			return TRUE;
		}

		case WM_NOTIFY:
			return ExplorerFileHookNotify(hDlg, (LPOFNOTIFY)lParam);

	 	case WM_COMMAND:

			switch(wParam)
			{
				case _R(IDC_LIBPATH_HELP_BUTTON):
					//ERROR3("CC Help button pressed");
					// Special check for the help button, and a bodgy way of invoking help for
					// this dialog, which is a generic CFileDialog type with a custom template,
					// and thus has no unique class-name.
					_HelpUser(TEXT("SGalleryAddFolderDlg"));
					break;

				case IDOK:
				{
					// Close the dbox when OK pressed
					String_256 TmpPath;
					INT32 Length = 0;
					Length = CommDlg_OpenSave_GetFolderPath((HWND)GetParent(hDlg), (TCHAR *)TmpPath, 255);
					if(Length > 0)
					{
						SGLibOil::BrowsePath = TmpPath;
						((TCHAR *)SGLibOil::BrowsePath)[Length] = (TCHAR)'\0x00';
					}

					PostMessage((HWND)GetParent(hDlg), WM_COMMAND, IDCANCEL, (INT32)FALSE);
					break;
				}

				case IDCANCEL:
					// Close the dbox when Cancel pressed
					SGLibOil::BrowsePath = "";
					PostMessage((HWND)GetParent(hDlg), WM_COMMAND, IDCANCEL, (INT32)FALSE);
					break;

				case _R(IDC_LIBPATH_GENERATE):
				{
					// Generate clicked, close the window, then re-open it later on...
					SGLibOil::GenerateButtonClicked = TRUE;
					String_256 TmpPath;

					INT32 Length = 0;
					Length = CommDlg_OpenSave_GetFolderPath((HWND)GetParent(hDlg), (TCHAR *)TmpPath, 255);
					if(Length > 0)
					{
						SGLibOil::BrowsePath = TmpPath;
						((TCHAR *)SGLibOil::BrowsePath)[Length] = (TCHAR)'\0x00';
					}

					PostMessage((HWND)GetParent(hDlg), WM_COMMAND, IDCANCEL, (INT32)FALSE);
					break;
				}

				case _R(IDC_LIBPATH_SEARCH):
				{
					// Scan (Search) for some clipart
	
					if(!SGLibOil::ParentLibraryGallery->ScanForLocation(SGLibOil::LibType,
																		&SGLibOil::SearchDirectory))
					{
						INT32 ButtonPressed = InformWarning(_R(IDS_LIBRARY_BROWSING_CANT_FIND_LIBRARY), _R(IDS_OK));
						Error::ClearError();
						return FALSE;
					}

					DirPath Path(SGLibOil::SearchDirectory);
					if(!Path.IsValid())
					{
						ERROR3("Scanned to path is invalid");
						// Keep the dialog open - don't change the path
						return FALSE;
					}

					// Remember the pathname and which button was clicked so we can update the dialog
					SGLibOil::SearchButtonClicked = TRUE;

					PostMessage((HWND)GetParent(hDlg), WM_COMMAND, IDCANCEL, (INT32)FALSE);
					return FALSE;
				}

				default:
					break;
			}
			break;

	 	case WM_HELP:
			ERROR3("Context sensitive help clicked...");
			break;

		default:
			return FALSE;
	}

    return FALSE;
}


/***********************************************************************************************

>	static BOOL SGLibOil::ExplorerFileHookNotify(HWND hDlg, LPOFNOTIFY pofn)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/96

	Inputs:		hDlg - HWND of the dialog box.
 				pofn - pointer to notify structure
 
	Returns:	TRUE if ok...
 
	Purpose:	WM_NOTIFY is sent to the explorer hook - decode the various notify messages...
				Only really used to keep the path field up to date and check for indexes when
				changing folder...

***********************************************************************************************/

BOOL SGLibOil::ExplorerFileHookNotify(HWND hDlg, LPOFNOTIFY pofn)
{
	switch(pofn->hdr.code)
	{
		// The current selection has changed.
		case CDN_SELCHANGE:
			break;

		case CDN_FOLDERCHANGE:	// The current folder has changed.
		case CDN_INITDONE:		// or initialisation has finished
		{
			String_256 TmpPath;
			INT32 Length = 0;
			Length = CommDlg_OpenSave_GetFolderPath((HWND)GetParent(hDlg), (TCHAR *)TmpPath, 255);
			if(Length > 0)
			{
				SGLibOil::BrowsePath = TmpPath;
				
				for(INT32 i=(Length-1); i<=255; i++)
					((TCHAR *)SGLibOil::BrowsePath)[i] = (TCHAR)0;

				// Seems to be problems with this bit of code - the control gets random pants
				// stuck at the end of the pathname randomly. Removing the truncated path stuff
				// fixes it...
				/*PathName BPath(SGLibOil::BrowsePath);
				String_256 TruncatedPath;
				TruncatedPath = BPath.GetTruncatedPath(100);
				SetDlgItemText(hDlg, _R(IDC_LIBPATH_PATHNAME), (TCHAR *)TruncatedPath);*/
				SetDlgItemText(hDlg, _R(IDC_LIBPATH_PATHNAME), (TCHAR *)BrowsePath);

				if(SGLibOil::CheckForIndex(&SGLibOil::BrowsePath, SGLibOil::ParentLibraryGallery))
					SetDlgItemText(hDlg, _R(IDC_LIBPATH_ISLIBRARY), (TCHAR *)String_256(_R(IDS_LIBPATH_INDEX_EXISTS)));
				else
					SetDlgItemText(hDlg, _R(IDC_LIBPATH_ISLIBRARY), (TCHAR *)String_256(_R(IDS_LIBPATH_INDEX_DOESNT_EXIST)));
			}
			break;
		}

		// The Help button has been clicked - this shouldn't happen because we've disabled the default
		// help button...
		case CDN_HELP:
			break;

		// The OK button has been clicked, er, but hang on, we haven't got an OK
		// button, so this should never get called !
		case CDN_FILEOK:
			break;

		// Received a sharing violation
		case CDN_SHAREVIOLATION:
			ERROR3("SGLibOil::ExplorerFileHookNotify got a share violation");
			break;

		// Received when the file type changes in the Files Of Type box
		case CDN_TYPECHANGE:
			break;

		// oops, we've missed one...
		default:
			break;
	}

	return TRUE;
}

/***********************************************************************************************

>	static BOOL SGLibOil::CheckForIndex(String_256 *Path, LibraryGallery *ParentGallery)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/96

	Inputs:		Path			- Path to start looking from
				ParentGallery	- Library gallery for the index we're interested in

	Returns: 	TRUE if we find an index of the desired type in the given path, or the XaraInfo
				subdirectory of the given path

	Purpose:	Check if an index exists in the given path...

	Notes:		We return TRUE if an index.txt is found as well as the normal XaraClip.txt, or
				whatever. In an ideal world we should scan the index.txt for references to
				indexes of the desired type... Perhaps we do, and I've not updated this comment...

***********************************************************************************************/

BOOL SGLibOil::CheckForIndex(String_256 *Path, LibraryGallery *ParentGallery)
{
	ERROR3IF(Path == NULL || ParentGallery == NULL, "SGLibOil::CheckForIndex given NULLs");
	
	String_256 OurPath(*Path);
	if(LibraryFile::TidyUpSubPath(&OurPath))
	{
		// Add a trailing backslash if there isn't one
		SGLibOil::AppendSlashIfNotPresent(&OurPath);

 		OurPath += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));

		PathName Index;
		String_256 IndexPath;
		
		// Check for the default index for this directory
		String_256 DefIndex;
		ParentGallery->GetDefaults(&DefIndex, NULL, NULL);

		IndexPath = OurPath;
		SGLibOil::AppendSlashIfNotPresent(&OurPath);
		IndexPath += DefIndex;

		Index.SetPathName(IndexPath);
		Error::ClearError();
		if(FileExists(&Index))
			return TRUE;
		
		// Check for the global index.txt file
		IndexPath = OurPath;
		SGLibOil::AppendSlashIfNotPresent(&IndexPath);
		IndexPath += String_256(_R(IDS_LIBRARIES_INDEX_FILENAME));

		Index.SetPathName(IndexPath);
		Error::ClearError();
		if(FileExists(&Index))
			return TRUE;
	}

	Error::ClearError();
	return FALSE;
}
					
/***********************************************************************************************

>	static BOOL SGLibOil::GenerateClicked(LibraryGallery *LG, PathName *GenDirectory)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95

	Inputs:		LG		- required so we have access to the parent gallery's default indexey
						  values and stuff...
				GenDirectory - contains a pathname for the location of the index file to
						  generate. We strip the filename bit off, but it should have a
						  filename anyhow...
	Returns: 	TRUE if we generated an index ok

	Purpose:	Generate an index file in a given directory, go through all the various
				checks with the user, etc. This function is intended to be called after a
				click on the 'Generate' button in the browse dbox.
	Notes:		

***********************************************************************************************/

BOOL SGLibOil::GenerateClicked(LibraryGallery *LG, PathName *GenDirectory)
{
	if(GenDirectory == NULL || LG == NULL)
	{
		ERROR3("GenerateClicked given a null params");
		return FALSE;
	}
	
	if(!GenDirectory->IsValid())
	{
		ERROR3("GenerateClicked given an invalid path");
		return FALSE;
	}

	String_256 OurDirectory = GenDirectory->GetLocation();
//	OurDirectory = DialogManager::GetStringGadgetValue(hDlg, stc1, NULL);

	if(OurDirectory.Sub((String_8)"...") != -1)
	{
		ERROR3("Sorry, pathname has been truncated, can't generate index");
		return FALSE;
	}
	else
	{
		if(OurDirectory.Length() > 1)
		{
			// Remove the xarainfo and trailing \'s if there are any
			LibraryFile::TidyUpSubPath(&OurDirectory);

			DirPath SubLibraryPath(OurDirectory);			

			// Bodge for A:, B:, etc...
/*			BOOL RootDir = FALSE;
			if(OurDirectory.Length() == 2)
			{
				RootDir = TRUE;
				Error::ClearError();
			}*/

			if(SubLibraryPath.IsValid()/* || RootDir*/)
			{
				// Get pathname of default sublib index and find out whether we can generate indexes, etc
				String_256 IndexName;
				String_256 IndexDesc;
				BOOL CanGenerate = LG->CanCreateIndexes();		

				if(LG->GetDefaults(&IndexName, &IndexDesc, NULL))
				{
				 	if(CanGenerate)
					{
						String_256 FullIndexPathStr(OurDirectory);
						SGLibOil::AppendSlashIfNotPresent(&FullIndexPathStr);
						FullIndexPathStr += String_16(_R(IDS_LIBRARIES_XARAINFO_DIRNAME));
						FullIndexPathStr += TEXT("\\") + IndexName;
						PathName FullIndexPath(FullIndexPathStr);
						if(FullIndexPath.IsValid())
						{
							INT32 ButtonPressed = 0;
							BOOL DoThumbnails = TRUE;

							#if 0
							if(SGLibOil::FileExists(&FullIndexPath))
							{
								// Warn about existing index file
								ButtonPressed = InformWarning(_R(IDS_LIBRARY_OVERWRITE_INDEX_FILE), _R(IDS_GENERATE), _R(IDS_CANCEL));
								Error::ClearError();
								if(ButtonPressed == 2)
									return FALSE;
							}
							#endif

							#if 0
							// Ask if they want thumbnails or not
							ButtonPressed = InformWarning(_R(IDS_WITH_THUMBNAILS_OR_NOT), _R(IDS_THUMBNAILS), _R(IDS_NOTHUMBNAILS), _R(IDS_CANCEL)/*, _R(IDS_HELP)*/);
							Error::ClearError();
							if(ButtonPressed > 2)
								return FALSE;
							DoThumbnails = (ButtonPressed == 1);
							#endif
							
							if(SGLibOil::FileExists(&FullIndexPath) && GenerateIndexFile::IsDirectoryReadOnly(&SubLibraryPath))
							{
								// "Cannot update chosen folder because it is read only and already has an index."
								InformMessage(_R(IDS_LIBRARY_UPDATE_RO_WITH_INDEX), _R(IDS_CANCEL));
								Error::ClearError();
								return FALSE;
							}

							// Check they really want to generate the index - warn about disk usage
							ButtonPressed = InformWarning(_R(IDS_LIBRARY_CHECK_INDEX_CREATE), _R(IDS_CREATE), _R(IDS_CANCEL)/*, _R(IDS_HELP)*/);
							Error::ClearError();
							if(ButtonPressed > 1)
								return FALSE;
							DoThumbnails = TRUE;

							// Generate an index...
							String_64 Author("");
							
							BOOL ok = LibraryFile::GenerateDefaultIndex(&FullIndexPath, &SubLibraryPath, &Author, LibType, DoThumbnails);
							if(FullIndexPathStr != FullIndexPath.GetPath())
							{
								// The index location has changed - remote index...
								*GenDirectory = FullIndexPath.GetLocation(FALSE);
							 	SGLibOil::BrowsePath = GenDirectory->GetPath();
							}
							return ok;
						}
					}
					else
					{
						ERROR3("You can't generate an index file for this type of library");
					}
				}
			}
			else
			{
				ERROR3("Invalid pathname...");
			}
		}
	}
	return FALSE;
}


/**********************************************************************************************/

// Static required for hook function
SGLibType SGLibOil::LibType = SGLib_Blank;
LibraryGallery *SGLibOil::ParentLibraryGallery = NULL;

// Static containing the full path given by browsing
String_256 SGLibOil::BrowsePath = "";

// TRUE if the generate button was clicked in the browse dbox
BOOL SGLibOil::GenerateButtonClicked = FALSE;

// TRUE if the search button was clicked in the browse dbox
BOOL SGLibOil::SearchButtonClicked = FALSE;

// Directory located by the Search code
String_256 SGLibOil::SearchDirectory = "";

// Should we be using the explorer dialog ?
BOOL SGLibOil::UseExplorerForAdd = FALSE;

/***********************************************************************************************

>	static BOOL SGLibOil::GetLibPath(LibraryGallery *LG, PathName *Path, BOOL CanGenerate,
															SGLibType Type, BOOL *Updated = NULL)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/4/95

	Inputs:		LG - LibraryGallery for gallery which we're GetLibPath'ing...
				Path points to the default path to start the dbox off with
				CanGenerate should be TRUE if the dialog is to have a generate button
				Type of library we're browsing for (used by generate if the button is hit)
	Outputs:	Path is updated with the resulting library path
				Updated - if not null, returns true or false if the added item was updated or not...
	Returns: 	TRUE if the outputpath has valid index file in XaraInfo on return

	Purpose:	Locate a library index by popping up a patched file open dialog and
				letting the user scan through their directory tree. Also give them the
				option to generate an index if none exists for the selected directory.
	Notes:		

***********************************************************************************************/

BOOL SGLibOil::GetLibPath(LibraryGallery *LG, PathName *ThePath, BOOL CanGenerate, SGLibType Type,
																					BOOL *Updated)
{
	if(ThePath == NULL || LG == NULL)
	{
		ERROR3("SGLibOil::GetLibPath given null params");
		return FALSE;
	}

	// Which window style should we use ?
	BOOL Explorer = FALSE;
#if _MFC_VER >= 0x400 
/*	if(IsWindows95() && SGLibOil::UseExplorerForAdd)
		Explorer = TRUE;*/
	if(CCamApp::IsNewWindowsUI())
		Explorer = TRUE;
#endif
	
	if(Updated)	*Updated = FALSE;

	SGLibOil::LibType = Type;
	SGLibOil::ParentLibraryGallery = LG;

	TCHAR Filter[] = "Library directories|TheIndex|||";   // This is never displayed
		
	DWORD CFileFlags = OFN_HIDEREADONLY | OFN_NOTESTFILECREATE |
					   OFN_ENABLEHOOK   | OFN_ENABLETEMPLATE;
#if _MFC_VER >= 0x400 
	if(Explorer)
		CFileFlags |= OFN_EXPLORER;
#endif
	
	CFileDialog BrowseFileDialog(TRUE,
								 NULL,
								 (const TCHAR *)ThePath->GetPath(),
								 CFileFlags,
								 (TCHAR *)Filter,
								 NULL);
	
	String_256 InitialPath(ThePath->GetPath(TRUE));

	// This will contain the chosen path on return (provided they don't hit cancel)
	SGLibOil::BrowsePath = "";

	String_256 ExplorerBrowsePath;

	BrowseFileDialog.m_ofn.lStructSize = sizeof(OPENFILENAME);
	BrowseFileDialog.m_ofn.hwndOwner = NULL;
	BrowseFileDialog.m_ofn.hInstance = AfxGetResourceHandle();
	if(Explorer)
		BrowseFileDialog.m_ofn.lpstrFile = (TCHAR *)ExplorerBrowsePath;
	else
		BrowseFileDialog.m_ofn.lpstrFile = (TCHAR *)SGLibOil::BrowsePath;

	BrowseFileDialog.m_ofn.nMaxFile = 255;
	BrowseFileDialog.m_ofn.lpstrInitialDir = (TCHAR *)InitialPath;
	BrowseFileDialog.m_ofn.Flags = CFileFlags;

	String_64 DlgTitle(_R(IDS_LIBPATHDLG)/*Add new folder*/);
	BrowseFileDialog.m_ofn.lpstrTitle = (TCHAR *)DlgTitle;

	// Select our dialog template depending on the user's UI
#if _MFC_VER >= 0x400 
	if(Explorer)
	{
		BrowseFileDialog.m_ofn.lpfnHook = (LPOFNHOOKPROC)SGLibOil::ExplorerFileHook;
		if(CanGenerate)
			BrowseFileDialog.m_ofn.lpTemplateName = MAKEINTRESOURCE(_R(IDD_LIBPATHGENDLG95));
		else
			BrowseFileDialog.m_ofn.lpTemplateName = MAKEINTRESOURCE(_R(IDD_LIBPATHDLG95));
	}
	else
#endif
	{
		BrowseFileDialog.m_ofn.lpfnHook = (LPOFNHOOKPROC)SGLibOil::FileHook;
		if(CanGenerate)
			BrowseFileDialog.m_ofn.lpTemplateName = MAKEINTRESOURCE(_R(IDD_LIBPATHGENDLG));
		else
			BrowseFileDialog.m_ofn.lpTemplateName = MAKEINTRESOURCE(_R(IDD_LIBPATHDLG));
	}

	GenerateButtonClicked = FALSE;
	SearchButtonClicked = FALSE;

	// Keep popping the dbox up if generate is clicked
	BOOL Again = TRUE;
	while(Again)
	{
		// Keep Control Helper system informed
		ControlHelper::InformModalDialogOpened();
		
		INT32 Result = BrowseFileDialog.DoModal();

//		ERROR3_PF(("Before... Path: %s", (TCHAR *)SGLibOil::BrowsePath));

		if(Explorer)
		{
			// This will be empty if cancel was pressed
			if(SGLibOil::BrowsePath.Length() > 0)
			{
				// Add a trailing slash if it hasn't got one (usually, roots have 'em, but sub-dirs don't)
				SGLibOil::AppendSlashIfNotPresent(&SGLibOil::BrowsePath);

				SGLibOil::BrowsePath += TEXT("fish.wib"); // Required for later pathname nastyness, never displayed
			}
		}

//		ERROR3_PF(("After... Path: %s, Result: %d", (TCHAR *)SGLibOil::BrowsePath, Result));

		// Keep Control Helper system informed
		ControlHelper::InformModalDialogClosed();				

		// Check that we have floating point exception handler in place
		// as we have just used a common dialog box and under win32s this might
		// have reset the floating point exception handler.
PORTNOTE("dialog","Removed FixFPControlRegister usage - is it needed anymore?")
#ifndef EXCLUDE_FROM_XARALX
		FixFPControlRegister();
#endif
		
		// Close the browse box next time
		Again = FALSE;

		// The search / scan button in the dialog was clicked, SGLibOil::SearchDirectory contains
		// the location...
		if(SearchButtonClicked)
		{
			// We can't simply tell the dialog to change it's location whilst open, we have to
			// close it and reopen it - like the generate situation...
			SearchButtonClicked = FALSE;

			// Reopen the browse dbox
			Again = TRUE;		

			// Update the default path to be the one just selected
			BrowseFileDialog.m_ofn.lpstrInitialDir = SGLibOil::SearchDirectory;
		}


		// The generate button in the browse dialog was clicked... SGLibOil::BrowsePath contains
		// the currently selected path to generate an index for
		if(GenerateButtonClicked)
		{
			// Since the only way to return the full path is by a click on OK, we simulate
			// an OK click but flag a generate click as 
			GenerateButtonClicked = FALSE;
			PathName GenPath(SGLibOil::BrowsePath);

			if(GenPath.IsValid())
			{
				// If there was a problem with the index file - non created, etc, reopen the browse dbox...
				if(!GenerateClicked(LG, &GenPath))
				{
					// Reopen the browse dbox
					Again = TRUE;

					// Update the default path to be the one just selected
					BrowseFileDialog.m_ofn.lpstrInitialDir = SGLibOil::BrowsePath;				
				}
				else
				{
					if(Updated)	*Updated = TRUE;
				}
			}
			else
			{
				Error::ClearError();
				Again = TRUE;
			}
		}

		if(Again)
		{
			// Clear the internal path so cancel works properly
			SGLibOil::BrowsePath = "";

			if(!(Explorer && SearchButtonClicked))
				BrowseFileDialog.m_ofn.lpstrFile = (TCHAR *)SGLibOil::BrowsePath;
		}
	}

	if(SGLibOil::BrowsePath == (String_256)"")
	{
		Error::ClearError();
		return FALSE;
	}
	else
	{
		PathName Ret(SGLibOil::BrowsePath);
		if(Ret.IsValid())
		{
			// Normally we don't want the trailing backslash, if the path is invalid without
			// it then we do want it... ("C:" or "C:\")
			String_256 Location = Ret.GetLocation(FALSE);
			if(IsRootDirectory(&Location))
				Location = Ret.GetLocation(TRUE);

			PathName PathForReturn(Location);
			*ThePath = PathForReturn;
		}
		else
		{
			ERROR3("SGLibOil::GetLibPath !Ret.IsValid()");
			Error::ClearError();	
			return FALSE;
		}
	}

	Error::ClearError();

//	ERROR3_PF(("GetLibPath BrowsePath '%s'", (TCHAR *)SGLibOil::BrowsePath));
//	String_256 RetPath(ThePath->GetPath());
//	ERROR3_PF(("GetLibPath returning '%s'", (TCHAR *)RetPath));

	return TRUE;
}

#endif


/***********************************************************************************************

>	static BOOL SGLibOil::MakeSound(PathName *WaveFile)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/1/96

	Inputs:		WaveFile	- Full filename of .wav file
	Outputs:	
	Returns: 	TRUE if played ok...
	
	Purpose:	Plays a wave file through the computer's sound system (if it has one)

***********************************************************************************************/

BOOL SGLibOil::MakeSound(PathName *WaveFile)
{
#if defined(__WXMSW__)
	// Actually play the sound		
	String_256 File(WaveFile->GetPath());
	BOOL ok = PlaySound((TCHAR *)File, NULL, SND_ASYNC | SND_NODEFAULT | SND_NOWAIT | SND_FILENAME);
	if(!ok)
		ok = sndPlaySound((TCHAR *)File, SND_ASYNC | SND_NODEFAULT | SND_NOWAIT | SND_FILENAME);

	return ok;
#else
	TRACE( wxT("Warning - SGLibOil::MakeSound called\n") );
	return TRUE;
#endif
}


/***********************************************************************************************

>	static BOOL SGLibOil::PlayRandomSample(String_256 *SampleString, String_256 *SubPath)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/1/96

	Inputs:		SampleString	- String such as "Fish.wav|fred.wav|C:\tmp.wav"
				SubPath			- Path to append to file -> C:\animals
	Returns: 	TRUE if item found and played ok...
	
	Purpose:	Plays a random wave file through the computer's sound system (if it has one)

***********************************************************************************************/

BOOL SGLibOil::PlayRandomSample(String_256 *SampleString, String_256 *SubPath)
{
#if defined(__WXMSW__)
	String_256 SoundToUse(*SampleString);

	if(SoundToUse.Length() > 2)
	{
		// Pick a random sound from a line such as "Frog.wav|fish.wav|water.wav"
		INT32 Fields = SampleString->CountChar('|');

		if(Fields > 0)
		{
			INT32 Count = (rand()%(Fields+1))+1;
			String_256 TmpSoundToUse(SoundToUse);

			if(Count > 0)
			{
				TCHAR *Start = (TCHAR *)*SampleString;
				TCHAR *Finish = Start + SampleString->Length();

				// char to look for
				const TCHAR BAR = '|';

				TCHAR Ch = *Start;
				INT32 FieldCount = 1;

				// scan through fields until we hit our baby
				while(Count > FieldCount && Start < Finish)
				{
					if(Ch == 0) break;
					if(Ch == BAR) FieldCount ++;
					Start += sizeof(TCHAR);
					Ch = *Start;
				}

				// Pointer to sound to use
				if(Count == FieldCount)
					TmpSoundToUse = Start;
			}

			// Strip any trailing sounds
			if(TmpSoundToUse.Sub(String_8(_T("|"))) != -1)
				TmpSoundToUse.Left(&SoundToUse, TmpSoundToUse.Sub(String_8(_T("|"))));
			else
				SoundToUse = TmpSoundToUse;
		}

		// Path for normal files is with the pictures
		if(SoundToUse.Sub(String_8(_T("\\"))) == -1)
		{
			String_256 File = *SubPath;
			SGLibOil::AppendSlashIfNotPresent(&File);
			File += SoundToUse;
			SoundToUse = File;
		}

		PathName SoundPath(SoundToUse);
		return SGLibOil::MakeSound(&SoundPath);
	}
	return FALSE;
#else
	TRACE( wxT("Warning - SGLibOil::PlayRandomSample called\n") );
	return TRUE;
#endif
}


/***********************************************************************************************

>	static BOOL SGLibOil::IsRootDirectory(String_256 *Path)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/96

	Inputs:		Path	- Path string -> "A:\" or "C:\fred"
	Returns: 	TRUE if the path describes a root directory -> "A:\" or "\\deepthought\materials",
				"\\fred\fred\", etc...
	
	Purpose:	Checks if root directory or not...
`
***********************************************************************************************/

BOOL SGLibOil::IsRootDirectory(String_256 *Path)
{
	TRACE( wxT("Warning - SGLibOil::PlayRandomSample called\n") );
#if defined(__WXMSW__)
	ERROR3IF(Path == NULL, "SGLibOil::IsRootDirectory given a NULL string");
	if(Path == NULL)
		return FALSE;
//	ERROR3_PF(("IsRootDirectory -> %s", (TCHAR *)*Path));
	
	// Remove trailing '\'s
	String_256 TmpPath(*Path);
	if(TmpPath[TmpPath.Length()-1] == _T('\\'))
		Path->Left(&TmpPath, Path->Length()-1);

	// Check for 'a:'s...
	if(	TmpPath.Length() == 2 && TmpPath[1] == _T(':') )
		return TRUE;

//	ERROR3_PF(("For %s, Sub = %d, Count = %d", (TCHAR *)TmpPath, TmpPath.Sub(String_8("\\\\"), 0, TCHAR('*')), TmpPath.CountChar('\\')));
	
	// Check if unc root dir
	if( ( TmpPath.Sub( String_8( wxT("\\\\") ), 0, wxT('*') ) == 0 && TmpPath.CountChar( wxT('\\') ) <= 3 ) )
	{
//		ERROR3("UNC root");
		return TRUE;
	}

//	ERROR3_PF(("%s is not a root", (TCHAR *)*Path));
	return FALSE;
#else
	return FALSE;
#endif
}

/***********************************************************************************************

>	static BOOL SGLibOil::LoadSoundDefinition(CXaraFileRecordHandler *pXFileRecHandler,
					CXaraFile * pCXFile, INT32 Tag, UINT32 Size, UINT32 RecordNumber);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/96

	Inputs:		pXFileRecHandler - The CXaraFileRecordHandler that is being used to provide read functions etc.
				pCXFile	- The CXaraFile that is being used to provide read functions etc.
				Tag				- The tag of the record and hence the type of file being imported
				Size			- The size of this record
				RecordNumber	- The record number for this record
	Returns:	TRUE if the record was succesfully imported;
				FALSE if not (e.g. out of memory)
	
***********************************************************************************************/

BOOL SGLibOil::LoadSoundDefinition(CXaraFileRecordHandler *pXFileRecHandler, CXaraFile * pCXFile,
								 INT32 Tag, UINT32 Size, UINT32 RecordNumber)
{
	TRACE( wxT("Warning - SGLibOil::LoadSoundDefinition called\n") );
#if defined(__WXMSW__)
	if(TmpSndName != NULL)
	{
		PathName TheFile(TmpSndName);
		FileDelete(&TheFile);
		TmpSndName = NULL;
	}

	// Create a temporary filename to use
	if(( TmpSndName = _ttmpnam( NULL ) ) != NULL )
	{
		UINT32 Size = pCXFile->GetCurrentRecordSize();
		CCFile *pRecordFile = pCXFile->GetCCFile();

		void *Buffer = CCMalloc(Size + 4);
	
		BOOL ok = FALSE;

		if(Buffer != NULL)
		{
			CCDiskFile File(1024, FALSE, TRUE);
 
			// Open file and check if it exists at the same time
			try
			{
				// Read file into the buffer
				pRecordFile->read(Buffer, Size);

				// Write file from buffer into output file
				PathName OutputPath(TmpSndName);
				File.open(OutputPath, ios::out | ios::binary);
				File.write(Buffer, Size);
				if(File.isOpen())
					File.close();

				// File copied ok...
				ok = TRUE;
			}		
			catch( CFileException )
			{
				// File not copied ok... Clear the errors and fall though to alternative low-
				// memory file copy... (this should be very rare !)
				Error::ClearError();
			}

			CCFree(Buffer);
		}

		if(ok)
		{
			ok = PlaySound((TCHAR *)TmpSndName, NULL, SND_ASYNC | SND_NODEFAULT | SND_NOWAIT | SND_FILENAME);
			if(!ok)
				ok = sndPlaySound((TCHAR *)TmpSndName, SND_ASYNC | SND_NODEFAULT | SND_NOWAIT | SND_FILENAME);		
		}
/*		PathName TheFile(TmpSndName);
		FileDelete(&TheFile);*/
	}
	
	return TRUE;
#else
	return FALSE;
#endif
}


/***********************************************************************************************

>	static BOOL SGLibOil::ExportWav(BaseCamelotFilter *pFilter)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/96

	Inputs:		pFilter		- Filter point to which we want the data streaming
	Returns:	TRUE if the record was succesfully exported;
				FALSE if not
	
***********************************************************************************************/

BOOL SGLibOil::ExportWav(BaseCamelotFilter *pFilter)
{
#if 0
	BOOL ok = TRUE;
	INT32 RecordNumber = pFilter->StartStreamedRecord(TAG_DEFINESOUND_WAV, CXF_UNKNOWN_SIZE);

	// If we had a problem starting the record up then exit now
	if (RecordNumber <= 0)
		return RecordNumber;

	PathName Source("C:\\sample.wav");	
	INT32 Size = SGLibOil::FileSize(&Source);
	void *Buffer = CCMalloc(Size + 4);

	if(Buffer != NULL)
	{
		CCDiskFile File(1024, FALSE, TRUE);

		// Open file and check if it exists at the same time
		TRY
		{
			// Read file into the buffer
			File.open(Source.GetPath(), ios::in | ios::binary | ios::nocreate);
			File.read(Buffer, Size);
			if(File.isOpen())
				File.close();

			// Write file from buffer into output file
			//pOutput->write(Buffer, Size);
			for(INT32 i=0; i<Size; i++)
				pFilter->Write(((BYTE *)Buffer)[i]);
		}		

		CATCH( CFileException, e)
		{
			// File not copied ok... Clear the errors and fall though to alternative low-
			// memory file copy... (this should be very rare !)
			Error::ClearError();
		}
		END_CATCH

		CCFree(Buffer);
	}

	// Ask for the record to be ended and hence items like the size in the record header
	// to be cleaned up and hence made correct
	if (ok) ok = pFilter->EndStreamedRecord();

	return ok;
#else
	return FALSE;
#endif
}

/***********************************************************************************************

>	static void SGLibOil::AppendSlashIfNotPresent(String_256 *Str)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/96

	Inputs:		Str		- String to append slash to
	Outputs:	Str		- String with slash appended
  
	Purpose:	Checks if the string has a slash on the end, and appends one if not

***********************************************************************************************/

void SGLibOil::AppendSlashIfNotPresent(String_256 *Str)
{
	ERROR3IF(Str == NULL, "SGLibOil::AppendSlashIfNotPresent given a NULL string");
	if(Str == NULL)
		return;

	if(Str->Length() > 0)
		if( ((TCHAR *)*Str)[Str->Length()-1] != '/' )
			*Str += TEXT("/");
}


/***********************************************************************************************

>	static DWORD SGLibOil::GetDriveSerialNumber(String_256 *PathStr)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/7/96

	Inputs:		PathStr	- Pathname string containing drive, etc
	Returns:	Serial number for given pathname's drive, or 0 if there were problems
  
	Purpose:	Obtains a serial number for the drive hosting the given pathname. Theoretically
				CD's all contain unique serial numbers, so we can use this to check for new
				CD's in the drive, etc...

***********************************************************************************************/

DWORD SGLibOil::GetDriveSerialNumber(String_256 *PathStr)
{
	TRACE( wxT("Warning - SGLibOil::GetDriveSerialNumber called\n") );
#if defined(__WXMSW__)
	if(PathStr == NULL)
		return 0;

	TCHAR				Drive[MAX_PATH];
	BOOL				driveFound = FALSE;

	// Just for safety
	String_256 Path = *PathStr;
	const TCHAR *fn;
    fn = Path;
	BOOL BadCharacter = FALSE;

	// First check if there is a valid drive name or net drive name present
	// If the first character of the path is an alpha then assume drive and check if valid
	// and so of the form D:/
	// Otherwise, check for a UNC (Universal naming convention) form of drive of the form
	// //Deepthought 
	if (String::IsAlpha(*fn))			
   		driveFound = getDrive(&fn, Drive);	// Parse the Drive 
	else 
		driveFound = getNetDrive(&fn, Drive, &BadCharacter);	//Parse as UNC or network drive

	if(driveFound && !BadCharacter)
	{
		String_256 FileSystemName;
		String_256 VolumeName;
		DWORD SerialNumber = 0;
		BOOL bRC = FALSE;

		bRC=GetVolumeInformation(Drive,
				(TCHAR *)VolumeName,
				255,
				&SerialNumber,
				NULL,
				NULL,
				(TCHAR *)FileSystemName,
				255);

		if(bRC)
			return SerialNumber;
	}

	return 0;
#else
	return 0x00BAD000;
#endif
}






/********************************************************************************************

>	TCHAR* InetUtils::GetStringField(UINT32 nIndex, UINT32 nCompositeStringID)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/01/97

	Inputs:		nCompositeStringID - ID of the composite string
					nIndex - relative position of the field	within the composite string 
	Returns:	 a pointer to a buffer containing the field, NULL
					if the string is not valid/composite

	Purpose:	Allows string resource sharing

  	Note:		The composite string is limited to 1024 characters

********************************************************************************************/

namespace InetUtils {

TCHAR* GetStringField( UINT32 nIndex, UINT32 nCompositeStringID )
{
	PORTNOTETRACE("dialog","GetStringField - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	TCHAR rgtchBuff[1024];
	BOOL bLoad = ::LoadString(AfxGetResourceHandle(), nCompositeStringID, rgtchBuff, sizeof(rgtchBuff));
	ERROR3IF(!bLoad, "Failed to load string from resource");
	TCHAR* szField = _tcstok(rgtchBuff, _T("|"));
	for (UINT32 i = 0; i < nIndex && szField; i++)
		szField = _tcstok(NULL, _T("|"));
	return szField;
#else
	return NULL;
#endif
}


/********************************************************************************************

>	void InetUtils::MakeURL(const String_256& rRelativePath, const String_256& rRootURL, String_256& rURL) 

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/96

	Params:		rRelativePath (in) - const reference to string holding a relative file path (no leading slash),
						ex. "clipart\\drawings\\xarainfo\\xaraclip.txt"
					 rRootURL (in) - const reference to string holding the root clipart URL on our web site, 
						currently "http://www.xara.com/"
										
	Returns:	URL of file on our site as a String_256 object. Note that this has no lvalue and should
					only be used as an initializer for a local string.

	Purpose:	Allows us to obtain the URL of a file knowing its relative path (from the main clipart index
						file) and the root URL. The URL is necessary to download the file. 
					 

********************************************************************************************/

String_256 MakeURL(const String_256& rRelativePath, const String_256& rRootURL)
{
	String_256 strURL = rRootURL;
	INT32 nCharCount = rRelativePath.Length();
	if (camStrstr( (const TCHAR *)strURL, _T("file://") ) )
		strURL += rRelativePath;
	else for (INT32 i = 0; i < nCharCount; i++)
	{
		// Walk down the relative path string, appending characters to the URL as we go along
		// Backslashes are converted to normal slashes
		if (rRelativePath[i] == _T('\\'))
			strURL += _T('/');
		else
			strURL += camTolower( rRelativePath[i] );
	}
	return strURL;
}


/***********************************************************************************************
>	String_256 InetUtils::MakeLocalPath(const String_256& rRelPath)
	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/96
	Params:		rRelPath (in) - reference to string containing a relative file path (no leading slash)
				as read from a web clipart index file
	Returns:	A full local in the applicatoin's data folder eg if rRelPath is
				"clipart\drawings\anatomy\baby.xar" then the function returns
				"c:\documents and settings\jonathan\local settings\application data\xara\Cache\clipart\drawings\anatomy\baby.xar".
	Purpose:	Returns the expected location of a web clipart catalog file on the local machine,
				having its relative path from the main index file. 
***********************************************************************************************/

String_256 MakeLocalPath(const String_256& rRelPath)
{
	String_256 strAppDataPath;
	GetAppDataPath(&strAppDataPath);
	if (strAppDataPath.IsEmpty()) // failed to get the path for some reason
		return strAppDataPath;
	PathNameEx appDataPath(strAppDataPath);
	String_256 strLocalPath = appDataPath.GetLocation(TRUE);
	strLocalPath += String_256(_R(IDS_CACHEPATH));
	strLocalPath += rRelPath;
	return strLocalPath;
}


/***********************************************************************************************
  >	BOOL InetUtils::GetTempInetDir(String_256* pTempDir)
	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> (rewritten by Jonathan)
	Created:	1712/96 (rewritten 31 Jan 2001)
	Inputs:		pTempDir (out) - string returning the path to the OS temporary internet
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Get path to the internet cache directory used to download web clipart files
***********************************************************************************************/
BOOL GetTempInetDir(String_256* pTempDir)
{
#if defined(__WXMSW__)
	if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_INTERNET_CACHE | CSIDL_FLAG_CREATE,
						NULL, 0, static_cast<TCHAR *>(*pTempDir))))
		return FALSE;
#else
#pragma error( "No equiverlent for SHGetFolderPath" )
#endif

	*pTempDir += wxT("\\");

	return TRUE;
}

/********************************************************************************************
>	BOOL GetAppDataPath(String_256* pPath)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/2001
	Inputs:		pPath	- String 256 to fill with path data
	Purpose:	Gets the app's data path eg "C:\ Documents And Settings\ Jonathan\
				Local Settings\ Application Data\ Xara X\" (NOTE: includes slash at
				end)
********************************************************************************************/
BOOL GetAppDataPath(String_256* pPath)
{
#if defined(__WXMSW__)
	if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
						NULL, 0, static_cast<TCHAR *>(*pPath))))
		return FALSE;
#else
#pragma error( "No equiverlent for SHGetFolderPath" )
#endif

	*pPath += wxT("\\");
	*pPath += String_256(PRODUCT_MANUFACTURER); // PRODUCT_MANUFACTURER == "Xara"
	*pPath += wxT("\\");
	*pPath += String_256(PROGRAM_NAME); // PROGRAM_NAME == "Xara X" || "Xara X (debug)"
	*pPath += wxT("\\");

	return TRUE;
}

/********************************************************************************************
>	BOOL GetAppCachePath(String_256* pPath)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/2001
	Inputs:		pPath	- String 256 to fill with path data
	Purpose:	Gets the app's data path eg "C:\ Documents And Settings\ Jonathan\
				Local Settings\ Application Data\ Xara X\ Cache\" (NOTE: includes slash at
				end)
********************************************************************************************/
BOOL GetAppCachePath(String_256* pPath)
{
	if (!GetAppDataPath(pPath))
		return FALSE;

	*pPath += String_256(_R(IDS_CACHEPATH)); // _R(IDS_CACHEPATH) == "Cache\\"

	return TRUE;
}


/***********************************************************************************************

>	BOOL InetUtils::DeleteTree(const String_256& rPath)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1712/96

	Inputs:		reference to String_256 containing the path to the root directory
					of the tree to be removed.
	Returns:	TRUE if successful, FALSE in case of total or partial failure (some 
					of the files are locked etc.)
  
	Purpose:	remove a directory tree from the hard drive

  See also:		PathNameEx::RemoveRecursively()
	

***********************************************************************************************/

BOOL DeleteDirectoryTree(const String_256& rPath)
{
	PathNameEx dirPath(rPath);
	return dirPath.Remove();
}}

// <<webster


