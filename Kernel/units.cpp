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
// Implementation of the Unit class

/*
*/

#include "camtypes.h"
#include "units.h"
//#include "markn.h"
//#include "justin2.h"
#include "document.h"
//#include "oilfiles.h"

DECLARE_SOURCE("$Revision$");
CC_IMPLEMENT_DYNAMIC(Unit,ListItem)
CC_IMPLEMENT_DYNAMIC(DocUnitList,List)
CC_IMPLEMENT_DYNAMIC(UnitMsg,Msg)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	Unit::Unit()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for a unit.
				Adds the unit to the end of Unit::pUnitList.

********************************************************************************************/

Unit::Unit()
{
}

/********************************************************************************************

>	Unit::~Unit()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default destructor for a unit - does nothing

********************************************************************************************/

Unit::~Unit()
{
}

/********************************************************************************************

>	static BOOL Unit::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialises the unit class
				MUST BE CALLED BEFORE ANY OTHER FUNCTION IN THIS CLASS
				Inits the list of units, defines all the default units and places them
				on the list.

********************************************************************************************/

BOOL Unit::Init()
{
	return TRUE;
}

/********************************************************************************************

>	static BOOL Unit::Deinit()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if deinitialised OK
	Purpose:	Deinitialises the unit class
				It scans Unit::pUnitList, removing and deleting all its members, then
				it destroys the list.

********************************************************************************************/

BOOL Unit::Deinit()
{
	return TRUE;
}

/********************************************************************************************

>	BOOL Unit::IsDefault()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this unit is one of Camelot's default units
	Purpose:	Find out if this is a default unit

********************************************************************************************/

BOOL Unit::IsDefault()
{
	// Cannot return the state of the flag directly as this is not BOOLEAN due to the
	// way bit arrays work. Only reliable test on this is FALSE == 0.
	return (Flags.DefaultUnit != 0);
}

/********************************************************************************************

>	void Unit::SetDefaultState(BOOL State)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		State = new default state
	Outputs:	-
	Returns:	the old state of the default unit flag
	Purpose:	Allows you to set/clear the default unit flag.
				Updated 4/9/95 to return the old state.

********************************************************************************************/

BOOL Unit::SetDefaultState(BOOL State)
{
	BOOL OldState = (Flags.DefaultUnit != 0);
	Flags.DefaultUnit = State;
	
	return OldState;	
}

/********************************************************************************************

>	BOOL Unit::IsPrefix()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this unit's specifier should be placed as a prefix in ed fields
				FALSE means suffix
	Purpose:	Find out whether unit specifier should be a prefix or suffix

********************************************************************************************/

BOOL Unit::IsPrefix()
{
	// Cannot return the state of the flag directly as this is not BOOLEAN due to the
	// way bit arrays work. Only reliable test on this is FALSE == 0.
	return (Flags.Prefix != 0);
}

/********************************************************************************************

>	BOOL Unit::SetPrefixState(BOOL State)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		State = new Prefix state
	Outputs:	-
	Returns:	The old state of the Prefix flag
	Purpose:	Allows you to set/clear the Prefix flag
				Updated 4/9/95 to return the old state.

********************************************************************************************/

BOOL Unit::SetPrefixState(BOOL State)
{
	BOOL OldState = (Flags.Prefix != 0);
	Flags.Prefix = State;
	
	return OldState;
}

/********************************************************************************************

>	double Unit::GetMillipoints()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	The num millipoints in one of these units
	Purpose:	Want the number of millipoints in a single instance of this unit?
				This is the call for you!

********************************************************************************************/

double Unit::GetMillipoints()
{
	return Millipoints;
}

/********************************************************************************************

>	BOOL Unit::SetMillipoints(double NewMillipoints)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		NewMillipoints = num millipoints in one of these units
	Outputs:	-
	Returns:	True is set ok, False otherwise.
	Purpose:	Lets you define exactly how big this unit is in the real world, not that we sad
				computer programmers have any idea what the "real world" is, even if it came
				up and generated pulses in a recognisable protocol on our ethernet cable.
				Updated 4/9/95 to return the True if can set the new state ok.

********************************************************************************************/

BOOL Unit::SetMillipoints(double NewMillipoints)
{
	// Sanity checks
	// - ensure not zero otherwise just wait for those divide by zero errors!
	// - check for negative values as well as these will be bad
	if (NewMillipoints <= 0)
		return FALSE;

	Millipoints = NewMillipoints;

	return TRUE;
}

/********************************************************************************************

>	UnitType Unit::GetBaseUnitType()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	Unit type this unit is based on
	Purpose:	Find out which unit this unit is based on

********************************************************************************************/

UnitType Unit::GetBaseUnitType()
{
	return BaseUnitType;
}

/********************************************************************************************

>	BOOL Unit::SetBaseUnitType(UnitType NewBaseUnitType)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		NewBaseUnitType = the unit this unit is based on
	Outputs:	-
	Returns:	True is set ok, False otherwise.
	Purpose:	Lets you set that base unit type. Oh boy!
				Updated 4/9/95 to return the True if can set the new state ok.

********************************************************************************************/

BOOL Unit::SetBaseUnitType(UnitType NewBaseUnitType)
{
	BaseUnitType = NewBaseUnitType;

	return TRUE;
}

/********************************************************************************************

>	double Unit::GetBaseNumerator()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	Get the base numerator
	Purpose:	Get the numerator of the fraction that defines how many BaseUnitTypes are
				in one of these units.
				Millipoints = (BaseNumerator/BaseDenominator)*BaseUnit->GetMillipoints();

********************************************************************************************/

double Unit::GetBaseNumerator()
{
	return BaseNumerator;
}

/********************************************************************************************

>	void Unit::SetBaseNumerator(double NewBaseNumerator)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		NewBaseNumerator = the new numerator value
	Outputs:	-
	Returns:	True is set ok, False otherwise.
	Purpose:	Sets the numerator of the fraction that defines how many BaseUnitTypes are
				in one of these units.
				Millipoints = (BaseNumerator/BaseDenominator)*BaseUnit->GetMillipoints();
				Updated 4/9/95 to return the True if can set the new state ok.

********************************************************************************************/

BOOL Unit::SetBaseNumerator(double NewBaseNumerator)
{
	// Sanity checks
	// - ensure not zero otherwise just wait for those divide by zero errors!
	// - check for negative values as well as these will be bad
	if (NewBaseNumerator <= 0)
		return FALSE;

	BaseNumerator = NewBaseNumerator;
	
	return TRUE;
}

/********************************************************************************************

>	double Unit::GetBaseDenominator()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	Get the base Denominator
	Purpose:	Get the Denominator of the fraction that defines how many BaseUnitTypes are
				in one of these units.
				Millipoints = (BaseNumerator/BaseDenominator)*BaseUnit->GetMillipoints();

********************************************************************************************/

double Unit::GetBaseDenominator()
{
	return BaseDenominator;
}

/********************************************************************************************

>	BOOL Unit::SetBaseDenominator(double NewBaseDenominator)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		NewBaseDenominator = the new Denominator value
	Outputs:	-
	Returns:	True is set ok, False otherwise.
	Purpose:	Sets the Denominator of the fraction that defines how many BaseUnitTypes are
				in one of these units.
				Millipoints = (BaseNumerator/BaseDenominator)*BaseUnit->GetMillipoints();
				Updated 4/9/95 to return the True if can set the new state ok.

********************************************************************************************/

BOOL Unit::SetBaseDenominator(double NewBaseDenominator)
{
	// Sanity checks
	// - ensure not zero otherwise just wait for those divide by zero errors!
	// - check for negative values as well as these will be bad
	if (NewBaseDenominator <= 0)
		return FALSE;

	BaseDenominator = NewBaseDenominator;

	return TRUE;
}

/********************************************************************************************

>	String_32 Unit::GetToken()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	The token string.
	Purpose:	Returns the token string which might be used in say a drop-down list of units.
				This is the full name of the unit in question
				E.g. "centimetres" specifies the default centimetres unit with the
				abbreviation or specifier of "cm".
	SeeAlso:	Unit::SetToken; DocUnitList::GetSpecifier;
	
********************************************************************************************/

String_32 Unit::GetToken()
{
	return Token;
}

/********************************************************************************************

>	BOOL Unit::SetToken(const String_32& NewToken)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		NewToken = new token string
	Outputs:	-
	Returns:	True is set ok, False otherwise.
	Purpose:	Sets the token string for the unit. This is the full name of the unit
				in question
				E.g. "centimetres" specifies the default centimetres unit with the
				abbreviation or specifier of "cm". 
				This must not be blank and must not contain numbers as otherwise number
				parsing may be difficult. Hence, new units should be labelled with A, B .... 
				Hence also, updated to return BOOL 4/9/95 so can return to the setter the
				fact that the new specifier cannot be allowed.
	SeeAlso:	Unit::GetToken;	DocUnitList::GetSpecifier;

********************************************************************************************/

BOOL Unit::SetToken(const String_32& NewToken)
{
	// Sanity checks
	// - ensure not zero length
	// - should really check for duplicates as unless we allow for overiding of preset units
	// - should check for dodgy characters such as numbers as these may be very bad
	if (NewToken.Length() == 0)
		return FALSE;

	Token = NewToken;

	return TRUE;
}

/********************************************************************************************

>	String_32 Unit::GetSpecifier()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	The Specifier string
	Purpose:	Returns the Specifier string used in in ed fields when specifying units
				E.g. "cm" specifies centimetres
	SeeAlso:	DocUnitList::GetSpecifier;
********************************************************************************************/

String_32 Unit::GetSpecifier()
{
	return Specifier;
}

/********************************************************************************************

>	BOOL Unit::SetSpecifier(const String_32& NewSpecifier)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		NewSpecifier = new Specifier string
	Outputs:	-
	Returns:	True is set ok, False otherwise.
	Purpose:	Sets the Specifier string for the unit. This is the abbreviation for the unit.
				E.g. "cm" specifies centimetres.
				Might be bad to allow blanks or duplicates of previosly defined specifiers.
				Hence, updated to return BOOL 4/9/95 so can return to the setter the fact that
				the new specifier cannot be allowed.
	SeeAlso:	Unit::GetSpecifier; DocUnitList::GetSpecifier;

********************************************************************************************/

BOOL Unit::SetSpecifier(const String_32& NewSpecifier)
{
	// Sanity checks
	// - ensure not zero length
	// - should really check for duplicates as unless we allow for overiding of preset units
	// - should check for dodgy characters such as numbers as these are very bad
	// - should check for the decimal point character and thousands separator
	INT32 LenNewSpecifier = NewSpecifier.Length();	
	if (LenNewSpecifier == 0)
		return FALSE;

	// Check for the existance of numbers in the string.
	// At present, check for the prescence of the fixed decimal point and thousand separator
	// characters as no easy way of getting at the values in convert. 
	String_32 CheckString( _T("0123456789.,") );
	INT32 LenCheckString = CheckString.Length();	
	BOOL Found = FALSE;
	TCHAR CheckChar;
	// Now go through every character in the check string and see if it exists in
	// the required new specifier string
	for (INT32 i = 0; ( (i < LenNewSpecifier) && (!Found) ); i++)
	{
		// Get the next character to be checked in the new specifier string
		CheckChar = ((const TCHAR*)NewSpecifier)[i];
		
		// Now compare this against all the characters in the validation string
		for (INT32 j = 0; ( (j < LenCheckString) && (!Found) ); j++)
		{
			if (((TCHAR*)CheckString)[j] == CheckChar)
			{
				Found = TRUE;
			}
		}
	}
	// We found a problem character so return this result to the caller
	if (Found)
		return FALSE;		

	Specifier = NewSpecifier;
	
	return TRUE;
}

/********************************************************************************************

>	UnitType Unit::GetUnitType()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	The UnitType. This is a enumerated number for all the units defined.
	Purpose:	Returns the UnitType of this unit

********************************************************************************************/

UnitType Unit::GetUnitType()
{
	return ThisUnitType;
}

/********************************************************************************************

>	void Unit::SetUnitType(UnitType NewUnitType)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		NewUnitType = new UnitType string
	Outputs:	-
	Returns:	True is set ok, False otherwise.
	Purpose:	Sets the UnitType for the unit. 

********************************************************************************************/

BOOL Unit::SetUnitType(UnitType NewUnitType)
{
	ThisUnitType = NewUnitType;
	
	return TRUE;
}

//-------------------------------------------------------
//-------------------------------------------------------
//-------------------------------------------------------
//-------------------------------------------------------

/********************************************************************************************

>	BOOL DocUnitList::CheckUnitTypesValid()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if valid, FALSE if not
	Purpose:	This checks to see if the UnitType enum and the #defines that specify unit IDs match up.

				UnitType was originally defined as an enum so that it could by used like a normal type.

				Later on, internationalisation came along requiring the ability to define the default
				set of units via an .ini file. This required unit types to be specified using #defines so that
				C preprocessing could be used to make the creation of the built-in .ini file easy to read.

				This conflict meant that it could be possible for these values to get out of step.  This function
				goes some way to ensure that they don't.

********************************************************************************************/

BOOL DocUnitList::CheckUnitTypesValid()
{
	BOOL ok = TRUE;

	if (ok) ok = (MILLIMETRES 	== UNIT_MILLIMETRES);
	if (ok) ok = (CENTIMETRES 	== UNIT_CENTIMETRES);
	if (ok) ok = (METRES	 	== UNIT_METRES);
	if (ok) ok = (INCHES	 	== UNIT_INCHES);
	if (ok) ok = (FEET	 		== UNIT_FEET);
	if (ok) ok = (YARDS 		== UNIT_YARDS);
	if (ok) ok = (COMP_POINTS 	== UNIT_COMP_POINTS);
	if (ok) ok = (PICAS 		== UNIT_PICAS);
	if (ok) ok = (MILLIPOINTS 	== UNIT_MILLIPOINTS);
	if (ok) ok = (MILES 		== UNIT_MILES);
	if (ok) ok = (KILOMETRES 	== UNIT_KILOMETRES);
	if (ok) ok = (PIXELS	 	== UNIT_PIXELS);

	ERROR2IF(!ok,FALSE,"One or more enum UnitType values are different to the #define UNIT_ values");

	return (ok);
}

/********************************************************************************************

>	DocUnitList::DocUnitList()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for a document unit list.
				Initialises the object.

********************************************************************************************/

DocUnitList::DocUnitList()
{
	NumUnits = 0;
	NextUnitNum = 0;
	NextUnitType = NUM_DEFAULT_UNIT_TYPES;
	DefaultsMade = FALSE;
	LastEditedUnit = NOTYPE;

	SetScaleUnits(AUTOMATIC);
	SetPageUnits(CENTIMETRES);
	SetFontUnits(COMP_POINTS);
}

/********************************************************************************************

>	DocUnitList::~DocUnitList()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default destructor for a document unit list.
				Deletes all units in the list

********************************************************************************************/

DocUnitList::~DocUnitList()
{
/*	Unit* pUnit = (Unit*)this->GetHead();
	Unit* pOldUnit;

	while (pUnit != NULL)
	{
		pOldUnit = pUnit;
		pUnit = (Unit*)this->GetNext(pOldUnit);
		this->RemoveItem(pOldUnit);
		delete pOldUnit;
	}
*/
	ListItem* pItem;

	while ((pItem = this->RemoveHead()) != NULL)
		delete pItem;
}

/********************************************************************************************

>	static BOOL DocUnitList::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialises the DocUnitList class
				MUST BE CALLED BEFORE ANY OTHER FUNCTION IN THIS CLASS
				
********************************************************************************************/

BOOL DocUnitList::Init()
{
	return CheckUnitTypesValid();
}

/********************************************************************************************

>	static BOOL DocUnitList::Deinit()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if deinitialised OK
	Purpose:	Deinitialises the doc unit list class

********************************************************************************************/

BOOL DocUnitList::Deinit()
{
	return TRUE;
}

/********************************************************************************************

>	BOOL DocUnitList::MakeNewUnit(Unit** ppUnit,BOOL Initialise = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		ppUnit = ptr to ptr to a unit
				Initialise - 	if TRUE then the new unit is initialised with 
								senisble values.
								Also, UnitMsg::NEW is only broadcast when this is TRUE
	Outputs:	If OK, *ppUnit = ptr to the new unit
	Returns:	TRUE if OK, i.e. successfully created and initialised a new unit
	Purpose:	Defines all the default units and places them on the list.
	Errors:		Can only be called once per instance. Errors if called again.

********************************************************************************************/

BOOL DocUnitList::MakeNewUnit(Unit** ppUnit,BOOL Initialise)
{
	Unit* pUnit = new Unit;
	if (pUnit == NULL) return FALSE;

	if (Initialise)
	{
		String_32 num(_R(IDS_UNITS_NEW_UNIT_ABBREV));
		String_32 abbrev(_R(IDS_UNITS_NEW_UNIT_ABBREV));
		
		INT32 i;
		for( i = 0;i<(NextUnitNum / 20) && i<10;i++)
			num += abbrev;

		((LPTSTR) num)[i] += ((NextUnitNum++) % 20);
		
		String_32 TokenStr(_R(IDS_UNITS_NEW_UNIT_NAME));
		String_32 SpecifierStr(_R(IDS_UNITS_NEW_UNIT_SPECIFIER));
		TokenStr += num;
		SpecifierStr += num;

		pUnit->SetUnitType((UnitType) NextUnitType++);
		pUnit->SetDefaultState(FALSE);
		pUnit->SetPrefixState(FALSE);
		pUnit->SetBaseUnitType(MILLIMETRES);
		pUnit->SetBaseNumerator(10);
		pUnit->SetBaseDenominator(1);
		pUnit->SetMillipoints(CM_MP_VAL);
		pUnit->SetToken(TokenStr);
		pUnit->SetSpecifier(SpecifierStr);
	}

	this->AddTail(pUnit);
	*ppUnit = pUnit;
	NumUnits++;

	if (Initialise)
	{
	   	// Tell everybody that the unit has been created
		BROADCAST_TO_ALL(UnitMsg(this,pUnit->GetUnitType(),UnitMsg::NEW));
	}

	return TRUE;
}

/********************************************************************************************

>	UnitReason DocUnitList::DeleteUnit(UnitType ThisUnitType)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/94
	Inputs:		ThisUnitType = the type of the unit to delete
	Outputs:	-
	Returns:	UNITREASON_OK 			  -  The unit has been deleted
				UNITREASON_HASDESCENDENTS -  Other units are based on this unit
										 	 e.g. cm is based on mm
				UNITREASON_BEINGUSED      -  This unit type is being used in the document
											 e.g. page units may use this unit
	Purpose:	Lets you delete a unit from the list
	Errors:		Can only be called once per instance. Errors if called again.

********************************************************************************************/

UnitReason DocUnitList::DeleteUnit(UnitType ThisUnitType)
{
	if (HasDescendents(ThisUnitType))
		return UNITREASON_HASDESCENDENTS;

	if (PageUnits  == ThisUnitType ||
		ScaleUnits == ThisUnitType ||
		FontUnits  == ThisUnitType)
		return UNITREASON_BEINGUSED;

   	// Tell everybody that the unit is about to be deleted)
	BROADCAST_TO_ALL(UnitMsg(this,ThisUnitType,UnitMsg::BEFOREDELETE));

	this->RemoveItem(FindUnit(ThisUnitType));
	NumUnits--;

   	// Tell everybody that the unit is about to be deleted)
	BROADCAST_TO_ALL(UnitMsg(this,NOTYPE,UnitMsg::AFTERDELETE));

	return UNITREASON_OK;
}

/********************************************************************************************

>	INT32 DocUnitList::GetNumUnits()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		-
	Outputs:	-
	Returns:	The num defined units in the list
	Purpose:	Returns the num units in the list

********************************************************************************************/

INT32 DocUnitList::GetNumUnits()
{
	return NumUnits;
}

/********************************************************************************************

>	Unit* DocUnitList::FindUnit(UnitType ThisUnit)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		ThisUnit = the unit type of the unit we want
	Outputs:	-
	Returns:	Ptr to the unit.
				NULL is returned if not found.
	Purpose:	Find a unit using its type

********************************************************************************************/

Unit* DocUnitList::FindUnit(UnitType ThisUnit)
{
	Unit* pUnit = (Unit*) this->GetHead();

	while (pUnit != NULL)
	{
		if (pUnit->GetUnitType() == ThisUnit)
			return pUnit;

		pUnit = (Unit*) this->GetNext(pUnit);
	}

	ERROR3IF(pUnit == NULL,"Can't find the unit");

	return NULL;
}

/********************************************************************************************

>	Unit* DocUnitList::FindUnit(INT32 Index)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/94
	Inputs:		Index = indexes directly into the unit list (0 = first unit in list)
	Outputs:	-
	Returns:	Ptr to the unit.
				NULL is returned if not found.
	Purpose:	Find a unit by index

********************************************************************************************/

Unit* DocUnitList::FindUnit(INT32 Index)
{
	ERROR3IF(Index < 0,"DocUnitList::FindUnit A negative index? Now there's a novelty");
	ERROR3IF(Index >= NumUnits,"DocUnitList::FindUnit Index is greater than num units in system");
	if (Index < 0 || Index >= NumUnits)
		return NULL;	// Problem, so return quickly

	Unit* pUnit = (Unit*) this->GetHead();

	while ((pUnit != NULL) && (Index > 0))
	{
		pUnit = (Unit*) this->GetNext(pUnit);
		Index--;
	}

	ERROR3IF(pUnit == NULL,"DocUnitList::FindUnit Can't find the unit");

	return pUnit;
}

/*********************************************************************************************

> UnitType DocUnitList::FindUnitType(const StringBase& Str)

Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
Created:	5/7/94
Inputs:		Str = string specifying a unit specifier
Returns:	The type of unit represented by the string passed in
Purpose:	Identifies the unit by the unit specifier string
			E.g. 'cm', 'km','mi', 'bananas', etc
			If the unit is not recognised, NOTYPE is returned
SeeAlso:	DocUnitList::FindUnitTypeFromToken;

*********************************************************************************************/

UnitType DocUnitList::FindUnitType(const StringBase& Str)
{
	String_32 Specifier;
	String_32 SearchStr;
	INT32 len = min(32,Str.Length());
	const TCHAR* p1 = (const TCHAR *) Str;
	TCHAR* p2 = (TCHAR *) SearchStr;

	// Copy param Str into SearchStr
	INT32 i;
	for (i=0;i<len;i++)
		p2[i] = p1[i];

	p2[i] = 0;				// Terminate
	SearchStr.toLower();	// and convert to lower case

	Unit* pUnit = (Unit*) this->GetHead();

	while (pUnit != NULL)
	{
		Specifier = pUnit->GetSpecifier();
		Specifier.toLower();

		if (Specifier == SearchStr)
			return (pUnit->GetUnitType());

		pUnit = (Unit*) this->GetNext(pUnit);
	}

	return NOTYPE;
}


/*********************************************************************************************

> UnitType DocUnitList::FindUnitTypeFromToken(const StringBase& Str)

Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
Created:	1/9/95
Inputs:		Str = string specifying a unit token
Returns:	The type of unit represented by the string passed in
Purpose:	Identifies the unit by the unit token string
			E.g. 'cm', 'km','mi', 'bananas', etc
			If the unit is not recognised, NOTYPE is returned
SeeAlso:	DocUnitList::FindUnitType;

*********************************************************************************************/

UnitType DocUnitList::FindUnitTypeFromToken(const StringBase& Str)
{
	String_32 Token;
	String_32 SearchStr;
	INT32 len = min(32,Str.Length());
	const TCHAR* p1 = Str;
	TCHAR* p2 = SearchStr;

	// Copy param Str into SearchStr
	INT32 i;
	for (i=0;i<len;i++)
		p2[i] = p1[i];

	p2[i] = 0;				// Terminate
	SearchStr.toLower();	// and convert to lower case

	Unit* pUnit = (Unit*) this->GetHead();

	while (pUnit != NULL)
	{
	 	Token = pUnit->GetToken();
		Token.toLower();

		if (Token == SearchStr)
			return (pUnit->GetUnitType());

		pUnit = (Unit*) this->GetNext(pUnit);
	}

	return NOTYPE;
}


/*********************************************************************************************

> INT32 DocUnitList::FindUnitIndex(UnitType ThisUnit)

Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
Created:	5/7/94
Inputs:		ThisUnit = Type of unit you want
Returns:	The position of this unit in the list of units.
Purpose:	Get the position of the unit in the list of units.
			Used mainly in conjunction with drop-down lists of units

*********************************************************************************************/

INT32	DocUnitList::FindUnitIndex(UnitType ThisUnit)
{
	Unit* pUnit = (Unit*) this->GetHead();
	INT32 Index = 0;

	while ((pUnit != NULL) && (pUnit->GetUnitType() != ThisUnit))
	{
		pUnit = (Unit*) this->GetNext(pUnit);
		Index++;
	}

	ERROR3IF(pUnit == NULL,"Can't find the unit");
	if (pUnit == NULL) Index = 0; 	// Safe for retail builds

	return Index;
}


/*********************************************************************************************

> Unit* DocUnitList::FindFirstUserUnit()

Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
Created:	5/7/94
Inputs:		-
Outputs:	-
Returns:	Ptr to the first user (or non-default) unit in the list
			NULL is returned if all the units are default ones, or the list is empty
Purpose:	Lets you get hold of that elusive first user-defined unit.
			All units are either default or user-defined units.

*********************************************************************************************/

Unit* DocUnitList::FindFirstUserUnit()
{
	// Get a pointer to the first unit in the list
	Unit* pUnit = (Unit*) this->GetHead();

	while (pUnit != NULL)
	{
		if (!pUnit->IsDefault())
			return pUnit;

		pUnit = (Unit*) this->GetNext(pUnit);
	}

	return (pUnit);
}


/*********************************************************************************************

> Unit* DocUnitList::FindNextUserUnit(Unit* pUnit)

Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
Created:	5/7/94
Inputs:		pUnit = ptr to a unit in the list
Outputs:	-
Returns:	Ptr to the first user (or non-default) unit in the list
			NULL is returned if all the units are default ones, or the list is empty
Purpose:	Lets you get hold of that elusive first user-defined unit.
			All units are either default or user-defined units.

*********************************************************************************************/

Unit* DocUnitList::FindNextUserUnit(Unit* pUnit)
{
	pUnit = (Unit*) this->GetNext(pUnit);

	while (pUnit != NULL)
	{
		if (!pUnit->IsDefault())
			return pUnit;

		pUnit = (Unit*) this->GetNext(pUnit);
	}

	return (pUnit);
}


/*********************************************************************************************

> Unit* DocUnitList::FindUserUnit(INT32 Index)

Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
Created:	7/7/94
Inputs:		Index = the index into the list of user-defined units
Outputs:	-
Returns:	Ptr to the Index-th user (or non-default) unit in the list
			NULL is returned if there isn't an Index-th user unit
Purpose:	Lets you get hold of that elusive Index-th user-defined unit.
SeeAlso:	FindFirstUserUnit,FindNextUserUnit

*********************************************************************************************/

Unit* DocUnitList::FindUserUnit(INT32 Index)
{
	if (Index < 0) return NULL;

	Unit* pUnit = FindFirstUserUnit();

	while (pUnit != NULL && Index > 0)
	{
		pUnit = FindNextUserUnit(pUnit);
		Index--;
	}

	return (pUnit);
}


/********************************************************************************************

> void DocUnitList::RecalcUnit(Unit* pThisUnit)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		pThisUnit = unit to recalculate
	Outputs:	-
	Returns:	-
	Purpose:	Recalculates the number of millipoints in this unit by multiplying the ratio
				between this and the base unit, with the num millipoints in the base unit.
				E.g. Recalc Metres :
					Num Millipoints in metre = Num MP in centimetre * 100;
				IMPORTANT: This can only recalculate units that are based on another unit.
				If the unit is not based on another unit (i.e. its base type is NOTYPE) then
				nothing happens.

********************************************************************************************/

void DocUnitList::RecalcUnit(Unit* pThisUnit)
{
	if (pThisUnit->GetBaseUnitType() == NOTYPE) return;

	Unit* pBaseUnit = FindUnit(pThisUnit->GetBaseUnitType());
	double ratio = pThisUnit->GetBaseNumerator() / pThisUnit->GetBaseDenominator();

	pThisUnit->SetMillipoints(pBaseUnit->GetMillipoints() * ratio);
}

/********************************************************************************************

> BOOL DocUnitList::RecalcUnit(Unit* pThisUnit,UnitType ChangedUnitType)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		pThisUnit       = Unit to ask to recalculate
				ChangedUnitType = the unit type that has changed it's definition
								  i.e. the unit's Millipoint value has changed
	Outputs:	-
	Returns:	TRUE  - This unit has changed also as a result of unit ChangedUnitType changing
				FALSE - Hasn't changed
	Purpose:	This recalcs the unit's definition (i.e. the num millipoints that make it up)
				if it is a descendent of ChangedUnitType.

********************************************************************************************/
/*	Implementation

	Principle:

		A->B->C (B is based on A, and C is based on B)
		If A changes, then B and C must be recalculated
		It's important that B is recalculated before C

	Algorithm:

		Is this unit based on another?
			If no, return FALSE;

		Is this unit based on the changed unit?
			Is yes, recalc and return TRUE;

		Find this unit's base unit

		Has this unit's base unit recalculated as a result of the
		original unit change?
			If yes, recalc and return TRUE;
*/

BOOL DocUnitList::RecalcUnit(Unit* pThisUnit,UnitType ChangedUnitType)
{
	if (pThisUnit->GetBaseUnitType() == NOTYPE)
		return FALSE;

	if (pThisUnit->GetBaseUnitType() == ChangedUnitType)
	{
		RecalcUnit(pThisUnit);
		return TRUE;
	}

	Unit* pBaseUnit = FindUnit(pThisUnit->GetBaseUnitType());

	if (RecalcUnit(pBaseUnit,ChangedUnitType))
	{
		RecalcUnit(pThisUnit);
		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

> void DocUnitList::UnitHasChanged(UnitType ChangedUnitType)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		ChangedUnitType = the unit type that has changed it's definition
								  i.e. the unit's Millipoint value has changed
	Outputs:	-
	Returns:	-
	Purpose:	This informs the class that the definition of a unit has changed.
				It ensures that all units that are descendents of the changed unit
				are recalculated.

********************************************************************************************/

void DocUnitList::UnitHasChanged(UnitType ChangedUnitType)
{
	Unit* pUnit = (Unit*) this->GetHead();

	while (pUnit != NULL)
	{
		RecalcUnit(pUnit,ChangedUnitType);
		pUnit = (Unit*) this->GetNext(pUnit);
	}

   	// Tell everybody that the unit has changed
	BROADCAST_TO_ALL(UnitMsg(this,ChangedUnitType,UnitMsg::CHANGED));
}

/********************************************************************************************

> BOOL DocUnitList::IsDescendent(Unit* pThisUnit, UnitType NewUnitType)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		pThisUnit = ptr to unit in question
				NewUnitType = the unit type to check against accendency
	Outputs:	-
	Returns:	TRUE  - This unit is a descendent of NewUnitType
				FALSE - Never heard of the unit guv.
	Purpose:	Checks to see if this unit is a descendent of NewUnitType
				E.g. A->B->C (B is based on A, and C is based on B)
				Both B and C are descendents of A

********************************************************************************************/
/* 	Implementation note.

	Algorithm:

		Is unit based on another?
			If no, return FALSE;

		Is unit based on the new unit?
			If yes, return TRUE

		Check to see if unit's base unit is a descendent of the new base unit
*/

BOOL DocUnitList::IsDescendent(Unit* pThisUnit,UnitType NewUnitType)
{
	if (pThisUnit->GetBaseUnitType() == NOTYPE)
		return FALSE;	

	if (pThisUnit->GetBaseUnitType() == NewUnitType)
		return TRUE;

	Unit* pBaseUnit = FindUnit(pThisUnit->GetBaseUnitType());

	return (IsDescendent(pBaseUnit,NewUnitType));
}

/********************************************************************************************

> BOOL DocUnitList::HasDescendents(UnitType ThisUnitType)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/94
	Inputs:		ThisUnitType = The unit type to check for decendents
	Outputs:	-
	Returns:	TRUE  - This unit is has one or more descendents
				FALSE - No other unit is based on this unit
	Purpose:	Finds out is there are other units that are decendents of this unit,
				i.e. other units that are based on this unit either directly or indirectly
				e.g. A->B->C (B is based on A, C is based on B). 
				Both A nd B have decendents. C does not.

********************************************************************************************/

BOOL DocUnitList::HasDescendents(UnitType ThisUnitType)
{
	Unit* pUnit = (Unit*) this->GetHead();

	while (pUnit != NULL)
	{
		if (pUnit->GetUnitType() != ThisUnitType)
		{
			if (IsDescendent(pUnit,ThisUnitType))
				return TRUE;
		}
		pUnit = (Unit*) this->GetNext(pUnit);
	}

	return FALSE;
}

/********************************************************************************************

>	BOOL DocUnitList::SetBaseUnitType(Unit* pThisUnit,UnitType NewBaseUnitType)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7//94
	Inputs:		NewBaseUnitType = the unit this unit is based on
	Outputs:	-
	Returns:	TRUE if all went OK
				FALSE if NewBaseUnitType is this unit or a descendent of this unit
	Purpose:	Lets you set that base unit type, dude!

********************************************************************************************/

BOOL DocUnitList::SetBaseUnitType(Unit* pThisUnit,UnitType NewBaseUnitType)
{
	if (pThisUnit->GetUnitType() == NewBaseUnitType)
		return FALSE;

	if (NewBaseUnitType != NOTYPE)
	{
		Unit* pNewBaseUnit = FindUnit(NewBaseUnitType);
		
		if (IsDescendent(pNewBaseUnit,pThisUnit->GetUnitType()))
			return FALSE;
	}

	pThisUnit->SetBaseUnitType(NewBaseUnitType);
	return TRUE;
}

/********************************************************************************************

>	static DocUnitList* DocUnitList::GetCurrentDocUnitList()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		-
	Outputs:	-
	Returns:	Ptr to the doc unit list object of the current doc, or NULL if there isn't one
	Purpose:	Lets you set that base unit type, dude!

********************************************************************************************/

DocUnitList* DocUnitList::GetCurrentDocUnitList()
{
	// Get the current doc and return if there is not one
	Document* pDoc = Document::GetCurrent();
	//ERROR3IF(pDoc==NULL, "Can't find the current doc");
	if (pDoc==NULL)
	  	return NULL;

	// get the Unit list and return it
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	ERROR3IF(pDoc->GetDocUnitList()==NULL, "Can't find the doc unit list in the current doc");
#endif
	return (pDoc->GetDocUnitList());
}

/********************************************************************************************

>	void DocUnitList::SetScaleUnits(UnitType NewUnits)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		NewUnits = the new units to use
	Outputs:	-
	Returns:	-
	Purpose:	Sets the scale units of the object directly
	SeeAlso:	-

********************************************************************************************/

void DocUnitList::SetScaleUnits(UnitType NewUnits)
{
	ScaleUnits = NewUnits;
}

/********************************************************************************************

>	UnitType DocUnitList::GetScaleUnits()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		-
	Outputs:	-
	Returns:	The scale units of this object
	Purpose:	For getting hold of those pesky scale units
	SeeAlso:	-

********************************************************************************************/

UnitType DocUnitList::GetScaleUnits()
{
	return ScaleUnits;
}

/********************************************************************************************

>	void DocUnitList::SetPageUnits(UnitType NewUnits)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		NewUnits = the new units to use
	Outputs:	-
	Returns:	-
	Purpose:	Sets the page units of the object directly
	SeeAlso:	-

********************************************************************************************/

void DocUnitList::SetPageUnits(UnitType NewUnits)
{
	PageUnits = NewUnits;
}

/********************************************************************************************

>	UnitType DocUnitList::GetPageUnits()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		-
	Outputs:	-
	Returns:	The page units of this object
	Purpose:	For getting hold of those pesky page units
	SeeAlso:	-

********************************************************************************************/

UnitType DocUnitList::GetPageUnits()
{
	return (PageUnits);
}

/********************************************************************************************

>	void DocUnitList::SetFontUnits(UnitType NewUnits)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		NewUnits = the new units to use
	Outputs:	-
	Returns:	-
	Purpose:	Sets the font units of the object directly
	SeeAlso:	-

********************************************************************************************/

void DocUnitList::SetFontUnits(UnitType NewUnits)
{
	FontUnits = NewUnits;
}

/********************************************************************************************

>	UnitType DocUnitList::GetFontUnits()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		-
	Outputs:	-
	Returns:	The font units of this object
	Purpose:	For getting hold of those pesky font units
	SeeAlso:	-

********************************************************************************************/

UnitType DocUnitList::GetFontUnits()
{
	return (FontUnits);
}

/********************************************************************************************

>	String_32 DocUnitList::GetToken(UnitType ThisUnitType)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		ThisUnitType = type of unit you want token of
	Outputs:	-
	Returns:	The token for the unit, i.e. the string that describes the unit
				e.g. "Centimetres"
	Purpose:	Easy way of getting a unit token string.
				IMPORTANT: This is a general function that can cope with special UnitType values
				like AUTOMATIC and NOTYPE, i.e. types that don't correspond to an actual instance
				of Unit.
	SeeAlso:	DocUnitList::GetSpecifier; Unit::GetSpecifier;

********************************************************************************************/

String_32 DocUnitList::GetToken(UnitType ThisUnitType)
{        
	String_32 Str;
	
	switch (ThisUnitType)
	{
		case AUTOMATIC :
			Str = String_32(_R(IDS_AUTOMATIC));
			break;

		case NOTYPE :
			Str = String_32(_R(IDS_NOTYPE));
			break;

		default :
			{
				Unit * pUnit = NULL;
				pUnit = FindUnit(ThisUnitType);
				if (pUnit)
					Str = pUnit->GetToken();
				else
					Str = String_32(_R(IDS_NOTYPE));	
			}
			
			break;
	}

	return Str;
}

/********************************************************************************************

>	String_32 DocUnitList::GetSpecifier(UnitType ThisUnitType)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/95
	Inputs:		ThisUnitType = type of unit you want specfier of
	Outputs:	-
	Returns:	The Specifier for the unit, i.e. the string that describes the unit
				e.g. "cm"
	Purpose:	Easy way of getting a unit specifier string.
				If bad UnitType and hence no unit found, returns a blank string 
	SeeAlso:	DocUnitList::GetToken; Unit::GetToken

********************************************************************************************/

String_32 DocUnitList::GetSpecifier(UnitType ThisUnitType)
{        
	String_32 Str = _T("");
	
	Unit* pUnit = NULL;
	pUnit = FindUnit(ThisUnitType);
	if (pUnit)
		Str = pUnit->GetSpecifier();

	return Str;
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

// Tokens used in winoil\units.ini
static struct
{
	TCHAR*  Token;
} TokenTable[] = 
{
	{_T("Unit")},
	{_T("PageUnits")},
	{_T("FontUnits")},
	{_T("UnitsEnd")}
};

// Token indexes for the above tokens
enum UnTokenIndex
{
	UnTOKEN_NONE = -1,
	TOKEN_UNIT,
	TOKEN_PAGEUNITS,
	TOKEN_FONTUNITS,
	TOKEN_UNITSEND,
	// Add new token indexs BEFORE NUM_TOKENS
	UnNUM_TOKENS
};

// function for turning a token into a token indexes
static UnTokenIndex FindToken(const TCHAR* Token)
{
	for (INT32 i = 0; i < UnNUM_TOKENS; i++)
		if (camStrcmp(TokenTable[i].Token,Token) == 0) return (UnTokenIndex) i;

	return UnTOKEN_NONE;
}


/********************************************************************************************

> BOOL DocUnitList::MakeDefaultUnits()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	This creates the default units for a document.
				It tries to find the default units via three sources (highest priority first):
					Disk (Not implemented yet)
					Bound-in resource (defined in winoil\units.ini)
					Hard-wired defaults.
	SeeAlso:	-

********************************************************************************************/

BOOL DocUnitList::MakeDefaultUnits()
{
	ERROR3IF(DefaultsMade,"MakeDefaultUnits has already been called for this instance of DocUnitList");
	if (DefaultsMade) return TRUE;
	
	// Ensure the list is empty before proceeding
	DeleteAll();

	BOOL ok = TRUE;
	ok = ReadUnitsFromDisk();

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	if (!ok) ok = ReadUnitsFromRes();
#endif

	if (!ok) ok = MakeFactoryDefaultUnits();

	DefaultsMade = ok;

	return (ok);
}

/********************************************************************************************

> BOOL DocUnitList::ReadUnitsFromDisk()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads the unit definitions from disk (i.e. writable media)
	SeeAlso:	-

********************************************************************************************/

BOOL DocUnitList::ReadUnitsFromDisk()
{	
	// Not yet implemented

	return FALSE;
}

/********************************************************************************************

> BOOL DocUnitList::ReadUnitsFromRes()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads the unit definitions from the bound in resource.
	SeeAlso:	-

********************************************************************************************/

BOOL DocUnitList::ReadUnitsFromRes()
{
	PORTNOTETRACE("other","Removed DocUnitList::ReadUnitsFromRes()");
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Remove all previously defined units.  We're starting from scratch
	DeleteAll();

	CCResTextFile file;				// Resource File

	BOOL ok = file.open(_R(IDM_DEFAULT_UNITS),_R(IDT_CAM_UNITS));		// Open resource

	if (ok)
	{
		ok = ReadUnitsFromFile(file);
		file.close();
	}

	return (ok);
#else
	return FALSE;
#endif
}


/********************************************************************************************

> BOOL DocUnitList::OutputTrace(char* err,const TCHAR* TokenBuf)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Inputs:		err 	 = ptr to a string to appear in the trace output
				TokenBuf = ptr to buffer holding last token read
	Outputs:	-
	Returns:	-
	Purpose:	Helper funtion for the token-reading functions
	SeeAlso:	-

********************************************************************************************/

void DocUnitList::OutputTrace( char *err, const TCHAR *TokenBuf )
{
#if 0 != wxUSE_UNICODE
	size_t				cch = camMbstowcs( NULL, err, 0 ) + 1;
	wchar_t			   *pwsz = (wchar_t *)alloca( cch * 2 );
	camMbstowcs( pwsz, err, cch );
	
	TRACE( pwsz );
#else
	TRACE( err );
#endif
	
	TRACE( _T(", but got %s\n"), TokenBuf );
}

/********************************************************************************************

> BOOL DocUnitList::ReadLong(CCLexFile& file,void* pLong,char* err)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Inputs:		file = file to read INT32 value from
				pLong = ptr to place to store the INT32 value
				err   = ptr to a string to appear in the trace output if things go wrong
	Outputs:	*pLong = INT32 value read
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Reads a INT32 value from file and puts it in pLong.
				If it fails, err is passed to OutputTrace() and FALSE is returned

				Note: pLong is a void* so that any INT32 type var can be passed in, like enum types.
	SeeAlso:	-

********************************************************************************************/

BOOL DocUnitList::ReadLong(CCLexFile& file,void* pLong,char* err)
{
	ERROR2IF(pLong == NULL,FALSE,"pLong == NULL");

	const TCHAR		   *TokenBuf = file.GetTokenBuf();
	BOOL ok = file.GetSimpleToken();
	if (ok)
		ok = ( camSscanf( TokenBuf, _T("%li"), pLong ) == 1 );

	if (!ok)
		OutputTrace( err, TokenBuf );

	return ok;
}

/********************************************************************************************

> BOOL DocUnitList::ReadDouble(CCLexFile& file,double* pDouble,char* err)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Inputs:		file 	= file to read INT32 value from
				pDouble = ptr to place to store the double value
				err   	= ptr to a string to appear in the trace output if things go wrong
	Outputs:	*pDouble  = double value read
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Reads a double value from file and puts it in pDouble.
				If it fails, err is passed to OutputTrace() and FALSE is returned
	SeeAlso:	-

********************************************************************************************/

BOOL DocUnitList::ReadDouble(CCLexFile& file,double* pDouble,char* err)
{
	ERROR2IF(pDouble == NULL,FALSE,"pDouble == NULL");

	const TCHAR		    *TokenBuf = file.GetTokenBuf();
	BOOL				ok = file.GetSimpleToken();
	if (ok)
		ok = ( camSscanf( TokenBuf, _T("%lg"), pDouble ) == 1 );

	if (!ok)
		OutputTrace( err, TokenBuf );

	return ok;
}

/********************************************************************************************

> BOOL DocUnitList::ReadString(CCLexFile& file,StringBase* pStr,char* err)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Inputs:		file = file to read INT32 value from
				pStr = ptr to place to store the string 
				err  = ptr to a string to appear in the trace output if things go wrong
	Outputs:	*pStr = string value read
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Reads a string from file and puts it in pStr.
				If it fails, err is passed to OutputTrace() and FALSE is returned
	SeeAlso:	-

********************************************************************************************/

BOOL DocUnitList::ReadString(CCLexFile& file,StringBase* pStr,char* err)
{
	ERROR2IF(pStr == NULL,FALSE,"pStr == NULL");

	const TCHAR		   *TokenBuf = file.GetTokenBuf();
	BOOL				ok = file.GetSimpleToken();
	if (ok)
	{
		pStr->Empty();
		INT32 len = pStr->MaxLength();
		if (len > 0)
		{
			TCHAR	   *pBuf = *pStr;
			INT32			i;
			for ( i = 0; TBYTE(TokenBuf[i]) > TBYTE(' ') && i < len; i++ )
				pBuf[i] = TokenBuf[i];

			pBuf[i] = 0;
		}
	}

	if (!ok) OutputTrace(err,TokenBuf);

	return ok;
}		

/********************************************************************************************

> BOOL DocUnitList::ReadUnitsFromFile(CCLexFile& file)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/95
	Inputs:		file = file that contains the unit definitions
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads the unit definitions from the given file
	SeeAlso:	-

********************************************************************************************/

BOOL DocUnitList::ReadUnitsFromFile(CCLexFile& file)
{
	BOOL finished = FALSE;
	BOOL ok;;	
	
	// Initialise lexing routines, and aspects of the lexer
	ok = file.InitLexer();
	file.SetDelimiters("\r\n");			// Set token delimiting characters
	file.SetCommentMarker(';');			// Set comment marker char
	file.SetWhitespace(" \t");			// Set whitespace chars
	file.SetStringDelimiters("\"\"");	// Set string delimiters

	UnTokenIndex Token;
	const TCHAR		   *TokenBuf = file.GetTokenBuf();	// Token buffer remains constant until lexer deinitialisation
	
	while (ok && !finished)
	{
		// Grab a token
		ok = file.GetSimpleToken();

		// Look the token up in our table
		Token = FindToken( TokenBuf );

		switch (Token)
		{
			case TOKEN_UNIT:
			{
				Unit* pUnit;
				// Defining a unit.  Firstly make a new blank unit to put the definition in
				if (MakeNewUnit(&pUnit,FALSE))
				{
					// We have a blank unit, now read its definition fields

					double NumMillipoints;
					String_32 NameStr;
					String_32 SpecifierStr;
					UnitType TheUnitType,BaseUnitType;
					BOOL Prefix;
					double BaseNumerator,BaseDenominator;

					// Read in all the fields of the unit definition
					if (ok) ok = ReadLong(	file,&TheUnitType,		"Expected the unit's type");
					if (ok) ok = ReadString(file,&NameStr,			"Expected the unit's name");
					if (ok) ok = ReadString(file,&SpecifierStr,		"Expected the unit's specifier");
					if (ok) ok = ReadDouble(file,&NumMillipoints,	"Expected the unit's millipoint value");
					if (ok) ok = ReadLong(	file,&Prefix,			"Expected the unit's prefix flag");
					if (ok) ok = ReadLong(	file,&BaseUnitType,		"Expected the unit's base type");
					if (ok) ok = ReadDouble(file,&BaseNumerator,	"Expected the unit's base numerator");
					if (ok) ok = ReadDouble(file,&BaseDenominator,	"Expected the unit's base denominator");

					if (ok)
					{
						// If it's based on another unit, get a pointer to the base unit
						Unit* pBaseUnit = NULL;
						if (BaseUnitType != UNIT_NOTYPE)
							pBaseUnit = FindUnit(BaseUnitType);

						// If no base unit AND no millipoint value, error
						if (pBaseUnit == NULL && NumMillipoints <= 0.0)
						{
							TRACE( _T("Base unit not found AND the number of millipoints <= 0.0\n"));
							TRACE( _T("Either the base unit is not yet defined, or the millipoint value is wrong\n"));
							ok= FALSE;
						}
						else
						{
							if (pBaseUnit != NULL)
							{
								// If Millipoint value is based on another, calculate it
								// (error if denominator is 0 - we don't want to divide by this value)
								if (BaseDenominator != 0.0)
									NumMillipoints = (pBaseUnit->GetMillipoints()*BaseNumerator)/BaseDenominator;
								else
								{
									TRACE( _T("Base denominator is 0\n"));
									ok = FALSE;
								}
							}

							// If we still have a silly millipoint value, error
							if (NumMillipoints <= 0.0)
							{
								TRACE( _T("The millipoint value is <= 0.0\n"));
								ok = FALSE;
							}

							if (ok)
							{
								// Apply the read values
								pUnit->SetUnitType(TheUnitType);
								pUnit->SetDefaultState(TRUE);
								pUnit->SetPrefixState(Prefix);
								pUnit->SetBaseUnitType(BaseUnitType);

								pUnit->SetBaseNumerator(BaseNumerator);
								pUnit->SetBaseDenominator(BaseDenominator);
								pUnit->SetMillipoints(NumMillipoints);

								if (ok)
								{
									ok = pUnit->SetToken(NameStr);
									if (!ok) TRACE( _T("There's a problem with the name string : %s\n"),(TCHAR*)NameStr);
								}

								if (ok)
								{
									ok = pUnit->SetSpecifier(SpecifierStr);
									if (!ok) TRACE( _T("There's a problem with the specifier string : %s\n"),(TCHAR*)NameStr);
								}
							}
						}
					}
				}
			}
			break;

			case TOKEN_PAGEUNITS:
			{
				UnitType TheUnitType;
				if (ok) ok = ReadLong(file,&TheUnitType,"Expected the Page unit's type");

				if (ok)
				{
					ok = (FindUnit(TheUnitType) != NULL);
					if (!ok) TRACE( _T("The PageUnit specified cannot be found\n"));
				}					

				if (ok) SetPageUnits(TheUnitType);
			}
			break;

			case TOKEN_FONTUNITS:
			{
				UnitType TheUnitType;
				if (ok) ok = ReadLong(file,&TheUnitType,"Expected the Font unit's type");
				if (ok)
				{
					ok = (FindUnit(TheUnitType) != NULL);
					if (!ok) TRACE( _T("The FontUnit specified cannot be found\n"));
				}					
				if (ok) SetFontUnits(TheUnitType);
			}
			break;

			case TOKEN_UNITSEND:
				finished = TRUE;
				break;

			default:
				TRACE( _T("DocUnitList: Unexpected token - %s\n"),TokenBuf);
				break;
		}
	}

	if (!ok)
	{
		TRACE( _T("\nDocUnitList: Offending line - %s\n"),file.GetLineBuf());
		ERROR3("Error reading units.  See TRACE output for details");
	}

	// We are now finished with the lexer
	file.DeinitLexer();

	return (ok);
}

/********************************************************************************************

>	BOOL DocUnitList::MakeFactoryDefaultUnits()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE  - successfully added all default units to the doc unit list
				FALSE - Can't add all the units to the list (lack of memory most likely)
	Purpose:	Defines all the default units for a document and places them on the list.
				Called early on in the document initialisation routines.
				NOTE: Added some ERROR2 checks so that problems in the default units can be
				trapped early on rather than a lot later. Should not normally go off unless
				some of the error checking has problems.

				(Markn 23/11/95 - The name of this function has changed so that it is no longer the
				routine called by the document initialisation code)

	Errors:		Can only be called once per instance. Errors if called again.

********************************************************************************************/

BOOL DocUnitList::MakeFactoryDefaultUnits()
{
	// Remove all previously defined units.  We're starting from scratch
	DeleteAll();

	Unit* pUnit = NULL;
	BOOL ok = TRUE;

	// Default unit - Millimetres
	if (MakeNewUnit(&pUnit,FALSE))
	{
		String_32 TokenStr(_R(IDS_MILLIMETRES));
		String_32 SpecifierStr(_R(IDS_MM));

		pUnit->SetDefaultState(TRUE);
		pUnit->SetPrefixState(FALSE);
		pUnit->SetUnitType(MILLIMETRES);


		ok = 	pUnit->SetToken(TokenStr) &&
				pUnit->SetSpecifier(SpecifierStr) &&
				pUnit->SetBaseUnitType(NOTYPE) &&
				pUnit->SetMillipoints(MM_MP_VAL);
			 
		ERROR2IF(!ok,FALSE,"DocUnitList::MakeFactoryDefaultUnits bad MILLIMETRES");
	}
	else
		return FALSE;

	// Default unit - Centimetres
	if (MakeNewUnit(&pUnit,FALSE))
	{
		String_32 TokenStr(_R(IDS_CENTIMETRES));
		String_32 SpecifierStr(_R(IDS_CM));

		pUnit->SetUnitType(CENTIMETRES);
		pUnit->SetDefaultState(TRUE);
		pUnit->SetPrefixState(FALSE);
		pUnit->SetBaseUnitType(MILLIMETRES);

		ok = 	pUnit->SetBaseNumerator(10) &&
				pUnit->SetBaseDenominator(1) &&
				pUnit->SetMillipoints(CM_MP_VAL) &&
				pUnit->SetToken(TokenStr) &&
				pUnit->SetSpecifier(SpecifierStr);

		ERROR2IF(!ok,FALSE,"DocUnitList::MakeFactoryDefaultUnits bad CENTIMETRES");
	}
	else
		return FALSE;

	// Default unit - Metres
	if (MakeNewUnit(&pUnit,FALSE))
	{
		String_32 TokenStr(_R(IDS_METRES));
		String_32 SpecifierStr(_R(IDS_M));

		pUnit->SetUnitType(METRES);
		pUnit->SetDefaultState(TRUE);
		pUnit->SetPrefixState(FALSE);
		pUnit->SetBaseUnitType(CENTIMETRES);
		
		ok = 	pUnit->SetBaseNumerator(100) &&
				pUnit->SetBaseDenominator(1) &&
				pUnit->SetMillipoints(M_MP_VAL) &&
				pUnit->SetToken(TokenStr) &&
				pUnit->SetSpecifier(SpecifierStr);
		ERROR2IF(!ok,FALSE,"DocUnitList::MakeFactoryDefaultUnits bad METRES");
	}
	else
		return FALSE;

	// Default unit - Kilometres
	if (MakeNewUnit(&pUnit,FALSE))
	{
		String_32 TokenStr(_R(IDS_KILOMETRES));
		String_32 SpecifierStr(_R(IDS_KM));

		pUnit->SetUnitType(KILOMETRES);
		pUnit->SetDefaultState(TRUE);
		pUnit->SetPrefixState(FALSE);
		pUnit->SetBaseUnitType(METRES);
		ok = 	pUnit->SetBaseNumerator(1000) &&
				pUnit->SetBaseDenominator(1) &&
				pUnit->SetMillipoints(KM_MP_VAL) &&
				pUnit->SetToken(TokenStr) &&
				pUnit->SetSpecifier(SpecifierStr);
		ERROR2IF(!ok,FALSE,"DocUnitList::MakeFactoryDefaultUnits bad KILOMETRES");
	}
	else
		return FALSE;

	// Default unit - Millipoints
	if (MakeNewUnit(&pUnit,FALSE))
	{
		String_32 TokenStr(_R(IDS_MILLIPOINTS));
		String_32 SpecifierStr(_R(IDS_MP));

		pUnit->SetUnitType(MILLIPOINTS);
		pUnit->SetDefaultState(TRUE);
		pUnit->SetPrefixState(FALSE);
		pUnit->SetBaseUnitType(NOTYPE);
		ok = 	pUnit->SetMillipoints(MP_MP_VAL) &&
				pUnit->SetToken(TokenStr) &&
				pUnit->SetSpecifier(SpecifierStr);
		ERROR2IF(!ok,FALSE,"DocUnitList::MakeFactoryDefaultUnits bad MILLIPOINTS");
	}
	else
		return FALSE;

	// Default unit - Computer points
	if (MakeNewUnit(&pUnit,FALSE))
	{
		String_32 TokenStr(_R(IDS_COMP_POINTS));
		String_32 SpecifierStr(_R(IDS_PT));

		pUnit->SetUnitType(COMP_POINTS);
		pUnit->SetDefaultState(TRUE);
		pUnit->SetPrefixState(FALSE);
		pUnit->SetBaseUnitType(MILLIPOINTS);
		ok = 	pUnit->SetBaseNumerator(1000) &&
				pUnit->SetBaseDenominator(1) &&
				pUnit->SetMillipoints(PT_MP_VAL) &&
				pUnit->SetToken(TokenStr) &&
				pUnit->SetSpecifier(SpecifierStr);
		ERROR2IF(!ok,FALSE,"DocUnitList::MakeFactoryDefaultUnits bad COMP_POINTS");
	}
	else
		return FALSE;

	// Default unit - Picas
	if (MakeNewUnit(&pUnit,FALSE))
	{
		String_32 TokenStr(_R(IDS_PICAS));
		String_32 SpecifierStr(_R(IDS_PI));

		pUnit->SetUnitType(PICAS);
		pUnit->SetDefaultState(TRUE);
		pUnit->SetPrefixState(FALSE);
		pUnit->SetBaseUnitType(COMP_POINTS);
		ok =	pUnit->SetBaseNumerator(12) &&
				pUnit->SetBaseDenominator(1) &&
				pUnit->SetMillipoints(PI_MP_VAL) &&
				pUnit->SetToken(TokenStr) &&
				pUnit->SetSpecifier(SpecifierStr);
		ERROR2IF(!ok,FALSE,"DocUnitList::MakeFactoryDefaultUnits bad PICAS");
	}
	else
		return FALSE;

	// Default unit - Inches
	if (MakeNewUnit(&pUnit,FALSE))
	{
		String_32 TokenStr(_R(IDS_INCHES));
		String_32 SpecifierStr(_R(IDS_IN));

		pUnit->SetUnitType(INCHES);
		pUnit->SetDefaultState(TRUE);
		pUnit->SetPrefixState(FALSE);
		pUnit->SetBaseUnitType(PICAS);
		ok = 	pUnit->SetBaseNumerator(6) &&
				pUnit->SetBaseDenominator(1) &&
				pUnit->SetMillipoints(IN_MP_VAL) &&
				pUnit->SetToken(TokenStr) &&
				pUnit->SetSpecifier(SpecifierStr);
		ERROR2IF(!ok,FALSE,"DocUnitList::MakeFactoryDefaultUnits bad INCHES");
	}
	else
		return FALSE;

	// Default unit - Feet
	if (MakeNewUnit(&pUnit,FALSE))
	{
		String_32 TokenStr(_R(IDS_FEET));
		String_32 SpecifierStr(_R(IDS_FT));

		pUnit->SetUnitType(FEET);
		pUnit->SetDefaultState(TRUE);
		pUnit->SetPrefixState(FALSE);
		pUnit->SetBaseUnitType(INCHES);
		ok =	pUnit->SetBaseNumerator(12) &&
				pUnit->SetBaseDenominator(1) &&
				pUnit->SetMillipoints(FT_MP_VAL) &&
				pUnit->SetToken(TokenStr) &&
				pUnit->SetSpecifier(SpecifierStr);
		ERROR2IF(!ok,FALSE,"DocUnitList::MakeFactoryDefaultUnits bad FEET");
	}
	else
		return FALSE;

	// Default unit - Yards
	if (MakeNewUnit(&pUnit,FALSE))
	{
		String_32 TokenStr(_R(IDS_YARDS));
		String_32 SpecifierStr(_R(IDS_YD));

		pUnit->SetUnitType(YARDS);
		pUnit->SetDefaultState(TRUE);
		pUnit->SetPrefixState(FALSE);
		pUnit->SetBaseUnitType(FEET);
		ok = 	pUnit->SetBaseNumerator(3) &&
				pUnit->SetBaseDenominator(1) &&
				pUnit->SetMillipoints(YD_MP_VAL) &&
				pUnit->SetToken(TokenStr) &&
				pUnit->SetSpecifier(SpecifierStr);
		ERROR2IF(!ok,FALSE,"DocUnitList::MakeFactoryDefaultUnits bad YARDS");
	}
	else
		return FALSE;

	// Default unit - Miles
	if (MakeNewUnit(&pUnit,FALSE))
	{
		String_32 TokenStr(_R(IDS_MILES));
		String_32 SpecifierStr(_R(IDS_MI));

		pUnit->SetUnitType(MILES);
		pUnit->SetDefaultState(TRUE);
		pUnit->SetPrefixState(FALSE);
		pUnit->SetBaseUnitType(YARDS);
		ok = 	pUnit->SetBaseNumerator(1760) &&
				pUnit->SetBaseDenominator(1) &&
				pUnit->SetMillipoints(MI_MP_VAL) &&
				pUnit->SetToken(TokenStr) &&
				pUnit->SetSpecifier(SpecifierStr);
		ERROR2IF(!ok,FALSE,"DocUnitList::MakeFactoryDefaultUnits bad MILES");
	}
	else
		return FALSE;

	// Default unit - Pixels
	if (MakeNewUnit(&pUnit,FALSE))
	{
		String_32 TokenStr(_R(IDS_PIXELS));
		String_32 SpecifierStr(_R(IDS_PX));

		pUnit->SetDefaultState(TRUE);
		pUnit->SetPrefixState(FALSE);
		pUnit->SetUnitType(PIXELS);


		ok = 	pUnit->SetToken(TokenStr) &&
				pUnit->SetSpecifier(SpecifierStr) &&
				pUnit->SetBaseUnitType(NOTYPE) &&
				pUnit->SetMillipoints(PX_MP_VAL);
			 
		ERROR2IF(!ok,FALSE,"DocUnitList::MakeFactoryDefaultUnits bad PIXELS");
	}
	else
		return FALSE;
	
	return TRUE;
}
