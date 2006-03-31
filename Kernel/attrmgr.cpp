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
//*/ 

// Include files 
#include "camtypes.h"
#include "attrmgr.h"
//#include "mario.h"
#include "fixmem.h"
#include "app.h"
#include "toollist.h"
//#include "simon.h"
#include "attrappl.h"
#include "optsmsgs.h"
//#include "resource.h"
#include "qualattr.h"
//#include "jason.h"
#include "nodetext.h"
//#include "will.h"
#include "helpuser.h"			// for the SetNextMsgHelpContext function
#include "filltool.h"
#include "ndoptmz.h"
#include "colormgr.h"
#include "attrmap.h"
#include "isetattr.h"
#include "strkattr.h"
#include "unicdman.h"
#include "fontman.h"

//#include "will2.h"

#include "userattr.h"
#include "webattr.h"

#include "nodeshad.h"
//#include "gerry.h"

#include "attrbev.h"
#include "nodeshad.h"
#include "brshattr.h"
#include "fthrattr.h"
#include "clipattr.h"
#include "lineattr.h"
//#include "coldlog.h"

#include "cxfrec.h"
#include "cxftags.h"
#include "ophist.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(CurrentAttrChangedMsg, Msg) 

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW  

const INT32 AttributeTableGranularity = 20;

// WEBSTER - markn 14/8/97
// Webster 2 uses the attrs last applied by default

#ifdef WEBSTER
// Static attribute preference variables. They have the same names as the preference
BOOL AttributeManager::LastAttrAppliedBecomesCurrent =1;
#else
// Static attribute preference variables. They have the same names as the preference
BOOL AttributeManager::LastAttrAppliedBecomesCurrent = 0;
#endif // WEBSTER

// WEBSTER - markn 6/12/96
// Don't ask the user if they want to set the current attr
#ifdef WEBSTER
BOOL AttributeManager::AskBeforeSettingCurrentAttr	 =0;
#else
BOOL AttributeManager::AskBeforeSettingCurrentAttr	 =1;
#endif // WEBSTER

BOOL AttributeManager::SendMessages = 1;

BOOL AttributeManager::ShouldAskAboutContoneColours = 1;
BOOL AttributeManager::HaveAskedAboutContoneColours = 0;
BOOL AttributeManager::UserCancelledContoneColours = 0;

// Table variables (table is initially empty)
UINT32 			AttributeManager::AttributeTableSize = 0;
UINT32 		    AttributeManager::NumAttributes 	 = 0;
AttributeEntry *AttributeManager::DefaultAttrValues  = NULL;
NodeTypeEntry  *AttributeManager::DefaultNodeAttrs 	 = NULL;

// Default black/white. Defined as RGB so they will separate.
// These are reinitialised in RegisterBasicAttributes to be CMYK
// (because we can't do CMYK static initialisations easily)
DocColour AttributeManager::DefaultBlack = DocColour(0L, 0L, 0L);
DocColour AttributeManager::DefaultWhite = DocColour(255L, 255L, 255L);

NodeRenderableInk* AttributeManager::pLastNodeAppliedTo = NULL;
DocRect AttributeManager::LastAppliedBounds = DocRect(0,0,0,0);


static BOOL UseLastAttrApplied = FALSE;

AttributeManager::AttributeManager() 
{
}



/********************************************************************************************

	static void AttributeManager::FindDefaultColour(ColourList *ColList,
													UINT32 NameResID,  DocColour *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/94
	Inputs:		ColList - The colour list in which the default should reside
				NameResID - the ID of the name string for the desired default
				colour (see below)

	Outputs:	(An appropriate colour may have been added to ColList if necessary)
				Result - Is filled in with an appropriate colour. In the case
				of total failure, this may be an immediate DocColour, but will always
				be set to a safe/valid default.
	Returns:	-
	Purpose:	Used in initialisation of default attributes, and when ensuring no
				current attributes use deleted colour(s), to find a suitable default
				colour to use. Two default colours are currently provided, accessed
				by passing in NameResID of _R(IDS_WHITENAME), _R(IDS_BLACKNAME), in order to
				get the default white or black colour respectively.

	Errors:		ERROR3 if Result==NULL.
				Otherwise, will always return a valid result - even if it failed
				to allocate memory for a new IndexedColour, it'll return safely.

********************************************************************************************/

// Accuracy of colour matching - same value as Tim uses in importer for 'fuzzy match'
// of the colour.
#define COL_ACCURACY (0.001)

void AttributeManager::FindDefaultColour(ColourList *ColList,
										 UINT32 NameResID,
										 DocColour *Result)
{
//#pragma message( __LOCMSG__ "AttributeManager::FindDefaultColour- do nothing" )
//	TRACE( wxT("Warning - AttributeManager::FindDefaultColour called\n") );
	ERROR3IF(Result == NULL, "AttrMgr: FindDefaultColour - NULL Result param is illegal");
	if (Result == NULL)
		return;

	// Generate an appropriate colour definition for the desired default colour
	ColourRGBT DesiredDef;			// Existing colour we wish to match (RGB black)
	ColourHSVT DefaultDef;			// Definition, if we are forced to create a new colour (HSV black)
	ColourCMYK CMYKBlack;			// CMYK definition for black
	ColourCMYK CMYKWhite;			// CMYK definition for white

	DesiredDef.Red = DesiredDef.Green = DesiredDef.Blue = DesiredDef.Transparent = 0;
	DefaultDef.Hue = DefaultDef.Saturation = DefaultDef.Value = DefaultDef.Transparent = 0;
	CMYKBlack.Cyan = CMYKBlack.Magenta = CMYKBlack.Yellow = 0;
	CMYKBlack.Key = FIXED24(1.0);
	CMYKWhite.Cyan = CMYKWhite.Magenta = CMYKWhite.Yellow = CMYKWhite.Key = 0;


	if (NameResID ==_R(IDS_BLACKNAME))
		// We want the default black to be CMYK so overprint/seps work
		*Result = DocColour(COLOURMODEL_CMYK, (ColourGeneric *) &CMYKBlack);
	else if (NameResID == _R(IDS_WHITENAME))
		// We want the default white to be CMYK so overprint/seps work
	{
		*Result = DocColour(COLOURMODEL_CMYK, (ColourGeneric *) &CMYKWhite);
		// I need to define the desired colour to make the search work OK.
		DesiredDef.Red = DesiredDef.Green = DesiredDef.Blue = FIXED24(1.0);
		DefaultDef.Value = FIXED24(1.0);
	}
	else if (NameResID == _R(IDS_REDNAME))
	{
		DesiredDef.Red = FIXED24(1.0);
		DefaultDef.Saturation = DefaultDef.Value = FIXED24(1.0);
	}
	else if (NameResID == _R(IDS_YELLOWNAME))
	{
		DesiredDef.Red = DesiredDef.Green = FIXED24(1.0);
		DefaultDef.Hue = FIXED24(1.0 / 6.0);						// 60 degree hue.
		DefaultDef.Saturation = DefaultDef.Value = FIXED24(1.0);
	}
	else
	{
		// Fill in result with an appropriate Immediate colour in case we fail
		*Result = DocColour(COLOURMODEL_HSVT, (ColourGeneric *) &DefaultDef);
	}

	if (ColList != NULL)
	{	
		// Try to find a colour style of appropriate appearance in our colour list
		ColourContext *CCRGB = ColourContext::GetGlobalDefault(COLOURMODEL_RGBT);
		if (CCRGB == NULL)
		{
			ERROR3("AttrMgr: FindDefault Colour - Failed to get RGB Colour Context");
			return;
		}

		// Search the colour list to see if an existing colour is close enough to the desired
		// colour to use it.
		IndexedColour *NewDefault = (IndexedColour *) ColList->GetHead();
		ColourRGBT RGBDef;

		while (NewDefault != NULL)
		{
			if (!NewDefault->IsDeleted())		// If this colour is alive...
			{
				// Convert this colour to RGB, and see if it looks like our default
				// If it is, we drop out with NewDefault still pointing at it.
				CCRGB->ConvertColour(NewDefault, (ColourGeneric *) &RGBDef);

				if ((ABS(RGBDef.Red   - DesiredDef.Red))   <= FIXED24(COL_ACCURACY) && 
					(ABS(RGBDef.Green - DesiredDef.Green)) <= FIXED24(COL_ACCURACY) && 
					(ABS(RGBDef.Blue  - DesiredDef.Blue))  <= FIXED24(COL_ACCURACY))
					break;
			}

			NewDefault = (IndexedColour *) ColList->GetNext(NewDefault);
		}

		// If we failed to find a style, create a local colour instead
		if (NewDefault == NULL)
		{
			if (NameResID == _R(IDS_BLACKNAME))
				NewDefault = new INDEXEDCOLOUR_CMYK(&CMYKBlack);
			else if (NameResID == _R(IDS_WHITENAME))
				NewDefault = new INDEXEDCOLOUR_CMYK(&CMYKWhite);
			else
				NewDefault = new INDEXEDCOLOUR_HSVT(&DefaultDef);

			if (NewDefault == NULL)
			{
				ERROR3("AttrMgr: FindDefaultColour failed to generate new default IndexedColour");
				return;
			}

			// Set the colour to have a name.
			String_64 ColourName;
			ColourName.Load ( NameResID );

			NewDefault->SetName ( ColourName );

			// Add the new colour to the end of the colour list
			ColList->AddItem(NewDefault);
		}

		if (NewDefault != NULL)		// Fix the DocColour to reference the new IndexedColour
			Result->MakeRefToIndexedColour(NewDefault);
	}

	// else just return - Result has a safe immediate colour definition
}
#undef COL_ACCURACY


/********************************************************************************************

>	BOOL AttributeManager::InitInstance()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/94
	Returns:	FALSE if initialisation failed, An error will be set in this situation.
				TRUE is successful.
				
				Purpose:Creates the current attribute groups, and adds the initial current 
						attributes to them.

						For a description of attribute groups see CreateCurrentAttributeGroup
	Errors:		-
	SeeAlso:	AttributeManager::CreateCurrentAttributeGroup
	SeeAlso:	AttributeManager::UpdateCurrentAttribute


********************************************************************************************/

BOOL AttributeManager::InitInstance()
{
	// ****************************************************************************************
	// ****************************************************************************************
	// IMPORTANT !!, whenever a new attribute gets added here, make sure that the ObjectRegistry 
	// is updated as well. Also remember that there are now two current attribute groups to 
	// fill in (TEXT and INK)

	// Register all attribute groups here 

	// Register the NodeRenderableInk attribute group first. The attributes in this group 
	// get applied to all objects except text (sort of - see CreateCurrentAttributeGroup) 
				
	String_256			strGraphic( _R(IDS_GRAPHIC) );
	CreateCurrentAttributeGroup( CC_RUNTIME_CLASS(NodeRenderableInk), (CCRuntimeClass *)NULL, 
		strGraphic );

	// Set up initial current attributes 
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	AttrFlatColourFill* Attr1 = new AttrFlatColourFill; 
	if (Attr1 == NULL)
		return FALSE; 

	// Find default black/whith indexed colours. This will create the new
	// colours if necessary. This all occurs in the Current Document.
	// The constant values give the RGB definition of the colour, and
	// the id of its name string (e.g. 255,255,255 "Default White")
	DocColour DefWhite;
//	FindDefaultColour(ColourManager::GetCurrentColourList(),
//						_R(IDS_WHITENAME), &DefWhite);
	FindDefaultColour(NULL,
						_R(IDS_WHITENAME), &DefWhite);
	DocColour DefBlack;
//	FindDefaultColour(ColourManager::GetCurrentColourList(),
//						_R(IDS_BLACKNAME), &DefBlack);
	FindDefaultColour(NULL,
						_R(IDS_BLACKNAME), &DefBlack);

	Attr1->SetStartColour(&DefBlack);
	Attr1->SetEndColour(&DefWhite);

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), Attr1, FALSE, TRUE);
	
	 // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	AttrFlatTranspFill* Attr4 = new AttrFlatTranspFill; 
	if (Attr4 == NULL)
		return FALSE; 

	UINT32 Transp = 0;
	Attr4->SetStartTransp(&Transp);
	Attr4->SetEndTransp(&Transp);
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), Attr4, FALSE, TRUE); 

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrFillMappingLinear* FillMapp = new AttrFillMappingLinear; 
	if (FillMapp == NULL)
		return FALSE; 
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), FillMapp, FALSE, TRUE); 


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrTranspFillMappingLinear* TranspFillMapp = new AttrTranspFillMappingLinear; 
	if (TranspFillMapp == NULL)
		return FALSE; 
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), TranspFillMapp, FALSE, TRUE); 

								  
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrFillEffectFade* Attr5 = new AttrFillEffectFade; 
	if (Attr5 == NULL)
		return FALSE; 
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), Attr5, FALSE, TRUE); 

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrStrokeColour* Attr2 = new AttrStrokeColour; 
	if (Attr2 == NULL)
		return FALSE; 
	Attr2->Value.Colour = DefBlack;

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), Attr2, FALSE, TRUE);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrStrokeTransp* Attr6 = new AttrStrokeTransp; 
	if (Attr6 == NULL)
		return FALSE; 
	Attr6->Value.Transp = 0;
	Attr6->Value.TranspType = TT_Mix;
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), Attr6, FALSE, TRUE);  
    												 

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrLineWidth* Attr3 =  new AttrLineWidth;
	if (Attr3 == NULL)
		return FALSE; 

	Attr3->Value.LineWidth = 500;// 0.5pt
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), Attr3, FALSE, TRUE);  
 

     												
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrStartArrow* StartArrow =  new AttrStartArrow;
	if (StartArrow == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), StartArrow, FALSE, TRUE);  


    												
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrEndArrow* EndArrow =  new AttrEndArrow;
	if (EndArrow == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), EndArrow, FALSE, TRUE);  


    												
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrStartCap* StCaps =  new AttrStartCap;
	if (StCaps == NULL)
		return FALSE; 

	StCaps->Value.StartCap = LineCapButt; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), StCaps, FALSE, TRUE);  
    												
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrJoinType* JoinType =  new AttrJoinType;
	if (JoinType == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), JoinType, FALSE, TRUE);  
    												
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrMitreLimit* MitreLimit =  new AttrMitreLimit;
	if (MitreLimit == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), MitreLimit, FALSE, TRUE);  

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrUser* User =  new AttrUser;
	if (User == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), User, FALSE, TRUE); 

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrWebAddress* WebAddress =  new AttrWebAddress;
	if (WebAddress == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), WebAddress, FALSE, TRUE); 

    												
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~




	AttrWindingRule* WindingRule =  new AttrWindingRule;
	if (WindingRule == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), WindingRule, FALSE, TRUE);  
    												
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrDashPattern* DashPattern =  new AttrDashPattern;
	if (DashPattern == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), DashPattern, FALSE, TRUE);  


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// BLOCK
	{
		AttrOverprintLine *OPAttr = new AttrOverprintLine;
		if (OPAttr == NULL)
			return FALSE;

		UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), OPAttr, FALSE, TRUE);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// BLOCK
	{
		AttrOverprintFill *OPAttr = new AttrOverprintFill;
		if (OPAttr == NULL)
			return FALSE;

		UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), OPAttr, FALSE, TRUE);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// BLOCK
	{
		AttrPrintOnAllPlates *POAPAttr = new AttrPrintOnAllPlates;
		if (POAPAttr == NULL)
			return FALSE;

		UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), POAPAttr, FALSE, TRUE);
	}


#ifdef VECTOR_STROKING // Neville 6/8/97
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrStrokeType* pStroke = new AttrStrokeType;
	if (pStroke == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), pStroke, FALSE, TRUE);  


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrVariableWidth* pVarWidth =  new AttrVariableWidth;
	if (pVarWidth == NULL)
		return FALSE;

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), pVarWidth, FALSE, TRUE);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	AttrBevelIndent* pBevelIndent =  new AttrBevelIndent;
	if (pBevelIndent == NULL)
		return FALSE;

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), pBevelIndent, FALSE, TRUE);

	AttrBevelLightAngle* pBevelLightAngle =  new AttrBevelLightAngle;
	if (pBevelLightAngle == NULL)
		return FALSE;

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), pBevelLightAngle, FALSE, TRUE);

	AttrBevelContrast* pBevelContrast =  new AttrBevelContrast;
	if (pBevelContrast == NULL)
		return FALSE;

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), pBevelContrast, FALSE, TRUE);

	AttrBevelLightTilt* pBevelTilt =  new AttrBevelLightTilt;
	if (pBevelTilt == NULL)
		return FALSE;

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), pBevelTilt, FALSE, TRUE);
	
	AttrBevelType* pBevelType =  new AttrBevelType;
	if (pBevelType == NULL)
		return FALSE;

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), pBevelType, FALSE, TRUE);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	AttrBrushType* pBrushType = new AttrBrushType;
	if (pBrushType == NULL)
		return FALSE;

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), pBrushType, FALSE, TRUE);
	 
#endif // VECTOR_STROKING

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	AttrFeather* pFeather = new AttrFeather;
	if (pFeather == NULL)
		return FALSE;

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), pFeather, FALSE, TRUE);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// Create the attribute group for all text objects
	String_256			strText( _R(IDS_TEXT) );
	CreateCurrentAttributeGroup( CC_RUNTIME_CLASS(BaseTextClass), 
								 CC_RUNTIME_CLASS(NodeRenderableInk), 
								 strText );	// Base class


	// Attributes specific to text objects first
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// ----------------------------------------------------------------------------------
	// Create the Current text attributes

	AttrTxtFontTypeface* FntAttr = new AttrTxtFontTypeface;	
	if (FntAttr == NULL)
		return FALSE; 

	PORTNOTETRACE("text", "Japanese default font disabled");
#ifndef EXCLUDE_FROM_XARALX
	// If we are in Japan we don't want Times to be the current font
	if (UnicodeManager::IsDBCSOS())
	{
		// This is the Japanese standard font - it's specified in DBCS characters
		// so it won't work in UNICODE builds
		String_64 FontName  = wxT("\x82\x6c\x82\x72 \x82\x6f\x83\x53\x83\x56\x83\x62\x83\x4e");

		FONTMANAGER->CacheNamedFont(&FontName, FC_UNDEFINED);
		FntAttr->Value.HTypeface = FONTMANAGER->GetFontHandle(&FontName, FC_UNDEFINED);
	}
#endif

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), FntAttr, FALSE, TRUE);  


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

				 
	AttrTxtBold* BoldAttr = new AttrTxtBold;
	if (BoldAttr == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), BoldAttr, FALSE, TRUE);  


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrTxtItalic* ItalicAttr = new AttrTxtItalic;
	if (ItalicAttr == NULL)
		return FALSE;
		
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), ItalicAttr, FALSE, TRUE);  
 

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrTxtUnderline* UnderlineAttr	= new AttrTxtUnderline;
	if (UnderlineAttr == NULL)
		return FALSE;


	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), UnderlineAttr, FALSE, TRUE);  


   	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	AttrTxtFontSize* FontSizeAttr	= new AttrTxtFontSize;
	if (FontSizeAttr == NULL)
		return FALSE;

	// change default text size to 12pt (Jonathan)
	FontSizeAttr->Value.FontSize = 12000;

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), FontSizeAttr, FALSE, TRUE); 
	
   	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 


	AttrTxtAspectRatio*	AspectRatioAttr	= new AttrTxtAspectRatio;
	if (AspectRatioAttr == NULL)
		return FALSE;

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), AspectRatioAttr, FALSE, TRUE);  


   	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrTxtJustification* JustificationAttr	= new  AttrTxtJustification;
	if (JustificationAttr == NULL)
		return FALSE;

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), JustificationAttr, FALSE, TRUE);  


   	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	AttrTxtTracking* TxtTrackingAttr = new AttrTxtTracking;
	if (TxtTrackingAttr == NULL)
		return FALSE;
		
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), TxtTrackingAttr, FALSE, TRUE);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	AttrTxtScript* TxtScriptAttr = new AttrTxtScript;
	if (TxtScriptAttr == NULL)
		return FALSE;
		
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), TxtScriptAttr, FALSE, TRUE);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	AttrTxtBaseLine* TxtBaseLineAttr = new AttrTxtBaseLine;
	if (TxtBaseLineAttr == NULL)
		return FALSE;
		
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), TxtBaseLineAttr, FALSE, TRUE);  


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	AttrTxtLineSpace* TxtLineSpaceAttr = new AttrTxtLineSpace;
	if (TxtLineSpaceAttr == NULL)
		return FALSE;
		
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), TxtLineSpaceAttr, FALSE, TRUE);  

	// Attributes which are not unique to the BaseText group
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Attr1 = new AttrFlatColourFill; 
	if (Attr1 == NULL)
		return FALSE; 

	// Set default black and white colours
	Attr1->SetStartColour(&DefBlack);
	Attr1->SetEndColour(&DefWhite);

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), Attr1, FALSE, TRUE);
	
	 // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	Attr4 = new AttrFlatTranspFill; 
	if (Attr4 == NULL)
		return FALSE; 

	Transp = 0;
	Attr4->SetStartTransp(&Transp);
	Attr4->SetEndTransp(&Transp);
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), Attr4, FALSE, TRUE); 

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	FillMapp = new AttrFillMappingLinear; 
	if (FillMapp == NULL)
		return FALSE; 
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), FillMapp, FALSE, TRUE); 


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	TranspFillMapp = new AttrTranspFillMappingLinear; 
	if (TranspFillMapp == NULL)
		return FALSE; 
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), TranspFillMapp, FALSE, TRUE); 

								  
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	Attr5 = new AttrFillEffectFade; 
	if (Attr5 == NULL)
		return FALSE; 
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), Attr5, FALSE, TRUE); 

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	Attr2 = new AttrStrokeColour; 
	if (Attr2 == NULL)
		return FALSE; 
	Attr2->Value.Colour = DocColour(COLOUR_NONE);	 // Text has no stroke colour attribute

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), Attr2, FALSE, TRUE);  

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	
	Attr6 = new AttrStrokeTransp; 
	if (Attr6 == NULL)
		return FALSE; 
	Attr6->Value.Transp = 0;
	Attr6->Value.TranspType = TT_Mix;
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), Attr6, FALSE, TRUE);  
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	Attr3 =  new AttrLineWidth;
	if (Attr3 == NULL)
		return FALSE; 

	Attr3->Value.LineWidth = 250;	// Text has 250 line width
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), Attr3, FALSE, TRUE);  
 

     												
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	StartArrow =  new AttrStartArrow;
	if (StartArrow == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), StartArrow, FALSE, TRUE);  


    												
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	EndArrow =  new AttrEndArrow;
	if (EndArrow == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), EndArrow, FALSE, TRUE);  


    												
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	StCaps =  new AttrStartCap;
	if (StCaps == NULL)
		return FALSE; 

	StCaps->Value.StartCap = LineCapButt; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), StCaps, FALSE, TRUE);  
    												
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	JoinType =  new AttrJoinType;
	if (JoinType == NULL)
		return FALSE; 
	
	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), JoinType, FALSE, TRUE);  
    												
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	MitreLimit =  new AttrMitreLimit;
	if (MitreLimit == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), MitreLimit, FALSE, TRUE);  
    												
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	WindingRule =  new AttrWindingRule;
	if (WindingRule == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), WindingRule, FALSE, TRUE);  
    												
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	DashPattern =  new AttrDashPattern;
	if (DashPattern == NULL)
		return FALSE; 

	UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), DashPattern, FALSE, TRUE);
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// BLOCK
	{
		AttrOverprintLine *OPAttr = new AttrOverprintLine;
		if (OPAttr == NULL)
			return FALSE;

		UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), OPAttr, FALSE, TRUE);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// BLOCK
	{
		AttrOverprintFill *OPAttr = new AttrOverprintFill;
		if (OPAttr == NULL)
			return FALSE;

		UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), OPAttr, FALSE, TRUE);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// BLOCK
	{
		AttrPrintOnAllPlates *POAPAttr = new AttrPrintOnAllPlates;
		if (POAPAttr == NULL)
			return FALSE;

		UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), POAPAttr, FALSE, TRUE);
	}

#ifdef VECTOR_STROKING // Neville 6/8/97
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// BLOCK
	{
		AttrStrokeType* pStroke = new AttrStrokeType;
		if (pStroke == NULL)
			return FALSE; 

		UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), pStroke, FALSE, TRUE);  
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	// BLOCK
	{
		AttrVariableWidth* pVarWidth =  new AttrVariableWidth;
		if (pVarWidth == NULL)
			return FALSE;

		UpdateCurrentAttribute(CC_RUNTIME_CLASS(BaseTextClass), pVarWidth, FALSE, TRUE);
	} 

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	{
		
	}
	
#endif // VECTOR_STROKING
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	return TRUE; // Success
}

/********************************************************************************************

>	AttributeManager::~AttributeManager() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Attribute manager destructor
					Currently destroys all attributes in all attribute groups

	Errors:		-
	SeeAlso:	-

********************************************************************************************/


AttributeManager::~AttributeManager() 
{
	// Delete all groups 
	AttributeGroup* CurrentGroup = (AttributeGroup*)AttrGroupList.GetHead(); 
	NodeAttribute* CurrentAttr;
	NodeAttribute* NextAttr;  
	while (CurrentGroup != NULL)
	{
		// Delete all attributes in the group 
		CurrentAttr = CurrentGroup->AttrListHd;// get pointer to first attribute in the 
											   		// group 
		while (CurrentAttr != NULL)
		{
			NextAttr = (NodeAttribute*)CurrentAttr->FindNext(); 
			delete CurrentAttr; 
			CurrentAttr = NextAttr; 	
		}

		delete (AttrGroupList.RemoveItem((ListItem*)CurrentGroup));
		CurrentGroup =(AttributeGroup*)AttrGroupList.GetHead(); 		// Get next group  
	}
}

/********************************************************************************************

>	static BOOL AttributeManager::InitDefaults()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/05/94
	Returns:	TRUE if successful, FALSE otherwise

	Purpose:	Initialises the Attribute Manager. 
					Registers basic attributes 
					Declares attribute preferences 

********************************************************************************************/
/*********************************************************************************************

	Preference:	LastAttrAppliedBecomesCurrent
	Section:	Attributes
	Range:		TRUE or FALSE
	Purpose:	When TRUE - Whenever an attribute is applied to objects the attribute becomes
				a current attribute. 
				
	SeeAlso:	AskBeforeSettingCurrentAttr

**********************************************************************************************/ 

/*********************************************************************************************

	Preference:	AskBeforeSettingCurrentAttr
	Section:	Attributes
	Range:		TRUE or FALSE
	Purpose:	When TRUE - The user is prompted before setting a current attribute,
				except when the LastAttrAppliedBecomesCurrent preference is TRUE 
				when it makes no sense to prompt the user.

	SeeAlso:	LastAttrAppliedBecomesCurrent

**********************************************************************************************/ 

BOOL AttributeManager::InitDefaults()
{
	BOOL ok =  RegisterBasicAttributes(); 
#if !defined(EXCLUDE_FROM_RALPH)
	ok = ok && Camelot.DeclareSection(TEXT("Attributes"), 10) &&
		   	   Camelot.DeclarePref(TEXT("Attributes"), TEXT("LastAttrAppliedBecomesCurrent"),
		   					   		&AttributeManager::LastAttrAppliedBecomesCurrent, 0, 1) &&
		   	   Camelot.DeclarePref(TEXT("Attributes"), TEXT("AskBeforeSettingCurrentAttr"),
		   					   		&AttributeManager::AskBeforeSettingCurrentAttr,0, 1) &&
		   	   Camelot.DeclarePref(TEXT("Attributes"), TEXT("AskBeforeSettingContoneColours"),
		   					   		&AttributeManager::ShouldAskAboutContoneColours,0, 1);
#endif
	return (ok);
}

void AttributeManager::Deinit()
{
	// Delete all the default attributes
	for (UINT32 i = 0; i < NumAttributes; i++)
		delete DefaultAttrValues[i].pAttr;

	CCFree(DefaultAttrValues);
	CCFree(DefaultNodeAttrs);

#if !defined(EXCLUDE_FROM_RALPH)
	// Tidy up the Fill blob state record
	OpRepeatApplyAttribToSelected::FillBlobState.DeleteAll();
#endif
}

/********************************************************************************************

>	BOOL AttributeManager::RegisterBasicAttributes()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/05/94
	Returns:	TRUE if all basic attributes registered ok;
				FALSE if not.
	Purpose:	Register all the 'built-in' rendering attributes that Camelot has. This
				allows us to register these basic attributes first and in a known order, so
				we can use a constant instead of a variable to access them, thus allowing
				us to get to these attributes quicker.
	Errors:		Out of memory
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL AttributeManager::RegisterBasicAttributes()
{
	// First, make sure the default balck & white colours are sensible
	// (The defaults are RGB, which don't separate too well)
	ColourCMYK Def;
	Def.Cyan = Def.Magenta = Def.Yellow = 0;

	Def.Key = 1.0;
	DefaultBlack = DocColour(COLOURMODEL_CMYK, (ColourGeneric *) &Def);

	Def.Key = 0;
	DefaultWhite = DocColour(COLOURMODEL_CMYK, (ColourGeneric *) &Def);

	// Register all the basic Camelot attributes - these must be done 
	// first and in the correct order (i.e. in the same order as the enumerated 
	// type - see attrmgr.h).
	if (StrokeColourAttribute::Init() &&
		StrokeTranspAttribute::Init() &&
		ColourFillAttribute::Init() &&
		TranspFillAttribute::Init() &&
		FillMappingAttribute::Init() &&
		TranspFillMappingAttribute::Init() &&
		FillEffectAttribute::Init() &&
		LineWidthAttribute::Init() &&

		WindingRuleAttribute::Init() &&
//		StartCapAttribute::Init() &&		   // RIP
		JoinTypeAttribute::Init() &&
		QualityAttribute::Init() &&
		DashPatternAttribute::Init() &&
		StartCapAttribute::Init() &&
		StartArrowAttribute::Init() &&
		EndArrowAttribute::Init() &&
		MitreLimitAttribute::Init() &&
		UserAttribute::Init() &&
		WebAddressAttribute::Init() &&
//		MouldAttribute::Init() &&			  // RIP

		TxtFontTypefaceAttribute::Init() &&			//  < -- Text related attributes start here
		TxtBoldAttribute::Init() &&
		TxtItalicAttribute::Init() &&
		TxtAspectRatioAttribute::Init() &&
		TxtJustificationAttribute::Init() &&
		TxtTrackingAttribute::Init() &&
		TxtUnderlineAttribute::Init() &&
		TxtFontSizeAttribute::Init() &&
		TxtScriptAttribute::Init() &&
		TxtBaseLineAttribute::Init() &&
		TxtLineSpaceAttribute::Init() &&			// < -- And end here

		OverprintLineAttrValue::Init() &&			// Imagesetting attributes
		OverprintFillAttrValue::Init() &&
		PrintOnAllPlatesAttrValue::Init()

#ifdef VECTOR_STROKING // Neville 6/8/97
		&&
		StrokeTypeAttrValue::Init() &&				// New-style stroke attributes
		VariableWidthAttrValue::Init() &&
		
		BevelAttributeValueIndent::Init() &&			// Bevelling attributes
		BevelAttributeValueType::Init() &&
		BevelAttributeValueContrast::Init() &&
		BevelAttributeValueLightAngle::Init() &&
		BevelAttributeValueLightTilt::Init() &&

		BrushAttrValue::Init() &&
#endif // VECTOR_STROKING

		FeatherAttrValue::Init() &&
		ClipRegionAttribute::Init()
		)
	{
		// Other attributes initialised ok - set drawing mode attribute directly.
		// (Doesn't have a fixed ID as it's not used much, and there's no NodeAttribute
		// for it).
		DrawingModeAttribute *pDMAttr = new DrawingModeAttribute(DM_COPYPEN);
		if (pDMAttr==NULL)
			return FALSE;

		DrawingModeAttribute::ID = RegisterDefaultAttribute(NULL, pDMAttr);
		if (DrawingModeAttribute::ID==ATTR_BAD_ID)
			return FALSE;

		// All attributes initialised ok
		return TRUE;
	}

	// Failed to intialise
	return FALSE;
}

/********************************************************************************************

>	BOOL AttributeManager::CreateCurrentAttributeGroup(CCRuntimeClass* AttrGroup, 
													   CCRuntimeClass* BaseGroup,
													   Sting256& GrpName)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/94
	Inputs:		AttrGroup: The new group's identifier
				BaseGroup: The new group's base class group (explained below)
				GrpName:The name of the attribute group eg. text or graphic
	Returns:	FALSE if we run out of memory

	Purpose:	This fn creates a new AttributeGroup. An AttributeGroup will contain a set of
				current attributes which get applied to new objects which are associated
				with the group.

				Each object specifies the attribute group with which it is associated using the
				NodeRenderableInk::GetCurrentAttribGroup virtual function. The default
				attribute group is NodeRenderableInk (All objects except text)

				BaseGroups
				~~~~~~~~~~

				An attribute group can specify an optional BaseGroup. For example the 
				BaseTextClass group uses the NodeRenderableInk group as its base. This
				specifies a type of  inheritance relationship.
									  				 
					When an attribute is added to an attribute group using the 
				UpdateCurrentAttribute function, this first of all searches the list of 
				attributes in this group, and if it finds an attribute of the same type 
				then the attribute replaces the existing attribute.
					However if an attribute of the same type cannot be found in the 
				group, and the group has an associated base group, then we try to add
				the attribute to the base group etc.

				Current attributes are applied to an object by calling the 
				ApplyCurrentAttribsToNode function. This function determines the attribute
				group associated with the object and then applies all attributes in the 
				group to the object. If the group has a base group then all attributes
				in the base group with types different to those already applied get added
				to the object etc.


	Errors:		-
	SeeAlso:	AttributeManager::UpdateCurrentAttribute
	SeeAlso:	NodeRenderableInk::GetCurrentAttribGroup
	SeeAlso:	ApplyCurrentAttribsToNode


********************************************************************************************/

BOOL AttributeManager::CreateCurrentAttributeGroup(CCRuntimeClass* AttrGroup, 
												   CCRuntimeClass* BaseGroup, 
												   String_256& GrpName)
{
	// Try to create a new CurrentAttributeGroup 
	AttributeGroup* NewGrp = new AttributeGroup();
	ERRORIF(!NewGrp, _R(IDE_NOMORE_MEMORY), FALSE);
	NewGrp->AttrGroup = AttrGroup; 
	NewGrp->AttrListHd = NULL; // New group has no attributes 
	NewGrp->BaseGroup = BaseGroup;
	NewGrp->GroupName = GrpName; 
	// Add the new group to the attribute group list 
	AttrGroupList.AddHead((ListItem*)NewGrp);
	return TRUE; 
}

/********************************************************************************************

>	NodeAttribute* AttributeManager::UpdateCurrentAttribute(CCRuntimeClass* AttrGroup, 
										   	  	  NodeAttribute* Attribute,
										   	  	  BOOL fCheckTransparency = TRUE, 
										   	  	  BOOL DefiningGroups = FALSE, 
										   	  	  BOOL TellWorld = TRUE,
												  BOOL bForceNewValue = FALSE) 


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/94
	
	Inputs:		AttrGroup: The attribute group to add the attribute to
				Attribute: The attribute to add
	
				fCheckTransparency			if TRUE (the default) then the attribute will be
											tested on insertion into the tree as to whether
											it needs transparency mode.  This is prevented
											upon Camelot initialisation, when default attributes
											are inserted in the tree.

				DefiningGroups 				This should be set to TRUE when calling the function
											from the AttributeManager's Init method. It adds 
											the attribute to the attribute group specified. 
											The attribute does not have to already live in the group
											and  base groups are ignored. It indicates that 
											we are specifying which attributes go in which 
											groups.

				TellWorld:					Broadcast a CurrentAttrChangedMsg. Always FALSE when
											DefiningGroups is TRUE.
													
	Purpose:	This function Adds Attribute to the specified AttrGroup

				The function searches the set of attributes in AttrGroup, and if it finds 
				an attribute of the same type then the attribute replaces the existing attribute.
					However if an attribute of the same type cannot be found in the 
				group, and the group has an associated base group, then we try to add
				the attribute to the base group etc.
					If the attribute cannot be added to any attribute group then the function 
				errors.

	Errors:		-
	SeeAlso:	NodeRenderableInk::GetCurrentAttribGroup
	SeeAlso:	AttributeManager::CreateCurrentAttributeGroup
	SeeAlso:	AttributeManager::ApplyCurrentAttribsToNode


********************************************************************************************/
NodeAttribute* AttributeManager::UpdateCurrentAttribute(CCRuntimeClass* AttrGroup, 
										   	  NodeAttribute* Attribute, 
										   	  BOOL fCheckTransparency, /* = TRUE */
										   	  BOOL DefiningGroups, /* = FALSE */
										   	  BOOL TellWorld, /*= TRUE*/
											  BOOL bForceNewValue /*= FALSE*/)
{
	ERROR3IF (AttrGroup == NULL, "Invalid attribute group specified"); 

	// Before we update the current attribute inform the SelRange 

	SelRange* Sel = GetApplication()->FindSelection();
	if (Sel) 
		Sel->AttrsHaveChanged(); 


	// This is a bit of a botch
	// if we are applying a text attribute then it can only be added to the AttrTxtBase
	// attribute group. If we add more attribute groups in future then a more elegant
	// way of specifying this will need to be provided. This was added because if 
	// a text attribute is being applied, nothing is selected, and we are not in 
	// the text tool then the existing system does not know which attribute group
	// to add the attribute to.

	if (Attribute->IsKindOfTextAttribute())
	{
		AttrGroup = CC_RUNTIME_CLASS(BaseTextClass); 	
	}
	
	AttributeGroup* CurrentGrp;
	CCRuntimeClass* IDAttrGroupToAddTo = AttrGroup; // We want to add to this group
	

	// Loop until we have found a home for the attribute, we will loop back here if 
	// we try to add to a base group.
	do
	{
		// Try to find the attribute group we want to add the attribute to
		CurrentGrp = (AttributeGroup*)AttrGroupList.GetHead(); 
		while (CurrentGrp != NULL)
		{	
			if (CurrentGrp->AttrGroup == IDAttrGroupToAddTo)	
			{
				if (DefiningGroups)
				{
					AddAttributeToGroup(CurrentGrp, Attribute, fCheckTransparency, FALSE, bForceNewValue);
					return Attribute;
				}
				// Found the group, see if the attribute belongs in the group 
				NodeAttribute* CurrentAttrib = CurrentGrp->AttrListHd;
				while (CurrentAttrib != NULL) 
				{
					if (CurrentAttrib->GetAttributeType() == Attribute->GetAttributeType())
					{
						// Attributes share the same type
						// Do they have the same value ?
						if (!bForceNewValue && (CurrentAttrib->GetRuntimeClass()) == (Attribute->GetRuntimeClass()))
						{
							if ((*CurrentAttrib) == (*Attribute))
							{	
								delete Attribute;
								return NULL; // Job done, no need to change the attr it's the same
							}
						}

						if (CurrentGrp->AttrListHd == CurrentAttrib) 
						{	// We will be deleting the item at the head of the list
							CurrentGrp->AttrListHd = (NodeAttribute*)CurrentAttrib->FindNext();  
						}
					
						// It is safe to delete the old current attribute now
						CurrentAttrib->UnlinkNodeFromTree(); // Remove the attribute from the list
						delete CurrentAttrib; 
						
						AddAttributeToGroup(CurrentGrp, Attribute, fCheckTransparency, TellWorld, bForceNewValue);
						return Attribute; // attribute has been set 
					}
					// Try the next attribute in the group
					CurrentAttrib = (NodeAttribute*)CurrentAttrib->FindNext();
				}
				//break; // Attribute not found in the group, try the base class
			}
			//Find the next attribute group
			CurrentGrp = (AttributeGroup *) AttrGroupList.GetNext(CurrentGrp);
		}
	} while  (CurrentGrp != NULL); //(IDAttrGroupToAddTo != NULL);
	
	return NULL;
}


/********************************************************************************************

>	AttributeGroup* AttributeManager::GetAttributeGroup(CCRuntimeClass* GroupID)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/95
	Inputs:		GroupID: The groups ID
	Returns:	A pointer to the requested group, NULL if it was not found
	Purpose:	This function obtains a pointer to the requested group

********************************************************************************************/
				
AttributeGroup* AttributeManager::GetAttributeGroup(CCRuntimeClass* GroupID)
{
	AttributeGroup* CurrentGrp;
	CurrentGrp = (AttributeGroup*)AttrGroupList.GetHead();
	do
	{
		if (CurrentGrp->AttrGroup == GroupID)
		{
			return CurrentGrp;
		}
		CurrentGrp = (AttributeGroup *) AttrGroupList.GetNext(CurrentGrp);

	} while (CurrentGrp);

	return NULL; // Not found 
} 


/********************************************************************************************

>	void AttributeManager::AddAttributeToGroup(AttributeGroup* pAttrGroup, 
											   NodeAttribute* Attribute, 
											   BOOL fCheckTransparency, 
											   BOOL TellWorld  = TRUE,
											   BOOL bRetainValues = FALSE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/95
	Inputs:		pAttrGroup: The attribute group to add the attribute to
				Attribute:	The attribute to add

				fCheckTransparency			if TRUE (the default) then the attribute will be
											tested on insertion into the tree as to whether
											it needs transparency mode.  This is prevented
											upon Camelot initialisation, when default attributes
											are inserted in the tree.

				TellWorld: Broadcast a CurrentAttrChangedMsg

	Purpose:	This function is a helper for AttributeManager::UpdateCurrentAttribute.
				it adds the specified attribute to the specified Attribute group. 
				and BROADCASTS a CurrentAttrChangedMsg message.
	Errors:		-
	Scope: 		private
	SeeAlso:	AttributeManager::UpdateCurrentAttribute

********************************************************************************************/

void AttributeManager::AddAttributeToGroup(AttributeGroup* pAttrGroup, 
										   NodeAttribute* Attribute, 
										   BOOL fCheckTransparency, 
										   BOOL TellWorld, /* = TRUE */
										   BOOL bRetainValues /* = FALSE */)
{
	// Add Attribute to the group 
	if (pAttrGroup->AttrListHd != NULL)
		Attribute->AttachNode(pAttrGroup->AttrListHd, NEXT, fCheckTransparency); 
	else 
		pAttrGroup->AttrListHd = Attribute;
	
	// If this is a Fill Attribute then we need to make sure its
	// in a sensible state.
	if (Attribute->IsAFillAttr())
	{
		AttrFillGeometry* Fill = (AttrFillGeometry*)Attribute;
		// Ensure that all it's control points are deselected, else
		// when it is applied to an object, the user may get confused.
		Fill->DeselectAll();

		if (!bRetainValues && Fill->GetBoundingRect().IsEmpty())
		{
			// If the bounding box is Empty then we know that this
			// fill has never been applied to an object before.
			// We ensure that it's control points are Null, so
			// they will be set to some sensible default (centre of
			// the object usually) when eventually applied.
			DocCoord	coord;
			Fill->SetStartPoint( &coord );
			Fill->SetEndPoint( &coord );
			Fill->SetEndPoint2( &coord );
		}

	}

	// Tell everyone that the Current Attributes have Changed.
	if (TellWorld)
	{
		BROADCAST_TO_ALL(CurrentAttrChangedMsg()); 
	}
}	


/********************************************************************************************

>	static BOOL UpdateDeletedColour(ColourList *ColList, DocColour *TheColour,
									IndexedColour **NewDefault)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		ColList - The list in which IndexedColours referenced by the DocColours
				should reside.

				TheColour - The DocColour to be fixed

				NewDefault - NULL, or a pointer to the default colour returned from a
				previous call to this function. This makes fixing several colours in
				a row a lot more efficient.

	Outputs:	(ColList may have a new colour appended)
				NewDefault will point at the new default black colour, or will be NULL
				if the return value was FALSE.

	Returns:	TRUE if it had to fix this colour (In this case, NewDefault is returned
				pointing at the default used). FALSE if the colour did not need fixing
				(in which case NewDefault is untouched).

	Purpose:	static function called by UpdateForDeletedColours.
				Ensures that the given DocColour is a safe colour to use (does not
				reference a deleted IndexedColour). If it needs to be fixed, a default
				colour will be found (or created if necessary) in the given colour list
				and the colour will be made to reference the new default.

	Notes:		The return value should be used to determine if a CurrentAttrChanged Msg
				needs to be broadcast. 

				The NewDefault value should be set to NULL before the first call to this
				function. On subsequent calls, pass in the same value; if a change is
				made, this allows the system to be more efficient, and guarantees all
				changed colours reference the correct default colour.

	Scope:		private static function

	Errors:		-
	SeeAlso:	AttributeManager::UpdateForDeletedColours

********************************************************************************************/

static BOOL UpdateDeletedColour(ColourList *ColList, DocColour *TheColour,
								IndexedColour **NewDefault)
{
//#pragma message( __LOCMSG__ "UpdateDeletedColour - do nothing" )
//	TRACE( wxT("Warning - UpdateDeletedColour called\n") );
	if (TheColour == NULL)			// No colour to fix!
		return FALSE;

	IndexedColour *RefColour = TheColour->FindParentIndexedColour();
	if( RefColour == NULL ||		// Not referencing an IndexedColour, so nothing to fix
		!RefColour->IsDeleted() )	// Not deleted, so doesn't need fixing
		return FALSE;

	// Inform the SelRange that attributes have changed (or will do very shortly)
	// This will flush the cache of common attributes which could contain an indexed
	// colour which has or will be deleted.
	SelRange* Sel = GetApplication()->FindSelection();
	if (Sel) 
		Sel->AttrsHaveChanged(); 
	
	// We've got a dead colour on our hands! Medic! Medic!
	// If the passed-in NewDefault already points at an appropriate attribute,
	// then use it, else call FindDefaultColour to find/create one for us
	if (*NewDefault != NULL)
		TheColour->MakeRefToIndexedColour(*NewDefault);
	else
	{
		AttributeManager::FindDefaultColour(ColList, _R(IDS_BLACKNAME), TheColour);
		*NewDefault = TheColour->FindParentIndexedColour();
	}

	return TRUE;
}


/********************************************************************************************

>	void AttributeManager::UpdateForDeletedColours(ColourList *ParentList)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		ParentList - The colour list in which the deleted colours reside
				(must be within the current document). This list is used to find the
				new current colour if any of the current attribute colours have now
				been deleted. A new colour may be appended to this list if necessary.
	Outputs:	(ParentList may have a new colour appended)
	Purpose:	Called by the ColourManager when colours in use by the attribute manager
				may have been deleted. 

********************************************************************************************/

void AttributeManager::UpdateForDeletedColours(ColourList *ParentList)
{
	AttributeGroup* CurrentGrp = (AttributeGroup*)AttrGroupList.GetHead(); 
	NodeAttribute* CurrentAttrib;
	DocColour *pColour;
	UINT32 Context = 0;
	BOOL Changed = FALSE;
	IndexedColour *NewDefault = NULL;

	while (CurrentGrp != NULL)
	{	
		CurrentAttrib = CurrentGrp->AttrListHd;
		while (CurrentAttrib != NULL) 
		{
			Context = 0;
			do
			{
				// Get the next colour field from the attribute
				pColour = CurrentAttrib->EnumerateColourFields(Context++);

				// If there is another colour, see if we need to fix it.
				if (pColour != NULL)
					Changed |= UpdateDeletedColour(ParentList, pColour, &NewDefault);
			} 
			while (pColour != NULL);		// Do for all colour fields in this attribute
			
			CurrentAttrib = (NodeAttribute*)CurrentAttrib->FindNext(); 
		}		

		CurrentGrp = (AttributeGroup *) AttrGroupList.GetNext(CurrentGrp);
	}

	if (Changed)
	{
		// Tell everyone that the Current Attributes have Changed.
		BROADCAST_TO_ALL(CurrentAttrChangedMsg()); 
	}
}


/********************************************************************************************

>	BOOL AttributeManager::ApplyCurrentAttribsToNode(NodeRenderableInk* Node) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/94
	Returns:	FALSE if we run out of memory
	Purpose:	This function determines the attribute group associated with the object 
				and then applies all attributes in the group to the object. If the group has 
				a base group then all attributes in the base group with types different to 
				those already applied get added to the object etc.

	SeeAlso:	NodeRenderableInk::GetCurrentAttribGroup
	SeeAlso:	AttributeManager::CreateCurrentAttributeGroup
	SeeAlso:	AttributeManager::UpdateCurrentAttribute


********************************************************************************************/

BOOL AttributeManager::ApplyCurrentAttribsToNode(NodeRenderableInk* Node) 
{
	ERROR3IF(Node == NULL, "NULL object passed to AttributeManager::ApplyCurrentAttribsToNode"); 

	// Find the attribute group that contains the attributes we need to initially apply to the
	// node.
	CCRuntimeClass* IDCurrentAttrGroup = Node->GetCurrentAttribGroup(); 
	ERROR3IF (IDCurrentAttrGroup == NULL, "Object has NULL attribute group associated with it");  
	
	// We need to find out quickly if an attribute has already been applied to Node
	// Maps Attribute types to TRUE/FALSE values
	CCAttrMap* pAppliedAttrsMap; 			
	
	try
	{		 
		pAppliedAttrsMap = new 	CCAttrMap(30);
	} 
	catch( ... )
	{
		return FALSE; 
	}

	// Loop back here to apply base group attributes
	do
	{
		// Search for IDCurrentAttrGroup
		AttributeGroup* CurrentGrp = (AttributeGroup*)AttrGroupList.GetHead(); 
		while (CurrentGrp != NULL)
		{	
			if (CurrentGrp->AttrGroup == IDCurrentAttrGroup) // Found the group	
			{
				// Create a copy of each attribute and add it as a child of Node 
				NodeAttribute* CurrentAttrib = CurrentGrp->AttrListHd;
				while (CurrentAttrib != NULL) 
				{
					// Has an attribute of the same type already been applied to the node, 
					// this should only ever be the case if we are applying attributes from a base group !.
					CCRuntimeClass* CurrentAttrType = CurrentAttrib->GetAttributeType(); 

					void* NotUsed;
					if( !pAppliedAttrsMap->Lookup( CurrentAttrType, NotUsed ) &&
						Node->RequiresAttrib( CurrentAttrib ) )
					{
						// Ensure the nodes bounding box is up to date, so that	fill
						// attributes can be scaled correctly.
						Node->InvalidateBoundingRect();
						DocRect NodeBounds = Node->GetBoundingRect(TRUE);

						// Take a copy of the attribute 
						NodeAttribute* AttribClone = (NodeAttribute*)CurrentAttrib->SimpleCopy(); 
						ERRORIF(!AttribClone, _R(IDE_NOMORE_MEMORY), FALSE);

						// Scale and move the attributes bounds so that it is ready to apply to the
						// new object.
						AttribClone->TransformToNewBounds(NodeBounds); 
						
						// Attach AttribClone to Node
						AttribClone->AttachNode(Node, FIRSTCHILD); 

						if (AttribClone->GetAttributeType() == CC_RUNTIME_CLASS(AttrFillGeometry) ||
							AttribClone->GetAttributeType() == CC_RUNTIME_CLASS(AttrTranspFillGeometry))
						{
							// Now the Attribute is in the tree, we need to tell the fill
							// attribute to check that it's control points are valid.
							// Unless the fill we transformed above, this will usually 
							// involve the fill 'centring' itself within the bounds of its
							// parent.
						 	((AttrFillGeometry*)AttribClone)->AttributeChanged();
						} // end if 

						// Add the attributes type to the applied attributes map so that it does not get 
						// applied again.
						pAppliedAttrsMap->SetAt(CurrentAttrType, NULL); 
					}
					CurrentAttrib = (NodeAttribute*)CurrentAttrib->FindNext(); 
				}
				// ok now apply attributes from the base group, if one exists
 				IDCurrentAttrGroup = CurrentGrp->BaseGroup;
				break; 
			}
			CurrentGrp = (AttributeGroup *) AttrGroupList.GetNext(CurrentGrp);
			ERROR3IF(CurrentGrp == NULL, "Unable to find attribute group"); 
		}
		// break takes us here
	} while (IDCurrentAttrGroup != NULL);

	delete pAppliedAttrsMap; // Map no longer required
	return TRUE;  
}


/********************************************************************************************

>	NodeAttribute* AttributeManager::GetCurrentAttribute(CCRuntimeClass* pAttrGroup,
														 CCRuntimeClass* pAttrType) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/94

	Inputs:		AttrGroup: The Attribute group 
				AttrType:  The type of attribute to obtain 
					
	Returns:	A pointer to the NodeAttribute with class pAttrType in group AttrGroup.
				If AttrGroup does not have a pAttrType attribute then NULL is returned 
	
	Purpose:	For finding current attribute pAttrType in attribute group pAttrGroup
				
	Errors:		An ENSURE failure will occur if the attribute group could not be found
	SeeAlso:	AttributeManager::GetSelToolCurrentAttribute

********************************************************************************************/

NodeAttribute* AttributeManager::GetCurrentAttribute(CCRuntimeClass* AttrGroup,
										   		 	 CCRuntimeClass* AttrType) 
{
	CCRuntimeClass* GroupToFind = AttrGroup; 
	do
	{
		// First find AttrGroup
		AttributeGroup* CurrentGrp = (AttributeGroup*)AttrGroupList.GetHead(); 
		while (CurrentGrp != NULL)
		{	
			if (CurrentGrp->AttrGroup == GroupToFind)	
			{
				// Found the group 
				NodeAttribute* CurrentAttrib = CurrentGrp->AttrListHd;
				while (CurrentAttrib != NULL) 
				{
					if (CurrentAttrib->GetAttributeType() == AttrType)
					{
						return CurrentAttrib; 
					}
					CurrentAttrib = (NodeAttribute*)CurrentAttrib->FindNext(); 
				}
				// Attribute not found in the group, try a base group
				GroupToFind = CurrentGrp->BaseGroup;
				break;
			}
			CurrentGrp = (AttributeGroup *) AttrGroupList.GetNext(CurrentGrp);
			ERROR3IF(CurrentGrp == NULL, "Could not find attribute group"); 
		}
		// break to here
	} while (GroupToFind != NULL);

	ENSURE(FALSE,"Attribute manager unable to find current attribute");  
	return NULL; 	
}

/********************************************************************************************

>	NodeAttribute* AttributeManager:::GetSelToolCurrentAttribute(CCRuntimeClass* pAttrType) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/94
							  
	Inputs:		AttrType:  The type of attribute to obtain 
					
	Returns:	A pointer to the NodeAttribute with class pAttrType, NULL if the
				the attribute does not exist 
	
	Purpose:	For finding current attribute pAttrType in the current attribute group
				specified by the currently selected tool.
				
	Errors:		An ENSURE failure will occur if the attribute group could not be found
	SeeAlso:	AttributeManager:::GetCurrentAttribute

********************************************************************************************/

NodeAttribute* AttributeManager::GetSelToolCurrentAttribute(CCRuntimeClass* pAttrType) 
{
	 CCRuntimeClass* CurrentAttribGroup;	

	 // If the attribute is derived from AttrTxtBase then we must use the BaseTextClass 
	 // attribute group. This is not that nice (a better system will be written
	 // if current attributes become more complex in future !.

	 if (pAttrType->IsKindOf(CC_RUNTIME_CLASS(AttrTxtBase)))
	 {
	 	CurrentAttribGroup = CC_RUNTIME_CLASS(BaseTextClass); 
	 }
	 else
	 {
		 // Obtain the current attribute group associated with the current tool 
		 Tool* CurrentTool = Tool::GetCurrent(); 
		 if (CurrentTool == NULL)
		 {
		 	return FALSE; 
		 }

		 CurrentAttribGroup = 
		 	CurrentTool->Parent->m_ToolInfo.CurrentAttributeGroup;

		 ENSURE (CurrentAttribGroup != NULL, "Tool has no associated current attribute group"); 
	 }

	 return GetCurrentAttribute( CurrentAttribGroup, pAttrType); 
}

/********************************************************************************************

>	void AttributeManager::GetCurrentLineAndFillColour(CCRuntimeClass* AttrGroup, 
												   	   DocColour* LineCol, DocColour* FillCol)


	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void AttributeManager::GetCurrentLineAndFillColour(CCRuntimeClass* AttrGroup, 
												   DocColour* LineCol, DocColour* FillCol)
{
	// First find AttrGroup
	AttributeGroup* CurrentGrp = (AttributeGroup*)AttrGroupList.GetHead(); 
	while (CurrentGrp != NULL)
	{	
		if (CurrentGrp->AttrGroup == AttrGroup)	
		{
			// Found the group 
			NodeAttribute* CurrentAttrib = CurrentGrp->AttrListHd;
			while (CurrentAttrib != NULL) 
			{
				if (CurrentAttrib->GetAttributeType() == CC_RUNTIME_CLASS(AttrStrokeColour))
					*LineCol = ((AttrStrokeColour*)CurrentAttrib)->Value.Colour; 
				if ((CurrentAttrib->GetAttributeType() == CC_RUNTIME_CLASS(AttrFillGeometry)) &&
					(((AttrFillGeometry*)CurrentAttrib)->GetStartColour()))
					*FillCol = *((AttrFillGeometry*)CurrentAttrib)->GetStartColour(); 
				CurrentAttrib = (NodeAttribute*)CurrentAttrib->FindNext(); 
			}
			return; 
		}
		CurrentGrp = (AttributeGroup *) AttrGroupList.GetNext(CurrentGrp);
	}
	ENSURE(FALSE,"Attribute manager unable to find attribute group");  
} 

#define TSIZE(C) (AttributeTableSize * sizeof(C))

BOOL AttributeManager::EnsureTableSpaceAvailable()
{
	if (AttributeTableSize == 0)
	{
		// Set intitial table size.
		AttributeTableSize = AttributeTableGranularity;

		// Allocate table for AttributeValue objects.
		DefaultAttrValues = (AttributeEntry *) CCMalloc(TSIZE(AttributeEntry));
		if (DefaultAttrValues==NULL)
			return FALSE;

		// Allocate table for NodeAttribute objects.
		DefaultNodeAttrs = (NodeTypeEntry *) CCMalloc(TSIZE(NodeTypeEntry));
 		if (DefaultNodeAttrs==NULL)
			return FALSE;
	}
	else if (NumAttributes >= AttributeTableSize)
	{
		// Table is full - allocate some more table space.
		AttributeTableSize += AttributeTableGranularity;

		AttributeEntry *pTmpAttrs = 
			(AttributeEntry *) CCRealloc(DefaultAttrValues, TSIZE(AttributeEntry));

		if (pTmpAttrs==NULL)
			return FALSE;

		DefaultAttrValues = pTmpAttrs;

		// Increase table block for NodeAttributes as well.
		NodeTypeEntry *pTmpNodeAttrs = 
			(NodeTypeEntry *) CCRealloc(DefaultNodeAttrs, TSIZE(NodeTypeEntry));

		if (pTmpNodeAttrs == NULL)
		{
			// Error - try to realloc first block down to previous size 
			// (this shouldn't fail, but just in case...)
			AttributeTableSize -= AttributeTableGranularity;
			pTmpAttrs = (AttributeEntry *) CCRealloc(DefaultAttrValues, TSIZE(AttributeEntry));

			if (pTmpAttrs != NULL)
				// It worked, so install as actual pointer
				DefaultAttrValues = pTmpAttrs;

			// Return error
			return FALSE;
		}

		// Worked ok - install pointer.
		DefaultNodeAttrs = pTmpNodeAttrs;
	}

	// It worked
	return TRUE;
}

// Pass in a default attribute value object, and it returns the unique ID allocated
// for that attribute.
// Tim fn
UINT32 AttributeManager::RegisterDefaultAttribute(CCRuntimeClass *pNodeType, 
												AttributeValue *pValue)
{
	// Make sure we have the room to add a new attribute.
	if (!EnsureTableSpaceAvailable())
		return ATTR_BAD_ID;

	ENSURE(pValue != NULL, "NULL AttributeValue passed to RegisterDefaultAttribute()");

	// Add new attribute value - make it permanent so render regions don't try to
	// delete it when they exit.
	DefaultAttrValues[NumAttributes].pAttr 	= pValue;
	DefaultAttrValues[NumAttributes].Temp  	= FALSE;
	DefaultAttrValues[NumAttributes].Ignore = FALSE;
	
	// Add new attribute node type
	DefaultNodeAttrs[NumAttributes].pNodeType = pNodeType;

	// Update attribute count.
	NumAttributes++;

	// Allocate ID (actually just an offset into the arrays).
	return (NumAttributes - 1);
}


/********************************************************************************************

>	AttributeEntry *AttributeManager::GetDefaultAttributes()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Returns:	Pointer to the array of AttributeValue entries. You should call CCFree()
				on this pointer when you have finished with this block of memory.
				May be NULL if out of memory.
	Purpose:	Get a copy of all the default attributes.  This is used by render regions
				and import filters to initialise themselves to a sensible default state.

				NB. Do NOT change the attribute values pointed to by this array!!!
				They are the actual default attribute values - when you change an attribute,
				change the pointer (pAttr) to point to your own attribute - do not change
				what the pointer is pointing to.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager::GetNumAttributes; AttributeEntry

********************************************************************************************/

AttributeEntry *AttributeManager::GetDefaultAttributes()
{
	// Copy the default attribute table and return it.
	ENSURE(NumAttributes > 0, "No attributes have registered yet!");

	// Allocate the block required (NB. can't use new as it doesn't call the correct
	// new operator for some unknown reason - sees to be linked to array allocation?).
	INT32 Size = NumAttributes * sizeof(AttributeEntry);
	AttributeEntry *pAttrs = (AttributeEntry *) CCMalloc(Size);
	if (pAttrs == NULL)
		return NULL;

	// Copy the data and return a pointer to it
	memcpy(pAttrs, DefaultAttrValues, Size);

// ****** Bodge ********
// We must make the Line and Fill colours in the Default Attrs
// into Index colours in the Current Document !!
/*
	DocColour DefBlack;
	FindDefaultColour(ColourManager::GetCurrentColourList(),
						_R(IDS_BLACKNAME), &DefBlack);

	((StrokeColourAttribute*)DefaultAttrValues[ATTR_STROKECOLOUR].pAttr)->SetStartColour(&DefBlack);
	((FillGeometryAttribute*)DefaultAttrValues[ATTR_FILLGEOMETRY].pAttr)->SetStartColour(&DefBlack);
*/
// ****** End Bodge ********

	return pAttrs;
}

/********************************************************************************************

>	static NodeAttribute* AttributeManager::GetDefaultAttribute(AttrIndex AttrID)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/00
	Inputs:		AttrID		The attribute index of the attribute we want (e.g. ATTR_WEBADDRESS)
	Returns:	NodeAttribute pointer to newly allocated node attr
				NULL if insufficient mem to create new node attr
					OR no default attr registered with AttrID
	Purpose:	Gets the default value of a particular attribute, and returns it as a
				node attribute
	Errors:		If there is no default attribute registered of the AttrIndex we want
	SeeAlso:	WebAddressDlg::FindCommonWebAttribute

********************************************************************************************/

NodeAttribute* AttributeManager::GetDefaultAttribute(AttrIndex AttrID)
{
	AttributeValue * pAttrVal = NULL;
	NodeAttribute * pNewAttr = NULL;
	if(!DefaultAttrValues)
	{
		ENSURE(FALSE,"Default attr table not allocated yet.");
		return NULL;
	}

	if (AttrID==ATTR_BAD_ID)
	{
		ENSURE(FALSE,"BAD ID passed to GetDefaultAttribute");
		return NULL;
	}

	pAttrVal = DefaultAttrValues[AttrID].pAttr;
	if(!pAttrVal)
	{
		ENSURE(FALSE,"No default attr for ATTR_ID");
		return NULL;
	}

	// make a new node out of this attribute value
	pNewAttr = pAttrVal->MakeNode();
	if(!pNewAttr)
	{
		ENSURE(FALSE,"Not enough mem to create default attribute copy");
		return NULL;
	}

	return pNewAttr;
}

/********************************************************************************************

>	static AttributeValue* AttributeManager::GetDefaultAttribute(AttrIndex AttrID)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/00
	Inputs:		AttrID		The attribute index of the attribute we want (e.g. ATTR_WEBADDRESS)
	Returns:	AttributeValue pointer to default AttrValue
				NULL if no default attr registered with AttrID
	Purpose:	Used to get a pointer to a default attr _for_informational_purposes_only!
				NB do not modify the returned AttributeValue
	SeeAlso:	RenderRegion::InitDevice

********************************************************************************************/
AttributeValue* AttributeManager::GetDefaultAttributeVal(AttrIndex AttrID)
{
	AttributeValue * pAttrVal = NULL;
//	NodeAttribute * pNewAttr = NULL;
	if(!DefaultAttrValues)
	{
		ENSURE(FALSE,"Default attr table not allocated yet.");
		return NULL;
	}

	pAttrVal = DefaultAttrValues[AttrID].pAttr;
	if(!pAttrVal)
	{
		ENSURE(FALSE,"No default attr for ATTR_ID");
		return NULL;
	}

	return pAttrVal;
}

/********************************************************************************************

>	BOOL AttributeManager::GetDefaultAttribute(AttrIndex aiToGet, AttributeValue* pavReturn)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/97
	Inputs:		aiToGet		The attribute index of the attribute we want (e.g. ATTR_WEBADDRESS)
	Outputs:	pavReturn	A pointer to an AttributeValue in which to put a copy
							of the default AttributeValue

	Returns:	TRUE if there were no errors
				FALSE if there was no default attribute registered of the type we wanted
	Purpose:	Gets the default value of a particular attribute.

				Used by WebAddressDlg class to get the default Web Address attribute.

	Errors:		If there is no default attribute registered of the AttrIndex we want
	SeeAlso:	WebAddressDlg::FindCommonWebAttribute

********************************************************************************************/

BOOL AttributeManager::GetDefaultAttribute(AttrIndex aiToGet, AttributeValue* pavReturn)
{
	//Check our data
	ERROR3IF(NumAttributes <= 0, "AttributeManager::GetDefaultAttribute - No attributes have registered yet!");
	ERROR3IF(pavReturn==NULL, "AttributeManager::GetDefaultAttribute - passed NULL pointer");

	//Get the attribute entry we are looking for
	AttributeEntry* paeReturn=&DefaultAttrValues[aiToGet];

	//And return the attribute pointer hidden inside that attribute entry
	if (paeReturn && paeReturn->pAttr)
	{
		//Put a copy of the default AttributeValue into the AttributeValue that
		//the pointer we have been passed points to
		*pavReturn=*(paeReturn->pAttr);

		//And return TRUE
		return TRUE;
	}
	else
		//There's been a problem. Return FALSE.
		return FALSE;
}


/********************************************************************************************

>	UINT32 AttributeManager::GetNumAttributes()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Returns:	Number of registered atributes: 0 => no attributes registered yet.
	Purpose:	Find out how many attributes have been registered with the attribute
				manager.  This also tells you how long the default attribute array is
				as returned by AttributeManager::GetDefaultAttributes.
	SeeAlso:	AttributeManager::GetDefaultAttributes

********************************************************************************************/

UINT32 AttributeManager::GetNumAttributes()
{
	return NumAttributes;
}

/********************************************************************************************

>	BOOL AttributeManager::ApplyBasedOnDefaults(Node *Target, AttributeEntry *AttrsToApply)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		Target - the node to apply the attributes to.
				AttrsToApply - the attribute array to apply to this node.
	Returns:	TRUE if attributes applied to nodes successfully;
				FALSE if otherwise.
	Purpose:	Given a set of attributes, apply the ones that differ from the defaults to 
				the specified node.  Most commonly used by import filters.
	Errors:		Out of memory.

********************************************************************************************/

BOOL AttributeManager::ApplyBasedOnDefaults(Node *Target, AttributeEntry *AttrsToApply)
{
	// Cycle through each type of attribute, and add it if:
	//	(a) It is of a type that can take the attribute, and
	//  (b) the attribute is different to the default attribute.

	// There is a short cut for telling if the attribute has been changed from the default;
	// if the pointer to the attribute is still the same as our copy, then it can't have
	// changed

// ****** Bodge ********
// We must make the Line and Fill colours in the Default Attrs
// into Index colours in the Current Document !!
/*
	DocColour DefBlack;
	FindDefaultColour(ColourManager::GetCurrentColourList(),
						_R(IDS_BLACKNAME), &DefBlack);

	((StrokeColourAttribute*)DefaultAttrValues[ATTR_STROKECOLOUR].pAttr)->SetStartColour(&DefBlack);
	((FillGeometryAttribute*)DefaultAttrValues[ATTR_FILLGEOMETRY].pAttr)->SetStartColour(&DefBlack);
*/
// ****** End Bodge ********

	for (INT32 i = NumAttributes - 1; i >= 0; i--)
	{
		// Has it changed, and is it of the right node type?
		if (!AttrsToApply[i].Ignore &&
			(DefaultAttrValues[i].pAttr != AttrsToApply[i].pAttr) &&
			(Target->IsKindOf(DefaultNodeAttrs[i].pNodeType)))
		{
			// This node can take this type of attribute.
			if ((DefaultAttrValues[i].pAttr == NULL) || 
				(AttrsToApply[i].pAttr->GetRuntimeClass() != 
				 DefaultAttrValues[i].pAttr->GetRuntimeClass()) ||
		        AttrsToApply[i].pAttr->IsDifferent(DefaultAttrValues[i].pAttr))
			{
				// The attribute is different to the default - construct the appropriate 
				// attribute node.
				Node *pNode = AttrsToApply[i].pAttr->MakeNode();

				// Enough memory?
				if (pNode == NULL)
					return FALSE;

				// Add it to the target
				pNode->AttachNode(Target, FIRSTCHILD);
		    }
		}
		
	}

	// All default attributes processed ok.
	return TRUE;
}


/********************************************************************************************

>	void AttributeManager:: AttributeSelected(NodeAttribute* Attrib, NodeAttribute* OldAttr = NULL)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		Attrib:	The selected attribute.  All attributes of the same type in the
						selection will be replaced with this attribute.
						Don't delete this it may become a current attribute.
					
					New Bits Added by Will:
						When NULL this indicates that the OldAttr parameter should
						act as a Mutator.  A Mutator will change all the fill attributes
						in the selection to be the same 'type' as the mutator, whilst
						retaining their individual control points and colours.
						A special kind of Mutator, 'AttrColourChange' will force all
						objects in the selection to change their selected control point
						colours, or to 'Mutate'into an 'AttrFlatFill' if there are
						no selected control points anywhere in the selection.
						A Mutator must be derived from AttrFillGeometry.

				
				OldAttr: When no-null, this specifies the node which 'Attrib' is to replace.
						 It is used when an attribute is edited.

						 When Attrib == NULL, this specifies a Mutator (see above).

				Intermediate: TRUE when we expect someone to call AttributeSelected again
								after this call with the same attr types, same selection
								and Intermediate set to FALSE
				 
	Purpose:	This function should be called whenever an attribute is selected by 
				the user. 
				
				If any nodes are selected the function invokes an operation to apply the
				attribute to the selected nodes.

				If no nodes are selected then the attribute may become a current attribute
				depending on the preferences.

	Errors:		-
	SeeAlso:	AttributeManager::ReplaceAttributes; AttributeManager::ApplyAttribToNode

********************************************************************************************/

void AttributeManager::AttributeSelected(NodeAttribute* Attrib, NodeAttribute* OldAttr)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ENSURE( (Attrib != NULL || OldAttr != NULL), "NULL attributes passed to AttributeSelected"); 

	BOOL Mutate = (Attrib == NULL);
	AttrFillGeometry* Mutator = NULL;

	pLastNodeAppliedTo = NULL;

	// If parameter one is NULL then we must be doing a Mutation
	if (Mutate)
	{
		ERROR3IF(!OldAttr->IsAFillAttr(), "A Mutator must be a fill attribute");
		// Make a more sensible pointer
        Mutator = (AttrFillGeometry*)OldAttr;
	}

	// If there are any objects selected then the attribute needs to be applied to these objects
//	OpDescriptor* OpDesc = NULL;
//	BOOL MakeAttrCurrent = FALSE; // until we know better

	SelRange* Sel = GetApplication()->FindSelection();
	ERROR3IF(Sel==NULL,"Can't find SelRange!");
	if (Sel==NULL) return;

	// If there are no objects in the selection, or if none of the selected objects require
	// the attribute, then it will not be applied

	BOOL ApplyAttribute; 

	List AttrGroupList;  

	if (Attrib != NULL)
	{
		ApplyAttribute = CanBeAppliedToSelection(Attrib, &AttrGroupList);
	}
	else
	{
		ERROR3IF(OldAttr == NULL, "What are we applying then exactly ??");
		ApplyAttribute = CanBeAppliedToSelection(OldAttr, &AttrGroupList); // Mutator 
	} 
	

	if (ApplyAttribute)
	{ 
		// We need to invoke an operation to apply the attribute to the currently selected 
		// objects. 

		// Get the number of selected fill control points, 'cus we may want to do
		// something different depending on how many their are.
		AttrFillGeometry::SelectionCount = AttrFillGeometry::CountSelectionControlPoints();

		// Obtain a pointer to the op descriptor for the attribute operation 
		BOOL bApplying = TRUE;
		OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpApplyAttribToSelected));

		// But first ... Was the pervious Op the same ?
		Operation* pLastOp = Document::GetSelected()->GetOpHistory().FindLastOp();

		if (Mutate && pLastOp && pLastOp->GetRuntimeClass() == CC_RUNTIME_CLASS(OpApplyAttribToSelected))
		{
			// The last operation was similar to this one, but was it exactly the same type ?
			OpApplyAttribToSelected* pLastApplyOp = (OpApplyAttribToSelected*)pLastOp;

			if (pLastApplyOp->GetValueChangeType() == Mutator->GetRuntimeClass() &&
				pLastApplyOp->IsMergeableApplyOp())
			{
				// Yep, they are exactly the same type of value change.
				// So we don't need to do an undoable op for this new change,
				// we can just invoke a special op that will change the attributes
				// directly.

				// But first ... Is the selection still the same ?
				RestoreSelectionsAction* pRestoreSelAct = (RestoreSelectionsAction*)
						pLastApplyOp->GetUndoActionList()->FindActionOfClass(CC_RUNTIME_CLASS(RestoreSelectionsAction));
				SelectionState* LastOpsSelection = pRestoreSelAct->GetSelState();

				SelectionState CurrentSelection;
				CurrentSelection.Record();

				if (*LastOpsSelection == CurrentSelection)
				{
					FillBlobSelectionState CurrentFillBlobState;
					CurrentFillBlobState.Record();

					if (OpRepeatApplyAttribToSelected::FillBlobState == CurrentFillBlobState)
					{
						OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpRepeatApplyAttribToSelected));
						bApplying = FALSE;
					}
				}
			}
		}

		// Invoke the operation, passing Attrib as a parameter
		OpParam tempParam((void *)Attrib,(void *)OldAttr);
		OpDesc->Invoke(&tempParam);

		// Remember the current blob selection state, so we can decide whether or not to
		// merge future apply operations
		if (bApplying)
			OpRepeatApplyAttribToSelected::FillBlobState.Record();

		UpdateAfterAttrApplied(Mutate ? Mutator : NULL);

	}

	BOOL bCurrentUpdated = UpdateCurrentAppliedAttr(Mutate ? Mutator: Attrib, &AttrGroupList, ApplyAttribute, Mutate);
	if (!bCurrentUpdated)
	{
		// Allow fill tools to update after a cancel
		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED));
	}

	// Tidy up attributes because they were only patterns and are no longer needed
	if (Mutate)
	{
		if (Mutator != NULL)
		{
			delete Mutator;
			Mutator = NULL;
		}
	}
	if (Attrib != NULL)
	{
		delete Attrib;
		Attrib = NULL;
	}

	// Don't forget the AttrGroupList
	AttrGroupList.DeleteAll();

	// And ensure that if we've caused the SelRange to set up any delayed message
	// broadcasts, that it sends them now rather than at some random time in the future
	// when some Op just happens to end.
	Sel->BroadcastAnyPendingMessages();
#endif
}


BOOL AttributeManager::UpdateAfterAttrApplied(NodeAttribute* pAttr)
{
	if (SendMessages)
	{
		// Inform the selection that attributes have changed
		SelRange* Sel = GetApplication()->FindSelection();

		Sel->AttrsHaveChanged(); 

		AttrFillGeometry::SelectionCount = AttrFillGeometry::CountSelectionControlPoints();

		if (pAttr && pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrColourChange)))
		{
			// If the Attrib was a Colour Mutator, then we must have changed a colour
			// so lets tell someone about it
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED)); 
		}
		else
		{
			// We've probably changed a Colour
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED)); 
			// and the Attrib may have changed shape or summit.	Who knows ?
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED)); 

			Sel->UpdateBounds();
		}
	}

	return TRUE;
}

BOOL AttributeManager::UpdateCurrentAppliedAttr(NodeAttribute* pAttr, List* pAttrGroupList, BOOL bAttrApplied, BOOL bMutate)
{
	Document* CurrentDoc = Document::GetSelected();
	if (CurrentDoc == NULL)
		return FALSE; // We are not going to be able to do anything if there is no document

	AttributeManager& AttrMgr = CurrentDoc->GetAttributeMgr();
	if (AttrMgr.WeShouldMakeAttrCurrent(bAttrApplied, pAttr, pAttrGroupList))
	{
		NodeAttribute* NewCurrent = pAttr;

		// 
		UpdateCurrentAttr(NewCurrent, bMutate, pAttrGroupList);

		// Some Attributes require a second attribute to be changed as well.
		BOOL IsMutate;
		NodeAttribute* pOther = AttributeManager::GetOtherAttrToApply(NewCurrent, &IsMutate);
	
		if (pOther != NULL)
		{
			UpdateCurrentAttr(pOther, IsMutate, pAttrGroupList); // can't have a NULL AttrGroupList
																	 // or it will apply to tools list
			delete pOther;
		}

		return TRUE;
	}

	return FALSE;
}


/********************************************************************************************

>	static void AttributesSelected(List& AttribsToApply, UINT32 OpName);


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		AttribsToApply: A list of NodeAttributePtrItems. These are the attributes
								which will be applied to the selection. The function
								does not currently support attribute mutation. 
								
								All AttrFillGeometry attributes should have their current 
								bounds set	using  AttrFillGeometry::SetBoundingRect.
								
								very important
								~~~~~~~~~~~~~~

								This function only ever applies copies of attributes
								in the AttribsToApply list. It is the responsibility
								of the caller to delete these attributes, if this
								is appropriate.

				OpName:			The name to give the operation which is performed by this
								function to apply attributes. eg. 'Paste Attributes'

	Outputs:	-
	Returns:	-
	Purpose:	This high level function should be called whenever we need to apply multiple 
				attributes to the selection (eg. PasteAttributes)
				
				If any objects are selected then the function invokes an operation to apply the
				attributes to these objects. 
				Before an AttrFillGeometry is applied to an object it gets moved and scaled.

				If no nodes are selected then the attributes may become  current
				depending on the preferences and what the user decides.

	SeeAlso:	AttributeManager::AttributeSelected

********************************************************************************************/

void AttributeManager::AttributesSelected(List& AttribsToApply, UINT32 OpName)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// The AttributeList cannot be empty
	ERROR3IF(AttribsToApply.IsEmpty(), "No attributes to apply in AttributeManager::AttributesSelected"); 
	if (AttribsToApply.IsEmpty()) return; // nothing to do

	// If there is no current document then there is very little we can do
	Document* CurrentDoc = Document::GetSelected();
	if (CurrentDoc == NULL)
		return; // We are not going to be able to do anything if there is no document

	SelRange* Sel = GetApplication()->FindSelection();
	ERROR3IF(Sel==NULL,"Can't find SelRange!");
	if (Sel==NULL) return;

	// If there are no objects in the selection, or if none of the selected objects require
	// the attribute, then it will not be applied
						    
	// Obtain a pointer to the op descriptor for the operation 
	OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpApplyAttribsToSelected));
				
	List AttrGroupsList;  // Will contain a list of current Attribute group sets. one for each attribute 
						  // in the AttribsToApply list. Each item in this list will be a ListListItem.

	BOOL Success; 		  		// FALSE if the operation fails
	BOOL AnyAttrsApplied;		// FALSE if the operation has not applied any attributes

	// Build the parameter we are about to pass to the operation
	ApplyAttribsToSelectedParam OpParam(&AttribsToApply,
										OpName, 
										&AttrGroupsList, 		// One per attribute applied
										&Success,				// Indicates if op was successful
										&AnyAttrsApplied); 		// Indicates if the operation applied
																// any attributes

	
	// Invoke the operation
	OpDesc->Invoke(&OpParam);

	if (Success)
	{
		// Inform neccessary parties
		if (AnyAttrsApplied)
		{
			// Inform the selection that attributes have changed
			Sel->AttrsHaveChanged();

			// We've probably changed a Colour
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED)); 
			// and the Attrib may have changed shape or summit.	Who knows ?
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED)); 
			Sel->UpdateBounds();
		}
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Now decide which attributes in the AttribsToApply list should be made current
											   
		AttributeManager& AttrMgr = CurrentDoc->GetAttributeMgr();
		AttrsToMakeCurrent AttribsToMakeCurrent = AttrMgr.GetAttributesToMakeCurrent(AttribsToApply, 
								   					 						 AttrGroupsList,
																			 FALSE);
								   					 				
		if (AttribsToMakeCurrent == NONE)
		{
			// Allow fill tools to update after a cancel
			// I'm not sure if we need this ???? Ask Will
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED)); 
		}
		else
		{
			BOOL AttributeApplied;
			NodeAttributePtrItem* pAttr = (NodeAttributePtrItem*)(AttribsToApply.GetHead());
			ListListItem* pAttrsGroups = (ListListItem*) (AttrGroupsList.GetHead());
			while (pAttr)
			{
				AttributeApplied = (!(pAttrsGroups->list.IsEmpty())); // Empty Current attribute group list
				if (!AttributeApplied || (AttribsToMakeCurrent == ALL))
				{
					// Add attribute to all current attribute groups in the list, or to the 
					// group associated with the current tool if the list is empty.
					UpdateCurrentAttr(pAttr->NodeAttribPtr, 
									  FALSE, // Not a mutator
									  &(pAttrsGroups->list), 
									  FALSE); // Don't tell world yet (too slow)
				}
				pAttr = (NodeAttributePtrItem*)(AttribsToApply.GetNext(pAttr));
				pAttrsGroups = (ListListItem*)(AttrGroupsList.GetNext(pAttrsGroups));
			}
			// Tell world current attributes have changed
			BROADCAST_TO_ALL(CurrentAttrChangedMsg()); 
		} 
	}
	// Tidyup
	AttrGroupsList.DeleteAll(); // Delete all group lists 

	// And ensure that if we've caused the SelRange to set up any delayed message
	// broadcasts, that it sends them now rather than at some random time in the future
	// when some Op just happens to end.
	Sel->BroadcastAnyPendingMessages();
#endif
}


/********************************************************************************************

>	BOOL SelOperation::AttributesSelected(List& AttribsToApply, UINT32 OpName, BOOL bPasteAttrs);


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/94
	Inputs:		AttribsToApply: A list of NodeAttributePtrItems. These are the attributes
								which will be applied to the selection. The function
								does not currently support attribute mutation. 
								
								All AttrFillGeometry attributes should have their current 
								bounds set	using  AttrFillGeometry::SetBoundingRect.
								
								very important
								~~~~~~~~~~~~~~

								This function only ever applies copies of attributes
								in the AttribsToApply list. It is the responsibility
								of the caller to delete these attributes, if this
								is appropriate.

				OpName:			The name to give the operation which is performed by this
								function to apply attributes. eg. 'Paste Attributes'

	Outputs:	-
	Returns:	-
	Purpose:	This high level function should be called whenever we need to apply multiple 
				attributes to the selection (eg. PasteAttributes)
				
				If any objects are selected then the function invokes an operation to apply the
				attributes to these objects. 
				Before an AttrFillGeometry is applied to an object it gets moved and scaled.

				If no nodes are selected then the attributes may become  current
				depending on the preferences and what the user decides.

	SeeAlso:	AttributeManager::AttributeSelected

********************************************************************************************/

BOOL SelOperation::DoAttributesSelected(List& AttribsToApply, UINT32 OpName, BOOL bPasteAttrs)
{
	// The AttributeList cannot be empty
	ERROR3IF(AttribsToApply.IsEmpty(), "No attributes to apply in AttributeManager::AttributesSelected"); 
	if (AttribsToApply.IsEmpty()) return FALSE; // nothing to do

	// If there is no current document then there is very little we can do
	Document* CurrentDoc = Document::GetSelected();
	if (CurrentDoc == NULL)
		return TRUE; // We are not going to be able to do anything if there is no document

	SelRange* Sel = GetApplication()->FindSelection();
	ERROR3IF(Sel==NULL,"Can't find SelRange!");
	if (Sel==NULL) return TRUE;

	// If there are no objects in the selection, or if none of the selected objects require
	// the attribute, then it will not be applied
						    
	List AttrGroupsList;  // Will contain a list of current Attribute group sets. one for each attribute 
						  // in the AttribsToApply list. Each item in this list will be a ListListItem.

	BOOL Success; 		  		// FALSE if the operation fails
	BOOL AnyAttrsApplied;		// FALSE if the operation has not applied any attributes

	// Build the parameter we are about to pass to the operation
	ApplyAttribsToSelectedParam OpParam(&AttribsToApply,
										OpName, 
										&AttrGroupsList, 		// One per attribute applied
										&Success,				// Indicates if op was successful
										&AnyAttrsApplied); 		// Indicates if the operation applied
																// any attributes

	
	// Invoke the core of the operation
	DoApplyAttribsToSelection(&OpParam, !GetStarted());			// Don't clear actions if the caller already called DoStartSelOp

	if (Success)
	{
		// Inform neccessary parties
		if (AnyAttrsApplied)
		{
			// Inform the selection that attributes have changed
			Sel->AttrsHaveChanged();

			// We've probably changed a Colour
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED)); 
			// and the Attrib may have changed shape or summit.	Who knows ?
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED)); 
			Sel->UpdateBounds();
		}
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Now decide which attributes in the AttribsToApply list should be made current
											   
		AttributeManager& AttrMgr = CurrentDoc->GetAttributeMgr();
		AttributeManager::AttrsToMakeCurrent AttribsToMakeCurrent = AttrMgr.GetAttributesToMakeCurrent(AttribsToApply, 
								   					 						 AttrGroupsList,
																			 bPasteAttrs);
								   					 				
		if( AttribsToMakeCurrent == AttributeManager::NONE )
		{
			// Allow fill tools to update after a cancel
			// I'm not sure if we need this ???? Ask Will
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED)); 
		}
		else
		{
			BOOL AttributeApplied;
			NodeAttributePtrItem* pAttr = (NodeAttributePtrItem*)(AttribsToApply.GetHead());
			ListListItem* pAttrsGroups = (ListListItem*) (AttrGroupsList.GetHead());
			while (pAttr)
			{
				AttributeApplied = (!(pAttrsGroups->list.IsEmpty())); // Empty Current attribute group list
				if (!AttributeApplied || (AttribsToMakeCurrent == AttributeManager::ALL))
				{
					// Add attribute to all current attribute groups in the list, or to the 
					// group associated with the current tool if the list is empty.
					AttributeManager::UpdateCurrentAttr(pAttr->NodeAttribPtr, 
									  FALSE, // Not a mutator
									  &(pAttrsGroups->list), 
									  FALSE); // Don't tell world yet (too slow)
				}
				pAttr = (NodeAttributePtrItem*)(AttribsToApply.GetNext(pAttr));
				pAttrsGroups = (ListListItem*)(AttrGroupsList.GetNext(pAttrsGroups));
			}
			// Tell world current attributes have changed
			BROADCAST_TO_ALL(CurrentAttrChangedMsg()); 
		} 
	}
	// Tidyup
	AttrGroupsList.DeleteAll(); // Delete all group lists 

	// And ensure that if we've caused the SelRange to set up any delayed message
	// broadcasts, that it sends them now rather than at some random time in the future
	// when some Op just happens to end.
	Sel->BroadcastAnyPendingMessages();

	return Success;
}


/********************************************************************************************

>	AttrsToMakeCurrent AttributeManager::GetAttributesToMakeCurrent(List& Attribs, 
																	List& AttrGroupSetList,
																	BOOL bPasteAttrs)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95

	Inputs:		Attribs: 		A list of attributes that the user has tried to apply.
				AttrGroupList:	A list of Current Attribute group sets. One set for each attribute.
								in the Attribs list. If an attribute group exists in an 
								attribute's set then the attribute has been applied to an object
								which is associated with that group. If an attribute's set 
								is empty then this attribute has not been applied to any
								objects.
							   
	Returns:	ALL: 		 All attributes in the Attribs list should be made current
				NOT_APPLIED: Only those attributes which have not been applied to objects
							 should be made current.
				NONE:		 No attributes should be made current. 

	Purpose:	To determine which attributes in the Attribs list to make current. The user
				is prompted where neccessary.

	SeeAlso:	AttriibuteManager::AttributesSelected

********************************************************************************************/

AttributeManager::AttrsToMakeCurrent AttributeManager::GetAttributesToMakeCurrent(List& Attribs, 
																		 List& AttrGroupSetList,
																		 BOOL bPasteAttrs)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (LastAttrAppliedBecomesCurrent)	// Preference
	{
		// All attributes should become current, applied or not
		return ALL;
	}
	
	// if all attributes have been applied to at least one object then no attributes
	// should be made current
	
	BOOL AllAttrsApplied = TRUE; 

	ListListItem* pCurrentAttrSet = (ListListItem*)(AttrGroupSetList.GetHead()); 
	while (pCurrentAttrSet)
	{
		if (pCurrentAttrSet->list.IsEmpty())
		{
			// The current attribute group set is empty => Attribute has not been applied
			AllAttrsApplied = FALSE;
			break; 
		}
		pCurrentAttrSet = (ListListItem*)(AttrGroupSetList.GetNext(pCurrentAttrSet)); 
	}

	if (AllAttrsApplied)
		return NONE; 

	if (!AttributeManager::AskBeforeSettingCurrentAttr)
		return NOT_APPLIED;
			
	// ok so ask the user
	SelRange* Sel = GetApplication()->FindSelection();
	
	UINT32 nQueryStringID;

	if (Sel->Count() > 0)
	{
		if (bPasteAttrs)
			// This question is misleading in the context of Paste Attributes
			// where the paste has already succeeded but this question pops up
			// asking about current attrs...
			return NONE;
		else
			// 'Some attributes could not be applied to the selection.' 
			// 'Do you wish to make these Current Attributes ?'
			nQueryStringID = _R(IDS_ATTRS_NOT_REQD_BY_OBJS);
	}
	else
	{
		if (bPasteAttrs)
			nQueryStringID = _R(IDS_NO_OBJS_SEL_PASTE_ATTRS_CURRENT);
		else
			// 'No objects are currently selected.'
			// 'Do you wish to make the applied attributes Current Attributes ?'
			nQueryStringID = _R(IDS_NO_OBJS_SEL_MAKE_ATTRS_CURRENT);
	}

	String_256 QueryString(nQueryStringID);

	// The only way of bringing up a box with a string in it
	Error::SetError(0, QueryString, 0);
	SetNextMsgHelpContext(nQueryStringID);
	INT32 DlgResult = InformMessage(0, _R(IDS_SET), _R(IDS_CANCEL), _R(IDS_QUIET));
	Error::ClearError();

	// Now decide what to do
   	switch(DlgResult)
	{
		case 1:	 // Set
		{
			return NOT_APPLIED;
		}
		case 2: // Cancel
		{
			return NONE;
		}
		case 3:	// Quiet
		{
			// Don't ask again
			AttributeManager::AskBeforeSettingCurrentAttr = FALSE;  
			// Send round a message to say that this has changed so that if the edit options
			// tab is up, it knows about it.  
			BROADCAST_TO_ALL(OptionsChangingMsg(OptionsChangingMsg::ASKBEFORESETTINGATTR));
			return NOT_APPLIED;
		}
		default: 
		{
			ERROR3("I don't know what to do");
			return NONE;
		}
	}
#else
	return NONE;
#endif
}


 /********************************************************************************************

  >	static BOOL AttributeManager::WeShouldMakeAttrCurrent(BOOL AttributeApplied,
  														  NodeAttribute* pAttr, 
  														  List* pAttrGroupList, 
														  BOOL DroppedAttr  FALSE, 
  														  BOOL AttrDroppedOntoPage = FALSE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		AttributeApplied: Set to TRUE if the attribute has been applied to at
								  least one object

				pAttr:			  A pointer to the attribute

				pAttrGroupList: A list of attribute groups that the attribute should 
							    be added to. If this list is empty then the attribute
							    will be added to the attribute group associated with the
							    current tool.The List should contain AttributeGroupItems.

				DroppedAttr:	TRUE if the attribute is being applied by dragging and 
								dropping.

				AttrDroppedOntoPage: This flag should only be set to TRUE if a drag n' drop
									 drop attribute operation is taking place. Set to TRUE
									 if the attribute was not dropped onto an object.


	Outputs:	-
	Returns:	TRUE if an attribute should be made current
				FALSE if it should not
	Purpose:	To determine if we should make an attribute current or not. The routine
				does NOT delete the Attrib if it is not being made current.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
					
BOOL AttributeManager::WeShouldMakeAttrCurrent(BOOL AttributeApplied,  
											   NodeAttribute* pAttr, 
											   List* pAttrGroupList, 
											   BOOL DroppedAttr,
											   BOOL AttrDroppedOntoPage)
{
#if !defined(EXCLUDE_FROM_RALPH)
	UseLastAttrApplied = FALSE;

	//Graham 20/8/96: First we ask the attribute if it wants to be become current.
	//All attributes can become current except Hot Link attributes at the moment
	if(!pAttr->ShouldBecomeCurrent()) return FALSE;


	// We need to consider setting the attribute as current if 
	// a. It has not been applied
	// b. LastAttrAppliedBecomesCurrent is TRUE
	if ((!AttributeApplied) || LastAttrAppliedBecomesCurrent)
	{
		// Determine if we need to ask the user's permission prior to setting the attribute
		if (LastAttrAppliedBecomesCurrent || (!AttributeManager::AskBeforeSettingCurrentAttr))
		{
			UseLastAttrApplied = LastAttrAppliedBecomesCurrent;

			return TRUE;
		}
		else 
		{
			// Create a string describing the attribute groups the attribute could be added to
			// this is a little bit messy. It would be nicer if the group list contained the 
			// proper group set at this point. This is a release BODGE !

			String_256 AttrGroupsString;


			// If the attriburte is a text attribute then we will be adding it to a text group
			if (pAttr->IsKindOfTextAttribute())
			{
				AttrGroupsString = String(_R(IDS_TEXT));
			}
			else
			{
				// if the AttrGroupList	is empty we will use the attribute group associated with
				// the selected tool.
				if (pAttrGroupList->IsEmpty())
				{
					ToolListItem* pItem = Tool::GetCurrent()->Parent;
					if (pItem)
						AttrGroupsString = GetAttributeGroup((pItem->m_ToolInfo.CurrentAttributeGroup))->GroupName;
				}
				else
				{
					// Add all group names
					AttributeGroupItem* pAttributeGroupItem;
					pAttributeGroupItem	= (AttributeGroupItem*) pAttrGroupList->GetHead();
 					ERROR3IF(!pAttributeGroupItem, "Attribute group list should not be empty");
 					AttrGroupsString = (GetAttributeGroup(pAttributeGroupItem->AttrGroup))->GroupName; 
					pAttributeGroupItem	= (AttributeGroupItem*) pAttrGroupList->GetNext(pAttributeGroupItem);
					if (pAttributeGroupItem)
					{
						AttrGroupsString += String(TEXT(" "));
						AttrGroupsString += String(_R(IDS_AND));
						AttrGroupsString += String(TEXT(" "));
						AttrGroupsString += (GetAttributeGroup(pAttributeGroupItem->AttrGroup))->GroupName; 
					}
				}
			}
			  			
			AttrGroupsString += String(TEXT(" ")); 
			AttrGroupsString += String(_R(IDS_ATTRIBUTEC));
			AttrGroupsString += String(_R(IDS_QM));


			String_256 AttrNameString;
			AttrNameString += String(_R(IDS_K_ATTRMGR_OPENQUOTE));
			AttrNameString += String_256(pAttr->GetAttrNameID());
			AttrNameString += String(_R(IDS_K_ATTRMGR_CLOSEQUOTE));
			AttrNameString += String(_R(IDS_A_CURRENT));
			AttrNameString += String(TEXT(" "));
			AttrNameString += AttrGroupsString;
							  							   
			// Ask the user what they want to do in this situation
			SelRange* Sel = GetApplication()->FindSelection();
			UINT32 nQueryStringID;
			if (DroppedAttr)
			{
				// A drag n' drop apply?
				if (AttrDroppedOntoPage)
				{
					// Attribute was dropped onto the page
					nQueryStringID = _R(IDS_DROPPED_ONTO_PAGE);
				} 
				else 
				{
					// Object did not require the attribute
					nQueryStringID = _R(IDS_ATTRIB_NOT_REQUIRED_BY_OBJECT);
				}
			}
			else if (Sel->Count() > 0)
			{
				// The attribute was not required by anyone
				nQueryStringID = _R(IDS_ATTRIB_NOT_REQUIRED);
			}
			else
			{
				// There were no objects selected
				// No objects are currently selected\n Do you wish to make
				nQueryStringID = _R(IDS_MAKE_COL_CURRENT_ATTRIB);
			}

			// Load and build the question text.
			String_256 QueryString(nQueryStringID);
			QueryString += AttrNameString;

			// The only way of bringing up a box with a string in it
			Error::SetError(0, QueryString, 0);
			SetNextMsgHelpContext(nQueryStringID);
			INT32 DlgResult = InformMessage(0, _R(IDS_SET), _R(IDS_CANCEL), _R(IDS_QUIET));
			Error::ClearError();

			switch(DlgResult)
			{
				case 1:	 // Set
				{
					return TRUE;
				}
				case 2: // Cancel
				{
					return FALSE; 
				}
				case 3:	// Quiet
				{
					// Don't ask again
					AttributeManager::AskBeforeSettingCurrentAttr = FALSE;  
					// Send round a message to say that this has changed so that if the edit options
					// tab is up, it knows about it.  
					BROADCAST_TO_ALL(OptionsChangingMsg(OptionsChangingMsg::ASKBEFORESETTINGATTR));
					return TRUE;
				}
				default: 
				{
					ERROR3("I don't know what to do");
					return FALSE;
				}
			}

		}
	}
	else 
		return FALSE;	
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	static BOOL AttributeManager::CanBeAppliedToSelection(NodeAttribute* Attrib, List* pAttrGroups)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		Attrib: The attribute 

	Outputs:	AttrGroups: If any of the selected objects requires Attrib then this list
							is a set of all current attribute groups that the attribute
							should be added to if it were to be made current. This
							list should be passed in as a parameter to UpdateCurrentAttr.

							If there are no objects in the selection, or if none of the selected
							objects requires the attribute then this list will be empty on return.
							UpdateCurrentAttr handles this case by adding the attribute to
							the attribute group associated with the selected tool.

	Returns:	TRUE if at least one object requires Attrib

	Purpose:	This function would ideally live in the SelRange. It determines if the 
				attribute can be applied to at least one object in the selection.
	SeeAlso:	AttributeManager::AttributeSelected
	SeeAlso:	AttributeManager::UpdateCurrentAttr

********************************************************************************************/

BOOL AttributeManager::CanBeAppliedToSelection(NodeAttribute* Attrib, List* pAttrGroups)
{
#if !defined(EXCLUDE_FROM_RALPH)
	SelRange* Sel = GetApplication()->FindSelection();

	BOOL RequiresAttrib = FALSE; 

	// Go though all the selected nodes
	NodeRenderableInk* Current = (NodeRenderableInk*)Sel->FindFirst();
	while (Current != NULL)
	{
		// See if this node requires the attribute
		if (CanBeAppliedToNode(Current, Attrib, pAttrGroups))
		{
			RequiresAttrib = TRUE;
			if ((INT32)(pAttrGroups->GetCount()) == NUM_ATTR_GROUPS)
				break;
		}
		Current = (NodeRenderableInk*)(Sel->FindNext(Current));
	}
	return RequiresAttrib;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	static BOOL AttributeManager::CanBeAppliedToNode(NodeRenderableInk* pObject,
													 NodeAttribute* Attrib, List* pAttrGroups)


	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/95
	Inputs:		Attrib: The attribute 

	Outputs:	AttrGroups: If the object requires Attrib then this list is updated to 
							include the current attribute group that the attribute
							should be added to if it were to be made current. This
							list should be passed in as a parameter to UpdateCurrentAttr.

	Returns:	TRUE if the object requires Attrib

	Purpose:	This function would ideally live in the SelRange. It determines if the 
				attribute can be applied to a specified object.

	SeeAlso:	AttributeManager::CanBeAppliedToSelection;
				AttributeManager::AttributeSelected; 
				AttributeManager::UpdateCurrentAttr

********************************************************************************************/

BOOL AttributeManager::CanBeAppliedToNode(NodeRenderableInk* pObject,
										  NodeAttribute* Attrib, List* pAttrGroups)
{
	BOOL RequiresAttrib = FALSE; 

	CCRuntimeClass* NewAttrGroup;
	CCRuntimeClass* InSetAttrGroup; // One that's already in the list 
	AttributeGroupItem* pAttrGrpItem;

	BOOL InSet;

	NodeRenderableInk* Current = pObject;

	if (Current != NULL)
	{
		if (Current->RequiresAttrib(Attrib))
		{
			RequiresAttrib = TRUE; 

			// Obtain the attribute group
			NewAttrGroup = Current->GetCurrentAttribGroup();
			ERROR3IF(NewAttrGroup == NULL, "Object has a NULL attribute group"); 

			InSet = FALSE; 

			// Search the AttrGroups set to see if it's already there
			pAttrGrpItem = (AttributeGroupItem*)pAttrGroups->GetHead();
			while(pAttrGrpItem != NULL)
			{
				InSetAttrGroup = pAttrGrpItem->AttrGroup;
				ERROR3IF(InSetAttrGroup == NULL, "NULL attribute group found");
				if (NewAttrGroup == InSetAttrGroup)
				{
					InSet = TRUE; 
					break; // It's already in the set don't add it
				}
				pAttrGrpItem = (AttributeGroupItem*)pAttrGroups->GetNext(pAttrGrpItem);
			}
			
			if (!InSet)	// The AttrGroup needs adding to the pAttrGroups set
			{
				// Create a new AttrGroupItem
				pAttrGrpItem = new AttributeGroupItem();
				if (pAttrGrpItem == NULL)
					return FALSE; // Panic !!

				pAttrGrpItem->AttrGroup = NewAttrGroup;

				// And add it to our set
				pAttrGroups->AddHead(pAttrGrpItem); // Most recent at start of list.
			}  
		}
	}

	return RequiresAttrib;
}

/********************************************************************************************

>	void AttributeManager::ReplaceAttributes(NodeAttribute* Attrib, List* OldAttrs)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/1/95
	Inputs:		Attrib:		-
					
				OldAttrs:	-

	Purpose:	-

	SeeAlso:	AttributeManager::AttributeSelected; AttributeManager::ApplyAttribToNode

********************************************************************************************/

void AttributeManager::ReplaceAttributes(NodeAttribute* Attrib, List* OldAttrs)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Obtain a pointer to the op descriptor for the attribute operation 
	OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpReplaceAttributes));

	// Get the number of selected fill control points, 'cus we may want to do
	// something different depending on how many their are.
	AttrFillGeometry::SelectionCount = AttrFillGeometry::CountSelectionControlPoints();

	UseLastAttrApplied = FALSE;

	// Invoke the operation, passing Attrib as a parameter
	ReplaceAttributesParam param(Attrib, OldAttrs);
	OpDesc->Invoke(&param);

	if (SendMessages)
	{
		// We've probably changed a Colour
		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED)); 
		// and the Attrib may have changed shape or summit.	Who knows ?
		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED)); 
	}
#endif
}

/********************************************************************************************

>	void AttributeManager::ApplyAttribToNode(NodeRenderableInk* InkNode, NodeAttribute* NewAttr)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/1/94
	Inputs:		InkNode: The Ink node whos attribute is to be changed.
					
				NewAttr: The new attribute that is to replace the Ink Nodes expisting one.
				 
	Purpose:	Replaces an attribute within a specified Ink Node.

	SeeAlso:	AttributeManager::AttributeSelected; AttributeManager::ReplaceAttributes

********************************************************************************************/

void AttributeManager::ApplyAttribToNode(NodeRenderableInk* InkNode, NodeAttribute* NewAttr)
{
	// need to check the case where the selected node
	
	ApplyAttribToNodeHelper(InkNode, NewAttr);

	// We only need to tell everyone if the node is within the selection
	if (InkNode != NULL && (InkNode->IsSelected() || InkNode->IsChildOfSelected()) && SendMessages)
	{
		// Inform the selection that attributes have changed
		SelRange* Sel = GetApplication()->FindSelection();
		if (Sel) 
			Sel->AttrsHaveChanged(); 

		if (NewAttr->IsKindOf(CC_RUNTIME_CLASS(AttrColourChange)) )
		{
			// If the Attrib was a Colour Mutator, then we must have changed a colour
			// so lets tell someone about it
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED)); 
		}
		else
		{
			// We've probably changed a Colour
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED)); 
			// and the Attrib may have changed shape or summit.	Who knows ?
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED)); 

			if (Sel) Sel->UpdateBounds();
		}

		// And ensure that if we've caused the SelRange to set up any delayed message
		// broadcasts, that it sends them now rather than at some random time in the future
		// when some Op just happens to end.
		if (Sel)
			Sel->BroadcastAnyPendingMessages();
	}


	delete NewAttr;
}


/********************************************************************************************

>	void AttributeManager::ApplyAttribToNodeHelper(NodeRenderableInk* InkNode, NodeAttribute* NewAttr)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/1/94
	Inputs:		InkNode: The Ink node whos attribute is to be changed.
					
				NewAttr: The new attribute that is to replace the Ink Nodes expisting one.
				 
	Purpose:	Replaces an attribute within a specified Ink Node.

	Scope:		private

	SeeAlso:	AttributeManager::AttributeSelected; AttributeManager::ReplaceAttributes

********************************************************************************************/

void AttributeManager::ApplyAttribToNodeHelper(NodeRenderableInk* InkNode, NodeAttribute* NewAttr)
{
#if !defined(EXCLUDE_FROM_RALPH)
	BOOL ApplyAttribute = FALSE; 
	List AttrGroupList;  

	pLastNodeAppliedTo = NULL;

	if (InkNode != NULL && NewAttr != NULL)
	{
		ApplyAttribute = CanBeAppliedToNode(InkNode, NewAttr, &AttrGroupList);
	}

	if (ApplyAttribute)
	{
		// Obtain a pointer to the op descriptor for the attribute operation 
		OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpApplyAttribToNode));

		// Get the number of selected fill control points, 'cus we may want to do
		// something different depending on how many their are.

#ifdef BUILDSHADOWS		
		// New check which will only be done in BUILDSHADOWS
		if(InkNode->IS_KIND_OF(NodeShadow))
		{
			AttrFillGeometry::SelectionCount = 1;
		}
		else
#endif
		{
			AttrFillGeometry::SelectionCount = AttrFillGeometry::CountSelectionControlPoints();
		}

// Removed the following section as we can do this now
/*		if (IS_A(NewAttr, AttrColourDrop))
		{
			if (IS_A (InkNode, NodeBitmap))
			{
				// Ask the user wether or not they want to use a 24bit copy of the BMP or use the Default Bitmap?
				//InformWarning(_R(IDS_BFX_BMP_CONVERT_MSG),_R(IDS_OK),0,0,0,1,2);

				NodeBitmap* pBitmap = (NodeBitmap*) InkNode;

				if (pBitmap->GetBitmap ()->ActualBitmap->GetBitmapInfoHeader ()->biBitCount == 32)
				{
					// we don't want the code to fire if were trying to apply a nocolour attribute ....
						
					BOOL isNoColour = FALSE;

					isNoColour = (((FlatFillAttribute*) NewAttr->GetAttributeValue ())->GetStartColour ()->IsTransparent ());

					if (!isNoColour)
					{
						// Load and build the question text.
						String_256 QueryString(_R(IDS_QUERYTRANSP322));
											
						// The only way of bringing up a box with a string in it
						Error::SetError(0, QueryString, 0);
						INT32 DlgResult = InformMessage(NULL, _R(IDS_YES), _R(IDS_NO));
						Error::ClearError();

						switch (DlgResult)
						{
							case 2:				// NO
								return;			// break out of this stuff!
						}
					}
				}
			}
		}
*/
		// Invoke the operation, passing Attrib as a parameter
		OpParam tempParam((void *)InkNode,(void *)NewAttr);
		OpDesc->Invoke(&tempParam);
	}
	
	Document* CurrentDoc = Document::GetSelected();
	if (CurrentDoc == NULL)
		return; // We are not going to be able to do anything if there is no document

	AttributeManager& AttrMgr = CurrentDoc->GetAttributeMgr();

	if (AttrMgr.WeShouldMakeAttrCurrent(ApplyAttribute, NewAttr, &AttrGroupList, 
										TRUE, // Drag n' drop
										InkNode == NULL)) // Dropped onto page
	{
		UpdateCurrentAttr(NewAttr, NewAttr->IsAFillAttr(), &AttrGroupList);

		// Some Attributes require a second attribute to be changed as well.
		BOOL IsMutate;
		NodeAttribute* pOther = AttributeManager::GetOtherAttrToApply(NewAttr, &IsMutate);
	
		if (pOther != NULL)
		{
			UpdateCurrentAttr(pOther, IsMutate, &AttrGroupList); // can't have a NULL AttrGroupList
																 // or it will apply to tools list
		}

		if (pOther)
		{
			delete pOther;
		}
	}

	// Don't forget the AttrGroupList
	AttrGroupList.DeleteAll();
#endif
}

/********************************************************************************************

>	void AttributeManager::UpdateCurrentAttr(NodeAttribute* Attr, BOOL Mutate, 
											 List* pAttrGroupList, 
											 BOOL TellWorld = TRUE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>, Simon
	Created:	28/1/94

	Inputs:		Attr: The new current attr (or mutator to apply to make current).

				Mutate: TRUE if Attr is a mutator.

				pAttrGroupList:A list of attribute groups that the attribute should 
							   be added to. If this list is empty then the attribute
							   will be added to the attribute group associated with the
							   current tool.The List should contain AttributeGroupItems.

				TellWorld: Broadcast a CurrentAttrChangedMsg


	Purpose:	Adds Attr to all attribute groups in the AttrGroupList. If the list is
				empty then the attribute gets added to the attribute group associated 
				with the selected tool.

				The CanBeAppliedToSelection	routine outputs the AttrGroupList which should
				be passed to this routine.



	SeeAlso:	AttributeManager::CanBeAppliedToSelection

********************************************************************************************/

void AttributeManager::UpdateCurrentAttr(NodeAttribute* Attr, 
										 BOOL Mutate, 
										 List* pAttrGroupList, 
										 BOOL TellWorld /* = TRUE */)
{
	// Disabled until Will updates attrmgr
	//ERROR3IF(pAttrGroupList == NULL, "The Attribute group list is not an optional parameter");

	if (UseLastAttrApplied)
	{
		// Have we applied an Attribute ?
		if (pLastNodeAppliedTo != NULL)
		{			
			NodeAttribute* pLastAttrApplied;
			
			// Get the attribute applied to the last node we did something to
			if (!pLastNodeAppliedTo->FindAppliedAttribute(Attr->GetAttributeType(), &pLastAttrApplied))
			{
			 	return;
			}
		
			// Make a copy of the Attribute
			Attr = (NodeAttribute*)pLastAttrApplied->SimpleCopy();
			if (Attr == NULL)
				return;

			// We need to set the bounds for fill attributes
			if (Attr->IsAFillAttr())
			{
				DocRect Bounds = pLastNodeAppliedTo->GetBoundingRect();
				((AttrFillGeometry*)Attr)->SetBoundingRect(Bounds);
			}

			if (Attr->IsAFractalFill())
			{
				// The current attr, should always use the default DPI for fractals.
			 	((AttrFillGeometry*)Attr)->SetFractalDPI(AttrFillGeometry::FractalDPI);
			}

			Mutate = FALSE;
		}
	}

	Document* CurrentDoc = Document::GetSelected();

	if (CurrentDoc == NULL)
		return; // We are not going to be able to do anything if there is no document

	NodeAttribute* CurrentAttrib = Attr;

	// Find the attribute manager
	AttributeManager& AttrMgr = CurrentDoc->GetAttributeMgr();
	
	AttributeGroupItem* pAttributeGroupItem = NULL;	// Will remain NULL if there is no List
		
	if (pAttrGroupList != NULL)	  // Defensive coding, should have been trapped at start of routine
	{
		pAttributeGroupItem	= (AttributeGroupItem*) pAttrGroupList->GetHead();
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Obtain the first attribute group to add to

	CCRuntimeClass* CurrentAttribGroup; // The attribute group we are currently applying to.

	if (pAttributeGroupItem == NULL) 
	{
		// empty list or no list case

		// No attribute groups have been specified. Apply the attribute to the group 
		// associated with the selected tool.
	
		CurrentAttribGroup = 
 			Tool::GetCurrent()->Parent->m_ToolInfo.CurrentAttributeGroup;
	}
	else
	{
		// List has at least one item
		CurrentAttribGroup = pAttributeGroupItem->AttrGroup;
		ERROR3IF(CurrentAttribGroup == NULL, "Illegal NULL attribute group specified"); 
	}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Add the attribute to all neccessary attribute groups 
	do
	{
		if (Mutate)
		{
			// Special Mutate handling
			ERROR3IF(!(Attr->IS_KIND_OF(AttrFillGeometry)), "Mutator is not a AttrFillGeometry"); 
			
			// Get the current Fill Attribute of the same type as the Mutator
			AttrFillGeometry* CurrentAttr = (AttrFillGeometry*)AttrMgr
												.GetCurrentAttribute(CurrentAttribGroup, 
													Attr->GetAttributeType());

			// Mutate it into the new type
			CurrentAttrib = (AttrFillGeometry*)CurrentAttr->Mutate((AttrFillGeometry*)Attr);

			if (CurrentAttrib)
			{
				// Make sure the mutated attr has a valid attr bounds so it can be
				// transformed to new objects that are created
			 	DocRect Bounds = ((AttrFillGeometry*)Attr)->GetBoundingRect();
			 	((AttrFillGeometry*)CurrentAttrib)->SetBoundingRect(Bounds);
			}
		
			if (CurrentAttrib && (*CurrentAttrib == *CurrentAttr))
			{
				delete CurrentAttrib;
				CurrentAttrib = NULL;
			}
		}

		if (CurrentAttrib)	// NULL is not an error case
		{
			// Create a copy of the CurrentAttrib, as it may be added to many groups
			NodeAttribute* pAttrToApply = (NodeAttribute*)(CurrentAttrib->SimpleCopy());
			if (pAttrToApply == NULL)
			{
				return; // get outta here
			} 

			// Update the attribute group
			AttrMgr.UpdateCurrentAttribute(CurrentAttribGroup, pAttrToApply);

			if (Mutate)
				delete CurrentAttrib;
		}
		
		// Try to find the new AttributeGroup
		CurrentAttribGroup = NULL;
		if (pAttributeGroupItem)
		{
			pAttributeGroupItem = (AttributeGroupItem*)pAttrGroupList->GetNext(pAttributeGroupItem);
			if (pAttributeGroupItem)
			{
				CurrentAttribGroup = pAttributeGroupItem->AttrGroup;
				ERROR3IF(CurrentAttribGroup == NULL, "Illegal NULL attribute group specified"); 
			}
		}
	
	} while  (CurrentAttribGroup != NULL);

	if (UseLastAttrApplied && pLastNodeAppliedTo != NULL)
	{
		// Delete the copy we made
		delete Attr;
	}
}


/********************************************************************************************

>	NodeAttribute* AttributeManager::GetOtherAttrToApply(NodeAttribute* AttrApplied, 
															BOOL* IsMutate)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/1/94
	Inputs:		AttrApplied: The Ink node whos attribute is to be changed.
	Outputs:	IsMutate, is updated to indicate if the secondary attribute is a mutator.
	Returns:	The secondary attribute to apply or NULL if none needed.
	Purpose:	Some attribute changes require another attribute to be changed at the
				same time.  This routine checks for and returns any secondary attribute
				that need to be changed

	SeeAlso:	AttributeManager::AttributeSelected; AttributeManager::ReplaceAttributes

********************************************************************************************/

NodeAttribute* AttributeManager::GetOtherAttrToApply(NodeAttribute* AttrApplied, 
														BOOL* IsMutate)
{
	// Lets just ask the object
	return AttrApplied->GetOtherAttrToApply(IsMutate);

/*
	NodeAttribute* OtherAttr = NULL;

	// Stroke Colour Change
	if (AttrApplied->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrStrokeColourChange))
	{
		// A stroke colour change also needs to set the Stroke Transparency

		OtherAttr = new AttrStrokeTranspChange;
		if (OtherAttr == NULL)
			return NULL;

		UINT32 Transp;

		DocColour Col = *((AttrFillGeometry*)AttrApplied)->GetStartColour();

		if (Col.IsTransparent())
			Transp = 255;
		else
			Transp = 0;

		((AttrStrokeTranspChange *)OtherAttr)->SetStartTransp(&Transp);

		*IsMutate = TRUE;
	}

	// Transparency Change
	if (AttrApplied->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrTranspChange))
	{
		// A transparency change also needs to set the Stroke Transparency

		if (AttrFillGeometry::FillSelectionCount() > 0)
		return NULL; 	// Only set line transparency if no control
						// points are selected

		OtherAttr = new AttrStrokeTranspChange;
		if (OtherAttr == NULL)
			return NULL;

		UINT32 Transp = *((AttrFillGeometry*)AttrApplied)->GetStartTransp();

		((AttrStrokeTranspChange *)OtherAttr)->SetStartTransp(&Transp);

		*IsMutate = TRUE;
	}

	// Make Flat Transparency
	if (AttrApplied->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrMakeFlatTransp))
	{
		// A transparency change also needs to set the Stroke Transparency
		OtherAttr = new AttrStrokeTranspChange;
		if (OtherAttr == NULL)
			return NULL;

		UINT32 Transp = 128;

		((AttrStrokeTranspChange *)OtherAttr)->SetStartTransp(&Transp);

		*IsMutate = TRUE;
	}

	// Fractal Fill
	if (AttrApplied->IsAFractalFill() )
	{
		// A Fractal fill change also needs to set the Fill Mapping

		BOOL Tileable = ((AttrFillGeometry*)AttrApplied)->GetTileable();

		if (((AttrFillGeometry*)AttrApplied)->GetAttributeType() == 
				CC_RUNTIME_CLASS(AttrFillGeometry))
		{
			OtherAttr = new AttrFillMappingLinear;
		}
		else
		{
			OtherAttr = new AttrTranspFillMappingLinear;
		}

		if (OtherAttr == NULL)
			return NULL;

		if (Tileable)
			((AttrFillMappingLinear*)OtherAttr)->SetRepeat(RT_Repeating);
		else
			((AttrFillMappingLinear*)OtherAttr)->SetRepeat(RT_RepeatInverted);

		*IsMutate = FALSE;
	}

	// Fill Mapping Change
	if (AttrApplied->GetAttributeType() == CC_RUNTIME_CLASS(AttrFillMapping) ||
		AttrApplied->GetAttributeType() == CC_RUNTIME_CLASS(AttrTranspFillMapping))
	{
		// If the fill mapping is changing, then we must make sure that
		// any fractal tileable flags are updated
		
		OtherAttr = new AttrFractalTileableChange;
		if (OtherAttr == NULL)
			return NULL;

		if (AttrApplied->GetAttributeType() == CC_RUNTIME_CLASS(AttrFillMapping))
		{
			((AttrValueChange*)OtherAttr)->MutateColourFills(TRUE);
		}
		else
		{
			((AttrValueChange*)OtherAttr)->MutateTranspFills(TRUE);
		}

		INT32 Repeat = ((AttrFillMapping*)AttrApplied)->GetRepeat();
		BOOL Tile;

		switch (Repeat)
		{
			case RT_Simple:
				Tile = FALSE;
				break;

			case RT_Repeating:
				Tile = TRUE;
				break;

			case RT_RepeatInverted:
				Tile = FALSE;
				break;

			default:
				Tile = TRUE;
				break;
		}

		((AttrValueChange*)OtherAttr)->SetTileable(Tile);

		*IsMutate = TRUE;
	}

	// Transparency Type Change
	if (AttrApplied->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrTranspTypeChange))
	{
		// A transparency type change also needs to set the Stroke Transparency	type

		OtherAttr = new AttrStrokeTranspTypeChange;
		if (OtherAttr == NULL)
			return NULL;

		UINT32 TranspType = ((AttrFillGeometry*)AttrApplied)->GetTranspType();

		((AttrStrokeTranspTypeChange *)OtherAttr)->SetTranspType(TranspType);

		*IsMutate = TRUE;
	}

	// Remove Transparency also need to remove line transparency
	if (AttrApplied->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrRemoveTransp))
	{
		OtherAttr = new AttrRemoveStrokeTransp;
		if (OtherAttr == NULL)
			return NULL;

		*IsMutate = TRUE;
	}

	return OtherAttr;
*/
}




/********************************************************************************************

>	BOOL AttributeManager::WriteCurrentAttributes(BaseCamelotFilter* pFilter)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/03/2004
	Inputs:		-
	Purpose:	-
	Errors:		-

********************************************************************************************/
BOOL AttributeManager::WriteCurrentAttributes(BaseCamelotFilter* pFilter)
{
	BOOL ok = TRUE;
	AttributeGroup* CurrentGrp;

	// In all Attributes groups...
	CurrentGrp = (AttributeGroup*)AttrGroupList.GetHead();

	while (ok && CurrentGrp != NULL)
	{	
		if (ok) ok = WriteCurrentAttributeGroupRecord(pFilter, CurrentGrp);
		if (ok) ok = pFilter->WriteZeroSizedRecord(TAG_DOWN);

		// In this group, find all attributes
		NodeAttribute* CurrentAttrib = CurrentGrp->AttrListHd;
		while (ok && CurrentAttrib != NULL)
		{
			// Only write out those currents that differ from the fixed defaults...
			BOOL bWriteAttr = TRUE;		// If in doubt, write it out...
			NodeAttribute* pDefaultAttr = NULL;
			AttrIndex attrid = CurrentAttrib->GetAttributeIndex();

			if (attrid!=ATTR_BAD_ID && attrid<ATTR_FIRST_FREE_ID)
				pDefaultAttr = GetDefaultAttribute(attrid);

			if (pDefaultAttr!=NULL && *CurrentAttrib == *pDefaultAttr)
				// Found a default value and it was the same as the current value
				// So don't write it out
				bWriteAttr = FALSE;

			if (bWriteAttr)
			{
				// Write out the attribute to the filter
				CurrentAttrib->WritePreChildrenNative(pFilter);
				CurrentAttrib->WritePostChildrenNative(pFilter);
				if (CurrentAttrib->IsKindOf(CC_RUNTIME_CLASS(AttrFillGeometry)))
					((AttrFillGeometry*)CurrentAttrib)->WriteBoundingRect(pFilter);
			}

			if (pDefaultAttr)
				delete pDefaultAttr;

			// Try the next attribute in the group
			CurrentAttrib = (NodeAttribute*)CurrentAttrib->FindNext();
		}

		if (ok) ok = pFilter->WriteZeroSizedRecord(TAG_UP);

		//Find the next attribute group
		CurrentGrp = (AttributeGroup *) AttrGroupList.GetNext(CurrentGrp);
	}

	return ok;
}




/********************************************************************************************

>	BOOL AttributeManager::WriteCurrentAttributeGroupRecord(BaseCamelotFilter* pFilter, AttributeGroup* pGroup)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/2004
	Inputs:		-
	Purpose:	-
	Errors:		-

********************************************************************************************/
BOOL AttributeManager::WriteCurrentAttributeGroupRecord(BaseCamelotFilter* pFilter, AttributeGroup* pGroup)
{
	BOOL ok = TRUE;

	CXaraFileRecord Rec(TAG_CURRENTATTRIBUTES, TAG_CURRENTATTRIBUTES_SIZE);

	ok = Rec.Init();

	if (pGroup->AttrGroup == CC_RUNTIME_CLASS(NodeRenderableInk))
	{
		if (ok) ok = Rec.WriteBYTE(ATTRIBUTEGROUP_INK);	// write out 1 for main Ink group
	}
	else if (pGroup->AttrGroup == CC_RUNTIME_CLASS(BaseTextClass))
	{
		if (ok) ok = Rec.WriteBYTE(ATTRIBUTEGROUP_TEXT);	// write out 2 for text group
	}
	else
	{
		ERROR3("Unknown Attribute Group");
		ok = FALSE;
	}

	// Finally, write the record out to file
	// In the process get the record number that this was written out as
	INT32 RecordNumber = 0L;
	if (ok) RecordNumber = pFilter->Write(&Rec);
	
	// If we have had a problem at any of the stages then return that to the caller
	return (ok && RecordNumber > 0);
}



	
// CommonAttributeItem methods

/********************************************************************************************

>	BOOL CommonAttributeItem::InitSafeAttrItem(CCRuntimeClass* AtrType, 
											   NodeAttribute* pAttrib=NULL, 
						  					   Range::CommonAttribResult status = Range::ATTR_NONE); 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/95
	Inputs:		AttrType: The attributes type
				pAttrib:  Attribute pointer
				status:	  Status

	Outputs:	-
	Returns:	FALSE if we run out of memory. The AttributeItem's values will remain unchanged
				in this circumstance.

	Purpose:	Initialises a CommonAttributeItem. If pAttrib is non NULL then a copy of the 
				item is stored. This makes the function useful for caching 'safe' attribute
				details.
	SeeAlso:	CommonAttrSet

********************************************************************************************/


BOOL CommonAttributeItem::InitSafeAttrItem(CCRuntimeClass* AtrType, NodeAttribute* pAttrib, 
						  					Range::CommonAttribResult status)
{
	if (pAttrib)
	{
		// Try to make a copy of the attribute
		NodeAttribute* pAttrCopy = (NodeAttribute*) (pAttrib->SimpleCopy());
		if (!pAttrCopy)
			return FALSE;
		pAttr = pAttrCopy; 		
	}
	else
		pAttr = NULL;
		
 	AttrType = AtrType; 
 	Status = status; 
 	pAttrIsCopy = TRUE; // Indicate that a copy of the attribute is stored
	return TRUE;
} 

/********************************************************************************************

>	CommonAttributeItem::~CommonAttributeItem();  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Purpose:	CommonAttributeItem destructor

********************************************************************************************/

CommonAttributeItem::~CommonAttributeItem()  
{
	if (pAttrIsCopy && pAttr) 
		delete pAttr;
}; 
											   
