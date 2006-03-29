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
// OpImgSet - Ops for imagesetting options


#include "camtypes.h"

#include "app.h"
#include "mainfrm.h"
#include "attrmgr.h"
//#include "barsdlgs.h"
#include "colcontx.h"
#include "colormgr.h"
#include "docview.h"
#include "isetattr.h"
//#include "isetres.h"
//#include "jason.h"
#include "menuops.h"
#include "opimgset.h"
#include "progress.h"
//#include "simon.h"			// for _R(IDS_NO_OBJECTS_SELECTED)
#include "xaracms.h"
//#include "gerry.h"


CC_IMPLEMENT_DYNCREATE(OpShowPrinterColours, Operation)
//CC_IMPLEMENT_DYNCREATE(OpToggleCompositePreview, Operation)
//CC_IMPLEMENT_DYNCREATE(OpToggleCyanPreview, Operation)
//CC_IMPLEMENT_DYNCREATE(OpToggleMagentaPreview, Operation)
//CC_IMPLEMENT_DYNCREATE(OpToggleYellowPreview, Operation)
//CC_IMPLEMENT_DYNCREATE(OpToggleKeyPreview, Operation)
//CC_IMPLEMENT_DYNCREATE(OpToggleSpotPreview, Operation)
//CC_IMPLEMENT_DYNCREATE(OpToggleMonoPlatePreview, Operation)
CC_IMPLEMENT_DYNCREATE(OpOverprintLine, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpOverprintFill, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpPrintOnAllPlates, SelOperation)


#define new CAM_DEBUG_NEW



/*******************************************************************

>	BOOL ::InitImagesettingOps(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Purpose:	Centralised initialisation of all the imagesetting
				Ops in this file (opimgset.cpp).
				Called from ColourManager::Init()

*******************************************************************/

BOOL InitImagesettingOps(void)
{
	if (!OpShowPrinterColours::Init())
		return(FALSE);
#if FALSE
	if (!OpToggleCompositePreview::Init())
		return(FALSE);

	if (!OpToggleCyanPreview::Init())
		return(FALSE);

	if (!OpToggleMagentaPreview::Init())
		return(FALSE);

	if (!OpToggleYellowPreview::Init())
		return(FALSE);

	if (!OpToggleKeyPreview::Init())
		return(FALSE);

	if (!OpToggleSpotPreview::Init())
		return(FALSE);

	if (!OpToggleMonoPlatePreview::Init())
		return(FALSE);
#endif
	if (!OpOverprintLine::Init())
		return(FALSE);

	if (!OpOverprintFill::Init())
		return(FALSE);

	if (!OpPrintOnAllPlates::Init())
		return(FALSE);

	// Finally, register an OpDescriptor for the "Imagesetting" Submenu item,
	// which just connects up to DummyOp (see menuops.cpp)
	return(Operation::RegisterOpDescriptor(0,
								_R(IDS_IMAGESETTING),
								CC_RUNTIME_CLASS(DummyOp),
								OPTOKEN_IMAGESETTING,
								DummyOp::GetState,
								0,	/* help ID */
								0,  /* bubble help ID */
								0,	/* resource ID */
								0	/* control ID */
							));
}

IndexedColour* FindWhichSpot(INT32 WhichSpot)
{
	INT32 Count = 0;
	
	// Find the required "live" spot colour in the colour list
	ColourList *ColList = Document::GetSelected()->GetIndexedColours();
	IndexedColour *SpotColour = (IndexedColour *) ColList->GetHead();

	while (SpotColour != NULL)
	{
		if (!SpotColour->IsDeleted() && SpotColour->GetType() == COLOURTYPE_SPOT)
		{
			Count++;
		}

		if (Count == WhichSpot)
			break;

		SpotColour = (IndexedColour *) ColList->GetNext(SpotColour);
	}

	return(SpotColour);
}


void SetNewColourPlate(ColourPlateType NewType, INT32 WhichToggle, INT32 WhichSpot)
	// SetNewColourPlate Jason 22/5/96
	// Sets a new colour separation mode for the selected view
	//   NewType     -	if WhichToggle == 0, sets the new plate type
	//   WhichToggle -	0 = just set the type;
	//					1 = just toggle monochrome mode,
	//					2 = just toggle invert mode
	//					3 = just toggle overprint mode
	//	 WhichSpot   -  Number of spot plate required (1-based)
	//
	// If a SPOT plate is desired, the colour list will be searched until 
	// the required spot colour is found
{
	// For convenience in Jason's debug cases
	View *SelView = DocView::GetSelected();
	if (SelView == NULL)
		return;

	// Get the currently active ColourPlate descriptor for the selected view (may be NULL)
	ColourPlate *SepDescriptor = SelView->GetColourPlate();

	BOOL Mono   = (SepDescriptor != NULL) ? SepDescriptor->IsMonochrome()  : FALSE;
	BOOL Negate = (SepDescriptor != NULL) ? SepDescriptor->IsNegative()    : FALSE;
	BOOL Overprint = (SepDescriptor != NULL) ? SepDescriptor->Overprints() : FALSE;

	switch(WhichToggle)
	{
		case 0:
			// Nothing to do here
			break;

		case 1:
			Mono = (Mono) ? FALSE : TRUE;
			NewType = (SepDescriptor != NULL) ? SepDescriptor->GetType() : COLOURPLATE_NONE;
			break;

		case 2:
			Negate = (Negate) ? FALSE : TRUE;
			NewType = (SepDescriptor != NULL) ? SepDescriptor->GetType() : COLOURPLATE_NONE;
			break;

		case 3:
			Overprint = (Overprint) ? FALSE : TRUE;
			NewType = (SepDescriptor != NULL) ? SepDescriptor->GetType() : COLOURPLATE_NONE;
			break;
	}

	if (SepDescriptor && SepDescriptor->GetType() == COLOURPLATE_SPOT)
	{
		if (WhichSpot < 1)
		{
			IndexedColour *pSpotCol = SepDescriptor->GetSpotColour();
			if (pSpotCol)
			{
				// Count the spot colours until we find the required one
				ColourList* pColList = Document::GetSelected()->GetIndexedColours();
				IndexedColour* pCol = (IndexedColour*) pColList->GetHead();
				INT32 Count = 0;

				while (pCol != NULL)
				{
					if (pCol->GetType() == COLOURTYPE_SPOT && !pCol->IsDeleted())
					{
						Count++;
					}

					if (pCol == pSpotCol)
						break;

					pCol = (IndexedColour*) pColList->GetNext(pCol);
				}

				if (pCol && Count > 0)
					WhichSpot = Count;
			}
		}
	}

	// Make sure we blank this
	SepDescriptor = NULL;

	// Try to create a spot plate
	if (NewType == COLOURPLATE_SPOT)
	{
		if (WhichSpot < 1)
		{
			ERROR3("Spot plate must be greater than 0");
			return;
		}

		IndexedColour *SpotColour = FindWhichSpot(WhichSpot);

		// If we found a spot colour, create the plate
		if (SpotColour != NULL)
			SepDescriptor = new ColourPlate(SpotColour, Mono, Negate);

		// Can't do it - no memory or no spots
		if (SepDescriptor == NULL)
		{
			ERROR3("Can't create spot plate - requested spot colour does not exist");
			return;
		}
	}

	if (NewType == COLOURPLATE_COMPOSITE)
	{
		Mono = FALSE;
		Negate = FALSE;
		Overprint = FALSE;
	}

	if (SepDescriptor == NULL)
		SepDescriptor = new ColourPlate(NewType, Mono, Negate);

	if (SepDescriptor != NULL)
		SepDescriptor->SetOverprint(Overprint);

	SelView->SetColourPlate(SepDescriptor);		// This makes a copy of the plate
	delete SepDescriptor;

	// Redraw anything affected by the change
	// Removed by Gerry as it already happens in SetColourPlate above
//	ColourManager::SelViewContextHasChanged();
}




/*******************************************************************

>	void OpShowPrinterColours::Do(OpDescriptor *pOpDesc)

	Author: 	Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/2003

	Purpose:	Does a "show printer colours" op

*******************************************************************/

void OpShowPrinterColours::Do(OpDescriptor *OpDesc)
{
	if (DocView::GetSelected() != NULL &&				// A view to toggle!
		GetApplication()->GetCMSManager() != NULL)		// and CMS
	{
		String Token = OpDesc->Token;
		INT32 WhichToggle = 0;
		INT32 WhichSpot = 0;
		INT32 TokenSpot = 0;
		ColourPlateType TokenPlate = COLOURPLATE_NONE;
		IndexedColour* pSpotColour = NULL;
		IndexedColour* pTokenColour = NULL;
		String_16 TempString;
		Token.Left(&TempString, 11);
		if (Token == String(OPTOKEN_COMPOSITEPREVIEW))
			TokenPlate = COLOURPLATE_COMPOSITE;
		else if (Token == String(OPTOKEN_CYANPREVIEW))
			TokenPlate = COLOURPLATE_CYAN;
		else if (Token == String(OPTOKEN_MAGENTAPREVIEW))
			TokenPlate = COLOURPLATE_MAGENTA;
		else if (Token == String(OPTOKEN_YELLOWPREVIEW))
			TokenPlate = COLOURPLATE_YELLOW;
		else if (Token == String(OPTOKEN_KEYPREVIEW))
			TokenPlate = COLOURPLATE_KEY;
		else if (TempString == String(OPTOKEN_SPOTPREVIEWBASE))
		{
			TokenSpot = (Token[11] - _T('0'));
			if (TokenSpot > 8)
				TokenSpot = -1;
			pTokenColour = FindWhichSpot(TokenSpot);
			TokenPlate = COLOURPLATE_SPOT;
		}
		else if (Token == String(OPTOKEN_MONOPLATEPREVIEW))
			WhichToggle = 1;
		
		// Determine what mode we are in and check against which OpDescriptor
		ColourPlateType PlateType = COLOURPLATE_NONE;
		ColourContext *cc = DocView::GetSelected()->GetColourContext(COLOURMODEL_RGBT);
		if (cc && cc->GetColourPlate())
		{
			PlateType = cc->GetColourPlate()->GetType();

			if (PlateType == COLOURPLATE_SPOT)
			{
				pSpotColour = cc->GetColourPlate()->GetSpotColour();
				if (pSpotColour != pTokenColour)
					PlateType = COLOURPLATE_NONE;
			}
		}

		if (PlateType == TokenPlate ||
			(Token == String(OPTOKEN_CANCELPREVIEW) && PlateType != COLOURPLATE_NONE))
		{
			// To restore normal view, we just remove any attached ColourPlate from the view
			DocView::GetSelected()->SetColourPlate(NULL);
			ColourManager::SelViewContextHasChanged();	// Redraw everything which is affected
			CMainFrame* pMainFrame = GetMainFrame();
			if (pMainFrame)
				pMainFrame->SetPrinterColourStatus(FALSE);
		}
		else
		{
			SetNewColourPlate(TokenPlate, WhichToggle, TokenSpot);

			// To avoid a huge delay the next time a bitmap is rendered in this composite view,
			// we get XaraCMS to cache its lookup table now. If no deep bitmaps are rendered,
			// this is wasteful, but better than getting a several-second delay part-way
			// through the next document redraw.
			Progress::Start();		// Fail to show an hourglass (this doesn't work for some reason)
			DWORD *Dummy = GetApplication()->GetCMSManager()->GetPaperViewTable();
			Progress::Stop();
			CMainFrame* pMainFrame = GetMainFrame();
			if (pMainFrame)
			{
				String_256 sPlateName;

				switch (TokenPlate)
				{
				case COLOURPLATE_CYAN:
					sPlateName.Load(_R(IDS_PLATENAME_CYAN));
					break;
				case COLOURPLATE_MAGENTA:
					sPlateName.Load(_R(IDS_PLATENAME_MAGENTA));
					break;
				case COLOURPLATE_YELLOW:
					sPlateName.Load(_R(IDS_PLATENAME_YELLOW));
					break;
				case COLOURPLATE_KEY:
					sPlateName.Load(_R(IDS_PLATENAME_KEY));
					break;
				case COLOURPLATE_COMPOSITE:
					sPlateName.Load(_R(IDS_PLATENAME_COMPOSITE));
					break;
				case COLOURPLATE_SPOT:
					if (pTokenColour)
						sPlateName = *(pTokenColour->GetName());
					break;
				}
				
				pMainFrame->SetPrinterColourStatus(TRUE, &sPlateName);
			}
		}
	}

	End();
}



/*******************************************************************

>	static BOOL OpShowPrinterColours::Init()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Registers this Op

*******************************************************************/

BOOL OpShowPrinterColours::Init()
{

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLENORMALPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_CANCELPREVIEW,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLENORMALHELP),			// Help string ID
						_R(IDS_TOGGLENORMALBBL)			// Bubble help ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLECOMPPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_SHOWPRINTERCOLOURS,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLECOMPHELP),			// Help string ID
						_R(IDS_TOGGLECOMPBBL)			// Bubble help ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLESHOWPRINTCOLS),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_COMPOSITEPREVIEW,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLESHOWPRINTCOLSHELP),			// Help string ID
						_R(IDS_TOGGLESHOWPRINTCOLSBBL),			// Bubble help ID
						_R(IDD_BARCONTROLSTORE),		// resource ID
						_R(IDC_COMPOSITEPREVIEW),		// control ID
						SYSTEMBAR_UTILITIES			// Bar ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLECYANPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_CYANPREVIEW,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLECYANHELP),			// Help string ID
						_R(IDS_TOGGLECYANBBL)			// Bubble help ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLEMAGENTAPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_MAGENTAPREVIEW,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLEMAGENTAHELP),			// Help string ID
						_R(IDS_TOGGLEMAGENTABBL)			// Bubble help ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLEYELLOWPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_YELLOWPREVIEW,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLEYELLOWHELP),			// Help string ID
						_R(IDS_TOGGLEYELLOWBBL)			// Bubble help ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLEKEYPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_KEYPREVIEW,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLEKEYHELP),			// Help string ID
						_R(IDS_TOGGLEKEYBBL)			// Bubble help ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLEMONOPLATEPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_MONOPLATEPREVIEW,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLEMONOPLATEHELP),			// Help string ID
						_R(IDS_TOGGLEMONOPLATEBBL)			// Bubble help ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLESPOTPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_SPOTPREVIEW1,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLESPOTHELP),			// Help string ID
						_R(IDS_TOGGLESPOTBBL)			// Bubble help ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLESPOTPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_SPOTPREVIEW2,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLESPOTHELP),			// Help string ID
						_R(IDS_TOGGLESPOTBBL)			// Bubble help ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLESPOTPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_SPOTPREVIEW3,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLESPOTHELP),			// Help string ID
						_R(IDS_TOGGLESPOTBBL)			// Bubble help ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLESPOTPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_SPOTPREVIEW4,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLESPOTHELP),			// Help string ID
						_R(IDS_TOGGLESPOTBBL)			// Bubble help ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLESPOTPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_SPOTPREVIEW5,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLESPOTHELP),			// Help string ID
						_R(IDS_TOGGLESPOTBBL)			// Bubble help ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLESPOTPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_SPOTPREVIEW6,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLESPOTHELP),			// Help string ID
						_R(IDS_TOGGLESPOTBBL)			// Bubble help ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLESPOTPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_SPOTPREVIEW7,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLESPOTHELP),			// Help string ID
						_R(IDS_TOGGLESPOTBBL)			// Bubble help ID
						))
		return(FALSE);

	if (!Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLESPOTPREVIEW),
						CC_RUNTIME_CLASS(OpShowPrinterColours),
						OPTOKEN_SPOTPREVIEW8,
						OpShowPrinterColours::GetState,
						_R(IDS_TOGGLESPOTHELP),			// Help string ID
						_R(IDS_TOGGLESPOTBBL)			// Bubble help ID
						))
		return(FALSE);

	return(TRUE);
}



/*******************************************************************

>	static OpState OpShowPrinterColours::GetState(String_256*, OpDescriptor*)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	As usual for GetStates

*******************************************************************/

OpState OpShowPrinterColours::GetState(String_256 *ShadeReason, OpDescriptor* OpDesc)
{
	// By default we will be unticked, grey and present
	OpState OpSt(FALSE, TRUE, FALSE);

	if (DocView::GetSelected() == NULL ||				// No view to toggle!
		GetApplication()->GetCMSManager() == NULL)		// Or no CMS
	{
		// Set a sensible reason here
		ShadeReason->MakeMsg(_R(IDS_NOCOMPPREVIEW));
		return(OpSt);
	}

	String Token = OpDesc->Token;
	if (Token != String(OPTOKEN_MONOPLATEPREVIEW))
		OpSt.Greyed = FALSE;

	String_16 TempString;
	Token.Left(&TempString, 11);
	BOOL bIsSpot = (TempString == String(OPTOKEN_SPOTPREVIEWBASE));

	IndexedColour* pSpotColour = NULL;
	INT32 TokenSpot = 0;
	if (bIsSpot)
	{
		TokenSpot = (Token[11] - _T('0'));
		if (TokenSpot > 8)
			TokenSpot = -1;
	}

	IndexedColour *pTokenColour = NULL;
	if (TokenSpot > 0)
	{
		pTokenColour = FindWhichSpot(TokenSpot);
		if (pTokenColour == NULL)
		{
			// n'th spot colour doesn't exist so remove this item
			return(OpState(FALSE, FALSE, TRUE));
		}
		*ShadeReason = *(pTokenColour->GetName());
	}

	// Determine what mode we are in and check against which OpDescriptor
	ColourContext *cc = DocView::GetSelected()->GetColourContext(COLOURMODEL_RGBT);
	if (cc && cc->GetColourPlate())
	{
		ColourPlateType PlateType = cc->GetColourPlate()->GetType();
		if (TokenSpot > 0)
		{
			if (PlateType == COLOURPLATE_SPOT)
			{
				pSpotColour = cc->GetColourPlate()->GetSpotColour();
				if (pSpotColour == pTokenColour)
					OpSt.Ticked = TRUE;
			}
		
			return(OpSt);
		}
		else if (Token == String(OPTOKEN_MONOPLATEPREVIEW))
		{
			OpSt.Greyed = (PlateType != COLOURPLATE_CYAN && PlateType != COLOURPLATE_MAGENTA && PlateType != COLOURPLATE_YELLOW);
			OpSt.Ticked = cc->GetColourPlate()->IsMonochrome();
		}
		else if (Token == String(OPTOKEN_CANCELPREVIEW))
		{
			OpSt.Ticked = (PlateType == COLOURPLATE_NONE);
		}
		else
		{
			ColourPlateType TokenPlate = COLOURPLATE_NONE;
			if (Token == String(OPTOKEN_COMPOSITEPREVIEW))
				TokenPlate = COLOURPLATE_COMPOSITE;
			else if (Token == String(OPTOKEN_CYANPREVIEW))
				TokenPlate = COLOURPLATE_CYAN;
			else if (Token == String(OPTOKEN_MAGENTAPREVIEW))
				TokenPlate = COLOURPLATE_MAGENTA;
			else if (Token == String(OPTOKEN_YELLOWPREVIEW))
				TokenPlate = COLOURPLATE_YELLOW;
			else if (Token == String(OPTOKEN_KEYPREVIEW))
				TokenPlate = COLOURPLATE_KEY;
			
			if (PlateType == TokenPlate)
				OpSt.Ticked = TRUE;
		}
	}
	else if (Token == String(OPTOKEN_CANCELPREVIEW))
	{
		// No colour plate and the "Normal view" item
		OpSt.Ticked = TRUE;
	}

	return(OpSt);
}





/*******************************************************************

>	OpOverprintLine::OpOverprintLine()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Purpose:	Constructor for OpOverprintLine operation

*******************************************************************/

OpOverprintLine::OpOverprintLine()
{
}



/*******************************************************************

>	void OpOverprintLine::Do(OpDescriptor *OpDesc)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Purpose:	Applies the "overprint line" attribute to the selection

*******************************************************************/

void OpOverprintLine::Do(OpDescriptor *OpDesc)
{
	// Determine if we can apply ourself at the moment. We call GetState
	// so that the state determination code is nicely centralised.
	OpState State = GetState(NULL, OpDesc);

	if (!State.Greyed)
	{
		// Create a new overprint attribute
		NodeAttribute *Attrib = new AttrOverprintLine;

		if (Attrib != NULL)
		{
			// By default we've made an attribute which is OFF. If the State indicates
			// (via the ticked flag) that the selection contains some item(s) which are ON,
			// then we want to toggle the state OFF - otherwise, we force the attribute ON.
			if (!State.Ticked)
				((OverprintLineAttrValue *)Attrib->GetAttributeValue())->SetOverprint(TRUE);

			// And apply it
			AttributeManager::AttributeSelected(Attrib);
		}
	}

	End();
}



/*******************************************************************

>	static BOOL OpOverprintLine::Init()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Purpose:	Registers this Op

*******************************************************************/

BOOL OpOverprintLine::Init()
{
	return(Operation::RegisterOpDescriptor(
						0,
						_R(IDS_ATTROVERPRINTLINE),
						CC_RUNTIME_CLASS(OpOverprintLine),
						OPTOKEN_OVERPRINTLINE,
						OpOverprintLine::GetState,
						_R(IDS_HELP_OVERPRINTLINE),		// Help ID
						_R(IDS_BBL_OVERPRINTLINE),		// bubble help ID
						_R(IDD_BARCONTROLSTORE),		// resource ID
						_R(IDC_OVERPRINTLINE),			// control ID
						SYSTEMBAR_UTILITIES,		// Bar ID
						TRUE,						// Recieve system messages
						FALSE,						// Smart duplicate operation
						FALSE,						// NOT Clean operation
						NULL,						// No vertical counterpart
						NULL,						// String for one copy only error
						GREY_WHEN_NO_SELECTION |	// Auto state flags
						DONT_GREY_WHEN_SELECT_INSIDE
					));
}



/*******************************************************************

>	OpState OpOverprintLine::GetState(String_256 *ShadeReason, OpDescriptor *OpDesc)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Outputs:	ShadeReason - If this is non-NULL, and if the returned OpState
				indicates that this Op is shaded, it will be returned filled
				in with a string describing why the Op is shaded.
				Otherwise this string is left untouched

				OpDesc - The descriptor with which we'll be invoked

	Returns:	An OpState indicating this Op's state

	Purpose:	As usual for GetStates

*******************************************************************/

OpState OpOverprintLine::GetState(String_256 *ShadeReason, OpDescriptor *OpDesc)
{
	OpState State;

	SelRange *Selection = GetApplication()->FindSelection();
	if (Selection == NULL || Selection->Count() < 1)
	{
		State.Greyed = TRUE;
		if (ShadeReason != NULL)
			ShadeReason->MakeMsg(_R(IDS_NO_OBJECTS_SELECTED));
	}
	else
	{
		// If any of the selection has the attribute applied, tick the op
		NodeAttribute *CurAttr = NULL;

		SelRange::CommonAttribResult result =
			Selection->FindCommonAttribute(CC_RUNTIME_CLASS(AttrOverprintLine), &CurAttr);

		if (result == SelRange::ATTR_MANY)
			State.Ticked = TRUE;			// Selection includes all states, so mark it as ON
		else if (result == SelRange::ATTR_COMMON)
		{
			// The entire selection uses the same attribute - but is it ON or OFF?
			if ( ((OverprintLineAttrValue *)CurAttr->GetAttributeValue())->IsOverprintOn() )
				State.Ticked = TRUE;		// It's ON
		}
	}

	return(State);
}













/*******************************************************************

>	OpOverprintFill::OpOverprintFill()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Purpose:	Constructor for OpOverprintFill operation

*******************************************************************/

OpOverprintFill::OpOverprintFill()
{
}



/*******************************************************************

>	void OpOverprintFill::Do(OpDescriptor *OpDesc)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Purpose:	Applies the "overprint fill" attribute to the selection

*******************************************************************/

void OpOverprintFill::Do(OpDescriptor *OpDesc)
{
	// Determine if we can apply ourself at the moment. We call GetState
	// so that the state determination code is nicely centralised.
	OpState State = GetState(NULL, OpDesc);

	if (!State.Greyed)
	{
		// Create a new overprint attribute
		NodeAttribute *Attrib = new AttrOverprintFill;

		if (Attrib != NULL)
		{
			// By default we've made an attribute which is OFF. If the State indicates
			// (via the ticked flag) that the selection contains some item(s) which are ON,
			// then we want to toggle the state OFF - otherwise, we force the attribute ON.
			if (!State.Ticked)
				((OverprintFillAttrValue *)Attrib->GetAttributeValue())->SetOverprint(TRUE);

			// And apply it
			AttributeManager::AttributeSelected(Attrib);
		}
	}

	End();
}



/*******************************************************************

>	static BOOL OpOverprintFill::Init()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Purpose:	Registers this Op

*******************************************************************/

BOOL OpOverprintFill::Init()
{
	return(Operation::RegisterOpDescriptor(
						0,
						_R(IDS_ATTROVERPRINTFILL),
						CC_RUNTIME_CLASS(OpOverprintFill),
						OPTOKEN_OVERPRINTFILL,
						OpOverprintFill::GetState,
						_R(IDS_HELP_OVERPRINTFILL),
						_R(IDS_BBL_OVERPRINTFILL),		// bubble help ID
						_R(IDD_BARCONTROLSTORE),		// resource ID
						_R(IDC_OVERPRINTFILL),			// control ID
						SYSTEMBAR_UTILITIES,		// Bar ID
						TRUE,						// Recieve system messages
						FALSE,						// No smart duplicate operation
						FALSE,						// Not Clean operation
						NULL,						// No vertical counterpart
						NULL,						// String for one copy only error
						GREY_WHEN_NO_SELECTION |	// Auto state flags
						DONT_GREY_WHEN_SELECT_INSIDE
					));
}



/*******************************************************************

>	OpState OpOverprintFill::GetState(String_256 *ShadeReason, OpDescriptor *OpDesc)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Outputs:	ShadeReason - If this is non-NULL, and if the returned OpState
				indicates that this Op is shaded, it will be returned filled
				in with a string describing why the Op is shaded.
				Otherwise this string is left untouched

				OpDesc - The descriptor with which we'll be invoked

	Returns:	An OpState indicating this Op's state

	Purpose:	As usual for GetStates

*******************************************************************/

OpState OpOverprintFill::GetState(String_256 *ShadeReason, OpDescriptor *OpDesc)
{
	OpState State;

	SelRange *Selection = GetApplication()->FindSelection();
	if (Selection == NULL || Selection->Count() < 1)
	{
		State.Greyed = TRUE;
		if (ShadeReason != NULL)
			ShadeReason->MakeMsg(_R(IDS_NO_OBJECTS_SELECTED));
	}
	else
	{
		// If any of the selection has the attribute applied, tick the op
		NodeAttribute *CurAttr = NULL;

		SelRange::CommonAttribResult result =
			Selection->FindCommonAttribute(CC_RUNTIME_CLASS(AttrOverprintFill), &CurAttr);

		if (result == SelRange::ATTR_MANY)
			State.Ticked = TRUE;			// Selection includes all states, so mark it as ON
		else if (result == SelRange::ATTR_COMMON)
		{
			// The entire selection uses the same attribute - but is it ON or OFF?
			if ( ((OverprintFillAttrValue *)CurAttr->GetAttributeValue())->IsOverprintOn() )
				State.Ticked = TRUE;		// It's ON
		}
	}

	return(State);
}















/*******************************************************************

>	OpPrintOnAllPlates::OpPrintOnAllPlates()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Purpose:	Constructor for OpPrintOnAllPlates operation

*******************************************************************/

OpPrintOnAllPlates::OpPrintOnAllPlates()
{
}



/*******************************************************************

>	void OpPrintOnAllPlates::Do(OpDescriptor *OpDesc)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Purpose:	Applies the "print on all plates" attribute to the selection

*******************************************************************/

void OpPrintOnAllPlates::Do(OpDescriptor *OpDesc)
{
	// Determine if we can apply ourself at the moment. We call GetState
	// so that the state determination code is nicely centralised.
	OpState State = GetState(NULL, OpDesc);

	if (!State.Greyed)
	{
		// Create a new overprint attribute
		NodeAttribute *Attrib = new AttrPrintOnAllPlates;

		if (Attrib != NULL)
		{
			// By default we've made an attribute which is OFF. If the State indicates
			// (via the ticked flag) that the selection contains some item(s) which are ON,
			// then we want to toggle the state OFF - otherwise, we force the attribute ON.
			if (!State.Ticked)
				((PrintOnAllPlatesAttrValue *)Attrib->GetAttributeValue())->SetPrintOnAllPlates(TRUE);

			// And apply it
			AttributeManager::AttributeSelected(Attrib);
		}
	}

	End();
}



/*******************************************************************

>	static BOOL OpPrintOnAllPlates::Init()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Purpose:	Registers this Op

*******************************************************************/

BOOL OpPrintOnAllPlates::Init()
{
	return(Operation::RegisterOpDescriptor(
						0,
						_R(IDS_ATTRPRINTONALLPLATES),
						CC_RUNTIME_CLASS(OpPrintOnAllPlates),
						OPTOKEN_PRINTONALLPLATES,
						OpPrintOnAllPlates::GetState,
						_R(IDS_HELP_PRINTONALLPLATES),	// Help string ID
						_R(IDS_BBL_PRINTONALLPLATES),	// bubble help ID
						_R(IDD_BARCONTROLSTORE),		// resource ID
						_R(IDC_PRINTONALLPLATES),		// control ID
						SYSTEMBAR_UTILITIES,		// Bar ID
						TRUE,						// Recieve system messages
						FALSE,						// Smart duplicate operation
						FALSE,						// NOT Clean operation
						NULL,						// No vertical counterpart
						NULL,						// String for one copy only error
						GREY_WHEN_NO_SELECTION |	// Auto state flags
						DONT_GREY_WHEN_SELECT_INSIDE
					));
}



/*******************************************************************

>	OpState OpPrintOnAllPlates::GetState(String_256 *ShadeReason, OpDescriptor *OpDesc)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Outputs:	ShadeReason - If this is non-NULL, and if the returned OpState
				indicates that this Op is shaded, it will be returned filled
				in with a string describing why the Op is shaded.
				Otherwise this string is left untouched

				OpDesc - The descriptor with which we'll be invoked

	Returns:	An OpState indicating this Op's state

	Purpose:	As usual for GetStates

*******************************************************************/

OpState OpPrintOnAllPlates::GetState(String_256 *ShadeReason, OpDescriptor *OpDesc)
{
	OpState State;

	SelRange *Selection = GetApplication()->FindSelection();
	if (Selection == NULL || Selection->Count() < 1)
	{
		State.Greyed = TRUE;
		if (ShadeReason != NULL)
			ShadeReason->MakeMsg(_R(IDS_NO_OBJECTS_SELECTED));
	}
	else
	{
		// If any of the selection has the attribute applied, tick the op
		NodeAttribute *CurAttr = NULL;

		SelRange::CommonAttribResult result =
			Selection->FindCommonAttribute(CC_RUNTIME_CLASS(AttrPrintOnAllPlates), &CurAttr);

		if (result == SelRange::ATTR_MANY)
			State.Ticked = TRUE;			// Selection includes all states, so mark it as ON
		else if (result == SelRange::ATTR_COMMON)
		{
			// The entire selection uses the same attribute - but is it ON or OFF?
			if ( ((PrintOnAllPlatesAttrValue *)CurAttr->GetAttributeValue())->IsPrintOnAllPlatesOn() )
				State.Ticked = TRUE;		// It's ON
		}
	}

	return(State);
}



#if FALSE

/*******************************************************************

>	OpToggleCompositePreview::OpToggleCompositePreview()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Constructor for OpToggleCompositePreview operation

*******************************************************************/

OpToggleCompositePreview::OpToggleCompositePreview()
{
}



/*******************************************************************

>	void OpToggleCompositePreview::Do(OpDescriptor *)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Toggles the selected view between "normal view" and
				"composite print preview" modes.

*******************************************************************/

void OpToggleCompositePreview::Do(OpDescriptor *)
{
	if (CanToggleState())
	{
		if (IsInCompositeMode())
		{
			if (DocView::GetSelected() != NULL)
			{
				// To restore normal view, we just remove any attached ColourPlate from the view
				DocView::GetSelected()->SetColourPlate(NULL);
				ColourManager::SelViewContextHasChanged();	// Redraw everything which is affected
			}
		}
		else
		{
			SetNewColourPlate(COLOURPLATE_COMPOSITE, 0, 0);

			// To avoid a huge delay the next time a bitmap is rendered in this composite view,
			// we get XaraCMS to cache its lookup table now. If no deep bitmaps are rendered,
			// this is wasteful, but better than getting a several-second delay part-way
			// through the next document redraw.
			Progress::Start();		// Fail to show an hourglass (this doesn't work for some reason)
			DWORD *Dummy = GetApplication()->GetCMSManager()->GetPaperViewTable();
			Progress::Stop();
		}
	}

	End();
}



/*******************************************************************

>	static BOOL OpToggleCompositePreview::Init()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Registers this Op

*******************************************************************/

BOOL OpToggleCompositePreview::Init()
{
	return(Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLECOMPPREVIEW),
						CC_RUNTIME_CLASS(OpToggleCompositePreview),
						OPTOKEN_COMPOSITEPREVIEW,
						OpToggleCompositePreview::GetState,
						_R(IDS_TOGGLECOMPHELP),			// Help string ID
						_R(IDS_TOGGLECOMPBBL),			// Bubble help ID
						_R(IDD_BARCONTROLSTORE),		// resource ID
						_R(IDC_COMPOSITEPREVIEW),		// control ID
						SYSTEMBAR_UTILITIES,		// Bar ID
						TRUE,						// Recieve system messages
						FALSE,						// No smart duplicate operation
						TRUE,						// Clean operation
						NULL,						// No vertical counterpart
						NULL,						// String for one copy only error
						0							// Auto state flags
					));
}



/*******************************************************************

>	static OpState OpToggleCompositePreview::GetState(String_256*, OpDescriptor*)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	As usual for GetStates

*******************************************************************/

OpState OpToggleCompositePreview::GetState(String_256 *ShadeReason, OpDescriptor*)
{
	OpState OpSt;

	if (!CanToggleState())
	{
		OpSt.Greyed = TRUE;
		ShadeReason->MakeMsg(_R(IDS_NOCOMPPREVIEW));
	}

	if (IsInCompositeMode())
		OpSt.Ticked = TRUE;

	return OpSt;
}



/*******************************************************************

>	static BOOL OpToggleCompositePreview::CanToggleState(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Returns:	TRUE if this Op can be invoked

	Purpose:	To determine if the state can be toggled.
				This can only be done when we have a view to toggle, 
				and when the XaraCMS manager is available.

*******************************************************************/

BOOL OpToggleCompositePreview::CanToggleState(void)
{
	if (DocView::GetSelected() == NULL)					// No view to toggle!
		return(FALSE);

	if (GetApplication()->GetCMSManager() == NULL)		// Can't do composite preview
		return(FALSE);

	return(TRUE);
}



/*******************************************************************

>	static BOOL OpToggleCompositePreview::IsInCompositeMode(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Returns:	TRUE if in composite mode
				FALSE if not (i.e. if in normal mode, although other
				modes like separation plates will also return FALSE)

	Purpose:	To determine whether the currently selected view is
				in normal or composite view mode.

*******************************************************************/

BOOL OpToggleCompositePreview::IsInCompositeMode(void)
{
	if (DocView::GetSelected() == NULL)					// No view to toggle!
		return(FALSE);

	ColourContext *cc = DocView::GetSelected()->GetColourContext(COLOURMODEL_RGBT);

	if (cc == NULL || cc->GetColourPlate() == NULL)
		return(FALSE);									// No special plate

	if (cc->GetColourPlate()->GetType() != COLOURPLATE_COMPOSITE)
		return(FALSE);									// Not a composite plate

	return(TRUE);
}






/*******************************************************************

>	void OpToggleCyanPreview::Do(OpDescriptor *)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Toggles the selected view between "normal view" and
				"Cyan print preview" modes.

*******************************************************************/

void OpToggleCyanPreview::Do(OpDescriptor *)
{
	if (CanToggleState())
	{
		if (IsInCyanMode())
		{
			if (DocView::GetSelected() != NULL)
			{
				// To restore normal view, we just remove any attached ColourPlate from the view
				DocView::GetSelected()->SetColourPlate(NULL);
				ColourManager::SelViewContextHasChanged();	// Redraw everything which is affected
			}
		}
		else
		{
			SetNewColourPlate(COLOURPLATE_CYAN, 0, 0);

			// To avoid a huge delay the next time a bitmap is rendered in this composite view,
			// we get XaraCMS to cache its lookup table now. If no deep bitmaps are rendered,
			// this is wasteful, but better than getting a several-second delay part-way
			// through the next document redraw.
//			Progress::Start();		// Fail to show an hourglass (this doesn't work for some reason)
//			DWORD *Dummy = GetApplication()->GetCMSManager()->GetPaperViewTable();
//			Progress::Stop();
		}
	}

	End();
}



/*******************************************************************

>	static BOOL OpToggleCyanPreview::Init()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Registers this Op

*******************************************************************/

BOOL OpToggleCyanPreview::Init()
{
	return(Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLECYANPREVIEW),
						CC_RUNTIME_CLASS(OpToggleCyanPreview),
						OPTOKEN_CYANPREVIEW,
						OpToggleCyanPreview::GetState,
						_R(IDS_TOGGLECYANHELP),			// Help string ID
						_R(IDS_TOGGLECYANBBL),			// Bubble help ID
						NULL,						// resource ID
						NULL,						// control ID
						SYSTEMBAR_UTILITIES,		// Bar ID
						TRUE,						// Recieve system messages
						FALSE,						// No smart duplicate operation
						TRUE,						// Clean operation
						NULL,						// No vertical counterpart
						NULL,						// String for one copy only error
						0							// Auto state flags
					));
}



/*******************************************************************

>	static OpState OpToggleCyanPreview::GetState(String_256*, OpDescriptor*)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	As usual for GetStates

*******************************************************************/

OpState OpToggleCyanPreview::GetState(String_256 *ShadeReason, OpDescriptor*)
{
	OpState OpSt;

	if (!CanToggleState())
	{
		OpSt.Greyed = TRUE;
		ShadeReason->MakeMsg(_R(IDS_NOCOMPPREVIEW));
	}

	if (IsInCyanMode())
		OpSt.Ticked = TRUE;

	return OpSt;
}



/*******************************************************************

>	static BOOL OpToggleCyanPreview::CanToggleState(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Returns:	TRUE if this Op can be invoked

	Purpose:	To determine if the state can be toggled.
				This can only be done when we have a view to toggle, 
				and when the XaraCMS manager is available.

*******************************************************************/

BOOL OpToggleCyanPreview::CanToggleState(void)
{
	if (DocView::GetSelected() == NULL)					// No view to toggle!
		return(FALSE);

	if (GetApplication()->GetCMSManager() == NULL)		// Can't do composite preview
		return(FALSE);

	return(TRUE);
}



/*******************************************************************

>	static BOOL OpToggleCyanPreview::IsInCyanMode(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Returns:	TRUE if in Cyan mode
				FALSE if not (i.e. if in normal mode, although other
				modes like separation plates will also return FALSE)

	Purpose:	To determine whether the currently selected view is
				in normal or Cyan view mode.

*******************************************************************/

BOOL OpToggleCyanPreview::IsInCyanMode(void)
{
	if (DocView::GetSelected() == NULL)					// No view to toggle!
		return(FALSE);

	ColourContext *cc = DocView::GetSelected()->GetColourContext(COLOURMODEL_RGBT);

	if (cc == NULL || cc->GetColourPlate() == NULL)
		return(FALSE);									// No special plate

	if (cc->GetColourPlate()->GetType() != COLOURPLATE_CYAN)
		return(FALSE);									// Not a Cyan plate

	return(TRUE);
}






/*******************************************************************

>	void OpToggleMagentaPreview::Do(OpDescriptor *)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Toggles the selected view between "normal view" and
				"Magenta preview" modes.

*******************************************************************/

void OpToggleMagentaPreview::Do(OpDescriptor *)
{
	if (CanToggleState())
	{
		if (IsInMagentaMode())
		{
			if (DocView::GetSelected() != NULL)
			{
				// To restore normal view, we just remove any attached ColourPlate from the view
				DocView::GetSelected()->SetColourPlate(NULL);
				ColourManager::SelViewContextHasChanged();	// Redraw everything which is affected
			}
		}
		else
		{
			SetNewColourPlate(COLOURPLATE_MAGENTA, 0, 0);

			// To avoid a huge delay the next time a bitmap is rendered in this composite view,
			// we get XaraCMS to cache its lookup table now. If no deep bitmaps are rendered,
			// this is wasteful, but better than getting a several-second delay part-way
			// through the next document redraw.
//			Progress::Start();		// Fail to show an hourglass (this doesn't work for some reason)
//			DWORD *Dummy = GetApplication()->GetCMSManager()->GetPaperViewTable();
//			Progress::Stop();
		}
	}

	End();
}



/*******************************************************************

>	static BOOL OpToggleMagentaPreview::Init()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Registers this Op

*******************************************************************/

BOOL OpToggleMagentaPreview::Init()
{
	return(Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLEMAGENTAPREVIEW),
						CC_RUNTIME_CLASS(OpToggleMagentaPreview),
						OPTOKEN_MAGENTAPREVIEW,
						OpToggleMagentaPreview::GetState,
						_R(IDS_TOGGLEMAGENTAHELP),			// Help string ID
						_R(IDS_TOGGLEMAGENTABBL),			// Bubble help ID
						NULL,						// resource ID
						NULL,						// control ID
						SYSTEMBAR_UTILITIES,		// Bar ID
						TRUE,						// Recieve system messages
						FALSE,						// No smart duplicate operation
						TRUE,						// Clean operation
						NULL,						// No vertical counterpart
						NULL,						// String for one copy only error
						0							// Auto state flags
					));
}



/*******************************************************************

>	static OpState OpToggleMagentaPreview::GetState(String_256*, OpDescriptor*)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	As usual for GetStates

*******************************************************************/

OpState OpToggleMagentaPreview::GetState(String_256 *ShadeReason, OpDescriptor*)
{
	OpState OpSt;

	if (!CanToggleState())
	{
		OpSt.Greyed = TRUE;
		ShadeReason->MakeMsg(_R(IDS_NOCOMPPREVIEW));
	}

	if (IsInMagentaMode())
		OpSt.Ticked = TRUE;

	return OpSt;
}



/*******************************************************************

>	static BOOL OpToggleMagentaPreview::CanToggleState(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Returns:	TRUE if this Op can be invoked

	Purpose:	To determine if the state can be toggled.
				This can only be done when we have a view to toggle, 
				and when the XaraCMS manager is available.

*******************************************************************/

BOOL OpToggleMagentaPreview::CanToggleState(void)
{
	if (DocView::GetSelected() == NULL)					// No view to toggle!
		return(FALSE);

	if (GetApplication()->GetCMSManager() == NULL)		// Can't do composite preview
		return(FALSE);

	return(TRUE);
}



/*******************************************************************

>	static BOOL OpToggleMagentaPreview::IsInMagentaMode(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Returns:	TRUE if in composite mode
				FALSE if not (i.e. if in normal mode, although other
				modes like separation plates will also return FALSE)

	Purpose:	To determine whether the currently selected view is
				in normal or composite view mode.

*******************************************************************/

BOOL OpToggleMagentaPreview::IsInMagentaMode(void)
{
	if (DocView::GetSelected() == NULL)					// No view to toggle!
		return(FALSE);

	ColourContext *cc = DocView::GetSelected()->GetColourContext(COLOURMODEL_RGBT);

	if (cc == NULL || cc->GetColourPlate() == NULL)
		return(FALSE);									// No special plate

	if (cc->GetColourPlate()->GetType() != COLOURPLATE_MAGENTA)
		return(FALSE);									// Not a composite plate

	return(TRUE);
}




/*******************************************************************

>	void OpToggleYellowPreview::Do(OpDescriptor *)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Toggles the selected view between "normal view" and
				"Yellow preview" modes.

*******************************************************************/

void OpToggleYellowPreview::Do(OpDescriptor *)
{
	if (CanToggleState())
	{
		if (IsInYellowMode())
		{
			if (DocView::GetSelected() != NULL)
			{
				// To restore normal view, we just remove any attached ColourPlate from the view
				DocView::GetSelected()->SetColourPlate(NULL);
				ColourManager::SelViewContextHasChanged();	// Redraw everything which is affected
			}
		}
		else
		{
			SetNewColourPlate(COLOURPLATE_YELLOW, 0, 0);

			// To avoid a huge delay the next time a bitmap is rendered in this composite view,
			// we get XaraCMS to cache its lookup table now. If no deep bitmaps are rendered,
			// this is wasteful, but better than getting a several-second delay part-way
			// through the next document redraw.
//			Progress::Start();		// Fail to show an hourglass (this doesn't work for some reason)
//			DWORD *Dummy = GetApplication()->GetCMSManager()->GetPaperViewTable();
//			Progress::Stop();
		}
	}

	End();
}



/*******************************************************************

>	static BOOL OpToggleYellowPreview::Init()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Registers this Op

*******************************************************************/

BOOL OpToggleYellowPreview::Init()
{
	return(Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLEYELLOWPREVIEW),
						CC_RUNTIME_CLASS(OpToggleYellowPreview),
						OPTOKEN_YELLOWPREVIEW,
						OpToggleYellowPreview::GetState,
						_R(IDS_TOGGLEYELLOWHELP),			// Help string ID
						_R(IDS_TOGGLEYELLOWBBL),			// Bubble help ID
						NULL,						// resource ID
						NULL,						// control ID
						SYSTEMBAR_UTILITIES,		// Bar ID
						TRUE,						// Recieve system messages
						FALSE,						// No smart duplicate operation
						TRUE,						// Clean operation
						NULL,						// No vertical counterpart
						NULL,						// String for one copy only error
						0							// Auto state flags
					));
}



/*******************************************************************

>	static OpState OpToggleYellowPreview::GetState(String_256*, OpDescriptor*)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	As usual for GetStates

*******************************************************************/

OpState OpToggleYellowPreview::GetState(String_256 *ShadeReason, OpDescriptor*)
{
	OpState OpSt;

	if (!CanToggleState())
	{
		OpSt.Greyed = TRUE;
		ShadeReason->MakeMsg(_R(IDS_NOCOMPPREVIEW));
	}

	if (IsInYellowMode())
		OpSt.Ticked = TRUE;

	return OpSt;
}



/*******************************************************************

>	static BOOL OpToggleYellowPreview::CanToggleState(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Returns:	TRUE if this Op can be invoked

	Purpose:	To determine if the state can be toggled.
				This can only be done when we have a view to toggle, 
				and when the XaraCMS manager is available.

*******************************************************************/

BOOL OpToggleYellowPreview::CanToggleState(void)
{
	if (DocView::GetSelected() == NULL)					// No view to toggle!
		return(FALSE);

	if (GetApplication()->GetCMSManager() == NULL)		// Can't do composite preview
		return(FALSE);

	return(TRUE);
}



/*******************************************************************

>	static BOOL OpToggleYellowPreview::IsInYellowMode(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Returns:	TRUE if in composite mode
				FALSE if not (i.e. if in normal mode, although other
				modes like separation plates will also return FALSE)

	Purpose:	To determine whether the currently selected view is
				in normal or composite view mode.

*******************************************************************/

BOOL OpToggleYellowPreview::IsInYellowMode(void)
{
	if (DocView::GetSelected() == NULL)					// No view to toggle!
		return(FALSE);

	ColourContext *cc = DocView::GetSelected()->GetColourContext(COLOURMODEL_RGBT);

	if (cc == NULL || cc->GetColourPlate() == NULL)
		return(FALSE);									// No special plate

	if (cc->GetColourPlate()->GetType() != COLOURPLATE_YELLOW)
		return(FALSE);									// Not a composite plate

	return(TRUE);
}



/*******************************************************************

>	void OpToggleKeyPreview::Do(OpDescriptor *)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Toggles the selected view between "normal view" and
				"Key preview" modes.

*******************************************************************/

void OpToggleKeyPreview::Do(OpDescriptor *)
{
	if (CanToggleState())
	{
		if (IsInKeyMode())
		{
			if (DocView::GetSelected() != NULL)
			{
				// To restore normal view, we just remove any attached ColourPlate from the view
				DocView::GetSelected()->SetColourPlate(NULL);
				ColourManager::SelViewContextHasChanged();	// Redraw everything which is affected
			}
		}
		else
		{
			SetNewColourPlate(COLOURPLATE_KEY, 0, 0);

			// To avoid a huge delay the next time a bitmap is rendered in this composite view,
			// we get XaraCMS to cache its lookup table now. If no deep bitmaps are rendered,
			// this is wasteful, but better than getting a several-second delay part-way
			// through the next document redraw.
//			Progress::Start();		// Fail to show an hourglass (this doesn't work for some reason)
//			DWORD *Dummy = GetApplication()->GetCMSManager()->GetPaperViewTable();
//			Progress::Stop();
		}
	}

	End();
}



/*******************************************************************

>	static BOOL OpToggleKeyPreview::Init()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Registers this Op

*******************************************************************/

BOOL OpToggleKeyPreview::Init()
{
	return(Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLEKEYPREVIEW),
						CC_RUNTIME_CLASS(OpToggleKeyPreview),
						OPTOKEN_KEYPREVIEW,
						OpToggleKeyPreview::GetState,
						_R(IDS_TOGGLEKEYHELP),			// Help string ID
						_R(IDS_TOGGLEKEYBBL),			// Bubble help ID
						NULL,						// resource ID
						NULL,						// control ID
						SYSTEMBAR_UTILITIES,		// Bar ID
						TRUE,						// Recieve system messages
						FALSE,						// No smart duplicate operation
						TRUE,						// Clean operation
						NULL,						// No vertical counterpart
						NULL,						// String for one copy only error
						0							// Auto state flags
					));
}



/*******************************************************************

>	static OpState OpToggleKeyPreview::GetState(String_256*, OpDescriptor*)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	As usual for GetStates

*******************************************************************/

OpState OpToggleKeyPreview::GetState(String_256 *ShadeReason, OpDescriptor*)
{
	OpState OpSt;

	if (!CanToggleState())
	{
		OpSt.Greyed = TRUE;
		ShadeReason->MakeMsg(_R(IDS_NOCOMPPREVIEW));
	}

	if (IsInKeyMode())
		OpSt.Ticked = TRUE;

	return OpSt;
}



/*******************************************************************

>	static BOOL OpToggleKeyPreview::CanToggleState(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Returns:	TRUE if this Op can be invoked

	Purpose:	To determine if the state can be toggled.
				This can only be done when we have a view to toggle, 
				and when the XaraCMS manager is available.

*******************************************************************/

BOOL OpToggleKeyPreview::CanToggleState(void)
{
	if (DocView::GetSelected() == NULL)					// No view to toggle!
		return(FALSE);

	if (GetApplication()->GetCMSManager() == NULL)		// Can't do composite preview
		return(FALSE);

	return(TRUE);
}



/*******************************************************************

>	static BOOL OpToggleKeyPreview::IsInKeyMode(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Returns:	TRUE if in composite mode
				FALSE if not (i.e. if in normal mode, although other
				modes like separation plates will also return FALSE)

	Purpose:	To determine whether the currently selected view is
				in normal or composite view mode.

*******************************************************************/

BOOL OpToggleKeyPreview::IsInKeyMode(void)
{
	if (DocView::GetSelected() == NULL)					// No view to toggle!
		return(FALSE);

	ColourContext *cc = DocView::GetSelected()->GetColourContext(COLOURMODEL_RGBT);

	if (cc == NULL || cc->GetColourPlate() == NULL)
		return(FALSE);									// No special plate

	if (cc->GetColourPlate()->GetType() != COLOURPLATE_KEY)
		return(FALSE);									// Not a composite plate

	return(TRUE);
}



/*******************************************************************

>	void OpToggleSpotPreview::Do(OpDescriptor *)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Toggles the selected view between "normal view" and
				"Spot preview" modes.

*******************************************************************/

void OpToggleSpotPreview::Do(OpDescriptor *)
{
	if (CanToggleState())
	{
		if (IsInSpotMode())
		{
			if (DocView::GetSelected() != NULL)
			{
				// To restore normal view, we just remove any attached ColourPlate from the view
				DocView::GetSelected()->SetColourPlate(NULL);
				ColourManager::SelViewContextHasChanged();	// Redraw everything which is affected
			}
		}
		else
		{
			SetNewColourPlate(COLOURPLATE_SPOT, 0, 1);

			// To avoid a huge delay the next time a bitmap is rendered in this composite view,
			// we get XaraCMS to cache its lookup table now. If no deep bitmaps are rendered,
			// this is wasteful, but better than getting a several-second delay part-way
			// through the next document redraw.
//			Progress::Start();		// Fail to show an hourglass (this doesn't work for some reason)
//			DWORD *Dummy = GetApplication()->GetCMSManager()->GetPaperViewTable();
//			Progress::Stop();
		}
	}

	End();
}



/*******************************************************************

>	static BOOL OpToggleSpotPreview::Init()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Registers this Op

*******************************************************************/

BOOL OpToggleSpotPreview::Init()
{
	return(Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLESPOTPREVIEW),
						CC_RUNTIME_CLASS(OpToggleSpotPreview),
						OPTOKEN_SPOTPREVIEW,
						OpToggleSpotPreview::GetState,
						_R(IDS_TOGGLESPOTHELP),			// Help string ID
						_R(IDS_TOGGLESPOTBBL),			// Bubble help ID
						NULL,						// resource ID
						NULL,						// control ID
						SYSTEMBAR_UTILITIES,		// Bar ID
						TRUE,						// Recieve system messages
						FALSE,						// No smart duplicate operation
						TRUE,						// Clean operation
						NULL,						// No vertical counterpart
						NULL,						// String for one copy only error
						0							// Auto state flags
					));
}



/*******************************************************************

>	static OpState OpToggleSpotPreview::GetState(String_256*, OpDescriptor*)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	As usual for GetStates

*******************************************************************/

OpState OpToggleSpotPreview::GetState(String_256 *ShadeReason, OpDescriptor*)
{
	OpState OpSt;

	if (!CanToggleState())
	{
		OpSt.Greyed = TRUE;
		ShadeReason->MakeMsg(_R(IDS_NOCOMPPREVIEW));
	}

	if (IsInSpotMode())
		OpSt.Ticked = TRUE;

	return OpSt;
}



/*******************************************************************

>	static BOOL OpToggleSpotPreview::CanToggleState(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Returns:	TRUE if this Op can be invoked

	Purpose:	To determine if the state can be toggled.
				This can only be done when we have a view to toggle, 
				and when the XaraCMS manager is available.

*******************************************************************/

BOOL OpToggleSpotPreview::CanToggleState(void)
{
	if (DocView::GetSelected() == NULL)					// No view to toggle!
		return(FALSE);

	if (GetApplication()->GetCMSManager() == NULL)		// Can't do composite preview
		return(FALSE);

	return(TRUE);
}



/*******************************************************************

>	static BOOL OpToggleSpotPreview::IsInSpotMode(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Returns:	TRUE if in composite mode
				FALSE if not (i.e. if in normal mode, although other
				modes like separation plates will also return FALSE)

	Purpose:	To determine whether the currently selected view is
				in normal or composite view mode.

*******************************************************************/

BOOL OpToggleSpotPreview::IsInSpotMode(void)
{
	if (DocView::GetSelected() == NULL)					// No view to toggle!
		return(FALSE);

	ColourContext *cc = DocView::GetSelected()->GetColourContext(COLOURMODEL_RGBT);

	if (cc == NULL || cc->GetColourPlate() == NULL)
		return(FALSE);									// No special plate

	if (cc->GetColourPlate()->GetType() != COLOURPLATE_SPOT)
		return(FALSE);									// Not a spot plate

	return(TRUE);
}




/*******************************************************************

>	void OpToggleMonoPlatePreview::Do(OpDescriptor *)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Toggles the selected view between "normal view" and
				"Spot preview" modes.

*******************************************************************/

void OpToggleMonoPlatePreview::Do(OpDescriptor *)
{
	if (CanToggleState())
	{
		if (IsInMonoMode())
		{
			if (DocView::GetSelected() != NULL)
			{
				// To restore normal view, we just remove any attached ColourPlate from the view
				DocView::GetSelected()->SetColourPlate(NULL);
				ColourManager::SelViewContextHasChanged();	// Redraw everything which is affected
			}
		}
		else
		{
			SetNewColourPlate(COLOURPLATE_NONE, 1, 0);		// Just toggle the mono flag

			// To avoid a huge delay the next time a bitmap is rendered in this composite view,
			// we get XaraCMS to cache its lookup table now. If no deep bitmaps are rendered,
			// this is wasteful, but better than getting a several-second delay part-way
			// through the next document redraw.
//			Progress::Start();		// Fail to show an hourglass (this doesn't work for some reason)
//			DWORD *Dummy = GetApplication()->GetCMSManager()->GetPaperViewTable();
//			Progress::Stop();
		}
	}

	End();
}



/*******************************************************************

>	static BOOL OpToggleMonoPlatePreview::Init()

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	Registers this Op

*******************************************************************/

BOOL OpToggleMonoPlatePreview::Init()
{
	return(Operation::RegisterOpDescriptor(
						0,
						_R(IDS_TOGGLEMONOPLATEPREVIEW),
						CC_RUNTIME_CLASS(OpToggleMonoPlatePreview),
						OPTOKEN_MONOPLATEPREVIEW,
						OpToggleMonoPlatePreview::GetState,
						_R(IDS_TOGGLEMONOPLATEHELP),	// Help string ID
						_R(IDS_TOGGLEMONOPLATEBBL),		// Bubble help ID
						NULL,						// resource ID
						NULL,						// control ID
						SYSTEMBAR_UTILITIES,		// Bar ID
						TRUE,						// Recieve system messages
						FALSE,						// No smart duplicate operation
						TRUE,						// Clean operation
						NULL,						// No vertical counterpart
						NULL,						// String for one copy only error
						0							// Auto state flags
					));
}



/*******************************************************************

>	static OpState OpToggleMonoPlatePreview::GetState(String_256*, OpDescriptor*)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Purpose:	As usual for GetStates

*******************************************************************/

OpState OpToggleMonoPlatePreview::GetState(String_256 *ShadeReason, OpDescriptor*)
{
	OpState OpSt;

	if (!CanToggleState())
	{
		OpSt.Greyed = TRUE;
		ShadeReason->MakeMsg(_R(IDS_NOCOMPPREVIEW));
	}

	if (IsInMonoMode())
		OpSt.Ticked = TRUE;

	return OpSt;
}



/*******************************************************************

>	static BOOL OpToggleMonoPlatePreview::CanToggleState(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Returns:	TRUE if this Op can be invoked

	Purpose:	To determine if the state can be toggled.
				This can only be done when we have a view to toggle, 
				and when the XaraCMS manager is available.

*******************************************************************/

BOOL OpToggleMonoPlatePreview::CanToggleState(void)
{
	if (DocView::GetSelected() == NULL)					// No view to toggle!
		return(FALSE);

	// TODOG: Check that we are showing a standard plate (not a spot)

	return(TRUE);
}



/*******************************************************************

>	static BOOL OpToggleMonoPlatePreview::IsInMonoMode(void)

	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96

	Returns:	TRUE if in composite mode
				FALSE if not (i.e. if in normal mode, although other
				modes like separation plates will also return FALSE)

	Purpose:	To determine whether the currently selected view is
				in normal or composite view mode.

*******************************************************************/

BOOL OpToggleMonoPlatePreview::IsInMonoMode(void)
{
	if (DocView::GetSelected() == NULL)					// No view to toggle!
		return(FALSE);

	ColourContext *cc = DocView::GetSelected()->GetColourContext(COLOURMODEL_RGBT);

	if (cc == NULL || cc->GetColourPlate() == NULL)
		return(FALSE);									// No special plate

	if (!(cc->GetColourPlate()->IsMonochrome()))
		return(FALSE);									// Not mono

	return(TRUE);
}

#endif
