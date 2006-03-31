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
// A sdialog for testing HotLink things

/*
*/

#include "camtypes.h"
#include "hlinkdlg.h"		//This file's header

#include "reshlpid.h"		//For CC_HELP_BUTTON

#include "menuops.h"	

#include "oilprog.h"

#include "ccdc.h"			// specific #includes needed for kernel-rendered dialogues

#include "app.h"			//for GetApplication()
#include "range.h"			//for selection range stuff
#include "noderend.h"
//#include "barsdlgs.h"		// _R(IDD_BARCONTROLSTORE), _R(IDC_BTN_WEBADDRESSDLG)

//#include "hotdlg.h"			//The dialog rc file
//#include "hotlink.h"		//The string resource rc file

#include "attrmgr.h"
#include "dlgmgr.h"

#include "webattr.h"

#include "range.h"

#include "nodetxts.h"

#include "webaddr.h"
#include "pathname.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(WebAddressDlg, DialogOp);

#define new CAM_DEBUG_NEW

// Set up our static variables
const CDlgMode WebAddressDlg::Mode = MODELESS;
const UINT32 WebAddressDlg::IDD = _R(IDD_WEBADDRESSDLG);

WebAddressDlg *WebAddressDlg::TheDialog = NULL;	
BOOL WebAddressDlg::DontHandleNextTextMessage=FALSE;

BOOL WebAddressDlg::ms_fCorrect = TRUE; 

/*******************************************************************************************

>	WebAddressDlg::WebAddressDlg() : DialogOp(RenderDlg::IDD, RenderDlg::Mode) 

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> from Alex
	Created:	25/3/97
	Purpose:	Constructor

*******************************************************************************************/

WebAddressDlg::WebAddressDlg() : DialogOp(WebAddressDlg::IDD, WebAddressDlg::Mode) 
{
	//Set our member variable pointer so it points at ourself
	if (TheDialog == NULL)
		TheDialog = this;
}       



/*******************************************************************************************

>	WebAddressDlg::~WebAddressDlg()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> from Alex
	Created:	25/3/97
	Purpose:	Destructor

*******************************************************************************************/

WebAddressDlg::~WebAddressDlg()
{
	//Set our member variable pointer to NULL
	if (TheDialog == this)
		TheDialog = NULL;
} 
 
/********************************************************************************************
>	MsgResult WebAddressDlg::Message( Msg* Message)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/97
	Inputs:		Message: The incoming message 
		
	Returns:	DialogOp::Message (Message)

	Purpose:	This is the message handler for the WebAddressDlg. It simply takes the
				message and redirects it to one of our Message Handler functions.
	 
********************************************************************************************/

MsgResult WebAddressDlg::Message( Msg* Message)
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
				OnCreate();
				break;

			case DIM_COMMIT:
				OnAddButtonClicked();					
				break;
			
			case DIM_CANCEL:	
				fEndDialog = TRUE;
				break;

			case DIM_TEXT_CHANGED:
				//This ensures that, when this code changes some text
				//in an edit field, we don't handle the "text changed" message
				if (WebAddressDlg::DontHandleNextTextMessage)
				{
					WebAddressDlg::DontHandleNextTextMessage=FALSE;
				}
				else
					OnDialogChange();

				break;
				
			case DIM_LFT_BN_CLICKED:
				switch(pDialogMsg->GadgetID)
				{
				case _R(IDC_WEBADDRESS_REMOVE):
					OnRemoveButtonClicked();
					break;

				case _R(IDC_WEBADDRESS_OBJECTSHAPE):
				case _R(IDC_WEBADDRESS_RECTANGLE):
					OnDialogChange();
					break;

				case _R(IDC_WEBADDRESS_CORRECT):
					OnAutoCorrectClicked();
					break;
				}
				break;

			
				//NB: This means someone has altered the combo box. It doesn't mean
				//the selection has changed!
			case DIM_SELECTION_CHANGED: 
				OnDialogChange();
				break;
		}
	}
	else if (MESSAGE_IS_A(Message, SelChangingMsg))
	{
		if ( ((SelChangingMsg*)Message)->State == SelChangingMsg::SELECTIONCHANGED )
			OnSelectionChange();
	}
		
	if (fEndDialog) 
	{	
		Close(); 		   
		End();
	}


	return DialogOp::Message(Message);
} 

/*******************************************************************************************

>	OpState	WebAddressDlg::GetState(String_256*, OpDescriptor*)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> after Alex
	Created:	25/3/97
	Purpose:	Returns the OpState of the dialogue operation

*******************************************************************************************/

OpState	WebAddressDlg::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	// Tick the menu while the editor is open
	if (TheDialog != NULL)
		OpSt.Ticked = TRUE;

	return(OpSt);
}

		 

/*******************************************************************************************

>	BOOL WebAddressDlg::Init()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> after Alex
	Created:	25/3/97
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for a HotLink Dialog and declares
				some preferences

*******************************************************************************************/
BOOL WebAddressDlg::Init()
{
	if (Camelot.DeclareSection(TEXT("WebAddressDlg"), 2))
		Camelot.DeclarePref(TEXT("WebAddressDlg"), TEXT("Correct"), &ms_fCorrect);
	
	return (RegisterOpDescriptor(0,								// Tool ID
 								_R(IDS_WEBADDRESS_DLG),	 				// String resouirce ID
								CC_RUNTIME_CLASS(WebAddressDlg),		// Runtime class
 								OPTOKEN_WEBADDRESSDLG,				// Token string
 								WebAddressDlg::GetState,				// GetState function
								0,								// Help ID
								_R(IDBBL_WEBADDRESS),				// Bubble ID
								_R(IDD_BARCONTROLSTORE),			// Resource ID
								_R(IDC_BTN_WEBADDRESSDLG),		// Control ID
								SYSTEMBAR_UTILITIES,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								0,								// No vertical counterpart
								NULL,
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC)
								));	
						
}   
 
      


/********************************************************************************************

>	void WebAddressDlg::Do(OpDescriptor*)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> after Jason
	Created:	25/3/97

	Purpose:	Opens the dialog if it is closed. Closes it if it is open.
		
********************************************************************************************/

void WebAddressDlg::Do(OpDescriptor*)
{
	if (TheDialog==NULL || TheDialog==this)
	{
		if (Create())
			Open();
		else
			End();
	}
	else
	{
		TheDialog->Close();
		TheDialog->End();	// Kill existing editor

		End();				// Kill ourself
	}
}

/********************************************************************************************

>	BOOL WebAddressDlg::Create()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> after Jason
	Created:	25/3/97
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if it fails
	Purpose:	Creates a Web Address dialogue box. Fails if DialogOp::Create
				fails to create the box.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL WebAddressDlg::Create()
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

>	void WebAddressDlg::DoWithParam(OpDescriptor* pOp, OpParam* pParam)	   

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com> after Alex
	Created:	25/3/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the WebAddressDlg operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void WebAddressDlg::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	Do(pOp);

}

/********************************************************************************************

	WebAddressDlg message handling functions

	The following group of functions are called from WebAddressDlg::Message
	when a message is received.

	They call other functions which do whatever needs to be done.

********************************************************************************************/


/********************************************************************************************

	void WebAddressDlg::OnCreate()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that the dialog has just been created.

	Errors:		-
	SeeAlso:	WebAddressDlg::Message()

********************************************************************************************/

void WebAddressDlg::OnCreate()
{	
	//Set up the dialog in its initial state
	SetDialogInitialState();

	//And put the keyboard focus in the URL edit field
	SetKeyboardFocus(_R(IDC_WEBADDRESS_URL));
	HighlightText(_R(IDC_WEBADDRESS_URL));
}

/********************************************************************************************

	void WebAddressDlg::OnAddButtonClicked()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that the Add button has been clicked

	Errors:		-
	SeeAlso:	WebAddressDlg::Message()

********************************************************************************************/

void WebAddressDlg::OnAddButtonClicked()
{	
	//Apply the current Web Address to the selection
	ApplyWebAddress();
	
	//Set up the dialog in its initial state
	SetDialogInitialState();

	//And throw away the keyboard focus
	DialogManager::DefaultKeyboardFocus();
}

/********************************************************************************************

	void WebAddressDlg::OnRemoveButtonClicked()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that the Remove button has been clicked

	Errors:		-
	SeeAlso:	WebAddressDlg::Message()

********************************************************************************************/

void WebAddressDlg::OnRemoveButtonClicked()
{	
	//Remove the Web Address from the current selection
	RemoveWebAddress();
	
	//Set up the dialog in its initial state
	SetDialogInitialState();

	//And throw away the keyboard focus
	DialogManager::DefaultKeyboardFocus();
}

/********************************************************************************************

	void WebAddressDlg::OnAutoCorrectClicked()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that the Auto Correct radio button has been clicked

	Errors:		-
	SeeAlso:	WebAddressDlg::Message()

********************************************************************************************/

void WebAddressDlg::OnAutoCorrectClicked()
{	
	//Update our member variable
	ms_fCorrect=GetCorrect();
}


/********************************************************************************************

	void WebAddressDlg::OnDialogChange()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that something in the dialog has changed

				Note that the AutoCorrect switch is independent of the rest
				of the dialog and so changing that doesn't count as changing the dialog.

	Errors:		-
	SeeAlso:	WebAddressDlg::Message()

********************************************************************************************/

void WebAddressDlg::OnDialogChange()
{	
	//Set the dialog into its changed state
	SetDialogChangedState();
							
}

/********************************************************************************************

	void WebAddressDlg::OnSelectionChange()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Handles a message that there's been a selection change

	Errors:		-
	SeeAlso:	WebAddressDlg::Message()

********************************************************************************************/

void WebAddressDlg::OnSelectionChange()
{	
	//Put the dialog into its initial state
	SetDialogInitialState();
							
}





/********************************************************************************************

	WebAddressDlg action functions

	Called by the message handler functions to do whatever needs to be done

********************************************************************************************/

/********************************************************************************************

	void WebAddressDlg::SetDialogInitialState()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Sets up the dialog in its initial state

				This means:
				a. Calling another member function to set up the controls
				   so they reflect the Web Address attribute on the selection
				b. Setting a flag to say that the dialog is in its initial state


	Errors:		-
	SeeAlso:	WebAddressDlg::OnSelectionChange(), WebAddressDlg::OnCreate(), 
				WebAddressDlg::OnAddButtonClicked()

********************************************************************************************/

void WebAddressDlg::SetDialogInitialState()
{	
	//Set the controls up in their correct initial state
	SetDialogInitialControls();

	//And set a flag to say the dialog is in its initial state
	fDialogIsInInitialState=TRUE;

}


/********************************************************************************************

	void WebAddressDlg::SetDialogChangedState()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Sets up the dialog once it has been changed.

				This means:
				- If there is a selection, the ADD/CHANGE button is ungreyed
				- If there is a Web Address on the Selection, the ADD/CHANGE
				  button will read "Change", otherwise it will read "Add"
															  
	Errors:		-
	SeeAlso:	WebAddressDlg::OnSelectionChange(), WebAddressDlg::OnCreate(), 
				WebAddressDlg::OnAddButtonClicked()

********************************************************************************************/

void WebAddressDlg::SetDialogChangedState()
{	
	
	//If the dialog is still in its initial state...
	if (fDialogIsInInitialState)
	{
		//Then we need to change some things

		//First, let's find the state of the Web Address attributes applied to the 
		//selection
		WebCommonAttributeResult aasApplied=FindCommonWebAttribute();

		//If anything at all is selected
		if (aasApplied!=WCA_NOSELECTION)
		{
			//Then we need to ungrey the Add/Change button
			EnableAdd(TRUE);

			//We also need to clear any intermediate values from the dialog

			//If the URL field says <MANY>, clear that field
			if (GetURL()==String_256(_R(IDS_WEBADDRESSDLG_MANY)))
				ShowURL();

			//If the Frame field says <MANY>, put "self" into that field
			if (GetFrame()==String_256(_R(IDS_WEBADDRESSDLG_MANY)))
				ShowFrame(String_256(_R(IDS_WEBADDRESS_COMBO_SELF)));

			//If the clickable area is indeterminate, show the
			//recommended value for the selection in the radio buttons
			if (GetClickableArea()==2)
				ShowClickableArea(SelectionConsistsOfText());
		}



		//If there is no Web Address on the selection, tell the user
		//so
		if (aasApplied==WCA_DEFAULT)
			ShowWebAddressOnSelection(FALSE);
												
		//If there is any sort of Web Address on the selection, tell the
		//user
		if (aasApplied==WCA_SINGLE || aasApplied==WCA_MANY)
			ShowWebAddressOnSelection(TRUE);
		
	}

	//The dialog is no longer in its initial state
	fDialogIsInInitialState=FALSE;
}


/********************************************************************************************

	void WebAddressDlg::SetDialogInitialControls()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Sets up the dialog according to what attributes are applied to the
				selection. The cases are:

				If a single attribute is applied, that attribute will be put into the
				dialog box. The Add button button will be greyed and the Remove button
				will be active.

				If many attributes are applied, the dialog box edit fields will be
				set to read "<Many>" and both radio buttons will be switched off. The
				Remove button will be active and the Add button will be inactive.

				If nothing is selected, the dialog box edit fields will be cleared
				and both radio buttons will be switched off. The Add and Remove
				buttons will both be inactive.

				If the default attribute is applied, the dialog box edit fields
				will be cleared and:
				- if the selection consists only of text, the "rectangle surrounding
				  object" radio button will be selected
				- otherwise, the "object shape" radio button will be selected.

				
												  

	Errors:		-
	SeeAlso:	WebAddressDlg::OnSelectionChange(), WebAddressDlg::OnCreate(), 
				WebAddressDlg::OnAddButtonClicked()

********************************************************************************************/

void WebAddressDlg::SetDialogInitialControls()
{	
	//First, we want to know what the WebAddressAttribute applied to the selection
	//is.

	//This variable will hold that attribute
	WebAddressAttribute waaApplied;

	//Then call this function to find the state of the WebAddressAttribute(s)
	//applied to the selection
	WebCommonAttributeResult aasApplied=FindCommonWebAttribute(&waaApplied);
								 
	//Edit field and combo box

	//First set up the combo box list
	DeleteAllValues(_R(IDC_WEBADDRESS_FRAME));
	
	SetStringGadgetValue(_R(IDC_WEBADDRESS_FRAME), _R(IDS_WEBADDRESS_COMBO_SELF), FALSE, 0);
	SetStringGadgetValue(_R(IDC_WEBADDRESS_FRAME), _R(IDS_WEBADDRESS_COMBO_PARENT), FALSE, 1);
	SetStringGadgetValue(_R(IDC_WEBADDRESS_FRAME), _R(IDS_WEBADDRESS_COMBO_TOP), FALSE, 2);
	SetStringGadgetValue(_R(IDC_WEBADDRESS_FRAME), _R(IDS_WEBADDRESS_COMBO_BLANK), FALSE, 3);
	SetStringGadgetValue(_R(IDC_WEBADDRESS_FRAME), _R(IDS_WEBADDRESS_COMBO_DEFAULT), FALSE, 4);

	SetComboListLength(_R(IDC_WEBADDRESS_FRAME));

	// If a single attribute is applied or the default attribute
	// is applied, set it into the edit fields
	if (aasApplied==WCA_SINGLE || aasApplied==WCA_DEFAULT)
	{
		ShowURL(waaApplied.m_url.GetWebAddress());
		ShowFrame(waaApplied.m_pcFrame);
	}

	//If many attributes are applied, put <Many> into the edit fields
	if (aasApplied==WCA_MANY)
	{
		ShowURL(String_256(_R(IDS_WEBADDRESSDLG_MANY)));
		ShowFrame(String_256(_R(IDS_WEBADDRESSDLG_MANY)));
	}

	//If nothing is selected, clear the edit fields
	if (aasApplied==WCA_NOSELECTION)
	{
		ShowURL();
		ShowFrame();
	}

	//Radio buttons

	//If a single attribute is applied, set the radio buttons accordingly
	if (aasApplied==WCA_SINGLE)
		ShowClickableArea(waaApplied.m_fBounding); 
	
	//If many attributes are applied, or if nothing is selected,
	//give the radio buttons an indeterminate value
	if (aasApplied==WCA_MANY || aasApplied==WCA_NOSELECTION)
		ShowClickableArea(2);
	
	//If the default attribute is applied to the selection...
	if (aasApplied==WCA_DEFAULT)
		//Then if the selection consists only of text, set the "Rectangle" radio button.
		//Otherwise, set the "Object shape" radio button
		ShowClickableArea(SelectionConsistsOfText());

	//AutoCorrect switch
	ShowCorrect(ms_fCorrect);

	//Buttons

	//Grey the Add/Change button
	EnableAdd(FALSE);

	//If one or many attributes are applied to the selection, enable the Remove button.
	//Otherwise, grey the Remove button
	EnableRemove((aasApplied==WCA_SINGLE || aasApplied==WCA_MANY));
}

/********************************************************************************************

	void WebAddressDlg::ApplyWebAddress()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Applies the values in the dialog box to the selection

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void WebAddressDlg::ApplyWebAddress()
{	
	//First get the values out of the dialog box
	//Note that the function GetFrameComboBoxValue() will compare
	//the string in the Frame combo box with the list items.
	//If the string is a list item (such as "New window (_blank)")
	//the string returned to us will be a short version ("_blank")
	String_256 strURL=GetURL();														
	
	String_256 strFrame=GetFrame();

	BOOL fBounding=(GetClickableArea()==1);

	//Now, create a set of flags which tell us how the Web Address should be corrected
	WebCorrectFlags wcfToApply;

	//Since these flags have been created using the default constructor, they
	//indicate that the URL should not be corrected at all

	//Now, should we be correcting the URL?
	if (ms_fCorrect)
	{
		//Yes. So set the correction flags to the defaults for this dialog
		wcfToApply.SetForWebAddressDialog();
	}

	//Now create a Web Address object, which will correct the URL
	WebAddress urlToApply(strURL, wcfToApply);

	//And make a new Web Address attribute
	AttrWebAddress* pNodeToApply=new AttrWebAddress(urlToApply, fBounding, strFrame);

	//And apply it to the selection
	AttributeManager::AttributeSelected((NodeAttribute *) pNodeToApply);

}

/********************************************************************************************

	void WebAddressDlg::RemoveWebAddress()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	
	Purpose:	Removes any Web Address which is on the selection (i.e. applies
				the default Web Address to the selection).

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void WebAddressDlg::RemoveWebAddress()
{		
	//Simply get the default Web Address
	WebAddressAttribute waaDefault;

	if (AttributeManager::GetDefaultAttribute(ATTR_WEBADDRESS, &waaDefault))
	{
		//Make it into an AttrWebAddress
		AttrWebAddress* pNodeToApply=new AttrWebAddress();
		
		if (pNodeToApply)
		{
			pNodeToApply->Value=waaDefault;

			//And apply it to the selection
			AttributeManager::AttributeSelected((NodeAttribute *) pNodeToApply);
		}
	}


}

/********************************************************************************************

	WebAddressDlg control functions

	Functions that set the dialog box controls or get information from them

********************************************************************************************/

/********************************************************************************************

	void WebAddressDlg::ShowURL(TCHAR* pcURL)


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		pcURL - the URL string to show in the field
	Outputs:	-

	Returns:	TRUE if no problems
	
	Purpose:	Shows the URL in the dialog box.



	Errors:		-
	SeeAlso:	-

  Technical details:
	Whenever we set text into an edit field, a "Text changed" message will
	be sent round, as if someone had typed into that field. We must ignore
	this, so we set a flag to do so.

	This doesn't occur when you set text into a combo box.

	It's a bug in the Dialog Manager and it should really be fixed.

********************************************************************************************/

void WebAddressDlg::ShowURL(TCHAR* pcURL)
{	
	//First set a flag to ensure we don't handle the "Text changed" message
	//that will occur when we change the edit field.
	WebAddressDlg::DontHandleNextTextMessage=TRUE;

	//Is pcURL NULL?
	if (pcURL==NULL)
		//Yes. So set a blank string in the edit field
		SetStringGadgetValue(_R(IDC_WEBADDRESS_URL), &String_256(""), FALSE, -1);
	else
	{
		//No. So set the string we have been given in the edit field
		String_256 strToSet=pcURL;
		SetStringGadgetValue(_R(IDC_WEBADDRESS_URL), &strToSet, FALSE, -1);
	}
	
}

/********************************************************************************************

	void WebAddressDlg::ShowURL(UINT32 uiURL)


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		uiURL - resource number of the URL to show
	Outputs:	-

	Returns:	TRUE if no problems
	
	Purpose:	Shows the URL in the dialog box.



	Errors:		-
	SeeAlso:	-

  Technical details:
	Whenever we set text into an edit field, a "Text changed" message will
	be sent round, as if someone had typed into that field. We must ignore
	this, so we set a flag to do so.

	This doesn't occur when you set text into a combo box.

	It's a bug in the Dialog Manager and it should really be fixed.

********************************************************************************************/

void WebAddressDlg::ShowURL(UINT32 uiURL)
{	
	//First set a flag to ensure we don't handle the "Text changed" message
	//that will occur when we change the edit field.
	WebAddressDlg::DontHandleNextTextMessage=TRUE;

	//Simply set the string
	SetStringGadgetValue(_R(IDC_WEBADDRESS_URL), uiURL, FALSE, -1);
	
}


/********************************************************************************************

	String_256 WebAddressDlg::GetURL()  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-

	Returns:	The string in the URL field
	
	Purpose:	Finds the value that the URL field contains.
												  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

String_256 WebAddressDlg::GetURL()
{	
	return GetStringGadgetValue(_R(IDC_WEBADDRESS_URL));
}

/********************************************************************************************

	void WebAddressDlg::ShowCorrect(BOOL fValue)


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		fValue - whether to show the correction features as on or off
	Outputs:	-

	Returns:	-
	
	Purpose:	Shows the AutoCorrect state in the dialog.



	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void WebAddressDlg::ShowCorrect(BOOL fValue)
{	
	//Set the switch
	SetLongGadgetValue(_R(IDC_WEBADDRESS_CORRECT), fValue);
	
}


/********************************************************************************************

	BOOL WebAddressDlg::GetCorrect()  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-

	Returns:	The state of the AutoCorrect switch
	
	Purpose:	Finds the state of the AutoCorrect switch
												  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL WebAddressDlg::GetCorrect()
{	
	return GetLongGadgetValue(_R(IDC_WEBADDRESS_CORRECT), FALSE, TRUE, 0);
}


/********************************************************************************************

	void WebAddressDlg::ShowFrame(TCHAR* pcFrame)
										   
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		pcFrame - the frame string to show in the field
	Outputs:	-

	Returns:	TRUE if no problems
	
	Purpose:	Shows the Frame in the dialog box.

				Before showing the string, we check to see if is one of the four
				short strings (_self, _top, _parent and _blank) that correspond
				to values in the combo box. If it is, we select the appropriate
				list item in the combo box instead.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void WebAddressDlg::ShowFrame(TCHAR* pcFrame)
{	
	//Is pcFrame NULL?
	if (pcFrame==NULL)
		//Yes. So set a blank string in the frame field
		SetStringGadgetValue(_R(IDC_WEBADDRESS_FRAME), &String_256(""), FALSE, -1);
	else
	{
		//No.

		//First put the string into one of Camelot's string classes
		String_256 strFrame=pcFrame;

		//And check to see if the string is one of our short strings
		//This if/else statement is basically a large switch statement
					
		if (strFrame==String_256(_R(IDS_WEBADDRESS_SHORT_SELF)))
			SetSelectedValueIndex(_R(IDC_WEBADDRESS_FRAME), 0);

		else if (strFrame==String_256(_R(IDS_WEBADDRESS_SHORT_PARENT)))
			SetSelectedValueIndex(_R(IDC_WEBADDRESS_FRAME), 1);

		else if (strFrame==String_256(_R(IDS_WEBADDRESS_SHORT_TOP)))
			SetSelectedValueIndex(_R(IDC_WEBADDRESS_FRAME), 2);

		else if (strFrame==String_256(_R(IDS_WEBADDRESS_SHORT_BLANK)))
			SetSelectedValueIndex(_R(IDC_WEBADDRESS_FRAME), 3);

		else if (strFrame==String_256(_R(IDS_WEBADDRESS_SHORT_DEFAULT)))
			SetSelectedValueIndex(_R(IDC_WEBADDRESS_FRAME), 4);

		else
			//Otherwise, set the string in the edit field
			SetStringGadgetValue(_R(IDC_WEBADDRESS_FRAME), &strFrame, FALSE, -1);
		
	}
		 	
}

/********************************************************************************************

	void WebAddressDlg::ShowFrame(UINT32 uiFrame)


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		uiFrame - resource number of the URL to show
	Outputs:	-

	Returns:	TRUE if no problems
	
	Purpose:	Shows the Frame in the dialog box.



	Errors:		-
	SeeAlso:	-


********************************************************************************************/

void WebAddressDlg::ShowFrame(UINT32 uiFrame)
{	
	//First set a flag to ensure we don't handle the "Text changed" message
	//that will occur when we change the edit field.
	WebAddressDlg::DontHandleNextTextMessage=TRUE;

	//Simply set the string
	SetStringGadgetValue(_R(IDC_WEBADDRESS_FRAME), uiFrame, FALSE, -1);
	
}


/********************************************************************************************

	String_256 WebAddressDlg::GetFrame()  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-

	Returns:	The string in the Frame combo box or, if the string
				in the combo box is one of our preset strings (such as
				"New window (_blank)"), then we return a shorter
				version of that string ("_blank").
	
	Purpose:	Finds the value that the Frame combo box contains.
							

	Errors:		-
	SeeAlso:	-

  Technical details:

  We could use "GetSelectedValueIndex()" to find if one of our preset strings had
  been selected.

  But consider the case where the user selected one of our present strings, then
  accidentally clicked in the Frame edit field without changing anythin.

  In this case, the selected value (-1) would tell us that the combo box held
  a value that the user had typed in the edit field...not one of the preset strings.

  So, instead, to find if one of our preset strings is in the combo box, we do
  a direct string comparison.

********************************************************************************************/

String_256 WebAddressDlg::GetFrame()  
{	
	//Get the string from the combo box
	String_256 strCombo=GetStringGadgetValue(_R(IDC_WEBADDRESS_FRAME));

	//Check the string to see if it's one we want to stop
	if (strCombo==String_256(_R(IDS_WEBADDRESS_COMBO_SELF)))
		return String_256(_R(IDS_WEBADDRESS_SHORT_SELF));

	if (strCombo==String_256(_R(IDS_WEBADDRESS_COMBO_PARENT)))
		return String_256(_R(IDS_WEBADDRESS_SHORT_PARENT));

	if (strCombo==String_256(_R(IDS_WEBADDRESS_COMBO_TOP)))
		return String_256(_R(IDS_WEBADDRESS_SHORT_TOP));

	if (strCombo==String_256(_R(IDS_WEBADDRESS_COMBO_BLANK)))
		return String_256(_R(IDS_WEBADDRESS_SHORT_BLANK));
	
	if (strCombo==String_256(_R(IDS_WEBADDRESS_COMBO_DEFAULT)))
		return String_256(_R(IDS_WEBADDRESS_SHORT_DEFAULT));

	//Otherwise, simply return the string
	return strCombo;

}

/********************************************************************************************

	void WebAddressDlg::ShowClickableArea(UINT32 uiArea)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		uiArea	A number describing the clickable area to show:
						0	Object shape
						1	Rectangle
						2	Indeterminate
	Outputs:	-

	Returns:	-
	
	  Purpose:	Shows the clickable area in the dialog box as follows:

				0	Object shape radio button is on, rectangle radio button is off
				1	Rectangle radio button is on, object radio button is off
				2	Both radio buttons are off

							

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void WebAddressDlg::ShowClickableArea(UINT32 uiArea)  
{	
	SetLongGadgetValue(_R(IDC_WEBADDRESS_OBJECTSHAPE), (uiArea==0));
	SetLongGadgetValue(_R(IDC_WEBADDRESS_RECTANGLE), (uiArea==1));
}

/********************************************************************************************

	UINT32 WebAddressDlg::GetClickableArea()  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	-
	Returns:	uiArea	A number describing the clickable area:
						0	Object shape
						1	Rectangle
						2	Indeterminate

	Purpose:	Gets the clickable area shown in the dialog box

							

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 WebAddressDlg::GetClickableArea()  
{	
	BOOL fObjectShape=GetLongGadgetValue(_R(IDC_WEBADDRESS_OBJECTSHAPE), 0, 0);
	BOOL fRectangle=GetLongGadgetValue(_R(IDC_WEBADDRESS_RECTANGLE), 0, 0);

	if (fObjectShape && !fRectangle)
		return 0;

	if (!fObjectShape && fRectangle)
		return 1;
	
	return 2;
}

/********************************************************************************************

	void WebAddressDlg::EnableAdd(BOOL fValue=TRUE)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		TRUE to allow the user to add the web address
				FALSE to prevent the user from adding the Web Address
	
	Outputs:	-

	Returns:	-
	
	Purpose:	Switches the Add button on or off							

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void WebAddressDlg::EnableAdd(BOOL fValue)  
{	
	EnableGadget(IDOK, fValue);
}

/********************************************************************************************

	void WebAddressDlg::EnableRemove(BOOL fValue=TRUE)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		TRUE to allow the user to remove the Web Address
				FALSE to prevent the user from removing the Web Address
	
	Outputs:	-

	Returns:	-
	
	Purpose:	Switches the Remove button on or off							

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void WebAddressDlg::EnableRemove(BOOL fValue)  
{	
	EnableGadget(_R(IDC_WEBADDRESS_REMOVE), fValue);
}

/********************************************************************************************

	void WebAddressDlg::ShowWebAddressOnSelection(BOOL fValue=TRUE)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		TRUE to tell the user there is a Web Address on the selection
				FALSE to tell the user there is no Web Address on the selection
	
	Outputs:	-

	Returns:	-
	
	Purpose:	Changes the text on the Add/Change button to Change if there
				is a Web Address or Add if there isn't.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void WebAddressDlg::ShowWebAddressOnSelection(BOOL fValue)  
{	
	if (fValue)
		SetStringGadgetValue(IDOK, _R(IDS_ADDBUTTON_CHANGE));
	else
		SetStringGadgetValue(IDOK, _R(IDS_ADDBUTTON_ADD));
}

/********************************************************************************************

	WebAddressDlg toolkit functions

	Protected functions that are called by other WebAddressDlg functions.

********************************************************************************************/

/********************************************************************************************

	WebCommonAttributeResult WebAddressDlg::FindCommonWebAttribute(WebAddressAttribute* pwaaReturn)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/97
	Inputs:		-
	Outputs:	pwaaReturn	The caller should set this to point at a WebAddressAttribute
							if they want  a copy of the applied attribute returned.

	Returns:	WCA_DEFAULT	The default WebAddressAttribute is applied
							to the entire selection

				WCA_SINGLE	The same or identical WebAddressAttributes are
							applied to the whole selection

				WCA_MANY	Different WebAddressAttributes are applied to
							the selection

				WCA_NOSELECTION
							Nothing is selected
	
	Purpose:	Finds if the selection has a common Web Address attribute.

				It does this by calling Range::FindCommonAttribute, then if there's 
				a single attribute applied, doing a quick check to see if that 
				attribute is the default attribute of its type.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

WebCommonAttributeResult WebAddressDlg::FindCommonWebAttribute(WebAddressAttribute* pwaaReturn)
{	
	//First we need to get the current selection
	SelRange* pSelRange=GetApplication()->FindSelection();

	//If pSelRange doesn't exist, return No Selection
	if (!pSelRange)
		return WCA_NOSELECTION;	
	
	//Find the common Web Attribute on the selection
	Range::CommonAttribResult carResult;
	NodeAttribute* pnaApplied;
	
	carResult=pSelRange->FindCommonAttribute(CC_RUNTIME_CLASS(AttrWebAddress), &pnaApplied);
	
	//If this function has returned ATTR_NONE or ATTR_MANY, return our
	//analogous value
	if (carResult==SelRange::ATTR_NONE)
		return WCA_NOSELECTION;

	if (carResult==SelRange::ATTR_MANY)
		return WCA_MANY;

	//Otherwise, we know we have one Web Address attribute on the selection.

	//We must find out whether it is the default attribute

	//First extract the attribute value from the node we have found
	WebAddressAttribute* pWebAddressAttribute=
		(WebAddressAttribute*) pnaApplied->GetAttributeValue();

	WebAddressAttribute waaApplied=*pWebAddressAttribute;

	//And if the user wants this value returned, make a copy of it now
	if (pwaaReturn)
		*pwaaReturn=waaApplied;

	//And get the default Web Address attribute
	WebAddressAttribute waaDefault;
	if (AttributeManager::GetDefaultAttribute(ATTR_WEBADDRESS, &waaDefault))
	{
		//If they are the same, return WCA_DEFAULT. 
		//If they are different, return WCA_SINGLE
		if (!waaApplied.IsDifferent(&waaDefault))
			return WCA_DEFAULT;
	}
	
	return WCA_SINGLE;
	
}



/********************************************************************************************

	BOOL WebAddressDlg::SelectionConsistsOfText()  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/97
	Inputs:		-
	Outputs:	-

	Returns:	TRUE if the selection consists entirely of text objects
				FALSE otherwise
	
	Purpose:	To find if the selection is entirely text

	Errors:		If there is no selection

	SeeAlso:	Range::ConsistsOf

********************************************************************************************/

BOOL WebAddressDlg::SelectionConsistsOfText()  
{	
	//First get the current selection
	SelRange* pSelection=GetApplication()->FindSelection();
		
	//And use the Range::ConsistsOf function to find whether the selection
	//is entirely text.

	if (pSelection)
		return pSelection->ConsistsOf(CC_RUNTIME_CLASS(BaseTextClass));
	else
		return FALSE;
}
