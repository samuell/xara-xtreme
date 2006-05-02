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
// Implementation of the Undo tab of the options dialog box

/*

*/

#include "camtypes.h"
#include "appprefs.h"
#include "optsundo.h"
#include "app.h"		// Camelot object
#include "convert.h"	// StringToBytes and BytesToString
#include "ophist.h"

CC_IMPLEMENT_DYNAMIC(UndoTab, OptionsTabs)   
			  
// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

/********************************************************************************************

>	UndoTab::UndoTab() 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	UndoTab constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UndoTab::UndoTab()
{   
	// Do nothing at present
}        

/********************************************************************************************

>	UndoTab::~UndoTab()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	UndoTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
UndoTab::~UndoTab()
{   
}        

/********************************************************************************************

>	BOOL UndoTab::Init()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	UndoTab initialisation routine
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
BOOL UndoTab::Init()
{   
	return TRUE;
}        


/******************************************************************************************

>	CDlgResID UndoTab::GetPageID()()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CDlgResID UndoTab::GetPageID()
{
	return _R(IDD_OPTSTAB_UNDO);
}

/******************************************************************************************

>	BOOL UndoTab::IsDocumentOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a document option.	  
	Purpose:	Allows the document options dialog code to determine if this tab belongs
				to the group of document options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL UndoTab::IsDocumentOption()
{
	return TRUE;	// This tab is not a document option 
}

/******************************************************************************************

>	BOOL UndoTab::IsProgramOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a program option.	  
	Purpose:	Allows the program options dialog code to determine if this tab belongs
				to the group of program options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL UndoTab::IsProgramOption()
{
	return TRUE;	// This tab is a program option 
}


/******************************************************************************************

>	BOOL UndoTab::CommitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Undo section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL UndoTab::CommitSection()
{
TRACEUSER( "Neville", _T("commit undo section\n"));
	ERROR3IF(pPrefsDlg == NULL, "UndoTab::CommitSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_UNDO));	// The Undo View identifier
	if (!ok)
		return TRUE;	// Talk to View failed to return now

	// Ok has been pressed so take the values from this section of the dialog box
	BOOL Valid=TRUE;		// Flag for validity of value
//	BOOL State=FALSE;		// Flag for state of button/switch
//	BOOL SetOk=TRUE;		// Preference value set ok

	// Section = Undo settings

	// Get the currently selected document
	Document* pDocument = Document::GetSelected();
	if (pDocument != NULL)
	{
		// Getthe current undo size
		UINT32 CurrentSize = pDocument->GetOpHistory().GetSize();
		UINT32 CurrentMaxSize = pDocument->GetOpHistory().GetMaxSize();

		// Now read the state of the use maximum memory switch, if on then set TempWorkMemory to 0
		BOOL Unlimited = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_UNLIMITEDUNDO), 0, 1, 0, &Valid);

		UINT32 NewMaxUndoSize;
		if (Unlimited)
		{
			// Use the maximum possible value
			NewMaxUndoSize = UINT32_MAX;
		}
		else
		{
			// Read the value from the editable field limiting the values in the process
			NewMaxUndoSize = pPrefsDlg->GetMemoryGadgetValue(_R(IDC_OPTS_MAXUNDOSIZE),
															 1*1024, UINT32_MAX-1,
													 		 _R(IDE_OPTS_INVALIDUNDOSIZE), &Valid);
			if (!Valid)
				return FALSE; 			// Undo size incorrect, user has been warned already
		}
TRACEUSER( "Neville", _T("Current undo size =%d\n"),CurrentSize);
TRACEUSER( "Neville", _T("Current maximum undo size =%d\n"),CurrentMaxSize);
TRACEUSER( "Neville", _T("New maximum undo size =%d\n"),NewMaxUndoSize);
		
		// If a new size has been entered then try to set this as the new maximum
		if (NewMaxUndoSize != CurrentMaxSize)
		{
#if 0 //def _DEBUG
			// If the resize means that data will be lost then warn the user and ask for confirmation
			// otherwise just set the new limit.
			BOOL OkToResize = TRUE;
			if (NewMaxUndoSize < CurrentSize)
			{
				// Ask user to confirm loss of undo data
				// Warn the user that there is no selection = large export
				ErrorInfo Info;
				Info.ErrorMsg = _R(IDT_OPTS_LOSSOFUNDO);
				Info.Button[0] = _R(IDB_UNDOQUERY_RESIZE);
				Info.Button[1] = _R(IDB_UNDOQUERY_DONTRESIZE);
				if (AskQuestion(&Info) == _R(IDB_UNDOQUERY_DONTRESIZE))
				{
					// If the user has cancelled just display the current value again
					OkToResize = FALSE;
				}
			}
		
			// If everything is ok then try the resize
			if (OkToResize)
			{
				SetOk = pDocument->GetOpHistory().SetNewMaxSize(NewMaxUndoSize);
				if (!SetOk)
				{
					// Out of ID's at present so use wrong error message 
					InformError(_R(IDE_OPTS_BADUNDORESIZE));
				
					// Force the update to the old max size value
					OkToResize = FALSE;
				}
			}

			// If for any reason OkToResize has been set ot FALSE then update the undo size
			// field and exit FALSE
			if (!OkToResize)
			{
				// Restore the MaxSize in dialog field
			  	//String_256	String;
				// Get current size
				UINT32 MaxUndoSize = pDocument->GetOpHistory().GetMaxSize();
				// Show that value on the dialog box as a memory size
				pPrefsDlg->SetMemoryGadgetValue(_R(IDC_OPTS_MAXUNDOSIZE), MaxUndoSize);

				return FALSE;
			}
#else
			InformWarning(_R(IDW_NOUNDORESIZING));
#endif
		}
	}
	//else
	//	ERROR2(FALSE,_R(IDE_OPTS_SETPREF_UNDO));

	return TRUE;
}

/********************************************************************************************

>	BOOL UndoTab::GreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents 
	Errors:		-
	SeeAlso:	UndoTab::UngreySection;

********************************************************************************************/

BOOL UndoTab::GreySection()
{
TRACEUSER( "Neville", _T("GreySection in UndoTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UndoTab::GreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_UNDO));	// The UndoTab identifier
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

>	BOOL UndoTab::UngreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document 
	Errors:		-
	SeeAlso:	UndoTab::UngreySection;

********************************************************************************************/

BOOL UndoTab::UngreySection()
{
TRACEUSER( "Neville", _T("UngreySection in UndoTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UndoTab::UngreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_UNDO));	// The UndoTab identifier
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

>	BOOL UndoTab::ChangeControlStatus(const BOOL Status)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 
	Errors:		-
	SeeAlso:	UndoTab::GreySection;	UndoTab::UngreySection;

********************************************************************************************/

BOOL UndoTab::ChangeControlStatus(const BOOL Status)
{
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_UNDOGROUP), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_UNDOSIZETXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_UNDOSIZE), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_MAXUNDOSIZE), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_UNLIMITEDUNDO), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_MAXUNDOTXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_MAXUNDOSIZE), Status);

	return TRUE;
}			

/********************************************************************************************

>	BOOL UndoTab::UpdateSection(String_256 *DocumentName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		Name of document
	Outputs:	-
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. Should only init the tab if the page is present.  
				The document name allows the info field on the tab to be filled in correctly.
	Errors:		-
	SeeAlso:	UndoTab::GreySection; UndoTab::UngreySection; UndoTab::InitSection;

********************************************************************************************/

BOOL UndoTab::UpdateSection(String_256 *DocumentName)
{
TRACEUSER( "Neville", _T("UndoTab::UpdateSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UndoTab::UpdateSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_UNDO));	// The UndoTab identifier
	if (!ok)
		return TRUE;		// page not present

	// Remove any lists that we have created.
	//pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_SIZELIST));

	// Call the normal init section code
	return InitSection();
}

/********************************************************************************************

>	BOOL UndoTab::HandleMsg(DialogMsg* Msg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the Undo options tabs messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL UndoTab::HandleMsg(DialogMsg* Msg)
{
TRACEUSER( "Neville", _T("HandleUndoMsg\n"));
	ERROR2IF(Msg == NULL,FALSE,"UndoTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UndoTab::HandleMsg called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_UNDO));	// The Undo View identifier
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
			if (Msg->GadgetID == _R(IDC_OPTS_UNLIMITEDUNDO))
			{
				BOOL Valid;
				BOOL Unlimited = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_UNLIMITEDUNDO), 0, 1, 0, &Valid);
				// If set then grey the editable field and its name text otherwise ungrey
				pPrefsDlg->EnableGadget(_R(IDC_OPTS_MAXUNDOTXT), !Unlimited);
				pPrefsDlg->EnableGadget(_R(IDC_OPTS_MAXUNDOSIZE), !Unlimited);
			}
#if 0
			else if (Msg->GadgetID == _R(IDC_OPTS_DEFAULTUNDO))
			{
				// Set the MaxSize in the dialog field to a default value
				String_256	String;
				UINT32 MaxUndoSize = UINT32_MAX; //40*1024*1024;	// 40 Mbytes
				// Put that size in a string formatted as a memory size	value
				BytesToString(&String, MaxUndoSize);
				// Show that on the dialog box
				pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_MAXUNDOSIZE), &String);
			}
#endif
			break;
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

>	BOOL UndoTab::InitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the Undo section of the options dialog box.
				This section includes the:-
				- Unlimited undo memory size switch
				- OR maximum memory allowed for undo system 
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL UndoTab::InitSection()
{
TRACEUSER( "Neville", _T("InitUndoSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"UndoTab::InitSection called with no dialog pointer");

//	BOOL ReadOk = FALSE; 	// Flag to say whether the preference value was read ok 

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256 *DocumentName = GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_DOCUMENT), *DocumentName);


	// Section = Undo settings

	// Set up the display for the current size of the undo buffer.
	Document* pDocument = Document::GetSelected();
	if (pDocument != NULL)
	{
		// Get the current undo size
		UINT32 UndoSize = pDocument->GetOpHistory().GetSize();
		
		// Put that size in a string formatted as a memory size	value
		// and show that on the dialog box
		pPrefsDlg->SetMemoryGadgetValue(_R(IDC_OPTS_UNDOSIZE), UndoSize);

		// Now get the currently set maximum size
		UINT32 MaxUndoSize = pDocument->GetOpHistory().GetMaxSize();

		// If set at the maximum size possible then assume unlimited
		BOOL Unlimited = FALSE;
		if (MaxUndoSize == UINT32_MAX)
			Unlimited = TRUE;
		else
			Unlimited = FALSE;

		// Put that size in a string formatted as a memory size	value
		// and show that on the dialog box
		pPrefsDlg->SetMemoryGadgetValue(_R(IDC_OPTS_MAXUNDOSIZE), MaxUndoSize);

		// Set the unlimited switch state
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_UNLIMITEDUNDO), Unlimited);
		
		// Grey or ungrey the editable field and its name text accordingly
		pPrefsDlg->EnableGadget(_R(IDC_OPTS_MAXUNDOTXT), !Unlimited);
		pPrefsDlg->EnableGadget(_R(IDC_OPTS_MAXUNDOSIZE), !Unlimited);

TRACEUSER( "Neville", _T("undo size ='%d'\n"),UndoSize);
TRACEUSER( "Neville", _T("max undo size ='%d'\n"),MaxUndoSize);
	}
	else
	{
		// Ensure our section is greyed
		GreySection();
	}

	return TRUE;
}
