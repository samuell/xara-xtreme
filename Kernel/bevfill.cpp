// $Id: bevfill.cpp 662 2006-03-14 21:31:49Z alex $
//////////////////////////////////////////////////////
// bevel filling class implementation
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
#include "app.h"
#include "bevfill.h"

CC_IMPLEMENT_DYNAMIC(CBevelFills, CCObject)
CC_IMPLEMENT_DYNAMIC(CBevelFill, CCObject)
CC_IMPLEMENT_DYNAMIC(CBevelFillFlat, CBevelFill)
CC_IMPLEMENT_DYNAMIC(CBevelFillRound, CBevelFill)
CC_IMPLEMENT_DYNAMIC(CBevelFillSquare, CBevelFill)
CC_IMPLEMENT_DYNAMIC(CBevelFillFrame, CBevelFill)
CC_IMPLEMENT_DYNAMIC(CBevelFillSmooth, CBevelFill)
CC_IMPLEMENT_DYNAMIC(CBevelFillRuffle, CBevelFill)
CC_IMPLEMENT_DYNAMIC(CBevelFillPoint, CBevelFill)

#define new CAM_DEBUG_NEW

// the value to set to be transparent in some fills
const double SplitValue = 213;

void CBevelFill::EnsureValueRange(double *value)
{
	if (*value > 254)
	{
		*value = 254;
	}
	else if (*value < 1)
	{
		*value = 1;
	}
}

/////////////////////////////////////////////////////////
// the CBevFills class

CBevelFills::CBevelFills()
{
	SetUpFills();
}

CBevelFills::~CBevelFills()
{
	for (INT32 i = 0 ; i < m_NumFills; i++)
	{
		TRACE( _T("deleting bevel fill %d\n"), i);
		delete m_pBevFills[i];
	}

	delete m_pBevFills;
	m_pBevFills = NULL;
}

/********************************************************************************************

>	BOOL CBevelFills::SetUpFills()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/10/98
	Purpose:	Set up the bevel filling array
	Inputs:		
	Returns:	TRUE for success
	SeeAlso:	-

********************************************************************************************/

BOOL CBevelFills::SetUpFills()
{
	// set up the list first
	m_NumFills = 7;

	m_pBevFills = new CBevelFill *[m_NumFills];
	m_pBevFills[0] = new CBevelFillFlat;

	m_pBevFills[1] = new CBevelFillRound;

	m_pBevFills[2] = new CBevelFillSquare;

	m_pBevFills[3] = new CBevelFillFrame;

	m_pBevFills[4] = new CBevelFillSmooth;

	m_pBevFills[5] = new CBevelFillPoint;

	m_pBevFills[6] = new CBevelFillRuffle;

  	return TRUE;
}

COLORREF CBevelFills::CalcPixelValue(BYTE value, INT32 bevType, double Pos, INT32 Contrast)
{
	if (Pos < 0.00)
	{
		Pos = 0.00;
	}

	if (Pos >= 1.00)
	{
		Pos = 1.00;
	}

	if (bevType < 0 || bevType >= m_NumFills)
	{
		return m_pBevFills[0]->CalcPixelValue(value, Pos, Contrast);
	}
	
	return m_pBevFills[bevType]->CalcPixelValue(value, Pos, Contrast);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// all the fill class definitions

/********************************************************************************************

>	COLORREF CBevelFlatFill::CalcPixelValue(BYTE value, double Pos, INT32 Contrast);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/98
	Purpose:	Do a flat fill
	Inputs:		
	Returns:	TRUE for success
	SeeAlso:	-

********************************************************************************************/

COLORREF CBevelFillFlat::CalcPixelValue(BYTE value, double Pos, INT32 Contrast)
{
	double RValue = value;
	double GValue = value;
	double BValue = value;
	double AValue = value;

	if (AValue < SplitValue)
	{
		AValue *= 255.0 / SplitValue;
	}
	else 
	{
		AValue = 255.0 - AValue;
		AValue *= 255.0 / (255.0 - SplitValue);

		AValue = 1.0 - (AValue / 255.0);
		AValue *= AValue;
		AValue = (1.0 - AValue) * 255.0;
	}
	
	AValue *= 200.0 / 255.0;

	RValue += 0.5;
	GValue += 0.5;
	BValue += 0.5;

	EnsureValueRange(&AValue);
	
	BYTE RByte = (BYTE)RValue;
	BYTE GByte = (BYTE)GValue;
	BYTE BByte = (BYTE)BValue;
	BYTE AByte = (BYTE)AValue;

	return RByte | (GByte << 8) | (BByte << 16) | (AByte << 24);
}

/********************************************************************************************

>	COLORREF CBevelFillRound::CalcPixelValue(BYTE value, double Pos, INT32 Contrast);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/98
	Purpose:	Do a flat fill
	Inputs:		
	Returns:	TRUE for success
	SeeAlso:	-

********************************************************************************************/

COLORREF CBevelFillRound::CalcPixelValue(BYTE value, double Pos, INT32 Contrast)
{
	double RValue = value;
	double GValue = value;
	double BValue = value;
	double AValue = value;

	double Gradient = 0;


	if (Pos <= 0.5)
	{
		Pos = Pos / 0.5;

		Gradient = 128 - RValue;
		RValue = RValue + (Gradient * Pos);
	}	
	else
	{
		Pos = 1.0 - Pos;
		RValue = 255.0 - RValue;
		
		Pos = Pos / 0.5;

		Gradient = 128 - RValue;
		RValue = RValue + (Gradient * Pos);
	}		

	BYTE RByte = (BYTE)RValue;
	BYTE GByte = (BYTE)RValue;
	BYTE BByte = (BYTE)RValue;
	BYTE AByte = (BYTE)AValue;

	return RByte | (GByte << 8) | (BByte << 16) | (AByte << 24);
}
	
/********************************************************************************************

>	COLORREF CBevelFillSqaure::CalcPixelValue(BYTE value, double Pos, INT32 Contrast);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/98
	Purpose:	Do a flat fill
	Inputs:		
	Returns:	TRUE for success
	SeeAlso:	-

********************************************************************************************/

COLORREF CBevelFillSquare::CalcPixelValue(BYTE value, double Pos, INT32 Contrast)
{
double RValue = value;
	double GValue = value;
	double BValue = value;
	double AValue = value;

	double Gradient = 0;


	if (Pos <= 0.3)
	{
		// do nothing
	}
	else if (Pos >= 0.7)
	{
		RValue = 255.0 - RValue;
	}
	else
	{
		RValue = 128.0;
	}		

	BYTE RByte = (BYTE)RValue;
	BYTE GByte = (BYTE)RValue;
	BYTE BByte = (BYTE)RValue;
	BYTE AByte = (BYTE)AValue;

	return RByte | (GByte << 8) | (BByte << 16) | (AByte << 24);
}
	
/********************************************************************************************

>	COLORREF CBevelFillFrame::CalcPixelValue(BYTE value, double Pos, INT32 Contrast);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/98
	Purpose:	Do a Mesa fill
	Inputs:		
	Returns:	TRUE for success
	SeeAlso:	-

********************************************************************************************/

COLORREF CBevelFillFrame::CalcPixelValue(BYTE value, double Pos, INT32 Contrast)
{
	Pos = 1.0 - Pos;
	
	double RValue = value;
	double GValue = value;
	double BValue = value;
	double AValue = value;

	double kValue = value;

	// deal with the outside inwards
	if (value <= 128)
	{
		kValue = 128.0 - kValue;
		kValue *= (1.0 - Pos);
		kValue = 128.0 - kValue;
	}
	else
	{
		kValue -= 127.0;
		kValue *= (1.0 - Pos);
		kValue += 127.0;
	}

	RValue = kValue;
	GValue = kValue;
	BValue = kValue;
	AValue = kValue;
	
	BYTE RByte = (BYTE)RValue;
	BYTE GByte = (BYTE)GValue;
	BYTE BByte = (BYTE)BValue;
	BYTE AByte = (BYTE)AValue;

	return RByte | (GByte << 8) | (BByte << 16) | (AByte << 24);
}

/********************************************************************************************

>	COLORREF CBevelFillSmooth::CalcPixelValue(BYTE value, double Pos, INT32 Contrast);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/98
	Purpose:	Do a flat fill
	Inputs:		
	Returns:	TRUE for success
	SeeAlso:	-

********************************************************************************************/

COLORREF CBevelFillSmooth::CalcPixelValue(BYTE value, double Pos, INT32 Contrast)
{
	double RValue = value;
	double GValue = value;
	double BValue = value;
	double AValue = value;

	double kValue = value;

	// deal with the outside inwards
	if (value <= 128)
	{
		kValue = 128.0 - kValue;
		kValue *= (1.0 - Pos);
		kValue = 128.0 - kValue;
	}
	else
	{
		kValue -= 127.0;
		kValue *= (1.0 - Pos);
		kValue += 127.0;
	}

	RValue = kValue;
	GValue = kValue;
	BValue = kValue;
	AValue = kValue;
	
	BYTE RByte = (BYTE)RValue;
	BYTE GByte = (BYTE)GValue;
	BYTE BByte = (BYTE)BValue;
	BYTE AByte = (BYTE)AValue;

	return RByte | (GByte << 8) | (BByte << 16) | (AByte << 24);
}

/********************************************************************************************

>	COLORREF CBevelFillPoint::CalcPixelValue(BYTE value, double Pos, INT32 Contrast);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/98
	Purpose:	Do a flat fill
	Inputs:		
	Returns:	TRUE for success
	SeeAlso:	-

********************************************************************************************/

COLORREF CBevelFillPoint::CalcPixelValue(BYTE value, double Pos, INT32 Contrast)
{
	double RValue = value;
	double GValue = value;
	double BValue = value;
	double AValue = value;

	double kValue = value;

	if (Pos < 0.5)
	{
		// do nothing
	}
	else
	{
		kValue = 255.0 - kValue;
	}
	
	RValue = kValue;
	GValue = kValue;
	BValue = kValue;
	AValue = kValue;

	BYTE RByte = (BYTE)RValue;
	BYTE GByte = (BYTE)GValue;
	BYTE BByte = (BYTE)BValue;
	BYTE AByte = (BYTE)AValue;

	return RByte | (GByte << 8) | (BByte << 16) | (AByte << 24);
}

/********************************************************************************************

>	COLORREF CBevelFillRuffle::CalcPixelValue(BYTE value, double Pos, INT32 Contrast);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/98
	Purpose:	Do a ruffled fill
				This is the same as 3 rounded fills
	Inputs:		
	Returns:	TRUE for success
	SeeAlso:	-

********************************************************************************************/

COLORREF CBevelFillRuffle::CalcPixelValue(BYTE value, double Pos, INT32 Contrast)
{
	// change the position
	if (Pos >= 0 && Pos < 0.3)
	{
		Pos = Pos * (1.0/0.3);
	}
	else if (Pos >= 0.3 && Pos < 0.6)
	{
		Pos = (Pos - 0.3) * (1.0 / 0.3);
	}
	else 
	{
		Pos = (Pos - 0.6) * (1.0 / 0.4);
	}
	
	double RValue = value;
	double GValue = value;
	double BValue = value;
	double AValue = value;

	double kValue = value;

	if (Pos < 0.5)
	{
		// do nothing
	}
	else
	{
		kValue = 255.0 - kValue;
	}
	
	RValue = kValue;
	GValue = kValue;
	BValue = kValue;
	AValue = kValue;

	BYTE RByte = (BYTE)RValue;
	BYTE GByte = (BYTE)GValue;
	BYTE BByte = (BYTE)BValue;
	BYTE AByte = (BYTE)AValue;

	return RByte | (GByte << 8) | (BByte << 16) | (AByte << 24);
}
