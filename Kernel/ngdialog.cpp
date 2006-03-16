// $Id: ngdialog.cpp 662 2006-03-14 21:31:49Z alex $
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
/*
	$Header: /Camelot/kernel/ngdialog.cpp 4     12/09/00 12:31 Simonk $
	Attribute gallery dialog operations
*/

#include "camtypes.h"

#include "ngcore.h"
#include "ngdialog.h"
#include "ngitem.h"
#include "ngsentry.h"
#include "ngsetop.h"

#include "app.h"
#include "sginit.h"
#include "comattrmsg.h"

//#include "resource.h"
//#include "barsdlgs.h"
//#include "galres.h"

//#include "mario.h"
//#include "justin.h"
//#include "justin3.h"
#include "slicehelper.h"
// for the use of wix temple attribs
#include "cxfrech.h"
#include "userattr.h"
#include "tmpltatr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

DECLARE_SOURCE("$Revision: 662 $");

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(NameObjectsDlg, DialogOp);
CC_IMPLEMENT_DYNCREATE(RenameObjectsDlg, DialogOp);
CC_IMPLEMENT_DYNCREATE(OpNameGalleryPropIndexDesc, OpDescriptor);
CC_IMPLEMENT_DYNCREATE(OpDisplayNameGallery, Operation);

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW

// Preference for modal (default) or modeless Name Objects dialog.
BOOL NameObjectsDlg::m_fModeless = FALSE;


/********************************************************************************************
>	BaseNameObjectsDlg::BaseNameObjectsDlg(CDlgResID idRes, CDlgMode nMode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/99
	Inputs:		idRes		---		resource ID of the dialog template to use
				nMode		---		mode to run in, MODAL or MODELESS
	Purpose:	Create a BaseNameObjectsDlg, an abstract base class for the Name Objects
				and	Rename Objects dialogs.
	SeeAlso:	NameObjectsDlg; RenameObjectsDlg
********************************************************************************************/

BaseNameObjectsDlg::BaseNameObjectsDlg(CDlgResID idRes, CDlgMode nMode)
  :	DialogOp(idRes, nMode)
{   
	// Empty.
}        




/********************************************************************************************
>	virtual void BaseNameObjectsDlg::Do(OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/99
	Purpose:	Creates and runs a BaseNameObjectsDlg.
	SeeAlso:	NameObjectsDlg; RenameObjectsDlg
********************************************************************************************/

void BaseNameObjectsDlg::Do(OpDescriptor*)
{
	// Try to run this operation.
	if (!Create())
	{
		InformError(0, _R(IDS_OK));
		End();
	}
}



/********************************************************************************************
>	virtual MsgResult BaseNameObjectsDlg::Message(Msg* pMessage)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/99
	Purpose:	Handles kernel messages for the BaseNameObjectsDlg dialog operation.
********************************************************************************************/

MsgResult BaseNameObjectsDlg::Message(Msg* pMessage)
{
	// A message from the dialog or its gadgets?
	if (IS_OUR_DIALOG_MSG(pMessage))
	{
		DialogMsg* pMsg = (DialogMsg*) pMessage;
		switch (pMsg->DlgMsg)
		{
		case DIM_CREATE:
			InitGadgetText();
			break;

		case DIM_TEXT_CHANGED:
			// Disable the OK/Apply button if there's no document or entered text.
			EnableGadget(IDOK, !GetStringGadgetValue(_R(IDC_NAMEDLG_NAME), 0).IsEmpty()
							&& Document::GetSelected() != 0);
			break;
			
		case DIM_COMMIT:
			{
				// Check name for validity.
				String_256 strEnter = GetStringGadgetValue(_R(IDC_NAMEDLG_NAME), 0);
				UINT32 nErrID = IsValid(strEnter);
				if (nErrID != 0)
				{
					// Invalid, reinitialise.
					InformError(nErrID);
					InitGadgetText();
					break;
				}
				else
					// Entry is valid, try committing it.
					if (DoCommit(strEnter))
					{
						// Don't close, reinitialise (dialog is modeless).
						InitGadgetText();
						break;
					}
			}
			
		case DIM_CANCEL:
			Close();
			End();
			break;
		}
	}
	
	// Or has something changed the object selection?
	else if (MESSAGE_IS_A(pMessage, CommonAttrsChangedMsg) || 
			 (MESSAGE_IS_A(pMessage, OpMsg) && 
					(((OpMsg*) pMessage)->MsgType == OpMsg::AFTER_UNDO ||
					((OpMsg*) pMessage)->MsgType == OpMsg::AFTER_REDO)))
	{
		// Update the description.
		String_256 strEnter;
		SetStringGadgetValue(_R(IDC_NAMEDLG_DESC), GetDescription(&strEnter));

		// And if not changed or empty, also the suggestion.
		strEnter = GetStringGadgetValue(_R(IDC_NAMEDLG_NAME), 0);
		if (strEnter.IsEmpty() || strEnter == m_strSuggest)
		{
			SetStringGadgetValue(_R(IDC_NAMEDLG_NAME), GetSuggestion(&m_strSuggest));
			HighlightText(_R(IDC_NAMEDLG_NAME));
		}
	}

	// Has the document been switched or closed?
	else if (MESSAGE_IS_A(pMessage, DocChangingMsg))
	{
		DocChangingMsg* pMsg = (DocChangingMsg*) pMessage;
		if (pMsg->State == DocChangingMsg::SELCHANGED)
			EnableGadget(IDOK, pMsg->pNewDoc != 0);
	}

	// Pass everything on to the base class . . .
	return DialogOp::Message(pMessage);
}  



/********************************************************************************************
>	void BaseNameObjectsDlg::InitGadgetText()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Purpose:	Initialises the text of the gadgets in the dialog.
	SeeAlso:	BaseNameObjectsDlg::Message
********************************************************************************************/

void BaseNameObjectsDlg::InitGadgetText()
{
	String_256 str;
	SetStringGadgetValue(_R(IDC_NAMEDLG_DESC), GetDescription(&str));
	SetStringGadgetValue(_R(IDC_NAMEDLG_NAME), GetSuggestion(&m_strSuggest));
	HighlightText(_R(IDC_NAMEDLG_NAME));
	SetKeyboardFocus(_R(IDC_NAMEDLG_NAME));
}



/********************************************************************************************
>	virtual UINT32 BaseNameObjectsDlg::IsValid(const StringBase& strName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Inputs:		strName		---		name to test for suitability
	Returns:	0 if valid (by default any non-empty text is valid), or the ID of an
				error string explaining why if it is not.
	SeeAlso:	BaseNameObjectsDlg::Message
********************************************************************************************/

UINT32 BaseNameObjectsDlg::IsValid(const StringBase& strName)
{
	return 0;
}



/********************************************************************************************
>	NameObjectsDlg::NameObjectsDlg()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Purpose:	Constructs a NameObjectsDlg object, a modeless version of the base class
				dialog, which creates names of objects in the Attribute gallery.
	SeeAlso:	BaseNameObjectsDlg
********************************************************************************************/

NameObjectsDlg::NameObjectsDlg()
  :	BaseNameObjectsDlg(_R(IDD_NAMEDLG_NAME_TEMPLATE), m_fModeless ? MODELESS : MODAL)
{
	// Empty.
}



/********************************************************************************************
>	virtual StringBase* NameObjectsDlg::GetDescription(StringBase* pstrDesc)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Returns:	The input.
	Outputs:	pstrDesc	---		a description of the selected objects, if any,
									to be named.
	SeeAlso:	BaseNameObjectsDlg
********************************************************************************************/

StringBase* NameObjectsDlg::GetDescription(StringBase* pstrDesc)
{
	// No objects, one object, some objects.
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel->IsEmpty())
		pstrDesc->Load(_R(IDS_NAMEDLG_DESCRIBE_NOSELECT));
	else
		if (pSel->Count() == 1)
			pstrDesc->MakeMsg(_R(IDS_NAMEDLG_DESCRIBE_SELECT), &pSel->Describe(MENU));
		else
			pstrDesc->MakeMsg(_R(IDS_NAMEDLG_DESCRIBE_SELECT_PL), &pSel->Describe(STATUS_BAR));

	return pstrDesc;
}



/********************************************************************************************
>	virtual StringBase* NameObjectsDlg::GetSuggestion(StringBase* pstrSuggest)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Outputs:	pstrSuggest	 ---	a suggested name for the selected objects, if any.
	Returns:	pstrSuggest, ie. the input.
	Purpose:	Works out a suggestion for the name of the selected objects, if any.
	SeeAlso:	BaseNameObjectsDlg
********************************************************************************************/

StringBase* NameObjectsDlg::GetSuggestion(StringBase* pstrSuggest)
{
	// Work out what default text to put in the dialog edit-field.
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel->IsEmpty())
		pstrSuggest->Load(_R(IDS_NAMEDLG_DEFAULT_NAME));
	else
	{
		// Pluralisise / capitalise the selection description.
		if (pSel->Count() == 1)
		{
			*pstrSuggest = pSel->Describe(MENU);
			pstrSuggest->toTitle();
		}
		else
			*pstrSuggest = pSel->Describe(STATUS_BAR);
	}

	// Make the name unique, if necessary, by appending ascending numerals.
	Document* pDoc = Document::GetCurrent();
	if (pDoc != 0)
	{
		NodeSetSentinel* pSentry = pDoc->GetSetSentinel();
		if (pSentry != 0)
		{
			String_256 strBase = *pstrSuggest;
			INT32 nAppend = 2;
			while (pSentry->GetNameAttr(*pstrSuggest) != 0)
				pstrSuggest->MakeMsg(_R(IDS_NAMEDLG_UNIQUE_MASK), &strBase, nAppend++);
		}
	}		

	return pstrSuggest;
}



/********************************************************************************************
>	virtual UINT32 NameObjectsDlg::IsValid(const StringBase& strName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Inputs:		strName		---		name to test for suitability
	Returns:	0 if valid (ie. not a name already in use), ID of an error string
				explaining why if it is not (new names must be _new_, ie. different
				to any existing ones).
	SeeAlso:	BaseNameObjectsDlg::Message
********************************************************************************************/

UINT32 NameObjectsDlg::IsValid(const StringBase& strName)
{
	// All new names must be unique, it's a policy.
	return Document::GetSelected()->GetSetSentinel()->GetNameAttr(strName) != 0
						? _R(IDE_NAMEDLG_NAME_EXISTS) : 0;
}



/********************************************************************************************
>	virtual BOOL NameObjectsDlg::DoCommit(const StringBase& strName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Inputs:		strName			---		the name to OpApplyNames
	Returns:	TRUE / FALSE	---		don't / do close the dialog on returning.
	Purpose:	Applies the name to the selected objects, if any.
	SeeAlso:	BaseNameObjectsDlg::Message; OpApplyNames
********************************************************************************************/

BOOL NameObjectsDlg::DoCommit(const StringBase& strName)
{
	// Invoke either OpApplyNameToSel or OpApplyNameToNone, with a single name parameter.
	const TCHAR* pszTok = (GetApplication()->FindSelection()->IsEmpty())
								? OPTOKEN_APPLY_NAME_TO_NONE
								: OPTOKEN_APPLY_NAME_TO_SEL;

	// Apply and (do or do not) close the dialog.
	OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor((TCHAR*) pszTok);
	ERROR3IF(pDesc == 0, "NameObjectsDlg::DoCommit: can't find descriptor");
	pDesc->Invoke(&OpParam((INT32) &strName, 0));
	return m_fModeless;
}



/********************************************************************************************
>	static BOOL NameObjectsDlg::Init()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Returns:	TRUE if successful.
	Purpose:	Initialises preferences for this class.
	SeeAlso:	BaseNameObjectsDlg; InitNamingSystem
********************************************************************************************/

BOOL NameObjectsDlg::Init()
{
	ERROR2IF(!GetApplication()->DeclareSection(TEXT("Name Gallery"), 5) ||
			 !GetApplication()->DeclarePref(TEXT("Name Gallery"),
											TEXT("ModelessNameObjects"),
							 			    &m_fModeless, FALSE, TRUE),
			 FALSE,
			 _R(IDE_BAD_INI_FILE));

	return TRUE;
}



/********************************************************************************************
>	RenameObjectsDlg::RenameObjectsDlg()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Purpose:	Constructs a RenameObjectsDlg object, a modal version of the base class
				dialog, which renames highlighted name items in the Attribute gallery.
	SeeAlso:	BaseNameObjectsDlg
********************************************************************************************/

RenameObjectsDlg::RenameObjectsDlg()
  :	BaseNameObjectsDlg(_R(IDD_NAMEDLG_RENAME_TEMPLATE), MODAL)
{
	// Empty.
}



/********************************************************************************************
>	virtual StringBase* RenameObjectsDlg::GetDescription(StringBase* pstrDesc)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Returns:	The input.
	Outputs:	pstrDesc	---		a description of the objects to be renamed, if any.
	SeeAlso:	BaseNameObjectsDlg
********************************************************************************************/

StringBase* RenameObjectsDlg::GetDescription(StringBase* pstrDesc)
{
	// Get the highlighted item from the Attribute gallery.
	SGNameItem* pItem =
			(SGNameItem*) NameGallery::Instance()->GetUsedNames()->FindNextSelectedItem(0);
	ERROR3IF(pItem == 0, "NameGallery::OnRename: no highlighted item to rename");

	// Build up a string description of it.
	//INT32 nTotal = pItem->GetObjectCount();

	// dont rely on the object count as it counts the text story, text line and each char if just the story has the tag
	// better to count the number of tags found! (sjk 12/9/00)
	INT32 nTotal = 0;
	INT32 nSel = 0;

	String_256 ItemText = "";
	if (pItem)
		pItem->GetNameText(&ItemText);

	Node * pSpread = (Node *) Document::GetSelectedSpread();
	if (pSpread)
	{
		Node * pNode = SliceHelper::FindNextNameNode(pSpread, pSpread);
		while (pNode)
		{
			if (ItemText.CompareTo(((TemplateAttribute *)pNode)->GetParam()) == 0)
			{
				nTotal++; // found a name

				// is it selected?
				// or even is its parent selected?
				BOOL ParentSelected = FALSE;
				Node * pParent = pNode->FindParent();
				while (pParent && !ParentSelected)
				{
					if (pParent->IsSelected())
					{
						nSel++;
						ParentSelected = TRUE;
					}

					pParent = pParent->FindParent();
					if (pParent && pParent->IsLayer())
						pParent = NULL;
				}
			}

			pNode = SliceHelper::FindNextNameNode(pNode, pSpread);
		}
	}

	if (nTotal < 1 && !pSpread)
		pstrDesc->Load(_R(IDS_NAMEDLG_DESCRIBE_NOSET));
	else
	{
		// The set has some members, some or all of which might be selected.
		UINT32 idMask;
		String_256 strSel;
		if (nTotal == 1)
		{
			// Singular.  Work out if it's "which is" or "which isn't".
			idMask = _R(IDS_NAMEDLG_DESCRIBE_SET);
			if (nSel == 0)
				strSel = _R(IDS_NAMEDLG_WHICH_ISNT);
			else 
				strSel = _R(IDS_NAMEDLG_WHICH_IS);
		}
		else
		{		
			// Plural.  Work out if it's "none", "all", or "<a number>".
			idMask = _R(IDS_NAMEDLG_DESCRIBE_SET_PL);
			if (nSel == 0)
				strSel = _R(IDS_NAMEDLG_NONE);
			else if (nSel == nTotal)
			{
				strSel = _R(IDBBL_NAMEGAL_ALL);
				strSel.toLower();
			}
			else
				Convert::LongToString(nSel, &strSel);
		}

		String_256 strName;
		pItem->GetNameText(&strName);
		pstrDesc->MakeMsg(idMask, &strName, &strSel, nTotal);
	}

	return pstrDesc;
}



/********************************************************************************************
>	virtual StringBase* RenameObjectsDlg::GetSuggestion(StringBase* pstrSuggest)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Returns:	The input.
	Outputs:	pstrSuggest	---		a suggested name for the selected objects, if any.
	SeeAlso:	BaseNameObjectsDlg
********************************************************************************************/

StringBase* RenameObjectsDlg::GetSuggestion(StringBase* pstrSuggest)
{
	// Suggest the old name of the highlighted Attribute gallery item.
	SGNameItem* pItem =
			(SGNameItem*) NameGallery::Instance()->GetUsedNames()->FindNextSelectedItem(0);
	ERROR3IF(pItem == 0, "NameGallery::OnRename: no highlighted item to rename");
	
	String_256 str;
	pItem->GetNameText(&str);
	*pstrSuggest = str;
	return pstrSuggest;
}



/********************************************************************************************
>	virtual BOOL RenameObjectsDlg::DoCommit(const StringBase& strName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Inputs:		strName		---		the name to OpRenameNames
	Returns:	TRUE - close the dialog on returning.
	Purpose:	Called when the user enters a unique name and clicks OK in the Rename
				objects dialog.
	SeeAlso:	BaseNameObjectsDlg::Message; OpRenameNames
********************************************************************************************/

BOOL RenameObjectsDlg::DoCommit(const StringBase& strName)
{
	// Rename all the highlighted items to the new name and close.
	OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_RENAME_ALL);
	ERROR3IF(pDesc == 0, "RenameObjectsDlg::DoCommit: can't find OPTOKEN_RENAME_ALL");
	pDesc->Invoke(&OpParam((INT32) &strName, 0));
	return FALSE;
}



/**********************************************************************************************
>	OpNameGalleryPropIndexDesc::OpNameGalleryPropIndexDesc()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Purpose:	Constructs an OpNameGalleryPropIndexDesc object.
**********************************************************************************************/

/*
	OpDescriptor constructor's arguments:-

				 UINT32 toolID,                      	// Module Identifier
				 UINT32 txID,                         // String Resource ID
				 CCRuntimeClass* Op,				// pointer to the Op's runtime class
				 TCHAR* tok,						// pointer to the token string
				 pfnGetState gs,					// pointer to the GetState function
				 UINT32 helpId = 0,					// help identifier 
				 UINT32 bubbleID = 0,					// string resource for bubble help
				 UINT32 resourceID = 0,				// bitmap ("icon") or resource ID
				 UINT32 controlID =0,					// control ID
				 BOOL ReceiveMessages = FALSE,
				 BOOL Smart = FALSE,
				 BOOL Clean = TRUE,
				 UINT32 OneOpenInstID = 0,			// When not 0 only one live instance
				 									// of the operation is permitted, and
													// OneOpenInstID is the string resource
													// ID describing why.This will be useful
				 									// for 1 open instance dialogs.
				 UINT32 AutoStateFlags = 0			// Flags which indicate conditions when
				 									// the operation should automatically be
													// greyed/ticked etc..(This cuts down the 
													// number of tests that need to be made
													// in the GetState function).
*/

OpNameGalleryPropIndexDesc::OpNameGalleryPropIndexDesc()
  : OpDescriptor(0,
				 _R(IDS_NAME_GALLERY_PROP_INDEX),
				 CC_RUNTIME_CLASS(OpNameGalleryPropIndexDesc),
				 OPTOKEN_NAME_GALLERY_PROP_INDEX,
				 OpDisplayNameGallery::GetState,
				 0,
				 _R(IDBBL_NAME_GALLERY_PROP_INDEX),
				 0,
				 _R(IDC_NAMEGAL_PROP_IDX),
				 TRUE)
{
	// Empty.
}




/***********************************************************************************************
>	virtual void OpNameGalleryPropIndexDesc::OnControlCreate(OpDescControlCreateMsg* pCreateMsg)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Inputs:		pCreateMsg		---		message describing this creation event
	Purpose:	Called when the combo-boxes associated with this descriptor are created.
***********************************************************************************************/

void OpNameGalleryPropIndexDesc::OnControlCreate(OpDescControlCreateMsg* pCreateMsg)
{
	// Fill up the gadget's list with the names of properties.  These must be in
	// ascending order of property indices.
	pCreateMsg->pDlgOp->SetStringGadgetValue(
							pCreateMsg->SetGadgetID, &String(_R(IDS_NAMEGAL_EXPORT_PROP)));
	pCreateMsg->pDlgOp->SetStringGadgetValue(
							pCreateMsg->SetGadgetID, &String(_R(IDS_NAMEGAL_SLICE_PROP)));
	pCreateMsg->pDlgOp->SetStringGadgetValue(
							pCreateMsg->SetGadgetID, &String(_R(IDS_NAMEGAL_STRETCH_PROP)));

	// Resize the list to fit and update associated gadgets.
	pCreateMsg->pDlgOp->SetComboListLength(pCreateMsg->SetGadgetID);
	UpdateGadgets();
}



/**********************************************************************************************
>	void OpNameGalleryPropIndexDesc::OnSelectionChange(OpDescControlMsg* pSelChangedMsg,
													   List* pGadgetList)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Inputs:		pSelChangedMsg		---		message describing the change event
				pGadgetList			---		list of gadgets associated with this
	Purpose:	Called when the combo-boxes associated with this descriptor are changed.
**********************************************************************************************/

void OpNameGalleryPropIndexDesc::OnSelectionChange(OpDescControlMsg* pSelChangedMsg, List*)
{
	// Extract the index of the item selected from the list, if any.
	INT32 i = pSelChangedMsg->pDlgOp->GetSelectedValueIndex(pSelChangedMsg->SetGadgetID);
	if (i == -1) i = NameGallery::Instance()->GetPropertyIndex();

	// Update the gallery's property index.
	NameGallery::Instance()->SetPropertyIndex((INT32) i);
	UpdateGadgets();
}



/**********************************************************************************************
>	void OpNameGalleryPropIndexDesc::UpdateGadgets()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/99
	Purpose:	Sets the text in all the combos associated with this descriptor to the
				currently selected Name gallery property index.
	SeeAlso:	OpDescriptor::BuildGadgetList
**********************************************************************************************/

void OpNameGalleryPropIndexDesc::UpdateGadgets()
{
	// Create a list of all the gadget IDs
	List theGadgets;
	if (BuildGadgetList(&theGadgets))
	{
		// Iterate over each control in the list.
		INT32 nVal = NameGallery::Instance()->GetPropertyIndex();
		for (GadgetListItem* pgli = (GadgetListItem*) theGadgets.GetHead(); 
			 pgli != 0; 
			 pgli = (GadgetListItem*) theGadgets.GetNext(pgli))
		{
			// Set each control to display the text.
			pgli->pDialogBarOp->SetSelectedValueIndex(pgli->gidGadgetID, nVal);
		}

		// Tidy up.
		theGadgets.DeleteAll();
	}
} 



/**********************************************************************************************
>	static DialogBarOp* OpDisplayNameGallery::FindGallery()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Returns:	0 or a pointer to the Name gallery instance
	Purpose:	Finds the Name gallery class instance
	Notes:		The bars system always keeps one Name gallery alive for us.
				If one is not found, this usually indicates that it can't be found
				in bars.ini: Check that the 'Name' string *exactly* matches the
				title string given in bars.ini.
				Also check that bars.ini indicates the bar is of the NameGallery class
**********************************************************************************************/

DialogBarOp* OpDisplayNameGallery::FindGallery()
{
	String id(_R(IDS_SGNAME_GALLERY_NAME));						// "Name gallery"
	DialogBarOp* pOp = DialogBarOp::FindDialogBarOp(id);
	ERROR3IF(pOp == 0 || pOp->GetRuntimeClass() != CC_RUNTIME_CLASS(NameGallery), 
		"OpDisplayNameGallery::FindGallery: Can't find the gallery in bars.ini!\n");
	return pOp;
}



/**********************************************************************************************
>	static OpState OpDisplayNameGallery::GetState(String_256*, OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Purpose:	For finding the OpDisplayNameGallery's state. 
**********************************************************************************************/

OpState	OpDisplayNameGallery::GetState(String_256* UIDescription, OpDescriptor*)
{
	// If the gallery is currently open, then the menu item should be ticked
	OpState OpSt;  
	DialogBarOp* pDialogBarOp = FindGallery();
	if (pDialogBarOp != 0) OpSt.Ticked = pDialogBarOp->IsVisible();

	// If there are no open documents, you can't toggle the gallery
	OpSt.Greyed = (Document::GetSelected() == 0);
 	return OpSt;
}



/**********************************************************************************************
>	void OpDisplayNameGallery::Do(OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Purpose:	Displays the Names gallery
				Updates the button state for this Op (the button sticks down while the
				gallery is open)
**********************************************************************************************/

void OpDisplayNameGallery::Do(OpDescriptor*)
{
	DialogBarOp* pOp = FindGallery();
	if (pOp != 0)
	{
		// Toggle the visible state of the gallery window
		pOp->SetVisibility(!pOp->IsVisible());
		SGInit::UpdateGalleryButton(OPTOKEN_DISPLAY_NAME_GALLERY, pOp->IsVisible());
	}

	End();
}



/**********************************************************************************************
>	static BOOL OpDisplayNameGallery::Init()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Returns:	0 or a pointer to the Name gallery instance
	Purpose:	Initialises the Attribute gallery and it's associated gadgets.
**********************************************************************************************/

BOOL OpDisplayNameGallery::Init()
{
	ERRORIF(new OpNameGalleryPropIndexDesc == 0, _R(IDE_NOMORE_MEMORY), FALSE);
	return OpDisplayNameGallery::RegisterOpDescriptor(
					0,
					_R(IDS_DISPLAY_NAME_GALLERY),
					CC_RUNTIME_CLASS(OpDisplayNameGallery),
					OPTOKEN_DISPLAY_NAME_GALLERY,
					OpDisplayNameGallery::GetState,
					0,
					_R(IDBBL_DISPLAY_NAME_GALLERY));
}
