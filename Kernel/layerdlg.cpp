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

// Implementation of the LayerDlg class

/* 
*/ 

#include "camtypes.h" 
//#include "ensure.h"   - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "dlgtypes.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h"   
//#include "barsdlgs.h"
//#include "simon.h"
//#include "dialogop.h" // DialogOp header  - in camtypes.h [AUTOMATICALLY REMOVED]
#include "dlgmgr.h"   // Dialog manager class    
#include "layerdlg.h"   
#include "mainfrm.h"
//#include "msg.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "layermgr.h"

CC_IMPLEMENT_DYNCREATE(LayerDlg, DialogOp)   

const CDlgMode LayerDlg::Mode = MODELESS; 			// The dialog is modeless  
const INT32      LayerDlg::IDD  = _R(IDD_LAYERDLG);		// The dialogs ID

/********************************************************************************************

>	LayerDlg::LayerDlg(): DialogOp(LayerDlg::IDD, LayerDlg::Mode) 


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerDlg constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				

LayerDlg::LayerDlg(): DialogOp(LayerDlg::IDD, LayerDlg::Mode) 
{   

}        

/********************************************************************************************

>	MsgResult LayerDlg::Message( CDlgMessage DlgMsg, CGadgetID Gadget)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the Layer dialog's messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult LayerDlg::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;
		
		BOOL EndDialog = FALSE; 
		switch (Msg->DlgMsg)
		{
			case DIM_COMMIT:
			{
				EndDialog = ValidValues(); 
				if (EndDialog)
				{
					// commit all changes made 
					//(Document::GetCurrent()->GetLayerMgr()).CommitLayerChanges(); 
					// Reflect changes in rest of interface 
					//(Document::GetCurrent()->GetLayerMgr()).UpdateInterface();	   
				}
				break;
			}
			case DIM_SOFT_COMMIT:
			{
				TRACE( _T("Rgt OK handling here\n"));
				break; 
			}
			case DIM_CANCEL:
			{
				// Cancel all changes made to the Layers 
				//if((Document::GetCurrent()->GetLayerMgr()).CancelLayerChanges())
				//{ 
				//	EndDialog = TRUE; 
				//}
				// else InformError will have been called 
				break;
			}
		    case DIM_LFT_BN_CLICKED:
			{
				switch (Msg->GadgetID)
				{
					case _R(IDC_NEW):
						CreateNewLayer(); 
						break; 
					case _R(IDC_DELETE):
						DeleteSelectedLayer(); 
						break; 
					default:		
						ChangeAttribute(Msg->GadgetID); 
				}
				break; 
			}
			case  DIM_SELECTION_CHANGED_COMMIT:
			{
				if (Msg->GadgetID == _R(IDC_LAYER_LIST))
				{
					// A double click, make the Layer active 
					// Obtain the index of the selected item
					GetValueIndex(_R(IDC_LAYER_LIST), &SelectedLyrIndex);
					SetSelectedLayer(SelectedLyrIndex); // Sets the selected Layer 
					SetSelectedLayerActive(); 
					// Update the dialog to show the new Layers details 
			   		ShowLayerDetails(); 	
				}
				break;
			}
			case DIM_SELECTION_CHANGED:
			{
				if (Msg->GadgetID == _R(IDC_LAYER_LIST))
				{
					// User clicked on new Layer 
					// Obtain the index of the selected item
					GetValueIndex(_R(IDC_LAYER_LIST), &SelectedLyrIndex); 

					SetSelectedLayer(SelectedLyrIndex); // Sets the selected Layer 

					// Update the dialog to show the new Layers details 
			   		ShowLayerDetails(); 	
				}
				break; 	
			}
			case DIM_TEXT_CHANGED:
			{
				if (Msg->GadgetID == _R(IDC_LAYER_ID))
				{
					// User is editing the Layer identifier 
					UpdateLayerID(); 
					break; 
				}
			}
		}

		if (EndDialog)	// Dialog communication over 
		{
			Close(); // Close the dialog 
			End(); 	// Destroy dialog 
		//	CMainFrame::pInfoBar->EnableLayerControl(); // Enable the infobar's Layer control 
	   	}
//		return (DLG_EAT_IF_HUNGRY(Msg)); 
	}
//	return OK;

	// Pass everything on to the base class . . .
	return DialogOp::Message(Message);
}  


/********************************************************************************************

>	void LayerDlg::DeleteSelectedLyr()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deletes the selected Layer 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void LayerDlg::DeleteSelectedLayer()
{
	ENSURE(NumLayers > 1, "It is illegal delete the final Layer on a spread");
	// Delete the selected Layer from the Layer list 
	DeleteValue(_R(IDC_LAYER_LIST), FALSE, SelectedLyrIndex); 

	// Flag the Layer as having been deleted 
	SelectedLyr->Deleted = TRUE; 
	
	// If there is a Layer with a z-order one greater than this Layer, then this becomes the 
	// selected Layer. Else the Layer with a z-order one less becomes the selected Layer. 

	 if ( (SelectedLyrIndex+1) != NumLayers )
	 {
	 	// There is a Layer with a z-order one greater so select this  
		do
		{
			ENSURE(SelectedLyr != NULL, "Selected Layer is NULL"); 
			//SelectedLyr = (LyrDetails*)((Document::GetCurrent()->GetLayerMgr()).LyrDetList.GetNext(SelectedLyr));    
	 	} while ( ((LyrDetails*)SelectedLyr)->Deleted );
	 	// Note there is no need to change the SelectedLyrIndex
	 }
	 else 
	 {
	 	// Select the Layer which has a z-order one less
		do
		{
			ENSURE(SelectedLyr != NULL, "Selected Layer is NULL"); 
			//SelectedLyr = (LyrDetails*)((Document::GetCurrent()->GetLayerMgr()).LyrDetList.GetPrev(SelectedLyr));    
	 	} while ( ((LyrDetails*)SelectedLyr)->Deleted );
	 	SelectedLyrIndex--;  
	 }
	 ENSURE(SelectedLyr != NULL, "Selected Layer does not exist"); 
	 SetSelectedValueIndex(_R(IDC_LAYER_LIST), SelectedLyrIndex); // Select the active Layer 
	 NumLayers--; 
	 ShowLayerDetails(); 


}

/********************************************************************************************

>	void LyrMgr::UpdateLyrID()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Updates the  identifier 
	Scope:		private 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void LayerDlg::UpdateLayerID()
{
	BOOL Valid; 
	String_256 TempStr; 
	TempStr = GetStringGadgetValue(_R(IDC_LAYER_ID), &Valid); 
	// Only update if the string has really changed 
	if (TempStr != SelectedLyr->Status.StringLayerID)
	{
		// Update the selected layer's details 	
		SelectedLyr->Status.StringLayerID = TempStr; 
		ENSURE(Valid, "Error when trying to read a string value");
		// Replace the list box string with the updated string 
		GadgetRedraw(_R(IDC_LAYER_LIST),FALSE);	// Turn off drawing until we have finished making changes 
		DeleteValue(_R(IDC_LAYER_LIST), FALSE, SelectedLyrIndex); // Delete old value 
	   	SetStringGadgetValue(_R(IDC_LAYER_LIST),&(SelectedLyr->Status.StringLayerID), FALSE, SelectedLyrIndex); 
		// Reset the selection 
		SetSelectedValueIndex(_R(IDC_LAYER_LIST), SelectedLyrIndex); // Select the active layer 
	 	GadgetRedraw(_R(IDC_LAYER_LIST),TRUE);	
	}
}

/********************************************************************************************

>	BOOL LayerDlg::CreateNewLayer()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/1/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Creates a new layer, adding it to the end of the layer details list. If a new
				layer could not be allocated then InformError is called then FALSE returned. 
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

BOOL LayerDlg::CreateNewLayer()
{
	LyrDetails* LayerDet; 
	LayerDet = new LyrDetails(); 
	if (LayerDet == NULL)
	{
		// Inform the error to the user. The error message is set by new
		InformError(); 
		return FALSE; 
	}

	// Initialise default layer values 
	LayerDet->Layer = NULL;  	// Because the layer node has not yet been created !
	LayerDet->New = TRUE; 		// A new layer 
	LayerDet->Deleted = FALSE; 

	// Default attribute values 
	//LayerDet->Status.Flags.Visible = TRUE; 
	//LayerDet->Status.Flags.Locked = FALSE; 
	//LayerDet->Status.Flags.Active = FALSE; 
	//LayerDet->Status.Flags.Printable = TRUE; 
	//LayerDet->Status.Flags.Background = FALSE; 

	//LayerDet->Status.StringLayerID = Document::GetCurrent()->GetLayerMgr().CreateUniqueLayerID();
	//LayerDet->Status.LayerQualityLevel = 11; 

	// Add the new layer details record to the tail of the layer details list 
   	//(Document::GetCurrent()->GetLayerMgr()).LyrDetList.AddTail(LayerDet); 

	NumLayers++;
	SelectedLyr = LayerDet; // Make the new layer the selected layer 
	SelectedLyrIndex = NumLayers-1; 

	// Update the layer list gadget 
 	
	// Add layerID to end of list box 
	SetStringGadgetValue(_R(IDC_LAYER_LIST),&(SelectedLyr->Status.StringLayerID)); 
	SetSelectedValueIndex(_R(IDC_LAYER_LIST), SelectedLyrIndex); // Select the new layer  

	ShowLayerDetails(); 

	return TRUE; 
}

/********************************************************************************************

>	void LayerDlg::SetSelectedLayer(WORD Index)   

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/94
	Inputs:		Index:	List index of new selected layer 
	Outputs:	-
	Returns:	-
	Purpose:	Sets the Indexth layer in the list to be the selected layer   
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void LayerDlg::SetSelectedLayer(WORD Index)   
{
/*
	// Find the Indexth layer  
	// Traverse the Layer Manager's LyrDetList and add all layer identifiers to the list gadget
	List* LyrList = &((Document::GetCurrent()->GetLayerMgr()).LyrDetList); 
	ListItem* CurLayerDet = LyrList->GetHead();  
	ENSURE(CurLayerDet != NULL, "The LyrDetList is empty"); 

	WORD i =0; 

	while (CurLayerDet != NULL)
	{
		if (! (((LyrDetails*)CurLayerDet)->Deleted) )	// The layer has not been deleted 
		{
			if (i ==  Index)
			{
				// Found 
				SelectedLyr = (LyrDetails*)CurLayerDet; 
				return; 
			}
			i++; 
		}
		CurLayerDet = LyrList->GetNext(CurLayerDet); 
	
	}
	ENSURE(FALSE, "Could not find layer to select"); 
*/
}

/********************************************************************************************

>	void LayerDlg::ChangeAttribute(CGadgetID Gadget)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/94
	Inputs:		Gadget:	Gadget ID for the attribute 
	Outputs:	-
	Returns:	-
	Purpose:	Handles a request to change an attribute of the layer 
	Scope:		private 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void LayerDlg::ChangeAttribute(CGadgetID Gadget)
{
	switch (Gadget)
	{
		case _R(IDC_VISIBLE):
			//SelectedLyr->Status.Flags.Visible = !(SelectedLyr->Status.Flags.Visible);  
			break; 
		case _R(IDC_LOCKED):
			//SelectedLyr->Status.Flags.Locked = !(SelectedLyr->Status.Flags.Locked); 
			break; 
		case _R(IDC_ACTIVE): 
		
			SetSelectedLayerActive(); 
			break; 
		case _R(IDC_GLOBAL):
			// Don't know if we are to implement global or not 
			//SelectedLyr->Status.Flags.Global = !(SelectedLyr->Status.Flags.Global);  
			break; 
		case _R(IDC_PRINTABLE):
			//SelectedLyr->Status.Flags.Printable = !(SelectedLyr->Status.Flags.Printable);  
			break; 
		case _R(IDC_BACKGROUND):
			//SelectedLyr->Status.Flags.Background = !(SelectedLyr->Status.Flags.Background);  
			break; 
	
	}

}

/********************************************************************************************

>	void LayerDlg::SetSelectedLayerActive()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets the selected layer to be the active layer
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void LayerDlg::SetSelectedLayerActive()
{
/*
	// Traverse the Layer Manager's LyrDetList and set all Active flags to FALSE 
	List* LyrList = &((Document::GetCurrent()->GetLayerMgr()).LyrDetList); 
	ListItem* CurLayerDet = LyrList->GetHead();  
	ENSURE(CurLayerDet != NULL, "The LyrDetList is empty"); 
	while (CurLayerDet != NULL)
	{
	    ((LyrDetails*)CurLayerDet)->Status.Flags.Active = FALSE; 
		CurLayerDet = LyrList->GetNext(CurLayerDet); // Get next layer detail record 
	}
	SelectedLyr->Status.Flags.Active = TRUE; 
	// Disable the Active control
	EnableGadget(_R(IDC_ACTIVE), FALSE); 
	// Disable the Delete button 
	EnableGadget(_R(IDC_DELETE), FALSE); 
	// The visible and locked controls are set to their defaults and disabled 
	SetLongGadgetValue(_R(IDC_VISIBLE), 1); 
	SetLongGadgetValue(_R(IDC_LOCKED), 0); 
	EnableGadget(_R(IDC_VISIBLE), FALSE); 
	EnableGadget(_R(IDC_LOCKED), FALSE); 
	SelectedLyr->Status.Flags.Visible = TRUE; 
	SelectedLyr->Status.Flags.Locked = FALSE;   
*/
}


/********************************************************************************************

>	BOOL LayerDlg::ValidValues()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the values entered in the dialog are valid. 

	Purpose:	To validate the values that the user has input. If the values are not 
				valid then InformError is called to inform the user of the error. 

	Scope:		private 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL LayerDlg::ValidValues()
{
/*
	// Check that the names given to the layers are unique 
	List* LyrList = &((Document::GetCurrent()->GetLayerMgr()).LyrDetList); 
	ListItem* CurLyrDet = LyrList->GetHead(); 
	ListItem* CmpLyrDet;  
	while (CurLyrDet != NULL) 
	{
		CmpLyrDet = LyrList->GetHead(); 
		while (CmpLyrDet != NULL) 
		{
			if (CmpLyrDet != CurLyrDet)	// We are not looking at the same details structure 
			{
				if (((LyrDetails*)CurLyrDet)->Status.StringLayerID == 
				    ((LyrDetails*)CmpLyrDet)->Status.StringLayerID)
				{
					// duplicate layer names found, inform the user  
					InformError(_R(IDS_LAYERIDS_NOT_UNIQUE)); 
					return (FALSE); 
				}
			}
			CmpLyrDet = LyrList->GetNext(CmpLyrDet); 
		}
		CurLyrDet = LyrList->GetNext(CurLyrDet); // Get next Layer detail record 
	}
	return(TRUE); 
*/
return (TRUE);
} 


/********************************************************************************************

>	OpState	LayerDlg::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerDlg GetState method
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	LayerDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}
		 
/********************************************************************************************

>	BOOL LayerDlg::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerDlg Init method
	Errors:		-
	SeeAlso:	-

********************************************************************************************/



BOOL LayerDlg::Init()
{  
	return (TRUE);
/*
	return (RegisterOpDescriptor(
								0,
								_R(IDS_LAYERDLG), 
								CC_RUNTIME_CLASS(LayerDlg),
								OPTOKEN_LAYERDLG,
								LayerDlg::GetState,
								0,	// help ID
								_R(IDBBL_LAYERDLG),
								0	// bitmap ID
								)); 
*/
}   
 
/********************************************************************************************

>	BOOL LayerDlg::Create()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	LayerDlg create method 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

	   
BOOL LayerDlg::Create()
{                            
	ENSURE(FALSE,"The old layer dlg is now defunct!");
	return (FALSE);

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

/********************************************************************************************

>	void LayerDlg::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void LayerDlg::Do(OpDescriptor*)
{
	ENSURE(FALSE,"The old layer dlg is now defunct!");
	return;
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


/********************************************************************************************

>	BOOL LayerDlg::InitDialog()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets initial dialog values 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL LayerDlg::InitDialog()
{	
/*
	// Make the OK button dual function 
//	DualFunctionButton(IDOK);
	
	// Common controls have currently been disabled
	//SetGadgetRange(_R(IDC_QUALITY), 1, 11); 

	// Make the Layer list box dragable so that users can change the Z order of the Layers 
	//MakeListBoxDragable(_R(IDC_Lyr_LIST));  

	GadgetRedraw(_R(IDC_LAYER_LIST),FALSE);	// Turn off drawing until we have finished making changes 
										// to the gadget. 

	// Traverse the Layer Manager's LyrDetList and add all Layer identifiers to the list gadget
	List* LyrList = &((Document::GetCurrent()->GetLayerMgr()).LyrDetList); 
	ListItem* CurLyrDet = LyrList->GetHead();  
	ENSURE(CurLyrDet != NULL, "The LyrDetList is empty"); 

	WORD i =0; 
	NumLayers = 0; 

	while (CurLyrDet != NULL)
	{
		// Add string to end of list box 
		SetStringGadgetValue(_R(IDC_LAYER_LIST),&(((LyrDetails*)CurLyrDet)->Status.StringLayerID)); 
		
		// If the current Layer is active then make it the selected item 
		if (((LyrDetails*)CurLyrDet)->Status.Flags.Active)
		{
			SelectedLyr = (LyrDetails*)CurLyrDet; // Set the selected Layer 
			SelectedLyrIndex = i; 
		}

		CurLyrDet = LyrList->GetNext(CurLyrDet); 
		i++; 
		NumLayers++; 	
	}

	GadgetRedraw(_R(IDC_LAYER_LIST),TRUE);	// Turn drawing back on 

	SetSelectedValueIndex(_R(IDC_LAYER_LIST), SelectedLyrIndex); // Select the active Layer 
	
	ShowLayerDetails();					 

  	return TRUE; 
*/
return (FALSE);
} 


/********************************************************************************************

>	void LayerDlg::ShowLayerDetails()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/94
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Shows Layer info for the selected Layer 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void LayerDlg::ShowLayerDetails()
{
	ENSURE(SelectedLyr != NULL, "The Selected Layer is NULL"); 
	//SetLongGadgetValue(_R(IDC_VISIBLE), (INT32)SelectedLyr->Status.Flags.Visible ? 1: 0);
	//SetLongGadgetValue(_R(IDC_LOCKED), (INT32)SelectedLyr->Status.Flags.Locked ? 1: 0);  
	//SetLongGadgetValue(_R(IDC_ACTIVE), (INT32)SelectedLyr->Status.Flags.Active ? 1: 0);  
	SetLongGadgetValue(_R(IDC_GLOBAL), (INT32) FALSE);  				// Bodge
	//SetLongGadgetValue(_R(IDC_PRINTABLE), (INT32)SelectedLyr->Status.Flags.Printable ? 1: 0);  
	//SetLongGadgetValue(_R(IDC_BACKGROUND), (INT32)SelectedLyr->Status.Flags.Background ? 1: 0);   
	// Disable the quality gadget if global quality is set 
	SetStringGadgetValue(_R(IDC_LAYER_ID), &(SelectedLyr->Status.StringLayerID)); 
	// Disable the active, visible, and locked gadgets if the Layer is active
	//EnableGadget(_R(IDC_ACTIVE), !(SelectedLyr->Status.Flags.Active)); 
	//EnableGadget(_R(IDC_VISIBLE), !(SelectedLyr->Status.Flags.Active)); 
	//EnableGadget(_R(IDC_LOCKED), !(SelectedLyr->Status.Flags.Active)); 
	// Disable the delete gadget if the Layer is active or if there is only one Layer 
	//EnableGadget(_R(IDC_DELETE), !(SelectedLyr->Status.Flags.Active  || NumLayers == 1)); 
	return;  
}

