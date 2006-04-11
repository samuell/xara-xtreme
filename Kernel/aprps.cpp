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

#include "camtypes.h"

//#include "barsdlgs.h"	
#include "document.h"
#include "docview.h"
#include "dialogop.h"	
//#include "optsres.h"	
//#include "will.h"		
//#include "resource.h"	
#include "spread.h"
#include "sprdmsg.h"	
#include "optsmsgs.h"	
#include "layer.h"
//#include "nev.h"		
#include "sglayer.h"
#include "fixmem.h"
//#include "markn.h"
//#include "jason.h"
#include "layermsg.h"
#include "aprps.h"
//#include "gifdlg.h"
#include "app.h"

//#include "will3.h"
#include "prevwdlg.h"
//#include "simon.h"
//#include "bmpsdlgr.h"
//#include "ollie.h"
//#include "filtrres.h"
#include "filedlgs.h"
//#include "prevwres.h" 

CC_IMPLEMENT_DYNCREATE	(GIFAnimationPropertyTabsDlg,	DialogTabOp)
CC_IMPLEMENT_DYNCREATE	(FramePropertiesTab,			GIFAnimationPropertyTabs)
CC_IMPLEMENT_DYNCREATE	(AnimationPropertiesTab,		GIFAnimationPropertyTabs)   
CC_IMPLEMENT_DYNCREATE	(AnimationColoursTab,			GIFAnimationPropertyTabs)   
CC_IMPLEMENT_DYNCREATE	(PreviewInBrowserTab,			GIFAnimationPropertyTabs)   
CC_IMPLEMENT_DYNCREATE	(GIFAnimationPropertyTabs,		PropertyTabs)
CC_IMPLEMENT_DYNCREATE	(AnimExOptns,					GIFAnimationPropertyTabsDlg)
CC_IMPLEMENT_DYNAMIC	(BrowserPrvwChgdMsg,			Msg)

#define new CAM_DEBUG_NEW

const	CDlgResID	GIFAnimationPropertyTabsDlg::IDD	= _R(IDD_PROPERTIES);		// Dialog box id
		CDlgMode	GIFAnimationPropertyTabsDlg::Mode	= MODELESS;				// This dialog is modeless

		const CDlgResID	AnimExOptns::IDD = _R(IDD_OPTIONS_PROPERTIES);		 		// Dialog box id
		CDlgMode	AnimExOptns::Mode	 = MODAL;								// If the dialog is invoked through the options button on the export dialog,			
																				// then this dialog is modeless.				

GIFAnimationPropertyTabs	*GIFAnimationPropertyTabsDlg::m_pGIFAnimationProperties		 = NULL;	// pointer to the tab handling class
GIFAnimationPropertyTabsDlg *GIFAnimationPropertyTabsDlg::m_pGIFAnimationPropertyTabsDlg = NULL; 


List 			GIFAnimationPropertyTabs::GIFAnimationPropertyTabsList;				// The global list of installed options tabs.
DialogTabOp*	GIFAnimationPropertyTabs::pPropertiesDlg				= NULL;		// link to application options dialog class
UINT32			GIFAnimationPropertyTabs::m_Count						= 0;		// The number of instances of this dialog.
BOOL			GIFAnimationPropertyTabs::m_IsManyDisplayed				= FALSE;	// Is many displayed in the Delay edit field.
BOOL			GIFAnimationPropertyTabs::m_IncludeFramePropertiesTab	= TRUE;		// Should the frame properties tab be displayed.		
BOOL			GIFAnimationPropertyTabs::m_BrowserPreviewTab			= FALSE;	// This flag allows us to know when to exclude all other tabs.

// The following are used to open the specific tab in the options dialog box
UINT32 GIFAnimationPropertyTabs::FramePropertiesTabNumber		= 0;					// Tab number assigned to the frame properties tab.
UINT32 GIFAnimationPropertyTabs::AnimationColoursTabNumber	= 1;					// Tab number assigned to the animation colours tab.	
UINT32 GIFAnimationPropertyTabs::AnimationPropertiesTabNumber	= 2;					// Tab number assigned to the animation properties tab.
UINT32 GIFAnimationPropertyTabs::PreviewInBrowserTabNumber	= 3;					// Tab number assigned to the preview in browser tab.
INT32  GIFAnimationPropertyTabsDlg::m_PageToOpen = -1;								// The tab page to open at start up.
BOOL GIFAnimationPropertyTabs::m_CurrentApplyNow = FALSE;
BOOL GIFAnimationPropertyTabs::m_ApplyNow = FALSE;
BOOL GIFAnimationPropertyTabs::m_InitMessage = FALSE;	
BOOL GIFAnimationPropertyTabs::m_FrameGalleryOpen = TRUE;
BOOL GIFAnimationPropertyTabs::m_ChangeLayerState = FALSE;
BOOL GIFAnimationPropertyTabs::m_FrameDelayValueChanged	= FALSE;

//BrowserPreviewOptions	PreviewInBrowserTab::t_BrowserOptions;
BOOL					PreviewInBrowserTab::g_InfoInHtmlStub	= TRUE;
BOOL					PreviewInBrowserTab::g_Imagemap			= TRUE;
BrowserBackground		PreviewInBrowserTab::g_Background		= BROWSER_BGR_NONE;


/*****************************************************************************************************************************************************

>	GIFAnimationPropertyTabsDlg::GIFAnimationPropertyTabsDlg(): DialogTabOp(GIFAnimationPropertyTabsDlg::IDD, GIFAnimationPropertyTabsDlg::Mode) 

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GIFAnimationPropertyTabsDlg default constructor. 
	
********************************************************************************************************************************************************/

GIFAnimationPropertyTabsDlg::GIFAnimationPropertyTabsDlg(): DialogTabOp(GIFAnimationPropertyTabsDlg::IDD, GIFAnimationPropertyTabsDlg::Mode,
																		CC_RUNTIME_CLASS(DialogOp), GIFAnimationPropertyTabsDlg::GetPageToOpen())
{
	if (m_pGIFAnimationProperties)
	{
		m_pGIFAnimationProperties->SetApplyNowState(FALSE);		// grey the apply now button
		m_pGIFAnimationProperties->SetInitMessageState(FALSE);	// not sending an init message

		// The Document and spread we are supposed to be working on. 
		m_pGIFAnimationProperties->pScopeDocument = NULL;			
		m_pGIFAnimationProperties->SetCurrentDocAndSpread(NULL);		

		//Increment the count, as another instance of the dialog has been created.
		UINT32 Val=1;
		m_pGIFAnimationProperties->IncCount(Val);

		// Create a static link to this dialog class.
		m_pGIFAnimationPropertyTabsDlg = this;
	}
}


/*****************************************************************************************************************************************************

>	GIFAnimationPropertyTabsDlg::GIFAnimationPropertyTabsDlg(): DialogTabOp(GIFAnimationPropertyTabsDlg::IDD, GIFAnimationPropertyTabsDlg::Mode) 

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GIFAnimationPropertyTabsDlg constructor. 
				This allows us to specify a new dialog resource id and a different mode. 
				This constructor is overloaded since if the dialog is invoked through the options button 
				on the GIF Export options dialog, then we need to specify a different resouce ID and make the dialog MODAL.
	
********************************************************************************************************************************************************/

GIFAnimationPropertyTabsDlg::GIFAnimationPropertyTabsDlg(CDlgResID DialogResID, CDlgMode Mode): DialogTabOp(DialogResID, Mode,
																		CC_RUNTIME_CLASS(DialogOp), GIFAnimationPropertyTabsDlg::GetPageToOpen())
{
	if (m_pGIFAnimationProperties)
	{
		m_pGIFAnimationProperties->SetApplyNowState(FALSE);		// grey the apply now button
		m_pGIFAnimationProperties->SetInitMessageState(FALSE);	// not sending an init message

		// The Document and spread we are supposed to be working on. 
		m_pGIFAnimationProperties->pScopeDocument = NULL;			
		m_pGIFAnimationProperties->SetCurrentDocAndSpread(NULL);		

		//Increment the count, as another instance of the dialog has been created.
		UINT32 Val=1;
		m_pGIFAnimationProperties->IncCount(Val);

		// Create a static link to this dialog class.
		m_pGIFAnimationPropertyTabsDlg = this;
	}
}

/******************************************************************************************

>	GIFAnimationPropertyTabsDlg::~GIFAnimationPropertyTabsDlg()
	
	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	returns:	- 
	Purpose:	GIFAnimationPropertyTabsDlg destructor.

******************************************************************************************/

GIFAnimationPropertyTabsDlg::~GIFAnimationPropertyTabsDlg()
{
	// Ensure we have a valid ptr.
	if (m_pGIFAnimationProperties)
	{
		// Remove the link to the options tabs if all the dialogs have been closed. 
		if(m_pGIFAnimationProperties->GetCount() < 1)
		{
			// Remove the options tabs link to the dialog box class.
			m_pGIFAnimationProperties->SetTabbedDlg(NULL);
		}

		// A dialog has been destroyed, therefore decrement m_count.
		UINT32 Count =1;
		m_pGIFAnimationProperties->DecCount(Count);

		// Kill the static link to this dialog class.
		m_pGIFAnimationPropertyTabsDlg = NULL;

		// Set this flag to FALSE.
		m_pGIFAnimationProperties->SetCurrentApplyNow(FALSE);
	}
}

/********************************************************************************************

>	BOOL GIFAnimationPropertyTabsDlg::CommitDialogValues()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the dialog box and sets the return values accordingly
				Called when ok is pressed on the dialog box.

*********************************************************************************************/

BOOL GIFAnimationPropertyTabsDlg::CommitDialogValues()
{
	// Ok has been pressed so take the values from the dialog box.
	BOOL ok = TRUE;
	BOOL ValuesOk = TRUE;

	// Make sure we work on the correct document and spread.
	Document *pDocument = Document::GetSelected();
	m_pGIFAnimationProperties->SetCurrentDocAndSpread(pDocument);

	// Tell all the tabs to commit their values.
	GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetFirst();

	// Continue while we have a valid tab ptr.
	while (pGIFAnimationPropertyTabs != NULL)
	{
		// Only get the tab to commit if it is in use.
		if ( pGIFAnimationPropertyTabs->IsPropertyRequired() )
		{
			// Get the found page to commit its values.
			ok = pGIFAnimationPropertyTabs->CommitSection();
			if ( ok != TRUE)
			{
				if (ok == 2)
					InformError();		

				// Carry on with other pages.
				ValuesOk = FALSE;	
			}
			// Set the apply now flag to FALSE.
			pGIFAnimationPropertyTabs->SetApplyNowState(FALSE);

			// Grey yhe apply now/ok/Cancel buttons.
			pGIFAnimationPropertyTabs->GreyApplyNow();	
		}
		// Get the next option's tab.
		pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetNext(pGIFAnimationPropertyTabs);
	}

	// Return whether we found bad values or not.
	return ValuesOk;
}

/*********************************************************************************************************************

>	MsgResult GIFAnimationPropertyTabsDlg::Message(Msg* Message)
	
	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	OK	 		  Message handled ok.
				FAIL 		  Something terrible happened whilst processing the message.
				EAT_MSG       The Message was handled ok but don't send it to any more MessageHandlers. 
		
*************************************************************************************************************************/

MsgResult GIFAnimationPropertyTabsDlg::Message(Msg* Message)
{
	ERROR3IF(Message == NULL,"GIFAnimationPropertyTabsDlg::Message NULL message received");
	ERROR3IF(m_pGIFAnimationProperties == NULL,"GIFAnimationPropertyTabsDlg::Message no m_pGIFAnimationProperties ptr");
	
	// Cope with document changing messages. 
	if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *TheMsg = (DocChangingMsg *) Message;

		switch ( TheMsg->State )
		{
			case DocChangingMsg::DocState::SELCHANGED:
			{
				// In different cases we use different document pointers.
				Document *pDocument = NULL;
				if (TheMsg->State == DocChangingMsg::DocState::TITLECHANGED)
				{
					// Document title has changed message.
					pDocument = TheMsg->pChangingDoc;
				}
				else
				{
					// Document selection changed.
					pDocument = TheMsg->pNewDoc;
				}
							
				// Make sure that the tabs know what is the current document and spread to work on.
				m_pGIFAnimationProperties->SetCurrentDocAndSpread(pDocument);

				// Ensure correct current document units.
				m_pGIFAnimationProperties->SetUpDocUnits();

 				// Is there a new document.
				if (pDocument == NULL)
				{
					// Make sure all the tabs update themselves.
					GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetFirst();

					while (pGIFAnimationPropertyTabs != NULL)
					{
						// Only get the tab to update if it is in use.
						if ( pGIFAnimationPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pGIFAnimationPropertyTabs->GreySection();
						}

						// Get the next option's tab.
						pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetNext(pGIFAnimationPropertyTabs);
					}
				}
				else
				{
					// Make sure all the tabs update themselves.
					GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetFirst();

					while (pGIFAnimationPropertyTabs != NULL)
					{
						// Only get the tab to update if it is currently in use.
						if ( pGIFAnimationPropertyTabs->IsPropertyRequired() )
						{
							// Get this tab to update itself.
							pGIFAnimationPropertyTabs->UngreySection();
						}
						// Get the next option's tab.
						pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetNext(pGIFAnimationPropertyTabs);
					}
				}
			}
			break;
			
			// This message is sent when a new or just opened document is stable.
			case DocChangingMsg::DocState::BORNANDSTABLE:
			{
				// Get the changing document pointer.
				Document *pDocument = NULL;
				pDocument = TheMsg->pChangingDoc;
				if (pDocument != NULL)
				{
					// Ensure all the tabs update themsleves.
					m_pGIFAnimationProperties->SetCurrentDocAndSpread(pDocument);

					GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetFirst();

					while (pGIFAnimationPropertyTabs != NULL)
					{
						// Only get the tab to update if it is in use
						if ( pGIFAnimationPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pGIFAnimationPropertyTabs->UpdateSection();
						}

						// Get the next options tab.
						pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetNext(pGIFAnimationPropertyTabs);
					}
				}
			}
			break;
		}
	}

	// Cope with document view changing messages so that we allow all the tabs to update themselves when switching to a new view.
	if (MESSAGE_IS_A(Message, DocViewMsg))
	{
		DocViewMsg *TheMsg = (DocViewMsg *) Message;

		switch ( TheMsg->State )
		{
			case DocViewMsg::DocViewState::SELCHANGED:
			{
				if (TheMsg->pNewDocView != NULL)
				{
					// Ensure all the tabs update themsleves.
					Document *pDocument = TheMsg->pNewDocView->GetDoc();
					m_pGIFAnimationProperties->SetCurrentDocAndSpread(pDocument);

					GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetFirst();

					while (pGIFAnimationPropertyTabs != NULL)
					{
						// Only get the tab to update if it is in use.
						if ( pGIFAnimationPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pGIFAnimationPropertyTabs->UpdateSection();
						}

						// Get the next option's tab.
						pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetNext(pGIFAnimationPropertyTabs);
					}
				}
			}
			break;
		}
	}

	// Cope with document view changing messages so that we allow all the tabs to update themselves when switching to a new view.
	if (MESSAGE_IS_A(Message, SpreadMsg))
	{
		SpreadMsg *TheMsg = (SpreadMsg *) Message;

		switch ( TheMsg->Reason )
		{
			// The selected spread has changed
			case SpreadMsg::SpreadReason::SELCHANGED:
			{
				if (TheMsg->pNewSpread != NULL)
				{
					// Ensure all the tabs update themselves.
					Document *pDocument = (Document *)TheMsg->pNewSpread->FindOwnerDoc();
					m_pGIFAnimationProperties->SetCurrentDocAndSpread(pDocument);

					GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetFirst();

					while (pGIFAnimationPropertyTabs != NULL)
					{
						// Only get the tab to update if it is in use.
						if ( pGIFAnimationPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pGIFAnimationPropertyTabs->UpdateLayerSection();
						}

						// Get the next option's tab.
						pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetNext(pGIFAnimationPropertyTabs);
					}
				}
			}
			break;

			case SpreadMsg::SpreadReason::ANIMATIONPROPERTIESCHANGED:
			{
				GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetFirst();

					while (pGIFAnimationPropertyTabs != NULL)
					{
						// Only get the tab to update if it is in use.
						if ( pGIFAnimationPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pGIFAnimationPropertyTabs->UpdateLayerSection();
						}
						// Get the next option's tab.
						pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetNext(pGIFAnimationPropertyTabs);
					}
			}
			break;
 		}
	}

	// Handle a layer changing message.
	if (MESSAGE_IS_A(Message, LayerMsg))
	{
		LayerMsg *TheMsg = (LayerMsg *) Message;

		switch ( TheMsg->Reason )
		{
			// The active layer has changed.
			case LayerMsg::LayerReason::ACTIVE_LAYER_CHANGED:
			{
				if (TheMsg->pNewLayer != NULL)
				{
					// Ensure all the tabs update themsleves.
					m_pGIFAnimationProperties->SetActiveLayer(TheMsg->pNewLayer);

					// While Loop flag.
					BOOL FoundPage = FALSE;

					GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetFirst();

					while (pGIFAnimationPropertyTabs != NULL)
					{
						//  Get the page Id.
						CDlgResID PageId = pGIFAnimationPropertyTabs->GetPageID();

						// For Layer messages we only update the frame properties tab.
						if (PageId == _R(IDD_TAB_FRAME_PROPERTIES) || PageId == _R(IDD_TAB_ANIMATION_PROPERTIES))
						{
							// Only get the tab to update if it is in use.
							if ( pGIFAnimationPropertyTabs->IsPropertyRequired() )
							{
								// Get that tab to update itself.
								pGIFAnimationPropertyTabs->UpdateLayerSection();

								// Exit the while loop 
							//	FoundPage = TRUE;
							}
						}
						// Get the next option's tab.
						pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetNext(pGIFAnimationPropertyTabs);
					}
				}
			}
			break;

			// The active layer has been updated.
			case LayerMsg::LayerReason::UPDATE_ACTIVE_LAYER:
			{
				// While Loop flag.
				BOOL FoundPage = FALSE;

				// Ensure all the tabs update themsleves.
				m_pGIFAnimationProperties->SetActiveLayer(TheMsg->pNewLayer);

				GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetFirst();

				while (pGIFAnimationPropertyTabs != NULL && !FoundPage)
				{
					//  Get the page Id.
					CDlgResID PageId = pGIFAnimationPropertyTabs->GetPageID();

					// For Layer messages we only update the frame properties tab.
					if (PageId == _R(IDD_TAB_FRAME_PROPERTIES))
					{
						// Only get the tab to update if it is in use.
						if ( pGIFAnimationPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pGIFAnimationPropertyTabs->UpdateLayerSection();

							// Exit the loop.
							FoundPage = TRUE;
						}
					}
					// Get the next option's tab.
					pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetNext(pGIFAnimationPropertyTabs);
				}				
			}
		}
	}

	// The Browser Preview options have changed.
	if (MESSAGE_IS_A(Message, BrowserPrvwChgdMsg))
	{
		BrowserPrvwChgdMsg *TheMsg = (BrowserPrvwChgdMsg *) Message;

		switch ( TheMsg->State )
		{
			case BrowserPrvwChgdMsg::BrowserPrvwPropState::PROPERTIES_CHANGED:
			{
				// The properties have changed, so update the Browser preview tab.

				// While Loop flag.
				BOOL FoundPage = FALSE;

				// Get a ptr to the first tab.
				GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetFirst();

				// Loop untill we get aptr to the Preview in browser tab.
				while (pGIFAnimationPropertyTabs != NULL && !FoundPage)
				{
					//  Get the page Id.
					CDlgResID PageId = pGIFAnimationPropertyTabs->GetPageID();

					// For bitmap preview properties changed messages we only update the Browser preview tab.
					if (PageId == _R(IDD_TAB_PREVIEW_IN_BROWSER))
					{
						// Only get the tab to update if it is in use.
						if ( pGIFAnimationPropertyTabs->IsPropertyRequired() )
						{
							// Get that tab to update itself.
							pGIFAnimationPropertyTabs->UpdateLayerSection();

							// Exit the while loop 
							FoundPage = TRUE;
						}
					}
					// Get the next option's tab.
					pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetNext(pGIFAnimationPropertyTabs);
				}
			
			}
		}
	}

	// Handle messages for the dialog box.
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		MsgResult Result;

		// Main handler for dialog type messages
		BOOL EndDialog = FALSE;											// TRUE if we should quit the dialog
		BOOL CommitValues = FALSE;										// TRUE if we should commit the dialog values
		BOOL OldApplyNow = m_pGIFAnimationProperties->GetApplyNowState();	// Old ApplyNow button state  

		// Make sure that the tabs know what is the current document and spread to work on.
		Document *pDocument = Document::GetSelected();
		m_pGIFAnimationProperties->SetCurrentDocAndSpread(pDocument);

		// Make sure that the current document units are correct.
		m_pGIFAnimationProperties->SetUpDocUnits();
			
		// Check if sending init/create message and if so set flag True.
		// If True then do not ungrey the ApplyNow button on changing messages
		if (Msg->DlgMsg == DIM_CREATE)
			m_pGIFAnimationProperties->SetInitMessageState(TRUE);				

		// If the message is not for the main winndow then try the tab pages.
		BOOL FoundPage = FALSE;

		if (Msg->PageID != 0)
		{
			//Go through all the tabs declared to the system and see if the message is for that page.
			GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetFirst();

			while (pGIFAnimationPropertyTabs != NULL&& !FoundPage)
			{
				CDlgResID PageId = pGIFAnimationPropertyTabs->GetPageID();
				if (PageId == Msg->PageID)
				{
					// Found the correct page so stop the search and send the message to that page
					pGIFAnimationPropertyTabs->HandleMsg(Msg);
					FoundPage = TRUE;
				}
				// Get the next tab.
				pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetNext(pGIFAnimationPropertyTabs);							
			}
		}	
		else
		{
			// Main page window
			switch (Msg->DlgMsg)
			{
				case DIM_CREATE:
					if (Msg->DlgMsg == DIM_CREATE)					
					{
						//  When the dialog first starts up, only the first tab is initialised. When you change a value
						//  on this tab and press ok/apply, this causes problems.
						//  So, make sure that all the tabs are created
						GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetFirst();

						while (pGIFAnimationPropertyTabs != NULL)
						{
							//  Send the message to the page
							pGIFAnimationPropertyTabs->HandleMsg(Msg);
						
							//  Get the next tab.
							pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetNext(pGIFAnimationPropertyTabs);
						}
					}
					break;

				case DIM_COMMIT:		
					EndDialog = TRUE;
					CommitValues = TRUE;
				break;

				case DIM_SOFT_COMMIT:	
				{
					CommitValues = TRUE;
					m_pGIFAnimationProperties->SetApplyNowState(TRUE);
				}
				break;

				case DIM_CANCEL:		
				{
					EndDialog = TRUE;
				}
				break;
				}
			}

		// Commit values here.
		if (CommitValues)
		{
			BOOL ok = CommitDialogValues();
			if (!ok)
			{
				EndDialog = FALSE;	
			}
		}

		// Pass the message back to the Base class.
		Result = DialogTabOp::Message(Message);

		// End dialog here
		if (EndDialog) 
		{
			Close();		
			End();			

			// Remove the link to the options tabs if all dialogs have been cloosed. 
			if(m_pGIFAnimationProperties->GetCount() < 1)
			{
				// Make sure that we remove our options tabs link to the dialog box class as the dialog will now be destroyed.
				m_pGIFAnimationProperties->SetTabbedDlg(NULL);
			}
		}
		
		// Check if we have been sending an init/create message, if so then set the flag False.
		if (Msg->DlgMsg == DIM_CREATE)
			m_pGIFAnimationProperties->SetInitMessageState(FALSE);				
					
		return Result;
	}
	return DialogTabOp::Message(Message);
}

/***************************************************************************************************************

  >	OpState	GIFAnimationPropertyTabsDlg::GetState(String_256*, OpDescriptor*)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GIFAnimationPropertyTabsDlg GetState method.

****************************************************************************************************************/

OpState	GIFAnimationPropertyTabsDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;

	// If the pointer is non-null grey the menu option. 
	if (m_pGIFAnimationProperties && m_pGIFAnimationProperties->GetTabbedDlg() != NULL)
		OpSt.Greyed = TRUE;
	
	return(OpSt);
}

/******************************************************************************************

>	BOOL GIFAnimationPropertyTabsDlg::Init()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
******************************************************************************************/

BOOL GIFAnimationPropertyTabsDlg::Init()
{
	BOOL InitOK;

	// Register ourselves with the dialog system.
	InitOK = RegisterOpDescriptor(
								0,															// Tool ID 																
								_R(IDS_ANIMATION_TAB_DLG),										// String resource ID
								CC_RUNTIME_CLASS(GIFAnimationPropertyTabsDlg),				// Runtime class
								OPTOKEN_GIFANIMPROPERTYTABS,								// Token string
								GetState,													// GetState function
								0,															// help ID
								0,															// bubble help
								0, //_R(IDD_BARCONTROLSTORE),									// resource ID
								0, //_R(IDC_BTN_OPTIONSDLG),									// control ID
								SYSTEMBAR_ILLEGAL,											// Bar ID
								TRUE,														// Recieve system messages
								FALSE,														// Smart duplicate operation
								TRUE,														// Clean operation
								0,															// No vertical counterpart
								_R(IDS_ANIMATION_TAB_DLG_ONE),									// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC)	// Auto state flags
								);

	// Set up any animation property tabs that are required and get a link to them.
	InitOK = InitOK && GIFAnimationPropertyTabs::InitGIFAnimationPropertyTabs();	
	m_pGIFAnimationProperties = GIFAnimationPropertyTabs::GetFirst();

	return (InitOK);
}


/******************************************************************************************

>	void GIFAnimationPropertyTabsDlg::Deinit()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
******************************************************************************************/

void GIFAnimationPropertyTabsDlg::Deinit()
{
	// Remove all options tabs which have been declared to the system.
	GIFAnimationPropertyTabs::DeinitGIFAnimationPropertyTabs();	
	m_pGIFAnimationProperties = NULL;
}


/******************************************************************************************

>	void GIFAnimationPropertyTabsDlg::Do(OpDescriptor*)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user.
	
******************************************************************************************/

void GIFAnimationPropertyTabsDlg::Do(OpDescriptor*)
{
	BOOL ok = FALSE;

	if (m_pGIFAnimationProperties == NULL)
	{
		// Could not create the dialog box.
		ERROR3("GIFAnimationPropertyTabsDlg::Do no AnimationProperties present");
		End();
	}

	//Set up the name that the main dialog page will use.
	String_256 Temp(_R(IDS_ANIMATION_TAB_DLG)); 
	TitleString = Temp;
	SetName(&TitleString);
	
	// Make sure that our options tabs have a link to the dialog box class
	GIFAnimationPropertyTabs::SetTabbedDlg(this);
//	m_pGIFAnimationProperties->SetApplyNowState(TRUE);

	// Force the dialog box to be created, as it is non-modal it must be opened if the create works ok.
	ok = Create();		

	// Make sure we know the current document and spread to work on.
	Document *pDocument = Document::GetSelected();
	m_pGIFAnimationProperties->SetCurrentDocAndSpread(pDocument);

	if ( ok )
	{
		Open();	

		// Grey out the apply now button. 
		m_pGIFAnimationProperties->SetInitMessageState(FALSE);
		m_pGIFAnimationProperties->SetApplyNowState(FALSE);
		m_pGIFAnimationProperties->GreyApplyNow();		
	}
	else
	{
		// Something went wrong!
		InformError();				
  		End(); 		   				
	}				   	
}

/******************************************************************************************

>	BOOL GIFAnimationPropertyTabsDlg::RegisterYourPagesInOrderPlease()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	return TRUE if all calls to AddAPage returned TRUE, else FALSE
	Purpose:	This function will get called to give us  a chance to add our 
				pages to the DialogTabOp. 

******************************************************************************************/

BOOL GIFAnimationPropertyTabsDlg::RegisterYourPagesInOrderPlease()
{
	BOOL ok = TRUE;

	// Get the first tab.
	GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetFirst();

	while (pGIFAnimationPropertyTabs != NULL)
	{
		if ( pGIFAnimationPropertyTabs->IsPropertyRequired() )
		{
			CDlgResID PageId = pGIFAnimationPropertyTabs->GetPageID();
			ok = ok && AddAPage(PageId);
		}

		// Get next.
		pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetNext(pGIFAnimationPropertyTabs);
	}

	return (ok); 
}

/******************************************************************************************

>	GIFAnimationPropertyTabs * GIFAnimationPropertyTabsDlg::GetLayerPropertiesTabs()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	Pointer to the animation property tabs handling class.
	
******************************************************************************************/

GIFAnimationPropertyTabs * GIFAnimationPropertyTabsDlg::GetGIFAnimationPropertiesTabs()
{
	return m_pGIFAnimationProperties;
}

/********************************************************************************************

>	GIFAnimationPropertyTabs::GIFAnimationPropertyTabs()
	
	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Purpose:	Constructor.

********************************************************************************************/

GIFAnimationPropertyTabs::GIFAnimationPropertyTabs()
{
	pPropertiesDlg				= NULL;
	m_NoFrameLayers				= FALSE;	
	m_IgnoreTextChangeMsg		= FALSE;
}


/********************************************************************************************

>	GIFAnimationPropertyTabs::Init()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	
********************************************************************************************/

GIFAnimationPropertyTabs::Init()
{
	// Do nothing for now.
	return TRUE;
}

/******************************************************************************************/

/******************************************************************************************/

#define ADD_LAYERPROPERTY(Classname) 												\
	/* Create, initialise and install the specified tab to the options system */	\
	pGIFAnimationProperty = new Classname;											\
	if (pGIFAnimationProperty == NULL) return FALSE;								\
																					\
	if (!pGIFAnimationProperty->Init())												\
	{																				\
		/* Error occured - report it and stop trying to initialise options tab. */	\
		delete pGIFAnimationProperty;												\
		InformError();																\
		return TRUE;																\
	}																				\
																					\
	GIFAnimationPropertyTabsList.AddTail(pGIFAnimationProperty);

/********************************************************************************************

>	BOOL GIFAnimationPropertyTabs::InitGIFAnimationPropertyTabs()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Returns:	TRUE if initialised ok, FALSE if not.
	Purpose:	Scans all GIFAnimationPropertyTabs to find out which Tabs are available,
				creates and holds them in a list.

********************************************************************************************/

BOOL GIFAnimationPropertyTabs::InitGIFAnimationPropertyTabs()
{
	//Find the Tabs.
	GIFAnimationPropertyTabs *pGIFAnimationProperty = NULL;

	ADD_LAYERPROPERTY(FramePropertiesTab)			// Frame Properties tab
	ADD_LAYERPROPERTY(AnimationColoursTab)			// Animation colours tab.
	ADD_LAYERPROPERTY(AnimationPropertiesTab)		// Animation properties tab					  
	ADD_LAYERPROPERTY(PreviewInBrowserTab)			// Preview in Browser tab.
/*	
	// Loop variable.
	INT32 i = 0;

	// Go through the tabs declared to the system
	pGIFAnimationProperty = GIFAnimationPropertyTabs::GetFirst();

	while (pGIFAnimationProperty != NULL)
	{
		// Include the tab only if it is a document based option.
		if ( pGIFAnimationProperty->IsPropertyRequired() )
		{
			switch (pGIFAnimationProperty->GetPageID())
			{
				case _R(IDD_TAB_FRAME_PROPERTIES):
					FramePropertiesTabNumber = i;
					break;
				
				case _R(IDD_TAB_ANIMATION_PROPERTIES):
					 AnimationPropertiesTabNumber = i;
					break;						
					
				case _R(IDD_TAB_ANIMATION_COLOURS):
					 AnimationColoursTabNumber = i;
					break;					
					
				case _R(IDD_TAB_PREVIEW_IN_BROWSER):
					 PreviewInBrowserTabNumber = i;
					break;					
		}	
			i++ ;
		}
		// Get the next tab.
		pGIFAnimationProperty = GIFAnimationPropertyTabs::GetNext(pGIFAnimationProperty);
	}
*/
	return TRUE;
}	

/********************************************************************************************

>	static void GIFAnimationPropertyTabs::DeinitGIFAnimationPropertyTabs()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Purpose:	Destroy all the Tab objects.
	
********************************************************************************************/

BOOL GIFAnimationPropertyTabs::DeinitGIFAnimationPropertyTabs()
{
	GIFAnimationPropertyTabs *pGIFAnimationProperty = NULL;

	// Remove all the Tabs.
	GIFAnimationPropertyTabsList.DeleteAll();

	return TRUE;
}

/********************************************************************************************

>	BOOL GIFAnimationPropertyTabs::DeclareGIFAnimationProperty(GIFAnimationPropertyTabs *pGIFAnimationProperty)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		A pointer to a new GIFAnimationPropertyTabs class.
	Returns:	True if tab declared, False if something went wrong. 
	Purpose:	Function to declare a new tab.

********************************************************************************************/

BOOL GIFAnimationPropertyTabs::DeclareGIFAnimationProperty(GIFAnimationPropertyTabs *pGIFAnimationProperty)
{
	if (pGIFAnimationProperty == NULL) return FALSE;

	if (!pGIFAnimationProperty->Init())
	{
		InformError();
		return TRUE;
	}
	GIFAnimationPropertyTabsList.AddTail(pGIFAnimationProperty);
	
	return TRUE;
}

/********************************************************************************************

>	virtual DialogTabOp * GIFAnimationPropertyTabs::GetTabbedDlg()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Returns:	Pointer to the main dialog box used by this GIFAnimationPropertyTabs.		
	Purpose:	Let the base class know where the dialog box handling class is.
	
********************************************************************************************/

DialogTabOp * GIFAnimationPropertyTabs::GetTabbedDlg()
{
	// return a pointer to the main dialog box.
	return GIFAnimationPropertyTabs::pPropertiesDlg;
}

/********************************************************************************************

>	stataic BOOL GIFAnimationPropertyTabs::SetTabbedDlg(DialogTabOp * pDlg)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Returns:	Pointer to the main dialog box used by GIFAnimationPropertyTabs.
	
********************************************************************************************/

BOOL GIFAnimationPropertyTabs::SetTabbedDlg(DialogTabOp * pDlg)
{
	// Return a pointer to the main dialog box.
	pPropertiesDlg = pDlg;
	
	return TRUE;
}

/********************************************************************************************

>	static GIFAnimationPropertyTabs *GIFAnimationPropertyTabs::GetFirst()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Returns:	Pointer to the first Tab, or NULL if none.

********************************************************************************************/

GIFAnimationPropertyTabs *GIFAnimationPropertyTabs::GetFirst()
{
	return (GIFAnimationPropertyTabs *) GIFAnimationPropertyTabsList.GetHead();
}

/**************************************************************************************************************************

>	static GIFAnimationPropertyTabs *GIFAnimationPropertyTabs::GetNext(GIFAnimationropertyTabs *pGIFAnimationPropertyTabs)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		pGIFAnimationPropertyTabs - pointer to the GIFAnimationPropertyTabs to be used to get the following
				GIFAnimationPropertyTabs.
	Returns:	Pointer to the next GIFAnimationPropertyTabs.

***************************************************************************************************************************/

GIFAnimationPropertyTabs *GIFAnimationPropertyTabs::GetNext(GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs)
{
	return (GIFAnimationPropertyTabs *) GIFAnimationPropertyTabsList.GetNext(pGIFAnimationPropertyTabs);
}


/********************************************************************************************

>	virtual BOOL GIFAnimationPropertyTabs::GetApplyNowState() const

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	The current ApplyNow state, either True or False.

********************************************************************************************/

BOOL GIFAnimationPropertyTabs::GetApplyNowState()
{
	return m_ApplyNow;
} 

/********************************************************************************************

>	virtual BOOL GIFAnimationPropertyTabs::SetApplyNowState(BOOL NewState)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		NewState:- A new ApplyNow flag state.
	Outputs:	-
	Returns:	The old ApplyNow state, either True or False.
	Purpose:	Allows the setting of a new ApplyNow state.
	
********************************************************************************************/

BOOL GIFAnimationPropertyTabs::SetApplyNowState(BOOL NewState)
{
	//BOOL OldState = m_ApplyNow;		// Note current old state.

	if (!m_InitMessage)
		m_ApplyNow = NewState;		// Set up the required new state.	

	//return OldState;				// return old state. 
	return m_ApplyNow;
}

/********************************************************************************************

>	BOOL GIFAnimationPropertyTabs::GetInitMessageState() const

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	The current ApplInitMessageyNow state as either True or False.
	
********************************************************************************************/

BOOL GIFAnimationPropertyTabs::GetInitMessageState()
{
	return m_InitMessage;
} 

/********************************************************************************************

>	virtual BOOL GIFAnimationPropertyTabs::SetInitMessageState(BOOL NewState)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		New InitMessage state.
	Outputs:	-
	Returns:	The old InitMessage state as either True or False.
	Purpose:	Allows the setting of a new InitMessage state.
	
********************************************************************************************/

BOOL GIFAnimationPropertyTabs::SetInitMessageState(BOOL NewState)
{
	BOOL OldState = m_InitMessage;		// Note current old state.

	m_InitMessage = NewState;			// Set up the required new state.
	
	return OldState;					// Return the old state.
}

/******************************************************************************************

>	CDlgResID GIFAnimationPropertyTabs::GetPageID()()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  

******************************************************************************************/

CDlgResID GIFAnimationPropertyTabs::GetPageID()
{
	return NULL;
}

/******************************************************************************************

>	BOOL GIFAnimationPropertyTabs::CommitSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
								
******************************************************************************************/

BOOL GIFAnimationPropertyTabs::CommitSection()
{
	return TRUE;
}

/********************************************************************************************

>	BOOL GIFAnimationPropertyTabs::HandleMsg(DialogMsg* Msg)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the messages for this property tab.
				
********************************************************************************************/

BOOL GIFAnimationPropertyTabs::HandleMsg(DialogMsg* Msg)
{
	return TRUE;
}  

/******************************************************************************************

>	BOOL GIFAnimationPropertyTabs::InitSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE.
		
******************************************************************************************/

BOOL GIFAnimationPropertyTabs::InitSection()
{
	return TRUE;
}  


/******************************************************************************************

>	BOOL AnimationPropertiesTabs::SameflDelayValues()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all the frame layers have the same delay value, else FALSE.
	
******************************************************************************************/

BOOL GIFAnimationPropertyTabs::SameflDelayValues()
{
	BOOL NumOfLayers = 0;			// The number of frame layers in the current sperad.
	INT32 i = 0;						// loop variable.
	BOOL SameDelayValues = TRUE;	// Flag to indicate whether all the frame layer delay values are the same.
	Layer* pLayer = NULL;			// Ptr to the frame/active layer.
	m_NoFrameLayers = FALSE;	
	
	// Get a ptr to the first frame layer.
	pLayer = pSpread->FindFirstFrameLayer();

	// Check for a NULL ptr.
	if(!pLayer)
	{
		// If this is a new default document, then a NULL frame layer will be returned.
		// In that case get a ptr to the first layer.
		pLayer = pSpread->FindFirstLayer();

		// Again, check for a NULL ptr.
		if(!pLayer)
			return FALSE;
		else
			m_NoFrameLayers = TRUE;		// At this moment there are no frame layers.
	}

	// How many frame layers do we have.
	while (pLayer)
	{
		// How many layers do we have?
		if(m_NoFrameLayers)
		{
			pLayer = pLayer->FindNextLayer();
			NumOfLayers++;
		}
		else
		{
		 	// How many frame layers do we have?
			pLayer = pLayer->FindNextFrameLayer();

			NumOfLayers++;
		}
	}
	
	// If there is a single layer, then return TRUE, since then there is only a single delay value.
	if(NumOfLayers == 1)
		return TRUE;

	// Create a temporary buffer to store all the layer delay values.
	DWORD *pDelay = new DWORD[NumOfLayers];

	// Error check.
	if(!pDelay)
		return FALSE;	// Failed to allocate the memory.
	
	// If there are frame layers, get the first frame layer.
	if(!m_NoFrameLayers)
		pLayer = pSpread->FindFirstFrameLayer();
	else
		pLayer = pSpread->FindFirstLayer();

	// Error check.
	if(!pLayer)
		return FALSE;	// Failed to allocate the memory.

	// In the buffer make a note of all the frame layer delay values.
	while (pLayer)
	{
		// Get the delay value for this layer.
		pDelay[i++] = pLayer->GetFrameDelay();

		// If there are frame layers, get the next frame layer.
		if(!m_NoFrameLayers)
			pLayer = pLayer->FindNextFrameLayer();
		else
			pLayer = pLayer->FindNextLayer();
	}

	// Compare all the frame layer delay values.
	for(i=0; i<NumOfLayers; i++)
	{
		for(INT32 n=0; n<NumOfLayers; n++)
		{
			if (pDelay[i] != pDelay[n])
			{
				// If the delay values differ, set this flag to false.
				SameDelayValues = FALSE;		
			}
		}
	}
	// Delete the buffer.
	delete [] pDelay;
	
	// Return the flag value.
	return SameDelayValues;
}





				/********************************************************************************/
				/**********************										*********************/
				/**********************		Class:-	AnimationColoursTab		*********************/
				/**********************										*********************/
				/********************************************************************************/	






/********************************************************************************************

>	AnimationColoursTab::AnimationColoursTab() 


	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-	
	Outputs:	-
	Returns:	-
	Purpose:	AnimationColoursTab constructor.
	
********************************************************************************************/

AnimationColoursTab::AnimationColoursTab()
{   
	// Does nothing at present.
}   	     

/********************************************************************************************

>	AnimationColoursTab::~AnimationColoursTab()


	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	AnimationColoursTab destructor.

********************************************************************************************/
																				
AnimationColoursTab::~AnimationColoursTab()
{   
	// Does nothing at present.
}        

/********************************************************************************************

>	BOOL AnimationColoursTab::Init()


	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	
********************************************************************************************/
																				
BOOL AnimationColoursTab::Init()
{   
	return TRUE;
}        

/******************************************************************************************

>	CDlgResID AnimationColoursTab::GetPageID()()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	
******************************************************************************************/

CDlgResID AnimationColoursTab::GetPageID()
{
	return _R(IDD_TAB_ANIMATION_COLOURS);
}

/******************************************************************************************

>	BOOL AnimationColoursTab::IsPropertyRequired()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Returns:	Returns True if this tab is a program option.	  
	
******************************************************************************************/

BOOL AnimationColoursTab::IsPropertyRequired()
{
	//  If the dialog has been invoked via the options button on the Export dialog box,
	//  then hide this tab.
	if (GetBrowserPreviewTab())
		return FALSE;
	else
		return TRUE;	// This tab is required 	
}


/******************************************************************************************

>	BOOL AnimationColoursTab::CommitSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Animation colours tab. 
				Called when ok or apply now is pressed on the main dialog box.

******************************************************************************************/

BOOL AnimationColoursTab::CommitSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"animationColoursTab::CommitSection called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;			

	// Have any animation colour properties changed?
	if (pSpread && HavePropertiesChanged())
	{
		PALETTE_COLOURS PalCols;			// Browser/Optimized palette.
		WEB_PALETTE		WebPal;				// Global/Local palette.	
		DITHER			Dither;				// Current dither type.
		DWORD			CurNumColsInPal;	// The current number of colours in the palette.
		BOOL			Transparency;	// Whether or not we're transparent.

		// Find the specified Palette type.
		if (pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAME_GLOBALBROWSER)))
		{
			PalCols	=	PALCOL_BROWSER;
			WebPal	=	PAL_GLOBAL;		
		}
		else if (pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAME_GLOBALOPTIMISED)))
		{
			WebPal	= PAL_GLOBAL;
			PalCols	= PALCOL_OPTIMIZED;		
		}
		else if(pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAME_LOCALOPT)))
		{
			WebPal	= PAL_LOCAL;
			PalCols	= PALCOL_OPTIMIZED;		
		}

		// Find the specified Dither type.
		if (pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAME_DIFFUSION)))
			Dither = XARADITHER_ERROR_DIFFUSION;

		else if (pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAME_ORDDITHER)))
			Dither = XARADITHER_ORDERED_GREY;
		
		else if (pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAME_NODITHER)))
			Dither = XARADITHER_NONE;

		// Find the specified number of colours in the palette.
		BOOL Valid = FALSE;
		CurNumColsInPal = pPropertiesDlg->GetLongGadgetValue(_R(IDC_FRAME_NUMCOLOURS), 2, 256, 0,&Valid);

		// If invalid input, display error message.
		if (!Valid)
		{
			String_256 ErrMsg;
			ErrMsg.MakeMsg(_R(IDS_BMPPREFS_NUMCOLSWRONG),2,256);
			Error::SetError(0,ErrMsg,0);
			InformError();

			return FALSE;
		}

		// Find out whether we're transparent or not.
		Transparency = pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAME_TRANSPARENCY));

		// Check that all visible layers are actually frame layers
		FrameSGallery::EnsureFrameLayerIntegrity(pSpread);

		// Set the new animation colour prefterences in the spread.
		pSpread->SetAnimationColours( Dither, WebPal, PalCols, CurNumColsInPal, Transparency);

		// Mark all the frame layers as edited.
		Layer* pLayer = pSpread->FindFirstFrameLayer();

		while (pLayer != NULL)
		{
			// Mark this layer as edited.
			pLayer->SetEdited(TRUE);

			// Move to the next frame layer in the spread.
			pLayer = pLayer->FindNextFrameLayer();
		}				

		// Mark the document as edited.
		// Get a ptr to the current document.
		Document* pDoc = Document::GetCurrent();

		// Set this document as edited.
		if(pDoc)
			pDoc->SetModified(TRUE);
		
		// Is the animation playing.
		if (PreviewDialog::GetPreviewDialog())
		{
			// Stop the animation and regenerate our frames.
			//PreviewDialog::SetAnimationPropertiesChanged(TRUE);

			// Invoke the op.
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FRAME_GRABALLFRAMES); 
			if (pOpDesc != NULL)
				pOpDesc->Invoke();
			else
			{
				ERROR3("Couldn't find OPTOKEN_FRAME_GRABALLFRAMES op descriptor");
			}
		}
	}

	// Everything ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL AnimationColoursTab::GreySection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents.

********************************************************************************************/

BOOL AnimationColoursTab::GreySection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"AnimationcoloursTab::GreySection called with no dialog pointer");

	//Ensure we are talking to the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;

	// Only update if we are not already grey. 
	if (m_GreyStatus == TRUE)
		return TRUE;

	// Set the correct state of the controls.
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_GLOBALBROWSER),	FALSE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_GLOBALOPTIMISED),	FALSE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_LOCALOPT),		FALSE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_NUMCOLOURS),		FALSE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_NODITHER),		FALSE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_ORDDITHER),		FALSE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_DIFFUSION),		FALSE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_TRANSPARENCY),	FALSE);

	// Call the central greying/ungreying function
	ok = ChangeControlStatus(FALSE);	
	
	// Set the correct grey status
	m_GreyStatus = TRUE;
	SetApplyNowState(FALSE);
	
	// Everything ok.
	return ok;
} 

/********************************************************************************************

>	BOOL AnimationColoursTab::UngreySection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document.

********************************************************************************************/

BOOL AnimationColoursTab::UngreySection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"AnimationColoursTab::UngreySection called with a NULL dialog pointer");

	// Ensure we are talking to the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;

	// Only update if we are not already ungrey. 
	if (m_GreyStatus == FALSE)
		return TRUE;

	// Set the correct state of the controls.
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_GLOBALBROWSER),	TRUE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_GLOBALOPTIMISED),	TRUE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_LOCALOPT),		TRUE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_NUMCOLOURS),		TRUE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_NODITHER),		TRUE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_ORDDITHER),		TRUE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_DIFFUSION),		TRUE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAME_TRANSPARENCY),	TRUE);

	// Call the central greying/ungreying function
	ok = ChangeControlStatus(TRUE);	
	
	m_GreyStatus = FALSE;

	return ok;
}

/********************************************************************************************

>	BOOL AnimationColoursTab::ChangeControlStatus(const BOOL Status)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 

********************************************************************************************/


BOOL AnimationColoursTab::ChangeControlStatus(const BOOL Status)
{
	
	// Grey the apply now/ok/cancel buttons.
	AnimationColoursTab::GreyApplyNow();
	
	return TRUE;
}			


/********************************************************************************************

>	BOOL AnimationColoursTab::UpdateSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. 
	
********************************************************************************************/

BOOL AnimationColoursTab::UpdateSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"AnimationColoursTab::UpdateSection called with a NULL dialog pointer");

	// Ensure we talk to the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;		

	// Show the correct details.
	ShowDetails();

	return TRUE;
}

/********************************************************************************************

>	BOOL AnimationColoursTab::ShowDetails()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Shows the details of the associated SPREAD in this tab.
	
********************************************************************************************/

BOOL AnimationColoursTab::ShowDetails()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"AnimationColoursTab::ShowDetails called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;		

	if (pSpread != NULL)
	{
		// Ignore all DIM_TEXT_CHANGED msgs which are sent from this function.
		SetIgnoreTextChangeMsg(TRUE);

		// Retrieve the Animation details for the current spread.
		AnimPropertiesParam* pParam = &pSpread->GetSpreadAnimPropertiesParam();

		if(pParam)
		{
			// Set the correct dithering.
			DITHER Dither = pParam->GetDither();	

			switch(Dither)
			{
				case XARADITHER_NONE:
				{
					// Select the no dither radio button.
					pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_NODITHER),TRUE);
	
					// The remaining dither radio buttons should not be selected.
					pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_ORDDITHER),FALSE);
					pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_DIFFUSION),FALSE);
				}
				break;

				case XARADITHER_ORDERED_GREY:
				{
					// Select the ordered dither radio button.
					pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_ORDDITHER),TRUE);

					// The remaining dither radio buttons should not be selected.
					pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_DIFFUSION),FALSE);
					pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_NODITHER),FALSE);
				}
				break;

				case XARADITHER_ERROR_DIFFUSION:
				{
					// Select the error diffusion radio button.
					pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_DIFFUSION),TRUE);					

					// The remaining dither radio buttons should not be selected.
					pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_NODITHER),FALSE);
					pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_ORDDITHER),FALSE);
				}
				break;
				default:
				{
					// Select the ordered dither radio button.
					pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_ORDDITHER),TRUE);						

					// The remaining dither radio buttons should not be selected.
					pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_DIFFUSION),FALSE);
					pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_NODITHER),FALSE);
				}
				break;
			}

			// Set the correct transparency setting.
			pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_TRANSPARENCY), pParam->GetIsBackGroundTransp());

			// Set the correct Palette details.
			PALETTE_COLOURS PalCols = pParam->GetPaletteCols();		// Palette colours.
			WEB_PALETTE	WebPal = pParam->GetPalette();				// Palette type.	
			BOOL Optimized = FALSE;									// Has the optimized palette switch been selected.
			BOOL GlobalBrowserPalette = FALSE;						// Is the gloab browser palette option selected.
			
			// Global Browser Palette.
			if(WebPal == PAL_GLOBAL && PalCols == PALCOL_BROWSER)
			{
				// Select the global browser pallette radio button.
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_GLOBALBROWSER),TRUE);
				GlobalBrowserPalette = TRUE;

				// The remaining radio buttons should not be selected.
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_GLOBALOPTIMISED),FALSE);
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_LOCALOPT),FALSE);				
			}
			
			// Global Optimized Palette.
			if(WebPal == PAL_GLOBAL && PalCols == PALCOL_OPTIMIZED)
			{
				// Select the Global Optimized Palette.
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_GLOBALOPTIMISED),TRUE);
				Optimized = TRUE;
				
				// The remaining radio buttons should not be selected.
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_GLOBALBROWSER),FALSE);
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_LOCALOPT),FALSE);				
			}
			
			// Optimized Palette.
			if(WebPal == PAL_LOCAL && PalCols == PALCOL_OPTIMIZED)
			{
				//Select the Optimized Palette.
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_LOCALOPT),TRUE);
				Optimized = TRUE;

				// The remaining radio buttons should not be selected.
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_GLOBALBROWSER),FALSE);
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_GLOBALOPTIMISED),FALSE);
			}

			// If the palette is optimized grey out the ordered dither switch.
			if(Optimized)
			{
				// Grey the ordered dither switch.
				pPropertiesDlg->EnableGadget(_R(IDC_FRAME_ORDDITHER),FALSE);

				// Ungrey the 'Number of colours in Palette' editable field.
				pPropertiesDlg->EnableGadget(_R(IDC_FRAME_NUMCOLOURS), TRUE);

				// If the ordered dither option was selected, select the error diffusion option.
				if(Dither == XARADITHER_ORDERED_GREY)
					pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_DIFFUSION),TRUE);
			}

			// Set the correct "Number of colours in Palette" editable field.
			// If the 'Global browser palette' switch is selected, then the "Number of colours in palette" editable field is greyed.
			if(GlobalBrowserPalette)
			{
				DWORD NumColsInPalette = pParam->GetNumColsInPalette();
				pPropertiesDlg->SetLongGadgetValue(_R(IDC_FRAME_NUMCOLOURS), NumColsInPalette);
				pPropertiesDlg->EnableGadget(_R(IDC_FRAME_NUMCOLOURS), FALSE);
			}
			else
			{
				DWORD NumColsInPalette = pParam->GetNumColsInPalette();
				pPropertiesDlg->SetLongGadgetValue(_R(IDC_FRAME_NUMCOLOURS), NumColsInPalette);
			}
		}
	}

	// We once again wish to deal with DIM_TEXT_CHANGED msgs.
	SetIgnoreTextChangeMsg(FALSE);

	// Set the controls to the correct state. 
	ChangeControlStatus(!m_GreyStatus);
	return TRUE;
}

/*************************************************************************************************************************

>	BOOL AnimationColoursTab::UpdateLayerSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Called when the spread-related section of the tab should be updated.

***************************************************************************************************************************/
BOOL AnimationColoursTab::UpdateLayerSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"AnimationColoursTab::UpdateLayerSection() called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;	

	// Display the details in this tab.
	ShowDetails();
		
	// Everything ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL AnimationColoursTab::HandleMsg(DialogMsg* Msg)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the messages for this Animation colours tab.

********************************************************************************************/

BOOL AnimationColoursTab::HandleMsg(DialogMsg* Msg)
{
	ERROR2IF(Msg == NULL,FALSE,"AnimationColoursTab::Message null message received");
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"AnimationColoursTab::HandleMsg called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;	

	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:		
			m_GreyStatus = FALSE;	
			ok = InitSection();
			if (!ok)
				InformError();
			break;

		case DIM_LFT_BN_CLICKED:
		{
			HavePropertiesChanged();

			// Set the correct state of the applynow flag.
			AnimationColoursTab::SetApplyNowState(TRUE);

			// Ungrey the apply/ok/cancel buttons.
			AnimationColoursTab::UngreyApplyNow();
		}	
		break;

		case DIM_SELECTION_CHANGED:	
		{
			if(AnimationColoursTab::GetApplyNowState())
			{
				// Ungrey the apply/ok/cancel buttons.
				AnimationColoursTab::UngreyApplyNow();
			}
		}
		break;

		case DIM_TEXT_CHANGED:
		{
			// Do we want to deal with text change msgs?
			if(!GetIgnoreTextChangeMsg())
			{
				// Have the animation properties changed?
				if(HavePropertiesChanged())
				{
					AnimationColoursTab::SetApplyNowState(TRUE);

					// Ungrey the apply/ok/cancel buttons.
					AnimationColoursTab::UngreyApplyNow();
				}
			}
		}
		break;

		case DIM_SET_ACTIVE:
		{
			if(AnimationColoursTab::GetApplyNowState())
			{
				// Ungrey the apply/ok/cancel buttons.
				AnimationColoursTab::UngreyApplyNow();
			}
		}
		break;
	}
	return TRUE;
}  

/**********************************************************************************************

>	BOOL AnimationColoursTab::UpdateApplyState()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	This changes the 'Apply now' state based on the settings of the active spread
				and the settings in this tab.
				If the settings are the same, the 'Apply state' is FALSE, else it is TRUE.
	
************************************************************************************************/

BOOL AnimationColoursTab::UpdateApplyState()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"animationColoursTab::CommitSection called with a NULL dialog pointer");
/*
	// Ensure we talk ot the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;

	// Have the properties for this tab changed.
	if (pSpread != NULL)
		AnimationColoursTab::SetApplyNowState(HavePropertiesChanged());

	// The new state for the 'Apply/ok/Cancel' buttons.
	BOOL NewApplyNowState = AnimationColoursTab::GetApplyNowState();

	// If the properties for this tab have changed, then ungrey the apply now button.
	if (NewApplyNowState || GetCurrentApplyNow())
	{
			// Ungrey the apply/ok/cancel buttons.
			AnimationColoursTab::UngreyApplyNow();
	}
*/	
	// Everything ok.
	return TRUE;
}

/*************************************************************************************************************************

>	BOOL AnimationColoursTab::HavePropertiesChanged()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if settings in the dlg are different to those in the spread else FALSE.
	
****************************************************************************************************************************/

BOOL AnimationColoursTab::HavePropertiesChanged()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"animationColoursTab::HavePropertiesChanged called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;

	// Get the current details from animation colours tab.

	PALETTE_COLOURS CurPalCols;							// Browser/Optimized palette.
	WEB_PALETTE		CurWebPal;							// Global/Local palette.	
	DWORD			CurNumColsInPal			= 256;		// The current number of colours in the palette.
	BOOL			Global_Browser_Palette	= FALSE;	// Has the Global browser palette switch been selected.
	BOOL			Optimized				= FALSE;	// Has the optimized palette switch been selected.
	UINT32			CurPaletteType			= 0;		// The current palette type.
	UINT32			PaletteType				= 0;		// The palette type stored in the current spread.


	// Find the selected palette type.
	// Global Browser
	if (pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAME_GLOBALBROWSER)))
	{
		CurPalCols		=	PALCOL_BROWSER;
		CurWebPal		=	PAL_GLOBAL;
		CurPaletteType	=	1;
		Global_Browser_Palette = TRUE;
	}
	// Global Optimized
	else if (pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAME_GLOBALOPTIMISED)))
	{
		CurWebPal		= PAL_GLOBAL;
		CurPalCols		= PALCOL_OPTIMIZED;
		Optimized		= TRUE;		
		CurPaletteType	= 2;
		Global_Browser_Palette = FALSE;
	}
	// Local Optimized
	else if(pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAME_LOCALOPT)))
	{
		CurWebPal		= PAL_LOCAL;
		CurPalCols		= PALCOL_OPTIMIZED;
		Optimized		= TRUE;
		CurPaletteType	= 3;
		Global_Browser_Palette = FALSE;
	}

	// If the 'Global browser palette' switch is selected, then the 'Number of colours in palette' editable field is greyed.
	if(Global_Browser_Palette)
		pPropertiesDlg->EnableGadget(_R(IDC_FRAME_NUMCOLOURS), FALSE);
	else
		// Ungrey the 'Number of colours in Palette' editable field.
		pPropertiesDlg->EnableGadget(_R(IDC_FRAME_NUMCOLOURS), TRUE);

	//Find the 'Number of colours in palette' setting.
	CurNumColsInPal = pPropertiesDlg->GetLongGadgetValue(_R(IDC_FRAME_NUMCOLOURS), 0, 256, _R(IDS_BMPPREFS_INVALIDDELAY) ,NULL);

	// Get the current dither type.
	DITHER CurDither ;
	if (pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAME_DIFFUSION)))
		CurDither = XARADITHER_ERROR_DIFFUSION;

	else if (pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAME_ORDDITHER)))
		CurDither = XARADITHER_ORDERED_GREY;
		
	else if (pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAME_NODITHER)))
		CurDither = XARADITHER_NONE;
	
	// If the palette is optimized grey out the ordered dither option.
	if(Optimized)
	{
		pPropertiesDlg->EnableGadget(_R(IDC_FRAME_ORDDITHER),FALSE);

		// If the ordered dither option was selected, select the error diffusion option.
		if(CurDither == XARADITHER_ORDERED_GREY)
			pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAME_DIFFUSION),TRUE);
			pPropertiesDlg->SetLongGadgetValue(_R(IDC_FRAME_ORDDITHER), FALSE);			
	}
	else
		pPropertiesDlg->EnableGadget(_R(IDC_FRAME_ORDDITHER),TRUE);

	// Get the background transparency setting.
	BOOL CurTransparency = pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAME_TRANSPARENCY));

	// Retrieve the Animation details for the CURRENT spread.
	AnimPropertiesParam* pParam = &pSpread->GetSpreadAnimPropertiesParam();

	ERROR2IF(pParam == NULL,FALSE,"AnimationColoursTab::HavePropertiesChanged makes reference to a NULL ptr!");

	// Get the dither type for the current spread.
	DITHER Dither = pParam->GetDither();

	// Get the transparency setting for the current spread.
	BOOL Transparency = pParam->GetIsBackGroundTransp();

	// Get the palette type for the current spread.
	PALETTE_COLOURS PalCols = pParam->GetPaletteCols();
	WEB_PALETTE	WebPal = pParam->GetPalette();

	// Set the correct palette type.
	if(PalCols == PALCOL_BROWSER && WebPal == PAL_GLOBAL)
		PaletteType = 1;
		
	else if(PalCols == PALCOL_OPTIMIZED && WebPal == PAL_GLOBAL)
		PaletteType = 2;
			
	else if(PalCols ==  PALCOL_OPTIMIZED && WebPal == PAL_LOCAL)
		PaletteType = 3;
	
	// Get the number of colours in the palette  for the current spread.
	DWORD NumColsInPal = pParam->GetNumColsInPalette();

	// Compare the new preferences to those stored in the spread.
	return
		((CurDither		 != Dither)			||
		(CurTransparency != Transparency)	||
		(CurPaletteType	 != PaletteType)	||
		(CurNumColsInPal != NumColsInPal));			
}

/******************************************************************************************

>	BOOL AnimationColoursTab::InitSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for this tab on the animation properties dialog box.

******************************************************************************************/

BOOL AnimationColoursTab::InitSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"AnimationColoursTab::InitSection called with a NULL dialog pointer");

	// Get a ptr to the current document.
	Document* pDoc = Document::GetCurrent();

	// If there is no document, then grey the tab.
	if(!pDoc)
	{
		GreySection();
		return (TRUE);
	}

	ShowDetails();
	return TRUE;
}




				/********************************************************************************/
				/**********************										*********************/
				/**********************		Class:-	AnimationPropertiesTab	*********************/
				/**********************										*********************/
				/*********************************************************************************/





/********************************************************************************************

>	AnimationPropertiesTab::AnimationPropertiesTab() 


	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-	
	Outputs:	-
	Returns:	-
	Purpose:	AnimationPropertiesTab constructor. 

********************************************************************************************/

AnimationPropertiesTab::AnimationPropertiesTab()
{   	
}   	     

/********************************************************************************************

>	AnimationPropertiesTab::~AnimationPropertiesTab()


	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	AnimationPropertiesTab destructor

********************************************************************************************/
																				
AnimationPropertiesTab::~AnimationPropertiesTab()
{   	
}        

/********************************************************************************************

>	BOOL AnimationPropertiesTab::Init()


	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	AnimationPropertiesTab init routine.
	
********************************************************************************************/
																				
BOOL AnimationPropertiesTab::Init()
{   
	return TRUE;
}        

/******************************************************************************************

>	CDlgResID AnimationPropertiesTab::GetPageID()()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	
******************************************************************************************/

CDlgResID AnimationPropertiesTab::GetPageID()
{
	return _R(IDD_TAB_ANIMATION_PROPERTIES);
}

/******************************************************************************************

>	BOOL AnimationPropertiesTab::IsPropertyRequired()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Returns:	Returns True if this tab is a program option.	  
	Purpose:	Allows the Animation properties to determine if this tab is required.

******************************************************************************************/

BOOL AnimationPropertiesTab::IsPropertyRequired()
{
	//  If the dialog has been invoked via the options button on the Export dialog box,
	//  then hide this tab.
	if (GetBrowserPreviewTab())
		return FALSE;
	else
		return TRUE;	// This tab is required 		
}


/**********************************************************************************************************************

>	BOOL AnimationPropertiesTab::CommitSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog the box are ok, False otherwise.	  
	Purpose:	Takes the values in this tab of the Animation properties dialog box.
				Called when ok or apply is clicked on the main dialog box.
	
**************************************************************************************************************************/

BOOL AnimationPropertiesTab::CommitSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"AnimationPropertiesTab::CommitSection called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;		

	// Only do the op if we have a layer with a different set of properties.
	if (pSpread && HavePropertiesChanged(TRUE))
	{
		// Retrieve the values from the animation properties tab dialog.
		DWORD	Delay = 0;
		BOOL	DelayValid = FALSE;
		BOOL	LoopValid	= FALSE;
		BOOL	PropertiesChanged = FALSE;	// Flag to indicate that the properties have changed and the new entries are valid.

		BOOL	Loop_Continously = pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_ANIMOPTS_LOOP));
		DWORD	Loop  = pPropertiesDlg->GetLongGadgetValue(_R(IDC_ANIMOPTS_ITERATIONS), 0, 65535, _R(IDS_BMPPREFS_INVALIDLOOP) ,&LoopValid);
				
		// If the loop is invalid, return FALSE.
		if(!LoopValid)
			return FALSE;
		
		// Only check the delay edit field value if "Many" is not currently displayed.
		if(!m_IsManyDisplayed)
			Delay = pPropertiesDlg->GetLongGadgetValue(_R(IDC_ANIMTAB_DELAY), 0, 65535, _R(IDS_BMPPREFS_INVALIDDELAY) ,&DelayValid);

		// The GetLongGadgetValue function returns a Zero delay value when nothing is displayed 
		// in the 'Display each frame for:' edit field, we should therefore display a zero value.
		if(Delay == 0 && !m_IsManyDisplayed)
			pPropertiesDlg->SetLongGadgetValue(_R(IDC_ANIMTAB_DELAY), 0);	

		// If the delay is invalid, return FALSE.
		if(!DelayValid && !m_IsManyDisplayed)
			return FALSE;

		// If the 'Loop Continously' check box is ticked, then set the loop value to zero.
		if(Loop_Continously)
			Loop = 0;

		// Check that all visible layers are actually frame layers.
		FrameSGallery::EnsureFrameLayerIntegrity(pSpread);

		// Pass the new loop value to the spread.
		pSpread->SetAnimationLoop(Loop);

		//If we get this far, then the animation details have changed and the new entries are valid.
		PropertiesChanged = TRUE;

		// If 'Many' is not displayed, then pass on our new delay value.
		if(!m_IsManyDisplayed)
		{
			// Pass the new delay value to the spread.
			pSpread->SetAnimationDelay(Delay);

			// This is a global delay value, therefore, set this value in the frame layer and associated OILbitmap.
			SetGlobalDelay(pSpread, Delay);
		}
			
		//	If the Preview dialog is open, regenerate all the bitmaps in the animation.
		if (PreviewDialog::GetPreviewDialog())
		{
			// Make sure the preview dialog is not in a greyed state.
			// Get our static pointer to the Preview Dialog.
			PreviewDialog* pPreviewDlg = PreviewDialog::GetPreviewDialog();

			// Check, for a NULL ptr.
			if(pPreviewDlg)
			{
				// If the dlg is not in a greyed state, invoke the OP.
				if(!pPreviewDlg->GetDlgState())	
				{
					// Invoke the op.
					OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FRAME_GRABALLFRAMES); 
					if (pOpDesc != NULL)
						pOpDesc->Invoke();
					else
					{
						ERROR3("Couldn't find OPTOKEN_FRAME_GRABALLFRAMES op descriptor");
					}
				}
			}
		}
		
		// Update the display information in frame gallery.
		Layer* pLayer = pSpread->FindFirstFrameLayer();
		while (pLayer != NULL)
		{
			// This is just as a double check
			if (!pLayer->IsBackground() && !pLayer->IsGuide() &&
				!pLayer->IsPageBackground())
			{
				// Tell the frame gallery to update its display of the frame
				BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::REDRAW_LAYER));
			}

			// Move to the next frame layer in the animation
			pLayer = pLayer->FindNextFrameLayer();
		}				
		// Update the information in the frame properties tab.
		// Find the active layer.
		pLayer = pSpread->FindActiveLayer();

		// Boradcast the message.
		BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::UPDATE_ACTIVE_LAYER));			

		//The new delay entry is valid.
		PropertiesChanged = TRUE;
		
		// If the animation properties have changed and the new entries are valid, mark the doc. as modified.
		if(PropertiesChanged)
		{
			// Get a ptr to the current document.
			Document* pDoc = Document::GetCurrent();

			// Set this document as edited.
			if(pDoc)
				pDoc->SetModified(TRUE);			
		}
	}
	// If the delay value for any particular frame layer has changed, then we may need to 
	// alter the value displayed in the 'Display each frame for' edit field.
	if(GetFrameDelayValueChanged())
	{
		// At this point check to ensure the 'Display each frame for' edit field is displaying the correct value.
		if(SameflDelayValues())
		{
			// All the frame layers have the same delay value.
			// Take the delay value from the first layer.
			// Get a ptr to the first layer.
			Layer* pLayer = pSpread->FindFirstLayer();

			// Ensure we have valid spread and Layer pointers.
			if (pSpread && pLayer) 
			{
				// Get the delay value for the frame layer.
				DWORD GlobalDelay = pLayer->GetFrameDelay();

				// Set this frame layer delay value as the new global delay value.
				pSpread->SetAnimationDelay(GlobalDelay);

				// Display this value in the delay edit field for this tab.
				pPropertiesDlg->SetLongGadgetValue(_R(IDC_ANIMTAB_DELAY), pLayer->GetFrameDelay());	
			}
		}
		else
		{
			// The delay values differ, display "MANY" in the delay edit field.
			pPropertiesDlg->SetStringGadgetValue(_R(IDC_ANIMTAB_DELAY), _R(IDS_MANY));
		}	

		// We have taken all the neccessary actions, so set this this flag back to false.
		SetFrameDelayValueChanged(FALSE);	
	}

	// Everything ok!
	return TRUE;
}

/***********************************************************************************************************

>	BOOL AnimationPropertiesTab::SetGlobalDelay(Spread* pSpread, DWORD Delay)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		pSpread:-	A ptr to the currenT spread.
				Delay:-		The delay value to store in the layer/OiLBitmap.
	Outputs:	-
	Returns:	TRUE if successful, else false.
	Purpose:	The delay value in the animation properties tab has changed.  
				This is a global value therefore set this value in the frame layer and associated OILbitmap.
		
***************************************************************************************************************/

BOOL AnimationPropertiesTab::SetGlobalDelay(Spread *pSpread, DWORD Delay)
{
	ERROR2IF(pSpread == NULL,FALSE,"AnimationPropertiesTab::SetGlobalDelay called with NULL Spread pointer");

	// Get a ptr to the first frame layer.
	Layer* pLayer = pSpread->FindFirstFrameLayer();
	
	if(!pLayer)
		return FALSE;	// No frame layer.

	// Iterate through the list of layers.
	while (pLayer)
	{
		// Get the kernel bitmap associated with this frame layer. 
		// It might be the generated one
		KernelBitmap* pKernelBitmap = pLayer->GetGeneratedBitmap();
		// or if not present, may be a referenced one, i.e. single node bitmap on the layer
		if (pKernelBitmap == NULL)
			pKernelBitmap = pLayer->GetReferencedBitmap();

		// Is there a bitmap on this layer?
		if(pKernelBitmap != NULL)
		{
			// Get the OILBitmap assciated with this kernel bitmap.
			OILBitmap* pOILBitmap = pKernelBitmap->GetActualBitmap();
			if (pOILBitmap == NULL)
				return FALSE;

			// Set the delay value in the OILBitmap.
			pOILBitmap->SetBitmapAnimDelay(Delay);
		}

		// Set the delay value in the layer.
		pLayer->SetFrameDelay(Delay);
	
		// Get the next frame layer.
		pLayer = pLayer->FindNextFrameLayer();
	}
	
	return TRUE;
}



/***************************************************************************************************************

>	BOOL AnimationPropertiesTab::GreySection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when all documents have been closed.
	
********************************************************************************************/

BOOL AnimationPropertiesTab::GreySection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"animationPropertiesTab::GreySection called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;	

	// Only update if we are not already grey 
	if (m_GreyStatus == TRUE)
		return TRUE;

	// Set the correct control status.
	pPropertiesDlg->EnableGadget(_R(IDC_ANIMOPTS_ITERATIONS),	FALSE);
	pPropertiesDlg->EnableGadget(_R(IDC_ANIMOPTS_LOOP),			FALSE);
	pPropertiesDlg->EnableGadget(_R(IDC_ANIMTAB_DELAY),			FALSE);

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(FALSE);
	
	m_GreyStatus = TRUE;
	SetApplyNowState(FALSE);

	return ok;
} 

/********************************************************************************************

>	BOOL AnimationPropertiesTab::UngreySection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when a new document has been selected.
	
********************************************************************************************/

BOOL AnimationPropertiesTab::UngreySection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"animationpropertiseTab::UngreySection called with no dialog pointer");

	// Ensure we are talking to the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());	
	if (!ok)
		return TRUE;	

	// Only update if we are not already ungrey 
	if (m_GreyStatus == FALSE)
		return TRUE;

	// Set the correct control status.
	pPropertiesDlg->EnableGadget(_R(IDC_ANIMOPTS_ITERATIONS),	TRUE);
	pPropertiesDlg->EnableGadget(_R(IDC_ANIMOPTS_LOOP),			TRUE);
	pPropertiesDlg->EnableGadget(_R(IDC_ANIMTAB_DELAY),			TRUE);

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(TRUE);
	
	// Set the correct control grey status.
	m_GreyStatus = FALSE;

	return ok;
}

/********************************************************************************************

>	BOOL AnimationPropertiesTab::ChangeControlStatus(const BOOL Status)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 

********************************************************************************************/

BOOL AnimationPropertiesTab::ChangeControlStatus(const BOOL Status)
{
	// Grey the apply/ok/cancel buttons.
	AnimationPropertiesTab::GreyApplyNow();
		
	return TRUE;
}			

/****************************************************************************************************************

>	BOOL AnimationPropertiesTab::UpdateSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. 			
	
******************************************************************************************************************/

BOOL AnimationPropertiesTab::UpdateSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"animationPropertiesTab::UpdateSection called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;	

	// Display the details for this tab.
	ShowDetails();
		
	// Everythig ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL AnimationPropertiesTab::ShowDetails()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Shows the details of the associated spread in this tab.
	
********************************************************************************************/

BOOL AnimationPropertiesTab::ShowDetails()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"animationpropertiseTab::ShowDetails called with no dialog pointer");

	// Ensure we talk to the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;		
	
	// Ensure we have a valid spread ptr.
	if (pSpread)
	{
		// Ignore all DIM_TEXT_CHANGED msgs which are sent from this function.
		SetIgnoreTextChangeMsg(TRUE);

		// Retrieve the Animation details for the current spread.
		AnimPropertiesParam* pParam = &pSpread->GetSpreadAnimPropertiesParam();
		
		if(pParam)
		{
			// Do all the frame layers have the same delay value.
			if(SameflDelayValues())
			{
				// No frame layers exist.
				if(m_NoFrameLayers)	
				{
					// Take the delay value from the first layer.
					Layer* pLayer = pSpread->FindFirstLayer();
					if (pLayer)
					{
						// Get the delay value for the frame layer.
						DWORD GlobalDelay = pLayer->GetFrameDelay();

						// Set this frame layer delay value as the new global delay value.
						pSpread->SetAnimationDelay(GlobalDelay);

						// Display this value in the delay edit field for this tab.
						pPropertiesDlg->SetLongGadgetValue(_R(IDC_ANIMTAB_DELAY), pLayer->GetFrameDelay());	
					}

				}
				else
				{
					// If yes, then Set the numeric value in the "Display each frame for" edit field.
					// Take the delay value from the first frame layer, since all the layers have the same delay value.
					Layer* pLayer = pSpread->FindFirstFrameLayer();
					if (pLayer)
					{
						// Check that all visible layers are actually frame layers
						FrameSGallery::EnsureFrameLayerIntegrity(pSpread);

						// Get the delay value for the frame layer.
						DWORD GlobalDelay = pLayer->GetFrameDelay();

						// Set this frame layer delay value as the new global delay value.
						pSpread->SetAnimationDelay(GlobalDelay);

						// Display this value in the delay edit field for this tab.
						pPropertiesDlg->SetLongGadgetValue(_R(IDC_ANIMTAB_DELAY), pLayer->GetFrameDelay());	
					}				
				}
			}
			else
			{
				// The delay values differ, display "MANY" in the delay edit field.
				pPropertiesDlg->SetStringGadgetValue(_R(IDC_ANIMTAB_DELAY), _R(IDS_MANY));													
			}

			// Now Set the "Loop for" details.
			DWORD Loop = pParam->GetAnimLoop();

			// If the Loop value is zero, grey the loop edit field and check the loop cont. field.
			if(Loop == 0)
			{
				// Set a value of 1 in the field as zero is a bad value to display
				pPropertiesDlg->SetLongGadgetValue(_R(IDC_ANIMOPTS_ITERATIONS), 1);
				pPropertiesDlg->EnableGadget(_R(IDC_ANIMOPTS_ITERATIONS), FALSE);
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_ANIMOPTS_LOOP), TRUE);
			}
			else
			{
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_ANIMOPTS_LOOP), FALSE);
				pPropertiesDlg->SetLongGadgetValue(_R(IDC_ANIMOPTS_ITERATIONS), Loop);
			}
		}
	}

	// We once again wish to deal with DIM_TEXT_CHANGED msgs.
	SetIgnoreTextChangeMsg(FALSE);

	// Set the controls to the correct state. 
	ChangeControlStatus(!m_GreyStatus);

	// Everything ok.
	return TRUE;
}

/**************************************************************************************************************************

>	BOOL AnimationPropertiesTab::UpdateLayerSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Called when the spread-related section of the tab should be updated.

***************************************************************************************************************************/

BOOL AnimationPropertiesTab::UpdateLayerSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"AnimationPropertiesTab::UpdateLayerSection() called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;	

	// Display the details for this tab.
	ShowDetails();
	
	// Everything OK.
	return TRUE;
}

/********************************************************************************************

>	BOOL AnimationPropertiesTab::HandleMsg(DialogMsg* Msg)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the messages for this animation property tab.
	
********************************************************************************************/

BOOL AnimationPropertiesTab::HandleMsg(DialogMsg* Msg)
{
	ERROR2IF(Msg == NULL,FALSE,"AnimationPropertiesTab::Message Null message received");
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"AnimationPropertiesTab::HandleMsg called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;

	// Get this tab to update itself. Send it an active layer has changed msg.
	//Layer* pNewActiveLayer = pSpread->FindActiveLayer();
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:	
		{
			m_GreyStatus = FALSE;	
			ok = InitSection();
			// Grey the apply/ok/cancel buttons.
			AnimationPropertiesTab::GreyApplyNow();
			if (!ok)
				InformError();
		}
		break;

		case DIM_LFT_BN_CLICKED:
		{
			if(Msg->GadgetID == _R(IDC_ANIMOPTS_LOOP))
			{
				// If the "Loop continously" control box is ticked, grey the "Loop" ed field.
				BOOL Value = pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_ANIMOPTS_LOOP));
				if (Value)
					pPropertiesDlg->EnableGadget(_R(IDC_ANIMOPTS_ITERATIONS), FALSE);
				else
					pPropertiesDlg->EnableGadget(_R(IDC_ANIMOPTS_ITERATIONS), TRUE);								
			}

			AnimationPropertiesTab::SetApplyNowState(TRUE);

			// Ungrey the apply/ok/cancel buttons.
			AnimationPropertiesTab::UngreyApplyNow();
		}
		break;

		case DIM_SELECTION_CHANGED:
		{
			if(AnimationPropertiesTab::GetApplyNowState())
			{
				// Grey the apply/ok/cancel buttons.
				AnimationPropertiesTab::UngreyApplyNow();
			}
		}
		break;

		case DIM_TEXT_CHANGED:
		{
			// Do we want to deal with text change msgs?
			if(!GetIgnoreTextChangeMsg())
			{
				// Have the animation properties changed?
				if(HavePropertiesChanged())
				{
					// Set the correct state of the m_ApplyNow flag.
					AnimationPropertiesTab::SetApplyNowState(TRUE);

					// Grey the apply/ok/cancel buttons.
					AnimationPropertiesTab::UngreyApplyNow();

					// Force the m_FrameDelayValueChanged flag to FALSE.
					SetFrameDelayValueChanged(FALSE);
				}
			}
		}
		break;

		case DIM_SET_ACTIVE:
		{
			if(AnimationPropertiesTab::GetApplyNowState())
			{
				// Grey the apply/ok/cancel buttons.
				AnimationPropertiesTab::UngreyApplyNow();
			}
		}
		break;
	}	
	return TRUE;
}  

/***********************************************************************************************************************

>	BOOL AnimationPropertiesTab::UpdateApplyState()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	This changes the 'Apply now' state based on the settings of the active layer
				and the settings in this tab.
				If the settings are the same, the 'Apply state' is FALSE, else it is TRUE.
	
**************************************************************************************************************************/

BOOL AnimationPropertiesTab::UpdateApplyState()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"animationColoursTab::CommitSection called with a NULL dialog pointer");
/*
	// Ensure we talk to the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;

	// Have the properties for this tab changed.
	if (pSpread != NULL)
		AnimationPropertiesTab::SetApplyNowState(HavePropertiesChanged());

	// The new state for the 'Apply/ok/Cancel' buttons.
	BOOL NewApplyNowState = AnimationPropertiesTab::GetApplyNowState();

	// If the properties for this tab have changed, then un grey the apply now button.
	if (NewApplyNowState  || GetCurrentApplyNow())
	{
		// Ungrey the apply/ok/cancel buttons.
		AnimationPropertiesTab::UngreyApplyNow();
	}
	else
	{
		// Grey the apply/ok/cancel buttons.
		AnimationPropertiesTab::GreyApplyNow();
	}
*/
	// Everything ok.
	return TRUE;
}

/***********************************************************************************************************************

>	BOOL AnimationPropertiesTab::HavePropertiesChanged()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		ValidateDelayValue - Flag to determine when we need to validate the delay value.
				Defaults to false.
	Outputs:	-
	Returns:	TRUE if settings in the dlg are different to the given layer's settings, else FALSE.
	Purpose:	Central place for comparing settings in the dlg against the settings of a Spread
	
*************************************************************************************************************************/

BOOL AnimationPropertiesTab::HavePropertiesChanged(BOOL ValidateDelayValue)
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"AnimationPropertiessTab::CommitSection called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;

	BOOL UseLocalFrameDelayValues = FALSE;	// Flag to determine whether local frame delay values are being used.
	UINT32 CurrentDelay = 0;					//Current delay setting in the animation properties tab. 

	// Retrieve the details stored in the current spread.
	AnimPropertiesParam* pParam = &pSpread->GetSpreadAnimPropertiesParam();
	 	 
	// Get the delay/loop values.
	DWORD Loop	= pParam->GetAnimLoop();
	DWORD Delay = pParam->GetGlobalAnimDelay();

	// Retreive the current delay/Loop details from the dlg.
	BOOL	CurrentLoop_Continously = pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_ANIMOPTS_LOOP));
	DWORD	CurrentLoop  = pPropertiesDlg->GetLongGadgetValue(_R(IDC_ANIMOPTS_ITERATIONS), 0, 65535, _R(IDS_BMPPREFS_INVALIDLOOP) ,NULL);
		
	// If "Many" is displayed in the "Delay for" edit field, then we do not want to read the value.
	String_256 StrDelay = pPropertiesDlg->GetStringGadgetValue(_R(IDC_ANIMTAB_DELAY), NULL);
	INT32 result = strnicmp(StrDelay, "Many", 4);
			
	// Is 'Many' displayed in the delay edit field?
	if(result == 0)
	{
		// Set the flag to TRUE.
		m_IsManyDisplayed = TRUE;				

		// Bug fix. 'Many' is displayed in the edit field.
		// However, the global delay value is still set to a specific value e.g. 20cs.
		// Therefore, if the user sets a new global delay value, and specifies it as say 20cs,
		// we then perceive that no change has occurred. Therefore, force the delay value to
		// a value greater than 65535.
		pSpread->SetAnimationDelay(65536);

		// 'Many' is displayed, therefore set the Current Delay value to the value stored in the spread.
		CurrentDelay = Delay;
	}
	else
	{
		// 'Many' is not displayed, therefore set the flag to FALSE.
		m_IsManyDisplayed = FALSE;

		// Do we want to validate the delay input.
		if(ValidateDelayValue)
		{
			// Get the current delay input.
			BOOL DelayValid	= FALSE;
			CurrentDelay = pPropertiesDlg->GetLongGadgetValue(_R(IDC_ANIMTAB_DELAY), 0, 65535, _R(IDS_BMPPREFS_INVALIDDELAY) ,&DelayValid);

			// If the delay value is invalid, then go no further!
			if(!DelayValid)
				return FALSE;
		}
		else
		{
			// Get the current delay input.
			CurrentDelay = pPropertiesDlg->GetLongGadgetValue(_R(IDC_ANIMTAB_DELAY), 0, 65535, _R(IDS_BMPPREFS_INVALIDDELAY) ,NULL);
		}
	}

	// If this switch has been ticked, then set the current loop to a zero value.
	if(CurrentLoop_Continously)
		CurrentLoop = 0;

	// Compare the current values, against those stored in the spread.
	return ((CurrentDelay	!= Delay)	|| 
			(CurrentLoop	!= Loop));	
}

/******************************************************************************************

>	BOOL AnimationPropertiesTab::InitSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for this tab on the Animation properties dialog box.

******************************************************************************************/

BOOL AnimationPropertiesTab::InitSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"AnimationPropertiesTab::InitSection called with no dialog pointer");

	// Get a ptr to the current document.
	Document* pDoc = Document::GetCurrent();

	// If there is no document, then grey the tab.
	if(!pDoc)
	{
		GreySection();
		return (TRUE);
	}

	ShowDetails();

	return TRUE;
}





				/********************************************************************************/
				/**********************										*********************/
				/**********************		Class:-	FramePropertiesTab		*********************/
				/**********************										*********************/
				/********************************************************************************/	




/********************************************************************************************

>	FramePropertiesTab::FramePropertiesTab()


	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-	
	Outputs:	-
	Returns:	-
	Purpose:	FramePropertiesTab constructor. 
	
********************************************************************************************/

FramePropertiesTab::FramePropertiesTab()
{   
	//  Does nothing at present.
}   	     

/********************************************************************************************

>	FramePropertiesTab::~FramePropertiesTab()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	FramePropertiesTab destructor.
	
********************************************************************************************/
																				
FramePropertiesTab::~FramePropertiesTab()
{   
}        

/********************************************************************************************

>	BOOL FramePropertiesTab::Init()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	FramePropertiesTab init routine
	
********************************************************************************************/
																				
BOOL FramePropertiesTab::Init()
{   
	return TRUE;
}        

/******************************************************************************************

>	CDlgResID FramePropertiesTab::GetPageID()()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	
******************************************************************************************/

CDlgResID FramePropertiesTab::GetPageID()
{
	return _R(IDD_TAB_FRAME_PROPERTIES); 
}

/******************************************************************************************

>	BOOL FramePropertiesTab::IsPropertyRequired()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Returns:	Returns True if this tab is a program option.	  
	Purpose:	Allows the Frame properties to determine if this tab is required.

******************************************************************************************/

BOOL FramePropertiesTab::IsPropertyRequired()
{
	//  If the dialog has been invoked via the options button on the Export dialog box,
	//  then hide the frame properties tab.
	if (!GetIncludeFramePropertiesTab() || GetBrowserPreviewTab())
		return FALSE;
	else
		return TRUE;	// This tab is required 
}


/******************************************************************************************

>	BOOL FramePropertiesTab::CommitSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in this tab of the layer properties dialog box.
				Called when ok or apply now is pressed on the main dialog box.

******************************************************************************************/

BOOL FramePropertiesTab::CommitSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"FramePropertiesTab::CommitSection called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;	

	// Get a ptr to the active frame layer.
	Layer* pLayer = pSpread->FindActiveLayer();

	// Only do the op if we have a frame layer with a different set of properties.
	if (pLayer != NULL && HavePropertiesChanged(pLayer, TRUE))
	{
		DWORD Delay = 0;					// The delay value set for this frame layer.
		String_256 FrameLayerName;			// The Name for this frame layer.
		UINT32 AreDelayValuesSame = FALSE;	// Flag to indicate whether the frame layer delay values are the same.

		// Pass these new values onto the Frame layer.
		FrameLayerName	= pPropertiesDlg->GetStringGadgetValue(_R(IDC_FRAMETAB_NAME),NULL);	
		BOOL DelayValid	= FALSE;
		Delay = pPropertiesDlg->GetLongGadgetValue(_R(IDC_FRAMETAB_DELAY), 0, 65535, _R(IDS_BMPPREFS_INVALIDDELAY) ,&DelayValid);

		// If the delay value is invalid, then go no further!
		if(!DelayValid)
			return FALSE;

		// Are either of these check boxes selected.
		BOOL Overlay = pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAMETAB_OVERLAY));
		BOOL Solid = pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAMETAB_SOLID));

		// If the background and overlay flags are both set, force the ovrelay flag to false.
		// This is possible since the overlay check box may be greyed but at the same time be ticked.
		if(Solid && Overlay)
			Overlay = FALSE;

		// Is the "Show frame" check box ticked.
		BOOL IsHidden = pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAMETAB_SHOWFRAME));

		// Set the correct state of the layer IsHidden flag.
		if(IsHidden)
			pLayer->SetHiddenFrame(FALSE);
		else
			pLayer->SetHiddenFrame(TRUE);

		// Check if we are changing the solid (background) layer flag. If we are then
		// mark all dependent frames as edited.
		if (pLayer->IsSolid() != Solid)
		{
			// Mark all frames above this as edited until we reach the last frame
			// OR we hit a solid/background layer.
			Layer * pFrame = pLayer->FindNextFrameLayer();
			while (pFrame != NULL && !pFrame->IsSolid())
			{
				// Mark that frame as edited
				pFrame->SetEdited(TRUE);
				pFrame = pFrame->FindNextFrameLayer();
			}
		}

		// Pass the new data onto the layer.
		pLayer->SetSolid(Solid);
		pLayer->SetOverlay(Overlay);
		pLayer->SetFrameDelay(Delay);
		pLayer->SetLayerID(FrameLayerName);
		
		// Mark the document as edited.
		// First get a ptr to the current document.
		Document* pDoc = Document::GetCurrent();

		// Set this document as edited.
		if(pDoc)
			pDoc->SetModified(TRUE);					

		// Mark this layer as edited.
		pLayer->SetEdited(TRUE);

#ifdef _DEBUG
		// Tell the frame gallery to update its display of the frame
		BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::REDRAW_LAYER));
#endif

		// Get the OILBitmap Bitmap associated with this frame layer and set its delay value and Name details.
		// First find the kernel bitmap.
		// It might be the generated one
		KernelBitmap* pKernelBitmap = pLayer->GetGeneratedBitmap();
		// or if not present, may be a referenced one, i.e. single node bitmap on the layer.
		if (pKernelBitmap == NULL)
			pKernelBitmap = pLayer->GetReferencedBitmap();

		// Check for an empty frame layer. 
		if(pKernelBitmap == NULL)
		{
			//if the frame layer delay values for this animation are no longer the same, force the animation properties tab to update itself.
			if(!SameflDelayValues())
				BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::UPDATE_ACTIVE_LAYER));
			else
				AreDelayValuesSame = TRUE;

			// Currently "Many" is displayed in the animation propertiess tab, but the frame propertiess tab delay
			// values have changed to give all the frame layers the same delay value. Therfore, update the animation properties tab.
			if(AreDelayValuesSame && m_IsManyDisplayed)
				BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::UPDATE_ACTIVE_LAYER));
			
			// Force a redraw of the frame gallery. Allows us to update frame layer Name/Delay details.
			BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::REDRAW_LAYER));

			return TRUE;
		}
		
		// Get the OILBitmap assciated with this kernel bitmap.
		OILBitmap* pOILBitmap = pKernelBitmap->GetActualBitmap();
		if (pOILBitmap == NULL)
		{

			//if the frame layer delay values for this animation are no longer the same, force the animation properties tab to update itself.
			if(!SameflDelayValues())
				BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::UPDATE_ACTIVE_LAYER));
			else
				AreDelayValuesSame = TRUE;

			// Currently "Many" is displayed in the animation propertiess tab, but the frame propertiess tab delay
			// values have changed to give all the frame layers the same delay value. Therfore, update the animation properties tab.
			if(AreDelayValuesSame && m_IsManyDisplayed)
				BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::UPDATE_ACTIVE_LAYER));
						
			// Force a redraw of the frame gallery. Allows us to update frame layer Name/Delay details.
			BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::REDRAW_LAYER));

			return TRUE;
		}

		// Set the delay value in the OILBitmap.
		pOILBitmap->SetBitmapAnimDelay(Delay);

		// Set the name in the OILBitmap.
		pOILBitmap->SetName(FrameLayerName);

		if (PreviewDialog::GetPreviewDialog())
		{
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FRAME_GRABFRAME); 
			GIFAnimationExportParam Param;
			if (pOpDesc != NULL)
				pOpDesc->Invoke(&Param);
			else
			{
				ERROR3("Couldn't find OPTOKEN_FRAME_GRABALLFRAMES op descriptor");
			}
		}

		//if the frame layer delay values for this animation are no longer the same, force the animation properties tab to update itself.
		if(!SameflDelayValues())
			BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::UPDATE_ACTIVE_LAYER));
		else
			AreDelayValuesSame = TRUE;

		// Currently "Many" is displayed in the animation propertiess tab, a change to the frame properties tab delay
		// value has caused all the frame layers to have the same delay value. Therfore, update the animation properties tab.
		if(AreDelayValuesSame && m_IsManyDisplayed)
			BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::UPDATE_ACTIVE_LAYER));
	}

	// Force a redraw of the frame gallery.
	BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::REDRAW_LAYER));

	// Everything is OK!
	return TRUE;
}

/********************************************************************************************

>	BOOL FramePropertiesTab::GreySection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when all documents are closed.
	
********************************************************************************************/

BOOL FramePropertiesTab::GreySection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"FramePropertiesTab::GreySection called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;	

	// Only update if we are not already grey 
	if (m_GreyStatus == TRUE)
		return TRUE;

	// Set thewse two controls here, fixes a bug.
	pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_DELAY),FALSE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_SHOWFRAME),FALSE);

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(FALSE);
	
	// Set the correct grey status.
	m_GreyStatus = TRUE;
	SetApplyNowState(FALSE);

	return ok;
} 

/********************************************************************************************

>	BOOL FramePropertiesTab::UngreySection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document. 
	
********************************************************************************************/

BOOL FramePropertiesTab::UngreySection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"FramePropertiesTab::UngreySection called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());	// The FramePropertiesTab identifier
	if (!ok)
		return TRUE;	

	// Only update if we are not already ungrey 
	if (m_GreyStatus == FALSE)
		return TRUE;

	// Grey the controls on the frame properties tab.
	pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_SHOWFRAME),TRUE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_NAME),TRUE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_SOLID),TRUE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_DELAY),TRUE);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_SHOWFRAME),TRUE);
		
	// Set the correct grey status.
	m_GreyStatus = FALSE;

	return ok;
}

/***************************************************************************************************************

>	BOOL FramePropertiesTab::ChangeControlStatus(const BOOL Status)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		Status - Flag to determine whether to grey/ungrey the controls.
				IgnoreOverlay - If we wish to ignore this field, this parameter	is set TRUE, defaults FALSE. 
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey controls on this tab. 
	
*****************************************************************************************************************/

BOOL FramePropertiesTab::ChangeControlStatus(const BOOL Status, BOOL IgnoreOverlay)
{
	// Set the controls to the correct state.
	pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_NAME),Status);
	pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_SOLID),Status);

	// In some cases we do not want change the status of this check box.

	if(!IgnoreOverlay)
		pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_OVERLAY),Status);

	// Determine whether the apply now/ok/canel buttons should be greyed/ungreyed.
	// Ensure a vlaid spread ptr.
	if(pSpread)
	{
		// Get a ptr to the active layer.
		Layer* pLayer = pSpread->FindActiveLayer();

		// Ensure a valid layer ptr.
		if(pLayer)
		{
			// Have the properties changed for this layer.
			if(HavePropertiesChanged(pLayer))
			{
				// Grey the apply now/ok/cancel buttons.
				FramePropertiesTab::UngreyApplyNow();
			}
//			else
				// Grey the apply now/ok/cancel buttons.
//				FramePropertiesTab::GreyApplyNow();
		}
	}
	else
	{
		// Grey the apply now/ok/cancel buttons.
		FramePropertiesTab::GreyApplyNow();
	}
	
	// Everything ok.
	return TRUE;
}			

/********************************************************************************************

  >	BOOL FramePropertiesTab::UpdateSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab.
			
********************************************************************************************/

BOOL FramePropertiesTab::UpdateSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"FramePropertiesTab::UpdateSection called with no dialog pointer");

	// Ensure we talk to the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;		

	// Display the correct details.
	ShowDetails();

	// Everything ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL FramePropertiesTab::ShowDetails()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Shows the details of the associated layer in this tab.
	
********************************************************************************************/

BOOL FramePropertiesTab::ShowDetails()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"FramePropertiseTab::ShowDetails called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;		

	// Ensure we have a valid spread ptr.
	if(!pSpread)
		return FALSE;

	// Get a ptr to the active layer.
	Layer* pLayer = pSpread->FindActiveLayer();

	// Check for a NULL ptr.
	if(!pLayer)
		return FALSE;

	// Ignore all DIM_TEXT_CHANGED msgs which are sent from this function.
	SetIgnoreTextChangeMsg(TRUE);

	// Set the dialog detais
	// Set the layer Layer Name.
	pPropertiesDlg->SetStringGadgetValue(_R(IDC_FRAMETAB_NAME),	&pLayer->GetLayerID());

	// Set the "Background" check box.
	pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAMETAB_SOLID), pLayer->IsSolid());

	// If the "Background" check box is ticked, grey the "Overlay" check box field.
	BOOL BackGroundSelected = FALSE;		// Is the "Background" check box ticked.
	if(pLayer->IsSolid())
		BackGroundSelected = TRUE;

	// The backgoround check box is ticked.
	if(BackGroundSelected)
	{
		// Grey the "Overlay" check box.
		pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_OVERLAY), FALSE);
	}
	else
	{
		// Set the "Overlay" check box to the correct state.
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAMETAB_OVERLAY), pLayer->IsOverlay());			
	}

	// Set the "Frame Delay" edit field.
	pPropertiesDlg->SetLongGadgetValue(_R(IDC_FRAMETAB_DELAY), pLayer->GetFrameDelay());			
	
	// Is the frame hidden?
	BOOL IsHidden = pLayer->IsHiddenFrame();

	// The frame is not hidden.
	if(!IsHidden)
	{
		// Ungrey the 'Delay this frame for' check box.
		pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_DELAY), TRUE);

		// Pass this value onto the control.
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAMETAB_SHOWFRAME), TRUE);
	}
	else
	{
		// The frame is hidden.
		// Pass this value onto the control.
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAMETAB_SHOWFRAME), FALSE);

		// Grey the 'Delay this frame for' check box.
		pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_DELAY), FALSE);
	}

	// Set the controls to the correct state. 
	// If the "Backgorund" check box is selected then we have already determined the state of the "Overlay" check box.
	// Therefore setting the second parameter TRUE ensures we do not change the status of this control.
	if(BackGroundSelected)
		ChangeControlStatus(!m_GreyStatus, TRUE);
	else
		ChangeControlStatus(!m_GreyStatus);

	// We once again wish to deal with DIM_TEXT_CHANGED msgs.
	SetIgnoreTextChangeMsg(FALSE);

	// Everything ok.
	return TRUE;
}

/****************************************************************************************************************************

>	BOOL FramePropertiesTab::UpdateLayerSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Called when the layer-related section of the tab should be updated.
	
******************************************************************************************************************************/

BOOL FramePropertiesTab::UpdateLayerSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"FramePropertiesTab::UpdateLayerSection() called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;	

	// When the frame gallery is opening an UPDATE_ACTIVE_LAYER message is sent. 
	// We wish to ignore the message, therefore this flag will be set to false.
	if(m_FrameGalleryOpen && !m_ChangeLayerState)
		ShowDetails();

	// This flag is only true when the active layer state has been
	// changed from within the frame gallery. At all other times we
	// invoke the Showdetails function above.
	if(m_ChangeLayerState)
		ChangeLayerState();

	// Everything ok.
	return TRUE;
}

/****************************************************************************************************************************

>	BOOL FramePropertiesTab::ChangeLayerState()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Called when the layer state has been changed from within the frame gallery, 
				this function updates the controls of the frame properties tab.
	
******************************************************************************************************************************/
BOOL FramePropertiesTab::ChangeLayerState()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"FramePropertiesTab::UpdateLayerSection() called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;	

	// Ensure we have a valid spread ptr.
	if(!pSpread)
		return FALSE;

	// Get a ptr to the active layer.
	Layer* pLayer = pSpread->FindActiveLayer();

	// Check for a NULL ptr.
	if(!pLayer)
		return FALSE;

	// Is the Solid or Overlay flag selected?

	// If the "Background" check box is ticked, grey the "Overlay" check box field.
	if(pLayer->IsSolid())
	{

		// Set the "Background" check box to the correct state.
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAMETAB_SOLID), TRUE);			

		// Grey the "Overlay" check box.
		pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_OVERLAY), FALSE);
	}
	else
	{
		// Set the "Background" check box to the correct state.
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAMETAB_SOLID), FALSE);			

		// Ungrey the "Overlay" check box.
		pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_OVERLAY), TRUE);

	}

	// Is the overlay check box ticked?
	if(pLayer->IsOverlay()) 
	{
		// Set the "Background" check box to the correct state.
		pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_SOLID), TRUE);			

		// Set the "Overlay" check box to the correct state.
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAMETAB_OVERLAY), TRUE);			
	}
	else
	{
		// Set the "Overlay" check box to the correct state.
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_FRAMETAB_OVERLAY), FALSE);			
	}

	// Set the correct state of the flag.
	FramePropertiesTab::SetApplyNowState(TRUE);

	// Ungrey the apply/ok/cancel buttons.
	FramePropertiesTab::UngreyApplyNow();

	// Everything ok.
	return TRUE;
}


/********************************************************************************************

>	BOOL FramePropertiesTab::HandleMsg(DialogMsg* Msg)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the messages for this frame property tab.
	
********************************************************************************************/

BOOL FramePropertiesTab::HandleMsg(DialogMsg* Msg)
{
	ERROR2IF(Msg == NULL,FALSE,"FramePropertiesTab::Message null message received");
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"FramePropertiesTab::HandleMsg called with no dialog pointer");

	// Talk to the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;		

	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:	
			m_GreyStatus = FALSE;
			ok = InitSection();
			if (!ok)
				InformError();
			break;

		case DIM_LFT_BN_CLICKED:
		{
			switch (Msg->GadgetID)
			{
				case _R(IDC_FRAMETAB_SOLID):
				{
					// If the "Background" check box is ticked, grey the "Overlay" check box.
					BOOL Value = pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAMETAB_SOLID));
					if (Value)
					{
						// Grey the 'Overlay' check box.
						pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_OVERLAY), FALSE);
					}
					else
					{
						// Ungrey the 'Overlay' check box.
 						pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_OVERLAY), TRUE);								
					}
				}
				break;

				case _R(IDC_FRAMETAB_SHOWFRAME):
				{
					// If the "Show frame" check box is ticked, grey the "Disaply this frame for" check box.
					BOOL Value = pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAMETAB_SHOWFRAME));

					// Get a ptr to the active layer.
					Layer* pLayer = pSpread->FindActiveLayer();

					// Ensure we have a valid ptr.
					if(!pLayer)
						break;

					// The 'Show frame' check box is ticked.
					if(Value)
					{
						// Ungrey the 'Delay this frame for' check box.
						pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_DELAY), TRUE);

						// Pass this value onto the layer. This is necessary as when we force the frame gallery 
						// to update itself, it takes the value stored in the layer. 
						//pLayer->SetHiddenFrame(FALSE);
					}
					else
					{
						// Grey the 'Delay this frame for' check box.  
						pPropertiesDlg->EnableGadget(_R(IDC_FRAMETAB_DELAY), FALSE);

						// Pass this value onto the layer. This is necessary as when we force the frame gallery 
						// to update itself, it takes the value stored in the layer. 
						//pLayer->SetHiddenFrame(TRUE);
					}

					// Tell the frame gallery to update its display of the frame.
					// We do this to update the right click pop-up menu.
					BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::REDRAW_LAYER));
				}
				break;
			}
			
			// Set the correct state of the flag.
			FramePropertiesTab::SetApplyNowState(TRUE);

			// Ungrey the apply/ok/cancel buttons.
			FramePropertiesTab::UngreyApplyNow();
			
		}
		break;

		case DIM_SELECTION_CHANGED:
		{
			if(FramePropertiesTab::GetApplyNowState())
			{
				// Ungrey the apply/ok/cancel buttons.
				FramePropertiesTab::UngreyApplyNow();	
			}			
		}
		break;

		case DIM_TEXT_CHANGED:
		{
			// Do we want to deal with text change msgs?
			if(!GetIgnoreTextChangeMsg())
			{
				// Get a ptr to the active layer.
				Layer* pLayer = pSpread->FindActiveLayer();

				// Ensure we have a valid layer ptr.
				if (pLayer)
				{
					// Have the frame properties changed?
					if(HavePropertiesChanged(pLayer))
					{
						// Set the correct state of the m_ApplyNow flag.
						FramePropertiesTab::SetApplyNowState(TRUE);

						// Ungrey the apply/ok/cancel buttons.
						FramePropertiesTab::UngreyApplyNow();
					}
				}

				// We need to make a note of this value change, 
				// this impacts on the value displayed in the delay 
				// edit field in the animation properties tab.
				SetFrameDelayValueChanged(TRUE);
			}
		}
		break;

		case DIM_SET_ACTIVE:
		{
			if(FramePropertiesTab::GetApplyNowState())
			{
				// Ungrey the apply/ok/cancel buttons.
				FramePropertiesTab::UngreyApplyNow();	
			}			
		}
		break;
	}
	return TRUE;
}  

/******************************************************************************************

>	BOOL FramePropertiesTab::UpdateApplyState()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	This changes the 'Apply now' state based on the settings of the active layer
				and the settings in this tab.
				If the settings are the same, the 'Apply state' is FALSE, else it are TRUE
	
******************************************************************************************/

BOOL FramePropertiesTab::UpdateApplyState()
{
/*
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"animationColoursTab::CommitSection called with a NULL dialog pointer");

	// Ensure we talk to the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;

	// Ensure we have an active layer.
	Layer* pLayer = GetActiveLayer();

	// Have the properties for this changed.
	if (pLayer != NULL)
		FramePropertiesTab::SetApplyNowState(HavePropertiesChanged(pLayer));

	// The new state for the 'Apply/ok/Cancel' buttons.
	BOOL NewApplyNowState = FramePropertiesTab::GetApplyNowState();

	// If the properties for this tab have changed, then ungrey the apply now button.
	if (NewApplyNowState  || GetCurrentApplyNow())
	{
		// Ungrey the apply/ok/cancel buttons.
		FramePropertiesTab::UngreyApplyNow();
	}
*/
	// Everything ok.
	return TRUE;
}

/**************************************************************************************************************************

>	BOOL FramePropertiesTab::HavePropertiesChanged(Layer* pLayer)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		pLayer - ptr to a layer
				ValidateDelayValue - Flag to determine when we need to validate the delay value.
				Defaults to false.
	Outputs:	-
	Returns:	TRUE if settings in the dlg are different to the given layer's settings, else FALSE
	
****************************************************************************************************************************/

BOOL FramePropertiesTab::HavePropertiesChanged(Layer* pLayer, BOOL ValidateDelayValue)
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"FramePropertiesTab::HavePropertiesChanged called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;

	// Get the current values from the tab.
	BOOL 		CurBackground	=	pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAMETAB_SOLID));
	BOOL 		CurOverlay		=	pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAMETAB_OVERLAY));
	String_256 	CurName			=	pPropertiesDlg->GetStringGadgetValue(_R(IDC_FRAMETAB_NAME),NULL);
	BOOL		CurIsHidden		=	pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_FRAMETAB_SHOWFRAME));
	DWORD		CurDelay = 0;

	// Do we want to validate the delay input.
	if(ValidateDelayValue)
	{
		BOOL DelayValid	= FALSE;
		CurDelay = pPropertiesDlg->GetLongGadgetValue(_R(IDC_FRAMETAB_DELAY), 0, 65535, _R(IDS_BMPPREFS_INVALIDDELAY) ,&DelayValid);

		// If the delay value is invalid, then go no further!
		if(!DelayValid)
			return FALSE;
	}
	else
	{
		CurDelay = pPropertiesDlg->GetLongGadgetValue(_R(IDC_FRAMETAB_DELAY), 0, 65535, _R(IDS_BMPPREFS_INVALIDDELAY) ,NULL);
	}	

	// If the check box is selected, then the frame is not hidden.
	if(CurIsHidden)
		CurIsHidden = FALSE;
	else
		CurIsHidden = TRUE;

	// If the background and overlay flags have both been set, then force the ovrelay flag to false.
	// This can occur since the overlay check box may be greyed but at the same time be ticked.
	if(CurBackground && CurOverlay)
		CurOverlay = FALSE;

	// Get the values stored in the frame layer.
	BOOL 		Background	=	pLayer->IsSolid();
	BOOL 		Overlay		=	pLayer->IsOverlay();
	String_256 	Name		=	pLayer->GetLayerID();
	DWORD		Delay		=	pLayer->GetFrameDelay();
	BOOL		IsHidden	=	pLayer->IsHiddenFrame();

	// Compare the current values against those stored in the frame layer.
	return ((Background	!= CurBackground) 	|| 
			(Overlay	!= CurOverlay)		||
			(Name		!= CurName)			||
			(Delay		!= CurDelay)		||
			(IsHidden	!= CurIsHidden ));	
}

/***************************************************************************************************************

>	BOOL FramePropertiesTab::InitSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for this tab on the animation properties dialog box.
	
*****************************************************************************************************************/

BOOL FramePropertiesTab::InitSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"FramePropertiesTab::InitSection called with no dialog pointer");

	// Get a ptr to the current document.
	Document* pDoc = Document::GetCurrent();

	// If there is no document, then grey the tab.
	if(!pDoc)
	{
		GreySection();
		return (TRUE);
	}

	// Display the details for this tab.
	ShowDetails();

	return TRUE;
}


						/**********************************************************************

							Animation Export Options dialog class.	
							This class is derived off GIFAnimationPropertyTabsDlg,
							this allows us to invoke two instances' of the same dialog.

						***********************************************************************/





/******************************************************************************************

>	BOOL AnimExOptns::Init()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
******************************************************************************************/

BOOL AnimExOptns::Init()
{
	BOOL InitOK;

	// Register ourselves with the dialog system.
	InitOK = RegisterOpDescriptor(
								0,															// Tool ID 																
								_R(IDS_ANIMATION_TAB_DLG),										// String resource ID
								CC_RUNTIME_CLASS(AnimExOptns),								// Runtime class
								OPTOKEN_EXPORTGIFANIMTABSDLG,								// Token string
								GetState,													// GetState function
								0,															// help ID
								0,															// bubble help
								0, //_R(IDD_BARCONTROLSTORE),									// resource ID
								0, //_R(IDC_BTN_OPTIONSDLG),									// control ID
								SYSTEMBAR_ILLEGAL,											// Bar ID
								TRUE,														// Recieve system messages
								FALSE,														// Smart duplicate operation
								TRUE,															// Clean operation
								0,														// No vertical counterpart
								_R(IDS_ANIMATION_TAB_DLG_ONE),									// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC)	// Auto state flags
								);
	return (InitOK);
}



/*****************************************************************************************************************************************************

>	AnimExOptns::AnimExOptns(): GIFAnimationPropertyTabsDlg(GIFAnimationPropertyTabsDlg::IDD, GIFAnimationPropertyTabsDlg::Mode) 

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	AnimExOptns constructor. 
	
********************************************************************************************************************************************************/

AnimExOptns::AnimExOptns():GIFAnimationPropertyTabsDlg(AnimExOptns::IDD, AnimExOptns::Mode)
{
	// Base class does all the work.
}


/***************************************************************************************************************

>	AnimExOptns::~AnimExOptns()
	
	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	returns:	- 
	Purpose:	AnimExOptns destructor.

*****************************************************************************************************************/

AnimExOptns::~AnimExOptns()
{
	// As the dialog is dying, set this flag TRUE, therefore if the Full animation properties dialog is invoked, 
	// we will include all the tabs registered to the system.

	// Ensure the dialog still exists.
	if(m_pGIFAnimationProperties)
	{
		// Set any necessary flags to the correct state.
		m_pGIFAnimationProperties->SetIncludeFramePropertiesTab(TRUE);
		m_pGIFAnimationProperties->SetBrowserPreviewTab(FALSE);	
	}
}

/******************************************************************************************

>	void AnimExOptns::Do(OpDescriptor*)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user.
					
******************************************************************************************/

void AnimExOptns::Do( OpDescriptor * pOpDesc ) 
{
	BOOL ok = FALSE;

	if (m_pGIFAnimationProperties == NULL)
	{
		// Could not create the dialog box.
		ERROR3("GIFAnimationPropertyTabsDlg::Do no AnimationProperties present");
		End();
	}

	//Set up the name that the main dialog page will use.
	String_256 Temp(_R(IDS_ANIMATION_EXPORT_OPTIONS)); 
	TitleString = Temp;
	SetName(&TitleString);
	
	// Make sure that our options tabs have a link to the dialog box class
	GIFAnimationPropertyTabs::SetTabbedDlg(this);

	// Force the apply now/ok buttons to a grey state.
	m_pGIFAnimationProperties->SetApplyNowState(FALSE);

	// On start up ungrey the OK button.
	m_pGIFAnimationProperties->SetOkState(TRUE);

	// Force the dialog box to be created and displayed.
	Create();		
};

/*********************************************************************************************************************

>	MsgResult GIFAnimationPropertyTabsDlg::Message(Msg* Message)
	
	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	OK	 		  Message handled ok.
				FAIL 		  Something terrible happened whilst processing the message.
				EAT_MSG       The Message was handled ok but don't send it to any more MessageHandlers. 
		
*************************************************************************************************************************/

MsgResult AnimExOptns::Message(Msg* Message)
{
	ERROR3IF(Message == NULL,"AnimExOptns::Message NULL message received");

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		MsgResult Result;

		// Main handler for dialog type messages
		BOOL EndDialog = FALSE;											// TRUE if we should quit the dialog
		BOOL CommitValues = FALSE;										// TRUE if we should commit the dialog values
		BOOL OldApplyNow = m_pGIFAnimationProperties->GetApplyNowState();	// Old ApplyNow button state  

		// Make sure that the tabs know what is the current document and spread to work on.
		Document *pDocument = Document::GetSelected();
		m_pGIFAnimationProperties->SetCurrentDocAndSpread(pDocument);

		// Make sure that the current document units are correct.
		m_pGIFAnimationProperties->SetUpDocUnits();
			
		// Check if sending init/create message and if so set flag True.
		// If True then do not ungrey the ApplyNow button on changing messages
		if (Msg->DlgMsg == DIM_CREATE)
			m_pGIFAnimationProperties->SetInitMessageState(TRUE);				

		// If the message is not for the main winndow then try the tab pages. 
		BOOL FoundPage = FALSE;

		if (Msg->PageID != 0)
		{
			//Go through all the tabs declared to the system and see if the message is for that page.
			GIFAnimationPropertyTabs *pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetFirst();

			while (pGIFAnimationPropertyTabs != NULL&& !FoundPage)
			{
				CDlgResID PageId = pGIFAnimationPropertyTabs->GetPageID();
				if (PageId == Msg->PageID)
				{
					// Found the correct page so stop the search and send the message to that page
					pGIFAnimationPropertyTabs->HandleMsg(Msg);
					FoundPage = TRUE;
				}
				// Get the next option's tab.
				pGIFAnimationPropertyTabs = GIFAnimationPropertyTabs::GetNext(pGIFAnimationPropertyTabs);							
			}
		}	
		else
		{
			// Main page window
			switch (Msg->DlgMsg)
			{
				case DIM_COMMIT:		
					EndDialog = TRUE;
					CommitValues = TRUE;
				break;

				case DIM_SOFT_COMMIT:	
				{
					CommitValues = TRUE;
					m_pGIFAnimationProperties->SetApplyNowState(TRUE);
				}
				break;

				case DIM_CANCEL:		
					EndDialog = TRUE;
					break;
				}
			}

		// Commit values here.
		if (CommitValues)
		{
			BOOL ok = CommitDialogValues();
			if (!ok)
			{
				EndDialog = FALSE;	
			}
		}
		
		// Pass the message back to the Base class.
		Result = DialogTabOp::Message(Message);

		// End dialog here
		if (EndDialog) 
		{
			Close();		
			End();			

			// Remove the link to the options tabs if all dialogs have been cloosed. 
			if(m_pGIFAnimationProperties->GetCount() < 1)
			{
				// Make sure that we remove our options tabs link to the dialog box class as the dialog will now be destroyed.
				m_pGIFAnimationProperties->SetTabbedDlg(NULL);
			}
		}
		
		// Check if we have been sending an init/create message, if so then set the flag False.
		if (Msg->DlgMsg == DIM_CREATE)
			m_pGIFAnimationProperties->SetInitMessageState(FALSE);
					
		//return DialogTabOp::Message(Message);
		return Result;
	}
	else
	{
		//Pass the message onto the base class message handler.
		return GIFAnimationPropertyTabsDlg::Message(Message);
	}
}







						/************************************************************/
						/************************************************************

								
								  	  PPREVIEW IN BROWSER TAB


						**************************************************************
						**************************************************************/











/********************************************************************************************

>	PreviewInBrowserTab::PreviewInBrowserTab() 


	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-	
	Outputs:	-
	Returns:	-
	Purpose:	PreviewInBrowserTab constructor.
	
********************************************************************************************/

PreviewInBrowserTab::PreviewInBrowserTab()
{   
	// Does nothing at present.
}   	     

/********************************************************************************************

>	PreviewInBrowserTab::~PreviewInBrowserTab()


	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	AnimationColoursTab destructor.

********************************************************************************************/
																				
PreviewInBrowserTab::~PreviewInBrowserTab()
{   
}        

/********************************************************************************************

>	BOOL PreviewInBrowserTab::Init()


	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	
********************************************************************************************/
																				
BOOL PreviewInBrowserTab::Init()
{   
	return TRUE;
}        

/******************************************************************************************

>	CDlgResID PreviewInBrowserTab::GetPageID()()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	
******************************************************************************************/

CDlgResID PreviewInBrowserTab::GetPageID()
{
	return _R(IDD_TAB_PREVIEW_IN_BROWSER);
}

/******************************************************************************************

>	BOOL PreviewInBrowserTab::IsPropertyRequired()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Returns:	Returns True if this tab is a program option.	  
	
******************************************************************************************/

BOOL PreviewInBrowserTab::IsPropertyRequired()
{
	return TRUE;	// This tab is required 
}


/******************************************************************************************

>	BOOL PreviewInBrowserTab::CommitSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Animation colours tab. 
				Called when ok or apply now is pressed on the main dialog box.

******************************************************************************************/

BOOL PreviewInBrowserTab::CommitSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"PreviewInBrowserTab::CommitSection called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;			

	// Have the preview in browser tab properties changed?
	if (HavePropertiesChanged())
	{
		//  Get the new details.
		BrowserBackground Bgr;

		if(pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_PLAINBKGND)))     
			Bgr = BROWSER_BGR_NONE;

		else if (pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_DOCBKGND)))
			Bgr = BROWSER_BGR_DOC;
	
		else if (pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_CHQBKGND)))
			Bgr = BROWSER_BGR_CHECKER;
	
		BOOL Info = pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_HTMLSTUB));

		// Store these new details in the global variables.
		g_Background = Bgr;	
		g_InfoInHtmlStub =Info;
	}
	
	// Everything ok.
	return TRUE;
}


/*************************************************************************************************************************

>	BOOL PreviewInBrowserTab::HavePropertiesChanged()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if settings in the dlg are different to those in the spread else FALSE.
	
****************************************************************************************************************************/

BOOL PreviewInBrowserTab::HavePropertiesChanged()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"PreviewInBrowserTab::HavePropertiesChanged called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;

	// Get the current details.
	BrowserBackground Bgr;

	if(pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_PLAINBKGND)))     
		Bgr = BROWSER_BGR_NONE;

	else if (pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_DOCBKGND)))
		Bgr = BROWSER_BGR_DOC;
	
	else if (pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_CHQBKGND)))
		Bgr = BROWSER_BGR_CHECKER;
	
	BOOL Info = pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_HTMLSTUB));

	// Compare the new details to those stored in the global variables.
	return
	((	Bgr != g_Background) ||
	(	Info != g_InfoInHtmlStub));			
}

/********************************************************************************************

>	BOOL PreviewInBrowserTab::GreySection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents.

********************************************************************************************/

BOOL PreviewInBrowserTab::GreySection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"PreviewInBrowserTab::GreySection called with no dialog pointer");

	//Ensure we are talking to the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;

	// Only update if we are not already grey. 
	if (m_GreyStatus == TRUE)
		return TRUE;

	// Call the central greying/ungreying function
	ok = ChangeControlStatus(FALSE);	

	// Set the correct grey status
	m_GreyStatus = TRUE;
	SetApplyNowState(FALSE);

	return ok;
} 

/********************************************************************************************

>	BOOL PreviewInBrowserTab::UngreySection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document.

********************************************************************************************/

BOOL PreviewInBrowserTab::UngreySection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"PreviewInBrowserTab::UngreySection called with a NULL dialog pointer");

	// Ensure we are talking to the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;

	// Only update if we are not already ungrey. 
	if (m_GreyStatus == FALSE)
		return TRUE;

	// Call the central greying/ungreying function
	ok = ChangeControlStatus(TRUE);	
	
	m_GreyStatus = FALSE;

	return ok;
}

/********************************************************************************************

>	BOOL PreviewInBrowserTab::ChangeControlStatus(const BOOL Status)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 

********************************************************************************************/


BOOL PreviewInBrowserTab::ChangeControlStatus(const BOOL Status)
{
	pPropertiesDlg->EnableGadget(_R(IDC_PLAINBKGND),	Status);
	pPropertiesDlg->EnableGadget(_R(IDC_DOCBKGND),		Status);
	pPropertiesDlg->EnableGadget(_R(IDC_CHQBKGND),		Status);
	pPropertiesDlg->EnableGadget(_R(IDC_GIF_BITMAPPREVIEW),	Status);
	pPropertiesDlg->EnableGadget(_R(IDC_HTMLSTUB),		Status);
//	pPropertiesDlg->EnableGadget(_R(IDC_GIF_T5CHECK2),		Status);

	// Grey the apply now/ok/cancel buttons.
	PreviewInBrowserTab::GreyApplyNow();
	return TRUE;
}			


/**********************************************************************************************************************

>	BOOL PreviewInBrowserTab::UpdateSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. 
	
*************************************************************************************************************************/

BOOL PreviewInBrowserTab::UpdateSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"PreviewInBrowserTab::UpdateSection called with a NULL dialog pointer");

	// Ensure we a re talking to the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;		

	// Show the correct details.
	ShowDetails();

	return TRUE;
}

/********************************************************************************************

>	BOOL PreviewInBrowserTab::ShowDetails()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Shows the details of the associated SPREAD in this tab.
	
********************************************************************************************/

BOOL PreviewInBrowserTab::ShowDetails()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"PreviewInBrowserTab::ShowDetails called with a NULL dialog pointer");

	// Talk to the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;		

	BrowserBackground Bgr = g_Background;	// Get the background details.
	BOOL Info = g_InfoInHtmlStub;			// Get the Include 'file info within HTML stub' details.

	// Selelct the correct 'background' radio button.
	if(Bgr == BROWSER_BGR_NONE)
	{
		// Check this radio button.
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_PLAINBKGND), TRUE);

		// Uncheck all other radio buttons in this group.
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_DOCBKGND), FALSE);
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_CHQBKGND), FALSE);
	}
			
	else if (Bgr ==  BROWSER_BGR_DOC)
	{
		// Check this radio button.
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_DOCBKGND), TRUE);

		// Uncheck all other radio buttons in this group.
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_PLAINBKGND), FALSE);
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_CHQBKGND), FALSE);
	}
		
	else if (Bgr == BROWSER_BGR_CHECKER)
	{
		// Check this radio button.
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_CHQBKGND), TRUE);     

		// Uncheck all other radio buttons in this group.
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_PLAINBKGND), FALSE);
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_DOCBKGND), FALSE);
	}
					
	else if (Bgr == BROWSER_BGR_BITMAP)
	{
		// In the Bitmap Preview diagram, the user selected this radio button. 
		// In the animation properties dialog, this radio button has not been included.
		// We therfore, force the selection to BROWSER_BGR_NONE
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_PLAINBKGND), TRUE);	

		// Uncheck all other radio buttons in this group.
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_DOCBKGND), FALSE);
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_CHQBKGND), FALSE);

		// Make a note of this new selection.
		g_Background = BROWSER_BGR_BITMAP;
	}	

	// Set the correct state of the 'Include file info within HTML stub' check box.
	pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_HTMLSTUB), Info == TRUE);

	// Everything ok.
	return TRUE;
}

/*************************************************************************************************************************

>	BOOL PreviewInBrowserTab::UpdateLayerSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Called when the spread-related section of the tab should be updated.

***************************************************************************************************************************/
BOOL PreviewInBrowserTab::UpdateLayerSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"PreviewInBrowserTab::UpdateLayerSection() called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;	

	// Display the details in this tab.
	ShowDetails();
	
	// Everything OK.
	return TRUE;
}

/********************************************************************************************

>	BOOL PreviewInBrowserTab::HandleMsg(DialogMsg* Msg)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com> (from stefan code)
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the messages for this Animation colours tab.

********************************************************************************************/

BOOL PreviewInBrowserTab::HandleMsg(DialogMsg* Msg)
{
	ERROR2IF(Msg == NULL,FALSE,"PreviewInBrowserTab::Message null message received");
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"PreviewInBrowserTab::HandleMsg called with a NULL dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;	

	// Handle the message.
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
		{
			// Set the Grey status flag to FALSE.
			m_GreyStatus = FALSE;	

			// Perform the necessary initialization. 
			ok = InitSection();
			if (!ok)
				InformError();

			// Get the global settnigs.
			BrowserBackground Bgr = g_Background;
			BOOL Info = g_InfoInHtmlStub;
			
			// Set the background radiobuttons.
			if(Bgr == BROWSER_BGR_NONE)
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_PLAINBKGND), TRUE);
			
			else if (Bgr ==  BROWSER_BGR_DOC)
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_DOCBKGND), TRUE);
			
			else if (Bgr == BROWSER_BGR_CHECKER)
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_CHQBKGND), TRUE);     
					
			else if (Bgr == BROWSER_BGR_BITMAP)
			{
				// In the Bitmap Preview diagram, the user selected this radio button. 
				// In the animation properties dialog, this radio button has not been included.
				// We therfore, force the selection to BROWSER_BGR_NONE
				pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_PLAINBKGND), TRUE);	

				// Make a note of this new selection.
				g_Background = BROWSER_BGR_BITMAP;
			}
							
			// Setup the checkboxes.
			pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_HTMLSTUB), Info == TRUE);     

			break;
		}

		case DIM_LFT_BN_CLICKED:
		{
			switch(Msg->GadgetID)
			{
				case _R(IDC_GIF_BITMAPPREVIEW):
				{
					// A Click on the 'Browser Preview' button

					// The op cannot be invoked if the current doc. is empty.
					// Return false if the document is empty.
					String_256 ShadeReason;
					OpState State = OpGrabAllFrames::GetState(&ShadeReason, NULL);

					if (State.Greyed)
					{
						// The current doc, is empty, inform the user.
						InformError(_R(IDS_NOFRAMESTOPREVIEW));
					}
					else
					{
						// Preview the animtion in the browser.
						// Store the current browser options here.
						BrowserPreviewOptions BrowserOptions;

						// Get the browser options
						GetBrowserValues(&BrowserOptions);  	

						// Invoke the OP to preview the animation.
						OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FRAME_BROWSERPREVIEW); 

						if (pOpDesc != NULL)
						pOpDesc->Invoke();
					}
				}
				break;

				case _R(IDC_PLAINBKGND):
				{
					// Set the correct state of the applynow flag.
					PreviewInBrowserTab::SetApplyNowState(TRUE);

					// Ungrey the apply/ok/cancel buttons.
					PreviewInBrowserTab::UngreyApplyNow();
				}
				break;

				case _R(IDC_DOCBKGND):
				{
					// Set the correct state of the applynow flag.
					PreviewInBrowserTab::SetApplyNowState(TRUE);

					// Ungrey the apply/ok/cancel buttons.
					PreviewInBrowserTab::UngreyApplyNow();
				}
				break;

				case _R(IDC_CHQBKGND):
				{
					// Set the correct state of the applynow flag.
					PreviewInBrowserTab::SetApplyNowState(TRUE);

					// Ungrey the apply/ok/cancel buttons.
					PreviewInBrowserTab::UngreyApplyNow();
				}
				break;	

				case _R(IDC_HTMLSTUB):
				{
					// Set the correct state of the applynow flag.
					PreviewInBrowserTab::SetApplyNowState(TRUE);

					// Ungrey the apply/ok/cancel buttons.
					PreviewInBrowserTab::UngreyApplyNow();
				}
				break;
			}
		}
		break;

		case DIM_SET_ACTIVE:
		{
			//Force the  Apply now/ok buttons into a grey state.
			//PreviewInBrowserTab::GreyApplyNow();
			if(PreviewInBrowserTab::GetApplyNowState())
			{
				// Ungrey the apply/ok/cancel buttons.
				PreviewInBrowserTab::UngreyApplyNow();	
			}			
		}			
		break;
	}
	return TRUE;
}  


/********************************************************************************************

>	void PreviewInBrowserTab::GetBrowserValues(BrowserPreviewOptions  *pBrowserOptions)


	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com> (from stefan code)
	Created:	10/06/97
	Inputs:		-
	Outputs:	BrowserOptions - receives the browser options stored in the Browser tab's controls
	Returns:	TRUE if successful, else FALSE.
	Purpose:	- Gets the values of the browser controls.
	
********************************************************************************************/

BOOL PreviewInBrowserTab::GetBrowserValues(BrowserPreviewOptions  *pBrowserOptions)
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"PreviewInBrowserTab::HandleMsg called with a NULL dialog pointer");

/*	// talk to the browser tab
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE; */

	// Take the current settings from the tab
	// Talk to page done in CommitSection
	CommitSection();

	// Force the image map flag to false.
	BOOL Imagemap = FALSE;

	// Make a note of the options.
	pBrowserOptions->Set(g_Background, g_InfoInHtmlStub, Imagemap);

	// Everything ok.
	return TRUE;
}


/********************************************************************************************************************

>	BOOL PreviewInBrowserTab::UpdateApplyState()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	This changes the 'Apply now' state based on the settings of the active spread
				and the settings in this tab.
				If the settings are the same, the 'Apply state' is FALSE, else it is TRUE.
	
**********************************************************************************************************************/

BOOL PreviewInBrowserTab::UpdateApplyState()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"PreviewInBrowserTab::UpdateApplyState called with a NULL dialog pointer");
/*
	// Ensure we talk ot the correct page.
	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;

	// What is the current state of the buttons.
	BOOL ApplyState = GetApplyNow();

	if(!GetCurrentApplyNow())
	{
		// Make a note of the state.
		SetCurrentApplyNow(ApplyState);
	}

	//Force the  Apply now/ok buttons into a grey state.
	PreviewInBrowserTab::GreyApplyNow();

	// Set the status to GREY.
	PreviewInBrowserTab::SetApplyNowState(FALSE); 
*/
	// Everything ok.
	return TRUE;
}

/******************************************************************************************

>	BOOL PreviewInBrowserTab::InitSection()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for this tab on the animation properties dialog box.

******************************************************************************************/

BOOL PreviewInBrowserTab::InitSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"PreviewInBrowserTab::InitSection called with a NULL dialog pointer");

	// Get a ptr to the current document.
	Document* pDoc = Document::GetCurrent();

	// If there is no document, then grey the tab.
	if(!pDoc)
	{
		GreySection();
		return (TRUE);
	}

	// Display the details.
	ShowDetails();

	//Everything ok.
	return TRUE;
}

/********************************************************************************************

>	static BOOL PreviewInBrowserTab::Declare()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Returns:	TRUE if successfully declared preferences
				FALSE otherwise
	Purpose:	To declare preferences associated with the bitmap preview properties.

********************************************************************************************/

BOOL PreviewInBrowserTab::Declare()
{
	if (Camelot.DeclareSection("Browser Preview", 5))
	{
		Camelot.DeclarePref( NULL, "BrowserBackground", (INT32*)&g_Background, 0, 3 );
		Camelot.DeclarePref( NULL, "IncludeImageMap", &g_Imagemap, 0, 1 );
		Camelot.DeclarePref( NULL, "IncludeInfoInHTMLStub", &g_InfoInHtmlStub, 0, 1 );
	}

	// All ok
	return TRUE;
}

						

						
