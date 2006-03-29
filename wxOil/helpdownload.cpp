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

#include "helpdownload.h"
//#include "webster.h"
#include "impexpop.h"
//#include "resimmap.h"	//_R(IDS_HTML_DOWNLOADFAILED)
#include "xmlutils.h"
#include "fileutil.h"
#include "resdll.h"
//#include "phil.h"
#include "helpdownloadwebdialog.h"
#include "helpuser.h"
#include "registry.h"
#include "product.h"
#include "afxdisp.h"
#include "menuops.h"
#include "progress.h"

#import "webfileupdater.dll"
using namespace WEBFILEUPDATERLib;


DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE(HelpDownloadOp, DownloadOp)
CC_IMPLEMENT_DYNCREATE(HelpDownloadDlg, Operation)

typedef BOOL (WINAPI *P_GDFSE)(LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

#define new CAM_DEBUG_NEW


/********************************************************************************************

	Preference:	IndexBaseURL
	Section:	Update
	Range:		string (URL)
	Purpose:	Sets the base URL which will be used to check for Help and Support file
				updates

********************************************************************************************/

#ifdef PROD_XS
String_256 HelpDownloadOp::IndexBaseURL = "http://xara.xaraonline.com/XaraXS/Resources/HelpAndSupport";
#else
String_256 HelpDownloadOp::IndexBaseURL = "http://xara.xaraonline.com/XaraX2/Resources/HelpAndSupport";
#endif

/********************************************************************************************

	Preference:	HighBandwidth
	Section:	Update
	Range:		0 or 1 (TRUE or FALSE)
	Purpose:	Sets the base URL which will be used to check for Help and Support file
				updates

********************************************************************************************/

BOOL HelpDownloadOp::HighBandwidth = FALSE;


/********************************************************************************************

	Preference:	DeferredCopySrc
	Section:	Update
	Range:		string (Path)
	Purpose:	Gives the full path to the deferred copy source file (if any)

********************************************************************************************/

	String_256 HelpDownloadOp::DeferredCopySrc = "";


/********************************************************************************************

	Preference:	DeferredCopyDest
	Section:	Update
	Range:		string (Path)
	Purpose:	Gives the full path to the deferred copy destination file (if any)

********************************************************************************************/

	String_256 HelpDownloadOp::DeferredCopyDest = "";


/********************************************************************************************

	Preference:	IndexLeafName
	Section:	Update
	Range:		string
	Purpose:	Gives the leafname of the index file. Useful for testing releases of new
				file sets

********************************************************************************************/

	String_32 HelpDownloadOp::IndexLeafName = "index.xml";


/********************************************************************************************

	Preference:	CurrentIndexRelease
	Section:	Update
	Range:		0..35536
	Purpose:	Gives the leafname of the index file. Useful for testing releases of new
				file sets

********************************************************************************************/

	INT32 HelpDownloadOp::CurrentIndexRelease = 0;


/********************************************************************************************

>	HelpDownloadParam::HelpDownloadParam()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2004
	Inputs:		
	Returns:	
	Purpose:	Default constructor. Do nothing
	Errors:		

********************************************************************************************/

HelpDownloadParam::HelpDownloadParam()
{
	PathName pathTemp = FileUtil::GetTemporaryPathName();
	file = pathTemp.GetPath();
	m_pIndexDoc = NULL;
}

/********************************************************************************************

>	HelpDownloadParam::~HelpDownloadParam()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2004
	Inputs:		
	Returns:	
	Purpose:	Destructor. Do nothing
	Errors:		

********************************************************************************************/

HelpDownloadParam::~HelpDownloadParam()
{
//	m_pIndexDoc is a smart ptr, doesn't need to be NULLed
}

/********************************************************************************************

>	BOOL HelpDownloadOp::CheckForNewFiles(BOOL bForeground)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2004
	Inputs:		bForeground - TRUE: check for new index synchronously then parse index
							 FALSE: parse index then check for new index asyncronously
	Returns:	FALSE always (at the moment)
	Purpose:	To parse the HelpAndSupport index file and to check for a new version
				of said file.
				The order in which those two operations are done depends on the state
				of the foreground flag.
	Errors:		

********************************************************************************************/

BOOL HelpDownloadOp::CheckForNewFiles(BOOL bForeground)
{
	if (!bForeground)
	{
		//---------------------------------------------
		// First, check for an index file from a previous download
		CheckIndexFile();
	}

	//---------------------------------------------
	// Next ask the server to start an asynchronous download of an index file

	if (bForeground)
		Progress::Start(FALSE, _R(IDS_UPDATING_HANDS), -1);

	CString strClientFolder = GetSupportFilepath();
	strClientFolder.TrimRight('\\');
	BOOL bOK = OpUpdate::UpdateFiles(bForeground, strClientFolder, (CString)IndexBaseURL, (CString)IndexLeafName, _T("UpdateHelpAndSupport"));

	if (bForeground)
		Progress::Stop();

	if (bOK && bForeground)
	{
		//---------------------------------------------
		// Finally, in foreground mode Check through the newly downloaded index file
		CurrentIndexRelease = 0;			// Reset version checking
		BOOL bDidSomething = CheckIndexFile();
		if (!bDidSomething)
		{
			// We found nothing to update so we must tell the user, not just
			// leave him dangling.
			CHelpDownloadWebDialog dlgReportNothing(_T("HelpAndSupportOK.web"));
			dlgReportNothing.DoModal();
		}
	}

	return FALSE;
}


/********************************************************************************************

>	BOOL HelpDownloadOp::CheckIndexFile()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/2004
	Inputs:		
	Returns:	TRUE if the check found something to update
				FALSE if there was nothing to do.
	Purpose:	
	Errors:		

********************************************************************************************/

BOOL HelpDownloadOp::CheckIndexFile()
{
	IXMLDOMDocumentPtr pDoc = CXMLUtils::NewDocument();
	HRESULT hr;
	BOOL bDidSomething = FALSE;

	// Ensure that the HelpAndSupport folder exists...
	::CreateDirectory(GetSupportFilepath(), NULL);

	VARIANT_BOOL varResult;

	CString strIndexFilename = GetSupportFilepath(GetIndexFileForLocale(_T("")));
	hr = pDoc->load(_variant_t(strIndexFilename), &varResult);
	if (FAILED(hr) || varResult==VARIANT_FALSE)
	{
		// Fallback to ENG like WFU does...
		strIndexFilename = GetSupportFilepath(GetIndexFileForLocale(_T("ENG")));
		hr = pDoc->load(_variant_t(strIndexFilename), &varResult);
	}

	// If we loaded an index document successfully then parse it...
	if (SUCCEEDED(hr) && varResult==VARIANT_TRUE)
	{
		// The index file still exists, so we must parse it, check for updates and then delete it
		// bSilent = FALSE because this is a user-initiated first call of ProcessDownloadIndex
		BOOL bFinishedWithIndex = ProcessDownloadIndex(pDoc, FALSE, &bDidSomething);
// Don't delete the index file because if it was non-English, that could cause the logic above
// to start scanning the English index file!
//		if (bFinishedWithIndex)
//		{
//			::DeleteFile(strIndexFilename);
//		}
	}

	return bDidSomething;
}


/********************************************************************************************

>	CString HelpDownloadOp::GetSupportFilepath(const CString& strLeafName, const CString& strSupportFolder)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/2004
	Inputs:		Leaf name (may include relative folders)
	Returns:	Full path to specified leaf file
	Purpose:	
	Errors:		

********************************************************************************************/

CString HelpDownloadOp::GetSupportFilepath(const CString& strLeafName, const CString& strSupportFolder)
{
	CString strPath;
	LPTSTR pathbuffer = strPath.GetBuffer(_MAX_PATH);

	TCHAR achzExePath[_MAX_PATH];
	if (::GetModuleFileName(AfxGetInstanceHandle(), achzExePath, _MAX_PATH) == 0)
	{
		TRACEUSER( "Phil", _T("GetModuleFileName failed in GetSupportFilepath\n"));
		return _T("");
	}

	TCHAR achzDrive[_MAX_DRIVE];
	TCHAR achzDirectory[_MAX_DIR];
	TCHAR achzFileName[_MAX_FNAME];
	TCHAR achzExtn[_MAX_EXT];
	TCHAR achzLeafPath[_MAX_DIR];
	TCHAR achzLeafFile[_MAX_FNAME];
	TCHAR achzLeafExtension[_MAX_EXT];
	_tsplitpath(achzExePath, achzDrive, achzDirectory, achzFileName, achzExtn);
	_tsplitpath(strLeafName, NULL, achzLeafPath, achzLeafFile, achzLeafExtension);

	_tcscat(achzDirectory, strSupportFolder);
	_tcscat(achzDirectory, achzLeafPath);

	_tmakepath(pathbuffer, achzDrive, achzDirectory, achzLeafFile, achzLeafExtension);

	strPath.ReleaseBuffer();

	return strPath;
}


CString HelpDownloadOp::GetIndexFileForLocale(const CString& strLocale)
{
	if (strLocale.IsEmpty())
		return CResDll::GetCurrentLocale() + _T("\\") + IndexLeafName;
	else
		return strLocale + _T("\\") + IndexLeafName;
}


CString HelpDownloadOp::GetFileForLocale(const CString& strLocale, const CString& strFile)
{
	if (strLocale.IsEmpty())
		return CResDll::GetCurrentLocale() + _T("\\") + strFile;
	else
		return strLocale + _T("\\") + strFile;
}


/********************************************************************************************

>	BOOL HelpDownloadOp::Init()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2004
	Inputs:		
	Returns:	TRUE if the Init worked ok
	Purpose:	
	Errors:		

********************************************************************************************/

BOOL HelpDownloadOp::Init()
{
	Camelot.DeclareSection("Update", 12);	// 12 because OpUpdate also puts 6 entries here...
	Camelot.DeclarePref("Update", "HighBandwidth", &HighBandwidth);
	Camelot.DeclarePref("Update", "DeferredCopySrc", &DeferredCopySrc);
	Camelot.DeclarePref("Update", "DeferredCopyDest", &DeferredCopyDest);
	Camelot.DeclarePref("Update", "HelpAndSupportIndexLeafName", &IndexLeafName);
	Camelot.DeclarePref("Update", "HelpAndSupportCurrentIndexRelease", &CurrentIndexRelease);

	HelpDownloadDlg::Init();

	return RegisterOpDescriptor(
		0,									// Tool ID
 		_R(IDS_HELPDOWNLOADOP),					// String resource ID
		CC_RUNTIME_CLASS(HelpDownloadOp),	// Runtime class
 		OPTOKEN_HELPDOWNLOADOP,				// Token string
 		HelpDownloadOp::GetState,			// GetState function
		0,									// Help ID
		0,									// Bubble ID
		0,									// Resource ID
		0									// Control ID
	);
}


/********************************************************************************************

>	OpState HelpDownloadOp::GetState(String_256*, OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2004
	Inputs:		
	Returns:	
	Purpose:	
	Errors:		

********************************************************************************************/

OpState HelpDownloadOp::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;
	return(OpSt);
}


/********************************************************************************************

>	BOOL HelpDownloadOp::OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2004
	Inputs:		pChangingDoc -
				State -
	Returns:	
	Purpose:	End (abort) the operation when for instance the current document has been destroyed
				while downloading.
	Errors:		

********************************************************************************************/

BOOL HelpDownloadOp::OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State)
{
	return TRUE;
}


/********************************************************************************************

>	void HelpDownloadOp::OnDownloadSuccess()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2004
	Inputs:		
	Returns:	
	Purpose:	Function called when the download has finished and is successful
				
	Errors:		

********************************************************************************************/

void HelpDownloadOp::OnDownloadSuccess()
{
	// get a pointer to the OpParam so that I can retrieve some useful information
	HelpDownloadParam* pGenericParam = (HelpDownloadParam*) pParam;

	String_256 GenericFile = (pGenericParam->file).GetFileName();
	if (IsUserName("Phil"))
		TRACE1("HelpDownloadOp::OnDownloadSuccess(), file = %s\n", (TCHAR*)GenericFile);

	PathName pathTemp = pGenericParam->file;
	String_256 URL = pGenericParam->strURL;
	PathName pathDestination = pGenericParam->m_pathDestination;

	// NOTE: DON'T "Touch" the downloaded file with the specified time
	// There is not an easy way to guarantee that you will be setting
	// a UTC time into the file's status (FAT doesn't store UTC).
	// Just rely on the time the file was last modified by the downloader...

	if (pGenericParam->m_strResourceType==String_16("Help"))
	{
		// ------------------------------------------
		// File is a new Help file
		// Attempt to copy into place
		// ::MoveFile(pathTemp.GetPath(), pathDestination.GetPath());
		// Can't use MoveFile in case temp path and dest path are on different drives
		BOOL bOK = ::CopyFile(pathTemp.GetPath(), pathDestination.GetPath(), FALSE);
		if (bOK)
		{
			::DeleteFile(pathTemp.GetPath());
		}
		else
		{
			// We must remember this temp file location for later processing...
			DeferFileCopy(pathTemp.GetPath(), pathDestination.GetPath());
		}
	}
	else
	{
		// ------------------------------------------
		// Generic Help and Support file
		// Just copy into final location
		// ::MoveFile(pathTemp.GetPath(), pathDestination.GetPath());
		// Can't use MoveFile in case temp path and dest path are on different drives
		BOOL bOK = ::CopyFile(pathTemp.GetPath(), pathDestination.GetPath(), FALSE);
		if (bOK)
		{
			::DeleteFile(pathTemp.GetPath());
		}
		else
		{
			// We must remember this temp file location for later processing...
			DeferFileCopy(pathTemp.GetPath(), pathDestination.GetPath());
		}
	}

	// Update the download count for this option node in the index xml
	// (In memory ONLY!)
	IXMLDOMNodePtr pOptionNode = pGenericParam->m_pOptionNode;
	if (pOptionNode)
	{
		CXMLUtils::ReplaceElement(pOptionNode, "Downloaded", TRUE);
	}

	BOOL bFinishedWithIndex = ProcessDownloadIndex(pGenericParam->m_pIndexDoc, TRUE);
}


/********************************************************************************************

>	void HelpDownloadOp::OnDownloadFail()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2004
	Inputs:		
	Returns:	
	Purpose:	Function called when the download has failed
	Errors:		

********************************************************************************************/


void HelpDownloadOp::OnDownloadFail()
{
	//First get a pointer to the parameters
	HelpDownloadParam* pGenParam = (HelpDownloadParam*) pParam;

	if (pGenParam==NULL)
	{
		ERROR2RAW("OpBitmapDownload::OnDownloadFail - no download parameters");
		return;
	}

	//And put it up as a message
	String_256 strMessage;
	PathName path(pGenParam->m_pathDestination);
	String_256 strLeafname = path.GetFileName();
	strMessage.MakeMsg(_R(IDS_HANDS_DOWNLOADFAILED), &strLeafname);
	Error::SetError(0, strMessage, 0);
	InformError();

}


/********************************************************************************************

>	void HelpDownloadOp::OnDownloadAbort()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2004
	Inputs:		
	Returns:	
	Purpose:	Function called when the user aborts download
	Errors:		

********************************************************************************************/

void HelpDownloadOp::OnDownloadAbort()
{

	// get a pointer to the OpParam so that I can retrieve some useful information
	HelpDownloadParam* pGenericParam = (HelpDownloadParam*) pParam;

	String_256 GenericFile = pGenericParam->strURL;
	if (IsUserName("Phil"))
		TRACE1("HelpDownloadOp::OnDownloadAbort(), file = %s\n", (TCHAR*)GenericFile);

	// stop the import op
//	SelOperation* Op = pGenericParam->m_Op;
//	((OpMenuImport*)Op)->EndImport();
}


/********************************************************************************************

>	BOOL HelpDownloadOp::ProcessDownloadIndex(IXMLDOMDocumentPtr pDoc, BOOL bSilent, BOOL* pbDidSomething)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2004
	Inputs:		
	Returns:	TRUE if the Index file can be deleted
	Purpose:	
	Errors:		

********************************************************************************************/

BOOL HelpDownloadOp::ProcessDownloadIndex(IXMLDOMDocumentPtr pDoc, BOOL bSilent, BOOL* pbDidSomething)
{
	// For each resource declared in the index,
	//   Select appropriate option (given current locale and bandwidth prefs)
	//   Test whether that option is out of date on the client machine
	//   If so, attempt to download it by creating an instance of this Op
	HRESULT hr;

	if (pbDidSomething)
		*pbDidSomething = FALSE;

	// Check whether we're online or not before even doing any of this!
	DWORD Flags = 0;
	if (!InternetGetConnectedState(&Flags, 0))
		// Not connected - do nothing
		return FALSE;

	// Read the update number from the XML
	IXMLDOMNodePtr pRoot = NULL;
	hr = pDoc->selectSingleNode(_bstr_t("/HelpAndSupportFiles"), &pRoot);
	if (SUCCEEDED(hr) && pRoot!=NULL)
	{
		INT32 ThisRelease = CXMLUtils::GetAttributeLong(pRoot, "ReleaseNumber");
		if (CurrentIndexRelease>=ThisRelease)
			return FALSE;
	}

	// Load up the CD index if we can find it...
	// This is not essential - only used to avoid download of files which are on the CD
//	IXMLDOMDocumentPtr pCDIndexDoc = CXMLUtils::NewDocument();
//	VARIANT_BOOL varResult;
//	CString strCDIndexLocale = CResDll::GetCurrentLocale();
//	CString strCDIndexFilename = GetSupportFilepath(GetFileForLocale(strCDIndexLocale, _T("cdindex.xml")));
//	hr = pCDIndexDoc->load(_variant_t(strCDIndexFilename), &varResult);
//	if (FAILED(hr) || varResult==VARIANT_FALSE)
//		pCDIndexDoc = NULL;

	IXMLDOMNodePtr pOptionNode = NULL;
	CString strResourceType;
	CString strLocale;
	pOptionNode = FindFirstOption(pDoc, &strResourceType, &strLocale);
	if (pOptionNode)
	{
		// We have found something that needs to be updated but we must
		// ask the user before we go ahead...
		if (pbDidSomething)
			*pbDidSomething = TRUE;

		if (!bSilent)
		{
			INT32 ret = IDOK;
			CHelpDownloadWebDialog webdlg(_T("NewHelpAndSupportFile.web"));
			webdlg.m_bFastConnection = HighBandwidth;
			ret = webdlg.DoModal();
			if (ret!=IDOK)
			{
				if (ret==_R(IDB_IGNORE))
				{
					// The user doesn't want to download help files
					// To streamline the process we will take that to mean that
					// he doesn't want to be asked again either!
					// So we delete the index file and update our record of the
					// current release number to prevent further prompts
					// until a new one is delivered from the server
					CurrentIndexRelease = CXMLUtils::GetAttributeLong(pRoot, "ReleaseNumber");	// Default to 0
					pOptionNode = NULL;
					return TRUE;				// And tell the caller he can delete the file
				}
				else
				{
					// We'll leave the release number alone, leave the index in place
					// and come back to it another time...
					return FALSE;
				}
			}
			else
			{
				BOOL bChanged = (HighBandwidth!=webdlg.m_bFastConnection);

				// Record the state of the internet speed switch
				HighBandwidth = webdlg.m_bFastConnection;

				if (bChanged)
				{
					// User changed the bandwidth selection so start scan again
					// (may result in pOptionNode being NULL! But what can you do?)
					pOptionNode = FindFirstOption(pDoc, &strResourceType, &strLocale);
				}
			}
		}

		if (pOptionNode)
		{
			// Extract useful info from the OptionNode and then test it...
			CString strURL = CXMLUtils::GetNodeString(pOptionNode, _T("si:URL"));
			CString strFilename = CXMLUtils::GetNodeString(pOptionNode, _T("si:Name"));
			COleDateTime filedate;
			BOOL bdateOK = filedate.ParseDateTime(CXMLUtils::GetNodeString(pOptionNode, _T("si:DateTime")), 0, MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_ENGLISH_UK), SORT_DEFAULT));

			UINT32 filesize = CXMLUtils::GetNodeLong(pOptionNode, _T("si:Size"));
			CString strDescription = CXMLUtils::GetNodeString(pOptionNode, _T("si:Description"));

			ERROR2IF(strResourceType.IsEmpty(), FALSE, "About to download but Resource type is unknown.");
			ERROR2IF(strLocale.IsEmpty(), FALSE, "About to download but Resource locale is unknown.");
			CString strPathname = GetFullResourcePath(strResourceType, strFilename, strLocale);

			// Invoke a background download of the specified file...
			// When this download has completed, this routine will be called again
			// to find the next item in the index for download and this time
			// bSilent will be FALSE so it will NOT prompt the user any more.
			//
			HelpDownloadParam* pParam = new HelpDownloadParam;
			if (pParam==NULL)
				return FALSE;

			// Check for enough disk space...
			// On both the temp location and the final destination
			//
			while ((!EnoughDiskSpace((CString)pParam->file.GetPath(), filesize) || !EnoughDiskSpace(strPathname, filesize*2)))
			{
				INT32 ret = InformWarning(_R(IDS_NOTENOUGHSPACETODOWNLOAD), _R(IDS_TRYAGAIN), _R(IDS_CANCEL));
				if (ret==2) // _R(IDS_CANCEL)
					return FALSE;
			}

			// Don't set pParam->file
			// Use default value (a temp path), then copy to pathDestination
			// in OnDownloadSuccess handler
			// pParam->file				= String_256(strPathname);
			pParam->bHasProgressDlg		= TRUE;
			pParam->m_pathDestination	= String_256(strPathname);
			pParam->strURL				= String_256(strURL);
			if (!strDescription.IsEmpty())
				pParam->strDescription	= String_256(strDescription);
			pParam->type				= TYPE_SUPPORT;
			pParam->m_strResourceType	= String_16(strResourceType);
			pParam->priority			= AsynchDownload::PRIORITY_NORMAL;
			pParam->m_date				= filedate;
			pParam->m_pIndexDoc			= pDoc;
			pParam->m_pOptionNode		= pOptionNode;

			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_HELPDOWNLOADOP);
			if (pOpDesc)
			{
				pOpDesc->Invoke(pParam, FALSE);
			}

			return FALSE;
		}
	}

	// Nothing useful in the index file so we can mark ourselves up to date
	// and delete the index...
	CurrentIndexRelease = CXMLUtils::GetAttributeLong(pRoot, "ReleaseNumber");	// Default to 0
	return TRUE;
}


/********************************************************************************************

>	IXMLDOMNodePtr HelpDownloadOp::FindFirstOption(IXMLDOMDocumentPtr pDoc,
												   CString* pstrResourceType,
												   CString* pstrLocale)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/04/2004
	Inputs:		
	Returns:	Full path to the specified resource file
	Purpose:	
	Errors:		

********************************************************************************************/

IXMLDOMNodePtr HelpDownloadOp::FindFirstOption(IXMLDOMDocumentPtr pDoc,
											   CString* pstrResourceType,
											   CString* pstrLocale)
{
	HRESULT hr;
	IXMLDOMNodePtr pOptionNode = NULL;
	IXMLDOMNodeListPtr pList = NULL;
	IXMLDOMNodePtr pResourceNode = NULL;
	CString strType = "";

	// Iterate through the list of Resource nodes, picking the appropriate Option...
	hr = pDoc->selectNodes(_bstr_t("//si:Resource"), &pList);
	if (SUCCEEDED(hr) && pList!=NULL)
	{
		hr = pList->nextNode(&pResourceNode);
		while (pResourceNode)
		{
			pOptionNode = NULL;
			CString strResourceType = CXMLUtils::GetAttributeString(pResourceNode, _T("type"));
			if (pstrResourceType)
				*pstrResourceType = strResourceType;

			// Now select the appropriate option file for this resource...
			CString strOptionSelector;
			CString strLocale = CResDll::GetCurrentLocale();
			if (pstrLocale)
				*pstrLocale = strLocale;
			INT32 i=2;
			while (pOptionNode==NULL && i--)
			{
				if (i==0)
				{
					// Fallback to the default Locale if we didn't find anything yet
					strLocale = _T("ENG");
					if (pstrLocale)
						*pstrLocale = strLocale;
				}

				// First look for the appropriate bandwidth option
				strOptionSelector = _T("si:Option[");
				if (HighBandwidth)
				{
					strType = _T("large");
					strOptionSelector += _T("@size=\"large\" ");
				}
				else
				{
					strType = _T("small");
					strOptionSelector += _T("@size=\"small\" ");
				}
				strOptionSelector += _T("and @locale=\"") + strLocale + "\" ";
				strOptionSelector += _T("]");
				hr = pResourceNode->selectSingleNode(_bstr_t(strOptionSelector), &pOptionNode);

				// If we can do high bandwith things but didn't find a specific high bandwidth
				// option then look for a low bandwidth option
				if ((FAILED(hr) || pOptionNode==NULL) && HighBandwidth)
				{
					strOptionSelector = _T("si:Option[");
					strOptionSelector += _T("@size=\"small\" ");
					strOptionSelector += _T("and @locale=\"") + strLocale + "\" ";
					strOptionSelector += _T("]");
					strType = _T("");
					hr = pResourceNode->selectSingleNode(_bstr_t(strOptionSelector), &pOptionNode);
				}
			}

			if (pOptionNode==NULL)
				pOptionNode = pResourceNode;

			if (pOptionNode)
			{
				// Extract useful info from the OptionNode and then test it...
				CString strURL = CXMLUtils::GetNodeString(pOptionNode, _T("si:URL"));
				CString strFilename = CXMLUtils::GetNodeString(pOptionNode, _T("si:Name"));
				COleDateTime filedate;
				BOOL bdateOK = filedate.ParseDateTime(CXMLUtils::GetNodeString(pOptionNode, _T("si:DateTime")), 0, MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_ENGLISH_UK), SORT_DEFAULT));

				UINT32 filesize = CXMLUtils::GetNodeLong(pOptionNode, _T("si:Size"));
				CString strDescription = CXMLUtils::GetNodeString(pOptionNode, _T("si:Description"));

				CString strPathname = GetFullResourcePath(strResourceType, strFilename, strLocale);

				// Make sure we've got the correct CD Index loaded...
//				if (strLocale!=*pstrCDIndexLocale)
//				{
					IXMLDOMDocumentPtr pCDIndexDoc = CXMLUtils::NewDocument();
					VARIANT_BOOL varResult;
//					&pstrCDIndexLocale = strLocale;
					CString strCDIndexFilename = GetSupportFilepath(GetFileForLocale(strLocale, _T("cdindex.xml")));
					hr = pCDIndexDoc->load(_variant_t(strCDIndexFilename), &varResult);
					if (FAILED(hr) || varResult==VARIANT_FALSE)
						pCDIndexDoc = NULL;
//				}

				// Now check to see if we're allowed to download this file
				if (!strFilename.IsEmpty()										// If we have a filename
					&& strPathname!=DeferredCopyDest							// And it's not already a deferred copy
					&& !MatchFileAttributes(strPathname, filedate, filesize)	// And it differs from the online file
					&& !FileOnCD(pCDIndexDoc, strFilename, filedate, filesize, strLocale, strType)	// And it's not on the CD
					&& LegalFileType(strFilename)								// And file type is acceptable
					&& LegalPathname(strPathname)								// And pathname doesn't escape our folder
					)
				{
					// If this file has already been downloaded in this session
					if (HasBeenDownloaded(pOptionNode))
					{
						// Tell the user that something's gone wrong
						String_256 strMessage;
						String_256 strName = CXMLUtils::GetNodeString(pOptionNode, _T("si:Name"));
						strMessage.MakeMsg(_R(IDS_WARN_REPEATDOWNLOAD), &strName);
						Error::SetError(0, strMessage, 0);
						INT32 ret = InformWarning(0, _R(IDS_CONTINUEUPDATE), _R(IDS_STOPUPDATE));
						if (ret==2) // _R(IDS_STOP)
							return NULL;
		
						// and continue looking for another option to download...
					}
					else
						// Else return it for downloading
						return pOptionNode;
				}
			}
			hr = pList->nextNode(&pResourceNode);
		}
	}

	return NULL;
}


/********************************************************************************************

>	CString HelpDownloadOp::GetFullResourcePath(const CString& strResourceType,
												const CString& strFilename,
												const CString& strLocale = _T(""))

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/04/2004
	Inputs:		
	Returns:	Full path to the specified resource file
	Purpose:	
	Errors:		

********************************************************************************************/

CString HelpDownloadOp::GetFullResourcePath(const CString& strResourceType,
											const CString& strFilename,
											const CString& strLocale)
{
	CString strPath;

	if (strResourceType.CompareNoCase(_T("Movie"))==0)
	{
		// It's a movie
		strPath = GetSupportFilepath(GetFileForLocale(strLocale, strFilename));
	}
	else if (strResourceType.CompareNoCase(_T("Help"))==0)
	{
		// It's a help file
		strPath = GetSupportFilepath(GetFileForLocale(strLocale, strFilename));
	}
	else
		ERROR3("Unknown Support Resource Type");

	return strPath;
}


/********************************************************************************************

>	BOOL HelpDownloadOp::MatchFileAttributes(const CString& strFilePath, COleDateTime datetime, UINT32 size)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/04/2004
	Inputs:		
	Returns:	TRUE if the file exists and has the specified date or younger and size
	Purpose:	
	Errors:		

********************************************************************************************/

BOOL HelpDownloadOp::MatchFileAttributes(const CString& strFilePath, const COleDateTime& datetime, UINT32 size)
{
	try
	{
		HANDLE hFile;
		WIN32_FIND_DATA FindFileInfo;
		hFile = ::FindFirstFile(strFilePath, &FindFileInfo);
		unsigned __int64 i64filesize = 0;
		if (hFile!=INVALID_HANDLE_VALUE)
		{
			i64filesize = ((__int64)FindFileInfo.nFileSizeHigh*((__int64)MAXDWORD+1))+FindFileInfo.nFileSizeLow;
			::FindClose(hFile);
		}
		// NOTE! For FAT volumes, FindFirstFile automatically converts the local stored times
		// into UTC times!!!
		// So whether FAT or NTFS, ftLastWriteTime is in UTC.
		COleDateTime filetime(FindFileInfo.ftLastWriteTime);

		if (datetime.GetStatus()!=COleDateTime::valid || filetime.GetStatus()!=COleDateTime::valid)
		{
			TRACE( _T("MatchFileAttributes one or other COleDateTime is invalid!\n"));
			return i64filesize==size;
		}

TRACEUSER( "Phil", _T("MFA: File = %s\n"), strFilePath);
TRACEUSER( "Phil", _T("MFA: UK xml time  = %s\n"), datetime.Format(LOCALE_NOUSEROVERRIDE, MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_ENGLISH_UK), SORT_DEFAULT)));
TRACEUSER( "Phil", _T("MFA: UK file time = %s\n"), filetime.Format(LOCALE_NOUSEROVERRIDE, MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_ENGLISH_UK), SORT_DEFAULT)));
TRACEUSER( "Phil", _T("MFA: xml size  = %d\n"), size);
TRACEUSER( "Phil", _T("MFA: file size = %d\n"), i64filesize);
		return (filetime>=datetime && i64filesize==size);
	}
	catch (...)
	{
		return FALSE;
	}
}


/********************************************************************************************

>	BOOL HelpDownloadOp::FileOnCD(IXMLDOMDocumentPtr pDoc, const CString& strFilePath, COleDateTime datetime, UINT32 size, const CString& strType)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/04/2004
	Inputs:		
	Returns:	TRUE if the file exists on the CD (or a larger version exists on CD)
	Purpose:	
	Errors:		

********************************************************************************************/

BOOL HelpDownloadOp::FileOnCD(IXMLDOMDocumentPtr pDoc,
							  const CString& strName,
							  const COleDateTime& datetime,
							  UINT32 size,
							  const CString& strLocale,
							  const CString& strType)
{
	if (pDoc==NULL)
		return FALSE;

	// Now select the appropriate option file for this resource...
	IXMLDOMNodePtr pNode = NULL;
	IXMLDOMNodeListPtr pOptionsList = NULL;
	CString strOptionSelector;

	// First look for the appropriate bandwidth option
	strOptionSelector = _T("//si:Resource/si:Option[");
	if (strType==_T("large"))
		strOptionSelector += _T("(@size=\"large\" or @size=\"cd\") ");
	else
		strOptionSelector += _T("(@size=\"small\" or @size=\"large\" or @size=\"cd\") ");
//	if (HighBandwidth)
//		strOptionSelector += _T("@size=\"large\" ");
//	else
//		strOptionSelector += _T("@size=\"small\" ");
	strOptionSelector += _T("and @locale=\"") + strLocale + "\" ";
	strOptionSelector += _T("and si:Name=\"") + strName + "\" ";
	strOptionSelector += _T("]");
	HRESULT hr = pDoc->selectNodes(_bstr_t(strOptionSelector), &pOptionsList);
	if (SUCCEEDED(hr) && pOptionsList!=NULL)
	{
		hr = pOptionsList->nextNode(&pNode);
		while (pNode)
		{
			CString strCDName = CXMLUtils::GetNodeString(pNode, _T("si:Name"));
			COleDateTime dtCDdate;
			BOOL bdateOK = dtCDdate.ParseDateTime(CXMLUtils::GetNodeString(pNode, _T("si:DateTime")), 0, MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_ENGLISH_UK), SORT_DEFAULT));
			UINT32 ulCDSize = CXMLUtils::GetNodeLong(pNode, _T("si:Size"));

			// We know that locale and size match already
			// Now test the Name, Date and Size
			if (dtCDdate.GetStatus()!=COleDateTime::valid || datetime.GetStatus()!=COleDateTime::valid)
			{
				if (strCDName==strName
//					&& ulCDSize==size		// Don't check size - if type is same or larger that's good enough
					)
				{
					// The specified file is the same as one on the CD...
					return TRUE;
				}
			}
			else
			{
				if (strCDName==strName
					&& dtCDdate==datetime
//					&& ulCDSize==size		// Don't check size - if type is same or larger that's good enough
					)
				{
					// The specified file is the same as one on the CD...
					return TRUE;
				}
			}

			hr = pOptionsList->nextNode(&pNode);
		}
	}


	return FALSE;
}


/********************************************************************************************

>	BOOL HelpDownloadOp::LegalFileType(const CString& strFilename)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/04/2004
	Inputs:		
	Returns:	TRUE if the filename has an acceptable file extension
	Purpose:	Applies some security checking
	Errors:		

********************************************************************************************/

BOOL HelpDownloadOp::LegalFileType(const CString& strFilename)
{
	TCHAR achzExtn[_MAX_EXT];

	_tsplitpath(strFilename, NULL, NULL, NULL, achzExtn);

	return (_tcsstr(_T(".htm .txt .xml .chm .mpg .avi .jpg .png .gif .web .xar .wix .wmv"), achzExtn)!=NULL);
}


/********************************************************************************************

>	BOOL HelpDownloadOp::LegalPathname(const CString& strPathname)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/04/2004
	Inputs:		
	Returns:	TRUE if the filename has an acceptable file extension
	Purpose:	Applies some security checking
	Errors:		

********************************************************************************************/

BOOL HelpDownloadOp::LegalPathname(const CString& strPathname)
{
	return (_tcsstr(strPathname, _T(".."))==NULL);
}


/********************************************************************************************

>	BOOL HelpDownloadOp::HasBeenDownloaded(IXMLDOMNodePtr pNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/04/2004
	Inputs:		
	Returns:	TRUE if the node contains a Downloaded node with a true value
	Purpose:	Check whether the specified option node has been downloaded before
	Errors:		

********************************************************************************************/

BOOL HelpDownloadOp::HasBeenDownloaded(IXMLDOMNodePtr pNode)
{
	return (pNode!=NULL && CXMLUtils::GetNodeBOOL(pNode, "Downloaded"));
}


/********************************************************************************************
>	void HelpDownloadOp::DeferFileCopy(const String_256& pTempFile, const String_256& pDestFile)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/2004
	Returns:	-
	Purpose:	Records the specified file copy operation as being deferred until we next
				start up...
	SeeAlso:	DoDeferredFileCopy
********************************************************************************************/

void HelpDownloadOp::DeferFileCopy(const String_256& pTempFile, const String_256& pDestFile)
{
	// NOTE: Can't use prefs because this function is called before the prefs system
	// has been initialised...
//	GetRegString(hKey, TEXT("DeferredCopySrc"), &DeferredCopySrc);
//	GetRegString(hKey, TEXT("DeferredCopyDest"), &DeferredCopyDest);

	if (DeferredCopySrc.IsEmpty())
	{
		DeferredCopySrc = pTempFile;
		DeferredCopyDest = pDestFile;
	}

//	SetRegString(hKey, TEXT("DeferredCopySrc"), &DeferredCopySrc);
//	SetRegString(hKey, TEXT("DeferredCopyDest"), &DeferredCopyDest);
}



/********************************************************************************************
>	void HelpDownloadOp::DoDeferredFileCopy()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/2004
	Returns:	-
	Purpose:	Check to see if there is a deferred file copy pending and if so, do it
	SeeAlso:	UserHelpDeferFileCopy
********************************************************************************************/

void HelpDownloadOp::DoDeferredFileCopy()
{
	// NOTE: Can't use prefs because this function is called before the prefs system
	// has been initialised...
	HKEY hKey = OpenRegKey(hAppStateRegKey, TEXT("Options\\Update"));
	if (hKey)
	{
		GetRegString(hKey, TEXT("DeferredCopySrc"), &DeferredCopySrc);
		GetRegString(hKey, TEXT("DeferredCopyDest"), &DeferredCopyDest);

		if (!DeferredCopySrc.IsEmpty() && !DeferredCopyDest.IsEmpty())
		{
			BOOL bOK = ::CopyFile((LPCTSTR)DeferredCopySrc, (LPCTSTR)DeferredCopyDest, FALSE);
		}

		if (!DeferredCopySrc.IsEmpty())
			::DeleteFile((LPCTSTR)DeferredCopySrc);

		DeferredCopySrc = "";
		DeferredCopyDest = "";

		SetRegString(hKey, TEXT("DeferredCopySrc"), &DeferredCopySrc);
		SetRegString(hKey, TEXT("DeferredCopyDest"), &DeferredCopyDest);

		CloseRegKey(hKey);
	}
}



/********************************************************************************************

>	BOOL HelpDownloadOp::EnoughDiskSpace(const CString& strPath, const UINT32 sizeRequired)

	Author:         Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:        20/04/2004
	Returns:        TRUE is enough space, FALSE otherwise
	Purpose:        Check for enough space for the specified file

********************************************************************************************/

BOOL HelpDownloadOp::EnoughDiskSpace(const CString& strPath, const UINT32 sizeRequired)
{
	TCHAR szDrive[_MAX_DRIVE + 1];
	_tsplitpath(strPath, szDrive, NULL, NULL, NULL);
	lstrcat(szDrive, _T("\\"));

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
		if (fResult)
			return ((__int64)sizeRequired < i64FreeBytesToCaller);
	}
	else
	{
		fResult = GetDiskFreeSpace (szDrive, &dwSectorsPerCluster, &dwBytesPerSector, &dwNumberOfFreeClusters, &dwTotalNumberOfClusters);
		if (fResult)
		{
			// Force 64bit maths so that drives over 4Gb work properly !!! 
			i64FreeBytes = (__int64)dwNumberOfFreeClusters * dwSectorsPerCluster * dwBytesPerSector;

			// Test to see if our requested cache size is larger than the current available space !!!
			return ((__int64)sizeRequired < i64FreeBytes);
		}
	}

	// Just incase we failed to call either of the above functions...
	if (!fResult) { ERROR3("Can't GetDiskFreeSpace()!"); }
	return FALSE;
}



/********************************************************************************************

>	BOOL HelpDownloadDlg::Init()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2004
	Inputs:		
	Returns:	TRUE if the Init worked ok
	Purpose:	
	Errors:		

********************************************************************************************/

BOOL HelpDownloadDlg::Init()
{
	return RegisterOpDescriptor(
		0,									// Tool ID
 		_R(IDS_HELPDOWNLOADDLG),				// String resource ID
		CC_RUNTIME_CLASS(HelpDownloadDlg),	// Runtime class
 		OPTOKEN_HELPDOWNLOADDLG,			// Token string
 		HelpDownloadDlg::GetState,			// GetState function
		0,									// Help ID
		0,									// Bubble ID
		0,									// Resource ID
		0									// Control ID
	);

}


/********************************************************************************************

>	OpState HelpDownloadDlg::GetState(String_256*, OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2004
	Inputs:		
	Returns:	
	Purpose:	
	Errors:		

********************************************************************************************/

OpState HelpDownloadDlg::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;
	return(OpSt);
}


/********************************************************************************************

>	void HelpDownloadDlg::Do(OpDescriptor*)

	Author:         Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:        16/04/2004
	Returns:        
	Purpose:        The default operation is called from the menu

********************************************************************************************/

void HelpDownloadDlg::Do(OpDescriptor* od)
{
	HelpDownloadOp::CheckForNewFiles(TRUE);		// Foreground!
}



