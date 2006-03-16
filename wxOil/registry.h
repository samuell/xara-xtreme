// $Id: registry.h 662 2006-03-14 21:31:49Z alex $
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

	registry.h

	Interface to the system registry, which provides a container for storing information about
	the app.  Under Windows NT / Windows 95 this is placed in the Registry, under Windows 3.1
	it is placed in an .INI file.
*/

#ifndef REGISTRY_H
#define REGISTRY_H 


#define SAVE_PREFS_TO_REGISTRY

// Initialises our registry entries.
BOOL InitRegistry();
BOOL DeInitRegistry();

// This string is our special Application identifier.
extern TCHAR szCamelotAppRegistryID[];

// This holds the full path to our executable.
extern TCHAR szCamelotExePath[_MAX_PATH];

// This handle is maintained by REGISTRY.CPP.  All application state info should be
// stored under this key.
extern HKEY hAppStateRegKey;

// These create or open a registry key, and set a value within it.
HKEY CreateRegKey(HKEY hKey, LPCTSTR lpcszSubKey, LPTSTR lpszClass = NULL);
HKEY OpenRegKey(HKEY hKey, LPCTSTR lpcszSubKey);
HKEY OpenRegKeyReadOnly(HKEY hKey, LPCTSTR lpcszSubKey);
BOOL CloseRegKey(HKEY hKey);
BOOL GetRegValue(HKEY hKey, LPTSTR szValueName, LPDWORD pdwType,
				 LPVOID pvBuf, LPDWORD pdwBufSize);
BOOL SetRegValue(HKEY hKey, LPCTSTR lpcszValueName, DWORD dwType,
				 const VOID* pcData, DWORD cbData);

// Useful variations on the main functions
BOOL	SetRegDword(HKEY hKey, LPCTSTR lpcszValueName, DWORD Value);
BOOL	SetRegInt(HKEY hKey, LPCTSTR lpcszValueName, INT32 Value);
BOOL	SetRegBool(HKEY hKey, LPCTSTR lpcszValueName, BOOL Value);
BOOL	SetRegString(HKEY hKey, LPCTSTR lpcszValueName, String_256* str);
BOOL	SetRegString(HKEY hKey, LPCTSTR lpcszValueName, TCHAR * str);

// These are much more convenient, but don't have very sophisticated error reporting.
INT32		GetRegInt(HKEY hKey, LPCTSTR szValueName);
BOOL	GetRegBool(HKEY hKey, LPCTSTR szValueName);
DWORD	GetRegDword(HKEY hKey, LPCTSTR szValueName);
BOOL	GetRegString(HKEY hKey, LPCTSTR szValueName, String_256* pOut);
BOOL	GetRegString(HKEY hKey, LPCTSTR szValueName, TCHAR * str, UINT32 str_len = 256);

// This deletes the given sub-key of hKey.
BOOL DeleteRegKey(HKEY hKey, LPCTSTR szSubKey);
// This deletes the given sub-key and any sub-keys of that
BOOL DeleteRegKeyAndSubKeys(HKEY hKey, LPCTSTR szSubKey);

// Useful general functions
String_256 GetProgramNameRegistryKey();
BOOL CheckForRegistryEntry(TCHAR * SubKeyName);
BOOL ConvertCLSIDToString(const CLSID& ID, StringBase* const pString);

/****************************************************************************

>	class SaveRegistryEntries : public CC_CLASS_MEMDUMP

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/97
	Purpose:	This class can be used for saving a list of items into the
				registry.

****************************************************************************/

class SaveRegistryEntries : public CC_CLASS_MEMDUMP
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(SaveRegistryEntries);
public:
	SaveRegistryEntries() { m_hRegKey = NULL; m_Counter = 0; };
	~SaveRegistryEntries();

public:		// Scanning through a set of particular files in a directory
	BOOL StartSaving(String_256 *pKeySpecifier, HKEY hSubKey = hAppStateRegKey);
	BOOL SaveNextEntry(String_256 *pNextName);
	BOOL StopSaving();

	HKEY GetRegKey() { return m_hRegKey; }

private:	// Private data
	HKEY m_hRegKey;	// the key we are saving under
	INT32	m_Counter;	// a counter of the items saved
};

/****************************************************************************

>	class LoadRegistryEntries : public CC_CLASS_MEMDUMP

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/97
	Purpose:	This class can be used for loading a list of items into the
				registry.

****************************************************************************/

class LoadRegistryEntries : public CC_CLASS_MEMDUMP
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(LoadRegistryEntries);
public:
	LoadRegistryEntries() { m_hRegKey = NULL; m_Counter = 0; };
	~LoadRegistryEntries();

public:		// Scanning through a set of particular files in a directory
	BOOL StartLoading(String_256 *pKeySpecifier, HKEY hSubKey = hAppStateRegKey);
	BOOL LoadNextEntry(String_256 *pNextName);
	BOOL StopLoading();

	HKEY GetRegKey() { return m_hRegKey; }
	INT32  GetNumberOfItemsLoaded() { return m_Counter; }

private:	// Private data
	HKEY m_hRegKey;	// the key we are saving under
	INT32	m_Counter;	// a counter of the items saved
};


extern const String_8 NullString;

/********************************************************************************************

>	class Registry

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	There is more than one registry you know. 
				At present, this class is only ever the local one (honest)

********************************************************************************************/
class ClassCategoryProbe;

class Registry
{
public:
//	static String_256 GetProgramNameRegistryKey();

//	Registry();
//	BOOL InitRegistry();

//	BOOL DeInitRegistry()
//	~Registry();

//	BOOL CheckForRegistryEntry(const StringBase& SubKeyName);
//	BOOL UpdateInstanceVersion(	const StringBase& ThisVersion,
//								DWORD dwThisBuildMajor, DWORD dwThisBuildMinor);

	BOOL InitializeProbe(ClassCategoryProbe& Probe);
private:
};


/********************************************************************************************

>	class RegistryEntry

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Represents an entry in the registry consisting of a key, subkeys, and some
				values. 

	Notes:		It can't be instantiated, but derived classes should provide implementation
				to initialize the HKEY held herein.

	See Also:	ClassRegistryEntry

********************************************************************************************/
class RegistryEntry
{
public:
	virtual ~RegistryEntry();

	virtual BOOL GetValue(const StringBase& ValueName, StringBase* const pValue);
	virtual BOOL GetEmbeddedEntry(const StringBase& Name, RegistryEntry* const pEntry) const;

protected:
	RegistryEntry(HKEY hKey = 0) : m_hKey(hKey)	{}
	RegistryEntry(const RegistryEntry&);

	
protected:
	HKEY	m_hKey;
};



/********************************************************************************************

>	class ClassRegistryEntry : public RegistryEntry

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Represents an entry in CLSID section of the registry, consisting of a key, 
				subkeys, and some values.

********************************************************************************************/
class ClassRegistryEntry : public RegistryEntry
{
public:
	BOOL InitFromCLSID(const CLSID& ClassID);

private:
	static const String_8	s_CLSIDString;
};


/********************************************************************************************

>	class VersionIndependentProgID : public RegistryEntry

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Represents the VersionIndependentProgID key in a registry.
				Saves us some typing really.

********************************************************************************************/
class VersionIndependentProgID : public RegistryEntry
{
public:
	VersionIndependentProgID(const ClassRegistryEntry& CLSIDEntry);

	BOOL GetName(StringBase* const pName);

private:
	static const String_32 s_KeyName;
};


/********************************************************************************************

>	class ClassCategoryProbe //: public RegistryProbe

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Allows us to whizz through a component category picking out all the registry
				entries for it.
				At present, this class is only ever associated with the local registry

********************************************************************************************/
#pragma warning( disable : 4091 )	// otherwise it wants <comcat.h> included
typedef CATID;
#pragma warning( default : 4091 )
struct ICatInformation;
struct IEnumCLSID;

class ClassCategoryProbe //: public RegistryProbe
{
public:
	ClassCategoryProbe();
	ClassCategoryProbe(const ClassCategoryProbe& OtherProbe);
	~ClassCategoryProbe();

	BOOL Init();

	BOOL GetNextEntry(ClassRegistryEntry* const pEntry);
	BOOL IsLast() const;

protected:
	// Implementation
	BOOL GetNextCLSID(CLSID* const pNextCLSID);
	ICatInformation* CreateCatInformer();

	virtual const CATID& GetCategoryID() const = 0;

private:
	IEnumCLSID*		m_pEnumCLSID;
	CLSID			m_NextCLSIDToReturn;

	static const String_256	s_CLSIDKey;
};


Registry* GetLocalRegistry();


inline RegistryEntry::~RegistryEntry()
{
	CloseRegKey(m_hKey);
}

inline RegistryEntry::RegistryEntry(const RegistryEntry&)
{	
	TRACE( _T("RegistryEntry - Copy constructor not implemented\n"));
}


#endif	// REGISTRY_H
