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
#include <fstream.h>
#include <io.h>
#include <stdlib.h>
#include <direct.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <process.h>
#include "sglcart.h"
#include "pathnmex.h"
#include "camnet.h"
#include "sgliboil.h"
//#include "webster.h"
#include "thumb.h"
#include "registry.h"
//#include "resource.h"
//#include "simon.h"
//#include "tim.h"
#include "app.h"
//#include "helpdownload.h"

using namespace InetUtils;

CC_IMPLEMENT_DYNAMIC(AsynchDownload, CCObject);
CC_IMPLEMENT_MEMDUMP(DownloadQueue, CCObject);
CC_IMPLEMENT_MEMDUMP(DownloadCache, CCObject);
CC_IMPLEMENT_MEMDUMP(InternetManager, CCObject);

#define new CAM_DEBUG_NEW

#define MAX_ATTEMPTS	5 // maximum no. of attempts made to download a file
#define ICON_ANIMATION		101 // timer event for icon animation
#define ICON_UPDATE_INTERVAL	300


// static members
CTypedPtrMap<CMapPtrToPtr, HWND, AsynchDownload::AsynchBindStatusCallback*> AsynchDownload::AsynchBindStatusCallback::m_CallbackTable; 
HWND AsynchDownload::AsynchBindStatusCallback::m_hNormalPriorityProgressDlg = NULL;
UINT32 AsynchDownload::AsynchBindStatusCallback::m_nNormalPriorityIconID = _R(IDI_DOWNLOADING);
INT32 AsynchDownload::m_nNormalPriorityInstanceCount = 0;
INT32 AsynchDownload::m_nHighPriorityInstanceCount = 0;
HICON AsynchDownload::AsynchBindStatusCallback::m_hiconProgress = 0;

// Matt 15/02/2001
typedef BOOL (WINAPI *P_GDFSE)(LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);
												  
												  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AsynchDownload
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
												  
/********************************************************************************************

  >	AsynchDownload::AsynchDownload(DOWNLOAD_HANDLE hDownload, LPDOWNLOADINFO pDownloadInfo)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		hDownload - handle of download as assigned by the InternetManager
					pDownloadInfo - pointer to DOWNLOADINFO struct
								
	Purpose:	Constructor
					 
	Notes:		This constructor is not meant to be called directly  - use InternetManager::RegisterDownload()
					if you want to download a file

********************************************************************************************/

AsynchDownload::AsynchDownload(DOWNLOAD_HANDLE hDownload, LPDOWNLOADINFO pDownloadInfo)
{
	m_Handle = hDownload;
	m_bSuccess = FALSE;
	m_ulFileSize = 0;
	m_ulDownloaded = 0;
	m_nPercentageDownloaded = 0;
	m_bAbort = FALSE;
    m_pMoniker = NULL;
    m_pBindContext = NULL;
    m_pCallback = NULL;
	m_strTargetURL = pDownloadInfo->strURL;
	m_strLocalFilePath = pDownloadInfo->strLocalFile;
	m_nFileType = (FileType) pDownloadInfo->nFileType;
	m_nAttempts = 0;
	m_Priority = (Priority) pDownloadInfo->nPriority;
	m_bHasProgressDlg = pDownloadInfo->bHasProgressDlg;
	if (m_bHasProgressDlg = pDownloadInfo->bHasProgressDlg)
		m_strDescription = pDownloadInfo->strDescription;
	if (m_Priority == PRIORITY_NORMAL)
		m_nNormalPriorityInstanceCount++;
	else if (m_Priority == PRIORITY_HIGH)
		m_nHighPriorityInstanceCount++;

	m_hwndNotifyWindow = pDownloadInfo->hwndNotifyWindow;
	m_lNotifyToken = pDownloadInfo->lNotifyToken;

	AfxOleLockApp();
}

AsynchDownload::~AsynchDownload()
{
	// Deallocate whatever resources were alocated during the binding operation
	Cleanup();
	if (m_Priority == PRIORITY_NORMAL)
		m_nNormalPriorityInstanceCount--;
	else if (m_Priority == PRIORITY_HIGH)
		m_nHighPriorityInstanceCount--;
	AfxOleUnlockApp();
}



/********************************************************************************************

  >	HRESULT AsynchDownload::DoBind()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		none 
	Return:		S_OK if successful, E_FAIL otherwise
	Purpose:	Binds asynchronously an URL moniker to local storage (a file in the Internet cache)
					The function returns immediately an HRESULT	indicating merely whether the bind was started successfuly,
					or failed due to invalid data, insufficient memory, etc., not waiting for the bind to complete. The result of the
					started bind operation is returned by AsynchBindCallback::OnStopBind().
					 
	
********************************************************************************************/



HRESULT AsynchDownload::DoBind()
{
	if (m_strTargetURL.IsEmpty() || m_strLocalFilePath.IsEmpty())
	{
		ERROR3("Invalid (empty) URL or file path");
		return E_FAIL;
	}
#ifdef _DEBUG
	String_256 strErrorMsg(m_strTargetURL); // we keep a non-wide copy of the URL for error reporting purposes
#endif
	WCHAR wchURL[INTERNET_MAX_PATH_LENGTH];
	MultiByteToWideChar(CP_ACP, 0,  (TCHAR*) m_strTargetURL, -1, wchURL, INTERNET_MAX_PATH_LENGTH);
	if (IsValidURL(NULL, wchURL, 0) == S_FALSE)
	{
#ifdef _DEBUG
		strErrorMsg += _T(" is not a valid URL");
		ERROR3(strErrorMsg);
#endif
		return E_FAIL;
	}
	IStream* pStream = NULL;
	m_pCallback = new AsynchBindStatusCallback(this);
	if (!m_pCallback)
	{ 
		ERROR3("Memory allocation error");
		return E_OUTOFMEMORY;
	}
    HRESULT hr = CreateURLMoniker(NULL, wchURL, &m_pMoniker);
    if (FAILED(hr))
	{
		ERROR3("Could not create URL Moniker");
		Cleanup();
		return hr;
	}
    hr = CreateBindCtx(0, &m_pBindContext);
    if (FAILED(hr))
	{
		ERROR3("Could not create bind context");
		Cleanup();
		return hr;
	}
    hr = RegisterBindStatusCallback(m_pBindContext, m_pCallback, 0, 0L);
    if (FAILED(hr))
    {
		ERROR3("Failed to register callback object");
		Cleanup();
		return hr;
	}
    hr = m_pMoniker->BindToStorage(m_pBindContext, 0, IID_IStream, (void**)&pStream);
    m_nAttempts++;
	return hr;
}



/********************************************************************************************
>	void AsynchDownload::Cleanup()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:			-
	Returns:	-
	Purpose:	Releases COM objects allocated during the binding process.


********************************************************************************************/
void AsynchDownload::Cleanup()
{
	if (m_pCallback)
	{
		m_pCallback->Release();
		m_pCallback = NULL;
	}
	if (m_pMoniker)
	{
		m_pMoniker->Release();
		m_pMoniker = NULL;
	}
	if (m_pBindContext)
	{
		m_pBindContext->Release();
		m_pBindContext = NULL;
	}
}

/********************************************************************************************
>	HRESULT AsynchDownload::Retry()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:			-
	Returns:	- always S_OK				

	Purpose:	called in case of network error to retry the download

********************************************************************************************/
HRESULT AsynchDownload::Retry()
{
	if (m_nAttempts <= MAX_ATTEMPTS) // try another bind if we're allowed
	{
		TRACEUSER( "adrian", _T("\n\nRetrying the %d time to download %s\n\n"), m_nAttempts, (TCHAR*) m_strTargetURL);
		Cleanup();
		DoBind();
	}
	else // just inform the download manager that we're done
	{
		TRACEUSER( "adrian", _T("\n\nGiving up on %s\n\n"), (TCHAR*) m_strTargetURL);
		InternetManager::OnDownloadComplete(this);
	}
	return S_OK;
}


/********************************************************************************************
>	HRESULT AsynchDownload::AbortDownload()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:			-
	Returns:	- S_OK if the download operation operation was successfully aborted.
					- S_FALSE if the download was already aborted.
					- E_FAIL if the download operation could not be aborted.

	Purpose:	cancels this download.


********************************************************************************************/


HRESULT AsynchDownload::AbortDownload()
 {
	if (m_bAbort)
		return S_FALSE;
	else
		m_bAbort = TRUE;
	if (m_pCallback)
	{
		if (m_pCallback->m_pBinding)
			return m_pCallback->m_pBinding->Abort();
		else
			return E_FAIL;
	}
	return E_FAIL;
}


/********************************************************************************************
>	void AsynchDownload::Release()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:			-
	Returns:	- 

	Purpose:	Provides a safe way to free memory and resources allocated to this download
					Should be used in preference to the delete operator which may cause access
					violations if the download is still in progress

********************************************************************************************/

void AsynchDownload::Release()
{
	if (m_pCallback && m_pCallback->m_pBinding)
		m_pCallback->m_pBinding->Abort();
	else
		delete this;
}



/********************************************************************************************
>  AsynchDownload::AsynchBindStatusCallback::AsynchBindStatusCallback(AsynchDownload* pDownload)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  pDownload		- pointer to the AsynchDownload object owning the callback
	Returns:	none
	Purpose:   callback object constructor.

********************************************************************************************/


AsynchDownload::AsynchBindStatusCallback::AsynchBindStatusCallback(AsynchDownload* pDownload)
{
	m_dwRef = 1;
	m_pDownload = pDownload;
	m_pBinding = NULL;
	m_hProgressDlg = NULL;
	m_nIconID = _R(IDI_DOWNLOADING); 
}


/********************************************************************************************
>	AsynchDownload::AsynchBindStatusCallback::~AsynchBindStatusCallback()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  none
	Returns:	none
	Purpose:   callback object destructor.

********************************************************************************************/

AsynchDownload::AsynchBindStatusCallback::~AsynchBindStatusCallback()
{
	if (m_pBinding) // download still in progress
	{
		// this will almost certainly result in an access violation
		ERROR3("AsynchDownload deleted while still in progress");
	}
}

/********************************************************************************************
>	HRESULT _stdcall AsynchDownload::AsynchBindStatusCallback::QueryInterface(REFIID riid,void ** ppv)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  riid	-	identifier of the requested interface
					 ppvObject	-	indirect pointer to the object
	Returns:	S_OK if the interface is supported, E_NOINTERFACE otherwise.
	Purpose:   standard IUnknown method

********************************************************************************************/
HRESULT _stdcall AsynchDownload::AsynchBindStatusCallback::QueryInterface(REFIID riid,void ** ppv)
{
    *ppv = NULL;

    if (riid==IID_IUnknown || riid==IID_IBindStatusCallback)
	{
		*ppv = this;
		AddRef();
		return S_OK;
	}
	else if (riid == IID_IAuthenticate)
	{
		*ppv = (IAuthenticate *)this;
		AddRef();
		return S_OK;
	}

    return E_NOINTERFACE;
}  

/********************************************************************************************
>	HRESULT _stdcall AsynchDownload::AsynchBindStatusCallback::OnStartBinding(DWORD dwReserved, IBinding* pbinding)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  dwReserved	-	not used, must be 0
					 pbinding	-	pointer to the IBinding interface for the current bind operation
	Returns: S_OK 
	Purpose:   standard IBindStatusCallback method (must be  implemented). Allows us to get a pointer to the binding. 
	See also:  MS Internet SDK documentation (URL Monikers)
********************************************************************************************/
HRESULT _stdcall AsynchDownload::AsynchBindStatusCallback::OnStartBinding(DWORD dwReserved, IBinding* pbinding)
{
	m_pDownload->m_ulFileSize = 0;
	m_pDownload->m_ulDownloaded = 0;
    // We save the pointer to the binding
	m_pBinding = pbinding;
    if (m_pBinding != NULL)
        m_pBinding->AddRef();
	// Create & display the progress dialog if the download requires one
	if (m_pDownload->m_bHasProgressDlg)
	{
		HWND hProgressDlg;
		BOOL bNeedsPositioning = (m_pDownload->m_Priority != PRIORITY_NORMAL) ||
				(m_pDownload->m_Priority == PRIORITY_NORMAL && !m_hNormalPriorityProgressDlg);
		if (!AttachProgressDlg())
		{
			ERROR3("Could not create progress dialog");
			goto END; 
		}
		if (m_pDownload->m_Priority == PRIORITY_NORMAL)
			hProgressDlg = m_hNormalPriorityProgressDlg;
		else
			hProgressDlg = m_hProgressDlg;
		if (hProgressDlg)
		{
			HWND hProgressBar = GetDlgItem(hProgressDlg, _R(IDC_PROGRESSBAR)); // hadle of progress bar
			HWND hPercentage = GetDlgItem(hProgressDlg, _R(IDC_PERCENTAGE)); // handle of static text used to display percent downloaded
			ERROR3IF(!(hProgressBar && hPercentage), "Cannot find control");
			// Set initial position on the progress bar to 0
			::SendMessage(hProgressBar, PBM_SETPOS, (WPARAM) 0, 0);
			// Set initial percentage
			String_256 strPercentage = _T("0%");
			// Set the dialog caption
			String_256 strURL = m_pDownload->m_strTargetURL;
			strtok(strURL, _T(":/"));
			String_256 strCaption;
			strCaption.MakeMsg(_R(IDS_CONTACTING_HOST), strtok(NULL, _T(":/")));
			::SetWindowText(hProgressDlg, (TCHAR*) strCaption);
			::SetWindowText(hPercentage, (TCHAR*) strPercentage);
			// Position the window if necessary 
			if (bNeedsPositioning)
			{
				CRect rcDialog;
				::GetWindowRect(hProgressDlg, &rcDialog);
				// Finally position & show the dialog box
				::SetWindowPos(hProgressDlg, HWND_TOPMOST, ::GetSystemMetrics(SM_CXSCREEN) - rcDialog.Width(), 0,
						0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			}
			else
				::SetWindowPos(hProgressDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

			::UpdateWindow(hProgressBar);
			::UpdateWindow(hPercentage);
			::UpdateWindow(hProgressDlg);
		}
	}
END:    
	return S_OK;
}

/********************************************************************************************
>	HRESULT _stdcall  AsynchDownload::AsynchBindStatusCallback::OnProgress(UINT32 ulProgress, UINT32 ulProgressMax, UINT32 ulStateCode,
							LPCWSTR pwzStateText)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  ulProgress	-	progress indicator for bind operation
					 ulProgressMax	-	expected maximum value of ulProgress parameter
					 ulStateCode	-	BINDSTATUS value indicating bind progress
					 szStateText	-	displayable information indicating bind progress
	Returns:	NOERROR or E_ABORT if Abort() has been called 
	Purpose:   standard IBindStatusCallback (must be implemented). Allows monitoring the progress of the bind.
					changes 
	See also:  MS Internet SDK documentation (URL Monikers)
********************************************************************************************/
HRESULT _stdcall  AsynchDownload::AsynchBindStatusCallback::OnProgress(UINT32 ulProgress, UINT32 ulProgressMax, UINT32 ulStateCode,
							LPCWSTR pwzStateText)
{
    if (m_pDownload->m_bAbort)
	{
		return E_ABORT;
	}
	TCHAR   sz[256];
	if(pwzStateText != NULL)
	    WideCharToMultiByte(CP_ACP, 0, pwzStateText, -1, sz, sizeof(sz)/sizeof(TCHAR), 0, 0);
	// Update download progress
	// (Note that ulProgressMax is the file size returned by the server and if may be 0 if the server doesn't know it)
	// (Hopefully our server is better than that)
	m_pDownload->m_ulFileSize = (ulProgress > ulProgressMax) ? ulProgress : ulProgressMax;
	m_pDownload->m_ulDownloaded = ulProgress;
	INT32 nPercentage = (ulProgress && ulProgressMax) ? (100 * ulProgress/ulProgressMax) : 0;

	if (nPercentage!=m_pDownload->m_nPercentageDownloaded)
	{
		// we should inform our client that things are happening...
		InternetManager::SetState(m_pDownload->m_Handle, AsynchDownload::STATE_PENDING);
	}

	m_pDownload->m_nPercentageDownloaded = nPercentage;
	if (m_pDownload->m_bHasProgressDlg)
	{
		HWND hProgressDlg;
		if (m_pDownload->m_Priority == PRIORITY_NORMAL)
			hProgressDlg = m_hNormalPriorityProgressDlg;
		else
			hProgressDlg = m_hProgressDlg;
		// Make sure the progress dialog handle is still valid before attempting to update it 
		if (hProgressDlg && ::IsWindow(hProgressDlg))
		{	
			HWND hPercentage = GetDlgItem(hProgressDlg, _R(IDC_PERCENTAGE)); // handle of static text used to display percent downloaded
			HWND hProgressBar = GetDlgItem(hProgressDlg, _R(IDC_PROGRESSBAR)); // handle of progress bar
			if (!(hPercentage && hProgressBar))
			{
				ERROR3("Cannot find control");
			}
			else
			{
				String_256 strCaption, strPercentage;
				switch (ulStateCode)
				{
					case BINDSTATUS_FINDINGRESOURCE: // these 3 states are now displayed as "Contacting host..."
					case BINDSTATUS_CONNECTING:
					case BINDSTATUS_SENDINGREQUEST:
						break;

					case BINDSTATUS_DOWNLOADINGDATA:
						{
							strCaption = String_256(_R(IDS_DOWNLOAD));
							if (m_pDownload->m_strDescription.IsEmpty()) // no description available, just use the filename
							{
								strCaption+= _T("'");
								strCaption += strrchr(m_pDownload->m_strTargetURL, _T('/')) + 1;
								strCaption+= _T("'");
							}
							else
							{
								strCaption += m_pDownload->m_strDescription;
							}
							if (!ulProgressMax)
							{
								strPercentage = "N/A"; // in case the server didn't return any size information
								m_pDownload->m_nPercentageDownloaded = 0;
							}
							else // set the progress bar to the percentage we have downloaded
							{
								wsprintf(strPercentage, "%d%%", nPercentage);
								::SendMessage(hProgressBar, PBM_SETPOS, (WPARAM) nPercentage, 0);
								// Force an immediate repaint to avoid any redraw latency
								::UpdateWindow(hProgressBar);
							}
						}
						break;
					case BINDSTATUS_ENDDOWNLOADDATA:
						strPercentage = _T("100%");
						::SendMessage(hProgressBar, PBM_SETPOS, (WPARAM) 100, 0);
						// Force an immediate repaint to avoid any redraw latency
						::UpdateWindow(hProgressBar);
						strCaption = String_256(_R(IDS_ENDDOWNLOAD));
						break;
				}
				// We'll update the controls only if the new strings are different to the current ones so that we don't 
				// get any flicker 
				TCHAR tchBuff[96];
				::GetWindowText(hPercentage, tchBuff, sizeof(tchBuff)/sizeof(TCHAR));
				if (strPercentage != String_256(tchBuff))
				{
					::SetWindowText(hPercentage, (TCHAR*) strPercentage);
					::UpdateWindow(hPercentage);
				}
				::GetWindowText(hProgressDlg, tchBuff, sizeof(tchBuff)/sizeof(TCHAR));
				if (strCaption.Length() && strCaption != String_256(tchBuff)) // we need to update the message
				{
					::SetWindowText(hProgressDlg, (TCHAR*) strCaption);
					::UpdateWindow(hProgressDlg);
				}
			}
		}
	}	
	// Save filename of local image if available
	if (ulStateCode == BINDSTATUS_CACHEFILENAMEAVAILABLE)
		m_pDownload->m_strCacheFileName = sz;

	return (NOERROR);
}


/********************************************************************************************
>	HRESULT _stdcall AsynchDownload::AsynchBindStatusCallback::OnStopBinding(HRESULT hrState, LPCWSTR szError)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  hrState	-	 	bind operation status code
					 szError	-	currently not used
	Returns:	S_OK 
	Purpose:   standard IBindStatusCallback (must be implemented). Allows us to retrieve the result of the bind in hrState.
	See also:  MS Internet SDK documentation (URL Monikers)
********************************************************************************************/

HRESULT _stdcall AsynchDownload::AsynchBindStatusCallback::OnStopBinding(HRESULT hrState, LPCWSTR szError)
{
#ifdef _DEBUG
	if (hrState) // download diagnostic in debug builds
	{
		TCHAR szErrorMsg[256];
		if (hrState == 0x800C0006) // file not found
			wsprintf(szErrorMsg, "%s %s\n", (TCHAR*) (String_256) m_pDownload->m_strTargetURL, _T("not found on the server"));
		else
			wsprintf(szErrorMsg, "%s %s\n", _T("Got network error (possibly host not found) downloading"), (TCHAR*) (String_256) m_pDownload->m_strTargetURL);
		 if (hrState != 0x80004004) // user cancel
			 TRACE(szErrorMsg);

		 // Write error to log
		 String_256 strAppDataPath;
		 GetAppDataPath(&strAppDataPath);
		 PathName logPath(strAppDataPath);
		 String_256 strLogName("errlog.txt");
		 logPath.SetFileNameAndType(strLogName);
		 static BOOL bShouldOverwrite = TRUE; // overwrite any error log generated in a previous session
		 INT32 iFlags = bShouldOverwrite ? ios::out | ios::trunc	:	ios::out | ios::app;
		 bShouldOverwrite = FALSE;
		 ofstream logFile((TCHAR*) (String_256) logPath.GetPath(), iFlags);
		 if (!logFile.bad())
			 logFile << szErrorMsg;
		 logFile.close();
	}
#endif

	// Clean up
	if (m_pBinding)
	{
		m_pBinding->Release();
		m_pBinding = NULL;
	}
	if (m_pDownload->m_bHasProgressDlg)
		DetachProgressDlg(); // release/destroy the progress dialog, if any

	if (!hrState && !m_pDownload->m_bAbort) // file downloaded successfully
	{
		// The file has been downloaded to the system internet cache, so we need to copy it over to the required location
		PathNameEx path(m_pDownload->m_strLocalFilePath);
		BOOL bIsCacheFile = DownloadCache::IsCachePath(m_pDownload->m_strLocalFilePath); 
		if (!path.CreateLocation()) // create the directory path if it doesn't exist - otherwise ofstream() may fail
		{
			InformError(_R(IDS_DIRCREATEFAIL), _R(IDS_OK));
			m_pDownload->m_bSuccess = FALSE;
			goto COMPLETE;
		}
		ifstream stmCache((TCHAR*) m_pDownload->m_strCacheFileName, ios::in | ios::nocreate | ios::binary);
		// We overwrite the existing file if any
		DownloadCache::RemoveFile(m_pDownload->m_strLocalFilePath);
		if (bIsCacheFile)
			DownloadCache::m_CacheMonitor.IgnoreEvents(1);
		ofstream stmFile((TCHAR*) m_pDownload->m_strLocalFilePath, ios::out | ios::binary | ios::trunc);
		if (stmCache.bad() || stmFile.bad())
		{
			String_256 strTemp;
			PathName path(m_pDownload->m_strLocalFilePath);
			strTemp.MakeMsg(_R(IDS_FILEIOERROR), (TCHAR*) String_256(path.GetFileName()));
			Error::SetError(0, strTemp, 0);
			InformError();
			stmFile.close();
			stmCache.close();
			goto COMPLETE;
		}
		stmFile << stmCache.rdbuf();
		if (stmFile.bad() || stmCache.bad())
		{
			String_256 strTemp;
			PathName path(m_pDownload->m_strLocalFilePath);
			strTemp.MakeMsg(_R(IDS_FILEIOERROR), (TCHAR*) String_256(path.GetFileName()));
			Error::SetError(0,strTemp, 0);
			InformError();
		}
		else
			m_pDownload->m_bSuccess = TRUE;
		stmFile.close();
		stmCache.close();
	}
	else if (hrState == 0x800C0007) // we got a network error (unfortunately these error codes are not documented by Microsoft)
	{
		return m_pDownload->Retry();
	}
		 
COMPLETE:
	InternetManager::OnDownloadComplete(m_pDownload);
    return S_OK;
}  

/********************************************************************************************
>	HRESULT _stdcall AsynchDownload::AsynchBindStatusCallback::GetBindInfo(DWORD* pgrfBINDF, BINDINFO* pbindInfo)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  pgrfBINDF	-	pointer to BINDF value
					 pbindinfo	-	pointer to BINDINFO structure
	Returns:	S_OK 
	Purpose:   standard IBindStatusCallback method (must be implemented)
	See also:  MS Internet SDK documentation (URL Monikers)
********************************************************************************************/
HRESULT _stdcall AsynchDownload::AsynchBindStatusCallback::GetBindInfo(DWORD* pgrfBINDF, BINDINFO* pbindInfo)
{
    *pgrfBINDF = BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE | BINDF_GETNEWESTVERSION; // bind asynchronously
    pbindInfo->cbSize = sizeof(BINDINFO);
    pbindInfo->szExtraInfo = NULL;
    memset(&pbindInfo->stgmedData, 0, sizeof(STGMEDIUM));
    pbindInfo->grfBindInfoF = 0;
    pbindInfo->dwBindVerb = BINDVERB_GET;
    pbindInfo->szCustomVerb = NULL;
    return S_OK;
} 

/********************************************************************************************
> HRESULT _stdcall AsynchDownload::AsynchBindStatusCallback::OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC *pfmtetc,
							STGMEDIUM* pstgmed)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  gfBSCF	-	BSCF enumeration values
					 dwSize	-	length of data in bytes available from current bind operation
					 pfmtetc	-	pointer to FORMATETC structure
					 pstgmed	-	Pointer to STGMEDIUM structure
	Returns:	S_OK 
	Purpose:   standard IBindStatusCallback method (must be implemented, so we just return S_OK)
	See also:  MS Internet SDK documentation (URL Monikers)
********************************************************************************************/
HRESULT _stdcall AsynchDownload::AsynchBindStatusCallback::OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC *pfmtetc,
							STGMEDIUM* pstgmed)
{
    // We're not reading any data before the download is complete, so just return S_OK 
	return S_OK;
}  

/********************************************************************************************
>	STDMETHODIMP AsynchDownload::AsynchBindStatusCallback::Authenticate(HWND *phwnd,
													LPWSTR *pszUserName, LPWSTR *pszPassword)
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/02/2001
	Purpose:	This method is called when athentication is required to access a resource.
				We simply tell the calling function to go away and the authentication it's
				self.
	See also:	MSDN docs for the function
********************************************************************************************/
STDMETHODIMP AsynchDownload::AsynchBindStatusCallback::Authenticate(HWND *phwnd, LPWSTR *pszUserName, LPWSTR *pszPassword)
{
	*phwnd = AfxGetMainWnd()->m_hWnd;
	*pszUserName = 0;
	*pszPassword = 0;

	return S_OK;
}

/********************************************************************************************
> BOOL CALLBACK AsynchDownload::AsynchBindStatusCallback::DialogProc(HWND hwndDlg, UINT32 message, WPARAM wParam, LPARAM lParam)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		 hwndDlg - identifies the dialog box. 
					uMsg - specifies the message. 
					wParam - specifies additional message-specific information. 
					lParam - specifies additional message-specific information

	Returns:	non-zero if it processes the message, 0 if not
	Purpose:   progress dialog procedure

********************************************************************************************/

BOOL CALLBACK AsynchDownload::AsynchBindStatusCallback::DialogProc(HWND hDlg, UINT32 message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
     {
		case WM_TIMER:
		{
			if (wParam == ICON_ANIMATION)
			{
				UINT32* pIconID = NULL;
				if (hDlg == m_hNormalPriorityProgressDlg)
					pIconID = &m_nNormalPriorityIconID;
				else
				{
					AsynchBindStatusCallback* pCallback = NULL;
					if (!m_CallbackTable.Lookup(hDlg, pCallback))
					{
						ERROR3("Callback not found in the lookup table");
					}
					else if (pCallback)
						pIconID = &pCallback->m_nIconID;
				}
				if (*pIconID == _R(IDI_DOWNLOADING6))
					*pIconID = _R(IDI_DOWNLOADING);
				else
					(*pIconID)++;

				HICON hiconPrev = m_hiconProgress;
				m_hiconProgress = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(*pIconID), IMAGE_ICON, 16, 16, 0);
				::SendMessage(hDlg, WM_SETICON, (WPARAM) FALSE, (LPARAM) m_hiconProgress );
				::DestroyIcon(hiconPrev);
				return TRUE;
			}
		}
		break;
		case WM_COMMAND:
	    {
			switch (LOWORD(wParam))
			{
		   		case IDCANCEL:
			    {
CANCEL:					
					if (m_hiconProgress)
					{
						::DestroyIcon(m_hiconProgress);
						m_hiconProgress = 0;
					}

					// Got CANCEL from the user, find the callback this dialog belongs to
					if (hDlg == m_hNormalPriorityProgressDlg) // thumbnail/catalog file download progress
					{
						AsynchDownload* pDownload = InternetManager::GetCurrentNormalPriorityDownload();
						if (pDownload)
							pDownload->AbortDownload();
						return TRUE;
					}
					AsynchBindStatusCallback* pCallback = NULL;
					if (!m_CallbackTable.Lookup(hDlg, pCallback))
					{
						ERROR3("Callback not found in the lookup table");
					}
					else
					{
						if (pCallback && pCallback->m_pDownload)
						{
							// Although the callback object doesn't contain any runtime information and
							// thus we cannot directly check if it's still valid or not, a non-null pointer in the callback
							// table should guarantee that, as callback objects call DetachProgressDlg() when
							// a download completes or is terminated by the user.
							// Never use the delete operator on a download in progress (instead of calling the Release() member
							// function) as it could result in pCallback being no longer valid at this point and cause an access violation
							HRESULT result = pCallback->m_pDownload->AbortDownload();
							ERROR3IF(result == E_FAIL, "Could not abort download");
						}
					}
					return TRUE;
				}
				break;
			}
		}
		break;
		case WM_CLOSE:
			goto CANCEL; // we assume they actually want to cancel the download
	}
	return FALSE;
}


/********************************************************************************************
> BOOL AsynchDownload::AsynchBindStatusCallback::AttachProgressDlg()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		 -
	Returns:	TRUE if the dialog box was created successful, FALSE otherwise
	Purpose:   creates a modeless progress dialog box and attach it to this callback,
					i.e. make an entry in the callback table mapping the callback pointer 
					to the handle of its dialog

********************************************************************************************/
BOOL AsynchDownload::AsynchBindStatusCallback::AttachProgressDlg()
{
	static BOOL bShouldRegisterClass = TRUE;
	if (bShouldRegisterClass) // register the progress dlg class if needed
	{
		WNDCLASSEX wndClass;
		static char szClassName[] =  "DownloadProgressDlg";
		if (!::GetClassInfoEx(NULL, MAKEINTRESOURCE(32770), &wndClass))
		{
			ERROR3("Failed to get class info");
		}
		wndClass.lpszClassName = szClassName;
		wndClass.hInstance = AfxGetResourceHandle();
		wndClass.hIcon = NULL;
		wndClass.hIconSm = (HICON) ::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(_R(IDI_DOWNLOADING)), IMAGE_ICON, 16, 16, 0);
		wndClass.cbSize = sizeof(wndClass);
		ERROR3IF(!wndClass.hIconSm, "Failed to load icon");
		if (!::RegisterClassEx(&wndClass))
		{
			ERROR3("Failed to register progress dlg class");
		}
		else
			bShouldRegisterClass = FALSE;
	}
	if (m_pDownload->m_Priority == PRIORITY_HIGH) // high priority download - gets its own progress dialog
	{
		m_hProgressDlg = ::CreateDialogParam(AfxGetResourceHandle(), MAKEINTRESOURCE(_R(IDD_PROGRESS)), 
			HWND_DESKTOP,	(DLGPROC) &DialogProc, NULL);
		if (!m_hProgressDlg)
		{
			ERROR3("Could not create progress dialog");
			return FALSE;
		}
		else
		{
			// Enter the handle in the callback table
			m_CallbackTable[m_hProgressDlg] = this;
			::SetTimer(m_hProgressDlg, ICON_ANIMATION, ICON_UPDATE_INTERVAL, NULL);
		}
	}
	else if (!m_hNormalPriorityProgressDlg) // low priority download - create dialog only if there is none to be reused
	{
		m_hNormalPriorityProgressDlg = ::CreateDialogParam(AfxGetResourceHandle(), MAKEINTRESOURCE(_R(IDD_PROGRESS)), 
			HWND_DESKTOP,	(DLGPROC) &DialogProc, NULL);
		if (!m_hNormalPriorityProgressDlg)
		{
			ERROR3("Could not create progress dialog");
			return FALSE;
		}
		else
			::SetTimer(m_hNormalPriorityProgressDlg, ICON_ANIMATION, ICON_UPDATE_INTERVAL, NULL);
	}
	return TRUE;
}


/********************************************************************************************
> void AsynchDownload::AsynchBindStatusCallback::DetachProgressDlg()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		 -
	Returns:	-
	Purpose:   called by OnStopBinding() to remove this callback's entry in the 
					callback table - as it will be no longer valid - and destroy the 
					progress dialog if one exists

********************************************************************************************/
void AsynchDownload::AsynchBindStatusCallback::DetachProgressDlg()
{
	if (m_hProgressDlg && m_pDownload->m_Priority == PRIORITY_HIGH)
	{
		BOOL bDestroyed = FALSE;
		// Kill the icon animation timer
		bDestroyed = ::KillTimer(m_hProgressDlg, ICON_ANIMATION);
		ERROR3IF(!bDestroyed, "Warning: timer not destroyed");
		// Make sure the progress dialog is destroyed 
		if (::IsWindow(m_hProgressDlg))
			bDestroyed = ::DestroyWindow(m_hProgressDlg);
		ERROR3IF(!bDestroyed, "Warning: progress dialog not destroyed");
		// Just in case we couldn't destroy the window for some reason we'll set the dialog handle
		// entry in the callback table to NULL indicating it references a callback pointer no longer valid
		m_CallbackTable[m_hProgressDlg] = NULL;
		m_hProgressDlg = NULL;
	}
	else if (m_hNormalPriorityProgressDlg && m_pDownload->m_Priority == PRIORITY_NORMAL)
	{
		if (m_pDownload->m_nNormalPriorityInstanceCount == 1)
		{
			// Kill the icon animation timer
			BOOL bDestroyed = ::KillTimer(m_hNormalPriorityProgressDlg, ICON_ANIMATION);
			ERROR3IF(!bDestroyed, "Warning: timer not destroyed");
			::DestroyWindow(m_hNormalPriorityProgressDlg);
			m_hNormalPriorityProgressDlg = NULL;
		}
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DownloadQueue
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************
> DownloadQueue::~DownloadQueue()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  none
	Returns:	none 
	Purpose:   destructor
********************************************************************************************/

DownloadQueue::~DownloadQueue()
{
	if (!IsEmpty())
		Flush();
}



/********************************************************************************************
> DownloadQueue::Queue(AsynchDownload* pDownload)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  pDownload		-	pointer to download to be queued
	Returns:	TRUE is successful, FALSE otherwise
	Purpose:   Adds a new download  pointer to the queue
********************************************************************************************/


BOOL DownloadQueue::Queue(AsynchDownload* pDownload)
{
	if (!pDownload)
	{
		ERROR3("Unexpected NULL pointer");
		return FALSE;
	}
	if (m_enType != LIFO && m_enType != FIFO)
	{
		ERROR3("Invalid queue type");
		return FALSE;
	}
	try
	{
		if (m_enType == FIFO)
			m_List.AddTail(pDownload);
		else
			m_List.AddHead(pDownload);
	}
	catch (CMemoryException* pxMem)
	{
		pxMem->Delete();
		ERROR2(FALSE, "Could not queue download - memory problems");
	}
	return TRUE;
}


/********************************************************************************************
> DownloadQueue::Remove(AsynchDownload* pDownload)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  pDownload		-	pointer to download to be removed
	Returns:	TRUE is successful, FALSE otherwise
	Purpose:   remove a download  from the queue. The object is also deleted
********************************************************************************************/


BOOL DownloadQueue::Remove(AsynchDownload* pDownload)
{
	if (!pDownload)
	{
		ERROR3("Unexpected NULL pointer");
		return FALSE;
	}
	POSITION pos = m_List.Find(pDownload);
	if (!pos)
		return FALSE; // the object is not is this queue
	m_List.RemoveAt(pos);
	pDownload->Release();
	return TRUE;
}


/********************************************************************************************
> DownloadQueue::Flush()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  pDownload		-	pointer to download to be removed
	Returns:	none
	Purpose:	Flushes the queue and deletes the objects at the same time    
********************************************************************************************/

void DownloadQueue::Flush()
{
	if (IsEmpty())
		return;
	POSITION pos = m_List.GetHeadPosition();
	while (pos)
	{
		AsynchDownload* pDownload = m_List.GetNext(pos);
		if (pDownload)
			pDownload->Release();
	}
	m_List.RemoveAll();
}



/********************************************************************************************
> AsynchDownload* DownloadQueue::FindDownload(const String_256& strFileName)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  strFileName		-	local filename of the download searched for
	Returns:	pointer to AsynchDownload object if found, NULL otherwise
	Purpose:	searches the queue for an object downloading to a known local file    
********************************************************************************************/

AsynchDownload* DownloadQueue::FindDownload(const String_256& strFileName)
{
	if (IsEmpty())
		return NULL;
	String_256 strLocalFileName(strFileName);
	AsynchDownload* pIterator = NULL;
	POSITION pos = m_List.GetHeadPosition();
	while (pos)
	{
		pIterator = m_List.GetNext(pos);
		if (!strLocalFileName.CompareTo(pIterator->GetLocalFileName(), FALSE))
			return pIterator;
	}
	return NULL;
}


/********************************************************************************************
> AsynchDownload* DownloadQueue::FindDownload(DOWNLOAD_HANDLE hDownload)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  hDownload		-	handle of download searched for
	Returns:	pointer to AsynchDownload object if found, NULL otherwise
	Purpose:	searches the queue for an object wirh a known handle    
********************************************************************************************/



AsynchDownload* DownloadQueue::FindDownload(DOWNLOAD_HANDLE hDownload)
{
	AsynchDownload* pDownload = NULL;
	if (IsEmpty())
		return pDownload;
	AsynchDownload* pIterator = NULL;
	POSITION pos = m_List.GetHeadPosition();
	while (pos && !pDownload)
	{
		pIterator = m_List.GetNext(pos);
		if (pIterator->GetHandle() == hDownload)
			pDownload = pIterator;
	}
	return pDownload;
}




/********************************************************************************************
> AsynchDownload* DownloadQueue::GetNextDownload()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  none
	Returns:	pointer to AsynchDownload object if any waiting in the queue, NULL otherwise
	Purpose:	access the next object in the queue (which is removed from the queue at the same time)       
********************************************************************************************/



AsynchDownload* DownloadQueue::GetNextDownload()
{
	AsynchDownload* pDownload = NULL;
	if (m_List.IsEmpty())
		return pDownload;
	POSITION pos = m_List.GetHeadPosition();
	pDownload = m_List.GetAt(pos);
	m_List.RemoveAt(pos); // remove the download from the queue
	return pDownload;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DownloadCache
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UINT32 DownloadCache::m_lMaxSize = 5000 * SIZEOFKILOBYTE; // initial cache size
STL::priority_queue< CacheEntry, stl::vector<CacheEntry>, CacheRemovalAlgorithm> DownloadCache::m_CacheData;
STL::priority_queue< CacheEntry, stl::vector<CacheEntry>, CacheRemovalAlgorithm> DownloadCache::m_TemporaryCacheData;
INT32 DownloadCache::m_nInstanceCount = 0;
UINT32 DownloadCache::m_lCurrentSize = 0;
String_256 DownloadCache::m_strCachePath;
DownloadCache::CacheMonitor DownloadCache::m_CacheMonitor;
HANDLE DownloadCache::CacheMonitor::rgEvents[2]; 
volatile INT32 DownloadCache::CacheMonitor::m_nIgnoreCount;
CRITICAL_SECTION DownloadCache::CacheMonitor::m_CacheDataCriticalSection;



/********************************************************************************************
> BOOL DownloadCache::SetPath(const String_256& rCachePath)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - rCachePath - reference to string containing the full path of the cache root directory
						If the directory does not exist, it will be created
	Returns:	- TRUE if succeeed, FALSE otherwise
	Purpose:	- allocate a directory on the hard disk containing the files that the cache is to manage
********************************************************************************************/


BOOL DownloadCache::SetPath(const String_256& rCachePath)
{
	m_strCachePath = rCachePath;
	// if the directory does not exist, we create it
	if (_taccess(m_strCachePath, 0) == -1)
	{
		PathNameEx cachePath(m_strCachePath);
		if (!cachePath.CreateLocation())
			return FALSE;
		else
		{
			while (!m_CacheData.empty())
				m_CacheData.pop();
			m_lCurrentSize = 0; // the cache is currently empty
			return TRUE;
		}
	}
	else
	{
		Refresh(); // update cache data 
		return TRUE;
	}
#ifdef _DEBUG
	AssertCacheDataValid();
#endif
}




/********************************************************************************************
> BOOL DownloadCache::SetSize(UINT32 lSize)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - lSize - new cache size 
	Returns:	- TRUE if succeeed, FALSE otherwise
	Purpose:	- modify cache size
********************************************************************************************/

BOOL DownloadCache::SetSize(UINT32 lSize)
{
	ERROR2IF(m_strCachePath.IsEmpty(), FALSE, "Invalid cache path");

	// ***********************************************************************
	// Matt - 15/02/2001
	// Modified so that large hard disks don't report incorrect disk space...
	// ***********************************************************************

	TCHAR szDrive[_MAX_DRIVE + 1];
	_tsplitpath(m_strCachePath, szDrive, NULL, NULL, NULL);
	camStrcat(szDrive, _T("\\"));

	// Get pointer to the GetDiskFreeSpaceEx function if it exists on this platform...
	BOOL fResult = FALSE;
	P_GDFSE pGetDiskFreeSpaceEx = NULL;
	pGetDiskFreeSpaceEx = (P_GDFSE)GetProcAddress(GetModuleHandle ("kernel32.dll"), "GetDiskFreeSpaceExA");

	unsigned __int64 i64FreeBytesToCaller, i64TotalBytes, i64FreeBytes;
	DWORD dwSectorsPerCluster, dwBytesPerSector, dwNumberOfFreeClusters, dwTotalNumberOfClusters;

	if (pGetDiskFreeSpaceEx)
	{
		fResult = pGetDiskFreeSpaceEx(szDrive, (PULARGE_INTEGER)&i64FreeBytesToCaller, (PULARGE_INTEGER)&i64TotalBytes, (PULARGE_INTEGER)&i64FreeBytes);

		// Test to see if our requested cache size is larger than the current available space !!!
		if (((__int64)lSize >= i64FreeBytesToCaller) || (lSize < 500 * SIZEOFKILOBYTE)) { return FALSE; }
	}
	else
	{
		fResult = GetDiskFreeSpace (szDrive, &dwSectorsPerCluster, &dwBytesPerSector, &dwNumberOfFreeClusters, &dwTotalNumberOfClusters);
		if (fResult)
		{
			// Force 64bit maths so that drives over 4Gb work properly !!! 
			i64FreeBytes = (__int64)dwNumberOfFreeClusters * dwSectorsPerCluster * dwBytesPerSector;

			// Test to see if our requested cache size is larger than the current available space !!!
			if (((__int64)lSize >= i64FreeBytes) || (lSize < 500 * SIZEOFKILOBYTE)) { return FALSE; }
		}
	}

	// Just incase we failed to call either of the above functions...
	if (!fResult) { ERROR3("Can't GetDiskFreeSpace()!"); return FALSE; }

	// Set the preference value to the new value
	InternetManager::g_CacheSize = lSize;
/*	
	// This code is an abortion and unmaintainable. Why not use the preference system?
	HKEY arhKeys[6];
	DWORD dwDisposition;
	memset(arhKeys, 0, sizeof(arhKeys));
	String_256 strProgramKey = GetProgramNameRegistryKey();
	BOOL bResult = ((RegOpenKeyEx(HKEY_CURRENT_USER, "Software", 0, KEY_ALL_ACCESS, &arhKeys[0]) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[0], _tcstok(strProgramKey, _T("\\")),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[1], &dwDisposition) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[1], _tcstok(NULL, _T("\\")),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[2], &dwDisposition) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[2], _tcstok(NULL, _T("\\")),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[3], &dwDisposition) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[3], _T("Options"),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[4], &dwDisposition) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[4], _T("Internet"),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[5], &dwDisposition) == ERROR_SUCCESS) &&
		(RegSetValueEx(arhKeys[5], _T("Cache Size"), NULL, REG_DWORD, (LPBYTE) &lSize,	sizeof(lSize)) == ERROR_SUCCESS));
	for (INT32 i = 5; i >= 0; i--) RegCloseKey(arhKeys[i]);
	ERROR3IF(!bResult, "Failed to save changes to registry");
*/
	
	m_lMaxSize = lSize;
	while (m_lCurrentSize > m_lMaxSize && m_CacheData.size())  // discard old files until we are within the cache limit
	{
		if (!_tremove(m_CacheData.top().szFilePath))
		{
			m_lCurrentSize -= m_CacheData.top().Size();
			TRACEUSER( "adrian", _T("Discarded %s - new cache size is %u bytes\n"), m_CacheData.top().szFilePath, m_lCurrentSize);
			TRACEUSER( "adrian", _T("New cache usage is %d percent\n"), GetUsage());
		}
		m_CacheData.pop();
	}
#ifdef _DEBUG
	AssertCacheDataValid();
#endif
	return TRUE;
}

	

/********************************************************************************************
> void DownloadCache::Flush()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - 
	Returns:	- 
	Purpose:	- empty the cache

********************************************************************************************/

void DownloadCache::Flush()
{
	if (m_strCachePath.IsEmpty())
		return; 
	Traverse(m_strCachePath, TRUE);
}



/********************************************************************************************
> void DownloadCache::Refresh()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - 
	Returns:	- 
	Purpose:	- update cache data - should be called either when the root has 
						changed or when files in the cache have been modified outside
						the program (ex. the user deleted some of the files, etc)
********************************************************************************************/


void DownloadCache::Refresh()
{
	if (m_strCachePath.IsEmpty())
	{
		ERROR3("Invalid cache path!");
		return;
	}
	::EnterCriticalSection(&m_CacheMonitor.m_CacheDataCriticalSection);
	__try
	{
		while (!m_CacheData.empty())
			m_CacheData.pop();
		m_lCurrentSize = 0;
		if (!Traverse(m_strCachePath))
		{
			ERROR3("Unknown cache error");
			__leave;
		}
		TRACEUSER( "adrian", _T("Current cache size is %u bytes\n"), m_lCurrentSize);
		TRACEUSER( "adrian", _T("Cache usage is %d percent\n"), GetUsage());
		if (!m_lMaxSize)
		{
			ERROR3("Cache size is 0!");
			__leave;
		}
		while (m_lCurrentSize > m_lMaxSize && m_CacheData.size())  // discard old files until we are within the cache limit
		{
			m_CacheMonitor.IgnoreEvents(1);
			if (!_tremove(m_CacheData.top().szFilePath))
			{
				m_lCurrentSize -= m_CacheData.top().Size();
				TRACEUSER( "adrian", _T("Discarded %s - new cache size is %u bytes\n"), m_CacheData.top().szFilePath, m_lCurrentSize);
				TRACEUSER( "adrian", _T("New cache usage is %d percent\n"), GetUsage());
			}
			else
				m_CacheMonitor.IgnoreEvents(-1);
			m_CacheData.pop();
		}
	#ifdef _DEBUG
		AssertCacheDataValid();
	#endif
	}
	__finally
	{
		::LeaveCriticalSection(&m_CacheMonitor.m_CacheDataCriticalSection);
	}
}


/********************************************************************************************
> BOOL DownloadCache::Traverse(const String_256& strPath, BOOL bFlush = FALSE)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - strDirPath - full path of directory tree to traverse
					- bFlush - TRUE if we also want to flush the cache as we traverse it
	Returns:	- TRUE if successful, FALSE otherwise
	Purpose:	- walk the named directory tree collecting information about its contents
********************************************************************************************/

 BOOL DownloadCache::Traverse(const String_256& strDirPath, BOOL bFlush)
 {
 		::EnterCriticalSection(&m_CacheMonitor.m_CacheDataCriticalSection);
		_finddata_t	findData;
		String_256 strSearchPattern(strDirPath);
		if (strSearchPattern[strSearchPattern.Length() -1] == _T('\\'))
			strSearchPattern += _T('*');
		else
			strSearchPattern += _T("\\*"); // add wildcard
		INT32 hSearch = _tfindfirst(strSearchPattern, &findData);
		if (hSearch == -1)
			return TRUE; // the directory is empty
		do
		{
			if (!(camStrcmp(findData.name, _T(".")) &&  camStrcmp(findData.name, _T(".."))))
				continue; // skip this directory (.) or its parent (..)
			String_256 strFilePath(strDirPath);
			if (strFilePath[strFilePath.Length() -1] != _T('\\'))
				strFilePath += _T('\\');
			strFilePath += findData.name;
			strFilePath.toLower();
			CacheEntry entry(strFilePath);
			if (!entry.IsValid())
			{
				ERROR3("Invalid cache entry");
				::LeaveCriticalSection(&m_CacheMonitor.m_CacheDataCriticalSection);
				return FALSE;
			}
			BOOL bIsFolder = entry.IsFolder();
			BOOL bIsFile = entry.IsFile();
			if (entry.IsFolder()) 
				Traverse(strFilePath, bFlush);
			else if (entry.IsFile()) 
			{
				if (!bFlush)
				{
					m_CacheData.push(entry);
					m_lCurrentSize += entry.Size();
				}
				else if (!camStrstr(strFilePath, _T(".txt")))
					RemoveFile(strFilePath);
			}
		}
		while (_tfindnext(hSearch, &findData) == 0);
		_findclose(hSearch);
		::LeaveCriticalSection(&m_CacheMonitor.m_CacheDataCriticalSection);
		return TRUE;
 }


 /********************************************************************************************
 > BOOL DownloadCache::IsCachePath(const TCHAR* szPath);

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - szPath - full path of file/folder
	Returns:	- TRUE if the path points into the cache, FALSE othewise
	Purpose:	-  used internally, mainly to find out if cache monitoring should
						be turned off while modifying the file
 ********************************************************************************************/
 
 BOOL DownloadCache::IsCachePath(const TCHAR* szPath)
 {
	 String_256 strPath(szPath);
 	 strPath.toLower();
	 String_256 cachePath(m_strCachePath);
	 cachePath.toLower();
	 return (camStrstr(strPath, cachePath)) ? TRUE : FALSE;
 }

 
 
 /********************************************************************************************
> void DownloadCache::InsertFile(const TCHAR* szPath)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - szPath - full path of file being added
	Returns:	- 
	Purpose:	- notifies the cache that a new file has been added to it. If the 
						cache would overflow after the insertion, old files will be discarded
						until the cache size is below the preset limit
 ********************************************************************************************/

 void DownloadCache::InsertFile(const TCHAR* szPath)
 {
	// make sure the path points to a file in the cache - if it doesn't we'll just return
	if (!IsCachePath(szPath))
		return;
	 CacheEntry entry(szPath);
	if (!entry.IsValid())
	{
#ifdef _DEBUG
		TCHAR szMsg[256];
		TCHAR szError[128];
		switch (errno)
		{
			case ENOENT:
				camStrcpy(szError, "path not found (ENOENT)");
				break;
			default:
				wsprintf(szError, "errno = %d", errno);
		}
		wsprintf(szMsg, "Invalid cache entry: %s, %s", szPath, szError);
		ERROR3(szMsg);
#endif
		return;
	}
	if (!entry.IsFile())
		return;
	TRACEUSER( "adrian", _T("Cache size before inserting %s is %u bytes\n"), entry.szFilePath, m_lCurrentSize);
	::EnterCriticalSection(&m_CacheMonitor.m_CacheDataCriticalSection);
	m_CacheData.push(entry);
	m_lCurrentSize += entry.Size();
	TRACEUSER( "adrian", _T("Cache size after inserting %s is %u bytes\n"), entry.szFilePath, m_lCurrentSize);
	TRACEUSER( "adrian", _T("Cache usage is %d percent\n"), GetUsage());
	 // if the maximum cache size has been exceeded, discard files until we are within the limit again
	while (m_lCurrentSize > m_lMaxSize && m_CacheData.size() && entry != m_CacheData.top()) 
	{
		m_CacheMonitor.IgnoreEvents(1);
		if (!_tremove(m_CacheData.top().szFilePath))
		{
			m_lCurrentSize -= m_CacheData.top().Size();
			TRACEUSER( "adrian", _T("Discarded %s - new cache size is %u bytes\n"), m_CacheData.top().szFilePath, m_lCurrentSize);
			TRACEUSER( "adrian", _T("New cache usage is %d percent\n"), GetUsage());
		}
		else
			m_CacheMonitor.IgnoreEvents(-1);
		m_CacheData.pop();
	}
#ifdef _DEBUG
	AssertCacheDataValid();
#endif
	::LeaveCriticalSection(&m_CacheMonitor.m_CacheDataCriticalSection);
}


/********************************************************************************************
> INT32 DownloadCache::RemoveFile(const TCHAR* szPath)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - strPath - full path of file being removed
	Returns:	- 0 if the file is successfully deleted; otherwise, it returns 1 and sets 
					errno either to EACCES to indicate that the path specifies a read-only file,
					or to ENOENT to indicate that the filename or path was not found or that the
					path specifies a directory.

	Purpose:	- removes a file from the cache. Files in the cache should only be
						deleted via this function
 ********************************************************************************************/

 INT32 DownloadCache::RemoveFile(const TCHAR* szPath)
 {
	::EnterCriticalSection(&m_CacheMonitor.m_CacheDataCriticalSection);
	__try
	{
		 // make sure the path points to a file in the cache - if it doesn't we'll just return _tremove
		if (!IsCachePath(szPath))
			 return _tremove(szPath);
		CacheEntry entry(szPath);
		INT32 nRet;
		if (!entry.IsValid())
		{
			if (errno != ENOENT)
			{
				ERROR3("Invalid cache entry");
				m_CacheMonitor.IgnoreEvents(1);
				nRet = _tremove(szPath);
				if (nRet)
					m_CacheMonitor.IgnoreEvents(-1);
			}
			else 
				nRet = -1;
			return nRet;

		}
		if (!entry.IsFile())
		{
			errno = ENOENT;
			 return -1;
		}
		m_CacheMonitor.IgnoreEvents(1);
		nRet =_tremove(szPath);
		if (!nRet)
		{
			RemoveEntry(entry);
			m_lCurrentSize -= entry.Size();
		}
		else
		{
			m_CacheMonitor.IgnoreEvents(-1);	
	#ifdef _DEBUG
			TCHAR szMsg[256];
			TCHAR szError[128];
			switch (errno)
			{
				case EACCES:
					camStrcpy(szError, "access denied (EACCES)");
					break;
				case ENOENT:
					camStrcpy(szError, "path not found (ENOENT)");
					break;
				default:
					wsprintf(szError, "errno = %d", errno);
			}
			wsprintf(szMsg, "Failed to delete %s , %s", szPath, szError);
			ERROR3(szMsg);
	#endif
		}
	#ifdef _DEBUG
		AssertCacheDataValid();
	#endif
		return nRet;
	}
	__finally
	{
		::LeaveCriticalSection(&m_CacheMonitor.m_CacheDataCriticalSection);
	}

	return 0;
}


/********************************************************************************************
> INT32 DownloadCache::RenameFile(const TCHAR* szOldName, const TCHAR* szNewName)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - szOldName, szNewName old/new filenames (paths)
	Returns:	- 0 if it is successful. On an error, the function  returns a nonzero value and sets errno to
					one of the following values:
					EACCES :  File or directory specified by newname already exists or could not be created
					(invalid path); or strOldName is a directory and strNewName specifies a different path.
					ENOENT :  File or path specified by oldname not found.
	Purpose:	- renames a file in the cache. Files in the cache should only be
						renamed via this function
 ********************************************************************************************/

 INT32 DownloadCache::RenameFile(const TCHAR* szOldName, const TCHAR* szNewName)
 {
	String_256 strOldName(szOldName), strNewName(szNewName);
	BOOL bIsCachePath = IsCachePath(szOldName);
	if (bIsCachePath)
		m_CacheMonitor.IgnoreEvents(1);
	INT32 nRetValue = _trename(strOldName, strNewName);
	if (nRetValue && bIsCachePath)
		m_CacheMonitor.IgnoreEvents(-1);
	 // make sure the path points to a file in the cache - if it doesn't we'll just return nRetValue
	if (!bIsCachePath)
		 return nRetValue;
 	::EnterCriticalSection(&m_CacheMonitor.m_CacheDataCriticalSection);
	if (!nRetValue)
	{	
		CacheEntry entry(strNewName);
		if (!entry.IsValid())
		{
			ERROR3("Invalid cache entry");
		 	::LeaveCriticalSection(&m_CacheMonitor.m_CacheDataCriticalSection);
			return nRetValue;
		}
		if (!entry.IsFile())
		{
		 	::LeaveCriticalSection(&m_CacheMonitor.m_CacheDataCriticalSection);
			return nRetValue;
		}
		m_CacheData.push(entry); // push the new file entry on the cache data stack
		camStrcpy(entry.szFilePath, strOldName); 
		RemoveEntry(entry); // remove old entry
	}
#ifdef _DEBUG
	else
	{
		TCHAR szMsg[256];
		TCHAR szError[128];
		switch (errno)
		{
			case EACCES:
				camStrcpy(szError, "file already exists or could not be created (EACCES)");
				break;
			case ENOENT:
				camStrcpy(szError, "path not found (ENOENT)");
				break;
			default:
				wsprintf(szError, "errno = %d", errno);
		}
		wsprintf(szMsg, "Failed to rename %s to %s, %s", strOldName, strNewName, szError);
		ERROR3(szMsg);
	}
	AssertCacheDataValid();
#endif
 	::LeaveCriticalSection(&m_CacheMonitor.m_CacheDataCriticalSection);
	return nRetValue;
}


/********************************************************************************************
> DownloadCache::CacheMonitor::CacheMonitor()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - 
	Returns:	- 
	Purpose:	- constructor
 ********************************************************************************************/

 DownloadCache::CacheMonitor::CacheMonitor()
 {
	 m_hMonitorThread = NULL;
	 m_dwMonitorThreadID = 0;
	 ::InitializeCriticalSection(&m_CacheDataCriticalSection);
 }


/********************************************************************************************
> DownloadCache::CacheMonitor::~CacheMonitor()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - 
	Returns:	- 
	Purpose:	- destructor
 ********************************************************************************************/

 DownloadCache::CacheMonitor::~CacheMonitor()
 {
	 if (m_hMonitorThread && m_dwMonitorThreadID)
		 Deactivate();
	 ::DeleteCriticalSection(&m_CacheDataCriticalSection);
 }



/********************************************************************************************
> BOOL DownloadCache::CacheMonitor::Activate(String_256& strCachePath)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - strCachePath - the cache path to be monitored
	Returns:	- TRUE if succesful, FALSE otherwise
	Purpose:	- start up the cache monitor
 ********************************************************************************************/

BOOL DownloadCache::CacheMonitor::Activate(String_256& strCachePath)
{
	if (rgEvents[EVENT_DEACTIVATE] || rgEvents[EVENT_CHANGE])
		return FALSE;
	if (!(rgEvents[EVENT_DEACTIVATE] = ::CreateEvent(NULL, TRUE, FALSE, NULL)))
		return FALSE;
	if ((rgEvents[EVENT_CHANGE] = ::FindFirstChangeNotification(strCachePath, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME)) == INVALID_HANDLE_VALUE)
		return FALSE;
	if (!(m_hMonitorThread = (HANDLE) ::_beginthreadex(NULL, 0, &Monitor, (LPVOID) rgEvents, 0, &m_dwMonitorThreadID)))
		return FALSE;
	return TRUE;
}


/********************************************************************************************
> unsigned __stdcall DownloadCache::CacheMonitor::Monitor(LPVOID pParam)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - pParam - pointer to an array of event handles
	Returns:	- 0 if succesful, 0xFFFFFFFF otherwise
	Purpose:	- monitor thread function
 ********************************************************************************************/

unsigned __stdcall DownloadCache::CacheMonitor::Monitor(LPVOID pParam)
{
	TRACEUSER( "adrian", _T("Entering monitor thread\n"));
	HANDLE* lpEvents = (HANDLE*) pParam;
	while (::WaitForMultipleObjects(2, lpEvents, FALSE, INFINITE) != WAIT_OBJECT_0)
	{
		TRACEUSER( "adrian", _T("\n\n\nCache event detected\n"));
		if (m_nIgnoreCount > 0)
		{
			m_nIgnoreCount--;
#ifdef _DEBUG
			TCHAR szMsg[96];
			wsprintf(szMsg, "Cache ignore count decremented to %d\n\n\n", m_nIgnoreCount);
			TRACEUSER("adrian", szMsg);
#endif
		}
		else
			OnCacheEvent();
		if (!::FindNextChangeNotification(*(lpEvents + 1)))
			::_endthreadex(0xFFFFFFFF);
	}
	TRACEUSER( "adrian", _T("Exiting monitor thread\n"));
	return 0;
}


/********************************************************************************************
> BOOL DownloadCache::CacheMonitor::Deactivate()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  -
	Returns:	- TRUE if succesful, FALSE otherwise
	Purpose:	- cache monitor close-down function - should be called on exit
 ********************************************************************************************/

BOOL DownloadCache::CacheMonitor::Deactivate()
{
	BOOL bRet = TRUE;
	if (!m_hMonitorThread)
		return bRet;
	if (!::SetEvent(rgEvents[0]))
		goto TERMINATE;
	if (::WaitForSingleObject(m_hMonitorThread, 1000) == WAIT_TIMEOUT)
	{
TERMINATE:
		bRet = FALSE;
		ERROR3("Warning: monitor thread not released - will attempt to terminate it");
		if (!::TerminateThread(m_hMonitorThread, 0xFFFFFFFF))
		{
			TCHAR szErrorMsg[96];
			wsprintf(szErrorMsg, "Cannot kill zombie thread, thread ID = %lx", m_dwMonitorThreadID);
			ERROR3(szErrorMsg);
		}
	}
	::CloseHandle(m_hMonitorThread);
	m_hMonitorThread = NULL;
	m_dwMonitorThreadID = 0;
	::CloseHandle(rgEvents[0]);
	::FindCloseChangeNotification(rgEvents[1]);
	memset(rgEvents, 0x00, sizeof(rgEvents));
	return bRet;
}


/********************************************************************************************
> BOOL DownloadCache::CacheMonitor::OnCacheEvent()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - 
	Returns:	- 
	Purpose:	- very simple cache event-handling function. It will only output
						a warning message and update the cache data
 ********************************************************************************************/

void DownloadCache::CacheMonitor::OnCacheEvent()
{
	String_256 strErrorMsg(_R(IDS_CACHEMODIFIED));
	String_256 strCaption(_R(IDS_ERRORBOX_WARNING));
	::MessageBox(NULL, strErrorMsg, strCaption, MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_OK);
	::EnterCriticalSection(&m_CacheDataCriticalSection);
	DownloadCache::Refresh();
	::LeaveCriticalSection(&m_CacheDataCriticalSection);
}



/********************************************************************************************
> CacheEntry::CacheEntry(const String_256& strPath)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/97
	Inputs:		  - path of file that that the object represents
	Returns:	- 
	Purpose:	- constructor
********************************************************************************************/
CacheEntry::CacheEntry(const String_256& strPath)
{
	camStrcpy(szFilePath, strPath);
	if (m_bIsValid = (!_tstat(szFilePath, &m_FileStat))) 
	{
		String_256 strFilePath(strPath);
		strFilePath.toLower();
		PathName filePath(strFilePath);
		if (filePath.GetType() == _T("txt"))
			m_nPriority = PRIORITY_CURRENT_CATALOG_FILE;
		else if (filePath.GetType() == _T("old"))
			m_nPriority = PRIORITY_OLD_CATALOG_FILE;
		else
		{
			//find out if the file is in a clipart, font or fill folder
			INT32 rgTypes[5] = {SGLib_ClipArt, SGLib_Font, SGLib_Fractal, SGLib_ClipArt_WebThemes, SGLib_Blank};
			INT32 nLibType;
			String_256 strAppDataPath;
			GetAppDataPath(&strAppDataPath);
			PathName appDataPath(strAppDataPath);
			for (INT32 i = 0; i < 5; i++)
			{
				nLibType = rgTypes[i];
				if (nLibType == SGLib_Blank)
					break;
				String_256 strCacheSubdir = GetStringField(rgTypes[i], _R(IDS_CACHEDIR));
				String_256 strTypePath(appDataPath.GetLocation(TRUE)); // path of files of type rgTypes[i]
				strTypePath += strCacheSubdir;
				strTypePath.toLower();
				if (camStrstr(strFilePath, strTypePath))
					break;
			}
			// is the file a thumbnail?
			BOOL bIsThumb = (camStrstr(strFilePath, _T("xarainfo"))) && (filePath.GetType() == _T("png"));
			switch (nLibType)
			{
				case SGLib_ClipArt:
				case SGLib_ClipArt_WebThemes:
					m_nPriority = bIsThumb ? PRIORITY_CLIPART_THUMBNAIL : PRIORITY_CLIPART_FILE;
					break;
				case SGLib_Font:
					m_nPriority = bIsThumb ? PRIORITY_FONT_THUMBNAIL : PRIORITY_FONT_FILE;
					break;
				case SGLib_Fractal:
					m_nPriority = bIsThumb ? PRIORITY_FILL_THUMBNAIL : PRIORITY_FILL_FILE;
					break;
				default:
					m_nPriority = PRIORITY_OTHER;
			}
		}
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Internet Manager
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Static members
INT32 InternetManager::nMaxConnections = 32; // maximum no. of simultaneous connections/downloads
CTypedPtrArray<CPtrArray, AsynchDownload*> InternetManager::m_CurrentDownloads;
DownloadQueue InternetManager::m_NormalPriorityQueue;
DownloadQueue InternetManager::m_HighPriorityQueue;
DOWNLOAD_HANDLE InternetManager::m_NextHandle = 1L;
AsynchDownload::State InternetManager::m_StateTable[1000];
String_256 InternetManager::strTempFileURL; 
BOOL InternetManager::m_bIsSuspended = FALSE;
INT32 InternetManager::rgConnections[4] = {4, 8, 20, 64};

// Preferences
UINT32	InternetManager::g_ConnectionType		= CONNECTION_SLOWMODEM; // we default to the most prudent setting
UINT32	InternetManager::g_CacheSize			= 5000;


/********************************************************************************************
> InternetManager::Initialize

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  -
	Returns:	- 
	Purpose:	- initialization function
********************************************************************************************/
void InternetManager::Initialize()
{
	WORD wVersionRequested; // first initialize to socket library
	WSADATA wsaData; 
	wVersionRequested = MAKEWORD(1, 1); 
 	if (::WSAStartup(wVersionRequested, &wsaData))
		InformError(_R(IDS_SOCKETNOTINITIALIZED));
	m_HighPriorityQueue.SetType(DownloadQueue::LIFO); // the high priority queue is LIFO by default
	String_256 strCachePath;
	GetAppCachePath(&strCachePath);
	DownloadCache::SetPath(strCachePath);
	/*if (!DownloadCache::m_CacheMonitor.Activate(strCachePath))
	{
		ERROR3("Cannot activate the cache monitor");
	}*/
/*
	// This code is an abortion and unmaintainable. Why not use the preference system?
	HKEY arhKeys[6];
	DWORD dwConnectionType = 0L;
	UINT32 lCacheSize = 0L;
	DWORD dwSizeofDword = sizeof(DWORD), dwSizeofLong = sizeof(INT32);
	DWORD dwDisposition;
	memset(arhKeys, 0, sizeof(arhKeys));
	String_256 strProgramKey = GetProgramNameRegistryKey();
	BOOL bResult = ((RegOpenKeyEx(HKEY_CURRENT_USER, "Software", 0, KEY_ALL_ACCESS, &arhKeys[0]) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[0], _tcstok(strProgramKey, _T("\\")),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[1], &dwDisposition) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[1], _tcstok(NULL, _T("\\")),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[2], &dwDisposition) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[2], _tcstok(NULL, _T("\\")),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[3], &dwDisposition) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[3], _T("Options"),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[4], &dwDisposition) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[4], _T("Internet"),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[5], &dwDisposition) == ERROR_SUCCESS) &&
		(RegQueryValueEx(arhKeys[5], _T("Connection Type"),	NULL, NULL, (LPBYTE) &dwConnectionType,	&dwSizeofDword) == ERROR_SUCCESS) && 
		(RegQueryValueEx(arhKeys[5], _T("Cache Size"),	NULL, NULL, (LPBYTE) &lCacheSize, &dwSizeofLong) == ERROR_SUCCESS));
	for (INT32 i = 5; i >= 0; i--) RegCloseKey(arhKeys[i]);
	if (bResult)
	{
		nMaxConnections = rgConnections[dwConnectionType];
		DownloadCache::SetSize(lCacheSize);
	}
	else
		SetConnectionType(CONNECTION_SLOWMODEM); // we default to the most prudent setting 
*/

	if (Camelot.DeclareSection(TEXT("Internet"), 4))
	{
		Camelot.DeclarePref(NULL, TEXT("Connection Type"), &g_ConnectionType, 0, 3);
		Camelot.DeclarePref(NULL, TEXT("Cache Size"), &g_CacheSize, 64 * SIZEOFKILOBYTE);
	}
	nMaxConnections = rgConnections[g_ConnectionType];
	DownloadCache::SetSize(g_CacheSize);
}


/********************************************************************************************
> InternetManager::OnExitInstance()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  -
	Returns:	-
	Purpose:	- close-down function
********************************************************************************************/


void InternetManager::OnExitInstance()
{
	::WSACleanup();
	/*if (!DownloadCache::m_CacheMonitor.Deactivate())
	{
		ERROR3("Cannot deactivate the cache monitor - likely to leak on exit");
	}*/

	// ensure there are no downloads left behind
	if (HasDownloadsPending())
	{
 		m_HighPriorityQueue.Flush();
		m_NormalPriorityQueue.Flush();
		for (INT32 i = 0; i < m_CurrentDownloads.GetSize(); i++) 
		{
			if (m_CurrentDownloads[i])
				m_CurrentDownloads[i]->Release();
		}
	}
	// Remove the last downloaded file from the cache, if any
	if (!strTempFileURL.IsEmpty())
	{
		UnlockUrlCacheEntryFile((TCHAR*) strTempFileURL, 0);
		DeleteUrlCacheEntry((TCHAR*) strTempFileURL); // nb: this call may fail if the file is still locked for some reason
	}
}




/********************************************************************************************
> DOWNLOAD_HANDLE InternetManager::RegisterDownload(LPDOWNLOADINFO pDownloadInfo)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		 pDownloadInfo - pointer to DOWNLOADINFO struct
	Returns:	non-zero download handle if successful, 0 otherwise
	Purpose:	register a new file download. The download will be started immediately if nothing else is pending; otherwise, it will just be 
					queued up
	NOTES	 1. Client objects are not directly notified of the outcome of the downloads they have registered. Callback functions called
					through pointers are not a very safe way to go as the pointer might not be valid anymore by the time the download is over.
					Instead, the client should poll the InternetManager for the result of the download, using the handle return by RegisterDownload().
					Usually the client object will be derived from Operation so the polling could be done on IdleEvent() .
					2. If the queue with the given priority is locked, the download is registered and you get a valid handle but it will immediatelly
					flagged as ABORTED. The queues are locked in response to a user cancel. 

	SUBSEQUENT NOTES by Phil:
	04/05/2004	Client objects CAN now be notified of the outcome of the downloads. (Idle
				polling of the download status was thought to be too tacky.)
				RegisterDownload now takes in a window handle parameter and  token parameter.
				If the handle is valid and the token is non-zero then custom
				messages will be sent to that window containing the token. The handler of
				those messages can use the token to decide which object should receive the
				notification.

********************************************************************************************/



DOWNLOAD_HANDLE InternetManager::RegisterDownload(LPDOWNLOADINFO pDownloadInfo)
{
	if (m_NextHandle == 1L) // first download, initialise the state table
		memset(m_StateTable, 0, sizeof(m_StateTable));

	// Check if the download has been previously registered - if so, return the same handle
	AsynchDownload* pPreviousDownload = GetPendingDownload(pDownloadInfo->strLocalFile);
	if (pPreviousDownload)
		return pPreviousDownload->GetHandle();

	DOWNLOAD_HANDLE handle = m_NextHandle;
	m_NextHandle++;
 	if (camStrstr((TCHAR*) pDownloadInfo->strURL, _T("file://")))
	{
		PathNameEx(pDownloadInfo->strLocalFile).CreateLocation();
#ifdef UNICODE
		WCHAR wchURL[INTERNET_MAX_PATH_LENGTH];
		MultiByteToWideChar(CP_ACP, 0,  (TCHAR*) pDownloadInfo->strURL, -1, wchURL, INTERNET_MAX_PATH_LENGTH);
		if (SUCCEEDED(::URLDownloadToFile(NULL, wchURL, (TCHAR*) pDownloadInfo->strLocalFile, 0, NULL)))
#else
		if (SUCCEEDED(::URLDownloadToFile(NULL, (TCHAR*) pDownloadInfo->strURL, (TCHAR*) pDownloadInfo->strLocalFile, 0, NULL)))
#endif
//			m_StateTable[handle] = AsynchDownload::STATE_SUCCEEDED;
			SetState(handle, AsynchDownload::STATE_SUCCEEDED);
		else
//			m_StateTable[handle] = AsynchDownload::STATE_FAILED;
			SetState(handle, AsynchDownload::STATE_FAILED);
		return handle;
	}
	AsynchDownload* pDownload = new AsynchDownload(handle, pDownloadInfo);
	if (!pDownload)
	{
		ERROR3("Memory allocation error");
		return (DOWNLOAD_HANDLE) INVALID_HANDLE_VALUE;
	}
//	m_StateTable[handle] = AsynchDownload::STATE_PENDING;
	SetState(handle, AsynchDownload::STATE_PENDING);
	if (m_CurrentDownloads.GetSize() < nMaxConnections && !m_bIsSuspended) // can start it straight away
	{
		if (pDownloadInfo->nPriority == AsynchDownload::PRIORITY_NORMAL && GetOpenConnections(AsynchDownload::PRIORITY_NORMAL))
			goto QUEUE; // this is a normal pDownloadInfo->nPriority download and we already have one executing, so we just queue it up
		if (!SUCCEEDED(pDownload->StartDownload()))
		{
//			m_StateTable[handle] = AsynchDownload::STATE_FAILED;
			SetState(handle, AsynchDownload::STATE_FAILED);
			pDownload->Release();
		}
		else
		{
			// add it to the array of executing downloads
			m_CurrentDownloads.Add(pDownload);
			if (pDownloadInfo->nPriority == AsynchDownload::PRIORITY_NORMAL)
			TRACEUSER( "adrian", _T("Download fired up for %s\n"), (TCHAR*) String_256(pDownload->GetLocalFileName())); 
		}
	}
	else
	{
QUEUE:
		// queue the download, as we shouldn't exceed the allowed number of connections
		if (pDownloadInfo->nPriority == AsynchDownload::PRIORITY_NORMAL)
			m_NormalPriorityQueue.Queue(pDownload);
		else
		{
			// switch the high pDownloadInfo->nPriority queue to FIFO mode if this is a thumbnail
			if (pDownloadInfo->nFileType == TYPE_THUMBNAIL)
				m_HighPriorityQueue.SetType(DownloadQueue::FIFO);
			m_HighPriorityQueue.Queue(pDownload);
			if (pDownloadInfo->nFileType == TYPE_THUMBNAIL)
				m_HighPriorityQueue.SetType(DownloadQueue::LIFO); // back to default (LIFO) mode
		}
	}
	return handle;
}



/********************************************************************************************
> BOOL InternetManager::UnregisterDownload(DOWNLOAD_HANDLE hDownload)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  hDownload - handle of download as returned by a previous call to RegisterDownload()
	Returns:	TRUE if succeeds, FALSE
	Purpose:	unregisters a previously registered download. It behaves like the following:
							- if the download has already finished, the function will fail
							- if the download is in progress, it will be aborted on the next call from the callback object,
								and the AsynchDownload object deleted as soon as this call returns
							- if the download is still queued up, it will be removed from the queue and deleted
********************************************************************************************/



BOOL InternetManager::UnregisterDownload(DOWNLOAD_HANDLE hDownload)
{
	AsynchDownload::State downloadState = GetDownloadState(hDownload);
	if (downloadState == AsynchDownload::STATE_PENDING) // download is still pending, i.e. either happening now or queued up
	{
		AsynchDownload* pDownload = GetPendingDownload(hDownload);
		if (!pDownload)
		{
			ERROR3("Unexpected NULL pointer");
			return FALSE;
		}
		// We flag the download as aborted, although we may not be able to abort it immediately - it might even succeed in the mean time
		m_StateTable[pDownload->GetHandle()] = AsynchDownload::STATE_ABORTED; 
// Call SetState???
		// Find out if the download is actually executing
		BOOL bIsExecuting = FALSE;
		for (INT32 i = 0; i < m_CurrentDownloads.GetSize(); i++) 
		{
			if (m_CurrentDownloads[i] == pDownload)
				bIsExecuting = TRUE;
		}
		if (bIsExecuting) // download happening now
		{
			// Abort the download. Note that we don't delete it, doing so would result in a leak of socket resources.
			// AbortDownload() is asynchronous and returns immediately - the download will actually be aborted
			// when the server acknowedges the TCP_RESET message and closes the connection so Winsock.dll
			// can close its open socket.
			HRESULT result = pDownload->AbortDownload();
			ERROR3IF(result == E_FAIL, "Could not abort download");
		}
		else // download is still in one of the queues
		{
			if (m_NormalPriorityQueue.Remove(pDownload) || m_HighPriorityQueue.Remove(pDownload))
				return TRUE;
			else
			{
				ERROR3("Failed to unregister download");
				return FALSE;
			}
		}
	}
	return TRUE;
}




/********************************************************************************************
> AsynchDownload InternetManager::GetPendingDownload(const String_256& strFileName)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  strFileName
	Returns:	pointer to AsynchDownload object if found, NULL otherwise
	Purpose:	get a pointer to a download object knowing the the local file it's downloading to
********************************************************************************************/


AsynchDownload* InternetManager::GetPendingDownload(const String_256& strFileName)
{
	AsynchDownload* pDownload = NULL;
	String_256 strLocalFileName(strFileName);
	// Check the downloads currently executing
	for (INT32 i = 0; i < m_CurrentDownloads.GetSize(); i++) 
		if (!strLocalFileName.CompareTo(m_CurrentDownloads[i]->GetLocalFileName(), FALSE))
			return m_CurrentDownloads[i];
	// Check the download queues
	if (pDownload = m_NormalPriorityQueue.FindDownload(strLocalFileName))
		return pDownload;
	pDownload = m_HighPriorityQueue.FindDownload(strLocalFileName);
	return pDownload;
}



/********************************************************************************************
> INT32 InternetManager::GetPercentageDownloaded(DOWNLOAD_HANDLE hDownload)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  hDownload
	Returns:	an INT32 in the range of 0-100 if the handle is valid, -1 otherwise
	Purpose:	monitor the percentage downloaded on a handle
********************************************************************************************/

INT32 InternetManager::GetPercentageDownloaded(DOWNLOAD_HANDLE hDownload)
{
	AsynchDownload* pDownload = GetPendingDownload(hDownload);
	if (pDownload)
		return pDownload->GetPercentageDownloaded();
	else
		return -1;
}



/********************************************************************************************
> AsynchDownload InternetManager::GetPendingDownload(DOWNLOAD_HANDLE hDownload)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  hDownload
	Returns:	pointer to AsynchDownload object if found, NULL otherwise
	Purpose:	get a pointer to a download object knowing the download's handle
********************************************************************************************/

AsynchDownload* InternetManager::GetPendingDownload(DOWNLOAD_HANDLE hDownload)
{
	AsynchDownload* pDownload = NULL;
	AsynchDownload::State downloadState = GetDownloadState(hDownload);
	if (downloadState != AsynchDownload::STATE_PENDING)
		return pDownload;
	// Check the downloads currently executing
	for (INT32 i = 0; i < m_CurrentDownloads.GetSize(); i++) 
	{
		if (m_CurrentDownloads[i]->GetHandle() == hDownload)
			return m_CurrentDownloads[i];
	}
	// Check the download queues
	if (pDownload = m_NormalPriorityQueue.FindDownload(hDownload))
		return pDownload;
	pDownload = m_HighPriorityQueue.FindDownload(hDownload);
	return pDownload;
}



/********************************************************************************************
> AsynchDownload InternetManager::GetDownload(DOWNLOAD_HANDLE hDownload)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/05/2004
	Inputs:		  hDownload
	Returns:	pointer to AsynchDownload object if found, NULL otherwise
	Purpose:	get a pointer to a download object knowing the download's handle
********************************************************************************************/

AsynchDownload* InternetManager::GetDownload(DOWNLOAD_HANDLE hDownload)
{
	AsynchDownload* pDownload = NULL;

	// Check the downloads currently executing
	for (INT32 i = 0; i < m_CurrentDownloads.GetSize(); i++) 
	{
		if (m_CurrentDownloads[i]->GetHandle() == hDownload)
			return m_CurrentDownloads[i];
	}
	// Check the download queues
	if (pDownload = m_NormalPriorityQueue.FindDownload(hDownload))
		return pDownload;
	pDownload = m_HighPriorityQueue.FindDownload(hDownload);
	return pDownload;
}



/********************************************************************************************
> AsynchDownload:State InternetManager::GetDownloadState(DOWNLOAD_HANDLE hDownload)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  hDownload - handle of the download inquired on
	Returns:	a value of InternetManager::State type reflecting the state of the download:
					STATE_ERROR - the hDownload param is not valid or some other internal error occurred
					STATE_PENDING - download is still pending 
					STATE_FAILED - download has failed through network or other "natural" errors
					STATE_SUCCEEDED - download has succeeded 
					STATE_ABORTED - user cancel
	Purpose:   Test the current state of a registered download - usually used in a loop polling for
					completion, either successful or failed
********************************************************************************************/


AsynchDownload::State InternetManager::GetDownloadState(DOWNLOAD_HANDLE hDownload)
{
	// For quick access handles are used as indexes into the state table
	if ((hDownload < 1) || (hDownload >= MAX_HANDLE))
		return AsynchDownload::STATE_ERROR;
	else
		return m_StateTable[hDownload];
}


/********************************************************************************************
> void InternetManager::OnDownloadComplete(AsynchDownload* pDownload)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  pDownload - pointer to object sending the notification
	Returns:	none 
	Purpose:   Callback function notifying the download manager the an object has completed downloading
					Used internally only.
********************************************************************************************/



void InternetManager::OnDownloadComplete(AsynchDownload* pDownload)
{
	if (!pDownload)
	{
		ERROR3("Unexpected NULL pointer");
		return;
	}
	if (pDownload->HasSucceeded())
	{
//		m_StateTable[pDownload->GetHandle()] = AsynchDownload::STATE_SUCCEEDED;
		DownloadCache::InsertFile(pDownload->GetLocalFileName());
		SetState(pDownload->GetHandle(), AsynchDownload::STATE_SUCCEEDED);
	}
	else
	{
		if (pDownload->WasAborted()) // user cancel
//			m_StateTable[pDownload->GetHandle()] = AsynchDownload::STATE_ABORTED;	
			SetState(pDownload->GetHandle(), AsynchDownload::STATE_ABORTED);
		else // failed through network or other errors
//			m_StateTable[pDownload->GetHandle()] = AsynchDownload::STATE_FAILED;
			SetState(pDownload->GetHandle(), AsynchDownload::STATE_FAILED);
	}
	
	// Remove the download from its slot 
	INT32 nSlot = -1;
	for (INT32 i = 0; i < m_CurrentDownloads.GetSize(); i++) 
	{
		if (m_CurrentDownloads[i] == pDownload)
			nSlot = i;
	}
	ERROR3IF(nSlot == -1, "Download terminated without being executed");
	if (nSlot != -1)
		m_CurrentDownloads.RemoveAt(nSlot);
	if (!strTempFileURL.IsEmpty())
	{
		UnlockUrlCacheEntryFile((TCHAR*) strTempFileURL, 0);
		DeleteUrlCacheEntry((TCHAR*) strTempFileURL); // nb: this call may fail if the file is still locked for some reason
	}
	strTempFileURL = pDownload->GetTargetURL(); // URL of temp file to be removed on the next call
	if (nSlot != -1)
		pDownload->Release();
	if (m_bIsSuspended) // if suspended simply return
		return;
	// Start the next download in the queues, if any - we should have at least one connection available
	while ((!m_HighPriorityQueue.IsEmpty() || !m_NormalPriorityQueue.IsEmpty()) && m_CurrentDownloads.GetSize() < nMaxConnections)
	{
		AsynchDownload* pNextDownload = m_HighPriorityQueue.GetNextDownload();
		if (!pNextDownload)
		{
			// the high priority queue is empty; we'll start a normal priority one if none is already executing or the queue is not
			// locked
			if  (!GetOpenConnections(AsynchDownload::PRIORITY_NORMAL))
				pNextDownload = m_NormalPriorityQueue.GetNextDownload();
		}
		if (!pNextDownload) // both queues empty or locked
			return;
		if (!SUCCEEDED(pNextDownload->StartDownload()))
		{
//			m_StateTable[pNextDownload->GetHandle()] = AsynchDownload::STATE_FAILED;
			SetState(pNextDownload->GetHandle(), AsynchDownload::STATE_FAILED);
			pNextDownload->Release();
		}
		else
		{
			// add it the array of executing downloads
			m_CurrentDownloads.Add(pNextDownload);
			TRACEUSER( "adrian", _T("Download fired up for %s\n"), (TCHAR*) String_256(pNextDownload->GetLocalFileName())); 
		}
	}
}


/********************************************************************************************
> void InternetManager::CancelAllDownloads()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  none
	Returns:	none 
	Purpose:   Aborts any downloads in progress and flushes both queues. Any download
					thus removed will be flagged as ABORTED.
					Should be called either if the user chooses to abort all downloads or
					we detect some severe network error that makes it pointless to continue.
********************************************************************************************/
		
void InternetManager::CancelAllDownloads()
{
	// First empty the queues
	while (!m_HighPriorityQueue.IsEmpty())
	{
		AsynchDownload* pDownload = m_HighPriorityQueue.GetNextDownload();
		if (pDownload)
			UnregisterDownload(pDownload->GetHandle());
	}
	while (!m_NormalPriorityQueue.IsEmpty())
	{
		AsynchDownload* pDownload = m_NormalPriorityQueue.GetNextDownload();
		if (pDownload)
			UnregisterDownload(pDownload->GetHandle());
	}
	// Abort the downloads in progress if any
	// Note that we don't delete it straight away as we might leek resources if we did so - instead,
	// the object will be deleted when AbortDownload() (which is asynchronous) completes
	for (INT32 i = 0; i < m_CurrentDownloads.GetSize(); i++) 
	{
		HRESULT result = m_CurrentDownloads[i]->AbortDownload();
		ERROR3IF(result == E_FAIL , "Could not abort download");
	}
}




/********************************************************************************************
> INT32 InternetManager::GetOpenConnections(AsynchDownload::Priority priority)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  priority of the connections enquired about (enumerated type in AsynchDownload)
	Returns:	number of connections with the given priority which are currently open
	Purpose:   used in scheduling downloads
********************************************************************************************/

INT32 InternetManager::GetOpenConnections(AsynchDownload::Priority priority)
{
	INT32 nDownloads = 0;
	for (INT32 i = 0; i < m_CurrentDownloads.GetSize(); i++) 
	{
		if (m_CurrentDownloads[i]->GetPriority() == priority)
			nDownloads++;
	}
	return nDownloads;
}



							

/********************************************************************************************
> void InternetManager::Resume()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  none
	Returns:	none 
	Purpose:   Resumes normal downloading previously stopped by a call to Suspend()

********************************************************************************************/

void InternetManager::Resume()
{
	m_bIsSuspended = FALSE;
	while ((!m_HighPriorityQueue.IsEmpty() || !m_NormalPriorityQueue.IsEmpty()) && m_CurrentDownloads.GetSize() < nMaxConnections)
	{
		AsynchDownload* pNextDownload = m_HighPriorityQueue.GetNextDownload();
		if (!pNextDownload)
		{
			// the high priority queue is empty; we'll start a normal priority one if none is already executing or the queue is not
			// locked
			if  (!GetOpenConnections(AsynchDownload::PRIORITY_NORMAL))
				pNextDownload = m_NormalPriorityQueue.GetNextDownload();
		}
		if (!pNextDownload) // both queues empty or locked
			return;
		if (!SUCCEEDED(pNextDownload->StartDownload()))
		{
//			m_StateTable[pNextDownload->GetHandle()] = AsynchDownload::STATE_FAILED;
			SetState(pNextDownload->GetHandle(), AsynchDownload::STATE_FAILED);
			pNextDownload->Release();
		}
		else
		{
			// add it the array of executing downloads
			m_CurrentDownloads.Add(pNextDownload);
			TRACEUSER( "adrian", _T("Download fired up for %s\n"), (TCHAR*) String_256(pNextDownload->GetLocalFileName())); 
		}
	}
}

/********************************************************************************************
> AsynchDownload* InternetManager::GetCurrentNormalPriorityDownload()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  none
	Returns:	pointer to the currently executing normal priority download, NULL if none is found 
	Purpose:   provides access to thumbnail & catalog downloading (monitor/abort it, etc)

********************************************************************************************/

AsynchDownload* InternetManager::GetCurrentNormalPriorityDownload()
{
	AsynchDownload* pDownload = NULL;
	for (INT32 i = 0; i < m_CurrentDownloads.GetSize(); i++) 
	{
		if (m_CurrentDownloads[i]->GetPriority() == AsynchDownload::PRIORITY_NORMAL)
			pDownload = m_CurrentDownloads[i];
	}
	return pDownload;
}




/********************************************************************************************
> void InternetManager::SetConnectionType()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  type - one of CONNECTION_SLOWMODEM, CONNECTION_FASTMODEM, 
						CONNECTION_X2, CONNECTION_ISDN 
	Returns:	- 
	Purpose:   - allows the altering of the default connection type (CONNECTION_SLOWMODEM) 
						Setting this parameter accurately should result in improved download speed.

********************************************************************************************/
void InternetManager::SetConnectionType(ConnectionType type)
{
	// Save away the type in our preference variable
	g_ConnectionType = type;
	// Now set the new number of maximum connections according to this
	nMaxConnections = rgConnections[type];

	// When the user sets their coannection speed explicitly, we should also
	// update the flag that the helkp and support download system uses
	if (type==CONNECTION_ISDN)
		HelpDownloadOp::HighBandwidth = TRUE;
	else
		HelpDownloadOp::HighBandwidth = FALSE;

/*
	// This code is an abortion and unmaintainable. Why not use the preference system?
	HKEY arhKeys[6];
	DWORD dwConnectionType = (DWORD) type;
	DWORD dwDisposition;
	memset(arhKeys, 0, sizeof(arhKeys));
	String_256 strProgramKey = GetProgramNameRegistryKey();
	BOOL bResult = ((RegOpenKeyEx(HKEY_CURRENT_USER, "Software", 0, KEY_ALL_ACCESS, &arhKeys[0]) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[0], _tcstok(strProgramKey, _T("\\")),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[1], &dwDisposition) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[1], _tcstok(NULL, _T("\\")),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[2], &dwDisposition) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[2], _tcstok(NULL, _T("\\")),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[3], &dwDisposition) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[3], _T("Options"),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[4], &dwDisposition) == ERROR_SUCCESS) &&
		(RegCreateKeyEx(arhKeys[4], _T("Internet"),  0, NULL, REG_OPTION_NON_VOLATILE,  KEY_ALL_ACCESS, NULL, &arhKeys[5], &dwDisposition) == ERROR_SUCCESS) &&
		(RegSetValueEx(arhKeys[5], _T("Connection Type"),	NULL, REG_DWORD, (LPBYTE) &dwConnectionType,	sizeof(dwConnectionType)) == ERROR_SUCCESS));
	for (INT32 i = 5; i >= 0; i--) RegCloseKey(arhKeys[i]);
	ERROR3IF(!bResult, "Failed to save changes to registry");
*/
 }


/********************************************************************************************
> void InternetManager::GetConnectionType()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		  -
	Returns:	-  one of CONNECTION_SLOWMODEM, CONNECTION_FASTMODEM, 
						CONNECTION_X2, CONNECTION_ISDN
	Purpose:   - retrieves the connection type the InternetManager is configured for. The
						connection type is inferred from the maximum number of connections 
						that can be opened simultaneously

********************************************************************************************/
ConnectionType InternetManager::GetConnectionType()
{
	for (INT32 type = (INT32) CONNECTION_SLOWMODEM; type <= (INT32) CONNECTION_ISDN; type++)
	{
		if (rgConnections[type] == nMaxConnections) 
			break; 
	}
	return (ConnectionType) type;
}

/********************************************************************************************
> BOOL InternetManager::GetProxyServer(SERVENT* pProxyEntry, bool* pbProxyEnabled)
	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		- pProxyEntry - pointer to SERVENT structure to receive proxy information 						
	Returns:	-  TRUE if successful, FALSE otherwise (in which case the struct pointed to by 
				pProxyAddress will contain garbage and should not be used any further)
	Purpose:	- gets the current (system-wide) proxy settings
********************************************************************************************/
bool InternetManager::GetProxyServer(SERVENT* pProxyEntry, bool* pbProxyEnabled)
{
	// Get the current proxy settings
	TCHAR szProxy[_MAX_PATH];
	String_256 KeySectionName(TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"));
	HKEY hKey = OpenRegKey(HKEY_CURRENT_USER, KeySectionName);
	if (hKey)
	{
		String_256 ProxyServer;
		BOOL ok = GetRegString(hKey, TEXT("ProxyServer"), &ProxyServer);
		if (ok)
			camStrcpy(szProxy, (TCHAR*)ProxyServer);
		*pbProxyEnabled = GetRegBool(hKey, TEXT("ProxyEnable")) == TRUE;
		CloseRegKey(hKey);
	}

	// If the string is blank then the proxy must be off
	if (!camStrlen(szProxy))
	{
		*pbProxyEnabled = false;
		return true;
	}

	String_256 strHost, strPort;
	TCHAR tchBuff[_MAX_PATH];
	camStrcpy(tchBuff, szProxy);
	TCHAR* lpszTemp = NULL;
	lpszTemp = camStrstr(tchBuff, _T("http="));
	if (lpszTemp != NULL)
	{
		strHost = _tcstok((lpszTemp + 5), _T(":"));
		strPort = _tcstok(NULL, _T(";"));
	}
	else
	{
		strHost = _tcstok(tchBuff, _T(":"));
		strPort = _tcstok(NULL, _T(";"));
	}
	if (!camStrlen(strHost) || !camStrlen(strPort))
	{
		*pbProxyEnabled = false;
		return true;
	}

	camStrcpy(pProxyEntry->s_name, strHost);
	pProxyEntry->s_port = (unsigned short) atoi(strPort);
	return true;
}


/********************************************************************************************
> void InternetManager::SetState(DOWNLOAD_HANDLE hDownload, AsynchDownload::State state)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/05/2004
	Inputs:		-
	Returns:	-
	Purpose:	- Set the status of this download and inform the client of the change

********************************************************************************************/
void InternetManager::SetState(DOWNLOAD_HANDLE hDownload, AsynchDownload::State state)
{
	m_StateTable[hDownload] = state;

	// -------------------------------------
	// Find the download object
	AsynchDownload* pDownload = GetDownload(hDownload);

	// -------------------------------------
	// If we have it then see whether the client wants to be notified
	if (pDownload)
	{
		HWND hwndNotify = pDownload->GetNotifyHWND();
		INT32 lNotifyToken = pDownload->GetNotifyToken();
		if (hwndNotify && lNotifyToken)
		{
			// Send the message to the client without waiting
			::PostMessage(hwndNotify, WM_USER_DOWNLOADSTATUS, state, lNotifyToken);
		}
	}
}

