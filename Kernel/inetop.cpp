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
#include "camtypes.h"
#include "sgliboil.h"

#include "app.h"		
//#include "galstr.h"
#include "sglcart.h"
#include "sgfonts.h"
#include "sglib.h"
#include "sgscan.h"		
#include "sgscanf.h"	
#include "errors.h"
#include "oilfltrs.h"
#include "selop.h"
#include "filedlgs.h"
//#include "resource.h"	// _R(IDS_CANCEL)
#include "camdoc.h"
//#include "webster.h"
//#include "richard.h"
#include "pathnmex.h"
#include "inetop.h"
#include "sglbase.h"
#include "progress.h"
#include "sglfills.h"
#include "backgrnd.h"

#include <io.h>			// for FileExists
#include <stdio.h>
#include <stdlib.h>

using namespace InetUtils;

CC_IMPLEMENT_DYNCREATE(DownloadOp, Operation)
CC_IMPLEMENT_DYNCREATE(OpAsynchClipartImport, OpClipartImport)
CC_IMPLEMENT_DYNCREATE(OpAddWebFolders, Operation)
CC_IMPLEMENT_DYNCREATE(OpAddWebLibrary, Operation)
CC_IMPLEMENT_DYNCREATE(OpThumbDownload, Operation)
CC_IMPLEMENT_DYNCREATE(OpAsynchFontInstall, DownloadOp)
CC_IMPLEMENT_DYNCREATE(OpAsynchBitmapImport, DownloadOp)


#define new CAM_DEBUG_NEW

void* DownloadOpParam::pAllocatedMemory;

DownloadOpParam::DownloadOpParam()
{
#ifdef _DEBUG
	if ((void*) this != pAllocatedMemory)
		ERROR3("DownloadOpParam-derived objects should be allocated using the new operator");
	pAllocatedMemory = NULL;
#endif
	// default to clipart download parameters
	type = TYPE_CLIPART;
	priority = AsynchDownload::PRIORITY_HIGH;
	bHasProgressDlg = TRUE;
	strDescription = _T("");
}

/********************************************************************************************

>	DownloadOp::~DownloadOp() 

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/04/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	DownloadOp constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
DownloadOp::DownloadOp()
{
	pParam = NULL;
	m_hDownload = 0;
	m_nPercentageCompleted = 0;
	OpFlags.HasOwnTimeIndicator = TRUE;
//	GetApplication()->RegisterIdleProcessor(IDLEPRIORITY_LOW, this); // start the polling loop
}       

/********************************************************************************************

>	DownloadOp::~DownloadOp() 

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/04/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	DownloadOp destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DownloadOp::~DownloadOp()
{
	GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_LOW, this); // end polling loop
	if (pParam)
		delete pParam;
}

/******************************************************************************************

>	BOOL DownloadOp::Init()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/04/97
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for a DownloadOp
	SeeAlso:

******************************************************************************************/


BOOL DownloadOp::Init()
{  
	return RegisterOpDescriptor(
		0,								// Tool ID
 		_R(IDS_OPDOWNLOAD),				// String resource ID
		CC_RUNTIME_CLASS(DownloadOp),	// Runtime class
 		OPTOKEN_OPDOWNLOAD,			// Token string
 		DownloadOp::GetState,			// GetState function
		0,								// Help ID
		0,								// Bubble ID
		0,								// Resource ID
		0								// Control ID
	);
}   

/******************************************************************************************

>	OpState	DownloadOp::GetState(String_256* pString, OpDescriptor* pOpDesc)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/04/97
	Purpose:	Returns the OpState of the DownloadOp operation.
	SeeAlso:

******************************************************************************************/

OpState	DownloadOp::GetState(String_256* pString, OpDescriptor* pOpDesc)
{
	OpState OpSt;
	return(OpSt);
}



/********************************************************************************************

>	void DownloadOp::Do(OpDescriptor*)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/97
	Inputs:		OpDescriptor* (unused)
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		Always fails - should not be called
	SeeAlso:	-

******************************************************************************************/
		
void DownloadOp::Do(OpDescriptor *NotUsed)
{
	ERROR3("This operation does not provide a Do() function - Use DoWithParam");
	End();
}


/******************************************************************************************
>	void DownloadOp::DoWithParam(OpDescriptor* pOp, OpParam* pDownloadOpParam)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		pOp - OpDescriptor as for all Do() functions
				pDownloadOpParam - pointer to  DownloadOpParam structure . This structure
					must be created on the heap as it should not be destroyed when it goes
					out of scope.
	Outputs:	-  none
						Note that this function returns immediately after starting the download.
	Returns:	-
	Purpose:	Request a download with the parameters passed in pDownloadOpParam

******************************************************************************************/

void DownloadOp::DoWithParam(OpDescriptor* pOp, OpParam* pDownloadOpParam)
{
	DownloadOpParam *pInfo = (DownloadOpParam*) pDownloadOpParam;
	if (pInfo == NULL)
	{
		ERROR3("DoWithParam called with invalid pointer");
		goto FAIL;
	}
	else
	{
		if (!pInfo->file.IsValid() || pInfo->strURL.IsEmpty())
		{
			ERROR3("Invalid file path or URL");
			goto FAIL;
		}
		// Register the file for download
		DOWNLOADINFO downloadInfo;
		downloadInfo.strURL = pInfo->strURL;
		downloadInfo.strLocalFile = (String_256) pInfo->file.GetPath();
		downloadInfo.nFileType = pInfo->type;
		downloadInfo.nPriority = pInfo->priority;
		downloadInfo.strDescription = pInfo->strDescription;
		downloadInfo.bHasProgressDlg = TRUE;

		// NEW! Tell this downloader that we can handle status messages
		// (We don't use Idle event polling)
		downloadInfo.hwndNotifyWindow = GetMainFrame()->GetSafeHwnd();
		downloadInfo.lNotifyToken = (INT32)this;

		m_hDownload = InternetManager::RegisterDownload(&downloadInfo);
		if (!m_hDownload || m_hDownload == (DOWNLOAD_HANDLE) INVALID_HANDLE_VALUE)
		{
			ERROR3("Failed to register download");
			goto FAIL;
		}
		pParam = pInfo;
		if (pInfo->Output)
			*(pInfo->Output) = TRUE;
	}
	return;

FAIL:
		if (pInfo->Output)
			*(pInfo->Output) = FALSE;
		FailAndDiscard(); 
		End();
}

/********************************************************************************************

>	void DownloadOp::End()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	overrides Operation::End() to avoid having an ERROR2 (caused
					by Document::GetSelectedSpread() returning NULL) go off when
					Camelot exits while a download is still in progress 
				
	Errors:		-
	SeeAlso:	Operation::End()

********************************************************************************************/

void DownloadOp::End()
{
	if (m_hDownload)
		InternetManager::UnregisterDownload(m_hDownload);
	if (Document::GetSelectedSpread())
		Operation::End();
	else
		EndOp();
}	




/******************************************************************************************
>	virtual BOOL DownloadOp::OnIdleEvent()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		-
	Outputs:	- 
						
	Returns:	- TRUE if we want to continue polling, FALSE otherwise
	Purpose:	- idle time polling loop; continues until either the download completes
						or the operation is terminated on receiving a death message.
						The appropriate functions are called according to the state of the download
						on completion (succeeded/aborted/failed)

******************************************************************************************/

BOOL DownloadOp::OnIdleEvent()
{
//ERROR2(FALSE, "DownloadOp::OnIdleEvent should no longer be called!!!");
	INT32 nPercentageCompleted = 0;
	ERROR2IF(!pParam, FALSE, "Unexpected NULL pointer");
	switch (InternetManager::GetDownloadState(m_hDownload))
	{
		case AsynchDownload::STATE_PENDING:
			// check the percentage downloaded so far
			nPercentageCompleted = InternetManager::GetPercentageDownloaded(m_hDownload);
			ERROR3IF(nPercentageCompleted == -1, "Invalid download handle");
			if (nPercentageCompleted != m_nPercentageCompleted) // we made some progress
			{
				m_nPercentageCompleted = nPercentageCompleted;
				OnDownloadProgress(nPercentageCompleted);
			}
			// download pending, so we continue polling
			return TRUE;
		case AsynchDownload::STATE_SUCCEEDED:
			// file has transferred successfully, we call OnDownloadSuccess() to process it
			OnDownloadSuccess();
			SucceedAndDiscard();
			End();
			return FALSE;
		case AsynchDownload::STATE_FAILED:
			OnDownloadFail();
			break;
		case AsynchDownload::STATE_ABORTED:
			OnDownloadAbort();
			break;
		default:
			ERROR3("Error or unrecognized state");
	}
	FailAndDiscard();
	End();
	return FALSE;
}




/******************************************************************************************
>	virtual void DownloadOp::OnDownloadSuccess()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		-
	Outputs:	- 
						
	Returns:	- 
	Purpose:	- This function is called if the file download completes successfully
					so that we can process it or take some other action. Derived classes 
					should override this function - the current implementation does nothing. 

******************************************************************************************/

void DownloadOp::OnDownloadSuccess()
{
}

/******************************************************************************************
>	virtual void DownloadOp::OnDownloadFail()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		-
	Outputs:	- 
						
	Returns:	- 
	Purpose:	- This function is called if the file download fails. The current implemention
					simply outputs an error message based on the file type declared in DownloadOpParam.
					If you want something more sophisticated override this function in your class.

******************************************************************************************/

void DownloadOp::OnDownloadFail()
{
		String_256 strTemp;
		strTemp.MakeMsg(_R(IDS_DOWNLOADFAILED), (TCHAR*) (String_256) GetStringField((UINT32) pParam->type, _R(IDS_GALLERY_DESCRIPTIONS)), (TCHAR*) pParam->strDescription);
		Error::SetError(0, strTemp, 0);
		InformError();
}


/******************************************************************************************
>	virtual void DownloadOp::OnDownloadAbort()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		-
	Outputs:	- 
						
	Returns:	- 
	Purpose:	- This function is called if the file download is cancelled by the user.
						Derived classes should override this function if some specific action
						is required in this case. 

******************************************************************************************/

void DownloadOp::OnDownloadAbort()
{
}




/******************************************************************************************
>	virtual void DownloadOp::OnDownloadPending()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/05/2004
	Inputs:		-
	Outputs:	- 
						
	Returns:	- 
	Purpose:	- This function is called if the file download is just in the process of working.
						Derived classes should override this function if some specific action
						is required in this case. 

******************************************************************************************/

void DownloadOp::OnDownloadPending()
{
	// check the percentage downloaded so far
	INT32 nPercentageCompleted = 0;
	nPercentageCompleted = InternetManager::GetPercentageDownloaded(m_hDownload);
//	ERROR3IF(nPercentageCompleted == -1, "Invalid download handle");
	if (nPercentageCompleted != -1)
	{
		if (nPercentageCompleted != m_nPercentageCompleted) // we made some progress
		{
			m_nPercentageCompleted = nPercentageCompleted;
			OnDownloadProgress(nPercentageCompleted);
		}
	}
}




// Asynchonous clipart import
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


AsynchClipartImportParam::AsynchClipartImportParam()
{
	Import 		= FALSE;
	bDropped = FALSE;
}

AsynchClipartImportParam::~AsynchClipartImportParam()
{
}


OpAsynchClipartImport::OpAsynchClipartImport()
{
	pParam = NULL;
	m_hDownload = 0;
	m_pTargetDoc = NULL;
	m_pCurrentSelDoc = NULL;
	GetApplication()->RegisterIdleProcessor(IDLEPRIORITY_LOW, this);
}       

/********************************************************************************************

>	OpAsynchClipartImport::~OpAsynchClipartImport() 

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpAsynchClipartImport destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpAsynchClipartImport::~OpAsynchClipartImport()
{
	GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_LOW, this);
	if (pParam)
		delete pParam;
}

/******************************************************************************************

>	BOOL OpAsynchClipartImport::Init()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/01/97
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for an AsynchClipartImport operation
	SeeAlso:

******************************************************************************************/


BOOL OpAsynchClipartImport::Init()
{  
	return RegisterOpDescriptor(
		0,								// Tool ID
 		_R(IDS_OPASYNCHCLIPARTIMPORT),				// String resource ID
		CC_RUNTIME_CLASS(OpAsynchClipartImport),	// Runtime class
 		OPTOKEN_OPASYNCHCLIPARTIMPORT,			// Token string
 		OpAsynchClipartImport::GetState,			// GetState function
		0,								// Help ID
		0,								// Bubble ID
		0,								// Resource ID
		0								// Control ID
	);
}   

/******************************************************************************************

>	OpState	OpAsynchClipartImport::GetState(String_256* pString, OpDescriptor* pOpDesc)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Purpose:	Returns the OpState of the OpAsynchClipartImport operation.
	SeeAlso:

******************************************************************************************/

OpState	OpAsynchClipartImport::GetState(String_256* pString, OpDescriptor* pOpDesc)
{
	OpState OpSt;
	return(OpSt);
}


/******************************************************************************************

>	BOOL OpAsynchClipartImport::OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/97
	Returns:	- 
	Purpose:	Notifies us if the document we intend to import into is about to be destroyed,
					in which case we should abort the operation
	SeeAlso:	MessageHandler::OnDocChangingMsg()

******************************************************************************************/

BOOL OpAsynchClipartImport::OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State)
{
	if (pChangingDoc == m_pTargetDoc && State == DocChangingMsg::ABOUTTODIE)
	{
		// We end the operation, but don't abort the clipart download - it's up to the user to do that
		FailAndExecute(); 
		End();
	}
	return TRUE;
}


/********************************************************************************************

>	void OpAsynchClipartImport::Do(OpDescriptor*)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/97
	Inputs:		OpClipartImport (unused)
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		Always fails - should not be called
	SeeAlso:	-

******************************************************************************************/
		
void OpAsynchClipartImport::Do(OpDescriptor *NotUsed)
{
	ERROR3("OpAsynchClipartImport does not provide a Do() function - Use DoWithParam");
	End();
}

/******************************************************************************************
>	void OpAsynchClipartImport::DoWithParam(OpDescriptor* pOp, OpParam* pClipartImportParam)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		pOp - OpDescriptor as for all Do() functions
				pClipartImportParam - pointer to  AsynchClipartImport structure . This structure
					must be created on the heap as it should not be destroyed when it goes
					out of scope.
	Outputs:	-  none
						Note that this function returns immediately after starting the download, so its success is not relevant to the outcome
						of the actual clipart import operation, which will be done on the idle loop if the download
						eventually succeeds. In case of failure, however, the user will be informed that the download - and
						clipart import - cannot be started.
	Returns:	-
	Purpose:	Performs an asynch clipart import / open depending on the parameters
	Notes:		In case the downloads succeeds, the actual clipart import will be done
					on the idle loop in order to avoid reentering the import code (potentially messy)
					in case of multiple	symultaneous downloads
	SeeAlso: OpClipartImport, OpAsynchClipartImport::OnIdleEvent()

******************************************************************************************/

void OpAsynchClipartImport::DoWithParam(OpDescriptor* pOp, OpParam* pClipartImportParam)
{
	AsynchClipartImportParam *pInfo = (AsynchClipartImportParam*) pClipartImportParam;

	if(pInfo == NULL)
	{
		ERROR3("OpAsynchClipartImport called with NULL info pointer");
		goto FAIL;
	}
	else
	{
		if (!pInfo->File.IsValid() || pInfo->strURL.IsEmpty())
		{
			ERROR3("Invalid file path or URL");
			goto FAIL;
		}
		// Remember the document pointer
		m_pTargetDoc = Document::GetSelected();
		m_pCurrentSelDoc = Document::GetSelected(); // target and selected are the same at this stage
		// Register the file for download
		DOWNLOADINFO downloadInfo;
		downloadInfo.strURL = pInfo->strURL;
		downloadInfo.strLocalFile = (String_256) pInfo->File.GetPath();
		downloadInfo.nFileType = TYPE_CLIPART;
		downloadInfo.nPriority = AsynchDownload::PRIORITY_HIGH;
		downloadInfo.strDescription = pInfo->strDescription;
		downloadInfo.bHasProgressDlg = TRUE;
		downloadInfo.hwndNotifyWindow = 0;
		downloadInfo.lNotifyToken = 0;
		m_hDownload = InternetManager::RegisterDownload(&downloadInfo);
		if (!m_hDownload || m_hDownload == (DOWNLOAD_HANDLE) INVALID_HANDLE_VALUE)
		{
			ERROR3("Failed to register download");
			goto FAIL;
		}
		pParam = pInfo;
	}
	return;

FAIL:
		FailAndExecute(); 
		End();
}

/********************************************************************************************

>	void OpAsynchClipartImport::End()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	overwrites Operation::End() to avoid having an ERROR2 (caused
					by Document::GetSelectedSpread() returning NULL) go off when
					Webster exits while an asynch clipart import is still in progress 
				
	Errors:		-
	SeeAlso:	Operation::End()

********************************************************************************************/

void OpAsynchClipartImport::End()
{
	if (Document::GetSelectedSpread())
		Operation::End();
	else
		EndOp();
}	




/******************************************************************************************
>	virtual BOOL OpAsynchClipartImport::OnIdleEvent()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		-
	Outputs:	- 
						
	Returns:	- TRUE if we want to continue polling, FALSE otherwise
	Purpose:	- idle time polling loop; continues until either the download completes
						or the operation is terminated on receiving a death message.
						Different actions are taken according to the state of the download
						on completion (succeeded/aborted/failed)

******************************************************************************************/

BOOL OpAsynchClipartImport::OnIdleEvent()
{
	ERROR2IF(!pParam, FALSE, "Unexpected NULL pointer");
	switch (InternetManager::GetDownloadState(m_hDownload))
	{
		case AsynchDownload::STATE_PENDING:
			// download pending, so we continue polling
			return TRUE;

		case AsynchDownload::STATE_SUCCEEDED:
		{
			// file has transferred successfully, so we can proceed with a synchronous import/open operation
			if (pParam->Import)
			{
				if (m_pTargetDoc != Document::GetSelected())
				{
					m_pCurrentSelDoc = Document::GetSelected();
					Document::SetSelectedViewAndSpread(m_pTargetDoc);
					// If the target document is not selected yet we keep on polling until it is  
					if (m_pTargetDoc != Document::GetSelected()) 
						return TRUE;
				}
				OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpClipartImport));
				if (pOpDesc != NULL)
				{
					ClipartImportParam Param;
					// Set up the parameters which we require to import the clipart
					Param.File = &(pParam->File);
					Param.Import = pParam->Import;
					Param.Result = TRUE;
					if (pParam->bDropped) // if the file has been dropped on the page, get positioning informatiom
						Param.DropInfo =  &(pParam->DropInfo);
					pOpDesc->Invoke((OpParam*) &Param);
					BOOL ok = Param.Result;
					if (!ok)
					{
						ERROR3("LibClipartSGallery::ImportClipart - Problem importing file");
					}
				}
				if (m_pCurrentSelDoc != m_pTargetDoc)
					Document::SetSelectedViewAndSpread(m_pCurrentSelDoc);
			}
			else // open the file as new document
			{
				CWinApp* pApp = AfxGetApp();
				BaseFileDialog::SelectedFilter = 0;
				// Open a document
				CCamDoc* pDoc = (CCamDoc*) pApp->OpenDocumentFile((TCHAR*) String_256(pParam->File.GetPath()));
				// And redraw the imported document
				if(pDoc) pDoc->GetKernelDoc()->ForceRedraw();
			}
			if (LibFillsSGallery::ThisGallery)
				LibFillsSGallery::ThisGallery->SelectionHasChanged();
			SucceedAndDiscard();
			End();
			return FALSE;
		}

		case AsynchDownload::STATE_FAILED:
		{
			String_256 strTemp;
			strTemp.MakeMsg(_R(IDS_DOWNLOADFAILED), (TCHAR*) (String_256) GetStringField((UINT32) SGLib_ClipArt, _R(IDS_GALLERY_DESCRIPTIONS)), (TCHAR*) pParam->strDescription);
			Error::SetError(0, strTemp, 0);
			InformError();
			goto FAIL;
		}

		case AsynchDownload::STATE_ABORTED:
		{
			// no errror message as this state is the result of a user cancel
			goto FAIL;
		}

		default:
			ERROR3("Error or unrecognized state");
			goto FAIL;
	}

FAIL:
	FailAndDiscard();
	End();
	return FALSE;
}
			



// OpAddWebFolders
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL OpAddWebFolders::Success[8];


OpAddWebFolders::OpAddWebFolders()
{
	pParam = NULL;
	GetApplication()->RegisterIdleProcessor(IDLEPRIORITY_LOW, this);
	OpFlags.HasOwnTimeIndicator = TRUE;
	m_hDownload = 0;
}       

/********************************************************************************************

>	OpAddWebFolders::~OpAddWebFolders() 

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpAddWebFolders destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpAddWebFolders::~OpAddWebFolders()
{
	GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_LOW, this);
	if (pParam)
		delete pParam;
}

/******************************************************************************************

>	BOOL OpAddWebFolders::Init()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for an OpAddWebFolders operation
	SeeAlso:

******************************************************************************************/

BOOL OpAddWebFolders::Init()
{  
	return RegisterOpDescriptor(
		0,								// Tool ID
 		_R(IDS_OPADDWEBFOLDERS),				// String resource ID
		CC_RUNTIME_CLASS(OpAddWebFolders),	// Runtime class
 		OPTOKEN_OPADDWEBFOLDERS,			// Token string
 		OpAddWebFolders::GetState,			// GetState function
		0,								// Help ID
		0,								// Bubble ID
		0,								// Resource ID
		0								// Control ID
	);
}   

/******************************************************************************************

>	OpState	OpAddWebFolders::GetState(String_256* pString, OpDescriptor* pOpDesc)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96
	Purpose:	Returns the OpState of the OpAddWebFolders operation.
	SeeAlso:

******************************************************************************************/

OpState	OpAddWebFolders::GetState(String_256* pString, OpDescriptor* pOpDesc)
{
	OpState OpSt;
	return(OpSt);
}

/********************************************************************************************

>	void OpAddWebFolders::Do(OpDescriptor*)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		OpAddWebFolders (unused)
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		Always fails - should not be called
	SeeAlso:	-

********************************************************************************************/
			
void OpAddWebFolders::Do(OpDescriptor *NotUsed)
{
	ERROR3("OpAddWebFolders does not provide a Do() function - Use DoWithParam");
	End();
}

/******************************************************************************************
>	void OpAddWebFolders::DoWithParam(OpDescriptor* pOp, OpParam* pAddWebFoldersParam)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		pOp - OpDescriptor as for all Do() functions
					pAddWebFoldersParam - pointer to  AddWebFoldersParam structure
	Outputs:	- none
	Returns:	-
	Purpose:	attempts to download asynchronously the main web clipart index file, and if this succeeds
					it goes on to parse it and launch separate downloads for each of the catalog files referenced
					by the index
	

******************************************************************************************/

void OpAddWebFolders::DoWithParam(OpDescriptor* pOp, OpParam* pAddWebFoldersParam)
{
	AddWebFoldersParam *pInfo = (AddWebFoldersParam*) pAddWebFoldersParam;
	if (pInfo == NULL)
	{
		ERROR3("OpAddWebFolders called with NULL info pointer");
		goto FAIL;
	}
	else
	{
		if (!pInfo->localFile.IsValid() || pInfo->strURL.IsEmpty() || !pInfo->pLibGal)
		{
			ERROR3("OpAddWebFolders: invalid parameters");
			goto FAIL;
		}
		// Register the file for download
		DOWNLOADINFO downloadInfo;
		downloadInfo.strURL = pInfo->strURL;
		downloadInfo.strLocalFile = (String_256) pInfo->localFile.GetPath();
		downloadInfo.nFileType = TYPE_CATALOG;
		downloadInfo.nPriority = AsynchDownload::PRIORITY_NORMAL;
		downloadInfo.bHasProgressDlg = TRUE;
		downloadInfo.hwndNotifyWindow = 0;
		downloadInfo.lNotifyToken = 0;
		m_hDownload = InternetManager::RegisterDownload(&downloadInfo);
		if (!m_hDownload || m_hDownload == (DOWNLOAD_HANDLE) INVALID_HANDLE_VALUE)
		{
			InformError(_R(IDS_CATALOGDOWNLOADFAILED), _R(IDS_OK));
			goto FAIL;
		}
		pParam = pInfo;
	}
	// Make some optimistic assumptions...
	Success[pParam->type] = TRUE; 
	return;

FAIL:
	Success[pParam->type] = FALSE; 
	FailAndExecute(); 
	End();
}

/******************************************************************************************
>	virtual BOOL OpAddWebFolders::OnDeathMsg()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		-
	Outputs:	- 
						
	Returns:	- always TRUE
	Purpose:	- aborts the download (so that we won't leak resources) 
						and ends the op on Webster exit

******************************************************************************************/
BOOL OpAddWebFolders::OnDeathMsg()
{
	InternetManager::UnregisterDownload(m_hDownload);
	End();
	return TRUE;
}



/******************************************************************************************
>	virtual BOOL OpAddWebFolders::OnIdleEvent()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		-
	Outputs:	- 
						
	Returns:	- TRUE if we want to continue polling, FALSE otherwise
	Purpose:	- idle time polling loop; continues until either the download completes
						or the operation is terminated on receiving a death message.
						Different actions are taken according to the state of the download
						on completion (succeeded/aborted/failed)

******************************************************************************************/

BOOL OpAddWebFolders::OnIdleEvent()
{
	ERROR2IF(!pParam, FALSE, "Unexpected NULL pointer");
	CCDiskFile mainIndex;
	switch (InternetManager::GetDownloadState(m_hDownload))
	{
		case AsynchDownload::STATE_PENDING:
			// download pending, so we continue polling
			return TRUE;

		case AsynchDownload::STATE_SUCCEEDED:
		{
			// file has transferred successfully, so we can go on and parse it
			if (!mainIndex.open(pParam->localFile,ios::in))
			{
				// I/O error - no need to inform the user as CCDiskFile is error reporting by default
				goto FAIL;
			}
			if (!mainIndex.InitLexer())
			{
				String_256 strErrorMsg;
				if (pParam->type == SGLib_ClipArt || pParam->type == SGLib_ClipArt_WebThemes)
					strErrorMsg = _T("Failed to parse web clipart index file - web folders cannot be updated");
				else if (pParam->type == SGLib_Font)
					strErrorMsg = _T("Failed to parse web fonts index file - web folders cannot be updated");
				ERROR2(FALSE,strErrorMsg); 
				goto FAIL;
			}
			LexTokenType tokType = TOKEN_EOL;	
			// Initialise lexing routines, and aspects of the lexer
			mainIndex.SetDelimiters("\r\n");			// Set token delimiting characters
			mainIndex.SetCommentMarker('#');			// Set comment marker char
			mainIndex.SetWhitespace("\t");			// Set whitespace chars
			const TCHAR* TokenBuf = mainIndex.GetTokenBuf();	// Token buffer remains constant until lexer deinitialisation
			String_256 strFolderName("");
			String_256 strRelativePath("");
			String_256 strRootURL("");
			INT32 nFieldOffset = 0;
			INT32 nRecords = 0;
			while (tokType != TOKEN_EOF)
			{	
				mainIndex.GetToken();
				tokType = mainIndex.GetTokenType();
				if (tokType == TOKEN_NORMAL)
				{
					switch (nFieldOffset)
					{
						case 0: // first field
							strFolderName = TokenBuf;
							break;
						case 1: // second field
							strRelativePath = TokenBuf;
							break;
						case 2: // third field
							strRootURL = TokenBuf;
							break;
						default:
							InformError(_R(IDS_CORRUPTEDINDEXFILE), _R(IDS_OK));
							mainIndex.DeinitLexer();
							goto FAIL;
					}
					if (nFieldOffset == 2) // finished reading a line, so try adding the corresponding library
					{
						// append a slash to the root URL if necessary
						if (camStrstr((TCHAR*) strRootURL, _T("file://")))
						{
							if (strRootURL[strRootURL.Length() - 1] != _T('\\'))
								strRootURL += _T('\\');
						}
						else if (strRootURL[strRootURL.Length() - 1] != _T('/'))
							strRootURL += _T('/');
						Library::URLBase = strRootURL; // set the base URL (this is a static variable at present)
						OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpAddWebLibrary));
						if (pOpDesc != NULL)
						{
							AddWebFoldersParam* Param = NULL;
							try
							{
								Param = new AddWebFoldersParam;
							}
							catch (CMemoryException* pExc)
							{
								pExc->Delete();
								Error::SetError(_R(IDS_OUT_OF_MEMORY));
								InformError();
								mainIndex.DeinitLexer();
								goto FAIL;
							}
							// Set up the web library's param
							Param->localFile.SetPathName((TCHAR *) MakeLocalPath(strRelativePath)); // local path of catalog file
							// We change the file extention from .txt to .tmp to avoid overwriting the old one - if any - so that we can compare
							// their sizes and figure out if the web catalog has been modified since the last download
							ERROR3IF(Param->localFile.GetType() != _T("txt") && Param->localFile.GetType() != _T("TXT"),
								"Catalog files are expected to have the .txt extention");
							Param->localFile.SetType(String_32(_T("tmp")));
							Param->strURL = MakeURL(strRelativePath, strRootURL); // URL of catalog file on our web server
							Param->pLibGal = pParam->pLibGal; // pass on the pointer to the gallery
							Param->strFolderName = strFolderName;
							Param->type = pParam->type; // pass on the gallery type
							// Invoke the operation
							// This will initiate a download of the catalog file - if it succeeds, the folder will be added to the gallery
							pOpDesc->Invoke((OpParam*) Param);
						}					
						nRecords++; // we've gone through another record
						nFieldOffset = 0; // reset field pointer
					}
					else
					{
						nFieldOffset++;
					}
				}
				else if (tokType == TOKEN_EOL)
				{
					// if the field pointer is not 0, the previous line must have had either less or more than 3 fields - the
					// file might have been corrupted or contains an error, so we skip the rest
					if (nFieldOffset)
					{
						InformError(_R(IDS_CORRUPTEDINDEXFILE), _R(IDS_OK));
						mainIndex.DeinitLexer();
						goto FAIL;
					}
				}
			}
			// We are now finished with the lexer
			mainIndex.DeinitLexer();
			mainIndex.close();
			// remove the file as we don't need it anymore. Using remove() is OK in this case as the file is not in the cache
			_tremove((TCHAR*) String_256(pParam->localFile.GetPath()));
			if 	(!nRecords) // eof reached without coming across any records - a bit fishy
			{
				InformError(_R(IDS_CORRUPTEDINDEXFILE), _R(IDS_OK));
				goto FAIL;
			}
			// if we got this far we can assume we succeeded
			SucceedAndDiscard();
			End();
			return FALSE;
		}

		case AsynchDownload::STATE_FAILED:
		{
			String_256 strErrorMsg;
			strErrorMsg.MakeMsg(_R(IDS_CATALOGDOWNLOADFAILED), (TCHAR*) (String_256) GetStringField((UINT32) pParam->type, _R(IDS_GALLERY_DESCRIPTIONS)),
					(TCHAR*) (String_256) GetStringField((UINT32) pParam->type, _R(IDS_GETX_CAPTIONS)));
			Error::SetError(0, strErrorMsg, 0);
			InformError();
			goto FAIL;
		}

		case AsynchDownload::STATE_ABORTED:
		{
			// no errror message as this state is the result of a user cancel
			goto FAIL;
		}

		default:
			ERROR3("Error or unrecognized state");
			goto FAIL;
	}

FAIL:
	// Make sure the file is closed so we can delete it
	if (mainIndex.isOpen())
		mainIndex.close();
	_tremove((TCHAR*) String_256(pParam->localFile.GetPath()));
	Success[pParam->type] = FALSE; 
	FailAndDiscard();
	End();
	return FALSE;
}
			

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OpAddWebLibrary
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INT32 OpAddWebLibrary::InstanceCount[8];
INT32 OpAddWebLibrary::FoldersAdded[8];
INT32 OpAddWebLibrary::ErrorCount[8];
INT32 OpAddWebLibrary::CachedFilesUsed[8];
BOOL OpAddWebLibrary::m_bIsSuspended = FALSE;
std::list<DOWNLOAD_HANDLE> OpAddWebLibrary::m_lstPendingDownloads;


OpAddWebLibrary::OpAddWebLibrary()
{
	// The instance count is incremented in DoWithParam and not here, as at this point the type of the op is not known
	pParam = NULL;
	GetApplication()->RegisterIdleProcessor(IDLEPRIORITY_LOW, this);
	m_hDownload = 0;
	OpFlags.HasOwnTimeIndicator = TRUE;
}       

/********************************************************************************************

>	OpAddWebLibrary::~OpAddWebLibrary() 

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpAddWebLibrary destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpAddWebLibrary::~OpAddWebLibrary()
{
	GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_LOW, this);
	if (pParam)
	{
		InstanceCount[pParam->type]--;
		delete pParam;
	}
}

/******************************************************************************************

>	BOOL OpAddWebLibrary::Init()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for an OpAddWebLibrary operation
	SeeAlso:

******************************************************************************************/

BOOL OpAddWebLibrary::Init()
{  

	return RegisterOpDescriptor(
		0,								// Tool ID
 		_R(IDS_OPADDWEBLIBRARY),				// String resource ID
		CC_RUNTIME_CLASS(OpAddWebLibrary),	// Runtime class
 		OPTOKEN_OPADDWEBLIBRARY,			// Token string
 		OpAddWebLibrary::GetState,			// GetState function
		0,								// Help ID
		0,								// Bubble ID
		0,								// Resource ID
		0								// Control ID
	);
}   

/******************************************************************************************

>	OpState	OpAddWebLibrary::GetState(String_256* pString, OpDescriptor* pOpDesc)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96
	Purpose:	Returns the OpState of the OpAddWebLibrary operation.
	SeeAlso:

******************************************************************************************/

OpState	OpAddWebLibrary::GetState(String_256* pString, OpDescriptor* pOpDesc)
{
	OpState OpSt;
	return(OpSt);
}

/********************************************************************************************

>	void OpAddWebLibrary::Do(OpDescriptor*)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		OpAddWebLibrary (unused)
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		Always fails - should not be called
	SeeAlso:	-

********************************************************************************************/
			
void OpAddWebLibrary::Do(OpDescriptor *NotUsed)
{
	ERROR3("OpAddWebLibrary does not provide a Do() function - Use DoWithParam");
	End();
}

/******************************************************************************************
>	void OpAddWebLibrary::DoWithParam(OpDescriptor* pOp, OpParam* pAddWebFoldersParam)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		pOp - OpDescriptor as for all Do() functions
					pAddWebFoldersParam - pointer to  AddWebFoldersParam structure
	Outputs:	- none
	Returns:	-
	Purpose:	attempts to download asynchronously the library's catalog (xaraclip.txt) file - in case
					of success a corresponding folder will be added to the clipart gallery
	

******************************************************************************************/

void OpAddWebLibrary::DoWithParam(OpDescriptor* pOp, OpParam* pAddWebFoldersParam)
{
	AddWebFoldersParam* pInfo = (AddWebFoldersParam*) pAddWebFoldersParam;
	if (pInfo == NULL || m_bIsSuspended) 
	{
		ERROR3IF(!pInfo, "OpAddWebLibrary called with NULL info pointer");
		goto FAIL;
	}
	else
	{
		if (!pInfo->localFile.IsValid() || pInfo->strURL.IsEmpty() || pInfo->strFolderName.IsEmpty() || !pInfo->pLibGal)
		{
			ERROR3("OpAddWebLibrary: invalid parameters");
			goto FAIL;
		}
		// Register the file for download
		String_256 strDescription(GetStringField((UINT32) pInfo->type, _R(IDS_GALLERY_DESCRIPTIONS)));
		strDescription += String_32(_R(IDS_CATALOGFILE));
		DOWNLOADINFO downloadInfo;
		downloadInfo.strURL = pInfo->strURL;
		downloadInfo.strLocalFile = (String_256) pInfo->localFile.GetPath();
		downloadInfo.nFileType = TYPE_CATALOG;
		downloadInfo.nPriority = AsynchDownload::PRIORITY_NORMAL;
		downloadInfo.strDescription = strDescription;
		downloadInfo.bHasProgressDlg = TRUE;
		downloadInfo.hwndNotifyWindow = 0;
		downloadInfo.lNotifyToken = 0;
		m_hDownload = InternetManager::RegisterDownload(&downloadInfo);
		if (!m_hDownload || m_hDownload == (DOWNLOAD_HANDLE) INVALID_HANDLE_VALUE)
		{
			String_256 strErrorMsg;
			strErrorMsg.MakeMsg(_R(IDS_CATALOGDOWNLOADFAILED), (TCHAR*) (String_256) GetStringField((UINT32) pInfo->type, _R(IDS_GALLERY_DESCRIPTIONS)),
					(TCHAR*) (String_256) GetStringField((UINT32) pInfo->type, _R(IDS_GETX_CAPTIONS)));
			Error::SetError(0, strErrorMsg, 0);
			InformError();
		}
		else
			m_lstPendingDownloads.push_back(m_hDownload);
		pParam = pInfo;
		InstanceCount[pParam->type]++; // update instance count
		if (InstanceCount[pParam->type] == 1) // first instance, reset counters
		{
			ErrorCount[pParam->type] = 0; 
			FoldersAdded[pParam->type] = 0;
			CachedFilesUsed[pParam->type] = 0;
		}
	}
	return;
FAIL:
		OpAddWebFolders::Success[pParam->type] = FALSE; 
		FailAndExecute(); 
		End();
}


/******************************************************************************************
>	virtual BOOL OpAddWebLibrary::OnDeathMsg()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		-
	Outputs:	- 
						
	Returns:	- always TRUE
	Purpose:	- aborts the download (so that we won't leak resources) 
						and ends the op on Webster exit

******************************************************************************************/
BOOL OpAddWebLibrary::OnDeathMsg()
{
	InternetManager::UnregisterDownload(m_hDownload);
	End();
	return TRUE;
}



/******************************************************************************************
>	virtual BOOL OpAddWebLibrary::OnIdleEvent()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		-
	Outputs:	- 
						
	Returns:	- TRUE if we want to continue polling, FALSE otherwise
	Purpose:	- idle time polling loop; continues until either the download completes
						or the operation is terminated on receiving a death message.
						Different actions are taken according to the state of the download
						on completion (succeeded/aborted/failed)

******************************************************************************************/


BOOL OpAddWebLibrary::OnIdleEvent()
{
	ERROR2IF(!pParam, FALSE, "Unexpected NULL pointer");
	if (m_bIsSuspended)
		return FALSE;
	UINT32 nModifiedFlag = FOLDER_UNMODIFIED; // flag indicating folder's status (FOLDER_UNMODIFIED/FOLDER_UPDATED/FOLDER_NEW)
	BOOL bHaveCatalogFile = FALSE; // flag indicating if we should add this folder to the gallery
	PathName localFile(pParam->localFile); // path to local (current) catalog file (may not exist if this is a new folder)
	localFile.SetType(String_32(_T("txt"))); // should be a .txt file
	PathName oldFile(pParam->localFile); // path to old catalog file (may not exist if this is a new folder)
	oldFile.SetType(String_32(_T("old")));

	LibraryGallery* pGallery = pParam->pLibGal;
	switch (InternetManager::GetDownloadState(m_hDownload))
	{
		case AsynchDownload::STATE_PENDING:
			// download pending, so we continue polling
			return TRUE;
		
		case AsynchDownload::STATE_SUCCEEDED:
		{
			// Check if the folder exists
			BOOL bFolderExists = (pGallery->FindLibrary((String_256) localFile.GetPath())) ? TRUE : FALSE;
			// File has transferred successfully, check if we have an older copy
			if (_taccess((String_256) localFile.GetPath(), 0) == -1) // not  found
			{
				if (DownloadCache::RenameFile((String_256) pParam->localFile.GetPath(), (String_256) localFile.GetPath()))
				{
					InformError(_R(IDS_LOCALFILEERROR), _R(IDS_OK));
					goto FAIL; // couldn't rename it for some reason
				}
				else
				{
					bHaveCatalogFile = TRUE;
					nModifiedFlag = FOLDER_NEW;
				}
			}
			else // we already have an older version of this file, so we compare the sizes to see if the web version is different
			{
				CCDiskFile currentVersion, newVersion;
				if (!currentVersion.open(localFile, ios::in) || !newVersion.open(pParam->localFile, ios::in))
				{
					// Cannot compare sizes due to some I/O error, so in case one of the files is good we'll just use that
					BOOL bCurrentVersionBad = currentVersion.bad(); 
					BOOL bNewVersionBad = newVersion.bad(); 
					currentVersion.close();
					newVersion.close();
					if (!bNewVersionBad)
					{
						// move the new version over the old one
						if (DownloadCache::RemoveFile((String_256) localFile.GetPath())) // failed - can't move
						{
								InformError(_R(IDS_LOCALFILEERROR), _R(IDS_OK));
								DownloadCache::RemoveFile((String_256) pParam->localFile.GetPath());
								goto FAIL;
						}
						else // rename the new version 
						{
							if (DownloadCache::RenameFile((String_256) pParam->localFile.GetPath(), (String_256) localFile.GetPath()))
							{
								InformError(_R(IDS_LOCALFILEERROR), _R(IDS_OK));
								goto FAIL; // couldn't rename it for some reason
							}
							else
							{
								bHaveCatalogFile = TRUE;
								if (!bFolderExists)
									nModifiedFlag = FOLDER_NEW;
							}
						}
					}
					else if (!bCurrentVersionBad)
					{
						// the downloaded catalog file may be corrupted, so delete it and use the current one
						DownloadCache::RemoveFile((String_256) pParam->localFile.GetPath());
						bHaveCatalogFile = TRUE;
						if (!bFolderExists)
							nModifiedFlag = FOLDER_NEW;
					}
					else // none of the files can be opened - just clean up and inform the error
					{
						DownloadCache::RemoveFile((String_256) pParam->localFile.GetPath());
						InformError(_R(IDS_LOCALFILEERROR), _R(IDS_OK));
						goto FAIL; 
					}
				}
				else
				{
					if (!bFolderExists)
						nModifiedFlag = FOLDER_NEW;
					else if (newVersion.Size() != currentVersion.Size())
						nModifiedFlag = FOLDER_UPDATED;
					currentVersion.close();
					newVersion.close();
					DownloadCache::RemoveFile((String_256) oldFile.GetPath());
					if (DownloadCache::RenameFile((String_256) localFile.GetPath(), (String_256) oldFile.GetPath())) // failed - can't move
					{
							InformError(_R(IDS_LOCALFILEERROR), _R(IDS_OK));
							DownloadCache::RemoveFile((String_256) pParam->localFile.GetPath());
							goto FAIL;
					}
					else // rename the new version
					{
						if (DownloadCache::RenameFile((String_256) pParam->localFile.GetPath(), (String_256) localFile.GetPath()))
						{
							InformError(_R(IDS_LOCALFILEERROR), _R(IDS_OK));
							goto FAIL; // couldn't rename it for some reason
						}
						else
						{
							bHaveCatalogFile = TRUE;
						}
					}
				}
			}
		}
		break;

		case AsynchDownload::STATE_FAILED:
		{
			ErrorCount[pParam->type]++;
			OpAddWebFolders::Success[pParam->type] = FALSE; 
			// If we have on older catalog file we'll use that, otherwise we'll FAIL
			if (_taccess((String_256) localFile.GetPath(), 0) == -1) // not  found
			{
				goto FAIL;
			}
			else 
			{
				CachedFilesUsed[pParam->type]++;
				bHaveCatalogFile = TRUE;
			}
		}
		break;

		case AsynchDownload::STATE_ABORTED:
		{
			// no errror message as this state is the result of a user cancel
			Suspend(); // ensure we are not reentered while we remove pending downloads
			std::list<DOWNLOAD_HANDLE>::iterator listIterator = m_lstPendingDownloads.begin();
			INT32 nListSize = m_lstPendingDownloads.size();
			for (INT32 i = 0; i < nListSize; i++)
				InternetManager::UnregisterDownload(*listIterator++);
			Resume();
			goto FAIL;
		}

		default:
			ERROR3("Error or unrecognized state");
			goto FAIL;
	}
	// Add/update the folder if everything's fine
	if (bHaveCatalogFile && (nModifiedFlag == FOLDER_UPDATED || nModifiedFlag == FOLDER_NEW)) 
	{
		LibraryFile* LibFile = NULL;
		try
		{
			LibFile  = new LibraryFile;
		}
		catch (CMemoryException* pExc)
		{
			pExc->Delete();
			Error::SetError(_R(IDS_OUT_OF_MEMORY));
			InformError();
			goto FAIL;
		}
		String_256 strFileName(localFile.GetFileName());
		PathName FilesPath(localFile.GetLocation());
		pGallery->RemoveSimilarLibraryGroups(&FilesPath, &strFileName, TRUE, FALSE, NULL);
		if (!LibFile->AddSpecificIndex(pGallery, &localFile, pParam->type, &pParam->strFolderName, nModifiedFlag, TRUE))
		{
			delete LibFile;
			goto FAIL;
		}
		else
		{
			pGallery->OpenLibFiles.AddTail(LibFile);
			LibFile->DoScrollRedraw();
			FoldersAdded[pParam->type]++;
		}
		// If we got this far we can assume we succeeded
		SucceedAndDiscard();
		goto END;
	}
	else
	{
		goto END;
	}

FAIL:
	OpAddWebFolders::Success[pParam->type] = FALSE; 
	FailAndDiscard();

END:
	// inform errors, if any
	if (InstanceCount[pParam->type] == 1 && ErrorCount[pParam->type])
	{
		String_256 strErrorMsg;
		if (FoldersAdded[pParam->type]) // partial failure (we have added some of the folders)
		{
			if (CachedFilesUsed[pParam->type])
				strErrorMsg.MakeMsg(_R(IDS_CATALOGPARTIALUSECACHE), (TCHAR*) (String_256) GetStringField((UINT32) pParam->type, _R(IDS_GALLERY_DESCRIPTIONS)),
					(TCHAR*) (String_256) GetStringField((UINT32) pParam->type, _R(IDS_GETX_CAPTIONS)));
			else
				strErrorMsg.MakeMsg(_R(IDS_CATALOGDOWNLOADPARTIALFAIL), (TCHAR*) (String_256) GetStringField((UINT32) pParam->type, _R(IDS_GALLERY_DESCRIPTIONS)),
					(TCHAR*) (String_256) GetStringField((UINT32) pParam->type, _R(IDS_GETX_CAPTIONS)));
		}
		else
		{
			if (CachedFilesUsed[pParam->type])
				strErrorMsg.MakeMsg(_R(IDS_CATALOGUSECACHE), (TCHAR*) (String_256) GetStringField((UINT32) pParam->type, _R(IDS_GALLERY_DESCRIPTIONS)),
					(TCHAR*) (String_256) GetStringField((UINT32) pParam->type, _R(IDS_GETX_CAPTIONS)));
			else // failed completely...
				strErrorMsg.MakeMsg(_R(IDS_CATALOGDOWNLOADFAILED), (TCHAR*) (String_256) GetStringField((UINT32) pParam->type, _R(IDS_GALLERY_DESCRIPTIONS)),
					(TCHAR*) (String_256) GetStringField((UINT32) pParam->type, _R(IDS_GETX_CAPTIONS)));
		}
		Error::SetError(0, strErrorMsg, 0);
		InformError();
	}
	m_lstPendingDownloads.remove(m_hDownload);
	End();
	return FALSE;
}
			
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Thumbnail downloading
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INT32 OpThumbDownload::ErrorCount[8];
INT32 OpThumbDownload::ThumbsDownloaded[8];
INT32 OpThumbDownload::InstanceCount[8];
BOOL OpThumbDownload::m_bIsSuspended = FALSE;
std::list<SGLibDisplayItem*> OpThumbDownload::m_lstFailedItems;


ThumbDownloadParam::ThumbDownloadParam()
{
	pItem = NULL;
	pOp = NULL;
	bSuccess = FALSE;
	type = SGLib_Blank;
}

ThumbDownloadParam::~ThumbDownloadParam()
{
}


OpThumbDownload::OpThumbDownload()
{
	pParam = NULL;
	m_nPercentageCompleted = 0;
	m_hDownload = 0;
	pGallery = NULL;
	GetApplication()->RegisterIdleProcessor(IDLEPRIORITY_LOW, this);
	OpFlags.HasOwnTimeIndicator = TRUE;
}       

/********************************************************************************************

>	OpThumbDownload::~OpThumbDownload() 

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpThumbDownload destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpThumbDownload::~OpThumbDownload()
{
	GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_LOW, this);
	if (pParam)
	{
		InstanceCount[pParam->type]--;
		delete pParam;
	}
}

/******************************************************************************************

>	BOOL OpThumbDownload::Init()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for an OpThumbDownload operation
	SeeAlso:

******************************************************************************************/

BOOL OpThumbDownload::Init()
{  
	return RegisterOpDescriptor(
		0,								// Tool ID
 		_R(IDS_OPTHUMBDOWNLOAD),				// String resource ID
		CC_RUNTIME_CLASS(OpThumbDownload),	// Runtime class
 		OPTOKEN_OPTHUMBDOWNLOAD,			// Token string
 		OpThumbDownload::GetState,			// GetState function
		0,								// Help ID
		0,								// Bubble ID
		0,								// Resource ID
		0								// Control ID
	);
}   

/******************************************************************************************

>	OpState	OpThumbDownload::GetState(String_256* pString, OpDescriptor* pOpDesc)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Purpose:	Returns the OpState of the OpThumbDownload operation.
	SeeAlso:

******************************************************************************************/

OpState	OpThumbDownload::GetState(String_256* pString, OpDescriptor* pOpDesc)
{
	OpState OpSt;
	return(OpSt);
}




/********************************************************************************************

>	void OpThumbDownload::Do(OpDescriptor*)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/97
	Inputs:		 (unused)
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		Always fails - should not be called
	SeeAlso:	-

******************************************************************************************/
		
void OpThumbDownload::Do(OpDescriptor *NotUsed)
{
	ERROR3("OpThumbDownload does not provide a Do() function - Use DoWithParam");
	End();
}

/******************************************************************************************
>	void OpThumbDownload::DoWithParam(OpDescriptor* pOp, OpParam* pThumbDownloadParam)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		pOp - OpDescriptor as for all Do() functions
				pThumbDownloadParam - pointer to  ThumbDownloadParam structure . This structure
					must be created on the heap as it should not be destroyed when it goes
					out of scope.
	Outputs:	-  none
						Note that this function returns immediately after starting the download, so its success is not relevant
						to the outcome of the download operation.
	Returns:	-
	Purpose:	- starts a thumbnail download

******************************************************************************************/

void  OpThumbDownload::DoWithParam(OpDescriptor* pOp, OpParam* pThumbDownloadParam)
{
	ThumbDownloadParam* pInfo = (ThumbDownloadParam*) pThumbDownloadParam;

	if (pInfo == NULL || pInfo->pItem == NULL)
	{
		ERROR3("OpThumbDownload called with NULL info pointer");
		End();
	}
	else
	{
		String_256 thumbPath;
		if (!pInfo->pItem->GetThumbFileName(&thumbPath))
		{
			ERROR3("Can't get thumbnail file name");
			goto FAIL;
		}
		String_256 thumbNetPath = thumbPath;
		Library* pLibrary = pInfo->pItem->GetParentLibrary();
		if (!pLibrary)
		{
			ERROR3("Illegal NULL pointer");
			goto FAIL;
		}
		if (!pLibrary->IsWebLibrary())
			goto FAIL;
		pLibrary->LocalPath2URL(&thumbNetPath);
		// Register the file for download
		DOWNLOADINFO downloadInfo;
		downloadInfo.strURL = thumbNetPath;
		downloadInfo.strLocalFile = thumbPath;
		downloadInfo.nFileType = TYPE_THUMBNAIL;
		downloadInfo.nPriority = AsynchDownload::PRIORITY_HIGH;
		downloadInfo.bHasProgressDlg = FALSE;
		downloadInfo.hwndNotifyWindow = 0;
		downloadInfo.lNotifyToken = 0;
		m_hDownload = InternetManager::RegisterDownload(&downloadInfo);
		if (!m_hDownload || m_hDownload == (DOWNLOAD_HANDLE) INVALID_HANDLE_VALUE)
		{
			InformWarning(_R(IDS_THUMBDOWNLOADFAILED), _R(IDS_OK));
			if (pGallery = (LibraryGallery*) pInfo->pItem->GetParentGallery())
				pGallery->bThumbDownloading = FALSE; // turn off thumb downloading so we don't flood the user with error messages 
			goto FAIL;
		}
		pParam = pInfo;
		if (pGallery = (LibraryGallery*) pInfo->pItem->GetParentGallery())
			pGallery->m_lstPendingThumbs.push_back(this);
		pInfo->pOp = this;
		pInfo->bSuccess = TRUE;
		InstanceCount[pParam->type]++;
		// thumb downloads occur in "bursts", so when a new burst begins we must reset the error and download counters
		// so that we don't get confused by previous errors
		if (InstanceCount[pParam->type] == 1)
		{
			ErrorCount[pParam->type] = 0;
			ThumbsDownloaded[pParam->type] = 0;
			pGallery->SelectionHasChanged();
		}
		return;
	}
FAIL:
		FailAndExecute(); 
		End();
}

/********************************************************************************************

>	void  OpThumbDownload::End()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Ends the download operation, displaying the thumbnail if successful 
				
	Errors:		-
	SeeAlso:	Operation::End()

********************************************************************************************/

void  OpThumbDownload::End()
{
	// Abort the download if still pending - this can only happen if the item is being destroyed
	if (m_hDownload && InternetManager::GetDownloadState(m_hDownload) ==  AsynchDownload::STATE_PENDING)
		InternetManager::UnregisterDownload(m_hDownload);
	LibraryGallery* pGallery = (LibraryGallery*) pParam->pItem->GetParentGallery();
	if (pGallery)
	{
		pGallery->m_lstPendingThumbs.remove(this);
		if (InstanceCount[pParam->type] == 1)
			pGallery->SelectionHasChanged();
	}
	if (pParam && pParam->pItem) // tell the item that the thumb download is finished
		pParam->pItem->OnThumbDownloadComplete();
	Operation::End();
}	



/********************************************************************************************

>	void  OpThumbDownload::Abort()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Aborts the download operation
				
	Errors:		-
	SeeAlso:	Operation::End()

********************************************************************************************/

void  OpThumbDownload::Abort()
{
	if (m_hDownload && InternetManager::GetDownloadState(m_hDownload) ==  AsynchDownload::STATE_PENDING)
		InternetManager::UnregisterDownload(m_hDownload);
	LibraryGallery* pGallery = (LibraryGallery*) pParam->pItem->GetParentGallery();
	if (pGallery)
	{
		pGallery->m_lstPendingThumbs.remove(this);
		if (InstanceCount[pParam->type] == 1)
			pGallery->SelectionHasChanged();
	}
	Operation::End();
}	



/******************************************************************************************
>	virtual BOOL OpThumbDownload::OnIdleEvent()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		-
	Outputs:	- 
						
	Returns:	- TRUE if we want to continue polling, FALSE otherwise
	Purpose:	- idle time polling loop; continues until either the download completes
						or the operation is terminated by a call to End().
						Different actions are taken according to the state of the download
						on completion (succeeded/aborted/failed)

******************************************************************************************/

BOOL OpThumbDownload::OnIdleEvent()
{
	ERROR2IF(!pParam, FALSE, "Unexpected NULL pointer");
	if (m_bIsSuspended) // if the Op is suspended we don't do anything
		return FALSE;
	switch (InternetManager::GetDownloadState(m_hDownload))
	{
		case AsynchDownload::STATE_PENDING:
		{
			// check the percentage downloaded so far
			INT32 nPercentageCompleted = InternetManager::GetPercentageDownloaded(m_hDownload);
			ERROR3IF(nPercentageCompleted == -1, "Invalid download handle");
			if (nPercentageCompleted != m_nPercentageCompleted) // we made some progress
			{
				m_nPercentageCompleted = nPercentageCompleted;
				 if (pParam && pParam->pItem) // tell the item to update itself
					 pParam->pItem->OnThumbDownloadProgress(nPercentageCompleted);
			}
			// the download is still pending, so we continue polling
			return TRUE;
		}
		case AsynchDownload::STATE_SUCCEEDED:
		{
			// We got lucky
			ThumbsDownloaded[pParam->type]++;
			SucceedAndDiscard();
			goto END;
		}
		case AsynchDownload::STATE_FAILED:
		{
			ErrorCount[pParam->type]++;
			m_lstFailedItems.push_back(pParam->pItem);
			goto FAIL;
		}
		case AsynchDownload::STATE_ABORTED:
		{
			// user cancel
			goto FAIL;
		}
		default:
			ERROR3("Error or unrecognized state");
			goto FAIL;
	}

FAIL:
	FailAndDiscard();
END:
	// Thumb downloads occur in "bursts", so we report errors only once per burst
	if (InstanceCount[pParam->type] == 1 && ErrorCount[pParam->type])
	{
		String_256 strErrorMsg;
		if (ThumbsDownloaded[pParam->type]) // partial failure
			strErrorMsg.MakeMsg(_R(IDS_THUMBDOWNLOADPARTIALFAIL), (TCHAR*) (String_256) GetStringField((UINT32) pParam->type, _R(IDS_GETX_CAPTIONS)));
		else // complete failure...
			strErrorMsg.MakeMsg(_R(IDS_THUMBDOWNLOADFAILED), (TCHAR*) (String_256) GetStringField((UINT32) pParam->type, _R(IDS_GETX_CAPTIONS)));

		// As something went wrong (most likely a connection problem) suspend thumb downloading. This means the 
		// user can scroll this gallery without more such error popups. Fixes,with LibraryGallery change, #10471
		pGallery->bThumbDownloading = FALSE;
		
		Error::SetError(0, strErrorMsg, 0);
		InformError();
	}
	End();
	return FALSE;
}
			

/******************************************************************************************
>	static INT32 OpThumbDownload::GetLastError(SGLibDisplayItem* pItem)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		- pointer to a SGLibDisplayItem
	Outputs:	- 
						
	Returns:	- an error code (DOWNLOAD_ERROR) if the item got an error while downloading 
					its thumb, 0 otherwise
	Purpose:	- used to decide whether or not we should reattempt a download - if the item got
					an error no further attempts are made before the error is cleared by the user

******************************************************************************************/

INT32 OpThumbDownload::GetLastError(SGLibDisplayItem* pItem)
 {
	if (m_lstFailedItems.empty())
		return 0;
	std::list<SGLibDisplayItem*>::iterator listIterator = m_lstFailedItems.begin();
	INT32 nListSize = m_lstFailedItems.size();
	for (INT32 i = 0; i < nListSize; i++)
		if (pItem == *listIterator++)
			return DOWNLOAD_ERROR;
	return 0;
}


/******************************************************************************************
>	static void OpThumbDownload::ClearErrors()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		- 
	Outputs:	- 
						
	Returns:	- 
	Purpose:	- resets the error counter and flushes the list of items with download problems,
						which will allow us to reattempt downloads for these items

******************************************************************************************/

void OpThumbDownload::ClearErrors()
{
	memset(&ErrorCount, 0x00, sizeof(ErrorCount));
	if (!m_lstFailedItems.empty())
	{
		std::list<SGLibDisplayItem*>::iterator itrBegin = m_lstFailedItems.begin();
		std::list<SGLibDisplayItem*>::iterator itrEnd = m_lstFailedItems.end();
		m_lstFailedItems.erase(itrBegin, itrEnd); // zapp the entire list
		ERROR3IF(!m_lstFailedItems.empty(), "Warning: failed to empty error list");
	}
}


// OpAsynchFontInstall 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


WebFontInstallParam::WebFontInstallParam()
{
	bIsTemp = FALSE;
	pGallery = NULL;
}

WebFontInstallParam::~WebFontInstallParam()
{
}


/********************************************************************************************
  >	BOOL OpAsynchFontInstall::Init()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/04/97
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for an WebFontInstall operation
	SeeAlso:

******************************************************************************************/


BOOL OpAsynchFontInstall::Init()
{  
	return RegisterOpDescriptor(
		0,								// Tool ID
 		_R(IDS_OPASYNCHFONTINSTALL),				// String resource ID
		CC_RUNTIME_CLASS(OpAsynchFontInstall),	// Runtime class
 		OPTOKEN_OPASYNCHFONTINSTALL,			// Token string
 		DownloadOp::GetState,			// GetState function
		0,								// Help ID
		0,								// Bubble ID
		0,								// Resource ID
		0								// Control ID
	);
}   


/******************************************************************************************
>	virtual void OpAsynchFontInstall::OnDownloadSuccess()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		-
	Outputs:	- 
						
	Returns:	- 
	Purpose:	- Performs the actual install using the font file just downloaded.

******************************************************************************************/

void OpAsynchFontInstall::OnDownloadSuccess()
{
	// Verify that the pointer is valid & the right type
	WebFontInstallParam* pInstallParam =  (WebFontInstallParam*) pParam;
	if (!pInstallParam->pGallery || !pInstallParam->pGallery->IsKindOf(CC_RUNTIME_CLASS(FontsSGallery)))
	{
		ERROR3("Invalid gallery pointer");
		return;
	}

	// if we have just downloaded a "pfb" file we need to download the
	// corresponding .pfm file
	String_256 FileExt((String_256)pInstallParam->file.GetType());
	FileExt.toLower();
	if (FileExt == String_256("pfb"))
	{
		OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpAsynchFontInstall));
		if (pOpDesc != NULL)
		{
			BOOL ok = TRUE;
			WebFontInstallParam* Param = new WebFontInstallParam;
			ERROR3IF(!Param, "Memory allocation error");
			if (!Param)
				return;

			// Set up the parameters which we need to download and install the font
			Param->type = TYPE_FONT;
			Param->priority = AsynchDownload::PRIORITY_HIGH;
			Param->pGallery = pInstallParam->pGallery;
			PathName FileName(pInstallParam->file);
			FileName.SetType("pfm");
			Param->file = FileName;
			Param->strDescription = pInstallParam->strDescription;
			Param->strAltDescription = pInstallParam->strAltDescription;

			// get the url address of the font
			String_256 URL;
			UINT32 Count = pInstallParam->strURL.Length() - FileName.GetFileName().Length();
			pInstallParam->strURL.Left(&Param->strURL, Count);

			// add the file name
			String_256 FName = FileName.GetFileName();
			FName.toLower();
			Param->strURL += FName;
			Param->bIsTemp = FALSE;
			Param->Output = &ok;

			// Invoke the operation
			pOpDesc->Invoke((OpParam*) Param);
			return;
		}
		else
		{
			ERROR3("Can't find Op descriptor");
		}
			return;
	}
	else if (FileExt == String_256("pfm"))
		pInstallParam->file.SetType(String_256("pfb"));
	
	FontsSGallery* pFontsGallery = (FontsSGallery*) pInstallParam->pGallery; // the cast should be safe at this point
	String_64 SlowJob;
	SlowJob.MakeMsg(_R(IDS_FONTS_INSTALLING), (TCHAR*) pInstallParam->strAltDescription);
	BeginSlowJob(-1, FALSE, &SlowJob);
	FontsSGallery::DontUpdate = TRUE;
	BOOL ok = pFontsGallery->InstallFont(&pInstallParam->file, &pInstallParam->strDescription, pInstallParam->bIsTemp);
	FontsSGallery::DontUpdate = FALSE;
	EndSlowJob();
	if (!ok)
	{
		// Problems installing a specified font
		String_256 ErrMsg;
		ErrMsg.MakeMsg(_R(IDS_PROBLEMS_INSTALLING), (TCHAR *) pInstallParam->strAltDescription);
		Error::SetError(0, ErrMsg, 0);
		InformError();
		Error::ClearError();
	}
	else
	{
		// Warn everyone that the fonts have changed - and update our gallery
		SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	}
	// remove the temporary font file
	DownloadCache::RemoveFile((String_256) pInstallParam->file.GetPath());
	pFontsGallery->SelectionHasChanged();
}




// OpAsynchBitmapImport 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


AsynchBitmapImportParam::AsynchBitmapImportParam()
{
	pTargetDoc		= 0;
	TagObjectToFill	= 0;
	TagObjectValid	= false;
	pObject			= 0;
	pObjectValid	= false;
}


/********************************************************************************************
  >	BOOL OpAsynchBitmapImport::Init()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/04/97
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for an AsynchBitmapImport operation
	SeeAlso:

******************************************************************************************/


BOOL OpAsynchBitmapImport::Init()
{  
	return RegisterOpDescriptor(
		0,								// Tool ID
 		_R(IDS_OPASYNCHBITMAPIMPORT),				// String resource ID
		CC_RUNTIME_CLASS(OpAsynchBitmapImport),	// Runtime class
 		OPTOKEN_ASYNCHBITMAPIMPORT,			// Token string
 		DownloadOp::GetState,			// GetState function
		0,								// Help ID
		0,								// Bubble ID
		0,								// Resource ID
		0								// Control ID
	);
}   


/******************************************************************************************
>	virtual void OpAsynchBitmapImport::OnDownloadSuccess()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Inputs:		-
	Outputs:	- 
						
	Returns:	- 
	Purpose:	- Performs - if required - the actual import using the bitmap file just downloaded.

******************************************************************************************/

void OpAsynchBitmapImport::OnDownloadSuccess()
{
	AsynchBitmapImportParam* pImportParam =  (AsynchBitmapImportParam*) pParam;

	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpBitmapImport));

	if (pOpDesc != 0)
	{
		BitmapImportParam Param;

		Param.File				= &pImportParam->file;
		Param.Result			= TRUE;
		Param.FillType			= pImportParam->FillType;
		Param.pTargetDoc		= pImportParam->pTargetDoc;
		Param.TagObjectToFill	= pImportParam->TagObjectToFill;
		Param.TagObjectValid	= pImportParam->TagObjectValid;
		Param.pObject			= pImportParam->pObject;
		Param.pObjectValid		= pImportParam->pObjectValid;
		Param.pSpread			= pImportParam->pSpread;
		Param.DropPos			= pImportParam->DropPos;

		// Do it...
		pOpDesc->Invoke((OpParam *) &Param);

		if (!Param.Result)
			ERROR3("Problem importing file from internet");
	}
}


/******************************************************************************************

>	BOOL OpAsynchBitmapImport::OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/97
	Returns:	- 
	Purpose:	Notifies us if the document we intend to import into is about to be destroyed,
					in which case we should abort the operation
	SeeAlso:	MessageHandler::OnDocChangingMsg()

******************************************************************************************/

BOOL OpAsynchBitmapImport::OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State)
{
	AsynchBitmapImportParam* pImportParam = (AsynchBitmapImportParam*) pParam;
	if (pChangingDoc == pImportParam->pTargetDoc && State == DocChangingMsg::ABOUTTODIE)
	{
		// We end the operation, but don't abort the clipart download - it's up to the user to do that
		FailAndExecute(); 
		End();
	}
	return TRUE;
}






