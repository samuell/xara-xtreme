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

// brush name dialog implementation

#include "camtypes.h"
#include "brshname.h"
//#include "brushres.h"  // dialog resources
#include "brushop.h"  // change brush op
//#include "resource.h"
#include "brshcomp.h"
//#include "rik.h"   // for the strings
#include "reshlpid.h"

CC_IMPLEMENT_DYNAMIC(CBaseBrushNameDlg, DialogOp)
CC_IMPLEMENT_DYNCREATE(CNameBrushDlg, CBaseBrushNameDlg)
CC_IMPLEMENT_DYNCREATE(CInitBrushNameDlg, CNameBrushDlg)

// Preference for modal (default) or modeless Name Objects dialog.
BOOL CBaseBrushNameDlg::m_bModeless = FALSE;


/********************************************************************************************
>	CBaseBrushNameDlg::CBaseBrushNameDlg(CDlgResID idRes, CDlgMode nMode)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Inputs:		idRes		---		resource ID of the dialog template to use
				nMode		---		mode to run in, MODAL or MODELESS
	Purpose:	Create a CBaseBrushNameDlg, an abstract base class for the Name name
				and	Rename brush dialogs.
	SeeAlso:	CBrushNameDlg; CRenameBrushDlg
********************************************************************************************/

CBaseBrushNameDlg::CBaseBrushNameDlg(CDlgResID idRes, CDlgMode nMode)
  :	DialogOp(idRes, nMode)
{   
	m_BrushHandle = BrushHandle_NoBrush;
}        




/********************************************************************************************
>	virtual void CBaseBrushNameDlg::Do(OpDescriptor*)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Purpose:	Creates and runs a CBaseBrushNameDlg.  Shouldn't really use this as we will
				have no definition to name
	SeeAlso:	CBaseBrushNameDlg; CBaseBrushNameDlg
********************************************************************************************/

void CBaseBrushNameDlg::Do(OpDescriptor*)
{
	// Try to run this operation.
	if (!Create())
	{
		InformError(0, _R(IDS_OK));
		End();
	}
	if (m_bModeless == TRUE)
		Open();
	
}

/********************************************************************************************
>	virtual void CBaseBrushNameDlg::Do(OpDescriptor*)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Purpose:	Creates and runs a CBaseBrushNameDlg to get a name for the brush definition
				included in our param
	SeeAlso:	CBaseBrushNameDlg; CBaseBrushNameDlg
********************************************************************************************/

void CBaseBrushNameDlg::DoWithParam(OpDescriptor* pDesc, OpParam* pParam)
{
	if (pParam == NULL)
	{
		ERROR3("Param is null in CBaseBrushNameDlg::DoWithParam");
		return;
	}
	// first extract the brush handle
	m_BrushHandle =  static_cast<BrushHandle> (pParam->Param1);
	
	// now do it
	Do(pDesc);
}

/********************************************************************************************
>	virtual MsgResult CBaseBrushNameDlg::Message(Msg* pMessage)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Purpose:	Handles kernel messages for the CBaseBrushNameDlg dialog operation.
********************************************************************************************/

MsgResult CBaseBrushNameDlg::Message(Msg* pMessage)
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
			EnableGadget(IDOK, !GetStringGadgetValue(_R(IDC_EDITBRUSHNAME), 0).IsEmpty()
							&& Document::GetSelected() != 0);
			break;

		case DIM_LFT_BN_CLICKED:	// enter messages when the edit box has the focus get cast to this
			if (pMsg->GadgetID == _R(ID_CC_HELP_BUTTON))
				break;
		case DIM_COMMIT:
			{
				// Check name for validity.
				String_256 strEnter = GetStringGadgetValue(_R(IDC_EDITBRUSHNAME), 0);
				UINT32 nErrID = IsValid(strEnter);
				
				if (nErrID != 0)
				{
					// Invalid, reinitialise.
					InformError(nErrID);
					if (nErrID == _R(IDS_BRUSHNAME_INVALID))
						InitGadgetText(&(String_32(strEnter)), FALSE);
					else
						InitGadgetText(NULL);
					break;
				}
				else
				{
					// Entry is valid, try committing it.
					String_32 FinalString(strEnter);
					if (DoCommit(strEnter))
					{
						// Don't close, reinitialise (dialog is modeless).
						InitGadgetText();
						break;
					}
					else
					{
						Close();
						End();
					}
				}
						
			}
			break;
		case DIM_CANCEL:
			HandleCancel();
			Close();
			End();
			break;
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
>	virtual UINT32 CBaseBrushNameDlg::IsValid(const StringBase& strName)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Inputs:		strName		---		name to test for suitability
	Returns:	0 if valid (by default any non-empty text is valid), or the ID of an
				error string explaining why if it is not.
	SeeAlso:	CBaseBrushNameDlg::Message
********************************************************************************************/

UINT32 CBaseBrushNameDlg::IsValid(const StringBase& strName)
{
	return 0;
}

/*---------------------------------------------------------------------------------------------
----------------------------------CBrushNameDlg class implementation---------------------------
----------------------------------------------------------------------------------------------*/



/********************************************************************************************
>	CNameBrushDlg::CNameBrushDlg()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Purpose:	Constructs a CNameBrushDlg object, a modeless version of the base class
				dialog, which gets a name for a newly created brush
	SeeAlso:	CBaseNameBrushDlg
********************************************************************************************/

CNameBrushDlg::CNameBrushDlg()
  :	CBaseBrushNameDlg(_R(IDD_BRUSHNAMEDLG), m_bModeless ? MODELESS : MODAL)
{
	INT32 i = 1;
	// Empty.
}



/********************************************************************************************
>	virtual StringBase* CNameBrushDlg::GetDescription(StringBase* pstrDesc)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Returns:	The input.
	Outputs:	what came in!
	Purpose:	Nothing, at the moment, when I get around to writing some descriptive strings
				I will add them in here
	
********************************************************************************************/

StringBase* CNameBrushDlg::GetDescription(StringBase* pstrDesc)
{
	return pstrDesc;
}



/********************************************************************************************
>	virtual void CNameBrushDlg::InitGadgetText(String_32* pString)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Inputs:		pString - if you want to suggest a name based on a particular string
				then pass a string, defaults to FALSE though so you don't have to
	Purpose:	Initialises the text of the gadgets in the dialog.
	SeeAlso:	CBaseBrushNameDlg::Message
********************************************************************************************/

void CNameBrushDlg::InitGadgetText(String_32* pString, BOOL resizeDialog /*= TRUE*/)
{
	if (pString != NULL)
		m_strSuggest = *pString;
	SetStringGadgetValue(_R(IDC_EDITBRUSHNAME), GetSuggestion(&m_strSuggest));
	HighlightText(_R(IDC_EDITBRUSHNAME));
	SetKeyboardFocus(_R(IDC_EDITBRUSHNAME));

	// change the dialog title to "Enter brush name:"
	String_256 Name = TEXT("Enter brush name:");
	SetTitlebarName(&Name);

	if (resizeDialog)
	{
		// what we want to do is shrink the dialog by the size of the label and then hide it
		RECT LabelRect;
		GetGadgetPosition(_R(IDC_STATICBRUSHGROUP), &LabelRect);
		
		// hide the label
		HideGadget(_R(IDC_STATICBRUSHGROUP), TRUE);

		// get the size of this dialog
		RECT DialogRect;
		GetWindowPosition(&DialogRect);
		
		// subtract the size of the label
		INT32 Subtract = LabelRect.bottom - LabelRect.top;
		DialogRect.bottom -= Subtract;

		// We want to move up the buttons
		RECT OkRect;
		if (GetGadgetPosition(IDOK, &OkRect))
		{
			OkRect.top -= Subtract;
			OkRect.bottom -= Subtract;
			INT32 Width = OkRect.right - OkRect.left;
			Width = (Width * 2) / 3;
			OkRect.left += Width ;
			OkRect.right += Width;
			SetGadgetPosition(IDOK, OkRect);
		}

		RECT CancelRect;
		if (GetGadgetPosition(IDCANCEL, &CancelRect))
		{
			CancelRect.top -= Subtract;
			CancelRect.bottom -= Subtract;
			INT32 Width = CancelRect.right - CancelRect.left;
			Width = (Width * 2) / 3;
			CancelRect.left += Width;
			CancelRect.right += Width;
			SetGadgetPosition(IDCANCEL, CancelRect);
		}

		// Reset the window
		SetWindowPosition(DialogRect);
		
		// Hide the help button
		HideGadget(_R(ID_CC_HELP_BUTTON), TRUE);
	}
}


/********************************************************************************************
>	virtual StringBase* CNameBrushDlg::GetSuggestion(StringBase* pstrSuggest)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Outputs:	pstrSuggest	 ---	a suggested name for the new brush, if any.
	Returns:	pstrSuggest, ie. the input.
	Purpose:	Works out a suggestion for the name of the new brush
	
********************************************************************************************/

StringBase* CNameBrushDlg::GetSuggestion(StringBase* pstrSuggest)
{
	ERROR2IF(pstrSuggest == NULL, FALSE, "Null input pointer to CNameBrushDlg::GetSuggestion");

	// get the brush component from the document
	Document* pDoc = Document::GetCurrent();
	ERROR2IF(pDoc == NULL, FALSE, "Wheres the document?");
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	ERROR2IF(pBrushComp == NULL, FALSE, "No brush component");
	
	if (!pstrSuggest->IsEmpty())
		*pstrSuggest = pBrushComp->GetUniqueName((String_32*)pstrSuggest);
	else
		*pstrSuggest = pBrushComp->GetUniqueName(NULL);

	return pstrSuggest;
}



/********************************************************************************************
>	virtual UINT32 CNameBrushDlg::IsValid(const StringBase& strName)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/99
	Inputs:		strName		---		name to test for suitability
	Returns:	0 if valid (ie. not a name already in use), ID of an error string
				explaining why if it is not (new names must be _new_, ie. different
				to any existing ones).
	SeeAlso:	CBaseNameBrushDlg::Message
********************************************************************************************/

UINT32 CNameBrushDlg::IsValid(const StringBase& strName)
{
	UINT32 NumChars = strName.Length();
	if (NumChars > 20)
		return _R(IDS_BRUSHNAME_TOOLONG);
	// get the brush component from the document
	Document* pDoc = Document::GetCurrent();
	ERROR2IF(pDoc == NULL, FALSE, "Wheres the document?");
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	ERROR2IF(pBrushComp == NULL, FALSE, "No brush component");
	
	if (pBrushComp->NameIsUnique(strName))
		return 0;

	return _R(IDS_BRUSHNAME_INVALID);
}



/********************************************************************************************
>	virtual BOOL CNameBrushDlg::DoCommit(const StringBase& strName)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Inputs:		strName			---		the name to apply
	Returns:	TRUE / FALSE	---		don't / do close the dialog on returning.
	Purpose:	Invokes the op to apply the name to the brush
	SeeAlso:	BaseCNameBrushDlg::Message; OpApplyNames
********************************************************************************************/

BOOL CNameBrushDlg::DoCommit(const StringBase& strName)
{
	// Apply and (do or do not) close the dialog.
	OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGEBRUSHDEF);
	
	// if we haven't got a brush then error
	if (m_BrushHandle == BrushHandle_NoBrush)
	{
		ERROR3("No brush handle in CNameBrushDlg::DoCommit");
		return FALSE;
	}

	// our param needs to know the brush handle and new name
	ChangeBrushDefOpParam Param;
	Param.m_NewBrushName = (String_32)strName;
	Param.m_Handle = m_BrushHandle;
	Param.ChangeType = CHANGEBRUSH_NAME;

	ERROR3IF(pDesc == 0, "CNameBrushDlg::DoCommit: can't find descriptor");
	pDesc->Invoke(&Param);
	return m_bModeless;
}


/********************************************************************************************
>	static BOOL CNameBrushDlg::Init()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Purpose:	Register operation.
	SeeAlso:	
********************************************************************************************/

BOOL CNameBrushDlg::Init()
{  
	return RegisterOpDescriptor(0,
								_R(IDS_OK),
								CC_RUNTIME_CLASS(CNameBrushDlg),
								OPTOKEN_NAME_BRUSH_DLG,
								CNameBrushDlg::GetState,
								0,								// help ID
								0);								// bubble ID
}   



/********************************************************************************************
>	static OpState CNameBrushDlg::GetState(String_256*, OpDescriptor*)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Purpose:	The usual default GetState function for the CNameBrushDlg dialog operation.
********************************************************************************************/

OpState	CNameBrushDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	INT32 i = 1;
	return OpSt;
}



/*--------------------------------------------------------------------------------------------
----------------------------------CInitBrushNameDlg implementation----------------------------
---------------------------------------------------------------------------------------------*/


/********************************************************************************************
>	static BOOL CNameBrushDlg::Init()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Purpose:	Register operation.
	SeeAlso:	
********************************************************************************************/

BOOL CInitBrushNameDlg::Init()
{  
	return RegisterOpDescriptor(0,
								_R(IDS_OK),
								CC_RUNTIME_CLASS(CInitBrushNameDlg),
								OPTOKEN_INITNAME_BRUSH_DLG,
								CInitBrushNameDlg::GetState,
								0,								// help ID
								0);								// bubble ID
}   



/********************************************************************************************
>	static OpState CNameBrushDlg::GetState(String_256*, OpDescriptor*)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Purpose:	The usual default GetState function for the CNameBrushDlg dialog operation.
********************************************************************************************/

OpState	CInitBrushNameDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	INT32 i = 1;
	return OpSt;
}



/********************************************************************************************
>	virtual void CInitBrushNameDlg::InitGadgetText(String_32* pString)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Inputs:		pString - if you want to suggest a name based on a particular string
				then pass a string, defaults to FALSE though so you don't have to
	Purpose:	Initialises the text of the gadgets in the dialog.
	SeeAlso:	CBaseBrushNameDlg::Message
********************************************************************************************/

void CInitBrushNameDlg::InitGadgetText(String_32* pString, BOOL resizeDialog /*= TRUE*/)
{
	if (pString != NULL)
		m_strSuggest = *pString;
	SetStringGadgetValue(_R(IDC_EDITBRUSHNAME), GetSuggestion(&m_strSuggest));

	// change the dialog title to "Enter brush name:"
	// Errr, NO!  This is very bad for translation!
//	String_256 Name = TEXT("Create new brush:");
//	SetTitlebarName(&Name);

	// Show the help button
	HideGadget(_R(ID_CC_HELP_BUTTON), FALSE);

	// show the group info text
	SetStringGadgetValue(_R(IDC_STATICBRUSHGROUP), _R(IDS_BRUSHGROUP));
	HideGadget(_R(IDC_STATICBRUSHGROUP), FALSE);

	
	
	HighlightText(_R(IDC_EDITBRUSHNAME));
	SetKeyboardFocus(_R(IDC_EDITBRUSHNAME));


}

/********************************************************************************************
>	virtual BOOL CNameBrushDlg::DoCommit(const StringBase& strName)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Inputs:		strName			---		the name to apply
	Returns:	TRUE / FALSE	---		don't / do close the dialog on returning.
	Purpose:	simply retrieves the brush definition and changes the name, this means
				that it will not be undoable
	SeeAlso:	BaseCNameBrushDlg::Message; OpApplyNames
********************************************************************************************/

BOOL CInitBrushNameDlg::DoCommit(const StringBase& strName)
{
	String_32 Name(strName);
	BrushComponent::SetNewName(strName);
	return m_bModeless;
}



/********************************************************************************************
>	virtual BOOL CNameBrushDlg::DoCommit(const StringBase& strName)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Inputs:		strName			---		the name to apply
	Returns:	TRUE / FALSE	---		don't / do close the dialog on returning.
	Purpose:	simply retrieves the brush definition and changes the name, this means
				that it will not be undoable
	SeeAlso:	BaseCNameBrushDlg::Message; OpApplyNames
********************************************************************************************/

void CInitBrushNameDlg::HandleCancel()
{
	BrushComponent::CancelNewBrush();
}

/********************************************************************************************
>	CNameBrushDlg::CNameBrushDlg()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Purpose:	Constructs a CNameBrushDlg object, a modeless version of the base class
				dialog, which gets a name for a newly created brush
	SeeAlso:	CBaseNameBrushDlg
********************************************************************************************/

CInitBrushNameDlg::CInitBrushNameDlg()
  :	CNameBrushDlg()
{
	// Empty.
}
