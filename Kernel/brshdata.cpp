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
#include "brshdata.h"

DECLARE_SOURCE("$Revision$");


/***********************************************************************************************

>	BrushData::BrushData()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/3/2000

	Purpose:	Constructor

***********************************************************************************************/
BrushData::BrushData()
{
	m_BrushHandle			= BrushHandle_NoBrush;

	m_BrushSpacing			= 10000;
	m_BrushSpacingIncrProp  = 1.0; 
	m_BrushSpacingIncrConst = 0;
	m_BrushSpacingMaxRand   = 0;
	m_BrushSpacingRandSeed  = 0;
	
	m_bTile                 = TRUE;
	m_bRotate               = FALSE;
	m_RotateAngle			= 0.0;
	m_RotAngleIncrConst		= 0;
	m_RotAngleIncrProp      = 1.0;
	m_RotationMaxRand		= 0;	
	m_RotationRandSeed		= 0; 
	m_RotationMaxPressure		= 0;

	m_PathOffsetType		= OFFSET_NONE;  
	m_PathOffsetValue		= 0;
	m_PathOffsetIncrProp	= 1.0;
	m_PathOffsetIncrConst	= 0;
	m_OffsetTypeRandSeed    = 0;
	
	m_OffsetValueMaxRand	= 0;
	m_OffsetValueRandSeed	= 0;
	
	m_BrushScaling			= 1.0;
	m_BrushScalingIncr      = 1.0;
	m_BrushScalingIncrConst = 0.0;
	m_BrushScalingMaxRand	= 0;
	m_BrushScalingRandSeed	= 0;
	
	m_SequenceType			= SEQ_FORWARD;
	m_SequenceRandSeed		= 0;

	m_LineWidth				= -1;

	m_bUseLocalFillColour	= FALSE;
	m_bUseLocalTransp       = FALSE;
	m_bUseNamedColour       = TRUE;
	m_bTimeStampBrushes     = FALSE;
	m_TimeStampPeriod       = 0;

	m_BrushTransparency     = -1;
	m_TranspMaxPressure     = 0;

}


/***********************************************************************************************

>	BrushData::BrushData(BrushData Other)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/3/2000

	Purpose:	copy Constructor

***********************************************************************************************/

BrushData::BrushData(const BrushData &Other)
{
	this->operator =(Other);
}

/***********************************************************************************************

>	BrushData::operator=(BrushData Other)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/3/2000

	Purpose:	assignment operator

***********************************************************************************************/

const BrushData& BrushData::operator =(const BrushData& Other)
{
	m_Name                  = Other.m_Name;
	m_BrushHandle			= Other.m_BrushHandle;

	m_BrushSpacing			= Other.m_BrushSpacing;
	m_BrushSpacingIncrProp  = Other.m_BrushSpacingIncrProp;
	m_BrushSpacingIncrConst = Other.m_BrushSpacingIncrConst;
	m_BrushSpacingMaxRand   = Other.m_BrushSpacingMaxRand;
	m_BrushSpacingRandSeed  = Other.m_BrushSpacingRandSeed;
	
	m_bTile                 = Other.m_bTile;
	m_bRotate               = Other.m_bRotate;
	m_RotateAngle			= Other.m_RotateAngle;
	m_RotAngleIncrProp      = Other.m_RotAngleIncrProp;
	m_RotAngleIncrConst     = Other.m_RotAngleIncrConst;
	m_RotationMaxRand		= Other.m_RotationMaxRand;	
	m_RotationRandSeed		= Other.m_RotationRandSeed;
	m_RotationMaxPressure   = Other.m_RotationMaxPressure;

	m_PathOffsetType		= Other.m_PathOffsetType; 
	m_PathOffsetValue		= Other.m_PathOffsetValue;
	m_PathOffsetIncrProp	= Other.m_PathOffsetIncrProp;
	m_PathOffsetIncrConst	= Other.m_PathOffsetIncrConst;
	m_OffsetTypeRandSeed    = Other.m_OffsetTypeRandSeed;
	
	m_OffsetValueMaxRand	= Other.m_OffsetValueMaxRand;
	m_OffsetValueRandSeed	= Other.m_OffsetValueRandSeed;
	
	m_BrushScaling			= Other.m_BrushScaling;
	m_BrushScalingIncr      = Other.m_BrushScalingIncr;
	m_BrushScalingIncrConst = Other.m_BrushScalingIncrConst;
	m_BrushScalingMaxRand	= Other.m_BrushScalingMaxRand;
	m_BrushScalingRandSeed	= Other.m_BrushScalingRandSeed;
	m_ScalingMaxPressure    = Other.m_ScalingMaxPressure;
	
	m_SequenceType			= Other.m_SequenceType;
	m_SequenceRandSeed		= Other.m_SequenceRandSeed;

	m_LineWidth				= Other.m_LineWidth;

	m_bUseLocalFillColour	= Other.m_bUseLocalFillColour;
	m_bUseLocalTransp       = Other.m_bUseLocalTransp;
	m_bUseNamedColour       = Other.m_bUseNamedColour;
	m_bTimeStampBrushes     = Other.m_bTimeStampBrushes;
	m_TimeStampPeriod       = Other.m_TimeStampPeriod;

	m_BrushSatIncrement		= Other.m_BrushSatIncrement;
	m_BrushSatMaxRand		= Other.m_BrushSatMaxRand;
	m_BrushSatRandSeed		= Other.m_BrushSatRandSeed;
	m_BrushHueIncrement		= Other.m_BrushHueIncrement;
	m_BrushHueMaxRand		= Other.m_BrushHueMaxRand;
	m_BrushHueRandSeed		= Other.m_BrushHueRandSeed;

	m_BrushTransparency     = Other.m_BrushTransparency;
	m_TranspMaxPressure     = Other.m_TranspMaxPressure;

	return *this;
}

/***********************************************************************************************

>	BOOL BrushData::operator ==(const BrushData& Other)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/3/2000

	Purpose:	equality operator

***********************************************************************************************/

BOOL BrushData::operator ==(const BrushData& Other)
{
	if (
		m_Name                  != Other.m_Name ||
		m_BrushHandle			!= Other.m_BrushHandle ||

		m_BrushSpacing			!= Other.m_BrushSpacing ||
		m_BrushSpacingIncrProp  != Other.m_BrushSpacingIncrProp ||
		m_BrushSpacingIncrConst != Other.m_BrushSpacingIncrConst ||
		m_BrushSpacingMaxRand   != Other.m_BrushSpacingMaxRand ||
		m_BrushSpacingRandSeed  != Other.m_BrushSpacingRandSeed ||
		
		m_bTile                 != Other.m_bTile ||
		m_bRotate               != Other.m_bRotate ||
		m_RotateAngle			!= Other.m_RotateAngle ||
		m_RotAngleIncrProp      != Other.m_RotAngleIncrProp ||
		m_RotAngleIncrConst     != Other.m_RotAngleIncrConst ||
		m_RotationMaxRand		!= Other.m_RotationMaxRand ||	
		m_RotationRandSeed		!= Other.m_RotationRandSeed ||
		m_RotationMaxPressure   != Other.m_RotationMaxPressure ||

		m_PathOffsetType		!= Other.m_PathOffsetType || 
		m_PathOffsetValue		!= Other.m_PathOffsetValue ||
		m_PathOffsetIncrProp	!= Other.m_PathOffsetIncrProp ||
		m_PathOffsetIncrConst	!= Other.m_PathOffsetIncrConst ||
		m_OffsetTypeRandSeed    != Other.m_OffsetTypeRandSeed ||
		
		m_OffsetValueMaxRand	!= Other.m_OffsetValueMaxRand ||
		m_OffsetValueRandSeed	!= Other.m_OffsetValueRandSeed ||
		
		m_BrushScaling			!= Other.m_BrushScaling ||
		m_BrushScalingIncr      != Other.m_BrushScalingIncr ||
		m_BrushScalingIncrConst != Other.m_BrushScalingIncrConst ||
		m_BrushScalingMaxRand	!= Other.m_BrushScalingMaxRand ||
		m_BrushScalingRandSeed	!= Other.m_BrushScalingRandSeed ||
		m_ScalingMaxPressure    != Other.m_ScalingMaxPressure ||
		
		m_SequenceType			!= Other.m_SequenceType ||
		m_SequenceRandSeed		!= Other.m_SequenceRandSeed ||

		m_LineWidth				!= Other.m_LineWidth ||

		m_bUseLocalFillColour	!= Other.m_bUseLocalFillColour ||
		m_bUseLocalTransp       != Other.m_bUseLocalTransp ||
		m_bUseNamedColour       != Other.m_bUseNamedColour ||
		m_bTimeStampBrushes     != Other.m_bTimeStampBrushes ||
		m_TimeStampPeriod       != Other.m_TimeStampPeriod ||

		m_BrushSatIncrement		!= Other.m_BrushSatIncrement ||
		m_BrushSatMaxRand		!= Other.m_BrushSatMaxRand ||
		m_BrushSatRandSeed		!= Other.m_BrushSatRandSeed ||
		m_BrushHueIncrement		!= Other.m_BrushHueIncrement ||
		m_BrushHueMaxRand		!= Other.m_BrushHueMaxRand ||
		m_BrushHueRandSeed		!= Other.m_BrushHueRandSeed ||

		m_BrushTransparency     != Other.m_BrushTransparency ||
		m_TranspMaxPressure     != Other.m_TranspMaxPressure
		)
		return FALSE;
	else
		return TRUE;
}
