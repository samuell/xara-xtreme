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

// This code handles the interface between the kernel preference system, and 
// the platform's own particular method of storing preferences.


#include "camtypes.h"

//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "oilprefs.h"

#include "product.h"	// PRODUCT_OPTIONS_REGISTRYKEY


/********************************************************************************************

>	static OILPreferences* OILPreferences::Init()

	Author:		Luke_Hart & Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/Jun/2006
	Returns:	OILPreferences objects if initialisation succeeded, NULL otherwise.
	Purpose:	Performs any initialisation of the OIL layer preferences mechanisms that
				may fail.

********************************************************************************************/

OILPreferences* OILPreferences::Init()
{
	wxStandardPaths		Paths;
	wxString	strPath( Paths.GetUserConfigDir() );
	strPath += _T("/.xaralx");

	// Delete any file that exists where the directory should be
	if (wxFile::Exists(strPath))
		::wxRemoveFile(strPath);

	// Create directory iff not exist
	if( !wxDir::Exists( strPath ) )
		::wxMkdir( strPath );

	TRACEUSER( "jlh92", _T("OILPreferences::Init %s\n"), PCTSTR(strPath) );

	// Open config storage
	strPath += _T("/preferences");
	OILPreferences* pPrefs = new OILPreferences(_T("xaralx"), _T("Xara"), strPath);

	TRACEUSER( "jlh92", _T("OILPreferences::Init2 %s\n"), PCTSTR(strPath) );
	
	// Return the object we made or a NULL pointer if we failed
	return pPrefs;
}

/********************************************************************************************

>	void OILPreferences::WipePreferenceFile()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/93
	Purpose:	Remove all entries from a preference file.  This is done by deleting the
				file and creating a new one.

********************************************************************************************/

void OILPreferences::WipePreferenceFile()
{
	DeleteAll();
}

/********************************************************************************************

>	void OILPreferences::WipeDangerousPrefs()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/2001
	Purpose:	Remove potentially dangerous entries from the registry.

********************************************************************************************/

void OILPreferences::WipeDangerousPrefs()
{
	// wipe the potentially dangerous settings from the registry
	DeleteGroup(_T("Gallery"));
	DeleteGroup(_T("Options/Templates"));
	DeleteGroup(_T("Options/NewTemplates"));
	
//	DeleteRegKeyAndSubKeys (hAppStateRegKey, PRODUCT_REGISTRYKEY_GALLERY);
//	HKEY optsKey = OpenRegKey(hAppStateRegKey, PRODUCT_REGISTRYKEY_OPTIONS);
//	DeleteRegKeyAndSubKeys (optsKey, TEXT ("Templates"));
//	DeleteRegKeyAndSubKeys (optsKey, TEXT ("NewTemplates"));
//	CloseRegKey (optsKey);
}

/********************************************************************************************

>	void OILPreferences::Write(LPTCHAR Section, LPTCHAR PrefName, PreferenceType Type,
						       PrefData Data)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/93
	Inputs:		Section  - Name of the section to write the preference into.
				PrefName - Name of the preference itself.
				Type     - Type of the preference (INT32, UINT32, etc.)
				Data     - Union holding the pointer to the data to write out to the 
						   preference file.
	Purpose:	This function writes a given preference out to the application's own
				initialisation (profile/preference) file.
	SeeAlso:	OILPreferences; OILPreferences::Read

********************************************************************************************/

void OILPreferences::Write(LPTCHAR Section, LPTCHAR PrefName, PreferenceType Type,
						   PrefData Data)
{
	wxString strKey = _T("/Options/") + wxString(Section) + _T("/") + wxString(PrefName);

	BOOL Worked=FALSE;

	switch (Type)
	{
		case PREF_INT:
		{
			/*TYPENOTE: Correct*/ long l = (long)(*Data.pInt);
			Worked = wxConfig::Write(strKey, l);
			break;
		}
		
		case PREF_UINT:
		{
			// Note that unsigned cast to signed here will appear to store the wrong value
			// in the Config store but this apparent mistake will be rectified when the
			// value is read in and cast back to UINT32 again.
			// (Could write UINT32 prefs as strings...)
			//
			/*TYPENOTE: Correct*/ long l = (long)(*Data.pUInt);
			Worked = wxConfig::Write(strKey, l);
			break;
		}

		case PREF_DOUBLE:
			Worked = wxConfig::Write(strKey, (double)*(Data.pDouble));
			break;
			
		case PREF_STRING:
			Worked = wxConfig::Write(strKey, wxString((TCHAR*)*(Data.pString)));
//			Worked = wxConfig::Write(strKey, *(Data.pString));	// use this form when StringBase derived classes support direct conversion
			break;
			
		default:
			ENSURE(FALSE, "OILPreferences::Write() called with an illegal preference type");
			break;
	}

	if (!Worked)
	{
		ERROR3	("Error saving preference - See trace message for details");
		TRACE( _T("Error saving preference '%s'\n"), PrefName);
	}
}

/********************************************************************************************

>	void OILPreferences::Read(LPTCHAR Section, LPTCHAR PrefName, 
							  PreferenceType Type, PrefData pData)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/93
	Inputs:		Section  - The name of the preferences section to search.
				PrefName - The name of the preference to search for.
				Type     - The data-type of the preference.
				pData    - Pointer to the location in which to put the value read from the
						   preference file.
				MaxSize  - If the preference is a string, the size of the buffer that pData
						   points to.
	Outputs:	pData - the data pointed to by pData is updated from the preference file (if
						the preference was found - otherwise it is unchanged).
	Returns:	-
	Purpose:	This function attempts to read the given preference in the given section
				from the preference file.  No error is raised if the preference cannot be
				found, because this is a perfectly valid occurence - e.g. when a tool is
				being used for the first time, it has no preferences in the preference
				file.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

/********************************************************************************************

TECHNICAL NOTES

This function does not use the MFC GetProfileString() function becuase this returns a CString
and we're trying to avoid these in Camelot.  It uses the standard SDK function instead.

********************************************************************************************/

void OILPreferences::Read(LPTCHAR Section, LPTCHAR PrefName,
						  PreferenceType Type, PrefData pData)
{
	wxString strKey = _T("/Options/") + wxString(Section) + _T("/") + wxString(PrefName);
	
	switch (Type)
	{
		case PREF_INT:
		{
			// Use the value already in pData->Int as the value to return if the 
			// preference is not found.
			/*TYPENOTE: Correct*/ long l;
			if (wxConfig::Read(strKey, &l))
				*pData.pInt = (INT32)l; // Do not write directly as may be longer than 32 bits
			break;
		}

		case PREF_UINT:
		{
			// Note that signed value is read and cast directly into Unsigned memory
			// allocation reversing the effects fo the cast used in Write above...
			/*TYPENOTE: Correct*/ long l;
			if (wxConfig::Read(strKey, &l))
				*pData.pUInt = (UINT32)l; // Do not write directly as may be longer than 32 bits
			break;
		}
		case PREF_DOUBLE:
		{
			// Get the textual version of the double and convert it to a double.
			// default to null string
			wxConfig::Read(strKey, (double*)(pData.pDouble));
			break;
		}	
		case PREF_STRING:
		{
			// Just get the string - need to ask for the address of the String's
			// text buffer so we can pass it to the SDK profile API.
			wxString str;
			if (wxConfig::Read(strKey, &str))
			{
				str.Truncate(256);
				*(pData.pString) = (LPCTSTR)str;
//				*(pData.pString) = String_256(str);	// use this form when StringBase derived classes support direct conversion
			}
			break;
		}

		default:
			ENSURE(FALSE, "OILPreferences::Read() called with an illegal preference type");
			break;
	}
}

/********************************************************************************************

>	BOOL OILPreferences::OpenInput()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/93
	Returns:	TRUE if the INT32 file was successfully opened, FALSE otherwise.
	Purpose:	Prepares the preference system for reading in preferences.

********************************************************************************************/

BOOL OILPreferences::OpenInput()
{
	// Tell wxWidgets this is the default wxConfig object
	wxConfig::Set(this);

	return TRUE;
}

/********************************************************************************************

>	BOOL OILPreferences::CloseInput()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/93
	Returns:	TRUE if the input file was successfully closed, FALSE otherwise.
	Purpose:	Performs clean-up necessary when the preferences file is no longer needed
				for reading.

********************************************************************************************/

BOOL OILPreferences::CloseInput()
{
	wxConfig::Set(NULL);

	return TRUE;
}


/********************************************************************************************

>	BOOL OILPreferences::OpenOutput()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/93
	Returns:	TRUE if the output file was successfully opened, FALSE otherwise.
	Purpose:	Prepares the OIL for writing preferences out to the preferences file.

********************************************************************************************/

BOOL OILPreferences::OpenOutput()
{
	return TRUE;
}

/********************************************************************************************

>	BOOL OILPreferences::CloseOutput()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/93
	Returns:	TRUE if the output file was successfully closeed, FALSE otherwise.
	Purpose:	Closes the preference file when all the preferences have been written.

********************************************************************************************/

BOOL OILPreferences::CloseOutput()
{
	Flush();
	return TRUE;
}
