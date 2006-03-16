// $Id: barcreationdlg.cpp 666 2006-03-15 12:11:56Z luke $
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
#include "spread.h"
#include "barcreationdlg.h"		//This file's header
#include "menuops.h"	
#include "ccdc.h"			// specific #includes needed for kernel-rendered dialogues
//#include "barsdlgs.h"		// _R(IDD_BARCONTROLSTORE)
//#include "simon.h"			// contians the resources of the dlg
//#include "sliceres.h"		// more resources
//#include "resimmap.h"
#include "nodetxts.h"
#include "sprdmsg.h"	// SpreadMsg
#include "slice.h" // so export can invoke this op
#include "opbarcreation.h" // to invoke the create op
#include "slicetool.h" // for the findstate static fn

// these are for the template attribute definitions
// required for the scanning for existing bar attribs
#include "cxfrech.h"
#include "userattr.h"
#include "tmpltatr.h"

#include "layermsg.h"	// the layer messaging

// named set stuff
#include "ngcore.h"
#include "ngitem.h"
#include "ngsentry.h"
#include "ngsetop.h"

//#include "resource.h" // for _R(IDS_CANCEL)

#include "xshelpid.h"


CC_IMPLEMENT_DYNCREATE(BarCreationDlg, DialogOp);
CC_IMPLEMENT_DYNCREATE(BarStatesDlg, DialogOp);
CC_IMPLEMENT_DYNCREATE(BarRedefineStateDlg, DialogOp);

#define new CAM_DEBUG_NEW

#define DLG_TIMER	2020

// Set up our static variables
const CDlgMode BarCreationDlg::Mode = MODELESS;
const UINT32 BarCreationDlg::IDD = _R(IDD_BARCREATIONDLG);

BarCreationDlg *BarCreationDlg::TheDialog = NULL;	


// statics to hold the status of the controls
// so when the dlg is closed and re-openned they
// remember what they were
BOOL BarCreationDlg::ms_WantDefault = TRUE; 
BOOL BarCreationDlg::ms_WantMouse = FALSE; 
BOOL BarCreationDlg::ms_WantClicked = FALSE; 
BOOL BarCreationDlg::ms_WantSelected = FALSE; 
BOOL BarCreationDlg::ms_IsVertical = TRUE; 
BOOL BarCreationDlg::ms_SuggestDesign = TRUE;
BOOL BarCreationDlg::ms_Continuous = TRUE;
BOOL BarCreationDlg::ms_FromSelection = TRUE;
INT32 BarCreationDlg::ms_StateToCreate = 0;
INT32 BarCreationDlg::ms_SizeJustification = 0;
 
INT32 BarCreationDlg::ms_NoOfButtons = 4; 
INT32 BarCreationDlg::ms_Spacing = 0; 
INT32 BarCreationDlg::ms_Mode = NEW_BAR; 
INT32 BarCreationDlg::ms_Stretching = 0;
BOOL BarCreationDlg::ms_GroupsStretch = TRUE;

/*******************************************************************************************

>	BarCreationDlg::BarCreationDlg() : DialogOp(RenderDlg::IDD, RenderDlg::Mode) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> from Alex
	Created:	13/8/99
	Purpose:	Constructor

*******************************************************************************************/

BarCreationDlg::BarCreationDlg() : DialogOp(BarCreationDlg::IDD, BarCreationDlg::Mode) 
{
	//Set our member variable pointer so it points at ourself
	if (TheDialog == NULL)
		TheDialog = this;

	m_IsEditing = FALSE;
	m_FirstIsEditing = FALSE;
	m_AlreadyInThisFunction = FALSE;
	m_HasBackBar = FALSE;
	m_ValidBar = TRUE;
}       



/*******************************************************************************************

>	BarCreationDlg::~BarCreationDlg()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> from Alex
	Created:	13/8/99
	Purpose:	Destructor

*******************************************************************************************/

BarCreationDlg::~BarCreationDlg()
{
	//Set our member variable pointer to NULL
	if (TheDialog == this)
		TheDialog = NULL;
} 
 
/********************************************************************************************
>	MsgResult BarCreationDlg::Message( Msg* Message)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/8/99
	Inputs:		Message: The incoming message 
		
	Returns:	DialogOp::Message (Message)

	Purpose:	This is the message handler for the BarCreationDlg. It simply takes the
				message and redirects it to one of our Message Handler functions.
	 
********************************************************************************************/

MsgResult BarCreationDlg::Message( Msg* Message)
{
	BOOL fEndDialog=FALSE;

	//If the message is a dialog message
	if (IS_OUR_DIALOG_MSG(Message))
	{
		//Then work out what it says
		DialogMsg* pDialogMsg = ((DialogMsg*) Message); 
	
		switch (pDialogMsg->DlgMsg)
		{
			case DIM_CREATE:
				SetStringGadgetValue(_R(IDC_BC_DIRECTION), _R(IDS_HORIZ), FALSE, 0);
				SetStringGadgetValue(_R(IDC_BC_DIRECTION), _R(IDS_VERT), FALSE, 1);
				SetStringGadgetValue(_R(IDC_BC_DIRECTION), _R(IDS_SCATTERED), TRUE, 2);

				SetStringGadgetValue(_R(IDC_BC_BUTTON_SIZE), _R(IDS_EXTENDS), FALSE, 0);
				SetStringGadgetValue(_R(IDC_BC_BUTTON_SIZE), _R(IDS_SCALES), FALSE, 1);
				SetStringGadgetValue(_R(IDC_BC_BUTTON_SIZE), _R(IDS_IS_STATIC), TRUE, 2);

				SetStringGadgetValue(_R(IDC_BC_BUTTONS_SAME), _R(IDS_BUTTONS_SAME_SIZE), FALSE, 0);
				SetStringGadgetValue(_R(IDC_BC_BUTTONS_SAME), _R(IDS_BUTTONS_LEFT_SIZE), FALSE, 1);
				SetStringGadgetValue(_R(IDC_BC_BUTTONS_SAME), _R(IDS_BUTTONS_RIGHT_SIZE), FALSE, 2);
				SetStringGadgetValue(_R(IDC_BC_BUTTONS_SAME), _R(IDS_BUTTONS_DIFF_SIZE), TRUE, 3);
				
				SetUpControls();

				// throw the keyboard focus
				//DialogManager::DefaultKeyboardFocus();
				break;
	
			// these two messages are used to throw the focus back to the main window
			case DIM_TITLEFOCUSWARN:
				//DialogManager::DefaultKeyboardFocus();
				break;

			// focus throw
			case DIM_LFT_BN_UP:
				//DialogManager::DefaultKeyboardFocus();
				break;

			case DIM_CANCEL:	
				fEndDialog = TRUE;
				break;

			// special case to find the one typed into the field
			case DIM_TIMER:
				{
					KillTimer(DLG_TIMER);
					INT32 NewNoOfButtons = GetLongGadgetValue(_R(IDC_BC_NO_OF_BUTTONS), 1, MAX_BUTTONS_IN_A_BAR);

					if (NewNoOfButtons == 1)
					{
						OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SHORTENBAROP); 
						if (pOpDesc != NULL)
						{
							OpParamShortenBar ParamShortenBar(m_BarName, NewNoOfButtons);
							pOpDesc->Invoke(&ParamShortenBar);
							// update the dialog value
							ms_NoOfButtons = NewNoOfButtons;
							SetLongGadgetValue(_R(IDC_BC_NO_OF_BUTTONS), ms_NoOfButtons);
						}
						else
						{
							ERROR3("Couldn't find OPTOKEN_SHORTENBAROP op descriptor");
						}
					}
				}
				break;

			case DIM_TEXT_CHANGED:
				if (m_IsEditing)
					switch(pDialogMsg->GadgetID)
				{
				case _R(IDC_BC_SPACING):
					{
					// the text has changed for the spacing field
						INT32 spacing = 0;
						BOOL Valid=0;
						MILLIPOINT ActualSpacing = GetUnitGadgetValue(_R(IDC_BC_SPACING), PIXELS, -75000, 75000, 0, &Valid);
						String_256 ActualText = GetStringGadgetValue(_R(IDC_BC_SPACING));
						spacing = ActualSpacing;


						if (Valid && !ActualText.IsEmpty() && ms_Spacing != spacing)
						{
							ms_Spacing = spacing;

							if (m_IsEditing)
								WriteBarProperties();
						}
					}
					//DialogManager::DefaultKeyboardFocus(); // throws focus
					break;

				case _R(IDC_BC_NO_OF_BUTTONS):
					{
					// the text has changed for the spacing field
						INT32 NewNoOfButtons = GetLongGadgetValue(_R(IDC_BC_NO_OF_BUTTONS), 1, MAX_BUTTONS_IN_A_BAR);

						// get what the set abr props are then
						ReadBarProperties();

						if (NewNoOfButtons > ms_NoOfButtons && NewNoOfButtons <= MAX_BUTTONS_IN_A_BAR)
						{
							OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BARDUPLICATIONOP); 
							if (pOpDesc != NULL)
							{
								OpParamBarDuplication AddButtonsParam(m_BarName, m_BarName, NewNoOfButtons - ms_NoOfButtons, ms_Spacing, !ms_IsVertical);
								pOpDesc->Invoke(&AddButtonsParam);

								// update the dialog value
								ms_NoOfButtons = NewNoOfButtons;
								SetLongGadgetValue(_R(IDC_BC_NO_OF_BUTTONS), ms_NoOfButtons);
							}
							else
							{
								ERROR3("Couldn't find OPTOKEN_BARDUPLICATIONOP op descriptor");
							}
						}
						else
						if (NewNoOfButtons < ms_NoOfButtons && NewNoOfButtons > 1)
						{
							OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SHORTENBAROP); 
							if (pOpDesc != NULL)
							{
								OpParamShortenBar ParamShortenBar(m_BarName, NewNoOfButtons);
								pOpDesc->Invoke(&ParamShortenBar);
								// update the dialog value
								ms_NoOfButtons = NewNoOfButtons;
								SetLongGadgetValue(_R(IDC_BC_NO_OF_BUTTONS), ms_NoOfButtons);
							}
							else
							{
								ERROR3("Couldn't find OPTOKEN_SHORTENBAROP op descriptor");
							}
						}
						else
						if (NewNoOfButtons < ms_NoOfButtons && NewNoOfButtons == 1)
						{
							SetTimer(DLG_TIMER, 1000, NULL);
						}

					}
					//DialogManager::DefaultKeyboardFocus(); // throws focus
					break;

				} // end switch

				break;

			// manage button controls
			case DIM_LFT_BN_CLICKED:
				switch(pDialogMsg->GadgetID)
				{
				case _R(IDC_BC_CREATE):
					if (m_IsEditing)
					{ // open up the states dlg
						OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BARSTATESDLG); 
						if (pOpDesc != NULL)
						{
							OpParamBarStatesDlg ParamBarStatesDlg(m_BarName);
							pOpDesc->Invoke(&ParamBarStatesDlg);
						}

					}
					else
					{
						if(CreateBar()) // behave in a modal manner - close the dialog having done the deed.
						{
							fEndDialog = TRUE;
							m_IsEditing = TRUE;
							SetUpControls(TRUE);
						}
					}
					break;

				case _R(IDC_BC_REDEF_BACKBAR):
					if (m_IsEditing)
					{
						// create a back bar
						INT32 temp = ms_StateToCreate;
						ms_StateToCreate = BACKBAR;
						CreateBar();
						ms_StateToCreate = temp;
						SetUpControls(TRUE);
					}
					break;

				case _R(IDC_BC_GROUPS_STRETCH):
					ms_GroupsStretch = !GetBoolGadgetSelected(_R(IDC_BC_GROUPS_STRETCH));
					if (m_IsEditing)
						WriteBarProperties();
					break;
				}
				//DialogManager::DefaultKeyboardFocus(); // throws focus
				break;

			
				//NB: This means someone has altered the combo box. It doesn't mean
				//the selection has changed!
			case DIM_SELECTION_CHANGED: 
				switch(pDialogMsg->GadgetID)
				{
					// changed the direction of the bar
				case _R(IDC_BC_DIRECTION):
					switch (GetSelectedValueIndex(_R(IDC_BC_DIRECTION)))
					{
					case 0: // horz
						ms_IsVertical = FALSE;
						ms_Continuous = TRUE;
						break;

					case 1: // vert
						ms_IsVertical = TRUE;
						ms_Continuous = TRUE;
						break;

					case 2: // scattered
						ms_IsVertical = TRUE;
						ms_Continuous = FALSE;
						break;
					}

					EnableGadget(_R(IDC_BC_SPACING), ms_Continuous && m_ValidBar);

					if (m_IsEditing) // update the bar direction now!
						WriteBarProperties();

					//DialogManager::DefaultKeyboardFocus(); // throws focus
					break;

				case _R(IDC_BC_BUTTONS_SAME):
					ms_SizeJustification = GetSelectedValueIndex(_R(IDC_BC_BUTTONS_SAME));

					if (m_IsEditing)
						WriteBarProperties();
					//DialogManager::DefaultKeyboardFocus(); // throws focus
					break;

				case _R(IDC_BC_BUTTON_SIZE):
					ms_Stretching = GetSelectedValueIndex(_R(IDC_BC_BUTTON_SIZE));
					EnableGadget(_R(IDC_BC_GROUPS_STRETCH), ms_Stretching ? 0 : 1);
					if (m_IsEditing)
					{
						BYTE ExtendFlags = 0; // dont extend

						if (ms_Stretching == 0)
						{
							ExtendFlags = X_EXTEND | Y_EXTEND;
						}
						else
							if (ms_Stretching == 1)
							{
								ExtendFlags = X_STRETCH | Y_STRETCH;
							}

						OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGE_BAR_EXTENDS); 
						if (pOpDesc != NULL)
						{
							OpParam BarStretchParam(SliceHelper::GetBarNumberFromBarName(m_BarName), ExtendFlags);
							pOpDesc->Invoke(&BarStretchParam);
						}
						else
						{
							ERROR3("Couldn't find OPTOKEN_CHANGE_BAR_EXTENDS op descriptor");
						}

						WriteBarProperties();
					}
					//DialogManager::DefaultKeyboardFocus(); // throws focus
					break;

				}
				break;
		}
	}
	else if (MESSAGE_IS_A(Message, OpMsg))
	{
		if (((OpMsg *)Message)->MsgType == OpMsg::AFTER_UNDO ||
			((OpMsg *)Message)->MsgType == OpMsg::AFTER_REDO ||
			IS_A( ((OpMsg *)Message)->pOp, OpDelete) )
		{
			ReadBarProperties(); // set the controls from what the bar thinks it is
			SetUpControls(TRUE); // set all the controls
		}
	}
	// for whenever someone announces that they have changed the selection.
	else if (MESSAGE_IS_A(Message, SelChangingMsg))
	{
		SelChangingMsg* pSelChange = (SelChangingMsg*)Message;

		if (pSelChange->State == SelChangingMsg::SelectionState::SELECTIONCHANGED ||
			pSelChange->State == SelChangingMsg::SelectionState::NONCOLOURATTCHANGED)
		{
			SetUpDynamicControls();
		}
	}
	// If we've changed to a different document then get rid of the dlg.
	else if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg* TheMsg = (DocChangingMsg*) Message;
		if (TheMsg->State == DocChangingMsg::BORN || TheMsg->State == DocChangingMsg::SELCHANGED)
			Close();
	}
		
	if (fEndDialog) 
	{
		// read the number of buttons if the dlg is closed
		// so we don't have to re-type
		ms_NoOfButtons = GetLongGadgetValue(_R(IDC_BC_NO_OF_BUTTONS), 1, 20);
		if (ms_NoOfButtons > 20) ms_NoOfButtons = 20;
		if (ms_NoOfButtons < 1) ms_NoOfButtons = 1;

		// close the dlg
		Close(); 		   
		End();
	}


	return DialogOp::Message(Message);
} 

/*******************************************************************************************

>	OpState	BarCreationDlg::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> after Alex
	Created:	13/8/99
	Purpose:	Returns the OpState of the dialogue operation

*******************************************************************************************/

OpState	BarCreationDlg::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	// if we don't allow it
	OpSt.Greyed = TRUE;
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread && !pSpread->FindActiveLayer()->IsFrame())
		OpSt.Greyed = FALSE;

	// Tick the menu while the editor is open
	if (TheDialog != NULL)
		OpSt.Ticked = TRUE;

	return(OpSt);
}

		 

/*******************************************************************************************

>	BOOL BarCreationDlg::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> after Alex
	Created:	13/8/99
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for a HotLink Dialog and declares
				some preferences

*******************************************************************************************/
BOOL BarCreationDlg::Init()
{
	return (RegisterOpDescriptor(0,								// Tool ID
 								_R(IDS_BARCREATION),	 				// String resource ID
								CC_RUNTIME_CLASS(BarCreationDlg),		// Runtime class
 								OPTOKEN_BARCREATIONDLG,				// Token string
 								BarCreationDlg::GetState,				// GetState function
								_R(IDH_NavBar_Create_Dialog_Box),								// Help ID
								_R(IDBBL_BARCREATION),				// Bubble ID
								_R(IDD_BARCONTROLSTORE),			// Resource ID
								_R(IDC_BARCREATION),				// Control ID
								SYSTEMBAR_UTILITIES,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								0,								// No vertical counterpart
								NULL,
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC),
								TRUE
								));	
						
}   
 
      


/********************************************************************************************

>	void BarCreationDlg::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> after Jason
	Created:	13/8/99

	Purpose:	Opens the dialog if it is closed. Closes it if it is open.
				Should no longer be called, but use the with param instead
		
********************************************************************************************/

void BarCreationDlg::Do(OpDescriptor*)
{
	m_IsEditing = FALSE;
	m_BarName = "Bar1";
	m_FirstBarName = m_BarName;
	ms_Spacing = 0; // reset the spacing to zero

	if (TheDialog==NULL || TheDialog==this)
	{
		if (Create())
			Open();
		else
			End();
	}
	else
	{
		TheDialog->Open();
		TheDialog->BringToTop();

		End();				// Kill ourself
	}
}

/********************************************************************************************

>	BOOL BarCreationDlg::Create()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> after Jason
	Created:	13/8/99
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if it fails
	Purpose:	Creates a Web Address dialogue box. Fails if DialogOp::Create
				fails to create the box.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BarCreationDlg::Create()
{

	ReadBarProperties();	

	// manipulate the ticks if we are editing a state
	if (!m_IsEditing)
		// if we are creating we must create the default state
	{
		ms_StateToCreate = 0; // create the off state as default
		ms_FromSelection = TRUE; // from the selection of course

		ms_WantDefault = TRUE;
		ms_WantMouse = FALSE;
		ms_WantClicked = FALSE;
		ms_WantSelected = FALSE;

		ms_Spacing = 0; // reset the spacing to zero
		ms_GroupsStretch = TRUE;
	}


	if (TheDialog != this)		// Allow only one instance of this dialogue open at once
		return(FALSE);

	if (DialogOp::Create())
	{ 
		return(TRUE);
	}

	return(FALSE);
}

/********************************************************************************************

>	void BarCreationDlg::DoWithParam(OpDescriptor* pOp, OpParam* pParam)	   

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> after Alex
	Created:	13/8/99
	Inputs:		OpDescriptor
	Outputs:	-
	Returns:	-
	Purpose:	Performs the BarCreationDlg operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void BarCreationDlg::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	m_IsEditing = ((OpParamBarCreationDlg *)pParam)->m_IsEditing;
	m_BarName = ((OpParamBarCreationDlg *)pParam)->m_BarName;
	m_FirstBarName = m_BarName;
	m_FirstIsEditing = m_IsEditing;

	if (((OpParamBarCreationDlg *)pParam)->m_ButtonsToEdit)
		ms_NoOfButtons = ((OpParamBarCreationDlg *)pParam)->m_ButtonsToEdit;

	if (TheDialog==NULL || TheDialog==this)
	{
		if (Create())
		{
			Open();
			SetUpControls(TRUE);
		}
		else
			End();
	}
	else
	{
		TheDialog->Open();
		TheDialog->BringToTop();

		TheDialog->m_IsEditing = ((OpParamBarCreationDlg *)pParam)->m_IsEditing;
		TheDialog->m_BarName = ((OpParamBarCreationDlg *)pParam)->m_BarName;
		TheDialog->m_FirstBarName = m_BarName;
		TheDialog->m_FirstIsEditing = m_IsEditing;
		TheDialog->m_ValidBar = TRUE;
		TheDialog->ms_Spacing = 0; // reset the spacing to zero
		TheDialog->ms_GroupsStretch = TRUE;

		TheDialog->EnableAllMyControls(TRUE);

		TheDialog->SetUpControls(TRUE);

		End();				// Kill ourself
	}
}


/********************************************************************************************

>	BOOL BarCreationDlg::SetUpControls( BOOL DoScan )

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/99
	Returns:	TRUE always at present!
	Purpose:	Spookily enough it sets up all the controls in the dlg

********************************************************************************************/

BOOL BarCreationDlg::SetUpControls( BOOL DoScan )
{
	if (m_AlreadyInThisFunction)
		return TRUE;

	SetUpDynamicControls();

	m_AlreadyInThisFunction = TRUE;
	if (m_IsEditing)
	{
		if (DoScan)
		{
			// find out how many buttons there are
			INT32 NumberOfButtons = SliceHelper::CountButtonsInBar(m_BarName);
			if (NumberOfButtons > 0)
				ms_NoOfButtons = NumberOfButtons;

			String_256 BackBarLayer(_R(IDS_BACK_BAR));
			m_HasBackBar = SliceHelper::BarExistsOnLayer(m_BarName, BackBarLayer);
		}

		if (m_HasBackBar)
		{
			SetStringGadgetValue(_R(IDC_BC_REDEF_BACKBAR), _R(IDS_REDEFINE_BACKBAR)); // Redefine
			SetStringGadgetValue(_R(IDC_BACKBAR_TEXT), _R(IDS_RECREATE_BACKBAR_TEXT));
		}
		else
		{
			SetStringGadgetValue(_R(IDC_BC_REDEF_BACKBAR), _R(IDS_CREATE_BACKBAR)); // Create
			SetStringGadgetValue(_R(IDC_BACKBAR_TEXT), _R(IDS_CREATE_BACKBAR_TEXT));
		}
	}
	else
	{
		// if we're creating a new bar, we must have the option for default to be ticked.
		ms_WantDefault = TRUE;
	}

	HideGadget(_R(IDC_BC_CREATE_BACKBAR), m_IsEditing);
	HideGadget(_R(IDC_BACKBAR_TEXT), !m_IsEditing);
	HideGadget(_R(IDC_BC_REDEF_BACKBAR), !m_IsEditing);

	SetSelectedValueIndex(_R(IDC_BC_BUTTONS_SAME), ms_SizeJustification);
	SetSelectedValueIndex(_R(IDC_BC_BUTTON_SIZE), ms_Stretching);

	SetStringGadgetValue(_R(IDC_BC_CREATE), m_IsEditing ? _R(IDS_CREATE_STATES) : _R(IDS_CREATE_STATE)); // States.. or Create
	HideGadget(_R(IDC_BC_CREATE), m_IsEditing); // actually hide the control if we are editing (remove states...)
	
	SetStringGadgetValue(IDCANCEL, m_IsEditing ? _R(IDS_DLG_DONE) : _R(IDS_CANCEL_DIALOG)); // Close or Cancel

	// grey the spacing if it is scattered
	EnableGadget(_R(IDC_BC_SPACING), ms_Continuous && m_ValidBar);

	if (!ms_Continuous)
		SetSelectedValueIndex(_R(IDC_BC_DIRECTION), 2);
	else
		SetSelectedValueIndex(_R(IDC_BC_DIRECTION), ms_IsVertical ? 1 : 0);

	SetLongGadgetValue(_R(IDC_BC_NO_OF_BUTTONS), ms_NoOfButtons);
	SetBoolGadgetSelected(_R(IDC_BC_GROUPS_STRETCH), !ms_GroupsStretch);
	EnableGadget(_R(IDC_BC_GROUPS_STRETCH), ms_Stretching ? 0 : 1);

	SetUnitGadgetValue(_R(IDC_BC_SPACING), PIXELS, ms_Spacing);

	String_256 Title;
	if (m_IsEditing)
		Title.Load(_R(IDS_BAR_EDIT_TITLE));
	else
		Title.Load(_R(IDS_BAR_CREATION_TITLE));

	if (m_ValidBar)
		Title += m_BarName;
	else
	{
		String_256 temp(_R(IDS_NO_VALID_BAR));
		Title += temp;
	}

	SetTitlebarName(&Title);

	m_AlreadyInThisFunction = FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL BarCreationDlg::SetUpDynamicControls()

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/99
	Returns:	TRUE always at present!
	Purpose:	Sets up all the controls in the dlg that are
				dynamic and reviewed upon a timer.
				So this function is called on a poll, to read the
				states and set the controls.

********************************************************************************************/
BOOL BarCreationDlg::SetUpDynamicControls()
{
	// show the current layer in the dlg
	String_256 LayerName = "";
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread == NULL)
		return FALSE;

	SelRange * pSelection = GetApplication()->FindSelection();

	// scan the selection has the bar name changed?

	String_256 TempBarName = "";

	Node * pCurrent = pSelection->FindFirst();
	TemplateAttribute * pTemplAttrib = NULL;
	Node * pTempNode = NULL;

	BOOL HasASelection = pCurrent != NULL;

	while (pCurrent && TempBarName.IsEmpty())
	{
		// look at the bar name on the attrib
		pTemplAttrib = (TemplateAttribute *) SliceHelper::FindNextOfClass(pCurrent, pCurrent, CC_RUNTIME_CLASS(TemplateAttribute));
		while (pTemplAttrib && TempBarName.IsEmpty())
		{
			TempBarName = SliceHelper::GetBarName(pTemplAttrib);
			pTemplAttrib = (TemplateAttribute *) SliceHelper::FindNextOfClass(pTemplAttrib, pCurrent, CC_RUNTIME_CLASS(TemplateAttribute));
		}

		pCurrent = pSelection->FindNext(pCurrent);
	}

	// something is selected but that doesn't have a bar name
	if (HasASelection && TempBarName.IsEmpty() && m_IsEditing)
	{
		// does the selection intersect with any known bars?
		SliceHelper::BarNameInRect(pSelection->GetBoundingRect(), &TempBarName);
	}

	if (m_IsEditing && HasASelection && TempBarName.IsEmpty() && m_ValidBar)
	{
		m_ValidBar = FALSE;
		EnableAllMyControls(FALSE);
		SetUpControls(FALSE);
	}
	else
		if (!m_ValidBar && !TempBarName.IsEmpty() )
		{
			m_ValidBar = TRUE;
			EnableAllMyControls(TRUE);
			m_BarName = "";
		}

	if (TempBarName.CompareTo(m_BarName) != 0 && !TempBarName.IsEmpty())
	{
		m_BarName = TempBarName;
		m_IsEditing = TRUE;
		ReadBarProperties();
		SetUpControls();
	}

	// enable disable the create button if there is no selection
	EnableGadget(_R(IDC_BC_CREATE), m_ValidBar && (HasASelection || m_IsEditing));
	// enable disable the create backbar button if there is no selection
	EnableGadget(_R(IDC_BC_REDEF_BACKBAR), m_ValidBar && HasASelection);
	
	return TRUE;
}

void BarCreationDlg::EnableAllMyControls(BOOL enable)
{
	EnableGadget(_R(IDC_BC_NO_OF_BUTTONS),	enable);
	EnableGadget(_R(IDC_BC_DIRECTION),		enable);
	EnableGadget(_R(IDC_BC_SPACING),		enable);
	EnableGadget(_R(IDC_BC_BUTTON_SIZE),	enable);
	EnableGadget(_R(IDC_BC_BUTTONS_SAME),	enable);
	EnableGadget(_R(IDC_BC_REDEF_BACKBAR),	enable);
	EnableGadget(_R(IDC_BC_CREATE),			enable);
}


BOOL BarCreationDlg::CreateBar()
{
	BOOL Valid = TRUE;
	ms_NoOfButtons = GetLongGadgetValue(_R(IDC_BC_NO_OF_BUTTONS), 1, 20, 0 /*error id*/, &Valid);
	if (!Valid)
	{
		InformWarning(_R(IDS_TOO_MANY_BUTTONS));
		return FALSE;
	}

	// run the Op that does the copying of the selection / bar creation bit

	BOOL MakeBackBarAswell = !m_IsEditing && GetBoolGadgetSelected(_R(IDC_BC_CREATE_BACKBAR));

	ms_WantDefault = FALSE;
	ms_WantMouse = FALSE;
	ms_WantClicked = FALSE;
	ms_WantSelected = FALSE;
	switch(ms_StateToCreate)
	{
	case 0:
		ms_WantDefault = TRUE;
		break;

	case 1:
		ms_WantMouse = TRUE;
		break;

	case 2:
		ms_WantClicked = TRUE;
		break;

	case 3:
		ms_WantSelected = TRUE;
		break;

	}

	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BARCREATIONOP); 
	if (pOpDesc != NULL)
	{
		// set the mode of the create from
		// the editing flag for now
		if (m_IsEditing)
			ms_Mode = REPLACE_SOME_STATES;
		else
		{
			ms_Mode = NEW_BAR;

			// the text has changed for the spacing field
			INT32 spacing = 0;
			BOOL Valid=0; 
			MILLIPOINT ActualSpacing = GetUnitGadgetValue(_R(IDC_BC_SPACING), PIXELS, 0, 75000, 0, &Valid);
			String_256 ActualText = GetStringGadgetValue(_R(IDC_BC_SPACING));
			spacing = ActualSpacing;

			if (Valid && !ActualText.IsEmpty() && ms_Spacing != spacing)
			{
				ms_Spacing = spacing;
			}
		}

		// add the bar details
		WriteBarProperties();

		BYTE ExtendFlags = 0; // dont extend

		if (ms_Stretching == 0)
			ExtendFlags = X_EXTEND | Y_EXTEND;
		else
			if (ms_Stretching == 1)
				ExtendFlags = X_STRETCH | Y_STRETCH;

		OpParamBarCreation BarParam(ms_WantMouse,
									ms_WantClicked,
									ms_WantSelected,
									(ms_StateToCreate == BACKBAR) || MakeBackBarAswell,
									ms_IsVertical,
									ms_SuggestDesign,
									ms_NoOfButtons,
									ms_WantDefault,
									ms_Mode,
									ms_Spacing,
									ms_Continuous,
									ms_SizeJustification != 3, // are they the same size ?  0 = same centered 1 = same left 2 = same right 3 = independant
									m_BarName,
									(ms_StateToCreate == DEFAULT || ms_StateToCreate == BACKBAR) ? TRUE : ms_FromSelection,
									ExtendFlags);
		pOpDesc->Invoke(&BarParam);
	}
	else
	{
		ERROR3("Couldn't find OPTOKEN_BARCREATIONOP op descriptor");
	}

	return TRUE;
}





void BarCreationDlg::ReadBarProperties()
{
	NodeBarProperty* pNodeBarProperty = (NodeBarProperty*) Document::GetCurrent()->GetSetSentinel()->FindBarProperty();
	INT32 BarNumber = SliceHelper::GetBarNumberFromBarName(m_BarName);

	if (pNodeBarProperty && BarNumber < pNodeBarProperty->HowMany())
	{
		BarDataType NewBarData = pNodeBarProperty->Bar(BarNumber);
		ms_IsVertical = NewBarData.IsHorizontal ? FALSE : TRUE;
		ms_Continuous = NewBarData.RequiresShuffle;
		ms_SizeJustification = NewBarData.SameSize;
		ms_Spacing = NewBarData.Spacing;
		ms_Stretching = 2; // static no stretching
		if (NewBarData.ButtonsExtend)
			ms_Stretching = 0;
		else if (NewBarData.ButtonsScale)
			ms_Stretching = 1;
		ms_GroupsStretch = NewBarData.GroupsStretch;
	}
}

void BarCreationDlg::WriteBarProperties()
{
	NodeBarProperty* pNodeBarProperty = (NodeBarProperty*) Document::GetCurrent()->GetSetSentinel()->FindBarProperty();
	INT32 BarNumber = SliceHelper::GetBarNumberFromBarName(m_BarName);

	if (pNodeBarProperty )
	{
		BarDataType NewBarData;

		NewBarData.IsHorizontal = !ms_IsVertical;
		NewBarData.RequiresShuffle = ms_Continuous;
		NewBarData.SameSize = ms_SizeJustification;
		NewBarData.Spacing = ms_Spacing;
		NewBarData.ButtonsExtend = (ms_Stretching == 0 ? 1 : 0);
		NewBarData.ButtonsScale = (ms_Stretching == 1 ? 1 : 0);
		NewBarData.GroupsStretch = ms_GroupsStretch;

		pNodeBarProperty->MakeChange(BarNumber, NewBarData);
		// tell everyone that the selection may have changed
		// actually it wont have but the position of the blobs may well have
		// and sending this message means that everyone like the tool will
		// re-evaluate the postitions of the selection (sjk 11/7/00)
		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED));
	}
}


/*****************************************************************************************************/

// Set up our static variables
const CDlgMode BarStatesDlg::Mode = MODELESS;
const UINT32 BarStatesDlg::IDD = _R(IDD_BARSTATESDLG);

BarStatesDlg *BarStatesDlg::TheDialog = NULL;
INT32 BarStatesDlg::m_State = 1;
BOOL BarStatesDlg::ms_SuggestDesign = TRUE;


/*******************************************************************************************

>	BarStatesDlg::BarStatesDlg() : DialogOp(RenderDlg::IDD, RenderDlg::Mode) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/00
	Purpose:	Constructor

*******************************************************************************************/

BarStatesDlg::BarStatesDlg() : DialogOp(BarStatesDlg::IDD, BarStatesDlg::Mode) 
{
	//Set our member variable pointer so it points at ourself
	if (TheDialog == NULL)
		TheDialog = this;

	m_ValidBar = TRUE;
}       



/*******************************************************************************************

>	BarStatesDlg::~BarStatesDlg()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> from Alex
	Created:	13/8/99
	Purpose:	Destructor

*******************************************************************************************/

BarStatesDlg::~BarStatesDlg()
{
	//Set our member variable pointer to NULL
	if (TheDialog == this)
		TheDialog = NULL;
} 
 
/********************************************************************************************
>	MsgResult BarStatesDlg::Message( Msg* Message)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/00
	Inputs:		Message: The incoming message 
		
	Returns:	DialogOp::Message (Message)

	Purpose:	This is the message handler for the BarStatesDlg. It simply takes the
				message and redirects it to one of our Message Handler functions.
	 
********************************************************************************************/

MsgResult BarStatesDlg::Message( Msg* Message)
{
	BOOL fEndDialog=FALSE;

	//If the message is a dialog message
	if (IS_OUR_DIALOG_MSG(Message))
	{
		//Then work out what it says
		DialogMsg* pDialogMsg = ((DialogMsg*) Message); 
	
		switch (pDialogMsg->DlgMsg)
		{
			case DIM_CREATE:
				SetStringGadgetValue(_R(IDC_MUTATE_COLOURS), _R(IDS_STATE_COPY), FALSE, 0);
				SetStringGadgetValue(_R(IDC_MUTATE_COLOURS), _R(IDS_STATE_DISTINCT), TRUE, 1);
				ScanForBarDetails();
				SetUpControls();
				// throw the keyboard focus
				//DialogManager::DefaultKeyboardFocus();
				break;
	
			// these two messages are used to throw the focus back to the main window
			case DIM_TITLEFOCUSWARN:
				//DialogManager::DefaultKeyboardFocus();
				break;

			// focus throw
			case DIM_LFT_BN_UP:
				//DialogManager::DefaultKeyboardFocus();
				break;

			case DIM_CANCEL:	
				fEndDialog = TRUE;
				break;

			// manage button controls
			case DIM_LFT_BN_CLICKED:
				switch(pDialogMsg->GadgetID)
				{
				case _R(IDC_MOUSE_OFF):
					m_State = 0;
					SetUpControls();
					//DialogManager::DefaultKeyboardFocus(); // throws focus
					break;
				case _R(IDC_MOUSE_OVER):
					m_State = 1;
					SetUpControls();
					//DialogManager::DefaultKeyboardFocus(); // throws focus
					break;
				case _R(IDC_CLICKED):
					m_State = 2;
					SetUpControls();
					//DialogManager::DefaultKeyboardFocus(); // throws focus
					break;
				case _R(IDC_SELECTED):
					m_State = 3;
					SetUpControls();
					//DialogManager::DefaultKeyboardFocus(); // throws focus
					break;
				//case _R(IDC_BACK_BAR):
				//	m_State = 4;
				//	SetUpControls();
				//	break;

				case _R(IDC_CREATE_STATE):
					// create the state
					CreateBarStates(m_StateExists[m_State]);
					ScanForBarDetails();
					SetUpControls();
					//DialogManager::DefaultKeyboardFocus(); // throws focus
					break;
					
				case _R(IDC_DELETE_STATE):
					{
						OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_DELBAROP);

						if (pOpDesc && m_State > 0) // dont allow this to delete the MouseOff State!
						{
							OpParamDelBar ParamDelBar(m_BarName, m_State);
							pOpDesc->Invoke(&ParamDelBar);

							ScanForBarDetails();
							SetUpControls();
						}
						else if (pOpDesc)
						{
							if (AskQuestion(_R(IDS_DEL_WHOLE_BAR), _R(IDS_YES), _R(IDS_NO)) == 1)
							{
								OpParamDelBar ParamDelBar(m_BarName, 5 /* all states */);
								pOpDesc->Invoke(&ParamDelBar);

								// if the bar has been removed close this dlg
								fEndDialog = TRUE;
							}
						}
					}
					//DialogManager::DefaultKeyboardFocus(); // throws focus
					break;
				}
				break;	
			
			//NB: This means someone has altered the combo box. It doesn't mean
			//the selection has changed!
			case DIM_SELECTION_CHANGED: 
				switch(pDialogMsg->GadgetID)
				{
					// changed the direction of the bar
					case _R(IDC_MUTATE_COLOURS):
						ms_SuggestDesign = GetSelectedValueIndex(_R(IDC_MUTATE_COLOURS));;
					break;
				}
				break;

		}
	}
	// for whenever someone announces that they have changed the selection.
	else if (MESSAGE_IS_A(Message, SelChangingMsg))
	{
		SelChangingMsg* pSelChange = (SelChangingMsg*)Message;

		if (pSelChange->State == SelChangingMsg::SelectionState::SELECTIONCHANGED ||
			pSelChange->State == SelChangingMsg::SelectionState::NONCOLOURATTCHANGED)
			OnSelectionChanged();
	}
	// If we've changed to a different document then get rid of the dlg.
	else if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg* TheMsg = (DocChangingMsg*) Message;
		if (TheMsg->State == DocChangingMsg::BORN || TheMsg->State == DocChangingMsg::SELCHANGED)
			Close();
	}
	else if (MESSAGE_IS_A(Message, OpMsg))
	{
		if (((OpMsg *)Message)->MsgType == OpMsg::AFTER_UNDO ||
			((OpMsg *)Message)->MsgType == OpMsg::AFTER_REDO ||
			IS_A( ((OpMsg *)Message)->pOp, OpDelete) )
		{
			ScanForBarDetails();
			SetUpControls();
			OnSelectionChanged();
		}
	}
		
	if (fEndDialog) 
	{
		// close the dlg
		Close(); 		   
		End();
	}


	return DialogOp::Message(Message);
} 

/*******************************************************************************************

>	OpState	BarCreationDlg::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> after Alex
	Created:	13/8/99
	Purpose:	Returns the OpState of the dialogue operation

*******************************************************************************************/

OpState	BarStatesDlg::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	// if we don't allow it
	OpSt.Greyed = TRUE;
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread && !pSpread->FindActiveLayer()->IsFrame())
		OpSt.Greyed = FALSE;

	// Tick the menu while the editor is open
	if (TheDialog != NULL)
		OpSt.Ticked = TRUE;

	return(OpSt);
}

		 

/*******************************************************************************************

>	BOOL BarStatesDlg::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/00
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for a HotLink Dialog and declares
				some preferences

*******************************************************************************************/
BOOL BarStatesDlg::Init()
{
	return (RegisterOpDescriptor(0,								// Tool ID
 								_R(IDS_CREATE_STATES),	 				// String resource ID
								CC_RUNTIME_CLASS(BarStatesDlg),		// Runtime class
 								OPTOKEN_BARSTATESDLG,				// Token string
 								BarStatesDlg::GetState,				// GetState function
								_R(IDH_NavBar_Edit_States_Dialog_Box),	// Help ID
								_R(IDBBL_STATES),				// Bubble ID
								_R(IDD_BARCONTROLSTORE),			// Resource ID
								_R(IDC_STATES),				// Control ID
								SYSTEMBAR_UTILITIES,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								0,								// No vertical counterpart
								NULL,
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC),
								TRUE
								));	
						
}   
 
      


/********************************************************************************************

>	void BarStatesDlg::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	8/5/00

	Purpose:	Opens the dialog if it is closed. Closes it if it is open.
				Should no longer be called, but use the with param instead
		
********************************************************************************************/

void BarStatesDlg::Do(OpDescriptor*)
{
	m_BarName = "Bar1";

	if (TheDialog==NULL || TheDialog==this)
	{
		if (Create())
			Open();
		else
			End();
	}
	else
	{
		TheDialog->Open();
		TheDialog->BringToTop();

		End();				// Kill ourself
	}
}

/********************************************************************************************

>	BOOL BarStatesDlg::Create()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/00
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if it fails
	Purpose:	Creates a dlg
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BarStatesDlg::Create()
{
	if (TheDialog != this)		// Allow only one instance of this dialogue open at once
		return(FALSE);

	for (INT32 i = 0; i < 5; i++)
		m_StateExists[i] = FALSE;

	ScanForBarDetails();

	if (DialogOp::Create())
	{ 
		return(TRUE);
	}

	return(FALSE);
}

/********************************************************************************************

>	void BarStatesDlg::DoWithParam(OpDescriptor* pOp, OpParam* pParam)	   

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	8/5/00
	Inputs:		OpDescriptor
	Outputs:	-
	Returns:	-
	Purpose:	Performs the BarCreationDlg operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void BarStatesDlg::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	m_BarName = ((OpParamBarStatesDlg *)pParam)->m_BarName;

	if (TheDialog==NULL || TheDialog==this)
	{
		if (Create())
			Open();
		else
			End();
	}
	else
	{
		TheDialog->Open();
		TheDialog->BringToTop();

		TheDialog->m_BarName = ((OpParamBarStatesDlg *)pParam)->m_BarName;

		TheDialog->ScanForBarDetails(); // we may be looking at a different bar now
		TheDialog->SetUpControls();

		End();				// Kill ourself
	}
}


/********************************************************************************************

>	BOOL BarStatesDlg::ScanForBarDetails(INT32 &NoOfButtons)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99 - modified much on 8/5/00
	Returns:	TRUE for success FALSE for failure
	Purpose:	Scans the tree to fill in the variables passed to it
	Params:		NoOfButtons-How many buttons there are in the bar

********************************************************************************************/
BOOL BarStatesDlg::ScanForBarDetails()
{
	// find me the buttons in this bar
	String_256 templayer;

	// build up a list of the buttons in the bar
	// so that we can shift them along
	// it must be better to use a list than keep on
	// scanning each time

	Layer * pLayer = NULL;

	List BarList;

	templayer.Load(_R(IDS_ROLLOVER_DEFAULT));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if (pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, m_BarName);
	DWORD BarElements = BarList.GetCount();
	DWORD NextElements = BarElements;
	if (BarElements > 0)
		m_StateExists[0] = TRUE;
	else
		m_StateExists[0] = FALSE;

	templayer.Load(_R(IDS_ROLLOVER_MOUSE));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if (pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, m_BarName);
	NextElements = BarList.GetCount();
	if (NextElements > BarElements)
	{
		BarElements = NextElements;
		m_StateExists[1] = TRUE;
	}
	else
		m_StateExists[1] = FALSE;

	templayer.Load(_R(IDS_ROLLOVER_CLICKED));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if (pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, m_BarName);
	NextElements = BarList.GetCount();
	if (NextElements > BarElements)
	{
		BarElements = NextElements;
		m_StateExists[2] = TRUE;
	}
	else
		m_StateExists[2] = FALSE;

	templayer.Load(_R(IDS_ROLLOVER_SELECTED));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if (pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, m_BarName);
	NextElements = BarList.GetCount();
	if (NextElements > BarElements)
	{
		BarElements = NextElements;
		m_StateExists[3] = TRUE;
	}
	else
		m_StateExists[3] = FALSE;


	// need to do a scan to see if the bar has a back bar or not
	templayer.Load(_R(IDS_BACK_BAR));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	m_StateExists[4] = FALSE;
	if (pLayer)
	{
		Node * pTemplAttrib = SliceHelper::FindNextOfClass(pLayer, pLayer, CC_RUNTIME_CLASS(TemplateAttribute));
		while (pTemplAttrib)
		{
			if (m_BarName == SliceHelper::GetBarName((TemplateAttribute *)pTemplAttrib))
			{
				m_StateExists[4] = TRUE;
				break;
			}
			pTemplAttrib = SliceHelper::FindNextOfClass(pTemplAttrib, pLayer, CC_RUNTIME_CLASS(TemplateAttribute));
		}
	}

	NodeListItem *pNodeListItem = NULL;
	NodeListItem *pDelThisToo = NULL;
	
	// tidy up the list
	pNodeListItem = (NodeListItem *)BarList.GetHead();

	while(pNodeListItem)
	{
		pDelThisToo = pNodeListItem;

		pNodeListItem = (NodeListItem *)BarList.GetNext(pNodeListItem);

		// tidy up the list as we go along
		BarList.RemoveItem((NodeListItem *)pDelThisToo);
		delete pDelThisToo;
	}

	// if we have done this scan we are looking at a new bar
	// set the bar name in the title
	String_256 Title;
	Title.Load(_R(IDS_BAR_STATES_TITLE));
	if (m_ValidBar)
		Title += m_BarName;
	else
	{
		String_256 temp(_R(IDS_NO_VALID_BAR));
		Title += temp;
	}

	SetTitlebarName(&Title);

	return TRUE;
}



void BarStatesDlg::SetUpControls()
{
	SetLongGadgetValue(_R(IDC_MOUSE_OFF), m_State == 0);
	SetLongGadgetValue(_R(IDC_MOUSE_OVER), m_State == 1);
	SetLongGadgetValue(_R(IDC_CLICKED), m_State == 2);
	SetLongGadgetValue(_R(IDC_SELECTED), m_State == 3);
	//SetLongGadgetValue(_R(IDC_BACK_BAR), m_State == 4);

	HideGadget(_R(IDC_MOUSE_OFF_EXISTS), !m_StateExists[0]);
	HideGadget(_R(IDC_MOUSE_OVER_EXISTS), !m_StateExists[1]);
	HideGadget(_R(IDC_CLICKED_EXISTS), !m_StateExists[2]);
	HideGadget(_R(IDC_SELECTED_EXISTS), !m_StateExists[3]);
	//HideGadget(_R(IDC_BACK_BAR_EXISTS), !m_StateExists[4]);

	EnableGadget(_R(IDC_DELETE_STATE), m_StateExists[m_State]);

	if (m_StateExists[m_State])
		SetStringGadgetValue(_R(IDC_CREATE_STATE), _R(IDS_RECREATE));
	else
		SetStringGadgetValue(_R(IDC_CREATE_STATE), _R(IDS_CREATE_BUTTON_TEXT));

	SetSelectedValueIndex(_R(IDC_MUTATE_COLOURS), ms_SuggestDesign);
	HideGadget(_R(IDC_MUTATE_COLOURS), m_State == 4);

	if (m_State == 4)
		SetStringGadgetValue(_R(IDC_SOURCE_TEXT), _R(IDS_FROM_SELECTION));
	else
		SetStringGadgetValue(_R(IDC_SOURCE_TEXT), _R(IDS_FROM_MOUSE_OFF));
}

BOOL BarStatesDlg::CreateBarStates(BOOL ReCreate)
{
	NodeBarProperty* pNodeBarProperty = (NodeBarProperty*) Document::GetCurrent()->GetSetSentinel()->FindBarProperty();
	INT32 BarNumber = SliceHelper::GetBarNumberFromBarName(m_BarName);

	if (!pNodeBarProperty || BarNumber >= pNodeBarProperty->HowMany())
		return FALSE;

	BarDataType NewBarData = pNodeBarProperty->Bar(BarNumber);
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BARCREATIONOP); 

	if (!pOpDesc)
		return FALSE;

	BYTE ExtendFlags = 0; // dont extend

	if (NewBarData.ButtonsExtend)
		ExtendFlags = X_EXTEND | Y_EXTEND;
	else
		if (NewBarData.ButtonsScale)
			ExtendFlags = X_STRETCH | Y_STRETCH;

	OpParamBarCreation BarParam(m_State == 1,	//ms_WantMouse,
								m_State == 2,	//ms_WantClicked,
								m_State == 3,	//ms_WantSelected,
								m_State == 4,	//(ms_StateToCreate == BACKBAR),
								NewBarData.IsHorizontal ? FALSE : TRUE,
								ms_SuggestDesign,
								0, //ms_NoOfButtons,
								m_State == 0, // ms_WantDefault - dont use this to make the default from this dlg
								REPLACE_SOME_STATES,
								NewBarData.Spacing,
								NewBarData.RequiresShuffle,
								NewBarData.SameSize,
								m_BarName,
								(m_State == DEFAULT || m_State == BACKBAR) ? TRUE : FALSE, // from selection or from MouseOff?
								ExtendFlags,
								ReCreate);
	pOpDesc->Invoke(&BarParam);

	return TRUE;
}



BOOL BarStatesDlg::OnSelectionChanged()
{
	// show the current layer in the dlg
	String_256 LayerName = "";
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread == NULL)
		return FALSE;

	SelRange * pSelection = GetApplication()->FindSelection();

	// scan the selection has the bar name changed?

	String_256 TempBarName = "";

	Node * pCurrent = pSelection->FindFirst();
	TemplateAttribute * pTemplAttrib = NULL;
	Node * pTempNode = NULL;

	BOOL HasASelection = pCurrent != NULL;

	while (pCurrent && TempBarName.IsEmpty())
	{
		// look at the bar name on the attrib
		pTemplAttrib = (TemplateAttribute *) SliceHelper::FindNextOfClass(pCurrent, pCurrent, CC_RUNTIME_CLASS(TemplateAttribute));
		while (pTemplAttrib && TempBarName.IsEmpty())
		{
			TempBarName = SliceHelper::GetBarName(pTemplAttrib);
			pTemplAttrib = (TemplateAttribute *) SliceHelper::FindNextOfClass(pTemplAttrib, pCurrent, CC_RUNTIME_CLASS(TemplateAttribute));
		}

		pCurrent = pSelection->FindNext(pCurrent);
	}

	// something is selected but that doesn't have a bar name
	if (HasASelection && TempBarName.IsEmpty())
	{
		// does the selection intersect with any known bars?
		SliceHelper::BarNameInRect(pSelection->GetBoundingRect(), &TempBarName);
	}

	if (HasASelection && TempBarName.IsEmpty() && m_ValidBar)
	{
		m_ValidBar = FALSE;

		EnableGadget(_R(IDC_CREATE_STATE),		FALSE);
		EnableGadget(_R(IDC_MUTATE_COLOURS),	FALSE);
		EnableGadget(_R(IDC_DELETE_STATE),		FALSE);

		ScanForBarDetails();
	}
	else
		if (!m_ValidBar && !TempBarName.IsEmpty())
		{
			m_ValidBar = TRUE;

			EnableGadget(_R(IDC_CREATE_STATE),		TRUE);
			EnableGadget(_R(IDC_MUTATE_COLOURS),	TRUE);
			EnableGadget(_R(IDC_DELETE_STATE),		TRUE);

			m_BarName = "";
		}

	if (TempBarName.CompareTo(m_BarName) != 0 && !TempBarName.IsEmpty())
	{
		// found a new bar, reset the dlg
		m_BarName = TempBarName;
		ScanForBarDetails();
		SetUpControls();
	}

	return TRUE;
}





/*****************************************************************************************************/

// Set up our static variables
const CDlgMode BarRedefineStateDlg::Mode = MODELESS;
const UINT32 BarRedefineStateDlg::IDD = _R(IDD_REDEFINE_STATE);

BarRedefineStateDlg *BarRedefineStateDlg::TheDialog = NULL;


/*******************************************************************************************

>	BarRedefineStateDlg::BarRedefineStateDlg() : DialogOp(RenderDlg::IDD, RenderDlg::Mode) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/00
	Purpose:	Constructor

*******************************************************************************************/

BarRedefineStateDlg::BarRedefineStateDlg() : DialogOp(BarRedefineStateDlg::IDD, BarRedefineStateDlg::Mode) 
{
	//Set our member variable pointer so it points at ourself
	if (TheDialog == NULL)
		TheDialog = this;

}       



/*******************************************************************************************

>	BarRedefineStateDlg::~BarRedefineStateDlg()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	9/5/00
	Purpose:	Destructor

*******************************************************************************************/

BarRedefineStateDlg::~BarRedefineStateDlg()
{
	//Set our member variable pointer to NULL
	if (TheDialog == this)
		TheDialog = NULL;
} 
 
/********************************************************************************************
>	MsgResult BarRedefineStateDlg::Message( Msg* Message)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/00
	Inputs:		Message: The incoming message 
		
	Returns:	DialogOp::Message (Message)

	Purpose:	This is the message handler for the BarStatesDlg. It simply takes the
				message and redirects it to one of our Message Handler functions.
	 
********************************************************************************************/

MsgResult BarRedefineStateDlg::Message( Msg* Message)
{
	BOOL fEndDialog=FALSE;

	//If the message is a dialog message
	if (IS_OUR_DIALOG_MSG(Message))
	{
		//Then work out what it says
		DialogMsg* pDialogMsg = ((DialogMsg*) Message); 
	
		switch (pDialogMsg->DlgMsg)
		{
			case DIM_CREATE:
				SetUpControls();
				//DialogManager::DefaultKeyboardFocus(); // throws focus
				break;
	
			// these two messages are used to throw the focus back to the main window
			case DIM_TITLEFOCUSWARN:
				//DialogManager::DefaultKeyboardFocus();
				break;

			// focus throw
			case DIM_LFT_BN_UP:
				//DialogManager::DefaultKeyboardFocus();
				break;
	
			case DIM_CANCEL:	
				fEndDialog = TRUE;
				break;

			case DIM_COMMIT:
				RedefineState();
				fEndDialog = TRUE;
				break;
		}
	}
	// If we've changed to a different document then get rid of the dlg.
	else if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg* TheMsg = (DocChangingMsg*) Message;
		if (TheMsg->State == DocChangingMsg::BORN || TheMsg->State == DocChangingMsg::SELCHANGED)
			Close();
	}
		
	if (fEndDialog) 
	{
		// close the dlg
		Close(); 		   
		End();
	}


	return DialogOp::Message(Message);
} 

/*******************************************************************************************

>	OpState	BarRedefineStateDlg::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> after Alex
	Created:	13/8/99
	Purpose:	Returns the OpState of the dialogue operation

*******************************************************************************************/

OpState	BarRedefineStateDlg::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	// if we don't allow it
	OpSt.Greyed = TRUE;
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread && !pSpread->FindActiveLayer()->IsFrame())
		OpSt.Greyed = FALSE;

	// Tick the menu while the editor is open
	if (TheDialog != NULL)
		OpSt.Ticked = TRUE;

	return(OpSt);
}

		 

/*******************************************************************************************

>	BOOL BarRedefineStateDlg::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/00
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for a HotLink Dialog and declares
				some preferences

*******************************************************************************************/
BOOL BarRedefineStateDlg::Init()
{
	return (RegisterOpDescriptor(0,								// Tool ID
 								_R(IDS_RECREATE_STATES),	 				// String resource ID
								CC_RUNTIME_CLASS(BarRedefineStateDlg),		// Runtime class
 								OPTOKEN_BARREDEFINESTATEDLG,				// Token string
 								BarRedefineStateDlg::GetState,				// GetState function
								_R(IDH_NavBar_Edit_Design_Dialog_Box),			// Help ID
								_R(IDBBL_REDEFINE_STATE),				// Bubble ID
								_R(IDD_BARCONTROLSTORE),			// Resource ID
								_R(IDC_REDEFINE_STATE),				// Control ID
								SYSTEMBAR_UTILITIES,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								0,								// No vertical counterpart
								NULL,
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC),
								TRUE
								));	
						
}   
 
      


/********************************************************************************************

>	void BarRedefineStateDlg::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	8/5/00

	Purpose:	Opens the dialog if it is closed. Closes it if it is open.
				Should no longer be called, but use the with param instead
		
********************************************************************************************/

void BarRedefineStateDlg::Do(OpDescriptor*)
{
	m_BarName = "Bar1";
	m_State = 0;

	if (TheDialog==NULL || TheDialog==this)
	{
		if (Create())
			Open();
		else
			End();
	}
	else
	{
		TheDialog->Open();
		TheDialog->BringToTop();

		End();				// Kill ourself
	}
}

/********************************************************************************************

>	BOOL BarRedefineStateDlg::Create()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/00
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if it fails
	Purpose:	Creates a dlg
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BarRedefineStateDlg::Create()
{
	if (TheDialog != this)		// Allow only one instance of this dialogue open at once
		return(FALSE);

	if (DialogOp::Create())
	{ 
		return(TRUE);
	}

	return(FALSE);
}

/********************************************************************************************

>	void BarRedefineStateDlg::DoWithParam(OpDescriptor* pOp, OpParam* pParam)	   

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	8/5/00
	Inputs:		OpDescriptor
	Outputs:	-
	Returns:	-
	Purpose:	Performs the BarCreationDlg operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void BarRedefineStateDlg::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	m_BarName = ((OpParamBarRedefineStateDlg *)pParam)->m_BarName;
	m_State = ((OpParamBarRedefineStateDlg *)pParam)->m_State;

	if (TheDialog==NULL || TheDialog==this)
	{
		if (Create())
			Open();
		else
			End();
	}
	else
	{
		TheDialog->Open();
		TheDialog->BringToTop();

		End();				// Kill ourself
	}
}

BOOL BarRedefineStateDlg::SetUpControls()
{
	// fill in the bar state we are creating

	// set the text in the redefine state button
	String_256 Temp(_R(IDS_ROLLOVER_DEFAULT));
	if (m_State == 1)
		Temp.Load(_R(IDS_ROLLOVER_MOUSE));
	else if (m_State == 2)
		Temp.Load(_R(IDS_ROLLOVER_CLICKED));
	else if (m_State == 3)
		Temp.Load(_R(IDS_ROLLOVER_SELECTED));
	else if (m_State == 4)
		Temp.Load(_R(IDS_BACK_BAR));
	String_256 StateText = Temp;
	Temp.Load(_R(IDS_STATE_TEXT));
	StateText += Temp;
	SetStringGadgetValue(_R(IDC_STATE_TEXT), &StateText);


	// first find all the bar names available
	String_256 DefLayerName;
	DefLayerName.Load(_R(IDS_ROLLOVER_DEFAULT));
	Layer * pDef = SliceHelper::FindLayerCalled(DefLayerName);

	if (!pDef) return FALSE;

	NameGallery* pNameGallery = NameGallery::Instance();
		// make sure we are dealing with the most uptodate information
	pNameGallery->FastUpdateNamedSetSizes();

	String_256 BarName = "";

	// find a new bar name
	INT32 BarNo = 0;
	INT32 misses = 0;
	INT32 selthis = 0;
	do
	{
		BarNo++;
		BarName.MakeMsg(_R(IDS_BARNAME), BarNo);
		BOOL BarExists = SliceHelper::BarNameExists(pDef, BarName);
		if (BarExists)
		{
			SetStringGadgetValue(_R(IDC_BAR_NO_COMBO), &BarName, FALSE, BarNo-1-misses);
			if (BarName == m_BarName)
				selthis = BarNo-1-misses;
		}
		else
			misses++;
	}
	while (misses < 3);

	// then select the bar name we have been passed
	SetSelectedValueIndex(_R(IDC_BAR_NO_COMBO), selthis);


	return TRUE;
}



BOOL BarRedefineStateDlg::IsAttribInSelected(Node * pNode)
{
	Node *pParent = pNode->FindParent();

	// if it is a bevel controller check the bev components for selection
	if (pParent->IsCompound())
	{
		pNode = pParent->FindFirstChild();
		while (pNode)
		{
			if (pNode->IsSelected())
				return TRUE;

			pNode = pNode->FindNext();
		}
	}

	// look for selected parents
	while (pParent && !pParent->IsLayer())
	{
		if (pParent->IsSelected())
			return TRUE;

		pParent = pParent->FindParent();
	}

	return FALSE;
}


BOOL BarRedefineStateDlg::RedefineState()
{
	// get the barname from the drop down
	m_BarName = GetStringGadgetValue(_R(IDC_BAR_NO_COMBO));

	// get the no of buttons in the bar
	INT32 NoOfButtons = SliceHelper::CountButtonsInBar(m_BarName);

	NodeBarProperty* pNodeBarProperty = (NodeBarProperty*) Document::GetCurrent()->GetSetSentinel()->FindBarProperty();
	INT32 BarNumber = SliceHelper::GetBarNumberFromBarName(m_BarName);

	if (!pNodeBarProperty || BarNumber >= pNodeBarProperty->HowMany())
		return FALSE;

	BarDataType NewBarData = pNodeBarProperty->Bar(BarNumber);

	// we need to check if the user doesn't have selected all the elements of a button from this bar
	// if not warn the user that elements not selected will be deleted
	// first which button is it that we think we have selected - if none then use a different warning
	String_256 ButtonName = "";

	Node * pNode = NULL;
	Layer * pLayer = NULL;
	String_256 templayer = "";

	switch (m_State)
	{
	case 0:
		templayer.Load(_R(IDS_ROLLOVER_DEFAULT));
		break;

	case 1:
		templayer.Load(_R(IDS_ROLLOVER_MOUSE));
		break;

	case 2:
		templayer.Load(_R(IDS_ROLLOVER_CLICKED));
		break;

	case 3:
		templayer.Load(_R(IDS_ROLLOVER_SELECTED));
		break;

	case 4:
		templayer.Load(_R(IDS_BACK_BAR));
		break;
	}

	if (!templayer.IsEmpty())
		pLayer = SliceHelper::FindLayerCalled(templayer);

	if (!pLayer)
		return FALSE;

	BOOL FoundSelected = FALSE;

	// scan this layer for which button is selected
	pNode = SliceHelper::FindNextOfClass(pLayer, pLayer, CC_RUNTIME_CLASS(TemplateAttribute));
	while (pNode && !FoundSelected)
	{
		if (m_BarName.CompareTo(SliceHelper::GetBarName((TemplateAttribute *)pNode)) == 0)
		{
			// set the button name we are looking for
			if (IsAttribInSelected(pNode)) 
			{
				ButtonName = ((TemplateAttribute *)pNode)->GetParam();
				FoundSelected = TRUE;
			}
		}

		pNode = SliceHelper::FindNextOfClass(pNode, pLayer, CC_RUNTIME_CLASS(TemplateAttribute));
	}

	// scan this layer for none selected elements of the button
	pNode = SliceHelper::FindNextOfClass(pLayer, pLayer, CC_RUNTIME_CLASS(TemplateAttribute));
	while (FoundSelected && pNode)
	{
		if (m_BarName.CompareTo(SliceHelper::GetBarName((TemplateAttribute *)pNode)) == 0)
		{
			// if we have found a selected button name are other
			if (ButtonName.CompareTo(((TemplateAttribute *)pNode)->GetParam()) == 0)
			{
				if (!IsAttribInSelected(pNode))
				{
					INT32 msg = InformWarning(_R(IDS_BUTTON_NOT_ALL_SELECTED), _R(IDS_CANCEL), _R(IDS_CARRY_ON));
					if (msg == 1)
						return FALSE; // give up & go back
					else
						break; // stop the test and go ahead
				}
			}
		}

		pNode = SliceHelper::FindNextOfClass(pNode, pLayer, CC_RUNTIME_CLASS(TemplateAttribute));
	}

	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BARCREATIONOP); 

	if (!pOpDesc)
		return FALSE;

	BYTE ExtendFlags = 0; // dont extend

	if (NewBarData.ButtonsExtend)
		ExtendFlags = X_EXTEND | Y_EXTEND;
	else
		if (NewBarData.ButtonsScale)
			ExtendFlags = X_STRETCH | Y_STRETCH;

	OpParamBarCreation BarParam(m_State == 1,	//ms_WantMouse,
								m_State == 2,	//ms_WantClicked,
								m_State == 3,	//ms_WantSelected,
								m_State == 4,	//(ms_StateToCreate == BACKBAR),
								NewBarData.IsHorizontal ? FALSE : TRUE,
								FALSE,
								NoOfButtons,
								m_State == 0, // ms_WantDefault - dont use this to make the default from this dlg
								REPLACE_SOME_STATES,
								NewBarData.Spacing,
								NewBarData.RequiresShuffle,
								NewBarData.SameSize,
								m_BarName,
								TRUE, // alsways from selection
								ExtendFlags);
	pOpDesc->Invoke(&BarParam);

	return TRUE;
}
