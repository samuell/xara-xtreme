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

//	The following few functions and types are used in convert.cpp
// 	to allow convertion between strings and millipoints. There main
// 	use will probably be for interacting with users via dialog boxes.
	
/*
*/

#ifndef INC_CONVERT
#define INC_CONVERT

#include "unittype.h"

class Node;
//class DimScale;

/***********************************************************************************************

>	class Convert : public CCObject

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Purpose:	This class is used to convert things from one form to another. Mainly contains
				static functions.
				Started life as a series of conversion functions before being made to see the
				light and profess the one and only true class. 
				At present in the Winoil class as it needs converting to TCHARs and using the
				MFC forms of scanf and sprintf ..

				Note by Rik who did the original functions 11/5/93:-

				These functions will provide a means of converting from Doc Coords (millipoints)
				to Strings and visa versa. For example, convert 72000 millipoints to the string
				1in or 72pt as well as converting from the string '1in72pt' to 144000
				millipoints. The units to convert to can be chosen, as can the number of 
				decimal points. When converting from string back to millipoints, all the valid
				units are added together. If an error is detected (ie '4cm72zx' ), then
				everything up to the error will be used (4cm).
				
	SeeAlso:	DimScale, Spread, Layer

***********************************************************************************************/

class Convert
{

public:
	// Allows us to declare preferences, read the current number formats from the operating
	// system etc.
	static	BOOL Init();

	// This is the setting function for the values below, as these are read from the OS.
	static  BOOL GetCurrentNumberFormat();

	// Allows other classes to find out factors about the current number format which the
	// conversion class is using.
	static	char GetDecimalPointChar();
	static	char GetThousandsSepChar();
	static	UINT32 GetNumberDecimalPlaces();

	// Just incase we decide these are required to overide the OS's defaults.
	static	BOOL SetDecimalPointChar(char NewDecimalPoint);
	static	BOOL SetThousandsSepChar(char NewThousandsSep);
	static	BOOL SetNumberDecimalPlaces(UINT32 NewDPs);

public:
	// Now for some actual conversion functions
	static	MILLIPOINT 	StringToMillipoints( const StringBase &, UnitType, BOOL* );
	static 	void		NumberToString( MILLIPOINT Number, StringBase* MyString );
	static	BOOL 		MillipointsToString( double, UnitType, StringBase*, INT32 DecimalPlaces = -1);
	static	double 		StringToDouble( const StringBase &, UnitType, BOOL * );
	static	BOOL 		StringToDouble( const StringBase &InputString, double *Number);
	static	BOOL 		DoubleToString( double Number, StringBase* OutputString, INT32 DecimalPlaces = -1 );

	static	BOOL 		StringToComponents(const StringBase &InputString,double* pValue,UnitType* pUnits);
	static	double 		ConvertToNewUnits(double Value,UnitType Units,UnitType NewUnits);
	static	BOOL		ConvertDoubleToMillipoint(double Value,MILLIPOINT* pResult);
	static	void		ConvertMillipointToDouble(MILLIPOINT Value,double* pResult);

	static	BOOL 		LongToString( INT32 Number, StringBase* OutputString );
	static	BOOL 		StringToLong( const StringBase &InputString, INT32* Number );

	// Convert memory values to strings and strings to memory values
	static	UINT32 		StringToBytes(const StringBase &pString, BOOL *Valid);
	static	BOOL 		BytesToString(StringBase* pString, UINT32 Bytes);

	// Some useful functions
	static  BOOL ReplaceDecimalPoint(StringBase* pString);
	static  BOOL StripTrailingZeros(StringBase* pString);
	static  BOOL IsCharUnitType(TCHAR Char);
	static  BOOL IsCharStartOfNumber(TCHAR Char);
	static  BOOL ReadUnitType(const StringBase &Str,INT32* pPos,UnitType* pUnitType);
	static  BOOL ReadNumber(const StringBase &Str,INT32* pPos,double* pResult);

	// This defines a name for a pointer to the "string parser" function that takes
	// a string and calculates the number from it, returning TRUE if it is valid.

	typedef BOOL (*PFNSTRINGTOINT32)(const StringBase &, INT32*);
	typedef BOOL (*PFNSTRINGTODOUBLE)(const StringBase &, double*);
 
protected:
	// These are the default display options
	static char	DecimalPoint;	 	// Character to separate whole part of number from fractional part
	static char ThousandSep;	 	// Character to separate thousands, e.g. 10,000
	static char MinusSign;			// What to show if go less than zero
	static UINT32 NumDecimalPlaces;	// Number of decimal places to display to by default

};

/***********************************************************************************************

>	class DimScale : public CCObject

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/93
	Purpose:	This class is used to scale millipoint dimension values to and from a user-set
				preference. 
				E.g. the user may specify that 1 mile = 2 centimetres.  This class will record 
				the units (in this case miles) and the scale factor used to convert	internal 
				millipoint values to and from the units the user wishes to work in.
				
	SeeAlso:	Spread, Layer

***********************************************************************************************/

class DimScale : public CCObject
{
	CC_DECLARE_DYNAMIC( DimScale )   
public:
	DimScale();

	BOOL		IsActive();
	void		SetActiveState(BOOL State);

	double 		GetScaleFactor();
	BOOL		SetScaleFactor(double NewScaleFactor);
	BOOL		SetScaleFactor();	// Sets the scale factor using 'Drawing' and 'Real' strings

	UnitType	GetUnits();
	UnitType	GetScaleUnits();

	String_32	GetDrawingScaleStr();
	BOOL		SetDrawingScaleStr(String_32& NewStr);
	String_32	GetRealScaleStr();
	BOOL		SetRealScaleStr(String_32& NewStr);

	BOOL		ConvertToUnits(double Value,String_256* pStr,BOOL UnitSpecifier=TRUE,
								INT32 dp=-1, UnitType units=NOTYPE, BOOL bSquare = FALSE);
	BOOL		ConvertToMillipoints(const StringBase& Str, MILLIPOINT* pMpValue);
	BOOL 		ConvertToDouble(const StringBase& Str,double* pResult);

	BOOL		ComponentsToMillipoint(double* pMillipoint, double Number, UnitType Units);

	DimScale& 	operator=(const DimScale& other);
	BOOL	 	operator==(const DimScale& other);

	static BOOL			Init();		// Must be called before any other call in this class
	static void			Deinit();	// Cleans up before the program quits
	static DimScale* 	GetPtrDimScale(Node* pNode);
	static DimScale*	GetPtrDefaultDimScale();

private:
	BOOL		Active;
	double		ScaleFactor;
	String_32	DrawingScaleStr;
	String_32	RealScaleStr;

//	// The default DimScale object is an inactive one that is used as a fall-back in the 
//	// frightening event of one not being found by DimScale::GetPtrDimScale(Node*)
//	static DimScale * pDefaultDimScale;
};

#endif	// INC_CONVERT
