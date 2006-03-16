// $Id: fixed24.h 662 2006-03-14 21:31:49Z alex $
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
*/ 

// Stardate 070494.10:25. Captain's Log - supplementary
// ====================================================
//
// 		Pirated fixed16 code to base fixed24 upon
// 		This was from revision 1.27 of fixed16.h
//												  Jason


/*********************************************************************************************

>	class FIXED24

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com> (Hijacked by Jason)
	Created:	26/4/1993 (07/04/94)
	SeeAlso:	CCMaths.h
	SeeAlso:	Fixedasm.h
	SeeAlso:	Div32By32
	SeeAlso:	Keyword FIXED24
	SeeAlso:	Keyword FIXED16
	Purpose:	
		A FIXED24 is a signed 32-bit value with a binary point between bits 23 and 24
		Its purpose is to represent floating point numbers in 32 bits.
		The implementation is very similar to FIXED16, but note that many methods are
		currently not defined in FIXED24, as it is used only in the colour system, which
		does not need large chunks of the FIXED16 functionality.
		No-one should include fixed24.h directly - always use ccmath.h. You should also
		never use the lower-case type 'fixed24' - always used 'FIXED24'.    

		Operations from FIXED16 that are not available in FIXED24 are currently removed
		by the use of #ifdef FULL_FIXED24 directives. (if there is no help on a function,
		then it is currently unsupported in FIXED24)

    Errors:		None.
 	
********************************************************************************************/

#ifndef INC_FIXED24
#define INC_FIXED24

#if !defined(USE_NATIVE_XLONG)
class CCAPI xlong;
#endif

#ifdef FULL_FIXED24
class CCAPI String;
#endif

class CCAPI fixed24
{
public:
	INT32 all;

public:

	#define	F24SHIFT (24)								// marks binary point position

	// this converts a short into a fixed24 longword by casting to an INT32 then shifting it
	#define	SHORT_FIXED24(x)	( ((INT32)(x)) << F24SHIFT )

	inline fixed24() {};
	inline fixed24(const fixed24&);
	inline fixed24(INT32);
	inline fixed24(short);
	inline fixed24(double);

#ifdef FULL_FIXED24
	fixed24& Sin();
	fixed24& Cos();
    
    void ToString(String* out) const;
#endif


	// Friend Functions

	friend fixed24 CCAPI operator+ (const fixed24&, const fixed24&);
	friend fixed24 CCAPI operator- (const fixed24&, const fixed24&);
	friend fixed24 CCAPI operator- (const fixed24&);
	friend fixed24 CCAPI operator* (const fixed24&, const fixed24&);
	friend fixed24 CCAPI operator/ (const fixed24&, const fixed24&);
	friend fixed24 CCAPI operator>> (const fixed24&, UINT32);
	friend fixed24 CCAPI operator<< (const fixed24&, UINT32);

	// Relational Operators
    
    friend INT32 CCAPI operator== (const fixed24&, const fixed24&);
    friend INT32 CCAPI operator!= (const fixed24&, const fixed24&);
    friend INT32 CCAPI operator> (const fixed24&, const fixed24&);
    friend INT32 CCAPI operator< (const fixed24&, const fixed24&);
    friend INT32 CCAPI operator>= (const fixed24&, const fixed24&);
    friend INT32 CCAPI operator<= (const fixed24&, const fixed24&);

    friend INT32 CCAPI operator== (const fixed24&, const short);
    friend INT32 CCAPI operator!= (const fixed24&, const short);
    friend INT32 CCAPI operator> (const fixed24&, const short);
    friend INT32 CCAPI operator< (const fixed24&, const short);
    friend INT32 CCAPI operator>= (const fixed24&, const short);
    friend INT32 CCAPI operator<= (const fixed24&, const short);
    
	// Assignment Operators

	inline fixed24& operator= (const fixed24&);
	fixed24& operator= (const INT32);
	inline fixed24& operator= (const short);
	fixed24& operator= (const double);
	fixed24& operator+= (const fixed24&);
	fixed24& operator+= (const short);
	fixed24& operator-= (const fixed24&); 
	fixed24& operator-= (const short);
	fixed24& operator*= (const fixed24&); 
	fixed24& operator*= (const short);
	fixed24& operator/= (const fixed24&);
	fixed24& operator/= (const short); 
	fixed24& operator<<= (const UINT32); 
	fixed24& operator>>= (const UINT32); 


	// Increment\Decrement Operators

	fixed24& operator++ ();							// prefix
	fixed24 operator++ (INT32);						// postfix 
	fixed24& operator-- ();							// prefix 
	fixed24 operator-- (INT32);						// postfix 

	// Conversion functions

	inline INT32 	 MakeInt() const;
	inline short MakeShort() const;
	inline double MakeDouble() const;
	inline INT32  MakeLong() const;
	inline xlong MakeXlong() const;

	inline static fixed24 FromRawLong(INT32 d);		// DO NOT USE!

	inline INT32 GetAsFixed24(void);
	inline void SetAsFixed24(INT32 NewValue);
};


typedef fixed24 FIXED24;


#include "fix24.h"

/********************************************************************************************

>	FIXED24_DBL(d) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/93
	Inputs:		double.
	Outputs:	None
	Returns:	FIXED24.
	Purpose:	
		This macro converts constant doubles into FIXED24s at compile time. It is 
		very effecient because there is no runtime overhead unlike normal conversion.  
		
	Errors:		None.

********************************************************************************************/

#define FIXED24_DBL(d) fixed24::FromRawLong((INT32)((d)*(1<<24)))

/********************************************************************************************

>	inline FIXED24::FIXED24 () 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	Default constructor for FIXED24. It does not actually do anything!
	Errors:		None.

********************************************************************************************/

// actual (null) function can be found in class

/********************************************************************************************

>	inline FIXED24::FIXED24 (const FIXED24&) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	Copy constructor.
	Errors:		None.

********************************************************************************************/

inline fixed24::fixed24(const fixed24& initValue) 
{
	all = initValue.all;
}

/********************************************************************************************

>	inline FIXED24::FIXED24 (INT32) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	Constructor.
	Errors:		None.

********************************************************************************************/

inline fixed24::fixed24(INT32 initValue) 
{
	all = SHORT_FIXED24( initValue );
}

/********************************************************************************************

>	inline FIXED24::FIXED24 (short) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	Constructor.
	Errors:		None.

********************************************************************************************/

inline fixed24::fixed24(short initValue) 
{
	all = SHORT_FIXED24( initValue );
}

/********************************************************************************************

>	inline FIXED24::FIXED24 (double) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93
	SeeAlso:	Fixedasm.h
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	Constructor.
	Errors:		None.

********************************************************************************************/

inline fixed24::fixed24(double initValue) 
{
	*this = DoubleToFixed24(initValue);
}

/********************************************************************************************

>	inline FIXED24 FIXED24::FromRawLong(INT32 d) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93
	Inputs:		An INT32
	Outputs:	A Fixed24
	Returns:	None.
	Scope:		Private - for FIXED_DBL macro
				If you wish to access the private data in a FIXED24, then please
				use GetAsFixed24, which is an interface inline function which
				will always present the data to the outside world in 8.24 bit
				format, even if we decide to change the internal format one day.
	Purpose:	converts an INT32 to a FIXED24.
	Errors:		None.

********************************************************************************************/

inline fixed24 fixed24::FromRawLong(INT32 d)
{
	fixed24 temp;

	temp.all = d;
	return temp;
}



/********************************************************************************************

>	inline INT32 FIXED24::GetAsFixed24(void) 

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		None
	Outputs:	None
	Returns:	A fixed point value stored as 8.24 bits in a INT32
	Scope:		Sortof private for use in the DocColour system
	Purpose:	converts a FIXED24 to an 8.24 bit fixed point value stored in
				a INT32. (i.e. pulls the 8.24 data word directly out of the FIXED24,
				though obviously if the storage model in a fixed24 changes in the future
				we can change this access function to still return a 8.24 INT32)
	Errors:		None.

********************************************************************************************/

inline INT32 fixed24::GetAsFixed24(void)
{
	return((INT32) all);
}


/********************************************************************************************

>	inline INT32 FIXED24::SetAsFixed24(void) 

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		None
	Outputs:	None
	Returns:	None
	Scope:		Sortof private for use in the DocColour system
	Purpose:	Sets a FIXED24 to an 8.24 bit fixed point value stored in a INT32.
				(i.e. plonks the 8.24 data word directly into the FIXED24, though
				obviously if the storage model in a fixed24 changes in the future
				we can change this access function to still accept a 8.24 INT32)
	Errors:		None.

********************************************************************************************/

inline void fixed24::SetAsFixed24(INT32 NewValue)
{
	all = NewValue;
}



/********************************************************************************************

>	inline INT32 FIXED24::MakeInt() const 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	converts a FIXED24 to an integer.
	Errors:		None.

********************************************************************************************/

inline INT32 fixed24::MakeInt() const
{
	return (INT32) (all >> F24SHIFT);
}

/********************************************************************************************

>	inline short FIXED24::MakeShort() const 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93
	SeeAlso:	Fixedasm.h
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	converts a FIXED24 to an integer.
	Errors:		None.

********************************************************************************************/

inline short fixed24::MakeShort() const
{
	return (short) (all >> F24SHIFT);
}

/********************************************************************************************

>	inline INT32 FIXED24::MakeLong() const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	converts a FIXED24 to a INT32.
	Errors:		None.

********************************************************************************************/

inline INT32 fixed24::MakeLong() const
{
	return all >> F24SHIFT;
}

/********************************************************************************************

>	inline XLONG FIXED24::MakeXlong() const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/93
	Inputs:		None
	Outputs:	None
	Returns:	None.
	Purpose:	converts a FIXED24 to a INT32.
	Errors:		None.

********************************************************************************************/

inline xlong fixed24::MakeXlong() const
{
    return xlong( (short) (all >> F24SHIFT) );
}

/********************************************************************************************

>	inline FIXED24& operator= (const FIXED24& operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	Inputs:		operand to be  assigned - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED24 with result of assignment.
	Purpose:	Overloading the assignment operator for FIXED24s.
	Errors:		None.

********************************************************************************************/

inline fixed24& fixed24::operator= (const fixed24& operand)
{ 
	this->all = operand.all;
	
	return *this; 
}
/********************************************************************************************

>	inline double FIXED24::MakeDouble() const

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/93
	SeeAlso:	fixedasm.h
	Inputs:		None
	Outputs:	None
	Returns:    double. 
	Purpose:	Makes a double (32-bit signed value) out of a fixed point 24-bit value.
	Errors:		None.

********************************************************************************************/

inline double fixed24::MakeDouble() const
{
	return Fixed24ToDouble(*this);
}

/********************************************************************************************

>	inline FIXED24& operator= (const short operand) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/93
	SeeAlso:	Fixedasm.h
	Inputs:		operand to be  assigned - rhs of assignment.
	Outputs:	None.
	Returns:	A reference to an FIXED24 with result of assignment.
	Purpose:	Overloading the assignment operator for FIXED24s.
	Errors:		None.

********************************************************************************************/

inline fixed24& fixed24::operator= (const short operand)
{ 
	this->all = SHORT_FIXED24( operand );
	return *this;
}




#endif
