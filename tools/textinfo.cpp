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
// Implementation of the text tool infobar

/*
*/

#include "camtypes.h"
#include "textinfo.h"	

// Resource files
//#include "richard.h"
//#include "richard2.h"
//#include "richard3.h"
//#include "simon.h"
//#include "textres.h"

// Code headers
#include "nodetxts.h"
#include "nodetxtl.h"
#include "texttool.h"
#include "prefs.h"
#include "app.h"
#include "txtattr.h"
#include "attrmgr.h"
#include "errors.h"
#include "textops.h"
#include "optsmsgs.h"
#include "nodetext.h"
#include "rndrgn.h"
#include "finfodlg.h"
#include "app.h"
#include "spread.h"
//#include "fonts.h"
//#include "atminfo.h"
#include "fontman.h"
#include "fontdrop.h"
#include "fontbase.h"
//#include "sgfonts.h"
#include "localenv.h"
#include "unicdman.h"
#include "docview.h"
//#include "camvw.h"
#include "blobs.h"

DECLARE_SOURCE( "$Revision$" );

CC_IMPLEMENT_DYNCREATE(TextInfoBarOp,InformationBarOp)
CC_IMPLEMENT_DYNCREATE(TextInfoBarData,CCObject)
CC_IMPLEMENT_DYNCREATE(TextInfoBarEnumFont, OILEnumFonts )

// Must come after the last CC_IMPLEMENT.. macro
#define new CAM_DEBUG_NEW     

// consts ...
const INT32 KernNudge      = 10;		// ems/1000
const INT32 KernLimit      = 999999;	// ems/1000    +/-
const INT32 BaseLineNudge  = 200;		// millipoints
const INT32 BaseLineLimit  = 999999;	// millipoints +/-
const INT32 TrackingNudge  = 10;		// ems/1000
const INT32 TrackingLimit  = 9999;	// ems/1000    +/-
const INT32 LineSpaceNudge = 200;		// millipoints
const INT32 LineSpaceLimit = 999999;	// millipoints +/-
const INT32 LineSpacePercentNudge = 5; // percent
const INT32 LineSpacePercentMin = -99999;	// percent
const INT32 LineSpacePercentMax = 99999;// percent
const INT32 FontSizeMin    = 100;		// millipoints
const INT32 FontSizeMax    = 999999;	// millipoints
const INT32 FontAspectMin  = 1;		// percent
const INT32 FontAspectMax  = 9999;	// percent

#define INVALID_ATTVAL -1000000
// statics ...
double TextInfoBarOp::SuperScriptSize;
double TextInfoBarOp::SuperScriptOffset;
double TextInfoBarOp::SubScriptSize;
double TextInfoBarOp::SubScriptOffset;

// the current text infobar allow static access
InformationBarOp* TextInfoBarOp::pTextInfoBar     = NULL;
BOOL 			  TextInfoBarOp::DisplayFontIsInstalled  = FALSE;
BOOL			  TextInfoBarOp::RegainCaretAfterOp = FALSE;

UnitType          TextInfoBarOp::CurrentFontUnits = COMP_POINTS;
TextInfoBarData   TextInfoBarOp::InfoData;
Document*         TextInfoBarOp::pDoc             = NULL;
CommonAttrSet 	  TextInfoBarOp::CommonAttrsToFindSet; 	// A set which will contain all attribute types
														// that we need to find common attributes for

PORTNOTE("text", "Removed custom dropdown");
#ifndef EXCLUDE_FROM_XARALX
FontDropDown	*TextInfoBarOp::NameDropDown = NULL;	// Font name drop-down list support for the font list and
#endif

String_64 	TextInfoBarData::FontName;
FontClass 	TextInfoBarData::FontType;
JustifyMode TextInfoBarData::Justify;
BOOL  		TextInfoBarData::Bold;
BOOL  		TextInfoBarData::Italic;
BOOL  		TextInfoBarData::UnderLine;
ScriptModes	TextInfoBarData::CurrentScriptMode;
FIXED16		TextInfoBarData::AspectRatio;
MILLIPOINT	TextInfoBarData::FontSize; // in 1/72000
MILLIPOINT	TextInfoBarData::BaseLineShift;
MILLIPOINT	TextInfoBarData::LineSpace;
double	 	TextInfoBarData::LineSpacePercent;
BOOL		TextInfoBarData::IsLineSpaceAPercent;
INT32 		TextInfoBarData::HorizontalKern;
INT32 		TextInfoBarData::Tracking;
BOOL 		TextInfoBarData::AutoKerning;


/********************************************************************************************

>	TextInfoBarData::TextInfoBarData() 

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/02/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor

********************************************************************************************/
				   

TextInfoBarData::TextInfoBarData()
{
	FontType = FC_UNDEFINED;
	Bold = FALSE;
	Italic = FALSE;
	UnderLine = FALSE;
	CurrentScriptMode = NormalScript;
	AspectRatio = 1;
	FontSize = 16000;
	BaseLineShift = 0;
	HorizontalKern = 0;
	Tracking = 0;
	LineSpace = 0;
	IsLineSpaceAPercent = TRUE;
	LineSpacePercent = 100;
	Justify = JustifyLeft;
	AutoKerning = TRUE;
}

/********************************************************************************************

>	BOOL TextInfoBarOp::IsDisplayFontInstalled() 

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/95				
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the font has been installed
	Purpose:	Determine whether our display fonts are installed

********************************************************************************************/

BOOL TextInfoBarOp::IsDisplayFontInstalled()
{
PORTNOTE("text", "Removed IsDisplayFontInstalled");
#ifndef EXCLUDE_FROM_XARALX
	// if we are running on a DBCS OS then we don't use our display font at all.
	if (UnicodeManager::IsDBCSOS())
		return TRUE;

	// get a DC
	CDC *pDesktopDC = CWnd::GetDesktopWindow()->GetDC();
	
	// Alex added ...
	if (!pDesktopDC) return FALSE; // Apparently we don't need to set an error

	// create one of the fonts we use
	CFont * TestFont = FontFactory::GetCFont(STOCKFONT_DIALOGBARSMALL);
	// if it's null, run on and we'll select a NULL font.

	// select it into the DC
	CFont * pOldFont = pDesktopDC->SelectObject(TestFont);
	
	// get the type face name
	TCHAR buff[64];
	pDesktopDC->GetTextFace(64,buff);

	// Select old font back into screen DC
	pDesktopDC->SelectObject(pOldFont);
	
	// Alex added Free the DC (better do it the way Chris got it :-) )
	CWnd::GetDesktopWindow()->ReleaseDC(pDesktopDC);

	// is it one of ours ?
	String_32 CCSmall(_R(IDS_FONTS_EDITFIELDSMALL)); // "CCSMALL"
	return camStrncmp(buff, (TCHAR *)CCSmall, 64)==0;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	INT32 TextInfoBarOp::BuildFontList() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/95				
	Inputs:		-
	Outputs:	-
	Returns:	the number of fonts placed in the list
	Purpose:	Build a simple list of fonts. This function uses the call back API facility
				EnumFontFamilies to build a list of TrueType fonts

********************************************************************************************/

INT32 TextInfoBarOp::BuildFontList()
{
	TextInfoBarEnumFont EnumObj;
	EnumObj.Execute();

	return EnumObj.GetCount();
}

BOOL TextInfoBarEnumFont::NewFont(FontClass, ENUMLOGFONT *lpelf)
{
	return TRUE;
}


/********************************************************************************************

>	TextInfoBarOp::~TextInfoBarOp()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor - deletes the font cache
	Errors:		-
	SeeAlso:	

********************************************************************************************/

TextInfoBarOp::~TextInfoBarOp()
{
	// DeleteFontCache();
	pTextInfoBar= NULL;

PORTNOTE("text", "Removed custom dropdown");
#ifndef EXCLUDE_FROM_XARALX
	// Delete our drop-down font list
	if (NameDropDown != NULL)
	{
		delete NameDropDown;
		NameDropDown = NULL;
	}
#endif

}


/********************************************************************************************

>	TextInfoBarOp::TextInfoBarOp() 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/02/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialises the infobar

********************************************************************************************/

TextInfoBarOp::TextInfoBarOp()
{
	DlgResID = _R(IDD_TEXT_INFO_BAR);

	SuperScriptSize   = Text_SuperScriptSize;
	SuperScriptOffset = Text_SuperScriptOffset;
	SubScriptSize     = Text_SubScriptSize;
	SubScriptOffset   = Text_SubScriptOffset;
	/*if (Camelot.DeclareSection("ScriptValues", 6))
	{
		Camelot.DeclarePref(NULL, "SuperScriptSize",   &SuperScriptSize);
		Camelot.DeclarePref(NULL, "SuperScriptOffset", &SuperScriptOffset);
		Camelot.DeclarePref(NULL, "SubScriptSize",     &SubScriptSize);
		Camelot.DeclarePref(NULL, "SubScriptOffset",   &SubScriptOffset,-100,100);
	}
	*/
	pTextTool = NULL;
	// InitFontCache();
	pDoc = Document::GetSelected();
	if(pDoc)
		CurrentFontUnits = pDoc->GetDocFontUnits();	

	// NameDropDown = NULL;
}

/********************************************************************************************

>	static BOOL TextInfoBarOp::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/95
	Returns:	FALSE if we run out of memory
	Purpose:	Initialises the TextInfoBarOp's static data. 

********************************************************************************************/



BOOL TextInfoBarOp::Init()
{
	// Initialise the CommonAttrsToFindSet 
	BOOL ok = CommonAttrsToFindSet.AddTypeToSet(CC_RUNTIME_CLASS(AttrTxtJustification));
	if (ok) ok = CommonAttrsToFindSet.AddTypeToSet(CC_RUNTIME_CLASS(AttrTxtBold));
	if (ok) ok = CommonAttrsToFindSet.AddTypeToSet(CC_RUNTIME_CLASS(AttrTxtItalic));
	if (ok) ok = CommonAttrsToFindSet.AddTypeToSet(CC_RUNTIME_CLASS(AttrTxtFontSize));
	if (ok) ok = CommonAttrsToFindSet.AddTypeToSet(CC_RUNTIME_CLASS(AttrTxtFontTypeface));
	if (ok) ok = CommonAttrsToFindSet.AddTypeToSet(CC_RUNTIME_CLASS(AttrTxtAspectRatio));
	if (ok) ok = CommonAttrsToFindSet.AddTypeToSet(CC_RUNTIME_CLASS(AttrTxtTracking));
	if (ok) ok = CommonAttrsToFindSet.AddTypeToSet(CC_RUNTIME_CLASS(AttrTxtLineSpace));
	if (ok) ok = CommonAttrsToFindSet.AddTypeToSet(CC_RUNTIME_CLASS(AttrTxtBaseLine));
	if (ok) ok = CommonAttrsToFindSet.AddTypeToSet(CC_RUNTIME_CLASS(AttrTxtScript));
	return ok; 
}

/********************************************************************************************

>	static void TextInfoBarOp::DeInit()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/95
	Purpose:	DeInits the TextInfoBarOp's static data. 

********************************************************************************************/

void TextInfoBarOp::DeInit()
{
	CommonAttrsToFindSet.DeleteAll();
} 



/********************************************************************************************

>void TextInfoBarOp::OnFieldChange()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		ThisChange - Enum Value for the Attribute that has changed
	Outputs:	-
	Returns:	-
	Purpose:	Called whenever the user changes a field on the infobar
				Applies atrributes to the current selection

				Note: This function also applies autokerning to a story which is not
				an attribute
	Errors:		Out Of Memory errors if we can't create new attributes
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::OnFieldChange(FontAttribute ThisChange)
{
	NodeAttribute * Attrib = NULL;
	
	switch (ThisChange)
	{
				
		case BaseLineShiftA:
		{
			if(InfoData.BaseLineShift != INVALID_ATTVAL)
			{
				AttrTxtBaseLine  * BaseLineAttrib = new AttrTxtBaseLine();
				if (BaseLineAttrib == NULL)
				{
					InformError();
					return;
				}
				MILLIPOINT  RealBaseLine = InfoData.BaseLineShift;
				BaseLineAttrib->Value.Value = RealBaseLine;
				Attrib = BaseLineAttrib;	
			}
			break;
		}
		case LineSpacePercentA:
		{
			if(InfoData.LineSpacePercent != INVALID_ATTVAL)
			{
				FIXED16 NewRatio =	 InfoData.LineSpacePercent/100;
				AttrTxtLineSpace  * LineSpaceAttrib = new AttrTxtLineSpace(NewRatio);
				if (LineSpaceAttrib == NULL)
				{
					InformError();
					return;
				}
				Attrib = LineSpaceAttrib;	
			}	
			break;
		}
		case LineSpaceA:
		{
			if(InfoData.LineSpace != INVALID_ATTVAL)
			{
				MILLIPOINT  RealLineSpace = InfoData.LineSpace;
				AttrTxtLineSpace  * LineSpaceAttrib = new AttrTxtLineSpace(RealLineSpace);
				if (LineSpaceAttrib == NULL)
				{
					InformError();
					return;
				}
				Attrib = LineSpaceAttrib;	
			}
			break;
		}
		case ScriptA:
		{
			AttrTxtScript  * ScriptAttrib = new AttrTxtScript();
			if (ScriptAttrib == NULL)
			{
				InformError();
				return;
			}

			// normal
			ScriptAttrib->Value.Offset = 0;
			ScriptAttrib->Value.Size = 1;

			if(InfoData.CurrentScriptMode== SubScript)
			{
				ScriptAttrib->Value.Offset = FIXED16(SubScriptOffset);
				ScriptAttrib->Value.Size = 	 FIXED16(SubScriptSize);
			}
			else if(InfoData.CurrentScriptMode == SuperScript)
			{
				ScriptAttrib->Value.Offset = FIXED16(SuperScriptOffset);
				ScriptAttrib->Value.Size =   FIXED16(SuperScriptSize);
			}

			Attrib = ScriptAttrib;	
			
			break;
		}
		case JustifyA:
		{
			AttrTxtJustification  * JustifyAttrib = new AttrTxtJustification();
			if (JustifyAttrib == NULL)
			{
				InformError();
				return;
			}
			JustifyAttrib->Value.justification = (Justification) InfoData.Justify;
			Attrib = JustifyAttrib;	
			break;
		}
		case BoldA:
		{
			AttrTxtBold  * BoldAttrib = new AttrTxtBold();
			if (BoldAttrib == NULL)
			{
				InformError();
				return;
			}
			BoldAttrib->Value.BoldOn = InfoData.Bold;
			Attrib = BoldAttrib;
			break;
		}
		case ItalicA:
		{
			AttrTxtItalic  * ItalicAttrib = new AttrTxtItalic();
			if (ItalicAttrib == NULL)
			{
				InformError();
				return;
			}
			ItalicAttrib->Value.ItalicOn = InfoData.Italic;
			Attrib = ItalicAttrib;
			break;
		}
		case UnderLineA:
		{
			AttrTxtUnderline  * UnderLineAttrib = new AttrTxtUnderline();
			if (UnderLineAttrib == NULL)
			{
				InformError();
				return;
			}
			UnderLineAttrib->Value.Underlined = InfoData.UnderLine;
			Attrib = UnderLineAttrib;	
			break;
		}
		case AspectRatioA:
		{	
			if(InfoData.AspectRatio != FIXED16(INVALID_ATTVAL))
			{
				AttrTxtAspectRatio  * AspectAttrib = new AttrTxtAspectRatio();
				if (AspectAttrib == NULL)
				{
					InformError();
					return;
				}
				FIXED16  RealAspect = InfoData.AspectRatio;
				AspectAttrib->Value.AspectRatio = RealAspect;
				Attrib = AspectAttrib;	
			}
			break;
		}
		case FontSizeA:
		{
			if(InfoData.FontSize != INVALID_ATTVAL)
			{
				AttrTxtFontSize * FontSizeAttrib = new AttrTxtFontSize();
				if (FontSizeAttrib == NULL)
				{
					InformError();
					return;
				}
				MILLIPOINT  RealSize = InfoData.FontSize;
				FontSizeAttrib->Value.FontSize = RealSize;
				Attrib = FontSizeAttrib;
			}
			break;
		}
		case FontNameA:
		{
						
			AttrTxtFontTypeface * TypeFaceAttrib = new AttrTxtFontTypeface();	
			if (TypeFaceAttrib == NULL)
			{
				InformError();
				return;
			}
			
			TypeFaceAttrib->Value.HTypeface = FONTMANAGER->GetFontHandle(&InfoData.FontName, InfoData.FontType);
			Attrib = TypeFaceAttrib;
			break;
		}
		case TrackingA:
		{
			if(InfoData.Tracking != INVALID_ATTVAL)
			{
				AttrTxtTracking * TrackingAttrib = new AttrTxtTracking();
				if (TrackingAttrib == NULL)
				{
					InformError();
					return;
				}TrackingAttrib->Value.Tracking = InfoData.Tracking;
				Attrib = TrackingAttrib;
			}
			break;	
		}
		default:
			break;
	}
	
	if (Attrib)
		AttributeManager::AttributeSelected(Attrib,NULL); 		 
	// make sure the infobar reflects the current attributes
	switch (ThisChange)
	{
		case HorizontalKernA:
		{
			// Invoke an operation to apply the kern
			OpDescriptor* OpDesc =
				OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpTextKern));

			if (OpDesc != NULL)
			{
				OpParam param(InfoData.HorizontalKern,0);
				OpDesc->Invoke(&param);
			}
			break;
		}
		case AutoKernText:
		{
			// Invoke an operation to apply the kern
			OpDescriptor* OpDesc =
				OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpTextAutoKern));

			if (OpDesc != NULL)
			{
				OpParam param(InfoData.AutoKerning,0);
				OpDesc->Invoke(&param);
			}
			break;
		}
		default:
			break;
	}

	Update();
}





/********************************************************************************************

>void TextInfoBarOp::UpdateGadgets()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Update all infobar fields according to the data in InfoData
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::UpdateGadgets()
{

	if(pTextInfoBar== NULL)
		return;
	if(!pTextInfoBar->HasWindow())
		return ;

PORTNOTE("text", "Removed custom dropdown");
#ifndef EXCLUDE_FROM_XARALX
	// Font Name
	NameDropDown->SetTopFontName(&InfoData.FontName, InfoData.FontType, FALSE);
	FontDropItem Dummy(InfoData.FontName, InfoData.FontType);
	NameDropDown->SetSelection(&Dummy);
#endif

	// Kerning
	pTextInfoBar->SetLongGadgetValue(_R(IDC_KERN_EDIT_X),InfoData.HorizontalKern,0,-1);
	if(Document::GetSelected()!= NULL)
		pTextInfoBar->SetUnitGadgetValue(_R(IDC_KERN_EDIT_Y),CurrentFontUnits,InfoData.BaseLineShift,0,-1);
	else
	{
		String_64 ZeroPt(_R(IDS_TEXTINFO_0PT));
		pTextInfoBar->SetStringGadgetValue(_R(IDC_KERN_EDIT_Y),ZeroPt,0,-1);
	}

	SetLineSpaceGadget();
	// Tracking
	pTextInfoBar->SetLongGadgetValue(_R(IDC_TRACKING_EDIT),InfoData.Tracking);
 
	// Aspect Ratio
	pTextInfoBar->SetDoubleGadgetValue(_R(IDC_ASPECTEDIT),InfoData.AspectRatio.MakeDouble()*100); 
 
	// buttons
	UpdateButtonStates();

}
/********************************************************************************************

>void TextInfoBarOp::Update()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Update the infobar according to the current selection 
				called on selchange messages 
				only update fields as required
				return if tool is not active
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL TextInfoBarOp::Update(BOOL DoUpdate)
{
	
	static BOOL ForceUpdate = FALSE;
	static BOOL BoldChanged = FALSE;
	static BOOL ItalicChanged = FALSE;
	static BOOL ValidChanged = FALSE;

	BOOL IllegalHandle = FALSE;
	BOOL KernSet = FALSE;
	
	Document* pDoc = Document::GetCurrent();
	
	if (pDoc==NULL)
		return FALSE;


	Document *SelectedDoc = Document::GetSelected();
	TextStory *SelectedStory = TextStory::GetFocusStory(); // used for updating auto-kerning button

	if (SelectedDoc == NULL)
		return FALSE;

	if(pTextInfoBar== NULL)
		return FALSE;
 	if(!pTextInfoBar->HasWindow())
		return FALSE;


	SelRange *Selection = Camelot.FindSelection();
	ENSURE(Selection != NULL, "No Selection SelRange!?!");

	// Find common attribute details for all attribute types we need to know about
	if (!Selection->FindCommonAttributes(&CommonAttrsToFindSet))
		return FALSE; 

	SelRange::CommonAttribResult result;
	NodeAttribute* pAttr;

	// Justification  ---------------------------------------------------------------		
	AttrTxtJustification * JustifyAttrib;
	
	CommonAttrsToFindSet.FindAttrDetails(CC_RUNTIME_CLASS(AttrTxtJustification), 
									   &pAttr,
								   	   &result);
	
	JustifyAttrib = (AttrTxtJustification*)pAttr; 

 	if ( result != SelRange ::ATTR_MANY)
	{
	 	ERROR2IF(JustifyAttrib==NULL,FALSE,"FindCommonAttribute returned NULL");
	 	JustifyMode	J =	 (JustifyMode) JustifyAttrib->Value.justification ;
	 	if(InfoData.Justify != J||DoUpdate)
	 	{
	 		InfoData.Justify = 	J;
			UpdateJustifyButtons();
		}
	}
	else
	{
		  UpdateJustifyButtons(TRUE);
	}

	// BUTTONS 
	// Bold	-------------------------------------------------------------------------
	AttrTxtBold * BoldAttrib;
	CommonAttrsToFindSet.FindAttrDetails(CC_RUNTIME_CLASS(AttrTxtBold), 
									   &pAttr,
								   	   &result);
	
	BoldAttrib = (AttrTxtBold*)pAttr; 

 	if (result == SelRange ::ATTR_MANY)
	{
		if(InfoData.Bold != FALSE)
		{
			InfoData.Bold = FALSE;
			pTextInfoBar->SetBoolGadgetSelected(_R(IDC_BOLDBUTTON),InfoData.Bold);
		}
	}
	else
	{
	 	ERROR2IF(BoldAttrib==NULL,FALSE,"FindCommonAttribute returned NULL");
		BOOL B = BoldAttrib->Value.BoldOn;
		if(InfoData.Bold!=B||DoUpdate)
	 	{
	 		InfoData.Bold = B;
			pTextInfoBar->SetBoolGadgetSelected(_R(IDC_BOLDBUTTON),InfoData.Bold);
		}
	}
	// AutoKerning -------------------------------------------------------------------------
	if (SelectedStory)
	{
		bool kerning = SelectedStory->IsAutoKerning();
		InfoData.AutoKerning = kerning;
		pTextInfoBar->SetBoolGadgetSelected(_R(IDC_AUTOKERN),InfoData.AutoKerning);
	}

	// Italic ------------------------------------------------------------------------
	AttrTxtItalic * ItalicAttrib;
	CommonAttrsToFindSet.FindAttrDetails(CC_RUNTIME_CLASS(AttrTxtItalic), 
									   &pAttr,
								   	   &result);
	
	ItalicAttrib = (AttrTxtItalic*)pAttr; 

 	if (result == SelRange ::ATTR_MANY)
	{
		if(InfoData.Italic != FALSE)
		{
			InfoData.Italic = FALSE;
			pTextInfoBar->SetBoolGadgetSelected(_R(IDC_ITALICBUTTON),InfoData.Italic);
		}		
	}
	else
	{
	 	ERROR2IF(ItalicAttrib==NULL,FALSE,"FindCommonAttribute returned NULL");
		BOOL I =  ItalicAttrib->Value.ItalicOn;
		if(InfoData.Italic!=I||DoUpdate)
	 	{
		 	InfoData.Italic = ItalicAttrib->Value.ItalicOn;
			pTextInfoBar->SetBoolGadgetSelected(_R(IDC_ITALICBUTTON),InfoData.Italic);
		}
	}

	// Font Size ---------------------------------------------------------------------
	
	AttrTxtFontSize * FontSizeAttrib;
	CommonAttrsToFindSet.FindAttrDetails(CC_RUNTIME_CLASS(AttrTxtFontSize), 
									   	 &pAttr,
								   	     &result);
	
	FontSizeAttrib = (AttrTxtFontSize*)pAttr; 

 	if (result == SelRange ::ATTR_MANY)
	{
		InfoData.FontSize = INVALID_ATTVAL;
		String_64 empty(_T(""));
		pTextInfoBar->SetStringGadgetValue(_R(IDC_POINT_COMBO),empty,0,-1);

	}
	else 
	{
		ERROR2IF(FontSizeAttrib==NULL,FALSE,"FindCommonAttribute returned NULL");
	 	MILLIPOINT DocFontSize = FontSizeAttrib->Value.FontSize;
		if(DocFontSize != InfoData.FontSize||DoUpdate )			
		{										   
	 		InfoData.FontSize = DocFontSize;
			pTextInfoBar->SetUnitGadgetValue(_R(IDC_POINT_COMBO),CurrentFontUnits,InfoData.FontSize,0,-1);
 		} 
 	}
	
	// Font Name --------------------------------------------------------------------
	AttrTxtFontTypeface * FontTypeAttrib;

	CommonAttrsToFindSet.FindAttrDetails(CC_RUNTIME_CLASS(AttrTxtFontTypeface), 
									   	 &pAttr,
								   	     &result);
	
	FontTypeAttrib = (AttrTxtFontTypeface*)pAttr; 

 	if (result == SelRange ::ATTR_MANY)
	{
PORTNOTE("text", "Removed custom dropdown");
#ifndef EXCLUDE_FROM_XARALX
		String_64 Multiple(_R(IDS_TEXTTOOL_MULTIPLE));
		NameDropDown->SetTopFontName(&Multiple, FC_UNDEFINED, TRUE);
		ForceUpdate = TRUE;
		InfoData.FontName = _R(IDS_TEXTINFO_MULTIPLE); // "Multiple" just to invalidate this field
#endif
	}
	else 
	{
		ERROR2IF(FontTypeAttrib==NULL,FALSE,"FindCommonAttribute returned NULL");
		//CachedFontItem* ThisData = FONTMANAGER->GetFont(FontTypeAttrib->Value.HTypeface,FALSE);
		CachedFontItem* ThisData = FONTMANAGER->GetCachedFont(FontTypeAttrib->Value.HTypeface);
		if (ThisData)
		{
			
			if( !ThisData->Compare(&InfoData.FontName)
				||ThisData->GetFontClass() != InfoData.FontType
				||ForceUpdate
				||DoUpdate
				||FontTypeAttrib->IsBold()!=BoldChanged
				||FontTypeAttrib->IsItalic()!=ItalicChanged
				||ThisData->IsValid()!=ValidChanged
				)
			{
				InfoData.FontType = ThisData->GetFontClass();
				InfoData.FontName = *(ThisData->GetFontName());
				String_256 FontName = InfoData.FontName;

				// If this is set to TRUE, then the current selection in the font menu will be deselected.
				// This makes it possible to override the text in the top icon with a asterisk, B or I...
  				BOOL Deselect = FALSE;

				// indicate that the font isn't installed 
				if(ThisData->IsReplaced())
				{
	  				Deselect = TRUE;
					FontName += String_8(_R(IDS_TEXTINFO_STAR));  //" *"
					ValidChanged = FALSE;
				}
				else
					ValidChanged = TRUE;

				// remember Bold\Italic states so we can force an update if they change
				// modify the string accordingly
				if(FontTypeAttrib->IsBold())
				{
	  				Deselect = TRUE;
					FontName += String_8(_R(IDS_TEXTINFO_BOLD));  //" B";
					BoldChanged = TRUE;
				}
				else
					BoldChanged = FALSE;
				
				if(FontTypeAttrib->IsItalic())
				{
	  				Deselect = TRUE;
					FontName += String_8(_R(IDS_TEXTINFO_ITALIC));  //" I";
					ItalicChanged = TRUE;
				}
				else
					ItalicChanged = FALSE;

PORTNOTE("text", "Removed custom dropdown");
#ifndef EXCLUDE_FROM_XARALX
				NameDropDown->SetTopFontName(&FontName, InfoData.FontType, Deselect);
				if(!Deselect)
				{
					FontDropItem Dummy(InfoData.FontName, InfoData.FontType);
					NameDropDown->SetSelection(&Dummy);
				}
#endif
				ForceUpdate = !ThisData->IsValid();
 			}
 		}
		// couldn't find a font with that handle
		else 
		 	IllegalHandle = TRUE;

 	}

	// AspectRatio	-----------------------------------------------------------------
	AttrTxtAspectRatio * AspectAttrib;
	CommonAttrsToFindSet.FindAttrDetails(CC_RUNTIME_CLASS(AttrTxtAspectRatio), 
									   	 &pAttr,
								   	     &result);
	
	AspectAttrib = (AttrTxtAspectRatio*)pAttr; 

	if (result == SelRange ::ATTR_MANY)
	{
		InfoData.AspectRatio = INVALID_ATTVAL;
		String_64 empty(_T(""));
		pTextInfoBar->SetStringGadgetValue(_R(IDC_ASPECTEDIT),empty,0,-1);
	}
	else 
	{
		ERROR2IF(AspectAttrib==NULL,FALSE,"FindCommonAttribute returned NULL");
		FIXED16 DocAspect = AspectAttrib->Value.AspectRatio;
		if(InfoData.AspectRatio != DocAspect||DoUpdate)
		{
			InfoData.AspectRatio = DocAspect;
			pTextInfoBar->SetDoubleGadgetValue(_R(IDC_ASPECTEDIT),InfoData.AspectRatio.MakeDouble()*100,0,-1); 
		}
	}

	// Tracking ---------------------------------------------------------------------
	AttrTxtTracking * TrackingAttrib;

	
	CommonAttrsToFindSet.FindAttrDetails(CC_RUNTIME_CLASS(AttrTxtTracking), 
										 &pAttr,
										 &result);
	
	TrackingAttrib = (AttrTxtTracking*)pAttr; 

	if (result == SelRange ::ATTR_MANY)
	{
		InfoData.Tracking = INVALID_ATTVAL;
		String_64 empty(_T(""));
		pTextInfoBar->SetStringGadgetValue(_R(IDC_TRACKING_EDIT),empty,0,-1);
	}
	else 
	{
		ERROR2IF(TrackingAttrib==NULL,FALSE,"FindCommonAttribute returned NULL");
		INT32 T = TrackingAttrib->Value.Tracking;
		if(InfoData.Tracking != T||DoUpdate)
		{
			InfoData.Tracking = T;
			pTextInfoBar->SetLongGadgetValue(_R(IDC_TRACKING_EDIT),InfoData.Tracking,0,-1);
			pTextInfoBar->PaintGadgetNow(_R(IDC_TRACKING_EDIT));
		}
	}

	// Line Spacing ---------------------------------------------------------------------
	AttrTxtLineSpace * LineSpaceAttrib;
		
	CommonAttrsToFindSet.FindAttrDetails(CC_RUNTIME_CLASS(AttrTxtLineSpace), 
										 &pAttr,
										 &result);
	
	LineSpaceAttrib = (AttrTxtLineSpace*)pAttr; 

	if (result == SelRange ::ATTR_MANY)
	{
		String_64 empty(_T(""));
		pTextInfoBar->SetStringGadgetValue(_R(IDC_SPACING_EDIT),empty,0,-1);
		InfoData.LineSpacePercent = InfoData.LineSpace = INVALID_ATTVAL;
		
	}
	else 
	{
		ERROR2IF(LineSpaceAttrib==NULL,FALSE,"FindCommonAttribute returned NULL");

		if(LineSpaceAttrib->IsARatio())
		{
			FIXED16 Ratio =  LineSpaceAttrib->Value.Ratio;
			double Percent = (Ratio + FIXED16_DBL(0.00003)).MakeDouble()*100;
			if(Percent != InfoData.LineSpacePercent||DoUpdate|| !InfoData.IsLineSpaceAPercent)
			{
				SetCurrentLineSpacePercent(Percent);
				SetLineSpaceGadget();
			}
		}
		else
		{
			MILLIPOINT DocLineSpace = LineSpaceAttrib->Value.Value;
			if(InfoData.LineSpace != DocLineSpace||DoUpdate||InfoData.IsLineSpaceAPercent)
			{
				SetCurrentLineSpace(DocLineSpace);
				pTextInfoBar->SetUnitGadgetValue(_R(IDC_SPACING_EDIT),CurrentFontUnits,InfoData.LineSpace,0,-1);
			}
		}
		pTextInfoBar->PaintGadgetNow(_R(IDC_SPACING_EDIT)); 
	}
	
	// Base Line Shift ---------------------------------------------------------------------
	AttrTxtBaseLine * BaseLineAttrib;
	CommonAttrsToFindSet.FindAttrDetails(CC_RUNTIME_CLASS(AttrTxtBaseLine), 
									   	 &pAttr,
								   	     &result);
	
	BaseLineAttrib = (AttrTxtBaseLine*)pAttr; 

	if (result == SelRange ::ATTR_MANY)
	{
		InfoData.BaseLineShift = INVALID_ATTVAL;
		String_64 empty(_T(""));
		pTextInfoBar->SetStringGadgetValue(_R(IDC_KERN_EDIT_Y),empty,0,-1);
	}
	else 
	{
		ERROR2IF(BaseLineAttrib==NULL,FALSE,"FindCommonAttribute returned NULL");
		MILLIPOINT DocBaseLine = BaseLineAttrib->Value.Value;
		if(InfoData.BaseLineShift != DocBaseLine||DoUpdate)
		{
			InfoData.BaseLineShift = DocBaseLine;
			pTextInfoBar->SetUnitGadgetValue(_R(IDC_KERN_EDIT_Y),CurrentFontUnits,InfoData.BaseLineShift,0,-1);
			pTextInfoBar->PaintGadgetNow(_R(IDC_KERN_EDIT_Y));
 		}
 	}
	
	// Script ---------------------------------------------------------------------
	AttrTxtScript * ScriptAttrib;
	CommonAttrsToFindSet.FindAttrDetails(CC_RUNTIME_CLASS(AttrTxtScript), 
									   	 &pAttr,
								   	     &result);
	
	ScriptAttrib = (AttrTxtScript*)pAttr; 


 	if (result == SelRange ::ATTR_MANY)
	{
		InfoData.CurrentScriptMode = NormalScript;
		UpdateScriptButtons();
	}
	else 
	{
	 	ERROR2IF(ScriptAttrib==NULL,FALSE,"FindCommonAttribute returned NULL");
	 	FIXED16 Offset = ScriptAttrib->Value.Offset;
	 	FIXED16 Size = ScriptAttrib->Value.Size;
	 	ScriptModes NewMode = NormalScript;
		if(Size == FIXED16(SuperScriptSize) 
		   && Offset == FIXED16(SuperScriptOffset))
		{	
				NewMode = SuperScript;
		}
		else if(Size == FIXED16(SubScriptSize) 
		   && Offset == FIXED16(SubScriptOffset))
		{	
				NewMode = SubScript;
		}
	 	if(InfoData.CurrentScriptMode!= NewMode||DoUpdate)
		{
	 		InfoData.CurrentScriptMode = NewMode;
			UpdateScriptButtons();
		}
 	}

	// Horizontal Kerning update - check the node to the left of the caret
	TextStory * ActiveStory = TextStory::GetFocusStory();
	if(ActiveStory)
	{
		CaretNode * pCaret = ActiveStory->GetCaret();
		if(pCaret)
		{
			VisibleTextNode * LastNode = pCaret->FindPrevAbstractTextCharInStory();
			if(LastNode)
			{
				if(IS_A(LastNode, KernCode))
				{
					KernCode * Kern = (KernCode *) LastNode;
					DocCoord ThisKern = Kern->GetValue();
					InfoData.HorizontalKern =ThisKern.x;
					pTextInfoBar->SetLongGadgetValue(_R(IDC_KERN_EDIT_X),InfoData.HorizontalKern,0,-1);
					pTextInfoBar->PaintGadgetNow(_R(IDC_KERN_EDIT_X)); 
					KernSet = TRUE;
				}
			}
		}

	}
	
	if(InfoData.HorizontalKern!=0 && ! KernSet)
	{
		InfoData.HorizontalKern =0;
		pTextInfoBar->SetLongGadgetValue(_R(IDC_KERN_EDIT_X),InfoData.HorizontalKern,0,-1); 
	}
	
	ERROR2IF(IllegalHandle==TRUE,FALSE,"Can't find a font for that handle");

	return TRUE;
}


/********************************************************************************************

>void TextInfoBarOp::UpdateJustifyButtons()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	updates the Justify Buttons
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void TextInfoBarOp::UpdateJustifyButtons(BOOL Clear)
{
	if(Clear)
	{
		pTextInfoBar->SetBoolGadgetSelected(_R(IDC_JUSTIFYLEFT),FALSE);
		pTextInfoBar->SetBoolGadgetSelected(_R(IDC_JUSTIFYCENTRE),FALSE);
		pTextInfoBar->SetBoolGadgetSelected(_R(IDC_JUSTIFYRIGHT),FALSE);
		pTextInfoBar->SetBoolGadgetSelected(_R(IDC_JUSTIFYFULL),FALSE);
	}
	else
	{
		pTextInfoBar->SetBoolGadgetSelected(_R(IDC_JUSTIFYLEFT),InfoData.Justify==JustifyLeft);
		pTextInfoBar->SetBoolGadgetSelected(_R(IDC_JUSTIFYCENTRE),InfoData.Justify==JustifyCentre);
		pTextInfoBar->SetBoolGadgetSelected(_R(IDC_JUSTIFYRIGHT),InfoData.Justify==JustifyRight);
		pTextInfoBar->SetBoolGadgetSelected(_R(IDC_JUSTIFYFULL),InfoData.Justify==JustifyFull);
	}
}

/********************************************************************************************

>void TextInfoBarOp::UpdateScriptButtons()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	updates the Super/SubScript Buttons
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void TextInfoBarOp::UpdateScriptButtons()
{
	pTextInfoBar->SetBoolGadgetSelected(_R(IDC_SUPERSCRIPT),InfoData.CurrentScriptMode==SuperScript);
	pTextInfoBar->SetBoolGadgetSelected(_R(IDC_SUBSCRIPT),InfoData.CurrentScriptMode==SubScript);
}

/********************************************************************************************

>void TextInfoBarOp::UpdateButtonStates()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	updates the info bar buttons and edit fields
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void TextInfoBarOp::UpdateButtonStates()
{
	pTextInfoBar->SetBoolGadgetSelected(_R(IDC_BOLDBUTTON),InfoData.Bold);
	pTextInfoBar->SetBoolGadgetSelected(_R(IDC_ITALICBUTTON),InfoData.Italic);
	UpdateJustifyButtons();
	UpdateScriptButtons();
}

/********************************************************************************************

>void TextInfoBarOp::SetCurrentJustify(JustifyMode NewJustify)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to set the current justify state
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::SetCurrentJustify(UINT32 Button)
{
	if (Button == _R(IDC_JUSTIFYFULL))
	{
		InfoData.Justify = JustifyFull;
	}
	else if (Button == _R(IDC_JUSTIFYLEFT))
	{
		InfoData.Justify = JustifyLeft;
	}
	else if (Button == _R(IDC_JUSTIFYRIGHT))
	{
		InfoData.Justify = JustifyRight;
	}
	else if (Button == _R(IDC_JUSTIFYCENTRE))
	{
		InfoData.Justify = JustifyCentre;
	}

	UpdateJustifyButtons();
}

/********************************************************************************************

>void TextInfoBarOp::SetCurrentScript(ScriptMode Script)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to set the current font ( i.e.the one in the edit field of the combo )
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::SetCurrentScript(ScriptModes Script)
{
	
	if(InfoData.CurrentScriptMode!=Script)
		InfoData.CurrentScriptMode = Script;
	else
		InfoData.CurrentScriptMode=NormalScript;
	
	UpdateScriptButtons();
}



/********************************************************************************************

>BOOL TextInfoBarOp::SetCurrentPointSize(double  PointSize)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if value unchanged TRUE otherwise
	Purpose:	Called to set the current PointSize
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL TextInfoBarOp::SetCurrentPointSize(MILLIPOINT PointSize)
{
	if(InfoData.FontSize == PointSize)
		return FALSE;
	InfoData.FontSize = PointSize;
	return TRUE;
}

/********************************************************************************************

>void TextInfoBarOp::SetCurrentFontBold(BOOL IsBold)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to set the current Bold State
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void TextInfoBarOp::SetCurrentFontBold(BOOL IsBold)
{
	InfoData.Bold = IsBold;
}

/********************************************************************************************

>void TextInfoBarOp::SetCurrentFontItalic(BOOL IsItalic)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to set the current Italic State
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::SetCurrentFontItalic(BOOL IsItalic)
{
	InfoData.Italic = IsItalic;
}

/********************************************************************************************

>void TextInfoBarOp::SetCurrentAutoKerning(BOOL IsAutoKerning)

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/2000
	Purpose:	Called to set the current auto kerning State

********************************************************************************************/
void TextInfoBarOp::SetCurrentAutoKerning(BOOL IsAutoKerning)
{
	InfoData.AutoKerning = IsAutoKerning;
}

/********************************************************************************************

>void TextInfoBarOp::EnableGadgets(BOOL Enable)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to GREY/ENABLE the gadgets on this bar
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::EnableGadgets(BOOL Enable)
{
	if(pTextInfoBar== NULL)
		return;
	if(!pTextInfoBar->HasWindow())
		return;
	pTextInfoBar->EnableGadget(_R(IDC_TRACKINGBIT) ,Enable);
	pTextInfoBar->EnableGadget(_R(IDC_KERNINGBIT) ,Enable);

//	pTextInfoBar->EnableGadget(_R(IDC_TEXT),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_FONT_COMBO),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_POINT_COMBO),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_BOLDBUTTON),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_ITALICBUTTON),Enable);
//	pTextInfoBar->EnableGadget(_R(IDC_UNDERLINEBUTTON),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_ASPECTBIT),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_ASPECTEDIT),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_JUSTIFYLEFT),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_JUSTIFYCENTRE),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_JUSTIFYRIGHT),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_JUSTIFYFULL),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_KERN_EDIT_X),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_KERN_EDIT_Y),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_KERN_BUMP_X_LESS),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_KERN_BUMP_X_MORE),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_KERN_BUMP_Y_LESS),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_KERN_BUMP_Y_MORE),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_TRACKING_EDIT),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_TRACKING_LESS),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_TRACKING_MORE),Enable);

	pTextInfoBar->EnableGadget(_R(IDC_SPACING_EDIT),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_SPACING_LESS),Enable);
	pTextInfoBar->EnableGadget(_R(IDC_SPACING_MORE),Enable);

	pTextInfoBar->EnableGadget(_R(IDC_AUTOKERN),Enable);
	//pTextInfoBar->EnableGadget(_R(IDC_KERN_EDIT_Y2),Enable);

}
	 

/********************************************************************************************

>void TextInfoBarOp::SetCurrentFontUnderLine(BOOL IsUnderLine)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to set the current UnderLine State
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::SetCurrentFontUnderLine(BOOL IsUnderLine)
{
	InfoData.UnderLine = IsUnderLine;
}

/********************************************************************************************

>void TextInfoBarOp::SetCurrentTracking(INT32 Tracking)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Purpose:	Called to set the current Tracking

********************************************************************************************/

BOOL TextInfoBarOp::SetCurrentTracking(INT32 Tracking)
{
	
	if(InfoData.Tracking == Tracking)
		return FALSE;
	InfoData.Tracking = Tracking;
	return TRUE;
	
}	 
  	 
/********************************************************************************************

>BOOL TextInfoBarOp::SetCurrentAspectRatio(FIXED16 AspectRatio)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Returns:	FALSE if value unchanged TRUE otherwise
	Purpose:	Called to set the current AspectRatio

********************************************************************************************/

BOOL TextInfoBarOp::SetCurrentAspectRatio(FIXED16 AspectRatio)
{
	if(InfoData.AspectRatio == AspectRatio)
		return FALSE;
	InfoData.AspectRatio = AspectRatio;
	return TRUE;
}	 

/********************************************************************************************

>BOOL TextInfoBarOp::SetCurrentBaseLineShift(INT32 BaseLine)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Returns:	FALSE if value unchanged TRUE otherwise
	Purpose:	Called to set the current BaseLineShift

********************************************************************************************/

BOOL TextInfoBarOp::SetCurrentBaseLineShift(INT32 BaseLine)
{
	if(InfoData.BaseLineShift == BaseLine)
		return FALSE;
	InfoData.BaseLineShift = BaseLine;
	return TRUE;

}	
/********************************************************************************************

>BOOL TextInfoBarOp::SetCurrentLineSpace(MILLIPOINT LineSpace)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if value unchanged TRUE otherwise
	Purpose:	Called to set the current Line Spacing absolute
				Set LineSpacePercent zero to indicate which mode we are in(Relative/ABSOLUTE)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL TextInfoBarOp::SetCurrentLineSpace(MILLIPOINT LineSpace)
{
	if(	InfoData.LineSpace == LineSpace)
		return FALSE;
	InfoData.LineSpace = LineSpace;
	InfoData.IsLineSpaceAPercent = FALSE;
	return TRUE;
}
/********************************************************************************************

>BOOL TextInfoBarOp::SetCurrentLineSpacePercent(double Percent)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if value unchanged TRUE otherwise
	Purpose:	Called to set the current Line Spacing as a relative percent
				Set LineSpace zero to indicate which mode we are in(RELATIVE/Absolute)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL TextInfoBarOp::SetCurrentLineSpacePercent(double Percent)
{
	if(InfoData.LineSpacePercent == Percent)
		return FALSE;
	InfoData.LineSpacePercent = Percent;
	InfoData.IsLineSpaceAPercent = TRUE;
	return TRUE;

}
/********************************************************************************************

>void TextInfoBarOp::SetCurrentHorizontalKern(INT32 Kern)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if value unchanged TRUE otherwise
	Purpose:	Called to set the current HorizontalKern
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL TextInfoBarOp::SetCurrentHorizontalKern(INT32 Kern)
{
	if(InfoData.HorizontalKern == Kern)
		return FALSE;
	
	InfoData.HorizontalKern = Kern;
	return TRUE;
}

/********************************************************************************************

>void TextInfoBarOp::SetCurrentFontName(String_64 * Name, FontClass Type, BOOL Cache)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to set the current font name (and now Class as well)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL TextInfoBarOp::SetCurrentFontName(String_64 * Name, FontClass Type, BOOL Cache)
{
	ERROR2IF(Name == NULL,FALSE,"Null FontName");

	if(InfoData.FontName == *Name && InfoData.FontType == Type)
		return FALSE;

	InfoData.FontName = *Name;
	InfoData.FontType = Type;

	if (Cache)
		FONTMANAGER->CacheNamedFont(Name, Type);

PORTNOTE("text", "Removed custom dropdown");
#ifndef EXCLUDE_FROM_XARALX
	NameDropDown->SetTopFontName(Name, Type, FALSE);
	FontDropItem Dummy(*Name, Type);
	NameDropDown->SetSelection(&Dummy);
#endif

	return TRUE;
}

/********************************************************************************************

>void TextInfoBarOp::RedrawUnitGadgets()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Force a redraw of all gadgets that work in units 
				called on units changed message
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::RedrawUnitGadgets()
{
	if ((pTextInfoBar != NULL) && pTextInfoBar->HasWindow())
	{
		pTextInfoBar->SetUnitGadgetValue(_R(IDC_POINT_COMBO),CurrentFontUnits,InfoData.FontSize,0,-1);
		pTextInfoBar->SetUnitGadgetValue(_R(IDC_KERN_EDIT_Y),CurrentFontUnits,InfoData.BaseLineShift,0,-1); 
	}
	SetLineSpaceGadget();

}

/********************************************************************************************

>void TextInfoBarOp::SetLineSpaceGadget()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Set up the line space edit field
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::SetLineSpaceGadget()
{
	if ((pTextInfoBar != NULL) && pTextInfoBar->HasWindow())
	{
 		if(InfoData.IsLineSpaceAPercent == FALSE)
		{
			pTextInfoBar->SetUnitGadgetValue(_R(IDC_SPACING_EDIT),CurrentFontUnits,InfoData.LineSpace,0,-1);
		}
		else
		{ 	
		 	String_256 StrValue;
			// Convert::DoubleToString(InfoData.LineSpacePercent, &StrValue);
			// StrValue += String_8(_R(IDS_TEXTINFO_PERCENT));			
			StrValue.MakePercent(InfoData.LineSpacePercent);
			pTextInfoBar->SetStringGadgetValue(_R(IDC_SPACING_EDIT), StrValue, 0,-1);
		}
	}
}

/********************************************************************************************

>void TextInfoBarOp::DoKernBumps(UINT32 Button)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to "bump" kerning values
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::DoKernBumps(UINT32 Button)
{
	if (Button == _R(IDC_KERN_BUMP_X_LESS))
	{
		if(InfoData.HorizontalKern>=-(KernLimit-KernNudge))
		{
			SetCurrentHorizontalKern(InfoData.HorizontalKern-KernNudge);
			pTextInfoBar->SetLongGadgetValue(_R(IDC_KERN_EDIT_X),InfoData.HorizontalKern,0,-1); 	
		}
	}
	else if (Button == _R(IDC_KERN_BUMP_X_MORE))
	{
		if(InfoData.HorizontalKern<=(KernLimit-KernNudge))
		{
			SetCurrentHorizontalKern(InfoData.HorizontalKern+KernNudge);
			pTextInfoBar->SetLongGadgetValue(_R(IDC_KERN_EDIT_X),InfoData.HorizontalKern,0,-1); 	
		}
	}
	else if (Button == _R(IDC_KERN_BUMP_Y_LESS))
	{
		if ((InfoData.BaseLineShift != INVALID_ATTVAL) && (InfoData.BaseLineShift>=-(BaseLineLimit-BaseLineNudge)))
		{
			SetCurrentBaseLineShift(InfoData.BaseLineShift-BaseLineNudge);
			pTextInfoBar->SetUnitGadgetValue(_R(IDC_KERN_EDIT_Y),CurrentFontUnits,InfoData.BaseLineShift,0,-1);
		}
	}
	else if (Button == _R(IDC_KERN_BUMP_Y_MORE))
	{
		if ((InfoData.BaseLineShift != INVALID_ATTVAL) && (InfoData.BaseLineShift<=(BaseLineLimit-BaseLineNudge)))
		{
			SetCurrentBaseLineShift(InfoData.BaseLineShift+BaseLineNudge);
			pTextInfoBar->SetUnitGadgetValue(_R(IDC_KERN_EDIT_Y),CurrentFontUnits,InfoData.BaseLineShift,0,-1);
		}
	}
}


/********************************************************************************************

>void TextInfoBarOp::DoTrackingBumps(UINT32 Button)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to "bump" tracking values
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::DoTrackingBumps(UINT32 Button)
{
	if (InfoData.Tracking == INVALID_ATTVAL)
		return;

	if (Button == _R(IDC_TRACKING_LESS))
	{
		if(InfoData.Tracking>=-(TrackingLimit-TrackingNudge))
		{
			SetCurrentTracking(InfoData.Tracking-TrackingNudge);
			pTextInfoBar->SetLongGadgetValue(_R(IDC_TRACKING_EDIT),InfoData.Tracking,0,-1); 	
		}
	}
	else if (Button == _R(IDC_TRACKING_MORE))
	{
		if(InfoData.Tracking<=(TrackingLimit-TrackingNudge))
		{
			SetCurrentTracking(InfoData.Tracking+TrackingNudge);
			pTextInfoBar->SetLongGadgetValue(_R(IDC_TRACKING_EDIT),InfoData.Tracking,0,-1); 	
		}
	}
}

/********************************************************************************************

>void TextInfoBarOp::DoLineSpacingBumps(UINT32 Button)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to "bump" Line Space values
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::DoLineSpacingBumps(UINT32 Button)
{
	BOOL ChangedValue = FALSE;

	if (Button == _R(IDC_SPACING_LESS))
	{
		if(InfoData.IsLineSpaceAPercent == FALSE)
		{
			if(InfoData.LineSpace == INVALID_ATTVAL)
				return;
			if(InfoData.LineSpace>=-(LineSpaceLimit-LineSpaceNudge))
			{
				SetCurrentLineSpace(InfoData.LineSpace-LineSpaceNudge);
				ChangedValue = TRUE;
			}
		}
		else
		{
			if(InfoData.LineSpacePercent == INVALID_ATTVAL)
				return;
			if(InfoData.LineSpacePercent>=(LineSpacePercentMin+LineSpacePercentNudge))
			{
				SetCurrentLineSpacePercent(InfoData.LineSpacePercent - LineSpacePercentNudge);
				ChangedValue = TRUE;
			}
		}
	}
	else if (Button == _R(IDC_SPACING_MORE))
	{
		if(InfoData.IsLineSpaceAPercent == FALSE)
		{
			if(InfoData.LineSpace == INVALID_ATTVAL)
				return;
			if(InfoData.LineSpace<=(LineSpaceLimit-LineSpaceNudge))
			{
				SetCurrentLineSpace(InfoData.LineSpace+LineSpaceNudge);
				ChangedValue = TRUE;
			}
		}
		else
		{
			if(InfoData.LineSpacePercent == INVALID_ATTVAL)
				return;
			if(InfoData.LineSpacePercent<=(LineSpacePercentMax - LineSpacePercentNudge))
			{
				SetCurrentLineSpacePercent(InfoData.LineSpacePercent + LineSpacePercentNudge);
				ChangedValue = TRUE;
			}
		}
	}

	if(ChangedValue)
		SetLineSpaceGadget();
}

/********************************************************************************************

> static void TextInfoBarOp::DoFontChange()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called by Mainframe when a sys FontChange message is received
				Validates the font cache and updates the font menu 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::DoFontChange()
{
	// force a reformat and redraw of all text stories on  all documents 
	
	// Get a pointer to the StartDocument 
	/*
	
	Document* pDocument = (Document*) Camelot.Documents.GetHead();
	DocRect InvalidRgn;
	DocRect NewRect;

	while (pDocument != NULL)
	{
	
  		// Get a pointer to the StartDocument node	
		Node* pStartNode = pDocument->GetFirstNode();
	 	ERROR3IF(pStartNode == NULL, "(BaseDocument::PostImport) TreeStart was NULL");
		BOOL ok = TRUE;

		// Get a pointer to the NodeDocument node
		if (pStartNode != NULL)
		{
			pStartNode = pStartNode->FindNext();
		 	ERROR3IF(pStartNode == NULL, "(BaseDocument::PostImport) No NodeDocument after StartDocument");
		}

		// Scan through the tree, formating and redrawing all text stories
		if (pStartNode != NULL)
		{
			Node* pCurrentNode = pStartNode->FindFirstDepthFirst();

			while (pCurrentNode != NULL)
			{
				if(IS_A(pCurrentNode,TextStory))
				{
					TextStory* pTextStory=(TextStory*)pCurrentNode;
			  		
			  		InvalidRgn = pTextStory->GetUnionBlobBoundingRect();
					pTextStory->FlagAffectedByOp();
			  		ok=pTextStory->FormatAndChildren();
				
					if (ok)		
					{
						NewRect =  pTextStory->GetUnionBlobBoundingRect();
						InvalidRgn = InvalidRgn.Union(NewRect);
					}
					
				  	if (ok)
						pDocument->ForceRedraw(pTextStory->FindParentSpread(), InvalidRgn, TRUE);
			  
				}
				pCurrentNode = pCurrentNode->FindNextDepthFirst(pStartNode);
			}//  end depth first search loop
		}

		pDocument->FlushRedraw();
		pDocument = (Document*) Camelot.Documents.GetNext(pDocument);
	}// end reformat loop

	*/

	OpAffectFontChange* pFontChange = new OpAffectFontChange();
	if (pFontChange!=NULL)
		pFontChange->DoAffectFontChange();

PORTNOTE("text", "Removed custom dropdown");
#ifndef EXCLUDE_FROM_XARALX
	// Update the text info bar font drop list (if it's there)
	if(NameDropDown != NULL)
	{
		NameDropDown->KillList();
		NameDropDown->FillInFontList();
		FontDropItem Dummy(InfoData.FontName, InfoData.FontType);
		NameDropDown->SetSelection(&Dummy);
	}
#endif

	// Update the actual combobox if the texttool is active
	if(pTextInfoBar!= NULL &&pTextInfoBar->HasWindow())
	{		
		pTextInfoBar->SetComboListLength(_R(IDC_FONT_COMBO));
	 	Update();
	}
}


/********************************************************************************************
>	static BOOL TextInfoBarOp::StringToDouble(StringBase* pstrIn, double* pnOut,BOOL IsMultiple)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> (thanks to JustinF)
	Created:	31/5/95
	Inputs:		pstrIn			---		pointer to the string containing text
				pnOut			--- 	pointer to the number which will contain the result
				pIsMultiple		---		pointer to a BOOL which indicates a mulitple
	Outputs:	-
	Returns:	TRUE if the text is successfully parsed, FALSE otherwise.
	Purpose:	Parses the text within the zoom factor combo-box, checking for a percentage
				factor ("%") or a multiplier factor ("x"), converting it to a number.
	Errors:		-
	SeeAlso:	OpZoomComboDescriptor::OnSelectionChange; DialogOp::GetLongGadgetValue
********************************************************************************************/

BOOL TextInfoBarOp::StringToDouble(StringBase* pstrIn, double* pnOut,BOOL * pIsMultiple)
{
	ENSURE(pstrIn != NULL && pnOut != NULL,
			"Null parameter(s) in TextInfoBarOp::StringToLong");

	// Remove any leading or trailing whitespace.  psczStart is set to point to first non-
	// whitespace character (or the terminating null), pszEnd points to the last.  If the
	// string is entirely whitespace these two will cross over, and we return FALSE.
	const TCHAR* psczStart = (const TCHAR*) (*pstrIn);
	const TCHAR* psczForward = psczStart;
	const TCHAR* psczBack = psczStart + pstrIn->Length() - 1;
	
	*pIsMultiple = FALSE;
	
	String_16 space(_R(IDS_TEXTINFO_PARSE_SPACE));
	while (*psczForward == *((TCHAR *)space)/*TEXT(' ')*/) 
		psczForward++;
	
	while (psczBack > psczForward && *psczBack == *((TCHAR *)space)/*TEXT(' ')*/) 
		psczBack--;
	
	if (psczForward > psczBack) 
		return FALSE;

	// Check if the string ends with a '%' or an 'x'.  If it is an 'x' then the number
	// is a multipler, eg. "2 x" (two times).  If it is a '%', or not there at all,
	// then it represents a percentage.
	String_16 smallx(_R(IDS_TEXTINFO_PARSE_SMALLX));
	String_16 largex(_R(IDS_TEXTINFO_PARSE_LARGEX));
	String_16 percent(_R(IDS_TEXTINFO_PARSE_PERCENT));
	BOOL bIsMultiplier = FALSE;
	if (	*psczBack == *((TCHAR *)smallx)/*TEXT('x')*/
		 ||	*psczBack == *((TCHAR *)largex)/*TEXT('X')*/)
	{
		// Parse a multiplier.  Skip over the 'x'.
		psczBack--;
		bIsMultiplier = TRUE;
	}
	else if (*psczBack == *((TCHAR *)percent) /*TEXT('%'))*/)
	{
		// Parse a percentage.  Skip over the '%'
		psczBack--;
	}
	else if (!StringBase::IsNumeric(*psczBack))
	{
		// Can't recognise the format - not a number.
		return FALSE;
	}

	// Make a working copy of what is left of the string.
	String_256 strWork;
	pstrIn->Mid(&strWork, (INT32) (psczForward - psczStart),
				(INT32) (psczBack - psczForward) + 1);

	if (!Convert::StringToDouble(strWork, pnOut)) 
		return FALSE;
	
	// Make sure it's within allowed bounds.
	INT32 nMaxMultiple = LineSpacePercentMax/100;
	INT32 nMinMultiple = LineSpacePercentMin/100;

	if (*pnOut > LineSpacePercentMax)
		*pnOut = LineSpacePercentMax;
	if (*pnOut < LineSpacePercentMin)
		*pnOut = LineSpacePercentMin;
    
    if (bIsMultiplier) 
    {
    	*pIsMultiple = TRUE;
		if (*pnOut > nMaxMultiple)
			*pnOut = nMaxMultiple;
		if (*pnOut < nMinMultiple)
			*pnOut = nMinMultiple;	
	}
	return TRUE;
}

/********************************************************************************************

>	MsgResult TextInfoBarOp::(Msg* Message) 

	Author:		Mark_Goodall (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		Message = The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Text info bar dialog message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::DoInputError(UINT32 GadgetID) 
{
 	String_256 sErrString;
	String_64 sWarnString(_R(IDS_INVALID_FONTSIZE));
	String_8 sAndString(_R(IDS_AND));
	String_16 sMaxStr(_T("0"));
	String_16 sMinStr(_T("0"));

	if (GadgetID == _R(IDC_TRACKING_EDIT))
	{
		sMinStr._MakeMsg(TEXT("#1%dems"),-TrackingLimit);
		sMaxStr._MakeMsg(TEXT("#1%dems"),TrackingLimit);
	}
	else if (GadgetID == _R(IDC_KERN_EDIT_X))
	{
		sMinStr._MakeMsg(TEXT("#1%dems"),-KernLimit);
		sMaxStr._MakeMsg(TEXT("#1%dems"),KernLimit);
	}	
	else if (GadgetID == _R(IDC_KERN_EDIT_Y))
	{
		Convert::MillipointsToString(-BaseLineLimit, MILLIPOINTS, &sMinStr);
		Convert::MillipointsToString(BaseLineLimit, MILLIPOINTS, &sMaxStr);
	}
	else if (GadgetID == _R(IDC_SPACING_EDIT))
	{
		Convert::MillipointsToString(-LineSpaceLimit, MILLIPOINTS, &sMinStr);
		Convert::MillipointsToString(LineSpaceLimit, MILLIPOINTS, &sMaxStr);
	}
	else if (GadgetID == _R(IDC_ASPECTEDIT))
	{
		sMinStr._MakeMsg(TEXT("#1%d%"),FontAspectMin);
		sMaxStr._MakeMsg(TEXT("#1%d%"),FontAspectMax);
	}
	else if (GadgetID == _R(IDC_POINT_COMBO))
	{
		Convert::MillipointsToString(FontSizeMax, CurrentFontUnits, &sMaxStr);
		Convert::MillipointsToString(FontSizeMin, CurrentFontUnits, &sMinStr);  
	}
	else
	{
		return;
	}

	sErrString._MakeMsg(_T("#1%s #2%s #3%s #4%s\n"),
						(TCHAR*)sWarnString,
						(TCHAR*)sMinStr,
						(TCHAR*)sAndString,
						(TCHAR*)sMaxStr); 
			
	Error::SetError(0,(TCHAR* )sErrString,0);
	InformError();
}


/********************************************************************************************

>	MsgResult TextInfoBarOp::(Msg* Message) 

	Author:		Mark_Goodall (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		Message = The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Text info bar dialog message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult TextInfoBarOp::Message(Msg* Message) 
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		if (Msg->DlgMsg == DIM_CANCEL)
		{
			Close();  
		}
		else if (Msg->DlgMsg == DIM_CREATE)
		{
			// Initialise the infobar controls here
			// This is sent when you create the infobar in your tool startup code
			InitControls();
			
			BOOL Greyed = (OpTextKern::GetState(NULL, NULL)).Greyed;
			pTextInfoBar->EnableGadget(_R(IDC_KERN_EDIT_X),!Greyed);
			pTextInfoBar->EnableGadget(_R(IDC_KERN_BUMP_X_LESS),!Greyed);
			pTextInfoBar->EnableGadget(_R(IDC_KERN_BUMP_X_MORE),!Greyed);

			Greyed = (OpTextAutoKern::GetState(NULL, NULL)).Greyed;
			pTextInfoBar->EnableGadget(_R(IDC_AUTOKERN),!Greyed);
		}
		else
		{
			if (FALSE) {}
			else if (Msg->GadgetID == _R(IDC_SUPERSCRIPT))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_LFT_BN_CLICKED:
						SetCurrentScript(SuperScript);
						OnFieldChange(ScriptA);		
						break;
					default:
						break;
				}
			}
			else if (Msg->GadgetID == _R(IDC_SUBSCRIPT))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_LFT_BN_CLICKED:
						SetCurrentScript(SubScript);
						OnFieldChange(ScriptA);	
						break;
					default:
						break;
				}
			}
			else if (Msg->GadgetID == _R(IDC_KERN_EDIT_Y))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED :
						{
							BOOL Valid= FALSE;
							MILLIPOINT  BaseShift = GetUnitGadgetValue(_R(IDC_KERN_EDIT_Y),CurrentFontUnits, 
												-BaseLineLimit,BaseLineLimit,0,&Valid);
							if(Valid)
							{
								if(SetCurrentBaseLineShift(BaseShift))
									OnFieldChange(BaseLineShiftA);
							}
							else
							{
								DoInputError(Msg->GadgetID);
								SetUnitGadgetValue(_R(IDC_KERN_EDIT_Y),CurrentFontUnits,InfoData.BaseLineShift,0,-1);
							}
						}
						break;
					default:
						break;
				}
			}
			else if (Msg->GadgetID == _R(IDC_KERN_EDIT_X))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED :
					{
						BOOL Valid= FALSE;
						INT32 Kern = GetLongGadgetValue(_R(IDC_KERN_EDIT_X) , 
											-KernLimit,KernLimit,0,&Valid);
						if(Valid)
						{
							if(SetCurrentHorizontalKern(Kern))
								OnFieldChange(HorizontalKernA);
						}
						else
						{
							DoInputError(Msg->GadgetID);
							SetLongGadgetValue(_R(IDC_KERN_EDIT_X),InfoData.HorizontalKern,0,-1); 
						}
					}
					break;
					default:
						break;
				}
			}
			else if ((Msg->GadgetID == _R(IDC_SPACING_MORE)) || (Msg->GadgetID == _R(IDC_SPACING_LESS)))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_LFT_BN_CLICKED:
					{
						DoLineSpacingBumps(Msg->GadgetID);
						break;
					}
					case DIM_LFT_BN_UP:
					{
						if(InfoData.IsLineSpaceAPercent == FALSE)
							OnFieldChange(LineSpaceA);
						else
							OnFieldChange(LineSpacePercentA);
						break;
					}
					default:
						break;
				}
			}
			else if ((Msg->GadgetID == _R(IDC_TRACKING_MORE)) || (Msg->GadgetID == _R(IDC_TRACKING_LESS)))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_LFT_BN_CLICKED:
					{
						DoTrackingBumps(Msg->GadgetID);
						break;
					}
					case DIM_LFT_BN_UP:
					{
						OnFieldChange(TrackingA);
						break;
					}
					default:
						break;
				}
			}
			else if ((Msg->GadgetID == _R(IDC_KERN_BUMP_X_MORE)) || (Msg->GadgetID == _R(IDC_KERN_BUMP_X_LESS)))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_LFT_BN_CLICKED:
					{
						DoKernBumps(Msg->GadgetID);
						break;
					}
					case DIM_LFT_BN_UP:
					{
						OnFieldChange(HorizontalKernA);
						break;
					}
					default:
						break;
				}
			}
			else if ((Msg->GadgetID == _R(IDC_KERN_BUMP_Y_MORE)) || (Msg->GadgetID == _R(IDC_KERN_BUMP_Y_LESS)))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_LFT_BN_CLICKED:
					{
						DoKernBumps(Msg->GadgetID);
						break;
					}
					case DIM_LFT_BN_UP:
					{
						OnFieldChange(BaseLineShiftA);
						break;
					}
					default:
						break;
				}
			}
			else if (Msg->GadgetID == _R(IDC_SPACING_EDIT))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED :
					{
						BOOL Valid= FALSE;
						
						String_256 Str = GetStringGadgetValue(_R(IDC_SPACING_EDIT),&Valid);
						if(!Valid)
							break;

						double Percentage =0;
						BOOL IsMultiple = FALSE;
						BOOL WasAPercent = StringToDouble(&Str, &Percentage,&IsMultiple);
						if(WasAPercent )
						{
							// was it a multiple
							if(IsMultiple)
								Percentage *= 100;
							if(SetCurrentLineSpacePercent(Percentage))
								OnFieldChange(LineSpacePercentA);	
						}
						else
						{

							MILLIPOINT  Spacing = GetUnitGadgetValue(_R(IDC_SPACING_EDIT),CurrentFontUnits, 
												-LineSpaceLimit,LineSpaceLimit,0,&Valid);
							if(Valid)
							{
								if(SetCurrentLineSpace(Spacing))
									OnFieldChange(LineSpaceA);
							}
							else
							{
								DoInputError(Msg->GadgetID);
								SetLineSpaceGadget();
							}
						}
						break;
					}
					default:
						break;
				}
			}
			else if (Msg->GadgetID == _R(IDC_TRACKING_EDIT))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED :
					{
						BOOL Valid= FALSE;
						INT32 TrackingVal = GetLongGadgetValue(_R(IDC_TRACKING_EDIT), 
											-TrackingLimit,TrackingLimit,0,&Valid);
						if(Valid)
						{
							if(SetCurrentTracking(TrackingVal))
								OnFieldChange(TrackingA);
						}
						else
						{
							DoInputError(Msg->GadgetID);
							SetDoubleGadgetValue(_R(IDC_TRACKING_EDIT),InfoData.Tracking,0,-1);
						}
						break;
					}
					default:
						break;
				}
			}
			else if (Msg->GadgetID == _R(IDC_ASPECTEDIT))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED :
					{
						BOOL Valid= FALSE;
						FIXED16 Ratio = GetDoubleGadgetValue(_R(IDC_ASPECTEDIT), 
											FontAspectMin,FontAspectMax,0,&Valid)/100;
						if(Valid)
						{
							if(SetCurrentAspectRatio(Ratio))
								OnFieldChange(AspectRatioA);
						}
						else
						{
							DoInputError(Msg->GadgetID);
							SetDoubleGadgetValue(_R(IDC_ASPECTEDIT),InfoData.AspectRatio.MakeDouble()*100,0,-1); 
						}
						break;
					}
					default:
						break;
				}
			}
			else if (
				(Msg->GadgetID == _R(IDC_JUSTIFYFULL)) ||
				(Msg->GadgetID == _R(IDC_JUSTIFYLEFT)) ||
				(Msg->GadgetID == _R(IDC_JUSTIFYRIGHT)) ||
				(Msg->GadgetID == _R(IDC_JUSTIFYCENTRE))
				)
			{
				switch (Msg->DlgMsg)
				{
					case DIM_LFT_BN_CLICKED:
					{
						SetCurrentJustify(Msg->GadgetID);
						OnFieldChange(JustifyA);		
						break;
					}
					default:
						break;
				}
			}
			else if (Msg->GadgetID == _R(IDC_BOLDBUTTON))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_LFT_BN_CLICKED:
					{
						SetCurrentFontBold(!InfoData.Bold);
						OnFieldChange(BoldA);	
						break;
					}
					default:
						break;
				}
			}
			else if (Msg->GadgetID == _R(IDC_ITALICBUTTON))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_LFT_BN_CLICKED:
					{
						SetCurrentFontItalic(!InfoData.Italic);	
						OnFieldChange(ItalicA);
						break;
					}
					default:
						break;
				}
			}
			else if (Msg->GadgetID == _R(IDC_AUTOKERN))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_LFT_BN_CLICKED:
					{
						SetCurrentAutoKerning(!InfoData.AutoKerning);
						OnFieldChange(AutoKernText);	
						break;
					}
					default:
						break;
				}
			}
			else if (Msg->GadgetID == _R(IDC_POINT_COMBO))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED :
					{
						BOOL Valid= FALSE;
						MILLIPOINT  Size = GetUnitGadgetValue(_R(IDC_POINT_COMBO),CurrentFontUnits, 
											FontSizeMin,FontSizeMax,0,&Valid);
						if(Valid)
						{
							if(SetCurrentPointSize(Size))
								OnFieldChange(FontSizeA);
						}
						else
							DoInputError(Msg->GadgetID);

						SetUnitGadgetValue(_R(IDC_POINT_COMBO),CurrentFontUnits,InfoData.FontSize,0,-1);
						break;
					}
					default:
						break;
				}
			}
			else if (Msg->GadgetID == _R(IDC_FONT_COMBO))
			{
				if(Msg->DlgMsg == DIM_SELECTION_CHANGED_COMMIT)
				{
PORTNOTE("text", "Removed custom dropdown");
#ifndef EXCLUDE_FROM_XARALX
					// Handle selections in the font name menu.
					INT32 SelIndex = GetSelectedValueIndex(_R(IDC_FONT_COMBO));
					
					if(SelIndex != -1)
					{
						if (NameDropDown != NULL)
						{
							FontDropItem *Selected = NameDropDown->DecodeSelection((INT32)SelIndex);

							if (FONTMANAGER->IsFontInstalled(&Selected->FontName, Selected->Type))
							{
								if (SetCurrentFontName(&Selected->FontName, Selected->Type, TRUE))
									OnFieldChange(FontNameA);
							}
							else
							{
								InformWarning(_R(IDS_INVALIDFONT));
								Update();
							}			 
						}
					}
#endif
				}
			}
		}
	}
		// Does this message mean that the selected object has changed?
	else if (MESSAGE_IS_A(Message, SelChangingMsg))
	{
		// Re-render the tool blobs as they may have moved
		if (pTextTool!=NULL && pTextTool->IsCurrent())
		{
			if (!pTextTool->SelectionHasChanged())
				InformError();
		}

		if(pTextInfoBar==NULL || !pTextInfoBar->HasWindow())
			return(InformationBarOp::Message(Message));

		// update the info bar according to the selection
		BOOL Greyed = (OpTextKern::GetState(NULL, NULL)).Greyed;
		pTextInfoBar->EnableGadget(_R(IDC_KERN_EDIT_X),!Greyed);
		pTextInfoBar->EnableGadget(_R(IDC_KERN_BUMP_X_LESS),!Greyed);
		pTextInfoBar->EnableGadget(_R(IDC_KERN_BUMP_X_MORE),!Greyed);

		Greyed = (OpTextAutoKern::GetState(NULL, NULL)).Greyed;
		pTextInfoBar->EnableGadget(_R(IDC_AUTOKERN),!Greyed);

		Update();
	}

	// How about a view changing message?
	else if (MESSAGE_IS_A(Message, DocViewMsg))
	{
		if(Document::GetSelected()== NULL)
			EnableGadgets(FALSE);
		else
			EnableGadgets(TRUE);

		DocViewMsg* msg = (DocViewMsg*) Message;

		// Render the tool blobs off the old view just before it changes
		if (msg->State == DocViewMsg::SELABOUTTOCHANGE)
		{
			if (msg->pOldDocView!=NULL && pTextTool!=NULL && !GetApplication()->CamelotIsDying())
			{
				// Do additional checks here to avoid ensures!
				CCamView* pOilView = msg->pOldDocView->GetConnectionToOilView();
				BlobManager* pBlobManager = GetApplication()->GetBlobManager();

PORTNOTE("text", "Removed IsScreenCamView() test");
				if (pOilView!=NULL /*&& pOilView->IsScreenCamView()*/)
					pBlobManager->RenderToolBlobsOff(pTextTool, msg->pOldDocView->GetVisibleSpread(), NULL);
			}
		}
	}
	
	// How about a document changing message?
	else if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg* Msg = (DocChangingMsg*)Message;
		pDoc = Document::GetSelected();
		
		if(pDoc)
			CurrentFontUnits = pDoc->GetDocFontUnits();	
		
		if (Msg->State == DocChangingMsg::SELCHANGED)
		{
			// Clear the focus story
			TextStory::SetFocusStory(NULL);

			// Now check that we havent got a selected caret outside the text tool.
			if (!TextTool::IsCurrentTool())
			{
				SelRange *pSelection = GetApplication()->FindSelection();
				if (pSelection != NULL)
				{
					BOOL ChangedSel = FALSE;
					Node* pNode = pSelection->FindFirst();
					while (pNode != NULL)
					{
						if (pNode->IsAVisibleTextNode() && ((VisibleTextNode*)pNode)->IsACaret())
						{
							((CaretNode*)pNode)->SetSelected(FALSE);
							ChangedSel = TRUE;
						}

						pNode = pSelection->FindNext(pNode);
					}

					if (ChangedSel)
						GetApplication()->UpdateSelection();
				}
			}
		 	Update(TRUE);
		}
		
	}
	else if(MESSAGE_IS_A(Message,OptionsChangingMsg))
	{

		OptionsChangingMsg* Msg = (OptionsChangingMsg*)Message;

		if(Msg->State == OptionsChangingMsg::NEWUNITS)
		{
			pDoc = Document::GetSelected();
			if(pDoc)
				CurrentFontUnits = pDoc->GetDocFontUnits();	
			RedrawUnitGadgets();
		}
		
	}

	// or possibly an op message?
  	else if (MESSAGE_IS_A(Message,OpMsg))
	{
		OpMsg* pOpMsg = (OpMsg*)Message;

		if (pOpMsg->MsgType == OpMsg::END)
		{
			// an operation has ended, was it the one we were after?
			if (IS_A(pOpMsg->pOp, OpCut))
			{
				if (RegainCaretAfterOp)
				{
					// make sure we turn this off after the op has come through
					RegainCaretAfterOp = FALSE;

					if (!(pOpMsg->pOp->GetOpFlgs()).Failed)
					{
						TextStory* pStory = TextStory::GetFocusStory();
						if (pStory != NULL)
						{
							CaretNode* pCaret = pStory->GetCaret();
							if (pCaret != NULL)
							{
								pCaret->SetSelected(TRUE);
								pStory->AttachCaretAttributes();
								GetApplication()->FindSelection()->Update();
							}
						}
					}
				}
			}
		}
	}

	else if (MESSAGE_IS_A(Message,LocaleChangedMsg))
	{
		if (pTextTool!= NULL)
			pTextTool->LocaleChanged();
	}

	// Pass the message on to the immediate Text class
	return (InformationBarOp::Message(Message));
}    


/********************************************************************************************

>void TextInfoBarOp::InitControls()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialise the info bar
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void TextInfoBarOp::InitControls()
{
	// current info bar public for callbacks argh..
	pTextInfoBar = this;

PORTNOTE("text", "Removed custom dropdown");
#ifndef EXCLUDE_FROM_XARALX
	// Only enum the fonts if it's the first time - should respond to font change messages now !
	if (NameDropDown == NULL)
	{
		NameDropDown = new FontDropDown;
	}

	if(NameDropDown != NULL)
	{
		NameDropDown->Init((CWindowID)WindowID, _R(IDC_FONT_COMBO));
		NameDropDown->FillInFontList();
	}
#endif

	pTextInfoBar->SetComboListLength(_R(IDC_FONT_COMBO));
 	UpdateGadgets();
  	
	// Point Size 
	SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_8PT) );
	SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_10PT));
	SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_11PT));
	SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_12PT));
	SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_14PT));
	SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_16PT));
	SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_18PT));
	SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_20PT));
	SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_24PT));
	SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_28PT));
	SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_32PT));
	SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_36PT));
	SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_48PT));
	SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_72PT));
	SetComboListLength(_R(IDC_POINT_COMBO));
	
 	
	if(Document::GetSelected()!= NULL)
 		SetUnitGadgetValue(_R(IDC_POINT_COMBO),CurrentFontUnits,InfoData.FontSize,0,-1);
	else
		SetStringGadgetValue(_R(IDC_POINT_COMBO), _R(IDS_TEXTINFO_16PT),0,-1);

	if(Document::GetSelected()== NULL)
		EnableGadgets(FALSE);
	
	Update(TRUE);
	
}	   
/********************************************************************************************

>void TextInfoBarOp::AddFontToCombo(String_64 * FontName)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to add a font name to the end of the font combo box
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TextInfoBarOp::AddFontToCombo(String_64 * FontName)
{
//	if(pTextInfoBar)
//   		pTextInfoBar->SetStringGadgetValue(_R(IDC_FONT_COMBO),FontName,TRUE,0); 
}


/********************************************************************************************

>	static void TextInfoBarOp::RegainCaretOnOpEnd()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/6/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Used to try to regain the caret after an operation has completed.
				Currently only used from OpCut.

********************************************************************************************/

void TextInfoBarOp::RegainCaretOnOpEnd()
{
	RegainCaretAfterOp = TRUE;
}



/********************************************************************************************
>	BOOL TextInfoBarOp::UpdateFieldsAfterTyping()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called on idle events after typing.  As simple typing does not send a
				SelChanged message (in a vain attempt to make typing faster) this function is
				called on idles in order to update the parts on the infobar that may have
				been affected by the typing.
********************************************************************************************/

BOOL TextInfoBarOp::UpdateFieldsAfterTyping()
{
	// Skip out now if there isn't an infobar
	if (pTextInfoBar==NULL || !pTextInfoBar->HasWindow())
		return TRUE;

	// Update the horizontal kern field
	TextStory * ActiveStory = TextStory::GetFocusStory();
	if (ActiveStory != NULL)
	{
		CaretNode * pCaret = ActiveStory->GetCaret();
		if (pCaret != NULL)
		{
			VisibleTextNode * LastNode = pCaret->FindPrevAbstractTextCharInStory();
			if (LastNode != NULL)
			{
				if (IS_A(LastNode, KernCode))
					pTextInfoBar->SetLongGadgetValue(_R(IDC_KERN_EDIT_X),((KernCode*)LastNode)->GetValue().x,0,-1);
				else
					pTextInfoBar->SetLongGadgetValue(_R(IDC_KERN_EDIT_X), 0, 0, -1);

				pTextInfoBar->PaintGadgetNow(_R(IDC_KERN_EDIT_X)); 
			}
		}
	}

	return TRUE;
}
