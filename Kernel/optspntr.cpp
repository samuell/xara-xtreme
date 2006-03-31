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
// Implementation of the Pointers tab of the options dialog box

/*

*/

#include "camtypes.h"

// WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER

#include "appprefs.h"
//#include "prefsdlg.h"	// dialog/gadget ids
//#include "optsres.h"	// error strings
#include "optspntr.h"
#include "app.h"		// Camelot object
#include "units.h"		// units
#include "binds.h"		// ButtonFunction

CC_IMPLEMENT_DYNAMIC(PointersTab, OptionsTabs)   

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

			  
/********************************************************************************************

>	PointersTab::PointersTab() 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PointersTab constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

PointersTab::PointersTab()
{   
	// Do nothing at present
}        

/********************************************************************************************

>	PointersTab::~PointersTab()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PointersTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
PointersTab::~PointersTab()
{   
}        

/********************************************************************************************

>	BOOL PointersTab::Init()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	PointersTab initialisation routine
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
BOOL PointersTab::Init()
{   
	return TRUE;
}        


/******************************************************************************************

>	CDlgResID PointersTab::GetPageID()()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CDlgResID PointersTab::GetPageID()
{
	return _R(IDD_OPTSTAB_POINTERS);
}

/******************************************************************************************

>	BOOL PointersTab::IsDocumentOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a document option.	  
	Purpose:	Allows the document options dialog code to determine if this tab belongs
				to the group of document options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL PointersTab::IsDocumentOption()
{
	return FALSE;	// This tab is not a document option 
}

/******************************************************************************************

>	BOOL PointersTab::IsProgramOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a program option.	  
	Purpose:	Allows the program options dialog code to determine if this tab belongs
				to the group of program options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL PointersTab::IsProgramOption()
{
	return TRUE;	// This tab is a program option 
}


/******************************************************************************************

>	BOOL PointersTab::CommitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Pointers section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
				Middle button removed for now as nothing seems to use it.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PointersTab::CommitSection()
{
TRACEUSER( "Neville", _T("commit PointersTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PointersTab::CommitSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_POINTERS));	// The pointers page identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Section = Pointers

	// Ok has been pressed so take the values from this section of the dialog box
	BOOL Valid=FALSE;		// Flag for validity of value
	BOOL State=FALSE;		// Flag for state of button/switch
	BOOL SetOk=TRUE;		// Preference value set ok

	// Middle button removed for now as nothing seems to use it.

	// 0, 1 or 2 indicating Left Middle or Right mouse button
	ButtonFunction LeftButton = (ButtonFunction)pPrefsDlg->GetSelectedValueIndex(_R(IDC_OPTS_LEFTBUTTON));
//	ButtonFunction MiddleButton = (ButtonFunction)pPrefsDlg->GetSelectedValueIndex(_R(IDC_OPTS_MIDDLEBUTTON));
	ButtonFunction RightButton = (ButtonFunction)pPrefsDlg->GetSelectedValueIndex(_R(IDC_OPTS_RIGHTBUTTON));
	ClickModifiers::SetButtonFunc(0, LeftButton);
//	ClickModifiers::SetButtonFunc(1, MiddleButton);
	ClickModifiers::SetButtonFunc(2, RightButton);

	// All units work off the selected document and so we must do nothing if there is no
	// selected document. Check our clas variable to see if this is true or not.
	// Units should have been set up by the caller.
	if (pDocument == NULL || pSpread == NULL)
		return (TRUE);

	// Now the magnetic radii
	INT32 LineRadius = 0;					
	INT32 PointRadius = 0;					
	LineRadius = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_LINERADIUS), CurrentPageUnits,
											 0, INT_MAX, _R(IDE_OPTS_INVALIDLINERAD), &Valid);
TRACEUSER( "Neville", _T("commit LineRadius distance='%d' Valid ='%d'\n"),LineRadius, Valid);
	if (Valid)
	{
		SetOk = Camelot.SetPrefValue(TEXT("Magnetic Options"), TEXT("Line Radius"), &LineRadius);
		ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_EDIT));
	}
	else
		return FALSE; 			// Magnetic line radius incorrect, user has been warned already

	PointRadius = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_POINTRADIUS), CurrentPageUnits,
											 0, INT_MAX, _R(IDE_OPTS_INVALIDPOINTRAD), &Valid);
TRACEUSER( "Neville", _T("commit PointRadius distance='%d' Valid ='%d'\n"),PointRadius, Valid);
	if (Valid)
	{
		SetOk = Camelot.SetPrefValue(TEXT("Magnetic Options"), TEXT("Point Radius"), &PointRadius);
		ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_EDIT));
	}
	else
		return FALSE; 			// Magnetic point radius incorrect, user has been warned already


	return TRUE;
}

/******************************************************************************************

>	BOOL PointersTab::NewUnitsInSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Function called when new default units come into operation. We must convert
				all units fields to display in the newly specified units.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PointersTab::NewUnitsInSection()
{
TRACEUSER( "Neville", _T("New units in edit section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PointersTab::NewUnitsInSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_POINTERS));	// The edit page identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Set up the common variables
	BOOL Valid=FALSE;		// Flag for validity of value

	// We will not error on any of the conversions as this might mean double errors
	// when say an apply is used but instead just ignore the conversion. This also means
	// not specifying an error message in the GetUnitGadgetValue calls.

	// Call the normal code which inits the magnetic radii section
	InitMagneticRadii();

	return TRUE;
}

#if 0
	// All units work off the selected document and so we must do nothing if there is no
	// selected document. Check our clas variable to see if this is true or not.
	// Units should have been set up by the caller.
	if (pDocument == NULL || pSpread == NULL)
		return (TRUE);

	// Now the magnetic radii
	INT32 LineRadius = 0;					
	INT32 PointRadius = 0;					
	LineRadius = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_LINERADIUS), CurrentPageUnits,
											 0, INT_MAX, NULL, &Valid);
	if (Valid)
	{
		ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_LINERADIUS), CurrentPageUnits, LineRadius, FALSE, -1);
	}

	PointRadius = pPrefsDlg->GetUnitGadgetValue(_R(IDC_OPTS_POINTRADIUS), CurrentPageUnits,
											 0, INT_MAX, NULL, &Valid);
	if (Valid)
	{
		ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_POINTRADIUS), CurrentPageUnits, PointRadius, FALSE, -1);
	}

	return TRUE;
}
#endif

/********************************************************************************************

>	BOOL PointersTab::GreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents 
	Errors:		-
	SeeAlso:	PointersTab::UngreySection;

********************************************************************************************/

BOOL PointersTab::GreySection()
{
TRACEUSER( "Neville", _T("GreySection in PointersTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PointersTab::GreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_POINTERS));	// The PointersTab identifier
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

>	BOOL PointersTab::UngreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document 
	Errors:		-
	SeeAlso:	PointersTab::UngreySection;

********************************************************************************************/

BOOL PointersTab::UngreySection()
{
TRACEUSER( "Neville", _T("UngreySection in PointersTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PointersTab::UngreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_POINTERS));	// The PointersTab identifier
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

>	BOOL PointersTab::ChangeControlStatus(const BOOL Status)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 
	Errors:		-
	SeeAlso:	PointersTab::GreySection;	PointersTab::UngreySection;

********************************************************************************************/

BOOL PointersTab::ChangeControlStatus(const BOOL Status)
{
	// Grey/ungrey the controls
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_INFO), Status);

//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_BUTTONGRP), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_LEFTBUTTON), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_MIDDLEBUTTON), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_RIGHTBUTTON), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_LEFTTXT), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_MIDDLETXT), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_RIGHTTXT), Status);
//
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_RESET), Status);
	
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_MAGNETICGROUP), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_POINTRADIUSTXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_LINERADIUSTXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_POINTRADIUS), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_LINERADIUS), Status);

	return TRUE;
}			

/********************************************************************************************

>	BOOL PointersTab::UpdateSection(String_256 *DocumentName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		Name of document
	Outputs:	-
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. Should only init the tab if the page is present.  
				The document name allows the info field on the tab to be filled in correctly.
	Errors:		-
	SeeAlso:	PointersTab::GreySection; PointersTab::UngreySection; PointersTab::InitSection;

********************************************************************************************/

BOOL PointersTab::UpdateSection(String_256 *DocumentName)
{
TRACEUSER( "Neville", _T("PointersTab::UpdateSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PointersTab::UpdateSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_POINTERS));	// The PointersTab identifier
	if (!ok)
		return TRUE;		// page not present

	// Remove any lists that we have created.
	//pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_SIZELIST));

	// Call the normal init section code
	// but only need to update the magnetic radii section so do this instead
	// Stops excesive updates of the pull down lists.
	InitMagneticRadii();

	return TRUE;
}


/********************************************************************************************

>	BOOL PointersTab::HandleMsg(DialogMsg* Msg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the Edit options tabs messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL PointersTab::HandleMsg(DialogMsg* Msg)
{
TRACEUSER( "Neville", _T("PointersTab::HandleMsg\n"));
	ERROR2IF(Msg == NULL,FALSE,"PointersTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PointersTab::HandleMsg called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_POINTERS));	// The edit page identifier
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
		case DIM_LFT_BN_CLICKED:
			OptionsTabs::SetApplyNowState(TRUE);
			// A control on the dialog box has been clicked...
			switch (Msg->GadgetID)
			{
				case _R(IDC_OPTS_RESET):
					// Somebody has pressed the reset defaults button
					// Go and call the nice function which resets the values and then
					// update our values again.
					ClickModifiers::RestoreDefaults();
					InitClickLists();
					break;
				default:
					break;
			}
			break;
		case DIM_SELECTION_CHANGED:
		case DIM_TEXT_CHANGED:
			OptionsTabs::SetApplyNowState(TRUE);
			break;
	}
	return TRUE;
}  


/******************************************************************************************

>	BOOL PointersTab::InitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the Pointers section of the options dialog box.
				This section includes the:-
				 - magnetic snap distances
				 - click modifier lists
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL PointersTab::InitSection()
{
TRACEUSER( "Neville", _T("PointersTab::InitSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PointersTab::InitSection called with no dialog pointer");

	BOOL ReadOk = FALSE; 	// Flag to say whether the preference value was read ok 
	BOOL ok	= TRUE;			// Flag for whether value set up ok 

	// Set up the click lists for the left, right and middle mouse buttons
	ok = InitClickLists();
	
	// Now set up the magnetic radii section 
	ok = InitMagneticRadii();

	return TRUE;
}

/******************************************************************************************

>	BOOL PointersTab::InitClickLists()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the click modifier lists in the Pointers section
				of the options dialog box.
				Middle button removed for now as nothing seems to use it.
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL PointersTab::InitClickLists()
{
TRACEUSER( "Neville", _T("InitClickLists\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PointersTab::InitClickLists called with no dialog pointer");

	BOOL ok	= TRUE;			// Flag for whether value set up ok 

	// Middle button removed for now as nothing seems to use it.

	// Make sure that the lists are empty
	pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_LEFTBUTTON));
//	pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_MIDDLEBUTTON));
	pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_RIGHTBUTTON));

	// 0, 1 or 2 indicating Left Middle or Right mouse button
	ButtonFunction LeftButton = ClickModifiers::GetButtonFunc(0);
//	ButtonFunction MiddleButton = ClickModifiers::GetButtonFunc(1);
	ButtonFunction RightButton = ClickModifiers::GetButtonFunc(2);

	// Safety checks on the current button function values
	if (LeftButton < (ButtonFunction)0 || LeftButton >= BUTTFUNC_LAST)
	   LeftButton = BUTTFUNC_NORMAL;
//	if (MiddleButton < (ButtonFunction)0 || MiddleButton >= BUTTFUNC_LAST)
//	   MiddleButton = BUTTFUNC_ZOOMIN;
	if (RightButton < (ButtonFunction)0 || RightButton >= BUTTFUNC_LAST)
	   RightButton = BUTTFUNC_MENU;

	String_32 FunctionName;
	ButtonFunction bf = (ButtonFunction)0; // BUTTFUNC_NORMAL; 
	for (INT32 i = (INT32)bf ; i < (INT32)BUTTFUNC_LAST; i++)
	{
		bf = (ButtonFunction)i;
		ClickModifiers::GetButtonFuncName(bf, &FunctionName);
		pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_LEFTBUTTON), &FunctionName);
//		pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_MIDDLEBUTTON), &FunctionName);
		pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_RIGHTBUTTON), &FunctionName);
	}
	// Set the length of the list
	pPrefsDlg->SetComboListLength(_R(IDC_OPTS_LEFTBUTTON));
//	pPrefsDlg->SetComboListLength(_R(IDC_OPTS_MIDDLEBUTTON));
	pPrefsDlg->SetComboListLength(_R(IDC_OPTS_RIGHTBUTTON));

	// Set up the default option displayed
	ok = pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_LEFTBUTTON), (INT32)LeftButton);
//	ok = pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_MIDDLEBUTTON), (INT32)MiddleButton);
	ok = pPrefsDlg->SetSelectedValueIndex(_R(IDC_OPTS_RIGHTBUTTON), (INT32)RightButton);

	return TRUE;
}	

/******************************************************************************************

>	BOOL PointersTab::InitMagneticRadii()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the the magnetic radii fields in the Pointers section
				of the options dialog box.
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL PointersTab::InitMagneticRadii()
{
TRACEUSER( "Neville", _T("InitMagneticRadii\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PointersTab::InitMagneticRadii() called with no dialog pointer");
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

	// General flags
	BOOL ok	= TRUE;			 
	BOOL ReadOk = TRUE;

	// Now the magnetic radii
	// These should just be shown in distance measurements rather than scaled distance
	// measurements as they define distance on screen rather than on the page. 
	INT32 LineRadius = 0;					
	INT32 PointRadius = 0;					
	ReadOk = Camelot.GetPrefValue(TEXT("Magnetic Options"), TEXT("Line Radius"), &LineRadius);
	ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_LINERADIUS), CurrentPageUnits, LineRadius, FALSE, -1);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_EDIT));

	ReadOk = Camelot.GetPrefValue(TEXT("Magnetic Options"), TEXT("Point Radius"), &PointRadius);
	ok = pPrefsDlg->SetUnitGadgetValue(_R(IDC_OPTS_POINTRADIUS), CurrentPageUnits, PointRadius, FALSE, -1);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_EDIT));

	return TRUE;
}	

#endif //webster
