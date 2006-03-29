// $Id$
// $Header:
// CNamedColourReplacer class implementation
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
#include "cnamecol.h"
#include "fillval.h"
#include "doccolor.h"


CC_IMPLEMENT_DYNCREATE(CNamedColourReplacer, CCObject);
CC_IMPLEMENT_DYNCREATE(CHSVChanger, CNamedColourReplacer);
CC_IMPLEMENT_DYNAMIC(CTransparencyReplacer, CCObject);

const INT32 MAX_SAT_VALUE = 255;
const INT32 MAX_HUE_VALUE = 360;
const INT32 MIN_VALUE = 0;
const INT32 MAX_TRANSPARENCY = 255;

/***********************************************************************************************

>	CNamedColourReplacer::CNamedColourReplacer()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000

	Purpose:	Constructor

***********************************************************************************************/

CNamedColourReplacer::CNamedColourReplacer()
{
	m_pColourFill	 = NULL;
	m_pReplaceColour = NULL;
}

/***********************************************************************************************

>	CNamedColourReplacer::CNamedColourReplacer(ColourFillAttribute* pFillAttr, DocColour* pColour)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		pFillAttr - the attribute value that we want to replace the colours in
				pColour - the colour we wish to replace them with
	Purpose:	Constructor with initialisation

***********************************************************************************************/

CNamedColourReplacer::CNamedColourReplacer(ColourFillAttribute* pFillAttr, DocColour* pColour)
{
	// call the default constructor that NULLs all the member pointers
	CNamedColourReplacer::CNamedColourReplacer();
	Initialise(pFillAttr, pColour);
}


/***********************************************************************************************

>	CNamedColourReplacer::~CNamedColourReplacer()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000

	Purpose:	Destructor

***********************************************************************************************/

CNamedColourReplacer::~CNamedColourReplacer()
{

}

/***********************************************************************************************

>	CNamedColourReplacer::Initialise(ColourFillAttribute* pFillAttr, DocColour* pColour)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		pFillAttr - the attribute value that we want to replace the colours in
				pColour - the colour we wish to replace them with
	Purpose:	initialisation function.

***********************************************************************************************/

BOOL CNamedColourReplacer::Initialise(ColourFillAttribute* pFillAttr, DocColour* pColour)
{
	if (pFillAttr == NULL || pColour == NULL)
		return FALSE;

	m_pColourFill = pFillAttr;
	m_pReplaceColour = pColour;

	return TRUE;
}


/***********************************************************************************************

>	BOOL CNamedColourReplacer::IsInitialised()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Retuns:		TRUE if our important members have been initialised, FALSE otherwise
	Purpose:	as above

***********************************************************************************************/

BOOL CNamedColourReplacer::IsInitialised()
{
	return ((m_pColourFill != NULL) && (m_pReplaceColour != NULL));
}


/***********************************************************************************************

>	BOOL CNamedColourReplacer::ReplaceColours()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	TRUE if all went well, FALSE if something went wrong
	Purpose:	This function asks the colour fill for its start colour, checks to see
				if they are named, or if they were created by a named colour. If so it copies them 
				and stores them.  It then replaces them with our replacement colour.

***********************************************************************************************/

BOOL CNamedColourReplacer::ReplaceColours()
{
//	PORTNOTETRACE("other","CNamedColourReplacer::ReplaceColours - do nothing");
//#ifndef EXCLUDE_FROM_XARALX
	if (m_pColourFill == NULL || m_pReplaceColour == NULL)
	{
		ERROR3("CNamedColourReplacer not initialised correctly");
		return FALSE;
	}

	// we only replace the start colour, so as to maintain the effect on
	// 2/3/4 colour fills
	DocColour* pOrigStartCol = m_pColourFill->GetStartColour();
	if (pOrigStartCol != NULL && pOrigStartCol->IsNamed())
	{
		// we've got one, so stash the original and replace it
		m_StartColour = *pOrigStartCol;
		m_pColourFill->SetStartColour(m_pReplaceColour);
		
	}

	return TRUE;
//#else
//	return FALSE;
//#endif
}


/***********************************************************************************************

>	BOOL CNamedColourReplacer::RestoreColours()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	TRUE if all went well, FALSE if something went wrong
	Purpose:	This function assumes that ReplaceColours has been previously called.  It uses its
				pointers to start and end colours to set colours in the attribute.

***********************************************************************************************/

BOOL CNamedColourReplacer::RestoreColours()
{
//	PORTNOTETRACE("other","CNamedColourReplacer::RestoreColours - do nothing");
//#ifndef EXCLUDE_FROM_XARALX
	if (m_pColourFill == NULL)
	{
		ERROR3("Colour fill is NULL in CNamedColourReplacer::RestoreColours");
		return FALSE;
	}

	// restore our colour
	if (m_StartColour.GetReservedFlag() == COL_NAMED)
		m_pColourFill->SetStartColour(&m_StartColour);
	

	return TRUE;
//#else
//	return FALSE;
//#endif
}


/***********************************************************************************************

>	ColourFillAttribute* CNamedColourReplacer::GetColourFill()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	pointer to the attribute that we are replacing colours in
	Purpose:	access fn.

***********************************************************************************************/

ColourFillAttribute* CNamedColourReplacer::GetColourFill()
{
	return m_pColourFill;
}



/***********************************************************************************************

>	BOOL CNamedColourReplacer::SetColourFill(ColourFillAttribute* pAttr)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		pAttr - pointer to a colour fill attribute that we wish to replace the colours in
	Returns:	TRUE, unless you pass in a duff pointer
	Purpose:	access fn.

***********************************************************************************************/

BOOL CNamedColourReplacer::SetColourFill(ColourFillAttribute* pAttr)
{
	if (pAttr == NULL)
		return FALSE;
	m_pColourFill = pAttr;
	return TRUE;
}



/***********************************************************************************************

>	DocColour* CNamedColourReplacer::GetReplacementColour()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	pointer to the the colour we are using to replace
	Purpose:	access fn.

***********************************************************************************************/

DocColour* CNamedColourReplacer::GetReplacementColour()
{
	return m_pReplaceColour;
}


/***********************************************************************************************

>	BOOL CNamedColourReplacer::SetReplacementColour(DocColour* pColour)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		pColour - the colour we wish to set in our attribute
	Returns:	TRUE, unless the colour is NULL
	Purpose:	access fn.

***********************************************************************************************/

BOOL CNamedColourReplacer::SetReplacementColour(DocColour* pColour)
{
	if (pColour == NULL)
		return FALSE;
	m_pReplaceColour = pColour;
	return TRUE;
}


/*-----------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------
------------------------------CSHVCHANGER CLASS IMPLEMENTATION-----------------------------------
------------------------------------------------------------------------------------------------*/


/***********************************************************************************************

>	CHSVChanger::CHSVChanger()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000

	Purpose:	default constructor

***********************************************************************************************/

CHSVChanger::CHSVChanger() : CNamedColourReplacer()
{
	m_HueMultiplier			= 1.0;
	m_SaturationMultiplier	= 1.0;
	m_ValueMultiplier		= 1.0;
	m_HueIncrement			= 0;
	m_SatIncrement			= 0;
}


/***********************************************************************************************

>	CHSVChanger::CHSVChanger()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000

	Purpose:	default constructor

***********************************************************************************************/

CHSVChanger::CHSVChanger(ColourFillAttribute* pFill, DocColour* pCol) : CNamedColourReplacer(pFill, pCol)
{
	m_HueMultiplier			= 1.0;
	m_SaturationMultiplier	= 1.0;
	m_ValueMultiplier		= 1.0;
	m_HueIncrement			= 0;
	m_SatIncrement			= 0;

}

/***********************************************************************************************

>	CHSVChanger::~CHSVChanger()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000

	Purpose:	destructor

***********************************************************************************************/

CHSVChanger::~CHSVChanger()
{
//	PORTNOTETRACE("other","CHSVChanger::~CHSVChanger - do nothing");
//#ifndef EXCLUDE_FROM_XARALX
	// have to do a hack here to ensure that we don't decrement the usage count on 
	// indexed colours which we never incremented in the first place
	m_OrigStartCol.HackColReplacerPreDestruct();
	m_OrigEndCol1.HackColReplacerPreDestruct();
	m_OrigEndCol2.HackColReplacerPreDestruct();
	m_OrigEndCol3.HackColReplacerPreDestruct();
//#endif
}


/***********************************************************************************************

>	virtual BOOL CHSVChanger::ReplaceColours()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	TRUE if all went well, FALSE if something went wrong
	Purpose:	This function asks the colour fill for its colours one by one, checks to see
				if they are named, or if they were created by a named colour. If so it copies them 
				and stores them.  It then replaces them with our replacement colour.

***********************************************************************************************/

BOOL CHSVChanger::ReplaceColours()
{
	// if we also want to replace colours then do that first
	if (m_pReplaceColour != NULL)
		return CNamedColourReplacer::ReplaceColours();

	return TRUE;
}


/***********************************************************************************************

>	BOOL CHSVChanger::ChangeHSVValues()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	TRUE if all went well, FALSE if something went wrong
	Purpose:	The raison d'etre of this object.  Changes the HSV values of the member fill
***********************************************************************************************/

BOOL CHSVChanger::ChangeHSVValues()
{
	if (m_pColourFill == NULL)
	{
		ERROR3("Colour fill is NULL in CHSVChanger::ChangeHSVValues");
		return FALSE;
	}
	
	// we need to ask for the colours one by one
	DocColour* pStartCol = NULL;
	if (m_pReplaceColour != NULL)
		pStartCol = m_pReplaceColour;
	else
		pStartCol = m_pColourFill->GetStartColour();
	
	if (pStartCol != NULL)
	{
		m_OrigStartCol = *pStartCol;
		ApplyIncrements(pStartCol);
		m_pColourFill->SetStartColour(pStartCol);
	}

	DocColour* pEndCol1 = m_pColourFill->GetEndColour();
	if (pEndCol1 != NULL) 
	{
		// first record our original values
		m_OrigEndCol1 = *pEndCol1;
		ApplyIncrements(pEndCol1);
	}
	
	DocColour* pEndCol2 = m_pColourFill->GetEndColour2();
	if (pEndCol2 != NULL) 
	{
		// first record our original values
		m_OrigEndCol2 = *pEndCol2;
		ApplyIncrements(pEndCol2);
	}

	DocColour* pEndCol3 = m_pColourFill->GetEndColour3();
	if (pEndCol3 != NULL)
	{
		// first record our original values
		m_OrigEndCol3 = *pEndCol3;
		ApplyIncrements(pEndCol3);
	}
	
	return TRUE;
}



/***********************************************************************************************

>	BOOL CHSVChanger::RestoreColours()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	TRUE if all went well, FALSE if something went wrong
	Purpose:	Restores the colours that were changed

***********************************************************************************************/
BOOL CHSVChanger::RestoreColours()
{
	if (m_pColourFill == NULL)
		return FALSE;

	// here we restore any named colours we replaced
	if (m_pReplaceColour != NULL)
		return CNamedColourReplacer::RestoreColours();

	// we need to ask for the colours one by one
	DocColour* pStartCol = m_pColourFill->GetStartColour();
	if (pStartCol != NULL)
	{
		*pStartCol = m_OrigStartCol;
	}

	DocColour* pEndCol1 = m_pColourFill->GetEndColour();
	if (pEndCol1 != NULL) 
	{
		*pEndCol1 = m_OrigEndCol1;

	}
	
	DocColour* pEndCol2 = m_pColourFill->GetEndColour2();
	if (pEndCol2 != NULL) 
	{
		*pEndCol2 = m_OrigEndCol2;
	}

	DocColour* pEndCol3 = m_pColourFill->GetEndColour3();
	if (pEndCol3 != NULL)
	{
		*pEndCol3 = m_OrigEndCol3;
	}
	
	return TRUE;	



}


/***********************************************************************************************

>	BOOL CHSVChanger::IsInitialised()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	true, if this object was initialised in any way
	Purpose:	
***********************************************************************************************/

BOOL CHSVChanger::IsInitialised()
{
	BOOL Init = m_pReplaceColour != NULL ||
				m_HueMultiplier != 1.0 || 
				m_SaturationMultiplier != 1.0;

	return Init;
}

/***********************************************************************************************

>	BOOL CHSVChanger::ShouldChangeHSV()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	true, if ChangeHSV should be called, FALSE otherwise
	Purpose:	does a few checks to see if it is worth changing hsv values, currently we only use
				the multipliers so it just checks to see if they are default or not
***********************************************************************************************/

BOOL CHSVChanger::ShouldChangeHSV()
{
	BOOL bShouldReplace = (m_HueIncrement != 0 || m_SatIncrement != 0);

	return bShouldReplace;
}



/***********************************************************************************************

>	void CHSVChanger::ApplyIncrements(DocColour* pCol)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		pCol - the colour to be incremented
	Returns:	-
	Purpose:	Takes a colour, extracts its HSV values, adds (or subtracts) the increment value

***********************************************************************************************/

void CHSVChanger::ApplyIncrements(DocColour* pCol)
{
//	PORTNOTETRACE("other","CHSVChanger::ApplyIncrements - do nothing");
//#ifndef EXCLUDE_FROM_XARALX
	if (pCol == NULL)
	{
		ERROR3("Colour pointer is NULL in CHSVChanger::ApplyIncrements");
		return;
	}

	INT32 Hue;
	INT32 Sat;
	INT32 Dummy;

	pCol->GetHSVValue(&Hue, &Sat, &Dummy);
	//TRACEUSER( "Diccon", _T("Original Sat = %d, "), Sat);
	//TRACEUSER( "Diccon", _T("Original Hue = %d, "), Hue);
	Hue += m_HueIncrement;
	if (Hue < 0)
		Hue += MAX_HUE_VALUE;
	else if (Hue > MAX_HUE_VALUE)
		Hue -= MAX_HUE_VALUE;

	Sat -= abs(m_SatIncrement);
		if (Sat < 0)
		Sat = 1;
	else if (Sat > MAX_SAT_VALUE)
		Sat = MAX_SAT_VALUE;
	
//	TRACEUSER( "Diccon", _T("SatIncr = %d, New Sat = %d\n"), m_SatIncrement, Sat);
//	TRACEUSER( "Diccon", _T("Hue Incr = %d, New Hue = %d\n"), m_HueIncrement, Hue);
	pCol->SetHSVValue(Hue, Sat, Dummy);
//#endif
}


/***********************************************************************************************

>	CHSVChanger::GetHueMultiplier()
	
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Outputs:	-
	Returns:	the hue multiplier, duh
	Purpose:	Access

***********************************************************************************************/

double CHSVChanger::GetHueMultiplier()
{
	return m_HueMultiplier;
}


/***********************************************************************************************

>	BOOL CHSVChanger::SetHueMultiplier(double Mult)
	
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		the hue multiplier, duh
	Outputs:	-
	Returns:	always TRUE at the moment
	Purpose:	Sets the Hue multiplier, also calculates the value that will be added/subtracted
				from the colour

***********************************************************************************************/

BOOL CHSVChanger::SetHueMultiplier(double Value)
{
	m_HueMultiplier = Value;
	m_HueIncrement = (INT32)Value; // * (double)MAX_HUE_VALUE);
	//TRACEUSER( "Diccon", _T("Hue Increment = %d\n"), m_HueIncrement);
	return TRUE;
}

/***********************************************************************************************

>	CHSVChanger::GetSaturationMultiplier()
	
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Outputs:	-
	Returns:	the Saturation multiplier, duh
	Purpose:	Access

***********************************************************************************************/

double CHSVChanger::GetSaturationMultiplier()
{
	return m_SaturationMultiplier;
}


/***********************************************************************************************

>	BOOL CHSVChanger::SetSaturationMultiplier(double Value)
	
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		the Saturation multiplier, as you might expect
	Outputs:	-
	Returns:	TRUE 
	Purpose:	Sets the saturation multiplier, also calculates the addition/subtraction value

***********************************************************************************************/

BOOL CHSVChanger::SetSaturationMultiplier(double Value)
{
	m_SaturationMultiplier = Value;

	m_SatIncrement = (INT32)(Value * (double)MAX_SAT_VALUE);
	
	//TRACEUSER( "Diccon", _T("Sat increment = %d\n"), m_SatIncrement);
	return TRUE;
}


/***********************************************************************************************

>	void CHSVChanger::ChangeColour(DocColour* pCol)
	
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		pCol - pointer to the colour we wish to change
	Outputs:	Colour with changed values
	Returns:	-
	Purpose:	Simply changes the values in this one colour alone, in case you're fed up of 
				worrying about attributes and the like
**********************************************************************************************/

void CHSVChanger::ChangeColour(DocColour* pCol)
{
	if (pCol == NULL)
		return;
	ApplyIncrements(pCol);
}



/*----------------------------------------------------------------------------------------------
----------------------CTransparency Class implementation----------------------------------------
------------------------------------------------------------------------------------------------*/


/***********************************************************************************************

>	CTransparencyReplacer::CTransparencyReplacer()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000

	Purpose:	default constructor

***********************************************************************************************/

CTransparencyReplacer::CTransparencyReplacer()
{
	m_pTranspFill = NULL;
	m_OriginalTransp = 0;
	m_ReplaceTransp = -1;
	m_TranspAdjust  = 1.0;
	m_InvertAdjust = 1.0;
}


/***********************************************************************************************

>	BOOL CTransparencyReplacer::SetReplacementValue(INT32 Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		the value to use as our replacement
	Returns:	TRUE unless Value is outside the legal range
	Purpose:	access

***********************************************************************************************/

BOOL CTransparencyReplacer::SetReplacementValue(INT32 Value)
{
	if (Value > MAX_TRANSP_VALUE)
		return FALSE;

	m_ReplaceTransp = Value;
	return TRUE;
}


/***********************************************************************************************

>	INT32 CTransparencyReplacer::GetReplacementValue()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	out replacement transparency value
	Purpose:	access

***********************************************************************************************/

INT32 CTransparencyReplacer::GetReplacementValue()
{
	return m_ReplaceTransp;
}


/***********************************************************************************************

>	BOOL CTransparencyReplacer::SetAdjustmentValue(double Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		our adjuster value
	Returns:	TRUE unless Value is outside the legal range
	Purpose:	access

***********************************************************************************************/

BOOL CTransparencyReplacer::SetAdjustmentValue(double Value)
{
	if (Value > 3.0 || Value < 0)
		return FALSE;
	m_TranspAdjust = Value;
	
	return TRUE;
}


/***********************************************************************************************

>	BOOL CTransparencyReplacer::GetAdjustmentValue(double Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		
	Returns:	our adjuster value
	Purpose:	access

***********************************************************************************************/

double CTransparencyReplacer::GetAdjustmentValue()
{
	return m_TranspAdjust;
}


/***********************************************************************************************

>	BOOL CTransparencyReplacer::SetInvertedAdjuster(double Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		our adjuster value
	Returns:	TRUE unless Value is outside the legal range
	Purpose:	access to our inverted adjuster 

***********************************************************************************************/

BOOL CTransparencyReplacer::SetInvertedAdjuster(double Value)
{
	if (Value > 3.0 || Value < 0)
		return FALSE;
	m_InvertAdjust = Value;
	
	return TRUE;
}


/***********************************************************************************************

>	BOOL CTransparencyReplacer::GetAdjustmentValue1(double Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		
	Returns:	our inverted adjuster value
	Purpose:	access

***********************************************************************************************/

double CTransparencyReplacer::GetInvertedAdjuster()
{
	return m_InvertAdjust;
}



/***********************************************************************************************

>	void CTransparencyReplacer::SetTransparencyAttr(TranspFillAttribute* pAttrVal)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		pAttrVal - the transparency attribute value that we are working on
	Returns:	-
	Purpose:	access

***********************************************************************************************/

void CTransparencyReplacer::SetTransparencyAttr(TranspFillAttribute* pAttrVal)
{
	m_pTranspFill = pAttrVal;
}


/***********************************************************************************************

>	BOOL CTransparencyReplacer::ReplaceTransparency()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	-
	Purpose:	Does the deed of replacing the transparency value in our attribute with the one
				we have been given

***********************************************************************************************/

BOOL CTransparencyReplacer::ReplaceTransparency()
{
	if (m_pTranspFill == NULL)
		return FALSE;

	// first cache the original value
	m_OriginalTransp = *m_pTranspFill->GetStartTransp();
	
	// recall that a negative replacement value means don't replace
	if (m_ReplaceTransp >= 0)
		m_pTranspFill->Transp = (UINT32)m_ReplaceTransp;

	if (m_TranspAdjust >= 0)
	{
		/* Ok, so what is going on here?  Basically this:
		- We have two adjusters, they both generate a value which is a proportion of the maximum transparency
		- One of the adjusters is inverted (i.e. as its value increases, its output value decreases 
		- Both adjusters make sure they are within legal limits	
		- The adjuster value is added to the original value
		- We take a straight mean of the two adjusted values and set it back to the attribute
		*/

		double TranspAdj = m_TranspAdjust * MAX_TRANSPARENCY;
		double TranspAdj1 = m_InvertAdjust * MAX_TRANSPARENCY;

		INT32 Adjust = INT32(TranspAdj); 
		Adjust -= MAX_TRANSPARENCY;

		INT32 InvertAdjust = INT32(MAX_TRANSPARENCY- TranspAdj1);
		
		INT32 FirstTransp = m_pTranspFill->Transp + Adjust;
		
		INT32 FinalTransp = FirstTransp + InvertAdjust;

		if (FinalTransp > MAX_TRANSPARENCY)
			FinalTransp = MAX_TRANSPARENCY;
		if (FinalTransp < 0)
			FinalTransp = 0;
	//	TRACEUSER( "Diccon", _T("New Transp = %d\n"), FinalTransp);
		m_pTranspFill->Transp = UINT32(FinalTransp); 
	}

	return TRUE;
}


/***********************************************************************************************

>	BOOL CTransparencyReplacer::RestoreTransparency()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	-
	Purpose:	Does the deed of replacing the transparency value in our attribute with the one
				we have been given

***********************************************************************************************/

BOOL CTransparencyReplacer::RestoreTransparency()
{
	if (m_pTranspFill == NULL)
		return FALSE;

	m_pTranspFill->Transp = m_OriginalTransp;

	return TRUE;
}


/***********************************************************************************************

>	BOOL CTransparencyReplacer::IsInitialised()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	True if the replacer has been initialised and is ready to replace, false otherwise
	Purpose:	as above
***********************************************************************************************/

BOOL CTransparencyReplacer::IsInitialised()
{
	return (m_pTranspFill != NULL && (m_ReplaceTransp >= 0 || m_TranspAdjust >= 0));
}
