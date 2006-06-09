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

	Line-attributes combo-box OpDescriptor abstract base class.

*/

#include "camtypes.h"
//#include "undoop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "dialogop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "lineattr.h"
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "convert.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "attrappl.h"
//#include "mario.h"
//#include "simon.h"
//#include "justin.h"
//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "selmsg.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "range.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "la_base.h"

#define new CAM_DEBUG_NEW


/********************************************************************************************
>	OpChangeLineAttribOpDesc::OpChangeLineAttribOpDesc(UINT32 nTxtID,
												   	   const TCHAR* lpcszToken,
												   	   UINT32 nBubbleID,
												   	   UINT32 nControlID)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Simply calls the UndoableOpDescriptor::UndoableOpDescriptor	base class
	Errors:		-
	SeeAlso:	UndoableOpDescriptor::UndoableOpDescriptor
********************************************************************************************/

OpChangeLineAttribOpDesc::OpChangeLineAttribOpDesc(UINT32 nTxtID,
												   const TCHAR* lpcszToken,
												   UINT32 nBubbleID,
												   UINT32 nControlID)
  :	UndoableOpDescriptor(0,
  						 nTxtID,
  						 CC_RUNTIME_CLASS(OpApplyAttribToNode),
  						 (TCHAR*) lpcszToken,
  						 GetState,
  						 0,
  						 nBubbleID,
  						 0,
  						 nControlID,
  						 TRUE,							// will receive messages
  						 FALSE,
  						 FALSE)
{
	// Empty.
}



/********************************************************************************************
>	void OpChangeLineAttribOpDesc::SetGadgetText(const String& strGadgetText)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		strGadgetText			the text to show in the OpDescriptor's gadgets
	Purpose:	Sets the text in the combos associated with this OpDescriptor.
********************************************************************************************/

void OpChangeLineAttribOpDesc::SetGadgetText(const String& strGadgetText)
{
	// Create a list for the dialogue manager to put gadget ID's on.
	List* pGadgetList = new List;
	if (pGadgetList == NULL)
	{
		InformError(_R(IDE_NOMORE_MEMORY));
		return;
	}

	// Obtain a list of all the combo controls.
	if (BuildGadgetList(pGadgetList))
	{
		// Iterate over each control in the list.
		for (GadgetListItem* pGadgetItem = (GadgetListItem*) pGadgetList->GetHead(); 
			 pGadgetItem != NULL; 
			 pGadgetItem = (GadgetListItem*) pGadgetList->GetNext(pGadgetItem))
		{
			// Set each control to display the text.
			pGadgetItem->pDialogOp->SetStringGadgetValue(pGadgetItem->gidGadgetID, 
														    ((String&) strGadgetText),
														    FALSE, -1);
		}

		// Tidy up.
		pGadgetList->DeleteAll();
	}

	// Deallocate gadget list.
	delete pGadgetList;
} 



/********************************************************************************************
	BOOL OpChangeLineAttribOpDesc::SetCurrentSelJoin()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/95
	Outputs:	-
	Returns:	FALSE if we run out of memory etc.
	Purpose:	Sets the currently selected line width value. 

				If no objects are selected, or no selected objects require a join type
				attribute then the current join type attribute for the tool is displayed.

				If all selected objects (which require a join type) share the same join 
				type attribute then this is displayed.
				
				if the selected objects have more than one join type value then the 
				string 'MANY' is displayed. 

	Errors:		ERROR is set if we run out of memory
	Scope:		private
	SeeAlso:	-
********************************************************************************************/

BOOL OpChangeLineAttribOpDesc::SetCurrentSelectedAttrib()
{
	// Nothing to do if no current document.
	if (Document::GetCurrent() == NULL)
	{
/*		TRACEUSER( "JustinF", _T("No current document in OpChangeLineAttribOpDesc")
							 "::SetCurrentSelectedAttrib\n");
*/		
		// There is no document so blank all gadgets.
		SetGadgetText(String(_T("")));
		return TRUE;
	}

	// Determine if there is a common attribute.
	NodeAttribute* pAttr;
	SelRange::CommonAttribResult eResult;
	eResult = GetApplication()->
				 FindSelection()->
					 FindCommonAttribute(GetAttribRuntimeClass(), &pAttr);

	// Work out what to put in the combo-box according to the returned result.
	UINT32 nTxtID;
	switch (eResult)
	{
		case SelRange::ATTR_COMMON:
		case SelRange::ATTR_NONE:
		{
			// If there isn't an attribute we just return (this logic is copied from the
			// line-width combo OpDescriptor code).
			if (pAttr == NULL)
			{
/*				TRACEUSER( "JustinF", _T("No attribute in OpChangeLineAttribOpDesc")
									 "::SetCurrentSelectedAttrib\n");
*/				return TRUE;
			}

			// Check for something screwy.
			ERROR3IF(pAttr->GetRuntimeClass() != GetAttribRuntimeClass(),
					 "Wrong kind of attribute in OpChangeLineAttribOpDesc"
					 "::SetCurrentSelectedAttrib");

			// There is, so call the derived class to provide some text.
			nTxtID = ConvertAttribToStringID(pAttr);
			if (nTxtID == 0)
			{
				ERROR3("Unexpected attribute type in OpChangeLineAttribOpDesc"
					   "::SetCurrentSelectedAttrib!");
				return FALSE;
			}
			break; 
		}

		case SelRange::ATTR_MANY:
			nTxtID = _R(IDS_MANY);
			break;

		default:
			ERROR3("Unexpected case in OpChangeLineAttribOpDesc::SetCurrentSelectedAttrib!");
			return FALSE;
	}

	// Set the gadget's text.
	SetGadgetText(String(nTxtID));
	return TRUE;
}



/********************************************************************************************
	virtual void OpChangeLineAttribOpDesc::OnControlCreate(OpDescControlCreateMsg* pCreateMsg);

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function is called whenever a combo control associated with the 
	`			OpDescriptor gets created. 
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void OpChangeLineAttribOpDesc::OnControlCreate(OpDescControlCreateMsg* pCreateMsg)
{
	// Initialise the combo-box with the join types.
	DialogOp* pDlg = pCreateMsg->pDlgOp;									    
	CGadgetID SetGadgetID = pCreateMsg->SetGadgetID;
	
	// Set each item in the combo-box.
	for (INT32 i = 0; ; i++)
	{
		UINT32 nStringID = GetItemStringID(i);
		if (nStringID == 0) break;
		pDlg->SetStringGadgetValue(SetGadgetID, String(nStringID));
	}
	
	// Update the current selection within the combo etc.
	pDlg->SetComboListLength(SetGadgetID);
	SetCurrentSelectedAttrib(); 
}



/********************************************************************************************
>	virtual void OpChangeLineAttribOpDesc::OnSelectionChange(OpDescControlMsg* pSelChangedMsg, 
														     List* pGadgetList)
																		 	
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function gets called whenever a selection is made from a combo
				box. It invokes the operation to set the line width.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void OpChangeLineAttribOpDesc::OnSelectionChange(OpDescControlMsg* pSelChangedMsg, List*)
{
	// Get the relevant fields.
	DialogOp* pDlg = pSelChangedMsg->pDlgOp;									    
	CGadgetID idSetGadgetID = pSelChangedMsg->SetGadgetID;

	// Try to get the text entered (or selected) within the combo.
	BOOL fValid;
	String_256 strEntered = pDlg->GetStringGadgetValue(idSetGadgetID, &fValid, -1);
	if (!fValid)
	{
		TRACEUSER( "JustinF", _T("Invalid text in OpChangeLineAttribOpDesc::OnSelectionChange\n"));
		Beep();
		SetCurrentSelectedAttrib();
		return;
	}

	INT32 nItemIndex;
	// Compare the string to each item that was placed in the combo list.
	for (nItemIndex = 0; ; nItemIndex++)
	{
		// Ask the derived class to provide the string at the given index position.
		UINT32 nTxtID = GetItemStringID(nItemIndex);
		
		// If there are no more strings then simply reset the combo to the current value.
		// The user has entered junk!
		if (nTxtID == 0)
		{
			// This will "recalc" the current value and set it.
			Beep();
			SetCurrentSelectedAttrib();
			return;
		}

		// Compare the two strings.  If they match then break out now.
		if (strEntered == String_256(nTxtID)) break;
	}

	// Obtain the index selected in the combo and pass to the derived class, which will
	// create the appropriate attribute.
	NodeAttribute* pAttr = GetNewAttrib(nItemIndex);
	if (pAttr == NULL)
	{
		TRACEUSER( "JustinF", _T("Out of memory in OpChangeLineAttribOpDesc")
							 "::OnSelectionChange\n");
		return;
	}

	// Pass it to the attribute manager.
	AttributeManager::AttributeSelected(pAttr); 

	// Update all list-boxes.  Note we must do this even if we have just set a new
	// attribute, as a SelChanging message will not be always be broadcast.
	SetCurrentSelectedAttrib();
}



/********************************************************************************************
>	virtual BOOL OpChangeLineAttribOpDesc::OnSelChangingMsg(SelChangingMsg::SelectionState state)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/94
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if we run out of memory
	Purpose:	This gets called when the selection state has changed.
	Errors:		-
	SeeAlso:	SelChangingMsg
********************************************************************************************/
/*
// Don't freak Justin. This This fn is replaced by OnCommonAttrsChangedMsg which handles
// all cases when the common attributes should be re-displayed. See the Message.doc file
// for more details if your curious.
BOOL OpChangeLineAttribOpDesc::OnSelChangingMsg(SelChangingMsg::SelectionState state)
{
	return (state == SelChangingMsg::SELECTIONCHANGED) ? SetCurrentSelectedAttrib()
													   : TRUE;
}
*/
/********************************************************************************************

>	virtual BOOL OpChangeLineAttribOpDesc::OnCommonAttrsChangedMsg(SelChangingMsg::SelectionState State)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/94
	Returns:	FALSE if we run out of memory
	Purpose:	This gets called when the common attributes have changed

********************************************************************************************/


BOOL OpChangeLineAttribOpDesc::OnCommonAttrsChangedMsg()
{
	return (SetCurrentSelectedAttrib());
}




/********************************************************************************************
>	virtual BOOL OpChangeLineAttribOpDesc::OnDocChangingMsg(Document* pChangingDoc,
														    DocState State)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/95
	Inputs:		-
	Outputs:	-
	Returns:	
	Purpose:	To update the selection width combo whenever the document changes
	Errors:		-
	SeeAlso:	DocChangingMsg
********************************************************************************************/

BOOL OpChangeLineAttribOpDesc::OnDocChangingMsg(Document*, DocChangingMsg::DocState)
{
	return SetCurrentSelectedAttrib();
}



/********************************************************************************************
>	static OpState OpChangeLineAttribOpDesc::GetState(String_256*, OpDescriptor*); 

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Generic GetState function for the OpChangeLineAttribOpDesc and derivatives.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

OpState OpChangeLineAttribOpDesc::GetState(String_256*, OpDescriptor*)
{
	OpState os;
	if (Document::GetSelected() == NULL) os.Greyed = TRUE;
	return os;
}
