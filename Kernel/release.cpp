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

// ReleaseInfo class - informs program about release information

/*
*/

#include "camtypes.h"
#include "release.h"
#include "brandinf.h"
//#include "justin2.h"
#include "app.h"		// Camelot object

// these are packed so that a brander program can easily find & verify them

// lets declare a Unicode version of the name, which will be much harder to find

// put these in a different segment to the others
#pragma data_seg( "CAMELOT", "DATA" )
struct
{
	UINT32 Magic;
	UINT32 Serial;
	wchar_t Name[BRANDED_NAME_MAX];
	wchar_t Company[BRANDED_NAME_MAX];
} SecretInfo =
{
	BRANDED_MAGIC2,
	BRANDED_SERIALNUMBER ^ BRANDED_XOR,
	BRANDED_NAME_UNICODE,
	BRANDED_COMPANY_UNICODE
};

#pragma data_seg()
struct
{
	UINT32	Magic;
	UINT32	Serial;
	char	Name[BRANDED_NAME_MAX];
	char	Company[BRANDED_NAME_MAX];
} PublicInfo =
{
	BRANDED_MAGIC1,
	BRANDED_SERIALNUMBER,
	BRANDED_NAME,
	BRANDED_COMPANY
};

// bug - before 28/2/97 only allocated 10 chars so when it puts 101- and then the
// number to 9 places it will overflow into the next string. This string dictates
// the key name it is written under. So it will be written/read from the wrong section.
static TCHAR szCachedSerial[25]	= _T("");

// Pseudo-Preferences
static TCHAR szUISection[]		= _T("UserInfo");

static TCHAR szSerialNumber[]	= _T("SerialNo");
static TCHAR szLicensee[]		= _T("Licensee");
static TCHAR szCompany[]		= _T("Company");

//***************************************************************************
// this returns the obvious user name. Not hidden at all


// Why was this class written to use type char, not TCHAR?   This is a royal pain in the backside.

const char *ReleaseInfo::GetInfo()
{
	static char buf[256];

#ifndef STANDALONE
//	wsprintf(buf, "%s (#101-%09ld)", PublicInfo.Name, PublicInfo.Serial );
//	wsprintf(buf, TEXT("%s (#%s)"), ReleaseInfo::GetLicensee(), ReleaseInfo::GetSerialNumber());

	String_256 jcf;
	jcf.MakeMsg(_R(IDS_RELEASE_INFO_FORMAT), ReleaseInfo::GetLicensee(), ReleaseInfo::GetSerialNumber());
	camStrcpy(buf, jcf);

#else
	camStrcpy(buf, PublicInfo.Name);
#endif

	return buf;
}

//***************************************************************************

const char* ReleaseInfo::GetLicensee()
{
	static char buffer[64];
#ifdef _DEBUG
	char* pLogName=_tgetenv("LOGNAME");
	if (pLogName!=NULL)
		wsprintf(buffer,"%s",pLogName);
	else
		wsprintf(buffer,"%s","NoLogName");
#else
	// read the licensee preference value into a string
	String_256 Str;
	BOOL ok = Camelot.GetPrefDirect(szUISection, szLicensee, &Str);
	if (ok) 
	{
		// copy the string into the return buffer
		TCHAR* StringBuf = (TCHAR*)Str;
		wsprintf(buffer, "%s", StringBuf);
	}
	else
	{
		// something went wrong so use a default, the bound in one
		wsprintf(buffer,"%s",PublicInfo.Name);
	}
#endif

	return buffer;
}

//***************************************************************************

const char* ReleaseInfo::GetSerialNumber()
{
	static char buffer[64];

	// read the serial number preference value into a string
	String_256 Str;
	BOOL ok = Camelot.GetPrefDirect(szUISection, szSerialNumber, &Str);
	if (ok) 
	{
		// copy the string into the return buffer
		TCHAR* StringBuf = (TCHAR*)Str;
		wsprintf(buffer, "%s", StringBuf);
	}
	else
	{
		// something went wrong so use a default, the bound in one
		wsprintf(buffer,"101-%09ld",PublicInfo.Serial);
	}

	return buffer;
}

//***************************************************************************

const char* ReleaseInfo::GetCompany()
{
	static char buffer[64];
	// read the company preference value into a string
	String_256 Str;
	BOOL ok = Camelot.GetPrefDirect(szUISection, szCompany, &Str);
	if (ok) 
	{
		// coy the string into the return buffer
		TCHAR* StringBuf = (TCHAR*)Str;
		wsprintf(buffer, "%s", StringBuf);
	}
	else
	{
		// something went wrong so use a default, the bound in one
		wsprintf(buffer,"%s",PublicInfo.Company);
	}

	return buffer;
}



/***********************************************************************************************

> 	void ReleaseInfo::CacheProfileSettings()

    Author: 	Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
    Created:	10/09/96
    Purpose:    Stores the current ReleaseInfo settings from the .ini file to an internal buffer
				This is currently used to prevent user information being lost when the default
				user profile is restored.
	Scope:		static

	See Also:	RestoreProfileSettings()
                   			                                     
***********************************************************************************************/
void ReleaseInfo::CacheProfileSettings()
{
	strcpy(PublicInfo.Name, GetLicensee());
#ifndef WEBSTER
	// No serial number in Webster v2.0.
	// This happens too early to get resources.
	strcpy(szCachedSerial, GetSerialNumber());
#endif
	strcpy(PublicInfo.Company, GetCompany());
}

/***********************************************************************************************

> 	BOOL ReleaseInfo::RestoreProfileSettings()

    Author: 	Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
    Created:	10/09/96
	Returns:	TRUE if successful
				FALSE otherwise
    Purpose:    Restores the current ReleaseInfo settings in the .ini file to the values
				present in the cache
				This is currently used to prevent user information being lost when the default
				user profile is restored.
	Scope:		static

	See Also:	CacheProfileSettings()
                   			                                     
***********************************************************************************************/
BOOL ReleaseInfo::RestoreProfileSettings()
{
	if (!WriteProfile(szSerialNumber, szCachedSerial))
	{
		return FALSE;
	}

	if (!WriteProfile(szLicensee, PublicInfo.Name))
	{
		return FALSE;
	}

	if (!WriteProfile(szCompany, PublicInfo.Company))
	{
		return FALSE;
	}

	return TRUE;
}


/***********************************************************************************************

> 	BOOL ReleaseInfo::WriteProfile(LPCTSTR pKey, LPCTSTR pValue)

    Author: 	Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
    Created:	10/09/96
	Inputs:		pKey:	The key in the .ini file
				pValue:	The value to which to set it
	Returns:	TRUE if successful
				FALSE otherwise
	Scope:		static protected

    Purpose:    Support function for RestoreCurrentSettings()
				Writes an .ini file setting in the UserInfo section of the current apps profile
				file using ::WritePrivateProfileString() and the given key and value.
                   			                                     
***********************************************************************************************/
BOOL ReleaseInfo::WriteProfile(LPCTSTR pKey, LPCTSTR pValue)
{
	ERROR2IF(pKey == NULL || pValue == NULL,FALSE,"ReleaseInfo::WriteProfile Bad params!");

	// This will be done when the ini file is being vaped. Hence it will happen very soon after
	// start up. If this happens to be the first time Camelot is started up then we may be reading
	// from ini files and wanting to write the data into the registry. If we do this now then we 
	// will be writing into the ini file as we only switch over to the registry when we are
	// destructing Camelot. So we must make sure that this info is written out into the registry.
	// If we do it now then the registry section may be vaped before we write the real data out.
	// We will then need to ensure that if we switch from ini file to registry mode that the data
	// is saved out before and restored into the new place afterwards.

	String_256 Key(pKey);
	String_256 Section(szUISection);
	// don't use this as for some reason it seems to be corrupt
	// and says Tips when it comes to calling this
	if (!Camelot.SetPrefDirect(Section, Key, pValue, TRUE))
		return FALSE;

	return TRUE;
}
