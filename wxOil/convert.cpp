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
// These functions provide conversion between Millipoints and Strings.

/*	Author:		Rik
	Created:	11/5/93
	Purpose:	These functions will provide a means of converting from Doc Coords (millipoints)
				to Strings and visa versa. For example, convert 72000 millipoints to the string
				1in or 72pt as well as converting from the string '1in72pt' to 144000
				millipoints. The units to convert to can be chosen, as can the number of 
				decimal points. When converting from string back to millipoints, all the valid
				units are added together. If an error is detected (ie '4cm72zx' ), then
				everything up to the error will be used (4cm).  */



// Better find out about the string class and all the other types and stuff

#include "camtypes.h"							// Good ol' Camtypes
#include "ccmaths.h"							// 64 bit muldiv
#include "convert.h"							// function prototypes
//#include "markn.h"								// For unit constants
#include "node.h"
#include "spread.h"
#include "units.h"
#include "app.h"								// Camelot object
//#include "peter.h"								// _R(IDS_MEMORYFORMAT_BYTE) ...
#include "localenv.h"							// for locale string settings
//#include "richard3.h"							// more naughty inline strings...


// Put my version number into the About box
DECLARE_SOURCE( "$Revision$" );

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

CC_IMPLEMENT_DYNAMIC(DimScale, CCObject)

// Set up the defaults for the Convert class

// The default DimScale object is an inactive one that is used as a fall-back in the 
// frightening event of one not being found by DimScale::GetPtrDimScale(Node*)
DimScale* 	pDefaultDimScale = NULL;

char		Convert::DecimalPoint 		= '.';	// Character to separate whole part of number from fractional part
char 		Convert::ThousandSep  		= ',';	// Char to separate thousands, e.g. 10,000
char 		Convert::MinusSign    		= '-';	// What to show if go less than zero
UINT32 		Convert::NumDecimalPlaces 	= 2;	// Number of decimal places to display to by default

/******************************************************************************************

>	static BOOL Convert::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	AppPrefsDlg Init method. Called when the kernel initialises.
				It reads from the Windows ini file the settings for things like the
				current decimal point character, thousands separator and number of
				decimal places and stores these for later use.
	SeeAlso:	GetCurrentNumberFormat();

******************************************************************************************/

BOOL Convert::Init()
{

//	// Declare any preferences that we require.
//	if ( Camelot.DeclareSection(TEXT("Preferences"), 3) )
//	{
//		// section declared ok so now define the preference option  
//		Camelot.DeclarePref(TEXT("Preferences"), TEXT("DecimalPointChar"), &DecimalPoint, 0, 255);
//		Camelot.DeclarePref(TEXT("Preferences"), TEXT("ThousandSepChar"), &ThousandSep, 0, 255);
//	}

	// Get the current details from the OS about the number format specified by the user
	// If fails then return this to the caller.
	return GetCurrentNumberFormat();
}

/********************************************************************************************

>	static  BOOL Convert::GetCurrentNumberFormat()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		-
	Outputs:	- 
	Returns:	-
	Purpose:	Read from the operating system what the user has defined to be their decimal
				point character and thousands separator character and store these in the
				class variables.
				It is now also called from the mainframe message handler, when it 
				receives a WM_WININICHANGE message.
	Errors:		-
	SeeAlso:	CMainFrame::OnWinIniChange()

********************************************************************************************/

BOOL Convert::GetCurrentNumberFormat()
{
	String_8	ThousandsSepStr( _T(",") ); 
	String_8	DecimalPointStr( _T(".") ); 
	NumDecimalPlaces = 2;

	// Read from the operating system what the user has defined to be their decimal point
	// character and thousands separator character.
	LocalEnvironment::GetThousandsSeparator(&ThousandsSepStr);
	LocalEnvironment::GetDecimalPointChar(&DecimalPointStr);
	LocalEnvironment::GetNumberOfDecimalPlaces(&NumDecimalPlaces);
	
	// Set up the static char variables to reflect these
	ThousandSep = ((TCHAR *) ThousandsSepStr)[0];
	DecimalPoint = ((TCHAR *) DecimalPointStr)[0];

	// Check for "testing" of internationalisation.
	ERROR3IF(ThousandSep == DecimalPoint,
				"Convert::GetCurrentNumberFormat: decimal point and "
				"thousands separator are the same");

//char to TCHAR for DBCS
TRACEUSER( "Neville", wxT("InitConvert() ThousandsSepStr=%s\n"), (TCHAR *)ThousandsSepStr );
TRACEUSER( "Neville", wxT("InitConvert() DecimalPointStr=%s\n"), (TCHAR *)DecimalPointStr );
TRACEUSER( "Neville", wxT("InitConvert() NumDecimalPlaces=%d\n"), NumDecimalPlaces );

	return TRUE;
}


/********************************************************************************************

>	static	char Convert::GetDecimalPointChar()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		-
	Outputs:	- 
	Returns:	The current decimal point character being used.
	Purpose:	To find out what the current decimal point character is.
	SeeAlso:	SetDecimalPointChar(); GetCurrentNumberFormat();

********************************************************************************************/

char Convert::GetDecimalPointChar()
{
	return DecimalPoint;
}

/********************************************************************************************

>	static	char Convert::GetThousandsSepChar()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		-
	Outputs:	- 
	Returns:	The current thousands separator character being used.
	Purpose:	To find out what the current thousands separator character is.
	SeeAlso:	SetThousandsSepChar(); GetCurrentNumberFormat();

********************************************************************************************/

char Convert::GetThousandsSepChar()
{
	return ThousandSep;
}


/********************************************************************************************

>	static	UINT32 Convert::GetNumberDecimalPlaces()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		-
	Outputs:	- 
	Returns:	The current number of decimal places being used.
	Purpose:	To find out what the current number of decimal places is.
	SeeAlso:	SetNumberDecimalPlaces();

********************************************************************************************/

UINT32 Convert::GetNumberDecimalPlaces()
{
	return NumDecimalPlaces;
}

/********************************************************************************************

>	static	BOOL Convert::SetDecimalPointChar(char NewDecimalPoint)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		-
	Outputs:	- 
	Returns:	True if set correctly, False otherwise.
	Purpose:	Allows the setting up of the character to separate whole part of number from
				the fractional part in displayed numbers e.g. 56.79
				This is read from the OS at present. This function just provided just in case
				we wish to overide this in the future.
	SeeAlso:	GetDecimalPointChar(); GetCurrentNumberFormat();

********************************************************************************************/

BOOL Convert::SetDecimalPointChar(char NewDecimalPoint)
{
	// Don't allow a blank to be set.
	//if (NewDecimalPoint = '') return FALSE;

	DecimalPoint = NewDecimalPoint;

	return TRUE;
}

/********************************************************************************************

>	static	BOOL Convert::SetThousandsSepChar(char NewThousandsSep)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		-
	Outputs:	- 
	Returns:	True if set correctly, False otherwise.
	Purpose:	Allows the setting up of the character to be used to separate thousands in
				displayed numbers e.g. 10,000.
				This is read from the OS at present. This function just provided just in case
				we wish to overide this in the future.

	SeeAlso:	GetThousandsSepChar(); GetCurrentNumberFormat();

********************************************************************************************/

BOOL Convert::SetThousandsSepChar(char NewThousandsSep)
{
	// Don't allow a blank to be set.
	//if (NewThousandsSep = '') return FALSE;

	ThousandSep = NewThousandsSep;

	return TRUE;
}

/********************************************************************************************

>	static	BOOL Convert::SetNumberDecimalPlaces(UINT32 NewDPs)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		NewDPs	new number of decimal places to use
	Outputs:	- 
	Returns:	True if set correctly, False otherwise.
	Purpose:	Allows the setting up of the number of decimal places to display to by default
				This is read from the OS at present. This function just provided just in case
				we wish to overide this in the future.
	SeeAlso:	GetNumberDecimalPlaces(); GetCurrentNumberFormat();

********************************************************************************************/

BOOL Convert::SetNumberDecimalPlaces(UINT32 NewDPs)
{
	// Set some limit to the number allowed
	if (NewDPs > 9) return FALSE;

	NumDecimalPlaces = NewDPs; 

	return TRUE;
}


/******************************************************************************************

>	static BOOL Convert::ReplaceDecimalPoint(StringBase* pString)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95
	Inputs:		pString - Pointer to the string to be parsed.
	Outputs:	pString - Pointer to the string that should now have the proper decimal point 
	Returns:	TRUE if the conversion worked, false otherwise
	Purpose:	Searches the specified string for a decimal point and replaces this with the
				specified DecimalPoint character. It assumes that a check has been made
				already to see if we are using a non-decimal point character and so this
				routine is only called when the replacement is actually required.
				This MUST ALWAYS be after the StripTrailingZeros code. Otherwise, the 
				StripTrailingZeros code would have to search for two items, a decimal point
				and the international form of the decimal point.

	SeeAlso:	MillipointsToString; DimScale::ConvertToUnits; DoubleToString;
				Convert::StripTrailingZeros;

********************************************************************************************/

BOOL Convert::ReplaceDecimalPoint(StringBase* pString)
{
	// Could probably be optimised to be quicker

	// Make up a string of the new decimal point character 
	String_8 InsertStr;
	((TCHAR *) InsertStr)[0] = DecimalPoint;
	((TCHAR *) InsertStr)[1] = 0;

	// Work out the position of the current decimal point character, - 1 if none
	INT32	Position;
	String_8			InsertMark(_R(IDS_CONVERT_DP_CHAR));

	Position = pString->Sub(InsertMark);
	
	// If decimal point was found then put the new decimal point character in place 
	if (Position > 0)
	{
		pString->Remove(Position, 1);
		pString->Insert(InsertStr, Position);		   
	}
	
	return TRUE;
}

/********************************************************************************************

>	static BOOL Convert::StripTrailingZeros(StringBase* pString)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/95
	Inputs:		pString - string to have trailing zeros removed
	Outputs:	pString - string with trailing zeros removed
	Returns:	FALSE if fails
	Purpose:	Strip trailing zeros (& decimal point) from a string containing a decimal number
				This MUST ALWAYS be called before the replace decimal point character code.
				Otherwise, we would have to search for two items, a decimal point and the 
				international form of the decimal point.
	SeeAlso:	MillipointsToString; DimScale::ConvertToUnits; DoubleToString;
				Convert::ReplaceDecimalPoint;

********************************************************************************************/

BOOL Convert::StripTrailingZeros(StringBase* pString)
{
	// Get the decimal point character that we are going to search for into one of our
	// strings. This MUST be the decimal point character as this is ALWAYS called before
	// the replace decimal point character. Otherwise, we would have to search for two
	// items.
	String_8 DPChar(_R(IDS_CONVERT_DP_CHAR));
	const String_8 ZeroChar(_R(IDS_CONVERT_ZERO_CHAR));

	// if there is a decimal point in the string ...
	if (pString->Sub(DPChar)!=-1)
	{
		// skip backwards over trailing zeros
		String_8		TmpStr;
		INT32				NewStrLen = pString->Length();
		do
		{
			NewStrLen-=1;
			pString->Mid(&TmpStr,NewStrLen,1);
		} while (TmpStr==ZeroChar);

		// if this previous char is NOT a decimal point DO NOT strip it
		if (TmpStr!=DPChar)
			NewStrLen+=1;

		// remove the trailing zeros from the original string
		pString->Left(pString,NewStrLen);
	}

	return TRUE;
}


/********************************************************************************************

>	static BOOL Convert::IsCharUnitType(TCHAR Char)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	27/6/94
	Inputs:		Char - the character to check
	Outputs:	-
	Returns:	TRUE  - This could be part of a unit type specifier
				FALSE - Sorry, but it's either a white space or a number
	Purpose:	Checks to see if this char could be part of a string that specifies a unit
				We have to allow most chars so that arbitrary units can be defined like such
				as pounds, dollars or percentages
				Illegal unit specifier chars are :-
					ASCII value <= ' '
					Numericals			- '1', '2', etc
					Minus sign			- '-'
					Decimal point		- '.'
					Thousand separator  - ',' as in "10,000" 
	Errors:		-

********************************************************************************************/

BOOL Convert::IsCharUnitType(TCHAR Char)
{
	return (	(unsigned(Char) > unsigned(TEXT(' '))) &&
				(!StringBase::IsNumeric(Char)) &&
				(Char != MinusSign) &&
				(Char != DecimalPoint) &&
				(Char != ThousandSep)
			);
} 

/********************************************************************************************

>	static BOOL Convert::IsCharStartOfNumber(TCHAR Char)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	27/6/94
	Inputs:		Char - the character to check
	Outputs:	-
	Returns:	TRUE  - This could be the first char of a number
				FALSE - Sorry, but it's definitely not a number
	Purpose:	Checks to see if this char could be the first char in a string that specifies
				a number
				Legal number-starting chars are :-
					Numericals			- '1', '2', etc
					Minus sign			- '-'
					Decimal point		- '.'
	Errors:		-

********************************************************************************************/

BOOL Convert::IsCharStartOfNumber(TCHAR Char)
{
	return (	(StringBase::IsNumeric(Char)) ||
				(Char == MinusSign) ||
				(Char == DecimalPoint)
			);
}

/********************************************************************************************

>	static BOOL Convert::ReadUnitType(StringBase* pStr,INT32* pPos,UnitType* pUnitType)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	27/6/94
	Inputs:		pStr - Ptr to the string to get unit type
				pPos - *pPos = the pos to start scannning the string from (0 is first char)
				pUnitType - Ptr to a UnitType
	Outputs:	*pPos is the pos the scanning stopped
				*pUnitType is the units represented by the string
	Returns:	TRUE  - units have been recognised and value stored in *pUnitType is valid
						OR
						no units have been specified and *pUnitType == NOTYPE
				FALSE - What are these units eh?
	Purpose:	General purpose routine for extracting the unit type from a string
				E.g. "42cm45mm" with *pPos = 2 returns TRUE with *pUnitType == CENTIMETRES & *pPos == 4
				     "42cm45mm" with *pPos = 6 returns TRUE with *pUnitType == MILLIMETRES & *pPos == 8
	Errors:		-

********************************************************************************************/

BOOL Convert::ReadUnitType(const StringBase &Str,INT32* pPos,UnitType* pUnitType)
{
	INT32					len = Str.Length();
	const TCHAR		   *pChar = (const TCHAR *)Str;
	TCHAR				UnitCharArray[256]; 
	INT32 i;
	BOOL ok = TRUE;
	//Added for DBCS; Strip leading crap
	while (*pPos < len && iswspace(pChar[*pPos])) //while (*pPos < len && pChar[*pPos] <= ' ')
	      (*pPos)++ ;			

	i = 0;
	if (IsCharUnitType(pChar[*pPos]))
	{
		// Copy unit-specifying chars into UnitCharArray
		// NO LONGER copes with wierd unit specifiers that contain numbers too. e.g. "nu2", "b54c"
		// as of 18/5/95 (MarkN)
		while ( *pPos < len && (IsCharUnitType(pChar[*pPos])) && i<256) // || StringBase::IsNumeric(pChar[*pPos])) && 
			UnitCharArray[i++] = pChar[(*pPos)++];
	}

	UnitCharArray[i] = 0;	// Visit the string Terminator (I'm a null char; that's what I do)

	String_256 UnitString = UnitCharArray;
	
	//char changed to TCHAR for DBCS 
//	TCHAR* p = UnitString;  // for debugging purposes
	DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();

	if (!UnitString.IsEmpty())
	{
		// if the string isn't empty, then we must recognise the unit
		// otherwise we'll have to fail
		*pUnitType = pDocUnitList->FindUnitType(UnitString);
		ok = (*pUnitType != NOTYPE);
	}
	else
		*pUnitType = NOTYPE;	// Empty string means NOTYPE means use a default type from somewhere
	

	return (ok);
}


/********************************************************************************************

>	static BOOL Convert::ReadNumber(StringBase* pStr,INT32* pPos,double* pResult)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	27/6/94
	Inputs:		pStr - Ptr to the string to get value from
				pPos - *pPos = the pos to start scannning the string from (0 is first char)
				pResult  - Ptr to a double
	Outputs:	*pPos is the pos the scanning stopped
				*pResult = The number represented at the specified portion of the string
	Returns:	TRUE  - a valid number was read
				FALSE - something's a bit dodgey about that number mate
	Purpose:	General purpose routine for extracting the number from a string
				E.g. "42cm45.7mm"  with *pPos == 0 returns TRUE with *pResult and  42.0 & *pPos == 2
					 "42cm-45.7mm" with *pPos == 4 returns TRUE with *pResult and -45.7 & *pPos == 9
	Errors:		-

********************************************************************************************/

BOOL Convert::ReadNumber( const StringBase &Str,INT32* pPos,double* pResult)
{
	INT32 			len 		= Str.Length();
	const TCHAR	   *pChar 		= Str;
	BOOL 			IsNegative	= FALSE;
	double	 		Number		= 0.0;

	//Added for DBCS Strip leading crap
	while (*pPos < len && iswspace(pChar[*pPos]))  //while (*pPos < len && pChar[*pPos] <= ' ')
		(*pPos)++;
	
	// Check for a minus sign; Added for DBCS
	if( pChar[*pPos] == MinusSign )	//if ( pChar[*pPos] == MinusSign )
	{
		IsNegative = TRUE;
		(*pPos)++;
		if (*pPos >= len)	return FALSE;
	}
	
	// Get the actual number										Added for DBCS									
	while( *pPos < len && 
		( StringBase::IsNumeric(pChar[*pPos]) || 
		   pChar[*pPos] == ThousandSep ) )//|| pChar[*pPos] == ThousandSep) )
	{
		//Added for DBCS
		if (pChar[*pPos] != ThousandSep ) //if (pChar[*pPos] != ThousandSep)
		{
			Number = (Number * 10.0) + ( pChar[*pPos] - TEXT('0') );
		}
		(*pPos)++;
	}

	// Check for a decimal place; Added for DBCS 
	if ( pChar[*pPos] == DecimalPoint) //if ( pChar[*pPos] == DecimalPoint )
	{
		(*pPos)++;
		if (*pPos < len)	// If the string ends in a DP, then we return the number now
		{	
			// Otherwise, we continue reading the fractional part of the string...
			INT32 DecimalFactor = 10;
			while ( *pPos < len  &&  StringBase::IsNumeric(pChar[*pPos]) )
			{
				Number += (double)( pChar[*pPos] - TEXT('0') ) / DecimalFactor;
				DecimalFactor *= 10;
				(*pPos)++;
			}
		}
	}

	// If the number started with a minus sign, then make sure it is negative
	if (IsNegative)
		Number = -Number;

	*pResult = Number;
	return TRUE;
}

/********************************************************************************************

>	static double Convert::ConvertToNewUnits(double Value,UnitType Units,UnitType NewUnits)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/94
	Inputs:		Value	- value to convert
				OldUnits- units Value is defined in
				NewUnits- the units you want Value to be converted toTYPE
	Outputs:	-
	Returns:	The new value
	Purpose:	Used convert one value into an equivalent value in another unit base
				E.g. ConvertToNewUnits(3,CENTIMETRES,MILLIMETRES) would return 30.0
				because 3cm == 30mm				

********************************************************************************************/

double Convert::ConvertToNewUnits(double Value,UnitType OldUnits,UnitType NewUnits)
{
	ENSURE(NewUnits != NOTYPE,"Can't convert to NOTYPE units");

	DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
	Unit* pNewUnit = pDocUnitList->FindUnit(NewUnits);
	double NumMillipoints = 1.0;

	if (OldUnits != NOTYPE)
	{
		Unit* pOldUnit = pDocUnitList->FindUnit(OldUnits);
		NumMillipoints = Value * pOldUnit->GetMillipoints();
	}
	else
		NumMillipoints = Value;

	return (NumMillipoints / pNewUnit->GetMillipoints());
}

/********************************************************************************************

>	Static BOOL Convert::StringToComponents(StringBase* InputString,
											double*	pValue,	UnitType* pUnits)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	14/6/94 (changed considerably on 27/6/94 by Markn)
	Inputs:		InputString - The string to get values from
				pValue		- The value of the numerical part of the string
				pUnits		- The unit value of the unit part of the string
							  if no units are specified, *pUnits = NOTYPE
	Outputs:	-
	Returns:	TRUE	- Valid string
				FALSE	- Something wrong with the string
	Purpose:	General purpose routine for extracting the value and unit components of a string.
				E.g.1 the string "42.6in" would extract the value 42.6 and the unit type INCHES
				E.g.2 The string "42.6" would give *pValue=42.6,*pUnits=NOTYPE, and TRUE is returned.
				E.g.3 "4in5in" gives *pValue=9,*pUnits=INCHES, and returns TRUE
				E.g.4 "1in2cm" gives *pValue = 1.79,*pUnits=INCHES, and returns TRUE
					In this case it takes the first unit specified as the dominant unit (INCHES in this
					case), so the result is 1in plus 2cm converted to INCHES which is about 0.79in
	Errors:		This function can fail if the string was not a valid one. ie the
				string '12zx'. zx is not a unit that Camelot understands
				and so fails.

********************************************************************************************/

BOOL Convert::StringToComponents( const StringBase &str,double* pValue,UnitType* pUnits)
{
	TCHAR*		MyString;
	TCHAR		Char;
	INT32 		Pos 		= 0;
	INT32 		len			= str.Length();
	String_256	InputString( str );
//	BOOL 	 	IsNegative 	= FALSE;
	double	 	Result 		= 0.0;
	UnitType 	MainUnits 	= NOTYPE;
	
	*pValue = 1.0;
	*pUnits = INCHES;
	
	// Make sure that the string is in lower case as per SI units
	InputString.toLower();
	MyString = (TCHAR *)InputString;

	// I might consider adding this to the base class StringBase, BUT at the moment
	// its not really necessary ....
	//
	// JCF: fixed this to be internationally portable ie. what is alphanumeric in
	// your language?	
	//
	while (Pos < len)
	{
		if (!StringBase::IsAlphaNumeric(MyString[Pos]) &&
			MyString[Pos] != TEXT('-') &&
			MyString[Pos] != TEXT('.') &&
			MyString[Pos] != TEXT(','))
		{
			return FALSE;
		}

		Pos++;
	}

	Pos = 0;

	while (Pos < len)
	{
		double Number = 0.0;		
		UnitType Units;

		// Strip leading crap	Added for DBCS
		while (Pos < len && iswspace( MyString[Pos] ))	 //while (Pos < len && MyString[Pos] <= ' ')
		 	   Pos++;
 	
		Char = MyString[Pos];
		
		if (IsCharStartOfNumber(Char))
		{
			if (!ReadNumber(InputString,&Pos,&Number))
				return FALSE;
			if (!ReadUnitType(InputString,&Pos,&Units))
				return FALSE;
		}
		else
		{
			if (!ReadUnitType(InputString,&Pos,&Units))
				return FALSE;
			if (!ReadNumber(InputString,&Pos,&Number))
				return FALSE;
		}
		
		if (MainUnits == NOTYPE) MainUnits = Units;

		if (MainUnits != Units && MainUnits != NOTYPE)
			Number = ConvertToNewUnits(Number,Units,MainUnits);

		Result += Number;
	}

	*pUnits = MainUnits;
	*pValue = Result;

	return TRUE;
}

/********************************************************************************************

>	static double Convert::StringToDouble( StringBase* pInputString, UnitType Default, 
           			                       BOOL* ValidString )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/3/94 - modified 14/6/94
	Inputs:		pInputString - The string to convert to its millipoint value
				Default - the Unit type to be used if no other units have been
				specified in the string
	Outputs:	ValidString - The Address of a bool. This will be set to TRUE if
				the function converted the string without error, FALSE otherwise.
	Returns:	double - the number of Millipoints represented by the string 
	Purpose:	Convert the string into a floating point representation of the number of 
				millipoints. eg '1in' would be converted to 72000.0 millipoints.
				Does NOT cope with '0.25in0.75in' - this would return 18000.0 millipoints
				instead of 72000.0 millipoints.
				If you want a MILLIPOINT value, use StringToMillipoint to ensure correct
				rounding.
	Errors:		This function can fail if the string was not a valid one. ie the
				string '12zx'. zx is not a unit that StringToDouble understands
				and so fails.
	SeeAlso:	StringToMillipoint

********************************************************************************************/

double Convert::StringToDouble( const StringBase &InputString, UnitType Default, BOOL* ValidString )
{
	double 		Number = 1.0;
	UnitType 	Units;

	*ValidString = StringToComponents( InputString,&Number,&Units);

	if (Units == NOTYPE)
		Units = Default;

	if (*ValidString)
	{
		DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
		Unit* pUnit = pDocUnitList->FindUnit(Units);
		return (Number * pUnit->GetMillipoints());		// Get the number to millipoints and multiply
	}
	else
		return 72000.0;
}


/********************************************************************************************

>	static MILLIPOINT Convert::StringToMillipoints( StringBase* pInputString, UnitType Default, 
                         				            BOOL* ValidString )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/3/94 - modified 14/6/94
	Inputs:		pInputString - The string to convert to its millipoint value
				Default - the Unit type to be used if no other units have been
				specified in the string
	Outputs:	ValidString - The Address of a bool. This will be set to TRUE if
				the function converted the string without error, FALSE otherwise.
	Returns:	MILLIPOINT - An integer version of the number of Millipoints represented by 
				the string 
	Purpose:	Convert the string into a MILLIPOINT representation of the number of 
				millipoints. eg '1in' would be converted to 72000 millipoints.
				Does NOT cope with '0.25in0.75in' - this would return 18000 millipoints
				instead of 72000 millipoints.
	Errors:		This function can fail if the string was not a valid one. ie the
				string '12zx'. zx is not a unit that StringToDouble understands
				and so fails.

********************************************************************************************/

MILLIPOINT Convert::StringToMillipoints( const StringBase &InputString, UnitType Default, BOOL* ValidString )
{
	double result = StringToDouble( InputString,Default,ValidString);

	return (MILLIPOINT)(result+0.5);
}


/********************************************************************************************

>	static void Convert::NumberToString( MILLIPOINT Number, StringBase* MyString )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		Number - The number to be turned into a string
	Outputs:	MyString - the string is appended to this string
	Purpose:	Appends the string representation of the given number to the end
				of MyString

********************************************************************************************/

void Convert::NumberToString( MILLIPOINT Number, StringBase* MyString )
{
	INT32  Factor = 1;
	TCHAR ch[2]; 
	ch[1] = TEXT('\0');
	
	// Get a factor that 
	while (Factor*10 <= Number)
		Factor *= 10;

	while (Factor > 1)
	{
		// calculate the most significant digit and remove it from the number
		INT32 Digit = INT32(Number / Factor);
		ch[0] = TEXT('0') + Digit;
		*MyString += String_8(ch);
		
		// Add the digit into the string
		Number = Number - (Digit*Factor);
		Factor /= 10;
	}

	// add in the last character
	ch[0] = TEXT('0') + (INT32)Number;
	*MyString += String_8(ch);
	
}




/********************************************************************************************

>	static BOOL Convert::MillipointsToString( double MpValue, UnitType TheUnit,
                        				      StringBase* OutputString, INT32 DecimalPlaces =  -1)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> (rewritten by MarkN to use new units system - 6/7/94)
	Created:	1/9/93
	Inputs:		MpValue - The number of millipoints to be converted
				TheUnit - The units that you need the result in
				OutputString - Stirng to put value into (See outputs)
				DecimalPlaces - The number of Decimal places required (0-3). This defaults to
				-1 which means use the default preference value. 
	Outputs:	OutputString - pointer to a string that is big enough to hold
				the resulting number plus its type (ie 12.45cm)
	Returns:	TRUE if all went well and the string is valid, FALSE otherwise
	Purpose:	Convert a number in MpValue to a string representation of that
				number in the given units. ie converting 72000 millipoints to inches
				would give the string '1in'.
				If DecimalPlaces = -1 then use the default preference value which the user
				has specified.
	See Also:	DocUnitList

********************************************************************************************/

BOOL Convert::MillipointsToString( double MpValue, UnitType TheUnit, StringBase* OutputString,
								   INT32 DecimalPlaces)
{
	ERROR2IF(OutputString == NULL,FALSE,"MillipointsToString NULL output string supplied");
	// Decimal places will be -1 if the default number is to be used.
	
	double Value = 1.0;
	String_32 Specifier;
	TCHAR p[256], format[256], dpformat[256];
	
	DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
	DocUnitList	FactoryDefUnits;
	if(pDocUnitList==NULL)
	{
		// certain classes (eg OpDescriptors) may want to call this function to set
		// values in controls after they have been read in by the bars/dialogs etc...
		// However at that point (Document::GetCurrent() == NULL) so you can't get
		// a DocUnitList from the current doc. Hence create our own default units
		FactoryDefUnits.Init();
		FactoryDefUnits.MakeDefaultUnits();
		pDocUnitList = &FactoryDefUnits;
	}

	Unit* pUnit = pDocUnitList->FindUnit(TheUnit);

	// Find out how many Units MpValue represents
	Value = MpValue / pUnit->GetMillipoints();

	// Get the specifier of the units we're in
	Specifier = pUnit->GetSpecifier();

	// check if we want the default number of decimal places
	if (DecimalPlaces == -1) DecimalPlaces = NumDecimalPlaces;

	//_stprintf(dpformat,"%%.%df",DecimalPlaces);	// Makes "%.xf" where x is num dp
	// Makes the output form "%.xf" where x is num dp.
	// %g instead of %f gives leading zero supression but then requires number significant
	// figures rather than number of decimal places.
	camSnprintf( dpformat, 256, TEXT("%%.%df"), DecimalPlaces) ;

	// Create the string to stick in an editable field (or wherever you like)
	if (pUnit->IsPrefix())
	{
		// Prefix units so show units followed by value
		camSnprintf( format, 256, _T("%%s%s"), (LPCTSTR) dpformat );				// ie. %s %.3f
		camSnprintf( p, 256, format, (LPCTSTR)Specifier, (double)Value );			// uses FP
		*OutputString = p;
		// Look for all zeros after decimal point and if so then remove
		//StripTrailingZeros(OutputString);
	}
	else
	{
		// Suffix units so show value followed by units
		camSnprintf( p, 256, dpformat, (double)Value );						// uses FP
		*OutputString = p;

		// Look for all zeros after decimal point and if so then remove
		StripTrailingZeros(OutputString);

		// Finally, add in the unit specifier
		*OutputString += Specifier;
	}

	
	// Extra bodge required so that if the user has specified display values using
	// a non full stop to show decimal points then we must replace the point that
	// is present by the specified character.
	if (DecimalPoint != '.')
	{
		ReplaceDecimalPoint(OutputString);
	}
	
	return TRUE;
}


/********************************************************************************************

>	static BOOL Convert::LongToString( INT32 Number, StringBase* OutputString )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/93
	Inputs:		Number - The number you want turning into a string
	Outputs:	OutputString - The string that gets created
	Returns:	TRUE if the conversion worked, false otherwise
	Purpose:	Converts a INT32 to a its string equivalent.
	SeeAlso:	StringToLong

********************************************************************************************/

BOOL Convert::LongToString( INT32 Number, StringBase* OutputString )
{
	OutputString -> Empty();

	if (Number < 0)
	{
		TCHAR	buff[2] ;
		buff[0] = TEXT('-');
		buff[1] = TEXT('\0');

		*OutputString += String_8(buff);
		Number = -Number;
		
	}
	
	NumberToString( Number, OutputString );
	
	return TRUE;
}

/********************************************************************************************

>	static BOOL Convert::DoubleToString( double Number, StringBase* OutputString, INT32 DecimalPlaces )

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/94
	Inputs:		Number - The number you want turning into a string
				DecimalPlaces - The number of Decimal places required (0-3). This defaults to
				-1 which means use the default preference value. Don't override it unless
				absolutely necessary.

	Outputs:	OutputString - The string that gets created
	Returns:	TRUE if the conversion worked, false otherwise
	Purpose:	Converts a double to a its string equivalent.
	SeeAlso:	StringToDouble

This routine is a bit of a bodge

********************************************************************************************/

BOOL Convert::DoubleToString( double Number, StringBase* OutputString, INT32 DecimalPlaces /* =-1 */ )
{
	// Generate the output string in a simple char array
	TCHAR dpformat[32]; 
	TCHAR TempString[32];

	// Use the default/preference or desired number of decimal places
	if (DecimalPlaces < 0)
		DecimalPlaces = (INT32) NumDecimalPlaces;		// Use the preference value

	camSnprintf( dpformat, 32, _T("%%.%df"), (INT32) DecimalPlaces);		// make %.xf formatting string
	camSnprintf( TempString, 32, dpformat, (double)Number );					// uses FP

	*OutputString = TempString;		// Copy the resulting string into the String
	
	// Look for all zeros after decimal point and if so then remove
	StripTrailingZeros(OutputString);

	// Extra bodge required so that if the user has specified display values using
	// a non full stop to show decimal points then we must replace the point that
	// is present by the specified character.
	if (DecimalPoint != '.')
	{
		ReplaceDecimalPoint(OutputString);
	}
	return TRUE;
}


/********************************************************************************************

>	static BOOL Convert::StringToLong( StringBase* InputString, INT32* Number )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/93
	Inputs:		InputString - The string that you want turning into a number
	Outputs:	Number - The number you want turning into a string
	Returns:	TRUE if the conversion worked, false otherwise
	Purpose:	Converts a string into its INT32 equivalent
	SeeAlso:	LongToString

********************************************************************************************/

BOOL Convert::StringToLong( const StringBase &InputString, INT32* pNumber)
{
	// Get a pointer version of the string
	const TCHAR* pMyString = (const TCHAR*)InputString;
	INT32 nLength = InputString.Length();
	INT32 nPos = 0;
	
	// Check for a minus sign - added for DBCS
	BOOL fIsNegative = FALSE;
	if (pMyString[nPos] == TEXT('-') )   // if (pMyString[Pos] == TEXT('-'))
	{
		fIsNegative = TRUE;
		nPos++;
		if (nPos >= nLength) return 0;
	}
			
	// Get the actual number
	INT32 nTotal	 = 0;
	while (nPos < nLength && StringBase::IsNumeric(pMyString[nPos]))
	{
		nTotal = (nTotal * 10) + pMyString[nPos] - TEXT('0');
		nPos++;
	}
	
	// If we have not read in the whole string, then it was not valid
	if (nPos < nLength && !StringBase::IsSpace(pMyString[nPos])) return FALSE;	

	// If the number started with a minus sign, then make sure it is negative
	if (fIsNegative) nTotal = -nTotal;

	*pNumber = nTotal;
	return TRUE;
}

/********************************************************************************************

>	static BOOL Convert::StringToDouble( StringBase* InputString, double * Number )

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/94
	Inputs:		InputString - The string that you want turning into a double
	Outputs:	Number - The number you want turning into a string
	Returns:	TRUE if the conversion worked, false otherwise
	Purpose:	Converts a string into its double equivalent
	SeeAlso:	DoubleToString

				Used to be a bodge as atof() doesn't work in unicode but converted to use
				the other functions which should do it properly.

********************************************************************************************/

BOOL Convert::StringToDouble( const StringBase &InputString, double * Number )
{
//#if UNICODE
//#error "I'm screwed! atof() doesn't work for UNICODE"
//#else
//	*Number = atof((char *)*InputString);
//	return(TRUE);
//#endif

	// Cannot use atof as does not cope with a non-decimal point character which
	// the user may be specifying so use our form of atof.
	// atof will also allow say 26.98mm where we should really be erroring.
	// ReadNumber ignores everything after a dodgy decimal point which is not correct. 
	// StringToComponents will include units as well. We do not want this part so ignore it.
	UnitType Units;
	BOOL ok = StringToComponents(InputString,Number,&Units);

	// Check if units are NOTYPE and if not then return failure.
	if (Units != NOTYPE)
		return FALSE;

	return ok;
}


#define MIN_MILLIPOINT_VAL (-2147483648.0)	// Min signed 32 bit val - 0x80000000
#define MAX_MILLIPOINT_VAL ( 2147483647.0)	// Max signed 32 bit val - 0x7fffffff

/********************************************************************************************

> 	static BOOL Convert::ConvertDoubleToMillipoint(double Value,MILLIPOINT* pResult)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/94
	Inputs:		Value = the fp value to convert
	Outputs:	pResult = the MILLIPOINT var to place the result in
	Returns:	TRUE if the conversion was OK
				FALSE if the conversion caused an overflow error
	Purpose:	Converts a double to a MILLIPOINT, checking first whether such a conversion
				is possible.
				It returns FALSE if Value is beyond the limits of a signed 32 bit integer.
	SeeAlso:	ConvertMillipointToDouble

********************************************************************************************/

BOOL Convert::ConvertDoubleToMillipoint(double Value,MILLIPOINT* pResult)
{
	Value += 0.5;	// Round up the value

	if ((Value < MIN_MILLIPOINT_VAL) || (Value > MAX_MILLIPOINT_VAL))
		return FALSE;

	*pResult = (MILLIPOINT)Value;
	return TRUE;
}

/********************************************************************************************

> 	static void Convert::ConvertMillipointToDouble(MILLIPOINT Value,double* pResult)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/94
	Inputs:		Value = the MILLIPOINT value to convert
	Outputs:	pResult = the double var to place the result in
	Returns:	-
	Purpose:	Converts a MILLIPOINT to a double.
				At the moment, this function just does a simple cast.
				I've only put it in because I did the sister function ConvertDoubleToMillipoint
	SeeAlso:	ConvertDoubleToMillipoint

********************************************************************************************/

void Convert::ConvertMillipointToDouble(MILLIPOINT Value,double* pResult)
{
	*pResult = (double)Value;
}


/******************************************************************************************

>	static UINT32 Convert::StringToBytes(String_256 *pStr, BOOL *Valid)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		pStr -  pointer to all manner of memory amount specifying strings
					    such as "1024k", "1024 bytes", "1024", "1048576", "1Mb".
					    (NB - The above will all return 1024 (and Valid == TRUE)).
	Outputs:	Valid - used to flag a valid result. Invalid results will return 0 and
						flag this variable to FALSE. Any non-numerically starting
						strings will cause this invalid state. Leading spaces shouldn't
						pose a problem... 
	Returns:	Number of bytes described by input string, so "1k" would produce 1024...
	Purpose:	Takes a string with a textual description of an amount of memory, and
				returns the amount of bytes described by the string.
	Errors:		At present we only use simple string matching, so "100 Tera-bytes" will
				return 100, as Tera is not supported and the bytes section is matched.
				Also "200 trams" will return 200*1024*1024, as the 'm' of trams is matched
				as flagging a mega-byte.
	SeeAlso:	-

******************************************************************************************/

UINT32 Convert::StringToBytes( const StringBase &Str, BOOL *Valid)
{
	// Set up bad return value until we are fairly happy with the string 
	*Valid = FALSE;
	
	//changed char to TCHAR for DBCS
	const TCHAR		   *CharStr = (const TCHAR *)Str;	

	double 	Number 	= 0.0;
	INT32 	Pos		= 0;
	UINT32	ret		= 0;
	// Try and read the value in using are routine
	// Cannot use atof as does not cope with non-standard decimal point and thousand
	// separator characters.
//	UINT32 ret=_ttoi(CharStr);								// Rip the numbers from the start
	BOOL ok = ReadNumber(Str, &Pos, &Number);
	if (!ok) return 0;										// bad number so return invalid 

	// If negative then return invalid and zero.
	if (Number < 0) return 0;
	// If bigger than what can be held in a UINT32 then return the maximum and FALSE. 
	if (Number > (double)UINT32_MAX) return UINT32_MAX;
	 
	// We have what looks like a fairly valid number so set return flag True;
	*Valid = TRUE;

	// If value is zero then return with no more checking as not relevent
	if (Number == 0) return (0);

	if (    cc_lstrstr(CharStr, (TCHAR *)String_16(_R(IDS_CONVERT_SMALL_K)) /*"k"*/)
		 || cc_lstrstr(CharStr, (TCHAR *)String_16(_R(IDS_CONVERT_LARGE_K)) /*"K"*/) )
	{
		// Check if K or K is in the string and if so multiply by 1024 
		Number = Number*1024;				// Kilobyte value
	}
	else if (   cc_lstrstr(CharStr, (TCHAR *)String_16(_R(IDS_CONVERT_SMALL_M)) /*"m"*/)
			 || cc_lstrstr(CharStr, (TCHAR *)String_16(_R(IDS_CONVERT_LARGE_M)) /*"M"*/) )
	{
		// Check if M or m is in the string and if so multiply by 1024 by 1024 
		Number = Number*1024*1024;			// Megabyte value
	}
	else if (   cc_lstrstr(CharStr, (TCHAR *)String_16(_R(IDS_CONVERT_SMALL_G)) /*"g"*/)
			 || cc_lstrstr(CharStr, (TCHAR *)String_16(_R(IDS_CONVERT_LARGE_G)) /*"G"*/) )
	{
		// Check if G or g is in the string and if so multiply by 1024 by 1024 by 1024 
		Number = Number*1024*1024*1024;		// Gigabyte value
	}
  	else if (   cc_lstrstr(CharStr, (TCHAR *)String_16(_R(IDS_CONVERT_SMALL_BYTE)) /*"byte"*/)
  			 || cc_lstrstr(CharStr, (TCHAR *)String_16(_R(IDS_CONVERT_MEDIUM_BYTE)) /*"Byte"*/)
  			 || cc_lstrstr(CharStr, (TCHAR *)String_16(_R(IDS_CONVERT_LARGE_BYTE)) /*"BYTE"*/) )
  	{
  		// Check if byte or variation of specified in which case just return the value 
    	// Number = Number;					// straight byte value
	}
	// if none of the above then assume value is in bytes and hence do nothing

	// If bigger than what can be held in a UINT32 then return the maximum and FALSE. 
	if (Number > (double)UINT32_MAX) return UINT32_MAX;

	// We just want the interger part of the number specified as fractional parts are non-valid
	ret = (UINT32)Number;
TRACEUSER( "Neville", _T("%s -> %d\n"), (char *)CharStr, ret );		
  		 
	return ret;		
}
		  
/*******************************************************************************************
>	static void Convert::BytesToString(StringBase* String, UINT32 Bytes);	
	
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26th August 1994
	Inputs:		String - The string to store the formatted memory size in.  The
						 existing contents of the string are overwritten.
				Bytes - The number of bytes which to format and store.  
	Returns:	-
	Purpose:	Converts the Bytes number into a string of the following format:
					Bytes = 0			"0 bytes"
					Bytes = 1			"1 byte"
					2 <= Bytes < 4096	"<n> bytes"
					4096 <= Bytes < 4M	"<n/1024> K"
					4M <= Bytes			"<n/1Meg> M"
*******************************************************************************************/

BOOL Convert::BytesToString(StringBase* pString, UINT32 Bytes)
{
	ERROR2IF(pString==NULL, FALSE, "NULL output pointer");
	ERROR3IF(Bytes<0, "The number of bytes must be positive");
	if(Bytes<0) Bytes = 0-Bytes;
	pString->Empty();
	
	INT32 Number = 0;
	UINT32 Resource = 0;	
	String_16 Working;
	String_8 ThousandsSepStr;
	ThousandsSepStr += (TCHAR)ThousandSep;


	// Work out which resource string to use depending on the number of bytes
	if (Bytes == 1)
	{
		Number = 1;
		Resource = _R(IDS_MEMORYFORMAT_BYTE);
	}
	else
	{
		if (Bytes < 4096)
		{
			Number = Bytes;
			Resource = _R(IDS_MEMORYFORMAT_BYTES);
		}
		else
		{
			if (Bytes < (4096*1024))
			{
				Number = Bytes/1024;
				Resource = _R(IDS_MEMORYFORMAT_KILO);
			}
			else
			{
				Number = Bytes/(1024*1024);
				Resource = _R(IDS_MEMORYFORMAT_MEGA);
			}
		}
	}

	// Put the number into a string and add thousand seperator
	Working._MakeMsg( wxT("#1%lu"), Number ); 
	if (Working.Length() > 3)
		Working.Insert(ThousandsSepStr, Working.Length()-3);
	
	// put the number string into the string resource	
	pString->MakeMsg(Resource, &Working);

	return TRUE;
}



//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------


/********************************************************************************************

>	static BOOL DimScale::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successfully initialised
	Purpose:	Initialises this class. Must be called before any other function
				in this class
	SeeAlso:	-

********************************************************************************************/

BOOL DimScale::Init()
{
	BOOL ok;

	pDefaultDimScale = new DimScale;
	ok = (pDefaultDimScale != NULL);
	if (ok)	pDefaultDimScale->SetActiveState(FALSE);

	return ok;
}

/********************************************************************************************

>	static void DimScale::Deinit()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialises this class. Must be called before any other function
				in this class
	SeeAlso:	-

********************************************************************************************/

void DimScale::Deinit()
{
	if (pDefaultDimScale != NULL)
	{
		delete pDefaultDimScale;
		pDefaultDimScale = NULL;
	}
}

/********************************************************************************************

>	DimScale::DimScale()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor
	SeeAlso:	-

********************************************************************************************/

DimScale::DimScale()
{
	// Set up some useful defaults
	Active 			= FALSE;
	DrawingScaleStr = _R(IDS_CONVERT_ONE_CM); //"1 cm";
	RealScaleStr	= _R(IDS_CONVERT_ONE_MI); //"1 mi";
	ScaleFactor		= MI_MP_VAL / CM_MP_VAL;
}

/********************************************************************************************
>	UnitType DimScale::GetUnits()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/95
	Returns:	Scale units if scaling active else page units (NOTYPE for error)
	Purpose:	For getting hold of those pesky units
********************************************************************************************/

UnitType DimScale::GetUnits()
{
	UnitType Units=GetScaleUnits();
	if (Units==NOTYPE)
	{
		DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
		ERROR2IF(pDocUnitList==NULL,NOTYPE,"DimScale::GetUnits() - pDocUnitList==NULL");
		Units = pDocUnitList->GetPageUnits();
	}

	return Units;
}


/********************************************************************************************
>	UnitType DimScale::GetScaleUnits()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/95
	Returns:	Scale units if active else NOTYPE (NOTYPE for error)
	Purpose:	For getting hold of those pesky scale units
********************************************************************************************/

UnitType DimScale::GetScaleUnits()
{
	if (IsActive()==FALSE)
		return NOTYPE;

	DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
	ERROR2IF(pDocUnitList==NULL,NOTYPE,"DimScale::GetScaleUnits() - pDocUnitList==NULL");

	UnitType ScaleUnits = pDocUnitList->GetScaleUnits();
	if (ScaleUnits==AUTOMATIC)
	{
		double dummy = 1.0;
		BOOL ok = Convert::StringToComponents( RealScaleStr,&dummy,&ScaleUnits);
		if (!ok || ScaleUnits==NOTYPE)
			ScaleUnits = pDocUnitList->GetPageUnits();
	}

	return ScaleUnits;
}


/********************************************************************************************

>	void DimScale::SetActiveState(BOOL State)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/94
	Inputs:		State = Boolean value of activity
	Outputs:	-
	Returns:	-
	Purpose:	Used to set the active state of this dimension scaling object.
				When inactive, no scaling is performed
	SeeAlso:	-

********************************************************************************************/

void DimScale::SetActiveState(BOOL State)
{
	Active = State;
}

/********************************************************************************************

>	BOOL DimScale::IsActive()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/94
	Inputs:		-
	Outputs:	-
	Returns:	The active state of this object
	Purpose:	Interogate the active state of this object
	SeeAlso:	-

********************************************************************************************/

BOOL DimScale::IsActive()
{
	return (Active);
}

/**************************************************************************************
>	String_32 DimScale::GetDrawingScaleStr()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		-
	Returns:	The string that represents the document scale
	Purpose:	This is the string that the user has typed into the "Drawing scale"
				editable field of the scale factor dialog

***************************************************************************************/

String_32 DimScale::GetDrawingScaleStr()
{
	return DrawingScaleStr;
}


/**************************************************************************************
>	void DimScale::SetDrawingScaleStr(String_32& NewStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		Reference to the new drawing scale string
	Returns:	TRUE if string represents a good value, FALSE otherwise.
	Purpose:	Allows the setting of a new string that is to be used as the drawing
				scaling	factor. This has usually been typed in by the user into the
				"Drawing scale"	editable field of the scale factor dialog. The string
				is attempted to be converted to a value and the value checked. If it
				is ok then TRUE is returned otherwise FALSE is returned.

				This ***MUST *** not ERROR2 or set an error on bad values as the scale
				options tab code tries to produce a nice error for the user when they
				have entered incorrect values rather than completely incomprehensible
				internal errors.
	SeeAlso:	ScaleTab::CommitSection; Document::ImportViewInfo;

***************************************************************************************/

BOOL DimScale::SetDrawingScaleStr(String_32& NewStr)
{
	double Scale = 1.0;
	BOOL ok	= TRUE;
	//changed char to TCHAR for DBCS
//	TCHAR* p = DrawingScaleStr;		// for debugging purposes

	ok = ConvertToDouble( NewStr,&Scale);
	if (!ok)
		return FALSE;			// let the caller decide what error to show!  
	//ERROR2IF(!ok,FALSE,"DrawingScaleStr is bad!");

	// Check that the converted value is a real positive value and certainly not zero!
	if (Scale <= 0)
		return FALSE;

	// Only if everything went ok will we set the DrawingScaleStr to be the newly
	// specified value.
	if (ok)
		DrawingScaleStr = NewStr;

	return TRUE;
}


/**************************************************************************************
>	String_32 DimScale::GetRealScaleStr()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		-
	Returns:	The string that represents the document scale
	Purpose:	This is the string that the user has typed into the "Real scale" editable
				field of the scale factor dialog

***************************************************************************************/

String_32 DimScale::GetRealScaleStr()
{
	return RealScaleStr;
}


/**************************************************************************************
>	void DimScale::SetRealScaleStr(String_32& NewStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		Reference to the new Real scale string
	Returns:	TRUE if string represents a good value, FALSE otherwise.
	Purpose:	Allows the setting of a new string that is to be used as the real
				scaling	factor. This has usually been typed in by the user into the
				"Real scale" editable field of the scale factor dialog. The string
				is attempted to be converted to a value and the value checked. If it
				is ok then TRUE is ereturned otherwise FALSE is returned.  

				This ***MUST *** not ERROR2 or set an error on bad values as the scale
				options tab code tries to produce a nice error for the user when they
				have entered incorrect values rather than completely incomprehensible
				internal errors.
	SeeAlso:	ScaleTab::CommitSection; Document::ImportViewInfo;

***************************************************************************************/

BOOL DimScale::SetRealScaleStr(String_32& NewStr)
{
	double Scale = 1.0;
	BOOL ok;
	//From char to TCHAR for DBCS
//	TCHAR* p = RealScaleStr;		// for debugging purposes

	ok = ConvertToDouble( NewStr,&Scale);
	if (!ok)
		return FALSE;			// let the caller decide what error to show!  
	//ERROR2IF(!ok,FALSE,"RealScaleStr is bad!"); VERY BAD!!!!

	// Check that the converted value is a real positive value and certainly not zero!
	if (Scale <= 0)
		return FALSE;

	// Only if everything went ok will we set the DrawingScaleStr to be the newly
	// specified value.
	if (ok)
		RealScaleStr = NewStr;

	return TRUE;
}

/**************************************************************************************
>	BOOL DimScale::SetScaleFactor()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		-
	Returns:	TRUE if successfully converted and a new scaling factor set, FALSE otherwise.
	Purpose:	Reads the DrawingScaleStr and RealScaleStr, which should specify the drawing
				and real scaling factors, and calculates the new scale factor from these.
				If either are negative or zero then it will return FALSE and no new scaling
				factor is set. 

				This ***MUST *** not ERROR2 or set an error on bad values as the scale
				options tab code tries to produce a nice error for the user when they
				have entered incorrect values rather than completely incomprehensible
				internal errors.
	SeeAlso:	ScaleTab::CommitSection; Document::ImportViewInfo;

***************************************************************************************/

BOOL DimScale::SetScaleFactor()
{
	double	RealScale		= 1.0;
	double	DrawingScale	= 1.0;
	BOOL	ok				= TRUE;
	//char to TCHAR for DBCS
//	TCHAR* pD = DrawingScaleStr;	// for debugging purposes
//	TCHAR* pR = RealScaleStr;		// for debugging purposes

	RealScale = Convert::StringToDouble( RealScaleStr,GetUnits(),&ok);
	if (!ok)
		return FALSE;			// let the caller decide what error to show!  
	//ERROR2IF(!ok,FALSE,"RealScaleStr is bad!");

	// Check that the converted value is a real positive value and certainly not zero!
	if (RealScale <= 0)
		return FALSE;

	DrawingScale = Convert::StringToDouble( DrawingScaleStr,GetUnits(),&ok);
	if (!ok)
		return FALSE;			// let the caller decide what error to show!  
	//ERROR2IF(!ok,FALSE,"DrawingScaleStr is bad!");

	// Check that we are not going to get a division by zero error
	if (DrawingScale != 0)
		ScaleFactor = RealScale / DrawingScale;
	else
		return FALSE;

	return TRUE;
}
 
/********************************************************************************************
>	BOOL DimScale::ConvertToUnits(double MpValue, String_256* pStr, BOOL UnitSpecifier=TRUE,
								  INT32 dp=-1, UnitType units=NOTYPE, BOOL bSquare = FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/94
	Inputs:		MpValue = A value in Camelot's internal millipoints
				pStr	= Ptr to a string to store the converted MpValue
				UnitSpecifier - If TRUE the unit specifier is included
								e.g. "1.27 cm" or "1.27" for TRUE or FALSE respectively
				dp    - number of decimal places (-1 means use system default)
												 (100+n means use n significant figures)
				units - type of units to convert to
				bSquare - MpValue measures an area
	Returns:	TRUE if successfully converted, FALSE otherwise
	Purpose:	This converts a millipoint value into a string representation that reflects
				the dimension scaling specified in this object.
				If this object represented a scaling of 1 mile = 1 inch, then if MpValue = 72000
				pStr would contain "1 mi" on exit.
	Errors:		dp<0
********************************************************************************************/

BOOL DimScale::ConvertToUnits(double MpValue, String_256* pStr, BOOL UnitSpecifier,
							  INT32 dp, UnitType units, BOOL bSquare)
{
	ERROR2IF(pStr == NULL, FALSE,"DimScale::ConvertToUnits() result string = Null");

	// If dp is -1 then use the default preference value specified by the user.
	if (dp == -1)
		dp = Convert::GetNumberDecimalPlaces(); // Use the default preference value
	else if (dp == 100)
		dp = 100 + Convert::GetNumberDecimalPlaces(); // Use the default preference value

	// If dp is anything else i.e. negative then make zero but warn debug users as bad value.
	if (dp < 0)
	{
		ERROR3("DimScale::ConvertToUnits() - dp<0");
		dp = 0; // Use no decimal places
	}

	double ScaledValue	= 1.0;
	double sf			= 1.0;
	String_32 Specifier;
	TCHAR p[256];
	
	// Get the current unit list from the document and the the current scale units
	DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
//	ERROR2IF(pDocUnitList == NULL, FALSE,"DimScale::ConvertToUnits() No current document units");
	if (pDocUnitList == NULL)
	{
		TRACE( wxT("No Unit list - if this not an OLE paste, PANIC!!!\n") );
		return FALSE;
	}

	if (units==NOTYPE)
		units = GetUnits();
	Unit* pScaleUnit = pDocUnitList->FindUnit(units);
	ERROR2IF(pScaleUnit == NULL, FALSE,"DimScale::ConvertToUnits() Null scale units");

	// If active, use the scale factor, otherwise use 1.0 (i.e. no scaling)
	if (Active)
	{
		sf = ScaleFactor;
		if (bSquare)
			sf *= ScaleFactor;
	}
	else
		sf = 1.0;

	// Find out how many Units MpValue represents, avoiding div by zero's
	double NumMpInUnit = pScaleUnit->GetMillipoints();
	if (NumMpInUnit != 0)
	{
		ScaledValue = MpValue*sf / NumMpInUnit;
		if (bSquare)
			ScaledValue = ScaledValue / NumMpInUnit;
	}
	else
		ScaledValue = 0;

	/// If the unit type is <= millimeters, clamp decimal-places to 1. [Harrison Ainsworth 19/11/97]
	///  Why? - hide the accuracy limits of my scaling fix (OpScale2Trans)
	//
	// [Phil 26/09/2005]
	// This kludge is retained only for back-compatibility with X1 - OpScale2Trans has
	// been removed
	if( ( NumMpInUnit <= ((double)MM_MP_VAL + (double)MP_MP_VAL))  &&  ( dp > 1 ) )
		dp  =  1;

	// If the value to 2dp is 1.00, use the singular unit name
	// (Only applicable when ExpandUnitName is TRUE)
//	Plural = (abs((INT32)((ScaledValue+0.005)*100.0)) != 100);

	// Get the specifier of the units we're in
	Specifier = pScaleUnit->GetSpecifier();

	// Create the string to stick in an editable field (or wherever you like)
	if (UnitSpecifier)
	{
		if (dp<=100)
		{
			if (pScaleUnit->IsPrefix())
			{
				// Prefix units required so print them first followed by the number
				camSnprintf( p, 256, _T("%s%.*f"), (LPCTSTR) Specifier, (INT32) dp, (double)ScaledValue );		// uses * & FP

				// Look for all zeros after decimal point and if so then remove
				*pStr = p;
				Convert::StripTrailingZeros(pStr);
			}
			else
			{
				// Suffix units required so print number followed by units
				// Used to do this in one go but makes life more difficult on the trailing
				// zero supression front.
				//_stprintf(p,"%.*f%s",dp,ScaledValue,(char*)Specifier);
				camSnprintf( p, 256, _T("%.*f") , (INT32) dp, (double) ScaledValue );	// uses * & FP

				// Look for all zeros after decimal point and if so then remove
				*pStr = p;
				Convert::StripTrailingZeros(pStr);

				// And finally add the units in
				*pStr += Specifier;
			}
		}
		else
		{
			if (pScaleUnit->IsPrefix())
			{
				// Prefix units required so print them first followed by the number
				camSnprintf( p, 256, _T("%s%.*g"), (LPCTSTR) Specifier, (INT32) dp-100, (double) ScaledValue);

				// Look for all zeros after decimal point and if so then remove
				*pStr = p;
			}
			else
			{
				// Suffix units required so print number followed by units
				camSnprintf( p, 256, _T("%.*g%s"), (INT32) dp-100, (double)ScaledValue, (LPCTSTR)Specifier );

				// Look for all zeros after decimal point and if so then remove
				*pStr = p;
			}
		}
	}
	else
	{
		if (dp<=100)
		{
			// No units so just output the value
			camSnprintf( p, 256, wxT("%.*f"), (INT32) dp, (double) ScaledValue);								// uses * & FP

			// Look for all zeros after decimal point and if so then remove
			*pStr = p;
			Convert::StripTrailingZeros(pStr);
		}
		else
		{
			// No units so just output the value
			camSnprintf( p, 256, wxT("%.*g"), (INT32) dp-100, (double)ScaledValue );

			// Look for all zeros after decimal point and if so then remove
			*pStr = p;
		}
	}

	//*pStr = p;

	// Extra bodge required so that if the user has specified display values using
	// a non full stop to show decimal points then we must replace the point that
	// is present by the specified character.
	if (Convert::GetDecimalPointChar() != '.')
	{
		Convert::ReplaceDecimalPoint(pStr);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL DimScale::ConvertToDouble(StringBase* pStr,double* pResult)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		pStr	= Ptr to a string containing value to convert
	Outputs:	pResult = Ptr to the resultant millipoint value in fp form.
	Returns:	TRUE if successfully converted, FALSE otherwise
	Purpose:	This takes a string of the form "45.3 km" and converts it to the correctly
				scaled millipoint value. E.g if 1 km = 2 cm then "45.3 km" would give
				(45.3 * 2 * 28346.52715) millipoints, where 28346.52715 is the num millipoints 
				in one centimetre.
	SeeAlso:	-

********************************************************************************************/

BOOL DimScale::ConvertToDouble(const StringBase &str,double* pResult)
{
	double Result=72000.0;	// Provide something useful in case BOOL result is ignored
	BOOL ok;

	// Convert the string into its component parts
	// Use scale units if units are NOT specified in the string
	Result = Convert::StringToDouble(str,GetUnits(),&ok);

	if (ok)
	{
		// Only scale down if this object is active
		if (Active)
        	Result /= ScaleFactor;		// Scale result to internal millipoints
	}

	*pResult = Result;					// Store the result

	return (ok);
}

/********************************************************************************************

>	BOOL DimScale::ConvertToMillipoints(const StringBase& pStr, MILLIPOINT* pMpValue)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/94
	Inputs:		pStr	= Ptr to a string containing value to convert
	Outputs:	pMpValue= Ptr to the resultant millipoint value.
	Returns:	TRUE if successfully converted, FALSE otherwise
	Purpose:	This takes a string of the form "45.3 km" and converts it to the correctly
				scaled millipoint value. E.g if 1 km = 2 cm then "45.3 km" would give
				(45.3 * 2 * 28346.52715) millipoints, where 28346.52715 is the num millipoints 
				in one centimetre.
	Errors:		ConvertToDouble() fails
				result overflows

********************************************************************************************/

BOOL DimScale::ConvertToMillipoints(const StringBase& Str, MILLIPOINT* pMpValue)
{
	// Set a sensible return value just in case we exit early and nobody notices
	// the boolean result
	*pMpValue = 72000;

	double result = 1.0;
	if (ConvertToDouble( Str, &result) == FALSE)
		return FALSE;

	result+=0.5;	// round up

	// We are going to try and convert this to a MILLIPOINT value so check that
	// we are not going to overflow. If we are then return FALSE and the maximum
	// possible value. 
	//if (result>MaxDocCoord || result<MinDocCoord)
	//	ERROR2(FALSE,"DimScale::ConvertToMillipoints() - overflow");
	// Rather than erroring do something sensible like returning false.
	if (result > MaxDocCoord)
	{
		*pMpValue = MaxDocCoord;
		return FALSE;
	}	

	if (result < MinDocCoord)
	{
		*pMpValue = MinDocCoord;
		return FALSE;
	}	
		
	// We should be fairly happy with that result so return it to the user
	*pMpValue=(MILLIPOINT)result;

	return TRUE;
}

/**************************************************************************************
>	DimScale& DimScale::operator=(const DimScale& other)


	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		A reference to a constant DimScale.
	Returns:	A reference to this.
	Purpose:	Assignment (set equal to) operator

***************************************************************************************/

DimScale& DimScale::operator=(const DimScale& other)
{
	Active 			= other.Active;
	ScaleFactor 	= other.ScaleFactor;
	DrawingScaleStr = other.DrawingScaleStr;
	RealScaleStr 	= other.RealScaleStr;

	return *this;
}

/**************************************************************************************
>	BOOL DimScale::operator==(const DimScale& other)


	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		A DimScale.
	Returns:	TRUE if they are identical in every way
	Purpose:	Equivalence operator

***************************************************************************************/

BOOL DimScale::operator==(const DimScale& other)
{
return (
			Active 			== other.Active &&
			ScaleFactor 	== other.ScaleFactor &&
			DrawingScaleStr == other.DrawingScaleStr &&
			RealScaleStr 	== other.RealScaleStr
  		);
}


	
/**************************************************************************************
>	static DimScale* DimScale::GetPtrDimScale(Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		A ptr to a node in the tree
	Returns:	Ptr to a DimScale
	Purpose:	Ptr to a DimScale object that can be used to scale dimension 
				values to and from internal millipoint values.
				The DimScale object is taken from the node's parent Spread.
				If a parent spread is not found or the spread's DimScale object is 
				inactive, the default DimScale is returned.

***************************************************************************************/

DimScale* DimScale::GetPtrDimScale(Node* pNode)
{
	Spread*	  pSpread;
	DimScale* pDimScale;

	if (!pNode->IsKindOf(CC_RUNTIME_CLASS(Spread)))
		pSpread = pNode->FindParentSpread();
	else
		pSpread = (Spread*)pNode;

	if (pSpread != NULL)
	{
		pDimScale = pSpread->GetPtrDimScale();
		if (pDimScale->IsActive())
			return pDimScale;
	}

	return (DimScale::GetPtrDefaultDimScale());
}


/**************************************************************************************
>	static DimScale* DimScale::GetPtrDefaultDimScale()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		-
	Returns:	Ptr to the default DimScale
	Purpose:	Ptr to the default DimScale object that is returned by GetPtrDimScale
				if it can't find one in the tree
	SeeAlso:	DimScale::GetPtrDimScale

***************************************************************************************/

DimScale* DimScale::GetPtrDefaultDimScale()
{
	return pDefaultDimScale;
}


/**************************************************************************************
>	BOOL DimScale::ComponentsToMillipoint(double* pMillipoint, double Number, UnitType Units)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/95
	Inputs:		Number -
				Units  - 
	Outputs:	pMillipoint - 
	Returns:	FALSE if fails
	Purpose:	Convert a number of specified units to an accurate millipoint value
	Note:		Unfortunately this duplicate some functionality in other routines
				but these functions are considered stable to change at the moment
***************************************************************************************/

BOOL DimScale::ComponentsToMillipoint(double* pMillipoint, double Number, UnitType Units)
{
	ERROR2IF(pMillipoint==NULL,  FALSE,"DimScale::ComponentsToMillipoint() - pMillipoint==NULL");
	ERROR2IF(      Units==NOTYPE,FALSE,"DimScale::ComponentsToMillipoint() - Units==NOTYPE");
	DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
	ERROR2IF(pDocUnitList==NULL,FALSE,"DimScale::ComponentsToMillipoint() - pDocUnitList==NULL");
	Unit* pUnit = pDocUnitList->FindUnit(Units);
	ERROR2IF(pUnit==NULL,FALSE,"DimScale::ComponentsToMillipoint() - pUnit==NULL");

	double Divisor = 1;
	if (Active)
       	Divisor = ScaleFactor;

	*pMillipoint = Number * pUnit->GetMillipoints() / Divisor;
	return TRUE;
}

