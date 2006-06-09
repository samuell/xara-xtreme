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
// Implementation of the Misc tab of the options dialog box

/*

*/

#include "camtypes.h"
#include "appprefs.h"
//#include "prefsdlg.h"	// dialog/gadget ids
//#include "optsres.h"	// error strings
#include "optsmisc.h"
//#include "app.h"		// Camelot object - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "spread.h"		// So that knows spreads are derived from nodes - in camtypes.h [AUTOMATICALLY REMOVED]
#include "sprdmsg.h"	// spread messages

CC_IMPLEMENT_DYNAMIC(MiscTab, OptionsTabs)   

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

			  
/********************************************************************************************

>	MiscTab::MiscTab() 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	MiscTab constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MiscTab::MiscTab()
{   
	// Do nothing at present
}        

/********************************************************************************************

>	MiscTab::~MiscTab()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	MiscTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
MiscTab::~MiscTab()
{   
}        

/********************************************************************************************

>	BOOL MiscTab::Init()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	MiscTab initialisation routine
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
BOOL MiscTab::Init()
{   
	return TRUE;
}        


/******************************************************************************************

>	CDlgResID MiscTab::GetPageID()()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CDlgResID MiscTab::GetPageID()
{
	return _R(IDD_OPTSTAB_MISC);
}

/******************************************************************************************

>	BOOL MiscTab::IsDocumentOption()

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

BOOL MiscTab::IsDocumentOption()
{
	return FALSE;	// This tab is not a document option 
}

/******************************************************************************************

>	BOOL MiscTab::IsProgramOption()

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

BOOL MiscTab::IsProgramOption()
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
	Purpose:	Takes the values in the Misc section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL MiscTab::CommitSection()
{
TRACEUSER( "Neville", _T("commit misc section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"MiscTab::CommitSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_MISC));	// The Misc page identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Section = Misc

	// Ok has been pressed so take the values from this section of the dialog box
	BOOL Valid=0;		// Flag for validity of value
	INT32 State=0;		// Flag for state of button/switch
	BOOL SetOk=0;		// Preference value set ok

	// Section = Import
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	//State = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_IMPWITHLAYERS), 0, 1, 0, &Valid);
	State = pPrefsDlg->GetSelectedValueIndex(_R(IDC_OPTS_IMPWITHLAYERS));
	Valid = State >= 0 && State <= 2;

	SetOk = Camelot.SetPrefValue(TEXT("Filters"), TEXT("ImportWithLayers"), &State);
	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_MISC));

	// Section = Preferences

	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_SAVEONEXIT), 0, 1, 0, &Valid);
	if ( State != FALSE )
		AppPrefsDlg::SetSaveOnExit(TRUE);
	else
		AppPrefsDlg::SetSaveOnExit(FALSE);

	// Section = File menu

	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_FILELISTSIZE), 1, 9, _R(IDE_OPTS_INVALIDFILELIST), &Valid);
	if (Valid)
	{
TRACEUSER( "Neville", _T("commit file list size='%d'\n"), State);
		SetOk = Camelot.SetPrefValue(TEXT("Recent File List"), TEXT("FileListSize"), &State);
		ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_MISC));
	}
	else
		return FALSE; 			// File list size incorrect, user has been warned already

	// Section = Layers

	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_VISIBLELAYER), 0, 1, 0, &Valid);
	SetOk = Camelot.SetPrefValue(TEXT("Layers"),TEXT("ActiveLayerVisibleAndEditable"),&State);
	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_MISC));
	// Now tell the layers gallery to update itself, if present
	// Broadcast a SpreadMsg with reason code SpreadMsg::LAYERCHANGES, when the pref changes.
	// The layer gallery will then redraw itself. 
	BROADCAST_TO_ALL(SpreadMsg(pSpread, pSpread, SpreadMsg::LAYERCHANGES));

	// Section = Attributes

	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_ASKSETATTRB), 0, 1, 0, &Valid);
	SetOk = Camelot.SetPrefValue(TEXT("Attributes"),TEXT("AskBeforeSettingCurrentAttr"),&State);
	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_MISC));

	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_LASTATTRB), 0, 1, 0, &Valid);
	SetOk = Camelot.SetPrefValue(TEXT("Attributes"),TEXT("LastAttrAppliedBecomesCurrent"),&State);
	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_MISC));

//	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_ASKGREYSCALEBMP), 0, 1, 0, &Valid);
	//AttributeManger::ShouldAskAboutContoneColours = State;
//	SetOk = Camelot.SetPrefValue(TEXT("Attributes"),TEXT("AskBeforeSettingContoneColours"),&State);
//	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_MISC));

	// Section = ask before closing view

	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_CLOSEDOCVIEWS), 0, 1, 0, &Valid);
	SetOk = Camelot.SetPrefValue(TEXT("Preferences"),TEXT("RemoveExistingDocs"),&State);
	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_MISC));

	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_GROUPTRANSPARENCY), 0, 1, 0, &Valid);
	SetOk = Camelot.SetPrefValue(TEXT("Attributes"),TEXT("GroupTransparency"),&State);
	ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_MISC));

#endif //webster
	// All units work off the selected document and so we must do nothing if there is no
	// selected document. Check our class variable to see if this is true or not.
	// Units should have been set up by the caller.
	if (pDocument == NULL || pSpread == NULL)
		return TRUE;

	// Section = Constraints

	// Now the constraint angle combo box
	// Convert to double uses units and so requires a selected document
	double Angle = PI / 4; 		// Angle stored in radians in the preference system
	double AngleInDegrees = 45;	// Angle displayed in degrees to the user
	AngleInDegrees = pPrefsDlg->GetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 1, 359,
													 _R(IDE_OPTS_INVALIDANGLE), &Valid);
	if (Valid)
	{
		// Angle in correct range so put it into the preference system
		Angle = AngleInDegrees * PI/180.0;		// convert angle to radians
TRACEUSER( "Neville", _T("commit constraint angle in degrees='%d' angle='%d' Valid ='%d'\n"), AngleInDegrees, Angle, Valid);
		SetOk = Camelot.SetPrefValue(TEXT("Constraints"), TEXT("Constrain Angle"), &Angle);
		ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_MISC));
	}
	else
		return FALSE; 			// Angle incorrect, user has been warned already

	// Now the duplicate distance
	// These should be shown in scaled coordinates as they measure distances on the page 
	// pSpread tells the dimension class which document to pick the dimensioning from
	MILLIPOINT x = 0;
	MILLIPOINT y = 0;
//	const INT32 Maxxy = INT_MAX;		// maximum value allowed as the distance
	BOOL bValidX = FALSE;
	BOOL bValidY = FALSE;

	ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_MISC));	// The Misc page identifier
	x = pPrefsDlg->GetDimensionGadgetValue(_R(IDC_OPTS_XDUPLICATE), pSpread, &bValidX);
/*	if (Valid && (x > -Maxxy) && (x < Maxxy))
	{
		// The preference system needs integers rather than MILLIPOINTS.
		INT32 xsize = (INT32)x;
		SetOk = Camelot.SetPrefValue(TEXT("Duplicate"), TEXT("DuplicatePlacementX"), &xsize);
		ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_MISC));
	}
	else
	{
		// There is a problem so warn the user that the x size is incorrect
		InformWarning(_R(IDE_OPTS_INVALIDDISTANCE));
		return FALSE;
	} 			
*/
	ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_MISC));	// The Misc page identifier
	y = pPrefsDlg->GetDimensionGadgetValue(_R(IDC_OPTS_YDUPLICATE), pSpread, &bValidY);
/*	if (Valid && (y > -Maxxy) && (y < Maxxy))
	{
		// The preference system needs integers rather than MILLIPOINTS.
		INT32 ysize = (INT32)y;
		SetOk = Camelot.SetPrefValue(TEXT("Duplicate"), TEXT("DuplicatePlacementY"), &ysize);
		ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_MISC));
	}
	else
	{
		// There is a problem so warn the user that the y size is incorrect
		InformWarning(_R(IDE_OPTS_INVALIDDISTANCE));
		return FALSE;
	} 			
*/
	if (bValidX && bValidY)
	{
		pDocument->SetDuplicationOffset(DocCoord(x,y));
		pDocument->SetModified(TRUE);
	}
	else
	{
		// There is a problem so warn the user that the y size is incorrect
		InformWarning(_R(IDE_OPTS_INVALIDDISTANCE));
		return FALSE;
	} 			

	// Now the nudge size distance
	MILLIPOINT Nudge = 0;
	ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_MISC));	// The Misc page identifier
	Nudge = pPrefsDlg->GetDimensionGadgetValue(_R(IDC_OPTS_NUDGESIZE), pSpread, &Valid);
	if (Valid && (Nudge > 0) && (Nudge < MaxDocCoord))
	{
		// Value apparently read ok so set up the value in the preference system
		// The preference system needs UINTs rather than MILLIPOINTS.
		UINT32 NudgeSize = (UINT32)Nudge;
		pDocument->SetDocNudge (NudgeSize);
		pDocument->SetModified (TRUE);
		//SetOk = TRUE;//Camelot.SetPrefValue(TEXT("Nudge"), TEXT("StepSize"), &NudgeSize);
		//ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_MISC));
	}
	else
	{
		// There is a problem so warn the user that the Nudge size is incorrect
		InformWarning(_R(IDE_OPTS_INVALIDNUDGE));
		return FALSE;
	}

	return TRUE;
}

/******************************************************************************************

>	BOOL MiscTab::NewUnitsInSection()

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

BOOL MiscTab::NewUnitsInSection()
{
TRACEUSER( "Neville", _T("MiscTab::NewUnitsInSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"MiscTab::NewUnitsInSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_MISC));	// The misc page identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Call the code which sets up any controls displaying measurements in this section code
	InitControlsWithUnits();
	
	return TRUE;
}
	
#if 0
	// Set up the common variables
	BOOL Valid=FALSE;		// Flag for validity of value

	// We will not error on any of the conversions as this might mean double errors
	// when say an apply is used but instead just ignore the conversion. This also means
	// not specifying an error message in the GetUnitGadgetValue calls.

	// All units work off the selected document and so we must do nothing if there is no
	// selected document. Check our class variable to see if this is true or not.
	// Units should have been set up by the caller.
	if (pDocument == NULL || pSpread == NULL)
		return TRUE;

	// Now the duplicate distance
	INT32 x = 0;
	INT32 y = 0;
	const INT32 Maxxy = INT_MAX;		// maximum value allowed as the distance

	x = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_XDUPLICATE), CurrentPageUnits,
										 -Maxxy, Maxxy, NULL, &Valid);
	if (Valid)
	{
		ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_XDUPLICATE), CurrentPageUnits, x, FALSE, -1);
	}

	y = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_YDUPLICATE), CurrentPageUnits,
										 -Maxxy, Maxxy, NULL, &Valid);
	if (Valid)
	{
		ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_YDUPLICATE), CurrentPageUnits, y, FALSE, -1);
	}

	// Now the nudge size distance
	UINT32 Nudge = 0;
	Nudge = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_NUDGESIZE), CurrentPageUnits,
											 0, INT_MAX, NULL, &Valid);
	if (Valid)
	{
		ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_NUDGESIZE), CurrentPageUnits, Nudge, FALSE, -1);
	}

	return TRUE;
}
#endif

/********************************************************************************************

>	BOOL MiscTab::GreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents 
	Errors:		-
	SeeAlso:	MiscTab::UngreySection;

********************************************************************************************/

BOOL MiscTab::GreySection()
{
TRACEUSER( "Neville", _T("GreySection in MiscTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"MiscTab::GreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_MISC));	// The MiscTab identifier
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

>	BOOL MiscTab::UngreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document 
	Errors:		-
	SeeAlso:	MiscTab::UngreySection;

********************************************************************************************/

BOOL MiscTab::UngreySection()
{
TRACEUSER( "Neville", _T("UngreySection in MiscTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"MiscTab::UngreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_MISC));	// The MiscTab identifier
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

>	BOOL MiscTab::ChangeControlStatus(const BOOL Status)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 
	Errors:		-
	SeeAlso:	MiscTab::GreySection;	MiscTab::UngreySection;

********************************************************************************************/

BOOL MiscTab::ChangeControlStatus(const BOOL Status)
{
	// Grey/ungrey the controls
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_INFO), Status);

	// Grey/ungrey the controlling Switch
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_IMPWITHLAYERS), Status);

//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_PREFSTXT), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SAVEONEXIT), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_SAVENOW), Status);

//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_FILETXT), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_FILELISTTXT), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_FILELISTSIZE), Status);

//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_VISIBLELAYER), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_LASTATTRB), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_ASKSETATTRB), Status);
//  pPrefsDlg->EnableGadget(_R(IDC_OPTS_CLOSEDOCVIEWS), Status);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_EDITGRP), Status);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_CONSTRAINTTXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_ANGLECONST), Status);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_NUDGETXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_NUDGESIZE), Status);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_DUPLICATETXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_XDUPLICATETXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_YDUPLICATETXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_XDUPLICATE), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_YDUPLICATE), Status);

	return TRUE;
}			

/********************************************************************************************

>	BOOL MiscTab::UpdateSection(String_256 *DocumentName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		Name of document
	Outputs:	-
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. Should only init the tab if the page is present.  
				The document name allows the info field on the tab to be filled in correctly.
	Errors:		-
	SeeAlso:	MiscTab::GreySection; MiscTab::UngreySection; MiscTab::InitSection;

********************************************************************************************/

BOOL MiscTab::UpdateSection(String_256 *DocumentName)
{
TRACEUSER( "Neville", _T("MiscTab::UpdateSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"MiscTab::UpdateSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_MISC));	// The MiscTab identifier
	if (!ok)
		return TRUE;		// page not present

	// Remove any lists that we have created.
	//pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_ANGLECONST));

	// Call the code which sets up any controls displaying measurements in this section code
	InitControlsWithUnits();

	return TRUE;
}


/********************************************************************************************

>	BOOL MiscTab::UpdateImportWithLayers()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the state of the option Import with layers has been changed.
				We therefore need to update the state of this button on this tab.
				Should only update it if the page is present.  
	Errors:		-
	SeeAlso:	MiscTab::GreySection; MiscTab::UngreySection; MiscTab::InitSection;

********************************************************************************************/

BOOL MiscTab::UpdateImportWithLayers()
{
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
TRACEUSER( "Neville", _T("MiscTab::UpdateImportWithLayers\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"MiscTab::UpdateImportWithLayers called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_MISC));	// The MiscTab identifier
	if (!ok)
		return TRUE;		// page not present

	BOOL ReadOk = FALSE;
	INT32 ImportWithLayers = 2; // use layer names as the default
	ReadOk = Camelot.GetPrefValue(TEXT("Filters"), TEXT("ImportWithLayers"), &ImportWithLayers);
//	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_IMPWITHLAYERS), ImportWithLayers);
	pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_IMPWITHLAYERS), ImportWithLayers);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_MISC));

	return (TRUE);
#else 
	return FALSE;
#endif //webster
}

/********************************************************************************************

>	BOOL MiscTab::UpdateAskBefore()

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

BOOL MiscTab::UpdateAskBefore()
{
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
TRACEUSER( "Neville", _T("MiscTab::UpdateAskBefore()\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"MiscTab::UpdateAskBefore() called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_MISC));	// The EditTab identifier
	if (!ok)
		return TRUE;		// page not present

	BOOL AskBeforeSetting = FALSE;
	BOOL ReadOk = FALSE;
	ReadOk = Camelot.GetPrefValue(TEXT("Attributes"), TEXT("AskBeforeSettingCurrentAttr"), &AskBeforeSetting);
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_ASKSETATTRB), AskBeforeSetting);
	//ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_MISC));

	return(TRUE);
#else
	return FALSE;
#endif //webster
	
}	

/********************************************************************************************

>	BOOL MiscTab::HandleMsg(DialogMsg* Msg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the Misc options tabs messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL MiscTab::HandleMsg(DialogMsg* Msg)
{
TRACEUSER( "Neville", _T("MiscTab::HandleMsg\n"));
	ERROR2IF(Msg == NULL,FALSE,"MiscTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"MiscTab::HandleMsg called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_MISC));	// The Misc page identifier
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
// Moved by Neville 4/6/97 to cover whole switch statement
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
		case DIM_SELECTION_CHANGED:
		case DIM_LFT_BN_CLICKED:
			// A control on the dialog box has been clicked...
			if (Msg->GadgetID == _R(IDC_OPTS_LASTATTRB))
			{
				// Ungrey the apply as we have clicked on this button
				OptionsTabs::SetApplyNowState(TRUE);
				// If last attibute applied becomes current is True then the ask before
				// setting attribute is superfluous as it will be ignored.
				// Get current button state. 
				BOOL Valid = TRUE;
				BOOL LastAttribute = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_LASTATTRB), 0, 1, 0, &Valid);
				// Set new state of ask before setting button.
				pPrefsDlg->EnableGadget(_R(IDC_OPTS_ASKSETATTRB), !LastAttribute);
				break;
			}
			else if (Msg->GadgetID == _R(IDC_OPTS_SAVENOW))
			{
				// save now action button pressed, save out preferences 
				// but first we must take all the current values
				// Should only be present in options mode
				if (pPrefsDlg->IsKindOf(CC_RUNTIME_CLASS(AppPrefsDlg)))
				{
					// Cast/Convert our dialog pointer to be the correct type 
					AppPrefsDlg *pOptionsDlg = (AppPrefsDlg*) pPrefsDlg;
					ok = pOptionsDlg->CommitDialogValues();
					if (ok)
						Camelot.WritePreferences();
				}
			}
			else
			{
				OptionsTabs::SetApplyNowState(TRUE);
			}
			break; // DIM_LFT_BN_CLICKED		
#endif //webster
		case DIM_TEXT_CHANGED:
			OptionsTabs::SetApplyNowState(TRUE);
			break; // DIM_TEXT_CHANGED		
		default:
			break;
	}
	
	return TRUE;
}  


/******************************************************************************************

>	BOOL MiscTab::InitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the Misc section of the options dialog box.
				This section includes the:-
					- Import with layers
					- Save settings on exit
					- Recently used file list size
					- Ask before setting attribute
					- Last attribute applied becomes current
					- Visible layer editing method
					- Constraint angle 
					- Duplication distance
					- Nudge size
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL MiscTab::InitSection()
{
TRACEUSER( "Neville", _T("InitMiscSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"MiscTab::InitSection called with no dialog pointer");

	BOOL ReadOk = FALSE; 	// Flag to say whether the preference value was read ok 
//	BOOL ok	= TRUE;			// Flag for whether value set up ok 

	// Make sure the information field displaying the name of the current document
	// is correct.
	//String_256 *DocumentName = GetDocumentName();
	//pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_DOCUMENT), DocumentName);

	// Section = Import
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
//	BOOL ImportWithLayers = FALSE;
//	ReadOk = Camelot.GetPrefValue(TEXT("Filters"), TEXT("ImportWithLayers"), &ImportWithLayers);
////	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_IMPWITHLAYERS), ImportWithLayers);
//	pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_IMPWITHLAYERS), ImportWithLayers);
//	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_MISC));

	if (pPrefsDlg->GetSelectedValueIndex(_R(IDC_OPTS_IMPWITHLAYERS)) == -1)
	{
		INT32 ImportWithLayers = 2;
		ReadOk = Camelot.GetPrefValue(TEXT("Filters"), TEXT("ImportWithLayers"), &ImportWithLayers);
		pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_IMPWITHLAYERS), _R(IDS_IMPORT_NEW_LAYER), FALSE, 0);
		pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_IMPWITHLAYERS), _R(IDS_IMPORT_ACTIVE_LAYER), FALSE, 1);
		pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_IMPWITHLAYERS), _R(IDS_IMPORT_NAMED_LAYER), FALSE, 2);
		pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_IMPWITHLAYERS), ImportWithLayers);
	}
#endif //webster

	
	
	// Section = Preferences

	// Set up the special save settings on exit switch which has no where else to live
	// at present
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	if ( AppPrefsDlg::IsSaveOnExitSet() != FALSE )
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_SAVEONEXIT), TRUE);
	else
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_SAVEONEXIT), FALSE);
#endif //webster
	// Section = File menu

//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	INT32 FileListSize = 1;
	ReadOk = Camelot.GetPrefValue(TEXT("Recent File List"), TEXT("FileListSize"), &FileListSize);
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_FILELISTSIZE), FileListSize);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_MISC));
#endif //webster
	// Section = Layers

//WEBSTER-Neville-4/6/97
#ifndef WEBSTER
	BOOL VisibleLayerEditing = FALSE;
	ReadOk = Camelot.GetPrefValue(TEXT("Layers"), TEXT("ActiveLayerVisibleAndEditable"), &VisibleLayerEditing);
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_VISIBLELAYER), VisibleLayerEditing);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_MISC));
#endif //webster

	// Section = Attributes
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	BOOL AskBeforeSetting = FALSE;
	ReadOk = Camelot.GetPrefValue(TEXT("Attributes"), TEXT("AskBeforeSettingCurrentAttr"), &AskBeforeSetting);
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_ASKSETATTRB), AskBeforeSetting);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_MISC));

	BOOL LastAttribute = FALSE;
	ReadOk = Camelot.GetPrefValue(TEXT("Attributes"), TEXT("LastAttrAppliedBecomesCurrent"), &LastAttribute);
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_LASTATTRB), LastAttribute);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_MISC));

	BOOL GroupTransparency = FALSE;
	ReadOk = Camelot.GetPrefValue(TEXT("Attributes"), TEXT("GroupTransparency"), &GroupTransparency);
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_GROUPTRANSPARENCY), GroupTransparency);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_MISC));

	// If last attibute applied becomes current is True then the ask before setting
	// attribute is superfluous as it will be ignored. 
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_ASKSETATTRB), !LastAttribute);
#endif //webster
//	BOOL AskGreyscaleBitmap = FALSE;
	//AttributeManger::ShouldAskAboutContoneColours;
//	ReadOk = Camelot.GetPrefValue(TEXT("Attributes"), TEXT("AskBeforeSettingContoneColours"), &AskGreyscaleBitmap);
//	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_ASKGREYSCALEBMP), AskGreyscaleBitmap);
//	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_MISC));

	// Section = ask before closing view
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	BOOL CloseDocViews = FALSE;
	ReadOk = Camelot.GetPrefValue(TEXT("Preferences"), TEXT("RemoveExistingDocs"), &CloseDocViews);
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_CLOSEDOCVIEWS), CloseDocViews);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_MISC));
#endif //webster

	// Set up the control list for this first time only
	SetUpAngleList();

	// Sets up controls which display measurements in units	
	InitControlsWithUnits();
	
	return TRUE;
}

/******************************************************************************************

>	BOOL MiscTab::SetUpAngleList()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets up the list of contraint angles. Try and only do this one time as
				otherwise we get a lot of flicker when it is deleted and recreated.
	Errors:		-
	SeeAlso:	InitSection()

******************************************************************************************/

BOOL MiscTab::SetUpAngleList()
{
	// Sets up the list of constraint angles
	pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 12.0);
	pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 15.0);
	pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 30.0);
	pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 45.0);
	pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 60.0);
	pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 72.0);
	pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), 90.0);
	pPrefsDlg->SetComboListLength(_R(IDC_OPTS_ANGLECONST));
	
	return TRUE;
}	


/******************************************************************************************

>	BOOL MiscTab::InitControlsWithUnits()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the controls on the Misc section of the options
				dialog box which display measurements in units.
				This section includes the:-
					- Constraint angle 
					- Duplication distance
					- Nudge size
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL MiscTab::InitControlsWithUnits()
{
TRACEUSER( "Neville", _T("InitControlsWithUnits\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"MiscTab::InitControlsWithUnits called with no dialog pointer");

	// All units work off the selected document and so we must do nothing if there is no
	// selected document. Check our clas variable to see if this is true or not.
	// Units should have been set up by the caller.
	if (pDocument == NULL || pSpread == NULL)
	{
		// If this fails then there is no document and so we must ensure
		// things which need a selected document are greyed.
		GreySection();
		return (TRUE);
	}

	BOOL ReadOk = FALSE; 	// Flag to say whether the preference value was read ok 
	BOOL ok	= TRUE;			// Flag for whether value set up ok 

	// Sets up controls which display measurements in units

	// Section = Constraints

	// Now the constraint angle combo box
	// Doubles actually need document based units to work.
	double Angle = 0.0; 		// Angle stored in radians in the preference system
	double AngleInDegrees = 0;	// Angle displayed in degrees to the user
	ReadOk = Camelot.GetPrefValue(TEXT("Constraints"), TEXT("Constrain Angle"), &Angle);
TRACEUSER( "Neville", _T("set constraint angle '%d'\n"),Angle);
	// Set up the default option displayed
	AngleInDegrees = (Angle * 180.0/PI);	// Convert angle to degrees
	ok = pPrefsDlg->SetDoubleGadgetValue(_R(IDC_OPTS_ANGLECONST), AngleInDegrees, FALSE, -1);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_MISC));

	// Now the duplicate distance
	// These should be shown in scaled coordinates as they measure distances on the page 
	// pSpread tells the dimensioning system which document to get the dimensioning
	// details from.
//	INT32 x = 0;
//	INT32 y = 0;
	DocCoord offset = pDocument->GetDuplicationOffset();
//	ReadOk = Camelot.GetPrefValue(TEXT("Duplicate"), TEXT("DuplicatePlacementX"), &x);
	ok = pPrefsDlg->SetDimensionGadgetValue(_R(IDC_OPTS_XDUPLICATE), offset.x, pSpread, TRUE, FALSE, -1);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_MISC));

//	ReadOk = Camelot.GetPrefValue(TEXT("Duplicate"), TEXT("DuplicatePlacementY"), &y);
	ok = pPrefsDlg->SetDimensionGadgetValue(_R(IDC_OPTS_YDUPLICATE), offset.y, pSpread, TRUE, FALSE, -1);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_MISC));

	// Now the nudge size distance
	UINT32 Nudge = pDocument->GetDocNudge ();
	//ReadOk = TRUE;//Camelot.GetPrefValue(TEXT("Nudge"), TEXT("StepSize"), &Nudge);
	ok = pPrefsDlg->SetDimensionGadgetValue(_R(IDC_OPTS_NUDGESIZE), Nudge, pSpread, TRUE, FALSE, -1);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_MISC));

	return TRUE;
}	
