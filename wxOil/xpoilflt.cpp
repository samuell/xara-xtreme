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

// The module that controls the OIL side of import/export filters.


#include "camtypes.h"

#include "xpoilflt.h"

#include "xpfilter.h"
//#include "xpfres.h"
//#include "mario.h"

#include "strlist.h"
#include "fileutil.h"
#include "sgliboil.h"

#include "ncntrcnt.h"
//#include "nev.h"		// For _R(IDN_USER_CANCELLED)
#include "xmlutils.h"

#include "camprocess.h"

CC_IMPLEMENT_MEMDUMP(PluginOILFilter, OILFilter)
CC_IMPLEMENT_DYNAMIC(PathNameListItem, ListItem)

PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
IMPLEMENT_DYNCREATE(PluginFilterCallback, CCmdTarget)

BEGIN_INTERFACE_MAP(PluginFilterCallback, CCmdTarget)
	INTERFACE_PART(PluginFilterCallback, __uuidof(IXPFCallback), _XPFCallback)
END_INTERFACE_MAP()
#endif

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

PropMapEntry aShadowTypes[] = { {_T("wall"), XPFP_SHADOWTYPE_WALL},
								{_T("floor"), XPFP_SHADOWTYPE_FLOOR},
								{_T("glow"), XPFP_SHADOWTYPE_GLOW},
								{_T("feather"), XPFP_SHADOWTYPE_FEATHER},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aBevelTypes[] = { {_T("flat"), XPFP_BEVELTYPE_FLAT},
								{_T("round"), XPFP_BEVELTYPE_ROUND},
								{_T("halfround"), XPFP_BEVELTYPE_HALFROUND},
								{_T("frame"), XPFP_BEVELTYPE_FRAME},
								{_T("mesa1"), XPFP_BEVELTYPE_MESA_1},
								{_T("mesa2"), XPFP_BEVELTYPE_MESA_2},
								{_T("smooth1"), XPFP_BEVELTYPE_SMOOTH_1},
								{_T("smooth2"), XPFP_BEVELTYPE_SMOOTH_2},
								{_T("point1"), XPFP_BEVELTYPE_POINT_1},
								{_T("point2a"), XPFP_BEVELTYPE_POINT_2a},
								{_T("point2b"), XPFP_BEVELTYPE_POINT_2b},
								{_T("ruffle2a"), XPFP_BEVELTYPE_RUFFLE_2a},
								{_T("ruffle2b"), XPFP_BEVELTYPE_RUFFLE_2b},
								{_T("ruffle3a"), XPFP_BEVELTYPE_RUFFLE_3a},
								{_T("ruffle3b"), XPFP_BEVELTYPE_RUFFLE_3b},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aBevelSides[] = { {_T("inner"), XPFP_BEVELSIDE_INNER},
								{_T("outer"), XPFP_BEVELSIDE_OUTER},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aColourEffects[] = { {_T("fade"), XPFP_COLOUREFFECT_FADE},
								{_T("rainbow"), XPFP_COLOUREFFECT_RAINBOW},
								{_T("altrainbow"), XPFP_COLOUREFFECT_ALTRAINBOW},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aMouldTypes[] = { {_T("envelope"), XPFP_MOULDTYPE_ENVELOPE},
								{_T("perspective"), XPFP_MOULDTYPE_PERSPECTIVE},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aLineCaps[] = { {_T("butt"), XPFP_LINECAP_BUTT},
								{_T("round"), XPFP_LINECAP_ROUND},
								{_T("square"), XPFP_LINECAP_SQUARE},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aLineJoins[] = { {_T("mitre"), XPFP_LINEJOIN_MITRE},
								{_T("round"), XPFP_LINEJOIN_ROUND},
								{_T("bevel"), XPFP_LINEJOIN_BEVEL},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aFillShapes[] = { {_T("flat"), FILLSHAPE_FLAT},
								{_T("linear"), FILLSHAPE_LINEAR},
								{_T("circular"), FILLSHAPE_CIRCULAR},
								{_T("elliptical"), FILLSHAPE_ELLIPTICAL},
								{_T("conical"), FILLSHAPE_CONICAL},
								{_T("diamond"), FILLSHAPE_DIAMOND},
								{_T("3point"), FILLSHAPE_3POINT},
								{_T("4point"), FILLSHAPE_4POINT},
								{_T("bitmap"), FILLSHAPE_BITMAP},
								{_T("clouds"), FILLSHAPE_CLOUDS},
								{_T("plasma"), FILLSHAPE_PLASMA},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aFillRepeats[] = { {_T("simple"), XPFP_FILLREPEAT_SIMPLE},
								{_T("repeat"), XPFP_FILLREPEAT_REPEAT},
								{_T("repeatinv"), XPFP_FILLREPEAT_REPEATINV},
								{_T("repeatex"), XPFP_FILLREPEAT_REPEATEXTRA},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aTransTypes[] = { {_T("none"), TT_NoTranspType},
								{_T("mix"), TT_Mix},
								{_T("stained"), TT_StainGlass},
								{_T("bleach"), TT_Bleach},
								{_T("contrast"), TT_CONTRAST},
								{_T("saturation"), TT_SATURATION},
								{_T("darken"), TT_DARKEN},
								{_T("lighten"), TT_LIGHTEN},
								{_T("brightness"), TT_BRIGHTNESS},
								{_T("luminosity"), TT_LUMINOSITY},
								{_T("hue"), TT_HUE},
								{_T("bevel"), TT_BEVEL},
								{NULL, XPFP_UNKNOWN}};


PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
/******************************************************************************************

>	PluginFilterCallback::PluginFilterCallback(PluginNativeFilter* pFilter = NULL)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/2005
	Purpose:	Constructs a new PluginFilterCallback object.

******************************************************************************************/

PluginFilterCallback::PluginFilterCallback(PluginNativeFilter* pFilter)
{
	m_pFilter = pFilter;
}




/******************************************************************************************

>	PluginFilterCallback::~PluginFilterCallback()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/2005
	Purpose:	Destructs a PluginFilterCallback object.

******************************************************************************************/

PluginFilterCallback::~PluginFilterCallback()
{
}




/********************************************************************************************

>	STDMETHODIMP PluginFilterCallback::X_XPEHost::QueryInterface()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/2005

********************************************************************************************/

STDMETHODIMP PluginFilterCallback::X_XPFCallback::QueryInterface(REFIID iid, LPVOID* ppvObj)
{
	METHOD_PROLOGUE(PluginFilterCallback, _XPFCallback)
	return pThis->ExternalQueryInterface(&iid, ppvObj);
}




/********************************************************************************************

>	STDMETHODIMP PluginFilterCallback::X_XPEHost::AddRef()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/2005

********************************************************************************************/


STDMETHODIMP_(UINT32) PluginFilterCallback::X_XPFCallback::AddRef()
{
	METHOD_PROLOGUE(PluginFilterCallback, _XPFCallback)
	return pThis->ExternalAddRef();
}




/********************************************************************************************

>	STDMETHODIMP PluginFilterCallback::X_XPEHost::Release()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/2005

********************************************************************************************/


STDMETHODIMP_(UINT32) PluginFilterCallback::X_XPFCallback::Release()
{
	METHOD_PROLOGUE(PluginFilterCallback, _XPFCallback)
	return pThis->ExternalRelease();
}




/********************************************************************************************

>	STDMETHODIMP PluginFilterCallback::X_XPEHost::EndEdit(INT32 EndEditFlags)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/2005

********************************************************************************************/

STDMETHODIMP PluginFilterCallback::X_XPFCallback::Progress(INT32 lProgress)
{
	METHOD_PROLOGUE(PluginFilterCallback, _XPFCallback)

	TRACE( _T("IXPFCallback::Progress(%d)\n"), lProgress);

	if (pThis->m_pFilter && lProgress > 0)
		pThis->m_pFilter->SetProgressBarCount((UINT32)lProgress);

	return S_OK;
}


/****************************************************************************

>	static BOOL PluginOILFilter::AutoRegisterFilters()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/2005

	Returns:	TRUE if ok, FALSE if bother
	Purpose:	Registers any plugin filter objects that aren't registered

****************************************************************************/

BOOL PluginOILFilter::AutoRegisterFilters()
{
	// Build a list of the registered filters (dll leafnames)

	// Iterate through the component category adding each filter to the filter list
	HRESULT hRes = S_OK;

	CComPtr<ICatInformation> pCatInformer;
	hRes = pCatInformer.CoCreateInstance(CLSID_StdComponentCategoriesMgr);
	if (FAILED(hRes))
	{
		// Return an error here
		return(TRUE);
	}

	CComPtr<IEnumCLSID> pEnumCLSID;
	CATID Categories[4] = {
							{0x42F818E1, 0x9EF6, 0x4241, {0x90, 0x9B, 0x91, 0xE7, 0x83, 0xB9, 0xB9, 0x35}},
							{0x42F818E1, 0x9EF6, 0x4241, {0x90, 0x9B, 0x91, 0xE7, 0x83, 0xB9, 0xB9, 0x36}},
							{0x42F818E1, 0x9EF6, 0x4241, {0x90, 0x9B, 0x91, 0xE7, 0x83, 0xB9, 0xB9, 0x37}},
							{0x42F818E1, 0x9EF6, 0x4241, {0x90, 0x9B, 0x91, 0xE7, 0x83, 0xB9, 0xB9, 0x38}}
						};
	hRes = pCatInformer->EnumClassesOfCategories(4, Categories, (UINT32)-1, NULL, &pEnumCLSID);
	if (FAILED(hRes))
	{
		// Return an error here
		return(TRUE);
	}

	List FilterList;

	UINT32 NumRead = 1;
	CLSID Clsid;
	while (NumRead != 0)
	{
		NumRead = 0;
		hRes = pEnumCLSID->Next(1, &Clsid, &NumRead);
		if (FAILED(hRes))
			break;

		if (NumRead > 0)
		{
			// Read the InProc32 regsitry entry for this Clsid
			
			// Generate the relevant registy key name
			TCHAR sKey[64];
			wsprintf(sKey, _T("CLSID\\{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}\\InProcServer32"),
							Clsid.Data1,
							Clsid.Data2,
							Clsid.Data3,
							Clsid.Data4[0],
							Clsid.Data4[1],
							Clsid.Data4[2],
							Clsid.Data4[3],
							Clsid.Data4[4],
							Clsid.Data4[5],
							Clsid.Data4[6],
							Clsid.Data4[7]);

			CRegKey rKey;
			if (rKey.Open(HKEY_CLASSES_ROOT, sKey, KEY_READ) == ERROR_SUCCESS)
			{
				DWORD Size = 256;
				String_256 sServer;
				if (rKey.QueryValue(sServer, _T(""), &Size) == ERROR_SUCCESS)
				{
					PathName Path(sServer);
					if (SGLibOil::FileExists(&Path))
					{
						PathNameListItem* pItem = new PathNameListItem(Path);
						if (pItem)
						{
							FilterList.AddTail(pItem);
//							RELTRACE(_T("Adding %s to list\n"), (LPCTSTR)Path.GetPath());
						}
					}
				}
			}
		}
	}
	
	// Scan through the Filters folder and register any DLL whose leafname isn't in FilterList
	String_256 sPath;
	GetModuleFileName(NULL, sPath, 256);
	PathName ExePath(sPath);
	sPath = ExePath.GetLocation();
	sPath += _T("Filters\\");
	String_256 sFindPath(sPath);
	sFindPath += _T("*.*");

	FindFiles Finder;
	BOOL bIsFolder = FALSE;
	if (Finder.StartFindingFiles(&sFindPath))
	{
		String_256 sLeafName;
		while (TRUE)
		{
			if (!Finder.FindNextFile(&sLeafName, &bIsFolder))
				break;

//			RELTRACE(_T("Testing %s\n"), (TCHAR*)sLeafName);

			String_256 sFullPath(sPath);
			sFullPath += sLeafName;

			if (bIsFolder)
			{
				sFullPath += _T("\\");
				sLeafName += _T(".dll");
				sFullPath += sLeafName;
			}

			// At this point sFullPath should point at the dll
			// Check it exists and is a DLL
			PathName TestPath(sFullPath);
			if (TestPath.GetType() == _T("dll") && SGLibOil::FileExists(&TestPath))
			{
//				RELTRACE(_T("FullPath %s\n"), (TCHAR*)sFullPath);

				// If the full path of the DLL is in the installed list then 
				// the correct version is already registered so do nothing.

				// If the leafname of the DLL is in the installed list then 
				// register the one in the Filters folder if it is newer than 
				// the one in the installed list (this will also trap the 
				// currently installed one not really existing).

				// If the leafname is not in the installed list then register it. 

				BOOL bRegister = TRUE;
				PathNameListItem* pItem = (PathNameListItem*)FilterList.GetHead();
				while (pItem)
				{
					if (sLeafName.CompareTo(pItem->m_Path.GetFileName(TRUE), FALSE) == 0)
					{
//						RELTRACE(_T("Leaf name is in list\n"));
						// Found the leafname in the list
						if (sFullPath.CompareTo(pItem->m_Path.GetPath(), FALSE) == 0)
						{
							// The full path is the same so do nothing
//							RELTRACE(_T("Full path is in list\n"));
							bRegister = FALSE;
						}
						else
						{
							// Get the version number of the installed version
							String_64 sVerInstalled;
							if (GetFileVersion(&(pItem->m_Path), &sVerInstalled))
							{
//								RELTRACE(_T("Current version is %s\n"), (TCHAR*)sVerInstalled);

								// Get the version number of the Filters version
								String_64 sVerFilters;
								PathName FiltersPath(sFullPath);
								if (GetFileVersion(&FiltersPath, &sVerFilters))
								{
//									RELTRACE(_T("This version is %s\n"), (TCHAR*)sVerFilters);
									// If the Installed version is greater than or equal
									// to the Filters version then don't register
									if (CompareVersions(sVerInstalled, sVerFilters) >= 0)
									{
//										RELTRACE(_T("Current is new enough\n"));
										bRegister = FALSE;
									}
								}
							}
						}
						break;
					}
					pItem = (PathNameListItem*)FilterList.GetNext(pItem);
				}

				if (bRegister)
				{
					// Register the DLL
					RELTRACE(_T("Registering %s\n"), (TCHAR*)sFullPath);
					BOOL bRegOk = RegisterDLL(sFullPath);
					RELTRACE(_T("RegisterDLL returned %s\n"), bRegOk ? _T("true") : _T("false"));
				}
			}
		}
		Finder.StopFindingFiles();
	}

	FilterList.DeleteAll();

	return(TRUE);
}



/****************************************************************************

>	static BOOL PluginOILFilter::RegisterDLL(String_256& sPath)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/2005

	Inputs:		sPath		- 
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL PluginOILFilter::RegisterDLL(String_256& sPath)
{
	// Pointer to reg server function
	INT32 (__stdcall *pDllRegisterServer)(void);

	// Other Vars
	HINSTANCE hThisCOM;
	INT32 hRegResult;

	// Load library into this process
	if (hThisCOM = LoadLibraryEx(sPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH))
	{
		// Get pointer to standard dll registry function
		pDllRegisterServer = GetProcAddress(hThisCOM, _T("DllRegisterServer"));
		if (!pDllRegisterServer)
		{
			return FALSE;
		}

		// Tell server to register itself
		hRegResult = pDllRegisterServer();
		if (FAILED(hRegResult))
		{
			return FALSE;
		}

		FreeLibrary(hThisCOM);
		return(TRUE);
	}

	return(FALSE);
}




/****************************************************************************

>	BOOL PluginOILFilter::GetFileVersion(PathName* pPath, String_64* pVersion)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/07/2005

	Inputs:		pPath		- pointer to a PathName
				pVersion	- pointer to a String_64 to recieve the version string
	Returns:	TRUE if ok, FALSE if version number couldn't be extracted
	Purpose:	Gets the version number (as a string) from a file's version 
				resource.

****************************************************************************/

BOOL PluginOILFilter::GetFileVersion(PathName* pPath, String_64* pVersion)
{
	BOOL bRetVal = FALSE;
	*pVersion = _T("0, 0, 0, 0");

	if (SGLibOil::FileExists(pPath))
	{
		String_256 sFilename(pPath->GetPath());
		if (sFilename.Length() <= 126)
		{
			DWORD dwVerHnd = 0;
			DWORD VersionSize = GetFileVersionInfoSize(sFilename, &dwVerHnd);
			// First get the size of the version info
			if (VersionSize > 0)
			{
				BYTE* pVersionInfo = new BYTE[VersionSize];

				// Now get the Version block from the dll!
				if (pVersionInfo && GetFileVersionInfo(sFilename, NULL, VersionSize, (void*)pVersionInfo))
				{
					struct LANGANDCODEPAGE
					{
						WORD wLanguage;
						WORD wCodePage;
					} *lpTranslate;
					UINT32 cbTranslate;

					// Read the list of languages and code pages.
					VerQueryValue(pVersionInfo, 
								  TEXT("\\VarFileInfo\\Translation"),
								  (LPVOID*)&lpTranslate,
								  &cbTranslate);
					
					UINT32 ValueLen;
					LPVOID pValue;
					TCHAR pValName[64];
					wsprintf(pValName, _T("\\StringFileInfo\\%04x%04x\\FileVersion"), lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
					// Now get the value from the block
					if (VerQueryValue((void*)pVersionInfo, pValName, &pValue, &ValueLen))
					{
						*pVersion = (TCHAR*)pValue;
						bRetVal = TRUE;
					}
					delete [] pVersionInfo;
				}
			}
		}
	}

	return(bRetVal);
}


/****************************************************************************

>	INT32 PluginOILFilter::CompareVersions(const String_64& lhs, const String_64& rhs)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/07/2005

	Inputs:		lhs			- a version string
				rhs			- another version string
	Returns:	integer indicating the comparison of the versions in the 
				standard C manner (-ve is lhs < rhs, 0 is lhs = rhs, 
				+ve is lhs > rhs)
	Purpose:	Compares two version strings

****************************************************************************/

INT32 PluginOILFilter::CompareVersions(const String_64& lhs, const String_64& rhs)
{
	INT32 la, lb, lc, ld;
	INT32 ra, rb, rc, rd;

	if (camSscanf(lhs, _T("%d, %d, %d, %d"), &la, &lb, &lc, &ld) != 4)
	{
		ASSERT(FALSE);
		return(0);
	}

	if (camSscanf(rhs, _T("%d, %d, %d, %d"), &ra, &rb, &rc, &rd) != 4)
	{
		ASSERT(FALSE);
		return(0);
	}

	if (la - ra)
		return(la - ra);

	if (lb - rb)
		return(lb - rb);

	if (lc - rc)
		return(lc - rc);

	return(ld - rd);
}
#endif



/********************************************************************************************

>	PluginOILFilter::PluginOILFilter()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	Sets up the list of filename extensions that this filter understands
				and handles connection to external plugin object

********************************************************************************************/

PluginOILFilter::PluginOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	m_bImport = FALSE;
	m_bExport = FALSE;
}




/****************************************************************************

>	BOOL PluginOILFilter::Init(const CLSID& rCLSID)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/02/2005

	Inputs:		rCLSID		- 
	Returns:	TRUE if ok, FALSE if we find a problem and shouldn't use it
	Purpose:	Initialises the OIL specific parts of this filter (file 
				extension, file type name, link to COM object etc).

****************************************************************************/

BOOL PluginOILFilter::Init( xmlNode* pFilterNode )
{
PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
	// Remember the CLSID for later use
	m_CLSID = rCLSID;
	
	// Generate the relevant registy key name
	TCHAR sClassKey[64];
	wsprintf(sClassKey, _T("CLSID\\{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}"),
					m_CLSID.Data1,
					m_CLSID.Data2,
					m_CLSID.Data3,
					m_CLSID.Data4[0],
					m_CLSID.Data4[1],
					m_CLSID.Data4[2],
					m_CLSID.Data4[3],
					m_CLSID.Data4[4],
					m_CLSID.Data4[5],
					m_CLSID.Data4[6],
					m_CLSID.Data4[7]);

	CRegKey rClassKey;
	if (rClassKey.Open(HKEY_CLASSES_ROOT, sClassKey, KEY_READ) != ERROR_SUCCESS)
		return(FALSE);

	CRegKey rServerKey;
	if (rServerKey.Open(rClassKey, _T("InProcServer32"), KEY_READ) != ERROR_SUCCESS)
		return(FALSE);

	DWORD Size = _MAX_PATH;
	TCHAR ServerPath[_MAX_PATH];
	if (rServerKey.QueryValue(ServerPath, _T(""), &Size) != ERROR_SUCCESS)
		return(FALSE);

	if (GetFileAttributes(ServerPath) == INVALID_FILE_ATTRIBUTES)
	{
		DWORD Err = GetLastError();
		RELTRACE(_T("Error accessing filter at %s (%d)\n"), ServerPath, Err);
		return(FALSE);
	}

	CRegKey rConfigKey;
	if (rConfigKey.Open(rClassKey, _T("Config"), KEY_READ) != ERROR_SUCCESS)
		return(FALSE);
#endif

	m_FilterPath.SetPathName(_T("/home/gerry/src/XPFilter/debugu/XPFilter"));

	// This should be set to some sensible path but I've hardcoded it for now
	// We should change to making ~/.XaraLX into a directory and store the main config
	// file and these filter config files in there
	m_XMLFile.SetPathName(_T("/home/gerry/.XPFilters/XPFilter.xml"));

//	Size = 32;
//	TCHAR Exts[32];
//	if (rConfigKey.QueryValue(Exts, _T("Extensions"), &Size) != ERROR_SUCCESS)
//		return(FALSE);

	TCHAR Exts[32] = _T("*.xar");

	// We have to convert the plugin extension format into the XaraX format
	TCHAR* pTokTemp = NULL;
	TCHAR* pExt = camStrtok(Exts, _T(";"), &pTokTemp);
	BOOL NoneFound = TRUE;
	FilterExt = _T("");
	while (pExt != NULL)
	{
		if (pExt[0] == _T('*') && pExt[1] == _T('.'))
		{
			pExt += 2;
			if (camStrstr((TCHAR *)FilterExt, pExt) == NULL)
			{
				// Not already present - add the string.
				if (!NoneFound)
					// Don't add a comma if this is the first one we find.
					FilterExt += _T(",");
				FilterExt += pExt;
				NoneFound = FALSE;
			}
		}

		pExt = camStrtok(NULL, _T(";"), &pTokTemp);
	}

//	Size = 64;
//	TCHAR TypeName[64];
//	if (rConfigKey.QueryValue(TypeName, _T("TypeName"), &Size) != ERROR_SUCCESS)
//		return(FALSE);
//	FilterName = TypeName;

	FilterName = _T("Plugin Test");

#if !defined(EXCLUDE_FROM_XARALX)
	CRegKey rCatKey;
	if (rCatKey.Open(rClassKey, T("Implemented Categories"), KEY_READ) != ERROR_SUCCESS)
		return(FALSE);

	CRegKey rImpKey;
	m_bImport = (rImpKey.Open(rCatKey, _T("{42F818E1-9EF6-4241-909B-91E783B9B935}"), KEY_READ) == ERROR_SUCCESS) ||
				(rImpKey.Open(rCatKey, _T("{42F818E1-9EF6-4241-909B-91E783B9B937}"), KEY_READ) == ERROR_SUCCESS);

	CRegKey rExpKey;
	m_bExport = (rExpKey.Open(rCatKey, _T("{42F818E1-9EF6-4241-909B-91E783B9B936}"), KEY_READ) == ERROR_SUCCESS) ||
				(rExpKey.Open(rCatKey, _T("{42F818E1-9EF6-4241-909B-91E783B9B938}"), KEY_READ) == ERROR_SUCCESS);
#endif	

	m_bImport = TRUE;
	m_bExport = TRUE;

	return(m_bImport || m_bExport);		// If it doesn't do either then it isn't a filter
}



/****************************************************************************

>	INT32 PluginOILFilter::HowCompatible(PathName& Filename)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/02/2005

	Inputs:		Filename	- The file to test compatability of
	Returns:	An integer from 0 to 10, 0 meaning not interested and 10 
				meaning it is definitely my format
	Purpose:	This function is called to determine if this plugin object 
				can accept the specified file.

****************************************************************************/

INT32 PluginOILFilter::HowCompatible(PathName& FileName)
{
	INT32 HowCompatible = 0;

PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
	if (!CreateFilterObject())
		return(0);

	CComBSTR bsFileName((LPCTSTR)Filename.GetPath());
	HRESULT hRes = m_pFilterObj->HowCompatible(bsFileName, &HowCompatible);
	if (FAILED(hRes))
		return(0);
#endif

	// Here we need to run the plugin synchronously with the following options
	// -c -f <filename>

	// Check stderr for errors
	// Get HowCompatible from stdout

	wxString sCommand;
	// Does this need double quotes to cope with spaces in filenames?
	sCommand.Printf(_T("%s -c -f %s"), (LPCTSTR)m_FilterPath.GetPath(), (LPCTSTR)FileName.GetPath());

	TRACE(_T("Running '%s'"), sCommand.c_str());

	wxArrayString saOutput;
	wxArrayString saErrors;
	int code = wxExecute(sCommand, saOutput, saErrors);
	TRACE(_T("wxExecute returned %d"), code);
	if (code == 0)
	{
		// Extract the value from saOutput
		if (saOutput.Count() > 0)
		{
			INT32 Val = wxAtoi(saOutput[0]);
			TRACE(_T("Command '%s' returned string '%s'"), sCommand.c_str(), saOutput[0].c_str());
			TRACE(_T("Value = %d"), Val);
			if (Val >= 0 && Val <= 10)
			{
				HowCompatible = Val;
			}
		}
		else
		{
			TRACE(_T("Command '%s' returned no output value"), sCommand.c_str());
		}
	}
	else
	{
		TRACE(_T("Command '%s' exited with code %d"), sCommand.c_str(), code);
	}

	return(HowCompatible);
}




/****************************************************************************

>	BOOL PluginOILFilter::GetImportFile(CCLexFile* pFile, CCLexFile** ppNewFile)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/02/2005

	Inputs:		pFile		- pointer to a CCLexFile
				ppNewFile	- pointer to a pointer to a CCLexFile
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL PluginOILFilter::GetImportFile(CCLexFile* pFile, CCLexFile** ppNewFile)
{
	ERROR2IF(pFile == NULL, FALSE,"PluginOILFilter::GetImportFile no file to import from");
	ERROR2IF(ppNewFile == NULL, FALSE,"PluginOILFilter::GetImportFile no newfile pointer");

	*ppNewFile = NULL;

PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
	ERROR2IF(!m_pFilterObj, FALSE, "No filter object in GetImportFile");

	if (m_pXarStream)
		m_pXarStream.Release();

	// Create an IStream for the flare data...
	HRESULT hRes = CreateStreamOnHGlobal(NULL, FALSE, &m_pXarStream);
	if (FAILED(hRes))
	{
		ERROR1(FALSE, _R(IDE_XPF_CREATESTREAMFAILED));
	}

	// Setup a callback object to handle progress
	// TODOG: Pass something useful to callback object
	PluginFilterCallback* pCallback = new PluginFilterCallback((PluginNativeFilter*)Parent);

	// Call DoImport
	PathName FileName = pFile->GetPathName();
	CComBSTR bsFileName(FileName.GetPath());
	hRes = m_pFilterObj->DoImport(bsFileName, m_pXarStream, pCallback->GetInterface());

	// Delete the callback object
	delete pCallback;

	if (FAILED(hRes))
	{
		ERROR1(FALSE, _R(IDE_XPF_DOIMPORTFAILED));
	}

	CCOleStream* pStreamFile = new CCOleStream;
	if (!pStreamFile)
	{
		ERROR1(FALSE, _R(IDE_NOMORE_MEMORY));
	}

	LARGE_INTEGER Pos = {0};
	m_pXarStream->Seek(Pos, STREAM_SEEK_SET, NULL);

	pStreamFile->attach(m_pXarStream);	// Attach to our stream

	m_pXarStream.p->AddRef();

	*ppNewFile = pStreamFile;		// Return the new file pointer

	return(TRUE);
#endif

	// Here we should really run the plugin asynchronously with the following options
	// -i -g -f <filename>

	// Redirect stdout to a CCLexFile
	// Check stderr during the Xar import and abort if an error is reported

	m_TempXarFile.SetPathName(_T("/tmp/xpftemp.xar"));

	CCDiskFile TempFile;
	if (!TempFile.open(m_TempXarFile, ios::out | ios::trunc | ios::binary))
	{
		// report an error here
		return FALSE;
	}

	PathName FileName = pFile->GetPathName();

	wxString sCommand;
	// Does this need double quotes to cope with spaces in filenames?
	sCommand.Printf(_T("%s -i -f %s"), (LPCTSTR)m_FilterPath.GetPath(), (LPCTSTR)FileName.GetPath());

	// Create a process with the TempFile as the stdout
	// We will need to derive a new class from CamProcess to handle
	// the processing of stderr for errors, warnings and progress
	CamProcess TheProc(NULL, &TempFile);

	int code = TheProc.Execute(sCommand);
	TempFile.close();
	if (code != 0)
	{
		TRACE(_T("Execution of '%s' failed."), sCommand.c_str());
		// Extract error from a derived CamProcess class and report it
		return(FALSE);
	}

	CCDiskFile* pTempFile = new CCDiskFile();
	if (pTempFile)
	{
		if (pTempFile->open(m_TempXarFile, ios::in | ios::binary))
		{
			*ppNewFile = pTempFile;
			return(TRUE);
		}

		delete pTempFile;
	}

	return(FALSE);
}



/****************************************************************************

>	BOOL PluginOILFilter::GetExportFile(CCLexFile** ppNewFile)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/2005

	Inputs:		ppNewFile	- pointer to a pointer to a CCLexFile
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL PluginOILFilter::GetExportFile(PathName* pPath, CCLexFile** ppNewFile)
{
	ERROR2IF(ppNewFile == NULL, FALSE,"PluginOILFilter::GetExportFile no newfile pointer");

	*ppNewFile = NULL;

	// Here we should run the plugin asynchronously with the following options
	// -e -g -f <filename> -x <xmlfilename>

	// The xmlfilename is a path to a user and filter specific file
	// e.g. ~/.XaraLX/filtername.xml
	// Create a CCLexFile derived object that sends its data to stdin
	// Check stderr during the Xar export and abort if an error is reported

	// However, this will not be trivial so intsead we will just create a 
	// CCDiskFile attached to a temporary filename and run the export process
	// in DoExport instead

	// Generate a temporary file name
	m_TempXarFile.SetPathName(_T("/tmp/xpftemp.xar"));

	CCDiskFile* pFile = new CCDiskFile();
	if (pFile)
	{
		if (pFile->open(m_TempXarFile, ios::out | ios::binary | ios::trunc))
		{
			*ppNewFile = pFile;
			return(TRUE);
		}

		delete pFile;
	}

	return(FALSE);

PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
	if (!CreateFilterObject())
	{
		return(FALSE);
	}

	if (m_pXarStream)
		m_pXarStream.Release();

	// Create an IStream for the flare data...
	HRESULT hRes = CreateStreamOnHGlobal(NULL, FALSE, &m_pXarStream);
	if (FAILED(hRes))
	{
		ERROR1(FALSE, _R(IDE_XPF_CREATESTREAMFAILED));
	}
		
	CCOleStream* pStreamFile = new CCOleStream(m_pXarStream);
	if (!pStreamFile)
	{
		ERROR1(FALSE, _R(IDE_NOMORE_MEMORY));
	}

	m_pXarStream.p->AddRef();

	*ppNewFile = pStreamFile;		// Return the new file pointer

	return(TRUE);
#endif
}



/****************************************************************************

>	BOOL PluginOILFilter::GetCapabilities(CCLexFile* pFile, CapabilityTree* pCapTree)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/2005

	Inputs:		pPlugCaps	- pointer to a PluginCapabilities
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL PluginOILFilter::GetCapabilities(CCLexFile* pFile, PathName* pPath, CapabilityTree* pCapTree)
{
PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
	ERROR2IF(!m_pFilterObj, FALSE, "No filter object");

	String_256 sPath = pPath->GetPath();
	ERROR2IF(sPath.IsEmpty(), FALSE, "No filename in GetCapabilities");

	CComBSTR bsFileName(sPath);
	CComBSTR bsXML;
	// Call the filter object to do the conversion
	HRESULT hRes = m_pFilterObj->PrepareExport(bsFileName, &bsXML);
	if (FAILED(hRes))
	{
		Error::SetHResultError(hRes, m_pFilterObj, __uuidof(IXPFilter));
		return(FALSE);
	}
	if (hRes == S_FALSE)
	{
		// User has cancelled the export so return the special error that will be ignored
		Error::SetError(_R(IDN_USER_CANCELLED), 0);
		return(FALSE);
	}

	// Now we build the capabilities structure from the XML
	hRes = BuildCapabilityTree(bsXML, pCapTree);
	if (FAILED(hRes))
	{
		Error::SetHResultError(hRes);
		return(FALSE);
	}
#endif

	// Here we need to run the plugin synchronously with the following options
	// -p -f <filename> -x <xmlfilename>

	// The xmlfilename is a path to a user and filter specific file
	// e.g. ~/.XaraLX/filtername.xml

	// The XML is returned via the file

	// Does this need double quotes to cope with spaces in filenames?
	wxString sCommand;
	sCommand.Printf(_T("%s -p -f %s -x %s"), (LPCTSTR)m_FilterPath.GetPath(), (LPCTSTR)pPath->GetPath(), (LPCTSTR)m_XMLFile.GetPath());

	wxArrayString saOutput;
	wxArrayString saErrors;
	int code = wxExecute(sCommand, saOutput, saErrors);

	for (UINT32 i = 0; i < saErrors.GetCount(); i++)
	{
		TRACE(_T("stderr: %s"), saErrors[i].c_str());
	}

	if (code == 0)
	{
		BuildCapabilityTree(m_XMLFile.GetPath(), pCapTree);
	}
	else
	{
		TRACE(_T("Command '%s' exited with code %d"), sCommand.c_str(), code);

		// Get error message from saErrors
		return(FALSE);
	}

	return(TRUE);
}



/****************************************************************************

>	BOOL PluginOILFilter::DoExport(CCLexFile* pXarFile, PathName* pPath)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/2005

	Inputs:		pXarFile	- pointer to a CCLexFile
				pPath		- pointer to a PathName
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL PluginOILFilter::DoExport(CCLexFile* pXarFile, PathName* pPath)
{
PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
	ERROR2IF(!m_pFilterObj, FALSE, "No filter object\n");

	// Reset the stream back to the start ready for the filter to translate it
	pXarFile->seek(0);

	CComBSTR bsFileName(pPath->GetPath());

	// Should probably check the filename here and give a specific error

	// Setup a callback object to handle progress
	// TODOG: Pass something useful to callback object
	PluginFilterCallback* pCallback = new PluginFilterCallback((PluginNativeFilter*)Parent);

	// Call the filter object to do the conversion
	HRESULT hRes = m_pFilterObj->DoExport(bsFileName, m_pXarStream, pCallback->GetInterface());

	// Delete the callback object
	delete pCallback;

	if (FAILED(hRes))
	{
		ERROR1(FALSE, _R(IDE_XPF_DOEXPORTFAILED));
	}
#endif

	// Here we should just need to wait for the process started in GetExportFile 
	// to finish
	// Check stderr for errors and progress

	// However for now we will instead
	// Run the plugin with the following options
	// -e -g -f <filename> -x <xmlfilename>

	// The xmlfilename is a path to a user and filter specific file
	// e.g. ~/.XaraLX/filtername.xml

	// Check stderr for errors

	wxString sCommand;
	// Does this need double quotes to cope with spaces in filenames?
	sCommand.Printf(_T("%s -e -f %s -x %s"), (LPCTSTR)m_FilterPath.GetPath(), (LPCTSTR)pPath->GetPath(), (LPCTSTR)m_XMLFile.GetPath());

	CCDiskFile TempFile(CCFILE_DEFAULTSIZE, FALSE, FALSE);
	if (!TempFile.open(m_TempXarFile, ios::in | ios::binary))
	{
		// report an error here
		return FALSE;
	}

	// Create a process with the TempFile as the stdin
	// We will need to derive a new class from CamProcess to handle
	// the processing of stderr for errors, warnings and progress
	CamProcess TheProc(&TempFile, NULL);

	int code = TheProc.Execute(sCommand);
	TempFile.close();
	if (code != 0)
	{
		TRACE(_T("Execution of '%s' failed."), sCommand.c_str());
		// Extract error from a derived CamProcess class and report it
		return(FALSE);
	}

	return(TRUE);
}


PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
/****************************************************************************

>	BOOL PluginOILFilter::CreateFilterObject()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/2005

	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL PluginOILFilter::CreateFilterObject()
{
	if (!m_pFilterObj)
	{
		HRESULT hRes = m_pFilterObj.CoCreateInstance(m_CLSID);
		if (FAILED(hRes))
		{
			ERROR1_MSG(FALSE, (_R(IDE_XPF_CREATEFILTERFAILED), &(((PluginNativeFilter*)Parent)->FilterName), hRes));
		}
	}
	return(TRUE);
}


/****************************************************************************

>	void PluginOILFilter::ReleaseFilterObject()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/2005

	Purpose:	

****************************************************************************/

void PluginOILFilter::ReleaseFilterObject()
{
	m_pFilterObj.Release();
	m_pXarStream.Release();
}
#endif


/****************************************************************************

>	BOOL PluginOILFilter::BuildCapabilityTree(wxString strXmlFilename, CapabilityTree* pCapTree)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/2005

	Inputs:		bsXML		- The XML to load
				pCapTree	- pointer to a CapabilityTree to be filled in
	Returns:	Standard HRESULT
	Purpose:	Populates a CapabilityTree object with the XML

****************************************************************************/

/*HRESULT PluginOILFilter::BuildCapabilityTree(BSTR bsXML, CapabilityTree* pCapTree)
{
	// First we need to load the XML into an XML DOM object
	CComPtr<MSXML2::IXMLDOMDocument2> pDoc;
	
	HRESULT hRes = S_OK;
	hRes = pDoc.CoCreateInstance(__uuidof(MSXML2::DOMDocument40));

	doc = xmlNewDocument();
	hRes = pDoc->setProperty(CComBSTR(_T("SelectionLanguage")), CComVariant(_T("XPath")));
	hRes = pDoc->put_async(VARIANT_FALSE);
	hRes = pDoc->put_preserveWhiteSpace(VARIANT_TRUE);
	hRes = pDoc->put_validateOnParse(VARIANT_FALSE);
	hRes = pDoc->put_resolveExternals(VARIANT_FALSE);

	VARIANT_BOOL bSuccess = VARIANT_FALSE;
	hRes = pDoc->loadXML(bsXML, &bSuccess);
	if (bSuccess == VARIANT_FALSE)
	{
		ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
	}

	// Check if the root element is correct
	CComPtr<IXMLDOMElement> pRootElem;
	hRes = pDoc->get_documentElement(&pRootElem);

	CComBSTR bsName;
	hRes = pRootElem->get_nodeName(&bsName);

	if (!(bsName == _T("XPFilterConfig")))
	{
		ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
	}

	// There are 5 phases to the parsing
	// We will loop round until we run out of child elements
	// After parsing a node the phase counter will be set to the phase just parsed
	// If an element should have already been parsed (using the phase counter)
	// then an error will be indicated

	CComPtr<IXMLDOMNode> pChild;
	CComPtr<IXMLDOMElement> pChildElem;
	hRes = pRootElem->get_firstChild(&pChild);
	INT32 Phase = 0;

	while (pChild)
	{
		CComBSTR bsChildName;
		hRes = pChild->get_nodeName(&bsChildName);
		
		if (bsChildName == _T("#text"))
		{
			// ignore it
		}
		else if (bsChildName == _T("Options"))
		{
			if (Phase > 0)
			{
				ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
			}
			hRes = ReadOptions(pChild, pCapTree);		// Read the options attributes
			Phase = 1;
		}
		else if (bsChildName == _T("Rasterise"))
		{
			if (Phase > 1)
			{
				ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
			}
			hRes = ReadRasterise(pChild, pCapTree);		// Read the dpi and alpha attributes
			Phase = 2;
		}
		else if (bsChildName == _T("Spread"))
		{
			if (Phase > 2)
			{
				ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
			}
			hRes = ReadSpread(pChild, pCapTree);		// Read the as attribute
			Phase = 3;
		}
		else if (bsChildName == _T("Objects"))
		{
			if (Phase > 3)
			{
				ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
			}
			ReadObjects(pChild, pCapTree);		// Build the tree of XPFCapability derived objects
			Phase = 4;
		}
		else if (bsChildName == _T("Attributes"))
		{
			if (Phase > 4)
			{
				ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
			}
			ReadAttributes(pChild, pCapTree);	// Build the tree of XPFCapability derived objects
			Phase = 5;
		}
		else if (bsChildName == _T("Colour"))
		{
			if (Phase > 5)
			{
				ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
			}
			ReadColour(pChild, pCapTree);		// Build the tree of XPFColour objects
			Phase = 6;
		}
		else
		{
			ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
		}

		CComPtr<IXMLDOMNode> pNextChild;
		hRes = pChild->get_nextSibling(&pNextChild);
		pChild = pNextChild;
	}
	return S_OK;
}*/


BOOL PluginOILFilter::BuildCapabilityTree(wxString strFilename, CapabilityTree* pCapTree)
{

	// First we need to load the XML into an XML DOM object

	// Set flags in Parser context here?

	BOOL bOK = TRUE;
    xmlDocPtr doc;

	// If string param contains xml (like original Windows version)
//	wxCharBuffer buf = strXML.mb_str(wxConvUTF8);
//	doc = xmlParseDoc((const xmlChar*)buf.data());	// buf will be deallocated when it goes out of scope

	// If string param gives xml filename (like new LX version)
	wxCharBuffer buf = strFilename.ToAscii();
	doc = xmlParseFile(buf.data());					// buf will be deallocated when it goes out of scope
#if _DEBUG
	if (doc==NULL)
		doc = xmlParseFile("/tmp/XaraLX/capstest.xml");
#endif
	ERROR1IF(doc==NULL, FALSE, _R(IDE_XPF_BADXML));

	// The name of the root element should be XPFilterConfig

	xmlNodePtr node = xmlDocGetRootElement(doc);
	wxString strName = CXMLUtils::ConvertToWXString(node->name);
	if (strName!=_T("XPFilterConfig"))
		ERROR1(FALSE, _R(IDE_XPF_BADXML));

	xmlNodePtr pChild = node->children;
	INT32 Phase =0;

	// There are 5 phases to the parsing
	// We will loop round until we run out of child elements
	// After parsing a node the phase counter will be set to the phase just parsed
	// If an element should have already been parsed (using the phase counter)
	// then an error will be indicated

	while (pChild && bOK)
	{
		wxString strChildName = CXMLUtils::ConvertToWXString(pChild->name);
		
		if (strChildName == _T("#text") || xmlNodeIsText(pChild))
		{
			// ignore it
		}
		else if (strChildName == _T("Options"))
		{
			if (Phase > 0)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML));
			}
			bOK = ReadOptions(pChild, pCapTree);		// Read the options attributes
			Phase = 1;
		}
		else if (strChildName == _T("Rasterise"))
		{
			if (Phase > 1)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML));
			}
			bOK = ReadRasterise(pChild, pCapTree);		// Read the dpi and alpha attributes
			Phase = 2;
		}
		else if (strChildName == _T("Spread"))
		{
			if (Phase > 2)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML));
			}
			bOK = ReadSpread(pChild, pCapTree);		// Read the as attribute
			Phase = 3;
		}
		else if (strChildName == _T("Objects"))
		{
			if (Phase > 3)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML));
			}
			bOK = ReadObjects(pChild, pCapTree);		// Build the tree of XPFCapability derived objects
			Phase = 4;
		}
		else if (strChildName == _T("Attributes"))
		{
			if (Phase > 4)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML));
			}
			bOK = ReadAttributes(pChild, pCapTree);	// Build the tree of XPFCapability derived objects
			Phase = 5;
		}
		else if (strChildName == _T("Colour"))
		{
			if (Phase > 5)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML));
			}
			bOK = ReadColour(pChild, pCapTree);		// Build the tree of XPFColour objects
			Phase = 6;
		}
		else
		{
			ERROR1(FALSE, _R(IDE_XPF_BADXML));
		}

		pChild = pChild->next;
	}


	return bOK;
}


/*HRESULT PluginOILFilter::ReadOptions(IXMLDOMNode* pNode, CapabilityTree* pCapTree)
{
	CComQIPtr<IXMLDOMElement> pElem;
	pElem = pNode;
	if (!pElem)
	{
		ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
	}

	HRESULT hRes;
	BoundsWriteLevel Level = BWL_NONE;
	CComVariant vLevel;
	hRes = pElem->getAttribute(CComBSTR(_T("boundslevel")), &vLevel);
	if (SUCCEEDED(hRes))
	{
		hRes = vLevel.ChangeType(VT_BSTR);
		if (vLevel == CComVariant(_T("none")))
		{
			Level = BWL_NONE;
		}
		else if (vLevel == CComVariant(_T("compound")))
		{
			Level = BWL_COMPOUND;
		}
		else if (vLevel == CComVariant(_T("all")))
		{
			Level = BWL_ALL;
		}
		else
		{
			ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
		}
	}
	pCapTree->SetBoundsLevel(Level);

	BOOL bSelection = FALSE;
	CComVariant vSelection;
	hRes = pElem->getAttribute(CComBSTR(_T("selection")), &vSelection);
	if (SUCCEEDED(hRes))
	{
		hRes = vSelection.ChangeType(VT_BSTR);
		bSelection = (vSelection == CComVariant(_T("true")));
	}
	pCapTree->SetSelection(bSelection);

	return(S_OK);
}*/


BOOL PluginOILFilter::ReadOptions(xmlNodePtr pNode, CapabilityTree* pCapTree)
{
	BoundsWriteLevel Level = BWL_NONE;

	wxString strLevel = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"boundslevel"));
	if (strLevel == _T("none"))
	{
		Level = BWL_NONE;
	}
	else if (strLevel == _T("compound"))
	{
		Level = BWL_COMPOUND;
	}
	else if (strLevel == _T("all"))
	{
		Level = BWL_ALL;
	}
	else
	{
		ERROR1(FALSE, _R(IDE_XPF_BADXML));
	}

	pCapTree->SetBoundsLevel(Level);

	BOOL bSelection = FALSE;
	wxString strSelection = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"selection"));
	bSelection = (strSelection == _T("true"));

	pCapTree->SetSelection(bSelection);

	return TRUE;
}


/*HRESULT PluginOILFilter::ReadRasterise(IXMLDOMNode* pNode, CapabilityTree* pCapTree)
{
	CComQIPtr<IXMLDOMElement> pElem;
	pElem = pNode;
	if (!pElem)
	{
		ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
	}

	HRESULT hRes;
	double DPI = 96.0;
	BOOL bAlpha = TRUE;

	CComVariant vDPI;
	hRes = pElem->getAttribute(CComBSTR(_T("dpi")), &vDPI);
	if (SUCCEEDED(hRes))
	{
		hRes = vDPI.ChangeType(VT_R8);
		DPI = vDPI.dblVal;
	}

	CComVariant vAlpha;
	hRes = pElem->getAttribute(CComBSTR(_T("alpha")), &vAlpha);
	if (SUCCEEDED(hRes))
	{
		hRes = vAlpha.ChangeType(VT_BSTR);
		bAlpha = (vAlpha == CComVariant(_T("true")));
	}

	pCapTree->SetRasterise(DPI, bAlpha);

	return(S_OK);
}
*/


BOOL PluginOILFilter::ReadRasterise(xmlNodePtr pNode, CapabilityTree* pCapTree)
{
	double DPI = 96.0;
	BOOL bAlpha = TRUE;
	wxString str;

	str = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"dpi"));
	if (!str.IsEmpty())
	{
		str.ToDouble(&DPI);
	}

	str = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"alpha"));
	if (!str.IsEmpty())
	{
		bAlpha = (str == _T("true"));
	}

	pCapTree->SetRasterise(DPI, bAlpha);

	return TRUE;
}


/*HRESULT PluginOILFilter::ReadSpread(IXMLDOMNode* pNode, CapabilityTree* pCapTree)
{
	CComQIPtr<IXMLDOMElement> pElem(pNode);
	if (!pElem)
	{
		ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
	}

	HRESULT hRes;

	CComVariant vAs;
	XPFConvertType Type = XPFCONVTYPE_UNKNOWN;
	hRes = pElem->getAttribute(CComBSTR(_T("as")), &vAs);
	if (SUCCEEDED(hRes))
	{
		hRes = vAs.ChangeType(VT_BSTR);
		if (vAs == CComVariant(_T("")))
		{
			Type = XPFCONVTYPE_NATIVE;
		}
		else if (vAs == CComVariant(_T("bitmap")))
		{
			Type = XPFCONVTYPE_BITMAP;
		}
		else
		{
			ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
		}
	}

	if (Type != XPFCONVTYPE_UNKNOWN)
		pCapTree->SetSpreadType(Type);

	return(S_OK);
}*/


BOOL PluginOILFilter::ReadSpread(xmlNodePtr pNode, CapabilityTree* pCapTree)
{
	XPFConvertType Type = XPFCONVTYPE_UNKNOWN;
	wxString str = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"as"));
	if (str.IsEmpty())
	{
		Type = XPFCONVTYPE_NATIVE;
	}
	else if (str == _T("bitmap"))
	{
		Type = XPFCONVTYPE_BITMAP;
	}
	else
	{
		ERROR1(FALSE, _R(IDE_XPF_BADXML));
	}

	if (Type != XPFCONVTYPE_UNKNOWN)
		pCapTree->SetSpreadType(Type);

	return TRUE;
}


/*HRESULT PluginOILFilter::ReadObjects(IXMLDOMNode* pNode, CapabilityTree* pCapTree)
{
	// We must loop through the tree of elements

	// pNode is the Objects element so read the default as attribute

	XPFCapability* pObjects = NULL;
	XPFConvertType ObjectsType = XPFCONVTYPE_UNKNOWN;

	HRESULT hRes;
	hRes = GetConvertAsType(pNode, &ObjectsType);

	// Loop through each child calling the CreateObjectNode recursive function
	// for each one

	CComPtr<IXMLDOMNode> pChild;
	CComPtr<IXMLDOMElement> pChildElem;
	hRes = pNode->get_firstChild(&pChild);
	INT32 Phase = 0;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCap = CreateObjectNode(pChild);

		if (pCap)
		{
			// If we have a capability node then add it to the list
			// If we do not have a node already then set pObjects
			if (pLast)
			{
				pLast->SetNext(pCap);
			}
			else
			{
				pObjects = pCap;
			}
			pLast = pCap;
		}

		CComPtr<IXMLDOMNode> pNextChild;
		hRes = pChild->get_nextSibling(&pNextChild);
		pChild = pNextChild;
	}

	pCapTree->SetObjectsTree(pObjects, ObjectsType);

	return(S_OK);
}*/


BOOL PluginOILFilter::ReadObjects(xmlNodePtr pNode, CapabilityTree* pCapTree)
{
	// We must loop through the tree of elements

	// pNode is the Objects element so read the default as attribute

	XPFCapability* pObjects = NULL;
	XPFConvertType ObjectsType = XPFCONVTYPE_UNKNOWN;

	HRESULT hRes;
	hRes = GetConvertAsType(pNode, &ObjectsType);

	// Loop through each child calling the CreateObjectNode recursive function
	// for each one

	xmlNodePtr pChild;
	pChild = pNode->children;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCap = CreateObjectNode(pChild);

		if (pCap)
		{
			// If we have a capability node then add it to the list
			// If we do not have a node already then set pObjects
			if (pLast)
			{
				pLast->SetNext(pCap);
			}
			else
			{
				pObjects = pCap;
			}
			pLast = pCap;
		}

		pChild = pChild->next;
	}

	pCapTree->SetObjectsTree(pObjects, ObjectsType);

	return TRUE;
}


/*HRESULT PluginOILFilter::ReadAttributes(IXMLDOMNode* pNode, CapabilityTree* pCapTree)
{
	// We must loop through the tree of elements

	// pNode is the Attributes element so read the default as attribute

	XPFCapability* pAttrs = NULL;
	XPFConvertType AttrType = XPFCONVTYPE_UNKNOWN;

	HRESULT hRes;
	hRes = GetConvertAsType(pNode, &AttrType);

	// Loop through each child calling the CreateAttributeNode recursive function
	// for each one

	CComPtr<IXMLDOMNode> pChild;
	CComPtr<IXMLDOMElement> pChildElem;
	hRes = pNode->get_firstChild(&pChild);
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCap = CreateAttributeNode(pChild);

		if (pCap)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCap);
			}
			else
			{
				pAttrs = pCap;
			}
			pLast = pCap;
		}

		CComPtr<IXMLDOMNode> pNextChild;
		hRes = pChild->get_nextSibling(&pNextChild);
		pChild = pNextChild;
	}

	pCapTree->SetAttributesTree(pAttrs, AttrType);

	return(S_OK);
}*/


BOOL PluginOILFilter::ReadAttributes(xmlNodePtr pNode, CapabilityTree* pCapTree)
{
	// We must loop through the tree of elements

	// pNode is the Attributes element so read the default as attribute

	XPFCapability* pAttrs = NULL;
	XPFConvertType AttrType = XPFCONVTYPE_UNKNOWN;

	BOOL bOK = GetConvertAsType(pNode, &AttrType);
	if (!bOK)
		return FALSE;

	// Loop through each child calling the CreateAttributeNode recursive function
	// for each one

	xmlNodePtr pChild;
	pChild = pNode->children;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCap = CreateAttributeNode(pChild);

		if (pCap)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCap);
			}
			else
			{
				pAttrs = pCap;
			}
			pLast = pCap;
		}

		pChild = pChild->next;
	}

	pCapTree->SetAttributesTree(pAttrs, AttrType);

	return TRUE;
}


/*HRESULT PluginOILFilter::ReadColour(IXMLDOMNode* pNode, CapabilityTree* pCapTree)
{
	// We must loop through the tree of elements

	// pNode is the Attributes element so read the default as attribute

	XPFCapability* pCols = NULL;
	XPFConvertType ColType = XPFCONVTYPE_UNKNOWN;

	HRESULT hRes;
	hRes = GetConvertAsType(pNode, &ColType);

	// Loop through each child calling the CreateColourNode recursive function
	// for each one

	CComPtr<IXMLDOMNode> pChild;
	CComPtr<IXMLDOMElement> pChildElem;
	hRes = pNode->get_firstChild(&pChild);
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCap = CreateColourNode(pChild);

		if (pCap)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCap);
			}
			else
			{
				pCols = pCap;
			}
			pLast = pCap;
		}

		CComPtr<IXMLDOMNode> pNextChild;
		hRes = pChild->get_nextSibling(&pNextChild);
		pChild = pNextChild;
	}

	pCapTree->SetColoursTree(pCols, ColType);

	return(S_OK);
}*/


BOOL PluginOILFilter::ReadColour(xmlNodePtr pNode, CapabilityTree* pCapTree)
{
	// We must loop through the tree of elements

	// pNode is the Attributes element so read the default as attribute

	XPFCapability* pCols = NULL;
	XPFConvertType ColType = XPFCONVTYPE_UNKNOWN;

	BOOL bOK = GetConvertAsType(pNode, &ColType);
	if (!bOK)
		return FALSE;

	// Loop through each child calling the CreateColourNode recursive function
	// for each one

	xmlNodePtr pChild;
	pChild = pNode->children;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCap = CreateColourNode(pChild);

		if (pCap)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCap);
			}
			else
			{
				pCols = pCap;
			}
			pLast = pCap;
		}

		pChild = pChild->next;
	}

	pCapTree->SetColoursTree(pCols, ColType);

	return TRUE;
}


/*HRESULT PluginOILFilter::GetConvertAsType(IXMLDOMNode* pNode, XPFConvertType* pValue)
{
	XPFConvertType AsType = XPFCONVTYPE_UNKNOWN;

	CComQIPtr<IXMLDOMElement> pElem(pNode);
	if (pElem)
	{
		HRESULT hRes;

		CComVariant vAs;
		hRes = pElem->getAttribute(CComBSTR(_T("as")), &vAs);
		if (SUCCEEDED(hRes))
		{
			if (vAs.vt != VT_NULL)
			{
				hRes = vAs.ChangeType(VT_BSTR);
				if (vAs == CComVariant(_T("native")))
				{
					AsType = XPFCONVTYPE_NATIVE;
				}
				else if (vAs == CComVariant(_T("simple")))
				{
					AsType = XPFCONVTYPE_SIMPLE;
				}
				else if (vAs == CComVariant(_T("stroked")))
				{
					AsType = XPFCONVTYPE_STROKED;
				}
				else if (vAs == CComVariant(_T("bitmap")))
				{
					AsType = XPFCONVTYPE_BITMAP;
				}
				else if (vAs == CComVariant(_T("bitmapfill")))
				{
					AsType = XPFCONVTYPE_BITMAPFILL;
				}
				else if (vAs == CComVariant(_T("bitmaptrans")))
				{
					AsType = XPFCONVTYPE_BITMAPTRANS;
				}
				else if (vAs == CComVariant(_T("bitmapfilltrans")))
				{
					AsType = XPFCONVTYPE_BITMAPFILLTRANS;
				}
				else if (vAs == CComVariant(_T("bitmapspan")))
				{
					AsType = XPFCONVTYPE_BITMAPSPAN;
				}
				else if (vAs == CComVariant(_T("reformat")))
				{
					AsType = XPFCONVTYPE_REFORMAT;
				}
				else if (vAs == CComVariant(_T("remove")))
				{
					AsType = XPFCONVTYPE_REMOVE;
				}
				else if (vAs == CComVariant(_T("simplergb")))
				{
					AsType = XPFCONVTYPE_SIMPLERGB;
				}
				else if (vAs == CComVariant(_T("rgb")))
				{
					AsType = XPFCONVTYPE_RGB;
				}
				else if (vAs == CComVariant(_T("cmyk")))
				{
					AsType = XPFCONVTYPE_CMYK;
				}
				else
				{
					ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
				}
			}
		}
	}
	else
	{
		ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
	}

	*pValue = AsType;
	return(S_OK);
}*/


BOOL PluginOILFilter::GetConvertAsType(xmlNodePtr pNode, XPFConvertType* pValue)
{
	XPFConvertType AsType = XPFCONVTYPE_UNKNOWN;

	if (pNode)
	{
		wxString str = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"as"));
		if (!str.IsEmpty())
		{
			if (str == _T("native"))
			{
				AsType = XPFCONVTYPE_NATIVE;
			}
			else if (str == _T("simple"))
			{
				AsType = XPFCONVTYPE_SIMPLE;
			}
			else if (str == _T("stroked"))
			{
				AsType = XPFCONVTYPE_STROKED;
			}
			else if (str == _T("bitmap"))
			{
				AsType = XPFCONVTYPE_BITMAP;
			}
			else if (str == _T("bitmapfill"))
			{
				AsType = XPFCONVTYPE_BITMAPFILL;
			}
			else if (str == _T("bitmaptrans"))
			{
				AsType = XPFCONVTYPE_BITMAPTRANS;
			}
			else if (str == _T("bitmapfilltrans"))
			{
				AsType = XPFCONVTYPE_BITMAPFILLTRANS;
			}
			else if (str == _T("bitmapspan"))
			{
				AsType = XPFCONVTYPE_BITMAPSPAN;
			}
			else if (str == _T("reformat"))
			{
				AsType = XPFCONVTYPE_REFORMAT;
			}
			else if (str == _T("remove"))
			{
				AsType = XPFCONVTYPE_REMOVE;
			}
			else if (str == _T("simplergb"))
			{
				AsType = XPFCONVTYPE_SIMPLERGB;
			}
			else if (str == _T("rgb"))
			{
				AsType = XPFCONVTYPE_RGB;
			}
			else if (str == _T("cmyk"))
			{
				AsType = XPFCONVTYPE_CMYK;
			}
			else
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML));
			}
		}
	}
	else
	{
		ERROR1(FALSE, _R(IDE_XPF_BADXML));
	}

	*pValue = AsType;

	return TRUE;
}


/*HRESULT PluginOILFilter::GetXPFBOOL(IXMLDOMNode* pNode, LPTSTR pAttrName, XPFBOOL* pbValue)
{
	XPFBOOL bValue = XPFB_UNKNOWN;

	CComQIPtr<IXMLDOMElement> pElem(pNode);
	if (pElem)
	{
		HRESULT hRes;

		CComVariant vVal;
		hRes = pElem->getAttribute(CComBSTR(pAttrName), &vVal);
		if (SUCCEEDED(hRes))
		{
			if (vVal.vt != VT_NULL)
			{
				hRes = vVal.ChangeType(VT_BSTR);
				if (vVal == CComVariant(_T("true")))
				{
					bValue = XPFB_TRUE;
				}
				else if (vVal == CComVariant(_T("false")))
				{
					bValue = XPFB_FALSE;
				}
				else
				{
					ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
				}
			}
		}
	}
	else
	{
		ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
	}

	*pbValue = bValue;
	return(S_OK);
}*/


BOOL PluginOILFilter::GetXPFBOOL(xmlNodePtr pNode, LPTSTR pAttrName, XPFBOOL* pbValue)
{
	XPFBOOL bValue = XPFB_UNKNOWN;

	if (pNode)
	{
		wxString strAttrName(pAttrName);
		wxCharBuffer buf = strAttrName.mb_str(wxConvUTF8);			// buf will remain allocated in this scope
		wxString str = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)buf.data()));
		if (!str.IsEmpty())
		{
			if (str == _T("true"))
			{
				bValue = XPFB_TRUE;
			}
			else if (str == _T("false"))
			{
				bValue = XPFB_FALSE;
			}
			else
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML));
			}
		}
	}
	else
	{
		ERROR1(FALSE, _R(IDE_XPF_BADXML));
	}

	*pbValue = bValue;

	return TRUE;
}


/*HRESULT PluginOILFilter::GetXPFProp(IXMLDOMNode* pNode, LPTSTR pAttrName, PropMapEntry aMap[], XPFProp* pValue)
{
	XPFProp Value = XPFP_UNKNOWN;

	CComQIPtr<IXMLDOMElement> pElem(pNode);
	if (pElem)
	{
		HRESULT hRes;

		CComVariant vVal;
		hRes = pElem->getAttribute(CComBSTR(pAttrName), &vVal);
		if (SUCCEEDED(hRes))
		{
			if (vVal.vt != VT_NULL)
			{
				hRes = vVal.ChangeType(VT_BSTR);

				// Loop through the map until we find it or the NULL indicating the end
				INT32 Index = 0;
				while (aMap[Index].pName)
				{
					if (vVal == CComVariant(aMap[Index].pName))
					{
						Value = aMap[Index].Value;
						break;
					}
					Index++;				
				}
				
				if (Value == XPFP_UNKNOWN)
				{
					ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
				}
			}
		}
	}
	else
	{
		ERROR1(E_FAIL, _R(IDE_XPF_BADXML));
	}

	*pValue = Value;
	return(S_OK);
}*/


BOOL PluginOILFilter::GetXPFProp(xmlNodePtr pNode, LPTSTR pAttrName, PropMapEntry aMap[], XPFProp* pValue)
{
	XPFProp Value = XPFP_UNKNOWN;

	if (pNode)
	{
		wxString strAttrName(pAttrName);
		wxCharBuffer buf = strAttrName.mb_str(wxConvUTF8);			// buf will remain allocated in this scope
		wxString str = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)buf.data()));
		if (!str.IsEmpty())
		{
			// Loop through the map until we find it or the NULL indicating the end
			INT32 Index = 0;
			while (aMap[Index].pName)
			{
				if (str == aMap[Index].pName)
				{
					Value = aMap[Index].Value;
					break;
				}
				Index++;
			}

			if (Value == XPFP_UNKNOWN)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML));
			}
		}
	}
	else
	{
		ERROR1(FALSE, _R(IDE_XPF_BADXML));
	}

	*pValue = Value;

	return TRUE;
}


/*XPFCapability* PluginOILFilter::CreateObjectNode(IXMLDOMNode* pNode)
{
	XPFCapability* pCap = NULL;

	CComBSTR bsName;
	HRESULT hRes = pNode->get_nodeName(&bsName);
	
	if (bsName == _T("#text"))
	{
		return(NULL);
	}

	XPFConvertType AsType = XPFCONVTYPE_UNKNOWN;
	hRes = GetConvertAsType(pNode, &AsType);
	if (bsName == _T("Layer"))
	{
		// Read the visible and locked attributes
		XPFBOOL bVisible = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("visible"), &bVisible);
		XPFBOOL bLocked = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("locked"), &bLocked);
		XPFBOOL bPrintable = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("printable"), &bPrintable);
		XPFBOOL bActive = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("active"), &bActive);
		XPFBOOL bBackground = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("background"), &bBackground);
		XPFBOOL bGuide = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("guide"), &bGuide);
		pCap = new XPFCLayer(AsType, bVisible, bLocked, bPrintable, bActive, bBackground, bGuide);
	}
	else if (bsName == _T("Contour"))
	{
		pCap = new XPFCContour(AsType);
	}
	else if (bsName == _T("Shadow"))
	{
		// Read the type attribute
		XPFProp Type = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("type"), aShadowTypes, &Type);
		pCap = new XPFCShadow(AsType, Type);
	}
	else if (bsName == _T("Bevel"))
	{
		// Read the type and side attributes
		XPFProp Type = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("type"), aBevelTypes, &Type);
		XPFProp Side = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("side"), aBevelSides, &Side);
		pCap = new XPFCBevel(AsType, Type, Side);
	}
	else if (bsName == _T("Blend"))
	{
		// Read the effect, oncurve, posprofile and attrprofile attributes
		XPFProp Effect = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("effect"), aColourEffects, &Effect);
		XPFBOOL bOnCurve = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("oncurve"), &bOnCurve);
		XPFBOOL bObjProfile = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("posprofile"), &bObjProfile);
		XPFBOOL bAttrProfile = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("attrprofile"), &bAttrProfile);
		pCap = new XPFCBlend(AsType, Effect, bOnCurve, bObjProfile, bAttrProfile);
	}
	else if (bsName == _T("Mould"))
	{
		// Read the as and type attributes
		XPFProp Type = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("type"), aMouldTypes, &Type);
		XPFBOOL bGradFill = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("gradfill"), &bGradFill);
		pCap = new XPFCMould(AsType, Type, bGradFill);
	}
	else if (bsName == _T("Rectangle"))
	{
		// Read the as, rounded and complex attributes
		XPFBOOL bComplex = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("complex"), &bComplex);
		XPFBOOL bRounded = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("rounded"), &bRounded);
		XPFBOOL bStellated = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("stellated"), &bStellated);
		XPFBOOL bReformed = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("reformed"), &bReformed);
		pCap = new XPFCRectangle(AsType, bComplex, bRounded, bStellated, bReformed);
	}
	else if (bsName == _T("Ellipse"))
	{
		// Read the as and complex attributes
		XPFBOOL bComplex = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("complex"), &bComplex);
		pCap = new XPFCEllipse(AsType, bComplex);
	}
	else if (bsName == _T("Polygon"))
	{
		// Read the as, rounded, stellated and reformed attributes
		XPFBOOL bRounded = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("rounded"), &bRounded);
		XPFBOOL bStellated = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("stellated"), &bStellated);
		XPFBOOL bReformed = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("reformed"), &bReformed);
		pCap = new XPFCPolygon(AsType, bRounded, bStellated, bReformed);
	}
	else if (bsName == _T("Bitmap"))
	{
		// Read the complex and contone attributes
		XPFBOOL bComplex = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("complex"), &bComplex);
		XPFBOOL bContone = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("contone"), &bContone);
		pCap = new XPFCBitmap(AsType, bComplex, bContone);
	}
	else if (bsName == _T("Text"))
	{
		// Read the onpath, complex and plain attributes
		XPFBOOL bOnPath = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("onpath"), &bOnPath);
		XPFBOOL bComplex = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("complex"), &bComplex);
		XPFBOOL bPlain = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("plain"), &bPlain);
		XPFBOOL bAutoKern = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("autokern"), &bAutoKern);
		XPFBOOL bJustified = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("justified"), &bJustified);
		pCap = new XPFCText(AsType, bOnPath, bComplex, bPlain, bAutoKern, bJustified);
	}
	else if (bsName == _T("ClipView"))
	{
		pCap = new XPFCClipView(AsType);
	}
	else if (bsName == _T("BitmapEffect"))
	{
		pCap = new XPFCBitmapEffect(AsType);
	}
	else if (bsName == _T("Feather"))
	{
		pCap = new XPFCFeather(AsType);
	}
	else
	{
		ERROR1(NULL, _R(IDE_XPF_BADXML));
	}

	CComPtr<IXMLDOMNode> pChild;
	CComPtr<IXMLDOMElement> pChildElem;
	hRes = pNode->get_firstChild(&pChild);
	INT32 Phase = 0;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCapNode = CreateObjectNode(pChild);

		if (pCapNode)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCapNode);
			}
			else
			{
				pCap->SetChild(pCapNode);
			}
			pLast = pCapNode;
		}

		CComPtr<IXMLDOMNode> pNextChild;
		hRes = pChild->get_nextSibling(&pNextChild);
		pChild = pNextChild;
	}

	return(pCap);
}*/


XPFCapability* PluginOILFilter::CreateObjectNode(xmlNodePtr pNode)
{
	XPFCapability* pCap = NULL;

	wxString strName = CXMLUtils::ConvertToWXString(pNode->name);
	
	if (strName == _T("#text") || xmlNodeIsText(pNode))
	{
		return(NULL);
	}

	XPFConvertType AsType = XPFCONVTYPE_UNKNOWN;
	HRESULT hRes = GetConvertAsType(pNode, &AsType);
	if (strName == _T("Layer"))
	{
		// Read the visible and locked attributes
		XPFBOOL bVisible = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("visible"), &bVisible);
		XPFBOOL bLocked = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("locked"), &bLocked);
		XPFBOOL bPrintable = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("printable"), &bPrintable);
		XPFBOOL bActive = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("active"), &bActive);
		XPFBOOL bBackground = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("background"), &bBackground);
		XPFBOOL bGuide = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("guide"), &bGuide);
		pCap = new XPFCLayer(AsType, bVisible, bLocked, bPrintable, bActive, bBackground, bGuide);
	}
	else if (strName == _T("Contour"))
	{
		pCap = new XPFCContour(AsType);
	}
	else if (strName == _T("Shadow"))
	{
		// Read the type attribute
		XPFProp Type = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("type"), aShadowTypes, &Type);
		pCap = new XPFCShadow(AsType, Type);
	}
	else if (strName == _T("Bevel"))
	{
		// Read the type and side attributes
		XPFProp Type = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("type"), aBevelTypes, &Type);
		XPFProp Side = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("side"), aBevelSides, &Side);
		pCap = new XPFCBevel(AsType, Type, Side);
	}
	else if (strName == _T("Blend"))
	{
		// Read the effect, oncurve, posprofile and attrprofile attributes
		XPFProp Effect = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("effect"), aColourEffects, &Effect);
		XPFBOOL bOnCurve = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("oncurve"), &bOnCurve);
		XPFBOOL bObjProfile = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("posprofile"), &bObjProfile);
		XPFBOOL bAttrProfile = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("attrprofile"), &bAttrProfile);
		pCap = new XPFCBlend(AsType, Effect, bOnCurve, bObjProfile, bAttrProfile);
	}
	else if (strName == _T("Mould"))
	{
		// Read the as and type attributes
		XPFProp Type = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("type"), aMouldTypes, &Type);
		XPFBOOL bGradFill = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("gradfill"), &bGradFill);
		pCap = new XPFCMould(AsType, Type, bGradFill);
	}
	else if (strName == _T("Rectangle"))
	{
		// Read the as, rounded and complex attributes
		XPFBOOL bComplex = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("complex"), &bComplex);
		XPFBOOL bRounded = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("rounded"), &bRounded);
		XPFBOOL bStellated = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("stellated"), &bStellated);
		XPFBOOL bReformed = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("reformed"), &bReformed);
		pCap = new XPFCRectangle(AsType, bComplex, bRounded, bStellated, bReformed);
	}
	else if (strName == _T("Ellipse"))
	{
		// Read the as and complex attributes
		XPFBOOL bComplex = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("complex"), &bComplex);
		pCap = new XPFCEllipse(AsType, bComplex);
	}
	else if (strName == _T("Polygon"))
	{
		// Read the as, rounded, stellated and reformed attributes
		XPFBOOL bRounded = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("rounded"), &bRounded);
		XPFBOOL bStellated = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("stellated"), &bStellated);
		XPFBOOL bReformed = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("reformed"), &bReformed);
		pCap = new XPFCPolygon(AsType, bRounded, bStellated, bReformed);
	}
	else if (strName == _T("Bitmap"))
	{
		// Read the complex and contone attributes
		XPFBOOL bComplex = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("complex"), &bComplex);
		XPFBOOL bContone = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("contone"), &bContone);
		pCap = new XPFCBitmap(AsType, bComplex, bContone);
	}
	else if (strName == _T("Text"))
	{
		// Read the onpath, complex and plain attributes
		XPFBOOL bOnPath = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("onpath"), &bOnPath);
		XPFBOOL bComplex = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("complex"), &bComplex);
		XPFBOOL bPlain = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("plain"), &bPlain);
		XPFBOOL bAutoKern = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("autokern"), &bAutoKern);
		XPFBOOL bJustified = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("justified"), &bJustified);
		pCap = new XPFCText(AsType, bOnPath, bComplex, bPlain, bAutoKern, bJustified);
	}
	else if (strName == _T("ClipView"))
	{
		pCap = new XPFCClipView(AsType);
	}
	else if (strName == _T("BitmapEffect"))
	{
		pCap = new XPFCBitmapEffect(AsType);
	}
	else if (strName == _T("Feather"))
	{
		pCap = new XPFCFeather(AsType);
	}
	else
	{
		ERROR1(NULL, _R(IDE_XPF_BADXML));
	}

	xmlNodePtr pChild;
	pChild = pNode->children;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCapNode = CreateObjectNode(pChild);

		if (pCapNode)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCapNode);
			}
			else
			{
				pCap->SetChild(pCapNode);
			}
			pLast = pCapNode;
		}

		pChild = pChild->next;
	}

	return(pCap);
}


/*XPFCapability* PluginOILFilter::CreateAttributeNode(IXMLDOMNode* pNode)
{
	XPFCapability* pCap = NULL;

	CComBSTR bsName;
	HRESULT hRes = pNode->get_nodeName(&bsName);
	
	if (bsName == _T("#text"))
	{
		return(NULL);
	}

	XPFConvertType AsType = XPFCONVTYPE_UNKNOWN;
	hRes = GetConvertAsType(pNode, &AsType);
	if (bsName == _T("Fill"))
	{
		XPFProp Shape = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("shape"), aFillShapes, &Shape);
		XPFProp Repeat = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("repeat"), aFillRepeats, &Repeat);
		XPFBOOL bMultistage = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("multistage"), &bMultistage);
		XPFProp Effect = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("effect"), aColourEffects, &Effect);
		XPFBOOL bProfile = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("profile"), &bProfile);
		XPFBOOL bContone = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("contone"), &bContone);
		pCap = new XPFCFill(AsType, Shape, Repeat, bMultistage, Effect, bProfile, bContone);
	}
	else if (bsName == _T("FillTrans"))
	{
		XPFProp Shape = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("shape"), aFillShapes, &Shape);
		XPFProp Type = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("type"), aTransTypes, &Type);
		XPFProp Repeat = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("repeat"), aFillRepeats, &Repeat);
		XPFBOOL bProfile = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("profile"), &bProfile);
		pCap = new XPFCFillTrans(AsType, Shape, Type, Repeat, bProfile);
	}
	else if (bsName == _T("Line"))
	{
		XPFBOOL bDash = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("dash"), &bDash);
		XPFBOOL bArrowhead = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("arrowhead"), &bArrowhead);
		XPFProp Cap = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("cap"), aLineCaps, &Cap);
		XPFProp Join = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("join"), aLineJoins, &Join);
		XPFBOOL bStroke = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("stroke"), &bStroke);
		XPFBOOL bBrush = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("brush"), &bBrush);
		pCap = new XPFCLine(AsType, bDash, bArrowhead, Cap, Join, bStroke, bBrush);
	}
	else if (bsName == _T("LineTrans"))
	{
		XPFProp Type = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("type"), aTransTypes, &Type);
		pCap = new XPFCLineTrans(AsType, Type);
	}
	else if (bsName == _T("Feather"))
	{
		pCap = new XPFCFeather(AsType);
	}
	else
	{
		ERROR1(NULL, _R(IDE_XPF_BADXML));
	}

	CComPtr<IXMLDOMNode> pChild;
	CComPtr<IXMLDOMElement> pChildElem;
	hRes = pNode->get_firstChild(&pChild);
	INT32 Phase = 0;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCapNode = CreateAttributeNode(pChild);

		if (pCapNode)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCapNode);
			}
			else
			{
				pCap->SetChild(pCapNode);
			}
			pLast = pCapNode;
		}

		CComPtr<IXMLDOMNode> pNextChild;
		hRes = pChild->get_nextSibling(&pNextChild);
		pChild = pNextChild;
	}

	return(pCap);
}*/


XPFCapability* PluginOILFilter::CreateAttributeNode(xmlNodePtr pNode)
{
	XPFCapability* pCap = NULL;

	wxString strName = CXMLUtils::ConvertToWXString(pNode->name);
	
	if (strName == _T("#text") || xmlNodeIsText(pNode))
	{
		return(NULL);
	}

	XPFConvertType AsType = XPFCONVTYPE_UNKNOWN;
	HRESULT hRes = GetConvertAsType(pNode, &AsType);
	if (strName == _T("Fill"))
	{
		XPFProp Shape = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("shape"), aFillShapes, &Shape);
		XPFProp Repeat = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("repeat"), aFillRepeats, &Repeat);
		XPFBOOL bMultistage = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("multistage"), &bMultistage);
		XPFProp Effect = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("effect"), aColourEffects, &Effect);
		XPFBOOL bProfile = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("profile"), &bProfile);
		XPFBOOL bContone = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("contone"), &bContone);
		pCap = new XPFCFill(AsType, Shape, Repeat, bMultistage, Effect, bProfile, bContone);
	}
	else if (strName == _T("FillTrans"))
	{
		XPFProp Shape = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("shape"), aFillShapes, &Shape);
		XPFProp Type = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("type"), aTransTypes, &Type);
		XPFProp Repeat = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("repeat"), aFillRepeats, &Repeat);
		XPFBOOL bProfile = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("profile"), &bProfile);
		pCap = new XPFCFillTrans(AsType, Shape, Type, Repeat, bProfile);
	}
	else if (strName == _T("Line"))
	{
		XPFBOOL bDash = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("dash"), &bDash);
		XPFBOOL bArrowhead = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("arrowhead"), &bArrowhead);
		XPFProp Cap = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("cap"), aLineCaps, &Cap);
		XPFProp Join = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("join"), aLineJoins, &Join);
		XPFBOOL bStroke = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("stroke"), &bStroke);
		XPFBOOL bBrush = XPFB_UNKNOWN;
		hRes = GetXPFBOOL(pNode, _T("brush"), &bBrush);
		pCap = new XPFCLine(AsType, bDash, bArrowhead, Cap, Join, bStroke, bBrush);
	}
	else if (strName == _T("LineTrans"))
	{
		XPFProp Type = XPFP_UNKNOWN;
		hRes = GetXPFProp(pNode, _T("type"), aTransTypes, &Type);
		pCap = new XPFCLineTrans(AsType, Type);
	}
	else if (strName == _T("Feather"))
	{
		pCap = new XPFCFeather(AsType);
	}
	else
	{
		ERROR1(NULL, _R(IDE_XPF_BADXML));
	}

	xmlNodePtr pChild;
	pChild = pNode->children;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCapNode = CreateAttributeNode(pChild);

		if (pCapNode)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCapNode);
			}
			else
			{
				pCap->SetChild(pCapNode);
			}
			pLast = pCapNode;
		}

		pChild = pChild->next;
	}

	return(pCap);
}


/*XPFCapability* PluginOILFilter::CreateColourNode(IXMLDOMNode* pNode)
{
	XPFCapability* pCap = NULL;

	CComBSTR bsName;
	HRESULT hRes = pNode->get_nodeName(&bsName);
	
	if (bsName == _T("#text"))
	{
		return(NULL);
	}

	XPFConvertType AsType = XPFCONVTYPE_UNKNOWN;
	hRes = GetConvertAsType(pNode, &AsType);
	if (bsName == _T("Colour"))
	{
		pCap = new XPFCColour(AsType);
	}
	else
	{
		ERROR1(NULL, _R(IDE_XPF_BADXML));
	}

	CComPtr<IXMLDOMNode> pChild;
	CComPtr<IXMLDOMElement> pChildElem;
	hRes = pNode->get_firstChild(&pChild);
	INT32 Phase = 0;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCapNode = CreateColourNode(pChild);

		if (pCapNode)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCapNode);
			}
			else
			{
				pCap->SetChild(pCapNode);
			}
			pLast = pCapNode;
		}

		CComPtr<IXMLDOMNode> pNextChild;
		hRes = pChild->get_nextSibling(&pNextChild);
		pChild = pNextChild;
	}

	return(pCap);
}*/

XPFCapability* PluginOILFilter::CreateColourNode(xmlNodePtr pNode)
{
	XPFCapability* pCap = NULL;

	wxString strName = CXMLUtils::ConvertToWXString(pNode->name);
	
	if (strName == _T("#text") || xmlNodeIsText(pNode))
	{
		return(NULL);
	}

	XPFConvertType AsType = XPFCONVTYPE_UNKNOWN;
	HRESULT hRes = GetConvertAsType(pNode, &AsType);
	if (strName == _T("Colour"))
	{
		pCap = new XPFCColour(AsType);
	}
	else
	{
		ERROR1(NULL, _R(IDE_XPF_BADXML));
	}

	xmlNodePtr pChild;
	pChild = pNode->children;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCapNode = CreateColourNode(pChild);

		if (pCapNode)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCapNode);
			}
			else
			{
				pCap->SetChild(pCapNode);
			}
			pLast = pCapNode;
		}

		pChild = pChild->next;
	}

	return(pCap);
}

/****************************************************************************

>	void PluginOILFilter::Cleanup()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/2005

	Purpose:	

****************************************************************************/

void PluginOILFilter::Cleanup()
{
PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
	ReleaseFilterObject();
#endif
}



