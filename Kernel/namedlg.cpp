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

/*
*/
    
#include "camtypes.h"                          
//#include "dialogop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "namedlg.h"

//#include "tmpltres.h"
#include "xshelpid.h"


DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_MEMDUMP(NameDialogParam, OpParam);
CC_IMPLEMENT_DYNCREATE(NameDialog, DialogOp)   
			  
// Give this file in memory dumps
#define new CAM_DEBUG_NEW


#define ENSURE_NOT_NULL(Pointer)		ERROR2IF(Pointer == NULL, 0, "NULL Args")
#ifndef VOID_ENSURE_NOT_NULL
#define VOID_ENSURE_NOT_NULL(p)		if (p == NULL) {	ERROR2RAW("NULL Args")	}
#endif
#define ENSURE_KIND(pNode, Kind)		ERROR3IF(!pNode->IS_KIND_OF(Kind), "pNode is not kind of " #Kind);



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//					N  A  M  E  D  I  A  L  O  G  P  A  R  A  M
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //







/********************************************************************************************

>	BOOL NameDialogParam::SetNameGiven(const StringBase& StringEntered)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from MarkN)
	Created:	13/07/97

	Purpose:	Data access
	Returns:	TRUE if the whole string were copied, FALSE if not

********************************************************************************************/
BOOL NameDialogParam::SetNameGiven(const StringBase& StringEntered)
{
	BOOL AllCopied = TRUE;		// return this

	UINT32 LengthToCopy = 0;
	if (StringEntered.Length() <= m_StringEntered.MaxLength())
	{
		LengthToCopy = StringEntered.Length();
	}
	else
	{
		AllCopied = FALSE;
		LengthToCopy = m_StringEntered.MaxLength();
	}

	StringEntered.Left(&m_StringEntered, LengthToCopy);

	return AllCopied;
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//								N  A  M  E  D  I  A  L  O  G
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //





const CDlgMode 	NameDialog::Mode 	= MODAL;   // The dialog is modal
const INT32		NameDialog::IDD		= _R(IDD_NAME);


/********************************************************************************************

>	static BOOL NameDialog::Init()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from MarkN)
	Created:	13/07/97

	Purpose:	Declares a NameDialog

********************************************************************************************/
BOOL NameDialog::Init()
{  
	return (RegisterOpDescriptor(
								0,
								_R(IDS_STYLENAMEDLG),
								CC_RUNTIME_CLASS(NameDialog),
								OPTOKEN_NAMEDLG,
								NameDialog::GetState,
								0, //_R(IDH_Command_Wizard_Properties),	// help ID
								0   // bubble ID
								)
			); 
}   


/********************************************************************************************

>	static BOOL NameDialog::Invoke(NameDialogParam& NameParam)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from MarkN)
	Created:	13/07/97

	Purpose:	Pops up a name dialog without you having to type in all that OpDescriptor 
				hoo haa

	Inputs:		NameParam:	A NameDialogParam containing any name you want to appear in the
							dialog when it starts up.
	Outputs:	NameParam:	The NameDialogParam containing any name typed into the dialog.

	Returns:	TRUE if the entry was okayed, FALSE if cancelled.

********************************************************************************************/
BOOL NameDialog::Invoke(NameDialogParam& NameParam)
{
	OpDescriptor* const pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NAMEDLG);
	if (pOpDesc != NULL)
	{
		pOpDesc->Invoke(&NameParam);
	}
	else
	{
		ERROR3("Couldn't find OPTOKEN_NAMEDLG op descriptor");
	}

	return !NameParam.DialogCancelled();
}


/********************************************************************************************

>	static OpState	NameDialog::GetState(String_256*, OpDescriptor*)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from MarkN)
	Created:	13/07/97

	Purpose:	See DialogOp

********************************************************************************************/
OpState	NameDialog::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;

	return(OpSt);
}

		 
 
/********************************************************************************************

>	NameDialog::NameDialog(): DialogOp(NameDialog::IDD, NameDialog::Mode) 

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from MarkN)
	Created:	13/07/97

	Purpose:	NameDialog constructor. Creates a non-undoable operation

********************************************************************************************/
NameDialog::NameDialog(): DialogOp(NameDialog::IDD, NameDialog::Mode) 
{   
	m_pNameParam = NULL;
}        


/********************************************************************************************

>	virtual void NameDialog::DoWithParam(OpDescriptor* ,OpParam* pOpParam)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from MarkN)
	Created:	13/07/97

	Purpose:	Opens a dialog with the name given in pOpParam

	Inputs:		pOpParam : a NameDialogParam, containing any suggested name
	Outputs:	pOpParam : the NameDialogParam contains

********************************************************************************************/
void NameDialog::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
	VOID_ENSURE_NOT_NULL(pOpParam);
	ENSURE_KIND(pOpParam, NameDialogParam);

	SetNameParam(*(NameDialogParam*)pOpParam);

	BOOL ok = Create();

	if (!ok)			// Could not create 
	{
  		End(); 		   	// End the operation 
	}
}


/********************************************************************************************

>	MsgResult NameDialog::Message(Msg* Message)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from MarkN)
	Created:	13/07/97

	Purpose:	See DialogOp

********************************************************************************************/
MsgResult NameDialog::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* pMsg = (DialogMsg*)Message;
		BOOL EndDialog = FALSE;
		switch (pMsg->DlgMsg)
		{
			case DIM_CREATE:
				OnCreate();
				break;
				
			case DIM_COMMIT:
				OnCommit();
				EndDialog = TRUE;
				break;

			case DIM_CANCEL:
				OnCancel();
				EndDialog = TRUE;
				break;
		}

		if (EndDialog)
		{
			Close();	// Close the dialog 
			End();		// Destroy dialog - BEWARE! delete's this
		}

	}

	// Pass everything on to the base class . . .
	return DialogOp::Message(Message);
}  



	   

/********************************************************************************************

>	void NameDialog::OnCreate()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from MarkN)
	Created:	13/07/97

	Purpose:	Responds to a DIM_CREATE message by initializing the dialog control.
				It sets the name control with the string passed in the NameDialogParam via 
				the DoWithParam() member

********************************************************************************************/
void NameDialog::OnCreate()
{
	String_256 Name = GetName();

	BOOL Ok = SetStringGadgetValue(_R(IDC_NAME_NAME),&Name);

	if (Ok)
	{
		SetKeyboardFocus(_R(IDC_NAME_NAME));
		HighlightText(_R(IDC_NAME_NAME));
	}
}


/********************************************************************************************

>	virtual void NameDialog::OnCommit()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from MarkN)
	Created:	13/07/97

	Purpose:	Takes the string from the dialog and puts it in the NameDialogParam given in
				DoWithParam()

********************************************************************************************/
void NameDialog::OnCommit()
{
	BOOL Valid = TRUE;

	String_256 NewName;

	// Get the string from the layer name dialog
	NewName = GetStringGadgetValue(_R(IDC_NAME_NAME), &Valid);

	if (Valid)
	{
		Valid = SetName(NewName);
	}
}



/********************************************************************************************

>	virtual void NameDialog::OnCancel()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from MarkN)
	Created:	13/07/97

	Purpose:	Responds to a DIM_CANCEL message, by setting ???????????????????
			in the NameDialogParam.

********************************************************************************************/
void NameDialog::OnCancel()
{
	SetCancelled();
}


/********************************************************************************************

>	void NameDialog::SetNameParam(NameDialogParam& NameParam)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from MarkN)
	Created:	13/07/97

	Purpose:	Data access

********************************************************************************************/
void NameDialog::SetNameParam(NameDialogParam& NameParam)
{
	m_pNameParam = &NameParam;
}


BOOL NameDialog::SetName(const StringBase& StringEntered)
{
	ENSURE_NOT_NULL(m_pNameParam);

	return m_pNameParam->SetNameGiven(StringEntered);
}


const StringBase& NameDialog::GetName() const
{
	ERROR2IF(m_pNameParam == NULL, NullString, "NULL Member");

	return m_pNameParam->GetNameEntered();
}


BOOL NameDialog::SetCancelled()
{
	ENSURE_NOT_NULL(m_pNameParam);

	m_pNameParam->SetDialogCancelled();

	return TRUE;
}

