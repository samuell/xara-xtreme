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
// ToolDlg.CPP
//
// This file implements the dialog box that allows the user to configure which
// toolbars are shown around the edges of the main frame window.

/*
*/

#include "camtypes.h"
//#include "ensure.h"   - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "dlgtypes.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h" // _R(IDB_WINDOWTOOLBARS)  
//#include "barsdlgs.h"
//#include "phil.h"
//#include "mario.h"
//#include "dialogop.h" // DialogOp header - in camtypes.h [AUTOMATICALLY REMOVED]
#include "dlgmgr.h"   // Dialog manager class
#include "tooldlg.h"
//#include "pump.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "mainfrm.h"
#include "csrstack.h"
//#include "infobar.h"
//#include "msg.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "progress.h"

//#include "printdlg.h"
#include "customlist.h"

CC_IMPLEMENT_DYNCREATE(ToolbarDlg, DialogOp)
CC_IMPLEMENT_DYNCREATE(ToolnameDlg, DialogOp)
CC_IMPLEMENT_DYNCREATE(CustomizeBarDlg, DialogOp)

#define new CAM_DEBUG_NEW



const CDlgMode ToolbarDlg::Mode = MODELESS; // The dialog is modeless
const INT32 ToolbarDlg::IDD = _R(IDD_TOOLBARS);
ToolbarDlg* ToolbarDlg::CurrentToolbarDlg = NULL;


String_16 ToolbarDlg::InfoBarName;			// Internationalised name of "infobar"
											// But don't init it here do that in ::Init
											// because it's too early to do it here.

String_16 ToolbarDlg::ControlBankName;
/******************************************************************************************

>	ToolbarDlg::ToolbarDlg(): DialogOp(ToolbarDlg::IDD, ToolbarDlg::Mode) 


	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ToolbarDlg constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

ToolbarDlg::ToolbarDlg(): DialogOp(ToolbarDlg::IDD, ToolbarDlg::Mode) 
{
	CurrentToolbarDlg = this;
}

/******************************************************************************************

>	ToolbarDlg::ToolbarDlg(): DialogOp() 


	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ToolbarDlg destructor sets CurrentToolbarDlg to NULL
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

ToolbarDlg::~ToolbarDlg() 
{
	CurrentToolbarDlg = NULL;
}


/********************************************************************************************

>	void ToolBarDlg::GetSelectedBarName(String_32 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/96

	Outputs:	Result - Will be filled in witht he name of the selected bar
	Purpose:	Get the selected item's name

********************************************************************************************/

void ToolbarDlg::GetSelectedBarName(String_32 *Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");
	*Result = TEXT("");

	CCustomList* pListGadget = CCustomList::GetGadget(GetReadWriteWindowID(), _R(IDC_BARLIST));
	if (pListGadget == NULL)
		return;

	INT32 SelItem = pListGadget->GetSelectedItemIndex();

	if (SelItem >= 0)
		pListGadget->GetItemString(*Result, SelItem, 1);
}



/********************************************************************************************

>	MsgResult ToolbarDlg::Message( CDlgMessage DlgMsg, CGadgetID Gadget)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the Toolbar dialog's messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult ToolbarDlg::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;
		BOOL EndDialog = FALSE;

		switch (Msg->DlgMsg)
		{
			case DIM_CANCEL:
			case DIM_COMMIT:
 				// Cancel all changes the user wants to make to toolbars...
 				EndDialog = TRUE;
 			break;
	
 		 	case DIM_SELECTION_CHANGED:
 		 	case DIM_SELECTION_CHANGED_COMMIT:
				TakeToolbarDetails();
				break;
	
			case DIM_LFT_BN_CLICKED:
			{	// A control in the Toolbars... dialog has been clicked...
			 	CMainFrame* pFrame = GetMainFrame();
				BOOL	check;
				BOOL	VV;
				switch (Msg->GadgetID)
				{
					case _R(IDC_BIGALL):
						// Select all the large buttons...
						SetLongGadgetValue(_R(IDC_BIGTOP),1);
						SetLongGadgetValue(_R(IDC_BIGLEFT),1);
						SetLongGadgetValue(_R(IDC_BIGRIGHT),1);
						SetLongGadgetValue(_R(IDC_BIGBOTTOM),1);
						SetLongGadgetValue(_R(IDC_BIGFLOAT),1);
						TakeToolbarDetails();
					break;

					case _R(IDC_BIGNONE):
						// Deselect all the large buttons...
						SetLongGadgetValue(_R(IDC_BIGTOP),0);
						SetLongGadgetValue(_R(IDC_BIGLEFT),0);
						SetLongGadgetValue(_R(IDC_BIGRIGHT),0);
						SetLongGadgetValue(_R(IDC_BIGBOTTOM),0);
						SetLongGadgetValue(_R(IDC_BIGFLOAT),0);
						TakeToolbarDetails();
					break;

					case _R(IDC_NEWBAR):
						NewToolbar();
						break;

					case _R(IDC_BIGTOP):
						check = (BOOL)GetLongGadgetValue(_R(IDC_BIGTOP),0,1,_R(IDS_PHILS_EXAMPLE),&VV);
						pFrame->GetDockBar(DOCKBAR_TOP)->SetBigControlState(check);
						break;
					case _R(IDC_BIGLEFT):
						check = (BOOL)GetLongGadgetValue(_R(IDC_BIGLEFT),0,1,_R(IDS_PHILS_EXAMPLE),&VV);
						pFrame->GetDockBar(DOCKBAR_LEFT)->SetBigControlState(check);
						break;
					case _R(IDC_BIGRIGHT):
						check = (BOOL)GetLongGadgetValue(_R(IDC_BIGRIGHT),0,1,_R(IDS_PHILS_EXAMPLE),&VV);
						pFrame->GetDockBar(DOCKBAR_RIGHT)->SetBigControlState(check);
						break;
					case _R(IDC_BIGBOTTOM):
						check = (BOOL)GetLongGadgetValue(_R(IDC_BIGBOTTOM),0,1,_R(IDS_PHILS_EXAMPLE),&VV);
						pFrame->GetDockBar(DOCKBAR_BOTTOM)->SetBigControlState(check);
						break;
					case _R(IDC_BIGFLOAT):	
						check = (BOOL)GetLongGadgetValue(_R(IDC_BIGFLOAT),0,1,_R(IDS_PHILS_EXAMPLE),&VV);
						pFrame->GetDockBar(DOCKBAR_FLOAT)->SetBigControlState(check);
						break;
					
				
					case _R(IDC_DELETEBAR):		
					{	
						String_32 BarName;
						GetSelectedBarName(&BarName);

						if (BarName != String_8(TEXT("")))
						{
		 					// we can't delete the infoobar
							if(BarName == String_32(TEXT("Infobar")))
							{
								InformWarning(_R(IDS_CANTDELETEINFOBAR),_R(IDS_OK));
								break;
							}

							INT32 ButtonPressed = 1;
							ButtonPressed = InformWarning(_R(IDS_WARNDELETEBAR), _R(IDS_OK),_R(IDS_CANCEL));
							Error::ClearError();
							if (ButtonPressed ==  1)
								DeleteSelectedBar();
						}
						break;	
					}

					case _R(IDC_RESETBARS):		
					{	
						INT32 ButtonPressed = 1;						
						ButtonPressed = InformWarning(_R(IDS_WARNRESETBARS), _R(IDS_OK),_R(IDS_CANCEL));
						Error::ClearError();

						if(ButtonPressed ==  1)
						{
					  		Progress::Start(FALSE);
							// lock the window to avoid untidy layout recalcs
							::LockWindowUpdate(GetMainFrame()->GetSafeHwnd());

							// tell the mainframe we are changing modes - this prevents
							// some unnecessary work
							GetMainFrame()->SetChangingMode(TRUE);

							// kill all bars ( except infobar at the mo..)
							BROADCAST_TO_CLASS(DialogMsg(NULL, DIM_BAR_DEATH, NULL ),DialogBarOp);
							// hide the info bar
							InformationBarOp::SetVisibility(FALSE,TRUE);

							DialogBarOp::WipeBarFiles();

							DialogBarOp::LoadDefaultBars();
							
							// force the info bar to the correct visible state
							InformationBarOp::SetVisibility(InformationBarOp::IsVisible(),TRUE);
							// All clear to Mainframe
							GetMainFrame()->SetChangingMode(FALSE);

							// Make sure everything is positioned correctly
							GetMainFrame()->RecalcLayout();

							// Now handle the toolbar ....
							GetMainFrame ()->SetToolbarCreated (TRUE);
							GetMainFrame ()->RelocateToolbar ();

							// and unlock and show the window
							::LockWindowUpdate(NULL);
							ShowToolbarList();
							ShowToolbarSizes();
							Progress::Stop();
						}
						break;			
					}
				}
				break;
			}
		}

		if (EndDialog)	// Dialog communication over
		{

			Close();	// Close the dialog
			End();		// Destroy dialog
			return(OK);	// After destroying this object, it's very dangerous to call base class!
	   	}
	}
	else if ( MESSAGE_IS_A(Message,BarMsg) )
	{
		switch ( ((BarMsg*)Message)->Reason )
		{
			case BAR_CHANGESTATE:
				// A bar simply became horizontal, vertical or floating and we don't care
				// about that so do nothing.
				break;

			default:
				// In most cases of BarMsg we want to show the user what happened...
				ShowToolbarList();		// Show changes in bars to user
										// Note: it would be more optimal to use the pointer
										// to the bar that comes with this message to just
										// alter that item in the list.
				break;
		}
	}

	return DialogOp::Message(Message);
}




/******************************************************************************************

>	OpState	ToolbarDlg::GetState(String_256*, OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ToolbarDlg GetState method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

OpState	ToolbarDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}




/******************************************************************************************

>	BOOL ToolbarDlg::Init()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ToolbarDlg Init method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL ToolbarDlg::Init()
{
	BOOL InitOK;

	// Initialise the contents of the static InfoBarName variable here...
	String_16 Blobby(_R(IDS_INFOBARNAME));
	InfoBarName = Blobby;

	String_16 Noel(_R(IDS_CONTROLBANKNAME));
	ControlBankName = Noel;

	InitOK = RegisterOpDescriptor(
								0,	/* Tool ID */
								_R(IDS_TOOLBARS_DLG),
								CC_RUNTIME_CLASS(ToolbarDlg),
								OPTOKEN_TOOLBARDLG,
								ToolbarDlg::GetState,
								0,					/* help ID */
								0,  				/* bubble help ID _R(IDBBL_LAYERDLG), */
								_R(IDB_WINDOWTOOLBARS),	/* resource ID */
								0,					/* control ID */
								SYSTEMBAR_ILLEGAL,				// Bar ID
								FALSE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								0,								// No vertical counterpart
								_R(IDS_BARSINFO_ONE)				// String for one copy only
								
								)
			&& RegisterOpDescriptor(
								0,
								_R(IDS_TOOLBARNAME_DLG),
								CC_RUNTIME_CLASS(ToolnameDlg),
								OPTOKEN_TOOLBARNAMEDLG,
								ToolnameDlg::GetState,
								0,	/* help ID */
								0,  /* bubble help ID _R(IDBBL_LAYERDLG), */
								0	/* bitmap ID */
								)
			&& RegisterOpDescriptor(
								0,
								_R(IDS_CUSTOMIZE_BAR_DLG),
								CC_RUNTIME_CLASS(CustomizeBarDlg),
								OPTOKEN_CUSTOMIZEBARDLG,
								CustomizeBarDlg::GetState,
								0,	/* help ID */
								0,  /* bubble help ID _R(IDBBL_LAYERDLG), */
								0	/* bitmap ID */
								)

			;

	return (InitOK);
}




/********************************************************************************************

>	BOOL ToolbarDlg::Create()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	ToolbarDlg create method 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

	   
BOOL ToolbarDlg::Create()
{                            
	if (DialogOp::Create())
	{ 
		// Set the initial control values 
		InitDialog();
		return TRUE; 
    }
    else
    {
		return FALSE; 
	}
}




/******************************************************************************************

>	void ToolbarDlg::Do(OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void ToolbarDlg::Do(OpDescriptor*)
{
	if (Create())
	{
		Open();
	}
	else			  // Could not create 
	{
		InformError();
  		End(); 		   // End the operation
	}
}




/******************************************************************************************

>	BOOL ToolbarDlg::InitDialog()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets initial dialog values 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL ToolbarDlg::InitDialog()
{
	// Make the OK button dual function
//	DualFunctionButton(IDOK);

	ShowToolbarList();
	ShowToolbarSizes();

  	return TRUE;
}




/******************************************************************************************

>	void ToolbarDlg::ShowToolbarList()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/04/94
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Shows Toolbar info in the Toolbars... dialog.
				Ignores all SystemBarOps and all InformationBarOps.
				Adds one line specifically to cover all InformationBarOps.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void ToolbarDlg::ShowToolbarList()
{
	if (!WindowID)
		return;

	// set the custom list gadget 
	CCustomList* pListGadget = CCustomList::GetGadget(GetReadWriteWindowID(), _R(IDC_BARLIST));
	if(!pListGadget)
	{
		ERROR2RAW("No list gadget");
		return;
	}
	pListGadget->SetColumnWidth(0,GetSystemMetrics(SM_CXMENUCHECK) + 10);

	CMainFrame* pFrame = GetMainFrame();
	ERROR3IF(pFrame == NULL, "Can't find Main Frame in ShowToolBarList");

	if(pFrame->IsChangingViewMode())
		return ;

	// Fetch the names and states of all defined bars...
	List* pBarList = MessageHandler::GetClassList(CC_RUNTIME_CLASS(DialogBarOp));
	ENSURE(pBarList,"Can't find list of DialogBarOps in ShowToolBarList");

	// Delete any existing list items...
	pListGadget->DeleteAllItems();

	// Add names of all bars to the list...
	// First do the infobar...
	pListGadget->AddItem(InfoBarName);
	pListGadget->SetSwitchState(InformationBarOp::IsVisible(), (pListGadget->GetItemCount() - 1), 0);

	// Scan the list of all barops and add the names of those which should be shown
	// to the user to the scrollable list.
	DialogBarOp* pBar = (DialogBarOp*)pBarList->GetHead();
	
	BOOL FoundControlBank = FALSE;
	BOOL ControlBankVisible = FALSE;
	while (pBar)
	{
		// Show any bar that's NOT a system bar and NOT an information bar
		// and NOT a Super Gallery put the Controlbank at the end
		if ( IS_A(pBar,DialogBarOp) )
		{
			// Put its name in the list
			String_32 BarName = pBar->GetName();
			if (BarName != ControlBankName)
			{	
				pListGadget->AddItem(BarName);
				pListGadget->SetSwitchState(pBar->IsVisible(), (pListGadget->GetItemCount() - 1), 0);
			}
			else
			{
				FoundControlBank = TRUE;
				ControlBankVisible = pBar->IsVisible();
			}
		}
		pBar = (DialogBarOp*)pBarList->GetNext(pBar);
	}
	
	if (FoundControlBank)
	{
		pListGadget->AddItem(ControlBankName);
		pListGadget->SetSwitchState(ControlBankVisible, (pListGadget->GetItemCount() - 1), 0);
	}
}



/******************************************************************************************

>	void ToolbarDlg::ShowToolbarSizes()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/94
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Shows Toolbar info in the Toolbars... dialog.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void ToolbarDlg::ShowToolbarSizes()
{
	CMainFrame* pFrame = GetMainFrame();
	ENSURE(pFrame,"Can't find Main Frame in ShowToolBarSizes");

	// Only attempt to redisplay things if the toolbars dlg exists...
	if (WindowID)
	{
		// Fetch the states of all the docking bars...
		SetLongGadgetValue(_R(IDC_BIGTOP),		(INT32) pFrame->GetDockBar(DOCKBAR_TOP)->IsBigControls());
		SetLongGadgetValue(_R(IDC_BIGLEFT),		(INT32) pFrame->GetDockBar(DOCKBAR_LEFT)->IsBigControls());
		SetLongGadgetValue(_R(IDC_BIGRIGHT),	(INT32) pFrame->GetDockBar(DOCKBAR_RIGHT)->IsBigControls());
		SetLongGadgetValue(_R(IDC_BIGBOTTOM),	(INT32) pFrame->GetDockBar(DOCKBAR_BOTTOM)->IsBigControls());
		SetLongGadgetValue(_R(IDC_BIGFLOAT),	(INT32) pFrame->GetDockBar(DOCKBAR_FLOAT)->IsBigControls());
	}
}



/******************************************************************************************

>	BOOL ToolbarDlg::CanDeleteSelection()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/96

	Returns:	TRUE if it is safe to delete the current selection
	Purpose:	-

******************************************************************************************/

BOOL ToolbarDlg::CanDeleteSelection()
{
	String_32 BarName;
	GetSelectedBarName(&BarName);

	if (BarName == String_8(TEXT("")))		// No selection to delete
		return(FALSE);

	if (BarName == InfoBarName)				// Don't delete InfoBar
		return(FALSE);

   	if (BarName == ControlBankName)			// Don't delete button palette
		return(FALSE);

	return(TRUE);
}



/******************************************************************************************

>	void ToolbarDlg::DeleteSelectedBar()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/04/94
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Delete the Bar that is currently selected in the bar listbox 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void ToolbarDlg::DeleteSelectedBar()
{
	// we can't delete the control bank !!
   	if (!CanDeleteSelection())
		return;

	String_32 BarName;
	GetSelectedBarName(&BarName);

	// If nothing is selected, we can't do anything
	if (BarName == String_8(TEXT("")))
		return;
	
	// we can't delete the infoobar
	if (BarName == String_32(TEXT("Infobar")))
		return;

	DialogBarOp* pBar = DialogBarOp::FindDialogBarOp(BarName);

	ENSURE(pBar,"Cannot find named bar in TakeToolbarDetails");

	// If bar is of correct type then adjust its visibility according to the
	// check mark...
	if ( !( pBar->IsKindOf(CC_RUNTIME_CLASS(SystemBarOp)) ||
			pBar->IsKindOf(CC_RUNTIME_CLASS(InformationBarOp))
	       )
		)
	{
		pBar->Delete();
		pBar->End();
		ShowToolbarList();
	}
}



/******************************************************************************************

>	void ToolbarDlg::TakeToolbarDetails()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/04/94
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Take current state from the Toolbars... dialog.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void ToolbarDlg::TakeToolbarDetails()
{
	EnableGadget(_R(IDC_DELETEBAR), CanDeleteSelection());

	CMainFrame* pFrame = GetMainFrame();

	BOOL	check;
	BOOL	VV;

	check = (BOOL)GetLongGadgetValue(_R(IDC_BIGTOP),0,1,_R(IDS_PHILS_EXAMPLE),&VV);
	pFrame->GetDockBar(DOCKBAR_TOP)->SetBigControlState(check);

	check = (BOOL)GetLongGadgetValue(_R(IDC_BIGLEFT),0,1,_R(IDS_PHILS_EXAMPLE),&VV);
	pFrame->GetDockBar(DOCKBAR_LEFT)->SetBigControlState(check);

	check = (BOOL)GetLongGadgetValue(_R(IDC_BIGRIGHT),0,1,_R(IDS_PHILS_EXAMPLE),&VV);
	pFrame->GetDockBar(DOCKBAR_RIGHT)->SetBigControlState(check);

	check = (BOOL)GetLongGadgetValue(_R(IDC_BIGBOTTOM),0,1,_R(IDS_PHILS_EXAMPLE),&VV);
	pFrame->GetDockBar(DOCKBAR_BOTTOM)->SetBigControlState(check);

	check = (BOOL)GetLongGadgetValue(_R(IDC_BIGFLOAT),0,1,_R(IDS_PHILS_EXAMPLE),&VV);
	pFrame->GetDockBar(DOCKBAR_FLOAT)->SetBigControlState(check);


	CCustomList* pListGadget = CCustomList::GetGadget(GetReadWriteWindowID(), _R(IDC_BARLIST));
	if (pListGadget == NULL)
		return;
	
	// Adjust the visibility of the bars...
	INT32 listitem = 0;
	INT32 listcount = pListGadget->GetItemCount();
	String_32 BarName;
		
	// variables control the dynamic relocation of the 'floating' toolbar ....
//	INT32 ToolbarIndex = -1;
//	BOOL BeenHidden = FALSE;
//	BOOL ToolbarCurrentState = FALSE;
	
	while (listitem < listcount)
	{
		// Find bar by name...
		pListGadget->GetItemString(BarName, listitem, 1);

		// If name is "Infobar" deal with it specially...
		if (BarName == InfoBarName)
			InformationBarOp::SetVisibility(pListGadget->GetSwitchState(listitem, 0));
		else if (BarName == String_32 (TEXT ("Toolbar")))
		{
			// do special stuff now ....

			DialogBarOp* pBar = DialogBarOp::FindDialogBarOp(BarName);
			ERROR3IF(pBar == NULL, "Cannot find named bar in TakeToolbarDetails");

			// If bar is of correct type then adjust its visibility according to the
			// check mark...
			if (!pBar->IsKindOf(CC_RUNTIME_CLASS(SystemBarOp)) ||
				!pBar->IsKindOf(CC_RUNTIME_CLASS(InformationBarOp)) )
			{
				//ToolbarIndex = listitem;
				
				BOOL CurrentState = pBar->IsVisible ();
				//ToolbarCurrentState = CurrentState;
				pBar->SetVisibility(pListGadget->GetSwitchState(listitem, 0));

				if (pListGadget->GetSwitchState(listitem, 0) != CurrentState)
				{	
					if (pListGadget->GetSwitchState(listitem, 0))
					{
						GetMainFrame ()->SetToolbarCreated (TRUE);
						GetMainFrame ()->RelocateToolbar ();
					}
					else
					{
						// so that we can lock out the toolbar 'onmove' and 'onsize' code
						// from being executed when there is no toolbar
						GetMainFrame ()->SetToolbarCreated (FALSE);
					}
				}
			//	else
			//	{
			//		BeenHidden = TRUE;
			//	}
			}
		}
		else
		{
			// Look for the named bar in the barops list...
			DialogBarOp* pBar = DialogBarOp::FindDialogBarOp(BarName);
			ERROR3IF(pBar == NULL, "Cannot find named bar in TakeToolbarDetails");

			// If bar is of correct type then adjust its visibility according to the
			// check mark...
			if (!pBar->IsKindOf(CC_RUNTIME_CLASS(SystemBarOp)) ||
				!pBar->IsKindOf(CC_RUNTIME_CLASS(InformationBarOp)) )
			{
				pBar->SetVisibility(pListGadget->GetSwitchState(listitem, 0));
			}
		}

		listitem++;
	}

	// dynamically reposition the toolbar when the above options are changed ....
	
/*	if ((ToolbarIndex != -1) && (!BeenHidden))
	{
		if (pListGadget->GetSwitchState(ToolbarIndex, 0) != ToolbarCurrentState)
		if (pListGadget->GetSwitchState(ToolbarIndex, 0))
		{
			GetMainFrame ()->SetToolbarCreated (TRUE);
			GetMainFrame ()->RelocateToolbar ();
		}
	}*/
}




/******************************************************************************************

>	void ToolbarDlg::NewToolbar()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Make a new toolbar...
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void ToolbarDlg::NewToolbar()
{
	// Invoke the new toolbar name operation
	// Obtain a pointer to the op descriptor for the new toolbar name operation
	OpDescriptor* pOpDesc =
		OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(ToolnameDlg));
	ENSURE(pOpDesc,"Can't find ToolnameDlg Op.");
	// Invoke the operation.
	pOpDesc->Invoke();
}



/******************************************************************************************

>	ToolbarDlg* ToolbarDlg::GetToolbarDlg()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/94
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Make a new toolbar...
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

ToolbarDlg* ToolbarDlg::GetToolbarDlg()
{
	return(CurrentToolbarDlg);
}













/******************************************************************************************
******************************************************************************************/

const CDlgMode ToolnameDlg::Mode = MODAL; // The dialog is modeless
const INT32 ToolnameDlg::IDD = _R(IDD_NEWTOOLBAR);

/******************************************************************************************

>	ToolnameDlg::ToolnameDlg(): DialogOp(ToolnameDlg::IDD, ToolnameDlg::Mode) 


	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ToolnameDlg constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

ToolnameDlg::ToolnameDlg()
  : DialogOp(ToolnameDlg::IDD, ToolnameDlg::Mode) 
{
//	pToolbarDlg = NULL;
}




/********************************************************************************************

>	MsgResult ToolnameDlg::Message( CDlgMessage DlgMsg, CGadgetID Gadget)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the Toolbar dialog's messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult ToolnameDlg::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{

		DialogMsg* Msg = (DialogMsg*)Message; 
		BOOL EndDialog = FALSE; 
		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
			    SetKeyboardFocus (_R(IDC_EDITNEWBARNAME)) ;
				HighlightText    (_R(IDC_EDITNEWBARNAME)) ;
				break ;

			case DIM_COMMIT:
 			{
 				// Accept all changes that the user wants to make to toolbars...
 				BOOL VV;
 				String_32 NewBarName = GetStringGadgetValue(_R(IDC_EDITNEWBARNAME),&VV);
				if(NewBarName.Length()==0)
				{
				  	UINT32 Number = DialogBarOp::FindUniqueBarNumber();	
					NewBarName._MakeMsg(TEXT("Bar #1%ld"),Number); 
				}
				DialogBarOp* pNewBar = new DialogBarOp(NewBarName);
 				if (pNewBar)
 				{
 					pNewBar->SetDockBarType(DOCKBAR_FLOAT);
 					pNewBar->Create();
					// The Create function sends a message which we intercept to update
					// the list of bars in the Toolbars... dialog.
					// this is very dangerous in fact it blows up..
					// so I have removed it .. chris.
					/*	if (pNewBar)
					{
						// Find toolbar dialog and close it...
 						ToolbarDlg* pToolbarDlg = ToolbarDlg::GetToolbarDlg();
						if (pToolbarDlg)
						{
							pToolbarDlg->Close();
							pToolbarDlg->End();
						}
					}
					*/
					ToolbarDlg* pToolbarDlg = ToolbarDlg::GetToolbarDlg();
					pToolbarDlg->ShowToolbarList();
 				}
 				EndDialog = TRUE;
			
 			}
 			break;

 			case DIM_CANCEL:
 				// Cancel all changes the user wants to make to toolbars...
 				EndDialog = TRUE;
	 			break;

//			default:
		}

		if (EndDialog)	// Dialog communication over 
		{
			Close();	// Close the dialog 
			End();		// Destroy dialog 
	   	}

//		return (DLG_EAT_IF_HUNGRY(Msg)); 
	}

//	return OK; 

	// Pass everything on to the base-class . . .
	return DialogOp::Message(Message);
}




/******************************************************************************************

>	OpState	ToolnameDlg::GetState(String_256*, OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ToolnameDlg GetState method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

OpState	ToolnameDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}




/******************************************************************************************

>	BOOL ToolnameDlg::Init()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ToolnameDlg Init method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL ToolnameDlg::Init()
{  
	return (RegisterOpDescriptor(
								0,
								_R(IDS_TOOLBARNAME_DLG),
								CC_RUNTIME_CLASS(ToolnameDlg),
								OPTOKEN_TOOLBARNAMEDLG,
								ToolnameDlg::GetState,
								0,	/* help ID */
								0,  /* bubble help ID _R(IDBBL_LAYERDLG), */
								0	/* bitmap ID */));
}




/********************************************************************************************

>	BOOL ToolnameDlg::Create()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	ToolnameDlg create method 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

	   
BOOL ToolnameDlg::Create()
{                            
	if (DialogOp::Create())
	{ 
		// Set the initial control values 
		InitDialog();

		return TRUE; 
    }
    else
    {
		return FALSE; 
	}
}




/******************************************************************************************

>	void ToolnameDlg::Do(OpDescriptor* pOpDesc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void ToolnameDlg::Do(OpDescriptor* pOpDesc)
{
	if (Create())	
	{
//		pToolbarDlg = pDlg;
	//	Open();
	}
	else			  // Could not create 
	{
		InformError();
  		End(); 		   // End the operation 
	}
}




/******************************************************************************************

>	BOOL ToolnameDlg::InitDialog()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets initial dialog values 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL ToolnameDlg::InitDialog()
{
  	return TRUE;
}













/******************************************************************************************
******************************************************************************************/

const CDlgMode CustomizeBarDlg::Mode = MODELESS; // The dialog is modeless
const INT32 CustomizeBarDlg::IDD = _R(IDD_CUSTOMIZEBAR);

/******************************************************************************************

>	CustomizeBarDlg::CustomizeBarDlg(): DialogOp(CustomizeBarDlg::IDD, CustomizeBarDlg::Mode) 


	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	CustomizeBarDlg constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CustomizeBarDlg::CustomizeBarDlg()
  : DialogOp(CustomizeBarDlg::IDD, CustomizeBarDlg::Mode) 
{
	// Empty.
}




/********************************************************************************************

>	MsgResult CustomizeBarDlg::Message( CDlgMessage DlgMsg, CGadgetID Gadget)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the Toolbar dialog's messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult CustomizeBarDlg::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message; 
		BOOL EndDialog = FALSE; 

		switch (Msg->DlgMsg)
		{
		 	case DIM_COMMIT:
			{
				EndDialog = TRUE;
			}
			break;

			case DIM_CANCEL:
				// Cancel all changes the user wants to make to toolbars...
				EndDialog = TRUE;
			break;

//			default:
		}

		if (EndDialog)	// Dialog communication over 
		{
			Close(); // Close the dialog 
			End(); 	// Destroy dialog 
	   	}
//		return (DLG_EAT_IF_HUNGRY(Msg)); 
	}
//	return OK;

	// Pass all messages on to the base class for final handling . . .
	return DialogOp::Message(Message);
}




/******************************************************************************************

>	OpState	CustomizeBarDlg::GetState(String_256*, OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	CustomizeBarDlg GetState method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

OpState	CustomizeBarDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}




/******************************************************************************************

>	BOOL CustomizeBarDlg::Init()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	CustomizeBarDlg Init method
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL CustomizeBarDlg::Init()
{  
	return (RegisterOpDescriptor(
								0,
								_R(IDS_CUSTOMIZE_BAR_DLG),
								CC_RUNTIME_CLASS(CustomizeBarDlg),
								OPTOKEN_CUSTOMIZEBARDLG,
								CustomizeBarDlg::GetState,
								0,	/* help ID */
								0,  /* bubble help ID _R(IDBBL_LAYERDLG), */
								0	/* bitmap ID */));
}




/********************************************************************************************

>	BOOL CustomizeBarDlg::Create()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	CustomizeBarDlg create method 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

	   
BOOL CustomizeBarDlg::Create()
{                            
	if (DialogOp::Create())
	{ 
		// Set the initial control values 
		InitDialog();

		return TRUE; 
    }
    else
    {
		return FALSE; 
	}
}




/******************************************************************************************

>	void CustomizeBarDlg::Do(OpDescriptor* pOpDesc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog in response to a request from the user.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void CustomizeBarDlg::Do(OpDescriptor* pOpDesc)
{
	if (Create())
	{
//		pToolbarDlg = pDlg;
		Open();
	}
	else			  // Could not create 
	{
		InformError();
  		End(); 		   // End the operation 
	}
}




/******************************************************************************************

>	BOOL CustomizeBarDlg::InitDialog()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets initial dialog values 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL CustomizeBarDlg::InitDialog()
{
	ShowOpsList();
  	return TRUE;
}




/******************************************************************************************

>	void CustomizeBarDlg::ShowOpsList()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets initial dialog values 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void CustomizeBarDlg::ShowOpsList()
{
	OpListItem* pOpListItem = OpDescriptor::GetFirstDescriptor();
	OpDescriptor* pCurrentOpDesc;

	ENSURE(pOpListItem,"Wot, no OpDescriptors?");

	while (pOpListItem)
	{
		pCurrentOpDesc = pOpListItem->pOpDesc;
		ENSURE(pCurrentOpDesc,"OpListitem doesn't point to OpDescriptor in ShowOpsList");

		SetStringGadgetValue(_R(IDC_OPSLIST),&(pCurrentOpDesc->Token),TRUE,0);

		pOpListItem = OpDescriptor::GetNextDescriptor(pOpListItem);
	}

}
