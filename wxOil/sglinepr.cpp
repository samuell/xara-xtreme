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
// SGLinePr.cpp - the LineGallery property dialog classes

#include "camtypes.h"
#include "sglinepr.h"

#include "camelot.h"
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "galstr.h"
//#include "galres.h"
#include "mainfrm.h"
//#include "sgallery.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "sgdrag.h"
#include "sgmenu.h"
//#include "sgtree.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "sgstroke.h"
#include "strkcomp.h"
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "app.h"			// for RedrawStrokesInDocuments - in camtypes.h [AUTOMATICALLY REMOVED]
#include "objchge.h"		// for RedrawStrokesInDocuments
#include "strkattr.h"		// for RedrawStrokesInDocuments
#include "ppstroke.h"		// for RedrawStrokesInDocuments
#include "ppvecstr.h"		// for RedrawStrokesInDocuments

//#include "rikdlg.h"			// for slider stuff

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(SGalleryLinePropertiesDlg, DialogOp)

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW

// OpParam class for opening the line properties dlgs - local to this .cpp file
class LinePropertiesDlgParam : public OpParam
{
CC_DECLARE_MEMDUMP(LinePropertiesDlgParam)
public:
	LinePropertiesDlgParam(SuperGallery *Parent) : OpParam(0, 0) { ParentGal = Parent; };
	SuperGallery *ParentGal;
};
CC_IMPLEMENT_MEMDUMP(LinePropertiesDlgParam, OpParam)


// Used for the commit when text is typed stuff
static BOOL SGLinePrJustSetATextEditChangeMessage = FALSE;

// Mode of the dialog  
const CDlgMode SGalleryLinePropertiesDlg::Mode = MODELESS;

// Dialog template to use
const UINT32 SGalleryLinePropertiesDlg::IDD = _R(IDD_SGLINEPROPERTIES);

const INT32 SliderMax = 50;
/********************************************************************************************

>	SGalleryLinePropertiesDlg::SGalleryLinePropertiesDlg(): 
				DialogOp(SGalleryLinePropertiesDlg::IDD, SGalleryLinePropertiesDlg::Mode)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	Constructor

********************************************************************************************/

SGalleryLinePropertiesDlg::SGalleryLinePropertiesDlg()
				   : DialogOp(SGalleryLinePropertiesDlg::IDD, SGalleryLinePropertiesDlg::Mode)
{
	ParentGallery = NULL;
	m_pItem = NULL;
}



/********************************************************************************************

>	SGalleryLinePropertiesDlg::~SGalleryLinePropertiesDlg()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	destructor

********************************************************************************************/

SGalleryLinePropertiesDlg::~SGalleryLinePropertiesDlg()
{
	if (ParentGallery != NULL)
		ParentGallery->CurrentLinePropertiesDlg	= NULL;
}



/********************************************************************************************

>	void SGalleryLinePropertiesDlg::InitValues(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	Initialises the line properties dialogue, and sets its controls
	Scope:		private

	SeeAlso:	SGalleryLinePropertiesDlg::SetControls

********************************************************************************************/

void SGalleryLinePropertiesDlg::InitValues(void)
{
	if (ParentGallery == NULL)
	{
		ERROR3("SGalleryLinePropertiesDlg MUST be given a valid parent pointer");
		return;
	}

	// The fixed thingy slider
	SetSliderMinMax(0, SliderMax);
	SetGadgetBitmaps(_R(IDC_SGLINEPROPERTIES_FIXED_SLIDER), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
	SetSliderPos(0);

	// Set up the OK button to respond to Adjust (SOFT_COMMIT) clicks
	// Don't call this more than once per dialog - fireworks ensue !
//	DualFunctionButton(IDOK);

	// Fill in the controls with the current settings
	SetControls();
}



/********************************************************************************************

>	static void SGalleryLinePropertiesDlg::InvokeDialog(SuperGallery *Parent)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		Parent - the parent (owner) of this display dialogue. This pointer
				is used to inform the parent of changes to display/sort modes.
				May NOT be NULL.

	Purpose:	Creates a new instance of this dialogue type, connects it to the given
				gallery, and opens the dialogue on screen.

********************************************************************************************/

void SGalleryLinePropertiesDlg::InvokeDialog(SuperGallery *Parent)
{
	if (Parent->CurrentLinePropertiesDlg != NULL)		// There's one already open!
	{
		// Update the settings in the dialog
		Parent->CurrentLinePropertiesDlg->SetControls();

		// Bring it to the front of the window stack, then return
		Parent->CurrentLinePropertiesDlg->BringToTop();
		return;
	}

	LinePropertiesDlgParam OptInfo(Parent);
	OpDescriptor *Dlg = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(SGalleryLinePropertiesDlg));

	ERROR3IF(Dlg == NULL, "I can't find the Dialog OpDescriptor");

	if (Dlg != NULL)
		Dlg->Invoke(&OptInfo);
}




/********************************************************************************************

>	void SGalleryLinePropertiesDlg::SetControls(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/97

	Purpose:	(Re)Initialises the line properties dialogue controls

********************************************************************************************/

void SGalleryLinePropertiesDlg::SetControls(void)
{
	if (ParentGallery == NULL)
	{
		ERROR3("ParentGallery is NULL!");
		return;
	}

	// Call gallery first
	BOOL Result = ParentGallery->ApplyAction(SGACTION_SETLINEPROPERTIES);

	// Now set values in the dialog
	SGDisplayNode *pNode = ParentGallery->DisplayTree->FindNextSelectedItem(NULL);
	if(pNode != NULL && pNode->IsKindOf(CC_RUNTIME_CLASS(StrokeTypeItem)))
	{
		StrokeTypeItem *pItem = (StrokeTypeItem *)pNode;
		m_pItem = pItem;
	
		// Set the edit name field
		String_256 ItemText;
		pItem->GetNameText(&ItemText);
		SetNameEditField(&ItemText);

		// Set the repeating button
		SetRepeatingBool(pItem->IsRepeating());

		// Set the fixed repeating values
		SetRepeatingLong(pItem->NumRepeats());

		// Grey the fixed repeat stuff
		GreyRepeats(pItem->IsRepeating() == 0);

		// Set the override fill button
		SetOverrideFillBool(pItem->OverridesFill());

		// Set the override transparency button
		SetOverrideTransBool(pItem->OverridesTrans());
	}

	InvalidateGadget(_R(IDC_SGLINEPROPERTIES_DIAGRAM));
}



/********************************************************************************************

>	void SGalleryLinePropertiesDlg::Commit(BOOL RedrawDocument = TRUE)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/97

	Inputs:		RedrawDocument	- For things like changing the textual description of the
									stroke, we don't really need to redraw the document
									each time. Normally it's safer to redraw the lot...

	Purpose:	Applies the current settings in the dialog to its parent gallery and the
				referenced stroke in the document.

********************************************************************************************/

void SGalleryLinePropertiesDlg::Commit(BOOL RedrawDocument)
{
	if (ParentGallery == NULL)
	{
		ERROR3("No parent gallery!");
		return;
	}

	if(m_pItem == NULL)
	{
		ERROR3("SGalleryLinePropertiesDlg::Commit - No associated item");
		return;
	}

	// Commit changes to item and redraw the various bits
	if(m_pItem->IsKindOf(CC_RUNTIME_CLASS(StrokeTypeItem)))
	{
		StrokeTypeItem *pItem = (StrokeTypeItem *)m_pItem;

		StrokeDefinition *pStroke = pItem->GetStrokeDefinition();
		if (pStroke != NULL)
		{
			// Apply the checkboxes
			pStroke->SetStrokeRepeating(GetRepeatingBool());
			pStroke->SetNumStrokeRepeats(GetRepeatingLong());
			pStroke->SetOverrideFill(GetOverrideFillBool());
			pStroke->SetOverrideTrans(GetOverrideTransBool());

			// Change the brush name
			String_256 NewName;
			GetNameEditField(&NewName);
			pStroke->SetStrokeName(&NewName);

			// Get the number of repeats
			INT32 Repeats = GetRepeatingLong();
			pStroke->SetNumStrokeRepeats(Repeats);
			SetSliderPos(Repeats);

			// Grey the fixed repeat stuff
			GreyRepeats(pItem->IsRepeating() == 0);

			pItem->UpdateGalleryItem();
			InvalidateGadget(_R(IDC_SGLINEPROPERTIES_DIAGRAM));

			if(RedrawDocument)
			{
				StrokeHandle Handle = pItem->GetStrokeHandle();
				RedrawStrokesInDocuments(Handle);
			}
		}
	}
}



/********************************************************************************************

>	MsgResult SGalleryLinePropertiesDlg::Message( Msg* Message)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		Msg - The dialogue manager message to handle

	Returns:	A MsgResult
	Purpose:	Standard DialogOp message handler which handles redraw and commit stuff

********************************************************************************************/

MsgResult SGalleryLinePropertiesDlg::Message( Msg* Message)
{
	if (!(IS_OUR_DIALOG_MSG(Message))) return DialogOp::Message(Message);

	DialogMsg* TheMsg = (DialogMsg*)Message;

	switch(TheMsg->DlgMsg)
	{
		case DIM_SLIDER_POS_CHANGING:
		case DIM_SLIDER_POS_IDLE:
		case DIM_SLIDER_POS_SET:	
		{
			BOOL Valid = FALSE;

			INT32 Value = GetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_FIXED_SLIDER),0,SliderMax,0,&Valid);
			if(Value != 0 && Valid)
			{
				SetRepeatingLong(SliderMax - Value);
			}
			break;
		}

		case DIM_LFT_BN_CLICKED:
		case DIM_RGT_BN_CLICKED:
			Commit();
			break;

		case DIM_TEXT_CHANGED:
			// Only commit if the user typed something rather than the code
			if(SGLinePrJustSetATextEditChangeMessage)
				SGLinePrJustSetATextEditChangeMessage = FALSE;
			else
			{
				CGadgetID GadgetID = TheMsg->GadgetID;
				if(GadgetID == _R(IDC_SGLINEPROPERTIES_NAMEEDIT))
					Commit(FALSE);	// No need to redraw the document here
				else
					Commit(TRUE);	// OK, we need to redraw the document
			}
			break;

		case DIM_COMMIT:						// OK clicked
			Commit();
			// Drop through to CANCEL to close the dlg...

		case DIM_CANCEL:						// Cancel clicked
			Close();
			End();
			break;

		case DIM_SOFT_COMMIT:					// OK "adjust clicked"
			Commit();
			break;

		case DIM_REDRAW:				// handle redraw request for diagram gadget
			if (TheMsg->GadgetID == _R(IDC_SGLINEPROPERTIES_DIAGRAM))
				RedrawDiagram((ReDrawInfoType*)TheMsg->DlgMsgParam);
			break;

	}

	return DialogOp::Message(Message);
}



/********************************************************************************************

>	OpState	SGalleryLinePropertiesDlg::GetState(String_256*, OpDescriptor*)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Get the state of the Line Properties Dialog Op
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	SGalleryLinePropertiesDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}



/********************************************************************************************

>	BOOL SGalleryLinePropertiesDlg::Init()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialises the Line Properties Dialogue Op
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SGalleryLinePropertiesDlg::Init()
{
	return (RegisterOpDescriptor(
 								0,
 								_R(IDS_SGLINEPROPERTIESDLG),
								CC_RUNTIME_CLASS(SGalleryLinePropertiesDlg),
 								OPTOKEN_SGLINEPROPERTIESDLG,
 								SGalleryLinePropertiesDlg::GetState,
 								_R(IDST_GALLERY_MENU),	// Status line help
 								_R(IDBBL_GALLERY_MENU),	// Bubble help
 								0	/* bitmap ID */
								));
}



/********************************************************************************************

>	BOOL SGalleryLinePropertiesDlg::Create()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Returns:	FALSE if it fails
	Purpose:	Creates a Line Properties dialogue box

********************************************************************************************/

BOOL SGalleryLinePropertiesDlg::Create()
{
	ERROR3IF(ParentGallery == NULL, "My ParentGallery is NULL!");

	if (DialogOp::Create())
	{ 
		ParentGallery->CurrentLinePropertiesDlg = this;

		InitValues();
		return(TRUE);
	}

	return(FALSE);
}



/********************************************************************************************

>	void SGalleryLinePropertiesDlg::Do(OpDescriptor*)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		OPDescriptor - the Opdescriptor for this op

	Purpose:	'Does' a line properties dialogue op.
				DO NOT call this method - it must be invoked via DoWithParam

	Notes:		To show this dialogue you should call InvokeDialog
																  
	SeeAlso:	SGalleryLinePropertiesDlg::InvokeDialog

********************************************************************************************/

void SGalleryLinePropertiesDlg::Do(OpDescriptor*)
{
	ERROR3("SGalleryLinePropertiesDlg - You must use DoWithParam (Call InvokeDialog)");
	End();
}



/********************************************************************************************

>	void SGalleryLinePropertiesDlg::DoWithParam(OpDescriptor *Bob, OpParam *Param)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		Bob - the Opdescriptor for this op
				Param - The parameter. Must point at a LinePropertiesDlgParam object

	Purpose:	'Does' a line properties dialogue op. Shows the dialogue.

	Notes:		To show this dialogue you should call InvokeDialog
																  
	SeeAlso:	SGalleryLinePropertiesDlg::InvokeDialog

********************************************************************************************/

void SGalleryLinePropertiesDlg::DoWithParam(OpDescriptor*, OpParam *Param)
{
	ERROR3IF(Param == NULL, "Null parameters are illegal");

	ParentGallery = ((LinePropertiesDlgParam *)Param)->ParentGal;

	ERROR3IF(ParentGallery == NULL, "SGalleryLinePropertiesDlg needs a non-NULL parent gallery!");	

	if (ParentGallery != NULL && Create())
		Open();
	else
		End();
}



/********************************************************************************************

>	void SGalleryLinePropertiesDlg::SetNameEditField(String_256 *pString)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/97
	Inputs:		pString		- The string to copy into the edit field
	Purpose:	Can be called by sgline to set the name text from the dialog

********************************************************************************************/

void SGalleryLinePropertiesDlg::SetNameEditField(String_256 *pString)
{
	if(pString != NULL)
	{
		SGLinePrJustSetATextEditChangeMessage = TRUE;
		SetStringGadgetValue(_R(IDC_SGLINEPROPERTIES_NAMEEDIT), pString);
	}
}

/********************************************************************************************

>	void SGalleryLinePropertiesDlg::GetNameEditField(String_256 *pString)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/97
	Returns:	pString	- Copies the edit text into this string (which should have been
							allocated and passed to the function).
	Purpose:	Can be called by sgline to get the name text from the dialog

********************************************************************************************/

void SGalleryLinePropertiesDlg::GetNameEditField(String_256 *pString)
{
	if(pString != NULL)
	{
		*pString = "";

		BOOL Valid = FALSE;
		String_256 TheString = GetStringGadgetValue(_R(IDC_SGLINEPROPERTIES_NAMEEDIT), &Valid);
		if(Valid)
			*pString = TheString;
	}
}

/********************************************************************************************

>	void SGalleryLinePropertiesDlg::SetRepeatingBool(BOOL Repeating)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/97
	Inputs:		Repeating	- TRUE and we'll put a tick by the repeating item
	Purpose:	Can be called by sgline to set the repeating button state from the dialog

********************************************************************************************/

void SGalleryLinePropertiesDlg::SetRepeatingBool(BOOL Repeating)
{
	INT32 RadioValue = 0;
	if(Repeating)
		RadioValue = 1;

	SetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_REPEAT), RadioValue, TRUE, 0);
}

/********************************************************************************************

>	BOOL SGalleryLinePropertiesDlg::GetRepeatingBool()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/97
	Returns:	TRUE if the checkbox for 'repeating' is ticked
	Purpose:	Can be called by sgline to get the repeating button state from the dialog

********************************************************************************************/

BOOL SGalleryLinePropertiesDlg::GetRepeatingBool()
{
	BOOL Valid = FALSE;
	INT32 Value = GetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_REPEAT),0,1,0,&Valid);
	if(Valid && Value > 0)
		return TRUE;

	return FALSE;
}

/********************************************************************************************

>	void SGalleryLinePropertiesDlg::SetRepeatingLong(INT32 Repeats)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/97
	Inputs:		Repeats		- Number of fixed repeats for the brush
	Purpose:	Set the UI corresponding to the number of fixed repeats for a brush (where
				0 repeats really means optimal repeats)

********************************************************************************************/

void SGalleryLinePropertiesDlg::SetRepeatingLong(INT32 Repeats)
{
	SetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_FIXED), (Repeats > 0), TRUE, 0);
	SetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_OPTIMAL), (Repeats == 0), TRUE, 0);
	SetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_FIXED_EDIT), Repeats, TRUE, 0);
}

/********************************************************************************************

>	INT32 SGalleryLinePropertiesDlg::GetRepeatingLong()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/97
	Returns:	Number of fixed repeats for the brush
	Purpose:	Get the number of fixed repeats for this brush from the UI.	0 means optimal.

********************************************************************************************/

INT32 SGalleryLinePropertiesDlg::GetRepeatingLong()
{
	BOOL Valid = FALSE;

	INT32 Fixed = GetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_FIXED),0,1,0,&Valid);
	if(Fixed != 0 && Valid)
	{
		INT32 Value = GetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_FIXED_EDIT),0,255,0,&Valid);
		if(Valid && (Value > 0))
			return Value;
	}

//	SetRepeatingLong(0);

	return 0;
}


void SGalleryLinePropertiesDlg::SetSliderMinMax(INT32 Min, INT32 Max)
{
	SetGadgetRange(_R(IDC_SGLINEPROPERTIES_FIXED_SLIDER), Min, Max, 1);
}

void SGalleryLinePropertiesDlg::SetSliderPos(INT32 Value)
{
	SetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_FIXED_SLIDER), SliderMax - Value); 
}

// Grey's / un-grey's the repeat section
void SGalleryLinePropertiesDlg::GreyRepeats(BOOL Grey)
{
	EnableGadget(_R(IDC_SGLINEPROPERTIES_FIXED), !Grey);
	EnableGadget(_R(IDC_SGLINEPROPERTIES_OPTIMAL), !Grey);
	EnableGadget(_R(IDC_SGLINEPROPERTIES_FIXED_EDIT), !Grey);
	EnableGadget(_R(IDC_SGLINEPROPERTIES_FIXED_SLIDER), !Grey);

/*	SetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_FIXED), (Repeats > 0), TRUE, 0);
	SetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_OPTIMAL), (Repeats == 0), TRUE, 0);
	SetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_FIXED_EDIT), Repeats, TRUE, 0);*/
}

/********************************************************************************************

>	void SGalleryLinePropertiesDlg::SetOverrideFillBool(BOOL OverrideFill)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/97
	Inputs:		OverrideFill	- TRUE and we'll put a tick by the Override Fill item
	Purpose:	Set the Override Fill button state from the dialog

********************************************************************************************/

void SGalleryLinePropertiesDlg::SetOverrideFillBool(BOOL OverrideFill)
{
	INT32 RadioValue = 0;
	if(OverrideFill)
		RadioValue = 1;

	SetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_OVERRIDE_FILL), RadioValue, TRUE, 0);
}

/********************************************************************************************

>	BOOL SGalleryLinePropertiesDlg::GetOverrideFillBool()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/97
	Returns:	TRUE if the checkbox for 'Override Fill' is ticked
	Purpose:	Get the Override Fill button state from the dialog

********************************************************************************************/

BOOL SGalleryLinePropertiesDlg::GetOverrideFillBool()
{
	BOOL Valid = FALSE;
	INT32 Value = GetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_OVERRIDE_FILL),0,1,0,&Valid);
	if(Valid && Value > 0)
		return TRUE;

	return FALSE;
}

/********************************************************************************************

>	void SGalleryLinePropertiesDlg::SetOverrideTransBool(BOOL OverrideTrans)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/97
	Inputs:		OverrideTrans	- TRUE and we'll put a tick by the Override Trans item
	Purpose:	Set the Override Trans button state from the dialog

********************************************************************************************/

void SGalleryLinePropertiesDlg::SetOverrideTransBool(BOOL OverrideTrans)
{
	INT32 RadioValue = 0;
	if(OverrideTrans)
		RadioValue = 1;

	SetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_OVERRIDE_TRANS), RadioValue, TRUE, 0);
}

/********************************************************************************************

>	BOOL SGalleryLinePropertiesDlg::GetOverrideTransBool()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/97
	Returns:	TRUE if the checkbox for 'Override Trans' is ticked
	Purpose:	Get the Override Trans button state from the dialog

********************************************************************************************/

BOOL SGalleryLinePropertiesDlg::GetOverrideTransBool()
{
	BOOL Valid = FALSE;
	INT32 Value = GetLongGadgetValue(_R(IDC_SGLINEPROPERTIES_OVERRIDE_TRANS),0,1,0,&Valid);
	if(Valid && Value > 0)
		return TRUE;

	return FALSE;
}

/******************************************************************************

>	void SGalleryLinePropertiesDlg::RedrawDiagram(ReDrawInfoType* ExtraInfo)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/03/97
	Purpose:	Redraws the diagram on the line properties dialog

******************************************************************************/

void SGalleryLinePropertiesDlg::RedrawDiagram(ReDrawInfoType* ExtraInfo)
{
	INT32 dx = ExtraInfo->dx;	// In millipoints
	INT32 dy = ExtraInfo->dy;
	INT32 Dpi = ExtraInfo->Dpi;	// The DPI of the screen

	if(m_pItem == NULL || !(m_pItem->IsKindOf(CC_RUNTIME_CLASS(StrokeTypeItem))))
	{
		ERROR3("SGalleryLinePropertiesDlg::RedrawDiagram - Dodgy or no item");
		return;
	}

	StrokeTypeItem *pItem = (StrokeTypeItem *)m_pItem;

	// Total size of control
	DocRect ControlRect;
	ControlRect.lox = 0;
	ControlRect.loy = 0;
	ControlRect.hix = dx;
	ControlRect.hiy = dy;

	// make a render region
	DocRect RendRect = ControlRect;
	RenderRegion* pRender=CreateGRenderRegion(&RendRect,ExtraInfo);
	if (pRender!=NULL)
	{
		// Renders the stroke into the render region
//		pItem->RenderItem(pRender, &RendRect, (32000)); // 32pt line width
		pItem->RenderItem(pRender, &RendRect, dy); // almost as big as the control

		// Destroys and blits the render region
		DestroyGRenderRegion(pRender);
	}
}



/********************************************************************************************

>	void SGalleryLinePropertiesDlg::RedrawStrokesInDocuments(StrokeHandle Handle)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com> / Jason
	Created:	7/3/97
	Inputs:		Handle	- the handle of the stroke to search for in the docs
	Purpose:	Whizzes through all the documents, redrawing nodes containing the passed
				in stroke business.

********************************************************************************************/

void SGalleryLinePropertiesDlg::RedrawStrokesInDocuments(StrokeHandle Handle)
{
	Document *pOurDoc = (Document *) GetApplication()->Documents.GetHead();
	while (pOurDoc != NULL)
	{
		ObjChangeFlags ChangeFlags;
		ChangeFlags.Attribute = TRUE;

		ObjChangeParam ChangeParam(OBJCHANGE_STARTING, ChangeFlags, NULL, NULL);

		Node *CurNode = Node::DocFindFirstDepthFirst(pOurDoc);
		while (CurNode != NULL)
		{
			if (CurNode->IsAnAttribute() && CurNode->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeType)))
			{
				// actually, in here, we could possibly just use NodeAttribute::operator== to
				// see if it's the same as the Stroke Attribute that we're playing with
				// - this would be better than IsKindOf's

				// Have found a StrokeType Attribute. Check if it's a Vector Stroke attribute
				// If it is one using the stroke we're after, then force a redraw of our parent node
				AttrStrokeType *pNode = (AttrStrokeType *) CurNode;

				StrokeTypeAttrValue *pValue = (StrokeTypeAttrValue *)pNode->GetAttributeValue();

				PathProcessorStroke *pProcessor = pValue->GetPathProcessor();

				if (pProcessor != NULL && pProcessor->IsKindOf(CC_RUNTIME_CLASS(PathProcessorStrokeVector)))
				{
					StrokeHandle ThisHandle = ((PathProcessorStrokeVector *)pProcessor)->GetStrokeDefinition();
					if (ThisHandle == Handle || Handle == StrokeHandle_NoStroke)
					{
						Node *Parent = CurNode->FindParent();  
						if (Parent != NULL && Parent->IsBounded())
						{
							// Invoke AllowOp on the node to tag it so that parents such as blends
							// and moulds will also redraw if necessary. We ignore the return code
							// because we're not actually going to touch the node at all.
							CurNode->AllowOp(&ChangeParam);

							// And redraw the directly affected node (our parent)
							DocRect NodeBounds = ((NodeRenderableBounded *)Parent)->GetBoundingRect();
							pOurDoc->ForceRedraw(Parent->FindParentSpread(), NodeBounds, FALSE, Parent);
						}
					}
				}
			}
			CurNode = CurNode->DocFindNextDepthFirst();
		}
		
		// And make sure that the redraw occurs right away
		pOurDoc->FlushRedraw();

		ChangeParam.Define(OBJCHANGE_FINISHED, ChangeParam.GetChangeFlags(), NULL, NULL);
		UpdateAllChangedNodes(&ChangeParam);

		pOurDoc = (Document *) GetApplication()->Documents.GetNext(pOurDoc);
	}
}
