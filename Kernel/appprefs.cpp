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
// AppPrefs.cpp
//
// This file implements the dialogue box that allows the user to set the
// preferences for the application. Brought up from the tools options menu item.

/*
*/

#include "camtypes.h"
//#include "app.h"		// Camelot object - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h"	// _R(IDD_BARCONTROLSTORE), _R(IDC_BTN_UNITSETUPDLG) ...
//#include "barsdlgs.h"	// _R(IDD_BARCONTROLSTORE), _R(IDC_BTN_UNITSETUPDLG) ...
//#include "simon.h"		// _R(IDS_UNIT_DLG_OPEN) ...
//#include "mario.h"		// _R(IDS_FILEOPTIONS) ...
//#include "dialogop.h"	// DialogOp header - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h"		// _R(IDS_UTILOPTIONS) ...
//#include "markn.h"		// unit dialog box strings (_R(IDS_UNITDLG))
//#include "optsres.h"	// error strings
//#include "units.h"		// units - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "printmsg.h"
//#include "printdlg.h"
#include "camelot.h"
#include "bubbleid.h"

#include "appprefs.h"
//#include "prefsdlg.h"	// _R(IDD_APPLICATIONOPTIONS)
//#include "prnprefs.h"	// PrintingPrefs dialog box
#include "optsinet.h"
// The options tabs themselves
//#include "optsedit.h"	// Edit tab handler
#include "optsmisc.h"	// Misc tab handler
#include "optspage.h"	// Page tab handler
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
#include "optspntr.h"	// Pointers tab handler
#include "optsscal.h"	// Scaling tab handler
#include "optstune.h"	// Tune up tab handler
//#include "optsundo.h"	// Undo tab handler
#endif //webster
#include "optsunit.h"	// Units tab handler
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
#include "optsview.h"	// View tab handler
#endif //webster
#include "pagesize.h"	// PageSizes available e.g. A4
#include "optsmsgs.h"	// options changing messages class
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
#include "optsprin.h"	// Print tabs handler
#endif //webster
#include "optsgrid.h"	// Grid and Rulers tabs handler
#include "optsplug.h"	// Plug-ins tabs handler

//#include "will2.h"

//#include "printdlg.h"	// _R(IDD_OPTSTAB_PRINTLAYOUT)
//#include "prnmks.h"

#include "plugmngr.h"
//#include "plugres.h"



// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(AppPrefsDlg, DialogTabOp)
CC_IMPLEMENT_DYNCREATE(UnitsTabAppPrefsDlg, AppPrefsDlg)
CC_IMPLEMENT_DYNCREATE(ScaleTabAppPrefsDlg, AppPrefsDlg)
CC_IMPLEMENT_DYNCREATE(PageTabAppPrefsDlg, AppPrefsDlg)
CC_IMPLEMENT_DYNCREATE(GridTabAppPrefsDlg, AppPrefsDlg)
CC_IMPLEMENT_DYNCREATE(PlugsTabAppPrefsDlg, AppPrefsDlg)

CC_IMPLEMENT_DYNAMIC(OptionsTabs, ListItem)
CC_IMPLEMENT_DYNAMIC(OptionsChangingMsg, Msg)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

// Set up explicit details about this dialog box such as the id and type
const CDlgResID AppPrefsDlg::IDD 			= _R(IDD_APPLICATIONOPTIONS); // Dialog box id
//const CDlgMode AppPrefsDlg::Mode 			= MODAL;  	//make it modal for now 
const CDlgMode AppPrefsDlg::Mode 			= MODELESS;	// This dialog is modeless

PageSizesList *AppPrefsDlg::pPageSizesList 	= NULL; // List of available page sizes

static INT32 SaveOnExit 						= TRUE;	// flag for whether we save preferences on exit

DialogTabOp *OptionsTabs::pPrefsDlg 		= NULL;	// link to application options dialog class
List OptionsTabs::OptionsTabsList;					// The global list of installed options tabs.
UnitType OptionsTabs::CurrentPageUnits 		= NOTYPE;	// The units used to display page measurements in
UnitType OptionsTabs::CurrentScaledUnits 	= NOTYPE;	// The units used to display scaled measurements in
UnitType OptionsTabs::CurrentFontUnits 		= NOTYPE;	// The units used to display font measurements in
BOOL OptionsTabs::ApplyNow 					= FALSE;// True if Apply now should be ungreyed
BOOL OptionsTabs::InitMessage 				= FALSE;// True if sending init message

// The following are used to open the specific tab in the options dialog box
// These are the main application/document based options dialog box
UINT32 OptionsTabs::UnitsAppTabNumber 		= 0;	// Tab number assigned to the units tab
UINT32 OptionsTabs::ScaleAppTabNumber 		= 0;	// Tab number assigned to the scale tab
UINT32 OptionsTabs::PageAppTabNumber 			= 0;	// Tab number assigned to the page tab
UINT32 OptionsTabs::GridAppTabNumber 			= 0;	// Tab number assigned to the grid tab

// The plug-ins options dialog box
UINT32 OptionsTabs::PlugsAppTabNumber			= 0;	// Tab number assigned to the plug-ins tab

// These are the print options dialog box
UINT32 OptionsTabs::PrintTabNumber 			= 0;	// Tab number assigned to the print tab

Document	*OptionsTabs::pScopeDocument 	= NULL;	// Pointer to the current document
String_256 OptionsTabs::DocName 			= "-";	// default selected document name
Document	*OptionsTabs::pDocument 		= NULL;	// Document we are supposed to be working on
Spread		*OptionsTabs::pSpread 			= NULL; // Spread we are supposed to be working on

// There is some code which is removed as we no longer require print tabs in the main
// options tabs. This is absolutely essential functionality and so if the print tabs are put back
// then it ***MUST*** be replaced. All the code is flagged using the following define to make it
// easier to find and replace.
#define REMOVE_PRINT_TABS

/********************************************************************************************

	Preference:		SaveOnExit
	Section:		Preferences
	Range:			TRUE or FALSE
	Purpose:		If TRUE, then the preference file Camelot.INI will be saved when the 
					program exits. If FALSE, then no save will take place.
	SeeAlso:		-

********************************************************************************************/

/******************************************************************************************

>	BOOL AppPrefsDlg::IsSaveOnExitSet() 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the Save on exit flag as either TRUE or FALSE.
	Purpose:	Allows the state of the save preferences on exit flag to be read.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/
BOOL AppPrefsDlg::IsSaveOnExitSet()
{
	if ( SaveOnExit == 0 )
		return FALSE;
	else
		return TRUE;
}

/******************************************************************************************

>	BOOL AppPrefsDlg::SetSaveOnExit(BOOL NewState) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The old state of the save preferences on exit flag.
	Purpose:	This allows the state of the Save on exit flag to be set to either 
				TRUE or FALSE.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/
BOOL AppPrefsDlg::SetSaveOnExit(BOOL NewState)
{
	// First, remember old state
	BOOL OldSaveOnExit = SaveOnExit;

	if ( NewState != 0 )
		SaveOnExit = TRUE;
	else
		SaveOnExit = FALSE;

	return OldSaveOnExit;
}


/******************************************************************************************

>	AppPrefsDlg::AppPrefsDlg(): DialogTabOp(AppPrefsDlg::IDD, AppPrefsDlg::Mode) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	AppPrefsDlg constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

AppPrefsDlg::AppPrefsDlg(): DialogTabOp(AppPrefsDlg::IDD, AppPrefsDlg::Mode) 
{
	// At present, just:- 
	OptionsTabs::SetApplyNowState(FALSE);		// grey the apply now button
	OptionsTabs::SetInitMessageState(FALSE);	// not sending an init message
	OptionsTabs::pScopeDocument = NULL;			// Document in use when sending messages

	// Set default units
	OptionsTabs::SetDefaultUnits();				// set up our default display units

	// Document and spread we are supposed to be working on. Of course none at present.
	OptionsTabs::SetCurrentDocAndSpread(NULL);
}

/******************************************************************************************

>	AppPrefsDlg::AppPrefsDlg(INT32 StartingPage): DialogTabOp(AppPrefsDlg::IDD, AppPrefsDlg::Mode) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	AppPrefsDlg constructor which allows a starting page to be specified.
				Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

AppPrefsDlg::AppPrefsDlg(INT32 StartingPage): DialogTabOp(AppPrefsDlg::IDD, AppPrefsDlg::Mode,
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
}

/******************************************************************************************

>	AppPrefsDlg::~AppPrefsDlg()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	AppPrefsDlg destructor.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

AppPrefsDlg::~AppPrefsDlg()
{
	// Make sure that we remove our options tabs link to the dialog box class
	// as the dialog will now be destroyed
	OptionsTabs::pPrefsDlg = NULL;
}

/******************************************************************************************

>	BOOL AppPrefsDlg::CommitDialogValues()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the dialog box and sets the return values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL AppPrefsDlg::CommitDialogValues()
{
	// Ok has been pressed so take the values from the dialog box
	BOOL ok = TRUE;
	BOOL ValuesOk = TRUE;

	// Make sure that we have set up our class variables so that the tabs know what
	// is the current document and spread to work on.
	Document *pDocument = Document::GetSelected();
	OptionsTabs::SetCurrentDocAndSpread(pDocument);
	// Plus the current document units are correct
	OptionsTabs::SetUpDocUnits();

	// Go through all the tabs declared to the system in turn
	// and tell them to commit the values for their tab
	OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();

	while (pOptionsTabs != NULL)
	{
		// Only get the tab to commit if it is a program based option
		if ( pOptionsTabs->IsProgramOption() )
		{
			// Get the found page to commit its values.
			// Should return False if reported error and we should just fail.
			// 				 2 if set an error which we should report and then fail. 
			ok = pOptionsTabs->CommitSection();
			if ( ok != TRUE)
			{
				if (ok == 2)
					InformError();			// If error required show it.

				// Carry on with other pages so that report all errors and have a chance
				// to correct mistakes in other pages but make note of error state.
				ValuesOk = FALSE;	
			}
		}
		// Get the next option's tab.
		pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
	}

	// Return whether we found bad values or not.
	return ValuesOk;
}

/******************************************************************************************

>	MsgResult AppPrefsDlg::Message(Msg* Message)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
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

MsgResult AppPrefsDlg::Message(Msg* Message)
{
#ifndef STANDALONE

#ifndef REMOVE_PRINT_TABS
	// Jason - This is a message-loop lock which is used in the DIM_CREATE code to 
	// allow the print marks manager cache print marks - this causes a lot of diocument
	// message broadcats and suchlike, which cause re-entrant messages to come in
	// here and trigger 3 billion ERROR3s. While caching the print marks, we lock this
	// message handler so that it just ignores all resultant broadcasts.
	static BOOL IgnoreAllMessages = FALSE;
	if (IgnoreAllMessages)
		return DialogTabOp::Message(Message);
#endif

	ERROR3IF(Message == NULL,"AppPrefsDlg::Message null message received");
	
	// Cope with document changing messages so that if all documents are closed then
	// we take some remedial action such as greying all controls. 
	if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *TheMsg = (DocChangingMsg *) Message;

		switch ( TheMsg->State )
		{
			case DocChangingMsg::TITLECHANGED:
			case DocChangingMsg::SELCHANGED:
			{
				// In the different cases there are different document pointers that
				// we must use.
				Document *pDocument = NULL;
				if (TheMsg->State == DocChangingMsg::TITLECHANGED)
				{
					// Document title has changed message.
					pDocument = TheMsg->pChangingDoc;
				}
				else
				{
					// Document selection changed.
					pDocument = TheMsg->pNewDoc;
				}
							
				// Make sure that we have set up our class variables so that the 
				// tabs know what is the current document and spread to work on.
				OptionsTabs::SetCurrentDocAndSpread(pDocument);
				// Plus the current document units are correct
				OptionsTabs::SetUpDocUnits();
				// Set the document name up to be either:-
				// - the default if we have a null document pointer
				// - the current name of the document
				OptionsTabs::SetDocumentName(pDocument);

 				// Is the new one null ==> no new document
				if (pDocument == NULL)
				{
					// Now go through all the tabs declared to the system
					// And call a function to get them to update themselves.
					OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();

					while (pOptionsTabs != NULL)
					{
						// Only get the tab to update if it is a program based option
						if ( pOptionsTabs->IsProgramOption() )
						{
							// Get that tab to update itself.
							pOptionsTabs->GreySection();
						}

						// Get the next option's tab.
						pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
					}
				}
				else
				{
					// Now go through all the tabs declared to the system
					// And call a function to get them to update themselves.
					OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();

					while (pOptionsTabs != NULL)
					{
						// Only get the tab to update if it is a program based option
						if ( pOptionsTabs->IsProgramOption() )
						{
							// Get that tab to update itself.
							pOptionsTabs->UngreySection();
							//pOptionsTabs->UpdateSection();
						}
						// Get the next option's tab.
						pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
					}
				}
			}
			break;
			
			// This message is sent when a new or just opened document is stable.
			// Selection messages no good as the template document is loaded into
			// the same document and hence the selected document/spread/view does
			// not change and so we do not get the last update.
			// Could cache all messages from BORN to this and only update on this.
			case DocChangingMsg::BORNANDSTABLE:
			{
				// In the different cases there are different document pointers that
				// we must use, in this case the changing doc is relevent.
				Document *pDocument = NULL;
				pDocument = TheMsg->pChangingDoc;
				if (pDocument != NULL)
				{
					// Now go through all the tabs declared to the system
					// And call a function to get them to update themselves.
					// Work out the document name and pass this to the tab so that it has
					// chance to fill in its info field ok.
					OptionsTabs::SetCurrentDocAndSpread(pDocument);
					OptionsTabs::SetDocumentName(pDocument);

					OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();
					String_256 DocumentName = *OptionsTabs::GetDocumentName();

					while (pOptionsTabs != NULL)
					{
						// Only get the tab to update if it is a document based option
						if ( pOptionsTabs->IsProgramOption() )
						{
							// Get that tab to update itself.
							//pOptionsTabs->UngreySection();
							pOptionsTabs->UpdateSection(&DocumentName);
						}

						// Get the next option's tab.
						pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
					}
				}
			}
			break;

		default:
			break;

		}
	}
#ifndef REMOVE_PRINT_TABS
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	else if (MESSAGE_IS_A(Message,PrintMsg))		// Check for print messages
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
	}
#endif
#endif //webster
	else if (MESSAGE_IS_A(Message, DocViewMsg))
	{
		// Cope with document view changing messages so that we allow all the tabs to update
		// themselves when switching to a new view.
		// Unfortunately, at present if we sit on the DocView changing message and wait for
		// a selection then at this point there is NO SELECTED spread and so things will fail.
		// Therefore, wait for a selected spread message  

		DocViewMsg *TheMsg = (DocViewMsg *) Message;

		switch ( TheMsg->State )
		{
			case DocViewMsg::SELCHANGED:
			{
				if (TheMsg->pNewDocView != NULL)
				{
					// Now go through all the tabs declared to the system
					// And call a function to get them to update themselves.
					// Work out the document name and pass this to the tab so that it has
					// chance to fill in its info field ok.
					Document *pDocument = TheMsg->pNewDocView->GetDoc();
					OptionsTabs::SetCurrentDocAndSpread(pDocument);
					OptionsTabs::SetDocumentName(pDocument);

					OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();
					String_256 DocumentName = *OptionsTabs::GetDocumentName();

					while (pOptionsTabs != NULL)
					{
						// Only get the tab to update if it is a application based option
						if ( pOptionsTabs->IsProgramOption() )
						{
							// Get that tab to update itself.
							//pOptionsTabs->UngreySection();
							pOptionsTabs->UpdateSection(&DocumentName);
						}

						// Get the next option's tab.
						pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
					}
				}
			}
			break;

			default:
				break;
		}
	}
	else if (MESSAGE_IS_A(Message, OptionsChangingMsg))
	{
		// Cope with a new units messages so that we give the options tabs a chance to
		// update themselves to display using the newly specified defaults.

		OptionsChangingMsg *TheMsg = (OptionsChangingMsg *) Message;

		// Make sure that we have set up our class variables so that the tabs know what
		// is the current document and spread to work on.
		Document *pDocument = Document::GetSelected();
		OptionsTabs::SetCurrentDocAndSpread(pDocument);
		// Plus the current document units are correct
		OptionsTabs::SetUpDocUnits();

		switch ( TheMsg->State )
		{
			case OptionsChangingMsg::NEWUNITS:
			{
				// Current display units may have changed changed so may need to update
				// any display fields on other tabs which are using units. 
				// Now go through all the tabs declared to the system
				// And call a function to get them to update themselves.
				OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();

				while (pOptionsTabs != NULL)
				{
					// Only get the tab to update if it is a program based option
					if ( pOptionsTabs->IsProgramOption() )
					{
						// Get that tab to update itself.
						pOptionsTabs->NewUnitsInSection();
					}

					// Get the next option's tab.
					pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
				}
				break;
			}
	//	WEBSTER-ranbirr-13/11/96
	#ifndef WEBSTER
			case OptionsChangingMsg::ASKBEFORESETTINGATTR:
			{
				// Tell the Misc tab about the change in state of ask before setting
				// the attribute
				OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();
				while (pOptionsTabs != NULL)
				{
					if (pOptionsTabs->GetPageID() == _R(IDD_OPTSTAB_MISC))
					{
						MiscTab *pMiscTab = (MiscTab *) pOptionsTabs;
						pMiscTab->UpdateAskBefore();
					}

					// Get the next option's tab.
					pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
				}
				break;
			}
			case OptionsChangingMsg::IMPORTWITHLAYERS:
			{
				// Tell the Misc tab about the change in state of Import with layers
				OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();
				while (pOptionsTabs != NULL)
				{
					if (pOptionsTabs->GetPageID() == _R(IDD_OPTSTAB_MISC))
					{
						MiscTab *pMiscTab = (MiscTab *) pOptionsTabs;
						pMiscTab->UpdateImportWithLayers();
					}

					// Get the next option's tab.
					pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
				}
				break;
			}
	#endif //webster
			case OptionsChangingMsg::NEWPAGESIZE:
			case OptionsChangingMsg::PAGESIZEHASCHANGED:
			{
				// Current page size may have changed changed so may need to update
				// any display fields on other tabs which are showing pages. 
				// Now go through all the tabs declared to the system
				// And call a function to get them to update themselves.
				// PAGESIZEHASCHANGED is sent by the page tab so do not get it to update.
				// NEWPAGESIZE sent by the undo/redo actions so do get it to update
				OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();

				while (pOptionsTabs != NULL)
				{
					// If we are the page tab then call its special function but only if it
					// did not send the message.
					if (pOptionsTabs->GetPageID() == _R(IDD_OPTSTAB_PAGE) &&
						TheMsg->State == OptionsChangingMsg::NEWPAGESIZE )
					{
						PageTab *pPageTab = (PageTab *) pOptionsTabs;
						pPageTab->UpdatePageSection();
					}
					// Only sned the message to a tab if its an application based option
					if ( pOptionsTabs->IsProgramOption() )
					{
						// Call a function to allow other tabs to update themselves
						pOptionsTabs->PageSizeHasChanged();
					}
					
					// Get the next option's tab.
					pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
				}
				break;
			}
			case OptionsChangingMsg::NEWDEFAULTGRID:
			{
				// Current default grid may have changed changed
				// so may need to update any display fields on other tabs which
				// are showing pages. 
				// Now go through all the tabs declared to the system
				// And call a function to get them to update themselves.
				OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();

				while (pOptionsTabs != NULL)
				{
					if (pOptionsTabs->GetPageID() == _R(IDD_OPTSTAB_GRID))
					{
						GridTab *pGridTab = (GridTab *) pOptionsTabs;
						pGridTab->UpdateGridSection();
					}

					// Get the next option's tab.
					pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
				}
				break;
			}

			default:
				{
					// If nobody above has processed the message, then pass it around all the tabs in
					// case it's of use to them.
					OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();

					while (pOptionsTabs != NULL)
					{
						// Only sned the message to a tab if its an application based option
						if ( pOptionsTabs->IsProgramOption() )
							pOptionsTabs->Message(Message);
						pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
					}
				}
				break;
		}
	}
	else if (IS_OUR_DIALOG_MSG(Message))
	{
		// Cope with messages for the dialog box
		DialogMsg* Msg = (DialogMsg*)Message;

		MsgResult Result;

		// Main handler for dialog type messages
		BOOL EndDialog = FALSE;						// TRUE if we should quit the dialog
		BOOL CommitValues = FALSE;					// TRUE if we should commit the dialog values
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
			OptionsTabs::SetInitMessageState(TRUE);				
		
		// If the message is not for the main options window then try the tab pages 
		BOOL FoundPage = FALSE;
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
#ifndef REMOVE_PRINT_TABS
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
#endif
					break;
					// grey out the apply now button as this should only become enabled	when 
					// we have made a change to the dialog box.
//					OptionsTabs::SetInitMessageState(FALSE);
//					OptionsTabs::SetApplyNowState(FALSE);
//					OptionsTabs::GreyApplyNow();
//					break;
				
				case DIM_COMMIT:		// Want to commit and quit
				{
					OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();
					while (pOptionsTabs != NULL)
					{
						// Only give the tab the message if it is a application based option
						if ( pOptionsTabs->IsProgramOption() )
							pOptionsTabs->HandleMsg(Msg);
						pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
					}
					EndDialog = TRUE;
					CommitValues = TRUE;
				}
				break;

				case DIM_SOFT_COMMIT:	// Want to commit
				{
					OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();
					while (pOptionsTabs != NULL)
					{
						// Only give the tab the message if it is a application based option
						if ( pOptionsTabs->IsProgramOption() )
							pOptionsTabs->HandleMsg(Msg);
						pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
					}

					CommitValues = TRUE;
				}
				break;

				case DIM_CANCEL:		// Want to quit
				{

				// We pass the message to all the tabs so that they clean up and discard all changes made, if any
					OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();
					while (pOptionsTabs != NULL)
					{
						// Only give the tab the message if it is a application based option
						if ( pOptionsTabs->IsProgramOption() )
							pOptionsTabs->HandleMsg(Msg);
						pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
					}
					EndDialog = TRUE;
				}
				break;

				default:
					break;
			}
		}

		// Commit values here
		if (CommitValues)
		{
//			// To stop dangerous things happening such as being told to cancel the dialog box
//			// when we are in the middle of a apply now/ok, we will grey all the dodgy buttons
//			// whilst we are in danger of this happening.
//			// Example: if change show transparency state and apply this, then a dialog may
//			// pop up to question whether this is required or not. At this point the dialog
//			// box is still accessable. If click cancel then access violations happen.
//			CDlgResID PageID = GetCurrentPageID();	// Get currently selected Tab id
//			TalkToPage(NULL);						// Select the main tab
//			EnableGadget(_R(ID_APPLY_NOW), FALSE);		// grey button on main tab
//			EnableGadget(IDOK, FALSE);				// grey button on main tab
//			EnableGadget(IDCANCEL, FALSE);			// grey button on main tab
//			TalkToPage(PageID);						// Select the originally selected tab
//			BOOL ApplyNow = OptionsTabs::SetApplyNowState(FALSE);

			BOOL ok = CommitDialogValues();
			// Only unset EndDialog as in SOFT_COMMIT case we do not want to end the
			// Operation and hence close the dialog box
			if (!ok)
			{
				EndDialog = FALSE;	// Values not correct so do not allow exit
				Msg->DlgMsg = DIM_SOFT_COMMIT; // turn the message into a soft-commit so the base class does not destroy the dialog
			}

#ifndef REMOVE_PRINT_TABS
			// Now, a nasty bodge. The print tabs need to re-cache their local print control data
			// from the committed values in the document print control, or else all hell will break
			// loose.
	//	WEBSTER-ranbirr-13/11/96
	#ifndef WEBSTER
			PrintBaseTab::DocumentSettingsUpdated();
#endif

//			// Committing and not closing the dialog so re-enable the ok, cancel and apply
//			// now buttons. 
//			PageID = GetCurrentPageID();			// Get currently selected Tab id
//			TalkToPage(NULL);						// Select the main tab
//			EnableGadget(_R(ID_APPLY_NOW), TRUE);		// ungrey button on main tab
//			EnableGadget(IDOK, TRUE);				// ungrey button on main tab
//			EnableGadget(IDCANCEL, TRUE);			// ungrey button on main tab
//			TalkToPage(PageID);						// Select the originally selected tab
//			OptionsTabs::SetApplyNowState(ApplyNow);
#endif //webster
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
		// Note that this call can DELETE "this"
		Result = DialogTabOp::Message(Message);

		// End dialog here
		if (EndDialog) 
		{
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
	else
	{
		// If nobody above has processed the message, then pass it around all the tabs in
		// case it's of use to them.
		OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();

		while (pOptionsTabs != NULL)
		{
			// Only give the tab the message if it is a application based option
			if ( pOptionsTabs->IsProgramOption() )
				pOptionsTabs->Message(Message);
			pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
		}
	}
	

#endif

//	return OK; 
	return DialogTabOp::Message(Message);

}

/******************************************************************************************

>	OpState	AppPrefsDlg::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	AppPrefsDlg GetState method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

OpState	AppPrefsDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;

	// If the pointer is non-null then it implies that another dialog box is up which
	// uses our tab set and so we must force greying of the menu option 
	if (OptionsTabs::pPrefsDlg != NULL)
		OpSt.Greyed=TRUE;

	return(OpSt);
}




/******************************************************************************************

>	BOOL AppPrefsDlg::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	AppPrefsDlg Init method. Called when the kernel initialises.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL AppPrefsDlg::Init()
{
	BOOL InitOK = TRUE;

	// Declare any preferences that we require.
	if ( Camelot.DeclareSection(TEXT("Preferences"), 3) )
	{
		// section declared ok so now define the preference option  
		Camelot.DeclarePref(TEXT("Preferences"), TEXT("SaveOnExit"), &SaveOnExit, 0, 1);
	}

	// Now register ourselves with the dialog system
	InitOK = RegisterOpDescriptor(
								0,								// Tool ID 
								_R(IDS_UTILOPTIONS), 				// String resource ID
								CC_RUNTIME_CLASS(AppPrefsDlg),	// Runtime class
								OPTOKEN_UTILOPTIONS, 			// Token string
								GetState,						// GetState function
								0,								// help ID
								_R(IDBBL_OPTIONSDLG),				// bubble help
								_R(IDD_BARCONTROLSTORE),			// resource ID
								_R(IDC_BTN_OPTIONSDLG),				// control ID
								SYSTEMBAR_WINDOW,				// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								0,								// No vertical counterpart
								_R(IDS_APPPREFSDLG_ONE),			// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);

	// Now register ourselves with the dialog system for the units button on the windows
	// button bar as well
	InitOK = InitOK && RegisterOpDescriptor(
								0,								// Tool ID
								_R(IDS_UNITDLG),					// String resource ID
								CC_RUNTIME_CLASS(UnitsTabAppPrefsDlg),	// Runtime class
								OPTOKEN_UNITDLG,				// Token string
								GetState,						// GetState function
								0,								// help ID
								_R(IDBBL_UNITDLG),					// bubble help
								_R(IDD_BARCONTROLSTORE),			// resource ID
								_R(IDC_BTN_UNITSETUPDLG),			// control ID
								SYSTEMBAR_WINDOW,				// Bar ID
								TRUE, 							// Receive messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// No vertical counterpart
								_R(IDS_UNIT_DLG_OPEN),				// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags								);
								);

	// Now register ourselves with the dialog system for the units button on the windows
	// button bar as well
	InitOK = InitOK && RegisterOpDescriptor(
								0,								// Tool ID
								_R(IDS_SCALEDLG),					// String resource ID
								CC_RUNTIME_CLASS(ScaleTabAppPrefsDlg),	// Runtime class
								OPTOKEN_SCALEDLG,				// Token string
								GetState,						// GetState function
								0,								// help ID
								_R(IDBBL_SCALEDLG),					// bubble help
								_R(IDD_BARCONTROLSTORE),			// resource ID
								_R(IDC_BTN_SCALEFACTORDLG),			// control ID
								SYSTEMBAR_WINDOW,				// Bar ID
								TRUE, 							// Receive messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// No vertical counterpart
								_R(IDS_SCALE_DLG_OPEN),				// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags								);
								);

	// Now register ourselves with the dialog system for somebody who wants to get to the page options tab
	InitOK = InitOK && RegisterOpDescriptor(
								0,								// Tool ID 
								_R(IDS_PAGESIZEDLG), 				// String resource ID
								CC_RUNTIME_CLASS(PageTabAppPrefsDlg),	// Runtime class
								OPTOKEN_PAGESIZEDLG, 			// Token string
								GetState,						// GetState function
								0,								// help ID
								_R(IDBBL_PAGESIZEDLG),				// bubble help
								_R(IDD_BARCONTROLSTORE),			// resource ID
								_R(IDC_BTN_PAGESIZEDLG),			// control ID
								SYSTEMBAR_WINDOW,				// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,								// No vertical counterpart
								_R(IDS_APPPREFSDLG_ONE),			// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);

	// Now register ourselves with the dialog system for somebody who wants to get to the page options tab
	InitOK = InitOK && RegisterOpDescriptor(
								0,								// Tool ID 
								_R(IDS_GRIDANDRULERSDLG),			// String resource ID
								CC_RUNTIME_CLASS(GridTabAppPrefsDlg),	// Runtime class
								OPTOKEN_GRIDANDRULERSDLG,		// Token string
								GetState,						// GetState function
								0,								// help ID
								_R(IDBBL_GRIDANDRULERSDLG),			// bubble help
								0, //_R(IDD_BARCONTROLSTORE),			// resource ID
								0, //_R(IDC_BTN_PAGESIZEDLG),			// control ID
								SYSTEMBAR_WINDOW,				// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,								// No vertical counterpart
								_R(IDS_APPPREFSDLG_ONE),			// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);
	
	// Now register ourselves with the dialog system for somebody who wants to get to the plug-ins tab
	InitOK = InitOK && RegisterOpDescriptor(
								0,								// Tool ID 
								_R(IDS_PLUGINS_PLUGINSMNGR), 				// String resource ID
								CC_RUNTIME_CLASS(PlugsTabAppPrefsDlg),	// Runtime class
								OPTOKEN_PLUGSDLG,	// Token string
								GetState,						// GetState function
								0,								// help ID
								0,								// bubble help
								0,								// resource ID
								_R(IDS_PLUGINS_PLUGINSMNGR),		// control ID
								SYSTEMBAR_WINDOW,				// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,								// No vertical counterpart
								_R(IDS_APPPREFSDLG_ONE),			// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);

	// Set up any options tabs that are required
	InitOK = InitOK && OptionsTabs::InitOptionsTabs();	

	// Set up any page sizes that are required
	// Make the doc unit list
	pPageSizesList = new PageSizesList;
	InitOK = InitOK && (pPageSizesList != NULL && pPageSizesList->InitPageSizes());


	// Set up our special print options dialog box as well as this is effectively the
	// same dialog box.
//	WEBSTER-ranbirr-13/11/96
#pragma message( __LOCMSG__ "Removed PrintPrefsDlg::Init usage" )
#ifndef WEBSTER
//	InitOK = InitOK && PrintPrefsDlg::Init();
#endif //webster


	return (InitOK);
}


/******************************************************************************************

>	void AppPrefsDlg::Deinit()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	AppPrefsDlg Deinit method. Called when the kernel deinitialises.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void AppPrefsDlg::Deinit()
{
	BOOL ok;
	
	// Remove any options tabs that have been declared to the system.
	ok = OptionsTabs::DeinitOptionsTabs();	

	// Remove the page sizes list that is currently attached to the applications options
	// dialog box for the present moment.  
	if (pPageSizesList != NULL)
	{
		ok = pPageSizesList->DeinitPageSizes();	// delete all list items
		delete pPageSizesList;					// delete list item object
		pPageSizesList = NULL;					// set item pointer to null
	}
}



/******************************************************************************************

>	void AppPrefsDlg::Do(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void AppPrefsDlg::Do(OpDescriptor*)
{
	BOOL ok;

	//if (OptionsTabs::pPrefsDlg != NULL)
	//{
	//	ERROR3("AppPrefsDlg::Do an options tabbed dialog box is already present");
	//	return;
	//}
	
	// Set up the name that the main dialog page is going to use
	String_256 Temp(_R(IDN_APPLICATIONOPTS)); 
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
		// MarkH 29/6/99 - We need to physically walk through the tabs and initialize them as
		// the new MFC in VC6 dosn't do it for you!!! GIT!!!!
		// Now go through all the tabs declared to the system
		OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();

		while (pOptionsTabs != NULL)
		{
			// Only Initialize the Section tab if it is a program based option
			if ( pOptionsTabs->IsProgramOption() )
			{
				// Make sure we`re talking to the page!
				TalkToPage(pOptionsTabs->GetPageID());
				pOptionsTabs->InitSection();
			}

			// Get the next option's tab.
			pOptionsTabs = OptionsTabs::GetNext(pOptionsTabs);
		}

		// Modal at present so remove the Open()
		Open();								// created ok so open the dialog box up

		// grey out the apply now button as this should only become enabled when we have 
		// made a change to the dialog box.
		OptionsTabs::SetInitMessageState(FALSE);
		OptionsTabs::SetApplyNowState(FALSE);
		OptionsTabs::GreyApplyNow();
	}
	else
	{
		// Could not create the dialog box so call inform error 
		InformError();						// Show user the error
  		End(); 		   						// End the operation 
	}				   	
}

/******************************************************************************************

>	BOOL AppPrefsDlg::RegisterYourPagesInOrderPlease()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/94
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

BOOL AppPrefsDlg::RegisterYourPagesInOrderPlease()
{
	BOOL ok = TRUE;

	// Now go through all the tabs declared to the system
	OptionsTabs *pOptionsTabs = OptionsTabs::GetFirst();

	while (pOptionsTabs != NULL)
	{
		// Only add the tab if it is a program based option
		if ( pOptionsTabs->IsProgramOption() )
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

>	UnitsTabAppPrefsDlg::UnitsTabAppPrefsDlg(): AppPrefsDlg(1) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	UnitsTabAppPrefsDlg constructor. Creates a non-undoable operation.
				Just starts up the AppPrefsDlg specifying the tab number that it is in the
				list. This has been calculated when the options tabs were declared just to
				be on the safe side.
				Must use a number rather than the ID as this is what MFC's property sheet
				constructors require. 
	Errors:		-
	SeeAlso:	ScaleTabAppPrefsDlg; PageTabAppPrefsDlg;

******************************************************************************************/

UnitsTabAppPrefsDlg::UnitsTabAppPrefsDlg(): AppPrefsDlg( OptionsTabs::UnitsAppTabNumber ) 
{
	// At present, just:- do nothing
}

/******************************************************************************************

>	ScaleTabAppPrefsDlg::ScaleTabAppPrefsDlg(): AppPrefsDlg(1) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ScaleTabAppPrefsDlg constructor. Creates a non-undoable operation.
				Just starts up the AppPrefsDlg specifying the tab number that it is in the
				list. This has been calculated when the options tabs were declared just to
				be on the safe side.
				Must use a number rather than the ID as this is what MFC's property sheet
				constructors require. 
	Errors:		-
	SeeAlso:	UnitsTabAppPrefsDlg; PageTabAppPrefsDlg;

******************************************************************************************/

ScaleTabAppPrefsDlg::ScaleTabAppPrefsDlg(): AppPrefsDlg( OptionsTabs::ScaleAppTabNumber ) 
{
	// At present, just:- do nothing
}

/******************************************************************************************

>	PageTabAppPrefsDlg::PageTabAppPrefsDlg(): AppPrefsDlg(1) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PageTabAppPrefsDlg constructor. Creates a non-undoable operation.
				Just starts up the AppPrefsDlg specifying the tab number that it is in the
				list. This has been calculated when the options tabs were declared just to
				be on the safe side.
				Must use a number rather than the ID as this is what MFC's property sheet
				constructors require. 
	Errors:		-
	SeeAlso:	ScaleTabAppPrefsDlg; UnitsTabAppPrefsDlg

******************************************************************************************/

PageTabAppPrefsDlg::PageTabAppPrefsDlg(): AppPrefsDlg( OptionsTabs::PageAppTabNumber ) 
{
	// At present, just:- do nothing
}

/******************************************************************************************

>	GridTabAppPrefsDlg::GridTabAppPrefsDlg(): AppPrefsDlg(1) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GridTabAppPrefsDlg constructor. Creates a non-undoable operation.
				Just starts up the AppPrefsDlg specifying the tab number that it is in the
				list. This has been calculated when the options tabs were declared just to
				be on the safe side.
				Must use a number rather than the ID as this is what MFC's property sheet
				constructors require. 
	Errors:		-
	SeeAlso:	ScaleTabAppPrefsDlg; UnitsTabAppPrefsDlg; GridTabAppPrefsDlg;

******************************************************************************************/

GridTabAppPrefsDlg::GridTabAppPrefsDlg(): AppPrefsDlg( OptionsTabs::GridAppTabNumber ) 
{
	// At present, just:- do nothing
}

/******************************************************************************************

>	PlugsTabAppPrefsDlg::PlugsTabAppPrefsDlg(): AppPrefsDlg(1) 


	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Neville (above)
	Created:	13/09/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PlugsTabAppPrefsDlg constructor. Creates a non-undoable operation.
				Just starts up the AppPrefsDlg specifying the tab number that it is in the
				list. This has been calculated when the options tabs were declared just to
				be on the safe side.
				Must use a number rather than the ID as this is what MFC's property sheet
				constructors require. 
	Errors:		-
	SeeAlso:	ScaleTabAppPrefsDlg; PageTabAppPrefsDlg;

******************************************************************************************/

PlugsTabAppPrefsDlg::PlugsTabAppPrefsDlg(): AppPrefsDlg( OptionsTabs::PlugsAppTabNumber ) 
{
	// At present, just:- do nothing
}


/******************************************************************************************

// Code to handle a list of options tabs available to the system

*/

/********************************************************************************************

>	OptionsTabs::OptionsTabs()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Purpose:	Constructor.
	Errors:
	SeeAlso:	AppPrefsDlg;

********************************************************************************************/

OptionsTabs::OptionsTabs()
{
	// Ensure our link to the application options dialog box is set to a good default.
	pPrefsDlg = NULL;
	pScopeDocument = NULL;
}


/********************************************************************************************

>	OptionsTabs::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Purpose:	Initialiser.
	Errors:
	SeeAlso:	AppPrefsDlg;

********************************************************************************************/

BOOL OptionsTabs::Init()
{
	// Do nothing for now.
	return TRUE;
}


/******************************************************************************************
******************************************************************************************/
/*	if (!pOptionsTab->InitSection())												\*/
//Taken out!

#define ADD_OPTIONSTAB(Classname) 													\
	/* Create, initialise and install the specified tab to the options system */	\
	pOptionsTab = new Classname;													\
	if (pOptionsTab == NULL) return FALSE;											\
																					\
	if (!pOptionsTab->Init())														\
	{																				\
		/* Error occured - report it and stop trying to initialise options tab. */	\
		delete pOptionsTab;															\
		InformError();																\
		return TRUE;																\
	}																				\
																					\
	OptionsTabsList.AddTail(pOptionsTab);

/******************************************************************************************
******************************************************************************************/

/********************************************************************************************

>	BOOL OptionsTabs::InitOptionsTabs()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Returns:	TRUE if initialised ok, FALSE if not.
	Purpose:	Scans all OptionsTabs to find out which OptionsTabs are available for use, and
				instantiates one of each, and holds them in a list.
	Errors:		Out of memory (if a OptionsTab fails to initialise for other reasons then an 
				error is reported, but Camelot will still start up ok, i.e. TRUE is 
				returned).
	SeeAlso:	OptionsTab

********************************************************************************************/

BOOL OptionsTabs::InitOptionsTabs()
{
	// Find the OptionsTabs - the kernel ones are hard-wired.
	OptionsTabs *pOptionsTab = NULL;

#ifndef STANDALONE

	// Create, initialise and install the built in OptionsTabs list
	//
	// NOTE: These should be defined in alphabetical ordering unless there is a very good
	// reason otherwise.
	//
	//ADD_OPTIONSTAB(EditTab)
	ADD_OPTIONSTAB(MiscTab)			// General
	ADD_OPTIONSTAB(GridTab)			// Grid and Rulers
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
#ifdef BUILD_SEPARATIONS
	ADD_OPTIONSTAB(PrintImagesetterTab) // Imagesetting
#endif
#endif //webster
PORTNOTE("other", "Disabled internet tab")
#ifndef EXCLUDE_FROM_XARALX
	ADD_OPTIONSTAB(InternetTab)		// Internet options
#endif
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	ADD_OPTIONSTAB(PointersTab)		// Mouse
PORTNOTE("other", "Disabled print general tab")
#ifndef EXCLUDE_FROM_XARALX
	ADD_OPTIONSTAB(PrintGeneralTab)	// Output
#endif
#endif //webster
	ADD_OPTIONSTAB(PageTab)			// Page
#ifdef PHOTOSHOPPLUGINS
	// Only add in if required - general plug-in removal at present
	ADD_OPTIONSTAB(PlugInsTab)		// Plug-ins
#endif
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
PORTNOTE("other", "Disabled print layout tab")
#ifndef EXCLUDE_FROM_XARALX
	ADD_OPTIONSTAB(PrintLayoutTab)	// Print layout
#endif
	ADD_OPTIONSTAB(ScaleTab)		// Scaling
#ifdef BUILD_SEPARATIONS
PORTNOTE("other", "Disabled print separations tab")
#ifndef EXCLUDE_FROM_XARALX
	ADD_OPTIONSTAB(PrintSepsTab)	// Separation (Colour) 
#endif
#endif
	ADD_OPTIONSTAB(TuneTab)			// Tune ups
//	ADD_OPTIONSTAB(UndoTab)
	ADD_OPTIONSTAB(UnitsTab)		// Units
	ADD_OPTIONSTAB(ViewTab)			// View
#endif //webster

	// We must work out what the units tab number is when used in document mode so
	// that we can skip to it directly when using the units button on the tool bar.
	INT32 i = 0;
	INT32 print_i = 0;

	// Now go through all the tabs declared to the system
	pOptionsTab = OptionsTabs::GetFirst();

	while (pOptionsTab != NULL)
	{
		// Only include the tab if it is a document based option
		if ( pOptionsTab->IsProgramOption() )
		{
			//	WEBSTER-ranbirr-13/11/96
			#ifndef WEBSTER
			if( _R(IDD_OPTSTAB_UNITS) == pOptionsTab->GetPageID() )
					UnitsAppTabNumber = i;		// note that tab number
			else
			if( _R(IDD_OPTSTAB_SCALE) == pOptionsTab->GetPageID() )
					ScaleAppTabNumber = i;		// note that tab number
			else
			#endif //websdter
			if( _R(IDD_OPTSTAB_PAGE) == pOptionsTab->GetPageID() )
					PageAppTabNumber = i;		// note that tab number
			else
			if( _R(IDD_OPTSTAB_GRID) == pOptionsTab->GetPageID() )
					GridAppTabNumber = i;		// note that tab number
			else
			if( _R(IDD_OPTSTAB_PLUGINS) == pOptionsTab->GetPageID() )
					PlugsAppTabNumber = i;		// note that tab number
			
			i += 1;		// increment the counter 
		}
	//	WEBSTER-ranbirr-13/11/96
	#ifndef WEBSTER
		// In this section, only include the tab if it is a printing based option
		if ( pOptionsTab->IsPrintingOption() )
		{
			if( _R(IDD_OPTSTAB_PRINTLAYOUT) == pOptionsTab->GetPageID() )
			{
					PrintTabNumber = print_i;	// note that tab number
			}
			
			print_i += 1;		// increment the counter 
		}
	#endif //webster
		// Get the next option's tab.
		pOptionsTab = OptionsTabs::GetNext(pOptionsTab);
	}
TRACEUSER( "Neville", _T("OptionsTabs::UnitsAppTabNumber = %d\n"),OptionsTabs::UnitsAppTabNumber);
TRACEUSER( "Neville", _T("OptionsTabs::ScaleAppTabNumber = %d\n"),OptionsTabs::ScaleAppTabNumber);
TRACEUSER( "Neville", _T("OptionsTabs::PageAppTabNumber = %d\n"),OptionsTabs::PageAppTabNumber);
TRACEUSER( "Neville", _T("OptionsTabs::GridAppTabNumber = %d\n"),OptionsTabs::GridAppTabNumber);

TRACEUSER( "Neville", _T("OptionsTabs::PrintTabNumber = %d\n"),OptionsTabs::PrintTabNumber);
#endif

	// All ok
	return TRUE;
}

/********************************************************************************************

>	void OptionsTabs::DeinitOptionsTabs()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Purpose:	Destroy all the OptionsTab objects.
	SeeAlso:	OptionsTab

********************************************************************************************/

BOOL OptionsTabs::DeinitOptionsTabs()
{
	// Get rid of our OptionsTabs
	OptionsTabsList.DeleteAll();

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL OptionsTabs::DeclareOptionsTab(OptionsTabs *pOptionsTab)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Inputs:		A pointer to a new OptionsTabs class already declared into the system
	Returns:	True if tab declared, False if something went wrong. 
	Purpose:	A function to declare a new tab to the system.
	SeeAlso:	OptionsTab

********************************************************************************************/

BOOL OptionsTabs::DeclareOptionsTab(OptionsTabs *pOptionsTab)
{
	// First check to see if somebody has actually declared the pointer to be non null.
	if (pOptionsTab == NULL) return FALSE;

	if (!pOptionsTab->Init())
	{
		/* Error occured - report it and stop trying to initialise the options tab. */
		InformError();
		return TRUE;
	}
	OptionsTabsList.AddTail(pOptionsTab);
	
	// All ok
	return TRUE;
}


/********************************************************************************************

>	virtual MsgResult OptionsTabs::Message(Msg *pMessage)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/96

	Inputs:		pMessage - the message
	Returns:	OK	 		  Message handled ok (return this even if you don't need to 
							  respond to the message). 

				FAIL 		  Something terrible happened whilst processing the message
				 			  eg. we ran out of memory. You must set ERROR if you
				 			  are returning this value. 

	Purpose:	A traditional message handler

				NOTE that many generally useful messages like DocChangingMsgs are handled
				by the caller, and are converted into upcalls to functions like UpdateSection()
				so will NOT be passed on to this function. However, less-used messages like
				ColourChangingMsgs will be passed on to this generic handler.

				The base class simply returns OK. Derived classes which want to override
				this behaviour should return OK, unless something particuilarly unpleasant
				happened.

********************************************************************************************/

MsgResult OptionsTabs::Message(Msg *pMessage)
{
	return(OK);
}


/********************************************************************************************

>	OptionsTabs *OptionsTabs::GetFirst()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Returns:	Pointer to the first OptionsTab, or NULL if none.
	Purpose:	Iterating through the list of OptionsTabs known to Camelot.  This function
				returns the first OptionsTabs.
	SeeAlso:	OptionsTabs::GetNext()
	Scope:		Static

********************************************************************************************/

OptionsTabs *OptionsTabs::GetFirst()
{
	return (OptionsTabs *) OptionsTabsList.GetHead();
}

/********************************************************************************************

>	OptionsTabs *OptionsTabs::GetNext(OptionsTabs *pOptionsTabs)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Inputs:		pOptionsTabs - pointer to the OptionsTabs to be used to get the following
				OptionsTabs.
	Returns:	Pointer to the next OptionsTabs, or NULL if none.
	Purpose:	Iterating through the list of OptionsTabs known to Camelot.  This function
				returns the next OptionsTabs.
	SeeAlso:	OptionsTabs::GetFirst

********************************************************************************************/

OptionsTabs *OptionsTabs::GetNext(OptionsTabs *pOptionsTabs)
{
	return (OptionsTabs *) OptionsTabsList.GetNext(pOptionsTabs);
}


/******************************************************************************************

>	static void OptionsTabs::SetDefaultUnits()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets up the units class variables to the default values. 
	Errors:		-
	SeeAlso:	SetUpDocUnits;

******************************************************************************************/
void OptionsTabs::SetDefaultUnits()
{
	// Just to be on the safe side set up some default values 
	CurrentPageUnits = MILLIMETRES;			// Set up random default units.
	CurrentScaledUnits = MILLIMETRES;		// Set up random default units.
	CurrentFontUnits = COMP_POINTS;			// Set up random default units.

}

/******************************************************************************************

>	static void OptionsTabs::SetUpDocUnits()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if set up ok, FALSE otherwise.
	Purpose:	Sets up the units variable to the currently set document units 
	Errors:		-
	SeeAlso:	SetDefaultUnits;

******************************************************************************************/

BOOL OptionsTabs::SetUpDocUnits()
{
	BOOL SetUpOk = FALSE; 

	// Set up the default document measurement units that are used
	// Get the page units from the doc unit list of the current document unless there
	// is no document present.
	// Otherwise just pick a random default.
	pDocument = Document::GetSelected();
	if (pDocument == NULL)
	{
		// Just to be on the safe side set up some default values 
		CurrentPageUnits = MILLIMETRES;						
		CurrentScaledUnits = CurrentPageUnits;
		CurrentFontUnits = COMP_POINTS;
	}
	else
	{
		// Get the unit list attached to the current document
		//DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
		DocUnitList* pDocUnitList = pDocument->GetDocUnitList();
		ERROR3IF(pDocUnitList == NULL,"OptionsTabs::SetUpDocUnits No doc unit list attached to this doc yet");
	
		if (pDocUnitList != NULL)
		{
			CurrentPageUnits = pDocUnitList->GetPageUnits(); 	// Set to the page units.
		
			CurrentScaledUnits = pDocUnitList->GetScaleUnits();	// Set to the scale units.
			// If set to automatic then use the current page units
			if (CurrentScaledUnits == AUTOMATIC)
				CurrentScaledUnits = CurrentPageUnits; 
	
			CurrentFontUnits = pDocUnitList->GetFontUnits();	// Set to the font units.

			// Flag that everything went ok
			SetUpOk = TRUE; 
		}
		else
		{
			// Just to be on the safe side set up some default values 
			CurrentPageUnits = MILLIMETRES;						// Set up random default units.
			CurrentScaledUnits = MILLIMETRES;					// Set up random default units.
			CurrentFontUnits = COMP_POINTS;						// Set up random default units.
		}
	}

	return (SetUpOk);
}

/******************************************************************************************

>	BOOL OptionsTabs::NewUnitsInSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Called when new default units chosen. Gives any tabbed dialog a chance to
				update any fields with new units in.
				This is the base class version and so will be used for any tabs which have
				no specified handling code. 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL OptionsTabs::NewUnitsInSection()
{
	// Called when new default units chosen
	// Would normally be overriden by a a particular tab
	return TRUE;
}

/******************************************************************************************

>	BOOL OptionsTabs::GreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Called when the user has closed all documents. Gives any tabbed dialog a
				chance to grey any fields which are document specific.
				This is the base class version and so will be used for any tabs which have
				no specified handling code.  
	Errors:		-
	SeeAlso:	OptionsTabs::UngreySection(); OptionsTabs::UpdateSection();

******************************************************************************************/

BOOL OptionsTabs::GreySection()
{
	// Called when the user has closed all documents
	// Would normally be overriden by a a particular tab
	return TRUE;
}

/******************************************************************************************

>	BOOL OptionsTabs::UngreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Called when the user has opened a document. Gives any tabbed dialog a
				chance to update any fields which may have been greyed when the last
				document was closed.
				This is the base class version and so will be used for any tabs which have
				no specified handling code.   
	Errors:		-
	SeeAlso:	 OptionsTabs::GreySection(); OptionsTabs::UpdateSection();

******************************************************************************************/

BOOL OptionsTabs::UngreySection()
{
	// Called when the user has opened a document
	// Would normally be overriden by a a particular tab
	return TRUE;
}

/******************************************************************************************

>	BOOL OptionsTabs::UpdateSection(String_256 *DocumentName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/95
	Inputs:		Name of document
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Called when the user has clicked on a new document or opened a new document.
				Gives any tabbed dialog a chance to update any fields which are used to
				reflect the current document's status.
				The document name allows the info field on the tab to be filled in correctly.
				This is the base class version and so will be used for any tabs which have
				no specified handling code.   
	Errors:		-
	SeeAlso:	OptionsTabs::UngreySection(); OptionsTabs::GreySection();

******************************************************************************************/

BOOL OptionsTabs::UpdateSection(String_256 *DocumentName)
{
	// Called when the user has opened a document
	// Would normally be overriden by a a particular tab
	return TRUE;
}


/******************************************************************************************

>	BOOL OptionsTabs::GreyApplyNow()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Makes the Apply Now button on the main page of the tabbed dialog grey. 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL OptionsTabs::GreyApplyNow()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"OptionsTabs::GreyApplyNow called with no dialog pointer");

	// Grey out the apply now button on the main page
	// Must restore the state of the currently selected tab as otherwise you might start
	// switching to talking to the main page when we are in the middle of talking to controls
	// on one of the pages and hence give rise to unknown controls messages when we try to
	// talk to the pages controls again.
	CDlgResID PageID = pPrefsDlg->GetCurrentPageID();	// Get currently selected Tab id
	pPrefsDlg->TalkToPage(0);
	pPrefsDlg->EnableGadget(_R(ID_APPLY_NOW), FALSE);
	pPrefsDlg->TalkToPage(PageID); 						// Select the originally selected tab

	return TRUE;
}

/******************************************************************************************

>	BOOL OptionsTabs::UngreyApplyNow()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Makes the Apply Now button on the main page of the tabbed dialog ungrey. 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL OptionsTabs::UngreyApplyNow()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"OptionsTabs::UngreyApplyNow called with no dialog pointer");
	
	// Ungrey the apply now button on the main page
	// Must restore the state of the currently selected tab as otherwise you might start
	// switching to talking to the main page when we are in the middle of talking to controls
	// on one of the pages and hence give rise to unknown controls messages when we try to
	// talk to the pages controls again.
	CDlgResID PageID = pPrefsDlg->GetCurrentPageID();	// Get currently selected Tab id
	pPrefsDlg->TalkToPage(0);						// Select the main tab
	pPrefsDlg->EnableGadget(_R(ID_APPLY_NOW), TRUE);		// ungrey button on main tab
//// Do these two just in case
//pPrefsDlg->EnableGadget(IDOK, TRUE);				// ungrey button on main tab
//pPrefsDlg->EnableGadget(IDCANCEL, TRUE);			// ungrey button on main tab
	pPrefsDlg->TalkToPage(PageID); 						// Select the originally selected tab

	return TRUE;
}

/********************************************************************************************

>	BOOL OptionsTabs::GetApplyNowState() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The current ApplyNow state as either True or False.
	Purpose:	Finds out the current state of the apply now button.
	Errors:		-
	SeeAlso:	SetApplyNowState();

********************************************************************************************/

BOOL OptionsTabs::GetApplyNowState()
{
	return ApplyNow;
} 

/********************************************************************************************

>	BOOL OptionsTabs::SetApplyNowState(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Inputs:		New ApplyNow flag state.
	Outputs:	-
	Returns:	The old ApplyNow state as either True or False.
	Purpose:	Allows the setting of a new ApplyNow state.
	Errors:		-
	SeeAlso:	GetApplyNowState();

********************************************************************************************/

BOOL OptionsTabs::SetApplyNowState(BOOL NewState)
{
	BOOL OldState = ApplyNow;		// note current old state
	// Only make the change if we are not in an init message state. As otherwise, when we
	// do an init, we will be sent a message to say a control has been changed and then
	// try to set the ApplyNow TRUE and hence then try to ungrey the ApplyNow button when
	// we shouldn't. 
	if (!InitMessage)
		ApplyNow = NewState;		// Set up the required new state	

	return OldState;				// return old state to the caller
}

/********************************************************************************************

>	BOOL OptionsTabs::GetInitMessageState() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The current ApplInitMessageyNow state as either True or False.
	Purpose:	Finds out whether we are sending/handling an init message or not.
	Errors:		-
	SeeAlso:	SetApplyNowState();

********************************************************************************************/

BOOL OptionsTabs::GetInitMessageState()
{
	return InitMessage;
} 

/********************************************************************************************

>	BOOL OptionsTabs::SetInitMessageState(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Inputs:		New InitMessage state.
	Outputs:	-
	Returns:	The old InitMessage state as either True or False.
	Purpose:	Allows the setting of a new InitMessage state.
	Errors:		-
	SeeAlso:	GetInitMessageState();

********************************************************************************************/

BOOL OptionsTabs::SetInitMessageState(BOOL NewState)
{
	BOOL OldState = InitMessage;	// note current old state
	InitMessage = NewState;			// Set up the required new state	
	
	return OldState;				// return old state to the caller
}


/********************************************************************************************

>	String_256 *OptionsTabs::GetDocumentName() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/95
	Inputs:		-
	Outputs:	-
	Returns:	The name of the selected document.
	Purpose:	Allows the name of the selected document to be found and displayed on the 
				tab.
	Errors:		-
	SeeAlso:	SetDocumentName();

********************************************************************************************/

String_256 *OptionsTabs::GetDocumentName()
{
	return &DocName;
}

/********************************************************************************************

>	static void OptionsTabs::SetDocumentName()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Inputs:		pDoc	a pointer to the name to get the document from, NULL means set default
	Outputs:	-
	Returns:	-
	Purpose:	Allows the name of the selected document to be set and remembered for when
				the tabs ask what the name is so that they can display it correctly.
				Reads the title from the currently selected document and stores this in the
				class variable for later use.
				If the document pointer is null then it just sets the document name to a nice
				default.
	Errors:		-
	SeeAlso:	GetDocumentName();

********************************************************************************************/

void OptionsTabs::SetDocumentName(Document *pDoc)
{
	// Blank the present document name
	DocName.Empty();
	String_256 DocumentName;

	if (pDoc)
	{
		DocumentName = pDoc->GetDocName();
		//DocumentName = pDoc->GetTitle();
	}
	
	if (DocumentName.IsEmpty())
	{
		// No document name found so show a nice default blank type thing
		DocName = String(_R(IDS_K_APPPREF_NODOCNAME));
	}
	else
	{
		// Put the name into quotes
		DocName = String(_R(IDS_K_APPPREF_DOCNAMEQUOTES));
		DocName	+= DocumentName;
		DocName += String(_R(IDS_K_APPPREF_DOCNAMEQUOTES));
	}
}

/********************************************************************************************

>	static void OptionsTabs::SetDocumentName(String_256 *DocumentName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/95
	Inputs:		New name of document to use.
	Outputs:	-
	Returns:	- 
	Purpose:	Allows the name of the selected document to be set and remembered for when
				the tabs ask what the name is so that they can display it correctly.
				Uses the name supplied to set the new stored document name.
	Errors:		-
	SeeAlso:	GetDocumentName();

********************************************************************************************/

void OptionsTabs::SetDocumentName(String_256 *DocumentName)
{
	DocName = *DocumentName;
}


/******************************************************************************************

>	BOOL OptionsTabs::IsPrintingOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a printing option.	  
	Purpose:	Allows the printing options dialog code to determine if this tab belongs
				to the group of printing options.
				This base class version allow the majority of tabs to answer no to this
				question and hence only needs overiding if the tab is a printing one.
	Errors:		-
	SeeAlso:	IsProgramOption(); IsDocumentOption();

******************************************************************************************/

BOOL OptionsTabs::IsPrintingOption()
{
	return FALSE;
}

/********************************************************************************************

>	static void OptionsTabs::SetCurrentDocAndSpread(const Document *pDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		Pointer to the document to use, can be NULL of course.
	Outputs:	-
	Returns:	- 
	Purpose:	Sets up the document and spread that we are currently supposed to be working on.
				This can then be used by the tabs instead of each one using their own method
				to work it out.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OptionsTabs::SetCurrentDocAndSpread(Document *pDoc)
{
	// Note this new document in our class variable.
	pDocument = pDoc;

	// Set up what we consider to be the current spread using the current document
	// class variable. 
	if (pDocument)
		pSpread = pDocument->FindFirstSpread();
	else
		pSpread = NULL;	
}	

/********************************************************************************************

>	virtual BOOL OptionsTabs::PageSizeHasChanged()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		-
	Outputs:	-
	Returns:	True if handled ok, False if not. 
	Purpose:	Allows a tab to update any page size items being displayed. This is the base
				class version which is just here to be overidden.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OptionsTabs::PageSizeHasChanged()
{
	// Sent when the page size has been changed
	// This base class version does nothing
	return TRUE;		
}

/******************************************************************************************
********************************************************************************************/
