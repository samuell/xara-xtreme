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
// Encapsulate the kernel'ly bits of the rulers

/*
*/

#include "camtypes.h"

// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER

//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "rulers.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "sprdmsg.h"
#include "optsmsgs.h"
#include "userrect.h"
#include "grid.h"
#include "appprefs.h"
#include "layerprp.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_MEMDUMP(RulerPair, MessageHandler)
CC_IMPLEMENT_MEMDUMP(RulerBase, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(HorizontalRuler, RulerBase)
CC_IMPLEMENT_MEMDUMP(VerticalRuler,   RulerBase)
CC_IMPLEMENT_MEMDUMP(RulerContextMenu,  ContextMenu)
CC_IMPLEMENT_MEMDUMP(OriginContextMenu, ContextMenu)

#define new CAM_DEBUG_NEW

#define MAX_LEGEND_CHARS       12
#define LABEL_SPACING_STRING "8888"
#define ACCEPTIBLE_DEVIATION_FROM_INTEGER 1e-10
#define NEAR_ENOUGH_INTEGER(a) ( fabs(fmod(a,1)) < ACCEPTIBLE_DEVIATION_FROM_INTEGER )
#define MAKE_INTEGER(a)        (     (INT32)( (a) + ACCEPTIBLE_DEVIATION_FROM_INTEGER*3 ) )

//////////////////////////////////////////////////////////////////////////////
// RulerBase class

/*****************************************************************************
>	RulerBase::RulerBase()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/95
	Purpose:	Constructor for RulerBase class
*****************************************************************************/

RulerBase::RulerBase()
{
}


/*****************************************************************************
>	BOOL RulerBase::Redraw(OilRect* pUpdateOilRect)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/95
	Inputs:		pUpdateOilRect - only lox,hix used to determine start/end of portion
						         of the ruler to be redrawn for horizontal ruler
								 and loy,hiy for vertical ruler
	Returns:	FALSE if fails
	Purpose:	Redraw a region of a ruler
*****************************************************************************/

BOOL RulerBase::Redraw(OilRect* pUpdateOilRect)
{
	// get some useful pointers used later
	DocView*         pDocView = pRulerPair->GetpDocView();
	RulerRedrawData* pRD      = pRulerPair->GetpRedrawData();
	Spread*          pSpread  = pRulerPair->GetpSpread();

	// if no spread visible in doc view, just exit
	if (pSpread==NULL)
		return TRUE;

	// Convert the OilRect to a UserRect and limit to spread's paste board
	UserRect UpdateRect = pUpdateOilRect->ToDoc(pSpread,pDocView).ToSpread(pSpread,pDocView).ToUser(pSpread);
	MILLIPOINT LoLimit = GetOrd(pRD->PasteBoardUserRect.lo);
	MILLIPOINT HiLimit = GetOrd(pRD->PasteBoardUserRect.hi);
	if (GetOrd(UpdateRect.lo)<LoLimit) UpdateRect.lo=MakeCoord(LoLimit);
	if (GetOrd(UpdateRect.hi)>HiLimit) UpdateRect.hi=MakeCoord(HiLimit);
	if (GetOrd(UpdateRect.hi)<GetOrd(UpdateRect.lo))
		return TRUE;	// no region to redraw

	// find the first and last major graticules to be rendered
	double Start = GetOrd(UpdateRect.lo)/pRD->GratStepSize;
	double End   = GetOrd(UpdateRect.hi)/pRD->GratStepSize;
	INT32 FirstGrat = (INT32)floor(Start)*pRD->GratStep;
	INT32 LastGrat  = (INT32)ceil(End)*pRD->GratStep;

	// plot each major graticule from the first to the last
	String_8 GratLabelText("");
	INT32 Grat=FirstGrat;
	while (Grat<=LastGrat)
	{
		// calc position of graticule and convert to OilCoords
		MILLIPOINT GratPos=(MILLIPOINT)(Grat*pRD->GratUnitSize);
		OilCoord GratOilPos=MakeCoord(GratPos).ToSpread(pSpread).ToOil(pSpread,pDocView);

		// draw major graticule
		ERROR3IF(!NEAR_ENOUGH_INTEGER(fabs(Grat*pRD->ScaleFactor)),"RulerBase::Redraw() - Grat label should be integer!");
		INT32 GratLabel=MAKE_INTEGER(fabs(Grat*pRD->ScaleFactor));
		Convert::LongToString(GratLabel,&GratLabelText);
		pOILRuler->DrawMajorGraticule(GratOilPos,&GratLabelText);

		// draw minor grats
		INT32       MinorGrats   = pRD->MinorGrats;
		INT32       MinorGrat    = 1;
		MILLIPOINT MinorGratPos = 0;
		while (MinorGrat<MinorGrats)
		{
			INT32 Size=0;
			if (MinorGrat*2==MinorGrats) Size+=1;
			if (MinorGrats>5)
				if (MinorGrat*4==MinorGrats || MinorGrat*2==MinorGrats || MinorGrat*4==MinorGrats*3) Size+=2;
			MinorGratPos = (MILLIPOINT)(GratPos+pRD->MinorGratStepSize*MinorGrat);
			GratOilPos   = MakeCoord(MinorGratPos).ToSpread(pSpread).ToOil(pSpread,pDocView);
			pOILRuler->DrawMinorGraticule(GratOilPos,Size);
			MinorGrat+=1;
		}

		// move on to next graticule
		Grat+=pRD->GratStep;
	}

	// draw the mouse follower on this ruler in it's last drawn position
	DocCoord LastPos    = pRulerPair->GetMouseFollowerPosition();
	BOOL     Visibility = pRulerPair->GetMouseFollowerVisibility();

	if (Visibility==TRUE)
	{
		OilCoord LastOilPos = LastPos.ToOil(pSpread,pDocView);
		pOILRuler->PaintMouseFollower(LastOilPos, pDocView, RenderOn);
	
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
// HorizontalRuler class

/*****************************************************************************
>	HorizontalRuler::HorizontalRuler()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/95
	Purpose:	Constructor for HorizontalRuler
*****************************************************************************/

HorizontalRuler::HorizontalRuler()
{
	pOILRuler  = NULL;
	pRulerPair = NULL;
}


/*****************************************************************************
>	BOOL HorizontalRuler::Init(RulerPair* pRulers, OILHorizontalRuler* pOILRuler)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/95
	Inputs:		pRulers   - pointer to parent RulerPair object
				pOILRuler - pointer to the associated WinOil ruler class
	Returns:	FALSE if fails
	Purpose:	Init a HorizontalRuler
*****************************************************************************/

BOOL HorizontalRuler::Init(RulerPair* pRulers, OILHorizontalRuler* pHOILRuler)
{
	ERROR2IF(pHOILRuler==NULL,FALSE,"HorizontalRuler::Init() - pVOILRuler==NULL");
	ERROR2IF(   pRulers==NULL,FALSE,"HorizontalRuler::Init() - pRulers==NULL");

	pOILRuler  = pHOILRuler;
	pRulerPair = pRulers;

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
// VerticalRuler class

/*****************************************************************************
>	VerticalRuler::VerticalRuler()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/95
	Purpose:	Constructor for VerticalRuler
*****************************************************************************/

VerticalRuler::VerticalRuler()
{
	pOILRuler  = NULL;
	pRulerPair = NULL;
}


/*****************************************************************************
>	BOOL VerticalRuler::Init(RulerPair* pRulers, OILVerticalRuler* pOILRuler)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/95
	Inputs:		pRulers   - pointer to parent RulerPair object
				pOILRuler - pointer to the associated WinOil ruler class
	Returns:	FALSE if fails
	Purpose:	Init a VerticalRuler
*****************************************************************************/

BOOL VerticalRuler::Init(RulerPair* pRulers, OILVerticalRuler* pVOILRuler)
{
	ERROR2IF(pVOILRuler==NULL,FALSE,"VerticalRuler::Init() - pVOILRuler==NULL");
	ERROR2IF(   pRulers==NULL,FALSE,"VerticalRuler::Init() - pRulers==NULL");

	pOILRuler  = pVOILRuler;
	pRulerPair = pRulers;

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
// RulerPair class

/*****************************************************************************
>	RulerPair::RulerPair()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/95
	Purpose:	Constructor for RulerPair
*****************************************************************************/

RulerPair::RulerPair()
{
	pSpread  = NULL;
	pDocView = NULL;

	MouseFollowerVisibility = FALSE;
}


/*****************************************************************************
>	BOOL RulerPair::Init(DocView* pDView, OILHorizontalRuler* pOILHRuler,
					 	 OILVerticalRuler* pOILVRuler, OriginGadget* pOGadget)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/95
	Inputs:		pDView     - pointer to associated doc view
				pOILHRuler - pointer to associated WinOil ruler class
				pOILVRuler - pointer to associated WinOil ruler class
				pOGadget   - pointer to associated WinOil origin gadget
	Returns:	FALSE if fails
	Purpose:	Init the RulerPair
*****************************************************************************/

BOOL RulerPair::Init(DocView* pDView, OILHorizontalRuler* pOILHRuler,
					 OILVerticalRuler* pOILVRuler, OriginGadget* pOGadget)
{
	ERROR2IF(    pDView==NULL,FALSE,"RulerPair::Init() - pDocView==NULL");
	ERROR2IF(pOILHRuler==NULL,FALSE,"RulerPair::Init() - pOILHRuler==NULL");
	ERROR2IF(pOILVRuler==NULL,FALSE,"RulerPair::Init() - pOILVRuler==NULL");
	ERROR2IF(  pOGadget==NULL,FALSE,"RulerPair::Init() - pOGadget==NULL");

	HRuler.Init(this,pOILHRuler);
	VRuler.Init(this,pOILVRuler);
	pDocView      = pDView;
	pOriginGadget = pOGadget;

	return TRUE;
}


/*****************************************************************************
>	BOOL RulerPair::UpdateRedrawData()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/9/95
	Returns:	FALSE if fails
	Purpose:	Recache semi-permenent data associated with redrawing the rulers
				ie values which only change on change of page/grid/zoom/spread etc
*****************************************************************************/

extern BOOL SafeAppendString(StringBase*, const StringBase&, BOOL = TRUE);	// yuk - thanks Justin!

BOOL RulerPair::UpdateRedrawData()
{
	ERROR2IF( pSpread==NULL,FALSE,"RulerPair::UpdateRedrawData() - pSpread==NULL");
	ERROR2IF(pDocView==NULL,FALSE,"RulerPair::UpdateRedrawData() - pDocView==NULL");

	// get a pointer to the default grid associated with this spread
	NodeGrid* pDefaultGrid=pSpread->FindFirstDefaultGridInSpread();
	ERROR2IF(pDefaultGrid==NULL,FALSE,"RulerPair::UpdateRedrawData() - pDefaultGrid==NULL");

	// get pointer to the document's unit list
	DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
	ERROR2IF(pDocUnitList==NULL,FALSE,"RulerPair::UpdateRedrawData() - pDocUnitList==NULL");

	// determine the minimum distance between minor graticules (in Doc/Spread/UserCoords)
	OilRect MinGratSpacingOilRect;
	BOOL ok = OILRuler::GetMinGraticuleSpacing(&MinGratSpacingOilRect,pDocView);
	ERROR2IF(!ok,FALSE,"RulerPair::UpdateRedrawData() - OILRuler::GetMinGraticuleSpacing() failed");
	DocRect    MinGratSpacingDocRect = MinGratSpacingOilRect.ToDoc(pSpread,pDocView);
	MILLIPOINT MinGratSpacing        = max(MinGratSpacingDocRect.Width(),MinGratSpacingDocRect.Height());

	// determine the minimum distance between major graticules (in Doc/Spread/UserCoords)
	OilRect LabelSizeOilRect;
	ok = OILRuler::GetTextSize(&LabelSizeOilRect,&String_8(LABEL_SPACING_STRING),pDocView);
	ERROR2IF(!ok,FALSE,"RulerPair::UpdateRedrawData() - OILRuler::GetTextSize() failed");
	DocRect    LabelSizeDocRect = LabelSizeOilRect.ToDoc(pSpread,pDocView);
	MILLIPOINT MinLabelSpacing  = max(LabelSizeDocRect.Width(),LabelSizeDocRect.Height());

	// read the grid settings
	UnitType GridUnits           = pDefaultGrid->GetUnits();
	INT32     GridSubDivisions    = pDefaultGrid->GetSubdivisions();
	double   GridStepSize        = pDefaultGrid->GetMainStep();
	double   GridStepSizeInUnits = pDefaultGrid->GetDivisions();
	double   GridUnitSize        = GridStepSize/GridStepSizeInUnits;

	// detemine the major graticule label steps and ruler unit size
	// if step not an integer number of units, label in unit multiples of this step (eg 1,2,3; cms x2.54)
	// if step is  an integer number of units, label in the base unit and step by the integer (eg 4,8,12; Inches)
	double GratStepSize = GridStepSize;
	INT32   GratStep     = 1;
	double GratUnitSize = GratStepSize;
	if (NEAR_ENOUGH_INTEGER(GridStepSizeInUnits))
	{
		GratStep     = MAKE_INTEGER(GridStepSizeInUnits);
		GratUnitSize = GridUnitSize;
	}

	// Get pasteboard rect in user coords
	DocRect  PasteSpreadRect = pSpread->GetPasteboardRect(FALSE, pDocView);
	pSpread->DocCoordToSpreadCoord(&PasteSpreadRect);
	UserRect PasteBoardUserRect = PasteSpreadRect.ToUser(pSpread);

	// hence the start and end values in terms of ruler units
	double XStart = fabs(PasteBoardUserRect.lox/GratStepSize)*GratStep;
	double XEnd   = fabs(PasteBoardUserRect.hix/GratStepSize)*GratStep;
	double YStart = fabs(PasteBoardUserRect.loy/GratStepSize)*GratStep;
	double YEnd   = fabs(PasteBoardUserRect.hiy/GratStepSize)*GratStep;

	// hence a scale factor to keep the labels in the range 1..999
	double LogMax  = log10(max(max(XStart,XEnd),max(YStart,YEnd)));
	INT32   MaxDP   = (INT32)floor(LogMax);
	double ScaleFactor = (MaxDP<=2) ? 1 : 1/pow(10,MaxDP-2);

	// determine how often major graticules can be drawn (ie 1 in N, where N is 1eX, 2eX, 5eX)
	// (note: all done accurately with integers and multiplies)
	// tricky problem here - if GratFreq is 5, GratStep is 31 and ScaleFactor 0.1
	// then grat labels could be 400,415.5,431 => 400, 416, 431 ie inaccurate,
	// to prevent this, GratFreq*ScaleFactor*GratStep must be an integer
	INT32 GratFreq    = (INT32)ceil(MinLabelSpacing/GratStepSize);
	INT32 Mult        = 1;
	INT32 SafetyCount = 0;	// by special request from Alex 16/10/95
	while (1)
	{
		if (GratFreq<=1*Mult && NEAR_ENOUGH_INTEGER(1*Mult*GratStep*ScaleFactor)) { GratFreq=1*Mult; break; }
		if (GratFreq<=2*Mult && NEAR_ENOUGH_INTEGER(2*Mult*GratStep*ScaleFactor)) { GratFreq=2*Mult; break; }
		if (GratFreq<=5*Mult && NEAR_ENOUGH_INTEGER(5*Mult*GratStep*ScaleFactor)) { GratFreq=5*Mult; break; }
		Mult *= 10;
		ERROR2IF(SafetyCount++>1000,FALSE,"RulerPair::UpdateRedrawData() - SafetyCount exceeded!");
	}

	// and hence determine distance and label increments between visible major graticules
	double VisibleGratStepSize = GratStepSize*GratFreq;
	INT32   VisibleGratStep     = GratStep    *GratFreq;

	// determine the legend unit multiplier
	String_32 LegendUnitMultiplier("");
	if (GratStep*ScaleFactor!=GridStepSizeInUnits)
	{
		TCHAR mult[4]={' ',(TCHAR)0xD7,' ','\0'};		// ie a space followed by the multiply symbol
		LegendUnitMultiplier = mult;
		String_32 LegendUnitSize("");
		double GratUnit=GridStepSizeInUnits/GratStep;	// ie 1in=>1in, 10in=>1in, 2.54in=>2.54in
		ok=Convert::DoubleToString(GratUnit/ScaleFactor,&LegendUnitSize);
		SafeAppendString(&LegendUnitMultiplier,LegendUnitSize,FALSE);
	}

	// determine the ruler legend units
	String_32 LegendStr = pDocUnitList->GetToken(GridUnits);
	if (LegendStr.Length()+LegendUnitMultiplier.Length() > MAX_LEGEND_CHARS)
		LegendStr = pDocUnitList->GetSpecifier(GridUnits);
	SafeAppendString(&LegendStr,LegendUnitMultiplier,FALSE);

	// calc minor graticule step size, and number of minor graticules
	INT32 MinorGrats = GridSubDivisions;
	if (GratFreq>1)
		MinorGrats = GratFreq;
	double MinorGratStepSize = VisibleGratStepSize/MinorGrats;

	// hence calc visible minor graticules (ie ensure minimum spacing)
	INT32 MinorGratFreq = 1;
	if (MinorGratStepSize<MinGratSpacing)
	{
		// calc MinorGratFreq such that minor grats are not too close and which is a factor of MinorGrats
		MinorGratFreq = (INT32)ceil(MinGratSpacing/MinorGratStepSize);
		while (MinorGratFreq<MinorGrats && fmod((double)MinorGrats,(double)MinorGratFreq)!=0)
			MinorGratFreq+=1;
		if (MinorGratFreq>MinorGrats)
			MinorGratFreq=MinorGrats;
	}
	INT32   VisibleMinorGrats        = MinorGrats/MinorGratFreq;
	double VisibleMinorGratStepSize = VisibleGratStepSize/VisibleMinorGrats;

	// cache the necessary values
	RedrawData.GratUnitSize       = GratUnitSize;
	RedrawData.MinorGrats         = VisibleMinorGrats;
	RedrawData.MinorGratStepSize  = VisibleMinorGratStepSize;
	RedrawData.GratStep           = VisibleGratStep;
	RedrawData.GratStepSize       = VisibleGratStepSize;
	RedrawData.PasteBoardUserRect = PasteBoardUserRect;
	RedrawData.ScaleFactor        = ScaleFactor;
	RedrawData.LegendStr          = LegendStr;

	return TRUE;
}


/*****************************************************************************
>	MsgResult RulerPair::Message(Msg* pMsg)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/95
	Purpose:	handle system messages
	Inputs:		pMsg - pointer to message
	Returns:	OK, EAT_MSG, FAIL (see Errors)
*****************************************************************************/

MsgResult RulerPair::Message(Msg* pMsg)
{
	ERROR2IF(    pMsg==NULL,FAIL,"RulerPair::Message() - pMsg==NULL");
	if (pDocView==NULL)
	{
		// no DocView, so don't process the message
		return OK;
	}
	// get a pointer to the doc associated with this doc view
	Document* pDoc=pDocView->GetDoc();
	ERROR2IF(pDoc==NULL,FAIL,"RulerPair::Message() - pDoc==NULL");
	BOOL ok=TRUE;

	// if default grid changes or page size changes on do or undo/redo in a doc associated
	// with this ruler pair, update the rulers
	if ( MESSAGE_IS_A(pMsg,OptionsChangingMsg) && ((OptionsChangingMsg*)pMsg)->pScopeDoc==pDoc )
	{
		if ( ((OptionsChangingMsg*)pMsg)->State==OptionsChangingMsg::NEWDEFAULTGRID
		  || ((OptionsChangingMsg*)pMsg)->State==OptionsChangingMsg::NEWPAGESIZE
		  || ((OptionsChangingMsg*)pMsg)->State==OptionsChangingMsg::PAGESIZEHASCHANGED
		  || ((OptionsChangingMsg*)pMsg)->State==OptionsChangingMsg::NEWUNITS )
		{
			ok=Update();
		}
	}

	// if a doc has just been created, init it's associated spread and update rulers
	if ( MESSAGE_IS_A(pMsg,DocChangingMsg) && ((DocChangingMsg*)pMsg)->pChangingDoc==pDoc 
	  && ((DocChangingMsg*)pMsg)->State==DocChangingMsg::BORNANDSTABLE )
	{
		pSpread=pDocView->GetVisibleSpread();
		ok=Update();
	}

	// if spread changes and new spread is in the same doc as the one associated with this ruler pair
	// and the spread is differentfrom the previous one associated with this ruler pair
	// then change the spread associated with this ruler pair and update the rulers
	if ( MESSAGE_IS_A(pMsg,SpreadMsg)         && ((SpreadMsg*)pMsg)->Reason==SpreadMsg::SELCHANGED
	  && ((SpreadMsg*)pMsg)->pNewSpread!=NULL && ((SpreadMsg*)pMsg)->pNewSpread->FindOwnerDoc()==pDoc
	  && pSpread!=((SpreadMsg*)pMsg)->pNewSpread )
	{
		pSpread=((SpreadMsg*)pMsg)->pNewSpread;
		ok=Update();
	}

	// if a docview message is sent relating to the doc view associated with this ruler pair
	if ( MESSAGE_IS_A(pMsg,DocViewMsg) && ((DocViewMsg*)pMsg)->pDocView==pDocView )
	{
// this is now handled by a direct call from the zoom ops (Ed/Chris 29/9/95)
//		// if it is a scale changed, just updtae rulers
//		if ( ((DocViewMsg*)pMsg)->State==DocViewMsg::SCALECHANGED )
//			ok=Update();

		// if it is a newly created doc view then initialise the associated spread
		if ( ((DocViewMsg*)pMsg)->State==DocViewMsg::NEWANDSTABLE )
		{
			pSpread=pDocView->GetVisibleSpread();
			ok=Update();
		}
	}

	// if a docview message is sent relating to the doc view associated with this ruler pair
	if ( MESSAGE_IS_A(pMsg,DocViewMsg) && ((DocViewMsg*)pMsg)->State==DocViewMsg::SELCHANGED
		&& ((DocViewMsg*)pMsg)->pOldDocView==pDocView )
	{
		UpdateMouseFollowers(NULL);	// remove mouse followers
	}

	return ok ? OK : FAIL;
}


/*****************************************************************************
>	BOOL RulerPair::Update()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/95
	Returns:	FALSE if fails
	Purpose:	update a ruler pair (recache redraw data and force redraw)
*****************************************************************************/

BOOL RulerPair::Update()
{
	if (pSpread!=NULL)
		UpdateRedrawData();

	OILHorizontalRuler* pHOilRuler=(OILHorizontalRuler*)HRuler.GetpOILRuler();
	ERROR2IF(pHOilRuler==NULL,FALSE,"RulerPair::Update() - pHOilRuler==NULL");
	pHOilRuler->SetLegendText(&RedrawData.LegendStr);
	pHOilRuler->UpdateRuler();

	OILVerticalRuler* pVOilRuler=(OILVerticalRuler*)VRuler.GetpOILRuler();
	ERROR2IF(pVOilRuler==NULL,FALSE,"RulerPair::Update() - pVOilRuler==NULL");
	pVOilRuler->UpdateRuler();

	return TRUE;
}


/*****************************************************************************
>	BOOL RulerPair::UpdateMouseFollowers(DocCoord* pDocCoord)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		pDocCoord - doc coord position of mouse followers 
							if NULL, the followers are removed
	Returns:	FALSE if fails
	Purpose:	Update the mouse followers on the rulers
*****************************************************************************/

BOOL RulerPair::UpdateMouseFollowers(DocCoord* pDocCoord)
{
	ERROR2IF(pDocView==NULL,FALSE,"RulerPair::UpdateMouseFollowers() - pDocView==NULL");
	ERROR2IF( pSpread==NULL,FALSE,"RulerPair::UpdateMouseFollowers() - pSpread==NULL");

	// determine old and new follower visibility and position
	BOOL     NewVisibility = (pDocCoord!=NULL);
	DocCoord NewPos        = pDocCoord!=NULL ? *pDocCoord : DocCoord(0,0);
	DocCoord OldPos        = GetMouseFollowerPosition();
	BOOL     OldVisibility = GetMouseFollowerVisibility();

	// if there is no effective change, just return
	if (NewVisibility==FALSE && OldVisibility==FALSE)
		return TRUE;
	if (NewVisibility==TRUE && OldVisibility==TRUE && NewPos==OldPos)
		return TRUE;

	// get pointers to the oil rulers
	OILHorizontalRuler* pHOILRuler = (OILHorizontalRuler*)HRuler.GetpOILRuler();
	OILVerticalRuler*   pVOILRuler =   (OILVerticalRuler*)VRuler.GetpOILRuler();
	ERROR2IF(pHOILRuler==NULL,FALSE,"RulerPair::UpdateMouseFollowers() - pOILHRuler==NULL");
	ERROR2IF(pVOILRuler==NULL,FALSE,"RulerPair::UpdateMouseFollowers() - pOILVRuler==NULL");

	// remove old followers if there were any
	if (OldVisibility==TRUE)
	{
		OilCoord OldOilPos = OldPos.ToOil(pSpread,pDocView);
		pHOILRuler->PaintMouseFollower(OldOilPos, pDocView, RenderOff);
		pVOILRuler->PaintMouseFollower(OldOilPos, pDocView, RenderOff);
	}

	// draw on new followers if there were any
	if (NewVisibility==TRUE)
	{
		OilCoord NewOilPos = NewPos.ToOil(pSpread,pDocView);
		pHOILRuler->PaintMouseFollower(NewOilPos, pDocView, RenderOn);
		pVOILRuler->PaintMouseFollower(NewOilPos, pDocView, RenderOn);
	}

	// update the current position of the mouse followers
	MouseFollowerVisibility  = NewVisibility;
	MouseFollowerPosition    = NewPos;

	return TRUE;
}


/******************************************************************************
>	BOOL RulerPair::GetStatusLineText(String_256* pText, CPoint MousePos, HWND hWnd)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		MousePos - position in window
				hWnd     - handle of window
	Outputs:	pText    -
	Returns:	TRUE if ptext hold valid text, else FALSE if not over part of ruler
	Purpose:	if over a component of the rulers, get the associated help text
******************************************************************************/

BOOL RulerPair::GetStatusLineText(String_256* pText, CPoint MousePos, HWND hWnd)
{
	ERROR2IF(pText==NULL,FALSE,"RulerPair::GetStatusLineText() - pText==NULL");

	// get pointers to each component window of the rulers
	OriginGadget*       pOriginGadget = GetpOriginGadget();
	OILVerticalRuler*   pOilVRuler    = (OILVerticalRuler*)  GetpVerticalRuler()->GetpOILRuler();
	OILHorizontalRuler* pOilHRuler    = (OILHorizontalRuler*)GetpHorizontalRuler()->GetpOILRuler();
 	ERROR2IF(pOriginGadget==NULL,FALSE,"RulerPair::GetStatusLineText() - pOriginGadget==NULL");
	ERROR2IF(   pOilVRuler==NULL,FALSE,"RulerPair::GetStatusLineText() - pOilVRuler==NULL");
	ERROR2IF(   pOilHRuler==NULL,FALSE,"RulerPair::GetStatusLineText() - pOilHRuler==NULL");
	LegendLabel* pLegendLabel = pOilHRuler->GetpLegendLabel();
	ERROR2IF(pLegendLabel==NULL,FALSE,"RulerPair::GetStatusLineText() - pLegendLabel==NULL");

	BOOL valid=FALSE;
	if (!valid) valid =    pOilVRuler->GetStatusLineText(pText,MousePos,hWnd);
	if (!valid) valid =    pOilHRuler->GetStatusLineText(pText,MousePos,hWnd);
	if (!valid) valid =  pLegendLabel->GetStatusLineText(pText,MousePos,hWnd);
	if (!valid) valid = pOriginGadget->GetStatusLineText(pText,MousePos,hWnd);

	return valid;
}


///////////////////////////////////////////////////////////////////////////////
// RulerContextMenu

/******************************************************************************
>	BOOL RulerContextMenu::Build()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Returns:	FALSE if fails
	Purpose:	buid a context sensitive menu over a component of the rulers
******************************************************************************/

BOOL RulerContextMenu::Build()
{
	BOOL ok = TRUE;
	ok = ok && BuildCommand(OPTOKEN_DELETEALLGUIDELINES,TRUE);
	ok = ok && BuildCommand(OPTOKEN_NEWGUIDELINEPROPDLG);
	ok = ok && BuildCommand(OPTOKEN_GUIDEPROPERTIESDLG);
	ok = ok && BuildCommand(OPTOKEN_GRIDANDRULERSDLG);

	return ok;
}


///////////////////////////////////////////////////////////////////////////////
// OriginContextMenu

/******************************************************************************
>	BOOL OriginContextMenu::Build()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Returns:	FALSE if fails
	Purpose:	buid a context sensitive menu over a component of the rulers
******************************************************************************/

BOOL OriginContextMenu::Build()
{
	BOOL ok = TRUE;
	ok = ok && BuildCommand(OPTOKEN_RESETSPREADORIGIN,TRUE);
	ok = ok && BuildCommand(OPTOKEN_GRIDANDRULERSDLG);

	return ok;
}

#endif // WEBSTER == 0
