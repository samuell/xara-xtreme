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
// Implementation of the Shadow Tool info bar 

/*
	All new implementation of shadow code new/old produced by Mark Howitt 4/8/98!
*/

#include "camtypes.h"
//#include "nodecont.h"

#ifdef BUILDSHADOWS

#include "shadtool.h"
#include "shadinfo.h"
//#include "shadbar.h"
//#include "shadres.h"
#include "range.h"
//#include "resource.h"
//#include "barsdlgs.h"
//#include "rikdlg.h"
//#include "justin2.h"
//#include "simon.h"			// #define _R(IDS_AND)
#include "docview.h"
#include "document.h"
#include "unittype.h"
#include "optsmsgs.h"
#include "fillattr.h"
#include "attrmgr.h"
#include "pathops.h"
#include "opshadow.h"
#include "opbevel.h"
#include "nodeshad.h"
#include "app.h"
#include "cutop.h"
#include "blobs.h"
#include "camelot.h"
#include "dragmgr.h"
//#include "biasres.h"
#include "biasgain.h"
#include "objchge.h"
#include "biasdlg.h"
#include "nodeblnd.h"
#include "opliveeffects.h"
#include "selector.h"
#include "ophist.h"

DECLARE_SOURCE( "$Revision$" );
CC_IMPLEMENT_DYNCREATE(SoftShadowInfoBarOp,InformationBarOp)
CC_IMPLEMENT_DYNCREATE(ShadowDragData,ListItem)

#define new CAM_DEBUG_NEW

// constants used by the infobar controls
const	INT32			PositionShadowLimit	= 999;
const	INT32			PenumbraSizeLimit	= 999;
const	INT32			ShadowSizeLimit		= 999;
		MILLIPOINT	IncPositionShadow	= 750;	// 1 Pixel
const	MILLIPOINT	IncPenumbraSize		= 750;	// 1 Pixel


const INT32		CONTRASTSLIDERMAX	= 100;
const INT32		BLURSLIDERMAX		= 40 * 750; // 40 pixels
const double	ANGLESLIDERMAX		= 69;		// 69 degrees
const double	SIZESLIDERMAX		= 200;
const float		SHADOWHEIGHTMAX		= 1.5;		// times object height
const INT32		WIDTHSLIDERMAX		= 250000;

// static members of the SoftShadowInfoBarData class
InformationBarOp* SoftShadowInfoBarOp::pSoftShadowInfoBar	= NULL;
Document*         SoftShadowInfoBarOp::pDoc					= NULL;

//////////////////////////////////////////////////////////////
// Profile for the logarithmic width slider
CProfileBiasGain GlowSliderProfile((AFp)0.7, (AFp)0.0);

/********************************************************************************************

>	SoftShadowInfoBarOp::SoftShadowInfoBarOp()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/96
	Purpose:	Sets the info bars vars to NULL defaults

********************************************************************************************/

SoftShadowInfoBarOp::SoftShadowInfoBarOp()
{
	pSoftShadowTool = NULL;
	CurrentSpread = NULL;
	CurrentNodePath = NULL;

	// Used to know whether it's the Apply button or the Remove button in the InfoBar
	ApplyButton = TRUE;	

	m_bControlsExist = FALSE;

	m_bBumping = FALSE;
	
	DlgResID = _R(IDD_SOFTSHADOWTOOLBAR);
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::InitControls()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialise the slider Info Bar control 
	
********************************************************************************************/

void SoftShadowInfoBarOp::Init()
{
	// current info bar public for callbacks arg..
	pSoftShadowInfoBar = this;

	//  The slider (Shadow Transparency)
	// Set the range of the control
	pSoftShadowInfoBar->SetGadgetRange(_R(IDC_SOFTSHADOWSLIDER), 0, CONTRASTSLIDERMAX, 1);
	// Gives the 3d bitmap to the slider, otherwise it has the appearance of a Win 95 slider (2d)
	pSoftShadowInfoBar->SetGadgetBitmaps(_R(IDC_SOFTSHADOWSLIDER), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));

	pSoftShadowInfoBar->SetGadgetRange(_R(IDC_SOFTSHADOWSLIDER), 0, CONTRASTSLIDERMAX, 1);
	pSoftShadowInfoBar->SetGadgetBitmaps(_R(IDC_SOFTSHADOWTRANSPSLIDER), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));

	// now load in the combo text entries
	String_64 Str;

	UpdateGadgetBlurSlider();
	UpdateGadgetTranspSlider();

PORTNOTE("BiasGain", "Removed use of CBiasGainGadget in SoftShadowInfoBarOp")
#if !defined(EXCLUDE_FROM_XARALX)
	BiasGainGadget_m.LinkControlButton( this, _R(IDC_BIASGAIN), _R(IDBBL_BIASGAIN),  _R(IDS_BIASGAINDLG) );
	BiasGainGadget_m.ToggleFillProfile ();
#endif
}


/********************************************************************************************

>	BOOL SoftShadowInfoBarOp::ConvertValueToString(String_256 &In, const INT32 value, UnitType type, String_256 &unit)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/12/96
	Inputs:		The value to convert to a string (In)
	Outputs:	-
	Returns:	-
	Purpose:	TRUE if the function was able to convert the INT32 to a string

********************************************************************************************/

BOOL SoftShadowInfoBarOp::ConvertValueToString(String_256 &In, const INT32 value, UnitType type, TCHAR *unit)
{
	BOOL Converted = FALSE;	// True when a value has been converted
	Range	Selection(*(GetApplication()->FindSelection()));
	RangeControl rg = Selection.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Selection.SetRangeControl(rg);

	if (Document::GetCurrent() == NULL)
		return FALSE;

	Node*	pNode = Selection.FindFirst();

	while (pNode != NULL/* && Selection != NULL*/)
	{
		CurrentSpread = pNode->FindParentSpread();

		if (CurrentSpread != NULL)
		{
			// Get the dimension scaling object (units) associated with the given spread
			// and convert to its units.
			DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
//			UnitType Type = pDimScale->GetUnits();
			pDimScale->ConvertToUnits((INT32)value , &In);
			// In += unit;
				Converted = TRUE;
		}
		else
		{ 
			//TODO ??
		}
	
		pNode = Selection.FindNext(pNode);
	}

	return Converted;
		
}


/********************************************************************************************

>	BOOL SoftShadowInfoBarOp::ConvertStringToValue(const String_256 &Out, MILLIPOINT& value)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/12/96
	Inputs:		The value to convert to a string (In)
	Outputs:	-
	Returns:	-
	Purpose:	TRUE if the function was able to convert the INT32 to a string

********************************************************************************************/

BOOL SoftShadowInfoBarOp::ConvertStringToValue(CGadgetID ID, MILLIPOINT& Value)
{
	BOOL Valid = FALSE;	// True when a value has been converted

	Range	Selection(*(GetApplication()->FindSelection()));
	RangeControl rg = Selection.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Selection.SetRangeControl(rg);
	Node*	pNode = Selection.FindFirst();

	BOOL bContinue = FALSE;

	while (pNode != NULL/* && Selection != NULL*/)
	{	
		CurrentSpread = pNode->FindParentSpread();

		if (CurrentSpread != NULL)
		{
			// Get the dimension scaling object (units) associated with the given spread
			// and convert to its units.
			DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
			//String_128 FieldContents = GetStringGadgetValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_X), &Valid);
			String_128 FieldContents = GetStringGadgetValue(ID, &Valid);
			String_128 NumberString;

			bContinue = TRUE;
			/*

			for (INT32 i = 0 ; i < FieldContents.Length() && bContinue; i++)
			{
				if (FieldContents[i] == '-' || FieldContents == '.' ||
					(FieldContents[i] >= '0' && FieldContents[i] <= '9'))
				{
					NumberString += FieldContents[i];
				}
				else
				{
					bContinue = FALSE;
				}
			}

			NumberString += "pix";
		
			Valid = pDimScale->ConvertToMillipoints(&NumberString, &Value);
				*/

			Valid = pDimScale->ConvertToMillipoints(FieldContents, &Value);

		}
		else
		{ 
			//TODO ??
		}

		pNode = Selection.FindNext(pNode);
	}

	return Valid;
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::UpdateGadgetPositionShadowX()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		TRUE if several selected shadow => display "Many" in the InfoBar
	Outputs:	-
	Returns:	-
	Purpose:	Update the PositionShadowX field

********************************************************************************************/

void SoftShadowInfoBarOp::UpdateGadgetPositionShadowX()
{
	String_256 In(_T("0 pix"));

	NodeShadowController* pMaster = pSoftShadowTool->GetShadowEditField(smOFFSETX, &In);
	if (pMaster)
		ConvertValueToString(In, pMaster->GetOffsetX() , PIXELS, _T(" pix"));

	pSoftShadowInfoBar->SetStringGadgetValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_X), &In);
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::UpdateGadgetPositionShadowY()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		TRUE if several selected shadow => display "Many" in the InfoBar
	Outputs:	-
	Returns:	-
	Purpose:	Update the PositionShadowX field

********************************************************************************************/

void SoftShadowInfoBarOp::UpdateGadgetPositionShadowY()
{
	String_256 In(_T("0pix"));

	NodeShadowController* pMaster = pSoftShadowTool->GetShadowEditField(smOFFSETY, &In);
	if (pMaster)
		ConvertValueToString(In, pMaster->GetOffsetY(), PIXELS, _T(" pix"));

	pSoftShadowInfoBar->SetStringGadgetValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_Y), &In);
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::UpdateGadgetBlurSlider()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/96
	Inputs:		TRUE if several selected shadow => display "Many" in the InfoBar
	Outputs:	-
	Returns:	-
	Purpose:	Update the Slider
				If bUpdateSlider is FALSE, then the slider settings aren't changed - just
				the edit box

********************************************************************************************/

void SoftShadowInfoBarOp::UpdateGadgetBlurSlider(BOOL bUpdateSlider)
{
	// set the gadget range first
	pSoftShadowInfoBar->SetGadgetRange(_R(IDC_SOFTSHADOWSLIDER), 0, CONTRASTSLIDERMAX, 1);

	// Set the position of the slider
	String_256 Str(_T("0%"));

	double d = NodeShadowController::DefaultBlurSize;
	
	NodeShadowController* pMaster = pSoftShadowTool->GetShadowEditField(smPENUMBRA, &Str);
//	if (pMaster==NULL && !SliderDragged)
//	{
//		Str.Load(_R(IDS_SHADOWSTRING_MANY));
//	}
//	else
//	{
		if (pMaster)
		{
			d = pMaster->GetPenumbraWidth();
			ConvertValueToString(Str, (INT32)d, PIXELS, _T(" pix"));
		}
//	}
	
	pSoftShadowInfoBar->SetStringGadgetValue(_R(IDC_SHADOW_SLIDER_EDIT), &Str);
	
	d /= (double)BLURSLIDERMAX;
	//d = 1.0 - d;
	d *= 100;
	
	if (bUpdateSlider)
		pSoftShadowInfoBar->SetLongGadgetValue(_R(IDC_SOFTSHADOWSLIDER), (INT32)d);

	UpdateGadgetHelp();
	
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::UpdateGadgetTranspSlider()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/2000
	Inputs:		TRUE if several selected shadow => display "Many" in the InfoBar
	Outputs:	-
	Returns:	-
	Purpose:	Update the Transparency Slider
				If bUpdateSlider is FALSE, then the slider settings aren't changed - just
				the edit box

********************************************************************************************/

void SoftShadowInfoBarOp::UpdateGadgetTranspSlider(BOOL bUpdateSlider)
{
	// set the gadget range first
	pSoftShadowInfoBar->SetGadgetRange(_R(IDC_SOFTSHADOWTRANSPSLIDER), 0, CONTRASTSLIDERMAX, 1);

	// Set the position of the slider
	String_256 Str(_T("0%"));
	double dDarkness = 0;

	NodeShadowController* pMaster = pSoftShadowTool->GetShadowEditField(smDARKNESS, &Str);
	if (pMaster)
	{
		dDarkness = pMaster->GetShadow()->GetDarkness();
		SetDarknessEditGadget(dDarkness);
	}
	else
		pSoftShadowInfoBar->SetStringGadgetValue(_R(IDC_SHADOWTRANSP_EDIT), &Str);

	if (bUpdateSlider)
		SetDarknessSlider(dDarkness);

	UpdateGadgetHelp();	
}


/********************************************************************************************

>	void SoftShadowInfoBarOp::UpdateGadgetPenumbraSize()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/96
	Inputs:		TRUE if several selected shadow => display "Many" in the InfoBar
	Outputs:	-
	Returns:	-
	Purpose:	Update the PenumbraSize field

********************************************************************************************/

void SoftShadowInfoBarOp::UpdateGadgetPenumbraSize()
{	
	
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::UpdateAllGadgets()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/96
	Inputs:		-
	Outputs:	-
	Returns:	- 
	Purpose:	Update all the gadgets

********************************************************************************************/

void SoftShadowInfoBarOp::UpdateAllGadgets()
{
	UpdateGadgetPositionShadowX();
	UpdateGadgetPositionShadowY();
	UpdateGadgetBlurSlider();
	UpdateGadgetTranspSlider();
	UpdateGadgetPenumbraSize();
	UpdateGadgetTypeButtons();
	UpdateGadgetHelp();
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::UpdateInfoBar()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/12/96
	Inputs:		-
	Outputs:	-
	Returns:	- 
	Purpose:	Update the InfoBar fields according to the selected shadow(s)

********************************************************************************************/

void SoftShadowInfoBarOp::UpdateInfoBar()
{
	BOOL bNothingSelected = (GetApplication()->FindSelection()->FindFirst()==NULL);

	// Update the InfoBar fields according the selection
	NodeShadowController* pMaster = NULL;
	pSoftShadowTool->GetShadowEditRange(&pMaster);
	if (pMaster==NULL)
	{
		UpdateAllGadgets();

		if (bNothingSelected)
			EnableGadgets(FALSE);
		else
		{
			// Grey all controls apart from the button
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_EDIT_X),FALSE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_EDIT_Y),FALSE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_X_LESS) ,FALSE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_X_MORE) ,FALSE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_Y_LESS) ,FALSE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_Y_MORE) ,FALSE);
			
			// the following are enabled anyway to enable users to set a blur &
			// create a shadow
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWSLIDER) ,TRUE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SHADOW_SLIDER_EDIT), TRUE);

			// same for the transparency
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWTRANSPSLIDER), TRUE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SHADOWTRANSP_EDIT), TRUE);		
		}
	}
	else 
	{
		// Retrieve all values (position, size, transparency) of the shadow from the object
		// and then set in the InfoData
//		GetSetInfoBarValue(pSoftShadowTool->GetEditableShadowLevel()->pPPNode);
		UpdateAllGadgets();
		EnableGadgets(TRUE);
	}

	UpdateGadgetHelp();
}


/********************************************************************************************

>	void SoftShadowInfoBarOp::DoChangeShadPos(INT32 x, INT32 y, BOOL PosX, BOOL PosY)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/96
	Inputs:		Whether to update the X or the Y (or both) variables
	Outputs:	-
	Returns:	- 
	Purpose:	Update the Doc and the view according to the new pos of the shadow

********************************************************************************************/

void SoftShadowInfoBarOp::DoChangeShadPos(INT32 x, INT32 y, BOOL PosX, BOOL PosY)
{
	ShadowPosition ShadPos(x, y, PosX, PosY, pSoftShadowTool);

	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SHADOWPOSITION);
	if (pOpDesc != NULL)
	{
		String_256 UIDesc;
		OpState	State = OpShadowPosition::GetState(&UIDesc, pOpDesc);
		if (!State.Greyed)
			pOpDesc->Invoke(&ShadPos);
	}
}


/********************************************************************************************

>	void SoftShadowInfoBarOp::EnableGadgets(BOOL Enable)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/96
	Inputs:		FALSE if all controls have to be greyed, TRUE if not
	Outputs:	-
	Returns:	-
	Purpose:	Called to enable or disable all controls in the InfoBar
	
********************************************************************************************/

void SoftShadowInfoBarOp::EnableGadgets(BOOL Enable)
{
	if(pSoftShadowInfoBar== NULL)
		return;
	if(!pSoftShadowInfoBar->HasWindow())
		return;

	if (!Enable)
	{
		pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_EDIT_X), FALSE);
		pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_EDIT_Y), FALSE);
		pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_X_LESS), FALSE);
		pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_X_MORE), FALSE);
		pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_Y_LESS), FALSE);
		pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_Y_MORE), FALSE);
		pSoftShadowInfoBar->EnableGadget(_R(IDC_BIASGAIN), FALSE);

		// set all the gadgets to not being selected
//		pSoftShadowInfoBar->SetBoolGadgetSelected(_R(IDC_BTN_NOSHADOW), FALSE);
//		pSoftShadowInfoBar->SetBoolGadgetSelected(_R(IDC_BTN_WALLSHADOW), FALSE);
//		pSoftShadowInfoBar->SetBoolGadgetSelected(_R(IDC_BTN_FLOORSHADOW), FALSE);
//		pSoftShadowInfoBar->SetBoolGadgetSelected(_R(IDC_BTN_GLOWSHADOW), FALSE);
	}
	else
	{
		NodeShadowController* pMaster = pSoftShadowTool->GetShadowEditField(smTYPE);
		ShadowType stype = SHADOWTYPE_NONE;
		if (pMaster)
			stype = pMaster->GetShadowType();

		pSoftShadowInfoBar->EnableGadget(_R(IDC_BIASGAIN), (stype != SHADOWTYPE_NONE));
		
		if (stype == SHADOWTYPE_NONE ||
			stype == SHADOWTYPE_FLOOR ||
			stype == SHADOWTYPE_GLOW)
		{
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_EDIT_X), FALSE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_EDIT_Y), FALSE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_X_LESS), FALSE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_X_MORE), FALSE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_Y_LESS), FALSE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_Y_MORE), FALSE);
//			pSoftShadowInfoBar->EnableGadget(_R(IDC_BTN_NOSHADOW), TRUE);
		}
		else
		{
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_EDIT_X), TRUE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_EDIT_Y), TRUE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_X_LESS), TRUE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_X_MORE), TRUE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_Y_LESS), TRUE);
			pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWCENTRE_Y_MORE), TRUE);
//			pSoftShadowInfoBar->EnableGadget(_R(IDC_BIASGAIN), TRUE);
		}
	}

		
	pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWSLIDER) ,Enable);
	pSoftShadowInfoBar->EnableGadget(_R(IDC_SHADOW_SLIDER_EDIT) ,Enable);
	pSoftShadowInfoBar->EnableGadget(_R(IDC_SOFTSHADOWTRANSPSLIDER), Enable);
	pSoftShadowInfoBar->EnableGadget(_R(IDC_SHADOWTRANSP_EDIT), Enable);		
	
	pSoftShadowInfoBar->EnableGadget(_R(IDC_BTN_NOSHADOW), Enable);
	pSoftShadowInfoBar->EnableGadget(_R(IDC_BTN_WALLSHADOW), Enable);
	pSoftShadowInfoBar->EnableGadget(_R(IDC_BTN_FLOORSHADOW), Enable);
	pSoftShadowInfoBar->EnableGadget(_R(IDC_BTN_GLOWSHADOW), Enable);		

	UpdateGadgetHelp();
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::ChangeShadowPosIncremental(INT32 incx, INT32 incy)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	find out the new value of PositionShadowX and update the control
	
********************************************************************************************/

void SoftShadowInfoBarOp::ChangeShadowPosIncremental(INT32 incx, INT32 incy)
{
	Document * pDoc = Document::GetCurrent();
	Spread * pSpread = Document::GetSelectedSpread();
	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();
	DocRect BoundsToDraw;
	BOOL Valid = FALSE;
	MILLIPOINT x = 0;
	MILLIPOINT y = 0;

//	String_256 Str = GetStringGadgetValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_X), &Valid);
//	if (Str == ManyStr)
//		incx = 0;
//
//	Str = GetStringGadgetValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_Y), &Valid);
//	if (Str == ManyStr)
//		incy = 0;
//
	// This isn't strictly needed but makes the function consistent
	// with greying and DoChangeShadowPosIncremental.
	// It prevents the increments being applied to inconsistent shadows
	Valid = ConvertStringToValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_X), x);
	if (!Valid)
		incx = 0;
	Valid = ConvertStringToValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_Y), y);
	if (!Valid)
		incy = 0;

	if (incx==0 && incy==0)
		return;

	NodeShadowController* pMaster = NULL;
	ListRange* pEditRange = pSoftShadowTool->GetShadowEditRange(&pMaster);
	if (pEditRange)
	{
		Node* pNode = pEditRange->FindFirst();
		while (pNode)
		{
			ENSURE(pNode->IsAShadowController(), "Found unexpected node type in shadow stack level");
			NodeShadowController* pControl = (NodeShadowController*)pNode;
//			NodeShadow * pShadow = pControl->GetShadow();
			pControl->ReleaseCached(TRUE, FALSE, FALSE, TRUE);

			if (pControl->GetShadowType() == SHADOWTYPE_WALL)
			{
				BoundsToDraw = BoundsToDraw.Union(pControl->GetBoundingRect());

				DocCoord offset = pControl->GetWallShadowOffset();
				offset.x += incx;
				offset.y += incy;
				pControl->SetWallShadowOffset(offset, TRUE);

				DocRect dr = pControl->GetInsideBoundingRect();
				DocRect shadRect = dr;
				
				dr.lo.x += pControl->GetOffsetX();
				dr.hi.x += pControl->GetOffsetX();
				dr.lo.y += pControl->GetOffsetY();
				dr.hi.y += pControl->GetOffsetY();
				dr.Inflate(pControl->GetPenumbraWidth() + pBlobMgr->GetBlobSize());
				
				BoundsToDraw = BoundsToDraw.Union(dr);
				BoundsToDraw = BoundsToDraw.Union(shadRect);
			}

			pNode = pEditRange->FindNext(pNode);
		}

		UpdateGadgetPositionShadowX();
		UpdateGadgetPositionShadowY();

		if (pDoc && pSpread && BoundsToDraw.IsValid() && !BoundsToDraw.IsEmpty())
		{
			pDoc->ForceRedraw(pSpread, BoundsToDraw);
			GetApplication()->ServiceRendering();			
		}
	}
}


/********************************************************************************************

>	void SoftShadowInfoBarOp::DoChangeShadowPosIncremental(MILLIPOINT incx, MILLIPOINT incy)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/2005
	Inputs:		incx - the x increment
				incy - the y increment
	Outputs:	-
	returns		-
	Purpose:	Undoably change the positions of the shadows in the current edit range

*********************************************************************************************/

void SoftShadowInfoBarOp::DoChangeShadowPosIncremental(MILLIPOINT incx, MILLIPOINT incy)
{
	BOOL Valid = FALSE;
	MILLIPOINT x = 0;
	MILLIPOINT y = 0;
	String_256 ManyStr;
	ManyStr.Load(_R(IDS_SHADOWSTRING_MANY));

//	String_256 Str = GetStringGadgetValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_X), &Valid);
//	if (Str == ManyStr)
//		incx = 0;
//
//	Str = GetStringGadgetValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_Y), &Valid);
//	if (Str == ManyStr)
//		incy = 0;
//
	Valid = ConvertStringToValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_X), x);
	if (!Valid)
		incx = 0;
	Valid = ConvertStringToValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_Y), y);
	if (!Valid)
		incy = 0;

	if (incx==0 && incy==0)
		return;

	ShadowPosition ShadPos(x + incx, y + incy, incx!=0, incy!=0, pSoftShadowTool);

	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SHADOWPOSITION);
	if (pOpDesc != NULL)
	{
		String_256 UIDesc;
		OpState	State = OpShadowPosition::GetState(&UIDesc, pOpDesc);
		if (!State.Greyed)
			pOpDesc->Invoke(&ShadPos);
	}

	UpdateGadgetPositionShadowX();
	UpdateGadgetPositionShadowY();
}


/********************************************************************************************

>	virtual void SoftShadowInfoBarOp::HandleProfileSelChangingMsg(CBiasGainGadget& Gadget, CGadgetID ProfileGadgetID)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/2000
	Inputs:		CBiasGainGadget& and its GadgetID (for added safety)
	Outputs:	-
	Returns:	-
	Purpose:	Handles the updating/closing of the profile gadgets corrensponding profile
				dialog when a SelChangingMsg is received.	Errors:		-
	SeeAlso:	InformationBarOp::GetProfileFromSelection()

********************************************************************************************/

void SoftShadowInfoBarOp::HandleProfileSelChangingMsg(CBiasGainGadget& Gadget, CGadgetID GadgetID)
{
PORTNOTE("BiasGain", "Removed use of CBiasGainGadget in SoftShadowInfoBarOp")
#if !defined(EXCLUDE_FROM_XARALX)
//	BOOL ok = (GadgetID == _R(IDC_BIASGAIN));

//	ERROR2IF(ok==FALSE, FALSE, "Invalid gadgetID passed");
	
	BOOL bNone = TRUE;
	BOOL bConsistent = FALSE;
	BOOL bMany = FALSE;
	CProfileBiasGain* pProfile = NULL;
	NodeShadowController* pMasterProfile = pSoftShadowTool->GetShadowEditField(smBIASGAIN, &bNone, &bConsistent, &bMany);

	if (pMasterProfile)
	{
		NodeShadow* pShadow = pMasterProfile->GetShadow();
		ERROR3IF(pShadow==NULL, "GetProfileFromSelection");

		pProfile = pShadow->GetBiasGainPtr();
	}

	if (Gadget.IsDialogOpen())
	{
		if (bNone && !SelectorTool::GetNormalClick_CheckProfileDialog())
		{
			Gadget.CloseDialog();
			return;
		}

		Gadget.ReInitialiseDialog(pProfile, bMany);
		EnableGadget(GadgetID, FALSE);
	}
	else
	{
		EnableGadget(GadgetID, !bNone);
	}
#endif
}


/********************************************************************************************

>	virtual CProfileBiasGain* SoftShadowInfoBarOp::GetProfileFromSelection(CGadgetID GadgetID, BOOL* bMany, BOOL* bAllSameType)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Snook
	Created:	19/1/2000
	Inputs:		The GadgetID of the CBiasGainGadget that we are dealing with.
	Outputs:	bMany - returned as TRUE if we have MANY profiles selected.
				bAllSameType - returned as TRUE if objects within selection are all of the
				same type.
	returns		Ptr to common CProfileBiasGain, or NULL if there is NOT one.
	Purpose:	See InformationBarOp::GetProfileFromSelection () for a description of this
				function.

*********************************************************************************************/

CProfileBiasGain* SoftShadowInfoBarOp::GetProfileFromSelection(CGadgetID GadgetID, BOOL* bMany, BOOL* bAllSameType)
{
	BOOL ok = (GadgetID == _R(IDC_BIASGAIN));

	ERROR2IF(ok==FALSE, FALSE, "Invalid gadgetID passed");
	
	BOOL bProfileNone = TRUE;
	BOOL bProfileConsistent = FALSE;
	BOOL bProfileMany = FALSE;
	NodeShadowController* pMasterProfile = pSoftShadowTool->GetShadowEditField(smBIASGAIN, &bProfileNone, &bProfileConsistent, &bProfileMany);
//	NodeShadowController* pMasterType = pSoftShadowTool->GetShadowEditField(smTYPE);

	*bMany = bProfileMany || bProfileNone;
	*bAllSameType = bProfileConsistent || bProfileNone;

	if (pMasterProfile)
	{
		NodeShadow* pShadow = pMasterProfile->GetShadow();
		ERROR3IF(pShadow==NULL, "GetProfileFromSelection");

		return pShadow->GetBiasGainPtr();
	}

	return NULL;
}


/********************************************************************************************

>	void SoftShadowInfoBarOp::DoPenumbraSizeBumps()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/11/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	find out the new value of PenumbraSize and update the control
	
********************************************************************************************/

void SoftShadowInfoBarOp::DoPenumbraSizeBumps(UINT32 Button)
{
}


/********************************************************************************************

>	MsgResult SoftShadowInfoBarOp::Message(Msg* Message) 

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		Message = The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Shadow Tool info bar dialog message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult SoftShadowInfoBarOp::Message(Msg* Message) 
{

	if (!SoftShadowTool::IsCurrentTool() || !IsVisible())
    	return (InformationBarOp::Message(Message));

//	INT32 SliderType = 0;

	String_256 ManyStr;
	ManyStr.Load(_R(IDS_SHADOWSTRING_MANY));

	CProfileBiasGain Profile;
//	BOOL bMany = FALSE;

	BOOL Valid = FALSE;
//	INT32 Result = 0;
//	double d = 0;

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		// Check if the message is a CANCEL
		if (Msg->DlgMsg == DIM_CANCEL)
		{
			m_bControlsExist = FALSE;

			Close(); // Close the dialog 
		}
		else if (Msg->DlgMsg == DIM_CREATE)
		{
			SliderDragged = FALSE;

			// Initialise the infobar controls here
			Init();

			UpdateInfoBar();

			m_bControlsExist = TRUE;
		}
		else
		{
			// Note: Can't use switch because of the nature of _R()

			// Position (X and Y) of centre of shadow
			// First handle the edit field of position X then position Y
			if (Msg->GadgetID == _R(IDC_SOFTSHADOWCENTRE_EDIT_X) || Msg->GadgetID == _R(IDC_SOFTSHADOWCENTRE_EDIT_Y))
			{
				if (Msg->DlgMsg == DIM_SELECTION_CHANGED)	// A user has typed in something
				{
					BOOL Valid = FALSE;
					String_256 Str = GetStringGadgetValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_X),&Valid);

					INT32 PosShadowX = 0;
					// Parse the edit field string and then convert the value
					// Return true or false
					Valid = ConvertStringToValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_X),PosShadowX);

					if (Str != ManyStr)
					{

						if(Valid)	// Ok, we can set the internal value and then update
						{
							// Update and apply the new value to the object in the view and in the doc
							DoChangeShadPos(PosShadowX, 0, TRUE, FALSE);
						}
						else
						{
							// Error message
							DoInputError(Msg->GadgetID);
							UpdateGadgetPositionShadowX();
						}
					}

					// Now deal with the position Y of the centre of the shadow

					Str = GetStringGadgetValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_Y),&Valid);
					INT32 PosShadowY = 0;

					// Parse the edit field string and then convert the value
					// Return true or false
					if (Str != ManyStr)
					{
						Valid = ConvertStringToValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_Y),PosShadowY);

						if(Valid)	// Ok, we can set the internal value and then update
						{
							// Update and apply the new value to the object in the view and in the doc
							DoChangeShadPos(0, PosShadowY, FALSE, TRUE);
						}
						else
						{
							// Error message
							DoInputError(Msg->GadgetID);
							UpdateGadgetPositionShadowY();
						}
					}
				}
			}

			else if (Msg->GadgetID == _R(IDC_BIASGAIN))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_LFT_BN_CLICKED:
					{
PORTNOTE("BiasGain", "Removed use of CBiasGainGadget in SoftShadowInfoBarOp")
#if !defined(EXCLUDE_FROM_XARALX)
						HandleProfileButtonClick (BiasGainGadget_m, _R(IDC_BIASGAIN));
#endif
					}
					break;
					default:
						ProfileSelectionChange( Msg, Msg->GadgetID );
					break;
				}
			}

			else if (Msg->GadgetID == _R(IDC_SOFTSHADOWCENTRE_Y_LESS))
				HandleShadowPosBump(Msg, 0, -Document::GetCurrent()->GetDocNudge());

			else if (Msg->GadgetID == _R(IDC_SOFTSHADOWCENTRE_Y_MORE))
				HandleShadowPosBump(Msg, 0, Document::GetCurrent()->GetDocNudge());

			else if (Msg->GadgetID == _R(IDC_SOFTSHADOWCENTRE_X_LESS))
				HandleShadowPosBump(Msg, -Document::GetCurrent()->GetDocNudge(), 0);

			else if (Msg->GadgetID == _R(IDC_SOFTSHADOWCENTRE_X_MORE))
				HandleShadowPosBump(Msg, Document::GetCurrent()->GetDocNudge(), 0);

				//////////////////////////////
				// the transparency slider
			else if (Msg->GadgetID == _R(IDC_SOFTSHADOWTRANSPSLIDER))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SLIDER_POS_CHANGING:
					{
						if (!SliderDragged)
						{
							SaveShadowDragData(&ShadowDataList);
							Operation::SetQuickRender(TRUE);
						}

						Valid = FALSE;
						double dDarkness = GetDarknessSlider(&Valid);
						SetDarknessEditGadget(dDarkness);

						ChangeShadowDarkness(dDarkness);

						PaintGadgetNow(_R(IDC_SHADOWTRANSP_EDIT));
						SliderDragged = TRUE;
					}
					break;

					case DIM_SLIDER_POS_SET:
					{
						Operation::SetQuickRender(FALSE);

						// restore the original values
						RestoreShadowDragData(&ShadowDataList, FALSE);

						// Find the current scrollers position
						SliderDragged = FALSE;
						double dDarkness = GetDarknessSlider(&Valid);
						SetDarknessEditGadget(dDarkness);

						DoChangeShadowDarkness(dDarkness);

						SliderDragged = FALSE;

						SetBitmapUpdate(TRUE);
					}
					break;

					default:
					{
						// Do nothing
						break;
					}
				}
			}

				////////////////////////////////
				// The shadow tool slider
			else if (Msg->GadgetID == _R(IDC_SOFTSHADOWSLIDER))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SLIDER_POS_CHANGING:
					{
						if (!SliderDragged)
						{
							SaveShadowDragData(&ShadowDataList);
							Operation::SetQuickRender(TRUE);
						}

						SetBitmapUpdate(FALSE);

						// Find the current scrollers position
						BOOL Valid = FALSE;
						INT32 Result = GetLongGadgetValue(_R(IDC_SOFTSHADOWSLIDER), 0, CONTRASTSLIDERMAX, 0, &Valid);

						NodeShadowController* pMaster = NULL;
						pSoftShadowTool->GetShadowEditRange(&pMaster);
						if (pMaster!=NULL)
						{
							ChangeShadowPenumbra((MILLIPOINT)((((double)(/*100 - */Result)) / 100.0) * ((double)BLURSLIDERMAX)));

							UpdateGadgetBlurSlider(FALSE);
						}

						PaintGadgetNow(_R(IDC_SHADOW_SLIDER_EDIT));

						SliderDragged = TRUE;

						break;
					}

					case DIM_SLIDER_POS_SET:
					{
						Operation::SetQuickRender(FALSE);

						// restore the original values
						RestoreShadowDragData(&ShadowDataList, FALSE);

						// Find the current scrollers position
						BOOL Valid;
						SliderDragged = FALSE;
						INT32 Result = GetLongGadgetValue(_R(IDC_SOFTSHADOWSLIDER), 0, CONTRASTSLIDERMAX, 0, &Valid);

						DoChangePenumbra((MILLIPOINT)((((double)(/*100 - */Result)) / 100.0) * ((double)BLURSLIDERMAX)));
						SliderDragged = FALSE;

						SetBitmapUpdate(TRUE);
						break;
					}

					default:
					{
						// Do nothing
						break;
					}
				}
			}

			else if (Msg->GadgetID == _R(IDC_SHADOW_SLIDER_EDIT))
			{
				if (Msg->DlgMsg == DIM_SELECTION_CHANGED)
				{
					// handle the edit control changing
					HandleSliderEditChange();
				}
			}
			else if (Msg->GadgetID == _R(IDC_SHADOWTRANSP_EDIT))
			{
				if (Msg->DlgMsg == DIM_SELECTION_CHANGED)
				{
					// handle the edit control changing
					HandleSliderTranspEditChange();
				}
			}
			else if (Msg->GadgetID == _R(IDC_BTN_NOSHADOW))
			{
				if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
				{
					HandleNoShadowBtnClicked();
					return OK;
				}
			}
			else if (Msg->GadgetID == _R(IDC_BTN_WALLSHADOW))
			{
				if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
				{
					HandleWallShadowBtnClicked();
					return OK;
				}
			}
			else if (Msg->GadgetID == _R(IDC_BTN_FLOORSHADOW))
			{
				if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
				{
					HandleFloorShadowBtnClicked();
					return OK;
				}
			}
			else if (Msg->GadgetID == _R(IDC_BTN_GLOWSHADOW))
			{
				if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
				{
					HandleGlowShadowBtnClicked();
					return OK;
				}
			}
		}
	}

	else if (MESSAGE_IS_A(Message,OpMsg)) 	// Check for undo/redo
	{
		if ( (((OpMsg*)Message)->MsgType == OpMsg::AFTER_UNDO) ||
										(((OpMsg*)Message)->MsgType == OpMsg::AFTER_REDO) )
		{
			if (pSoftShadowInfoBar != NULL)
			{
//				if (pSoftShadowTool != NULL)
//					pSoftShadowTool->SelectionHasChanged();

				// update the gadgets
				UpdateInfoBar();

			}
		}
	}
	// Does this message mean that the selected object has changed?
	else if (MESSAGE_IS_A(Message, SelChangingMsg))
	{
		// Is it the current tool ?
		if (pSoftShadowInfoBar==NULL || !pSoftShadowInfoBar->HasWindow())
			return InformationBarOp::Message(Message);

		// Selection has changed so we must update our ideas about the
		// selected shadow range here (before profile dialog is updated)
		pSoftShadowTool->SetShadowEditRangeFromSelection(FALSE);

		// Tell the tool
//		if (pSoftShadowTool != NULL)
//			pSoftShadowTool->SelectionHasChanged();

PORTNOTE("BiasGain", "Removed use of CBiasGainGadget in SoftShadowInfoBarOp")
#if !defined(EXCLUDE_FROM_XARALX)
		HandleProfileSelChangingMsg(BiasGainGadget_m, _R(IDC_BIASGAIN));
#endif

		if (SliderDragged)
		{
			// this means that the selection has changed inside of a drag
			// therefore, restore the original values to the selection
			RestoreShadowDragData(&ShadowDataList, TRUE);
			SliderDragged = FALSE;
		}							

		// Enable gadget according the selection
		Range Sel(*(GetApplication()->FindSelection()));
		RangeControl rg = Sel.GetRangeControlFlags();
		rg.PromoteToParent = TRUE;
		Sel.SetRangeControl(rg);
	
		UINT32 NumSelected = Sel.Count();

		// Nothing selected
		if(NumSelected == 0)
			EnableGadgets(FALSE);
		else
		{
			EnableGadgets(TRUE);

			// The user may have clicked on a shadow, so we must attempt to set up
			// valid m_pEditRange from the selection here before
			// UpdateInfobar decides that it must find a TopClassRange
//			if (pSoftShadowTool)
//				pSoftShadowTool->SetShadowEditRangeFromSelection();
		}

		// Update the InfoBar
		UpdateInfoBar();
	}
	else if(MESSAGE_IS_A(Message,OptionsChangingMsg))
	{

		OptionsChangingMsg* Msg = (OptionsChangingMsg*)Message;

		if(Msg->State == OptionsChangingMsg::NEWUNITS)
		{
			pDoc = Document::GetSelected();
			
			// update all controls
			UpdateInfoBar();
		}
	}
	
	// Pass the message on to the immediate blank class
	return (InformationBarOp::Message(Message));
}    

void SoftShadowInfoBarOp::HandleShadowPosBump(DialogMsg* dmsg, INT32 incx, INT32 incy)
{
	switch (dmsg->DlgMsg)
	{
		case DIM_LFT_BN_CLICKED:
		{
			if (!m_bBumping)
			{
				Operation::SetQuickRender(TRUE);		// Prevent recacheing while the button is held down
				SaveShadowDragData(&ShadowDataList);
				m_bBumping = TRUE;
			}

			ChangeShadowPosIncremental(incx, incy);		// Non-undoable

		}
		break;

		case DIM_LFT_BN_UP:
		{
			Operation::SetQuickRender(FALSE);			// Re-enable cacheing

			//Update the view and the document
			if (m_bBumping)
			{
				RestoreShadowDragData(&ShadowDataList);
				m_bBumping = FALSE;
			}

			DoChangeShadowPosIncremental(incx, incy);	// Undoable

//			if (pSoftShadowTool)
//				pSoftShadowTool->SelectionHasChanged();							
		}
		break;

		default:
		{
			// Do nothing
		}
		break;
	}
}



/********************************************************************************************

>	void SoftShadowInfoBarOp::ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> Snook
	Created:	17/9/99
	Purpose:	See InformationBarOp::ChangeProfile () for an explanation of this function.
	See Also:	InformationBarOp::ChangeProfile ()

*********************************************************************************************/

void SoftShadowInfoBarOp::ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID)
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SHADOWPROFILE);
	if (pOpDesc != NULL)
	{
		BOOL FireOp = TRUE;

		// we only want to generate one bit of undo information - so decided whether
		// we have to fire the above op, or whether we just 'pump' the values into
		// our nodes (thereby nolonger generating infinite undo information) ....

		Operation* pLastOp = NULL;

		if (Profile->GetGeneratesInfiniteUndo ())	// only do if they didn't select a preset profile
		{
			pLastOp = Document::GetSelected()->GetOpHistory().FindLastOp();
		}

		if (pLastOp)
		{
			if (pLastOp->GetRuntimeClass() == CC_RUNTIME_CLASS(OpChangeShadowProfile))
			{
				FireOp = FALSE;
			}
		}

		if (FireOp == TRUE)
		{
			ShadowProfileOpParam Param(*Profile, pSoftShadowTool);
//			Param.Profile = *Profile;
		
			pOpDesc->Invoke(&Param);
		}
		else
		{
			// we don't need/want any undo information - so just change the value ....

			ChangeProfileAction Action;
			Action.ChangeShadowProfileWithNoUndo(*Profile, pSoftShadowTool);
		}
	}
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::DoInputError(UINT32 GadgetID) 

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/96
	Inputs:		Gadget ID
	Outputs:	-
	Returns:	-
	Purpose:	Make the error string and then display a window
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SoftShadowInfoBarOp::DoInputError(UINT32 GadgetID) 
{
 	String_256 sErrString;
	String_64 sWarnString(_R(IDS_INVALID_INPUT));
	String_8 sAndString(_R(IDS_AND));
	String_16 sMaxStr(_T("0"));
	String_16 sMinStr(_T("0"));

	if (GadgetID == _R(IDC_SOFTSHADOWCENTRE_EDIT_X))
	{
		sMinStr._MakeMsg(TEXT("#1%dpix"),-PositionShadowLimit);
		sMaxStr._MakeMsg(TEXT("#1%dpix"),PositionShadowLimit);
	}

	else if (GadgetID == _R(IDC_SOFTSHADOWCENTRE_EDIT_Y))
	{
		sMinStr._MakeMsg(TEXT("#1%dpix"),-PositionShadowLimit);
		sMaxStr._MakeMsg(TEXT("#1%dpix"),PositionShadowLimit);
	}

	else if (GadgetID == _R(IDC_PENUMBRASIZE_EDIT))
	{
		sMinStr._MakeMsg(TEXT("#1%dpix"),0);
		sMaxStr._MakeMsg(TEXT("#1%dpix"),PenumbraSizeLimit);
	}

	else
		return;
	
	sErrString._MakeMsg(TEXT("#1%s #2%s #3%s #4%s \n"),
										(TCHAR*)sWarnString,
										(TCHAR*)sMinStr,
										(TCHAR*)sAndString,
										(TCHAR*)sMaxStr); 
			
	Error::SetError(0,(TCHAR* )sErrString,0);
	InformError();
}


/********************************************************************************************

>	void SoftShadowInfoBarOp::DoChangeShadowDarkness(double dDarkness)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Purpose:	Called when a the transparency slider is changed.

********************************************************************************************/

void SoftShadowInfoBarOp::DoChangeShadowDarkness(double dDarkness)
{
	// invoke the op
	OpDescriptor* pOpDesc = NULL;

	NodeShadowController* pMaster = NULL;
	pSoftShadowTool->GetShadowEditRange(&pMaster);
	if (pMaster==NULL)
	{
		// if there's no shadow - create one !!
		NodeShadowParam Param;
		Param.m_dDarkness = dDarkness;
		Param.StackPosition = STACKPOS_INSERT_TOP;
		Param.pPPStack = GetApplication()->FindSelection()->GetEffectsStack(TRUE);	// Get a copy while the sel changes
		Param.pShadowTool = pSoftShadowTool;

		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_APPLYSHADOW);

		if (pOpDesc)
		{
			pOpDesc->Invoke(&Param);
		}

		delete Param.pPPStack;

		return;		
	}

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SHADOWDARKNESS);

	ShadowDarknessOpParam Param(dDarkness, pSoftShadowTool);

	if (pOpDesc != NULL)
	{
		pOpDesc->Invoke(&Param);
	}
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::SetBitmapUpdate(BOOL OnOrOff)

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/98
	Purpose:	Turns the bitmap generation process on or off for shadows.

********************************************************************************************/

void SoftShadowInfoBarOp::SetBitmapUpdate(BOOL OnOrOff)
{
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.SetRangeControl(rg);

	// if (pSel != NULL)
	{
		Node* pNode = Sel.FindFirst();
		while (pNode != NULL)
		{
			if(IS_A(pNode,NodeShadowController))
			{
				NodeShadow* pShadow = ((NodeShadowController*)pNode)->GetShadow();
				pShadow->SetBitmapShadowRendering(OnOrOff);
			}
			pNode = Sel.FindNext(pNode);
		}
	}
}


/********************************************************************************************

>	void SoftShadowInfoBarOp::UpdateGadgetHelp()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	10/5/99
	Inputs:		
	Outputs:	-
	Returns:	- 
	Purpose:	Update the combos

********************************************************************************************/
void SoftShadowInfoBarOp::UpdateGadgetHelp()
{
	// do the bubble helps
	SetGadgetHelp(_R(IDC_SOFTSHADOWSLIDER), _R(IDBBL_SHADOWSLIDERBLUR), _R(IDS_SHADOWSLIDERBLURID));
	SetGadgetHelp(_R(IDC_SHADOW_SLIDER_EDIT), _R(IDBBL_SHADOWSLIDERBLUR), _R(IDS_SHADOWSLIDERBLURID));
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::DoChangePenumbra(UINT32 NewPenumbra)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	10/5/99
	Inputs:		The new penumbra
	Outputs:	-
	Returns:	- 
	Purpose:	Change the penumbra size

********************************************************************************************/
void SoftShadowInfoBarOp::DoChangePenumbra(INT32 NewPenumbra)
{
	OpDescriptor* pOpDesc = NULL;
	
	// if there's no shadow in the selection, then create one !
	NodeShadowController* pMaster = NULL;
	pSoftShadowTool->GetShadowEditRange(&pMaster);
	if (pMaster==NULL)
	{
		NodeShadowParam Param;
		Param.m_Blur = NewPenumbra;
		Param.StackPosition = STACKPOS_INSERT_TOP;
		Param.pPPStack = GetApplication()->FindSelection()->GetEffectsStack(TRUE);	// Get a copy while the sel changes
		Param.pShadowTool = pSoftShadowTool;

		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_APPLYSHADOW);

		if (pOpDesc)
		{
			pOpDesc->Invoke(&Param);
		}

		delete Param.pPPStack;

		return;
	}		
	
	ShadowPenumbra Param(NewPenumbra, pSoftShadowTool);

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SHADOWPENUMBRA);
	
	if (pOpDesc != NULL)
	{
		pOpDesc->Invoke(&Param);
	}

//	if (pSoftShadowTool)
//		pSoftShadowTool->SelectionHasChanged();
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::DoChangeWidth(UINT32 NewWidth)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	10/5/99
	Inputs:		The new width
	Outputs:	-
	Returns:	- 
	Purpose:	Change the glow width

********************************************************************************************/
void SoftShadowInfoBarOp::DoChangeWidth(INT32 NewWidth)
{
	NodeShadowParam Param;
	Param.m_GlowWidth = NewWidth;
	Param.pShadowTool = pSoftShadowTool;

	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_GLOWWIDTH);
	
	if (pOpDesc != NULL)
	{
		pOpDesc->Invoke(&Param);
	}

//	if (pSoftShadowTool)
//		pSoftShadowTool->SelectionHasChanged();
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::HandleSliderEditChange()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	10/5/99
	Inputs:		
	Outputs:	-
	Returns:	- 
	Purpose:	Edit control change for the slider

********************************************************************************************/
void SoftShadowInfoBarOp::HandleSliderEditChange()
{
	// first, get the value out of the edit box
	double Value = 0;
//	double d = 0;
	String_256 FieldContents;
	String_256 NumberString;
	BOOL Valid = FALSE;
//	BOOL bNegative = FALSE;
//	INT32 Start = 0;
	INT32 KValue = 0;

	String_256 ManyString;
	ManyString.Load(_R(IDS_SHADOWSTRING_MANY));

	FieldContents = GetStringGadgetValue(_R(IDC_SHADOW_SLIDER_EDIT), &Valid);

//	BOOL bValid = FALSE;

	// check for a many in the box
	BOOL bMany = FALSE;

	if (FieldContents == ManyString)
	{
		bMany = TRUE;
	}

	// this is the blur, so it's ok to go ahead
	if (ConvertStringToValue(_R(IDC_SHADOW_SLIDER_EDIT), KValue) && !bMany)
	{
		Value = KValue;
		if (Value >= 0 && Value <= BLURSLIDERMAX)
		{
			DoChangePenumbra((INT32)Value);
		}
	}

	UpdateInfoBar();
	
	// redraw all the blobs
//	if (pSoftShadowTool)
//	{
//		pSoftShadowTool->SelectionHasChanged();
//	}
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::HandleSliderTranspEditChange()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	10/5/99
	Inputs:		
	Outputs:	-
	Returns:	- 
	Purpose:	Edit control change for the transparency slider

********************************************************************************************/
void SoftShadowInfoBarOp::HandleSliderTranspEditChange()
{
	// get the value out of the edit box
	BOOL bOK = FALSE;
	BOOL bMany = FALSE;
	double dDarkness = 0;

	bOK = GetDarknessEditGadget(dDarkness, bMany);
	if (!bOK || bMany)
	{
		UpdateGadgetTranspSlider(FALSE);
		return;
	}

	// ok, let's do the transparency change
	DoChangeShadowDarkness(dDarkness);
	UpdateInfoBar();
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::DoChangeAngle(double Angle)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	10/5/99
	Inputs:		The angle
	Outputs:	-
	Returns:	- 
	Purpose:	Sets the angle for the selection

********************************************************************************************/
void SoftShadowInfoBarOp::DoChangeAngle(double Angle)
{
	NodeShadowParam Param;
	Param.m_FloorShadowAngle = Angle;
	Param.pShadowTool = pSoftShadowTool;
	
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SHADOWANGLE);
	
	if (pOpDesc != NULL)
	{
		pOpDesc->Invoke(&Param);
	}

//	if (pSoftShadowTool)
//		pSoftShadowTool->SelectionHasChanged();
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::DoChangeHeight(double Height)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	10/5/99
	Inputs:		The new height
	Outputs:	-
	Returns:	- 
	Purpose:	Sets the angle for the selection

********************************************************************************************/
void SoftShadowInfoBarOp::DoChangeHeight(double Height)
{
	NodeShadowParam Param;
	Param.m_FloorShadowHeight = Height;
	Param.pShadowTool = pSoftShadowTool;
	
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SHADOWHEIGHT);
	
	if (pOpDesc != NULL)
	{
		pOpDesc->Invoke(&Param);
	}

//	if (pSoftShadowTool)
//		pSoftShadowTool->SelectionHasChanged();
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::DoChangeScale(double Scale)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	10/5/99
	Inputs:		The new scale
	Outputs:	-
	Returns:	- 
	Purpose:	Sets the shadow scale for the selection

********************************************************************************************/
void SoftShadowInfoBarOp::DoChangeScale(double Scale)
{
	NodeShadowParam Param;
	Param.m_Scale = Scale;
	Param.pShadowTool = pSoftShadowTool;
	
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SHADOWSCALE);
	
	if (pOpDesc != NULL)
	{
		pOpDesc->Invoke(&Param);
	}

//	if (pSoftShadowTool)
//		pSoftShadowTool->SelectionHasChanged();
}



/********************************************************************************************

>	void SoftShadowInfoBarOp::ChangeShadowDarkness(double dDarkness)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	- 
	Purpose:	Directly sets transparency value into the selected shadows

********************************************************************************************/
void SoftShadowInfoBarOp::ChangeShadowDarkness(double dDarkness)
{
	if (ShadowDataList.IsEmpty())
	{
		return;
	}

	ShadowDragData* pItem = (ShadowDragData*)ShadowDataList.GetHead();
	
	NodeShadowController* pControl = NULL;
	NodeShadow* pShadow = NULL;

	Document* pDoc = Document::GetCurrent();

	DocRect BoundsToDraw;

	Spread* pSpread = NULL;

	while (pItem)
	{
		pControl = (NodeShadowController *)pItem->pNode;
		pShadow = pControl->GetShadow();
		pControl->ReleaseCached(TRUE, FALSE, FALSE, TRUE);	// Parents and derived data only

		pSpread = pControl->FindParentSpread();	
		BoundsToDraw = BoundsToDraw.Union(pControl->GetBlobBoundingRect());
		
		pShadow->SetDarkness(dDarkness);

		// force a redraw
		pControl->InvalidateBoundingRect();

		// we MUST regenerate in this case ....
		if (IS_A (pControl->FindParent (), NodeBlend))
		{
			pControl->RegenerateNode(NULL);
		}

		BoundsToDraw = BoundsToDraw.Union(pControl->GetBlobBoundingRect());
		
		pItem = (ShadowDragData*)ShadowDataList.GetNext(pItem);
	}

	if (pDoc)
		pDoc->ForceRedraw(pSpread, BoundsToDraw, FALSE);
	GetApplication()->ServiceRendering();
}




/********************************************************************************************

>	void SoftShadowInfoBarOp::ChangeShadowPenumbra(INT32 nBlur)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	- 
	Purpose:	Directly sets transparency value into the selected shadows

********************************************************************************************/
void SoftShadowInfoBarOp::ChangeShadowPenumbra(INT32 nBlur)
{
	if (ShadowDataList.IsEmpty())
	{
		return;
	}

	ShadowDragData* pItem = (ShadowDragData*)ShadowDataList.GetHead();
	
	NodeShadowController* pControl = NULL;
	NodeShadow* pShadow = NULL;

	Document* pDoc = Document::GetCurrent();

	DocRect BoundsToDraw;

	Spread* pSpread = NULL;

	while (pItem)
	{
		pControl = (NodeShadowController *)pItem->pNode;
		pShadow = pControl->GetShadow();
		pControl->ReleaseCached(TRUE, FALSE, FALSE, TRUE);	// Parents and derived data only

		pSpread = pControl->FindParentSpread();	
		BoundsToDraw = BoundsToDraw.Union(pControl->GetBlobBoundingRect());
		
		pControl->SetPenumbraWidth(nBlur);

		// force a redraw
		pControl->InvalidateBoundingRect();

		pControl->RegenerateNode(NULL);

		BoundsToDraw = BoundsToDraw.Union(pControl->GetBlobBoundingRect());
		
		pItem = (ShadowDragData*)ShadowDataList.GetNext(pItem);
	}

	if (pDoc)
		pDoc->ForceRedraw(pSpread, BoundsToDraw, FALSE);
	GetApplication()->ServiceRendering();
}




/********************************************************************************************

>	void SoftShadowInfoBarOp::SaveShadowDragData(List * pList)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	14/6/99
	Inputs:		The list to store all the shadow data to
	Outputs:	-
	Returns:	- 
	Purpose:	Stores all the shadow data in the ShadowDataList list

********************************************************************************************/
void SoftShadowInfoBarOp::SaveShadowDragData(List * pList)
{
	// first, build a list of all the shadow controller nodes in the selection
	pList->DeleteAll();

	// make up the list
	pList->DeleteAll();

	ShadowDragData * pShadDragItem = NULL;

	NodeShadowController * pController = NULL;
	NodeShadow *pShadow = NULL;

	NodeShadowController* pMaster = NULL;
	ListRange* pEditRange = pSoftShadowTool->GetShadowEditRange(&pMaster);
	if (pEditRange)
	{
		Node* pNode = pEditRange->FindFirst();
		while (pNode)
		{
			ENSURE(pNode->IsAShadowController(), "Found unexpected node type in shadow stack level");
			pController = (NodeShadowController*)pNode;

			pShadDragItem = new ShadowDragData;
			
			pShadDragItem->pNode = pController;
			pShadow = pController->GetShadow();

			pShadDragItem->PenumbraSize		= pController->GetPenumbraWidth();
			pShadDragItem->PositionShadowX	= pController->GetOffsetX();
			pShadDragItem->PositionShadowY	= pController->GetOffsetY();
			pShadDragItem->ShadowAngle		= pController->GetFloorShadowAngle();
			pShadDragItem->ShadowHeight		= pController->GetFloorShadowHeight();
			pShadDragItem->ShadowScale		= pController->GetWallShadowScale();
			pShadDragItem->GlowWidth		= pController->GetGlowWidth();
			pShadDragItem->FeatherWidth		= pController->GetFeatherWidth();
			pShadDragItem->ShadowTransparency  =  pShadow->GetTransp();

			pList->AddTail(pShadDragItem);

			pNode = pEditRange->FindNext(pNode);
		}
	}
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::RestoreShadowDragData(List * pList, BOOL bRegen = FALSE)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	14/6/99
	Inputs:		Whether to call the regenerate function on every node or not
	Outputs:	-
	Returns:	- 
	Purpose:	Stores all the shadow data in the ShadowDataList list

********************************************************************************************/
void SoftShadowInfoBarOp::RestoreShadowDragData(List * pList, BOOL bRegen,
												BOOL bForceRegen)
{
	if (pList->IsEmpty())
	{
		return;
	}

	ShadowDragData * pItem = (ShadowDragData *)pList->GetHead();
	
	NodeShadowController * pControl = NULL;
	NodeShadow			 * pShadow  = NULL;

	DocView * pView = DocView::GetCurrent();

	while (pItem)
	{
		pControl = (NodeShadowController *)pItem->pNode;

		// force an initial redraw
		if (pView)
		{
			pView->ForceRedraw(pControl->FindParentSpread(), pControl->GetBoundingRect(),
				FALSE);
		}

		pShadow = pControl->GetShadow();

		// I don;t need to regen if just the offsets have changed
		BOOL bMustIRegen = FALSE;
		
		if (pControl->GetFloorShadowAngle() != pItem->ShadowAngle)
		{
			pControl->SetFloorShadowAngle(pItem->ShadowAngle);
			bMustIRegen = TRUE;
		}

		if (pControl->GetFloorShadowHeight() != pItem->ShadowHeight)
		{
			pControl->SetFloorShadowHeight(pItem->ShadowHeight);
			bMustIRegen = TRUE;
		}

		if (pControl->GetPenumbraWidth() != pItem->PenumbraSize)
		{
			pControl->SetPenumbraWidth(pItem->PenumbraSize);
			bMustIRegen = TRUE;
		}


		pControl->SetOffsetX(pItem->PositionShadowX);
		pControl->SetOffsetY(pItem->PositionShadowY);
		
		if (pControl->GetWallShadowScale() != pItem->ShadowScale)
		{
			pControl->SetWallShadowScale(pItem->ShadowScale);
			bMustIRegen = TRUE;
		}

		if (pControl->GetGlowWidth() != pItem->GlowWidth)
		{
			pControl->SetGlowWidth(pItem->GlowWidth);
			bMustIRegen = TRUE;
		}

		if (pControl->GetFeatherWidth() != pItem->FeatherWidth)
		{
			pControl->SetFeatherWidth(pItem->FeatherWidth);
			bMustIRegen = TRUE;
		}

		pShadow->SetTransp(pItem->ShadowTransparency);

		// force a redraw
		if (pView)
		{
			pControl->InvalidateBoundingRect();
			pControl->ReleaseCached(TRUE, FALSE, TRUE, TRUE);	// Parents, Self and Derived data
			
			if ((bRegen && bMustIRegen) || bForceRegen)
			{
				pControl->RegenerateNode(NULL);
			}

			pView->ForceRedraw(pControl->FindParentSpread(), pControl->GetBoundingRect(),
				FALSE);
		}

		pItem = (ShadowDragData *)pList->GetNext(pItem);
	}

	pList->DeleteAll();
	ShadowDataList.DeleteAll();
}

/********************************************************************************************

>	MILLIPOINT SoftShadowInfoBarOp::GetGlowSliderValue()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/2000
	Inputs:		
	Outputs:	The glow slider value
	Returns:	-
	Purpose:	Gets the glow slider value, which is logarithmic
	
********************************************************************************************/
MILLIPOINT SoftShadowInfoBarOp::GetGlowSliderValue()
{
	SetGadgetRange(_R(IDC_SOFTSHADOWSLIDER), 0, WIDTHSLIDERMAX);
	MILLIPOINT Value = GetLongGadgetValue(_R(IDC_SOFTSHADOWSLIDER), 0,
		WIDTHSLIDERMAX);
	
	// make the value appear between 0 and 1
	if (Value > WIDTHSLIDERMAX)
		Value = WIDTHSLIDERMAX;

	double dValue = Value - 0;
	dValue /= (double)(WIDTHSLIDERMAX - 0);
	//dValue = 1.0 - dValue;

	// change the profile
	GlowSliderProfile.SetBias(-GlowSliderProfile.GetBias());
	double dSliderValue = GlowSliderProfile.MapZeroToOne((AFp)(dValue));
	GlowSliderProfile.SetBias(-GlowSliderProfile.GetBias());
		
	dSliderValue *= (double)(WIDTHSLIDERMAX - 0);
	dSliderValue += 0;

	return (INT32)dSliderValue;	
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::SetGlowSliderValue(MILLIPOINT Value)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/2000
	Inputs:		
	Outputs:	The glow slider value
	Returns:	-
	Purpose:	Sets the glow slider value, which is logarithmic
	
********************************************************************************************/
void SoftShadowInfoBarOp::SetGlowSliderValue(MILLIPOINT Value)
{
	// make the value appear between 0 and 1
	if (Value > WIDTHSLIDERMAX)
		Value = WIDTHSLIDERMAX;

	if (Value < 0)
		Value = 0;

	double dValue = Value;
	dValue /= (double)(WIDTHSLIDERMAX);

	// change the profile
	double dSliderValue = dValue;
	
	dSliderValue = GlowSliderProfile.MapZeroToOne((AFp)(dValue));
	//dSliderValue = 1.0 - dSliderValue;
		
	dSliderValue *= (double)WIDTHSLIDERMAX;
	dSliderValue += 0;

	SetGadgetRange(_R(IDC_SOFTSHADOWSLIDER), 0, WIDTHSLIDERMAX);

	SetLongGadgetValue(_R(IDC_SOFTSHADOWSLIDER), (INT32)dSliderValue);
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::UpdateGadgetTypeButtons()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/2000
	Inputs:		Nothing
	Outputs:	Nothing
	Returns:	Nothing
	Purpose:	Updates the type button strip with the appropriate settings for the selection
	
********************************************************************************************/
void SoftShadowInfoBarOp::UpdateGadgetTypeButtons()
{
	BOOL bHasWallShadow = FALSE;
	BOOL bHasGlowShadow = FALSE;
	BOOL bHasFloorShadow = FALSE;
	BOOL bHasFeather = FALSE;

	NodeShadowController* pMaster = NULL;
	ListRange* pEditRange = pSoftShadowTool->GetShadowEditRange(&pMaster);
	if (pEditRange)
	{
		Node* pNode = pEditRange->FindFirst();
		while (pNode)
		{
			ENSURE(pNode->IsAShadowController(), "Found unexpected node type in shadow stack level");
			NodeShadowController* pControl = (NodeShadowController*)pNode;

			switch (pControl->GetShadowType())
			{
			case SHADOWTYPE_WALL:
				bHasWallShadow = TRUE;
				break;
			case SHADOWTYPE_FLOOR:
				bHasFloorShadow = TRUE;
				break;
			case SHADOWTYPE_GLOW:
				bHasGlowShadow = TRUE;
				break;
			case SHADOWTYPE_FEATHER:
				bHasFeather = TRUE;
				break;

			default:
				{
					// Do nothing
				}
				break;
			}

			pNode = pEditRange->FindNext(pNode);
		}
	}

	// select/deselect buttons
	SetBoolGadgetSelected(_R(IDC_BTN_WALLSHADOW), bHasWallShadow);
	SetBoolGadgetSelected(_R(IDC_BTN_FLOORSHADOW), bHasFloorShadow);
	SetBoolGadgetSelected(_R(IDC_BTN_GLOWSHADOW), bHasGlowShadow);
	// TODO: Feather?

	if (!bHasWallShadow && !bHasFloorShadow && !bHasGlowShadow)
	{
		SetBoolGadgetSelected(_R(IDC_BTN_NOSHADOW), TRUE);
	}
	else
	{
		SetBoolGadgetSelected(_R(IDC_BTN_NOSHADOW), FALSE);
	}

}

/********************************************************************************************

>	void SoftShadowInfoBarOp::HandleNoShadowBtnClicked()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/2000
	Inputs:		Nothing
	Outputs:	Nothing
	Returns:	Nothing
	Purpose:	Handles the clicking of the 'no shadow' button
	
********************************************************************************************/
void SoftShadowInfoBarOp::HandleNoShadowBtnClicked()
{
	// is this button already selected
	if (!GetBoolGadgetSelected(_R(IDC_BTN_NOSHADOW)))
	{
		// do nothing - can't do anything since there's no shadow in the selection!
		return;
	}

	HandleShadowTypeChange(SHADOWTYPE_NONE);
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::HandleWallShadowBtnClicked()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/2000
	Inputs:		Nothing
	Outputs:	Nothing
	Returns:	Nothing
	Purpose:	Handles the clicking of the 'wall shadow' button
	
********************************************************************************************/
void SoftShadowInfoBarOp::HandleWallShadowBtnClicked()
{
	HandleShadowTypeChange(SHADOWTYPE_WALL);
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::HandleFloorShadowBtnClicked()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/2000
	Inputs:		Nothing
	Outputs:	Nothing
	Returns:	Nothing
	Purpose:	Handles the clicking of the 'floor shadow' button
	
********************************************************************************************/
void SoftShadowInfoBarOp::HandleFloorShadowBtnClicked()
{
	HandleShadowTypeChange(SHADOWTYPE_FLOOR);
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::HandleGlowShadowBtnClicked()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/2000
	Inputs:		Nothing
	Outputs:	Nothing
	Returns:	Nothing
	Purpose:	Handles the clicking of the 'glow shadow' button
	
********************************************************************************************/
void SoftShadowInfoBarOp::HandleGlowShadowBtnClicked()
{
	HandleShadowTypeChange(SHADOWTYPE_GLOW);
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::HandleShadowTypeChange(ShadowType Type)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/2000
	Inputs:		Nothing
	Outputs:	Nothing
	Returns:	Nothing
	Purpose:	Performs the change in type indicated by a button being clicked
	
********************************************************************************************/
void SoftShadowInfoBarOp::HandleShadowTypeChange(ShadowType Type)
{
//	BOOL Valid = FALSE;

	NodeShadowParam Param(Type);
	Param.StackPosition = STACKPOS_INSERT_TOP;
	Param.pPPStack = GetApplication()->FindSelection()->GetEffectsStack(TRUE);	// Get a copy while the sel changes
	Param.pShadowTool = pSoftShadowTool;

	// create a new shadow
	if (Type != SHADOWTYPE_NONE)
	{
PORTNOTE("other", "Removed use of Feather effect from SoftShadowInfoBarOp")
#if !defined(EXCLUDE_FROM_XARALX)
		if (Type==SHADOWTYPE_FEATHER)
		{
#ifdef FEATHER_EFFECT
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_APPLY_FEATHEREFFECT);
			if (pOpDesc != NULL)
			{
				OpFeatherEffectParam fxparam;
				fxparam.StackPosition = STACKPOS_INSERT_TOP;
				fxparam.strOpUnique = POSTPRO_ID_FEATHER;
				fxparam.FeatherSize = 2;
				fxparam.pPPStack = Param.pPPStack;
				//param.Profile = ?
				pOpDesc->Invoke(&fxparam);

				// Update the InfoBar
//TODO:			UpdateInfoBar();
			}

			delete Param.pPPStack;
			return;
#endif
		}
		else
#endif
		{
			NodeShadowController* pMaster = NULL;
			pSoftShadowTool->GetShadowEditRange(&pMaster);
			if (pMaster==NULL)
			{
				// No consistent shadow on top of effect stack
				// So create a new one...
				OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_APPLYSHADOW);
				if (pOpDesc != NULL)
				{
					String_256 UIDesc;
					OpState	State = OpApplyShadow::GetState(&UIDesc, pOpDesc);
					if (!State.Greyed)
						pOpDesc->Invoke(&Param);
								
					// Update the InfoBar
					UpdateInfoBar();
				}

				delete Param.pPPStack;
				return;
			}
			else
			{
				// There is a consistent shadow on top of the effect stack
				// So we need to change it's type...
				OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGESHADOWTYPE);
				if (pOpDesc != NULL)
				{
					String_256 UIDesc;
					OpState	State = OpChangeShadowType::GetState(&UIDesc, pOpDesc);
					if (!State.Greyed)
						pOpDesc->Invoke(&Param);
								
					// Update the InfoBar
					UpdateInfoBar();
				}

				delete Param.pPPStack;
				return;
			}
		}
	}

	// Is it the no shadow type ? if so, remove all shadows
	else
	{
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_REMOVESHADOW);
		if (pOpDesc != NULL)
		{
			String_256 UIDesc;
			OpState	State = OpRemoveShadow::GetState(&UIDesc, pOpDesc);
			if (!State.Greyed)
				pOpDesc->Invoke(&Param);

			// Update the InfoBar
			UpdateInfoBar();
		}
	}
}

/********************************************************************************************

>	void SoftShadowInfoBarOp::SetDarknessEditGadget(double dDarkness)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/2005
	Inputs:		dDarkness	-	Standard 0-1 darkness value
				bMany		-	Whether to put the string 'many' into the gadget
	Outputs:	Nothing
	Returns:	Nothing
	Purpose:	Loads the gadget with the percentage string from the value
	
********************************************************************************************/
void SoftShadowInfoBarOp::SetDarknessEditGadget(double dDarkness)
{
	// Calculate percentage value from darkness
	INT32 percent = (INT32)(100 - (dDarkness * 100));

	// Set the percentage string
	TCHAR Str[32];
	String_32 temp(_R(IDS_PERCENT_FORMAT));
	camSnprintf(Str, 32, temp, percent);
	String_32 PercentStr(Str);
	pSoftShadowInfoBar->SetStringGadgetValue(_R(IDC_SHADOWTRANSP_EDIT), &PercentStr);
}

/********************************************************************************************

>	BOOL SoftShadowInfoBarOp::GetDarknessEditGadget(double& dDarkness, BOOL &bMany)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/2005
	Inputs:		-
	Outputs:	dDarkness	-	The darkness value currently in the gadget
				bMany		-	Whether a 'many' setting is in the gadget
	Returns:	TRUE for success, FALSE for failure (e.g. the gadget's string is invalid)
	Purpose:	Gets a percentage string from the gadget and turns it into a INT32
	
********************************************************************************************/
BOOL SoftShadowInfoBarOp::GetDarknessEditGadget(double& dDarkness, BOOL &bMany)
{
	dDarkness = 0;
	String_256 ManyStr;
	ManyStr.Load(_R(IDS_SHADOWSTRING_MANY));

	BOOL Valid = TRUE;
	String_256 FieldContents = pSoftShadowInfoBar->GetStringGadgetValue(_R(IDC_SHADOWTRANSP_EDIT), &Valid);
	if (!Valid)
		return FALSE;

	bMany = (FieldContents == ManyStr);
	if (bMany)
		return TRUE;

	// Why don't we have Get/SetPercentageGadgetValue functions???
	//
	// OK, now we can parse the string
	// Shouldn't this really use GetDoubleGadgetValue ???
	INT32	Length	= (INT32)FieldContents.Length();
	TCHAR*	StrText	= (TCHAR*)FieldContents;

	// Search for first numeric character
	INT32 i = 0;
	while (i < Length && !StringBase::IsNumeric(StrText[i]))
		i++;

	if (i>=Length)
		return FALSE;

	// Search for last numeric character
	INT32 j = i;
	while (j < Length && StringBase::IsNumeric(StrText[j]))
		j++;

	INT32 value;
	String_256 PercentStr;
	FieldContents.Mid(&PercentStr, i, (j - i));
	Convert::StringToLong(PercentStr, &value);

	if (value < 0 || value > 100)
		return FALSE;

	dDarkness = (100 - value)/100.0;
	SetDarknessEditGadget(dDarkness);

	return TRUE;
}

void SoftShadowInfoBarOp::SetDarknessSlider(double dDarkness)
{
	SetLongGadgetValue(_R(IDC_SOFTSHADOWTRANSPSLIDER), (INT32)((1.0-dDarkness) * CONTRASTSLIDERMAX));
}

double SoftShadowInfoBarOp::GetDarknessSlider(BOOL* pValid)
{
	return 1.0-((double)GetLongGadgetValue(_R(IDC_SOFTSHADOWTRANSPSLIDER), 0, CONTRASTSLIDERMAX, 0, pValid)/(double)CONTRASTSLIDERMAX);
}

BOOL SoftShadowInfoBarOp::SelectShadowsAlone()
{
	// We need to get a copy of the stack because we're going to party on the selection!
	EffectsStack* pMyStack = GetApplication()->FindSelection()->GetEffectsStack(TRUE);
	if (pMyStack==NULL)
	{
		FailAndExecute();
		End();
		return FALSE;
	}

	NodeRenderableInk::DeselectAll();

	NodeShadowController* pMaster = NULL;
	ListRange* pShadowRange = pSoftShadowTool->GetShadowEditRange(&pMaster);
	if (pShadowRange)
	{
		Node* pSCNode = pShadowRange->FindFirst();
		while (pSCNode)
		{
			ENSURE(pSCNode->IsAShadowController(), "Found unexpected node type in shadow stack level");
			NodeShadowController* pControl = (NodeShadowController*)pSCNode;
			NodeShadow * pShadow = pControl->GetShadow();

			pShadow->Select(TRUE);

			pSCNode = pShadowRange->FindNext(pSCNode);
		}
	}

//	ShadowList.DeleteAll();
	if (pMyStack)
	{
		delete pMyStack;
		pMyStack = NULL;
	}

	return TRUE;
}



#endif
