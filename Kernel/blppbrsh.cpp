// $Id: blppbrsh.cpp 662 2006-03-14 21:31:49Z alex $
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

// blended path processor brush implementation

#include "camtypes.h"
#include "blppbrsh.h"
#include "bldbrdef.h"

CC_IMPLEMENT_DYNAMIC(BlendedPathProcessorBrush, PathProcessorBrush);

#define new CAM_DEBUG_NEW  

/********************************************************************************************

>	BlendedPathProcessorBrush::BlendedPathProcessorBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/2000

	Purpose:	Constructor, mostly just calls the base class version

********************************************************************************************/

BlendedPathProcessorBrush::BlendedPathProcessorBrush() : PathProcessorBrush()
{
	m_pBlendedBrushDef = NULL;
}


/********************************************************************************************

>	BlendedPathProcessorBrush::~BlendedPathProcessorBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/2000

	Purpose:	Destructor
********************************************************************************************/

BlendedPathProcessorBrush::~BlendedPathProcessorBrush()
{

}

	
/********************************************************************************************

>	virtual BrushDefinition* BlendedPathProcessorBrush::GetOurBrushDefinition() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	if we have a blended brush definition then we will return that, otherwise
				we will use the base class
	Purpose:	As above

********************************************************************************************/

BrushDefinition* BlendedPathProcessorBrush::GetOurBrushDefinition()
{
	if (m_pBlendedBrushDef != NULL)
		return m_pBlendedBrushDef;

	return PathProcessorBrush::GetOurBrushDefinition();
}

	
/********************************************************************************************

>	void BlendedPathProcessorBrush::SetBlendedBrushDefinition(BlendedBrushDefinition* pDef) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		A blended brush definition, this can be NULL
	Returns:	-
	Purpose:	As above

********************************************************************************************/

void BlendedPathProcessorBrush::SetBlendedBrushDefinition(BlendedBrushDefinition* pDef)
{
	m_pBlendedBrushDef = pDef;
}
	
	
/********************************************************************************************

>	void BlendedPathProcessorBrush::DeleteBlendedBrushDefinition()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		-
	Returns:	-
	Purpose:	Deletes the blended brush definition, use with caution, as the brush defintions
				are often used by multiple path processors

********************************************************************************************/

void BlendedPathProcessorBrush::DeleteBlendedBrushDefinition()
{
	if (m_pBlendedBrushDef != NULL)
		delete m_pBlendedBrushDef;
	m_pBlendedBrushDef = NULL;
}

/********************************************************************************************

>	BOOL BlendedPathProcessorBrush::SetBlendRatio(double Ratio) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	TRUE if Ratio is in the range 0-1, otherwise false
	Purpose:	As above

********************************************************************************************/

BOOL BlendedPathProcessorBrush::SetBlendRatio(double Ratio)
{
	if (Ratio < 0 || Ratio > 1)
		return FALSE;
	m_BlendRatio = Ratio;
	return TRUE;
}




/********************************************************************************************

>	virtual PathProcessorBrush *PathProcessorBrush::Clone(void)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Returns:	NULL if we're out of memory, else
				a pointer to a clone (exact copy) of this object

	Purpose:	To copy BlendedPathProcessorBrush or derived-class object, exactly the same as
				the PPB fn. except for the BlendedBrushDef.

********************************************************************************************/

PathProcessorBrush *BlendedPathProcessorBrush::Clone(void)
{

	// Clone this object 
	BlendedPathProcessorBrush *pClone = new BlendedPathProcessorBrush;

	if (pClone == NULL)
		return NULL;

	// copy the data
	pClone->SetBrushDefinition(m_BrushHandle);
	pClone->SetPathOffsetType(m_PathOffsetType);
	pClone->SetPathOffsetValue(m_PathOffsetValue);
	pClone->SetRotated(m_bRotate);
	pClone->SetRotationAngle(m_RotateAngle);
	pClone->SetSpacing(m_BrushSpacing);
	pClone->SetTiling(m_bTile);
	pClone->SetBrushScaling(m_BrushScaling);
	pClone->SetBrushScalingIncr(m_BrushScalingIncr);
	pClone->SetSpacingIncrProp(m_BrushSpacingIncrProp);
	pClone->SetSpacingIncrConst(m_BrushSpacingIncrConst);
	pClone->SetPathOffsetIncrConst(m_PathOffsetIncrConst);
	pClone->SetPathOffsetIncrProp(m_PathOffsetIncrProp);
	pClone->SetSpacingMaxRand(m_BrushSpacingMaxRand);
	pClone->SetSpacingRandSeed(m_BrushSpacingRandSeed);
	pClone->SetScalingMaxRand(m_BrushScalingMaxRand);
	pClone->SetScalingRandSeed(m_BrushScalingRandSeed);
	pClone->SetSequenceType(m_SequenceType);
	pClone->SetSequenceSeed(m_SequenceRandSeed);
	pClone->SetOffsetValueMaxRand(m_OffsetValueMaxRand);
	pClone->SetOffsetValueRandSeed(m_OffsetValueRandSeed);
	pClone->SetOffsetTypeRandSeed(m_OffsetTypeRandSeed);

	pClone->SetRotationMaxRand(m_RotationMaxRand);
	pClone->SetRotationRandSeed(m_RotationRandSeed);

	pClone->SetUseLocalFillColour(m_bUseLocalFillColour);

	pClone->SetParentAttribute(m_pParentAttr);
	
	pClone->SetBlendedBrushDefinition(m_pBlendedBrushDef);

	return(pClone);
}
	
/********************************************************************************************

>	double BlendedPathProcessorBrush::GetBlendRatio() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	our blend ratio member
	Purpose:	As above

********************************************************************************************/

double BlendedPathProcessorBrush::GetBlendRatio()
{
	return m_BlendRatio;
}
