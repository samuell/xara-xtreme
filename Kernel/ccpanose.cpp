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
/********************************************************************************************

>	CCPanose.cpp

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/06
	SeeAlso:	<???>

********************************************************************************************/

#include "camtypes.h"
#include "ccpanose.h"

CC_IMPLEMENT_DYNCREATE( CCPanose, CCObject )

/********************************************************************************************

>	CCPanose::CCPanose()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/96
	Inputs:		-
	Returns:	-
	Purpose:	Default constructor for class CCPanose

********************************************************************************************/

CCPanose::CCPanose()
{
	SetAllToAny();
}

/********************************************************************************************

>	CCPanose::CCPanose(const PANOSE &PanoseNumber)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		PanoseNumber	- panose number to copy
	Returns:	-
	Purpose:	Constructor

********************************************************************************************/

CCPanose::CCPanose(const PANOSE &PanoseNumber)
{
	mFamilyType			= PanoseNumber.bFamilyType;
    mSerifStyle			= PanoseNumber.bSerifStyle; 
    mWeight				= PanoseNumber.bWeight; 
    mProportion			= PanoseNumber.bProportion; 
    mContrast			= PanoseNumber.bContrast;
    mStrokeVariation	= PanoseNumber.bStrokeVariation; 
    mArmStyle			= PanoseNumber.bArmStyle; 
    mLetterform			= PanoseNumber.bLetterform; 
    mMidline			= PanoseNumber.bMidline; 
    mXHeight			= PanoseNumber.bXHeight; 	
}

/********************************************************************************************

>	void CCPanose::SetAllToAny()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/07/96
	Inputs:		-
	Returns:	-
	Purpose:	Sets all 10 components of the Panose number to PAN_ANY

********************************************************************************************/

void CCPanose::SetAllToAny()
{
	SetAll(PAN_ANY, PAN_ANY, PAN_ANY, PAN_ANY, PAN_ANY, PAN_ANY, PAN_ANY, PAN_ANY, PAN_ANY, PAN_ANY);
}

/********************************************************************************************

>	void CCPanose::SetAll(
			BYTE FamilyType,
			BYTE SerifStyle,
			BYTE Weight,
			BYTE Proportion,
			BYTE Contrast,
			BYTE StrokeVariation,
			BYTE ArmStyle,
			BYTE LetterForm,
			BYTE Midline,
			BYTE XHeight);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/07/96
	Inputs:		Obvious really - the 10 components of the Panose number
	Returns:	-
	Purpose:	Sets the components of the Panose number to those given

********************************************************************************************/

void CCPanose::SetAll(
		BYTE FamilyType,
		BYTE SerifStyle,
		BYTE Weight,
		BYTE Proportion,
		BYTE Contrast,
		BYTE StrokeVariation,
		BYTE ArmStyle,
		BYTE LetterForm,
		BYTE Midline,
		BYTE XHeight)
{
	mFamilyType			= FamilyType;
    mSerifStyle			= SerifStyle; 
    mWeight				= Weight; 
    mProportion			= Proportion; 
    mContrast			= Contrast;
    mStrokeVariation	= StrokeVariation; 
    mArmStyle			= ArmStyle; 
    mLetterform			= LetterForm; 
    mMidline			= Midline; 
    mXHeight			= XHeight; 
}

/********************************************************************************************

>	void CCPanose::operator=(const CCPanose & x)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/07/96
	Inputs:		x - another CCPanose object to copy
	Returns:	-
	Purpose:	Copies a CCPanose object

********************************************************************************************/

void CCPanose::operator=(const CCPanose & x)
{
	this->mFamilyType = x.mFamilyType;
	this->mSerifStyle = x.mSerifStyle; 
 	this->mWeight = x.mWeight; 
	this->mProportion = x.mProportion; 
	this->mContrast = x.mContrast; 
	this->mStrokeVariation = x.mStrokeVariation; 
	this->mArmStyle = x.mArmStyle; 
	this->mLetterform = x.mLetterform; 
	this->mMidline = x.mMidline; 
	this->mXHeight = x.mXHeight; 
}

/********************************************************************************************

>	BOOL CCPanose::BodgeCCPanose(CCPanose *pCCPanose, BOOL Bold, BOOL Italic)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/96
	Inputs:		Bold	- Bold flag to bodge in
				Italic	- Italic flag to bodge in
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Bodges the bold and italic flags into the Weight and Letterform components.

********************************************************************************************/

BOOL CCPanose::BodgePanoseNumber(BOOL IsBold, BOOL IsItalic)
{
	if (mFamilyType != PAN_FAMILY_PICTORIAL)
	{
		// only do this if we're not a symbol font

		if (IsBold==FALSE)
		{
			// regular font
			if (mWeight==PAN_ANY || mWeight==PAN_NO_FIT || mWeight>PAN_WEIGHT_MEDIUM)
			{
				mWeight=PAN_WEIGHT_MEDIUM;
			}
		}
		else
		{
			// bold font
			if (mWeight==PAN_ANY || mWeight==PAN_NO_FIT || mWeight<PAN_WEIGHT_BOLD)
			{
				mWeight=PAN_WEIGHT_BOLD;
			}
		}

		if (IsItalic==FALSE)
		{
			// upright font
			if (mLetterform==PAN_ANY || mLetterform==PAN_NO_FIT)
			{
				mLetterform=PAN_LETT_NORMAL_CONTACT;
			}
			else if (mLetterform>=PAN_LETT_OBLIQUE_CONTACT && mLetterform<=PAN_LETT_OBLIQUE_SQUARE)
			{
				mLetterform=mLetterform-7;		// !!HACK!!
			}
		}
		else
		{
			// italic font
			if (mLetterform==PAN_ANY || mLetterform==PAN_NO_FIT)
			{
				mLetterform=PAN_LETT_OBLIQUE_CONTACT;
			}
			else if (mLetterform>=PAN_LETT_NORMAL_CONTACT && mLetterform<=PAN_LETT_NORMAL_SQUARE)
			{
				mLetterform=mLetterform+7;		// !!HACK!!
			}
		}

	}
	
	return TRUE;	// we have succeeded, haven't we?
}

/********************************************************************************************

>	BOOL CCPanose::BodgeToNormal()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/08/96
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Bodges the Panose number to normalise it, i.e. regular weight, non-italic.
	See Also:	CCPanose::BodgePanoseNumber

********************************************************************************************/

BOOL CCPanose::BodgeToNormal()
{
	return BodgePanoseNumber(FALSE, FALSE);
}

/********************************************************************************************

>	UINT32 CCPanose::Distance(const CCPanose &x, const CCPanose &y)
	UINT32 CCPanose::DistanceFamilyType(BYTE x, BYTE y);
	UINT32 CCPanose::DistanceSerifStyle(BYTE x, BYTE y);
	UINT32 CCPanose::DistanceWeight(BYTE x, BYTE y);
	UINT32 CCPanose::DistanceProportion(BYTE x, BYTE y);
	UINT32 CCPanose::DistanceContrast(BYTE x, BYTE y);
	UINT32 CCPanose::DistanceStrokeVariation(BYTE x, BYTE y);
	UINT32 CCPanose::DistanceArmStyle(BYTE x, BYTE y);
	UINT32 CCPanose::DistanceLetterform(BYTE x, BYTE y);
	UINT32 CCPanose::DistanceMidline(BYTE x, BYTE y);
	UINT32 CCPanose::DistanceXHeighte(BYTE x, BYTE y);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/08/96
	Inputs:		x, y	- CCPanose objects to determine the distance between
	Returns:	distance between x and y, as a CCPanoseDistance object
	Purpose:	Calculates the distance between x and y
	See Also:	class CCPanose

********************************************************************************************/

UINT32 CCPanose::Distance(const CCPanose &x, const CCPanose &y)
{
	UINT32 Distance = 0;

	Distance += DistanceFamilyType(x.mFamilyType, y.mFamilyType);
	Distance += DistanceSerifStyle(x.mSerifStyle, y.mSerifStyle);
	Distance += DistanceWeight(x.mWeight, y.mWeight);
	Distance += DistanceProportion(x.mProportion, y.mProportion);
	Distance += DistanceContrast(x.mContrast, y.mContrast);
	Distance += DistanceStrokeVariation(x.mStrokeVariation, y.mStrokeVariation);
	Distance += DistanceArmStyle(x.mArmStyle, y.mArmStyle);
	Distance += DistanceLetterform(x.mLetterform, y.mLetterform);
	Distance += DistanceMidline(x.mMidline, y.mMidline);
	Distance += DistanceXHeight(x.mXHeight, y.mXHeight);
	
	return Distance;
}

UINT32 CCPanose::DistanceFamilyType(BYTE x, BYTE y)
{
	//	Specifies the font family.
	//	
	//	Value	Meaning
	//	
	//		0	Any
	//		1	No fit
	//		2	Text and display
	//		3	Script
	//		4	Decorative
	//		5	Pictorial
	//	
	//	High weightings, since we _really_ want a font from the same family.
	
	const static UINT32 Metric[6][6] = {
		{0x00010000, 0x00100000, 0x00001000, 0x00001000, 0x00001000, 0x00001000},
		{0x00100000, 0x10000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000},
		{0x00001000, 0x01000000, 0x00000000, 0x000000ff, 0x000000ff, 0x000000ff},
		{0x00001000, 0x01000000, 0x000000ff, 0x00000000, 0x000000ff, 0x000000ff},
		{0x00001000, 0x01000000, 0x000000ff, 0x000000ff, 0x00000000, 0x000000ff},
		{0x00001000, 0x01000000, 0x000000ff, 0x000000ff, 0x000000ff, 0x00000000}};

	if ( x>5) x = 1;
	if ( y>5) y = 1;

	return Metric[x][y];
}

UINT32 CCPanose::DistanceSerifStyle(BYTE x, BYTE y)
{
	const static UINT32 Metric[16][16] = {
		{0x00010000, 0x00100000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000},
		{0x00100000, 0x10000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000},
		{0x00001000, 0x01000000, 0x00000000, 0x00000004, 0x00000004, 0x00000004, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000010},
		{0x00001000, 0x01000000, 0x00000004, 0x00000000, 0x00000004, 0x00000004, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000010},
		{0x00001000, 0x01000000, 0x00000004, 0x00000004, 0x00000000, 0x00000004, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000010},
		{0x00001000, 0x01000000, 0x00000004, 0x00000004, 0x00000004, 0x00000000, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000010},
		{0x00001000, 0x01000000, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000040, 0x00000040, 0x00000040, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000020, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000040, 0x00000040, 0x00000040, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000020, 0x00000020, 0x00000000, 0x00000020, 0x00000020, 0x00000040, 0x00000040, 0x00000040, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000020, 0x00000020, 0x00000020, 0x00000000, 0x00000020, 0x00000040, 0x00000040, 0x00000040, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000000, 0x00000040, 0x00000040, 0x00000040, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000000, 0x00000010, 0x00000010, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000000, 0x00000010, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000010, 0x00000000, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000040, 0x00000040, 0x00000040, 0x00000000, 0x00000020},
		{0x00001000, 0x01000000, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000040, 0x00000040, 0x00000040, 0x00000020, 0x00000000}};

	if ( x>15) x = 1;
	if ( y>15) y = 1;

	return Metric[x][y];
}

UINT32 CCPanose::DistanceWeight(BYTE x, BYTE y)
{
	const static UINT32 Metric[12][12] = {
		{0x00010000, 0x00100000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000},
		{0x00100000, 0x10000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000},
		{0x00001000, 0x01000000, 0x00000000, 0x00000001, 0x00000004, 0x00000009, 0x00000010, 0x00000019, 0x00000024, 0x00000031, 0x00000040, 0x00000051},
		{0x00001000, 0x01000000, 0x00000001, 0x00000000, 0x00000001, 0x00000004, 0x00000009, 0x00000010, 0x00000019, 0x00000024, 0x00000031, 0x00000040},
		{0x00001000, 0x01000000, 0x00000004, 0x00000001, 0x00000000, 0x00000001, 0x00000004, 0x00000009, 0x00000010, 0x00000019, 0x00000024, 0x00000031},
		{0x00001000, 0x01000000, 0x00000009, 0x00000004, 0x00000001, 0x00000000, 0x00000001, 0x00000004, 0x00000009, 0x00000010, 0x00000019, 0x00000024},
		{0x00001000, 0x01000000, 0x00000010, 0x00000009, 0x00000004, 0x00000001, 0x00000000, 0x00000001, 0x00000004, 0x00000009, 0x00000010, 0x00000019},
		{0x00001000, 0x01000000, 0x00000019, 0x00000010, 0x00000009, 0x00000004, 0x00000001, 0x00000000, 0x00000001, 0x00000004, 0x00000009, 0x00000010},
		{0x00001000, 0x01000000, 0x00000024, 0x00000019, 0x00000010, 0x00000009, 0x00000004, 0x00000001, 0x00000000, 0x00000001, 0x00000004, 0x00000009},
		{0x00001000, 0x01000000, 0x00000031, 0x00000024, 0x00000019, 0x00000010, 0x00000009, 0x00000004, 0x00000001, 0x00000000, 0x00000001, 0x00000004},
		{0x00001000, 0x01000000, 0x00000040, 0x00000031, 0x00000024, 0x00000019, 0x00000010, 0x00000009, 0x00000004, 0x00000001, 0x00000000, 0x00000001},
		{0x00001000, 0x01000000, 0x00000051, 0x00000040, 0x00000031, 0x00000024, 0x00000019, 0x00000010, 0x00000009, 0x00000004, 0x00000001, 0x00000000}};

	if ( x>11) x = 1;
	if ( y>11) y = 1;

	return Metric[x][y];
}

UINT32 CCPanose::DistanceProportion(BYTE x, BYTE y)
{
	const static UINT32 Metric[10][10] = {
		{0x00010000, 0x00100000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000},
		{0x00100000, 0x10000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000},
		{0x00001000, 0x01000000, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000020, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000020, 0x00000000, 0x00000040, 0x00000004, 0x00000080, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000020, 0x00000040, 0x00000000, 0x00000080, 0x00000004, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000020, 0x00000004, 0x00000080, 0x00000000, 0x000000c0, 0x00000080},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000020, 0x00000080, 0x00000004, 0x000000c0, 0x00000000, 0x00000080},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000020, 0x00000040, 0x00000040, 0x00000080, 0x00000080, 0x00000000}};

	if ( x>9) x = 1;
	if ( y>9) y = 1;

	return Metric[x][y];
}

UINT32 CCPanose::DistanceContrast(BYTE x, BYTE y)
{
	const static UINT32 Metric[10][10] = {
		{0x00010000, 0x00100000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000},
		{0x00100000, 0x10000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000},
		{0x00001000, 0x01000000, 0x00000000, 0x00000001, 0x00000004, 0x00000009, 0x00000010, 0x00000019, 0x00000024, 0x00000031},
		{0x00001000, 0x01000000, 0x00000001, 0x00000000, 0x00000001, 0x00000004, 0x00000009, 0x00000010, 0x00000019, 0x00000024},
		{0x00001000, 0x01000000, 0x00000004, 0x00000001, 0x00000000, 0x00000001, 0x00000004, 0x00000009, 0x00000010, 0x00000019},
		{0x00001000, 0x01000000, 0x00000009, 0x00000004, 0x00000001, 0x00000000, 0x00000001, 0x00000004, 0x00000009, 0x00000010},
		{0x00001000, 0x01000000, 0x00000010, 0x00000009, 0x00000004, 0x00000001, 0x00000000, 0x00000001, 0x00000004, 0x00000009},
		{0x00001000, 0x01000000, 0x00000019, 0x00000010, 0x00000009, 0x00000004, 0x00000001, 0x00000000, 0x00000001, 0x00000004},
		{0x00001000, 0x01000000, 0x00000024, 0x00000019, 0x00000010, 0x00000009, 0x00000004, 0x00000001, 0x00000000, 0x00000001},
		{0x00001000, 0x01000000, 0x00000031, 0x00000024, 0x00000019, 0x00000010, 0x00000009, 0x00000004, 0x00000001, 0x00000000}};

	if ( x>9) x = 1;
	if ( y>9) y = 1;

	return Metric[x][y];
}

UINT32 CCPanose::DistanceStrokeVariation(BYTE x, BYTE y)
{
	const static UINT32 Metric[9][9] = {
		{0x00010000, 0x00100000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000},
		{0x00100000, 0x10000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000},
		{0x00001000, 0x01000000, 0x00000000, 0x00000010, 0x00000010, 0x00000010, 0x00000020, 0x00000020, 0x00000040},
		{0x00001000, 0x01000000, 0x00000010, 0x00000000, 0x00000010, 0x00000010, 0x00000020, 0x00000020, 0x00000040},
		{0x00001000, 0x01000000, 0x00000010, 0x00000010, 0x00000000, 0x00000010, 0x00000020, 0x00000020, 0x00000040},
		{0x00001000, 0x01000000, 0x00000010, 0x00000010, 0x00000010, 0x00000000, 0x00000020, 0x00000020, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000000, 0x00000010, 0x00000020},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000010, 0x00000000, 0x00000020},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000020, 0x00000020, 0x00000000}};

	if ( x>8) x = 1;
	if ( y>8) y = 1;

	return Metric[x][y];
}

UINT32 CCPanose::DistanceArmStyle(BYTE x, BYTE y)
{
	const static UINT32 Metric[12][12] = {
		{0x00010000, 0x00100000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000},
		{0x00100000, 0x10000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000},
		{0x00001000, 0x01000000, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000010, 0x00000040, 0x00000040, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000040, 0x00000010, 0x00000040, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000000, 0x00000020, 0x00000020, 0x00000040, 0x00000040, 0x00000010, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000020, 0x00000000, 0x00000020, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000000, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000010},
		{0x00001000, 0x01000000, 0x00000010, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000040, 0x00000010, 0x00000040, 0x00000040, 0x00000040, 0x00000020, 0x00000000, 0x00000020, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000010, 0x00000040, 0x00000040, 0x00000020, 0x00000020, 0x00000000, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000040, 0x00000020, 0x00000020, 0x00000020, 0x00000000, 0x00000020},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000000}};

	if ( x>11) x = 1;
	if ( y>11) y = 1;

	return Metric[x][y];
}

UINT32 CCPanose::DistanceLetterform(BYTE x, BYTE y)
{
	const static UINT32 Metric[16][16] = {
		{0x00010000, 0x00100000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000},
		{0x00100000, 0x10000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000},
		{0x00001000, 0x01000000, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000010, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000040, 0x00000010, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000040, 0x00000040, 0x00000010, 0x00000040, 0x00000040, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000020, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000040, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000000, 0x00000020, 0x00000020, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000000, 0x00000020, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000000, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000010},
		{0x00001000, 0x01000000, 0x00000010, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000040, 0x00000010, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000020, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000010, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000020, 0x00000020, 0x00000000, 0x00000020, 0x00000020, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000040, 0x00000040, 0x00000040, 0x00000020, 0x00000020, 0x00000020, 0x00000000, 0x00000020, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000040, 0x00000040, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000000, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000040, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000000, 0x00000020},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000040, 0x00000010, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000020, 0x00000000}};

	if ( x>15) x = 1;
	if ( y>15) y = 1;

	return Metric[x][y];
}

UINT32 CCPanose::DistanceMidline(BYTE x, BYTE y)
{
	const static UINT32 Metric[14][14] = {
		{0x00010000, 0x00100000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000},
		{0x00100000, 0x10000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000},
		{0x00001000, 0x01000000, 0x00000000, 0x00000010, 0x00000010, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000010, 0x00000000, 0x00000010, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000040},
		{0x00001000, 0x01000000, 0x00000010, 0x00000010, 0x00000000, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000020},
		{0x00001000, 0x01000000, 0x00000020, 0x00000040, 0x00000040, 0x00000000, 0x00000010, 0x00000010, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000040, 0x00000020, 0x00000040, 0x00000010, 0x00000000, 0x00000010, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000040},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000020, 0x00000010, 0x00000010, 0x00000000, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000020},
		{0x00001000, 0x01000000, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000000, 0x00000010, 0x00000010, 0x00000020, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000010, 0x00000000, 0x00000010, 0x00000040, 0x00000020, 0x00000040},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000010, 0x00000010, 0x00000000, 0x00000040, 0x00000040, 0x00000020},
		{0x00001000, 0x01000000, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000000, 0x00000010, 0x00000010},
		{0x00001000, 0x01000000, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000010, 0x00000000, 0x00000010},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000040, 0x00000040, 0x00000020, 0x00000010, 0x00000010, 0x00000000}};

	if ( x>13) x = 1;
	if ( y>13) y = 1;

	return Metric[x][y];
}

UINT32 CCPanose::DistanceXHeight(BYTE x, BYTE y)
{
	const static UINT32 Metric[8][8] = {
		{0x00010000, 0x00100000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000, 0x00001000},
		{0x00100000, 0x10000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000},
		{0x00001000, 0x01000000, 0x00000000, 0x00000020, 0x00000020, 0x00000010, 0x00000040, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000000, 0x00000020, 0x00000040, 0x00000010, 0x00000040},
		{0x00001000, 0x01000000, 0x00000020, 0x00000020, 0x00000000, 0x00000040, 0x00000040, 0x00000010},
		{0x00001000, 0x01000000, 0x00000010, 0x00000040, 0x00000040, 0x00000000, 0x00000020, 0x00000020},
		{0x00001000, 0x01000000, 0x00000040, 0x00000010, 0x00000040, 0x00000020, 0x00000000, 0x00000020},
		{0x00001000, 0x01000000, 0x00000040, 0x00000040, 0x00000010, 0x00000020, 0x00000020, 0x00000000}};

	if ( x>7) x = 1;
	if ( y>7) y = 1;

	return Metric[x][y];
}

/********************************************************************************************

>	void CCPanose::GetDebugDetails(StringBase * Str)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/96
	Inputs:		Pointer to a StringBase
	Returns:	-
	Purpose:	Adds to PANOSE debug information to (*Str)

********************************************************************************************/

void CCPanose::GetDebugDetails(StringBase *pStr)
{
	TCHAR				s[256], t[256];

	tsprintf( s, 256, _T("\r\nPANOSE information\r\n\r\n") );
	(*pStr) += s;
	
	switch (mFamilyType)
	{
		case PAN_ANY						: tsprintf( t, 256, _T("Any") ); break;
		case PAN_NO_FIT						: tsprintf( t, 256, _T("No fit") ); break;
		case PAN_FAMILY_TEXT_DISPLAY		: tsprintf( t, 256, _T("Text and display") ); break;
		case PAN_FAMILY_SCRIPT				: tsprintf( t, 256, _T("Script") ); break;
		case PAN_FAMILY_DECORATIVE			: tsprintf( t, 256, _T("Decorative") ); break;
		case PAN_FAMILY_PICTORIAL			: tsprintf( t, 256, _T("Pictorial") ); break;
		default 							: tsprintf( t, 256, _T("!! Unknown !! (tell Andy)") ); break;
	}

	tsprintf( s, 256, _T("Family Type\t= (%d) %s\r\n") , mFamilyType, t);
	(*pStr) += s;

	switch (mSerifStyle)
	{
		case PAN_ANY						: tsprintf( t, 256, _T("Any") ); break;
		case PAN_NO_FIT						: tsprintf( t, 256, _T("No fit") ); break;
		case PAN_SERIF_COVE					: tsprintf( t, 256, _T("Cove") ); break;
		case PAN_SERIF_OBTUSE_COVE			: tsprintf( t, 256, _T("Obtuse cove") ); break;
		case PAN_SERIF_SQUARE_COVE			: tsprintf( t, 256, _T("Square cove") ); break;
		case PAN_SERIF_OBTUSE_SQUARE_COVE	: tsprintf( t, 256, _T("Obtuse square cove") ); break;
		case PAN_SERIF_SQUARE				: tsprintf( t, 256, _T("Square	") ); break;
		case PAN_SERIF_THIN					: tsprintf( t, 256, _T("Thin") ); break;
		case PAN_SERIF_BONE					: tsprintf( t, 256, _T("Bone") ); break;
		case PAN_SERIF_EXAGGERATED			: tsprintf( t, 256, _T("Exaggerated") ); break;
		case PAN_SERIF_TRIANGLE				: tsprintf( t, 256, _T("Triangle") ); break;
		case PAN_SERIF_NORMAL_SANS			: tsprintf( t, 256, _T("Normal sans serif") ); break;
		case PAN_SERIF_OBTUSE_SANS			: tsprintf( t, 256, _T("Obtuse sans serif") ); break;
		case PAN_SERIF_PERP_SANS			: tsprintf( t, 256, _T("Perp sans serif") ); break;
		case PAN_SERIF_FLARED				: tsprintf( t, 256, _T("Flared") ); break;
		case PAN_SERIF_ROUNDED				: tsprintf( t, 256, _T("Rounded") ); break;
		default 							: tsprintf( t, 256, _T("!! Unknown !! (tell Andy)") ); break;
	}

	tsprintf( s, 256, _T("Serif Style\t\t= (%d) %s\r\n") , mSerifStyle, t);
	(*pStr) += s;
	
	switch (mWeight)
	{
		case PAN_ANY						: tsprintf( t, 256, _T("Any") ); break;
		case PAN_NO_FIT						: tsprintf( t, 256, _T("No fit") ); break;
		case PAN_WEIGHT_VERY_LIGHT			: tsprintf( t, 256, _T("Very light") ); break;
		case PAN_WEIGHT_LIGHT				: tsprintf( t, 256, _T("Light") ); break;
		case PAN_WEIGHT_THIN				: tsprintf( t, 256, _T("Thin") ); break;
		case PAN_WEIGHT_BOOK				: tsprintf( t, 256, _T("Book") ); break;
		case PAN_WEIGHT_MEDIUM				: tsprintf( t, 256, _T("Medium") ); break;
		case PAN_WEIGHT_DEMI				: tsprintf( t, 256, _T("Demibold") ); break;
		case PAN_WEIGHT_BOLD				: tsprintf( t, 256, _T("Bold") ); break;
		case PAN_WEIGHT_HEAVY				: tsprintf( t, 256, _T("Heavy") ); break;
		case PAN_WEIGHT_BLACK				: tsprintf( t, 256, _T("Black") ); break;
		case PAN_WEIGHT_NORD				: tsprintf( t, 256, _T("Nord") ); break;
		default 							: tsprintf( t, 256, _T("!! Unknown !! (tell Andy)") ); break;
	}

	tsprintf( s, 256, _T("Weight\t\t= (%d) %s\r\n") , mWeight, t);
	(*pStr) += s;

	switch (mProportion)
	{
		case PAN_ANY						: tsprintf( t, 256, _T("Any") ); break;
		case PAN_NO_FIT						: tsprintf( t, 256, _T("No fit") ); break;
		case PAN_PROP_OLD_STYLE				: tsprintf( t, 256, _T("Old style") ); break;
		case PAN_PROP_MODERN				: tsprintf( t, 256, _T("Modern") ); break;
		case PAN_PROP_EVEN_WIDTH			: tsprintf( t, 256, _T("Even width") ); break;
		case PAN_PROP_EXPANDED				: tsprintf( t, 256, _T("Expanded") ); break;
		case PAN_PROP_CONDENSED				: tsprintf( t, 256, _T("Condensed") ); break;
		case PAN_PROP_VERY_EXPANDED			: tsprintf( t, 256, _T("Very expanded") ); break;
		case PAN_PROP_VERY_CONDENSED		: tsprintf( t, 256, _T("Very condensed") ); break;
		case PAN_PROP_MONOSPACED			: tsprintf( t, 256, _T("Monospaced") ); break;
		default 							: tsprintf( t, 256, _T("!! Unknown !! (tell Andy)") ); break;
	}

	tsprintf( s, 256, _T("Proportion\t= (%d) %s\r\n") , mProportion, t);
	(*pStr) += s;
	
	switch (mContrast)
	{
		case PAN_ANY						: tsprintf( t, 256, _T("Any") ); break;
		case PAN_NO_FIT						: tsprintf( t, 256, _T("No fit") ); break;
		case PAN_CONTRAST_NONE				: tsprintf( t, 256, _T("None") ); break;
		case PAN_CONTRAST_VERY_LOW			: tsprintf( t, 256, _T("Very low") ); break;
		case PAN_CONTRAST_LOW				: tsprintf( t, 256, _T("Low") ); break;
		case PAN_CONTRAST_MEDIUM_LOW		: tsprintf( t, 256, _T("Medium low") ); break;
		case PAN_CONTRAST_MEDIUM			: tsprintf( t, 256, _T("Medium") ); break;
		case PAN_CONTRAST_MEDIUM_HIGH		: tsprintf( t, 256, _T("Medium high") ); break;
		case PAN_CONTRAST_HIGH				: tsprintf( t, 256, _T("High") ); break;
		case PAN_CONTRAST_VERY_HIGH			: tsprintf( t, 256, _T("Very high") ); break;
		default 							: tsprintf( t, 256, _T("!! Unknown !! (tell Andy)") ); break;
	}

	tsprintf( s, 256, _T("Contrast\t\t= (%d) %s\r\n") , mContrast, t);
	(*pStr) += s;
	
	switch (mStrokeVariation)
	{
		case PAN_ANY						: tsprintf( t, 256, _T("Any") ); break;
		case PAN_NO_FIT						: tsprintf( t, 256, _T("No fit") ); break;
		case PAN_STROKE_GRADUAL_DIAG		: tsprintf( t, 256, _T("Gradual/diagonal") ); break;
		case PAN_STROKE_GRADUAL_TRAN		: tsprintf( t, 256, _T("Gradual/transitional") ); break;
		case PAN_STROKE_GRADUAL_VERT		: tsprintf( t, 256, _T("Gradual/vertical") ); break;
		case PAN_STROKE_GRADUAL_HORZ		: tsprintf( t, 256, _T("Gradual/horizontal") ); break;
		case PAN_STROKE_RAPID_VERT			: tsprintf( t, 256, _T("Rapid/vertical") ); break;
		case PAN_STROKE_RAPID_HORZ			: tsprintf( t, 256, _T("Rapid/horizontal") ); break;
		case PAN_STROKE_INSTANT_VERT		: tsprintf( t, 256, _T("Instant/vertical") ); break;
		default 							: tsprintf( t, 256, _T("!! Unknown !! (tell Andy)") ); break;
	}

	tsprintf( s, 256, _T("Stroke Variation\t= (%d) %s\r\n") , mStrokeVariation, t);
	(*pStr) += s;
	
	switch (mArmStyle)
	{
		case PAN_ANY						: tsprintf( t, 256, _T("Any") ); break;
		case PAN_NO_FIT						: tsprintf( t, 256, _T("No fit") ); break;
		case PAN_STRAIGHT_ARMS_HORZ			: tsprintf( t, 256, _T("Straight arms/horizontal") ); break;
		case PAN_STRAIGHT_ARMS_WEDGE		: tsprintf( t, 256, _T("Straight arms/wedge") ); break;
		case PAN_STRAIGHT_ARMS_VERT			: tsprintf( t, 256, _T("traight arms/vertical") ); break;
		case PAN_STRAIGHT_ARMS_SINGLE_SERIF	: tsprintf( t, 256, _T("Straight arms/single-serif") ); break;
		case PAN_STRAIGHT_ARMS_DOUBLE_SERIF	: tsprintf( t, 256, _T("Straight arms/double-serif") ); break;
		case PAN_BENT_ARMS_HORZ				: tsprintf( t, 256, _T("Non-straight arms/horizontal") ); break;
		case PAN_BENT_ARMS_WEDGE			: tsprintf( t, 256, _T("Non-straight arms/wedge") ); break;
		case PAN_BENT_ARMS_VERT				: tsprintf( t, 256, _T("Non-straight arms/vertical") ); break;
		case PAN_BENT_ARMS_SINGLE_SERIF		: tsprintf( t, 256, _T("Non-straight arms/single-serif") ); break;
		case PAN_BENT_ARMS_DOUBLE_SERIF		: tsprintf( t, 256, _T("Non-straight arms/double-serif") ); break;
		default 							: tsprintf( t, 256, _T("!! Unknown !! (tell Andy)") ); break;
	}

	tsprintf( s, 256, _T("Arm Style\t\t= (%d) %s\r\n") , mArmStyle, t);
	(*pStr) += s;
	
	switch (mLetterform)
	{
		case PAN_ANY						: tsprintf( t, 256, _T("Any") ); break;
		case PAN_NO_FIT						: tsprintf( t, 256, _T("No fit") ); break;
		case PAN_LETT_NORMAL_CONTACT		: tsprintf( t, 256, _T("Normal/contact") ); break;
		case PAN_LETT_NORMAL_WEIGHTED		: tsprintf( t, 256, _T("Normal/weighted") ); break;
		case PAN_LETT_NORMAL_BOXED			: tsprintf( t, 256, _T("Normal/boxed") ); break;
		case PAN_LETT_NORMAL_FLATTENED		: tsprintf( t, 256, _T("Normal/flattened") ); break;
		case PAN_LETT_NORMAL_ROUNDED		: tsprintf( t, 256, _T("Normal/rounded") ); break;
		case PAN_LETT_NORMAL_OFF_CENTER		: tsprintf( t, 256, _T("Normal/off center") ); break;
		case PAN_LETT_NORMAL_SQUARE			: tsprintf( t, 256, _T("Normal/square") ); break;
		case PAN_LETT_OBLIQUE_CONTACT		: tsprintf( t, 256, _T("Oblique/contact") ); break;
		case PAN_LETT_OBLIQUE_WEIGHTED		: tsprintf( t, 256, _T("Oblique/weighted") ); break;
		case PAN_LETT_OBLIQUE_BOXED			: tsprintf( t, 256, _T("Oblique/boxed") ); break;
		case PAN_LETT_OBLIQUE_FLATTENED		: tsprintf( t, 256, _T("Oblique/flattened") ); break;
		case PAN_LETT_OBLIQUE_ROUNDED		: tsprintf( t, 256, _T("Oblique/rounded") ); break;
		case PAN_LETT_OBLIQUE_OFF_CENTER	: tsprintf( t, 256, _T("Oblique/off center") ); break;
		case PAN_LETT_OBLIQUE_SQUARE		: tsprintf( t, 256, _T("Oblique/square") ); break;
		default 							: tsprintf( t, 256, _T("!! Unknown !! (tell Andy)") ); break;
	}

	tsprintf( s, 256, _T("Letter Form\t= (%d) %s\r\n") , mLetterform, t);
	(*pStr) += s;

	switch (mMidline)
	{
		case PAN_ANY						: tsprintf( t, 256, _T("Any") ); break;
		case PAN_NO_FIT						: tsprintf( t, 256, _T("No fit") ); break;
		case PAN_MIDLINE_STANDARD_TRIMMED	: tsprintf( t, 256, _T("Standard/trimmed") ); break;
		case PAN_MIDLINE_STANDARD_POINTED	: tsprintf( t, 256, _T("Standard/pointed") ); break;
		case PAN_MIDLINE_STANDARD_SERIFED	: tsprintf( t, 256, _T("Standard/serifed") ); break;
		case PAN_MIDLINE_HIGH_TRIMMED		: tsprintf( t, 256, _T("High/trimmed") ); break;
		case PAN_MIDLINE_HIGH_POINTED		: tsprintf( t, 256, _T("High/pointed") ); break;
		case PAN_MIDLINE_HIGH_SERIFED		: tsprintf( t, 256, _T("High/serifed") ); break;
		case PAN_MIDLINE_CONSTANT_TRIMMED	: tsprintf( t, 256, _T("Constant/trimmed") ); break;
		case PAN_MIDLINE_CONSTANT_POINTED	: tsprintf( t, 256, _T("Constant/pointed") ); break;
		case PAN_MIDLINE_CONSTANT_SERIFED	: tsprintf( t, 256, _T("Constant/serifed") ); break;
		case PAN_MIDLINE_LOW_TRIMMED		: tsprintf( t, 256, _T("Low/trimmed") ); break;
		case PAN_MIDLINE_LOW_POINTED		: tsprintf( t, 256, _T("Low/pointed") ); break;
		case PAN_MIDLINE_LOW_SERIFED		: tsprintf( t, 256, _T("Low/serifed") ); break;
		default 							: tsprintf( t, 256, _T("!! Unknown !! (tell Andy)") ); break;
	}

	tsprintf( s, 256, _T("Midline\t\t= (%d) %s\r\n") , mMidline, t);
	(*pStr) += s;

	switch (mXHeight)
	{
		case PAN_ANY						: tsprintf( t, 256, _T("Any") ); break;
		case PAN_NO_FIT						: tsprintf( t, 256, _T("No fit") ); break;
		case PAN_XHEIGHT_CONSTANT_SMALL		: tsprintf( t, 256, _T("Constant/small") ); break;
		case PAN_XHEIGHT_CONSTANT_STD		: tsprintf( t, 256, _T("Constant/standard") ); break;
		case PAN_XHEIGHT_CONSTANT_LARGE		: tsprintf( t, 256, _T("Constant/large") ); break;
		case PAN_XHEIGHT_DUCKING_SMALL		: tsprintf( t, 256, _T("Ducking/small") ); break;
		case PAN_XHEIGHT_DUCKING_STD		: tsprintf( t, 256, _T("Ducking/standard") ); break;
		case PAN_XHEIGHT_DUCKING_LARGE		: tsprintf( t, 256, _T("Ducking/large") ); break;
		default 							: tsprintf( t, 256, _T("!! Unknown !! (tell Andy)") ); break;
	}

	tsprintf( s, 256, _T("XHeight\t\t= (%d) %s\r\n") , mXHeight, t);
	(*pStr) += s;
}
