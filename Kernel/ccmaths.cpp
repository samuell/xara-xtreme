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
//#include "fixed.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "xlong.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ccmaths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fixed16.h" - in camtypes.h [AUTOMATICALLY REMOVED]

DECLARE_SOURCE("$Revision$");


/********************************************************************************************

>	XLONG Mul(const INT32 operand1, const INT32 operand2) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/93
	Inputs:		Two INT32 values
	SeeAlso:	Xlongasm.h
	Outputs:	None
	Returns:	A reference to an xlong with result of multiplication.
	Purpose:	Multiplication of two 32-bit signed numbers (xlongs). 
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

XLONG Mul(const INT32 operand1, const INT32 operand2)
{
	XLONG result;

	XMul32(operand1, operand2, result);

	return result;
}

/********************************************************************************************

>	XLONG Asl (const INT32, UINT32)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	Inputs:		INT32 - value to be shifted
				UINT32 - shift count
	Outputs:	None
	Returns:	XLONG - shifted number. 
	Purpose:	Performs arithmetic shift left. 
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

	Using the ASL provided with xlong for now.

********************************************************************************************/

XLONG Asl (const INT32 value, UINT32 shift)
{   
	XLONG result;
	
	result = MakeXLong(value);
	
	return 	(result << shift);

}

/********************************************************************************************

>	XLONG Asr (const INT32, UINT32)

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	Inputs:		INT32 - value to be shifted
				UINT32 - shift count
	Outputs:	None
	Returns:	XLONG - shifted number. 
	Purpose:	Performs arithmetic shift right. 
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

	Using the ASR provided with xlong for now.

********************************************************************************************/

XLONG Asr (const INT32 value, UINT32 shift)
{   
	return ( XLONG(value) >> shift);
}

/********************************************************************************************

>	INT32 MulDiv32By32(const INT32 operand1, const INT32 operand2, const INT32 operand3) 

	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
	Inputs:		operand1 - multiplicand 1
				operand2 - multiplicand 2
				operand3 - divisor
	Outputs:	None
	Returns:	INT32 -  result of multiply-divide. 
	Purpose:	Multiplies two 32-bit values and divides the result by another 32-bit value. 
	Errors:		None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

INT32 MulDiv32By32(const INT32 operand1, const INT32 operand2, const INT32 operand3)  
{
	return Mul32Div32( operand1, operand2, operand3 );
}

/**********************************************************************************************

>	XLONG MakeXLong (const INT32 operand)
 	
	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
	Inputs:		Long value to be converted.
	Outputs:	None.
	Returns:	an xlong value.
	Purpose:	To convert an INT32 value to an xlong. 
	Errors:		None.

**********************************************************************************************/
/*
Technical notes:

**********************************************************************************************/
  								
XLONG MakeXLong (const INT32 operand)
{
	return XLONG(operand);
}

/**********************************************************************************************

>	XLONG MakeXLong (const double operand)
 	
	Author: 	Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/93
	SeeAlso:	Xlongasm.h
	Inputs:		double value to be converted.
	Outputs:	None.
	Returns:	an xlong value.
	Purpose:	To convert a double value to an xlong. 
	Errors:		None.

**********************************************************************************************/
/*
Technical notes:

**********************************************************************************************/
  								
XLONG MakeXLong(const double operand)
{
	xlong result;

	DoubleToXlong(operand, result);

	return result;
}
/********************************************************************************************

>	FIXED16 Div32By32( INT32 arg1, INT32 arg2 )

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Inputs:		two signed 32-bit numbers
	Outputs:	None
	Returns:	FIXED16 result of arg1/arg2
	Scope:		Private to the CCMATHS library.
	Purpose:	Obtain a fixed-point representation of the ratio between two signed
			numbers.
	Errors:		None. Div0 and overflow will create a div0 exception.
	See Also:	InternalDiv32by32

This fn is inline.

********************************************************************************************/

/********************************************************

< Maths Types
< Maths Conversion Table
< Conversion

comment:
\pard\plain
\trowd \trgaph108\trleft-108 
\cellx1593\cellx3294\cellx4995\cellx6696\cellx8397\cellx10098\pard\plain 
\qc \intbl \fs20\lang2057 {\b From}{\b  }{\b \\}{\b  }{\b To}{\b \cell }{\b INT32}{\b \cell }
{\b INT32}{\b \cell }{\b XLONG}{\b \cell }{\b FIXED16}{\b \cell }{\b double}{\b \cell }\row 
\qc\intbl {\b INT32}{\b \cell }-\cell auto
\cell constructor
\par operator=\cell constructor
\par operator=\cell auto\cell \pard \intbl \row \pard \qc\intbl {\b INT32}{\b \cell }
built-in cast\cell -\cell MakeXlong()
\par constructor
\par operator=
\par cast operator\cell operator=
\par constructor\cell auto\cell \pard \intbl \row \pard \qc\intbl {\b XLONG}{\b \cell }
MakeInt()\cell MakeLong()\cell -\cell MakeFixed16()\cell MakeDouble()\cell \pard 
\intbl \row \pard \qc\intbl {\b FIXED16}{\b \cell }MakeInt()\cell MakeLong()\cell 
MakeXlong()\cell -\cell MakeDouble()\cell \row \qc\intbl {\b double}
{\b \cell }explicit cast\cell explicit cast\cell MakeXlong()\cell operator=
\par constructor\cell -\cell \pard \intbl \row \pard 

******************************************************************/
