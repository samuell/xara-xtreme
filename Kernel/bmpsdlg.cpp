// $Id: bmpsdlg.cpp 662 2006-03-14 21:31:49Z alex $
// Implementation of the Bitmap Properties Dialog for XWP.
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
#include "camelot.h"
#include "bmpsdlg.h"
//#include "bmpdlgr.h"
//#include "bmpsdlgr.h"
//#include "will3.h"
#include "dialogop.h"
#include "giffiltr.h"
#include "opdesc.h"
//#include "simon.h"
#include "bmpprefs.h"		


CC_IMPLEMENT_DYNCREATE(BmpDlg, DialogOp)
CC_IMPLEMENT_DYNAMIC(BmpDlgParam, OpParam)

#define new CAM_DEBUG_NEW
#define MORE_THAN_ONE_BITMAP 1
#define	OPTION_ONE  0
#define	OPTION_FOUR 4
#define	FOUR		4
#define ZERO		0

// Set up the static pointer to the class which the export dialog box uses to pass parameters to and from itself. 
BmpDlgParam*  BmpDlg::pParam = NULL;

// set up the dialog box details.
const UINT32 BmpDlg::IDD = _R(IDD_BMP_DLG);
const CDlgMode BmpDlg::Mode = MODAL;		//The dialog is Modal


/*********************************************************************************************************************

	BmpDlg::bmpdlg() : DialogOp(BmpDlg::IDD, BmpDlg::Mode)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/96
	Inputs:		-
	Outputs:	-
	Returns		-
	Purpose:	bmpdlg Constructor, Sets The dlgs resource ID and Mode.
	Errors		-
	See Also	-

***********************************************************************************************************************/

BmpDlg::BmpDlg() : DialogOp(BmpDlg::IDD, BmpDlg::Mode)
{
	m_HasDelayChanged = FALSE;
	m_bInterpolationChanged = FALSE;
	m_IsManyDisplayed = FALSE;
	m_ListDropped = FALSE;
}

/***********************************************************************************************************************

	BmpDlg::~BmpDlg()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/96
	Inputs:		-
	Outputs:	-
	Returns		-
	Purpose:	Destructor, Does nothing.
	Errors		-
	See Also	-

*************************************************************************************************************************/
BmpDlg::~BmpDlg()
{
}

/**************************************************************************************************************************

	void UpdateInterpolationCheckbox()

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/00
	Inputs:		-
	Outputs:	-
	Returns		-
	Purpose:	'smooth when scaled up' has been clicked --> update its state.
	Errors		-
	See Also	-

**************************************************************************************************************************/

void BmpDlg::UpdateInterpolationCheckbox()
{
	SetInterpolationChanged(TRUE);
	
	// get the current state
	BOOL bState = GetBoolGadgetSelected(_R(IDC_BMP_ALWAYS_INTERPOLATE));

	switch(bState)
	{
	case BST_INDETERMINATE:
		{
			// currently the checkbox is set to 'many'
			// i.e. several bmps are selected, and their interpolation settings vary
			// --> Tick the box (interpolate them all)
			bState = BST_CHECKED;
			break;
		}
	case BST_UNCHECKED:
		{
			bState = BST_CHECKED;
			break;
		}
	case BST_CHECKED:
		{
			bState = BST_UNCHECKED;
			break;
		}
	default:
		{
			// this should never happen!
			ERROR3("BmpDlg::UpdateInterpolationCheckbox - 3-state checkbox has invalid value");
			return;
		}
	}

	SetBoolGadgetSelected(_R(IDC_BMP_ALWAYS_INTERPOLATE), bState);

}


/**************************************************************************************************************************

	MsgResult Message(Msg* Messgse);	

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/96
	Inputs:		-
	Outputs:	-
	Returns		-
	Purpose:	Handles all the Bitmap Properties dialog's messages.
	Errors		-
	See Also	-

**************************************************************************************************************************/
MsgResult BmpDlg::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		// Now handle the required messages that we respond to.
		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
					{
						InitDialog();
						SetDelayChanged(FALSE);
						SetListDropped(FALSE);
						SetInterpolationChanged(FALSE);
					}
			break;	
			case DIM_COMMIT:
					{
						CommitDialogValue();
						Close();	// Hide the dialog box
						End();		// Finish the operation
							
					}
			break;
			case DIM_CANCEL:	
					{
						Close();	// Hide the dialog box
						End();		// Finish the operation
						return (DLG_EAT_IF_HUNGRY(Msg)); 
					}
			break;
			case DIM_LISTDROPPED:
				{
					SetListDropped(TRUE);

					if(GetIsManyDisplayed())
					{
						DeleteAllValues(_R(IDC_SETREMOVAL));	
						// Set up the new list

						SetStringGadgetValue(_R(IDC_SETREMOVAL), _R(IDS_RESTORE_NOTHING));
						SetStringGadgetValue(_R(IDC_SETREMOVAL), _R(IDS_RESTORE_LEAVEASIS));
						SetStringGadgetValue(_R(IDC_SETREMOVAL), _R(IDS_RESTORE_BACKGROUND));
						SetStringGadgetValue(_R(IDC_SETREMOVAL), _R(IDS_RESTORE_PREVIOUS));
						SetStringGadgetValue(_R(IDC_SETREMOVAL), _R(IDS_MANY));
						SetSelectedValueIndex(_R(IDC_SETREMOVAL),OPTION_FOUR);
						SetComboListLength  (_R(IDC_SETREMOVAL));			
						SetIsManyDisplayed(FALSE);
					}
				}
			break;
			case DIM_TEXT_CHANGED:
				{
					// A control on the dialog box has been typed in.
					switch (Msg->GadgetID)	
					{	
						case _R(IDC_SETDELAY):
							SetDelayChanged(TRUE);
					}
				}
			break;
			case DIM_LFT_BN_CLICKED:
				{
					// Left button clicked.
					switch (Msg->GadgetID)	
					{	
						case _R(IDC_BMP_ALWAYS_INTERPOLATE):
						{
							UpdateInterpolationCheckbox();
							break;
						}

					}
				}
			break;
			default:
				break;
		}
	}
	return DialogOp::Message(Message);
}	

/**************************************************************************************************************************

	static OpState	GetState(String_256*, OpDescriptor*)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/96
	Inputs:		
	Outputs:	-
	Returns		-
	Purpose:	Returns the OpState for the Bitmap properties dialog operation.
	Errors		-
	See Also	-

**************************************************************************************************************************/
 OpState BmpDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState opState;
	return opState;
}


/***********************************************************************************************************************

  static BOOL Init();

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/96
	Inputs:		-
	Outputs:	-
	Returns		False if Initialisation/setup fails.
	Purpose:	bmpdlg Init method.
	Errors		-
	See Also	-

*************************************************************************************************************************/

BOOL BmpDlg::Init()						
{
	BOOL Ok = FALSE;
	Ok = RegisterOpDescriptor(
								0,								/* Tool ID*/
 								_R(IDS_PROPERTIES_DLG), 			/* String resouirce ID*/
								CC_RUNTIME_CLASS(BmpDlg),		/* Runtime class*/
 								OPTOKEN_BMP_DLG,				/* Token string*/
 								BmpDlg::GetState,				/* GetState function*/
								0,								/* Help ID*/
								0,								/* Bubble ID*/
								0,								/* Resource ID*/
								0,								/* Control ID*/
								SYSTEMBAR_ILLEGAL,								/* Bar ID*/
								TRUE,							/* Recieve system messages*/
								FALSE,							/* Smart duplicate operation*/
								TRUE,							/* Clean operation*/
								NULL,							/* No vertical counterpart*/
								_R(IDS_PROPERTIES_DLG_ONE)				/* String for one copy only*/
								);
	return (Ok);
}

/************************************************************************************************************************
	bmpdlg::Create()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/96
	Inputs:		-
	Outputs:	-
	Returns		TRUE if successful, else FALSE.
	Purpose:	
	Errors		-
	See Also	-

****************************************************************************************************************************/

BOOL BmpDlg::Create()
{
	if (DialogOp::Create())
	{ 
		return TRUE; 
	}
	else
	{
		//Could not create the dialog box, so call inform error. 
		InformError();
		// End the operation 
  		End(); 					 
			return FALSE; 
	}
}

/***********************************************************************************************************************

	bmpdlg::Do()

	Author:	Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/96
	Inputs:		-
	Outputs:	-
	Returns		-
	Purpose:	Overrides DialogOp's ::Do function to create and display a Bitmap Properties Dialog.
	Errors		-
	See Also	-

*************************************************************************************************************************/

void BmpDlg::Do(OpDescriptor*)
{
ERROR3("BmpDlg::Do called, when should be using DoWithParam!");

// End the operation
End(); 		    

}

/*********************************************************************************************************

>	void BmpDlg::DoWithParam(OpDescriptor*)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This creates and opens the dialog in response to a request from the user.  
				Allows values to be passed in and returned to the caller through the BmpDlgParam class.
	Errors:		-
	SeeAlso:	-

*********************************************************************************************************/

void BmpDlg::DoWithParam(OpDescriptor*, OpParam* Param)
{
	ERROR3IF(Param == NULL, "BmpPrefsDlg::DoWithParam - NULL Param passed in");

	BOOL ok = FALSE;;
	// Force the dialog box to be created, as it is modal it will be opened via a message.
	ok = Create();

	if (!ok)
	{
		// Could not create the dialog box, so call inform error.
		InformError();
		// End the operation. 
  		End();		
	}
}
	
/***********************************************************************************************************************

BmpDlgParam* BmpDlg::CommitDialogValue()	

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/96
	Inputs:		-
	Outputs:	-
	Returns		The pParam member of the BmpDlg class.
	Purpose:	Called when ok is pressed on the dialog box.
	Errors		-
	See Also	-

**************************************************************************************************************************/
BmpDlgParam* BmpDlg::CommitDialogValue()
{
	if(GetDelayChanged())
	{
		BOOL Valid = FALSE;
		UINT32 Delay = GetLongGadgetValue(_R(IDC_SETDELAY), 0, 65535, _R(IDS_BMPPREFS_INVALIDDELAY) ,&Valid);
		if(Valid)
		{
			pParam->SetAnimDelay(Delay);

			if(pParam->GetListSize() >= MORE_THAN_ONE_BITMAP)
				pParam->SetAreDelayValuesSame(TRUE);
		}
	}

	if(GetListDropped())
	{
		UINT32 RestoreType = GetSelectedValueIndex(_R(IDC_SETREMOVAL));

		if (RestoreType != FOUR)
		{
			// A new Restore type has been entered.
			pParam->SetRestoreType((GIFDisposalMethod)RestoreType);
			pParam->SetSameRestoreType(TRUE);
		}
	}

	if(GetInterpolationChanged())
	{
		BOOL bInterpolation = GetBoolGadgetSelected(_R(IDC_BMP_ALWAYS_INTERPOLATE));

		// The 'smooth when scaled up' tick box has been changed.
		pParam->SetInterpolation(bInterpolation);

		if(pParam->GetListSize() >= MORE_THAN_ONE_BITMAP)
			pParam->SetAreInterpolationValuesSame(TRUE);
	}

	return pParam;
}

/***********************************************************************************************************************

BOOL BmpDlg::InitDialog()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/96
	Inputs:		-
	Outputs:	-
	Returns		True if successful, else false.
	Purpose:	Sets initial dialog values
	Errors		-
	See Also	-

**************************************************************************************************************************/
BOOL BmpDlg::InitDialog()
{
  ERROR3IF(pParam == NULL, "BmpsDlg::InitDialog called after duff initialisation?!");

	if (pParam->GetMany()==TRUE)
	{
		SetStringGadgetValue(_R(IDC_NAME), _R(IDS_MANY) );		// Name details
	
		if(pParam->GetSameFormat())
			SetStringGadgetValue(_R(IDC_FORMAT), &pParam->GetBitmapFormat());	// Format details.	
		else
			SetStringGadgetValue(_R(IDC_FORMAT), _R(IDS_MANY));
		
		// Change the size field and enter the measurements.
		SetStringGadgetValue(_R(IDC_SIZEI), _R(IDS_TOTALSIZE) );
		SetStringGadgetValue(_R(IDC_SIZEII), &pParam->GetMemoryUsed());

		// Dimensions info
		if(pParam->GetSameDimensions())
		{
			String_256 DimensionsInfo; //  = BitmapWidth+" by "+BitmapHeight+" pixels,  "+BitmapCols;
			DimensionsInfo.MakeMsg(_R(IDS_SGBITMAP_FULLINFO), (TCHAR *)pParam->GetBitmapWidth(), (TCHAR *)pParam->GetBitmapHeight());
			SetStringGadgetValue(_R(IDC_DIMENSIONS), &DimensionsInfo);
		}
		else
			SetStringGadgetValue(_R(IDC_DIMENSIONS), _R(IDS_MANY));
		
		// Colour Info
		if(pParam->GetSameColors())
			SetStringGadgetValue(_R(IDC_COLOURS), &pParam->GetBitmapCols());
		else
			SetStringGadgetValue(_R(IDC_COLOURS), _R(IDS_MANY));
		
		// Transparency Info
		if (pParam->GetSameTranspType())
			SetStringGadgetValue(_R(IDC_TRANSPARENT), &pParam->GetBitmapTranspType());
		else
			SetStringGadgetValue(_R(IDC_TRANSPARENT), _R(IDS_MANY));	
		
		// Animation Delay info
		if(pParam->GetAreDelayValuesSame())
			SetLongGadgetValue(_R(IDC_SETDELAY), pParam->GetAnimDelay());
		else
			SetStringGadgetValue(_R(IDC_SETDELAY), _R(IDS_MANY) );
		
		// 'smooth when scaled up' tickbox
		if(pParam->GetAreInterpolationValuesSame())
			SetBoolGadgetSelected(_R(IDC_BMP_ALWAYS_INTERPOLATE), pParam->GetInterpolation());
		else
		{
			// grey-out the box to represent 'many' different interpolation settings
			SetBoolGadgetSelected(_R(IDC_BMP_ALWAYS_INTERPOLATE), BST_INDETERMINATE );
		}


	/* Animation Restore deatils are no longer dsplyed in the information group. Removed - Ranbir.
	
		if(pParam->GetSameRestoreTpye())
		{
			GIFDisposalMethod DisposalMethod = pParam->GetRestoreType();
		
			switch(DisposalMethod)
			{
				case GDM_NONE:
				{
					SetStringGadgetValue(_R(IDC_RESTORE),_R(IDS_RESTORE_NOTHING));
				}
				break;
		
				case  GDM_LEAVE:
				{
					SetStringGadgetValue(_R(IDC_RESTORE), _R(IDS_RESTORE_LEAVEASIS));
				}
				break;

				case GDM_BACKTOBACK:
				{
					SetStringGadgetValue(_R(IDC_RESTORE),_R(IDS_RESTORE_BACKGROUND));
					
				}
				break;

				case GDM_PREVIOUS:
				{
					SetStringGadgetValue(_R(IDC_RESTORE),_R(IDS_RESTORE_PREVIOUS));
				}
				break;
			}
	*/
		if(pParam->GetSameRestoreType())
		{
			// Set up the list for our Animation Restore types.
			SetStringGadgetValue(_R(IDC_SETREMOVAL), &String_32(_R(IDS_RESTORE_NOTHING)));
			SetStringGadgetValue(_R(IDC_SETREMOVAL), &String_32(_R(IDS_RESTORE_LEAVEASIS)));
			SetStringGadgetValue(_R(IDC_SETREMOVAL), &String_32(_R(IDS_RESTORE_BACKGROUND)));
			SetStringGadgetValue(_R(IDC_SETREMOVAL), &String_32(_R(IDS_RESTORE_PREVIOUS)));
			SetComboListLength  (_R(IDC_SETREMOVAL));			

			// Select the correct restore tpye for the List box.
			SetSelectedValueIndex(_R(IDC_SETREMOVAL), pParam->GetRestoreType());
		}
		else
		{
			// The animation restore types differ, so display "Many" in the Listbox. 
		//	SetStringGadgetValue (_R(IDC_RESTORE), _R(IDS_MANY));
			SetStringGadgetValue (_R(IDC_SETREMOVAL), _R(IDS_MANY));
			SetSelectedValueIndex(_R(IDC_SETREMOVAL), OPTION_ONE);
			SetIsManyDisplayed(TRUE);
			pParam->SetSameRestoreType(FALSE);
		}
	}
	else // Only one Bitmap is currently selected.
	{
		SetStringGadgetValue(_R(IDC_NAME), &pParam->GetBitmapName());		// Name details.
		
		SetStringGadgetValue(_R(IDC_FORMAT), &pParam->GetBitmapFormat());	// Format details.	
		
		SetStringGadgetValue(_R(IDC_SIZEII), &pParam->GetMemoryUsed());		// Memeory details.	
		
		String_256 DimensionsInfo; //  = BitmapWidth+" by "+BitmapHeight+" pixels,  "+BitmapCols;

		DimensionsInfo.MakeMsg(_R(IDS_SGBITMAP_FULLINFO), (TCHAR *)pParam->GetBitmapWidth(), (TCHAR *)pParam->GetBitmapHeight());
		SetStringGadgetValue(_R(IDC_DIMENSIONS), &DimensionsInfo);			// Dimensions details

		SetStringGadgetValue(_R(IDC_COLOURS), &pParam->GetBitmapCols());	// Colour detalis.

		SetStringGadgetValue(_R(IDC_TRANSPARENT), &pParam->GetBitmapTranspType());	// Colour detalis.
		
		SetLongGadgetValue(_R(IDC_SETDELAY), pParam->GetAnimDelay());		// Delay details.

		SetBoolGadgetSelected(_R(IDC_BMP_ALWAYS_INTERPOLATE), pParam->GetInterpolation());

	/*	Restore details are no longer dislpayed, in the info. field
	
		GIFDisposalMethod DisposalMethod = pParam->GetRestoreType();	// Set the restore details.
		
			switch(DisposalMethod)
			{
				case GDM_NONE:
				{
					SetStringGadgetValue(_R(IDC_RESTORE),_R(IDS_RESTORE_NOTHING));
				}
				break;

				case  GDM_LEAVE:
				{
					SetStringGadgetValue(_R(IDC_RESTORE),_R(IDS_RESTORE_LEAVEASIS));
				}
				break;

				case GDM_BACKTOBACK:
				{
					SetStringGadgetValue(_R(IDC_RESTORE),_R(IDS_RESTORE_BACKGROUND));
				}
				break;

				case GDM_PREVIOUS:
				{
					SetStringGadgetValue(_R(IDC_RESTORE),_R(IDS_RESTORE_PREVIOUS));
				}
				break;

				default:
					break;

			}
	*/
			// Set up the list for our Animation Restore types.
			SetStringGadgetValue(_R(IDC_SETREMOVAL), &String_32(_R(IDS_RESTORE_NOTHING)));
			SetStringGadgetValue(_R(IDC_SETREMOVAL), &String_32(_R(IDS_RESTORE_LEAVEASIS)));
			SetStringGadgetValue(_R(IDC_SETREMOVAL), &String_32(_R(IDS_RESTORE_BACKGROUND)));
			SetStringGadgetValue(_R(IDC_SETREMOVAL), &String_32(_R(IDS_RESTORE_PREVIOUS)));
			SetComboListLength  (_R(IDC_SETREMOVAL));			
			SetSelectedValueIndex(_R(IDC_SETREMOVAL), pParam->GetRestoreType());
	}
	return TRUE;
}

/***********************************************************************************************************************

  BOOL BmpDlg::InvokeBmpDlg(BmpDlgParam* pBmpDlgParam)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/96
	Inputs:		A pointer to the BmpDlgParam Class.
	Outputs:	-
	Returns		-
	Purpose:	Open a BMP Dialog.
	Errors		-
	See Also	-

**************************************************************************************************************************/

void BmpDlg::InvokeBmpDlg(BmpDlgParam* pBmpDlgParam)
{
	ERROR3IF(pBmpDlgParam == NULL, "BmpDlg::BmpDlgParam - NULL Param passed in");

	// Save a refernece for use in the dialog code.
	pParam = pBmpDlgParam;	

	OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(BmpDlg));
	OpDesc->Invoke(pBmpDlgParam,(INT32)NULL);
}




