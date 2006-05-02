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
// Implementation of the unit setup dialog as a tab of the options dialog box

/*

*/

#include "camtypes.h"
#include "appprefs.h"
//#include "simon.h"
//#include "markn.h"
#include "optsunit.h"
//#include "prefsdlg.h"	// dialog/gadget ids
//#include "optsres.h"	// error strings
#include "app.h"		// Camelot object
//#include "resource.h"	// _R(IDS_OK)
#include "optsmsgs.h"	// options changing messages class
#include "document.h"	// document object
#include "grid.h" 		// checking if deleted units in grid
#include "spread.h"		// checking if deleted units in grid
#include "radio.h"		// class RadioGroup
#include "scunit.h"		// class ScaleUnit

//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
CC_IMPLEMENT_DYNAMIC(UnitsTab, OptionsTabs)   
CC_IMPLEMENT_DYNCREATE(UnitPropertiesDlg, DialogOp)   
#endif  //webster

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

// Where the default colour units are stored...Use GetColourUnitPreference() to do the fetching.

static TCHAR*	ColourUnitSection = TEXT("Displays");
static TCHAR*	ColourUnitPreference = TEXT("ColourEditorUnits");

// Set up the dialog box details stored as statics in the class
// This is for the user units properties dialog box
CDlgResID 		UnitPropertiesDlg::IDD 	= _R(IDD_OPTS_UNITPROPERTIES);	// default dialog box id
const CDlgMode 	UnitPropertiesDlg::Mode = MODAL;					// This dialog is modal

// Associations for RadioGroup class for Colour Units
static const UINT32 NUM_CLR_DEFAULTS = 2;

static CGadgetAssociation	ColourUnitAssociations[NUM_CLR_DEFAULTS] =
{
	{_R(IDC_OPTS_CLR_UNIT_USE_PERCENT),	&(StandardUnit::UnitPercent)},
	{_R(IDC_OPTS_CLR_UNIT_USE_BYTE),	&(StandardUnit::UnitDecimalD)}
};

//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER

/********************************************************************************************

>	UnitsTab::UnitsTab() 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	UnitsTab constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UnitsTab::UnitsTab()
{   
	// Do nothing at present
}        

/********************************************************************************************

>	UnitsTab::~UnitsTab()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	UnitsTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
UnitsTab::~UnitsTab()
{   
}        

/********************************************************************************************

>	BOOL UnitsTab::Init()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	UnitsTab initialisation routine
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
BOOL UnitsTab::Init()
{   
	CurrentUserUnitType  = NOTYPE;
	pDocUnitList 	 	 = NULL;

	// Initialise the dialog box that we require to create new and edit user units
	BOOL ok = UnitPropertiesDlg::Init();

	return ok;
}        


/******************************************************************************************

>	CDlgResID UnitsTab::GetPageID()()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CDlgResID UnitsTab::GetPageID()
{
	return _R(IDD_OPTSTAB_UNITS);
}

/******************************************************************************************

>	BOOL UnitsTab::IsDocumentOption()

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

BOOL UnitsTab::IsDocumentOption()
{
	return TRUE;	// This tab is a document option 
}

/******************************************************************************************

>	BOOL UnitsTab::IsProgramOption()

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

BOOL UnitsTab::IsProgramOption()
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
	Purpose:	Takes the values in the Units section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL UnitsTab::CommitSection()
{
TRACEUSER( "Neville", _T("commit units section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UnitsTab::CommitSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_UNITS));	// The units page identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Section = Units

	// All units work off the selected document and so we must do nothing if there is no
	// selected document. Check our clas variable to see if this is true or not.
	// Units should have been set up by the caller.
	if (pDocument == NULL || pSpread == NULL)
	{
		// If this fails then there is no document and so we must ensure
		// things which need a selected document are greyed.
		GreySection();
		return TRUE;
	}

	ok = CommitDialogValues();

	return ok;
}

/********************************************************************************************

>	BOOL UnitsTab::GreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents 
	Errors:		-
	SeeAlso:	UnitsTab::UngreySection;

********************************************************************************************/

BOOL UnitsTab::GreySection()
{
TRACEUSER( "Neville", _T("GreySection in UnitsTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UnitsTab::GreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_UNITS));	// The UnitsTab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_UNITS_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), DocumentName);

	// Only update if we are not already grey 
	if (GreyStatus == TRUE)
		return TRUE;

	m_ColourUnitRadioGroup.Disable();
	// Call our central greying/ungreying function
	ok = ChangeControlStatus(FALSE);
	
	GreyStatus = TRUE;

	return ok;
} 

/********************************************************************************************

>	BOOL UnitsTab::UngreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document 
	Errors:		-
	SeeAlso:	UnitsTab::UngreySection;

********************************************************************************************/

BOOL UnitsTab::UngreySection()
{
TRACEUSER( "Neville", _T("UngreySection in UnitsTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UnitsTab::UngreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_UNITS));	// The UnitsTab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_UNITS_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), DocumentName);

	// Only update if we are not already ungrey 
	if (GreyStatus == FALSE)
		return TRUE;

	m_ColourUnitRadioGroup.Enable();
	// Call our central greying/ungreying function
	ok = ChangeControlStatus(TRUE);
	
	GreyStatus = FALSE;

	return ok;
}

/********************************************************************************************

>	BOOL UnitsTab::ChangeControlStatus(const BOOL Status)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 
	Errors:		-
	SeeAlso:	UnitsTab::GreySection;	UnitsTab::UngreySection;

********************************************************************************************/

BOOL UnitsTab::ChangeControlStatus(const BOOL Status)
{
	// Main units section
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_INFO), Status);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_UNITSGROUP),	Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_PAGEUNITSTXT),	Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCALEDUNITSTXT),Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_FONTUNITSTXT),	Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_PAGEUNITS),		Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SCALEDUNITS),	Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_FONTUNITS),		Status);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_CLR_UNIT_USE_PERCENT),	Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_CLR_UNIT_USE_BYTE),		Status);
	
	// Buttons in the user units section
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_NEWUNIT),		Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_DELETEUNIT),	Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_UNITPROPERTY),	Status);

	return TRUE;
}			

/********************************************************************************************

>	BOOL UnitsTab::UpdateSection(String_256 *DocumentName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		Name of document
	Outputs:	-
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. Should only init the tab if the page is present.  
				The document name allows the info field on the tab to be filled in correctly.
	Errors:		-
	SeeAlso:	UnitsTab::GreySection; UnitsTab::UngreySection; UnitsTab::InitSection;

********************************************************************************************/

BOOL UnitsTab::UpdateSection(String_256 *DocumentName)
{
TRACEUSER( "Neville", _T("UnitsTab::UpdateSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UnitsTab::UpdateSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_UNITS));	// The UnitsTab identifier
	if (!ok)
		return TRUE;		// page not present

	// Call the normal init section code
	return InitSection();
}

/********************************************************************************************

>	BOOL UnitsTab::HandleMsg(DialogMsg* Msg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the units options tabs messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL UnitsTab::HandleMsg(DialogMsg* Msg)
{
TRACEUSER( "Neville", _T("HandleUnitsMsg\n"));
	ERROR2IF(Msg == NULL,FALSE,"UnitsTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UnitsTab::HandleMsg called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_UNITS));	// The edit page identifier
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
			OptionsTabs::SetApplyNowState(TRUE);
			if (Msg->GadgetID == _R(IDC_OPTS_UNITPROPERTY))
				// Show the user the properties of the selected user unit
				ShowUnitProperties();
			else if (Msg->GadgetID == _R(IDC_OPTS_NEWUNIT))
				// Create the user a new unit
				CreateNewUnit();
			else if (Msg->GadgetID == _R(IDC_OPTS_DELETEUNIT))
				// Delete the currently selected user unit
				DeleteUnit();
			break;

		case DIM_SELECTION_CHANGED:
		{
			OptionsTabs::SetApplyNowState(TRUE);
			ERROR2IF(pDocUnitList == NULL,FALSE,"UnitsTab::HandleMsg called with no doc unit list");
			
			WORD Index = 0;
			Unit* pUnit = NULL;
//			Unit* pCurUserUnit = NULL;
			
			if (Msg->GadgetID == _R(IDC_OPTS_UNITSLIST))
			{
				// Clicked on the list of user unit types
				// Now, switch to the new item
				pPrefsDlg->GetValueIndex(_R(IDC_OPTS_UNITSLIST), &Index); 
				pUnit = pDocUnitList->FindUserUnit(Index);
				if (pUnit != NULL)
				{
					CurrentUserUnitType = pUnit->GetUnitType();
				}
			}
		}
		default:
			break;
	}

	return TRUE;
}  


/********************************************************************************************

>	BOOL UnitsTab::CommitDialogValues()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/94
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Takes the values in the dialog.
				Called when the OK button is selected
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL UnitsTab::CommitDialogValues()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UnitsTab::CommitDialogValues called with no dialog pointer");
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitsTab::HandleMsg called with no doc unit list");


	// ****
	// All of the user units are handled as immediate edits and so no extra actions
	// are required.

	// ****
	// Now update the defaults units that may have been choosen by the user

	// Set up the units with useful defaults, just in case
	UnitType PageUnitType = MILLIMETRES;
	//UnitType ScaledUnitType = AUTOMATIC;
	UnitType FontUnitType = COMP_POINTS;
	WORD Index;

	// Work out which page units menu item is selected and then work out the unit type
	pPrefsDlg->GetValueIndex(_R(IDC_OPTS_PAGEUNITS), &Index);
	Unit *pPageUnit = pDocUnitList->FindUnit(Index);
	if (pPageUnit)
		PageUnitType = pPageUnit->GetUnitType();
	
	pPrefsDlg->GetValueIndex(_R(IDC_OPTS_FONTUNITS),  &Index);
	Unit *pFontUnit = pDocUnitList->FindUnit(Index);
	if (pFontUnit)
		FontUnitType = pFontUnit->GetUnitType();

//	pPrefsDlg->GetValueIndex(_R(IDC_OPTS_SCALEDUNITS), &Index);
//	if (Index <= 0)
//		ScaledUnitType = AUTOMATIC;
//	else
//	{
//		// Remove the automatic units from the calculation
//		Unit *pScaleUnit = pDocUnitList->FindUnit(Index - 1);
//		if (pScaleUnit)
//			ScaledUnitType = pScaleUnit->GetUnitType();
//	}

	if (pDocUnitList != NULL)
	{
		pDocUnitList->SetPageUnits(PageUnitType);
		//pDocUnitList->SetScaleUnits(ScaledUnitType);
		pDocUnitList->SetFontUnits(FontUnitType);
	}

	ScaleUnit*	pColourUnit = (ScaleUnit*)m_ColourUnitRadioGroup.GetSelected();
	ERROR3IF(pColourUnit == NULL, "UnitsTab::CommitDialogValues - Gibberish from GetSelected()");

	// If we have changed the units from the entry ones then tell other users about the change 
	// Do a blantant update if the current user units are the current page or font units
	if (
		(PageUnitType != OldPageUnits) ||
		//(ScaledUnitType != OldScaledUnits) ||
		(FontUnitType != OldFontUnits) ||
		(pColourUnit != NULL && m_pOldColourUnit != pColourUnit) ||
		(CurrentUserUnitType == PageUnitType) ||
		(CurrentUserUnitType == FontUnitType)
	   )
	{
		// Colour units have changed so store the default as a preference then send a message in case
		// the Colour Editor is listening
		if (m_pOldColourUnit != pColourUnit)
		{
			ERROR3IF(!(pColourUnit->IS_KIND_OF(ScaleUnit)), "UnitsTab::CommitDialogValues - Not ScaleUnit");
			Camelot.SetPrefDirect(ColourUnitSection, ColourUnitPreference, pColourUnit->GetQualifier()->GetToken(), TRUE);
			m_pOldColourUnit = pColourUnit;
		}
		// Now tell other users of units that there is a possibly new default units in operation
		// and so update any currently displayed units. 
		BROADCAST_TO_ALL(OptionsChangingMsg(pDocument, OptionsChangingMsg::NEWUNITS));

		// Mark the document as modified as we have changed something that is saved with the
		// document but only if the page or font units have changed.
		if (pDocument &&
			(
				(PageUnitType != OldPageUnits) ||
				(FontUnitType != OldFontUnits)
			)
		   )
		{
			pDocument->SetModified(TRUE);
		}	

		// Note the new entry state of the controls
		//OldScaledUnits = ScaledUnitType; 
		OldPageUnits = PageUnitType; 
		OldFontUnits = FontUnitType; 
	}
	
	// If there is a currently seleted user unit then make sure its name in all the lists is
	// correct. Only really true on Apply Now clicks as Oks will of course remove the dialog
	// box.
	if (CurrentUserUnitType != NOTYPE)
	{
		// Need to do this as otherwise for some strange reason the DialogManager errors
		// denying knowledge of the control. Really saying WindowHandler is 0!
		BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_UNITS));	// The edit page identifier
		if (!ok)
			return TRUE;		// page not present

		//InitControls();
		UpdateUnitListControls();
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL UnitsTab::CreateNewUnit()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/95
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Creates a new user unit that the user has just requested.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL UnitsTab::CreateNewUnit()
{
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitsTab::CreateNewUnit called with no doc unit list");

	Unit* pUnit = NULL;

	BOOL ok;
	ok = pDocUnitList->MakeNewUnit(&pUnit);

	if (ok)
	{
		CurrentUserUnitType = pUnit->GetUnitType();
		// Fire up the user units name dialog box, but only if we have
		// a current user unit to edit.
		BOOL Ok = TRUE; 
		if (CurrentUserUnitType != NOTYPE)
		{
			// This is ok as we are using a modal dialog box
			Ok = UnitPropertiesDlg::InvokeDialog(CurrentUserUnitType);
			
			if (!Ok)
				DeleteUnit();			
		}

		// Only do the rest of this if the user named the unit ok
		if (Ok)
		{
			EnableControls();
			SetUnitToken(_R(IDC_OPTS_PAGEUNITS),  CurrentUserUnitType, 0,TRUE);
			//SetUnitToken(_R(IDC_OPTS_SCALEDUNITS),CurrentUserUnitType,NULL,TRUE);
			SetUnitToken(_R(IDC_OPTS_FONTUNITS),  CurrentUserUnitType, 0,TRUE);
			InitControls();
			// Now make sure the current user unit is selected in the list
			SelectCurrentUserUnit();
			// Mark the document as modified as we have changed something
			// that is saved with the document.
			if (pDocument)
				pDocument->SetModified(TRUE);
		}
	}
	else
	{
		// Failed to create the new unit, usually due to no memory.
		// So report this to the user. Error set by new on failure. 
		InformError();
	}
	
	return TRUE;
}

/********************************************************************************************

>	BOOL UnitsTab::DeleteUnit()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/95
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Deletes the currently selected user unit.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL UnitsTab::DeleteUnit()
{
	ERROR2IF(pDocument == NULL,FALSE,"UnitsTab::DeleteUnit called with no document");
	ERROR2IF(pSpread == NULL,FALSE,"UnitsTab::DeleteUnit called with no document");
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitsTab::DeleteUnit called with no doc unit list");

	Unit* pNextUserUnit = NULL;
	Unit* pCurUserUnit = NULL;
	pCurUserUnit  = pDocUnitList->FindUnit(CurrentUserUnitType);
	pNextUserUnit = pDocUnitList->FindNextUserUnit(pCurUserUnit);
	if (pNextUserUnit == NULL)
	{
		pNextUserUnit = pDocUnitList->FindFirstUserUnit();
		if (pNextUserUnit->GetUnitType() == CurrentUserUnitType)
			pNextUserUnit = NULL;
	}

	UnitReason DeleteReason = UNITREASON_OK;

	// Before we try and delete the unit we must check if it is being used
	// by the default grid or by the scaling strings. If so then we should not
	// allow the operation to continue.
	// First check the grid
	NodeGrid* pDefaultGrid = pSpread->FindFirstDefaultGridInSpread();
	if (pDefaultGrid)
	{
		// Default grid present so ensure not using these units
		// Specify True so that we get the scaled units. Otherwise,
		// if scaling is set then the user is warned but cannot see the
		// units in the grid tab!
		UnitType GridUnits = pDefaultGrid->GetUnits(TRUE);
		if (CurrentUserUnitType == GridUnits)
			DeleteReason = UNITREASON_BEINGUSED;
	}

	// Now the document scaling, returns page units if not active
	// If present in inactive strings then user will be warned when	they
	// try and activate this scale.
	DimScale* pDimScale = pSpread->GetPtrDimScale();
	if (pDimScale)
	{
			UnitType ScaleUnits = pDimScale->GetScaleUnits();
		if (CurrentUserUnitType == ScaleUnits)
			DeleteReason = UNITREASON_BEINGUSED;
		else
		{
			// If scale string ok then check the drawing string, if the scale is
			// active
			BOOL Active = pDimScale->IsActive();
			if (Active)
			{
				String_32 Str;
				BOOL ok = TRUE;
				double dummy = 0.0;
				Str = pDimScale->GetDrawingScaleStr();
				ok = Convert::StringToComponents(Str, &dummy, &ScaleUnits);
				if (ok && CurrentUserUnitType == ScaleUnits)
					DeleteReason = UNITREASON_BEINGUSED;
			}
		}
	}

	// Try and delete the unit from the doc unit list, will complain if the
	// unit has decendents i.e. units based on it or it is being used for 
	// the default page, font or scaling units.
	// Only do it if we haven't found above reason not to delete it
	if (DeleteReason == UNITREASON_OK)
		DeleteReason = pDocUnitList->DeleteUnit(CurrentUserUnitType);

	switch (DeleteReason)
	{
		case UNITREASON_OK :
			if (pNextUserUnit == NULL)
				CurrentUserUnitType = NOTYPE;
			else
				CurrentUserUnitType = pNextUserUnit->GetUnitType();

			EnableControls();
			InitControls();
			// Now make sure the current user unit is selected in the list
			SelectCurrentUserUnit();

			// Mark the document as modified as we have changed something
			// that is saved with the document.
			if (pDocument)
				pDocument->SetModified(TRUE);
			break;

		case UNITREASON_HASDESCENDENTS:
			Error::SetError(_R(IDS_UNITERROR_HASDESCENDENTS),0);
			InformError();
			break;

		case UNITREASON_BEINGUSED:
			Error::SetError(_R(IDS_UNITERROR_BEINGUSED),0);
			InformError();
			break;

		default :
			ENSURE(FALSE,"Unexpected DeleteUnit reason");
			break;
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL UnitsTab::ShowUnitProperties()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/95
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Shows the current properties of the selected user unit.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL UnitsTab::ShowUnitProperties()
{
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitsTab::ShowUnitProperties called with no doc unit list");

	// Fire up the user units property dialog box, but only if we have
	// a current user unit to edit.
	BOOL Ok = TRUE; 
	if (CurrentUserUnitType != NOTYPE)
	{
		// This is ok as we are using a modal dialog box
		Ok = UnitPropertiesDlg::InvokeDialog(CurrentUserUnitType);

		// If Oked then make sure everything is updated to reflect this potential new info
		if (Ok)
		{
			// Make sure that the selected user units list item is updated with the new info.
			if (CurrentUserUnitType != NOTYPE)
			{
				INT32 UnitIndex = pDocUnitList->FindUnitIndex(CurrentUserUnitType);
				Unit * pUnit = pDocUnitList->FindFirstUserUnit();
				if (pUnit)
				{
					UnitType FirstUserUnitType = pUnit->GetUnitType();
					INT32 UserUnitIndex = pDocUnitList->FindUnitIndex(FirstUserUnitType); 
					INT32 Index = UnitIndex - UserUnitIndex;
					pPrefsDlg->DeleteValue(_R(IDC_OPTS_UNITSLIST), FALSE, Index);
					ShowUnitDetails(_R(IDC_OPTS_UNITSLIST), CurrentUserUnitType, Index, FALSE);
					pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_UNITSLIST), Index);
				}
			}

			// Make sure the controls displaying user units are updated with this change.
			UpdateUnitListControls();
		}
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL UnitsTab::SelectCurrentUserUnit()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/95
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Make sure that the current user unit, if any, is selected and hence shown as
				selected in the user units list.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL UnitsTab::SelectCurrentUserUnit()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UnitsTab::CommitUserDialogValues called with no dialog pointer");
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitsTab::CommitUserDialogValues called with no doc unit list pointer");

	// Check if the user has selected a valid user unit to edit
	// If not do nothing 
	if (CurrentUserUnitType != NOTYPE)
	{
		INT32 UnitIndex = pDocUnitList->FindUnitIndex(CurrentUserUnitType);
		Unit * pUnit = pDocUnitList->FindFirstUserUnit();
		if (pUnit)
		{
			UnitType FirstUserUnitType = pUnit->GetUnitType();
			INT32 UserUnitIndex = pDocUnitList->FindUnitIndex(FirstUserUnitType); 
			INT32 Index = UnitIndex - UserUnitIndex;
			pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_UNITSLIST), Index);
		}
	}
	
	return TRUE;
}

/********************************************************************************************

>	BOOL UnitsTab::UpdateUnitListControls()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/9/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Updates the new name name of the currently selected user unit, if any in all
				of the lists.
	SeeAlso:	UnitPropertiesDlg::DisplayUserUnitDetails;	UnitsTab::ShowUnitDetails;

********************************************************************************************/

BOOL UnitsTab::UpdateUnitListControls()
{
TRACEUSER( "Neville", _T("UnitsTab::UpdateUnitListControls()\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UnitsTab::UpdateUnitListControls called with no dialog pointer");
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitsTab::UpdateUnitListControls called with no doc unit list pointer");

	// Make sure that we have the text for the current unit up to date
	String_256 TokenStr = _T("");
	String_32 TokenStr_32 = _T("");
	Unit* pCurUserUnit = NULL;

	pCurUserUnit = pDocUnitList->FindUnit(CurrentUserUnitType);
	if (pCurUserUnit != NULL)
	{
		// Get the current name for this unit
		TokenStr = pCurUserUnit->GetToken();
		//TokenStr = TokenStr_32;

		// Now update the name in the lists
		INT32 UnitIndex = pDocUnitList->FindUnitIndex(CurrentUserUnitType);
		// Have to delete them and then reinsert them back into the list
		// These lists have all units on
		pPrefsDlg->DeleteValue(_R(IDC_OPTS_PAGEUNITS), FALSE, UnitIndex);
		pPrefsDlg->DeleteValue(_R(IDC_OPTS_FONTUNITS), FALSE, UnitIndex);
		pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_PAGEUNITS), TokenStr, FALSE, UnitIndex);
		pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_FONTUNITS), TokenStr, FALSE, UnitIndex);

		// Must set the selected item as otherwise it will blank the main list item. 

		// Work out the indicies which these units correspond to on a list of units 
		// Get the current default settings
		UnitType PageUnits  = pDocUnitList->GetPageUnits();
		UnitType FontUnits  = pDocUnitList->GetFontUnits();
		INT32 PageUnitsIndex = pDocUnitList->FindUnitIndex(PageUnits);
		INT32 FontUnitsIndex = pDocUnitList->FindUnitIndex(FontUnits);
		
		// And now select the correct item on the list of units
		pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_PAGEUNITS), PageUnitsIndex);
		pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_FONTUNITS), FontUnitsIndex);
	}

	return TRUE;
}


/********************************************************************************************

>	void UnitsTab::SetUnitToken(CGadgetID ID, UnitType ThisUnitType, INT32 Index = -1,
								BOOL EndOfList = FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/94
	Inputs:		ID = the ID of the control to put the unit token in
				ThisUnitType = the type of the unit whose token you want putting in the control
				Index = index into a drop list (-1 = main field)
				EndOfList - if TRUE, item is stuck on end of list, regardless of Index
	Outputs:	-
	Returns:	-
	Purpose:	Readable function for putting unit token strings in controls.
	SeeAlso:	-

********************************************************************************************/

void UnitsTab::SetUnitToken(CGadgetID ID, UnitType ThisUnitType, INT32 Index, BOOL EndOfList)
{
	String_32 Str = pDocUnitList->GetToken(ThisUnitType);
	String_256 Str256 = Str;
	pPrefsDlg->SetStringGadgetValue(ID, Str256, EndOfList, Index);
}

/********************************************************************************************

>	void UnitsTab::ShowUnitDetails(CGadgetID ID, UnitType ThisUnitType, INT32 Index = -1,
									BOOL EndOfList = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/95
	Inputs:		ID = the ID of the control to put the unit token in
				ThisUnitType = the type of the unit whose token you want putting in the control
				Index = index into a drop list (-1 = main field)
				EndOfList - if TRUE, item is stuck on end of list, regardless of Index
	Outputs:	-
	Returns:	-
	Purpose:	Shows the summary of the details of the specified unit for putting user unit
				token strings in the scrollable details list.
	SeeAlso:	-

********************************************************************************************/

void UnitsTab::ShowUnitDetails(CGadgetID ID, UnitType ThisUnitType, INT32 Index, BOOL EndOfList)
{
	String_256 Details;

	String_32 Name = pDocUnitList->GetToken(ThisUnitType);

	// Add the abbreviation
	String_32 Abbrev = pDocUnitList->GetSpecifier(ThisUnitType);

	Unit* pCurUserUnit = NULL;
	pCurUserUnit = pDocUnitList->FindUnit(ThisUnitType);

	String_32 PrefixStr;
	double BaseNumerator = 0.0;
	double BaseDenominator = 0.0;
	String_32 Numerator;
	String_32 Denominator;
	String_32 BasedOn;
	if (pCurUserUnit)
	{
		// Set up the prefix/suffix state for this user's unit.
		if (pCurUserUnit->IsPrefix())
			PrefixStr.MakeMsg(_R(IDS_UNITS_PREFIX));
		else
			PrefixStr.MakeMsg(_R(IDS_UNITS_SUFFIX));

		BaseNumerator = pCurUserUnit->GetBaseNumerator();
		BaseDenominator = pCurUserUnit->GetBaseDenominator();
		Convert::DoubleToString(BaseNumerator, &Numerator);
		Convert::DoubleToString(BaseDenominator, &Denominator);

		UnitType BaseUnit = pCurUserUnit->GetBaseUnitType();
		BasedOn = pDocUnitList->GetSpecifier(BaseUnit);
	}

	Details.MakeMsg(_R(IDS_UNITSDESCRIPTION), (TCHAR*)Name, (TCHAR*)Abbrev, (TCHAR*)PrefixStr,
										  (TCHAR*)Denominator, (TCHAR*)Numerator,
										  (TCHAR*)BasedOn);

	pPrefsDlg->SetStringGadgetValue(ID, Details, EndOfList, Index);
}

/********************************************************************************************

>	BOOL UnitsTab::InitControls()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Inits all the controls in the dialog
				Called immediately after the bar is created to init drop lists, grey
				out gadgets, etc.
	SeeAlso:	-

********************************************************************************************/

BOOL UnitsTab::InitControls()
{                            
TRACEUSER( "Neville", _T("UnitsTab::InitControls\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UnitsTab::InitControls called with no dialog pointer");
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitsTab::InitControls called with no doc unit list pointer");

	InitUnitListControls();

	// Get the current default settings
	UnitType PageUnits   = pDocUnitList->GetPageUnits();	// The units used to display page measurements
	//UnitType ScaledUnits = pDocUnitList->GetScaleUnits();	// The units to display scaled measurements
	UnitType FontUnits   = pDocUnitList->GetFontUnits();	// The units to display font measurements

	// Work out the indicies which these units correspond to on a list of units 
	INT32 PageUnitsIndex = pDocUnitList->FindUnitIndex(PageUnits);
	INT32 FontUnitsIndex = pDocUnitList->FindUnitIndex(FontUnits);
	
	// And now select the correct item on the list of units
	pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_PAGEUNITS), PageUnitsIndex);
	pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_FONTUNITS), FontUnitsIndex);

//	// Remember to add in the automatic units to the index calculation
//	if (ScaledUnits != AUTOMATIC)
//	{
//		// Not Automatic so work out the index which this item corresponds to on the unit list
//		INT32 ScaleUnitsIndex = pDocUnitList->FindUnitIndex(ScaledUnits);
//		pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_SCALEDUNITS), ScaleUnitsIndex + 1);
//	}
//	else
//		pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_SCALEDUNITS), 0);

	// Note entry state of controls
	//OldScaledUnits = ScaledUnits; 
	OldPageUnits = PageUnits; 
	OldFontUnits = FontUnits; 
	
	m_pOldColourUnit = GetColourUnitPreference();
	if (m_pOldColourUnit == NULL)
		return FALSE;
	m_ColourUnitRadioGroup.SetDefault(m_pOldColourUnit);

	EnableControls();

	return TRUE;
}

/********************************************************************************************

>	ScaleUnit* UnitsTab::GetColourUnitPreference()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the preferred colour unit
				NULL if an error occurred
	Purpose:	Retrieves the user's Colour Unit preference to be used by the Colour Editor
				If no preference is found StandardUnit::UnitPercent is the default
	Scope:		static

********************************************************************************************/
ScaleUnit* UnitsTab::GetColourUnitPreference()
{
	String_32 DefaultUnitString;
	Camelot.GetPrefDirect(ColourUnitSection, ColourUnitPreference, &DefaultUnitString);
	ScaleUnit* pColourUnit = StandardUnit::PercentGroup.FindUnitFromQualifier(DefaultUnitString);
	if (pColourUnit == NULL)		// no default has been set
		pColourUnit = &StandardUnit::UnitPercent;
	if (!pColourUnit->IS_KIND_OF(ScaleUnit))
	{
		ERROR3("m_pOldColourUnit is not");
		return NULL;
	}
	return pColourUnit;
}

		
/********************************************************************************************

>	BOOL UnitsTab::InitUnitListControls()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Fills in all the drop lists and combo boxes of units with the ones in the
				document.
	SeeAlso:	-

********************************************************************************************/

BOOL UnitsTab::InitUnitListControls()
{                            
TRACEUSER( "Neville", _T("UnitsTab::InitControls()\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UnitsTab::InitUnitListControls called with no dialog pointer");
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitsTab::InitUnitListControls called with no doc unit list pointer");

	INT32 i;
	Unit* pUnit = NULL;

	// Make sure all the lists are blank
	pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_PAGEUNITS));
	//pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_SCALEDUNITS));
	pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_FONTUNITS));

	pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_UNITSLIST));

	//SetUnitToken(_R(IDC_OPTS_SCALEDUNITS),AUTOMATIC,0);

	UnitType ThisUnitType = NOTYPE;
	for (i = 0; i < pDocUnitList->GetNumUnits(); i++)
	{
		pUnit = pDocUnitList->FindUnit(i);
		ThisUnitType = pUnit->GetUnitType();
		SetUnitToken(_R(IDC_OPTS_PAGEUNITS), ThisUnitType , i);
		//SetUnitToken(_R(IDC_OPTS_SCALEDUNITS), ThisUnitType, i+1);
		SetUnitToken(_R(IDC_OPTS_FONTUNITS), ThisUnitType, i);
	}

	// Update the list of user unit details
	pUnit = pDocUnitList->FindFirstUserUnit();
	for (i = 0; pUnit != NULL; i++)
	{
		ShowUnitDetails(_R(IDC_OPTS_UNITSLIST),pUnit->GetUnitType(), i);
		pUnit = pDocUnitList->FindNextUserUnit(pUnit);
	}

	return TRUE;
}           


/********************************************************************************************

>	BOOL UnitsTab::EnableControls()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Enables or disables the controls depending on the current context, i.e.
				the controls are disabled if there is no selection, etc.
	SeeAlso:	-

********************************************************************************************/

BOOL UnitsTab::EnableControls()
{
TRACEUSER( "Neville", _T("UnitsTab::EnableControls\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UnitsTab::EnableControls called with no dialog pointer");
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitsTab::EnableControls called with no doc unit list pointer");

 	BOOL enable = (pDocUnitList->FindFirstUserUnit() != NULL);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_DELETEUNIT),	enable);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_UNITPROPERTY),	enable);

	return TRUE;
}           



/******************************************************************************************

>	BOOL UnitsTab::InitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the Units section of the options dialog box.
				This section includes the:-
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL UnitsTab::InitSection()
{
TRACEUSER( "Neville", _T("InitUnitsSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UnitsTab::InitSection called with no dialog pointer");

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_UNITS_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_INFO), DocumentName);

	m_ColourUnitRadioGroup.SetAssociations((DialogOp*)pPrefsDlg, ColourUnitAssociations, NUM_CLR_DEFAULTS);

	// If no current document then do nothing as this may be valid
	// Use the document specified in the base class (OptionsTabs)
	if (pDocument != NULL)
	{
		// This uses current doc instead of selected doc
		//pDocUnitList = DocUnitList::GetCurrentDocUnitList();
		pDocUnitList = pDocument->GetDocUnitList();
		ERROR3IF(pDocUnitList == NULL,"Where's the current doc unit list eh?");

		if (pDocUnitList != NULL)
		{
			// Find out if we have any user defined units in this document 
			Unit* pUserUnit = pDocUnitList->FindFirstUserUnit();
			if (pUserUnit != NULL)
	        	CurrentUserUnitType = pUserUnit->GetUnitType();
			else
				CurrentUserUnitType  = NOTYPE;
		}
		else
		{
			// Could not create and hence find any units
	  		pDocUnitList = NULL;		// reset the unit list to null just in case
			// Ensure our section is greyed.
			GreySection();
	  		return FALSE;				// Could not initialise ok so fail 
		}					

		// Section = Units

		InitControls();
		SelectCurrentUserUnit();
		//ShowUnitDetails();
	}
	else
	{
		// Ensure our section is greyed.
		GreySection();
	}
			
	return TRUE;
}



/******************************************************************************************
***** User units properties dialog box													***
******************************************************************************************/



/******************************************************************************************

>	BOOL UnitPropertiesDlg::InvokeDialog(UnitType CurrentUserUnitType) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95
	Inputs:		The UnitType of the user unit to edit.
	Outputs:	-
	Returns:	True if Ok used or False if Cancel used.
	Purpose:	How to invoke or start a UnitPropertiesDlg box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL UnitPropertiesDlg::InvokeDialog(UnitType CurrentUserUnitType)
{
	OpDescriptor *pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_UNITPROPERTIESDLG);

	ERROR3IF(pOpDesc == NULL,"UnitPropertiesDlg::InvokeDialog is unable to find the UnitPropertiesDlg OpDescriptor");

	// Actually start up the user units properties dialog box.
	// Send in some useful paramters
	BOOL IsEditOK = FALSE;
	UnitType CurUserUnitType = CurrentUserUnitType;
	OpParam Params((void *)&CurUserUnitType, (void *)&IsEditOK);
	if (pOpDesc != NULL)
		pOpDesc->Invoke(&Params);

	return IsEditOK;
}



/******************************************************************************************

>	UnitPropertiesDlg::UnitPropertiesDlg(): DialogOp(UnitPropertiesDlg::IDD, UnitPropertiesDlg::Mode) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	UnitPropertiesDlg constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

UnitPropertiesDlg::UnitPropertiesDlg(): DialogOp(UnitPropertiesDlg::IDD, UnitPropertiesDlg::Mode) 
{
	// Set up some useful defaults
	CurrentUserUnitType = NOTYPE;
	pDocUnitList = NULL;
	pIsOk = NULL;
}

/******************************************************************************************

>	BOOL UnitPropertiesDlg::CommitDialogValues(UnitPropertiesDlg* pBmpPrefs)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95
	Inputs:		pointer to the dialogue box
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the dialog box and sets the return values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL UnitPropertiesDlg::CommitDialogValues()
{
	ERROR3IF(this == NULL, "UnitPropertiesDlg::CommitDialogValues called after duff initialisation?!");
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitPropertiesDlg::CommitDialogValues called with no doc unit list pointer");

	// Check if the user has selected a valid user unit to edit
	// If not do nothing 
	if (CurrentUserUnitType != NOTYPE)
	{
		// Read in the current unit values from the fields available
		Unit* pCurUserUnit = NULL;
		pCurUserUnit = pDocUnitList->FindUnit(CurrentUserUnitType);

		// General validation BOOLEAN 
		BOOL Valid	= FALSE;

		// Is the prefix or suffix radio buttons selected
		BOOL Prefix = GetLongGadgetValue(_R(IDC_OPTS_UNITPREFIX), 0, 1, 0, &Valid);

		// Work out the abbreviation that it has been gievn
		String_256 SpecifierStr = "";
		SpecifierStr = GetStringGadgetValue(_R(IDC_OPTS_UNITSPEC), &Valid);
		if (SpecifierStr.Length() == 0)
		{
			// There is a problem so warn the user that the value is incorrect
			InformWarning(_R(IDE_OPTS_BADSPECSTRING));
			return FALSE;
		}

		// Work out the name that the unit has been given
		String_256 TokenStr = "";
		TokenStr     = GetStringGadgetValue(_R(IDC_OPTS_UNITTOKEN), &Valid);
		if (TokenStr.Length() == 0)
		{
			// There is a problem so warn the user that the value is incorrect
			InformWarning(_R(IDE_OPTS_BADTOKENSTRING));
			return FALSE;
		}

		// Work out the number of this units that the user has specified as being
		// equivalent to the number of specified units.

		// We need to set a maximum value as some things are bound to fall over if not 
		// Status line x,y display is usually the first.
		const double MaxValue = (double)INT32_MAX;
		// Set up useful defaults in case of failure
		double NewBaseDenominator = 0.0;
		NewBaseDenominator = GetDoubleGadgetValue(_R(IDC_OPTS_NUMNEWUNITS),
						   							 0, MaxValue, 
													 _R(IDE_OPTS_BADUNITDENOMINATOR), &Valid);
		// If an invalid value has been entered then exit immeditaely, the user has
		// already been warned
		if (!Valid)
			return FALSE;
			
		// Must check if zero as this is a very bad value to set and difficult to check above.
		if (NewBaseDenominator == 0 )
		{
			// There is a problem so warn the user that the value is incorrect
			InformWarning(_R(IDE_OPTS_BADUNITDENOMINATOR));
			return FALSE;
		} 			

		// Now work out the number of the chosen units that the user has specified as being
		// equivalent to this unit. 
		// Set up useful defaults in case of failure
		double NewBaseNumerator = 0.0;
		NewBaseNumerator = GetDoubleGadgetValue(_R(IDC_OPTS_NUMBASEUNITS),
						   						 0, MaxValue, 
						   						 _R(IDE_OPTS_BADUNITNUMERATOR), &Valid);
		// If an invalid value has been entered then exit immeditaely, the user has
		// already been warned
		if (!Valid)
			return FALSE;

		// Must check if zero as this is a very bad value to set and difficult to check above.
		if (NewBaseNumerator == 0 )
		{
			// There is a problem so warn the user that the value is incorrect
			InformWarning(_R(IDE_OPTS_BADUNITNUMERATOR));
			return FALSE;
		} 			

		// Work out what unit type has been selected that this user unit is based on  
		WORD Index = 0;
		UnitType NewBaseUnitType = NOTYPE;
		Unit * pBaseUnit = NULL; 
		GetValueIndex(_R(IDC_OPTS_BASEUNIT), &Index);
		pBaseUnit = pDocUnitList->FindUnit(Index);
		if (pBaseUnit)
			NewBaseUnitType = pBaseUnit->GetUnitType();

		// As long as we have a valid user unit pointer, go and set the new values in it
		//TCHAR* pTokenStr = TokenStr;
		//TCHAR* pSpecifierStr = SpecifierStr;
		String_32  Str32 = _T("");
		if (pCurUserUnit != NULL)
		{
			BOOL ok = TRUE;
			
			// Set up the new values that we have verified may be useful new ones to use
			pCurUserUnit->SetPrefixState(Prefix);
			
			//Str32 = pTokenStr;
			TokenStr.Left(&Str32, Str32.MaxLength());
			ok = pCurUserUnit->SetToken(Str32);
			if (!ok)
			{
				InformWarning(_R(IDE_OPTS_BADTOKENSTRING));
				return FALSE;
			}

			//Str32 = pSpecifierStr;
			SpecifierStr.Left(&Str32, Str32.MaxLength());
			ok = pCurUserUnit->SetSpecifier(Str32);
			if (!ok)
			{
				InformWarning(_R(IDE_OPTS_BADSPECSTRING));
				return FALSE;
			}

			ok = pCurUserUnit->SetBaseNumerator(NewBaseNumerator);
			if (!ok)
			{
				InformWarning(_R(IDE_OPTS_BADUNITNUMERATOR));
				return FALSE;
			}

			ok = pCurUserUnit->SetBaseDenominator(NewBaseDenominator);
			if (!ok)
			{
				InformWarning(_R(IDE_OPTS_BADUNITDENOMINATOR));
				return FALSE;
			}

			if (NewBaseUnitType != NOTYPE)
				pCurUserUnit->SetBaseUnitType(NewBaseUnitType);

			pDocUnitList->RecalcUnit(pCurUserUnit);
			//pDocUnitList->UnitHasChanged(pCurUserUnit->GetUnitType());
			pDocUnitList->UnitHasChanged(CurrentUserUnitType);
		}
	}

	// Everything went swimminlgly and so we can exit in the knowledge that the dialog 
	// box can be closed.
	return TRUE;
}

/******************************************************************************************

>	MsgResult UnitPropertiesDlg::Message(CDlgMessage DlgMsg, CGadgetID Gadget) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the bitmap preferences dialog's messages
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

MsgResult UnitPropertiesDlg::Message(Msg* Message)
{
	ERROR3IF(this == NULL, "UnitPropertiesDlg::Message - Null this!");
	ERROR3IF(Message == NULL, "UnitPropertiesDlg::Message - Parameters not been set up");

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		MsgResult Result;

		BOOL EndDialog = FALSE;		// TRUE if we should quit the dialog

		// Should now handle the required messages that we respond to
		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				{
					InitDialog();
				}
			break;

			case DIM_COMMIT:
				{
					// (ok) accept all changes that the user has made to the settings
					BOOL ok;
					ok = CommitDialogValues();
					if (ok)
					{
						// Values found ok so quit dialogue
						EndDialog = TRUE;				// Flag to close and end
						if (pIsOk)
							*pIsOk = TRUE;				// flag an ok return
					}
				}
			break;

			case DIM_CANCEL:
				// Cancel all changes that the user has made to the settings
				EndDialog = TRUE;	// Flag to close and end
				if (pIsOk)
					*pIsOk = FALSE;	// flag a bad return
			break;

//			case DIM_LFT_BN_CLICKED:
//				// A control on the dialog box has been clicked...
//				switch (Msg->GadgetID)
//				{
//					case _R(IDC_BMPOPTS_SPREAD):
//					break;
//				}
//			break; // DIM_LFT_BN_CLICKED		
//
			case DIM_SELECTION_CHANGED:
			{
				// A control on the dialog box has been typed in...
				//ERROR2IF(pDocUnitList == NULL,FALSE,"UnitsTab::HandleMsg called with no doc unit list");
				
				WORD Index = 0;
				Unit* pUnit = NULL;
				Unit* pCurUserUnit = NULL;

				if (Msg->GadgetID == _R(IDC_OPTS_BASEUNIT))
				{
					// The user has chosen a new base unit
					GetValueIndex(_R(IDC_OPTS_BASEUNIT),&Index);
					pUnit        = pDocUnitList->FindUnit(Index);
					pCurUserUnit = pDocUnitList->FindUnit(CurrentUserUnitType);
					if (pUnit->GetUnitType() == CurrentUserUnitType)
					{
						// Warn the user that this is the current unit
						Error::SetError(_R(IDS_UNITERROR_BASEISCURRENT),0);
						InformError();
						// And reselect the old one unit type in the list
						UnitType BaseUnit = pCurUserUnit->GetBaseUnitType(); 
						INT32 BaseUnitIndex = pDocUnitList->FindUnitIndex(BaseUnit);
						SetSelectedValueIndex(_R(IDC_OPTS_BASEUNIT), BaseUnitIndex);
					}
					else if (pDocUnitList->IsDescendent(pUnit,CurrentUserUnitType))
					{
						// Warn the user that the selected unit is a descendent
						Error::SetError(_R(IDS_UNITERROR_BASEISDESCENDENT),0);
						InformError();
						// And reselect the old one unit type in the list
						UnitType BaseUnit = pCurUserUnit->GetBaseUnitType();
						INT32 BaseUnitIndex = pDocUnitList->FindUnitIndex(BaseUnit);
						SetSelectedValueIndex(_R(IDC_OPTS_BASEUNIT), BaseUnitIndex);
					}
					// Otherwise selection is ok so do nothing
				}
			}
			break; // DIM_SELECTION_CHANGED

			case DIM_TEXT_CHANGED:
				if (Msg->GadgetID == _R(IDC_OPTS_UNITTOKEN))
				{
					// User is editing the main user unit name
					// Update the text in the X <UnitsName> are equivalent to ... field 
					BOOL Valid;
					String_256 TokenStr = GetStringGadgetValue(_R(IDC_OPTS_UNITTOKEN),&Valid);
					SetStringGadgetValue(_R(IDC_OPTS_UNITTOKEN2),TokenStr);
				}
				break;	// DIM_TEXT_CHANGED

			default:
				break;
		}

		// Must do this before the Close and End
		Result = DialogOp::Message(Message);

		// End dialog here
		if (EndDialog) 
		{
			Close();				// Hide the dialog box
			End();					// Finish the operation
		}

		// The message was for our dialog box so return that we have handled it, if necessary
		return Result;
	}

	return DialogOp::Message(Message); 
}


/******************************************************************************************

>	OpState	UnitPropertiesDlg::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	UnitPropertiesDlg GetState method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

OpState	UnitPropertiesDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}

/******************************************************************************************

>	BOOL UnitPropertiesDlg::InitDialog()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95
	Inputs:		pointer to the dialog box
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial dialog values 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL UnitPropertiesDlg::InitDialog()
{
	ERROR3IF(this == NULL, "UnitPropertiesDlg::InitDialog called after duff initialisation?!");

	// Fill in the based on unit list with all the current units that are available
	INT32 i;
	Unit* pUnit = NULL;

	DeleteAllValues(_R(IDC_OPTS_BASEUNIT));

	UnitType ThisUnitType = NOTYPE;
	for (i = 0; i < pDocUnitList->GetNumUnits(); i++)
	{
		pUnit = pDocUnitList->FindUnit(i);
		ThisUnitType = pUnit->GetUnitType();
		SetUnitToken(_R(IDC_OPTS_BASEUNIT), ThisUnitType, i);
	}

	DisplayUserUnitDetails();

	return TRUE;
}


/******************************************************************************************

>	BOOL UnitPropertiesDlg::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	UnitPropertiesDlg Init method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL UnitPropertiesDlg::Init()
{
	BOOL InitOK;

	InitOK = RegisterOpDescriptor(
								0,								// Tool ID
								_R(IDS_UNITPROPERTIESDLG),				// String resource ID
								CC_RUNTIME_CLASS(UnitPropertiesDlg),	// Runtime class
								OPTOKEN_UNITPROPERTIESDLG,		// Token string
								GetState,						// GetState function
								0,								// help ID
								0,  							// bubble help
								0,								// resource ID
								0,								// control ID
								SYSTEMBAR_ILLEGAL,				// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								0,								// No vertical counterpart
								_R(IDS_UNITPROPERTIESDLG_ONE),		// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);

	return (InitOK);
}




/********************************************************************************************

>	BOOL UnitPropertiesDlg::Create()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	UnitPropertiesDlg create method 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

	   
BOOL UnitPropertiesDlg::Create()
{                            
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitPropertiesDlg::Create No doc unit list attached to this doc yet");

	if (DialogOp::Create())
	{ 
		// Set the initial control values 
		// Dialog now Modal so set up happens in the message handler
		//InitDialog(this);

		return TRUE; 
    }
    else
    {
		return FALSE; 
	}
}




/******************************************************************************************

>	void UnitPropertiesDlg::Do(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void UnitPropertiesDlg::Do(OpDescriptor*)
{
	BOOL ok;
	
	// Get a current document unit list
	Document * pDocument = Document::GetSelected();
	ERROR3IF(pDocument == NULL,"UnitPropertiesDlg::DoWithParam No selected document");

	// Set up some useful defaults
	pDocUnitList = NULL;
	pIsOk = NULL;
	
	if (pDocument)
		pDocUnitList = pDocument->GetDocUnitList();
	ERROR3IF(pDocUnitList == NULL,"UnitPropertiesDlg::DoWithParam No doc unit list attached to this doc yet");

	// Force a decent CurrentUserUnitType
	CurrentUserUnitType = NOTYPE;

	// Force the dialog box to be created, as it is modal it will be opened via a message
	ok = Create();

	if ( !ok )
	{
		// Could not create the dialog box so call inform error 
		InformError();
  		End(); 		   // End the operation 
	}
}

/******************************************************************************************

>	void UnitPropertiesDlg::DoWithParam(OpDescriptor*, OpParam* Param)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95
	Inputs:		OpDescriptor - Not Used.
				pOpParam->Param1 - The user unit type to be edited.
				pOpParam->Param2 - Should be a pointer to a BOOL that will hold the result of
				the operation.
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user.
				The parameter is actually the user unit type to be edited.	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void UnitPropertiesDlg::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
	ERROR3IF(pOpParam == NULL, "UnitPropertiesDlg::DoWithParam - NULL Param passed in");

	// Get a current document unit list
	Document * pDocument = Document::GetSelected();
	ERROR3IF(pDocument == NULL,"UnitPropertiesDlg::DoWithParam No selected document");

	// Get the unit list attached to the current document
	pDocUnitList = NULL;
	if (pDocument)
		pDocUnitList = pDocument->GetDocUnitList();
	ERROR3IF(pDocUnitList == NULL,"UnitPropertiesDlg::DoWithParam No doc unit list attached to this doc yet");

	// Use the OpParam that has been passed in to us
	// Force a decent CurrentUserUnitType
	CurrentUserUnitType = NOTYPE;
	UnitType * pCurUserUnitType = (UnitType*) (void *) pOpParam->Param1;
	pIsOk = (BOOL*) (void *) pOpParam->Param2;

	CurrentUserUnitType = *pCurUserUnitType;

	BOOL ok;
	
	// Force the dialog box to be created, as it is modal it will be opened via a message
	ok = Create();

	if ( !ok )
	{
		// Could not create the dialog box so call inform error 
		InformError();
  		End(); 		   // End the operation 

		*pIsOk = FALSE;	// flag a bad return
	}
}

/********************************************************************************************

>	void UnitPropertiesDlg::SetUnitToken(CGadgetID ID, UnitType ThisUnitType, INT32 Index = -1,
								BOOL EndOfList = FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/94
	Inputs:		ID = the ID of the control to put the unit token in
				ThisUnitType = the type of the unit whose token you want putting in the control
				Index = index into a drop list (-1 = main field)
				EndOfList - if TRUE, item is stuck on end of list, regardless of Index
	Outputs:	-
	Returns:	-
	Purpose:	Readable function for putting unit token strings in controls.
				MUST have a valid doc unit list
	SeeAlso:	-

********************************************************************************************/

void UnitPropertiesDlg::SetUnitToken(CGadgetID ID, UnitType ThisUnitType, INT32 Index, BOOL EndOfList)
{
	if (pDocUnitList)
	{
		String_32 Str = pDocUnitList->GetToken(ThisUnitType);
		String_256 Str256 = Str;
		SetStringGadgetValue(ID, Str256, EndOfList, Index);
	}
}

/********************************************************************************************

>	BOOL UnitPropertiesDlg::DisplayUserUnitDetails()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Displays the unit CurrentUserUnitType in the user-definable area of the
				dialog.
				Called when a new unit is made, or when another unit selected from the list
				of user units.
				UpdateUnitName added to allow suppression of the unit name updating. As when
				a new unit is choosen from the user units list, the name is highlighted, and
				hence overtyping works. If we fill the name in then the hightlighting is
				removed but overtyping still works! Only do it if user units present.
	SeeAlso:	UnitsTab::ShowUnitDetails 

********************************************************************************************/

BOOL UnitPropertiesDlg::DisplayUserUnitDetails()
{                            
	ERROR2IF(this == NULL,FALSE,"UnitPropertiesDlg::DisplayUserUnitDetails called with no dialog pointer");
	ERROR2IF(pDocUnitList == NULL,FALSE,"UnitPropertiesDlg::DisplayUserUnitDetails called with no doc unit list pointer");

	if (CurrentUserUnitType != NOTYPE)
	{
		Unit* pCurUserUnit = pDocUnitList->FindUnit(CurrentUserUnitType);
		ERROR2IF(pCurUserUnit == NULL,FALSE,"UnitPropertiesDlg::DisplayUserUnitDetails current user unit pointer null");

    	String_32 Str;
		//TCHAR* p = Str;
		String_256 Str256;
		//TCHAR* p256 = Str256;
		
		// Show the name of the user's unit
		Str = pCurUserUnit->GetSpecifier();
		Str256 = Str;
		SetStringGadgetValue(_R(IDC_OPTS_UNITSPEC), Str256);

		// Set up the prefix/suffix state for this user's unit.
		BOOL PrefixState = FALSE;
		BOOL SuffixState = TRUE;
		if (pCurUserUnit->IsPrefix())
		{
			PrefixState = TRUE;
			SuffixState = FALSE;	
		}
		else
		{
			PrefixState = FALSE;
			SuffixState = TRUE;	
		} 
		SetLongGadgetValue(_R(IDC_OPTS_UNITPREFIX), PrefixState);
		SetLongGadgetValue(_R(IDC_OPTS_UNITSUFFIX), SuffixState);

		// Show the chosen equivalent settings
		// Set up useful defaults in case of failure
		double NewBaseNumerator = pCurUserUnit->GetBaseNumerator();
		double NewBaseDenominator = pCurUserUnit->GetBaseDenominator();
		SetDoubleGadgetValue(_R(IDC_OPTS_NUMBASEUNITS), NewBaseNumerator);
		SetDoubleGadgetValue(_R(IDC_OPTS_NUMNEWUNITS), NewBaseDenominator);

		// Update the name
		SetUnitToken(_R(IDC_OPTS_UNITTOKEN),  CurrentUserUnitType, -1);
		SetUnitToken(_R(IDC_OPTS_UNITTOKEN2), CurrentUserUnitType, 0, TRUE);

		// Now work out which item to select on the menu
		UnitType BaseUnit = pCurUserUnit->GetBaseUnitType();
		INT32 BaseUnitIndex = pDocUnitList->FindUnitIndex(BaseUnit);
		SetSelectedValueIndex(_R(IDC_OPTS_BASEUNIT), BaseUnitIndex);
	}
	else
	{
		// Blank everything as unit is NOTYPE
		String_256 Str256 = "";

	 	SetStringGadgetValue(_R(IDC_OPTS_UNITTOKEN),	Str256);
	 	SetStringGadgetValue(_R(IDC_OPTS_UNITTOKEN2),	Str256);
	 	SetStringGadgetValue(_R(IDC_OPTS_UNITSPEC),		Str256);
	 	SetStringGadgetValue(_R(IDC_OPTS_NUMNEWUNITS),	Str256);
	 	SetStringGadgetValue(_R(IDC_OPTS_NUMBASEUNITS),	Str256);
	 	SetStringGadgetValue(_R(IDC_OPTS_BASEUNIT),		Str256);
	}

	return TRUE;
}           

#endif //webster
