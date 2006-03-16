// $Id: scunit.cpp 662 2006-03-14 21:31:49Z alex $
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
//#include "richard3.h"
#include "convert.h"
#include "scunit.h"

CC_IMPLEMENT_MEMDUMP(Qualifier, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(PrefixQualifier, Qualifier)
CC_IMPLEMENT_MEMDUMP(SuffixQualifier, Qualifier)
CC_IMPLEMENT_MEMDUMP(NullQualifier, Qualifier)
CC_IMPLEMENT_DYNAMIC(ScaleUnit, ListItem)
CC_IMPLEMENT_MEMDUMP(UnitGroup, CC_CLASS_MEMDUMP)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

#define ERROR2IFNOTCLASS(pc, classname)		ERROR2IF(pc == NULL || !pc->IS_KIND_OF(classname), \
											FALSE, #classname "is not");

/********************************************************************************************

>	Qualifier::Qualifier()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Purpose:	Default constructor.
	Scope:		public
	SeeAlso:	class Qualifier

********************************************************************************************/
Qualifier::Qualifier()
{
	m_bShow = FALSE;
}


/********************************************************************************************

>	Qualifier& Qualifier::operator=(const Qualifier& OtherQualifier)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Purpose:	Assignment. Required for CC_CLASS_MEMDUMP
	Scope:		public

********************************************************************************************/
Qualifier& Qualifier::operator=(const Qualifier& OtherQualifier)
{
	if (&OtherQualifier == this) return *this;

	m_bShow = OtherQualifier.m_bShow;
	m_Token = OtherQualifier.m_Token;
	return *this;
}




/********************************************************************************************

>	BOOL Qualifier::SetAttributes(const String_32& NewToken, QUALIFIER_SHOWN WhetherShown)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		ParseString : a string of the form "%sTOKEN" or "TOKEN%s" signifying the
				position of the qualifier token (TOKEN) relative to the value it's qualifying
				(%s).
				WhetherShown :either QUALIFIER_SHOWN::IS_SHOWN or QUALIFIER_SHOWN::IS_NOTSHOWN
	Returns:	TRUE if set ok
				FALSE otherwise
	Purpose:	Sets the Qualifier string for the unit. This is an abbreviation for a unit.
				E.g. "%" specifies percentage.
				Only characters returning TRUE from Convert::IsCharUnitType() are permitted
				in the qualifier token.
	Errors:		Parameter validation
	Scope:		public
	SeeAlso:	ScaleUnit::GetQualifier()

********************************************************************************************/
BOOL Qualifier::SetAttributes(const String_32& NewToken, QUALIFIER_SHOWN WhetherShown)
{
	ERROR2IF(WhetherShown != IS_SHOWN && WhetherShown != IS_NOTSHOWN, FALSE, "Not a QUALIFIER_SHOWN");

	INT32 nTokenLength = NewToken.Length();	
	if (nTokenLength == 0)
	{
		ERROR3("Qualifier::SetAttributes - token zero length");
		return FALSE;
	}
	// Check for the existence of possible clash characters in the token.
	for (INT32 i = 0; i < nTokenLength; ++i)
	{
		// Get the next character to be checked in the new specifier string
		if (!Convert::IsCharUnitType(NewToken[i]))
		{
			// We found a problem character so return this result to the caller
			ERROR3("Qualifier::SetAttributes - invalid char in token");
			return FALSE;
		}
	}
	m_Token = NewToken;

	switch (WhetherShown)
	{
		case IS_SHOWN:
			m_bShow = TRUE;
			break;

		case IS_NOTSHOWN:
			m_bShow = FALSE;
			break;

		default:		// should never happen
			return FALSE;
	}
	return TRUE;
}



/********************************************************************************************

>	String_32 Qualifier::GetToken() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Returns:	The token represeting the qualifier in a string
	Purpose:	To determine this Qualifier's representation in a string
	Scope:		public

********************************************************************************************/
String_32 Qualifier::GetToken() const
{
	return m_Token;
}



/********************************************************************************************

>	BOOL Qualifier::IsShown() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Returns:	TRUE : If qualifier token should be shown in a display string
				FALSE : otherwise
	Purpose:	Determine whether to display the token representing the qualifier
	Scope:		public

********************************************************************************************/
BOOL Qualifier::IsShown() const
{

	return (m_bShow == IS_SHOWN);
}


/********************************************************************************************

>	BOOL PrefixQualifier::MakeDisplayString(const StringBase& InString, StringBase* pOutString) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		InString : a string to display with the qualifier
	Outputs:	pOutString : pOutString concatenated with this Qualifier and InString
	Returns:	TRUE if string formed correctly
				FALSE otherwise
	Purpose:	We can create a string to display with the qualifier positioned correctly
	Errors:		Parameter validation
	Scope:		public
	SeeAlso:	Qualifier::SetAttributes()

********************************************************************************************/
BOOL PrefixQualifier::MakeDisplayString(const StringBase& InString, StringBase* pOutString) const
{
	if (IsShown())
	{
		*pOutString += GetToken();
	}
	*pOutString += InString;
	return TRUE;
}


/********************************************************************************************

>	BOOL SuffixQualifier::MakeDisplayString(const StringBase& InString, StringBase* pOutString) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		InString : a string to display with the qualifier
	Outputs:	pOutString : pOutString concatenated with InString & this Qualifier
	Returns:	TRUE if string formed correctly
				FALSE otherwise
	Purpose:	We can create a string to display with the qualifier positioned correctly
	Errors:		Parameter validation
	Scope:		public
	SeeAlso:	Qualifier::SetAttributes()

********************************************************************************************/
BOOL SuffixQualifier::MakeDisplayString(const StringBase& InString, StringBase* pOutString) const
{
	*pOutString += InString;
	if (IsShown())
	{
		*pOutString += GetToken();
	}
	return TRUE;
}


BOOL NullQualifier::MakeDisplayString(const StringBase& InString, StringBase* pOutString) const
{
	*pOutString += InString;
	return TRUE;
}













/********************************************************************************************

>	ScaleUnit::ScaleUnit()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Purpose:	Default constructor for a ScaleUnit giving a scale of 0 to 1
	Scope:		public

********************************************************************************************/
ScaleUnit::ScaleUnit()
{
	m_dScale = 1.0;
	m_dMin = 0.0;
	m_dMax = 1.0;
	m_nDPtoShow = 0;
	m_bInternalQualifier = FALSE;
}

/********************************************************************************************

>	ScaleUnit::ScaleUnit(const double dScale, const double dMin, const double dMax)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		dScale : The unit's scale
				dMin : The unit's minimum for which the scale is defined
				dMax : The unit's minimum for which the scale is defined
	Purpose:	Constructor for a ScaleUnit
	Scope:		Public

********************************************************************************************/

ScaleUnit::ScaleUnit(const double dScale, const double dMin, const double dMax)
{
	m_dScale = dScale;
	m_dMin = dMin;
	m_dMax = dMax;
	m_bInternalQualifier = FALSE;
}


/********************************************************************************************

>	ScaleUnit::ScaleUnit(const double dScale, const UINT32 nDPtoShow)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		dScale : The unit's scale
				nDPtoShow : The number of decimal places that should be shown when the number
				is scaled by this unit
	Purpose:	Constructor for a ScaleUnit, with a range 0..dScale.
	Scope:		Public

********************************************************************************************/

ScaleUnit::ScaleUnit(const double dScale, const UINT32 nDPtoShow)
{
	m_dScale = m_dMax = dScale;
	m_dMin = 0.0;
	m_nDPtoShow = nDPtoShow;
	m_bInternalQualifier = FALSE;
}

/********************************************************************************************

>	ScaleUnit::ScaleUnit(const ScaleUnit& UnitToCopy)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		UnitToCopy : the ScaleUnit from which to construct a replica
	Purpose:	Copy constructor

********************************************************************************************/

ScaleUnit::ScaleUnit(const ScaleUnit& UnitToCopy)
{
	if (this == &UnitToCopy) return;

	m_dScale = UnitToCopy.m_dScale;
	m_dMin = UnitToCopy.m_dMin;
	m_dMax = UnitToCopy.m_dMax;
	m_pQualifier = UnitToCopy.m_pQualifier;
	m_nDPtoShow = UnitToCopy.m_nDPtoShow;
	m_bInternalQualifier = FALSE;
}


/********************************************************************************************

>	ScaleUnit& ScaleUnit::operator = (const ScaleUnit& UnitToCopy)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		UnitToCopy : ScaleUnit to which this will be assigned
	Purpose:	Assignment operator

********************************************************************************************/
ScaleUnit& ScaleUnit::operator=(const ScaleUnit& UnitToCopy)
{
	if (this == &UnitToCopy) return *this;

	m_dScale = UnitToCopy.m_dScale;
	m_dMin = UnitToCopy.m_dMin;
	m_dMax = UnitToCopy.m_dMax;
	m_pQualifier = UnitToCopy.m_pQualifier;
	m_nDPtoShow = UnitToCopy.m_nDPtoShow;
	m_bInternalQualifier = FALSE;

	return *this;
}


/********************************************************************************************

>	ScaleUnit::~ScaleUnit()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Purpose:	Default destructor for a ScaleUnit - does nothing
	Scope:		public

********************************************************************************************/

ScaleUnit::~ScaleUnit()
{
	if (m_bInternalQualifier && m_pQualifier != NULL)
		delete m_pQualifier;
}



/*********************************************************************************************

>	BOOL ScaleUnit::operator == (const ScaleUnit& rhsUnit) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		rhsUnit : the unit on the right-hand side of the equality test
	Returns:	TRUE if this ScaleUnit is equal to rhsUnit
				FALSE otherwise
	Purpose:	Compares two ScaleUnits for equality
	Scope:		public
	SeeAlso:	class ScaleUnit

**********************************************************************************************/ 
BOOL ScaleUnit::operator==(const ScaleUnit& rhsUnit) const
{
	return (m_dScale == rhsUnit.m_dScale &&
			m_pQualifier->GetToken() == rhsUnit.m_pQualifier->GetToken() &&
			m_dMin == rhsUnit.m_dMin &&
			m_dMax == rhsUnit.m_dMax &&
			m_nDPtoShow == rhsUnit.m_nDPtoShow);
}


/*********************************************************************************************

>	BOOL ScaleUnit::operator != (const ScaleUnit& rhsUnit) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		rhsUnit : the unit on the right-hand side of the inequality test
	Returns:	TRUE this ScaleUnit is equal to rhsUnit
				FALSE otherwise
	Purpose:	Compares two ScaleUnits for inequality
	Scope:		public
	SeeAlso:	class ScaleUnit

**********************************************************************************************/ 
BOOL ScaleUnit::operator != (const ScaleUnit& rhsUnit) const
{
	return !(*this == rhsUnit);
}





/********************************************************************************************

>	BOOL ScaleUnit::SetQualifier(Qualifier& NewQualifier)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		NewQualifier : the Qualifier to use for this ScaleUnit
	Outputs:	-
	Returns:	TRUE : if set successfully
				FALSE : otherwise
	Purpose:	Allows the Qualifier for the given ScaleUnit to be specified
	Scope:		public
	SeeAlso:	UnitGroup::GetQualifier;

********************************************************************************************/
BOOL ScaleUnit::SetQualifier(Qualifier& NewQualifier)
{
	if (m_bInternalQualifier && m_pQualifier != NULL)
		delete m_pQualifier;
	m_bInternalQualifier = FALSE;
	m_pQualifier = &NewQualifier;
	return TRUE;
}


/********************************************************************************************

>	BOOL ScaleUnit::SetQualifier(	const StringBase& QualifierParseString, 
									const Qualifier::QUALIFIER_SHOWN WhetherShown)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		ParseString : a string of the form "#1%sTOKEN" or "TOKEN'1%s" signifying the
				position of the qualifier token (TOKEN) relative to the value it's qualifying
				(#1%s).
				WhetherShown : either QUALIFIER_SHOWN::IS_SHOWN or QUALIFIER_SHOWN::IS_NOTSHOWN
	Outputs:	-
	Returns:	TRUE : if set successfully
				FALSE : otherwise
	Purpose:	Allows the Qualifier for the given ScaleUnit to be specified
	Scope:		public
	SeeAlso:	UnitGroup::GetQualifier;

********************************************************************************************/
BOOL ScaleUnit::SetQualifier(	const StringBase& QualifierParseString, 
								const Qualifier::QUALIFIER_SHOWN WhetherShown)
{
	const String_32 StringString = TEXT("#1%s");
	INT32 numberPosition = QualifierParseString.Sub(StringString);
	ERROR2IF(numberPosition == -1, FALSE, "invalid QualifierParseString");

	UINT32 tokenLength = QualifierParseString.Length() - StringString.Length();

	String_32 NewToken;
	Qualifier* pNewQualifier = NULL;
	if (tokenLength == 0)	// there is no token
	{
		pNewQualifier = new NullQualifier;
	}
	else
	{
		if (numberPosition == 0)		// numberPosition came first...we have a suffix qualifier
		{
			QualifierParseString.Right(&NewToken, tokenLength);
			pNewQualifier = new SuffixQualifier;
		}
		else
		{
			QualifierParseString.Left(&NewToken, tokenLength);
			pNewQualifier = new PrefixQualifier;
		}
	}
	if (pNewQualifier == NULL)
		return FALSE;

	if (pNewQualifier->SetAttributes(NewToken, WhetherShown) == FALSE)
	{
		delete pNewQualifier;
		return FALSE;
	}

	SetQualifier(*pNewQualifier);
	m_bInternalQualifier = TRUE;		// ensure the qualifier's deleted in the destructor

	return TRUE;	
}


/********************************************************************************************

>	const Qualifier* ScaleUnit::GetQualifier() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the Qualifier for this ScaleUnit
	Purpose:	Allows access to the Qualifier for the ScaleUnit. The Qualifier to which the
				returned value is pointing is read-only and should be copied if necessary.
	Scope:		public
	SeeAlso:	UnitGroup::SetQualifier;

********************************************************************************************/
const Qualifier* ScaleUnit::GetQualifier() const
{
	return m_pQualifier;
}




/********************************************************************************************

>	BOOL ScaleUnit::SetDPtoShow(const UINT32 nNumber)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		nNumber : The number of decimal places to show
	Returns:	TRUE if set ok
				FALSE otherwise.
	Purpose:	When a value of this unit is displayed we may need to provide some default
				for the number of decimal places to display. This member sets that default.
	Scope:		public
	SeeAlso:	ScaleUnit::GetDPtoShow

********************************************************************************************/
BOOL ScaleUnit::SetDPtoShow(const UINT32 nNumber)
{
	m_nDPtoShow = nNumber;
	return TRUE;
}

	
/********************************************************************************************

>	UINT32 ScaleUnit::GetDPtoShow() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Returns:	The number of decimal places to show
	Purpose:	When displaying a number for a ScaleUnit we frequently wish to have it
				restricted to a	given number of decimal places when displayed. This number
				is returned here to be used by display routines.
	Scope:		public
	SeeAlso:	ScaleUnit::SetDPtoShow

********************************************************************************************/
UINT32 ScaleUnit::GetDPtoShow() const
{
	return m_nDPtoShow;
}


/********************************************************************************************

>	BOOL ScaleUnit::SetMin(const double dMin)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		dMin : The new minimum value.
	Returns:	True is set ok, False otherwise.
	Purpose:	The minimum value provides an offset from which numbers for this ScaleUnit
				will start when provided with 0 as the scaled value.
	Scope:		public
	SeeAlso:	ScaleUnit::GetMin

********************************************************************************************/
BOOL ScaleUnit::SetMin(const double dMin)
{
	m_dMin = dMin;
	return TRUE;
}


/********************************************************************************************

>	double ScaleUnit::GetMin() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Returns:	The current minimum
	Purpose:	Retrieves the starting offset value to be used by the conversion routines
				(or displaying the limits)
	Scope:		public
	SeeAlso:	ScaleUnit::SetMin

********************************************************************************************/
double ScaleUnit::GetMin() const
{
	return m_dMin;
}



/********************************************************************************************

>	BOOL ScaleUnit::SetMax(const double dMax)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		dMax : The new maximum value.
	Returns:	TRUE : if set ok
				FALSE : otherwise.
	Purpose:	The maximum value is the value for which, when scaled, 1 will be returned by
				ConvertTo0To1().
				This member function allows that maximum to be set.
	Scope:		public
	SeeAlso:	ScaleUnit::GetMax

********************************************************************************************/
BOOL ScaleUnit::SetMax(const double dMax)
{
	m_dMax = dMax;
	return TRUE;
}

/********************************************************************************************

>	double ScaleUnit::GetMax() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Returns:	The current maximum
	Purpose:	Returns the maximum value, which when scaled will return 1.
	Scope:		public
	SeeAlso:	ScaleUnit::SetMax

********************************************************************************************/
double ScaleUnit::GetMax() const
{
	return m_dMax;
}



/********************************************************************************************

>	BOOL ScaleUnit::SetScale(const double dScale)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		dScale : the new scale value.
	Returns:	TRUE if set ok, FALSE otherwise.
	Purpose:	Sets the value by which all numbers in the given max & min will be scaled
				to produce a value between 0 & 1.
	Scope:		public
	SeeAlso:	ScaleUnit::GetScale

********************************************************************************************/
BOOL ScaleUnit::SetScale(const double dScale)
{
	m_dScale = dScale;
	return TRUE;
}

/********************************************************************************************

>	double ScaleUnit::GetScale() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Returns:	The current scale value
	Purpose:	Returns the value by which all numbers in the given max & min will be scaled
				to produce a value between 0 & 1.
	Scope:		public
	SeeAlso:	ScaleUnit::SetScale

********************************************************************************************/
double ScaleUnit::GetScale() const
{
	return m_dScale;
}


/********************************************************************************************

>	BOOL ScaleUnit::ConvertTo0to1(double& value) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		value : The value to be scaled
	Outputs:	value : The value scaled to between 0 & 1
	Returns:	The current scale value
	Purpose:	Converts the given double to a value between 0 & 1 depending on the
				ScaleUnit's scale.
	Errors:		ERROR2 for zero value
	Scope:		public
	SeeAlso:	ScaleUnit::SetScale, ScaleUnit::GetScale

********************************************************************************************/
BOOL ScaleUnit::ConvertTo0to1(double& value) const
{
	ERROR2IF(m_dScale == 0, FALSE, "ScaleUnit scale is zero");

	value /= m_dScale;
	return TRUE;
}



/********************************************************************************************

>	BOOL ScaleUnit::ConvertFrom0to1(double& value) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		value : The value to be scaled
	Outputs:	value : The value scaled to between 0 & 1
	Returns:	The current scale value
	Purpose:	Converts the given double to a value between 0 & 1 depending on the
				ScaleUnit's scale.
	Errors:		-
	Scope:		public
	SeeAlso:	ScaleUnit::SetScale, ScaleUnit::GetScale

********************************************************************************************/
BOOL ScaleUnit::ConvertFrom0to1(double& value) const
{
	value *= m_dScale;
	return TRUE;
}




/********************************************************************************************

>	BOOL ScaleUnit::StringFromScale(double& dNumber, StringBase* pResult, UINT32 uSigFigs) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		dNumber : A scale from 0 - 1
				uSigFigs : The number of significant figures to be contained in the string
	Outputs:	dNumber : dNumber scaled to the Unit range
				pResult : A string containing the number & its qualifier
	Returns:	TRUE : The conversion was achieved successfully
				FALSE : No can do
	Purpose:	Will produce a string of the form <number>[<qualifier>] | <qualifier><number>
				depending on the units specification.
	Errors:		Parameter type checks
	Scope:		public

********************************************************************************************/
BOOL ScaleUnit::StringFromScale(double& dNumber, StringBase* pResult, const UINT32 uSigFigs) const
{
	ERROR2IF(pResult == NULL, FALSE, "pResult is NULL");
	ERROR2IF(uSigFigs > 32, FALSE, "uSigFigs out of range");

	if (ConvertFrom0to1(dNumber) == FALSE)
		return FALSE;

	String_32 TempString;
	Convert::DoubleToString(dNumber, (StringBase*)&TempString, GetDPtoShow());
	TempString.Left(&TempString, uSigFigs);

	const Qualifier* pQualifier = GetQualifier();
	if (pQualifier == NULL)
	{
		return FALSE;
	}
	ERROR3IF(!pQualifier->IS_KIND_OF(Qualifier), "ScaleUnit::StringFromScale - pQualifier is not");

	// Return the number and stick the qualifier token in there somewhere if needed
	// ...that darned lack of subclassing. May take some thought to do it properly (for the
	// reverse conversion)
	pQualifier->MakeDisplayString(TempString, pResult);
	return TRUE;
}








class ScaleUnitReference : public ListItem
{
	CC_DECLARE_DYNAMIC(ScaleUnitReference);
public:
	ScaleUnitReference() : m_pUnit(NULL), m_bChecked(FALSE) {}
	ScaleUnitReference(ScaleUnit& UnitToReference) : m_pUnit(&UnitToReference), m_bChecked(FALSE) {}

	ScaleUnit*		GetUnit() const	{	return m_pUnit;	}
	BOOL			SetUnit(ScaleUnit* const pUnit)	{	m_pUnit = pUnit; return TRUE;	}
protected:
	ScaleUnit*		m_pUnit;
	BOOL			m_bChecked;
};

CC_IMPLEMENT_DYNAMIC(ScaleUnitReference, ListItem);


/********************************************************************************************

>	UnitGroup::UnitGroup()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Purpose:	Default constructor for a UnitGroup.
				Provides safe defaults.
	Scope:		public

********************************************************************************************/
UnitGroup::UnitGroup()
{
	m_pDefaultUnit = NULL;
	m_bConstructedOK = TRUE;
}


/********************************************************************************************

>	UnitGroup::~UnitGroup()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Purpose:	Default destructor for the group
	Scope:		public

********************************************************************************************/
UnitGroup::~UnitGroup()
{
	ScaleUnit* pUnit;

	while ((pUnit = (ScaleUnit*)m_UnitList.RemoveHead()) != NULL)
		;
}


/********************************************************************************************

>	UnitGroup::UnitGroup(const UnitGroup& GroupToCopy)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Purpose:	Do not use
	Scope:		public

********************************************************************************************/
UnitGroup::UnitGroup(const UnitGroup& GroupToCopy)
{
	ERROR3("UnitGroup::UnitGroup - not expected to work");
	*this = GroupToCopy;
}


/********************************************************************************************

>	UnitGroup& UnitGroup::operator=(const UnitGroup& GroupToCopy)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Purpose:	Do not use
	Scope:		public

********************************************************************************************/
UnitGroup& UnitGroup::operator=(const UnitGroup& GroupToCopy)
{
	ERROR3("UnitGroup::operator= - not expected to work");

	if (this == &GroupToCopy) return *this;

	
	INT32 thisSize = m_UnitRefList.GetCount();
	if (thisSize == 0)		// no need to copy the units so exit stage left
	{
		m_pDefaultUnit = GroupToCopy.m_pDefaultUnit;
		m_bConstructedOK = TRUE;
		return *this;
	}
	// First bin the old list of units...
	ScaleUnitReference* pUnitRef;
	while ((pUnitRef = (ScaleUnitReference*)m_UnitList.RemoveHead()) != NULL)
		delete pUnitRef;

	// Create space for all the units in one go...
	ScaleUnitReference* UnitRefArray = new ScaleUnitReference[thisSize];
	if (UnitRefArray == NULL)
	{
		m_pDefaultUnit = NULL;
		m_bConstructedOK = FALSE;
		return *this;
	}

	ScaleUnitReference* pRefToCopy = (ScaleUnitReference*) m_UnitRefList.GetHead();
	UINT32 uIndex = 0;
	while (pRefToCopy != NULL)
	{
		ScaleUnitReference* NewRef = &UnitRefArray[uIndex++];
		NewRef->SetUnit(pRefToCopy->GetUnit());
		m_UnitRefList.AddTail(NewRef);
		pRefToCopy = (ScaleUnitReference*)m_UnitList.GetNext(pRefToCopy);
	}

	m_pDefaultUnit = GroupToCopy.m_pDefaultUnit;
	m_bConstructedOK = TRUE;
	return *this;
}


/********************************************************************************************

>	BOOL UnitGroup::operator==(const UnitGroup& RhsGroup) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Purpose:	Do not use
	Scope:		public

********************************************************************************************/
BOOL UnitGroup::operator==(const UnitGroup& RhsGroup) const
{
	INT32 rhsSize = RhsGroup.m_UnitRefList.GetCount();
	INT32 thisSize = m_UnitRefList.GetCount();
	if (rhsSize != thisSize)
		return FALSE;
	if (rhsSize == 0 && thisSize == 0)
		return TRUE;
		
	ERROR3("UnitGroup::operator== - not expected to work");
	// Create a list of references from which we can eliminate duplicates
	// in the RhsGroup

	UnitGroup pointerList = *this;

	// Now scan through the group we are comparing with eliminating duplicates in our
	// 
	ScaleUnit* pRhsUnit = (ScaleUnit*) RhsGroup.m_UnitList.GetHead();
	while (pRhsUnit != NULL)
	{
		ScaleUnitReference* pLhsUnit = (ScaleUnitReference*) pointerList.m_UnitRefList.GetHead();
		while (pLhsUnit != NULL)
		{
			if (*pLhsUnit->GetUnit() == *pRhsUnit)
			{
				// Eliminate matching pointer
				pointerList.m_UnitRefList.RemoveItem((ListItem*)pLhsUnit);
				break;
			}
			pLhsUnit = (ScaleUnitReference*)pointerList.m_UnitRefList.GetNext(pLhsUnit);
		}
		pRhsUnit = (ScaleUnit*)RhsGroup.m_UnitList.GetNext((ListItem*)pRhsUnit);
	}
	
	return pointerList.m_UnitRefList.IsEmpty();
}


/********************************************************************************************

>	BOOL UnitGroup::operator!=(const UnitGroup& RhsGroup) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Purpose:	Do not use
	Scope:		public

********************************************************************************************/
BOOL UnitGroup::operator!=(const UnitGroup& RhsGroup) const
{
	return !(*this == RhsGroup);
}


/********************************************************************************************

>	BOOL UnitGroup::AddUnit(ScaleUnit* pNewUnit)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		pNewUnit = ptr to a unit to add to the group
	Returns:	TRUE : If unit added to group
				FALSE : 
	Purpose:	Adds a ScaleUnit to a UnitGroup
	Errors:		Not allowed to add a NULL pointer to a UnitGroup
				ERROR3's if UnitGroup is corrupt
	Scope:		public

********************************************************************************************/
BOOL UnitGroup::AddUnit(ScaleUnit* pNewUnit)
{
	ERROR2IF(pNewUnit == NULL && !pNewUnit->IS_KIND_OF(ScaleUnit), FALSE, "Adding a bogus ScaleUnit");

	// Check for duplicate units in a group
	ScaleUnit* pCurrentUnit = (ScaleUnit*)m_UnitList.GetHead();
	while (pCurrentUnit != NULL)
	{
		ERROR3IF(!(pCurrentUnit->IS_KIND_OF(ScaleUnit)), "UnitGroup contains alien objects");

		const Qualifier* pNewQualifier = pNewUnit->GetQualifier();
		ERROR3IF(!(pNewQualifier->IS_KIND_OF(Qualifier)), "pNewUnit has alien qualifier");
		const Qualifier* pCurrentQualifier = pCurrentUnit->GetQualifier();
		ERROR3IF(!(pCurrentQualifier->IS_KIND_OF(Qualifier)), "UnitGroup contains alien qualifier");

		if (*(pNewQualifier->GetToken()) == *(pCurrentQualifier->GetToken()))
		{
			// ScaleUnit qualifier already in group
			ERROR3IF(*pNewUnit == *pCurrentUnit, "Adding unit with duplicate qualifier");
			return TRUE;
		}
		pCurrentUnit = (ScaleUnit*)m_UnitList.GetNext((ListItem*)pCurrentUnit);
	}
	// Unit not already in list so add on the end
	m_UnitList.AddTail((ListItem*)pNewUnit);

	return TRUE;
}


/********************************************************************************************

>	BOOL UnitGroup::AddUnit(const ScaleUnit& NewUnit)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		NewUnit : a ScaleUnit to add to this UnitGroup
	Returns:	TRUE : If unit added to group
				FALSE : 
	Purpose:	Adds a ScaleUnit to a UnitGroup
	Errors:		Not allowed to add a NULL pointer to a UnitGroup
				ERROR3's if UnitGroup is corrupt
	Scope:		public

********************************************************************************************/
BOOL UnitGroup::AddUnit(ScaleUnit& NewUnit)
{
	ERROR2IF(TRUE, FALSE, "Not tested");
	// Check for duplicate units in a group
	ScaleUnitReference* pCurrentRef = (ScaleUnitReference*)m_UnitRefList.GetHead();
	const Qualifier* pCurrentQualifier = NewUnit.GetQualifier();
	ERROR3IF(!(pCurrentQualifier->IS_KIND_OF(Qualifier)), "NewUnit contains alien qualifier");
	while (pCurrentRef != NULL)
	{
		ERROR3IF(!(pCurrentRef->IS_KIND_OF(ScaleUnitReference)), "UnitGroup contains alien objects");
		{
			const ScaleUnit* pNewUnit = pCurrentRef->GetUnit();
			const Qualifier* pNewQualifier = pNewUnit->GetQualifier();
			ERROR3IF(!(pNewQualifier->IS_KIND_OF(Qualifier)), "RhsRef has alien qualifier");

			if (*(pNewQualifier->GetToken()) == *(pCurrentQualifier->GetToken()))
			{
				ERROR3("Adding unit with duplicate qualifier");
				return FALSE;
			}
		}
		pCurrentRef = (ScaleUnitReference*)m_UnitRefList.GetNext((ListItem*)pCurrentRef);
	}
	// Unit not already in list so add on the end
	pCurrentRef = new ScaleUnitReference(NewUnit);
	m_UnitRefList.AddTail((ListItem*)pCurrentRef);
	return TRUE;
}


/********************************************************************************************

>	ScaleUnit*	UpdateUnit(const UINT32 uIndex, const ScaleUnit& NewUnit)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/94
	Inputs:		uIndex : Index of ScaleUnit to update (0..GetUnitCount() - 1)
				Unit : the ScaleUnit to replace the one at uIndex
	Returns:	A pointer to the old ScaleUnit if updated correctly
				NULL : Otherwise
	Purpose:	Allows you to update a ScaleUnit in a UnitGroup
	Errors:		ERROR2's for parameter validation
	Scope:		public

********************************************************************************************/
ScaleUnit*	UnitGroup::UpdateUnit(const UINT32 uIndex, ScaleUnit& NewUnit)
{
	ERROR2IF(uIndex >= GetUnitCount(), FALSE, "uIndex invalid");
	ERROR2IF(TRUE, FALSE, "Not tested");

	ScaleUnit* pOldUnit = FindUnitFromIndex(uIndex);

	// Can't change the qualifier??
	if (NewUnit.GetQualifier()->GetToken() != pOldUnit->GetQualifier()->GetToken())
		return NULL;

//	*pOldUnit = *pUnit;
	return pOldUnit;
}


/********************************************************************************************

>	BOOL UnitGroup::DeleteUnit(const ScaleUnit& UnitToDelete)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		pUnit : A pointer to the ScaleUnit to delete.
	Returns:	TRUE : Deletion completed
				FALSE : Not completed
	Purpose:	Lets you delete a ScaleUnit from the UnitGroup
	Scope:		public

********************************************************************************************/
BOOL UnitGroup::DeleteUnit(ScaleUnit& UnitToDelete)
{

	ERROR2IF(TRUE, FALSE, "UnitGroup::DeleteUnit - Not tested");

	ScaleUnit* pUnit = &UnitToDelete;
	// Allow something to prevent the deletion
	if (!AllowDeletion(UnitToDelete))
		return FALSE;

	pUnit = (ScaleUnit*)m_UnitList.RemoveItem((ListItem*)pUnit);
	if (pUnit == NULL)
	{
		ERROR3("UnitGroup::DeleteUnit - Item not in list");
		return FALSE;
	}

	if (m_pDefaultUnit == pUnit)		// Reset the default unit if this is being deleted
		m_pDefaultUnit = NULL;
   	
   	// Allow something to be informed that the unit is about to be deleted
	OnDeleteCompletion(UnitToDelete);

	return TRUE;
}


/*********************************************************************************************

> ScaleUnit* UnitGroup::FindUnitFromQualifier(const String_32& qualifier) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		qualifier : for the unknown unit
	Returns:	The ScaleUnit represented by the qualifier
				If no ScaleUnit has the given qualifier, the Default unit is returned (which
				may be NULL)
	Purpose:	Identifies a ScaleUnit from the given qualifier token, e.g., '%', 'd', etc
	Scope:		public
	SeeAlso:	UnitGroup::SetDefaultUnit()

*********************************************************************************************/
ScaleUnit* UnitGroup::FindUnitFromQualifier(const String_32& qualifier) const
{
	ScaleUnit* pUnit = (ScaleUnit*) m_UnitList.GetHead();

	while (pUnit != NULL)
	{
		ERROR3IF(!(pUnit->IS_KIND_OF(ScaleUnit)), "UnitGroup::FindUnitFromQualifier - UnitGroup contains alien objects");

		const Qualifier* pQualifier = pUnit->GetQualifier();
		ERROR3IF(!(pQualifier->IS_KIND_OF(Qualifier)), "UnitGroup::FindUnitFromQualifier - Qualifier is not");

		if (qualifier == pQualifier->GetToken())
			return pUnit;

		pUnit = (ScaleUnit*) m_UnitList.GetNext((ListItem*)pUnit);
	}

	return NULL;
}


/********************************************************************************************

>	ScaleUnit* UnitGroup::FindUnitFromIndex(UINT32 uIndex)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/94
	Inputs:		Index = indexes directly into the unit list (0 = first unit in list)
	Returns:	Ptr to the unit.
				NULL is returned if not found.
	Purpose:	Find a unit by index
	Scope:		public

********************************************************************************************/
ScaleUnit* UnitGroup::FindUnitFromIndex(const UINT32 uIndex) const
{
	ERROR2IF(uIndex >= GetUnitCount(), NULL, "uIndex invalid");

	ScaleUnit* pUnit = (ScaleUnit*) m_UnitList.FindItem(LISTPOS(uIndex));
	ERROR3IF(!pUnit->IS_KIND_OF(ScaleUnit), "UnitGroup contains alien objects");

	return pUnit;
}


/*********************************************************************************************

> INT32	UnitGroup::FindIndexFromUnit(const ScaleUnit& thisUnit) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		thisUnit = ScaleUnit to look for
	Returns:	An index for the given ScaleUnit.
				-1 if not found
	Purpose:	Gets a unique index for the given ScaleUnit in the UnitGroup by which it may
				be referred.
	Scope:		public

*********************************************************************************************/
INT32 UnitGroup::FindIndexFromUnit(const ScaleUnit& thisUnit) const
{
	ScaleUnit* pUnit = (ScaleUnit*) m_UnitList.GetHead();
	INT32 nIndex = 0;

	while (pUnit != NULL)
	{
		ERROR3IF(!(pUnit->IS_KIND_OF(ScaleUnit)), "UnitGroup contains alien objects");
		if (*pUnit == thisUnit) break;
		pUnit = (ScaleUnit*) m_UnitList.GetNext((ListItem*)pUnit);
		++nIndex;
	}
	if (pUnit == NULL) nIndex = -1;		// not found

	return nIndex;
}


/********************************************************************************************

>	UINT32 UnitGroup::GetUnitCount()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Returns:	The number of ScaleUnits in the group
	Purpose:	Provides a check for parameters usually
	Scope:		public

********************************************************************************************/
UINT32 UnitGroup::GetUnitCount() const
{
	return m_UnitList.GetCount();
}



/********************************************************************************************

>	BOOL UnitGroup::SetDefaultUnit(ScaleUnit* pUnit)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		pUnit : A pointer to the ScaleUnit to set as default. NULL is no default
	Returns:	TRUE : The unit was set
				FALSE : The unit was not in the UnitGroup
	Purpose:	To provide a default ScaleUnit to use when no unit is specified for
				ScaleFromString (& no ScaleUnit has a NULL qualifier)
	Scope:		public

********************************************************************************************/
BOOL UnitGroup::SetDefaultUnit(ScaleUnit* pUnit)
{
	if (pUnit == NULL)			// Set no default
	{
		m_pDefaultUnit = NULL;
		return TRUE;
	}

	ERROR2IF(!(pUnit->IS_KIND_OF(ScaleUnit)), FALSE, "pUnit Invalid");

	LISTPOS lp = m_UnitList.FindPosition(pUnit);
	if (lp == NOT_IN_LIST || lp == EMPTY_LIST)
		return FALSE;
	m_pDefaultUnit = pUnit;
	return TRUE;
}



/********************************************************************************************

>	ScaleUnit* GetDefaultUnit() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		-
	Returns:	A pointer to the default unit
	Purpose:	Provides a pointer to the default unit in the group. (NULL is no default)
	Scope:		public

********************************************************************************************/
ScaleUnit* UnitGroup::GetDefaultUnit() const
{
	return m_pDefaultUnit;
}


/*********************************************************************************************

>	class Parser : public CC_CLASS_MEMDUMP

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Purpose:	This parser is in fact a lexical analyzer for ScaleUnit's
				It matches Numbers (IsNumeric) & Qualifiers (IsCharUnitType).
				Anything else is considered Unknown.
				This is just a fill in while I consider how to provide something
				more generic. The code is blardy awful, partly because of lack of
				support in the StringBase class.
				To use it (not that you would), do something like:
		MonoOn:
				Parser myParser(stringToParse);
				if (PrepareForParse() == FALSE) ...
				if (GetToken() == Parser::TK_QUALIFIER)...
					myQualifier = GetSemanticValue();
		MonoOff:
				 Repeated calls to GetToken() will move along the stringToParse until
				 the end of the string is reached when TK_EOS is returned.

**********************************************************************************************/ 
class Parser : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(Parser);
public:
	Parser(const StringBase& ParseString) : m_pParseString(ParseString), m_uParsePosition(0) {}

	enum TOKEN
	{
		TK_EOS,
		TK_UNKNOWN,
		TK_QUALIFIER,
		TK_NUMBER
	};

	BOOL		PrepareForParse();
	TOKEN		GetToken();
	String_256	GetSemanticValue()	{	return m_SemVal;	}
	UINT32		GetCurrentIndex()	{	return m_uParsePosition;	}

protected:
	const StringBase&	m_pParseString;
	UINT32				m_uParsePosition;
	String_256			m_SemVal;
};

CC_IMPLEMENT_MEMDUMP(Parser, CC_CLASS_MEMDUMP)

/********************************************************************************************

>	BOOL Parser::PrepareForParse()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Returns:	TRUE : If the parser was prepared OK
				FALSE : otherwise
	Purpose:	Prepares the parser to parse the string given in the constructor
				from the beginning.
	Scope:		public

********************************************************************************************/
BOOL Parser::PrepareForParse()
{
	m_uParsePosition = 0;
	return TRUE;
}


/********************************************************************************************

>	Parser::TOKEN Parser::GetToken()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Returns:	The TOKEN representing a possible syntactic value for the string starting
				at the GetCurrentIndex() position in the string on entry to the function
				and ending at GetCurrentIndex() on exit. Possible values are:
				TK_QUALIFIER : A possible qualifier token. GetSemanticValue() will contain
				the string.
				TK_NUMBER : A number as defined by IsNumeric(). GetSemanticValue() will 
				contain the number corresponding.
				TK_EOS : End of string has been reached. No more tokens to read.
				TK_UNKNOWN : Not one of the above
	Purpose:	Retrieves a TOKEN from the input string to be used by some parser somewhere.
	Scope:		public
	See Also:	class Parser

********************************************************************************************/
Parser::TOKEN Parser::GetToken()
{
	m_SemVal.Empty();
	UINT32 Length = m_pParseString.Length();
	while (m_uParsePosition < Length)
	{
		TCHAR c = m_pParseString[m_uParsePosition];
		if (Convert::IsCharUnitType(c))
		{
			do
			{
				m_SemVal += c;
				++m_uParsePosition;
				c = m_pParseString[m_uParsePosition];
			} while (Convert::IsCharUnitType(c) && m_uParsePosition < Length);
			return TK_QUALIFIER;
		}
		else if (StringBase::IsNumeric(c) || (c == Convert::GetDecimalPointChar()))
		{
			do
			{
				m_SemVal += c;
				++m_uParsePosition;
				c = m_pParseString[m_uParsePosition];
			} while ((StringBase::IsNumeric(c)  || (c == Convert::GetDecimalPointChar()))
						&& m_uParsePosition < Length);

			return TK_NUMBER;
		}
		else if (_istspace(c))
		{
			++m_uParsePosition;
			continue;
		}
		else
		{
			++m_uParsePosition;
			return TK_UNKNOWN;
		}
	}
	return TK_EOS;
}

/********************************************************************************************

>	BOOL UnitGroup::ScaleFromString(const StringBase& string, double& dNumber) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		string : 
	Outputs:	dNumber : A scale from 0 - 1
	Returns:	TRUE : The string was able to be converted to a scale from 0 to 1.
				FALSE : No can do
	Purpose:	Given a string of the form <number>[<qualifier>] | <qualifier><number> this
				method will produce a number in the range 0 to 1 depending on the qualifier.
				If no qualifier is given the default for the group is used. If no default
				is provided FALSE is returned.
	Errors:		-
	Scope:		public
	SeeAlso:	

********************************************************************************************/
BOOL UnitGroup::ScaleFromString(const StringBase& string, double& dNumber) const
{
	String_32 qualifier, amount;
	Qualifier::QUALIFIER_AFFIX affixType;
	Parser parser(string);

	parser.PrepareForParse();

	Parser::TOKEN tk = parser.GetToken();
	if (tk == Parser::TK_QUALIFIER)		// expecting prefix qualifier
	{
		qualifier = parser.GetSemanticValue();
		affixType = Qualifier::PREFIX;
		tk = parser.GetToken();
		if (tk == Parser::TK_NUMBER)
		{
			amount = parser.GetSemanticValue();
		}
		else
		{
			//ERROR3("UnitGroup::ScaleFromString - no number");
			return FALSE;
		}
	}
	else if (tk == Parser::TK_NUMBER)	// expecting suffix qualifier
	{
		amount = parser.GetSemanticValue();
		tk = parser.GetToken();
		if (tk == Parser::TK_QUALIFIER)
		{
			affixType = Qualifier::SUFFIX;
			qualifier = parser.GetSemanticValue();
		}
		else if (tk == Parser::TK_EOS)
		{
			affixType = Qualifier::NONE;
		}
		else
		{
			//ERROR3("UnitGroup::ScaleFromString - invalid characters");
			return FALSE;
		}
	}
	else
	{
		//ERROR3("UnitGroup::ScaleFromString - invalid characters");
		return FALSE;
	}


	ScaleUnit* pUnit;
	// We've found a qualifier & amount so check them both
	pUnit = FindUnitFromQualifier(qualifier);
	if (pUnit == NULL)		// no match so use the default
	{
		pUnit = GetDefaultUnit();
	}
	else
	{
		//ERROR3IF(pUnit->GetQualifier()->GetAffix() != affixType, "UnitGroup::ScaleFromString - qualifier position incorrect");
	}
	if (pUnit == NULL) 		// no unit with that qualifier & no default
	{
		return FALSE;
	}
	ERROR3IF(!pUnit->IS_KIND_OF(ScaleUnit), "UnitGroup::ScaleFromString - pUnit isn't");
	Convert::StringToDouble( amount, &dNumber );
	return (pUnit->ConvertTo0to1(dNumber));
}



/********************************************************************************************

> void UnitGroup::OnUnitChanged(ScaleUnit* pChangedUnit)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		pChangedUnit : a pointer to the ScaleUnit that has changed its definition
	Outputs:	-
	Returns:	-
	Purpose:	This virtual function is called when the definition of a unit has changed.
				The default implementation does nothing, but subclasses can
				provide one if necessary.
	Errors:		-
	Scope:		protected
	SeeAlso:	

********************************************************************************************/
void UnitGroup::OnUnitChanged(const ScaleUnit& CAMUNUSED(UpdatedUnit))
{
	// Nothing to do here
}


/********************************************************************************************

> BOOL UnitGroup::AllowDeletion(const ScaleUnit& UnitToDelete)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		changedUnit : the ScaleUnit that has changed its definition
	Outputs:	-
	Returns:	TRUE : The given unit will be deleted
				FALSE : The unit will not be deleted
	Purpose:	This virtual function is called before a unit is about to be deleted from a
				UnitGroup. The default implementation always allows the deletion, but
				subclasses of UnitGroup may provide another one if necessary.
	Errors:		-
	Scope:		protected
	SeeAlso:	

********************************************************************************************/
BOOL UnitGroup::AllowDeletion(const ScaleUnit& WXUNUSED(UnitToDelete))
{
	// Nothing to do here
	return TRUE;
}

/********************************************************************************************

> void UnitGroup::OnDeleteCompletion(const ScaleUnit& DeletedUnit)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		DeletedUnit : the ScaleUnit just deleted from the group
	Outputs:	-
	Returns:	-
	Purpose:	This virtual function is called after a ScaleUnit has been deleted from a
				UnitGroup. The default implementation does nothing, but subclasses can
				provide one if necessary.
	Errors:		-
	Scope:		protected
	SeeAlso:	

********************************************************************************************/
void UnitGroup::OnDeleteCompletion(const ScaleUnit& CAMUNUSED(DeletedUnit))
{
	// Nothing to do here
}


/********************************************************************************************/

// Globals for StandardUnit class...
// Sets
ScaleUnit StandardUnit::UnitPercent(100.0, 1);
ScaleUnit StandardUnit::UnitDecimalD(255.0, 0);
ScaleUnit StandardUnit::UnitDecimald(255.0, 0);
ScaleUnit StandardUnit::UnitDegree(360.0, 1);

UnitGroup StandardUnit::PercentGroup;
UnitGroup StandardUnit::AngleGroup;

NullQualifier StandardUnit::TheNullQualifier;		// the Qualifier with no token


/********************************************************************************************

> BOOL StandardUnit::Init(void)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Returns:	TRUE if initialization successful, FALSE otherwise
	Purpose:	Initializes the Standard ScaleUnits & UnitGroups
	Errors:		ERROR2's if failed to initialize properly
	Scope:		public
	SeeAlso:	DeInit(), class StandardUnit

********************************************************************************************/
BOOL StandardUnit::Init(void)
{
	String_32 qualifierToken;

	ERROR2IF(!qualifierToken.Load(_R(IDS_PARSE_PERCENT)), FALSE, "Could not get percent token");
	ERROR2IF(!UnitPercent.SetQualifier(qualifierToken, Qualifier::IS_SHOWN), FALSE, "Could not set qualifier");

	ERROR2IF(!qualifierToken.Load(_R(IDS_PARSE_LARGED)), FALSE, "Could not get decimal token");
	ERROR2IF(!UnitDecimalD.SetQualifier(qualifierToken, Qualifier::IS_NOTSHOWN), FALSE, "Could not set qualifier");

	ERROR2IF(!qualifierToken.Load(_R(IDS_PARSE_SMALLD)), FALSE, "Could not get decimal token");
	ERROR2IF(!UnitDecimald.SetQualifier(qualifierToken, Qualifier::IS_NOTSHOWN), FALSE, "Could not set qualifier");

	ERROR2IF(!PercentGroup.AddUnit(&UnitPercent), FALSE, "could not add unit");
	ERROR2IF(!PercentGroup.AddUnit(&UnitDecimald), FALSE, "could not add unit");
	ERROR2IF(!PercentGroup.AddUnit(&UnitDecimalD), FALSE, "could not add unit");

	ERROR2IF(!UnitDegree.SetQualifier(StandardUnit::TheNullQualifier), FALSE, "Could not set qualifier");
	ERROR2IF(!AngleGroup.AddUnit(&UnitDegree), FALSE, "could not add unit");

#ifdef _DEBUG
	ERROR2IF(!qualifierToken.Load(_R(IDS_PARSE_PERCENT)), FALSE, "Could not get percent token");
	ScaleUnit testUnit(100.0, 1);
	testUnit.SetQualifier(qualifierToken, Qualifier::IS_NOTSHOWN);
	double num = 50.0;
	testUnit.StringFromScale(num, &qualifierToken, 5);
#endif

	return TRUE;
}


/********************************************************************************************

> BOOL StandardUnit::DeInit(void)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if deinitialization successful, FALSE otherwise
	Purpose:	Returns the StandardUnit class to a stable state as it was prior to Init()
				being called.
				Actually does nothing.
	Errors:		-
	Scope:		public
	SeeAlso:	Init(), class StandardUnit

********************************************************************************************/
BOOL StandardUnit::DeInit()
{
	UnitDecimalD.SetQualifier(StandardUnit::TheNullQualifier);
	UnitDecimald.SetQualifier(StandardUnit::TheNullQualifier);
	UnitPercent.SetQualifier(StandardUnit::TheNullQualifier);
	return TRUE;
}

