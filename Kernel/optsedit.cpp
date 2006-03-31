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
// Implementation of the Edit tab of the options dialog box

/*

*/

#include "camtypes.h"
#include "appprefs.h"
#include "optsedit.h"
#include "app.h"		// Camelot object
#include "units.h"		// units
//#include "resource.h"	// _R(IDS_OK)

CC_IMPLEMENT_DYNAMIC(EditTab, OptionsTabs)   

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

			  
/********************************************************************************************

>	EditTab::EditTab() 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	EditTab constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

EditTab::EditTab()
{   
	// Do nothing at present
}        

/********************************************************************************************

>	EditTab::~EditTab()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	EditTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
EditTab::~EditTab()
{   
}        

/********************************************************************************************

>	BOOL EditTab::Init()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	EditTab initialisation routine
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
BOOL EditTab::Init()
{   
	return TRUE;
}        


/******************************************************************************************

>	CDlgResID EditTab::GetPageID()()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CDlgResID EditTab::GetPageID()
{
	return _R(IDD_OPTSTAB_EDIT);
}

/******************************************************************************************

>	BOOL EditTab::IsDocumentOption()

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

BOOL EditTab::IsDocumentOption()
{
	return FALSE;	// This tab is not a document option 
}

/******************************************************************************************

>	BOOL EditTab::IsProgramOption()

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

BOOL EditTab::IsProgramOption()
{
	return TRUE;	// This tab is a program option 
}


/******************************************************************************************

>	BOOL EditTab::CommitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Edit section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL EditTab::CommitSection()
{
TRACEUSER( "Neville", _T("commit edit section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"EditTab::CommitSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_EDIT));	// The edit page identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Section = Edit

	// Ok has been pressed so take the values from this section of the dialog box
	BOOL Valid=FALSE;		// Flag for validity of value
	BOOL State=FALSE;		// Flag for state of button/switch
	BOOL SetOk=TRUE;		// Preference value set ok

	// Section = Attributes

	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_ASKSETATTRB), 0, 1, 0, &Valid);
	SetOk = Camelot.SetPrefValue(TEXT("Attributes"),TEXT("AskBeforeSettingCurrentAttr"),&State);
	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_EDIT));

	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_LASTATTRB), 0, 1, 0, &Valid);
	SetOk = Camelot.SetPrefValue(TEXT("Attributes"),TEXT("LastAttrAppliedBecomesCurrent"),&State);
	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_EDIT));


	// Set up the units that are used in the currently selected document
	// All units work off the selected document and so we must do nothing if there is no
	// selected document.
	ok = SetUpDocUnits();
	if (!ok)
		return (TRUE);

	// Section = Constraints

	// Now the constraint angle combo box
	// Convert to double uses units and so requires a selected document
	double Angle = 0.0; 		// Angle stored in radians in the preference system
	double AngleInDegrees = 0;	// Angle displayed in degrees to the user
	AngleInDegrees = pPrefsDlg->GetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST),
													  0, 360, _R(IDE_OPTS_INVALIDANGLE), &Valid);
	if (Valid)
	{
		// Angle in correct range so put it into the preference system
		Angle = AngleInDegrees * PI/180.0;		// convert angle to radians
TRACEUSER( "Neville", _T("commit constraint angle in degrees='%d' angle='%d' Valid ='%d'\n"), AngleInDegrees, Angle, Valid);
		SetOk = Camelot.SetPrefValue(TEXT("Constraints"), TEXT("Constrain Angle"), &Angle);
		ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_EDIT));
	}
	else
		return FALSE; 			// Angle incorrect, user has been warned already

	// Now the duplicate distance
	INT32 x = 0;
	INT32 y = 0;
	const INT32 Maxxy = INT_MAX;		// maximum value allowed as the distance
	BOOL bValidX = FALSE;
	BOOL bValidY = FALSE;

	ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_EDIT));	// The edit page identifier
	x = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_XDUPLICATE), CurrentScaledUnits,
										 -Maxxy, Maxxy, _R(IDE_OPTS_INVALIDDISTANCE), &bValidX);
/*	if (Valid)
	{
		SetOk = Camelot.SetPrefValue(TEXT("Duplicate"), TEXT("DuplicatePlacementX"), &x);
		ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_EDIT));
	}
	else
		return FALSE; 			// Duplicate x incorrect, user has been warned already
*/
	ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_EDIT));	// The edit page identifier
	y = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_YDUPLICATE), CurrentScaledUnits,
										 -Maxxy, Maxxy, _R(IDE_OPTS_INVALIDDISTANCE), &bValidY);
/*	if (Valid)
	{
		SetOk = Camelot.SetPrefValue(TEXT("Duplicate"), TEXT("DuplicatePlacementY"), &y);
		ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_EDIT));
	}
	else
		return FALSE; 			// Duplicate y incorrect, user has been warned already
*/
	if (bValidX && bValidY)
	{
		pDocument->SetDuplicationOffset(DocCoord(x,y));
		pDocument->SetModified(TRUE);
	}
	else
		return FALSE;

	// Now the nudge size distance
	UINT32 Nudge = 0;
	ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_EDIT));	// The edit page identifier
	Nudge = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_NUDGESIZE), CurrentScaledUnits,
											 0, INT_MAX, _R(IDE_OPTS_INVALIDNUDGE), &Valid);
TRACEUSER( "Neville", _T("commit nudge distance='%d' Valid ='%d'\n"),Nudge, Valid);
	if (Valid)
	{
		/*SetOk =*/ pDocument->SetDocNudge (Nudge);//Camelot.SetPrefValue(TEXT("Nudge"), TEXT("StepSize"), &Nudge);
		pDocument->SetModified (TRUE);
		//ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_EDIT));
	}
	else
		return FALSE; 			// Nudge size incorrect, user has been warned already

	return TRUE;
}

/******************************************************************************************

>	BOOL EditTab::NewUnitsInSection()

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

BOOL EditTab::NewUnitsInSection()
{
TRACEUSER( "Neville", _T("New units in edit section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"EditTab::NewUnitsInSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_EDIT));	// The edit page identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Set up the common variables
	BOOL Valid=FALSE;		// Flag for validity of value

	// We will not error on any of the conversions as this might mean double errors
	// when say an apply is used but instead just ignore the conversion. This also means
	// not specifying an error message in the GetUnitGadgetValue calls.

	// Set up the units that are used in the currently selected document
	// All units work off the selected document and so we must do nothing if there is no
	// selected document.
	ok = SetUpDocUnits();
	if (!ok)
		return (TRUE);

	// Now the duplicate distance
	INT32 x = 0;
	INT32 y = 0;
	const INT32 Maxxy = INT_MAX;		// maximum value allowed as the distance

	x = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_XDUPLICATE), CurrentScaledUnits,
										 -Maxxy, Maxxy, NULL, &Valid);
	if (Valid)
	{
		ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_XDUPLICATE), CurrentScaledUnits, x, FALSE, -1);
	}

	y = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_YDUPLICATE), CurrentScaledUnits,
										 -Maxxy, Maxxy, NULL, &Valid);
	if (Valid)
	{
		ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_YDUPLICATE), CurrentScaledUnits, y, FALSE, -1);
	}

	// Now the nudge size distance
	UINT32 Nudge = 0;
	Nudge = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_NUDGESIZE), CurrentScaledUnits,
											 0, INT_MAX, NULL, &Valid);
	if (Valid)
	{
		ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_NUDGESIZE), CurrentScaledUnits, Nudge, FALSE, -1);
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL EditTab::GreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents 
	Errors:		-
	SeeAlso:	EditTab::UngreySection;

********************************************************************************************/

BOOL EditTab::GreySection()
{
TRACEUSER( "Neville", _T("GreySection in EditTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"EditTab::GreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_EDIT));	// The EditTab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Only update if we are not already grey 
	if (GreyStatus == TRUE)
		return TRUE;

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(FALSE);
	
	GreyStatus = TRUE;

	return ok;
} 

/********************************************************************************************

>	BOOL EditTab::UngreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document 
	Errors:		-
	SeeAlso:	EditTab::UngreySection;

********************************************************************************************/

BOOL EditTab::UngreySection()
{
TRACEUSER( "Neville", _T("UngreySection in EditTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"EditTab::UngreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_EDIT));	// The EditTab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Only update if we are not already ungrey 
	if (GreyStatus == FALSE)
		return TRUE;

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(TRUE);
	
	GreyStatus = FALSE;

	return ok;
}

/********************************************************************************************

>	BOOL EditTab::ChangeControlStatus(const BOOL Status)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 
	Errors:		-
	SeeAlso:	EditTab::GreySection;	EditTab::UngreySection;

********************************************************************************************/

BOOL EditTab::ChangeControlStatus(const BOOL Status)
{
	// Grey/ungrey the controls
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_LASTATTRB), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_ASKSETATTRB), Status);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_ANGLEGROUP), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_ANGLECONST), Status);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_NUDGEGROUP), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_NUDGESIZE), Status);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_DUPLICATEGROUP), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_XDUPLICATETXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_YDUPLICATETXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_XDUPLICATE), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_YDUPLICATE), Status);

	return TRUE;
}			

/********************************************************************************************

>	BOOL EditTab::UpdateSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		Name of document
	Outputs:	-
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. Should only init the tab if the page is present.  
				The document name allows the info field on the tab to be filled in correctly.
	Errors:		-
	SeeAlso:	EditTab::GreySection; EditTab::UngreySection; EditTab::InitSection;

********************************************************************************************/

BOOL EditTab::UpdateSection(String_256 *DocumentName)
{
TRACEUSER( "Neville", _T("EditTab::UpdateSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"EditTab::UpdateSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_EDIT));	// The EditTab identifier
	if (!ok)
		return TRUE;		// page not present

	// Remove any lists that we have created.
	pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_ANGLECONST));

	// Call the normal init section code
	return InitSection();
}


/********************************************************************************************

>	BOOL EditTab::UpdateAskBefore()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has requested a change in the ask before setting
				attribute state and hence we need to update the relevent controls on this tab.
				Should only do this if the tab is present.
				Usually happens when the user applies an atribute to no selected object and
				responds Quiet to the prompt.  
	Errors:		-
	SeeAlso:	AttributeManager::AttributeSelected();

********************************************************************************************/

BOOL EditTab::UpdateAskBefore()
{
TRACEUSER( "Neville", _T("EditTab::UpdateAskBefore()\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"EditTab::UpdateAskBefore() called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_EDIT));	// The EditTab identifier
	if (!ok)
		return TRUE;		// page not present

	BOOL AskBeforeSetting = FALSE;
	BOOL ReadOk = FALSE;
	ReadOk = Camelot.GetPrefValue(TEXT("Attributes"), TEXT("AskBeforeSettingCurrentAttr"), &AskBeforeSetting);
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_ASKSETATTRB), AskBeforeSetting);
	//ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_EDIT));

	return(TRUE);
	
}	

/********************************************************************************************

>	BOOL EditTab::HandleMsg(DialogMsg* Msg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the Edit options tabs messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL EditTab::HandleMsg(DialogMsg* Msg)
{
TRACEUSER( "Neville", _T("HandleEditMsg\n"));
	ERROR2IF(Msg == NULL,FALSE,"EditTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"EditTab::HandleMsg called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_EDIT));	// The edit page identifier
	if (!ok)
		return TRUE;		// no page present = handled message
	
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:	// Initialise controls
			GreyStatus = FALSE;	// we are ungreyed by default
			ok = InitSection();
			if (!ok)
				InformError();
			break;
		case DIM_SELECTION_CHANGED:
		case DIM_LFT_BN_CLICKED:
		case DIM_TEXT_CHANGED:
			OptionsTabs::SetApplyNowState(TRUE);
			break;
	}
	return TRUE;
}  


/******************************************************************************************

>	BOOL EditTab::InitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the Edit section of the options dialog box.
				This section includes the:-
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL EditTab::InitSection()
{
TRACEUSER( "Neville", _T("InitEditSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"EditTab::InitSection called with no dialog pointer");

	BOOL ReadOk = FALSE; 	// Flag to say whether the preference value was read ok 
	BOOL ok	= TRUE;			// Flag for whether value set up ok 

	// Section = Attributes

	BOOL AskBeforeSetting = FALSE;
	ReadOk = Camelot.GetPrefValue(TEXT("Attributes"), TEXT("AskBeforeSettingCurrentAttr"), &AskBeforeSetting);
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_ASKSETATTRB), AskBeforeSetting);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_EDIT));

	BOOL LastAttribute = FALSE;
	ReadOk = Camelot.GetPrefValue(TEXT("Attributes"), TEXT("LastAttrAppliedBecomesCurrent"), &LastAttribute);
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_LASTATTRB), LastAttribute);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_EDIT));

	// Set up the units that are used in the currently selected document
	// All units work off the selected document and so we must do nothing if there is no
	// selected document.
	ok = SetUpDocUnits();
	if (!ok)
	{
		// If this fails then there is no document and so we must ensure
		// things which need a selected document are greyed.
		GreySection();
		return (TRUE);
	}

	// Section = Constraints

	// Now the constraint angle combo box
	double Angle = 0.0; 		// Angle stored in radians in the preference system
	double AngleInDegrees = 0;	// Angle displayed in degrees to the user
	ReadOk = Camelot.GetPrefValue(TEXT("Constraints"), TEXT("Constrain Angle"), &Angle);
TRACEUSER( "Neville", _T("set constraint angle '%d'\n"),Angle);
	// Set up the list of available options
	pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 12.0);
	pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 15.0);
	pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 30.0);
	pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 45.0);
	pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 60.0);
	pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 72.0);
	pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 90.0);
	pPrefsDlg->SetComboListLength(_R(IDC_OPTS_ANGLECONST));
	// Set up the default option displayed
	AngleInDegrees = (Angle * 180.0/PI);	// Convert angle to degrees
	ok = pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), AngleInDegrees, FALSE, -1);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_EDIT));

	// Now the duplicate distance
	INT32 x = 0;
	INT32 y = 0;
	DocCoord offset = pDocument->GetDuplicationOffset();
//	ReadOk = Camelot.GetPrefValue(TEXT("Duplicate"), TEXT("DuplicatePlacementX"), &x);
	ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_XDUPLICATE), CurrentScaledUnits, offset.x, FALSE, -1);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_EDIT));

//	ReadOk = Camelot.GetPrefValue(TEXT("Duplicate"), TEXT("DuplicatePlacementY"), &y);
	ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_YDUPLICATE), CurrentScaledUnits, offset.y, FALSE, -1);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_EDIT));

	// Now the nudge size distance
	UINT32 Nudge = pDocument->GetDocNudge ();
	//ReadOk SetOk = pDocument->SetDocNudge (Nudge);//Camelot.GetPrefValue(TEXT("Nudge"), TEXT("StepSize"), &Nudge);
	ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_NUDGESIZE), CurrentScaledUnits, Nudge, FALSE, -1);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_EDIT));

	return TRUE;
}
