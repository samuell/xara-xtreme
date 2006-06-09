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
// LayerPrp.cpp
//
// This file implements the dialogue box that allows the user to set the
// properties of a layer. Brought up from the layer gallery menu item.

/*
*/

#include "camtypes.h"

// WEBSTER - markn 15/1/97
// Don't need the layer properties dlg in Webster
//#ifndef WEBSTER

//#include "barsdlgs.h"	// _R(IDD_BARCONTROLSTORE), _R(IDC_BTN_UNITSETUPDLG) ...
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "dialogop.h"	// DialogOp header - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "optsres.h"	// error strings
//#include "will.h"		// for _R(IDS_CLOSEDOCS)
//#include "resource.h"	// for _R(IDS_CANCEL)

#include "layerprp.h"
//#include "lyrprop.h"	// tabs for the layer properties

// The property tabs themselves
#include "prpslyrs.h"	// Layer properties tab handler
#include "prpsgds.h"	// Guides properties tab handler

//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "sprdmsg.h"	// spread messages
#include "layermsg.h"	// spread messages
#include "colmsg.h"		// more messages
#include "optsmsgs.h"	// & more

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(LayerPropertyTabsDlg, DialogTabOp)
CC_IMPLEMENT_DYNCREATE(LayerPropertiesDlg, LayerPropertyTabsDlg)
CC_IMPLEMENT_DYNCREATE(GuidePropertiesDlg, LayerPropertyTabsDlg)

CC_IMPLEMENT_DYNAMIC(LayerPropertyTabs, PropertyTabs)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

// Set up explicit details about this dialog box such as the id and type
const CDlgResID LayerPropertyTabsDlg::IDD 					= _R(IDD_LAYERPROPERTIES); // Dialog box id
//const CDlgMode LayerPropertyTabsDlg::Mode 				= MODAL;  	//make it modal for now 
const CDlgMode LayerPropertyTabsDlg::Mode 					= MODELESS;	// This dialog is modeless

LayerPropertyTabs * LayerPropertyTabsDlg::pLayerProperties	= NULL;	// pointer to the tab handling class


List 		LayerPropertyTabs::LayerPropertyTabsList;				// The global list of installed options tabs.
DialogTabOp *LayerPropertyTabs::pPropertiesDlg 				= NULL;	// link to application options dialog class

//Layer		*LayerPropertyTabs::pCurrentLayer			 	= NULL; // Layer we are supposed to be working on

//BOOL LayerPropertyTabs::ApplyNow 							= FALSE;// True if Apply now should be ungreyed
//BOOL LayerPropertyTabs::InitMessage 						= FALSE;// True if sending init message

// The following are used to open the specific tab in the options dialog box
UINT32 LayerPropertyTabs::LayersTabNumber				= 0;			// Tab number assigned to the layers tab
UINT32 LayerPropertyTabs::GuidesTabNumber				= 0;			// Tab number assigned to the guides tab

/******************************************************************************************

>	LayerPropertyTabsDlg::LayerPropertyTabsDlg(): DialogTabOp(LayerPropertyTabsDlg::IDD, LayerPropertyTabsDlg::Mode) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerPropertyTabsDlg constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

LayerPropertyTabsDlg::LayerPropertyTabsDlg(): DialogTabOp(LayerPropertyTabsDlg::IDD, LayerPropertyTabsDlg::Mode) 
{
	// At present, just:- 
	if (pLayerProperties)
	{
		pLayerProperties->SetApplyNowState(FALSE);		// grey the apply now button
		pLayerProperties->SetInitMessageState(FALSE);	// not sending an init message


		// Set default units
		pLayerProperties->SetDefaultUnits();				// set up our default display units

		// Document and spread we are supposed to be working on. Of course none at present.
		pLayerProperties->pScopeDocument = NULL;			// Document in use when sending messages
		pLayerProperties->SetCurrentDocAndSpread(NULL);
	}
}

/******************************************************************************************

>	LayerPropertyTabsDlg::LayerPropertyTabsDlg(INT32 StartingPage): DialogTabOp(LayerPropertyTabsDlg::IDD, LayerPropertyTabsDlg::Mode) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerPropertyTabsDlg constructor which allows a starting page to be specified.
				Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

LayerPropertyTabsDlg::LayerPropertyTabsDlg(INT32 StartingPage): DialogTabOp(LayerPropertyTabsDlg::IDD, LayerPropertyTabsDlg::Mode,
														CC_RUNTIME_CLASS(DialogOp), StartingPage) 
{
	// At present, just:- 
	if (pLayerProperties)
	{
		pLayerProperties->SetApplyNowState(FALSE);		// grey the apply now button
		pLayerProperties->SetInitMessageState(FALSE);	// not sending an init message


		// Set default units
		pLayerProperties->SetDefaultUnits();				// set up our default display units

		// Document and spread we are supposed to be working on. Of course none at present.
		pLayerProperties->pScopeDocument = NULL;			// Document in use when sending messages
		pLayerProperties->SetCurrentDocAndSpread(NULL);
	}
}

/******************************************************************************************

>	LayerPropertyTabsDlg::~LayerPropertyTabsDlg()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerPropertyTabsDlg destructor.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

LayerPropertyTabsDlg::~LayerPropertyTabsDlg()
{
	// Make sure that we remove our options tabs link to the dialog box class
	// as the dialog will now be destroyed
	// At present, just:- 
	if (pLayerProperties)
	{
		pLayerProperties->SetTabbedDlg(NULL);
	}
}

/******************************************************************************************

>	BOOL LayerPropertyTabsDlg::CommitDialogValues()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the dialog box and sets the return values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL LayerPropertyTabsDlg::CommitDialogValues()
{
	// Ok has been pressed so take the values from the dialog box
	BOOL ok = TRUE;
	BOOL ValuesOk = TRUE;

	// Make sure that we have set up our class variables so that the tabs know what
	// is the current document and spread to work on.
	Document *pDocument = Document::GetSelected();
	pLayerProperties->SetCurrentDocAndSpread(pDocument);
	// Plus the current document units are correct
	pLayerProperties->SetUpDocUnits();

	// Go through all the tabs declared to the system in turn
	// and tell them to commit the values for their tab
	LayerPropertyTabs *pLayerPropertyTabs = LayerPropertyTabs::GetFirst();

	while (pLayerPropertyTabs != NULL)
	{
		// Only get the tab to commit if it is in use
		if ( pLayerPropertyTabs->IsPropertyRequired() )
		{
			// Get the found page to commit its values.
			// Should return False if reported error and we should just fail.
			// 				 2 if set an error which we should report and then fail. 
			ok = pLayerPropertyTabs->CommitSection();
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
		pLayerPropertyTabs = LayerPropertyTabs::GetNext(pLayerPropertyTabs);
	}

	// Return whether we found bad values or not.
	return ValuesOk;
}

/******************************************************************************************

>	MsgResult LayerPropertyTabsDlg::Message(CDlgMessage DlgMsg, CGadgetID Gadget) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		-
	Outputs:	-
	Returns:	OK	 		  Message handled ok (return this even if you don't need to 
							  respond to the message). 

				FAIL 		  Something terrible happened whilst processing the message
				 			  eg. we ran out of memory. You must set ERROR if you
				 			  are returning this value. 

				EAT_MSG       The Message was handled ok but don't send it to any
				 			  more MessageHandlers. 
	Purpose:	Handles all the layer properties dialog's messages
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

MsgResult LayerPropertyTabsDlg::Message(Msg* Message)
{
	ERROR3IF(Message == NULL,"LayerPropertyTabsDlg::Message null message received");
	ERROR3IF(pLayerProperties == NULL,"LayerPropertyTabsDlg::Message no pLayerProperties pointer");
	
	// Cope with document changing messages so that if all documents are closed then
	// we take some remedial action such as greying all controls. 
	if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *TheMsg = (DocChangingMsg *) Message;

		switch ( TheMsg->State )
		{
//			case DocChangingMsg::TITLECHANGED:
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
				pLayerProperties->SetCurrentDocAndSpread(pDocument);
				// Plus the current document units are correct
				pLayerProperties->SetUpDocUnits();

 				// Is the new one null ==> no new document
				if (pDocument == NULL)
				{
					// Now go through all the tabs declared to the system
					// And call a function to get them to update themselves.
					LayerPropertyTabs *pLayerPropertyTabs = LayerPropertyTabs::GetFirst();

					while (pLayerPropertyTabs != NULL)
					{
						// Only get the tab to update if it is in use
						if ( pLayerPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pLayerPropertyTabs->GreySection();
						}

						// Get the next option's tab.
						pLayerPropertyTabs = LayerPropertyTabs::GetNext(pLayerPropertyTabs);
					}
				}
				else
				{
					// Now go through all the tabs declared to the system
					// And call a function to get them to update themselves.
					LayerPropertyTabs *pLayerPropertyTabs = LayerPropertyTabs::GetFirst();

					while (pLayerPropertyTabs != NULL)
					{
						// Only get the tab to update if it is in use
						if ( pLayerPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pLayerPropertyTabs->UngreySection();
						}
						// Get the next option's tab.
						pLayerPropertyTabs = LayerPropertyTabs::GetNext(pLayerPropertyTabs);
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
					pLayerProperties->SetCurrentDocAndSpread(pDocument);

					LayerPropertyTabs *pLayerPropertyTabs = LayerPropertyTabs::GetFirst();

					while (pLayerPropertyTabs != NULL)
					{
						// Only get the tab to update if it is in use
						if ( pLayerPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pLayerPropertyTabs->UpdateSection();
						}

						// Get the next option's tab.
						pLayerPropertyTabs = LayerPropertyTabs::GetNext(pLayerPropertyTabs);
					}
				}
			}
			break;
			default:
				break;
		}
	}


	// Cope with document view changing messages so that we allow all the tabs to update
	// themselves when switching to a new view.
	// Unfortunately, at present if we sit on the DocView changing message and wait for
	// a selection then at this point there is NO SELECTED spread and so things will fail.
	// Therefore, wait for a selected spread message  
	if (MESSAGE_IS_A(Message, DocViewMsg))
	{
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
					pLayerProperties->SetCurrentDocAndSpread(pDocument);

					LayerPropertyTabs *pLayerPropertyTabs = LayerPropertyTabs::GetFirst();

					while (pLayerPropertyTabs != NULL)
					{
						// Only get the tab to update if it is in use
						if ( pLayerPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pLayerPropertyTabs->UpdateSection();
						}

						// Get the next option's tab.
						pLayerPropertyTabs = LayerPropertyTabs::GetNext(pLayerPropertyTabs);
					}
				}
			}
			break;
			default:
				break;
		}
	}

	// Cope with document view changing messages so that we allow all the tabs to update
	// themselves when switching to a new view.
	// Unfortunately, at present if we sit on the DocView changing message and wait for
	// a selection then at this point there is NO SELECTED spread and so things will fail.
	// Therefore, wait for a selected spread message  
	if (MESSAGE_IS_A(Message, SpreadMsg))
	{
		SpreadMsg *TheMsg = (SpreadMsg *) Message;

		switch ( TheMsg->Reason )
		{
			// The selected spread has changed
			case SpreadMsg::SELCHANGED:
			{
				// pOldSpread = ptr to the old selected spread
				// pNewSpread = ptr to the new selected spread
				if (TheMsg->pNewSpread != NULL)
				{
					// Now go through all the tabs declared to the system
					// And call a function to get them to update themselves.
					// Work out the document name and pass this to the tab so that it has
					// chance to fill in its info field ok.
					Document *pDocument = (Document *)TheMsg->pNewSpread->FindOwnerDoc();
					pLayerProperties->SetCurrentDocAndSpread(pDocument);

					LayerPropertyTabs *pLayerPropertyTabs = LayerPropertyTabs::GetFirst();

					while (pLayerPropertyTabs != NULL)
					{
						// Only get the tab to update if it is in use
						if ( pLayerPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pLayerPropertyTabs->UpdateLayerSection();
						}

						// Get the next option's tab.
						pLayerPropertyTabs = LayerPropertyTabs::GetNext(pLayerPropertyTabs);
					}
				}
			}
			break;

/*			// The layers of a spread have changed.
			case SpreadMsg::LAYERCHANGES:
			{
				// pOldSpread = pNewSpread = spread whose layers have changed
				if (TheMsg->pNewSpread != NULL)
				{
					// Now go through all the tabs declared to the system
					// And call a function to get them to update themselves.
					// Work out the document name and pass this to the tab so that it has
					// chance to fill in its info field ok.
					Document *pDocument = (Document *)TheMsg->pNewSpread->FindOwnerDoc();
					PropertyTabs::SetCurrentDocAndSpread(pDocument);

					LayerPropertyTabs *pLayerPropertyTabs = LayerPropertyTabs::GetFirst();

					while (pLayerPropertyTabs != NULL)
					{
						// Only get the tab to update if it is in use
						if ( pLayerPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pLayerPropertyTabs->UpdateLayerSection();
						}

						// Get the next option's tab.
						pLayerPropertyTabs = LayerPropertyTabs::GetNext(pLayerPropertyTabs);
					}
				}
			}
			break;
*/
			default:
				break;
		}
	}

	// Copes with a layer changing method
	if (MESSAGE_IS_A(Message, LayerMsg))
	{
		LayerMsg *TheMsg = (LayerMsg *) Message;

		switch ( TheMsg->Reason )
		{
			// The active layer has changed.
			case LayerMsg::ACTIVE_LAYER_CHANGED:
			{
				if (TheMsg->pNewLayer != NULL)
				{
					// Now go through all the tabs declared to the system
					// And call a function to get them to update themselves.
					pLayerProperties->SetActiveLayer(TheMsg->pNewLayer);

					LayerPropertyTabs *pLayerPropertyTabs = LayerPropertyTabs::GetFirst();

					while (pLayerPropertyTabs != NULL)
					{
						// Only get the tab to update if it is in use
						if ( pLayerPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pLayerPropertyTabs->UpdateLayerSection();
						}

						// Get the next option's tab.
						pLayerPropertyTabs = LayerPropertyTabs::GetNext(pLayerPropertyTabs);
					}
				}
			}
			break;

			case LayerMsg::GUIDELINES_CHANGED:
			{
				// Only pass on if the layer that's changed is the same as the layer we consider
				// to be the active layer.
				if (TheMsg->pNewLayer == pLayerProperties->GetGuideLayer())
				{
					LayerPropertyTabs *pLayerPropertyTabs = LayerPropertyTabs::GetFirst();

					while (pLayerPropertyTabs != NULL)
					{
						// Only get the tab to update if it is in use
						if ( pLayerPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pLayerPropertyTabs->UpdateGuidelineSection();
						}

						// Get the next option's tab.
						pLayerPropertyTabs = LayerPropertyTabs::GetNext(pLayerPropertyTabs);
					}
				}
			}
			break;
			default:
				break;
		}
	}

	// Cope with a new units messages so that we give the options tabs a chance to
	// update themselves to display using the newly specified defaults.
	if (MESSAGE_IS_A(Message, OptionsChangingMsg))
	{
		OptionsChangingMsg *TheMsg = (OptionsChangingMsg *) Message;

		// Make sure that we have set up our class variables so that the tabs know what
		// is the current document and spread to work on.
		Document *pDocument = Document::GetSelected();
		pLayerProperties->SetCurrentDocAndSpread(pDocument);
		// Plus the current document units are correct
		pLayerProperties->SetUpDocUnits();

		switch ( TheMsg->State )
		{
			case OptionsChangingMsg::NEWUNITS:
			{
				// Current display units may have changed changed so may need to update
				// any display fields on other tabs which are using units. 
				// Now go through all the tabs declared to the system
				// And call a function to get them to update themselves.
				LayerPropertyTabs *pLayerPropertyTabs = LayerPropertyTabs::GetFirst();

				while (pLayerPropertyTabs != NULL)
				{
					// Only get the tab to update if it is in use
					if ( pLayerPropertyTabs->IsPropertyRequired() )
					{
						// Get that tab to update itself.
						pLayerPropertyTabs->NewUnitsInSection();
					}

					// Get the next option's tab.
					pLayerPropertyTabs = LayerPropertyTabs::GetNext(pLayerPropertyTabs);
				}
				break;
			}
			default:
				break;
		}
	}

	
	// Cope with messages for the dialog box
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		MsgResult Result;

		// Main handler for dialog type messages
		BOOL EndDialog = FALSE;						// TRUE if we should quit the dialog
		BOOL CommitValues = FALSE;					// TRUE if we should commit the dialog values
		BOOL OldApplyNow = pLayerProperties->GetApplyNowState();	// Old ApplyNow button state  

		// Make sure that we have set up our class variables so that the tabs know what
		// is the current document and spread to work on.
		Document *pDocument = Document::GetSelected();
		pLayerProperties->SetCurrentDocAndSpread(pDocument);
		// Plus the current document units are correct
		pLayerProperties->SetUpDocUnits();
		
		// Check if sending init/create message and if so set flag True.
		// If True then do not ungrey the ApplyNow button on changing messages
		if (Msg->DlgMsg == DIM_CREATE)
			pLayerProperties->SetInitMessageState(TRUE);				
		
		// If the message is not for the main options window then try the tab pages 
		BOOL FoundPage = FALSE;
		if (Msg->PageID != 0)
		{
			// Now go through all the tabs declared to the system
			// And see if the message is for that page or not.
			LayerPropertyTabs *pLayerPropertyTabs = LayerPropertyTabs::GetFirst();

			while (pLayerPropertyTabs != NULL && !FoundPage)
			{
				CDlgResID PageId = pLayerPropertyTabs->GetPageID();
				if (PageId == Msg->PageID)
				{
					// Found the correct page so stop the search and send the message
					// to that page
					pLayerPropertyTabs->HandleMsg(Msg);
					FoundPage = TRUE;
				}
				// Get the next option's tab.
				pLayerPropertyTabs = LayerPropertyTabs::GetNext(pLayerPropertyTabs);
			}
		}
		else
		{
			// Main page window
			switch (Msg->DlgMsg)
			{
//				case DIM_CREATE:		// Creating the main dialog box
					// grey out the apply now button as this should only become enabled	when 
					// we have made a change to the dialog box.
//					pLayerProperties->SetInitMessageState(FALSE);
//					pLayerProperties->SetApplyNowState(FALSE);
//					pLayerProperties->GreyApplyNow();
//					break;
				
				case DIM_COMMIT:		// Want to commit and quit
					EndDialog = TRUE;
					CommitValues = TRUE;
					break;

				case DIM_SOFT_COMMIT:	// Want to commit
					CommitValues = TRUE;
					break;

				case DIM_CANCEL:		// Want to quit
					EndDialog = TRUE;
					break;

				default:
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

		if ( !OldApplyNow && pLayerProperties->GetApplyNowState()
			 && !EndDialog && !pLayerProperties->GetInitMessageState() )
		{
			// Check for a change from apply now being False to it becoming
			// True as an item has been changed.
			pLayerProperties->UngreyApplyNow();
		}

		if ( pLayerProperties->GetApplyNowState() && (Msg->DlgMsg == DIM_SOFT_COMMIT)
			 && !EndDialog && !pLayerProperties->GetInitMessageState() )
		{
			// Check for an apply now (DIM_SOFT_COMMIT) being used and the
			// apply being True, should always be the case,	so that we can reset
			// the grey state to False as we are in an unchanged state.
			pLayerProperties->SetApplyNowState(FALSE);
			pLayerProperties->GreyApplyNow();
		}

		// Allow the base class access to the message, it will do the
		// DLG_EAT_IF_HUNGRY(Msg) for us

		// Must do this before the Close and End
		Result = DialogTabOp::Message(Message);

		// End dialog here
		if (EndDialog) 
		{
			// Make sure that we remove our options tabs link to the dialog box class
			// as the dialog will now be destroyed
			pLayerProperties->SetTabbedDlg(NULL);
		}
		
		// Check if have been sending an init/create message and if so then set flag False.
		// Only do this in the init/create case as we might be sent one of these and then
		// be sent other messages whilst in the middle of the init/create message such as
		// text changing messages.
		if (Msg->DlgMsg == DIM_CREATE)
			pLayerProperties->SetInitMessageState(FALSE);				
		
		// The message was for our dialog box so return that we have handled it, if necessary
		//return (DLG_EAT_IF_HUNGRY(Msg)); 
		return Result;
	}

	if (MESSAGE_IS_A(Message, ColourChangingMsg))
	{
		ColourChangingMsg *Msg = (ColourChangingMsg *) Message;
		BOOL CallColourListChanged = FALSE;
		Document* pDoc = NULL;

		switch ( Msg->State )
		{
			case ColourChangingMsg::LISTDESELECTED:
				pDoc = NULL;
				CallColourListChanged = TRUE;
				break;

			case ColourChangingMsg::LISTPAGED:
			case ColourChangingMsg::LISTUPDATED:
			case ColourChangingMsg::COLOURUPDATED:
			case ColourChangingMsg::COLOURUPDATEDINVISIBLE:
				pDoc = Msg->ScopeDoc;
				CallColourListChanged = TRUE;
				break;
			default:
				break;
		}

		// Call the ColourListChanged() func is the colour list has changed
		if (CallColourListChanged)
		{
			LayerPropertyTabs *pLayerPropertyTabs = LayerPropertyTabs::GetFirst();

			while (pLayerPropertyTabs != NULL)
			{
				// Only get the tab to update if it is in use
				if ( pLayerPropertyTabs->IsPropertyRequired() )
					pLayerPropertyTabs->ColourListChanged(pDoc);

				// Get the next option's tab.
				pLayerPropertyTabs = LayerPropertyTabs::GetNext(pLayerPropertyTabs);
			}
		}
	}

	return DialogTabOp::Message(Message);
}

/******************************************************************************************

>	OpState	LayerPropertyTabsDlg::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerPropertyTabsDlg GetState method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

OpState	LayerPropertyTabsDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;

	// If the pointer is non-null then it implies that another dialog box is up which
	// uses our tab set and so we must force greying of the menu option 
	if (pLayerProperties && pLayerProperties->GetTabbedDlg() != NULL)
		OpSt.Greyed = TRUE;

	return(OpSt);
}




/******************************************************************************************

>	BOOL LayerPropertyTabsDlg::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerPropertyTabsDlg Init method. Called when the kernel initialises.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL LayerPropertyTabsDlg::Init()
{
	BOOL InitOK;

	// Now register ourselves with the dialog system
	InitOK = RegisterOpDescriptor(
								0,								// Tool ID 
								_R(IDS_LAYERPROPERTYTABDLG),		// String resource ID
								CC_RUNTIME_CLASS(LayerPropertyTabsDlg),	// Runtime class
								OPTOKEN_LAYERPROPERTYTABS,		// Token string
								GetState,						// GetState function
								0,								// help ID
								_R(IDBBL_LAYERPROPERTYTAB),			// bubble help
								0, //_R(IDD_BARCONTROLSTORE),			// resource ID
								0, //_R(IDC_BTN_OPTIONSDLG),				// control ID
								SYSTEMBAR_ILLEGAL,				// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								0,								// No vertical counterpart
								_R(IDS_LAYERPROPERTYTABDLG_ONE),	// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);

	// Now register ourselves with the dialog system for the units button on the windows
	// button bar as well
	InitOK = InitOK && RegisterOpDescriptor(
								0,								// Tool ID
								_R(IDS_LAYERPROPERTIESDLG),			// String resource ID
								CC_RUNTIME_CLASS(LayerPropertiesDlg),	// Runtime class
								OPTOKEN_LAYERPROPERTIESDLG,		// Token string
								GetState,						// GetState function
								0,								// help ID
								_R(IDBBL_LAYERPROPERTIESDLG),		// bubble help
								0, //_R(IDD_BARCONTROLSTORE),			// resource ID
								0, //_R(IDC_BTN_UNITSETUPDLG),			// control ID
								SYSTEMBAR_ILLEGAL,				// Bar ID
								TRUE, 							// Receive messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// No vertical counterpart
								_R(IDS_LAYERPROPERTIESDLG_ONE),		// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags								);
								);

	// Now register ourselves with the dialog system for the units button on the windows
	// button bar as well
	InitOK = InitOK && RegisterOpDescriptor(
								0,								// Tool ID
								_R(IDS_GUIDEPROPERTIESDLG),			// String resource ID
								CC_RUNTIME_CLASS(GuidePropertiesDlg),	// Runtime class
								OPTOKEN_GUIDEPROPERTIESDLG,		// Token string
								GetState,						// GetState function
								0,								// help ID
								_R(IDBBL_GUIDEPROPERTIESDLG),		// bubble help
								0, //_R(IDD_BARCONTROLSTORE),			// resource ID
								0, //_R(IDC_BTN_UNITSETUPDLG),			// control ID
								SYSTEMBAR_ILLEGAL,				// Bar ID
								TRUE, 							// Receive messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// No vertical counterpart
								_R(IDS_GUIDEPROPERTIESDLG_ONE),		// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags								);
								);

	// Set up any layer property tabs that are required and get our link to them
	InitOK = InitOK && LayerPropertyTabs::InitLayerPropertyTabs();	
	pLayerProperties = LayerPropertyTabs::GetFirst();

	return (InitOK);
}


/******************************************************************************************

>	void LayerPropertyTabsDlg::Deinit()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerPropertyTabsDlg Deinit method. Called when the kernel deinitialises.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void LayerPropertyTabsDlg::Deinit()
{
	// Remove any options tabs that have been declared to the system.
	LayerPropertyTabs::DeinitLayerPropertyTabs();	
	pLayerProperties = NULL;
}



/******************************************************************************************

>	void LayerPropertyTabsDlg::Do(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void LayerPropertyTabsDlg::Do(OpDescriptor*)
{
	BOOL ok;

//	if (LayerPropertyTabs::pPropertiesDlg != NULL)
//	{
//		ERROR3("LayerPropertyTabsDlg::Do an options tabbed dialog box is already present");
//		return;
//	}

	if (pLayerProperties == NULL)
	{
		// Could not create the dialog box so call inform error 
		ERROR3("LayerPropertyTabsDlg::Do no LayerProperty present");
		//InformError();						// Show user the error
  		End(); 		   						// End the operation 
	}

	// Set up the name that the main dialog page is going to use
	String_256 Temp(_R(IDN_LAYERPROPERTYTAB)); 
	TitleString = Temp;
	SetName(&TitleString);
	
	// Make sure that our options tabs have a link to the dialog box class
	LayerPropertyTabs::SetTabbedDlg(this);
	pLayerProperties->SetApplyNowState(FALSE);

	// Make sure that we have set up our class variables so that the tabs know what
	// is the current document and spread to work on.
	Document *pDocument = Document::GetSelected();
	pLayerProperties->SetCurrentDocAndSpread(pDocument);
		
	// Force the dialog box to be created, as it is non-modal it must be opened if the create
	// works ok.
	ok = Create();							// create the dialog box

	if ( ok )
	{
		// Modal at present so remove the Open()
		Open();								// created ok so open the dialog box up
		
		// grey out the apply now button as this should only become enabled when we have 
		// made a change to the dialog box.
		pLayerProperties->SetInitMessageState(FALSE);
		pLayerProperties->SetApplyNowState(FALSE);
		pLayerProperties->GreyApplyNow();
	}
	else
	{
		// Could not create the dialog box so call inform error 
		InformError();						// Show user the error
  		End(); 		   						// End the operation 
	}				   	
}

/******************************************************************************************

>	BOOL LayerPropertyTabsDlg::RegisterYourPagesInOrderPlease()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
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

BOOL LayerPropertyTabsDlg::RegisterYourPagesInOrderPlease()
{
	BOOL ok = TRUE;

	// Now go through all the tabs declared to the system
	LayerPropertyTabs *pLayerPropertyTabs = LayerPropertyTabs::GetFirst();

	while (pLayerPropertyTabs != NULL)
	{
		// Only add the tab if it is a program based option
		if ( pLayerPropertyTabs->IsPropertyRequired() )
		{
			CDlgResID PageId = pLayerPropertyTabs->GetPageID();
			ok = ok && AddAPage(PageId);
		}

		// Get the next option's tab.
		pLayerPropertyTabs = LayerPropertyTabs::GetNext(pLayerPropertyTabs);
	}

	return (ok); 
}

/******************************************************************************************

>	static LayerPropertyTabs * LayerPropertyTabsDlg::GetLayerPropertiesTabs(); 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/12/95
	Inputs:		-
	Outputs:	-
	Returns:	Pointer to the layer property tabs handling class, if any.
	Purpose:	Allows somebody to get a Pointer to the layer property tabs handling class. 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

LayerPropertyTabs * LayerPropertyTabsDlg::GetLayerPropertiesTabs()
{
	return pLayerProperties;
}


/******************************************************************************************

>	LayerPropertiesDlg::LayerPropertiesDlg(): LayerPropertyTabsDlg(1) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerPropertiesDlg constructor. Creates a non-undoable operation.
				Just starts up the LayerPropertyTabsDlg specifying the tab number that it is in the
				list. This has been calculated when the options tabs were declared just to
				be on the safe side.
				Must use a number rather than the ID as this is what MFC's property sheet
				constructors require. 
	Errors:		-
	SeeAlso:	GuidePropertiesDlg;

******************************************************************************************/

LayerPropertiesDlg::LayerPropertiesDlg(): LayerPropertyTabsDlg( LayerPropertyTabs::LayersTabNumber ) 
{
	// At present, just:- do nothing
}

/******************************************************************************************

>	GuidePropertiesDlg::GuidePropertiesDlg(): LayerPropertyTabsDlg(1) 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GuidePropertiesDlg constructor. Creates a non-undoable operation.
				Just starts up the LayerPropertyTabsDlg specifying the tab number that it is in the
				list. This has been calculated when the options tabs were declared just to
				be on the safe side.
				Must use a number rather than the ID as this is what MFC's property sheet
				constructors require. 
	Errors:		-
	SeeAlso:	LayerPropertiesDlg;

******************************************************************************************/

GuidePropertiesDlg::GuidePropertiesDlg(): LayerPropertyTabsDlg( LayerPropertyTabs::GuidesTabNumber ) 
{
	// At present, just:- do nothing
}









/******************************************************************************************
******************************************************************************************/

// Code to handle a list of options tabs available to the system

/********************************************************************************************

>	LayerPropertyTabs::LayerPropertyTabs()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Purpose:	Constructor.
	Errors:
	SeeAlso:	LayerPropertyTabsDlg;

********************************************************************************************/

LayerPropertyTabs::LayerPropertyTabs()
{
	// Ensure our link to the application options dialog box is set to a good default.
	pPropertiesDlg = NULL;

	ApplyNow = FALSE;
	InitMessage = FALSE;
}


/********************************************************************************************

>	LayerPropertyTabs::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Purpose:	Initialiser.
	Errors:
	SeeAlso:	LayerPropertyTabsDlg;

********************************************************************************************/

BOOL LayerPropertyTabs::Init()
{
	// Do nothing for now.
	return TRUE;
}


/******************************************************************************************
******************************************************************************************/

#define ADD_LAYERPROPERTY(Classname) 												\
	/* Create, initialise and install the specified tab to the options system */	\
	pLayerProperty = new Classname;													\
	if (pLayerProperty == NULL) return FALSE;										\
																					\
	if (!pLayerProperty->Init())													\
	{																				\
		/* Error occured - report it and stop trying to initialise options tab. */	\
		delete pLayerProperty;														\
		InformError();																\
		return TRUE;																\
	}																				\
																					\
	LayerPropertyTabsList.AddTail(pLayerProperty);

/******************************************************************************************
******************************************************************************************/

/********************************************************************************************

>	static BOOL LayerPropertyTabs::InitLayerPropertyTabs()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Returns:	TRUE if initialised ok, FALSE if not.
	Purpose:	Scans all LayerPropertyTabs to find out which LayerPropertyTabs are available
				for use, and instantiates one of each, and holds them in a list.
	Errors:		Out of memory (if a LayerProperty fails to initialise for other reasons then
				an error is reported, but Camelot will still start up ok, i.e. TRUE is 
				returned).
	SeeAlso:	LayerProperty

********************************************************************************************/

BOOL LayerPropertyTabs::InitLayerPropertyTabs()
{
	// Find the LayerPropertyTabs - the kernel ones are hard-wired.
	LayerPropertyTabs *pLayerProperty = NULL;

	// Create, initialise and install the built in LayerPropertyTabs list
	ADD_LAYERPROPERTY(LayerPropertiesTab)	// Layer
#ifndef WEBSTER	
	ADD_LAYERPROPERTY(GuidesPropertiesTab)	// Guide lines
#endif // WEBSTER

	// We must work out what the units tab number is when used in document mode so
	// that we can skip to it directly when using the units button on the tool bar.
	INT32 i = 0;

	// Now go through all the tabs declared to the system
	pLayerProperty = LayerPropertyTabs::GetFirst();

	while (pLayerProperty != NULL)
	{
		// Only include the tab if it is a document based option
		if ( pLayerProperty->IsPropertyRequired() )
		{
			if (pLayerProperty->GetPageID() == _R(IDD_TAB_LAYER_PROPERTIES))
			{
				LayersTabNumber = i;		// note that tab number
			}
			else if (pLayerProperty->GetPageID() == _R(IDD_TAB_GUIDELINE_PROPERTIES))
			{
				GuidesTabNumber = i;		// note that tab number
			}
			
			i += 1;		// increment the counter 
		}

		// Get the next option's tab.
		pLayerProperty = LayerPropertyTabs::GetNext(pLayerProperty);
	}

	// All ok
	return TRUE;
}

/********************************************************************************************

>	static BOOL LayerPropertyTabs::DeinitLayerPropertyTabs()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Purpose:	Destroy all the LayerProperty objects.
	SeeAlso:	LayerProperty

********************************************************************************************/

BOOL LayerPropertyTabs::DeinitLayerPropertyTabs()
{
//	LayerPropertyTabs *pLayerProperty = NULL;

	// Now, we can get rid of our LayerPropertyTabs
	LayerPropertyTabsList.DeleteAll();

	// All ok
	return TRUE;
}

/********************************************************************************************

>	static BOOL LayerPropertyTabs::DeclareLayerProperty(LayerPropertyTabs *pLayerProperty)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		A pointer to a new LayerPropertyTabs class already declared into the system
	Returns:	True if tab declared, False if something went wrong. 
	Purpose:	A function to declare a new tab to the system.
	SeeAlso:	LayerProperty

********************************************************************************************/

BOOL LayerPropertyTabs::DeclareLayerProperty(LayerPropertyTabs *pLayerProperty)
{
	// First check to see if somebody has actually declared the pointer to be non null.
	if (pLayerProperty == NULL) return FALSE;

	if (!pLayerProperty->Init())
	{
		/* Error occured - report it and stop trying to initialise the options tab. */
		InformError();
		return TRUE;
	}
	LayerPropertyTabsList.AddTail(pLayerProperty);
	
	// All ok
	return TRUE;
}

/********************************************************************************************

>	virtual DialogTabOp * LayerPropertyTabs::GetTabbedDlg()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Returns:	Pointer to the main dialog box used by this LayerPropertyTabs, maybe NULL
				if none.
	Purpose:	Let the base class know what/where the dialog box handling class is.
	SeeAlso:	LayerPropertyTabs::GreyApplyNow(); LayerPropertyTabs::UngreyApplyNow();
	Scope:		virtual

********************************************************************************************/

DialogTabOp * LayerPropertyTabs::GetTabbedDlg()
{
	// returns a pointer to the main dialog box used by this code
	return LayerPropertyTabs::pPropertiesDlg;
}

/********************************************************************************************

>	stataic BOOL LayerPropertyTabs::SetTabbedDlg(DialogTabOp * pDlg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Returns:	Pointer to the main dialog box used by this LayerPropertyTabs, maybe NULL
				if none.
	Purpose:	Let the base class know what/where the dialog box handling class is.
	SeeAlso:	LayerPropertyTabs::GreyApplyNow(); LayerPropertyTabs::UngreyApplyNow();
	Scope:		Static

********************************************************************************************/

BOOL LayerPropertyTabs::SetTabbedDlg(DialogTabOp * pDlg)
{
	// returns a pointer to the main dialog box used by this code
	pPropertiesDlg = pDlg;
	
	return TRUE;
}

/********************************************************************************************

>	static LayerPropertyTabs *LayerPropertyTabs::GetFirst()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Returns:	Pointer to the first LayerProperty, or NULL if none.
	Purpose:	Iterating through the list of LayerPropertyTabs known to Camelot.  This function
				returns the first LayerPropertyTabs.
	SeeAlso:	LayerPropertyTabs::GetNext()
	Scope:		Static

********************************************************************************************/

LayerPropertyTabs *LayerPropertyTabs::GetFirst()
{
	return (LayerPropertyTabs *) LayerPropertyTabsList.GetHead();
}

/********************************************************************************************

>	static LayerPropertyTabs *LayerPropertyTabs::GetNext(LayerPropertyTabs *pLayerPropertyTabs)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		pLayerPropertyTabs - pointer to the LayerPropertyTabs to be used to get the following
				LayerPropertyTabs.
	Returns:	Pointer to the next LayerPropertyTabs, or NULL if none.
	Purpose:	Iterating through the list of LayerPropertyTabs known to Camelot.  This function
				returns the next LayerPropertyTabs.
	SeeAlso:	LayerPropertyTabs::GetFirst

********************************************************************************************/

LayerPropertyTabs *LayerPropertyTabs::GetNext(LayerPropertyTabs *pLayerPropertyTabs)
{
	return (LayerPropertyTabs *) LayerPropertyTabsList.GetNext(pLayerPropertyTabs);
}


/********************************************************************************************

>	virtual BOOL LayerPropertyTabs::GetApplyNowState() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		-
	Outputs:	-
	Returns:	The current ApplyNow state as either True or False.
	Purpose:	Finds out the current state of the apply now button.
	Errors:		-
	SeeAlso:	SetApplyNowState();

********************************************************************************************/

BOOL LayerPropertyTabs::GetApplyNowState()
{
	return ApplyNow;
} 

/********************************************************************************************

>	virtual BOOL LayerPropertyTabs::SetApplyNowState(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		New ApplyNow flag state.
	Outputs:	-
	Returns:	The old ApplyNow state as either True or False.
	Purpose:	Allows the setting of a new ApplyNow state.
	Errors:		-
	SeeAlso:	GetApplyNowState();

********************************************************************************************/

BOOL LayerPropertyTabs::SetApplyNowState(BOOL NewState)
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

>	virtual BOOL LayerPropertyTabs::GetInitMessageState() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		-
	Outputs:	-
	Returns:	The current ApplInitMessageyNow state as either True or False.
	Purpose:	Finds out whether we are sending/handling an init message or not.
	Errors:		-
	SeeAlso:	SetApplyNowState();

********************************************************************************************/

BOOL LayerPropertyTabs::GetInitMessageState()
{
	return InitMessage;
} 

/********************************************************************************************

>	virtual BOOL LayerPropertyTabs::SetInitMessageState(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Inputs:		New InitMessage state.
	Outputs:	-
	Returns:	The old InitMessage state as either True or False.
	Purpose:	Allows the setting of a new InitMessage state.
	Errors:		-
	SeeAlso:	GetInitMessageState();

********************************************************************************************/

BOOL LayerPropertyTabs::SetInitMessageState(BOOL NewState)
{
	BOOL OldState = InitMessage;	// note current old state
	InitMessage = NewState;			// Set up the required new state	
	
	return OldState;				// return old state to the caller
}

/******************************************************************************************

>	CDlgResID LayerPropertyTabs::GetPageID()()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/12/95
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
				Baseclass version, needs overriding by the tab handlers.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CDlgResID LayerPropertyTabs::GetPageID()
{
	return 0;
}

/******************************************************************************************

>	BOOL LayerPropertyTabs::CommitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/12/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in this tab of the layer properties dialog box.
				Called when ok or apply now is pressed on the main dialog box.
				Baseclass version, needs overriding by the tab handlers.
	Errors:		-
	SeeAlso:	LayerPropertiesTab::InitSection()

******************************************************************************************/

BOOL LayerPropertyTabs::CommitSection()
{
	return TRUE;
}

/********************************************************************************************

>	BOOL LayerPropertyTabs::HandleMsg(DialogMsg* Msg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/12/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the messages for this layer property tab
				Baseclass version, needs overriding by the tab handlers.
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg; LayerPropertiesTab;

********************************************************************************************/

BOOL LayerPropertyTabs::HandleMsg(DialogMsg* Msg)
{
	return TRUE;
}  
/******************************************************************************************

>	BOOL LayerPropertyTabs::InitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/12/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for this tab on the layer properties dialog box.
				Baseclass version, needs overriding by the tab handlers.
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg; LayerPropertiesTab;

******************************************************************************************/

BOOL LayerPropertyTabs::InitSection()
{
	return TRUE;
}  

/******************************************************************************************
********************************************************************************************/

//#endif // WEBSTER
