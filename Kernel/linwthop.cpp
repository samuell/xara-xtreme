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
//*/

// Implementation of the OpChangeLineWidthOpDesc

#include "camtypes.h"
#include "linwthop.h"
#include "dialogop.h"
#include "lineattr.h"
#include "document.h"
#include "convert.h"
#include "attrappl.h"
//#include "mario.h"
//#include "simon.h"
#include "bars.h"
#include "app.h"
#include "selmsg.h"
#include "range.h"
#include "attrmgr.h"
#include "tool.h"
#include <limits.h>
#include "selmsg.h"  
//#include "assoc.h"

// There is no maximum line width. We set it to a large positive INT32 value
#define MAX_LINE_WIDTH_MP 2834646 // 1 metre

#define new CAM_DEBUG_NEW    


/********************************************************************************************

>	OpChangeLineWidthOpDesc::OpChangeLineWidthOpDesc(
						 	UINT32 toolID,                      
						 	UINT32 txID,                        
						 	CCRuntimeClass* Op,			
						 	TCHAR* tok,					
						 	pfnGetState gs,				
						 	UINT32 helpId = 0,				
						 	UINT32 bubbleID = 0,				
						 	UINT32 resourceID = 0,		
						 	UINT32 controlID = 0,			
						 	BOOL ReceiveMessages = TRUE, 		
						 	BOOL Smart = FALSE, 
						 	BOOL Clean = FALSE 
						   ): 


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Simply calls the UndoableOpDescriptor::UndoableOpDescriptor	base class
	Errors:		-
	SeeAlso:	UndoableOpDescriptor::UndoableOpDescriptor

********************************************************************************************/

OpChangeLineWidthOpDesc::OpChangeLineWidthOpDesc(	UINT32 toolID,                      
												 	UINT32 txID,                        
												 	CCRuntimeClass* Op,			
												 	TCHAR* tok,					
												 	pfnGetState gs,				
												 	UINT32 helpId,				
												 	UINT32 bubbleID,				
												 	UINT32 resourceID,		
												 	UINT32 controlID,			
												 	BOOL ReceiveMessages, 		
												 	BOOL Smart, 
												 	BOOL Clean 
						  						 )
	:UndoableOpDescriptor(toolID, txID, Op, tok, gs, helpId, bubbleID, 
						 resourceID, controlID, ReceiveMessages, Smart, Clean, 0, 0 /*GREY_WHEN_NO_CURRENT_DOC*/) 

{

}

/********************************************************************************************

	BOOL OpChangeLineWidthOpDesc::SetCurrentSelWidth()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/94
	Outputs:	-
	Returns:	FALSE if we run out of memory
	Purpose:	Sets the currently selected line width value. 

				If no objects are selected, or no selected objects require a line width
				attribute then the current line width attribute for the tool is displayed.

				If all selected objects (which require a line width) share the same line 
				width attribute then this is displayed.
				
				if the selected objects have more than one line width value then the 
				string 'MANY' is displayed. 

	Errors:		ERROR is set if we run out of memory
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

BOOL OpChangeLineWidthOpDesc::SetCurrentSelWidth()
{
	// This check should not be neccessary ! (It is a bodge, there should always be a 
	// current tool, however at the moment it can be NULL ?? 
	if (Tool::GetCurrent() != NULL)
	{

		if (Document::GetCurrent() != NULL) 
		{
			//AttrLineWidth* LineAttr;
			NodeAttribute* LineAttr;
			NodeAttribute* LineColour;

			// Determine if there is a common attribute (line width)
			SelRange::CommonAttribResult Result = GetApplication()->FindSelection()->
												FindCommonAttribute(CC_RUNTIME_CLASS(AttrLineWidth), 
													 		  	   &LineAttr);

			String_256 StrLinWidth;

			switch (Result)
			{
				case SelRange::ATTR_COMMON:
				// noparachute
				case SelRange::ATTR_NONE:
				{
					// When ATTR_NODE LineAttr will be the current line width attribute

					if (LineAttr == NULL)		// There is "no" line width currently
						return(TRUE);			// - but it is not an error, so return TRUE

					ENSURE(LineAttr->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrLineWidth), 
						"LineAttr of wrong type");  

					SelRange *pSel = GetApplication()->FindSelection();

					// set to NO_COLOUR, but only if all the line colours are transparent
					
					// Determine if there is a common attribute (colour)
					pSel->FindCommonAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), &LineColour);

					if ((LineColour != NULL) && 
						((AttrStrokeColour*)LineColour)->GetStartColour()->IsTransparent())
						StrLinWidth = String_8(_R(IDS_NONE)); // there is a common attr and it's transparent
					else
					{
						// go through the selection and search the applied 
						// line colour attributes for a transparent one

						Node *pNode = pSel->FindFirst();

						while (pNode != NULL)
						{
							NodeAttribute *pAppliedAttr = NULL;

							// find the applied line colour for this node
							if (pNode->IS_KIND_OF(NodeRenderableInk))
								((NodeRenderableInk *)pNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), &pAppliedAttr);

							// then check if it's transparent
							if ((pAppliedAttr) &&
								((AttrStrokeColour*)pAppliedAttr)->GetStartColour()->IsTransparent())
								break;	// found a transparent applied attr
							
							// go to the next one
							pNode = pSel->FindNext(pNode);
						}

						if (pNode != NULL)
						{
							// found a transparent attr, but not all are transparent, so set to 'Many'
							StrLinWidth = String_8(_R(IDS_MANY));
						}
						else
						{
							// set the common line width 

							// Find the current line width attribute
							MILLIPOINT LineWidth = ((AttrLineWidth*)LineAttr)->Value.LineWidth; 

							// Convert the line width to a string using default number of decimal places
							Convert::MillipointsToString(LineWidth, COMP_POINTS, &StrLinWidth);
						}
					}

					break; 
				}
				case SelRange::ATTR_MANY:		// it's either "many" or "none"
				{
					StrLinWidth = String_8(_R(IDS_MANY)); 

					// get the colour selection
					SelRange::CommonAttribResult Result = GetApplication()->FindSelection()->
												FindCommonAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), 
													 		  	   &LineColour);

					// only one colour and it has to be transparent
					if (( (Result == SelRange::ATTR_NONE) || (Result == SelRange::ATTR_COMMON)) && 
						(((AttrStrokeColour*)LineColour)->GetStartColour()->IsTransparent()))
					{
						// ok, so it's "none"
						StrLinWidth = String_8(_R(IDS_NONE));
					}
					break;
				}
			}

			// Set the string in all gadgets associated with the OpDescriptor
			if (!SetStringInGadgets(StrLinWidth))
				return FALSE;  
		}
		else 
		{
			// There is no document so blank all gadgets
			String_256 dummy;
			if (!SetStringInGadgets(dummy))
				return FALSE; 
		}

	} // end bodge if
	return TRUE; 	
}


/********************************************************************************************

>	BOOL OpChangeLineWidthOpDesc::SetStringInGadgets(String_256& Str)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		Str: The string to set in all line width combo's
	Outputs:	-
	Returns:	FALSE if we run out of memory

	Purpose:	This function finds all gadgets associated with the OpDescriptor and sets 
				the edit field in the gadgets to Str. 
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

BOOL OpChangeLineWidthOpDesc::SetStringInGadgets(String_256& Str)
{
	// Create a list for the dialogue manager to put gadget ID's on.
	List* pGadgetList = new List;
	if (pGadgetList == 0)
	{
		InformError(_R(IDE_NOMORE_MEMORY));
		return FALSE;
	}

	// Obtain a list of all line width controls
	if (BuildGadgetList(pGadgetList))
	{	  

		GadgetListItem*	GadgetItem;
		for (GadgetItem = (GadgetListItem*) pGadgetList->GetHead(); 
			 GadgetItem != 0; 
			 GadgetItem	= (GadgetListItem*) pGadgetList->GetNext(GadgetItem))
		{

			// If the gadget has no items in its list then initialise it now
			InitialiseControl(GadgetItem->pDialogOp, GadgetItem->gidGadgetID);

			GadgetItem->pDialogOp->SetStringGadgetValue(GadgetItem->gidGadgetID, 
														   Str, 
														   FALSE, 
														   -1); 
		}
	}
	pGadgetList->DeleteAll();
	delete pGadgetList;

	return TRUE;
}
 


/********************************************************************************************

>	BOOL OpChangeLineWidthOpDesc::SetLineWidthGadgets(String_256& Str)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		Str: The string to set in all line width combo's
	Outputs:	-
	Returns:	FALSE if we run out of memory

	Purpose:	public interface to SetStringInGadget
	Errors:		-
	Scope:		public
	SeeAlso:	-

********************************************************************************************/

BOOL OpChangeLineWidthOpDesc::SetLineWidthGadgets(String_256& Str)
{
	return SetStringInGadgets(Str);
}

/********************************************************************************************

	virtual void OpChangeLineWidthOpDesc::OnControlCreate(OpDescControlCreateMsg* CreateMsg);

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function is called whenever a combo control  associated with the 
	`			OpDescriptor gets created. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

// Function to initialise the Combo with line widths
void OpChangeLineWidthOpDesc::OnControlCreate(OpDescControlCreateMsg* CreateMsg)
{
	SetCurrentSelWidth(); 
}

/********************************************************************************************

>	void OpChangeLineWidthOpDesc::InitialiseControl(DialogOp* pDialogOp, CGadgetID GadgetID)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		pDlg: 			  The bar
				SetGadgetID:	  The gadget on the bar to initialise
	Outputs:	-
	Returns:	-
	Purpose:	This function checks to see if the combo's list has been initialised.
				if it has not then we add the list items
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
 
void OpChangeLineWidthOpDesc::InitialiseControl(DialogOp* pDlg, CGadgetID SetGadgetID)
{
	// We don't do anything if a Document does not exist
	if (Document::GetCurrent() != NULL)
	{
		// Does the combo's list need to be initialised ?
		INT32 NumItems = 0;

		pDlg->GetValueCount(SetGadgetID, &NumItems); 
		if (NumItems == 0)	 // combo has not been initialised.
		{	
			// Make sure decimal points are localised.
			String_8 str025pt(TEXT("0.25pt")), str05pt(TEXT("0.5pt"));
			if (Convert::GetDecimalPointChar() != '.')
			{
				Convert::ReplaceDecimalPoint(&str025pt);
				Convert::ReplaceDecimalPoint(&str05pt);
			}

			// Fill up the gadget combo-box.
			String_256 s;
			s=_R(IDS_NONE); pDlg->SetStringGadgetValue(SetGadgetID, s);
			pDlg->SetStringGadgetValue(SetGadgetID, str025pt);
			pDlg->SetStringGadgetValue(SetGadgetID, str05pt);		
			s=_T("1pt"); pDlg->SetStringGadgetValue(SetGadgetID, s);
			s=_T("2pt"); pDlg->SetStringGadgetValue(SetGadgetID, s);
			s=_T("4pt"); pDlg->SetStringGadgetValue(SetGadgetID, s);
			s=_T("8pt"); pDlg->SetStringGadgetValue(SetGadgetID, s);
			s=_T("12pt"); pDlg->SetStringGadgetValue(SetGadgetID, s);
			s=_T("16pt"); pDlg->SetStringGadgetValue(SetGadgetID, s);
			s=_T("24pt"); pDlg->SetStringGadgetValue(SetGadgetID, s);
			s=_T("36pt"); pDlg->SetStringGadgetValue(SetGadgetID, s);
			s=_T("48pt"); pDlg->SetStringGadgetValue(SetGadgetID, s);
			s=_T("72pt"); pDlg->SetStringGadgetValue(SetGadgetID, s);

			// Set the default line width units	
			//pDlg->SetUnitGadgetValue(SetGadgetID, COMP_POINTS, 250);	  // 0.25pt
			//pDlg->SetUnitGadgetValue(SetGadgetID, COMP_POINTS, 500);	  // 0.5pt
			//pDlg->SetUnitGadgetValue(SetGadgetID, COMP_POINTS, 1000);	  // 1pt
			//pDlg->SetUnitGadgetValue(SetGadgetID, COMP_POINTS, 2000);	  // 2pt
			//pDlg->SetUnitGadgetValue(SetGadgetID, COMP_POINTS, 4000);	  // 4t
			//pDlg->SetUnitGadgetValue(SetGadgetID, COMP_POINTS, 8000);	  // 8pt
			//pDlg->SetUnitGadgetValue(SetGadgetID, COMP_POINTS, 12000);  // 12pt
			//pDlg->SetUnitGadgetValue(SetGadgetID, COMP_POINTS, 16000);  // 16pt
		
			pDlg->SetComboListLength(SetGadgetID);
		}
	}
}


/********************************************************************************************

>	virtual void OpChangeLineWidthOpDesc::OnSelectionChange(OpDescControlMsg* SelChangedMsg, 
															List* GadgetList)
																		 	
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function gets called whenever a selection is made from a combo
				box. It invokes the operation to set the line width.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


// Function to handle a selection from the combo
void OpChangeLineWidthOpDesc::OnSelectionChange(OpDescControlMsg* SelChangedMsg, 
												List* GadgetList)
{
	DialogOp* pDlg = SelChangedMsg->pDlgOp;									    
	CGadgetID SetGadgetID = SelChangedMsg->SetGadgetID;

	BOOL Valid;

	// First check whether it's "none" or not
	String_256 str;
	str = pDlg->GetStringGadgetValue(SetGadgetID, &Valid, -1);

	if (str.IsIdentical(String_8(_R(IDS_NONE))))	// it's "none" => no colour
	{
		// create a new attribute
		NodeAttribute *Attr = NULL;
		DocColour ColourToApply(COLOUR_TRANS);

		Attr = new AttrStrokeColourChange(TRUE);
		if (Attr == NULL)
			return;

		((AttrStrokeColourChange *)Attr)->SetStartColour(&ColourToApply);

		if (Attr == NULL)
			return;

		// AttributeSelected knows what to do with a selected attribute
		AttributeManager::AttributeSelected(NULL,Attr); 
	}
	else	// it's another value
	{

		// Obtain the value entered in the dialog
		MILLIPOINT LineWidth = 
			pDlg->GetUnitGadgetValue(SetGadgetID, COMP_POINTS, 0, MAX_LINE_WIDTH_MP, 
									 _R(IDS_NOT_VALID_LINE_WIDTH), &Valid); 

		// Has a valid value been entered ?
		if (Valid)
		{	
			// Create a new line width NodeAttribute
			AttrLineWidth* Attr = new AttrLineWidth(); 

			if (Attr == NULL)
				return;
				   
			Attr->Value.LineWidth = LineWidth;

			// AttributeSelected knows what to do with a selected attribute
			AttributeManager::AttributeSelected(Attr); 
		}
	}

	// Update all Combos
	// Note we must do this even if we have just set a new attribute, as a SelChanging message will not be 
	// always be broadcast.
	SetCurrentSelWidth(); // Reset to the selection
}


/********************************************************************************************

>	virtual BOOL OpChangeLineWidthOpDesc::OnSelChangingMsg(SelChangingMsg::SelectionState State)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/94
	Returns:	FALSE if we run out of memory
	Purpose:	This gets called when the selection state has changed. The SelWidth combo
				gets updated to reflect the new selection. 
																			
********************************************************************************************/


BOOL OpChangeLineWidthOpDesc::OnSelChangingMsg(SelChangingMsg::SelectionState State)
{
	// We require more refined Sel msg handling here
	//if (State != SelChangingMsg::COLOURATTCHANGED)
	//{
	//	return (SetCurrentSelWidth());
	//}
	//Moved to OnCommonAttrsChangedMsg
	return TRUE; 
}

/********************************************************************************************

>	virtual BOOL OpChangeLineWidthOpDesc::OnCommonAttrsChangedMsg(SelChangingMsg::SelectionState State)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/94
	Returns:	FALSE if we run out of memory
	Purpose:	This gets called when the common attributes have changed. The SelWidth combo
				gets updated to reflect this.

********************************************************************************************/


BOOL OpChangeLineWidthOpDesc::OnCommonAttrsChangedMsg()
{
	return (SetCurrentSelWidth());
}


/********************************************************************************************

>	virtual BOOL OpChangeLineWidthOpDesc::OnDocChangingMsg(Document* pChangingDoc, DocState State)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Inputs:		-
	Outputs:	-
	Returns:	
				
	Purpose:	To update the selection width combo whenever the document changes

	Errors:		-
	SeeAlso:	DocChangingMsg


********************************************************************************************/


BOOL OpChangeLineWidthOpDesc::OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State)
{
	return (SetCurrentSelWidth());
}

/********************************************************************************************

>	static OpState OpChangeLineWidthOpDesc::GetState(String_256* Description, OpDescriptor*); 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GetState function for the OpChangeLineWidthOpDesc 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpChangeLineWidthOpDesc::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	if (!Document::GetSelected()) Blobby.Greyed = TRUE;
	return Blobby;
}
