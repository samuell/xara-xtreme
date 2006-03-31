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


	registry.cpp

	Interface to the system registry, which provides a container for storing information about
	the app under Windows NT and Windows 95.
*/

#include "camtypes.h"
#include <comcat.h>
#include "app.h"
#include "registry.h"
#include "camelot.h"
#include "errors.h"
#include "product.h"
#include "cversion.h"
#include "brandinf.h"
#include "appstate.h"
//#include "rik.h"

#include "sgrmfile.h"	// LibSettingsListItem

// Code version.
DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_MEMDUMP(SaveRegistryEntries, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(LoadRegistryEntries, CC_CLASS_MEMDUMP)


// We need to use the correct filename parsing functions, depending on whether we are
// a C/ASCII/ANSI or a Unicode build.
#undef SPLITPATH
#undef MAKEPATH

#ifdef UNICODE
	// Wide-character versions.
	#define SPLITPATH			(_wsplitpath)
	#define MAKEPATH			(_wmakepath)
#else
	// C/ASCII/ANSI versions.
	#define SPLITPATH			(_tsplitpath)
	#define MAKEPATH			(_tmakepath)
#endif

// This function, defined in HELPUSER.CPP, makes a short MS-DOS filename from an INT32 filename.
extern BOOL MakeShortPath(LPTSTR lpszPath, size_t cchMaxLen);


// WEBSTER - markn - 7/2/97
// Always use PRODUCT_MANUFACTURER for the company reg name
//#ifndef STANDALONE
//	#define PRODUCT_COMPANY_REG_NAME	"Corel"
//#else
//	#define PRODUCT_COMPANY_REG_NAME	PRODUCT_MANUFACTURER		// ie. Xara Ltd
//#endif


// The following has been moved into product.h so that other bits can find out the program
// name used by the registry
/******************
// This defines a different program (product) name for debug and retail builds.
#ifdef _DEBUG
	#define PROGRAM_NAME		PRODUCT_PRODUCTNAME " (Debug)"
	#define PRODUCT_APP_DOC_ID	PRODUCT_PRODUCTNAME ".Document.Debug"
#else
	#define PROGRAM_NAME		PRODUCT_PRODUCTNAME
	#define PRODUCT_APP_DOC_ID	PRODUCT_PRODUCTNAME ".Document"
#endif
******************/



Registry g_TheLocalRegistry;		// this isn't really, but makes it look nice

/********************************************************************************************

>	Registry* GetLocalRegistry()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Returns:	A pointer to the local registry

	Purpose:	Provides a proper interface to Registries and stuff

********************************************************************************************/
Registry* GetLocalRegistry()
{
	return &g_TheLocalRegistry;
}




// This defines the file extension the program uses for its document files.  Horrible, isn't it?
//#define PROGRAM_DOC_EXTENSION	".xar"


// This is our special registry application identifier string.  It's different for debug builds,
// so that release versions of CorelXARA don't get screwed by developer's debug builds.
TCHAR szCamelotAppRegistryID[] = TEXT(PRODUCT_APP_DOC_ID);

// This holds the full path to our executable.
TCHAR szCamelotExePath[_MAX_PATH];

// This key holds the place in the registry to load and save the app's state.
HKEY hAppStateRegKey = NULL;

/********************************************************************************************

>	String_256 GetProgramNameRegistryKey()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/97
	Returns:	The name that we use as the registry key when saving and loading preferences.
	Purpose:	Returns the name that should be used as the key when accessing the registry.
				It should be mapped into HKEY_CURRENT_USER\Software as the base by the OS.
				Used in CCamApp::InitPreferencesFilename() in the SetRegistryKey so that we
				load and save data out to the registry instead of to ini files.

********************************************************************************************/

String_256 GetProgramNameRegistryKey()
{
	return TEXT( PRODUCT_MANUFACTURER "\\" PROGRAM_NAME );
}

/********************************************************************************************

>	BOOL CheckForRegistryEntry(TCHAR * SubKeyName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/97
	Returns:	True if entry found otherwise False.
	Purpose:	

********************************************************************************************/

BOOL CheckForRegistryEntry(TCHAR* SubKeyName)
{
	ERROR2IF(hAppStateRegKey == NULL, FALSE, "CheckForRegistryEntry: bad main app key");
	HKEY htest = OpenRegKey(hAppStateRegKey, SubKeyName);

	// If we opened the key succesfully then it was present
	if (htest != 0)
	{
		CloseRegKey(htest);
		return TRUE;
	}

	// we didn't open it successfully so it wasn't present
	return FALSE;
}



/********************************************************************************************

>	BOOL ConvertCLSIDToString(const CLSID& ID, StringBase* const pString)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/07/97

	Purpose:	Converts a CLSID to a StringBase (39 characters if you've got it)

	Inputs:		ID:	A CLSID to convert
	Returns:	TRUE it worked, FALSE if it didn't

********************************************************************************************/
BOOL ConvertCLSIDToString(const CLSID& ID, StringBase* const pString)
{
	BOOL fOk = TRUE;		// return this
	LPOLESTR pWString = 0;
	fOk = (StringFromCLSID(ID, &pWString) == S_OK);

#ifndef _UNICODE
	if (fOk)
	{
		UINT32 Length = wcslen(pWString);
		ERROR2IF(Length != 38, FALSE, "Insufficient string size");

		// well it works ok for English
		TCHAR AString[40] = { TEXT('\0') };
		for (UINT32 i = 0; i <= Length; i++)
			AString[i] = TCHAR(pWString[i]);

		*pString = AString;
	}
#else
	if (fOk) *pString = pWString;
#endif

	CoTaskMemFree(pWString);
	return fOk;
}


/********************************************************************************************
>	static BOOL UpdateInstanceVersion(HKEY hkProductKey,
							  		  LPCTSTR pszThisVersion,
							  		  DWORD dwThisBuildMajor, DWORD dwThisBuildMinor)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Inputs:		hkProductKey		the Product registry subkey to check/set version info
				pszThisVersion		text form of this instance's version number, eg. "1.0"
				dwThisBuildMajor	major build no of this instance
				dwThisBuildMinor	minor build no of this instance
	Returns:	TRUE if this running version is the newest registered under the given subkey
	Purpose:	Private function that checks if this running instance is the newest
				registered under the given product subkey, either HKEY_LOCAL_MACHINE or
				HKEY_CURRENT_USER.  If there is no newer version/build registered already
				then registers this instance's version info.
	SeeAlso:	InitRegistry; winoil\cversion.h
********************************************************************************************/

static BOOL UpdateInstanceVersion(HKEY hkProductKey, 
						  		  LPCTSTR pszThisVersion,
						  		  DWORD dwThisBuildMajor, DWORD dwThisBuildMinor)
{
	// Work out if this instance is the newest instance registered under the given product key.
	// NB. GetRegDword returns 0 if there is no value.
	DWORD dwNewestBuildMajor = GetRegDword(hkProductKey, TEXT("NewestBuildMajor"));
	DWORD dwNewestBuildMinor = GetRegDword(hkProductKey, TEXT("NewestBuildMinor"));
	String_256 strNewestVers(TEXT(""));
	BOOL fIsNewest = !GetRegString(hkProductKey, TEXT("NewestVersion"), &strNewestVers)
				  || (strNewestVers < pszThisVersion)
				  || (dwNewestBuildMajor < dwThisBuildMajor)
				  || (dwNewestBuildMinor < dwThisBuildMinor);

	// If it is the newest then write its version info to the given product key.
	if (fIsNewest)
	{
		// Write this stuff.
		SetRegValue(hkProductKey, TEXT("NewestVersion"),
					REG_SZ, pszThisVersion, ::lstrlen(pszThisVersion) + 1);
		SetRegValue(hkProductKey, TEXT("NewestBuildMajor"),
					REG_DWORD, &dwThisBuildMajor, sizeof(DWORD));
		SetRegValue(hkProductKey, TEXT("NewestBuildMinor"),
					REG_DWORD, &dwThisBuildMinor, sizeof(DWORD));
	}

	// Return TRUE if this instance is the newest.
	return fIsNewest;
}



/********************************************************************************************
>	BOOL InitRegistry()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95
	Returns:	TRUE if successful.
	Purpose:	In the debug version makes sure the registry contains all the items the
				installed normally puts in there.  Also gets the handle to the key where
				app state information is stored.  Under Win32s does nothing.
	SeeAlso:	CreateRegKey; OpenRegKey; SetRegValue
********************************************************************************************/

BOOL InitRegistry()
{
#ifndef EXCLUDE_FROM_RALPH
	// We need to set some command lines to run the program, both here and in appstate.cpp,
	// so get the path to the Camelot EXE.  NB. don't use that crap Marioism, the PathName
	// class, as it uses String_256s for full paths, but unfortunately _MAX_PATH is bigger
	// then that!
	if (::GetModuleFileName(AfxGetInstanceHandle(), szCamelotExePath, _MAX_PATH) == 0)
	{
		TRACEUSER( "JustinF", _T("GetModuleFileName failed in InitRegistry\n"));
		return FALSE;
	}

	// Make the path into its "short form", ie. 8.3 MS-DOS compatible.  This is necessary
	// as Windows NT doesn't like paths with spaces, even though it allows them.
#ifdef _DEBUG
	ERROR3IF(!MakeShortPath(szCamelotExePath, _MAX_PATH), "MakeShortPath failed in InitRegistry");
#else
	MakeShortPath(szCamelotExePath, _MAX_PATH);
#endif

	// Under Windows 3.1 / Win32s we don't bother with anything else as there isn't a registry.
	if (IsWin32s() && !IsWin32c()) return TRUE;

	// We always create the following registry entries every time the program is started.
	// First, set up HKEY_LOCAL_MACHINE and HKEY_CURRENT_USER with the app's version info keys.
	TCHAR szProductSubkey[] = TEXT("Software\\" PRODUCT_MANUFACTURER "\\" PROGRAM_NAME);
//	HKEY hkProductLM = CreateRegKey(HKEY_LOCAL_MACHINE, szProductSubkey);
//	ERROR2IF(hkProductLM == NULL, FALSE, "Can't create LOCAL_MACHINE/Product Subkey");

	HKEY hkProductCU = CreateRegKey(HKEY_CURRENT_USER,  szProductSubkey);
	ERROR2IF(hkProductCU == NULL, FALSE, "Can't create CURRENT_USER/Product Subkey");

	// Make some version info from the compilation definitions.
	TCHAR szThisVers[] = TEXT("Version " PRODUCT_VERSION_NO);
	DWORD dwThisBuildMajor = CAMELOT_VERSION_MAJOR, dwThisBuildMinor = CAMELOT_VERSION_MINOR;

	// Check and if necessary update the version information in the CURRENT_USER and
	// LOCAL_MACHINE keys.  We do this separately as the current user may not have access
	// to the newest version on the local machine.
//	BOOL fIsNewestLM = UpdateInstanceVersion(hkProductLM,
//											 szThisVers,
//											 dwThisBuildMajor, dwThisBuildMinor);

	BOOL fIsNewestCU = UpdateInstanceVersion(hkProductCU,
											 szThisVers,
											 dwThisBuildMajor, dwThisBuildMinor);
#ifdef _DEBUG
	if (/* fIsNewestLM || */ fIsNewestCU)
		TRACE( _T("Registering %s (%s)\n"), (LPCTSTR) szProductSubkey, (LPCTSTR) szThisVers);
//	if (fIsNewestLM)
//		TRACE( _T("- This is the newest version of Xara X registered on the local machine\n"));
	if (fIsNewestCU)
		TRACE( _T("- This is the newest version of Xara X registered to the current user\n"));
#endif

	// Create a version subkey under the product subkeys.  We'll store the app's state under the
	// version key in Current Users.
// ## Alex - removed "Version 3.0" from Registry!!
//	hAppStateRegKey = CreateRegKey(hkProductCU, szThisVers);
	hAppStateRegKey = CreateRegKey(HKEY_CURRENT_USER, szProductSubkey);	// same as hkProductCU
	ERROR2IF(hAppStateRegKey == NULL, FALSE, "Can't create CURRENT_USER/AppState Subkey");



	// OK, if we aren't the newest version on the local machine we don't do anything more, as
	// the rest is registering shell commands etc in LOCAL_MACHINE that should always work for
	// the newest version globally.
//	if (!fIsNewestLM)
//	{
//		// We will leave the global hkAppStateRegKey open as we will close it in
//		// DeInitRegistry, on shutdown.
//		CloseRegKey(hkProductLM);
//		CloseRegKey(hkProductCU);
//		return TRUE;
//	}

	// Create a version key under local machine as well.
//	HKEY hkVersion = CreateRegKey(hkProductLM, szThisVers);
//	ERROR2IF(hkVersion == NULL, FALSE, "Can't create LOCAL_MACHINE/AppState Subkey");
//	CloseRegKey(hkVersion);

	// Next, register the filename extension.
//	HKEY hkExtension = CreateRegKey(HKEY_CLASSES_ROOT, TEXT(PROGRAM_DOC_EXTENSION));
//	ERROR2IF(hkExtension == NULL, FALSE, "Can't create " PROGRAM_DOC_EXTENSION " Subkey");

	
/*
	THIS IS DONE BY MFC IN CCamApp::InitInstance FOR NOW . . .

	SetRegValue(hkExtension, NULL, REG_SZ, szCamelotAppRegistryID, sizeof(szCamelotAppRegistryID));

	// Next, register the application's identifier key.  NB. when we get OLE working we should
	// also register a short name for our docs.
	HKEY hkAppID = CreateRegKey(HKEY_CLASSES_ROOT, szCamelotAppRegistryID);
	ERROR2IF(hkAppID == NULL, FALSE, "Can't create CLASSES_ROOT/AppID Subkey");

	// Set the ID key's sole value to the human-readable form of the document name.
#ifdef _DEBUG
	TCHAR szAppTypeHumanName[] = TEXT(PRODUCT_PRODUCTNAME  " Document (Debug)");
#else
	TCHAR szAppTypeHumanName[] = TEXT(PRODUCT_PRODUCTNAME  " Document");
#endif
	SetRegValue(hkAppID, NULL, REG_SZ, szAppTypeHumanName, sizeof(szAppTypeHumanName));

	// Work out the command lines for opening and printing a doc.  These are the app's full path
	// plus some parameters tagged on the end.
	TCHAR szOpenCmd[_MAX_PATH + 32];
	::lstrcpy(szOpenCmd,  szCamelotExePath);
	::lstrcat(szOpenCmd,  TEXT(" %1"));

	// Register some shell commands ...
	HKEY hkShellCmd = CreateRegKey(hkAppID, TEXT("shell"));
	ERROR2IF(hkShellCmd == NULL, FALSE, "Can't create AppID/shell Subkey");
	CloseRegKey(hkAppID);

	// ... such as the command-line used to open a doc.
	HKEY hkOpenVerb = CreateRegKey(hkShellCmd, TEXT("open"));
	ERROR2IF(hkOpenVerb == NULL, FALSE, "Can't create shell/open Subkey");
	HKEY hkOpenCmd  = CreateRegKey(hkOpenVerb, TEXT("command"));
	ERROR2IF(hkOpenCmd == NULL, FALSE, "Can't create open/command Subkey");
	SetRegValue(hkOpenCmd, NULL, REG_SZ, szOpenCmd, ::lstrlen(szOpenCmd) + 1);
	CloseRegKey(hkOpenVerb);
	CloseRegKey(hkOpenCmd);
*/

#ifndef STANDALONE
/*
	THIS IS DONE BY MFC IN CCamApp::InitInstance FOR NOW . . .

	// Build the command-line to print a doc (don't do print registration if this is the viewer).
	TCHAR szPrintCmd[_MAX_PATH + 32];
	::lstrcpy(szPrintCmd, szCamelotExePath);
	::lstrcat(szPrintCmd, TEXT(" /P %1"));

	// Register the command-line.
	HKEY hkPrintVerb = CreateRegKey(hkShellCmd, TEXT("print"));
	ERROR2IF(hkPrintVerb == NULL, FALSE, "Can't create shell/print Subkey");
	HKEY hkPrintCmd  = CreateRegKey(hkPrintVerb, TEXT("command"));
	ERROR2IF(hkPrintCmd == NULL, FALSE, "Can't create print/command Subkey");
	SetRegValue(hkPrintCmd, NULL, REG_SZ, szPrintCmd, ::lstrlen(szPrintCmd) + 1);
	CloseRegKey(hkShellCmd);
	CloseRegKey(hkPrintVerb);
	CloseRegKey(hkPrintCmd);
*/
	// Get the full path to the default document template.  We know it should be in the same
	// directory as the Camelot executable, so extract the path components of the EXE path.
	TCHAR szDrive[_MAX_DRIVE], szDirectory[_MAX_DIR], szFileName[_MAX_FNAME], szExtn[_MAX_EXT];
	SPLITPATH(szCamelotExePath, szDrive, szDirectory, szFileName, szExtn);

	/*
		Doesn't look like the next 4 lines are necessary? - seeing as the code which uses
		szDefDocPath is already	commented out.
		Not to mention the fact that they don't work as expected - ie _R(IDS_DEFAULTDOCNAME)
		resource can't be loaded from main HINSTANCE yet? Just end up with bogus string
		"XXXXXXX" defaulted to in SmartLoadString(UINT32 modID, UINT32 resID, LPTCHAR buf, INT32 size)
		after call to ::LoadString( hInst, resID, buf, size ) fails.
		Is _R(IDS_DEFAULTDOCNAME) in a resource DLL perhaps rather than main exe?
		(Ilan 23/11/99)
	// Now put them back together, substituting the default doc's filename.
	TCHAR szDefDocPath[_MAX_PATH]; 	
	String_256 strDefDocName(_R(IDS_DEFAULTDOCNAME));
	MAKEPATH(szDefDocPath, szDrive, szDirectory, strDefDocName, TEXT(""));
	*/
//	TRACEUSER( "JustinF", _T("Default doc path is %s\n"), (LPCTSTR) szDefDocPath);

	// Next, register how the shell should create a new doc for us, by copying the default
	// document.  (The viewer has no default doc and can't create new docs).
//	HKEY hkShellNew = CreateRegKey(hkExtension, TEXT("ShellNew"));
//	ERROR2IF(hkShellNew == NULL, FALSE, "Can't create ShellNew Subkey");
//	SetRegValue(hkShellNew, TEXT("FileName"), REG_SZ, szDefDocPath, ::lstrlen(szDefDocPath) + 1);
//	CloseRegKey(hkShellNew);
#endif

	// Success!
	return	/* CloseRegKey(hkProductLM) && */ 
			CloseRegKey(hkProductCU) 
			/* && CloseRegKey(hkExtension) */ ;
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	BOOL DeInitRegistry()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/96
	Returns:	TRUE if successful.
	Purpose:	Shuts down the Camelot registry system, releasing any keys it holds.
	SeeAlso:	InitRegistry
********************************************************************************************/

BOOL DeInitRegistry()
{
	if (hAppStateRegKey != NULL)
	{
		BOOL closed = CloseRegKey(hAppStateRegKey);
		hAppStateRegKey = NULL;
		return closed;
	}

	return TRUE;
}



/********************************************************************************************
>	HKEY CreateRegKey(HKEY hKey, LPCTSTR lpcszSubKey, LPTSTR lpszClass = NULL)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95
	Inputs:		hKey			the key to create a subkey under
				lpcszSubkey		the subkey(s) to create
				lpszClass		no idea what this is for, don't pass it
	Returns:	The handle of the newly created key.
	Purpose:	Creates a new subkey(s) under the given key.  Returns NULL if something
				goes wrong.
	SeeAlso:	OpenRegKey, SetRegValue
********************************************************************************************/

HKEY CreateRegKey(HKEY hKey, LPCTSTR lpcszSubKey, LPTSTR lpszClass)
{
	ERROR2IF(hKey == NULL,FALSE,"SetRegValue bad registry key!");
	ERROR2IF(lpcszSubKey == NULL,FALSE,"SetRegValue bad registry key name!");

	// Try to create/open the given key.
	HKEY  hkReturn;
	DWORD dwDisposition;

	// Bodge fix to some bad code: We need to give an acces mask when creating the key
	// but the function's interface does not give us any indication of what access is needed
	// so just try an access mask that may well work.  (Before my change we were trying to open
	// all regestry keys with KEY_ALL_ACCESS which appears to upset NT 4.0 quite a lot).
	REGSAM access = KEY_CREATE_SUB_KEY | KEY_ENUMERATE_SUB_KEYS | KEY_EXECUTE | KEY_QUERY_VALUE |
					KEY_SET_VALUE | KEY_READ | KEY_WRITE;

	INT32 nResult = ::RegCreateKeyEx(hKey, lpcszSubKey, 0,
									(lpszClass != NULL) ? lpszClass : TEXT(""),
								    REG_OPTION_NON_VOLATILE, access, NULL,
								    &hkReturn, &dwDisposition);
/*
#ifdef _DEBUG
	if (nResult != ERROR_SUCCESS)
	{
		TCHAR szBuf[256];
		::wsprintf(szBuf, TEXT("Failed - error %ld"), nResult);
		::MessageBox(NULL, szBuf, TEXT("CreateRegKey"), MB_OK);
	}
#endif
*/
	// Did it work?
	return (nResult == ERROR_SUCCESS) ? hkReturn : NULL;
}



/********************************************************************************************
>	HKEY OpenRegKey(HKEY hKey, LPCTSTR lpcszSubKey)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95
	Inputs:		hKey			the registry to open
				lpcszSubKey		the sub-keys under hKey to open
	Returns:	The handle of the opened sub-key, or NULL if there isn't one.
	Purpose:	Opens a registry sub-key without creating it if it doesn't exist.
	SeeAlso:	CreateRegKey; SetRegValue
********************************************************************************************/

HKEY OpenRegKey(HKEY hKey, LPCTSTR lpcszSubKey)
{
	ERROR2IF(hKey == NULL,FALSE,"OpenRegKey bad registry key!");
	ERROR2IF(lpcszSubKey == NULL,FALSE,"OpenRegKey bad registry key name!");

	HKEY hkReturn;

	// Bodge fix to some bad code: We need to give an acces mask when creating the key
	// but the function's interface does not give us any indication of what access is needed
	// so just try an access mask that may well work.  (Before my change we were trying to open
	// all regestry keys with KEY_ALL_ACCESS which appears to upset NT 4.0 quite a lot).
	REGSAM access = KEY_CREATE_SUB_KEY | KEY_ENUMERATE_SUB_KEYS | KEY_EXECUTE | KEY_QUERY_VALUE |
					KEY_SET_VALUE | KEY_READ | KEY_WRITE;
					
	INT32 nResult = ::RegOpenKeyEx(hKey, lpcszSubKey, 0, access, &hkReturn);

	if (nResult != ERROR_SUCCESS)
	{
		// Extra bodge: If the first open didn't work, try and get access with an easier going
		// access mask - this may make future calls on the returned HKEY fail (ie if they try to
		// write to it) but they were going to fail anyway.
		access = KEY_ENUMERATE_SUB_KEYS | KEY_EXECUTE | KEY_QUERY_VALUE | KEY_READ;
						
		nResult = ::RegOpenKeyEx(hKey, lpcszSubKey, 0, access, &hkReturn);
	}

	// Did it work?
	return (nResult == ERROR_SUCCESS) ? hkReturn : NULL;
}

/********************************************************************************************
>	HKEY OpenRegKey(HKEY hKey, LPCTSTR lpcszSubKey)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95
	Inputs:		hKey			the registry to open
				lpcszSubKey		the sub-keys under hKey to open
	Returns:	The handle of the opened sub-key, or NULL if there isn't one.
	Purpose:	Opens a registry sub-key without creating it if it doesn't exist.
	SeeAlso:	CreateRegKey; SetRegValue
********************************************************************************************/

HKEY OpenRegKeyReadOnly(HKEY hKey, LPCTSTR lpcszSubKey)
{
	ERROR2IF(hKey == NULL,FALSE,"OpenRegKeyReadOnly bad registry key!");
	ERROR2IF(lpcszSubKey == NULL,FALSE,"OpenRegKeyReadOnly bad registry key name!");

	HKEY hkReturn;
	INT32 nResult = ::RegOpenKeyEx(hKey, lpcszSubKey, 0, KEY_READ, &hkReturn);

	// Did it work?
	return (nResult == ERROR_SUCCESS) ? hkReturn : NULL;
}


/********************************************************************************************
>	BOOL CloseRegKey(HKEY hKey)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/96
	Inputs:		hKey			the previously-opened registry to close and be done with
	Returns:	TRUE if successful.
	Purpose:	Closes a previously-opened handle to a registry key.
	SeeAlso:	OpenRegKey
********************************************************************************************/

BOOL CloseRegKey(HKEY hKey)
{
	return ::RegCloseKey(hKey) == ERROR_SUCCESS;
}



/********************************************************************************************
>	BOOL GetRegValue(HKEY hKey, LPTSTR szValueName, LPDWORD pdwType,
					 LPVOID pvBuf, LPDWORD pdwBufSize)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/95
	Inputs:		hKey			registry key the value is under
				szValueName		the name of the value
	Outputs:	pdwType			points to where to put the value's type
				pvBuf			where to put the data
				pdwBufSize		the size of the buffer that will receive the data
	Returns:	TRUE if successful, ie. the named value exists.
	Purpose:	Retrives a named value from a registry (sub)key
	SeeAlso:	SetRegValue
********************************************************************************************/

BOOL GetRegValue(HKEY hKey, LPTSTR szValueName, LPDWORD pdwType, LPVOID pvBuf, LPDWORD pdwBufSize)
{
	return ::RegQueryValueEx(hKey, szValueName, NULL, pdwType, (LPBYTE) pvBuf, pdwBufSize)
				== ERROR_SUCCESS;
}



/********************************************************************************************
>	BOOL SetRegValue(HKEY hKey, LPCTSTR lpcszValueName, DWORD dwType,
					 const VOID* pcData, DWORD cbData)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95
	Inputs:		hKey				the key containing the value
				lpcszValueName		the value's name or NULL if it's the key's only value
				dwType				the type of the value
				pcData				pointer to the value
				cbData				size of the value in bytes
	Returns:	TRUE if successful.
	Purpose:	Write the given value to the given key.
	SeeAlso:	CreateRegKey; OpenRegKey
********************************************************************************************/

BOOL SetRegValue(HKEY hKey, LPCTSTR lpcszValueName, DWORD dwType, const VOID* pcData, DWORD cbData)
{
	ERROR2IF(hKey == NULL,FALSE,"SetRegValue bad registry key!");
	ERROR2IF(pcData == NULL,FALSE,"SetRegValue bad registry key name!");

	return ::RegSetValueEx(hKey, lpcszValueName, 0,
						   dwType, (CONST BYTE*) pcData, cbData) == ERROR_SUCCESS;
}

/********************************************************************************************
>	BOOL SetRegDword(HKEY hKey, LPCTSTR lpcszValueName, INT32 Value)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/2/97
	Inputs:		hKey				the key containing the value
				lpcszValueName		the value's name
				Value				the value to write
	Returns:	TRUE if successful.
	Purpose:	Write the given value to the given key.
	SeeAlso:	CreateRegKey; OpenRegKey; CloseRegKey;
********************************************************************************************/

BOOL SetRegDword(HKEY hKey, LPCTSTR lpcszValueName, DWORD Value)
{
	ERROR2IF(hKey == NULL,FALSE,"SetRegDword bad registry key!");
	ERROR2IF(lpcszValueName == NULL,FALSE,"SetRegDword bad registry key name!");

	DWORD pcData = Value;
	return ::RegSetValueEx(hKey, lpcszValueName, 0,
						   REG_DWORD, (CONST BYTE*)&pcData, sizeof(DWORD)) == ERROR_SUCCESS;
}

/********************************************************************************************
>	BOOL SetRegInt(HKEY hKey, LPCTSTR lpcszValueName, INT32 Value)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/2/97
	Inputs:		hKey				the key containing the value
				lpcszValueName		the value's name
				Value				the value to write
	Returns:	TRUE if successful.
	Purpose:	Write the given value to the given key.
	SeeAlso:	CreateRegKey; OpenRegKey; CloseRegKey;
********************************************************************************************/

BOOL SetRegInt(HKEY hKey, LPCTSTR lpcszValueName, INT32 Value)
{
	ERROR2IF(hKey == NULL,FALSE,"SetRegBool bad registry key!");
	ERROR2IF(lpcszValueName == NULL,FALSE,"SetRegBool bad registry key name!");

	DWORD pcData = Value;
	return ::RegSetValueEx(hKey, lpcszValueName, 0,
						   REG_DWORD, (CONST BYTE*)&pcData, sizeof(DWORD)) == ERROR_SUCCESS;
}

/********************************************************************************************
>	BOOL SetRegBool(HKEY hKey, LPCTSTR lpcszValueName, INT32 Value)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/2/97
	Inputs:		hKey				the key containing the value
				lpcszValueName		the value's name
				Value				the value to write
	Returns:	TRUE if successful.
	Purpose:	Write the given value to the given key.
	SeeAlso:	CreateRegKey; OpenRegKey; CloseRegKey;
********************************************************************************************/

BOOL SetRegBool(HKEY hKey, LPCTSTR lpcszValueName, BOOL Value)
{
	ERROR2IF(hKey == NULL,FALSE,"SetRegBool bad registry key!");
	ERROR2IF(lpcszValueName == NULL,FALSE,"SetRegBool bad registry key name!");

	DWORD pcData = Value;
	return ::RegSetValueEx(hKey, lpcszValueName, 0,
						   REG_BINARY, (CONST BYTE*)&pcData, sizeof(DWORD)) == ERROR_SUCCESS;
}

/********************************************************************************************
>	BOOL SetRegString(HKEY hKey, LPCTSTR lpcszValueName, String_256* str)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/2/97
	Inputs:		hKey				the key containing the value
				lpcszValueName		the string's name
				String				the string to write
	Returns:	TRUE if successful.
	Purpose:	Write the given string to the given key.
	SeeAlso:	CreateRegKey; OpenRegKey; CloseRegKey;
********************************************************************************************/

BOOL SetRegString(HKEY hKey, LPCTSTR lpcszValueName, String_256* str)
{
	ERROR2IF(hKey == NULL,FALSE,"SetRegString bad registry key!");
	ERROR2IF(lpcszValueName == NULL,FALSE,"SetRegString bad registry key name!");
	ERROR2IF(str == NULL,FALSE,"SetRegString bad string");

	return SetRegValue(hKey, lpcszValueName, REG_SZ, *str, str->Length());
}


/********************************************************************************************
>	BOOL SetRegString(HKEY hKey, LPCTSTR lpcszValueName, TCHAR * str)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/2/97
	Inputs:		hKey				the key containing the value
				lpcszValueName		the string's name
				String				the string to write
	Returns:	TRUE if successful.
	Purpose:	Write the given string to the given key.
	SeeAlso:	CreateRegKey; OpenRegKey; CloseRegKey;
********************************************************************************************/

BOOL SetRegString(HKEY hKey, LPCTSTR lpcszValueName, TCHAR * str)
{
	ERROR2IF(hKey == NULL,FALSE,"SetRegString bad registry key!");
	ERROR2IF(lpcszValueName == NULL,FALSE,"SetRegString bad registry key name!");
	ERROR2IF(str == NULL,FALSE,"SetRegString bad string");

	return SetRegValue(hKey, lpcszValueName, REG_SZ, str, ::lstrlen(str));
}


/********************************************************************************************
>	BOOL DeleteRegKey(HKEY hKey, LPCTSTR szSubKey)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Inputs:		hKey		the key containing the sub-key to delete
				szSubKey	the sub-key to be deleted
	Returns:	TRUE if successful.
	Purpose:	Deletes a sub-key from the registry.  To be compatible with Windows NT
				you may only delete a sub-key that has no sub-keys itself.
	SeeAlso:	CreateRegKey
********************************************************************************************/

BOOL DeleteRegKey(HKEY hKey, LPCTSTR szSubKey)
{
	return ::RegDeleteKey(hKey, szSubKey) == ERROR_SUCCESS;
}


/********************************************************************************************

>	BOOL DeleteRegKeyAndSubKeys(HKEY hKey, LPCTSTR szSubKey)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/97
	Inputs:		hKey		the key containing the sub-key to delete
				szSubKey	the sub-key to be deleted
	Returns:	TRUE if successful.
	Purpose:	Deletes a sub-key and all the sub- keys of that from the registry.
				This is to be compatible with Windows NT as this can only delete a sub-key
				that has no sub-keys itself.
				It assumes that there are only sub-keys of the specified one i.e. it does not
				recursively delete the sub-keys.
	SeeAlso:	DeleteRegKey;
********************************************************************************************/

BOOL DeleteRegKeyAndSubKeys(HKEY hKey, LPCTSTR szSubKey)
{
	ERROR2IF(hKey == NULL,FALSE,"DeleteRegKeyAndSubKeys bad registry key!");
	ERROR2IF(szSubKey == NULL,FALSE,"DeleteRegKeyAndSubKeys bad registry key name!");
		
	// To enumerate subkeys, an application should initially call the RegEnumKeyEx function
	// with the dwIndex parameter set to zero. The application should then increment the
	// dwIndex parameter and call RegEnumKeyEx until there are no more subkeys
	// (until the function returns ERROR_NO_MORE_ITEMS). 
	// The key identified by hKey must have been opened with KEY_ENUMERATE_SUB_KEYS access
	// (KEY_READ includes KEY_ENUMERATE_SUB_KEYS). Use the RegCreateKeyEx or RegOpenKeyEx
	// function to open the key. 

	// Open up the requested sub-key
	HKEY hSubKey = OpenRegKey(hKey, szSubKey);
	if (hSubKey == NULL)
		return FALSE;
	
	const INT32 buf_size = 256;
    DWORD dwIndex	= 0;				// index of subkey to enumerate 
    TCHAR Name[buf_size];				// buffer for subkey name
    DWORD cbName	= buf_size;			// size of subkey buffer 
    TCHAR Class[buf_size];				// buffer for class string 
    DWORD cbClass	= buf_size;			// size of class buffer 
    FILETIME ftLastWriteTime;			// time key last written to 

	INT32 nResult = ERROR_SUCCESS;
	// We are going to go through evaluating all the items and add them to a list
	// Then we will go through the list and delete the items.
	// Need to do this as otherwise the enum seems to miss out half the items!
	// We will use LibSettingsListItem as this is just a string stored as a list item
	// which is all we want.
	List *pRegItemsList = new List;
	if (pRegItemsList == NULL)
		return FALSE;

	String_256 KeyName;
	do
	{
		// reset the string sizes to their maximum for receiving data
		cbName	= buf_size;
		cbClass	= buf_size;
		// Does another sub-key exist?
		nResult = ::RegEnumKeyEx(hSubKey, dwIndex, Name, &cbName, NULL, Class, &cbClass, &ftLastWriteTime);
		if (nResult == ERROR_SUCCESS)
		{
			// we found a sub-key and so delete it
			KeyName = Name;
			LibSettingsListItem *pLibSettingsItem = new LibSettingsListItem(&KeyName);
			pRegItemsList->AddTail((ListItem *)pLibSettingsItem);
			//DeleteRegKey(hSubKey, Name); 
		}
		
		// increment our indexing item and try again
		dwIndex ++;

	} while (nResult == ERROR_SUCCESS);

	// now go through and delete the items
	LibSettingsListItem *pLibSettingsItem = (LibSettingsListItem *)pRegItemsList->GetHead();
	while (pLibSettingsItem)
	{
		DeleteRegKey(hSubKey, *pLibSettingsItem->Line);
		pLibSettingsItem = (LibSettingsListItem *)pRegItemsList->GetNext(pLibSettingsItem); 
	}

	// delete all the list items and the list that we created
	pRegItemsList->DeleteAll();
	delete pRegItemsList;

	CloseRegKey(hSubKey);
	
	// now delete the main sub-key
	DeleteRegKey(hKey, szSubKey);

	return TRUE;
}



/********************************************************************************************
>	DWORD GetRegDword(HKEY hKey, LPCTSTR szValueName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/95
	Inputs:		hKey			the registry key the value is stored under
				szValueName		the name of the value within the key
	Returns:	The value, or zero if the value isn't there or is of the wrong type.
				If you need more control of errors then you should use the full
				GetRegValue function.
	Purpose:	Retrieves a DWORD value form the registry.
	SeeAlso:	GetRegString; GetRegValue
********************************************************************************************/

DWORD GetRegDword(HKEY hKey, LPCTSTR szValueName)
{
	ERROR2IF(hKey == NULL,FALSE,"GetRegDword bad registry key!");
	ERROR2IF(szValueName == NULL,FALSE,"GetRegDword bad registry key name!");

	DWORD dwResult, dwType = REG_DWORD, dwSize = sizeof(DWORD);
	if (!GetRegValue(hKey, (LPTSTR) szValueName, &dwType, &dwResult, &dwSize) ||
		dwType != REG_DWORD || dwSize != sizeof(DWORD))
	{
		// Not there or wrong type so return zero.
		dwResult = 0;
	}
	return dwResult;
}

/********************************************************************************************
>	INT32 GetRegInt(HKEY hKey, LPCTSTR szValueName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/2/97
	Inputs:		hKey			the registry key the value is stored under
				szValueName		the name of the value within the key
	Returns:	The value, or zero if the value isn't there or is of the wrong type.
				If you need more control of errors then you should use the full
				GetRegValue function.
	Purpose:	Retrieves an INT32 value form the registry.
	SeeAlso:	GetRegString; GetRegValue
********************************************************************************************/

INT32 GetRegInt(HKEY hKey, LPCTSTR szValueName)
{
	ERROR2IF(hKey == NULL,FALSE,"GetRegInt bad registry key!");
	ERROR2IF(szValueName == NULL,FALSE,"GetRegInt bad registry key name!");

	DWORD dwResult = 0;
	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof(DWORD);
	if (!GetRegValue(hKey, (LPTSTR) szValueName, &dwType, &dwResult, &dwSize) ||
		dwType != REG_DWORD || dwSize != sizeof(DWORD))
	{
		// Not there or wrong type so return zero.
		dwResult = 0;
	}
	return (INT32)dwResult;
}

/********************************************************************************************
>	BOOL GetRegBool(HKEY hKey, LPCTSTR szValueName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/2/97
	Inputs:		hKey			the registry key the value is stored under
				szValueName		the name of the value within the key
	Returns:	The value, or zero if the value isn't there or is of the wrong type.
				If you need more control of errors then you should use the full
				GetRegValue function.
	Purpose:	Retrieves a bool value form the registry.
	SeeAlso:	GetRegString; GetRegValue
********************************************************************************************/

BOOL GetRegBool(HKEY hKey, LPCTSTR szValueName)
{
	ERROR2IF(hKey == NULL,FALSE,"GetRegBool bad registry key!");
	ERROR2IF(szValueName == NULL,FALSE,"GetRegBool bad registry key name!");

	DWORD dwResult = 0;
	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof(DWORD);
	if (!GetRegValue(hKey, (LPTSTR) szValueName, &dwType, &dwResult, &dwSize) ||
		(dwType != REG_DWORD && dwType != REG_BINARY) || dwSize != sizeof(DWORD))
	{
		// Not there or wrong type so return zero.
		dwResult = 0;
	}
	return (BOOL)dwResult;
}

/********************************************************************************************
>	BOOL GetRegString(HKEY hKey, LPCTSTR szValueName, String_256* pOut)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/95
	Inputs:		hKey			the registry containing the value
				szValueName		the name of the value
	Outputs:	pOut			pointer to a String_256 to receive the value
	Returns:	TRUE if successful, FALSE otherwise (eg. if the type is wrong).
	Purpose:	Retrieves a string from the registry.  If you need more control over
				errors you should use the general GetRegValue function.
	Errors:		-
	SeeAlso:	GetRegDword; GetRegValue
********************************************************************************************/

BOOL GetRegString(HKEY hKey, LPCTSTR szValueName, String_256* pOut)
{
	ERROR2IF(hKey == NULL,FALSE,"GetRegString bad registry key!");
	ERROR2IF(szValueName == NULL,FALSE,"GetRegString bad registry key name!");
	ERROR2IF(pOut == NULL,FALSE,"GetRegString bad return string buffer!");

	DWORD dwType = REG_SZ, dwSize = 256;
	return GetRegValue(hKey, (LPTSTR) szValueName, &dwType, (LPTSTR) *pOut, &dwSize) &&
		   dwType == REG_SZ && dwSize <= 256;
}

/********************************************************************************************
>	BOOL GetRegString(HKEY hKey, LPCTSTR szValueName, TCHAR * str, UINT32 str_len = 256)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/97
	Inputs:		hKey			the registry containing the value
				szValueName		the name of the value
				str_len			size of the return buffer, defaults to 256
	Outputs:	str				pointer to a TCHAR buffer to receive the value
	Returns:	TRUE if successful, FALSE otherwise (eg. if the type is wrong).
	Purpose:	Retrieves a string from the registry.  If you need more control over
				errors you should use the general GetRegValue function.
	Errors:		-
	SeeAlso:	GetRegDword; GetRegValue
********************************************************************************************/

BOOL GetRegString(HKEY hKey, LPCTSTR szValueName, TCHAR * str, UINT32 str_len)
{
	ERROR2IF(hKey == NULL,FALSE,"GetRegString bad registry key!");
	ERROR2IF(szValueName == NULL,FALSE,"GetRegString bad registry key name!");
	ERROR2IF(str == NULL,FALSE,"GetRegString bad return string buffer!");

	DWORD dwType = REG_SZ;
	DWORD dwSize = str_len;
	return GetRegValue(hKey, (LPTSTR) szValueName, &dwType, str, &dwSize) &&
		   dwType == REG_SZ && dwSize <= str_len;
}

/********************************************************************************************
/ helper classes
********************************************************************************************/

/********************************************************************************************

>	BOOL SaveRegistryEntries::StartSaving(String_256 *pKeySpecifier, HKEY hSubKey = hAppStateRegKey)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/97
	Inputs:		pKeySpecifier	string specifying the key name to store the data under
				hSubKey			the sub key to start the section under, defaults to the main
								program section of the regsitry
	Returns:	TRUE if successful.
	Purpose:	Starts the process of saving our a series of registry entries e.g. a pathname
				list.
				A sub-key to the main section can be specified so that instead of opening the
				section <program> \ *pKeySpecifier it opens a sub-section to this.

********************************************************************************************/

BOOL SaveRegistryEntries::StartSaving(String_256 *pKeySpecifier, HKEY hSubKey)
{
	// Don't do this under Win32s.
	if (IsWin32s() && !IsWin32c()) return FALSE;

	ERROR2IF(hSubKey == NULL, FALSE, "SaveRegistryEntries::StartSaving Bad main app key");
	ERROR2IF(pKeySpecifier == NULL, FALSE, "SaveRegistryEntries::StartSaving Bad key specifier");
	ERROR2IF(m_hRegKey != NULL, FALSE, "SaveRegistryEntries::StartSaving hRegKey is already non-null");

	// Ensure the section is blank before we start saving any data
	DeleteRegKey(hSubKey, *pKeySpecifier);

	// First open/create a new sub-key under the app-state key for holding the data.
	// e.g. TEXT("Workspace\\MDI")
	// Use the passed in key which will default to the main app hAppStateRegKey if not supplied
	m_hRegKey = CreateRegKey(hSubKey, *pKeySpecifier);
	if (m_hRegKey == NULL)
	{
		ERROR3("Can't get a registry key in SaveRegistryEntries::StartSaving");
		return FALSE;
	}

	// Ensure the section is blank before we start saving any data
/*	BOOL ok = TRUE;
	TCHAR buf[10];
	INT32 i = 0;
	while (ok)
	{
		wsprintf(buf, TEXT("%d"), i + 1);
		i++;
		ok = DeleteRegKey(m_hRegKey, buf);
	}
*/
	return TRUE;
}

/********************************************************************************************

>	BOOL SaveRegistryEntries::SaveNextEntry(String_256 *pNextName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/97
	Inputs:		pNextName	the next bit of data to be saved in the list
	Returns:	TRUE if successful.
	Purpose:	Saves out the next entry into the opened key.

********************************************************************************************/

BOOL SaveRegistryEntries::SaveNextEntry(String_256 *pNextName)
{
	if (m_hRegKey == NULL)
		return FALSE;

	// Cannot save out the item with a blank name so invent a unique name for it
	TCHAR buf[10];
	wsprintf(buf, TEXT("%d"), m_Counter + 1);
	m_Counter++;

	return SetRegValue(m_hRegKey, buf, REG_SZ, *pNextName, pNextName->Length() + 1);
}

/********************************************************************************************

>	BOOL SaveRegistryEntries::StopSaving()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/97
	Returns:	TRUE if successful.
	Purpose:	Stops the process of saving our a series of registry entries e.g. a pathname
				list.

********************************************************************************************/

BOOL SaveRegistryEntries::StopSaving()
{
	ERROR2IF(m_hRegKey == NULL, FALSE, "hRegKey is null");
	BOOL closed = CloseRegKey(m_hRegKey);
	m_hRegKey = NULL;
	return closed;
}

/********************************************************************************************

>	SaveRegistryEntries::~SaveRegistryEntries()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/97
	Purpose:	Standard destructor.

********************************************************************************************/

SaveRegistryEntries::~SaveRegistryEntries()
{
	if (m_hRegKey != NULL)
		CloseRegKey(m_hRegKey);
	return;
}




/********************************************************************************************

>	BOOL LoadRegistryEntries::StartLoading(String_256 *pKeySpecifier, HKEY hSubKey = hAppStateRegKey)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/97
	Inputs:		pKeySpecifier	string specifying the key name to store the data under
				hSubKey			the sub key to start the section under, defaults to the main
								program section of the regsitry
	Returns:	TRUE if successful.
	Purpose:	Starts the process of saving our a series of registry entries e.g. a pathname
				list.
				A sub-key to the main section can be specified so that instead of opening the
				section <program> \ *pKeySpecifier it opens a sub-section to this.

********************************************************************************************/

BOOL LoadRegistryEntries::StartLoading(String_256 *pKeySpecifier, HKEY hSubKey)
{
	// Don't do this under Win32s.
	if (IsWin32s() && !IsWin32c()) return FALSE;

	ERROR2IF(hSubKey == NULL, FALSE, "LoadRegistryEntries::StartLoading Bad main app key ");
	ERROR2IF(pKeySpecifier == NULL, FALSE, "LoadRegistryEntries::StartLoading Bad key specifier");
	ERROR2IF(m_hRegKey != NULL, FALSE, "LoadRegistryEntries::StartLoading hRegKey is already non-null");

	// First open a new sub-key under the app-state key for holding the data.
	// e.g. TEXT("Workspace\\MDI")
	// Use the passed in key which will default to the main app hAppStateRegKey if not supplied
	m_hRegKey = OpenRegKey(hSubKey, *pKeySpecifier);
	if (m_hRegKey == NULL)
	{
		// don't error as some people require this functionality
		//ERROR3("Can't get a registry key in LoadRegistryEntries::StartLoading");
		return FALSE;
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL LoadRegistryEntries::LoadNextEntry(String_256 *pNextName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/97
	Inputs:		pNextName	the next bit of data loaded from the list
	Returns:	TRUE if successful.
	Purpose:	Loads in the next entry into the opened key.

********************************************************************************************/

BOOL LoadRegistryEntries::LoadNextEntry(String_256 *pNextName)
{
	if (m_hRegKey == NULL)
		return FALSE;

	// Cannot save out the item with a blank name so invent a unique name for it
	TCHAR buf[10];
	wsprintf(buf, TEXT("%d"), m_Counter + 1);
	m_Counter++;

	return GetRegString(m_hRegKey, buf, pNextName);
}

/********************************************************************************************

>	BOOL LoadRegistryEntries::StopLoading()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/97
	Returns:	TRUE if successful.
	Purpose:	Stops the process of loading our a series of registry entries e.g. a pathname
				list.

********************************************************************************************/

BOOL LoadRegistryEntries::StopLoading()
{
	ERROR2IF(m_hRegKey == NULL, FALSE, "hRegKey is null");
	BOOL closed = CloseRegKey(m_hRegKey);
	m_hRegKey = NULL;
	return closed;
}

/********************************************************************************************

>	LoadRegistryEntries::~LoadRegistryEntries()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/97
	Purpose:	Standard destructor.

********************************************************************************************/

LoadRegistryEntries::~LoadRegistryEntries()
{
	if (m_hRegKey != NULL)
		CloseRegKey(m_hRegKey);
	return;
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//									R  E  G  I  S  T  R  Y
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //





/********************************************************************************************

>	BOOL Registry::InitializeProbe(ClassCategoryProbe& Probe)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	The TemplateOp COM things are registered as implementing CATID
				We can build a list of them at start up so the Webster Properties Dialog
				shows them.

	Inputs:		Probe:	The ClassCategoryProbe to initialize with data specific to this
						registry.

	Returns:	TRUE if successful, FALSE if not

	Notes:		Only works for the local registry no matter what

********************************************************************************************/
BOOL Registry::InitializeProbe(ClassCategoryProbe& Probe)
{
	return Probe.Init();
}






// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//						R  E  G  I  S  T  R  Y  E  N  T  R  Y
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //





/********************************************************************************************

>	virtual BOOL RegistryEntry::GetEmbeddedEntry(	const StringBase& Name, 
													RegistryEntry* const pEntry) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Returns a "subkey" of the given entry.

	Inputs:		Name:	The name of the subkey value (not Null String)
	Outputs:	pEntry:	A pointer to the registry entry of the given Name.

********************************************************************************************/
BOOL RegistryEntry::GetEmbeddedEntry(const StringBase& Name, RegistryEntry* const pEntry) const
{
	pEntry->m_hKey = OpenRegKey(m_hKey, Name);

	return (pEntry->m_hKey != NULL);
}


/********************************************************************************************

>	BOOL RegistryEntry::GetValue(const StringBase& ValueName, StringBase* const pValue)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Returns a value with the given name under this registry key.

	Inputs:		ValueName:	The name of the subkey value (Null String will work)
	Outputs:	pValue:		The data associated with the given name

	Returns:	TRUE if the value was found, FALSE if not

	Notes:		Only works for the local registry no matter what

********************************************************************************************/
BOOL RegistryEntry::GetValue(const StringBase& ValueName, StringBase* const pValue)
{
	ERROR2IF(pValue == NULL, FALSE, "NULL Args");

	DWORD dwType = REG_SZ, dwSize = pValue->MaxLength();

	return (::RegQueryValueEx(m_hKey, ValueName, NULL, &dwType, (LPBYTE)((LPTSTR)*pValue), &dwSize) == 
			ERROR_SUCCESS) && dwType == REG_SZ && dwSize <= DWORD(pValue->MaxLength());
}





/////////////////////////////////////////////////////////////////////////////////////////////





const String_8 ClassRegistryEntry::s_CLSIDString = TEXT("CLSID\\");


/********************************************************************************************

>	BOOL ClassRegistryEntry::InitFromCLSID(const CLSID& ClassID)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Initializes this RegistryEntry so it represents an entry for the given CLSID.
				Allows us to retrieve values from the CLSID's subkeys.

	Inputs:		ClassID:	The CLSID that is this RegistryEntry

	Returns:	TRUE if it worked, FALSE if not

	Notes:		Only works for the local registry no matter what

********************************************************************************************/
BOOL ClassRegistryEntry::InitFromCLSID(const CLSID& ClassID)
{
	BOOL Ok = TRUE;			// return this

	String_64 ClassIDString;
	Ok = ConvertCLSIDToString(ClassID, &ClassIDString);

	if (Ok)
	{
		String_64 KeyString = s_CLSIDString;
		KeyString += ClassIDString;
		m_hKey = OpenRegKey(HKEY_CLASSES_ROOT, KeyString);
		Ok = (m_hKey != NULL);
	}

	return Ok;
}






/////////////////////////////////////////////////////////////////////////////////////////////
const String_32 VersionIndependentProgID::s_KeyName = TEXT("VersionIndependentProgID");


VersionIndependentProgID::VersionIndependentProgID(const ClassRegistryEntry& CLSIDEntry)
{
	CLSIDEntry.GetEmbeddedEntry(s_KeyName, this);
}


BOOL VersionIndependentProgID::GetName(StringBase* const pName)
{
	return GetValue(NullString, pName);
}









/********************************************************************************************

>	ClassCategoryProbe::ClassCategoryProbe()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	This default constructor should be followed by a call to
				InitializeEnumerator() if you want it to work.

********************************************************************************************/
ClassCategoryProbe::ClassCategoryProbe()
{
	m_pEnumCLSID = NULL;
}


/********************************************************************************************

>	BOOL ClassCategoryProbe::Init(const CATID& ComponentCategoryCLSID)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Inputs:		ComponentCategoryCLSID:	The CLSID of the Component Category for which this
										probe will return CLSIDs of classes that implement
										the interfaces of that category.

	Returns:	S_OK if you're lucky
				E_... if you're not
				E_OUTOFMEMORY if the creation of ICatInformation failed in any way.

	Purpose:	This is the second part of the two part constructor. It can fail.
				I wanted the names of Template handler/op things from the registry which are
				under CATID

	Notes:		There should probably be another argument that the registry passes in (for
				remote registries etc)

********************************************************************************************/
BOOL ClassCategoryProbe::Init()
{
	// Don't try the enumeration more than once 'cos I haven't checked it will work
	ERROR2IF(m_pEnumCLSID != NULL, FALSE, "NULL Args");

	HRESULT Result = S_OK;

	ICatInformation* const pCatInformer = CreateCatInformer();
	if (pCatInformer == NULL)
	{
		Result = E_OUTOFMEMORY;
	}

	// Create an enumerator for all the CLSIDs in the given component category
	if (SUCCEEDED(Result))
	{
		CATID Implements = GetCategoryID();

		Result = pCatInformer->EnumClassesOfCategories(
								1,				//Number of category IDs in the rgcatidImpl array
								&Implements,	//Array of category identifiers
								0,				//Number of category IDs in the rgcatidReq array
								&Implements,	//Array of category identifiers
								&m_pEnumCLSID	//Location in which to return an IEnumCLSID interface 
									   );
	}

	// Get rid of our local ICatInfo
	if (pCatInformer != NULL)
	{
		pCatInformer->Release();
	}

	// Get the first CLSID so it's returned by GetNextCLSID and IsLast works properly
	if (SUCCEEDED(Result))
	{
		Result = m_pEnumCLSID->Next(1,						//Number of elements requested
									&m_NextCLSIDToReturn,	//Array of the elements
									NULL					//Pointer to the number of elements actually supplied
									);

		if (Result == S_FALSE)
		{
			// There weren't any CLSIDs so make this the NULL probe
			m_pEnumCLSID->Release();
			m_pEnumCLSID = NULL;
		}
	}

	return SUCCEEDED(Result);
}


/********************************************************************************************

>	ICatInformation* ClassCategoryProbe::CreateCatInformer()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Returns:	An ICatInformation interface.
				NULL if it failed in any way

	Purpose:	Support function to get us an instance of ICatInformation. Don't forget to
				Release it.

	Notes:		It should probably have an argument giving the registry to use

********************************************************************************************/
ICatInformation* ClassCategoryProbe::CreateCatInformer()
{
	HRESULT Result = S_OK;

	ICatInformation* pCatInformer = NULL;

	Result = CoCreateInstance(	CLSID_StdComponentCategoriesMgr,	//Class identifier (CLSID) of the object
								NULL,					//Pointer to whether object is or isn't part of an aggregate 
								CLSCTX_INPROC_SERVER,	//Context for running executable code
							    IID_ICatInformation,	//Reference to the identifier of the interface
								(LPVOID*)&pCatInformer	//Indirect pointer to requested interface
							);
	if (FAILED(Result))
	{
		TRACE( _T("ClassCategoryProbe::CreateCatInfoInterface failed with %x\n"), Result);
	}

	return pCatInformer;
}


/********************************************************************************************

>	ClassCategoryProbe::ClassCategoryProbe(const ClassCategoryProbe& OtherProbe)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	This copy constructor stops any COM objects disappearing between copies.

********************************************************************************************/
ClassCategoryProbe::ClassCategoryProbe(const ClassCategoryProbe& OtherProbe)
{
	m_pEnumCLSID = OtherProbe.m_pEnumCLSID;
	if (m_pEnumCLSID != NULL)
	{
		m_pEnumCLSID->AddRef();
	}
}


/********************************************************************************************

>	ClassCategoryProbe::~ClassCategoryProbe()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	This destructor Releases any COM objects

********************************************************************************************/
ClassCategoryProbe::~ClassCategoryProbe()
{
	if (m_pEnumCLSID != NULL)
	{
		m_pEnumCLSID->Release();
		m_pEnumCLSID = NULL;
	}
}


/********************************************************************************************

>	BOOL ClassCategoryProbe::GetNextEntry(ClassRegistryEntry* const pEntry)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Does the enumeration returning the ClassRegistryEntry of the next class in the
				category.

	Outputs:	The next RegistryEntry in the enumeration
	Returns:	TRUE if all was OK, FALSE otherwise

********************************************************************************************/
BOOL ClassCategoryProbe::GetNextEntry(ClassRegistryEntry* const pEntry)
{
	BOOL Ok = TRUE;

	CLSID NextCLSID;
	Ok = GetNextCLSID(&NextCLSID);

	if (Ok)
	{
		Ok = pEntry->InitFromCLSID(NextCLSID);
	}

	return Ok;
}


/********************************************************************************************

>	BOOL ClassCategoryProbe::GetNextCLSID(CLSID* const pNextCLSID)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Returns:	TRUE if all was OK, FALSE otherwise

	Purpose:	Does the enumeration returning the CLSID of any classes that implement
				the Category for this probe.

********************************************************************************************/
BOOL ClassCategoryProbe::GetNextCLSID(CLSID* const pNextCLSID)
{
	HRESULT Result = S_OK;

	*pNextCLSID = m_NextCLSIDToReturn;

	if (m_pEnumCLSID != NULL)
	{
		Result = m_pEnumCLSID->Next(1,						//Number of elements requested
									&m_NextCLSIDToReturn,	//Array of the elements
									NULL					//Pointer to the number of elements actually supplied
									);
	}
	else
	{
		TRACE( _T("ClassCategoryProbe::operator++(INT32) - NULL m_pEnumCLSID\n"));
		Result = E_UNEXPECTED;
	}

	if (Result == S_FALSE)
	{
		// That was the last one so make this the NULL probe
		m_pEnumCLSID->Release();
		m_pEnumCLSID = NULL;
	}

	return SUCCEEDED(Result);
}


/********************************************************************************************

>	BOOL ClassCategoryProbe::IsLast() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Returns:	FALSE if there are any class id's to be had, otherwise TRUE

	Purpose:	Determines when to finish the enumeration

********************************************************************************************/
BOOL ClassCategoryProbe::IsLast() const
{
	return m_pEnumCLSID == NULL;
}

