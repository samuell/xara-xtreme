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
// 


#include "camtypes.h"
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "tmpltarg.h"

#if defined( __WXMSW__ )
#include <comcat.h>		// should be in the slick oil registry.cpp
#endif

//#include "registry.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]


/********************************************************************************************

>	class WizOpProbe : public ClassCategoryProbe

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Purpose:	Searches through the (local) registry returning entries in the WizOp 
				category.

********************************************************************************************/
PORTNOTE("other","Removed WizOpProbe - derived from ClassCategoryProbe")
#ifndef EXCLUDE_FROM_XARALX
class WizOpProbe : public ClassCategoryProbe
{
protected:
	virtual const CATID& GetCategoryID() const	{	return s_WizOpCategoryCLSID;	}

private:
	static const CATID s_WizOpCategoryCLSID;
};
#endif


/********************************************************************************************

>	class WizOpRegistryEntry : public ClassRegistryEntry

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Purpose:	When the WizOpProbe returns a ClassRegistryEntry it returns one of these
				which can then be used to extract WizOp specific bits from the registry.

********************************************************************************************/
PORTNOTE("other","Removed WizOpRegistryEntry - derived from ClassRegistryEntry")
#ifndef EXCLUDE_FROM_XARALX
class WizOpRegistryEntry : public ClassRegistryEntry
{
public:
	BOOL GetInternalName(StringBase* const pValue);
	BOOL GetUserName(StringBase* const pValue);
	BOOL GetParamHint(StringBase* const pValue);

private:
//	static const String_8 s_InternalNameKey;
	static const String_16 s_UserNameKey;
	static const String_16 s_ParamHintKey;
};
#endif

// Place any IMPLEMENT type statements here
CC_IMPLEMENT_MEMDUMP(WizOp, ListItem)
CC_IMPLEMENT_MEMDUMP(WizOps, List)

PORTNOTE("other","Removed bulk of WizOp*")
#ifndef EXCLUDE_FROM_XARALX

#ifdef TEST_WIZOPS
const CATID WizOpProbe::s_WizOpCategoryCLSID = {0x40fc6ed5,0x2438,0x11cf,{0xA3,0xDB,0x08,0x00,0x36,0xF1,0x25,0x02}};
#else
// {21451410-E7E2-11d0-9AF6-0020AFE14B3D}
const CATID WizOpProbe::s_WizOpCategoryCLSID = { 0x21451410, 0xe7e2, 0x11d0, { 0x9a, 0xf6, 0x0, 0x20, 0xaf, 0xe1, 0x4b, 0x3d } };
#endif

const String_16 WizOpRegistryEntry::s_UserNameKey = TEXT("UserName");
const String_16 WizOpRegistryEntry::s_ParamHintKey = TEXT("ParamHint");
//const String_8 WizOpRegistryEntry::s_InternalNameKey = TEXT("");


// We want better memory tracking
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



// Functions follow

/********************************************************************************************

>	BOOL WizOpRegistryEntry::GetInternalName(StringBase* const pValue)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Provides a string from this registry entry for the internal name

	Returns:	TRUE if it worked, FALSE if it didn't
	Outputs:	pValue:	The string of the internal name.

********************************************************************************************/
BOOL WizOpRegistryEntry::GetInternalName(StringBase* const pValue)
{
	BOOL Ok = TRUE;

	VersionIndependentProgID VIPID(*this);
	String_128 WholeEntry;

	Ok = VIPID.GetName(&WholeEntry);

	if (Ok)
	{
		// The entry is of the form "ProgID.LongName". We want the LongName bit
		static const TCHAR ProgIDDelimiter = TEXT('.');

		INT32 StartDelimiterPosition = WholeEntry.FindNextChar(ProgIDDelimiter);

		const UINT32 LengthToCopy = WholeEntry.Length() - StartDelimiterPosition - 1;

		if (StartDelimiterPosition >= 0 && LengthToCopy > 0)
		{
			WholeEntry.Mid(pValue, StartDelimiterPosition + 1, LengthToCopy);
		}
		else
		{
			TRACE( _T("WizOpRegistryEntry::GetInternalName - no name\n"));
			Ok = FALSE;
		}
	}

	return Ok;
}


/********************************************************************************************

>	BOOL WizOpRegistryEntry::GetUserName(StringBase* const pValue)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Provides a string from this registry entry for the user name

	Returns:	TRUE if it worked, FALSE if it didn't
	Outputs:	pString:	The string of the user name.

********************************************************************************************/
BOOL WizOpRegistryEntry::GetUserName(StringBase* const pValue)
{
	return GetValue(s_UserNameKey, pValue);
}


/********************************************************************************************

>	BOOL WizOpRegistryEntry::GetParamHint(StringBase* const pValue)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Provides a string from this registry entry for the parameter hint

	Returns:	TRUE if it worked, FALSE if it didn't
	Outputs:	pString:	The string of the parameter hint

********************************************************************************************/
BOOL WizOpRegistryEntry::GetParamHint(StringBase* const pValue)
{
	return GetValue(s_ParamHintKey, pValue);
}



/*
WizOps& g_WizOps()
{
	static WizOps g_WizOps;

	return g_WizOps;
}
*/

#endif

WizOps& GetWizOps()
{
	// This static dummy object is used for returning in the ERROR2 case
	// The original used to return a ptr to a local variable, which is a tad dangerous.
	// This solution is not ideal, because there's a permanent instance of an object
	// that will probably never be used.
	static WizOps DummyOps;

	WizOps* const pWizOps = GetApplication()->GetWizOps();
	ERROR2IF(pWizOps == NULL, DummyOps, "No WizOps");

	return *pWizOps;
}




/********************************************************************************************

>	WizOps::~WizOps()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	We need an overridden destructor to delete the WizOp's we stuck on the heap.

********************************************************************************************/
WizOps::~WizOps()
{
//	TRACE( _T("Deleting those WizOps now\n"));
	DeleteAll();
}



/********************************************************************************************

>	BOOL WizOps::InitializeFromRegistry()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Goes whizzing through the registry to find all the WizOp's in it. Then
				builds this WizOps collection using those entries.

********************************************************************************************/
BOOL WizOps::InitializeFromRegistry()
{

	PORTNOTETRACE("other"," WizOps::InitializeFromRegistry - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BOOL Ok = TRUE;
	WizOpProbe Probe;
	GetLocalRegistry()->InitializeProbe(Probe);
	
	while (!Probe.IsLast() && Ok)
	{
		WizOpRegistryEntry Entry;
		// The internal name is given by the registry entry
		Ok = Probe.GetNextEntry(&Entry);
		
		String_64 InternalName;
		if (Ok)
		{	
			Ok = Entry.GetInternalName(&InternalName);
		}

		String_64 UserName;
		if (Ok)
		{	
			Entry.GetUserName(&UserName);
			if (UserName.IsEmpty())
			{
				UserName = InternalName;
			}
		}

		String_256 ParamHint;
		if (Ok)
		{	
			Entry.GetParamHint(&ParamHint);
		}

		if (Ok)
		{	
			Ok = AddWizOp(InternalName, UserName, ParamHint);
		}
	}

	if (!Ok)
	{
		TRACE( _T("Not all WizOps were created properly\n"));
	}
#endif
	return TRUE;	// we'll pretend it worked 'cos otherwise camelot won't start
}


/********************************************************************************************

>	BOOL WizOps::AddWizOp(	const StringBase& InternalName, const StringBase& UserName,
							const StringBase& ParamHint)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Adds a WizOp with the given gubbins to this set.

	Returns:	TRUE if if worked FALSE if it didn't

********************************************************************************************/
BOOL WizOps::AddWizOp(	const StringBase& InternalName, const StringBase& UserName,
						const StringBase& ParamHint)
{
	BOOL Success = TRUE;

	WizOp* const pNewWizOp = new WizOp(InternalName, UserName, ParamHint);

	if (pNewWizOp != NULL)
	{
		AddTail(pNewWizOp);
	}
	else
	{
		Success = FALSE;
	}

	return Success;
}



/********************************************************************************************

>	WizOp* WizOps::GetFirstWizOp() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Cheapskate duplicate code container enumeration

	Returns:	The first WizOp in the set

********************************************************************************************/
WizOp* WizOps::GetFirstWizOp() const
{
	return (WizOp*)GetHead();
}


/********************************************************************************************

>	WizOp* WizOps::GetNextWizOp(const WizOp* const pWizOp) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Cheapskate duplicate code container enumeration

	Inputs:		pWizOp:	Don't know why it's a pointer.
	Returns:	The WizOp after the given one in the set

********************************************************************************************/
WizOp* WizOps::GetNextWizOp(const WizOp* const pWizOp) const
{
	return (WizOp*)GetNext(pWizOp);
}


/********************************************************************************************

>	WizOp* WizOps::FindWizOpFromInternalName(const StringBase& InternalName) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Allows access to the WizOp's vital statistics

	Inputs:		InternalName :	The InternalName of the desired WizOp
	Returns:	NULL if none found
				The WizOp with the InternalName (CLSID), given in the constructor,
				InternalName.

********************************************************************************************/
WizOp* WizOps::FindWizOpFromInternalName(const StringBase& InternalName) const
{
	WizOp* pWizOp = GetFirstWizOp();

	BOOL Found = FALSE;
	while (pWizOp != NULL && !Found)
	{
		if (pWizOp->GetInternalName() == InternalName)
		{
			Found = TRUE;
		}

		if (!Found)
		{
			pWizOp = GetNextWizOp(pWizOp);
		}
	}

	return pWizOp;
}






// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//									W  I  Z  O  P
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //






/********************************************************************************************

>	WizOp::WizOp(	const StringBase& InternalName, const StringBase& UserName, 
					const StringBase& ParamHint)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This constructor provides the Wizard Properties with something to talk about.

	Inputs:		InternalName :	a persistant identifier for the WizOp. The CLSID of the entry
								extracted from the registry would be good.
				UserName :		the text that will appear in the Wizard Properties dialog.
								The "UserName" subkey under the InternalName (CLSID) is good.
				ParamHint:		If the WizOp can accept a parameter this string should
								contain some text explaining what it is.

********************************************************************************************/
WizOp::WizOp(	const StringBase& InternalName, const StringBase& UserName, 
				const StringBase& ParamHint) :
	m_Name(InternalName),
	m_UserName(UserName),
	m_ParamHint(ParamHint)
{
}


/********************************************************************************************

>	const StringBase& WizOp::GetInternalName() const
>	const StringBase& WizOp::GetUserName() const
>	const StringBase& WizOp::GetParamHint() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Allows access to the WizOp's vital statistics

	Returns:	Strings of things

********************************************************************************************/
const StringBase& WizOp::GetInternalName() const
{
	return (StringBase&)m_Name;
}


const StringBase& WizOp::GetUserName() const
{
	return (StringBase&)m_UserName;
}


const StringBase& WizOp::GetParamHint() const
{
	return (StringBase&)m_ParamHint;
}
