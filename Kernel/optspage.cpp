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
// Implementation of the Page tab of the options dialog box

/*

*/

#include "camtypes.h"
#include "appprefs.h"
//#include "prefsdlg.h"	// dialog/gadget ids
//#include "optsres.h"	// error strings
#include "optspage.h"
#include "optsmsgs.h"
//#include "app.h"		// Camelot object - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h"	// document object - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "spread.h"		// spread object - in camtypes.h [AUTOMATICALLY REMOVED]
#include "page.h"		// page object
//#include "units.h"		// units - in camtypes.h [AUTOMATICALLY REMOVED]
#include "pagesize.h"	// Page sizes available e.g. A4
//#include "trans2d.h"	// 2d translation matrix - in camtypes.h [AUTOMATICALLY REMOVED]
#include "layer.h"
#include "progress.h"	// Begin/EndSlowJob

CC_IMPLEMENT_DYNAMIC(PageTab, OptionsTabs)   

CC_IMPLEMENT_DYNCREATE(OpPageResize, UndoableOperation)   
CC_IMPLEMENT_DYNCREATE(ActionPageResize, Action)

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

/********************************************************************************************

>	PageTab::PageTab() 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PageTab constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

PageTab::PageTab()
{   
	// Do nothing for now
	InitedPageSizeList = FALSE;
}        

/********************************************************************************************

>	PageTab::~PageTab()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PageTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
PageTab::~PageTab()
{   
}        

/********************************************************************************************

>	BOOL PageTab::Init()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	PageTab initialisation routine
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
BOOL PageTab::Init()
{   
	// Init (register) all our undoable operations
	if (!OpPageResize::Init())
		return(FALSE);

	return TRUE;
}        


/******************************************************************************************

>	CDlgResID PageTab::GetPageID()()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CDlgResID PageTab::GetPageID()
{
	return _R(IDD_OPTSTAB_PAGE);
}

/******************************************************************************************

>	BOOL PageTab::IsDocumentOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a document option.	  
	Purpose:	Allows the document options dialog code to determine if this tab belongs
				to the group of document options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL PageTab::IsDocumentOption()
{
	return TRUE;	// This tab is not a document option 
}

/******************************************************************************************

>	BOOL PageTab::IsProgramOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a program option.	  
	Purpose:	Allows the program options dialog code to determine if this tab belongs
				to the group of program options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL PageTab::IsProgramOption()
{
	return TRUE;	// This tab is a program option 
}


/******************************************************************************************

>	BOOL AppPrefsDlg::CommitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Page section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PageTab::CommitSection()
{
TRACEUSER( "Neville", _T("commit page section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PageTab::CommitSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_PAGE));	// The Page page identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// All units work off the selected document and so we must do nothing if there is no
	// selected document. Check our clas variable to see if this is true or not.
	// Units should have been set up by the caller.
	if (pDocument == NULL)
		return (TRUE);

	// Ok has been pressed so take the values from this section of the dialog box
	BOOL Valid=0;		// Flag for validity of value
//	BOOL State=0;		// Flag for state of button/switch
//	BOOL SetOk=0;		// Preference value set ok



	// Section = Page

// WEBSTER Neville 13/8/97
#ifdef WEBSTER
	// Webster has a limited set of controls. So use the present settings on the page and just
	// allow the user to alter the few parameters that they can.

	// Find out the details on the presently selected page/spread
	MILLIPOINT	Width		= 0;
	MILLIPOINT	Height		= 0;
	MILLIPOINT	Margin		= 0;
	MILLIPOINT	Bleed		= 0;
	BOOL		Portrait	= TRUE;
	BOOL		Dps			= FALSE;
	BOOL		DropShadow	= FALSE;

	INT32 PageSizeId =0; 
	
	ok = GetSizeOfPage(&Width, &Height, &Margin, &Bleed, &Portrait, &Dps, &DropShadow);
	if (!ok)
		return FALSE;
TRACEUSER( "Neville", _T("Currently Width =%d Height =%d Margin=%d Bleed=%d\n"), Width, Height, Margin, Bleed);

#else
	// In Camelot mode, the user can change everything

	// First, get the current state of Portrait button
	BOOL Portrait = FALSE;
	Portrait = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_PORTRAIT), 0, 1, 0, &Valid);

	// Next, get the current values for the page width and height.
	// If custom use the editable fields otherwise use the actual page size from
	// the definition otherwise may encounter conversion errors.
	MILLIPOINT Width = 0;
	MILLIPOINT Height = 0;

	INT32 PageSizeSelected = CUSTOM;
	PageSizeSelected = pPrefsDlg->GetSelectedValueIndex(_R(IDC_OPTS_SIZELIST));

	if (PageSizeSelected == CUSTOM)
#endif //webster
	{
		// Portrait and landscape radio buttons should have been taken care of by the switch
		// changing the editable fields which we are just about to read. 
		Height = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_HEIGHT), CurrentPageUnits,
												  MinPageDimension, MaxPageDimension,
												  _R(IDE_OPTS_INVALIDHEIGHT), &Valid);
		if (!Valid)
			return FALSE; 			// height incorrect, user has been warned already

		Width  = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_WIDTH), CurrentPageUnits,
												  MinPageDimension, MaxPageDimension,
												  _R(IDE_OPTS_INVALIDWIDTH), &Valid);
		if (!Valid)
			return FALSE; 			// width incorrect, user has been warned already
	}
// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	else
	{
		if (AppPrefsDlg::pPageSizesList != NULL && InitedPageSizeList)
		{
			// We will need to take account of the Portrait and landscape radio buttons and
			// hence swapping of the width and height fields as we are reading the values
			// directly from the sizes list.
			PageId PageChosen = (PageId)PageSizeSelected;
			AppPrefsDlg::pPageSizesList->GetSizeFromPageId(PageChosen, &Width, &Height);
TRACEUSER( "Neville", _T("PageTab::CommitSection width %d height %d in MP\n"),Width,Height);
			
			// Now we have the width/height, check if landscape is selected and if so
			// swap the height and width over.
			if (!Portrait)
			{
				// Swap the height and width fields over.
				MILLIPOINT Temp = Width;
				Width = Height;
				Height = Temp;
			}
		}
		else
		{
			ERROR2IF(TRUE,FALSE,"Bad page sizes list in PageTab::CommitSection"); 
		}
	}

	// Now the page margin size
	// Limit the page margin to much less than the page size as things like zoom to 
	// spread/drawing assume that part of the spread is visible in the view so that
	// this can then be used to pick up the desired spread. 
	MILLIPOINT Margin = 0; // MinPageDimension;
	MILLIPOINT MaxPageMargin = MaxPageDimension;
	Margin = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_MARGIN), CurrentPageUnits,
										 	 MinPageDimension, MaxPageMargin,
										 	 0, &Valid);
	if (Margin < 0 || Margin >= MaxPageDimension) // ignore valid as it will be false
	{
		InformError(_R(IDE_OPTS_INVALIDMARGIN));
		return FALSE; 			// margin incorrect, user has been warned already
	}

	// Now the page bleed size
	MILLIPOINT Bleed = 0;
	BOOL BleedEnabled;
	BleedEnabled = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_BLEEDSWITCH), 0, 1, 0, &Valid);
	if (BleedEnabled)
	{
		const MILLIPOINT MaxBleed = (MILLIPOINT)(3 * 72000);	// maximum in inches
		Bleed = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_BLEED), CurrentPageUnits,
											 	 0, MaxBleed, _R(IDE_OPTS_INVALIDBLEED), &Valid);
		if (!Valid)
			return FALSE; 			// bleed incorrect, user has been warned already
	}
	else
		Bleed = 0;

	// Get current state of the double page spread button
	BOOL Dps = FALSE;
	Dps = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_DPS), 0, 1, 0, &Valid);

	// Get the current state of the drop shadow switch
	BOOL DropShadow = FALSE;
	DropShadow = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_SHADOW), 0, 1, 0, &Valid);
#endif //webster

	// Check pasteboard dimensions

	// Possibly should get the resize operation to check this and pass back a result, as
	// loading dodgy documents might be possibly another way to create completely large pages.

	// Check if the overall size of the pasteboard will break our maximum dimensions
	MILLIPOINT PagesWidth = Width;
	if (Dps)
		PagesWidth += Width;	// If double page spread then make two page wide
	MILLIPOINT PasteBoardWidth = Margin + PagesWidth + Margin;
	MILLIPOINT PasteBoardHeight = Margin + Height + Margin;
	if (PasteBoardWidth > MaxPageDimension || PasteBoardHeight > MaxPageDimension)
	{
		// There is a problem so warn the user that the value is incorrect
		InformWarning(_R(IDE_OPTS_OVERALLSPREADSIZE));
		return FALSE;
	}



	// Go and make those requested changes

	// Now that we have the required information, now go and do it
	// Set up the parameters which we require to do the page resizing operation 
	// This is undoable and redoable
	PageResizeInfo Param;

	// Set up the new page parameters that we want
	Param.Height         = Height;
	Param.Width          = Width;
	Param.Margin         = Margin;
	Param.Bleed          = Bleed;
	Param.Portrait       = Portrait;
	Param.Dps            = Dps;
	Param.ShowDropShadow = DropShadow;

	InvokeResize(&Param);

	return ok;
}

/******************************************************************************************

>	BOOL PageTab::NewUnitsInSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/1/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Function called when new default units come into operation. We must convert
				all units fields to display in the newly specified units.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PageTab::NewUnitsInSection()
{
TRACEUSER( "Neville", _T("New units in page section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PageTab::NewUnitsInSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_PAGE));	// The page tab identifier
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
	InitPageSection();

	return TRUE;
}


/********************************************************************************************

>	BOOL PageTab::GreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents 
	Errors:		-
	SeeAlso:	PageTab::UngreySection;

********************************************************************************************/

BOOL PageTab::GreySection()
{
TRACEUSER( "Neville", _T("GreySection in page section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PageTab::GreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_PAGE));	// The page tab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_PAGE_INFO)); 
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

>	BOOL PageTab::UngreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document 
	Errors:		-
	SeeAlso:	PageTab::UngreySection;

********************************************************************************************/

BOOL PageTab::UngreySection()
{
TRACEUSER( "Neville", _T("UngreySection in page section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PageTab::UngreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_PAGE));	// The page tab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_PAGE_INFO)); 
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

>	BOOL PageTab::ChangeControlStatus(const BOOL Status)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 
	Errors:		-
	SeeAlso:	PageTab::GreySection;	PageTab::UngreySection;

********************************************************************************************/

BOOL PageTab::ChangeControlStatus(const BOOL Status)
{
	// Grey/Ungrey the controls
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_INFO), Status);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_PAGEGROUP), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SIZENAME), Status);
	pPrefsDlg->EnableGadget(_R(IDN_PAGESIZE_CUSTOM), Status);
// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SIZELIST), Status);
#endif //webster
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_HEIGHTTXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_HEIGHT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_WIDTHTXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_WIDTH), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_ORIENTGROUP), Status);

// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_PORTRAIT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_LANDSCAPE), Status);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SPREADGROUP), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_MARGINTXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_MARGIN), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_BLEEDSWITCH), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_BLEED), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_BLEEDTXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_DPS), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SHADOW), Status);
#endif //webster
	return TRUE;
}			

/********************************************************************************************

>	BOOL PageTab::UpdateSection(String_256 *DocumentName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/95
	Inputs:		Name of document
	Outputs:	-
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. Should only init the tab if the page is present.  
				The document name allows the info field on the tab to be filled in correctly.
	Errors:		-
	SeeAlso:	PageTab::GreySection; PageTab::UngreySection; PageTab::InitSection;

********************************************************************************************/

BOOL PageTab::UpdateSection(String_256 *DocumentName)
{
TRACEUSER( "Neville", _T("PageTab::UpdateSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PageTab::UpdateSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_PAGE));	// The Page page identifier
	if (!ok)
		return TRUE;		// page not present

	// Do everything the same as the InitSection but do not init the page size list as this
	// causes a huge redraw.
	
	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocName(_R(IDT_OPTS_PAGE_INFO)); 
	DocName +=	*DocumentName;
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), DocName);


	// All units work off the selected document and so we must do nothing if there is no
	// selected document. Check our clas variable to see if this is true or not.
	// Units should have been set up by the caller.
	if (pDocument == NULL)
	{
		// If returns False then we are in an invalid situation and so should make sure that
		// the dialog box is greyed.
		GreySection();
		return TRUE;
	}

	// Check if we have an uninitialised page sizes list
	if (!InitedPageSizeList)
	{
		// Set up the list of available page size options, starting with the default.
		// Handle everything that we do not know as a CUSTOM setting.
		SetUpPageSizeList();
	}

	// Do the same things as the normal init section code
	InitPageSection();

	return TRUE;
}

/********************************************************************************************

>	BOOL PageTab::UpdatePageSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when we have undone a page resize operation and hence need to update the 
				page's details on the page section of this tab.
				Should only init the tab if the page is present.  
	Errors:		-
	SeeAlso:	PageTab::GreySection; PageTab::UngreySection; PageTab::InitSection;

********************************************************************************************/

BOOL PageTab::UpdatePageSection()
{
TRACEUSER( "Neville", _T("PageTab::UpdatePageSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PageTab::UpdatePageSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_PAGE));	// The Page page identifier
	if (!ok)
		return TRUE;		// page not present

	// Do everything the same as the InitSection but do not init the page size list as this
	// causes a huge redraw.

	// All units work off the selected document and so we must do nothing if there is no
	// selected document. Check our clas variable to see if this is true or not.
	// Units should have been set up by the caller.
	if (pDocument == NULL)
	{
		// If returns False then we are in an invalid situation and so should make sure that
		// the dialog box is greyed.
		GreySection();
		return (TRUE);
	}
	
	// Check if we have an uninitialised page sizes list
	if (!InitedPageSizeList)
	{
		// Remove any lists that we have created.
		//pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_SIZELIST));
		//InitedPageSizeList = FALSE;

		// Set up the list of available page size options, starting with the default.
		// Handle everything that we do not know as a CUSTOM setting.
		SetUpPageSizeList();
	}

	// Call the normal init section code for the page parts
	return InitPageSection();
}


/********************************************************************************************

>	BOOL PageTab::HandleMsg(DialogMsg* Msg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the Page options tabs messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL PageTab::HandleMsg(DialogMsg* Msg)
{
TRACEUSER( "Neville", _T("HandlePageMsg\n"));
	ERROR2IF(Msg == NULL,FALSE,"PageTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PageTab::HandleMsg called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_PAGE));	// The Page page identifier
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
		case DIM_SELECTION_CHANGED:
			OptionsTabs::SetApplyNowState(TRUE);
	// WEBSTER-ranbirr-13/11/96
	#ifndef WEBSTER
			if (Msg->GadgetID == _R(IDC_OPTS_SIZELIST))
			{
				// Get the currently selected item and set up the other fields accordingly 
				INT32 PageSizeSelected;
				PageSizeSelected = pPrefsDlg->GetSelectedValueIndex(_R(IDC_OPTS_SIZELIST));
				// Get whether we are in Portrait or Landscape mode
				BOOL Portrait;
				BOOL Valid;
				Portrait = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_PORTRAIT), 0, 1, 0, &Valid);
				HandlePageList(PageSizeSelected, Portrait);
				break;
			}
		case DIM_LFT_BN_CLICKED:
			OptionsTabs::SetApplyNowState(TRUE);
			if (FALSE) {}
			// WEBSTER-ranbirr-13/11/96
			#ifndef WEBSTER
			else if ((Msg->GadgetID == _R(IDC_OPTS_PORTRAIT)) || (Msg->GadgetID == _R(IDC_OPTS_LANDSCAPE)))
				// Portrait landscape buttons toggle the width/height fields
				HandlePortraitLandscape();
			#endif // Webster
			else if (Msg->GadgetID == _R(IDC_OPTS_BLEEDSWITCH))
			{
				// Bleed switch enables or disables the bleed size field 
				BOOL BleedEnabled;
				BOOL Valid;
				BleedEnabled = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_BLEEDSWITCH), 0, 1, 0, &Valid);
				if (BleedEnabled)
				{
					pPrefsDlg->EnableGadget(_R(IDC_OPTS_BLEED), TRUE);
					pPrefsDlg->EnableGadget(_R(IDC_OPTS_BLEEDTXT), TRUE);
				}
				else
				{
					pPrefsDlg->EnableGadget(_R(IDC_OPTS_BLEED), FALSE);
					pPrefsDlg->EnableGadget(_R(IDC_OPTS_BLEEDTXT), FALSE);
				}													
			}
	#endif //webster	
		case DIM_TEXT_CHANGED:
			OptionsTabs::SetApplyNowState(TRUE);
			break;
		default:
			break;
	}
	
	return TRUE;
}  


/******************************************************************************************

>	BOOL PageTab::InitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the Page section of the options dialog box.
				This section includes the:-
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs; UpdateSection;

******************************************************************************************/

BOOL PageTab::InitSection()
{
TRACEUSER( "Neville", _T("InitPageSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PageTab::InitSection called with no dialog pointer");
	ERROR2IF(AppPrefsDlg::pPageSizesList == NULL,FALSE,"PageTab::InitSection called with list of page sizes");

//	BOOL ok	= TRUE;			// Flag for whether value set up ok 

	// We have an uninitialised page sizes list, so flag that before we have a chance to exit
	pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_SIZELIST));
	InitedPageSizeList = FALSE;

	// Do everything the same as the UpdateSection but init the page size list for this
	// once and only time to stop redraws.

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_PAGE_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), DocumentName);


	// All units work off the selected document and so we must do nothing if there is no
	// selected document. Check our clas variable to see if this is true or not.
	// Units should have been set up by the caller.
	if (pDocument == NULL)
	{
		// If returns False then we are in an invalid situation and so should make sure that
		// the dialog box is greyed.
		GreySection();
		return (TRUE);
	}

	// Set up the list of available page size options, starting with the default.
	// Handle everything that we do not know as a CUSTOM setting.
	SetUpPageSizeList();

	InitPageSection();

	return TRUE;
}

/******************************************************************************************

>	BOOL PageTab::SetUpPageSizeList()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/95
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets up the pages sizes list in the Page tab on the	options	dialog box.
				Done separately to InitPageSection so that code is modular and should only
				be used once when the page is set up and not on updates otherwise you get
				the drop list causing a huge redraw problem. 
	Errors:		-
	SeeAlso:	InitSection; InitPageSection; AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL PageTab::SetUpPageSizeList()
{
	// already set this up so don't do it again
	if (InitedPageSizeList)
		return TRUE;

	// Set up the list of available page size options, starting with the default.
	// Handle everything that we do not know as a CUSTOM setting.
// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_SIZELIST), String_32(_R(IDN_PAGESIZE_CUSTOM)));
#endif //webster

	// Now go through the defined list
	PageSize* pPageSize = (PageSize*) AppPrefsDlg::pPageSizesList->GetFirstPageSize();

	while (pPageSize != NULL)
	{
		// Get the name of the current page size item
		String_32 *pPageName = pPageSize->GetPageName();
// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
		pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_SIZELIST), *pPageName);
#endif //webster
		// skip to the next item, if there is one
		pPageSize = (PageSize*) AppPrefsDlg::pPageSizesList->GetNextPageSize(pPageSize);
	}
// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	pPrefsDlg->SetComboListLength(_R(IDC_OPTS_SIZELIST));
#endif //webster
	// It is now set up and correct
	InitedPageSizeList = TRUE;

	return TRUE;
}	

/******************************************************************************************

>	BOOL PageTab::InitPageSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/95
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the page details section of the Page tab on the
				options	dialog box.
	Errors:		-
	SeeAlso:	InitSection; AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL PageTab::InitPageSection()
{
	BOOL ok = TRUE;

	// Section = Page

	// List size done elsewhere	so that only ever done once and not on updates as well.

	// Find out the details on the presently selected page/spread
	MILLIPOINT Width = 0;
	MILLIPOINT Height = 0;
	MILLIPOINT Margin = 0;
	MILLIPOINT Bleed = 0;
	PortraitSelected = TRUE;
	BOOL Dps = FALSE;
// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	BOOL PageShadow = TRUE;
#else
	BOOL PageShadow = FALSE;
#endif //webster

	INT32 PageSizeId =0; 
	
	ok = GetSizeOfPage(&Width, &Height, &Margin, &Bleed, &PortraitSelected, &Dps, &PageShadow);
	if (!ok)
		return FALSE;
TRACEUSER( "Neville", _T("Width =%d Height =%d Margin=%d Bleed=%d\n"), Width, Height, Margin, Bleed);

	PageSizeId = GetPageSizeItem(Width, Height);
TRACEUSER( "Neville", _T("PageSize =%d \n"), PageSizeId);
	// Set up the default option displayed
// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	ok = pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_SIZELIST),PageSizeId);

	// Get the current portrait/landscape state orientation
	if ( PortraitSelected != FALSE )
	{
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_PORTRAIT), TRUE);
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_LANDSCAPE), FALSE);
	}
	else
	{
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_PORTRAIT), FALSE);
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_LANDSCAPE), TRUE);
	}
#endif //webster
	// Now set up and enable/disable the height and width fields accordingly
	// If not Portrait i.e. Landscape then swap the width and the height fields over 
	if (PortraitSelected == FALSE)
	{
		// Swap width for height
		MILLIPOINT Temp = Width;
		Width = Height;
	 	Height = Temp;
	}	
	// Now set up the height and width fields accordingly
	ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_HEIGHT), CurrentPageUnits, Height, FALSE, -1);
	ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_WIDTH), CurrentPageUnits, Width, FALSE, -1);
	BOOL Enable;
// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	if (PageSizeId == CUSTOM)
		Enable = TRUE;
	else
		Enable = FALSE;	
#endif //webster
	// Enable should be True if the custom page size has been selected.
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_HEIGHT), Enable);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_WIDTH), Enable);
// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	// Now show the current page Margin
	ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_MARGIN), CurrentPageUnits, Margin, FALSE, -1);
#endif //webster
	// Now show the current page bleed, setting or unsetting the switch accordingly
	// and greying/ungreying the size field accordingly.
	BOOL BleedEnabled;
	if (Bleed == 0)
	{
		// No bleed
		BleedEnabled = FALSE;
	}
	else
	{
		// Bleed present
		BleedEnabled = TRUE;
	}
// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_BLEEDSWITCH), BleedEnabled);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_BLEED), BleedEnabled);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_BLEEDTXT), BleedEnabled);
#endif //webster
	// If the bleed is zero and disabled then fill in a default value of 3mm
	if ((Bleed == 0) && (!BleedEnabled))
	{
// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
		Bleed = (MILLIPOINT)(3 * MM_MP_VAL);	// 3mm in millipoints
#else
		Bleed = 0;
#endif //webster
	}

	// Now show the current value
	// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_BLEED), CurrentPageUnits, Bleed, FALSE, -1);
	// Now for the double page spread button
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_DPS), Dps);
	// Now for the show page shadow
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_SHADOW), PageShadow);
#endif //webster

	return TRUE;
}

/******************************************************************************************

>	INT32	PageTab::GetPageSizeItem(const MILLIPOINT Width, const MILLIPOINT Height)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Inputs:		Width	width of current page in millipoints
				Height	height of current page in millipoints
	Outputs:	Type of page found according to position on list
	Returns:	-
	Purpose:	Translates a width or height into a number which specifies whether it
				is A0, A1 ... or custom which corresponds to the list offered to the
				user. 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

INT32	PageTab::GetPageSizeItem(const MILLIPOINT Width, const MILLIPOINT Height)
{    
	PageId ItemInList = CUSTOM;		// set useful default of CUSTOM

	if (AppPrefsDlg::pPageSizesList != NULL)
	{
		// Use the PageSize and PageSizesList classes to work out if this is a known page
		// size or if it is regarded as a custom one.
		ItemInList = AppPrefsDlg::pPageSizesList->GetPageIdFromSize(Width, Height);
TRACEUSER( "Neville", _T("PageTab::GetPageSizeItem width %d height %d give item = %d\n"),Width,Height,ItemInList);
	}

	return (INT32) ItemInList; 	
}

/******************************************************************************************

>	BOOL	PageTab::HandlePageList(INT32 ItemClicked, BOOL Portrait)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Inputs:		ItemClicked		number of item clicked in list
				Portrait		True if Portrait, False if Landscape.
	Outputs:	-
	Returns:	-
	Purpose:	Sets up the height and width fields according to the item selected in the
				list.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL	PageTab::HandlePageList(INT32 ItemClicked, BOOL Portrait)
{    
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PageTab::HandlePageList called with no dialog pointer");

	// Now enable or disable and fill in the height and width fields dependent
	// on what size has been selected in the list of available pages. 
	BOOL Enable;
	BOOL ok;
	MILLIPOINT Width = 0;
	MILLIPOINT Height = 0;

	if ( ItemClicked == CUSTOM)
	{
		// If custom then just ungrey the present width/height editable fields
		Enable = TRUE;
	}
	else
	{
		Enable = FALSE;
		PageId PageChosen = (PageId)ItemClicked;
		if (AppPrefsDlg::pPageSizesList != NULL)
		{
			AppPrefsDlg::pPageSizesList->GetSizeFromPageId(PageChosen, &Width, &Height);
TRACEUSER( "Neville", _T("PageTab::HandlePageList width %d height %d in MP\n"),Width,Height);
		}

		// If not Portrait i.e. Landscape then swap the width and the height fields over 
		if (Portrait == FALSE)
		{
			// Swap width for height
			MILLIPOINT Temp = Width;
			Width = Height;
		 	Height = Temp;
		}	
		// Now set up the height and width fields accordingly
		ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_HEIGHT), CurrentPageUnits,
										 	 Height, FALSE, -1);
		ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_WIDTH), CurrentPageUnits,
										 	 Width, FALSE, -1);
	}	
	
	// Enable should be True if the custom page size has been selected.
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_HEIGHT), Enable);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_WIDTH), Enable);

	return TRUE;
}

/******************************************************************************************

>	BOOL	PageTab::HandlePortraitLandscape()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Purpose:	Checks to see if Portrait or Landscape has been selected. If a change has
				been made then swaps the height and width fields over.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
BOOL	PageTab::HandlePortraitLandscape()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PageTab::HandlePortraitLandscape called with no dialog pointer");

	// Get current state of Portrait button
	BOOL Portrait;
	BOOL Valid;
	Portrait = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_PORTRAIT), 0, 1, 0, &Valid);

	// Check for change of state, if none then exit now
	if (Portrait == PortraitSelected)
		return TRUE;

TRACEUSER( "Neville", _T("PageTab::HandlePortraitLandscape() change state\n"));
	// We have changed so swap the height and the width fields over
	// First, get the current values from the editable fields
	MILLIPOINT Width = 0;
	MILLIPOINT Height = 0;
	BOOL ok;
	Height = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_HEIGHT), CurrentPageUnits,
											  0, INT_MAX, _R(IDE_OPTS_INVALIDHEIGHT), &Valid);
	if (!Valid)
	{
		// failed to read correct value so return switch to known position and exit
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_PORTRAIT), PortraitSelected);
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_LANDSCAPE), !PortraitSelected);
		return FALSE;			
	}
	Width  = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_WIDTH), CurrentPageUnits,
											  0, INT_MAX, _R(IDE_OPTS_INVALIDWIDTH), &Valid);
	if (!Valid)
	{
		// failed to read correct value so return switch to known position and exit
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_PORTRAIT), PortraitSelected);
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_LANDSCAPE), !PortraitSelected);
		return FALSE;			
	}

	// Now set up the height and width fields reversly
	ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_HEIGHT), CurrentPageUnits,
								 		  Width, FALSE, -1);
	ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_WIDTH), CurrentPageUnits,
								 	 	  Height, FALSE, -1);
	
	// Now update the stored present state to the new state 
	PortraitSelected = Portrait;

	return TRUE;
}    
#endif //webster

/******************************************************************************************

>	BOOL	PageTab::GetSizeOfPage(MILLIPOINT *Width, MILLIPOINT *Height, MILLIPOINT *Bleed,
								   BOOL *Portrait, BOOL *Dps, BOOL *ShowDropShadow)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Outputs:	Width			found page width in millipoints
				Height			found page height in millipoints
				Margin			found page margin in millipoints
				Bleed			found page bleed in millipoints
				Portrait		set TRUE if found Width smaller than height
				Dps				set TRUE if double page spread present
				ShowDropShadow	set TRUE if currently displaying a drop shadow
	Returns: 	True if found the details ok.
	Purpose:	Checks to see what the size of the first page in the first spread in
				the document is and returns the width and height of it.
				Assumes at present a fairly simple document structure.
				Assumes the class variables pDocument, pSpread have been set up.
	Errors:		-
	SeeAlso:	AppPrefsDlg::SetCurrentSpread;

******************************************************************************************/

BOOL PageTab::GetSizeOfPage(MILLIPOINT *Width, MILLIPOINT *Height, MILLIPOINT *Margin,
							MILLIPOINT *Bleed, BOOL *Portrait, BOOL *Dps, BOOL *ShowDropShadow)
{
	// Set up defaults in case of dodgy exit
	*Width  = 0;
	*Height = 0;
	*Margin  = 0;
	*Bleed  = 0;
	*Portrait = TRUE;
	*Dps = FALSE;
	//	 WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	*ShowDropShadow = TRUE;
#else	//webster has no page shadow.		
	*ShowDropShadow = FALSE;
#endif	//webster

	// The dialog handler should have already set up the class variables pDocument and pSpread.
	// We will check that they are not null.
	// For now, get details on 1st page of spread 1
	ERROR2IF(pDocument == NULL,FALSE,"PageTab::GetSizeOfPage no selected document");
	ERROR2IF(pSpread == NULL,FALSE,"PageTab::GetSizeOfPage no spread to work on");

	// Set up some interim variables
	MILLIPOINT PageWidth = *Width;
	MILLIPOINT PageHeight = *Height;
	MILLIPOINT PageMargin = *Margin;
	MILLIPOINT PageBleed = *Bleed;
	BOOL Pages = *Dps;
	BOOL DropShadow = *ShowDropShadow;
		
	if (pSpread != NULL)
	{
		BOOL ok = pSpread->GetPageSize(&PageWidth, &PageHeight, &PageMargin, &PageBleed,
									   &Pages, &DropShadow);
		if (ok)
		{
			// page values read ok so set return parameters
			*Width  = PageWidth;
			*Height = PageHeight;
			*Margin  = PageMargin;
			*Bleed  = PageBleed;
			*Dps = Pages;
			*ShowDropShadow = DropShadow;
		}
		else
			return FALSE;
	}


	// Now check if landscape or portrait
	if (PageWidth < PageHeight)
	{
		*Portrait = TRUE;
		*Width = PageWidth;
		*Height = PageHeight;
	}
	else
	{
		*Portrait = FALSE;
		*Width = PageHeight;
		*Height = PageWidth;
	}
	
	// Remember the states found
	OldPageWidth = PageWidth;			// entry page width
	OldPageHeight = PageHeight;			// entry page height
	OldDpsState = Pages;				// entry state of dps
	OldDropShadowState = DropShadow;	// entry drop shadow state 
	OldMarginSize  = PageMargin;		// entry page margin size
	OldBleedSize  = PageBleed;			// entry page bleed size
	OldPortraitState = *Portrait;		// entry portrait/landscape state

	return TRUE;
}

/******************************************************************************************

>	BOOL	PageTab::SetNewSizeOfPage(MILLIPOINT Width, MILLIPOINT Height, MILLIPOINT Margin,
									  MILLIPOINT Bleed, BOOL Portrait, BOOL Dps, BOOL ShowDropShadow)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Inputs:		Width		new page width in millipoints
				Height		new page height in millipoints
				Margin		new page margin in millipoints
				Bleed		new page bleed in millipoints
				Portrait	set to TRUE if want Width smaller than height
				Dps			set to TRUE if double page spread required
	Returns: 	True if found the details ok.
	Purpose:	Checks to see what the size of the first page in the first spread in
				the document is and returns the width and height of it.
				Assumes at present a fairly simple document structure.
				This function should not no be used, should be using the undoable operation
				instead.
				Assumes the class variables pDocument, pSpread have been set up.
	Errors:		-
	SeeAlso:	AppPrefsDlg::SetCurrentSpread; InvokeResize;

******************************************************************************************/

//BOOL PageTab::SetNewSizeOfPage(MILLIPOINT Width, MILLIPOINT Height, MILLIPOINT Margin,
//							   MILLIPOINT Bleed, BOOL Portrait, BOOL Dps, BOOL ShowDropShadow)
//{
//	BOOL ok = FALSE;
//
//	// The dialog handler should have already set up the class variables pDocument and pSpread.
//	// We will check that they are not null.
//	// For now, get details on 1st page of spread 1
//	ERROR2IF(pDocument == NULL,FALSE,"PageTab::SetNewSizeOfPage no selected document")
//	ERROR2IF(pSpread == NULL,FALSE,"PageTab::SetNewSizeOfPage no spread to work on")
//
//	// Only change the page size/layout if something has changed 
//	// If the user has changed anything then warn them that nothing will happen
//	MILLIPOINT bodge = 5;		// extra leeway on comparison required
//	if ( (OldPageWidth <= (Width - bodge)) || (OldPageWidth >= (Width + bodge)) ||
//		 (OldPageHeight  <= (Height - bodge)) || (OldPageHeight  >= (Height + bodge)) || 
//		OldDpsState != Dps ||
//		OldPortraitState != Portrait ||
//		OldDropShadowState != ShowDropShadow ||
//		OldMarginSize != Margin ||
//		OldBleedSize != Bleed)
//	{
//		// Changes made so resize all pages in the spread.
//		pSpread->SetPageSize(Width, Height, Margin, Bleed, Dps, ShowDropShadow);
//		
//		// Force all views onto the document to be updated 
//		pDocument->ForceRedraw();
//		
//		// And make these the new defaults
//		OldPageWidth = Width;
//		OldPageHeight = Height;
//		OldDpsState = Dps;
//		OldPortraitState = Portrait;
//		OldDropShadowState = ShowDropShadow;
//		OldMarginSize = Margin;
//		OldBleedSize = Bleed;		
//	}
//
//	return TRUE;
//}


/******************************************************************************************

>	BOOL	PageTab::InvokeResize(PageResizeInfo *Param)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/2/95
	Inputs:	
	Returns: 	True if managed to change the page/spread  details ok.
	Purpose:	Sets the new details of the page size and of the default grid.
				The default grid should always be resident in the spread.
				Assumes the class variables pDocument, pSpread have been set up.
				This function should be used instead of SetNewSizeOfPage
				as it is undoable and they aren't.
	Errors:		-
	SeeAlso:	AppPrefsDlg::SetCurrentSpread; SetNewSizeOfPage;

******************************************************************************************/

BOOL PageTab::InvokeResize(PageResizeInfo *pParam)
{
	ERROR2IF(pParam==NULL,FALSE,"PageTab::InvokeResize() no pParam supplied");
	// For now, get details on 1st page of spread 1

	// The dialog handler should have already set up the class variables pDocument and pSpread.
	// We will check that they are not null.
	ERROR2IF(pDocument==NULL,FALSE,"PageTab::InvokeResize() no selected document");
	ERROR2IF(  pSpread==NULL,FALSE,"PageTab::InvokeResize() no spread to work on");

	// Only change the page size/layout if something has changed 
	// If the user has changed anything then warn them that nothing will happen
	MILLIPOINT bodge = 5;		// extra leeway on comparison required

	BOOL ChangeRequired = FALSE;
	if ( (OldPageWidth >= (pParam->Width - bodge)) && (OldPageWidth <= (pParam->Width + bodge)) &&
		 (OldPageHeight  >= (pParam->Height - bodge)) && (OldPageHeight  <= (pParam->Height + bodge)) && 
		OldDpsState == pParam->Dps &&
		OldDropShadowState == pParam->ShowDropShadow &&
		(OldMarginSize >= (pParam->Margin - bodge)) && (OldMarginSize <= (pParam->Margin + bodge)) &&
		(OldBleedSize >= (pParam->Bleed - bodge)) && (OldBleedSize <= (pParam->Bleed + bodge)))
	{
			// Set the page parameters so that we do not resize
			pParam->Height = 0;
			pParam->Width = 0;
			pParam->Margin = 0;
			pParam->Bleed = 0;
			pParam->Dps = FALSE;
	}
	else
	{
		// Flag we need to make a change
		ChangeRequired = TRUE;

		// Make these the new defaults
		OldPageWidth = pParam->Width;
		OldPageHeight = pParam->Height;
		OldDpsState = pParam->Dps;
		OldPortraitState = pParam->Portrait;
		OldDropShadowState = pParam->ShowDropShadow;
		OldMarginSize = pParam->Margin;
		OldBleedSize = pParam->Bleed;		
	}

	if (ChangeRequired)
	{
		// Set the flag to say that we are doing a move and resize operation
		// rather than a centre and resize used by batching
		pParam->ResizeAndCentre = FALSE;

		// Invoke the Page resizing operation so that we get Undo.
		OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpPageResize));

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

/******************************************************************************************

>	BOOL	PageTab::InvokeResizeAndCentre(PageResizeInfo *Param)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/95
	Inputs:	
	Returns: 	True if managed to change the page/spread  details ok.
	Purpose:	Sets the new details of the page size and of the default grid.
				The default grid should always be resident in the spread.
				Assumes the class variables pDocument, pSpread have been set up.
				This function should be used instead of SetNewSizeOfPage
				as it is undoable and they aren't.
				This does the same as resize but centres the objects on the page.
				Used by the batching code.
	Errors:		-
	SeeAlso:	AppPrefsDlg::SetCurrentSpread; SetNewSizeOfPage;

******************************************************************************************/

BOOL PageTab::InvokeResizeAndCentre(PageResizeInfo *pParam)
{
	ERROR2IF(pParam==NULL,FALSE,"PageTab::InvokeResizeAndCentre() no pParam supplied");

	// The dialog handler should have already set up the class variables pDocument and pSpread.
	// We will check that they are not null.
	ERROR2IF(pDocument==NULL,FALSE,"PageTab::InvokeResizeAndCentre() no selected document");
	ERROR2IF(  pSpread==NULL,FALSE,"PageTab::InvokeResizeAndCentre() no spread to work on");

	// Flag we need to make a change
//	BOOL ChangeRequired = TRUE;

	// Make these the new defaults
	OldPageWidth = pParam->Width;
	OldPageHeight = pParam->Height;
	OldDpsState = pParam->Dps;
	OldPortraitState = pParam->Portrait;
	OldDropShadowState = pParam->ShowDropShadow;
	OldMarginSize = pParam->Margin;
	OldBleedSize = pParam->Bleed;		

	// Set the flag to say that we are doing a centre and resize operation
	pParam->ResizeAndCentre = TRUE;

	// Invoke the Page resizing operation so that we get Undo.
	OpDescriptor* pOpDesc =	OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpPageResize));

	if (pOpDesc != NULL)
	{
		// Set up the parameters which we require to do the page resizing operation 
		pParam->pSpread = pSpread;

		// Go for it!
		pOpDesc->Invoke((OpParam *) pParam);
	}

	return TRUE;
}	



////////////////////////// OpPageResize class ... /////////////////////////////////////////

/******************************************************************************************
>	OpPageResize::OpPageResize() : UndoableOperation()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpPageResize constructor (Creates an undoable operation)
	SeeAlso:	PageTab;

******************************************************************************************/

OpPageResize::OpPageResize() : TransOperation() //UndoableOperation() 
{
}       

/********************************************************************************************

>	OpPageResize::~OpPageResize() 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpPageResize destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpPageResize::~OpPageResize()
{
}

/******************************************************************************************
>	BOOL OpAlign::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for a Page Resize operation
	SeeAlso:	PageTab;

******************************************************************************************/

BOOL OpPageResize::Init()
{  
	return RegisterOpDescriptor(
		0,								// Tool ID
 		_R(IDS_OPPAGERESIZE),				// String resource ID
		CC_RUNTIME_CLASS(OpPageResize),	// Runtime class
 		OPTOKEN_OPPAGERESIZE,			// Token string
 		OpPageResize::GetState,			// GetState function
		0,								// Help ID
		0,								// Bubble ID
		0,								// Resource ID
		0								// Control ID
//	needs	'GREY_WHEN_NO_CURRENT_DOC'
	);
}   

/******************************************************************************************
>	OpState	OpPageResize::GetState(String_256* pString, OpDescriptor* pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Purpose:	Returns the OpState of the OpPageResize dialogue operation.
	SeeAlso:	PageTab;

******************************************************************************************/

OpState	OpPageResize::GetState(String_256* pString, OpDescriptor* pOpDesc)
{
	OpState OpSt;

	return(OpSt);
}

/********************************************************************************************

>	void OpPageResize::Do(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		OpPageResize (unused)
	Outputs:	-
	Returns:	-
	Purpose:	(Performs the page resizing operation)
				OpPageResize has a special overloaded Do operator which takes
				parameters describing what is to be done - that version of Do must
				be used
	Errors:		Always generates an ENSURE failure because this Do doesn't.
	SeeAlso:	-

********************************************************************************************/
			
void OpPageResize::Do(OpDescriptor *NotUsed)
{
	ENSURE(FALSE, "OpPageResize does not provide a Do() function - Use DoWithParam");
	End();
}

/******************************************************************************************
>	void OpPageResize::DoWithParam(OpDescriptor* pOp, OpParam* pAlignParam)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		pOp - OpDescriptor as for all Do() functions
				Param - points to a Page resize structure with parameters:
					pSpread - The spread to be changed
	Outputs:	-
	Returns:	-
	Purpose:	Performs the page/spread resize operation, which resizes the pages in the
				specified spread (with undo) to the new size.
				Forces a redraw of the parent document for the specified spread.

	SeeAlso:	PageTab::SetNewSizeOfPage;

******************************************************************************************/

void OpPageResize::DoWithParam(OpDescriptor* pOp, OpParam* pResizeParam)
{
	PageResizeInfo *pInfo = (PageResizeInfo *) pResizeParam;

	ERROR3IF(pInfo == NULL,"OpPageResize called with NULL info pointer");
	ERROR3IF((pInfo == NULL) && (pInfo->pSpread == NULL),"OpPageResize called with NULL spread pointer");

	// Fail if the input parameters are incorrect
	if ((pInfo == NULL) || (pInfo->pSpread == NULL))
	{
		FailAndExecute();
		End();
		return;
	}

	Spread * pTheSpread = pInfo->pSpread;

	// As we are derived off TransOperation then we MUST call DoStartTransOp otherwise
	// it seems to fall over in a very nasty way!
	
	// Find the spreads first layer
   	Layer* pCurrentLayer = pTheSpread->FindFirstLayer();
	ERROR3IF(pCurrentLayer == NULL,"OpPageResize specified spread has no layers");
	// Fail if the input parameters are incorrect
	if (pCurrentLayer == NULL)
	{
		FailAndExecute();
		End();
		return;
	}

	// Find all objects on this spread, selected or unselected and move them.
	// Range Control parameters are:-
	// Include selected nodes - we want TRUE.
	// Include Unselected nodes - we want TRUE.
	// Cross layers - we want TRUE
	// Ignore locked layers - we want FALSE so that we include locked layers.
	// IsRenderable if set then calls NeedsToRender - we want TRUE
	// Ignore invisible layers - we want TRUE so that we include invisible layers.
	
	// DMc - included the promote to parents flag
	Range  SearchRange(pCurrentLayer, NULL, RangeControl(TRUE, TRUE, TRUE, FALSE, TRUE, TRUE,
		FALSE)); 

	// Must call this so that the Translation operation is set up ok.
	// NOTE! A FALSE return may simply mean there are no objects on the page!
	BOOL StartTransOpOk = DoStartTransOp(FALSE, NULL, &SearchRange);

	// See if we must do some resizing of the page(s).
	// If no change is required then both the height and width passed in will be zero
	if ((pInfo->Width != 0) && (pInfo->Height != 0))
	{
		// Get the details on the present page layout so that we can use this to undo what we
		// are about to try and do
		MILLIPOINT PageWidth = 0;
		MILLIPOINT PageHeight = 0;
		MILLIPOINT PageMargin = 0;
		MILLIPOINT PageBleed = 0;
		BOOL PresentDps = 0;
		BOOL PresentDropShadow = 0;
			
		pTheSpread->GetPageSize(&PageWidth, &PageHeight, &PageMargin, &PageBleed,
								&PresentDps, &PresentDropShadow);

		// Create an action to Undo the resizing operation
		ActionPageResize *APR;

		ActionCode PageResizeAction = ActionPageResize::Init(
 															this,
 															&UndoActions,
 															sizeof(ActionPageResize),
 															pInfo->pSpread,
 															PageWidth,
 															PageHeight,
 															PageMargin,
 															PageBleed,
 															PresentDps,
 															PresentDropShadow,
 															(Action**)(&APR)
															);

		// If the init happened ok then go and do the first operation
		if (PageResizeAction != AC_FAIL) 
		{
			// If the DoStartTransOp happened ok then move the objects on the page
			if (StartTransOpOk)
			{
				// Reposition the objects on the page correctly.
				// Do this first as if going from large to small the old position of the objects
				// might limit the size of the paste rectangle whereas the new positions wont. 
				if (
					!MoveObjectsOnPage(pTheSpread,
									   pInfo->Width, pInfo->Height,	pInfo->Margin, pInfo->Dps,
		 							   PageWidth, PageHeight, PageMargin, PresentDps,
									   &SearchRange,
									   pInfo->ResizeAndCentre
									   )
				   )
				{
					FailAndExecute();
					End();
					return;
				}
			}

			// Apply the initial 'Do' operation - resize that page

			// Save away the current spreads pasteboard rectangle as the specified margin
			// may be overridden if the bounding rectangle of the objects on the page would
			// not fit into a rectangle encompassing the new page structure and having a
			// margin gap around all 4 sides. 
			PasteboardRect = pTheSpread->GetPasteboardRect();

			pTheSpread->SetPageSize(pInfo->Width, pInfo->Height, pInfo->Margin, pInfo->Bleed,
								 	pInfo->Dps, pInfo->ShowDropShadow);
		}
		else
		{
			FailAndExecute();
			End();
			return;
		}
	}


//		NodeGrid* pGrid = NULL;
// 
//		pGrid = pTheSpread->FindFirstDefaultGridInSpread();
//		ERROR3IF(pGrid == NULL,"ActionSetDefaultGrid::Execute() no current default grid")
//
//		// Must set the origin of the grid to the bottom left corner of the
//		// union rectangle of all pages on the spread as this is where the x,y
//		// measurements are made from.
//		DocRect PagesRect;
//		BOOL ok = pTheSpread->GetPagesRect(&PagesRect);
//		if (ok)
//		{
//		   pGrid->SetOrigin(PagesRect.lo.x,PagesRect.lo.y);
//TRACEUSER( "Neville", _T("OpPageResize Grid origin at %d, %d\n"),PagesRect.lo.x,PagesRect.lo.y);
//	 	}
	
	// We must have been called to change either the page size OR the grid so always
	// force a redraw 
	// If everything has gone swimmingly then last thing is to redraw the document
	// JustinF says: only do this if the document isn't hidden, ie. it's one of the user's.
	// Don't do this if it's a hidden doc, such as the clipboard.
	Document *pParentDoc = (Document *)pTheSpread->FindOwnerDoc();
	if (pParentDoc != NULL && !pParentDoc->IsAHiddenDoc())
	{
		// Broadcast the special NEWPAGESIZE message which does not try and update the
		// page tab.
		pParentDoc->ForceRedraw();
		BROADCAST_TO_ALL(OptionsChangingMsg(pParentDoc,
											OptionsChangingMsg::PAGESIZEHASCHANGED));
	}

	// If we reached here then everything has happened ok and we can just end the
	// operation and exit 
	End();

	return;
}

/******************************************************************************************
>	BOOL OpPageResize::MoveObjectsOnPage(Spread *pSpread,
										 MILLIPOINT Width, MILLIPOINT Height,
										 MILLIPOINT Margin, BOOL Dps,
										 MILLIPOINT OldWidth, MILLIPOINT OldHeight,
										 MILLIPOINT OldMargin, BOOL OldDps,
										 Range  *pSearchRange,
										 BOOL ResizeAndCentre)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		pSpread 	- pointer to the spread whose objects areto be moved
				Width 		- new page width
				Height 		- new page height
				Margin 		- new page margin
				Dps			- single (FALSE) or double (TRUE) page spread flag
				Width 		- old page width
				Height 		- old page height
				Margin 		- old page margin
				Dps			- old single (FALSE) or double (TRUE) page spread flag
				pSearchRange- the range to be trasnformed 
				ResizeAndCentre - Set True if want a centre and resize operation 
								- False if want the resize and move operation
	Outputs:	-
	Returns:	-
	Purpose:	As part of the page/spread resize operation, which resizes the pages in the
				specified spread (with undo) to the new size, we must translate the objects
				on the page to a better position.

	SeeAlso:	OpPageResize::DoWithParam; ActionPageResize; PageTab::SetNewSizeOfPage;

******************************************************************************************/

BOOL OpPageResize::MoveObjectsOnPage(Spread *pSpread,
									 MILLIPOINT Width, MILLIPOINT Height,
									 MILLIPOINT Margin, BOOL Dps,
									 MILLIPOINT OldWidth, MILLIPOINT OldHeight,
									 MILLIPOINT OldMargin, BOOL OldDps,
									 Range  *pSearchRange,
									 BOOL ResizeAndCentre)
{
	ERROR2IF(pSpread == NULL, FALSE,"OpPageResize::MoveObjectsOnPage specified spread is null");
	ERROR2IF(pSearchRange == NULL, FALSE,"OpPageResize::MoveObjectsOnPage specified range is null");

	// Found no nodes on the page according to this spec so nothing to move
	if (pSearchRange->FindFirst() == NULL)
		return TRUE;

	// Work out the rectangle encompassing the old page structure
	MILLIPOINT OldWidthOfPages = 0;
	if (OldDps)
		OldWidthOfPages = OldMargin + 2 * OldWidth;
	else
		OldWidthOfPages = OldWidth;
	DocRect OldPagesRect(OldMargin,
						 OldMargin,
						 OldMargin + OldWidthOfPages + OldMargin,
						 OldMargin + OldHeight + OldMargin);

	// Work out the rectangle encompassing the new page structure
	MILLIPOINT WidthOfPages = 0;
	if (Dps)
		WidthOfPages = Margin + 2 * Width;
	else
		WidthOfPages = Width;
		
	DocRect	NewPagesRect(Margin,
						 Margin,
						 Margin + WidthOfPages + Margin,
						 Margin + Height + Margin);

	MILLIPOINT dx = 0;
	MILLIPOINT dy = 0;
	if (ResizeAndCentre)
	{ 
		/* Centre items on page... */
		DocRect OurItemRect(pSearchRange->GetBoundingRect());
		DocCoord ItemCentre((OurItemRect.Width()/2) + OurItemRect.lo.x,
							(OurItemRect.Height()/2) + OurItemRect.lo.y);
		DocCoord PageCentre(Width / 2, Height / 2);
  		dx = PageCentre.x - ItemCentre.x;
		dy = PageCentre.y - ItemCentre.y;
		dx += Margin;
		dy += Margin;
	}
	else
	{
		// Work out the difference between the centre points of the page structures
//		MILLIPOINT dx = NewPagesRect.lo.x + NewPagesRect.Width()/2
//						- (OldPagesRect.lo.x + OldPagesRect.Width()/2);
//		MILLIPOINT dy = NewPagesRect.lo.y + NewPagesRect.Height()/2
//						- (OldPagesRect.lo.y + OldPagesRect.Height()/2);
		dx = NewPagesRect.lo.x - OldPagesRect.lo.x;
		dy = NewPagesRect.lo.y - OldPagesRect.lo.y;
TRACEUSER( "Neville", _T("Old spread low x,y %d, %d\n"), OldPagesRect.lo.x, OldPagesRect.lo.y);
TRACEUSER( "Neville", _T("New spread low x,y %d, %d\n"), NewPagesRect.lo.x, NewPagesRect.lo.y);
TRACEUSER( "Neville", _T("Distance to move is %d, %d\n"), dx, dy);
	}
	
	// If the movement required is small then forget it
	const MILLIPOINT bodge = 100;
	if (
		(dx >= -bodge) && (dx <= bodge) &&
		(dy >= -bodge) && (dy <= bodge)
	   )
		return TRUE;

	// Construct the translation matrix that we want to apply to the objects
	Trans2DMatrix* pMatrix = new Trans2DMatrix(dx, dy);

	// Move all those nodes by the required amount (if it is a Renderable Ink Node)
	DoTransformNodes(*pSearchRange, pMatrix);

	// Update the selection cache
	GetApplication()->UpdateSelection();

	return TRUE;
}	




/********************************************************************************************

>	ActionPageResize::ActionPageResize()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/95
	Purpose:	ActionPageResize constructor

********************************************************************************************/
    
ActionPageResize::ActionPageResize()
{
}



/********************************************************************************************

>	ActionPageResize::~ActionPageResize() 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/95
	Purpose:	ActionPageResize destructor

********************************************************************************************/

ActionPageResize::~ActionPageResize()
{
}



/********************************************************************************************

>	virtual ActionCode ActionPageResize::Execute()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/95
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the ActionPageResize to go and resize the current spread/page
				and generates another ActionPageResize to undo this change.
				Forces a redraw of the parent document for the specified spread.

********************************************************************************************/

ActionCode ActionPageResize::Execute()
{
	ActionPageResize PageResizeAct;
	ActionCode ActCode = AC_FAIL;


	// Get the details on the present page layout so that we can use this to undo what we
	// are about to try and do
	MILLIPOINT PageWidth = 0;
	MILLIPOINT PageHeight = 0;
	MILLIPOINT PageMargin = 0;
	MILLIPOINT PageBleed = 0;
	BOOL PresentDps = 0;
	BOOL PresentDropShadow = 0;
		
	if (pSpread != NULL)
	{
		pSpread->GetPageSize(&PageWidth, &PageHeight, &PageMargin, &PageBleed,
									   &PresentDps, &PresentDropShadow);
	}

     // Create an action to restore the changes we are about to make
    if ((ActCode = ActionPageResize::Init(pOperation,
								     		pOppositeActLst,
								     		sizeof(ActionPageResize),
											pSpread,
											PageWidth,
											PageHeight,
											PageMargin,
											PageBleed,
											PresentDps,
											PresentDropShadow,
						 			 		( Action**)(&PageResizeAct))) != AC_FAIL)
	{
		// Need to do something here
		if (pSpread != NULL)
		{
			// Save away the current spreads pasteboard rectangle as the specified margin
			// may be overridden if the bounding rectangle of the objects on the page would
			// not fit into a rectangle encompassing the new page structure and having a
			// margin gap around all 4 sides. 
			PasteboardRect = pSpread->GetPasteboardRect();

			pSpread->SetPageSize(Width, Height, Margin, Bleed, Dps, ShowDropShadow);


			// Force all views onto the document to be updated 
			Document *pParentDoc = (Document *)pSpread->FindOwnerDoc();
			if (pParentDoc != NULL)
			{
				pParentDoc->ForceRedraw();

				BROADCAST_TO_ALL(OptionsChangingMsg(
												 pParentDoc,
												 OptionsChangingMsg::NEWPAGESIZE
								 				   )
								);
			}
		}
	}

	return (ActCode);
}



/********************************************************************************************

>	ActionCode ActionPageResize::Init(Operation* const pOp, 
										ActionList* pActionList, 	
							  			UINT32 ActionSize,
										Spread *pSpread,
										Page *pPage,
				  						Action** NewAction)
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/95

	Inputs:		pOp: The operation to which the action should be added
				pActionList: The action list in the operation object
				ActionSize:  The size of the action in bytes. This should be the total  
							 size of the action (including any objects pointed to by the
							 action).
				pSpread:	 The target spread which will be changed when we are executed
				Height		 The new page height required.
				Width		 The new page width required.
				Margin		 The new page margin required.
				Bleed		 The new page bleed required.
				Dps			 The new single/double page spread required.
				ShowPageShadow	The new page shadow display state required.


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
				of an ActionPageResize.
	Errors:		-
	SeeAlso:	Action::Init; OpPageResize; PageTab::SetNewSizeOfPage; 

********************************************************************************************/
   
ActionCode ActionPageResize::Init(Operation* const pOp, 
									ActionList* pActionList, 	
						  			UINT32 ActionSize,
									Spread *pTheSpread,
									MILLIPOINT NewWidth,
									MILLIPOINT NewHeight,
									MILLIPOINT NewMargin,
									MILLIPOINT NewBleed,
									BOOL NewDps,
									BOOL NewShowDropShadow,
				  					Action** NewAction)
{
	ActionCode Ac = (Action::Init(pOp,
									pActionList,
									ActionSize,
									CC_RUNTIME_CLASS(ActionPageResize), 
									NewAction));
	if (*NewAction != NULL)
	{
		ActionPageResize *ACC = (ActionPageResize*) (*NewAction);
		ACC->pSpread  = pTheSpread;
		ACC->Width = NewWidth;
		ACC->Height = NewHeight;
		ACC->Margin = NewMargin;
		ACC->Bleed = NewBleed;
		ACC->Dps = NewDps;
		ACC->ShowDropShadow = NewShowDropShadow;
	}

	return (Ac);
}


