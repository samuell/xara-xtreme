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
// implementation file for ATM Font manager winoil inclusion.

/*
*/

#include "camtypes.h"
#include "camelot.h"
#include "atmfonts.h"
#include "fontman.h"
#include "app.h"
#include "atminfo.h"
#include "oilmods.h"
#include "errors.h"
#include "txtattr.h"
#include "textfuns.h"
#include "atminst.h"
#include "unicdman.h"

DECLARE_SOURCE( "$Revision$" );

CC_IMPLEMENT_DYNCREATE( ATMFontMan, CCObject )
CC_IMPLEMENT_DYNCREATE( ATMFont, FontBase )

#define new CAM_DEBUG_NEW     
#define SCALEORD(ord) (MulDiv(ord,72000,65536))
#define MINORV(ver) LOBYTE(LOWORD(ver))
#define MAJORV(ver) HIBYTE(LOWORD(ver))

BOOL ATMFontMan::ATMRunning		= FALSE;
INT32 ATMFontMan::ATMVersion		= 0;
INT32 ATMFontMan::ATMUtilVersion = 0;
	
BOOL ATMFontMan::UseThunking	= TRUE;

// The DLL handle
HINSTANCE	ATMFontMan::DLLHandle		= NULL;

// Static pointers to functions in the DLL
ATMFontMan::PFN_ATMProperlyLoaded		ATMFontMan::pfnATMProperlyLoaded	= NULL;
ATMFontMan::PFN_ATMGetVersion			ATMFontMan::pfnATMGetVersion		= NULL;
ATMFontMan::PFN_ATMGetOutline			ATMFontMan::pfnATMGetOutline		= NULL;
ATMFontMan::PFN_ATMForceExactWidth		ATMFontMan::pfnATMForceExactWidth	= NULL;
ATMFontMan::PFN_ATMBeginFontChange		ATMFontMan::pfnATMBeginFontChange	= NULL;
ATMFontMan::PFN_ATMEndFontChange		ATMFontMan::pfnATMEndFontChange		= NULL;
ATMFontMan::PFN_ATMForceFontChange		ATMFontMan::pfnATMForceFontChange	= NULL;
ATMFontMan::PFN_ATMFontStatus			ATMFontMan::pfnATMFontStatus		= NULL;
ATMFontMan::PFN_ATMAddFont				ATMFontMan::pfnATMAddFont			= NULL;
ATMFontMan::PFN_ATMRemoveFont			ATMFontMan::pfnATMRemoveFont		= NULL;

// Static pointers to functions in the thunking DLL
LPFNATMPROPERLYLOADED		ATMFontMan::pfnXSATMProperlyLoaded	= NULL;
//LPFNATMGETVERSION			ATMFontMan::pfnXSATMGetVersion		= NULL;
LPFNATMGETOUTLINE			ATMFontMan::pfnXSATMGetOutline		= NULL;
LPFNATMFORCEEXACTWIDTH		ATMFontMan::pfnXSATMForceExactWidth	= NULL;
LPFNATMBEGINFONTCHANGE		ATMFontMan::pfnXSATMBeginFontChange	= NULL;
LPFNATMENDFONTCHANGE		ATMFontMan::pfnXSATMEndFontChange	= NULL;
//PFN_ATMForceFontChange	ATMFontMan::pfnXSATMForceFontChange	= NULL;
LPFNATMFONTSTATUS			ATMFontMan::pfnXSATMFontStatus		= NULL;
LPFNATMADDFONT				ATMFontMan::pfnXSATMAddFont			= NULL;
LPFNATMREMOVEFONT			ATMFontMan::pfnXSATMRemoveFont		= NULL;


/********************************************************************************************

>	ATMFont::ATMFont()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Purpose:	Default constructor

********************************************************************************************/

ATMFont::ATMFont()
{
}

/********************************************************************************************

>	ATMFont::~ATMFont()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Purpose:	destructor for the ATMFont class

********************************************************************************************/

ATMFont::~ATMFont()
{
}

/********************************************************************************************

>	void ATMFont::Dump()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95
	Purpose:	Dump the contents of this cache entry out

********************************************************************************************/

void ATMFont::Dump()
{
	FontBase::Dump();
	TRACE( _T(" FontClass = ATM Type1\n"));
}

/********************************************************************************************

>	ATMFontMan::ATMFontMan() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Purpose:	Default constructor

********************************************************************************************/

ATMFontMan::ATMFontMan()
{
}

/********************************************************************************************

>	static BOOL ATMFontMan::InitDLL()

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/95
	Inputs:		-
	Outputs:	
	Returns:	TRUE if the ATM mangler is actually running and can be used on this OpSys
	Purpose:	Determine whether we can make further calls to the Adobe Type Manager

********************************************************************************************/

BOOL ATMFontMan::InitDLL()
{
	if (!ATMRunning)
		ATMRunning = (ThunkProperlyLoaded()>0);
	
	ATMVersion = ThunkGetVersion();
	ATMUtilVersion = ThunkGetUtilVersion();

	return ATMRunning;
}


/********************************************************************************************

>	static void ATMFont::EnumAllFonts(OILEnumFonts* pClass)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	Give the kernel a way of enumerating fonts itself

********************************************************************************************/

void ATMFontMan::EnumAllFonts(OILEnumFonts* pClass)
{
	CDC Screen;
	if (Screen.CreateIC(TEXT("DISPLAY"), 0, 0, 0))
	{
		EnumFontFamilies(Screen.m_hDC,
						 NULL,
						 (FONTENUMPROC) ATMFontMan_CallBackDispatchFont,
						 (LPARAM)pClass);
		Screen.DeleteDC();
	}
}

/********************************************************************************************

>	static BOOL ATMFontMan::CacheNamedFont(String_64* pFontName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pFontName = a pointer to a fontname to find and cache
	Outputs:	-
	Returns:	-
	Purpose:	This function attempts to cache a font of the ATM variety

********************************************************************************************/

BOOL ATMFontMan::CacheNamedFont(String_64* pFontName)
{
	CDC Screen;						 					// and create a new one
	if (Screen.CreateIC(TEXT("DISPLAY"), 0, 0, 0))
	{	
		FontManager* pFontMan = GetApplication()->GetFontManager();
		ERROR2IF(pFontMan == NULL,FALSE,"NULL FontManager ptr");
		pFontMan->ClearTempFont();
		EnumFontFamilies(Screen.m_hDC,
						 NULL,
						 (FONTENUMPROC) ATMFontMan_CallBackCacheNamedFont,
						 (LPARAM)pFontName);
		Screen.DeleteDC();
		return pFontMan->TempFontValid();
	}
	return FALSE;
}

/********************************************************************************************

>	static BOOL ATMFontMan::CacheCompatibleFont(String_64* pFontName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pFontName = a pointer to a fontname to find and cache
	Outputs:	-
	Returns:	-
	Purpose:	This function attempts to cache a font of the ATM variety. It is called
				as a second pass function after CacheNamedFont() has tried to cache the
				actual font. From here we can perform several actions to attempt to
				construct the font if it is an instance of a multiple master.

********************************************************************************************/

BOOL ATMFontMan::CacheCompatibleFont(String_64* pFontName)
{

	// search for an underscore in the font
	INT32 pos = pFontName->Sub(String_32("_"),0,0);
	if (pos==-1)
		return FALSE;

	// pos is position of first _ character, is there a digit next?
	TCHAR digit = (*pFontName)[pos+1];
	if (!StringBase::IsNumeric(digit))
		return FALSE;

	String_64 MultiMaster;
	pFontName->Left(&MultiMaster, pos);

	// now we need to know whether this font exists.
	return FALSE;
}


/********************************************************************************************

>	static void ATMFontMan::ValidateCache()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	This function attempts to recache a font of the ATM variety

********************************************************************************************/

void ATMFontMan::ValidateCache()
{
	CDC Screen;
	if (Screen.CreateIC(TEXT("DISPLAY"), 0, 0, 0))
	{
		EnumFontFamilies(Screen.m_hDC,
						 NULL,
						 (FONTENUMPROC) ATMFontMan_CallBackValidateFont,
						 NULL);
		Screen.DeleteDC();
	}
}

/********************************************************************************************

>	static void ATMFontMan::FindClosestFont()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96
	Purpose:	Attempts to search the list of installed fonts, looking for a match

********************************************************************************************/

void ATMFontMan::FindClosestFont()
{
	CDC Screen;
	if (Screen.CreateIC(TEXT("DISPLAY"), 0, 0, 0))
	{
		EnumFontFamilies(Screen.m_hDC,
						 NULL,
						 (FONTENUMPROC) ATMFontMan_CallBackFindClosestFont,
						 NULL);
		Screen.DeleteDC();
	}
}

/********************************************************************************************

>	ATMFont* ATMFontMan::CreateNewFont(String_64* pFontName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	This function attempts to create a font instance and will be called by
				the font manager when new fonts are added.

********************************************************************************************/

ATMFont* ATMFontMan::CreateNewFont(String_64* pFontName)
{
	ATMFont *pFont = new ATMFont;
	if (pFont==NULL)
		return NULL;
	if (!pFont->Initialise(pFontName))
	{
		delete pFont;
		return NULL;
	}
	return pFont;
}

/********************************************************************************************

	static BOOL ATMFontMan::IsOkToCall()

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/95
	Returns:	TRUE if the ATM mangler is actually running and can be used on this OpSys
	Purpose:	Determine whether we can make further calls to the Adobe Type Manager

********************************************************************************************/

BOOL ATMFontMan::IsOkToCall()
{
	return ATMRunning;
}

/********************************************************************************************

	static HINSTANCE ATMFontMan::GetATMDLL()

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/95
	Returns:	the hInst instance of the ATM dll.
				NULL if the DLL has not been loaded or cannot be called due to error
	Purpose:	Returns the instance handle of the ATM DLL, or NULL if the DLL is not
				running.

********************************************************************************************/

HINSTANCE ATMFontMan::GetATMDLL()
{
	if (!IsOkToCall())
		return NULL;

	// Are we on Windows 95 and hence require the thunking DLL?
	// This should be the default case
	if (UseThunking)
		return ExtraDLLs[ ATM_Util_DLL ];

	// We must be on NT where we have a real DLL
	return DLLHandle;
}


/********************************************************************************************

>	BOOL ATMFontMan::CorrectVersion(INT32 version)

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/95
	Inputs:		version = the version number to check against
						  lo byte = minor version
						  hi byte = major version
	Returns:	TRUE if the running version of ATM is new enough to use
				FALSE if not
	Purpose:	This routine can be used by our OIL layer functions needing to call ATM 
				provided functions. The problem is there are likely to be many versions
				of ATM out there, and new functions obviously appear in later versions.
				So it is recommended that you check the running ATM version to make sure
				the function was actually implemented in this version. See ATM.h for all
				function versions. 
				NOTE all the functions we call should have MinVer_<functionname> defines
				in the header.

********************************************************************************************/

BOOL ATMFontMan::CorrectVersion(INT32 version)
{
	return (version <= ATMVersion);
}

/********************************************************************************************

>	INT32 ATMFontMan::ThunkProperlyLoaded()

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/95
	Inputs:		-
	Outputs:	
	Returns:	>0 if the ATM manager is actually running and can be used on this OpSys
	Purpose:	Determine whether we can make further calls to the Adobe Type Manager
				This call has been in the ATM from version 1.00 so there's no need to check
				it.

********************************************************************************************/

INT32 ATMFontMan::ThunkProperlyLoaded()
{
	typedef WORD (FAR PASCAL *PFN)();
	
	const HINSTANCE hLib = ExtraDLLs[ ATM_Util_DLL ];
	// if our DLL is not up then return FALSE.
	if (!hLib)
		return 0;

	// extract the function pointers that we require
	pfnXSATMProperlyLoaded	= (LPFNATMPROPERLYLOADED)	GetProcAddress( hLib, MAKEINTRESOURCE(XSATM_ATMProperlyLoaded));
//	pfnXSATMGetVersion		= (LPFNATMGETVERSION)		GetProcAddress( hLib, MAKEINTRESOURCE(XSATM_ATMGetVersion));
	pfnXSATMGetOutline		= (LPFNATMGETOUTLINE)		GetProcAddress( hLib, MAKEINTRESOURCE(XSATM_ATMGetOutline));
	pfnXSATMForceExactWidth	= (LPFNATMFORCEEXACTWIDTH)	GetProcAddress( hLib, MAKEINTRESOURCE(XSATM_ATMForceExactWidth));
	pfnXSATMBeginFontChange	= (LPFNATMBEGINFONTCHANGE)	GetProcAddress( hLib, MAKEINTRESOURCE(XSATM_ATMBeginFontChange));
	pfnXSATMEndFontChange	= (LPFNATMENDFONTCHANGE)	GetProcAddress( hLib, MAKEINTRESOURCE(XSATM_ATMEndFontChange));
//	pfnXSATMForceFontChange	= (PFN_ATMForceFontChange)	GetProcAddress( hLib, MAKEINTRESOURCE(XSATM_ATMForceFontChange"));
	pfnXSATMFontStatus		= (LPFNATMFONTSTATUS)		GetProcAddress( hLib, MAKEINTRESOURCE(XSATM_ATMFontStatus));
	pfnXSATMAddFont			= (LPFNATMADDFONT)			GetProcAddress( hLib, MAKEINTRESOURCE(XSATM_ATMAddFont));
	pfnXSATMRemoveFont		= (LPFNATMREMOVEFONT)		GetProcAddress( hLib, MAKEINTRESOURCE(XSATM_ATMRemoveFont));

	// If we had a problem then unload the DLL and say so
	if (pfnXSATMProperlyLoaded	== NULL ||
//		pfnXSATMGetVersion		== NULL ||
		pfnXSATMGetOutline		== NULL ||
		pfnXSATMForceExactWidth	== NULL ||
		pfnXSATMBeginFontChange	== NULL ||
		pfnXSATMEndFontChange	== NULL ||
//		pfnXSATMForceFontChange	== NULL ||
		pfnXSATMFontStatus		== NULL ||
		pfnXSATMAddFont			== NULL ||
		pfnXSATMRemoveFont		== NULL
		)
	{
		return FALSE;
	}

	INT32 result = (INT32)(*pfnXSATMProperlyLoaded)();
	return result;
}

/********************************************************************************************

>	static INT32 ATMFontMan::ThunkGetUtilVersion()

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/94
	Inputs:		None
	Outputs:	None
	Returns:	High byte major version, low byte minor version
	Purpose:	Returns the version number of our ATM thunking layer support DLL. 
	Errors:		None.

********************************************************************************************/

INT32 ATMFontMan::ThunkGetUtilVersion()
{
	// if our DLL is not up then return FALSE.
	const HINSTANCE hLib = GetATMDLL();
	if (!hLib)
		return 0;

	// do some dynamic linking to the DLL
	LPFNATMGETUTILVERSION lpfn = (LPFNATMGETUTILVERSION)GetProcAddress(hLib, MAKEINTRESOURCE(XSATM_GetUtilVersion) );
	if (lpfn==NULL)
		return 0;

	return (INT32)(*lpfn)();
}

/********************************************************************************************

>	static INT32 ATMFontMan::ThunkGetVersion()

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/94
	Inputs:		None
	Outputs:	None
	Returns:	High byte major version, low byte minor version
	Purpose:	The version number of the currently running ATM.

********************************************************************************************/

INT32 ATMFontMan::ThunkGetVersion()
{
	// if our DLL is not up then return FALSE.
	const HINSTANCE hLib = GetATMDLL();
	if (!hLib)
		return 0;

	// do some dynamic linking to the DLL
	LPFNATMGETVERSION lpfn = (LPFNATMGETVERSION)GetProcAddress(hLib, MAKEINTRESOURCE(XSATM_ATMGetVersion) );
	if (lpfn==NULL)
		return 0;

	INT32 result = (INT32)(*lpfn)();
	
	// swap the byte order as its rampant!
	INT32 minor = HIBYTE(LOWORD(result));
	INT32 major = LOBYTE(LOWORD(result));

	return (minor | (major<<8));
}

/********************************************************************************************

>	static BOOL ATMFontMan::ForceExactWidth()

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/95
	Inputs:		-
	Outputs:	-
    Returns:	TRUE if the call is successful, FALSE otherwise.
	Purpose:	This routine causes the 'next' call for creating a font to create
				a font without adjusting the width data for the screen.

********************************************************************************************/

BOOL ATMFontMan::ForceExactWidth()
{
	// Are we on Windows 95 and hence require the thunking DLL?
	// This should be the default case
	if (UseThunking)
	{
		// If we haven't got the correct version then we cannot call it.
		if (!CorrectVersion(MinVer_ForceExactWidth))
			return FALSE;

		if (pfnXSATMForceExactWidth == NULL)
			return FALSE;

		return (BOOL)(*pfnXSATMForceExactWidth)();
	}

	// We must be on NT where we have a real DLL
	if (pfnATMForceExactWidth == NULL)
		return FALSE;

	return (BOOL)(*pfnATMForceExactWidth)();
}




/********************************************************************************************

>	static INT32 ATMFontMan::BeginFontChange()

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/95
	Inputs:		-
	Outputs:	-
    Returns:
	Purpose:	Starts a series of font change operations consisting of calls to
				the rotuines ATMAddFont and ATMRemoveFont.  Must be called before
				calling ATMAddFont or ATMRemoveFont.

********************************************************************************************/

INT32 ATMFontMan::BeginFontChange()
{
	// Are we on Windows 95 and hence require the thunking DLL?
	// This should be the default case
	if (UseThunking)
	{
		const HINSTANCE hLib = ExtraDLLs[ ATM_Util_DLL ];
		// if our DLL is not up then return FALSE.
		if (!hLib)
			return ATM_NOTRUNNING;

		// If we haven't got the correct version then we cannot call it.
		if (!CorrectVersion(MinVer_BeginFontChange))
			return ATM_NOTSUPPORTED;

		if (pfnXSATMBeginFontChange == NULL)
			return ATM_NOTRUNNING;

		return (INT32)((INT8)(*pfnXSATMBeginFontChange)());
	}

	// We must be on NT where we have a real DLL
	if (pfnATMBeginFontChange == NULL)
		return ATM_NOTRUNNING;

	return (BOOL)(*pfnATMBeginFontChange)();
}



/********************************************************************************************

	static INT32 ATMFontMan::EndFontChange()

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/95
	Inputs:		-
	Outputs:	-
    Returns:
	Purpose:	Finishes a series of font change operations consisting of calls to
				the rotuines ATMAddFont and ATMRemoveFont.
				Call this function after font changes are completed, or your font
				changes may not take effect.

********************************************************************************************/

INT32 ATMFontMan::EndFontChange()
{
	// Are we on Windows 95 and hence require the thunking DLL?
	// This should be the default case
	if (UseThunking)
	{
		const HINSTANCE hLib = ExtraDLLs[ ATM_Util_DLL ];
		// if our DLL is not up then return FALSE.
		if (!hLib)
			return ATM_NOTRUNNING;

		// If we haven't got the correct version then we cannot call it.
		if (!CorrectVersion(MinVer_EndFontChange))
			return ATM_NOTSUPPORTED;

		if (pfnXSATMEndFontChange == NULL)
			return ATM_NOTRUNNING;

		return (INT32)((INT8)(*pfnXSATMEndFontChange)());
	}

	// We must be on NT where we have a real DLL
	if (pfnATMEndFontChange == NULL)
		return ATM_NOTRUNNING;

	return (BOOL)(*pfnATMEndFontChange)();
}


/********************************************************************************************

>	static INT32 ATMFontMan::FontStatus( TCHAR *pMenuName,
										WORD Style,
										TCHAR *lpMetricsFile,
										TCHAR *lpFontFile)

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/95
	Inputs:		The lpMenuName contains the Windows name of the font.
				The style parameter contains none, one or both of the flags
				ATM_ITALIC and ATM_BOLD.
				The lpMetricsFile contains the full pathname to the .pfm file
				for the font.
				The lpFontFile contains the full pathname to the .pfb file for
				the font.
	Outputs:	-
    Returns:	Successful return values are:
				 ATM_FONTINUSE            The paths and menu name specify an
				                          existing font which is in use by ATM.
				 ATM_FONTPRESENT          The paths and menu name specify an
				                          existing font which is NOT in use.
				 ATM_FONTDIFFERENT        The font, as specified by its menu name
				                          and styles, exists but its data files
				                          are different from those specified by
				                          lpMetricsFile & lpFontFile parameters.
				 ATM_FONTABSENT           There is no such font installed.

				Failure return values are:
				 ATM_PATHTOOLONG          At least one path is too long.
				 ATM_BADFONTTYPE          The font is uncompressed.
				 ATM_BADSTYLE             The style parameter has bad bits!
				 ATM_NOFONT               At least one path doesn't point to
				                          the correct file.
				 ATM_BADMENUNAME          Missing or invalid menu name given.
	
	Purpose:	This routine determines whether or not a given font is currently
				installed and/or in use by ATM.  It should be called prior to
				installing a new font.
				If the return value is ATM_FONTINUSE, then no attempt should be made
				to install the font.


********************************************************************************************/

INT32 ATMFontMan::FontStatus(TCHAR *pMenuName, WORD Style, TCHAR *lpMetricsFile, TCHAR *lpFontFile)
{
	// Are we on Windows 95 and hence require the thunking DLL?
	// This should be the default case
	if (UseThunking)
	{
		const HINSTANCE hLib = ExtraDLLs[ ATM_Util_DLL ];
		// if our DLL is not up then return FALSE.
		if (!hLib)
			return ATM_NOTRUNNING;

		// If we haven't got the correct version then we cannot call it.
		if (!CorrectVersion(MinVer_FontStatus))
			return ATM_NOTSUPPORTED;

		if (pfnXSATMFontStatus == NULL)
			return ATM_NOTRUNNING;

		return (INT32)((INT8)(*pfnXSATMFontStatus)(pMenuName, Style, lpMetricsFile, lpFontFile));
	}

	// We must be on NT where we have a real DLL
	if (pfnATMFontStatus == NULL)
		return ATM_NOTRUNNING;

	return (INT32)((INT8)(*pfnATMFontStatus)(pMenuName, Style, lpMetricsFile, lpFontFile));
}



/********************************************************************************************

>	static INT32 ATMFontMan::AddFont(TCHAR *pMenuName, WORD Style, TCHAR *lpMetricsFile, TCHAR *lpFontFile)

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/95
	Inputs:		The lpMenuName contains the Windows name of the font.
				The style parameter contains none, one or both of the flags
				ATM_ITALIC and ATM_BOLD.
				The lpMetricsFile contains the full pathname to the .pfm file
				for the font.
				The lpFontFile contains the full pathname to the .pfb file for
				the font.
	Outputs:	-
    Returns:	Successful return values are:
				 ATM_NOERR

				Failure return values are:
				 ATM_INVALIDFONT          Invalid font format.
				 ATM_PATHTOOLONG          At least one path is too long.
				 ATM_BADFONTTYPE          The font is uncompressed.
				 ATM_BADSTYLE             The style parameter has bad bits!
				 ATM_NOROOM               Can't add to internal tables.
				 ATM_NOFONT               At least one path doesn't point to
				                          the correct file.
				 ATM_BADMENUNAME          Missing or invalid menu name given.
				 ATM_FONTINUSE            The paths and menu name specify an
				                          existing font which is in use by ATM.
	
	Purpose:	This routine makes a newly installed font available to ATM.

********************************************************************************************/

INT32 ATMFontMan::AddFont(TCHAR *pMenuName, WORD Style, TCHAR *lpMetricsFile, TCHAR *lpFontFile)
{
	// Are we on Windows 95 and hence require the thunking DLL?
	// This should be the default case
	if (UseThunking)
	{
		const HINSTANCE hLib = ExtraDLLs[ ATM_Util_DLL ];
		// if our DLL is not up then return FALSE.
		if (!hLib)
			return ATM_NOTRUNNING;

		// If we haven't got the correct version then we cannot call it.
		if (!CorrectVersion(MinVer_AddFont))
			return ATM_NOTSUPPORTED;

		// do some dynamic linking to the DLL
		if (pfnXSATMAddFont == NULL)
			return ATM_NOTRUNNING;

		return (INT32) ((INT8) (   (*pfnXSATMAddFont)(pMenuName, Style, lpMetricsFile, lpFontFile)  ));
	}

	// We must be on NT where we have a real DLL
	if (pfnATMAddFont == NULL)
		return ATM_NOTRUNNING;

	return (INT32) ((INT8) (   (*pfnATMAddFont)(pMenuName, Style, lpMetricsFile, lpFontFile)  ));
}





/********************************************************************************************

>	static INT32 ATMFontMan::RemoveFont(TCHAR *pMenuName, WORD Style)

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/9/95
	Inputs:		The lpMenuName contains the Windows name of the font.
				The style parameter contains none, one or both of the flags
				ATM_ITALIC and ATM_BOLD.
	Outputs:	-
    Returns:	Successful return values are:
				 ATM_NOERR

				Failure return values are:
				 ATM_BADSTYLE             The style parameter has bad bits!
				 ATM_NOFONT               The specified font is not available.
				 ATM_BADMENUNAME          Missing or invalid menu name given.
	
	Purpose:	This call makes a font inaccessible to ATM. Any subsequent
				requests to ATM for this font will fail.

********************************************************************************************/

INT32 ATMFontMan::RemoveFont(TCHAR *pMenuName, WORD Style)
{
	// Are we on Windows 95 and hence require the thunking DLL?
	// This should be the default case
	if (UseThunking)
	{
		const HINSTANCE hLib = ExtraDLLs[ ATM_Util_DLL ];
		// if our DLL is not up then return FALSE.
		if (!hLib)
			return ATM_NOTRUNNING;

		// If we haven't got the correct version then we cannot call it.
		if (!CorrectVersion(MinVer_RemoveFont))
			return ATM_NOTSUPPORTED;

		// do some dynamic linking to the DLL
		if (pfnXSATMRemoveFont == NULL)
			return ATM_NOTRUNNING;

		return (INT32)((INT8)(*pfnXSATMRemoveFont)(pMenuName, Style));
	}

	// We must be on NT where we have a real DLL
	if (pfnATMRemoveFont == NULL)
		return ATM_NOTRUNNING;

	return (INT32)((INT8)(*pfnATMRemoveFont)(pMenuName, Style));
}



/********************************************************************************************

	> static BOOL ATMFontMan::GetCharOutline(CharDescription& ChDesc, 
											 DocCoord** ppCoords,
											 PathVerb** ppVerbs,
											 UINT32* pNumCoords,
											 CDC* pDC)
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		ChDesc	= description of char
				pDC		= (possible) pointer to a DC (for optimisation) defaults to NULL
	Outputs:	ppCoords	= pointer to path coords buffer
				ppVerbs		= pointer to path verbs buffer
				pNumCoords	= number of elements in path
	Returns:	FALSE if unable to generate the character outline
	Purpose:	Get the path associated with a given character

********************************************************************************************/

BOOL ATMFontMan::GetCharOutline(CharDescription& ChDesc, 
								 DocCoord** ppCoords,
							  	 PathVerb** ppVerbs,
							  	 UINT32* pNumCoords,
							  	 CDC* pDC)
{
	LPFNMYMOVETO	lpMyMoveTo		= NULL;
	LPFNMYLINETO	lpMyLineTo		= NULL;
	LPFNMYCURVETO	lpMyCurveTo		= NULL;
	LPFNMYCLOSEPATH lpMyClosePath	= NULL;

	ATMFixedMatrix	tMatrix = { ATMINTTOFIXED(1),0,0,ATMINTTOFIXED(1),0,0 };
	INT32			result;
	char			renderchar;
	WCHAR			wchar;
	CFont 			hFont, *pOldFont;
	LOGFONT			CharLogFont;
	BOOL			LocalDC=FALSE;

	// Check some input parameters
	ERROR2IF(ppCoords==NULL,FALSE,"ATMFontMan::GetCharOutline pCoords==NULL");
	ERROR2IF(ppVerbs==NULL,FALSE,"ATMFontMan::GetCharOutline ppVerbs==NULL");
	ERROR2IF(pNumCoords==NULL,FALSE,"ATMFontMan::GetCharOutline pNumCoords==NULL");

	// Check we can call the ATM first
	const HINSTANCE hLib = GetATMDLL();
	if (!hLib)
		return FALSE;
	
	// Do we need to create a local DC?
	LocalDC=(pDC==NULL);
	if (LocalDC)
	{
		pDC=new CDC;
		ERROR2IF(pDC==NULL,FALSE,"ATMFontMan::GetCharOutline() - failed to create DC");
		if (pDC->CreateCompatibleDC(NULL)==FALSE)
		{
			delete pDC;
			pDC=NULL;
			ERROR2(FALSE,"ATMFontMan::GetCharOutline() - CreateCompatibleDC() failed");
		}
	}

	// get the character to render.
	// we limit the character if we are trying to get a unicode char as ATM GetOutline
	// takes a char. This happens to be a signed which is a bit rampant but it seems
	// ATM can cope with -97 given as a character code and maps it correctly
	wchar = ChDesc.GetCharCode();
	renderchar = (char)(UnicodeManager::UnicodeToMultiByte(wchar));

	// fill the log font description
	if (!TextManager::GetLogFontFromCharDescriptor(pDC, ChDesc, &CharLogFont))
		goto OutlineError;

	// Values from ATM docs (Jonathan 10/2000)
	CharLogFont.lfHeight		= -1;
	CharLogFont.lfWidth			= 0;
	CharLogFont.lfEscapement	= 0;

	// now stick it in our CFont object
	if (!hFont.CreateFontIndirect(&CharLogFont))
		goto OutlineError;

	// Select the font into the CDC
	pOldFont = pDC->SelectObject(&hFont);
	if (pOldFont==NULL)
		goto OutlineError;

	// Make Proc instances
	lpMyMoveTo		= MakeProcInstance(ATMFontMan::MyMoveTo, AppInst);
	lpMyLineTo		= MakeProcInstance(ATMFontMan::MyLineTo, AppInst);
	lpMyCurveTo		= MakeProcInstance(ATMFontMan::MyCurveTo, AppInst);
	lpMyClosePath	= MakeProcInstance(ATMFontMan::MyClosePath, AppInst);

	// Set our path cache to 0
	OILFontMan::InitialiseOutlineCache();
	
	// Are we on Windows 95 and hence require the thunking DLL?
	// This should be the default case
	if (UseThunking)
	{
		if (pfnXSATMGetOutline)
			result = (*pfnXSATMGetOutline)( pDC->GetSafeHdc(),
										   renderchar,
										   &tMatrix,
										   lpMyMoveTo,
										   lpMyLineTo,
										   lpMyCurveTo,
										   lpMyClosePath,
										   NULL);
	}
	else
	{
		if (pfnATMGetOutline)
			result = (*pfnATMGetOutline)( pDC->GetSafeHdc(),
										   renderchar,
										   &tMatrix,
										   lpMyMoveTo,
										   lpMyLineTo,
										   lpMyCurveTo,
										   lpMyClosePath,
										   NULL);
	}

/*
	switch (result)
	{
		case ATM_NOERR:			TRACE( _T("Normal return\n"));							break;
		case ATM_INVALIDFONT:	TRACE( _T("Font not consistent\n"));						break;
		case ATM_CANTHAPPEN:	TRACE( _T("Some kind of internal error\n"));				break;
		case ATM_BADMATRIX:		TRACE( _T("Matrix inverse undefined\n"));				break;
		case ATM_MEMORY:		TRACE( _T("Out of memory\n"));							break;
		case ATM_NOTSUPPORTED:	TRACE( _T("Running ATM does not support this call\n"));	break;
		case ATM_NOTRUNNING:	TRACE( _T("ATM is not running\n"));						break;
		case ATM_FILLORDER:		TRACE( _T("Inconsitent fill calls\n"));					break;
		case ATM_CANCELLED:		TRACE( _T("Client halted operation\n"));					break;
		case ATM_NOCHAR:		TRACE( _T("No outline for char code in font\n"));		break;
		case ATM_BADPROC:		TRACE( _T("Bad callback proc address\n"));				break;
		case ATM_CANTDRAW:		TRACE( _T("Error in imaging a bad rop?\n"));				break;
		case ATM_BADPARM:		TRACE( _T("Bad parameter passed in\n"));					break;
		case ATM_SELECTED:		TRACE( _T("See ATMSelectObject\n"));						break;
		case ATM_SELECTERROR:	TRACE( _T("See ATMSelectObject\n"));						break;
		default:				TRACE( _T("ATM call returned unknown error result\n"));	break;
	}	
*/


	// Matt 5/12/2000
	// Temporary Bodge-Fix for ATM Fonts Problem...
	// If the result of the font outline caching is 'Cancelled' then we change it to 'NoErr' and remove the
	// dodgy cache info for this character...
	// On some configurations of WinNT4 with ATM4 Deluxe, entering the 'Space' (UNICODE 32)
	// character causes it to throw this 'cancelled' error... THIS IS INTENDED AS A TEMPORARY FIX
	// UNTIL WE FIND THE REAL PROBLEM!!!
	if (result == ATM_CANCELLED)
	{
		result = ATM_NOERR;
		OILFontMan::InitialiseOutlineCache();
	}



	pDC->SelectObject(pOldFont);

	// delete any allocated dc
	if (LocalDC && pDC!=NULL)
	{
		delete pDC;
		pDC=NULL;
	}

	// Tell our path cache its all over
	OILFontMan::FinaliseOutlineCache();

	// return the resulting array details.
	if (result==0)
	{
		(*pNumCoords) = OILFontMan::GetOutlineCache(ppCoords,ppVerbs);
	}

	return (result==0);

OutlineError:
	if (LocalDC && pDC!=NULL)
		delete pDC;
	return FALSE;
}




/********************************************************************************************

>	static INT32 ATMFontMan::InstallFont(TCHAR *pMenuName, WORD Style, TCHAR *lpMetricsFile, TCHAR *lpFontFile)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Purpose:	See ATMFontMan::AddFont for details

********************************************************************************************/

INT32 ATMFontMan::InstallFont(TCHAR *pMenuName, WORD Style, TCHAR *lpMetricsFile, TCHAR *lpFontFile)
{
	INT32 err = BeginFontChange();

	if (err==ATM_NOERR)
		err = AddFont(pMenuName, Style, lpMetricsFile, lpFontFile);

	EndFontChange();

	return err;
}


/********************************************************************************************

>	INT32 ATMFontMan::DeInstallFont(TCHAR *pMenuName, WORD Style)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Purpose:	See ATMFontMan::RemoveFont for details

********************************************************************************************/

INT32 ATMFontMan::DeInstallFont(TCHAR *pMenuName, WORD Style)
{
	INT32 err = BeginFontChange();

	if (err==ATM_NOERR)
		err = RemoveFont(pMenuName, Style);

	EndFontChange();

	return err;
}


/////////////////////////////////////////////////////////////////////////////
// the call back functions to handle input coordinates
/////////////////////////////////////////////////////////////////////////////

BOOL FAR PASCAL EXPORT ATMFontMan::MyClosePath(LPSTR lpData)
{
	return OILFontMan::ClosePath();
}

BOOL FAR PASCAL EXPORT ATMFontMan::MyMoveTo(LPATMFixedPoint lpFixPnt, LPSTR lpData)
{
	POINT p;
	p.x = SCALEORD(lpFixPnt->x);
	p.y = SCALEORD(lpFixPnt->y);
	return OILFontMan::AddMoveTo(p);
}

BOOL FAR PASCAL EXPORT ATMFontMan::MyLineTo(LPATMFixedPoint lpFixPnt, LPSTR lpData)
{
	POINT p;
	p.x = SCALEORD(lpFixPnt->x);
	p.y = SCALEORD(lpFixPnt->y);
	return OILFontMan::AddLineTo(p);
}

BOOL FAR PASCAL EXPORT ATMFontMan::MyCurveTo(LPATMFixedPoint lpFixPnt1,
											 LPATMFixedPoint lpFixPnt2,
											 LPATMFixedPoint lpFixPnt3,
											 LPSTR lpData)
{
	POINT p0,p1,p2;
	p0.x = SCALEORD(lpFixPnt1->x);
	p0.y = SCALEORD(lpFixPnt1->y);
	p1.x = SCALEORD(lpFixPnt2->x);
	p1.y = SCALEORD(lpFixPnt2->y);
	p2.x = SCALEORD(lpFixPnt3->x);
	p2.y = SCALEORD(lpFixPnt3->y);
	return OILFontMan::AddBezierTo(p0,p1,p2);
}

/********************************************************************************************

>	INT32 APIENTRY ATMFontMan_CallBackCacheNamedFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	The call back function providing all font data

********************************************************************************************/

INT32 APIENTRY ATMFontMan_CallBackCacheNamedFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  
	)
{
    UNREFERENCED_PARAMETER (lpntm);

	if (FontType & DEVICE_FONTTYPE)
	{
		String_64 FontName(lpelf->elfLogFont.lfFaceName);

		if (lParam==NULL || (_tcsncicmp(FontName, *((String_64 * )lParam), 64) == 0))
		{
			FontManager* pFontMan = GetApplication()->GetFontManager();
			pFontMan->SetTempFont(FC_ATM, &FontName, lpelf);
			return FALSE;
		}
	}
    return TRUE;
}

/********************************************************************************************

>	INT32 APIENTRY ATMFontMan_CallBackValidateFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	The call back function providing all font data

********************************************************************************************/

INT32 APIENTRY ATMFontMan_CallBackValidateFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  
	)
{
    UNREFERENCED_PARAMETER (lpntm);

	if (FontType & DEVICE_FONTTYPE)
	{
		String_64 FontName(lpelf->elfLogFont.lfFaceName);

		FontManager* pFontMan = GetApplication()->GetFontManager();
		pFontMan->ValidateItem(FC_ATM, &FontName, lpelf);
	}
    return TRUE;
}

/********************************************************************************************

>	INT32 APIENTRY ATMFontMan_CallBackFindClosestFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96
	Purpose:	The call back function for searching for a match to a panose number

********************************************************************************************/

INT32 APIENTRY ATMFontMan_CallBackFindClosestFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  
	)
{
    UNREFERENCED_PARAMETER (lpntm);

	if (FontType & DEVICE_FONTTYPE)
	{
		String_64 FontName(lpelf->elfLogFont.lfFaceName);

		FontManager* pFontMan = GetApplication()->GetFontManager();
		return pFontMan->FindClosestFontFullTry(FC_ATM, &FontName, lpelf);
	}
	
    return TRUE;
}

/********************************************************************************************

>	INT32 APIENTRY ATMFontMan_CallBackDispatchFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	The call back function providing all font data

********************************************************************************************/

INT32 APIENTRY ATMFontMan_CallBackDispatchFont(
	ENUMLOGFONT FAR*  lpelf,		// address of logical-font data 
    NEWTEXTMETRIC FAR*  lpntm,		// address of physical-font data 
    INT32 FontType,					// type of font 
    LPARAM lParam 					// address of application-defined data  
	)
{
    UNREFERENCED_PARAMETER (lpntm);
	if (FontType & DEVICE_FONTTYPE)
	{
		// Hide aliases...
		if(!ATMInstall::IsFontAlias(lpelf->elfLogFont.lfFaceName))
		{
			return ((OILEnumFonts*)lParam)->NewFont(FC_ATM, lpelf);
		}
	}
	return TRUE;
}


/********************************************************************************************
/// New ATM interfacing code, implemented for the ATM for Windows NT4
********************************************************************************************/

/********************************************************************************************

>	static BOOL ATMFontMan::InitLib()

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/10/97
	Inputs:		-
	Outputs:	
	Returns:	TRUE if the proper ATM library is actually running and can be used on this OpSys
	Purpose:	Determine whether we can make further calls to the Adobe Type Manager
	Notes:		Implemented for the ATM for Windows NT4

********************************************************************************************/

BOOL ATMFontMan::InitLib()
{
	if (!ATMRunning)
		ATMRunning = (LibProperlyLoaded() > 0);

	if (ATMRunning)
	{
		ATMVersion = LibGetVersion();
		ATMUtilVersion = 0; // ThunkGetUtilVersion();

		UseThunking = FALSE;
	}

	return ATMRunning;
}

/********************************************************************************************

>	INT32 ATMFontMan::LibProperlyLoaded()

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/10/97
	Inputs:		-
	Outputs:	
	Returns:	>0 if the ATM manager is actually running and can be used on this OpSys
	Purpose:	Determine whether we can make further calls to the Adobe Type Manager
				This call has been in the ATM from version 1.00 so there's no need to check
				it.

********************************************************************************************/

INT32 ATMFontMan::LibProperlyLoaded()
{
	// Try and set up the dynamic links to the AtmLib dll.
	LPSTR LibName = "AtmLib.DLL";
	DLLHandle = LoadLibrary(LibName);
	if (DLLHandle == NULL)
	{
		return FALSE;
	}

	// extract the function pointers that we require
	pfnATMProperlyLoaded	= (PFN_ATMProperlyLoaded)	GetProcAddress( DLLHandle, "ATMProperlyLoaded");
	pfnATMGetVersion		= (PFN_ATMGetVersion)		GetProcAddress( DLLHandle, "ATMGetVersion");
	pfnATMGetOutline		= (PFN_ATMGetOutline)		GetProcAddress( DLLHandle, "ATMGetOutlineA");
	pfnATMForceExactWidth	= (PFN_ATMForceExactWidth)	GetProcAddress( DLLHandle, "ATMForceExactWidth");
	pfnATMBeginFontChange	= (PFN_ATMBeginFontChange)	GetProcAddress( DLLHandle, "ATMBeginFontChange");
	pfnATMEndFontChange		= (PFN_ATMEndFontChange)	GetProcAddress( DLLHandle, "ATMEndFontChange");
	pfnATMForceFontChange	= (PFN_ATMForceFontChange)	GetProcAddress( DLLHandle, "ATMForceFontChange");
	pfnATMFontStatus		= (PFN_ATMFontStatus)		GetProcAddress( DLLHandle, "ATMFontStatusA");
	pfnATMAddFont			= (PFN_ATMAddFont)			GetProcAddress( DLLHandle, "ATMAddFontA");
	pfnATMRemoveFont		= (PFN_ATMRemoveFont)		GetProcAddress( DLLHandle, "ATMRemoveFontA");

	// If we had a problem then unload the DLL and say so
	if (pfnATMProperlyLoaded	== NULL ||
		pfnATMGetVersion		== NULL ||
		pfnATMGetOutline		== NULL ||
//		pfnATMForceExactWidth	== NULL ||
		pfnATMBeginFontChange	== NULL ||
		pfnATMEndFontChange		== NULL ||
		pfnATMForceFontChange	== NULL ||
		pfnATMFontStatus		== NULL ||
		pfnATMAddFont			== NULL ||
		pfnATMRemoveFont		== NULL
		)
	{
		FreeLibrary(DLLHandle);					// remove DLL as no longer required
		DLLHandle = NULL;
		return FALSE;
	}

	// Otherwise, check that the DLL thinks that everything is ok
	INT32 result = (INT32)(*pfnATMProperlyLoaded)();
	return result;
}

/********************************************************************************************

>	static INT32 ATMFontMan::LibGetVersion()

	Author: 	Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/10/97
	Inputs:		None
	Outputs:	None
	Returns:	High byte major version, low byte minor version
	Purpose:	The version number of the currently running ATM.

********************************************************************************************/

INT32 ATMFontMan::LibGetVersion()
{
	if (pfnATMGetVersion == NULL)
	{
		return FALSE;
	}

	INT32 result = (INT32)(*pfnATMGetVersion)();
	
	return result;
}
