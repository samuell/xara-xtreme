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


#include "camtypes.h"
#include "fixed16.h"
#include "strings.h"

DECLARE_SOURCE("$Revision$");

/********************************************************************************************
>	void FIXED16::ToString(StringBase* out) const 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93 - rewritten in C 12/8/93 by Andy - rewritten so it works: JCF 31/10/94
	Inputs:		None.
	Outputs:	Returns a string representing the value of a FIXED16 in the 'out' parameter.
				Optional - sign, up	to five digits of integer, then option period and five
				fractional digits then zero terminator (max 13 character result)
	Returns:	-
	Purpose:	Converts value of FIXED16 into a string representing it. The string will be
				ASCII or Unicode depending on the size of TCHAR
	Errors:		-
********************************************************************************************/

void fixed16::ToString(StringBase* pOut) const
{
	// Allocate some working storage etc.
	TCHAR buf[32];
	TCHAR* pch = buf;
	INT32 n = all;

	// If the number is negative then output a sign and make it positive.
	if (n < 0)
	{
		*pch++ = TEXT('-');
		n = -n;
	}

	// Convert and output the integer part of the number.
	pch += camSnprintf( pch, 256, TEXT("%u"), unsigned(n >> F16SHIFT) );

	// Mask off the fractional part, converting it if it exists.
	n &= (1uL << F16SHIFT) - 1;
	if (n != 0)
	{
		// Output a decimal point.
		*pch++ = TEXT('.');

		// "Normalise" into an integer by multiplying by 100000/65336.
		n = MulDiv32By32(n, 100000L, 65536L);

		// Convert and output.  We must do this ourselves as we must take
		// account of leading zeros.
		INT32 nPowerTen = 10000L;
		while (nPowerTen > 0)
		{
			// Is the remaining part of the number greater than our power of ten?
			if (n >= nPowerTen)
			{
				// Output a non-zero digit and subtract the closest multiple of the
				// power of ten.
				INT32 nFactor = n / nPowerTen;
				*pch++ = TEXT('0') + TCHAR(nFactor);
				n -= (n * nFactor);
			}
			else
			{
				// Output a leading zero.
				*pch++ = TEXT('0');
			}

			// Go on to the next highest power of ten.
			nPowerTen /= 10;
		}
	}

	// Terminate and write into the output parameter.
	*pch = TEXT('\0');
	*pOut = buf;
}




// Friend Functions - Basic Operators
/********************************************************************************************

>	FIXED16_DBL(d) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/93
	Inputs:		double.
	Outputs:	None
	Returns:	FIXED16.
	Purpose:	
		This macro converts constant doubles into FIXED16s at compile time. It is 
		very effecient because there is no runtime overhead unlike normal conversion.  
		
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline FIXED16::FIXED16 () 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	Default constructor for FIXED16. It does not actually do anything!
	Errors:		None.

********************************************************************************************/

// actual (null) function can be found in class

/********************************************************************************************

>	inline FIXED16::FIXED16 (const FIXED16&) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	Copy constructor.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline FIXED16::FIXED16 (INT32) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	Constructor.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline FIXED16::FIXED16 (short) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	Constructor.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline FIXED16::FIXED16 (double) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93
	SeeAlso:	Fixedasm.h
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	Constructor.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline FIXED16 FIXED16::FromRawLong(INT32 d) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Scope:		Private - is only used to implement FIXED16_DBL macro. Do not call directly!
	Purpose:	converts a INT32 to a FIXED16.
	Errors:		None.
	See Also:	Fixed16::FromShifted16

This function returns the fixed16 associated with the machine implementation representation of
the number passed in. Andy claims this may not be the same on all platforms, so this is the OIL
dependent way to get the internal representation. Compare and contrast FromShifted16 which does
the same thing on this platform.

This is intended for serialisation (not maths use), and also for the FIXED16_DBL macro.

********************************************************************************************/

/********************************************************************************************

>	inline FIXED16 FIXED16::FromShifted16(INT32 d) 

	Author: 	Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Scope:		Public
	Purpose:	converts a INT32 to a FIXED16.
	Errors:		None.
	See Also:	Fixed16::FromShifted16

This function returns the fixed 16 number which is the INT32 shifted down 16 places as a INT32,
i.e. divided by (2^16). Andy claims the internal representation may not be the same on all
platforms, so while FromRawLong() takes the internal representation, this is a platform
independent way of setting the 'meaning' of the fixed16, i.e. what you would get if you
divided the INT32 by 2^16. On this platform, it's the same as GetRawLong().

This is intended for maths use (not serialisation).

********************************************************************************************/

/********************************************************************************************

>	inline INT32 FIXED16::MakeInt() const 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	converts a FIXED16 to an integer.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline short FIXED16::MakeShort() const 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93
	SeeAlso:	Fixedasm.h
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	converts a FIXED16 to an integer.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline INT32 FIXED16::MakeLong() const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	converts a FIXED16 to a INT32.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline XLONG FIXED16::MakeXlong() const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	converts a FIXED16 to a INT32.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline FIXED16& operator= (const FIXED16& operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		operand to be  assigned - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of assignment.
	Purpose:	Overloading the assignment operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline double FIXED16::MakeDouble() const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/93
	SeeAlso:	fixedasm.h
	Inputs:		None
	Outputs:	None
	Returns:    double. 
	Purpose:	Makes a double (32-bit signed value) out of a fixed point 16-bit value.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline FIXED16& operator= (const short operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	SeeAlso:	Fixedasm.h
	Inputs:		operand to be  assigned - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of assignment.
	Purpose:	Overloading the assignment operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline INT32 fixed16::GetRawLong() const

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/94
	Inputs:		-
	Outputs:	-
	Returns:	The longword that holds the whole value, i.e. the value <<16.
	Purpose:	Very efficient way of getting an entire longword. Use sparingly.
	Errors:		fixed16::GetShifted16()

This function returns the machine implementation representation of the number. Andy claims
this may not be the same on all platforms, so this is the OIL dependent way to get the internal
representation. Compare and contrast GetShifted16 which does the same thing on this platform.

This is intended for serialisation (not maths use).

********************************************************************************************/

/********************************************************************************************

>	inline INT32 fixed16::GetShifted16() const

	Author: 	Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Outputs:	-
	Returns:	The value <<16.
	Purpose:	Efficient way to get the value shifted 16 places
	Errors:		None.
	See Also:	fixed16::GetRawLong

This function returns the fixed 16 number shifted up 16 places as a INT32, i.e. when multiplied.
by (2^16). Andy claims the internal representation may not be the same on all platforms, so 
while GetRawLong() returns the internal representation, this is a platform independent way
of returning the 'meaning' of the fixed16, i.e. what you would get if you multiplied it by
2^16. On this platform, it's the same as GetRawLong().

This is intended for maths use (not serialisation).

********************************************************************************************/

/********************************************************************************************

>	inline FIXED16 FIXED16::trunc() const

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Inputs:		Reference to a FIXED16.
	Outputs:	None.
	Returns:	The integer part of the input FIXED16.
	Purpose:	Take the integer part of FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline FIXED16 FIXED16::round() const

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Inputs:		Reference to a FIXED16.
	Outputs:	None.
	Returns:	The closest integer to the input FIXED16.
	Purpose:	Find the closest integer to a FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline FIXED16 FIXED16::abs() const

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Inputs:		Reference to a FIXED16.
	Outputs:	None.
	Returns:	The positive value of the input FIXED16.
	Purpose:	Find the absolute value (magnitude) of a FIXED16.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	inline FIXED16 FIXED16::sgn() const

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Inputs:		Reference to a FIXED16.
	Outputs:	None.
	Returns:	+1 if the input FIXED 16 was positive or 0, -1 if the input was negative.
	Purpose:	Find the sign of a FIXED16.
				Note the difference between this and BASIC SGN functions: This one returns
				1 when the input value is zero!
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	FIXED16 operator+ (const FIXED16& operand1, const FIXED16& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Friend:		FIXED16
	Inputs:		Two values to be added together.
	Outputs:	None.
	Returns:	A FIXED16 with result of addition.
	Purpose:	Overloading the addition operator for FIXED16s.
	Errors:		None.

NB - inline

********************************************************************************************/

/********************************************************************************************

>	FIXED16 operator- (const FIXED16& operand1, const FIXED16& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Friend:		FIXED16
	Inputs:		Two values to be subtracted from each other.
	Outputs:	None.
	Returns:	A FIXED16 with result of subtraction.
	Purpose:	Overloading the subtract operator for FIXED16s.
	Errors:		None.

NB - inline

********************************************************************************************/

/********************************************************************************************

>	FIXED16 operator- (const FIXED16& operand1)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/93
	Friend:		FIXED16
	Inputs:		A value to be negated.
	Outputs:	None.
	Returns:	A FIXED16 with result of negation.
	Purpose:	Overloading the negation operator for FIXED16s. Should be inline.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	FIXED16 operator* (const FIXED16& operand1, const FIXED16& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	SeeAlso:	Fixed16Mul().
	Friend:		FIXED16
	Inputs:		Two values to be multiplied togther.
	Outputs:	None.
	Returns:	A FIXED16 with result of multiplication.
	Purpose:	Overloading the multiplication operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	FIXED16 operator/ (const FIXED16& operand1, const FIXED16& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	SeeAlso:	Fixed16Div().
	Friend:		FIXED16
	Inputs:		Two values to be divided.
	Outputs:	None.
	Returns:	A FIXED16 with result of division.
	Purpose:	Overloading the division operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	FIXED16 operator>> (const FIXED16& operand1, UINT32 operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Friend:		FIXED16
	Inputs:		operand1 - value to be shifted.
				operand2 - shift count.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of ASR.
	Purpose:	Overloading the >> operator to mean shift right for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	FIXED16 operator<< (const FIXED16& operand1, UINT32 operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Friend:		FIXED16
	Inputs:		operand1 - value to be shifted.
				operand2 - shift count.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of ASL.
	Purpose:	Overloading the << operator to mean shift left for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	INT32 operator== (const FIXED16& operand1, const FIXED16& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	Friend:		FIXED16
	Inputs:		Two FIXED16s to be compared.
	Outputs:	None.
	Returns:	0 - False
				1 - True
	Purpose:	Overloading the equality operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	INT32 operator== (const FIXED16& operand1, const short operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93
	Friend:		FIXED16
	Inputs:		FIXED16
				INT8
	Outputs:	None.
	Returns:	0 - False
				1 - True
	Purpose:	Overloading the equality operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	INT32 operator!= (const FIXED16& operand1, const FIXED16& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	Friend:		FIXED16
	Inputs:		Two FIXED16s to be compared.
	Outputs:	None.
	Returns:	0 - False
				1 - True
	Purpose:	Overloading the inequality operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	INT32 operator!= (const FIXED16& operand1, const short operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	Friend:		FIXED16
	Inputs:		FIXED16
				short
	Outputs:	None.
	Returns:	0 - False
				1 - True
	Purpose:	Overloading the inequality operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	INT32 operator> (const FIXED16& operand1, const FIXED16& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	Friend:		FIXED16
	Inputs:		Two FIXED16s to be compared.
	Outputs:	None.
	Returns:	0 - False
				1 - True
	Purpose:	Overloading the greater-than operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	INT32 operator> (const FIXED16& operand1, const short operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	Friend:		FIXED16
	Inputs:		FIXED16
				short
	Outputs:	None.
	Returns:	0 - False
				1 - True
	Purpose:	Overloading the greater-than operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	INT32 operator< (const FIXED16& operand1, const FIXED16& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	Friend:		FIXED16
	Inputs:		Two FIXED16s to be compared.
	Outputs:	None.
	Returns:	0 - False
				1 - True
	Purpose:	Overloading the less-than operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	INT32 operator< (const FIXED16& operand1, const short operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	Friend:		FIXED16
	Inputs:		FIXED16
				short
	Outputs:	None.
	Returns:	0 - False
				1 - True
	Purpose:	Overloading the less-than operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	INT32 operator>= (const FIXED16& operand1, const FIXED16& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	Friend:		FIXED16
	Inputs:		Two FIXED16s to be compared.
	Outputs:	None.
	Returns:	0 - False
				1 - True
	Purpose:	Overloading the greater-than-equal-to operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	INT32 operator>= (const FIXED16& operand1, const short operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	Friend:		FIXED16
	Inputs:		FIXED16
				short
	Outputs:	None.
	Returns:	0 - False
				1 - True
	Purpose:	Overloading the greater-than-equal-to operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	INT32 operator<= (const FIXED16& operand1, const FIXED16& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	Friend:		FIXED16
	Inputs:		Two FIXED16s to be compared.
	Outputs:	None.
	Returns:	0 - False
				1 - True
	Purpose:	Overloading the less-than-equal-to operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	INT32 operator<= (const FIXED16& operand1, const short operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	Friend:		FIXED16
	Inputs:		FIXED16
				short
	Outputs:	None.
	Returns:	0 - False
				1 - True
	Purpose:	Overloading the less-than-equal-to operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

// Assignment operators

/********************************************************************************************

>	FIXED16& FIXED16::operator= (const INT32 operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		operand to be  assigned - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of assignment.
	Purpose:	Overloading the assignment operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	FIXED16& FIXED16::operator= (const double operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	SeeAlso:	DoubleToFixed16().
	Inputs:		operand to be  assigned - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of assignment.
	Purpose:	Overloading the assignment operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	FIXED16& FIXED16::operator+= (const FIXED16& operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		lvalue to be incremented
				operand to be added - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of assignment.
	Purpose:	Overloading the plus-equals operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	FIXED16& FIXED16::operator+= (const short operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		lvalue to be incremented
				operand to be added - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of assignment.
	Purpose:	Overloading the plus-equals operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	FIXED16& FIXED16::operator-= (const FIXED16& operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		lvalue to be decremented
				operand to be subtracted - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of assignment.
	Purpose:	Overloading the minus-equals operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	FIXED16& FIXED16::operator-= (const short operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		lvalue to be decremented
				operand to be subtracted - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of assignment.
	Purpose:	Overloading the minus-equals operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

/********************************************************************************************

>	FIXED16& FIXED16::operator*= (const FIXED16& operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	SeeAlso:	Fixed16Mul().
	Inputs:		lvalue to be multiplied
				multiplier - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of assignment.
	Purpose:	Overloading the times-equals operator for FIXED16s.
	Errors:		None.

********************************************************************************************/
 
/********************************************************************************************

>	FIXED16& FIXED16::operator*= (const short operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	SeeAlso:	Fixed16Mul().
	Inputs:		lvalue to be multiplied
				multiplier - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of assignment.
	Purpose:	Overloading the times-equals operator for FIXED16s.
	Errors:		None.

********************************************************************************************/
 
/********************************************************************************************

>	FIXED16& FIXED16::operator/= (const FIXED16& operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	SeeAlso:	Fixed16Div().
	Inputs:		lvalue to be divided
				divisor - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of assignment.
	Purpose:	Overloading the divide-equals operator for FIXED16s.
	Errors:		None.

********************************************************************************************/
 
/********************************************************************************************

>	FIXED16& FIXED16::operator/= (const short operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	SeeAlso:	Fixed16Div().
	Inputs:		lvalue to be divided
				divisor - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of assignment.
	Purpose:	Overloading the divide-equals operator for FIXED16s.
	Errors:		None.

********************************************************************************************/
 
/********************************************************************************************

>	FIXED16& FIXED16::operator<<= (const UINT32 operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		lvalue to be shifted
				shift count - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of assignment.
	Purpose:	Overloading the left-shift-equals operator for FIXED16s.
	Errors:		None.

********************************************************************************************/
 
/********************************************************************************************

>	FIXED16& FIXED16::operator>>= (const UINT32 operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		lvalue to be shifted
				shift count - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of assignment.
	Purpose:	Overloading the right-shift-equals operator for FIXED16s.
	Errors:		None.

********************************************************************************************/
  
// Increment FIXED16::operators

/********************************************************************************************

>	FIXED16& FIXED16::operator++ () 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		None.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of increment.
	Purpose:	Overloading the prefix plus-plus operator for FIXED16s.
	Errors:		None.

********************************************************************************************/
  
/********************************************************************************************

>	FIXED16 FIXED16::operator++ (INT32 dummy) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		dummy value to distinguish between pre- and post-fix application.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of increment.
	Purpose:	Overloading the postfix plus-plus operator for FIXED16s.
	Errors:		None.

********************************************************************************************/
  
/********************************************************************************************

>	FIXED16& FIXED16::operator-- () 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		None.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of decrement.
	Purpose:	Overloading the prefix minus-minus operator for FIXED16s.
	Errors:		None.

********************************************************************************************/
  
/********************************************************************************************

>	FIXED16 FIXED16::operator-- (INT32 dummy) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		dummy value to distinguish between pre- and post-fix application.
	Outputs:	None.
	Returns:	A reference to an FIXED16 with result of decrement.
	Purpose:	Overloading the postfix minus-minus operator for FIXED16s.
	Errors:		None.

********************************************************************************************/

