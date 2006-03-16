// $Id: oilpanse.cpp 662 2006-03-14 21:31:49Z alex $
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

>	oilpanse.cpp

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/07/96
	Purpose:	Implementation of the OIL related Panose code
	SeeAlso:	kernel\ccpanose.h, kernel\ccpanose.cpp

********************************************************************************************/

#include "camtypes.h"

#include "ccpanose.h"
#include "oilpanse.h"

/********************************************************************************************

>	HDC OILPanose::MyDC
	BOOL OILPanose::IsInitialised

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>	
	Created:	24/07/96
	Purpose:	Static member variables of the OILPanose class

********************************************************************************************/

CDC *OILPanose::pMyCDC = NULL;
BOOL OILPanose::IsInitialised = FALSE;

/********************************************************************************************

>	BOOL OILPanose::Init(CDC *pCDC)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/07/96
	Inputs:		pCDC	- pointer to a CDC to use
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Initialises the static member varibles of this class.
	See Also:	OILPanose::DeInit();

	Since DC's are in _very_ short supply, you must only call Init when needed, and call
	DeInit as soon as possible afterwards. There's no problem under NT, but both Win95 and
	Win3.11 have a limited supply of DCs.

********************************************************************************************/

BOOL OILPanose::Init(CDC *ptr)
{
	ERROR2IF(ptr==NULL, FALSE, "Parameter ptr == NULL");
	ERROR2IF(IsInitialised==TRUE, FALSE, "OILPanose::Init called when IsInitialised==TRUE.");

	pMyCDC = ptr;
	IsInitialised=TRUE;

	return TRUE;
}

/********************************************************************************************

>	BOOL OILPanose::DeInit()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/07/96
	Inputs:		-
	Returns:	TRUE if sucessful, FALSE otherwise
	Purpose:	Deinitialises this, the OILPanose class.
	See Also:	void OILPanose::Init()

********************************************************************************************/

BOOL OILPanose::DeInit()
{
	ERROR2IF(IsInitialised==FALSE, FALSE, "OILPanose::DeInit called when IsInitialised==FALSE");

	pMyCDC = NULL;
	IsInitialised = FALSE;

	return TRUE;
}

/********************************************************************************************

>	BOOL OILPanose::GetPanoseNumber(ENUMLOGFONT *pEnumLogFont, CCPanose *pPanose)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/07/96
	Inputs:		pEnumLogFont	- pointer to an ENUMLOGFONT structure
				pPanose			- pointer to an CCPanose structure
	Outputs:	Enters values into the CCPanose structure pointed to by pPanose.
	Purpose:	Platform depenedent method of getting the Panose number for a font

********************************************************************************************/

BOOL OILPanose::GetPanoseNumber(ENUMLOGFONT *pEnumLogFont, CCPanose *pPanose)
{
	ERROR2IF(pEnumLogFont==NULL, FALSE, "Parameter pEnumLogFont == NULL.");
	ERROR2IF(pPanose==NULL, FALSE, "Parameter pPanose == NULL.");
	ERROR2IF(IsInitialised==FALSE, FALSE, "Class OILPanose not initialised.");

	BOOL ok = FALSE;

	// watch closely - it gets fairly involved down here

	CFont *pNewCFont;

	pNewCFont = new CFont;

	if (pNewCFont->CreateFontIndirect(&(pEnumLogFont->elfLogFont)) != NULL)
	{
		// success, managed to get create a new font. now select it into the CDC we have
		CFont *pOldCFont;
		pOldCFont = pMyCDC->SelectObject(pNewCFont);

		if (pOldCFont!=NULL)
		{
			// success, managed to select the font into the DC

			TEXTMETRIC MyTextMetric;

			if (pMyCDC->GetTextMetrics(&MyTextMetric) != 0)
			{
				// great, we've got the TextMetric structure. now check to see if its
				// a TrueType font or not - only TrueType fonts have Panose information
				if ((MyTextMetric.tmPitchAndFamily & TMPF_TRUETYPE) == TMPF_TRUETYPE)
				{
					// its a TrueType font, so get the Panose number and run
					INT32 Value;
					OUTLINETEXTMETRIC *pOutlineTextMetric;

					// find out how much space we need for the information
					Value = pMyCDC->GetOutlineTextMetrics(NULL, NULL);

					// claim a block of memory at least this size
					pOutlineTextMetric = (OUTLINETEXTMETRIC *) malloc(Value);
					ERROR2IF(pOutlineTextMetric==NULL, FALSE, "Out of memory.");

					// now get the OutlineTextMetric structure
					Value = pMyCDC->GetOutlineTextMetrics(Value, pOutlineTextMetric);
					
					ERROR3IF(Value==FALSE, "Unable to retrieve OUTLINETEXTMETRIC information");

					// hurray, we've finally found what we were looking for - celebrate! ;)
					// now copy the information into the CCPanose strucure
					pPanose->SetFamilyType(pOutlineTextMetric->otmPanoseNumber.bFamilyType); 
					pPanose->SetSerifStyle(pOutlineTextMetric->otmPanoseNumber.bSerifStyle); 
					pPanose->SetWeight(pOutlineTextMetric->otmPanoseNumber.bWeight); 
					pPanose->SetProportion(pOutlineTextMetric->otmPanoseNumber.bProportion); 
					pPanose->SetContrast(pOutlineTextMetric->otmPanoseNumber.bContrast); 
					pPanose->SetStrokeVariation(pOutlineTextMetric->otmPanoseNumber.bStrokeVariation); 
					pPanose->SetArmStyle(pOutlineTextMetric->otmPanoseNumber.bArmStyle); 
					pPanose->SetLetterform(pOutlineTextMetric->otmPanoseNumber.bLetterform); 
					pPanose->SetMidline(pOutlineTextMetric->otmPanoseNumber.bMidline); 
					pPanose->SetXHeight(pOutlineTextMetric->otmPanoseNumber.bXHeight); 
					
					// finally free the memory we had claimed
					free((void *) pOutlineTextMetric);
				}
				else
				{
					// its not a TrueType font, so set the Panose number to some safe value
					pPanose->SetAllToAny();
				}

			}
			else
			{
				ERROR3("Unable to retrieve TEXTMETRIC structure from CDC.");
				ok = FALSE;
			}

			pMyCDC->SelectObject(pOldCFont);
		
			
		}
		else
		{
			ERROR3("Unable to select CFont object into the CDC.");
			ok = FALSE;
		}
	}
	else
	{
		ERROR3("Unable to create font.");
		ok = FALSE;
	}

	delete pNewCFont;

	return ok;
}

