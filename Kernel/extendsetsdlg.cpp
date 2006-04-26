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
// Implementation of the ExtendSetsDlg class.

#include "camtypes.h"

#include "extendsetsdlg.h"	// this file's header.
#include "ngcore.h"			// for NameGallery.
#include "ngitem.h"			// for SGNameItem

#include "ccdc.h"			// apparently this supplies some #includes req'd for kernel dlgs.
//#include "extendres.h"		// our dialog resources.

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

DECLARE_SOURCE("$Revision$");

// dynamic class creation stuff.
CC_IMPLEMENT_DYNCREATE(ExtendSetsDlg, DialogOp);

#define new CAM_DEBUG_NEW

// static variables used in dialog initialisation.
const CDlgMode ExtendSetsDlg::Mode = MODAL;
const UINT32 ExtendSetsDlg::IDD = _R(IDD_EXTENDSETSDLG);

// static indices into the extend-behaviour combo-boxes
const INT32 ExtendSetsDlg::EXTEND_BEHAVIOUR_NONE		= 0;
const INT32 ExtendSetsDlg::EXTEND_BEHAVIOUR_EXTEND	= 1;
const INT32 ExtendSetsDlg::EXTEND_BEHAVIOUR_STRETCH	= 2;

// pointer to the one and only instance of this class.
ExtendSetsDlg* ExtendSetsDlg::TheDialog = NULL;



/********************************************************************************************
>	ExtendSetsDlg::ExtendSetsDlg() : DialogOp(ExtendSetsDlg::IDD, ExtendSetsDlg::Mode)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/1999
	Purpose:	Constructor
********************************************************************************************/

ExtendSetsDlg::ExtendSetsDlg() : DialogOp(ExtendSetsDlg::IDD, ExtendSetsDlg::Mode)
{
	// initialise our public static pointer to ourself.
	if (TheDialog == NULL)
		TheDialog = this;

	// initialise our member variables.
	m_bArrayExtendFlags	= NULL;
	m_pDefineExtendSet	= NULL;
	m_pbCommit			= NULL;
	m_pfExtendFlags		= NULL;
}



/********************************************************************************************
>	ExtendSetsDlg::~ExtendSetsDlg()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/1999
	Purpose:	Destructor
********************************************************************************************/

ExtendSetsDlg::~ExtendSetsDlg()
{
	// the public static pointer must no longer point to us.
	if (TheDialog == this)
		TheDialog = NULL;

	// set all our data pointers to NULL, just to clear up.
	m_bArrayExtendFlags	= NULL;
	m_pDefineExtendSet	= NULL;
	m_pbCommit			= NULL;
	m_pfExtendFlags		= NULL;
}



/********************************************************************************************
>	MsgResult ExtendSetsDlg::Message(Msg* Message)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/1999
	Inputs:		Message		the incoming message.
	Returns:	DialogOp::Message(Message)
	Purpose:	Message handler for the ExtendSetsDlg.
********************************************************************************************/

MsgResult ExtendSetsDlg::Message(Msg* Message)
{
	BOOL bEndDialog = FALSE;

	// only process messages _intended_ for this or all dialogs.
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* pDialogMsg = (DialogMsg*)Message;

		switch (pDialogMsg->DlgMsg)
		{
		// sent when we must initialise the dialog, after it has been created.
		case DIM_CREATE:
			SetUpControls();
			break;

		// sent to dismiss and cancel the dialog.
		// CANCEL was chosen, so note that our list of named sets is now invalid.
		case DIM_CANCEL:
			*m_pbCommit = FALSE;
			bEndDialog = TRUE;
			break;

		// sent to dismiss and confirm the dialog.
		case DIM_COMMIT:
			*m_pbCommit = TRUE;
			GenerateListOfSets();
			bEndDialog = TRUE;
			break;

		// further message processing...
		default:
			break;
		}	// end switch (pDialogMsg->DlgMsg)
	}

	// if necessary, close the dialog.
	if (bEndDialog)
	{
		Close();
		End();
	}

	return DialogOp::Message(Message);
}



/********************************************************************************************
>	OpState ExtendSetsDlg::GetState(String_256*, OpDescriptor*)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/1999
	Inputs:		Does not make use of its inputs in any way.
	Returns:	the OpState of this dialog operation.
	Purpose:	Returns the OpState of this dialog operation.
********************************************************************************************/

OpState ExtendSetsDlg::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	// set the enabled state of any buttons/menus which work with this dialog.
	if (TheDialog != NULL)
		OpSt.Ticked = TRUE;

	return OpSt;
}



/********************************************************************************************
>	BOOL ExtendSetsDlg::Init()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/1999
	Returns:	TRUE if successful,
				FALSE otherwise (probably due to lack of memory).
	Purpose:	Create an OpDescriptor for the ExtendSetsDlg.
				May also declare ExtendSetsDlg preferences, although
				it does not at present.
	TODO:
				I don't know what half of the parameters below do,
				so this needs to be investigated!

	OpDescriptor constructor's arguments:-

				 UINT32 toolID,                      	// module ID, 0 if not in tool
				 UINT32 txID,                         // UI descriptions of op
				 CCRuntimeClass* Op,				// pointer to the Op's runtime class
				 TCHAR* tok,						// pointer to the OPTOKEN string
				 pfnGetState gs,					// pointer to the UI GetState function
				 UINT32 helpId = 0,					// assoc. help system identifier 
				 UINT32 bubbleID = 0,					// string resource for bubble help
				 UINT32 resourceID = 0,				// bitmap ("icon") or resource ID
				 UINT32 controlID =0,					// assoc. gadget control ID
				 BOOL ReceiveMessages = FALSE,		// send this OpDescriptor kernel Msgs
				 BOOL Smart = FALSE,				// op does smart attr optimisation
				 BOOL Clean = TRUE,					// op doesn't modify document
				 UINT32 OneOpenInstID = 0,			// When not 0 only one live instance
				 									// of the operation is permitted, and
													// OneOpenInstID is the string resource
													// ID describing why.This will be useful
				 									// for single instance only dialogs.
				 UINT32 AutoStateFlags = 0			// Flags which indicate conditions when
				 									// the operation should automatically be
													// greyed/ticked etc..(This cuts down the 
													// number of tests that need to be made
													// in the GetState function).
********************************************************************************************/

BOOL ExtendSetsDlg::Init()
{
	return (RegisterOpDescriptor(0,								// Tool ID
 								_R(IDS_EXTENDSETS),	 				// String resource ID
								CC_RUNTIME_CLASS(ExtendSetsDlg),// Runtime class
 								OPTOKEN_EXTENDSETSDLG,			// Token string
 								ExtendSetsDlg::GetState,		// GetState function
								0,								// Help ID
								_R(IDBBL_EXTENDSETS),				// Bubble ID
								0,								// Resource ID
								0,								// Control ID
								SYSTEMBAR_ILLEGAL,				// Bar ID
								TRUE,							// Receive system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								0,								// No vertical counterpart
								NULL,
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC)
								));
}



/********************************************************************************************
>	void ExtendSetsDlg::Do(OpDescriptor*)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/1999
	Inputs:		Does not make use of its inputs in any way.
	Purpose:	This function should not be called - DoWithParam should be used instead.
	Errors:		ERROR3 under debug, does nothing at all under release.
********************************************************************************************/

void ExtendSetsDlg::Do(OpDescriptor*)
{
	ERROR3("ExtendSetsDlg::Do- this function should not be called - use DoWithParam instead.");
}



/********************************************************************************************
>	void ExtendSetsDlg::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/1999
	Inputs:		pOp		an OpDescriptor which currently is not actually used.
				pParam	pointer to an OpParam structure, which is really of type
						OpParamBarCreationDlg, allowing this dialog to be created
						with specific initialisation parameters.
	Purpose:	Opens or dismisses the ExtendSetsDlg.
	Errors:		ERROR2 if any of the parameters are NULL.
********************************************************************************************/

void ExtendSetsDlg::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	// use pParam to initialise any internal member variables, eg:
	m_bArrayExtendFlags = ((OpParamExtendSetsDlg*)pParam)->m_bArrayExtendFlags;
	m_pDefineExtendSet = ((OpParamExtendSetsDlg*)pParam)->m_pDefineExtendSet;
	m_pbCommit = ((OpParamExtendSetsDlg*)pParam)->m_pbCommit;
	m_pfExtendFlags = ((OpParamExtendSetsDlg*)pParam)->m_pfExtendFlags;

	// validate input parameters.
	if (m_bArrayExtendFlags == NULL || m_pDefineExtendSet == NULL ||
		m_pbCommit == NULL || m_pfExtendFlags == NULL)
	{
		ERROR2RAW("ExtendSetsDlg::DoWithParam- one or more parameters was NULL!");
		End();
		return;
	}

	// attempt to create a new dialog.
	if ( !Create() )
	{
		// if we couldn't create the dialog, tell the user and end the operation.
		InformError();
  		End();
	}
}



/********************************************************************************************
>	BOOL ExtendSetsDlg::Create()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/1999
	Returns:	TRUE if successful,
				FALSE otherwise.
	Purpose:	Create an ExtendSetsDlg dialog box.
	See also:	DialogOp::Create
********************************************************************************************/

BOOL ExtendSetsDlg::Create()
{
	// perform any custom initialisation.

	// only allow one instance of this dialog.
	if (TheDialog != this)
		return FALSE;

	// call the base class implementation, to actually create the dialog.
	return DialogOp::Create();
}



/********************************************************************************************
>	BOOL ExtendSetsDlg::SetUpControls()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/1999
	Returns:	TRUE always.
	Purpose:	Initialise the dialog's set of controls.
********************************************************************************************/

BOOL ExtendSetsDlg::SetUpControls()
{
	// initialise controls here.

	// get the name of the set we are editing.
	String_256 strName;
	if (m_pDefineExtendSet != NULL)
		m_pDefineExtendSet->GetNameText(&strName);
	else
		strName = "TestName";	// this should never happen, but just in case...

	// set the dialog's title text.
	String_256 strPrompt;
	strPrompt.MakeMsg(_R(IDS_ES_EXTENDTITLE), &strName);
	SetTitlebarName(&strPrompt);

	// set the text of the 'extended-by' prompt
	strPrompt.MakeMsg(_R(IDS_ES_EXTENDEDBY), &strName);
	SetStringGadgetValue(_R(IDC_ES_EXTENDEDBY), &strPrompt);

	// set the text of the 'extend-type' prompt
	strPrompt.MakeMsg(_R(IDS_ES_EXTENDTYPE), &strName);
	SetStringGadgetValue(_R(IDC_ES_EXTENDTYPE), &strPrompt);

	// set the text of the horizontal and vertical prompts
	SetStringGadgetValue(_R(IDC_ES_VERTICAL), _R(IDS_ES_VERTICAL));
	SetStringGadgetValue(_R(IDC_ES_HORIZONTAL), _R(IDS_ES_HORIZONTAL));

	// add 'None', 'Extend' and 'Stretch' values to our two combo-boxes, and initialise them.
	INT32 selIndex = 0;
	SetStringGadgetValue(_R(IDC_ES_COMBO_XBEHAVIOUR), _R(IDS_ES_BEHAVIOUR_NONE), FALSE, EXTEND_BEHAVIOUR_NONE);
	SetStringGadgetValue(_R(IDC_ES_COMBO_XBEHAVIOUR), _R(IDS_ES_BEHAVIOUR_EXTEND), FALSE, EXTEND_BEHAVIOUR_EXTEND);
	SetStringGadgetValue(_R(IDC_ES_COMBO_XBEHAVIOUR), _R(IDS_ES_BEHAVIOUR_STRETCH), TRUE, EXTEND_BEHAVIOUR_STRETCH);

	selIndex =	(*m_pfExtendFlags & X_EXTEND) ?		EXTEND_BEHAVIOUR_EXTEND :
				(*m_pfExtendFlags & X_STRETCH) ?	EXTEND_BEHAVIOUR_STRETCH :
													EXTEND_BEHAVIOUR_NONE;
	SetSelectedValueIndex(_R(IDC_ES_COMBO_XBEHAVIOUR), selIndex);
	SetComboListLength(_R(IDC_ES_COMBO_XBEHAVIOUR));

	SetStringGadgetValue(_R(IDC_ES_COMBO_YBEHAVIOUR), _R(IDS_ES_BEHAVIOUR_NONE), FALSE, EXTEND_BEHAVIOUR_NONE);
	SetStringGadgetValue(_R(IDC_ES_COMBO_YBEHAVIOUR), _R(IDS_ES_BEHAVIOUR_EXTEND), FALSE, EXTEND_BEHAVIOUR_EXTEND);
	SetStringGadgetValue(_R(IDC_ES_COMBO_YBEHAVIOUR), _R(IDS_ES_BEHAVIOUR_STRETCH), TRUE, EXTEND_BEHAVIOUR_STRETCH);

	selIndex =	(*m_pfExtendFlags & Y_EXTEND) ?		EXTEND_BEHAVIOUR_EXTEND :
				(*m_pfExtendFlags & Y_STRETCH) ?	EXTEND_BEHAVIOUR_STRETCH :
													EXTEND_BEHAVIOUR_NONE;
	SetSelectedValueIndex(_R(IDC_ES_COMBO_YBEHAVIOUR), selIndex);
	SetComboListLength(_R(IDC_ES_COMBO_YBEHAVIOUR));

	// ok, now add our ultra-funky check-list box to our dialog.
	BOOL ok = m_tclbExtendingSets.Create(WindowID, _R(IDC_ES_CHECKLIST));
	if (ok)
		FillCheckListBox();

	return ok;
}



/********************************************************************************************
>	BOOL ExtendSetsDlg::FillCheckListBox()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/11/1999
	Inputs:		
	Outputs:	
	Returns:	TRUE always.
	Purpose:	Fill our check-list box with a list of all named sets currently
				in the document. We disable the item for the named set which is
				doing the extending, and if there are no named sets then we put
				a message saying so into the list box.
	Errors:		
	See also:	
********************************************************************************************/

BOOL ExtendSetsDlg::FillCheckListBox()
{
	// iterate over all names used within the document.
	NameGallery* pNameGallery = NameGallery::Instance();
	if (!pNameGallery)
		return TRUE;
	SGUsedNames* pNames = pNameGallery->GetUsedNames();
	SGNameItem* pItem = (SGNameItem*) pNames->GetChild();

	// if there are no named sets then add a disabled item to tell the user that there are
	// no named sets (this should never happen, as we have to be called with a set to extend)
	INT32 nIndex;
	String_256 strName;
	strName.Empty();
	if (pItem == NULL)
	{
		strName.Load(_R(IDS_ES_NO_NAMED_SETS));
		nIndex = m_tclbExtendingSets.AddString((TCHAR*)strName);
		m_tclbExtendingSets.Enable(nIndex, FALSE);
	}

	// ...otherwise, fill the check-list with all named sets.
	nIndex = 0;
	for (; pItem != NULL; pItem = (SGNameItem*) pItem->GetNext())
	{
		// Don't show the target item - it's redundant, illogical and confusing.
		if (pItem == m_pDefineExtendSet) continue;

		// add a ptr to the SGNameItem and also add its text to the checklistbox.
		pItem->GetNameText(&strName);
		nIndex = m_tclbExtendingSets.AddString((TCHAR*)strName);
		m_tclbExtendingSets.SetItemDataPtr(nIndex, pItem);
		m_tclbExtendingSets.SetCheck(nIndex, m_bArrayExtendFlags[nIndex]);
	}

	return TRUE;
}



/********************************************************************************************
>	void ExtendSetsDlg::GenerateListOfSets()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/11/1999
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Regenerates our list of ticked named sets.
	Errors:		
	See also:	
********************************************************************************************/

void ExtendSetsDlg::GenerateListOfSets()
{
	// go through each item in our checklist box and set a corresponding flag in our byte
	// array of flags, depending on its checked (also enabled? and/or visible?) status.
	INT32 nCount = m_tclbExtendingSets.GetCount();
	for (INT32 i = 0; i < nCount; i ++)
	{
		if (m_tclbExtendingSets.GetCheck(i) == 1)
			m_bArrayExtendFlags[i] = 1;
		else
			m_bArrayExtendFlags[i] = 0;
	}

	// set the contents of our pointer to the extend behaviour flag.
	BYTE fExtend = 0;
	switch (GetSelectedValueIndex(_R(IDC_ES_COMBO_XBEHAVIOUR)))		// x-axis behaviour
	{
	case (ExtendSetsDlg::EXTEND_BEHAVIOUR_EXTEND):
		fExtend |= X_EXTEND;
		break;

	case EXTEND_BEHAVIOUR_STRETCH:
		fExtend |= X_STRETCH;
		break;

	case EXTEND_BEHAVIOUR_NONE:
	default:
		// we do nothing if the index is set to none, or if we get an invalid value.
		break;
	}

	switch (GetSelectedValueIndex(_R(IDC_ES_COMBO_YBEHAVIOUR)))		// y-axis behaviour
	{
	case EXTEND_BEHAVIOUR_EXTEND:
		fExtend |= Y_EXTEND;
		break;

	case EXTEND_BEHAVIOUR_STRETCH:
		fExtend |= Y_STRETCH;
		break;

	case EXTEND_BEHAVIOUR_NONE:
	default:
		// we do nothing if the index is set to none, or if we get an invalid value.
		break;
	}

	*m_pfExtendFlags = fExtend;
}



//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
