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


/*
*/ 

// Stardate 070494.10:25. Captain's Log - supplementary
// ====================================================
//
// 		Pirated fixed16 code to base fixed24 upon
// 		This was from revision 1.24 of fixed16.cpp
//												  Jason



#include "camtypes.h"
#include "fixed24.h"
#include "fix24.h"



/********************************************************************************************

>	FIXED24 Fixed24Mul(FIXED24 arg1, FIXED24 arg2)

	Author: 	Gavin_Theobald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/2005
	Inputs:		two fixed24s
	Outputs:	None
	Returns:	FIXED24 result (arg1 * arg2)
	Scope:		Only to be used in the FIXED24 class.
	Purpose:	signed multiplication
	Errors:		None (no overflow checks). Rounds

********************************************************************************************/

fixed24 Fixed24Mul( fixed24 a, fixed24 b )
{
	fixed24				result;
	result.all = INT32((Int32x32To64(a.all,b.all)+0x800000)>>24) ;
	return result;
}

/********************************************************************************************

>	FIXED24 Fixed24Div(FIXED24 arg1, FIXED24 arg2)

	Author: 	Gavin_Theobald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/2005
	Inputs:		two fixed24s
	Outputs:	None
	Returns:	FIXED24 result (arg1 / arg2)
	Scope:		Only to be used in the FIXED24 class.
	Purpose:	signed division
	Errors:		None (no overflow checks). No rounding either (should it?)
			Overflow will cause a DIV0 exception, as will divide by zero

Note that this is not very efficient as it forces a multiply that
could instead be performed by a shift. The alternative is to use
int64s but this would then compile as a 64bit/64bit division which
is worse.

Why isn't there a Int64div32to32 function?

********************************************************************************************/

fixed24 Fixed24Div(fixed24 a,fixed24 b)
{
	fixed24 result ;
	result.all = MulDiv(a.all,1<<24,b.all) ;
	return result ;
}

/********************************************************************************************

>	double Fixed24ToDouble( FIXED24 arg )

	Author: 	Gavin_Theobald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/2005
	Inputs:		FIXED24 argument
	Outputs:	None
	Returns:	double-precision value
	Scope:		Only to be used in the FIXED24 class.
	Purpose:	General type conversion
	Errors:		None

This code relies on the compiler efficiently replacing the division by a multiplication
of the reciprical.

********************************************************************************************/

double Fixed24ToDouble( fixed24 a )
{
	return (double)a.all/(1<<24) ;
}

/********************************************************************************************

>	fixed24 DoubleToFixed24( double arg )

	Author: 	Gavin_Theobald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/2005
	Inputs:		double-precision argument
	Outputs:	None
	Returns:	FIXED24 value
	Purpose:	General type conversion
	Errors:		None (overflow will cause exception)

********************************************************************************************/

fixed24 DoubleToFixed24( double arg )
{
	fixed24 result ;
#if 0 //defined(_M_IX86)
	//
	// This avoids the slow fix operation. Add a large number so that
	// the double always has the same exponent and the mantissa becomes,
	// in affect, a fixed point value with the value we want in the
	// bottom 32 bits.
	//
	// Note that we are relying on the compiler to convert this to
	// efficient code.
	//
	// TODO: Test!
	//
	static const double fFix = (XLONG)3<<(DBL_MANT_DIG-24-2);
	double F = arg+fFix;
	result.all = (INT32&)F ;
#else
	result.all = INT32(arg*(1<<24)) ;
#endif
	return result ;
}
