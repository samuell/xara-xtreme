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
#include "ensure.h"
#include "xlong.h"

DECLARE_SOURCE("$Revision$");

#if !defined(USE_NATIVE_XLONG)

/********************************************************************************************

>	void XLONG::ToString() const 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93
	SeeAlso:	Xlongasm.h
	Inputs:		None.
	Outputs:	None
	Returns:	Returns a string representing the value of a XLONG.
	Purpose:	Converts value of XLONG into an ASCII string representing it.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

void xlong::ToString(String* out) const
{
	TCHAR s[21];
	             
	XlongToAscii(*this, s);             
	
	*out = s;
}


// Friend Functions - Basic Operators

/********************************************************************************************

>	XLONG operator+ (const XLONG& operand1, const XLONG& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be added together.
	Outputs:	None.
	Returns:	An XLONG with result of addition.
	Purpose:	Overloading the addition operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/


xlong operator+ (const xlong& operand1, const xlong& operand2)
{
	xlong result = operand1;

	XlongAddOne(operand2, result);

	return result;
}

/********************************************************************************************

>	XLONG operator+ (const XLONG& operand1, INT32 operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be added together.
	Outputs:	None.
	Returns:	An XLONG with result of addition.
	Purpose:	Overloading the addition operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/


xlong operator+ (const xlong& operand1, INT32 operand2)
{
	xlong result = operand1;
	xlong temp;
    
    LongToXlong(operand2, temp);
    
	XlongAddOne(temp, result);

	return result;
}

/********************************************************************************************

>	XLONG operator+ (const XLONG& operand1, INT32 operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be added together.
	Outputs:	None.
	Returns:	An XLONG with result of addition.
	Purpose:	Overloading the addition operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/


xlong operator+ (const xlong& operand1, INT32 operand2)
{
	xlong result = operand1;
	xlong temp;
    
    LongToXlong((INT32)operand2, temp);
    
	XlongAddOne(temp, result);

	return result;
}

/********************************************************************************************

>	XLONG operator- (const XLONG& operand1, const XLONG& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be subtracted from each other.
	Outputs:	None.
	Returns:	An XLONG with result of subtraction.
	Purpose:	Overloading the subtract operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong operator- (const xlong& operand1, const xlong& operand2)
{
	xlong result = operand1;

	XlongSubOne(operand2, result);

	return result;
}

/********************************************************************************************

>	XLONG operator- (const XLONG& operand1, const INT32 operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be subtracted from each other.
	Outputs:	None.
	Returns:	An XLONG with result of subtraction.
	Purpose:	Overloading the subtract operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong operator- (const xlong& operand1, INT32 operand2)
{
	xlong result = operand1;
   	xlong temp;
    
    LongToXlong(operand2, temp);

	XlongSubOne(temp, result);

	return result;
}

/********************************************************************************************

>	XLONG operator- (const XLONG& operand1, INT32 operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be subtracted from each other.
	Outputs:	None.
	Returns:	An XLONG with result of subtraction.
	Purpose:	Overloading the subtract operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong operator- (const xlong& operand1, INT32 operand2)
{
	xlong result = operand1;
   	xlong temp;
    
    LongToXlong((INT32)operand2, temp);

	XlongSubOne(temp, result);

	return result;
}

/********************************************************************************************

>	XLONG operator- (const XLONG& operand)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Value for negation.
	Outputs:	None.
	Returns:	The negated value. 
	Purpose:	Overloading the negation operator for XLONGs.
	Errors:		None.

********************************************************************************************/

xlong operator- (const xlong& input)
{
	xlong temp;

	XlongNegOne( temp = input );
	return temp;
}

/********************************************************************************************

>	XLONG operator* (const XLONG& operand1, const XLONG& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be multiplied togther.
	Outputs:	None.
	Returns:	An XLONG with result of multiplication.
	Purpose:	Overloading the multiplication operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong operator* (const xlong& operand1, const xlong& operand2)
{
	xlong result = operand1;

	XMul64One(operand2, result);

	return result;
}

/********************************************************************************************

>	XLONG operator* (const XLONG& operand1, INT32 operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be multiplied togther.
	Outputs:	None.
	Returns:	An XLONG with result of multiplication.
	Purpose:	Overloading the multiplication operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong operator* (const xlong& operand1, INT32 operand2)
{
	xlong result = operand1;
    xlong temp;
    
    LongToXlong(operand2, temp);

	XMul64One(temp, result);

	return result;
}

/********************************************************************************************

>	XLONG operator* (const XLONG& operand1, INT32 operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be multiplied togther.
	Outputs:	None.
	Returns:	An XLONG with result of multiplication.
	Purpose:	Overloading the multiplication operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong operator* (const xlong& operand1, INT32 operand2)
{
	xlong result = operand1;
    xlong temp;
    
    LongToXlong((INT32)operand2, temp);

	XMul64One(temp, result);

	return result;
}

#endif

/********************************************************************************************

>	XLONG operator* (const XLONG& operand1, FIXED16 operand2) 

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be multiplied togther.
	Outputs:	None.
	Returns:	An XLONG with result of multiplication.
	Purpose:	Overloading the multiplication operator for XLONGs and FIXED16s.
	Errors:		None.

********************************************************************************************/

xlong operator* (const xlong& operand1, FIXED16 operand2)
{
    xlong temp = operand1;

	XlongByFixed16One( &temp, operand2 );    		// more efficient to pass 1 ptr than 2

	return temp;
}

#if !defined(USE_NATIVE_XLONG)

/********************************************************************************************

>	XLONG operator/ (const XLONG& operand1, const XLONG& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be divided.
	Outputs:	None.
	Returns:	An XLONG with result of division.
	Purpose:	Overloading the division operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong operator/ (const xlong& operand1, const xlong& operand2)
{
	xlong result = operand1;

	XDiv64One(operand2, result);

	return result;
}

/********************************************************************************************

>	XLONG operator/ (const XLONG& operand1, INT32 operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be divided.
	Outputs:	None.
	Returns:	An XLONG with result of division.
	Purpose:	Overloading the division operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong operator/ (const xlong& operand1, INT32 operand2)
{
	xlong result = operand1;
    xlong temp;
    
    LongToXlong(operand2, temp);

	XDiv64One(temp, result);

	return result;
}

/********************************************************************************************

>	XLONG operator/ (const XLONG& operand1, INT32 operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be divided.
	Outputs:	None.
	Returns:	An XLONG with result of division.
	Purpose:	Overloading the division operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong operator/ (const xlong& operand1, INT32 operand2)
{
	xlong result = operand1;
    xlong temp;
    
    LongToXlong((INT32)operand2, temp);

	XDiv64One(temp, result);

	return result;
}

#endif

/********************************************************************************************

>	XLONG operator/ (const XLONG& operand1, const FIXED16& operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
    Friend:		XLONG
	SeeAlso:	Xlongasm.h
	Inputs:		Two values to be divided.
	Outputs:	None.
	Returns:	An XLONG with result of division.
	Purpose:	Overloading the division operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong operator/ (const xlong& operand1, const fixed16& operand2)
{
	xlong result;
	
	result = operand1;

//	XlongByFixed16One(&result, operand2);
	ENSURE( FALSE, "You are using the XLONG / operator which actually multiplies!" );

	return result;
}

#if !defined(USE_NATIVE_XLONG)

/********************************************************************************************

>	XLONG operator>> (const XLONG& operand, UINT32 shift) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		operand1 - value to be shifted.
				operand2 - shift count.
	Outputs:	None.
	Returns:	An XLONG with result of ASR.
	Purpose:	Overloading the >> operator to mean shift right for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong operator>> (const xlong& operand, UINT32 shift)
{
	xlong result = operand;

	XlongAsr64One(shift, result);

	return result;
}

/********************************************************************************************

>	XLONG operator<< (const XLONG& operand, UINT32 shift) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		operand - shift count.
	Outputs:	None.
	Returns:	An XLONG with result of ASL.
	Purpose:	Overloading the << operator to mean shift left for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong operator<< (const xlong& operand, UINT32 shift)
{
	xlong result = operand;

	XlongAsl64One(shift, result);

	return result;
}

// Assignment Operators

/********************************************************************************************

>	XLONG& XLONG::operator= (const XLONG& operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	Inputs:		operand to be  assigned - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of assignment.
	Purpose:	Overloading the assignment operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong& xlong::operator= (const xlong& operand)
{
	this->low = operand.low;
	this->high = operand.high;

	return *this;
}

/********************************************************************************************

>	XLONG& XLONG::operator= (INT32 operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
	Inputs:		operand to be  assigned - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of assignment.
	Purpose:	Overloading the assignment operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong& xlong::operator= (INT32 operand)
{
	xlong temp;
	
	LongToXlong(operand, temp);
	
	this->low = temp.low;
	this->high = temp.high;

	return *this;
}

/********************************************************************************************

>	XLONG& XLONG::operator+= (const XLONG& operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
	Inputs:		lvalue to be incremented
				operand to be added - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of assignment.
	Purpose:	Overloading the plus-equals operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong& xlong::operator+= (const xlong& operand)
{
	XlongAddOne(operand, *this);

	return *this;
}

/********************************************************************************************

>	XLONG& XLONG::operator+= (INT32 operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
	Inputs:		lvalue to be incremented
				operand to be added - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of assignment.
	Purpose:	Overloading the plus-equals operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong& xlong::operator+= (INT32 operand)
{                              
	xlong temp;
	      
	LongToXlong(operand, temp);
	
	XlongAddOne(temp, *this);

	return *this;
}

/********************************************************************************************

>	XLONG& XLONG::operator-= (const XLONG& operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
	Inputs:		lvalue to be incremented
				operand to be subtracted - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of assignment.
	Purpose:	Overloading the minus-equals operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong& xlong::operator-= (const xlong& operand)
{
	XlongSubOne(operand, *this);

	return *this;
}

/********************************************************************************************

>	XLONG& XLONG::operator-= (INT32 operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
	Inputs:		operand to be subtracted - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of assignment.
	Purpose:	Overloading the minus-equals operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

xlong& xlong::operator-= (INT32 operand)
{
	xlong temp;
	      
	LongToXlong(operand, temp);

	XlongSubOne(temp, *this);

	return *this;
}

/********************************************************************************************

>	XLONG& XLONG::operator*= (const XLONG& operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
	Inputs:		multiplier - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of assignment.
	Purpose:	Overloading the times-equals operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/
 
xlong& xlong::operator*= (const xlong& operand)
{
	XMul64One(operand, *this);

	return *this;
}

/********************************************************************************************

>	XLONG& XLONG::operator*= (INT32 operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
	Inputs:		multiplier - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of assignment.
	Purpose:	Overloading the times-equals operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/
 
xlong& xlong::operator*= (INT32 operand)
{
	xlong temp;
	      
	LongToXlong(operand, temp);

	XMul64One(temp, *this);

	return *this;
}

#endif

/********************************************************************************************

>	XLONG& XLONG::operator*= (FIXED16 operand) 

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/93
	SeeAlso:	Xlongasm.h
	Inputs:		multiplier - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of assignment.
	Purpose:	Overloading the times-equals operator for XLONGs, multiplying by a fixed16.
	Errors:		None (overflow ignored).
	SeeAlso:	XlongByFixed16One

********************************************************************************************/

xlong &operator*=( xlong &xl, fixed16 fix )
{
	XlongByFixed16One( &xl, fix );
	return xl;
}

#if !defined(USE_NATIVE_XLONG)

/********************************************************************************************

>	XLONG& XLONG::operator/= (const XLONG& operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
	Inputs:		divisor - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of assignment.
	Purpose:	Overloading the divide-equals operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/
 
xlong& xlong::operator/= (const xlong& operand)
{
	XDiv64One(operand, *this);

	return *this;
}
 
/********************************************************************************************

>	XLONG& XLONG::operator/= (INT32 operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
	Inputs:		divisor - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of assignment.
	Purpose:	Overloading the divide-equals operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/
 
xlong& xlong::operator/= (INT32 operand)
{
	xlong temp;
	      
	LongToXlong(operand, temp);

	XDiv64One(temp, *this);

	return *this;
}
 
/********************************************************************************************

>	XLONG& XLONG::operator<<= (UINT32 shift) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
	Inputs:		shift count - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of assignment.
	Purpose:	Overloading the left-shift-equals operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/
 
xlong& xlong::operator<<= (UINT32 shift)
{
	XlongAsl64One(shift, *this);

	return *this;
}

/********************************************************************************************

>	XLONG& XLONG::operator>>= (UINT32 operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
	Inputs:		shift count - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of assignment.
	Purpose:	Overloading the right-shift-equals operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/
  
xlong& xlong::operator>>= (UINT32 shift)
{
	XlongAsr64One(shift, *this);

	return *this;
} 

// Increment\Decrement Operators

/********************************************************************************************

>	XLONG& XLONG::operator++ () 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
	Inputs:		None.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of increment.
	Purpose:	Overloading the prefix plus-plus operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/
  
xlong& xlong::operator++ ()							// prefix
{
	XlongIncrement(*this);
	
	return *this;
}
							
/********************************************************************************************

>	XLONG XLONG::operator++ (INT32 dummy) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
	Inputs:		dummy value to distinguish between pre- and post-fix application.
	Outputs:	None.
	Returns:	An XLONG with result of increment.
	Purpose:	Overloading the postfix plus-plus operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/
  
xlong xlong::operator++ (INT32 dummy)   				// postfix 
{
	xlong result = *this;
	
	XlongIncrement(*this);
	
	return result;
}

/********************************************************************************************

>	XLONG& XLONG::operator-- () 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
	Inputs:		None.
	Outputs:	None.
	Returns:	A reference to an XLONG with result of decrement.
	Purpose:	Overloading the prefix minus-minus operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/
  
xlong& xlong::operator-- ()							// prefix 
{
	XlongDecrement(*this);
	
	return *this;
}

/********************************************************************************************

>	XLONG XLONG::operator-- (INT32 dummy) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
	Inputs:		dummy value to distinguish between pre- and post-fix application.
	Outputs:	None.
	Returns:	An XLONG with result of decrement.
	Purpose:	Overloading the postfix minus-minus operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/
  
xlong xlong::operator-- (INT32 dummy)				// postfix 
{
	xlong result = *this;
	
	XlongDecrement(*this);
	
	return result;
}

// Relational Operators

/********************************************************************************************

>	INT32 operator== (const XLONG& first, const XLONG& second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two XLONGs to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix equality operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator== (const xlong& first, const xlong& second)
{ 
	return XlongCmpEQ(first, second); 
}

/********************************************************************************************

>	INT32 operator== (const XLONG& first, INT32 second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix equality operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator== (const xlong& first, INT32 second) 
{                                 
	xlong temp;
	
	LongToXlong(second, temp);
	
	return XlongCmpEQ(first, temp); 
}

/********************************************************************************************

>	INT32 operator== (const XLONG& first, INT32 second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix equality operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator== (const xlong& first, INT32 second) 
{                                 
	xlong temp;
	
	LongToXlong((INT32)second, temp);
	
	return XlongCmpEQ(first, temp); 
}

/********************************************************************************************

>	INT32 operator!= (const XLONG& first, const XLONG& second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two XLONGs to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix inequality operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator!= (const xlong& first, const xlong& second) 
{ 
	return XlongCmpNE(first, second); 
}

/********************************************************************************************

>	INT32 operator!= (const XLONG& first, INT32 second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two valuess to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix inequality operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator!= (const xlong& first, INT32 second) 
{ 
	xlong temp;
	
	LongToXlong(second, temp);
	
	return XlongCmpNE(first, temp); 
}

/********************************************************************************************

>	INT32 operator!= (const XLONG& first, INT32 second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two valuess to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix inequality operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator!= (const xlong& first, INT32 second) 
{ 
	xlong temp;
	
	LongToXlong((INT32)second, temp);
	
	return XlongCmpNE(first, temp); 
}

/********************************************************************************************

>	INT32 operator< (const XLONG& first, const XLONG& second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two XLONGs to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix less-than operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator< (const xlong& first, const xlong& second) 
{ 
	return XlongCmpLT(first, second); 
}

/********************************************************************************************

>	INT32 operator< (const XLONG& first, INT32 second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix less-than operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator< (const xlong& first, INT32 second)
{ 
	xlong temp;
	
	LongToXlong(second, temp);
	
	return XlongCmpLT(first, temp); 
}
	
/********************************************************************************************

>	INT32 operator< (const XLONG& first, INT32 second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix less-than operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator< (const xlong& first, INT32 second)
{ 
	xlong temp;
	
	LongToXlong((INT32)second, temp);
	
	return XlongCmpLT(first, temp); 
}
	
/********************************************************************************************

>	INT32 operator> (const XLONG& first, const XLONG& second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two XLONGs to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix greater-than operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator> (const xlong& first, const xlong& second) 
{ 
	return XlongCmpGT(first, second); 
}

/********************************************************************************************

>	INT32 operator> (const XLONG& first, INT32 second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix greater-than operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator> (const xlong& first, INT32 second)
{ 
	xlong temp;
	
	LongToXlong(second, temp);
	
	return XlongCmpGT(first, temp); 
}

/********************************************************************************************

>	INT32 operator> (const XLONG& first, INT32 second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix greater-than operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator> (const xlong& first, INT32 second)
{ 
	xlong temp;
	
	LongToXlong((INT32)second, temp);
	
	return XlongCmpGT(first, temp); 
}

/********************************************************************************************

>	INT32 operator<= (const XLONG& first, const XLONG& second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two XLONGs to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix less-than--or-equal operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator<= (const xlong& first, const xlong& second) 
{ 
	return XlongCmpLE(first, second); 
}

/********************************************************************************************

>	INT32 operator<= (const XLONG& first, INT32 second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix less-than--or-equal operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator<= (const xlong& first, INT32 second)
{ 
	xlong temp;
	
	LongToXlong(second, temp);
	
	return XlongCmpLE(first, temp); 
}

/********************************************************************************************

>	INT32 operator<= (const XLONG& first, INT32 second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix less-than--or-equal operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator<= (const xlong& first, INT32 second)
{ 
	xlong temp;
	
	LongToXlong((INT32)second, temp);               
		
	return XlongCmpLE(first, temp); 
}

/********************************************************************************************

>	INT32 operator>= (const XLONG& first, const XLONG& second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two XLONGs to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix greater-than--or-equal operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator>= (const xlong& first, const xlong& second) 
{ 
	return XlongCmpGE(first, second); 
}

/********************************************************************************************

>	INT32 operator>= (const XLONG& first, INT32 second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix greater-than--or-equal operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator>= (const xlong& first, INT32 second)
{ 
	xlong temp;
	
	LongToXlong(second, temp);
	
	return XlongCmpGE(first, temp); 
}

/********************************************************************************************

>	INT32 operator>= (const XLONG& first, INT32 second)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
    Friend:		XLONG
	Inputs:		Two values to be compared
	Outputs:	None.
	Returns:	Integer - 	0 = False 
							1 = True 
	Purpose:	Overloading the postfix greater-than--or-equal operator for XLONGs.
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 operator>= (const xlong& first, INT32 second)
{ 
	xlong temp;
	
	LongToXlong((INT32)second, temp);
	
	return XlongCmpGE(first, temp); 
}

/********************************************************************************************

>	FIXED16 XLONG::MakeFixed16 ()
 	
	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	SeeAlso:	Xlongasm.h
	Inputs:		None.
	Outputs:	None.
	Returns:	INT32 value.
	Purpose:	To convert an XLONG value to a FIXED16. 
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

fixed16 xlong::MakeFixed16() const
{  
	return fixed16( (short)this->low );
}

#endif

/********************************************************************************************

>	void XlongAdd(Xlong *input1, Xlong *input2, Xlong *result)

>	void XlongAddOne(xlong *input1, xlong *result)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Scope:		Private to Xlong class
	Inputs:
	Outputs:	pointer to the xlong result
	Returns:	None
	Purpose:	64-bit signed addition
	Errors:		None (no overflow checks)

********************************************************************************************/

void XlongAdd( const xlong& input1,const xlong& input2, xlong& result )
{
	result = input1+input2 ;
}

void XlongAddOne( const xlong& input, xlong& result )
{
	result += input ;
}


/********************************************************************************************

>	void XlongSub(Xlong *input1, Xlong *input2, Xlong *result)

>	void XlongSubOne(xxlong *input, xlong *result)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Scope:		Private to Xlong class
	Inputs:
	Outputs:	pointer to the xlong result (input1 - input2) or (result-=input)
	Returns:	None
	Purpose:	64-bit signed subtraction
	Errors:		None (no overflow checks)

	Most of the overhead is in extracting the parameters from the pointers
	so this does *not* use macros to hide memory models etc
********************************************************************************************/

void XlongSub( const xlong& input1,const xlong& input2, xlong& result )
{
	result = input1-input2 ;
}

void XlongSubOne( const xlong& input, xlong& result )
{
	result -= input ;
}


/********************************************************************************************

>	extern "C" void XlongNegOne(Xlong *arg)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Scope:		Private to Xlong class
	Inputs:		pointer to value requiring negation
	Outputs:	contents of pointer pointer updated
	Returns:	None
	Purpose:	64-bit signed negation
	Errors:		None (no overflow checks)

********************************************************************************************/

void XlongNegOne( xlong& arg )
{
	arg = -arg ;
}

/********************************************************************************************

>	INT32 Mul32Div32(INT32 mul1, INT32 mul2, INT32 divisor)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/93
	Scope:		Private to Xlong class
	Inputs:		two multiplicands, one divisor
	Outputs:	result of mul1*mul2/div using 64-bit intermediate result (signed)
	Returns:	INT32 result
	Purpose:	To avoid losing precision 
	Errors:		If divisor zero or overflow will INT32 0 (div0)

********************************************************************************************/


INT32 Mul32Div32(INT32 mul1, INT32 mul2, INT32 divisor)
{
//	return MulDiv(mul1,mul2,divisor) ;
	return INT32(((xlong)mul1*mul2)/divisor) ;
}

/********************************************************************************************

>	void XMul32(INT32 mul1, INT32 mul2, Xlong& result)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/93
	Scope:		Private to Xlong class
	Inputs:		two 32-bit multiplicands
	Outputs:	signed 64-bit
	Returns:	none
	Purpose:	To avoid losing precision 
	Errors:		None (no overflow checks)

********************************************************************************************/

void XMul32(INT32 mul1, INT32 mul2, xlong& result)
{
	result = (xlong)mul1*mul2 ;
}


/********************************************************************************************

>	void XMul64( const Xlong& m1,const Xlong& m2, Xlong& result)

>	void XMul64One(const Xlong& m, Xlong& result)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/93
	Scope:		Private to Xlong class
	Inputs:		two 64-bit multiplicands
	Outputs:	signed 64-bit (m1*m2 or result*=m)
	Returns:	none
	Purpose:	Private to ccmath.h. Primitive for operator overloading
	Errors:		None (no overflow checks)

********************************************************************************************/

void XMul64( const xlong& m1,const xlong& m2, xlong& result)
{
	result = m1*m2 ;
}

void XlongMulOne( const xlong& m, xlong& result )
{
	result *= m ;
}

/********************************************************************************************

>	void longToXlong( const INT32 input, xlong& result)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/93
	Scope:		Private to Xlong class
	Inputs:		32-bit signed INT32
	Outputs:	64-bit signed xlong
	Returns:	none
	Purpose:	Private to ccmath.h. Primitive for operator overloading
	Errors:		None possible

********************************************************************************************/

void longToXlong( const INT32 input, xlong& result)
{
	result = input ;
}


/********************************************************************************************

>	INT32 XlongTolong( const xlong& input)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/93
	Scope:		Private to Xlong class
	Inputs:		64-bit signed xlong
	Outputs:	32-bit signed INT32
	Returns:	none
	Purpose:	Private to ccmath.cpp. Primitive for operator overloading
	Errors:		None (overflow ignored)

********************************************************************************************/

INT32 XlongTolong( const xlong& input)
{
	return (INT32)input ;
}
	

/********************************************************************************************

>	void XlongAsl64( const xlong& input, INT32 shift, xlong& result)

>	void XlongAsl64One( INT32 shift, xlong& result)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/93
	Scope:		Private to Xlong class
	Inputs:		64-bit signed xlong, INT32 shift count
	Outputs:	64-bit signed xlong
	Returns:	none
	Purpose:	Private to ccmath.cpp. Primitive for operator overloading
	Errors:		None (overflow ignored). The shift count is masked with 3F
			for compatibility with normal Intel shifts

********************************************************************************************/

void XlongAsl64( const xlong& input, INT32 shift, xlong& result)
{
	result = input<<shift ;
}

void XlongAsl64One( INT32 shift, xlong& result )
{
	result <<= shift ;
}

/********************************************************************************************

>	void XlongAsr64( const xlong& input, INT32 count, xlong& result)

>	void XlongAsr64One( INT32 count, xlong& result)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/93
	Scope:		Private to Xlong class
	Inputs:		64-bit signed xlong, INT32 shift count
	Outputs:	64-bit signed xlong
	Returns:	none
	Purpose:	Private to ccmath.cpp. Primitive for operator overloading
	Errors:		None (overflow ignored). The shift count is masked with 3F
			for compatibility with normal Intel shifts

********************************************************************************************/

void XlongAsr64( const xlong& input, INT32 shift, xlong& result)
{
	result = input>>shift ;
}

void XlongAsr64One( INT32 shift, xlong& result )
{
	result >>= shift ;
}

/********************************************************************************************

>	INT32 XlongCmp..( const xlong& first, const xlong& second)

	.. = comparison operator, one of:
	GT	>
	GE	>=
	LT	<
	LE	<=
	EQ	==
	NE	!=

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/93
	Scope:		Private to Xlong class
	Inputs:		two xlongs
	Outputs:	returns 1 if comparison true, else 0
	Returns:	none
	Purpose:	Private to ccmath.cpp. Primitive for operator overloading
	Errors:		None

********************************************************************************************/

INT32 XlongCmpGT( const xlong& first, const xlong& second)
{
	return first>second ;
}

INT32 XlongCmpGE( const xlong& first, const xlong& second)
{
	return first>=second ;
}

INT32 XlongCmpLT( const xlong& first, const xlong& second)
{
	return first<second ;
}

INT32 XlongCmpLE( const xlong& first, const xlong& second)
{
	return first<=second ;
}

INT32 XlongCmpEQ( const xlong& first, const xlong& second)
{
	return first==second ;
}

INT32 XlongCmpNE( const xlong& first, const xlong& second)
{
	return first!=second ;
}

/********************************************************************************************

>	void XlongIncrement( xlong& input )

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/93
	Scope:		Private to Xlong class
	Inputs:		64-bit signed xlong
	Outputs:	64-bit signed xlong
	Returns:	none
	Purpose:	Private to ccmath.cpp. Primitive for operator overloading. This is
			much more efficient than calling Add with a 1 as a second parameter.
	Errors:		None (overflow ignored).

********************************************************************************************/

void XlongIncrement( xlong& input )
{
	input++ ;
}

/********************************************************************************************

>	void XlongDecrement( xlong& input )

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/93
	Scope:		Private to Xlong class
	Inputs:		64-bit signed xlong
	Outputs:	64-bit signed xlong
	Returns:	none
	Purpose:	Private to ccmath.cpp. Primitive for operator overloading. This is
			much more efficient than calling Sub with a 1 as a second parameter.
	Errors:		None (underflow ignored).

********************************************************************************************/

void XlongDecrement( xlong& input )
{
	input-- ;
}

/********************************************************************************************

>	void XDiv64( xlong& top, xlong& bottom, xlong& result )

>	void XDiv64One( xlong &bottom, xlong &result)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/93
	Scope:		Private to Xlong class
	Inputs:		two 64-bit signed xlong
	Outputs:	64-bit signed xlong
	Returns:	none
	Purpose:	Private to ccmath.cpp. Primitive for operator overloading.
	Errors:		None (underflow ignored). Div0 will take exception

********************************************************************************************/

void XDiv64( xlong& top, xlong& bottom, xlong& result )
{
	result = top/bottom ;
}

void XDiv64One( xlong &bottom, xlong &result)
{
	result /= bottom ;
}

/********************************************************************************************

>	void XlongToAscii( const xlong& arg, LPSTR dest )

>	void XlongToUnicode( const xlong& arg, LPWORD dest )

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	Scope:		Private to Xlong class
	Inputs:		64-bit signed xlong
	Outputs:	ASCII string, optional - sign then up to 20 digits, then null byte
	Returns:	none
	Purpose:	Display xlongs in meaningful format
	Errors:		None

********************************************************************************************/

void XlongToAscii( const xlong& arg, LPSTR dest )
{
#if defined(__WXMSW__)
	sprintf(dest,"%I64i",arg) ;
#else
	sprintf(dest,"%lld",arg) ;
#endif
}

void XlongToUnicode( const xlong& arg, WCHAR *dest )
{
#if defined(__WXMSW__)
	swprintf( dest, 21, L"%I64i", arg );
#else
	swprintf( dest, 21, L"%lld", arg );
#endif
}

/********************************************************************************************

>	Xlong XMatrixCalc( FIXED16 fix1, Xlong l1, FIXED16 fix2, Xlong l2, Xlong& result)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/93
	Scope:		Private to Xlong class
	Inputs:		two FIXED16s, two xlongs
	Outputs:	Result returns modified. (32-bit version is a macro).
	Returns:	Xlong result of fix1*l1 + fix2*l2
	Purpose:	Matrix multiply primitive done using intermediate 128-bit results
			so full accuracy is maintained. Special cases are when fix1 or fix2
			are 0.0 or 1.0 as multiplications may be avoided (NOT YET IMPLEMENTED)
			Note that the result is rounded to the nearest integer.
	Errors:		None (overflow ignored).

********************************************************************************************/

void XMatrixCalc( fixed16 fix1, const xlong& l1, fixed16 fix2, const xlong& l2, xlong& result )
{
	result = xlong(((double)l1*fix1.all+(double)l2*fix2.all)/(1<<16)) ;
}

/********************************************************************************************

>	void XlongByFixed16One( Xlong* x1, FIXED16 fix)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/93
	Inputs:		An Xlong (which will be modified) and a FIXED16.
	Outputs:	None
	Returns:	Xlong result of x1 * fix
	Purpose:	Multiplying without losing precision. Private function used by
			operator overloading on *=.
	Errors:		None (overflow ignored).
	Scope:		Private

********************************************************************************************/

void XlongByFixed16One( xlong* x1, fixed16 fix )
{
	*x1 = xlong( ( (double)*x1 * fix.all ) / ( 1 << 16 ) );
}

/********************************************************************************************

>	double XlongToDouble( const Xlong& x )

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/93
	Inputs:		An Xlong
	Outputs:	None
	Returns:	A double-precision value
	Purpose:	Conversion between types. Private to ccmaths.h which has a better
			interface on top of this.
	Errors:		None (overflow impossible, some precision may be lost)
	Scope:		Private

********************************************************************************************/

double XlongToDouble( const xlong& x )
{
	return (double)x;
}

/********************************************************************************************

>	void DoubleToXlong( const double, Xlong& )

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/93
	Inputs:		A double for conversion and a reference to an xlong (for the return
			value).
	Outputs:	Xlong reference is updated with new value
	Returns:	None
	Purpose:	Conversion between types. Private to ccmaths.h which has a better
			interface on top of this.
	Errors:		None (overflow ignored). No rounding. (I think).
	Scope:		Private

********************************************************************************************/

void DoubleToXlong( const double arg, xlong& x )
{
	x = (xlong)arg ;
}
