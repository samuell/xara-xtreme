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
// PrintPrefs.cpp
//
// This file implements the dialogue box that allows the user to set the
// preferences for the printer. Brought up from the print dialog box.
// Uses the same system as the application preferences

/*
*/

#include "camtypes.h"
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
#include "app.h"		// Camelot object
#include "dialogop.h"	// DialogOp header
#include "appprefs.h"
#include "prnprefs.h"
#include "docview.h"	
#include "document.h"	
//#include "optsres.h"	// error strings
//#include "prefsdlg.h"	// _R(IDD_DOCUMENTOPTIONS)
#include "prnmks.h"
#include "optsprin.h"
#include "printmsg.h"
//#include "printdlg.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together,
// here at the start of the file
CC_IMPLEMENT_DYNCREATE(PrintPrefsDlg, DialogTabOp)
CC_IMPLEMENT_DYNCREATE(PrintTabPrintPrefsDlg, PrintPrefsDlg)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

// Set up explicit details about this dialog box such as the id and type
const CDlgResID PrintPrefsDlg::IDD 			= _R(IDD_DOCUMENTOPTIONS); 	// Dialog box id
const CDlgMode PrintPrefsDlg::Mode 			= MODAL;  	//make it modal for now 
//const CDlgMode PrintPrefsDlg::Mode = MODELESS;		// This dialog is modeless

BOOL			PrintPrefsDlg::OkUsed		= FALSE;	// remember whether ok or cancel used		
BOOL			PrintPrefsDlg::Open			= FALSE;	// Are we open?
PrintPrefsDlg*	PrintPrefsDlg::pCurrentDlg	= NULL;		// ptr to current op desc

/******************************************************************************************

>	static BOOL PrintPrefsDlg::InvokeDialog() 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/94
	Inputs:		-
	Outputs:	-
	Returns:	True if Ok used or False if Cancel used.
	Purpose:	How to invoke or start a PrintPrefsDlg box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PrintPrefsDlg::InvokeDialog()
{
	static BOOL FirstTime = TRUE;

	if (!Open)
	{
		Open = TRUE; 

		// First time through we want to force open the second Print tab as opposed to the Output
		// tab as this is deemed to be the better option.
		OpDescriptor *pOpDesc = NULL;
		if (!FirstTime)
		{
			pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(PrintPrefsDlg));
		}
		else
		{
			// Only do this the first time
			FirstTime = FALSE;
			pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_PRINTOPTIONSDLG);
		}
		ERROR3IF(pOpDesc == NULL,"PrintPrefsDlg::InvokeDialog is unable to find the PrintPrefsDlg OpDescriptor");

		if (pOpDesc != NULL)
			pOpDesc->Invoke();

		Open = FALSE;

		return OkUsed;
	}
	else
	{
		ERROR3("PrintPrefsDlg::InvokeDialog() called again, before previous call returned");
		return FALSE;
	}
}


/******************************************************************************************

>	PrintPrefsDlg::PrintPrefsDlg(): DialogTabOp(PrintPrefsDlg::IDD, PrintPrefsDlg::Mode) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintPrefsDlg constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

PrintPrefsDlg::PrintPrefsDlg(): DialogTabOp(PrintPrefsDlg::IDD, PrintPrefsDlg::Mode) 
{
	// At present, just:- 
	OptionsTabs::SetApplyNowState(FALSE);		// grey the apply now button
	OptionsTabs::SetInitMessageState(FALSE);	// not sending an init message
	OptionsTabs::pScopeDocument = NULL;			// Document in use when sending messages

	// Set default units
	OptionsTabs::SetDefaultUnits();				// set up our default display units

	// Document and spread we are supposed to be working on. Of course none at present.
	OptionsTabs::SetCurrentDocAndSpread(NULL);

	PrintPrefsDlg::SetCurrentDlg(this);
}

/******************************************************************************************

>	PrintPrefsDlg::PrintPrefsDlg(INT32 StartingPage): DialogTabOp(PrintPrefsDlg::IDD, PrintPrefsDlg::Mode) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintPrefsDlg constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

PrintPrefsDlg::PrintPrefsDlg(INT32 StartingPage): DialogTabOp(PrintPrefsDlg::IDD, PrintPrefsDlg::Mode,
														CC_RUNTIME_CLASS(DialogOp), StartingPage) 
{
	// At present, just:- 
	OptionsTabs::SetApplyNowState(FALSE);		// grey the apply now button
	OptionsTabs::SetInitMessageState(FALSE);	// not sending an init message
	OptionsTabs::pScopeDocument = NULL;			// Document in use when sending messages

	// Set default units
	OptionsTabs::SetDefaultUnits();				// set up our default display units

	// Document and spread we are supposed to be working on. Of course none at present.
	OptionsTabs::SetCurrentDocAndSpread(NULL);

	PrintPrefsDlg::SetCurrentDlg(this);
}


/******************************************************************************************

>	PrintPrefsDlg::~PrintPrefsDlg() 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintPrefsDlg destructor. 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

PrintPrefsDlg::~PrintPrefsDlg()
{
	// Make sure that we remove our options tabs link to the dialog box class
	// as the dialog will now be destroyed
	OptionsTabs::pPrefsDlg = NULL;

	PrintPrefsDlg::SetCurrentDlg(NULL);
}


/******************************************************************************************

>	BOOL PrintPrefsDlg::CommitDialogValues()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the dialog box and sets the return values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PrintPrefsDlg::CommitDialogValues()
{
	// Ok has been pressed so take the values from the dialog box
	BOOL ok = TRUE;
	BOOL ValuesOk = TRUE;

	// Go through all the tabs declared to the system in turn
	// and tell them to commit the values for their tab
	OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();

	while (pOptionsTabs != NULL)
	{
		// Only get the tab to commit if it is a print based option
		if ( pOptionsTabs->IsPrintingOption() )
		{
			// Get the found page to commit its values.
			// Should return False if reported error and we should just fail.
			// 				 2 if set an error which we should report and then fail. 
			ok = pOptionsTabs->CommitSection();
			if ( ok != TRUE )
			{
				if (ok == 2)
					InformError();			// If error required show it.

				// Carry on with other pages so that report all errors and have a chance
				// to correct mistakes in other pages but make note of error state.
				ValuesOk = FALSE;	
				//return FALSE;
			}
		}
		// Get the next option's tab.
		pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
	}

	// Return whether we found bad values or not.
	return ValuesOk;
}

/******************************************************************************************

>	MsgResult PrintPrefsDlg::Message(CDlgMessage DlgMsg, CGadgetID Gadget) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		-
	Outputs:	-
	Returns:	OK	 		  Message handled ok (return this even if you don't need to 
							  respond to the message). 

				FAIL 		  Something terrible happened whilst processing the message
				 			  eg. we ran out of memory. You must set ERROR if you
				 			  are returning this value. 

				EAT_MSG       The Message was handled ok but don't send it to any
				 			  more MessageHandlers. 
	Purpose:	Handles all the application preferences dialog's messages
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

MsgResult PrintPrefsDlg::Message(Msg* Message)
{
TRACEUSER( "Neville", _T("PrintPrefsDlg::Message\n"));
	ERROR3IF(Message == NULL,"PrintPrefsDlg::Message null message received");

	// Jason - This is a message-loop lock which is used in the DIM_CREATE code to 
	// allow the print marks manager cache print marks - this causes a lot of diocument
	// message broadcats and suchlike, which cause re-entrant messages to come in
	// here and trigger 3 billion ERROR3s. While caching the print marks, we lock this
	// message handler so that it just ignores all resultant broadcasts.
	static BOOL IgnoreAllMessages = FALSE;
	if (IgnoreAllMessages)
		return DialogTabOp::Message(Message);


	// Cope with messages for the dialog box
	if (MESSAGE_IS_A(Message,PrintMsg))		// Check for print messages
	{
		PrintMsg* pPrintMsg = (PrintMsg*)Message;

		if (pPrintMsg->Reason == PrintMsg::SETTINGSCHANGED)
		{
			// Tell the print layout tab about the print settings change
			OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();
			while (pOptionsTabs != NULL)
			{
				if (pOptionsTabs->GetPageID() == _R(IDD_OPTSTAB_PRINTLAYOUT) ||
					pOptionsTabs->GetPageID() == _R(IDD_OPTSTAB_PRINTGENERAL))
				{
					PrintBaseTab* pBaseTab = (PrintBaseTab *) pOptionsTabs;
					pBaseTab->PrintSettingsChanged();
				}

				// Get the next option's tab.
				pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
			}
		}

		return DialogTabOp::Message(Message);
	}

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		MsgResult Result;

		// Main handler for dialog type messages
		BOOL EndDialog = FALSE;			// TRUE if we should quit the dialog
		BOOL CommitValues = FALSE;		// TRUE if we should commit the dialog values
		BOOL OldApplyNow = OptionsTabs::GetApplyNowState();	// Old ApplyNow button state  

		// Make sure that we have set up our class variables so that the tabs know what
		// is the current document and spread to work on.
		Document *pDocument = Document::GetSelected();
		OptionsTabs::SetCurrentDocAndSpread(pDocument);
		// Plus the current document units are correct
		OptionsTabs::SetUpDocUnits();

		// Check if sending init/create message and if so set flag True.
		// If True then do not ungrey the ApplyNow button on changing messages
		if (Msg->DlgMsg == DIM_CREATE)
		{
TRACEUSER( "Neville", _T("PrintPrefsDlg::Message DIM_CREATE\n"));
			OptionsTabs::SetInitMessageState(TRUE);				
		}

		// If the message is not for the main options window then try the tab pages 
		BOOL FoundPage = FALSE;
TRACEUSER( "Neville", _T("PrintPrefsDlg::Message PageID = %d\n"),Msg->PageID);
		if (Msg->PageID != 0)
		{
			// Now go through all the tabs declared to the system
			// And see if the message is for that page or not.
			OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();

			while (pOptionsTabs != NULL && !FoundPage)
			{
				CDlgResID PageId = pOptionsTabs->GetPageID();
				if (PageId == Msg->PageID)
				{
					// Found the correct page so stop the search and send the message
					// to that page
					pOptionsTabs->HandleMsg(Msg);
					FoundPage = TRUE;
				}
				// Get the next option's tab.
				pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
			}
		}
		else
		{
			// Main page window
			switch (Msg->DlgMsg)
			{
				case DIM_CREATE:		// Creating the main dialog box
				{
					// grey out the apply now button as this should only become enabled	when 
					// we have made a change to the dialog box.
					OptionsTabs::SetInitMessageState(FALSE);

					IgnoreAllMessages = TRUE;
					// BLOCK
					{
						// Make sure the print marks are cached by the PMManager. This must be done now
						// because we want to demand-load the marks, but if we leave it any later we get
						// an awful lot of rather nasty problems with document messages flying around.
						PrintMarksMan* pMarksMan = GetApplication()->GetMarksManager();
						if (pMarksMan)
						{
							pMarksMan->ConstructCache();
							if (Document::GetSelected() != NULL)
								pMarksMan->AddDefaultMarksToDoc(Document::GetSelected());
						}
					}
					IgnoreAllMessages = FALSE;

					OptionsTabs::SetApplyNowState(FALSE);
					OptionsTabs::GreyApplyNow();
					// remove the apply now button as not required.
					CDlgResID PageID = GetCurrentPageID();	// Get currently selected Tab id
					TalkToPage(NULL);						// Select the main tab
					HideGadget(_R(ID_APPLY_NOW), TRUE);			// hide button on main tab
					TalkToPage(PageID);						// Select the originally selected tab

					// Pass the message up to all the tabs in case it's of use to them
					OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();
					while (pOptionsTabs != NULL)
					{
						if (pOptionsTabs->IsKindOf(CC_RUNTIME_CLASS(PrintSepsTab)) || // only these classes need this message
							pOptionsTabs->IsKindOf(CC_RUNTIME_CLASS(PrintImagesetterTab)))
							pOptionsTabs->HandleMsg(Msg);
						pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
					}
					break;
				}


				case DIM_COMMIT:		// Want to commit and quit
					{
						OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();
						while (pOptionsTabs != NULL)
						{
							if (pOptionsTabs->IsKindOf(CC_RUNTIME_CLASS(PrintSepsTab)) || // only these classes need this message
								pOptionsTabs->IsKindOf(CC_RUNTIME_CLASS(PrintImagesetterTab)))
								pOptionsTabs->HandleMsg(Msg);

							// Get the next option's tab.
							pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
						}
						OkUsed = TRUE;		// Flag ok used
						EndDialog = TRUE;
						CommitValues = TRUE;
					}
					break;

				case DIM_SOFT_COMMIT:	// Want to commit but leave the dialogue open
					{
						// Pass the message up to all the tabs in case it's of use to them
						OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();
						while (pOptionsTabs != NULL)
						{
							if (pOptionsTabs->IsKindOf(CC_RUNTIME_CLASS(PrintSepsTab)) || // only these classes need this message
								pOptionsTabs->IsKindOf(CC_RUNTIME_CLASS(PrintImagesetterTab)))
								pOptionsTabs->HandleMsg(Msg);
							pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
						}

						OkUsed = TRUE;		// Flag ok used
						CommitValues = TRUE;
					}
					break;

				case DIM_CANCEL:		// Want to quit
					{
						// Pass the message up to all the tabs in case it's of use to them
						OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();
						while (pOptionsTabs != NULL)
						{
							if (pOptionsTabs->IsKindOf(CC_RUNTIME_CLASS(PrintSepsTab)) || // only these classes need this message
								pOptionsTabs->IsKindOf(CC_RUNTIME_CLASS(PrintImagesetterTab)))
								pOptionsTabs->HandleMsg(Msg);
							pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
						}
					}
					OkUsed = FALSE;		// Flag ok used
					EndDialog = TRUE;
					break;
			}
		}

		// Commit values here
		if (CommitValues)
		{
			BOOL ok = CommitDialogValues();
			// Only unset EndDialog as in SOFT_COMMIT case we do not want to end the
			// Operation and hence close the dialog box
			if (!ok)
			{
				EndDialog = FALSE;	// Values not correct so do not allow exit
			}
		}

		if ( !OldApplyNow && OptionsTabs::GetApplyNowState()
			 && !EndDialog && !OptionsTabs::GetInitMessageState() )
		{
			// Check for a change from apply now being False to it becoming
			// True as an item has been changed.
			OptionsTabs::UngreyApplyNow();
		}

		if ( OptionsTabs::GetApplyNowState() && (Msg->DlgMsg == DIM_SOFT_COMMIT)
			 && !EndDialog && !OptionsTabs::GetInitMessageState() )
		{
			// Check for an apply now (DIM_SOFT_COMMIT) being used and the
			// apply being True, should always be the case,	so that we can reset
			// the grey state to False as we are in an unchanged state.
			OptionsTabs::SetApplyNowState(FALSE);
			OptionsTabs::GreyApplyNow();
		}

		// Allow the base class access to the message, it will do the
		// DLG_EAT_IF_HUNGRY(Msg) for us
		// Must do this before the Close and End
		Result = DialogTabOp::Message(Message);

		// End dialog here
		if (EndDialog) 
		{
			// Grey all the tabs to remove the Indexed colour in use error
			OptionsTabs::SetCurrentDocAndSpread(NULL);
			OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();
			while (pOptionsTabs != NULL)
			{
				if (pOptionsTabs->IsPrintingOption())
					pOptionsTabs->GreySection();
				pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
			}
			
			Close();				// Hide the dialog box
			End();					// Finish the operation
			
			// Make sure that we remove our options tabs link to the dialog box class
			// as the dialog will now be destroyed
			OptionsTabs::pPrefsDlg = NULL;
		}

		// Check if have been sending an init/create message and if so then set flag False.
		// Only do this in the init/create case as we might be sent one of these and then
		// be sent other messages whilst in the middle of the init/create message such as
		// text changing messages.
		if (Msg->DlgMsg == DIM_CREATE)
			OptionsTabs::SetInitMessageState(FALSE);				

		// The message was for our dialog box so return that we have handled it, if necessary
		//return (DLG_EAT_IF_HUNGRY(Msg)); 
		return Result;
	}

	// If nobody above has processed the message, then pass it around all the tabs in
	// case it's of use to them.
	OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();

	while (pOptionsTabs != NULL)
	{
		pOptionsTabs->Message(Message);
		pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
	}

	return DialogTabOp::Message(Message);
}

/******************************************************************************************

>	OpState	PrintPrefsDlg::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintPrefsDlg GetState method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

OpState	PrintPrefsDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;

	// If the pointer is non-null then it implies that another dialog box is up which
	// uses our tab set and so we must force greying of the menu option 
	// This one is different in that we force the main options dialog box to be closed.
	if (OptionsTabs::pPrefsDlg != NULL)
		OpSt.Greyed=TRUE;

	return(OpSt);
}




/******************************************************************************************

>	BOOL PrintPrefsDlg::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintPrefsDlg Init method. Called when the kernel initialises.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PrintPrefsDlg::Init()
{
	BOOL InitOK;

	// Now register ourselves with the dialog system
	InitOK = RegisterOpDescriptor(
								0,								// Tool ID 
								_R(IDS_PRINTOPTIONS),				// String resource ID
								CC_RUNTIME_CLASS(PrintPrefsDlg),	// Runtime class
								OPTOKEN_PRINTOPTIONS, 			// Token string
								GetState,						// GetState function
								0,								// help ID
								0,  							// bubble help
								0,								// resource ID
								0,								// control ID
								SYSTEMBAR_ILLEGAL,				// Bar ID
								FALSE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								0,								// No vertical counterpart
								_R(IDS_PRINTPREFSDLG_ONE),			// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags								);
								);

	// Now register ourselves with the dialog system for the units button on the windows
	// button bar as well
	InitOK = InitOK && RegisterOpDescriptor(
								0,								// Tool ID
								_R(IDS_PRINTOPTIONS),				// String resource ID
								CC_RUNTIME_CLASS(PrintTabPrintPrefsDlg),	// Runtime class
								OPTOKEN_PRINTOPTIONSDLG,		// Token string
								GetState,						// GetState function
								0,								// help ID
								0,  							// bubble help
								0,								// resource ID
								0,								// control ID
								SYSTEMBAR_ILLEGAL,				// Bar ID
								FALSE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								0,								// No vertical counterpart
								_R(IDS_PRINTPREFSDLG_ONE),			// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags								);
								);

	// Options tabs and page sizes initialised by the applications options (AppPrefsDlg)
	// class.

	return (InitOK);
}


/******************************************************************************************

>	void PrintPrefsDlg::Do(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/
void PrintPrefsDlg::Do(OpDescriptor*)
{
	BOOL ok;

	if (OptionsTabs::pPrefsDlg != NULL)
	{
		ERROR3("PrintPrefsDlg::Do an options tabbed dialog box is already present");
		return;
	}
	
	// Set up the name that the main dialog page is going to use
	String_256 Temp(_R(IDN_PRINTOPTS)); 
	TitleString = Temp;
	SetName(&TitleString);

	// Make sure that our options tabs have a link to the dialog box class
	OptionsTabs::pPrefsDlg = this;
	OptionsTabs::SetApplyNowState(FALSE);

	// Make sure that we have set up our class variables so that the tabs know what
	// is the current document and spread to work on.
	Document *pDocument = Document::GetSelected();
	OptionsTabs::SetCurrentDocAndSpread(pDocument);
	// Work out the document name so that it is ready for options tabs to display.
	OptionsTabs::SetDocumentName(pDocument);
	
		
	// Force the dialog box to be created, as it is non-modal it must be opened if the create
	// works ok.
	ok = Create();							// create the dialog box

	if ( ok )
	{
//		// Modal at present so remove the Open()
//		Open();								// created ok so open the dialog box up
//		
//		// grey out the apply now button as this should only become enabled when we have 
//		// made a change to the dialog box.
//		OptionsTabs::SetInitMessageState(FALSE);
//		OptionsTabs::SetApplyNowState(FALSE);
//		OptionsTabs::GreyApplyNow();
	}
	else
	{
		// Could not create the dialog box so call inform error 
		InformError();						// Show user the error
  		End(); 		   						// End the operation 
	}				   	
}

/******************************************************************************************

>	BOOL PrintPrefsDlg::RegisterYourPagesInOrderPlease()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		-
	Outputs:	-
	Returns:	return TRUE if all calls to AddAPage returned TRUE, else FALSE
	Purpose:	This virtual function will get called to give you a chance to add pages to your
				DialogTabOp. This function will usually make multiple calls to AddAPage to 
				register the initial set of pages to be contained in the tab dialog. The pages
				should all be the same size and be registered in the left to right order in which
				you wish them to appear. 

				This function get's called directly from the Create method, Don't call it 
				explicitly yourself. If you return FALSE from this method then the Create 
				method will fail. 

	Errors:		-
	SeeAlso:	DialogTabOp::AddAPage; DialogOp::Create

******************************************************************************************/

BOOL PrintPrefsDlg::RegisterYourPagesInOrderPlease()
{
	BOOL ok = TRUE;

	// Now go through all the tabs declared to the system
	OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();

	while (pOptionsTabs != NULL)
	{
		// Only add the tab if it is a document based option
		if ( pOptionsTabs->IsPrintingOption() )
		{
			CDlgResID PageId = pOptionsTabs->GetPageID();
			ok = ok && AddAPage(PageId);
		}

		// Get the next option's tab.
		pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
	}

	return (ok); 
}

/******************************************************************************************

>	static void PrintPrefsDlg::SetCurrentDlg(PrintPrefsDlg* pDlg)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/96
	Inputs:		pDlg = ptr to a PrintPrefsDlg
	Outputs:	-
	Returns:	-
	Purpose:	This sets the ptr to the currently created PrintPrefsDlg.
				There should only be one at a time, so if you set this value
				to a non-NULL value, and there's already one set, an ERROR3 will go off

				NOTE: This should only be called from the constructors & destructors of
				the PrintPrefsDlg class

	Errors:		-
	SeeAlso:	PrintPrefsDlg::CloseCurrentDlg()

******************************************************************************************/

void PrintPrefsDlg::SetCurrentDlg(PrintPrefsDlg* pDlg)
{
	ERROR3IF(pDlg != NULL && pCurrentDlg != NULL,"Setting current Dlg, but there's already one there");
	ERROR3IF(pDlg == NULL && pCurrentDlg == NULL,"Setting current Dlg to NULL, but there's no current dlg");

	pCurrentDlg = pDlg;
}

/******************************************************************************************

>	static void PrintPrefsDlg::CloseCurrentDlg()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This will close the currently open print prefs dlg, if there is one.
				If there isn't one open, nothing happens
	Errors:		-
	SeeAlso:	PrintPrefsDlg::SetCurrentDlg()

******************************************************************************************/

void PrintPrefsDlg::CloseCurrentDlg()
{
	if (pCurrentDlg != NULL)
	{
		DialogMsg Msg(pCurrentDlg->WindowID, DIM_CANCEL, 0);
		pCurrentDlg->Message(&Msg);
	}
}

/******************************************************************************************

>	PrintTabPrintPrefsDlg::PrintTabPrintPrefsDlg(): PrintPrefsDlg(1) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintTabPrintPrefsDlg constructor. Creates a non-undoable operation.
				Just starts up the PrintPrefsDlg specifying the tab number that it is in the
				list. This has been calculated when the options tabs were declared just to
				be on the safe side.
				Must use a number rather than the ID as this is what MFC's property sheet
				constructors require. 
	Errors:		-
	SeeAlso:	PrintPrefsDlg; GridTabAppPrefsDlg;

******************************************************************************************/

PrintTabPrintPrefsDlg::PrintTabPrintPrefsDlg(): PrintPrefsDlg( OptionsTabs::PrintTabNumber ) 
{
	// At present, just:- do nothing
}

#endif //webster
