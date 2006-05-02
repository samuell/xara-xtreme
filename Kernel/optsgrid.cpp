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
//
// Implementation of the Grid and Rulers tab of the options dialog box

/*

*/

#include "camtypes.h"
#include "appprefs.h"
//#include "prefsdlg.h"	// dialog/gadget ids
//#include "optsres.h"	// error strings
#include "optsmsgs.h"
#include "optsgrid.h"
#include "app.h"		// Camelot object
#include "document.h"	// document object
#include "spread.h"		// spread object
#include "page.h"		// page object
#include "units.h"		// units
#include "trans2d.h"	// 2d translation matrix
#include "layer.h"
#include "progress.h"	// Begin/EndSlowJob
#include "ophist.h"

CC_IMPLEMENT_DYNCREATE(OpGridResize, UndoableOperation)
CC_IMPLEMENT_DYNCREATE(ActionSetDefaultGrid, Action)

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

// Set some limit to the allowed page dimensions so that cannot have stupid values
// better to do this than fall over or stop rendering.
// The maximum at present which we can cope with is about 2.75M.
// This was determined by trial and error, and is the maximum page size that allows an
// object to be placed in the Top-Right corner, without our Render Matrix overflowing
// at maximum zoom (and hence the object not rendering).
// Future versions of Xara, should probably dynamically limit the Zoom factor to stop this.
// but limiting the Page size is easiest for now.

//const MILLIPOINT MaxPageDimension = 12 * 72000 * 12; 	// maximum = 12ft
const MILLIPOINT MaxPageDimension = 9 * 72000 * 12; 	// maximum = 108in = 9 ft = 275cm
const MILLIPOINT MinPageDimension = 72000/2; 			// minimum = 0.5inch


CC_IMPLEMENT_DYNAMIC(GridTab, OptionsTabs)   

/********************************************************************************************

>	GridTab::GridTab() 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GridTab constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

GridTab::GridTab()
{   
	// Do nothing for now
}        

/********************************************************************************************

>	GridTab::~GridTab()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GridTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
GridTab::~GridTab()
{   
}        

/********************************************************************************************

>	BOOL GridTab::Init()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	GridTab initialisation routine
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
BOOL GridTab::Init()
{   
	// Init (register) all our undoable operations
	if (!OpGridResize::Init())
		return(FALSE);

	return TRUE;
}        


/******************************************************************************************

>	CDlgResID GridTab::GetPageID()()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CDlgResID GridTab::GetPageID()
{
	return _R(IDD_OPTSTAB_GRID);
}

/******************************************************************************************

>	BOOL GridTab::IsDocumentOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a document option.	  
	Purpose:	Allows the document options dialog code to determine if this tab belongs
				to the group of document options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL GridTab::IsDocumentOption()
{
	return TRUE;	// This tab is not a document option 
}

/******************************************************************************************

>	BOOL GridTab::IsProgramOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a program option.	  
	Purpose:	Allows the program options dialog code to determine if this tab belongs
				to the group of program options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL GridTab::IsProgramOption()
{
	return TRUE;	// This tab is a program option 
}


/******************************************************************************************

>	BOOL AppPrefsDlg::CommitSection()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/95
	Returns:	TRUE if worked, FALSE if fails 
	Purpose:	Reads the gadgets on the Grid and Ruler Options Tab and sets the appropriate
				values in the grid in the spread dictated by the OptionsTab class, currently
				kept up to date with the first spread in the selected document

				If False is returned then **MUST** have reported error to the user otherwise
				the dialog box will refuse to close and the user will not know why.
				Also, individual controls must report their individual errors and not go
				through some crazy system and then ***NOT*** report the errors

******************************************************************************************/

BOOL GridTab::CommitSection()
{
	ERROR2IF(pPrefsDlg==NULL,FALSE,"GridTab::CommitSection() - pPrefsDlg==NULL");

	if (pDocument==NULL || pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_GRID))==FALSE)
		return TRUE;	// just return with no error

	// create storage for required values and init to arbitary but legal values
	UnitType GridUnits   = NOTYPE;
	double   GridSpacing = 72000.0;
	double   GridDivs    = 1.0;
	UINT32     GridSubDivs = 8;
	GridType TypeOfGrid  = RECTANGULAR;

	// Determine the current grid units and grid divisions (ie grid step in terms of units)
	// First the grid spacing
	BOOL ok = pPrefsDlg->GetDoubleAndUnitGadgetValue(&GridSpacing, &GridDivs, &GridUnits, _R(IDC_OPTS_GRIDSPACING), pSpread);
	if (!ok)
	{
		// We have had an error so inform the user that something is wrong
		InformWarning(_R(IDE_OPTS_INVALIDGRIDSPACE));
		return FALSE;	
	}
	// ensure sensible grid spacing
	if (GridSpacing <= 0 || GridSpacing >= MaxPageDimension)
	{
		InformWarning(_R(IDE_OPTS_INVALIDGRIDSPACE));
		return FALSE;
	}


	// determine grid sub-divisions
	GridSubDivs = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_GRIDDIVISIONS), 0,100, _R(IDE_OPTS_INVALIDGRIDDIV), &ok);
	if (!ok)
	{
		// We have had an error, informed the user so get out now before any more errors are reported
		return FALSE;	
	}

	// Make sure we have a sensible number of sub divisions
	if (GridSubDivs < 1)
		GridSubDivs = 1;

	// then grid type
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	TypeOfGrid = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_RECTANGULAR), 0,1,0,&ok) ? RECTANGULAR : ISOMETRIC;
#endif //webster
	// determine the origin
	DocCoord PageRelativeOrigin(0,0);
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	PageRelativeOrigin.x = pPrefsDlg->GetDimensionGadgetValue(_R(IDC_OPTS_ORIGINX), pSpread, &ok);
	if (ok) PageRelativeOrigin.y = pPrefsDlg->GetDimensionGadgetValue(_R(IDC_OPTS_ORIGINY), pSpread, &ok);
	if (!ok)
	{
		// We have had an error so inform the user that something is wrong
		InformWarning(_R(IDE_OPTS_INVALIDORIGIN));
		return FALSE;	
	}

	DocCoord Origin(0,0);
	ok = pSpread->PagesCoordToSpreadCoord(&Origin,PageRelativeOrigin);
	if (!ok)
	{
		// We have had an error so inform the user that something is wrong
		InformWarning(_R(IDE_OPTS_INVALIDORIGIN));
		return FALSE;	
	}

	// Ensure the origin falls within the spread's pasteboard
	DocRect SpreadRect = pSpread->GetPasteboardRect(FALSE);
	pSpread->DocCoordToSpreadCoord(&SpreadRect);

	if (!SpreadRect.ContainsCoord(Origin))
	{
		InformWarning(_R(IDE_OPTS_INVALIDORIGIN));
		return FALSE;
	}
#endif //webster
	// Set up param block with new grid info and invoke the resize op
	GridResizeInfo Param;
	Param.GridSpacing      = GridSpacing;
	Param.GridSubDivisions = GridSubDivs;
	Param.GridDivisions    = GridDivs;
	Param.GridUnits        = GridUnits;
	Param.TypeOfGrid       = TypeOfGrid;
	Param.OriginX          = PageRelativeOrigin.x;
	Param.OriginY          = PageRelativeOrigin.y;
	InvokeResize(&Param);

	return TRUE;
}

/******************************************************************************************

>	BOOL GridTab::NewUnitsInSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Function called when new default units come into operation. We must convert
				all units fields to display in the newly specified units.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL GridTab::NewUnitsInSection()
{
TRACEUSER( "Neville", _T("New units in page section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"GridTab::NewUnitsInSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_GRID));	// The page tab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now


	// All units work off the selected document and so we must do nothing if there is no
	// selected document. Check our clas variable to see if this is true or not.
	// Units should have been set up by the caller.
	if (pDocument == NULL)
	{
		// the dialog box is greyed.
		GreySection();
		return (TRUE);
	}

	// Do the same things as the normal init section code
	InitGridSection();

	return TRUE;
}

/********************************************************************************************

>	BOOL GridTab::GreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents 
	Errors:		-
	SeeAlso:	GridTab::UngreySection;

********************************************************************************************/

BOOL GridTab::GreySection()
{
TRACEUSER( "Neville", _T("GreySection in page section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"GridTab::GreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_GRID));	// The grid tab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_GRID_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), DocumentName);

	// Only update if we are not already grey 
	if (GreyStatus == TRUE)
		return TRUE;

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(FALSE);
	
	GreyStatus = TRUE;

	return ok;
} 

/********************************************************************************************

>	BOOL GridTab::UngreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document 
	Errors:		-
	SeeAlso:	GridTab::UngreySection;

********************************************************************************************/

BOOL GridTab::UngreySection()
{
TRACEUSER( "Neville", _T("UngreySection in page section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"GridTab::UngreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_GRID));	// The grid tab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_GRID_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), DocumentName);

	// Only update if we are not already ungrey 
	if (GreyStatus == FALSE)
		return TRUE;

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(TRUE);
	
	GreyStatus = FALSE;

	return ok;
}

/********************************************************************************************

>	BOOL GridTab::ChangeControlStatus(const BOOL Status)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 
	Errors:		-
	SeeAlso:	GridTab::GreySection;	GridTab::UngreySection;

********************************************************************************************/

BOOL GridTab::ChangeControlStatus(const BOOL Status)
{
	// Grey/Ungrey the controls
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_INFO), Status);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_ORIGINGROUP), Status);
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_ORIGINXTXT), Status);
#endif //webster
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_ORIGINYTXT), Status);
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_ORIGINX), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_ORIGINY), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_GRIDGROUP), Status);
#endif //webster
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_GRIDSPACETXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_GRIDSPACING), Status);
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_GRIDDIVTXT), Status);
#endif //webster
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_GRIDDIVISIONS), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_GRIDTYPE), Status);
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_ISOMETRIC), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_RECTANGULAR), Status);
#endif //webster
	return TRUE;
}			

/********************************************************************************************

>	BOOL GridTab::UpdateSection(String_256 *DocumentName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		Name of document
	Outputs:	-
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. Should only init the tab if the page is present.  
				The document name allows the info field on the tab to be filled in correctly.
	Errors:		-
	SeeAlso:	GridTab::GreySection; GridTab::UngreySection; GridTab::InitSection;

********************************************************************************************/

BOOL GridTab::UpdateSection(String_256 *DocumentName)
{
TRACEUSER( "Neville", _T("GridTab::UpdateSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"GridTab::UpdateSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_GRID));	// The Page page identifier
	if (!ok)
		return TRUE;		// page not present

	// Make sure the information field displaying the name of the current document is correct.
	String_256	DocName(_R(IDT_OPTS_GRID_INFO)); 
	DocName +=	*DocumentName;
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), DocName);

	// All units work off the selected document and so we must do nothing if there is no
	// selected document. Check our clas variable to see if this is true or not.
	// Units should have been set up by the caller.
	if (pDocument == NULL)
	{
		GreySection();
		return TRUE;
	}

	// Do the same things as the normal init section code
	InitGridSection();

	return TRUE;
}


/********************************************************************************************

>	BOOL GridTab::UpdateGridSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when we have undone a default grid resize operation and hence need to
				update the default grid's details on the page section of this tab.
				Should only init the tab if the page is present.  
	Errors:		-
	SeeAlso:	GridTab::GreySection; GridTab::UngreySection; GridTab::InitSection;

********************************************************************************************/

BOOL GridTab::UpdateGridSection()
{
TRACEUSER( "Neville", _T("GridTab::UpdateGridSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"GridTab::UpdateGridSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_GRID));	// The Page page identifier
	if (!ok)
		return TRUE;		// page not present

	// All units work off the selected document and so we must do nothing if there is no
	// selected document. Check our clas variable to see if this is true or not.
	// Units should have been set up by the caller.
	if (pDocument == NULL)
	{
		GreySection();
		return (TRUE);
	}

	// Call the normal init section code for the page parts
	return InitGridSection();
}

/********************************************************************************************

>	BOOL GridTab::HandleMsg(DialogMsg* Msg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the Page options tabs messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GridTab::HandleMsg(DialogMsg* Msg)
{
TRACEUSER( "Neville", _T("HandlePageMsg\n"));
	ERROR2IF(Msg == NULL,FALSE,"GridTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"GridTab::HandleMsg called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_GRID));	// The Page page identifier
	if (!ok)
		return TRUE;		// page not present

	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:	// Initialise controls
			GreyStatus = FALSE;	// we are ungreyed by default
			ok = InitSection();
			if (!ok)
				InformError();
			break;
		case DIM_LFT_BN_CLICKED:
		case DIM_SELECTION_CHANGED:
		case DIM_TEXT_CHANGED:
			OptionsTabs::SetApplyNowState(TRUE);
			break;
		default:
			break;
	}
	
	return TRUE;
}  


/******************************************************************************************

>	BOOL GridTab::InitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the Page section of the options dialog box.
				This section includes the:-
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs; UpdateSection;

******************************************************************************************/

BOOL GridTab::InitSection()
{
TRACEUSER( "Neville", _T("InitPageSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"GridTab::InitSection called with no dialog pointer");

//	BOOL ok	= TRUE;			// Flag for whether value set up ok 

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_GRID_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), DocumentName);

	// All units work off the selected document and so we must do nothing if there is no
	// selected document. Check our clas variable to see if this is true or not.
	// Units should have been set up by the caller.
	if (pDocument == NULL)
	{
		GreySection();
		return (TRUE);
	}

	// Set up our tab information
	InitGridSection();

	return TRUE;
}


/******************************************************************************************

>	BOOL GridTab::InitGridSection()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95
	Returns:	TRUE if everything went ok, FALSE if fails
	Purpose:	Reflects the state of the default grid in the grid tab of the options dialog

******************************************************************************************/

BOOL GridTab::InitGridSection()
{
	// validate spread/doc pointers associated with the options tab
	ERROR2IF(pDocument==NULL,FALSE,"GridTab::GetDefaultGridDetails() - pDocument==NULL");
	ERROR2IF(  pSpread==NULL,FALSE,"GridTab::GetDefaultGridDetails() - pSpread==|NULL");

	// get a pointer to the default grid
	NodeGrid* pDefaultGrid = pSpread->FindFirstDefaultGridInSpread();
	ERROR2IF(pDefaultGrid==NULL,FALSE,"GridTab::GetDefaultGridDetails() - pDefaultGrid==NULL");

	// read current grid settings
	double   GridSpacing      = pDefaultGrid->GetMainStep();		// seems to be equivalent
	UINT32     GridSubDivisions = pDefaultGrid->GetSubdivisions();
	UnitType GridUnits        = pDefaultGrid->GetUnits();
	GridType TypeOfGrid       = pDefaultGrid->GetGridType();
	ERROR2IF(GridUnits==NOTYPE,FALSE,"GridTab::GetDefaultGridDetails() - grid units should never be NOTYPE!");
	DocCoord GridOrigin(0,0);
	pDefaultGrid->GetOrigin(&GridOrigin.x, &GridOrigin.y);
	BOOL ok = pSpread->SpreadCoordToPagesCoord(&GridOrigin,GridOrigin);		// make origin page relative

	// reflect these in the gadgets
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	ok = ok && pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_RECTANGULAR), TypeOfGrid==RECTANGULAR);
	ok = ok && pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_ISOMETRIC),   TypeOfGrid==ISOMETRIC);
#endif //webster
	ok = ok && pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_GRIDDIVISIONS), GridSubDivisions);
	ok = ok && pPrefsDlg->SetDimensionUnitGadgetValue(_R(IDC_OPTS_GRIDSPACING), GridUnits, GridSpacing, pDefaultGrid);
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	ok = ok && pPrefsDlg->SetDimensionGadgetValue(_R(IDC_OPTS_ORIGINX), GridOrigin.x, pSpread);
	ok = ok && pPrefsDlg->SetDimensionGadgetValue(_R(IDC_OPTS_ORIGINY), GridOrigin.y, pSpread);
#endif //webster
	// Note the states of the gadgets
	OldGridSpacing      = GridSpacing;
	OldGridSubDivisions = GridSubDivisions;
	OldGridUnits        = GridUnits;
	OldTypeOfGrid       = TypeOfGrid;
	OldOriginX          = GridOrigin.x;
	OldOriginY          = GridOrigin.y;

	return ok;
}	


/******************************************************************************************

>	BOOL	GridTab::InvokeResize(GridResizeInfo *Param)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/2/95
	Inputs:	
	Returns: 	True if managed to change the page/spread  details ok.
	Purpose:	Sets the new details of the page size and of the default grid.
				The default grid should always be resident in the spread.
				Assumes the class variables pDocument, pSpread have been set up.
				This function should be used instead of SetNewSizeOfPage and 
				SetDefaultPageGrid as it is undoable and they aren't.
	Errors:		-
	SeeAlso:	AppPrefsDlg::SetCurrentSpread; SetDefaultPageGrid;

******************************************************************************************/

BOOL GridTab::InvokeResize(GridResizeInfo *pParam)
{
	// For now, get details on 1st page of spread 1

	// The dialog handler should have already set up the class variables pDocument and pSpread.
	// We will check that they are not null.
	ERROR2IF(pDocument==NULL,FALSE,"GridTab::InvokeResize() no selected document");
	ERROR2IF(  pSpread==NULL,FALSE,"GridTab::InvokeResize() no spread to work on");

	// Only change the page size/layout if something has changed 
	// If the user has changed anything then warn them that nothing will happen
	// if no grid params have changed, flag this in the param block, else use them as the new defaults
	MILLIPOINT bodge = 5;		// extra leeway on comparison required
	BOOL ChangeRequired = FALSE;
	if (
		OldGridSpacing == pParam->GridSpacing &&
		OldGridSubDivisions == pParam->GridSubDivisions &&
		OldGridUnits == pParam->GridUnits &&
		OldTypeOfGrid == pParam->TypeOfGrid &&
		(OldOriginX >= (pParam->OriginX - bodge)) && (OldOriginX <= (pParam->OriginX + bodge)) &&
		(OldOriginY >= (pParam->OriginY - bodge)) && (OldOriginY <= (pParam->OriginY + bodge))
	   )
	{
		pParam->GridSpacing      = 0;
		pParam->GridDivisions    = 0;
		pParam->GridSubDivisions = 0;
		pParam->GridUnits        = NOTYPE;
	}
	else
	{
		ChangeRequired 		= TRUE;
		OldGridSpacing      = pParam->GridSpacing;
		OldGridSubDivisions = pParam->GridSubDivisions;
		OldGridUnits        = pParam->GridUnits;
		OldTypeOfGrid       = pParam->TypeOfGrid;
		OldOriginX       	= pParam->OriginX;
		OldOriginY       	= pParam->OriginY;
	}

	if (ChangeRequired)
	{
		// Invoke the Page resizing operation so that we get Undo.
		OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpGridResize));

		if (pOpDesc != NULL)
		{
			// Set up the parameters which we require to do the page resizing operation 
			pParam->pSpread = pSpread;

			// Go for it!
			pOpDesc->Invoke((OpParam *) pParam);
		}
	}

	return TRUE;
}	



////////////////////////// OpGridResize class ... /////////////////////////////////////////

/******************************************************************************************
>	OpGridResize::OpGridResize() : UndoableOperation()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpGridResize constructor (Creates an undoable operation)
	SeeAlso:	GridTab;

******************************************************************************************/

OpGridResize::OpGridResize() : UndoableOperation() 
{
}       

/********************************************************************************************

>	OpGridResize::~OpGridResize() 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpGridResize destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpGridResize::~OpGridResize()
{
}

/******************************************************************************************
>	BOOL OpAlign::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for a Page Resize operation
	SeeAlso:	GridTab;

******************************************************************************************/

BOOL OpGridResize::Init()
{  
	return RegisterOpDescriptor(
		0,								// Tool ID
 		_R(IDS_OPSETDEFAULTGRID),			// String resource ID
		CC_RUNTIME_CLASS(OpGridResize),	// Runtime class
 		OPTOKEN_OPGRIDRESIZE,			// Token string
 		OpGridResize::GetState,			// GetState function
		0,								// Help ID
		0,								// Bubble ID
		0,								// Resource ID
		0								// Control ID
//	needs	'GREY_WHEN_NO_CURRENT_DOC'
	);
}   

/******************************************************************************************

>	OpState	OpGridResize::GetState(String_256* pString, OpDescriptor* pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Purpose:	Returns the OpState of the OpGridResize dialogue operation.
	SeeAlso:	GridTab;

******************************************************************************************/

OpState	OpGridResize::GetState(String_256* pString, OpDescriptor* pOpDesc)
{
	OpState OpSt;

	return(OpSt);
}

/********************************************************************************************

>	void OpGridResize::Do(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		OpGridResize (unused)
	Outputs:	-
	Returns:	-
	Purpose:	(Performs the page resizing operation)
				OpGridResize has a special overloaded Do operator which takes
				parameters describing what is to be done - that version of Do must
				be used
	Errors:		Always generates an ENSURE failure because this Do doesn't.
	SeeAlso:	-

********************************************************************************************/
			
void OpGridResize::Do(OpDescriptor *NotUsed)
{
	ENSURE(FALSE, "OpGridResize does not provide a Do() function - Use DoWithParam");
	End();
}

/******************************************************************************************

>	void OpGridResize::DoWithParam(OpDescriptor* pOpDesc, OpParam* pResizeInfo)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		pOpDesc     - unused
				pResizeInfo - points to a Page resize structure with parameters:
	Purpose:	Performs the grid resize operation via DoResizeGrid()

******************************************************************************************/

void OpGridResize::DoWithParam(OpDescriptor* pOp, OpParam* pResizeInfo)
{
	if (DoResizeGrid((GridResizeInfo*)pResizeInfo)==FALSE)
		FailAndExecute();
	End();
}


/******************************************************************************************

>	BOOL OpGridResize::DoResizeGrid(GridResizeInfo* pResizeInfo)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		pResizeInfo - info describing how to change grid
							  note origin is page relative and specified as 2 ordinates - why!
	Returns:	FALSE if fails
	Purpose:	Undoably performs the grid resize operation
				Forces a redraw of the parent document for the specified spread.

******************************************************************************************/

BOOL OpGridResize::DoResizeGrid(GridResizeInfo* pInfo)
{
	ERROR2IF(  pInfo==NULL,FALSE,"OpGridResize::DoResizeGrid() - pInfo==NULL");
	Spread* pSpread = pInfo->pSpread;
	ERROR2IF(pSpread==NULL,FALSE,"OpGridResize::DoResizeGrid() - pSpread==NULL");

	// If the grid is the same as last time then we want to do nothing
	// This apparently is signalled by the GridDivisions and GridSubDivisions being 0!
	if (pInfo->GridDivisions==0 || pInfo->GridSubDivisions==0)
		return TRUE;

	// find the present default grid and read it's settings for the undo action
	NodeGrid* pDefaultGrid = pSpread->FindFirstDefaultGridInSpread();
	ERROR2IF(pDefaultGrid==NULL,FALSE,"OpGridResize::DoResizeGrid() - pDefaultGrid==NULL");

	double     PresentGridDivisions    = pDefaultGrid->GetDivisions();
	UINT32       PresentGridSubDivisions = pDefaultGrid->GetSubdivisions();
	UnitType   PresentGridUnits        = pDefaultGrid->GetUnits();
	GridType   PresentGridType         = pDefaultGrid->GetGridType();

	DocCoord   PresentOrigin(0,0);
	pDefaultGrid->GetOrigin(&PresentOrigin.x, &PresentOrigin.y);
	BOOL ok=pSpread->SpreadCoordToPagesCoord(&PresentOrigin,PresentOrigin);	// make page relative
	if (!ok) return FALSE;

	// Create an action to Undo the resizing operation
	ActionSetDefaultGrid *ASDG;
	ActionCode SetDefaultGridAction= ActionSetDefaultGrid::Init(this,
																&UndoActions,
	 															sizeof(ActionSetDefaultGrid),
																pSpread,
																PresentGridDivisions,
																PresentGridSubDivisions,
																PresentGridUnits,
																PresentGridType,
																PresentOrigin.x,
																PresentOrigin.y,
																(Action**)(&ASDG)
																);
	ERROR2IF(SetDefaultGridAction==AC_FAIL,FALSE,"OpGridResize::DoResizeGrid() - ActionSetDefaultGrid::Init() failed");

	// Check if we must change grid types, if so, replace it with the correct type
	if (PresentGridType!=pInfo->TypeOfGrid)
	{
		// Create a grid of the new type
		NodeGrid* pNewGrid = NULL;
		switch (pInfo->TypeOfGrid)
		{
			case RECTANGULAR : ALLOC_WITH_FAIL(pNewGrid,(new NodeGridRect()),this); break;
			case ISOMETRIC   : ALLOC_WITH_FAIL(pNewGrid,(new NodeGridIso()), this); break;
			case INVALID_GRID_TYPE: ERROR3("Use of invalid grid type"); break;
		}
		ERROR2IF(pNewGrid==NULL,FALSE,"OpGridResize::DoResizeGrid() - failed to create new grid");

		// The grid will cover the entire spread pasteboard area
		DocRect GridRect = pSpread->GetPasteboardRect(FALSE);
		pSpread->DocCoordToSpreadCoord(&GridRect);

		// Attach the new grid as the last child of the specified spread
		pNewGrid->AttachNode(pSpread, LASTCHILD); 
		pNewGrid->SetDefault(TRUE);					// Its a default grid
		pNewGrid->SetBoundingRect(GridRect);

		// Now replace the old grid with the new one
		pDefaultGrid->CascadeDelete();
		pDefaultGrid = pNewGrid;
	}

	// the grid may have been replaced with a new one so must set up all remaining values
	DocCoord Origin(pInfo->OriginX, pInfo->OriginY);
	ok = pSpread->PagesCoordToSpreadCoord(&Origin,Origin);	// convert from page relaive to absolute
	if (ok) pDefaultGrid->SetOrigin(Origin.x,Origin.y);
	if (ok) ok = pDefaultGrid->SetGridParams(pInfo->GridDivisions, pInfo->GridSubDivisions, pInfo->GridUnits);
	if (!ok) return FALSE;
	
	// since grid changed sucessfully, force redraw doc and broadcast message (eg for rulers)
	Document* pParentDoc = (Document*)pSpread->FindOwnerDoc();
	ERROR2IF(pParentDoc==NULL,FALSE,"OpGridResize::DoResizeGrid() - pParentDoc==NULL");
	pParentDoc->ForceRedraw();
	BROADCAST_TO_ALL( OptionsChangingMsg(pParentDoc,OptionsChangingMsg::NEWDEFAULTGRID) );

	return TRUE;
}


/********************************************************************************************

>	ActionSetDefaultGrid::ActionSetDefaultGrid()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/2/95
	Purpose:	ActionSetDefaultGrid constructor

********************************************************************************************/
    
ActionSetDefaultGrid::ActionSetDefaultGrid()
{
}



/********************************************************************************************

>	ActionSetDefaultGrid::~ActionSetDefaultGrid() 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/2/95
	Purpose:	ActionSetDefaultGrid destructor

********************************************************************************************/

ActionSetDefaultGrid::~ActionSetDefaultGrid()
{
}



/********************************************************************************************

>	virtual ActionCode ActionSetDefaultGrid::Execute()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/95
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the ActionSetDefaultGrid to go and set up a new default grid in the
				specified spread and generates another ActionSetDefaultGrid to undo this change.
				Forces a redraw of the parent document of the specified spread.

********************************************************************************************/

ActionCode ActionSetDefaultGrid::Execute()
{
	ActionSetDefaultGrid SetDefaultGridAct;
	ActionCode ActCode = AC_OK;

	// Get the details on the present default grid layout so that we can use this to undo
	// what we are about to try and do
		
	double PresentGridDivisions = 0;
	UINT32 PresentGridSubDivisions = 0;
	UnitType PresentGridUnits = NOTYPE;
	GridType PresentGridType = RECTANGULAR;
	NodeGrid* pGrid = NULL; 
	MILLIPOINT	PresentOriginX = 0;
	MILLIPOINT	PresentOriginY = 0;

	if (pSpread != NULL)
	{
			pGrid = pSpread->FindFirstDefaultGridInSpread();
			ERROR3IF(pGrid == NULL,"ActionSetDefaultGrid::Execute() no current default grid");

			PresentGridDivisions = pGrid->GetDivisions();				 
			PresentGridSubDivisions = pGrid->GetSubdivisions();
			PresentGridUnits = pGrid->GetUnits();
			PresentGridType	= pGrid->GetGridType();

			// Grid origin should be made relative to bottom left hand corner of page union
			DocRect PagesRect;
			/*BOOL ok =*/ pSpread->GetPagesRect(&PagesRect);
			pGrid->GetOrigin(&PresentOriginX, &PresentOriginY);
			// Make relative to bottom left hand corner of page union
			PresentOriginX = PresentOriginX - PagesRect.lo.x;
			PresentOriginY = PresentOriginY - PagesRect.lo.y;
TRACEUSER( "Neville", _T("ActionSetDefaultGrid::Execute starting grid origin at %d, %d\n"), PresentOriginX, PresentOriginY);
	}

     // Create an action to restore the changes we are about to make
    if ((ActCode = ActionSetDefaultGrid::Init(pOperation,
								     		pOppositeActLst,
								     		sizeof(ActionSetDefaultGrid),
											pSpread,
											PresentGridDivisions,
											PresentGridSubDivisions,
											PresentGridUnits,
											PresentGridType,
											PresentOriginX,
											PresentOriginY,
						 			 		( Action**)(&SetDefaultGridAct))) != AC_FAIL)
	{
		// Need to do something here
		if ((pSpread != NULL) && (pGrid != NULL)) 
		{
			// Check if we must change grid types
			if (PresentGridType != TypeOfGrid)
			{
				NodeGrid* pNewGrid = NULL;

				// Work out the size required for the new grid
				DocRect Rect = pSpread->GetPasteboardRect(FALSE);
				pSpread->DocCoordToSpreadCoord(&Rect);

				// Create a grid of the new type
				switch (TypeOfGrid)
				{
					case RECTANGULAR 	: ALLOC_WITH_FAIL(pNewGrid,(new NodeGridRect()),pOperation); break;
					case ISOMETRIC 		: ALLOC_WITH_FAIL(pNewGrid,(new NodeGridIso()), pOperation); break;
					case INVALID_GRID_TYPE: ERROR3("Use of invalid grid type"); break;
				}

				if (pNewGrid != NULL)
				{
					// Attach the new grid as the last child of the specified spread
					pNewGrid->AttachNode(pSpread, LASTCHILD); 
					pNewGrid->SetDefault(TRUE);			// Its a default grid
					pNewGrid->SetBoundingRect(Rect);  	// Tell the NodeGrid of the new bounds

					// Now delete the old grid
					pGrid->CascadeDelete();

					// Make the present grid the new one so that we can set the new parameters  
					pGrid = pNewGrid;
				}
				else
				{
					// We failed to create the new grid so return that failure
					ActCode = AC_FAIL;
				}
			}


			// Set up those new grid parameters
			if (ActCode != AC_FAIL)
			{
				// Grid system works in divisions, which are the number of units between the
				// main grid points.
				pGrid->SetGridParams(GridDivisions, GridSubDivisions, GridUnits);
				
				// Must set the origin of the grid to the bottom left corner of the
				// union rectangle of all pages on the spread as this is where the x,y
				// measurements are made from.
				DocRect PagesRect;
				BOOL ok = pSpread->GetPagesRect(&PagesRect);
				if (ok)
				   pGrid->SetOrigin(PagesRect.lo.x + OriginX, PagesRect.lo.y + OriginY);

				// Force all views onto the document to be updated 
				// Also, tell the options dialog box that something has changed
				// Assume spread is in the selected document
				Document *pParentDoc = (Document *)pSpread->FindOwnerDoc();
				if (pParentDoc != NULL)
				{
					pParentDoc->ForceRedraw();
					BROADCAST_TO_ALL( OptionsChangingMsg(pParentDoc,OptionsChangingMsg::NEWDEFAULTGRID) );
				}
			}
		}
	}

	return (ActCode);
}



/********************************************************************************************

>	ActionCode ActionSetDefaultGrid::Init(Operation* const pOp, 
										ActionList* pActionList, 	
							  			UINT32 ActionSize,
										Spread *pSpread,
										double NewGridDivisions,
										UINT32 NewGridSubDivisions,
										UnitType NewGridUnits,
										GridType NewGridType,
										MILLIPOINT NewOriginX,
										MILLIPOINT NewOriginY,
				  						Action** NewAction)
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/95

	Inputs:		pOp: The operation to which the action should be added
				pActionList: The action list in the operation object
				ActionSize:  The size of the action in bytes. This should be the total  
							 size of the action (including any objects pointed to by the
							 action).
				pSpread:	 The target spread which will be changed when we are executed
				NewGridDivisions: 	The new number of grid divisions
				NewGridSubDivisions: The new number of grid subdivisions
				NewGridUnits:	 	The new grid units
				NewGridType:	 	The new type of grid required, either Isometric or Rectangular
				NewOriginX:			The new origin x coordinate to use
				NewOriginY:			The new origin y coordinate to use

 	Outputs:    NewAction:   A pointer to the action if it could be allocated. 

	Returns:	AC_FAIL:     There was not enough room in the operation history for the
							 action and the user did not wish to continue. Usually
							 End() should be called in this situation.

				AC_NORECORD: There was not enough room in the operation history for
							 the action, but the user requested that he wished to 
							 continue without undo. 

				AC_OK      : The action was successfully initialised and added to the 
							 operation. 
				         
				           
	Purpose:	To check that there is sufficient room for the action in the operation
				history, and if there is, then to add the action to the operations 
				action list. 
				
				The function calls the Action::Init function passing the runtime class 
				of an ActionSetDefaultGrid.
	Errors:		-
	SeeAlso:	Action::Init; OpSetDefaultGrid; GridTab::SetNewSizeOfPage; 

********************************************************************************************/
   
ActionCode ActionSetDefaultGrid::Init(Operation* const pOp, 
									ActionList* pActionList, 	
						  			UINT32 ActionSize,
									Spread *pTheSpread,
									double NewGridDivisions,
									UINT32 NewGridSubDivisions,
									UnitType NewGridUnits,
									GridType NewGridType,
									MILLIPOINT NewOriginX,
									MILLIPOINT NewOriginY,
				  					Action** NewAction)
{
	ActionCode Ac = (Action::Init(pOp,
									pActionList,
									ActionSize,
									CC_RUNTIME_CLASS(ActionSetDefaultGrid), 
									NewAction));
	if (*NewAction != NULL)
	{
		ActionSetDefaultGrid *ACC = (ActionSetDefaultGrid*) (*NewAction);
		ACC->pSpread  = pTheSpread;
		ACC->GridDivisions = NewGridDivisions;
		ACC->GridSubDivisions = NewGridSubDivisions;
		ACC->GridUnits = NewGridUnits;
		ACC->TypeOfGrid = NewGridType;
		ACC->OriginX = NewOriginX;
		ACC->OriginY = NewOriginY;
	}

	return (Ac);
}
