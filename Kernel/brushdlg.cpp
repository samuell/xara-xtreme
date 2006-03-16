// $Id: brushdlg.cpp 662 2006-03-14 21:31:49Z alex $
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

DECLARE_SOURCE("$Revision: 662 $");

#include "camtypes.h"		// the precompiled stuff

#include "brushdlg.h"		// include our class header

//#include "brdlgres.h"		// include our resources
//#include "barsdlgs.h"
#include "brshdata.h"		// for BrushData
//#include "rikdlg.h"			// _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER)
//#include "rik.h"
#include "optsmsgs.h"
#include "redraw.h"			// for REDRAWCLASS custom control
#include "qualattr.h"		// for REDRAWCLASS custom control
#include "ccdc.h"			// for REDRAWCLASS custom control

#include "brushop.h"
#include "brushmsg.h"

#include "opbevel.h"
#include "freeinfo.h"
#include "sgline.h"
#include "fixstr32.h"
//#include "resource.h"
#include "freehand.h"
#include "fixst256.h"

// firstly - I apologise for the lack of comments in this code - BUT I have more important
// things to be doing than writing comments at the moment ....

// BUT here are some helpful ones ....

// There are two types of Handle function:  Handle_____Msg	=>	which respond to ::message for the
//																active tab
//											Handle_____		=>	which handle messages to/from
//																controls; and interact with
//																functions that read the data

// All Init_____ functions are responsible for initialising the relevant controls

// All ReInit____ functions firstly reset the state of all relevant controls to active, and then
// call the relevant Init_____ functions to update the controls again (i.e.  this is how controls
// are enabled/disabled depending upon the selection).

// All Read____ functions get the actual data from the controls

// The CBrushEditDlg::Message () function is a bit nasty (take a look at it for more details).
// BUT, first and foremost it is a pig because I have tried to make this dialog box behave
// like a normal dialog box:  1) don't shut down when ENTER is pressed, and 2) update the values
// of the brush directly when EDIT is pressed ....

// CGS

CC_IMPLEMENT_DYNCREATE( CBrushEditDlg,  DialogTabOp )//DialogOp )

const INT32 MinSpacingSlider = 0;
const INT32 MaxSpacingSlider = 99;
const INT32 MinOffsetSlider = 0;
const INT32 MaxOffsetSlider = 100;
const INT32 MinScalingSlider = 0;
const INT32 MaxScalingSlider = 99;
const INT32 MinMultiSlider   = 90;
const INT32 MaxMultiSlider	= 110;
const INT32 MinScalingIncrSlider = -50;
const INT32 MaxScalingIncrSlider	= 50;
const double DefaultRotationAngle = 0;

const INT32   MIN_ROTATION_SLIDER = 0;
const INT32	 MAX_ROTATION_SLIDER = 360;
const INT32   MIN_ROTATION_INCR_SLIDER = -180;
const INT32   MAX_ROTATION_INCR_SLIDER = 180;
const INT32	 MAX_ROTATION_RANDOM_SLIDER = 360;
const INT32   MIN_RANDOM_SLIDER = 0;
const INT32   MAX_RANDOM_SLIDER = 100;
const INT32	 MAX_RANDOM_SAT_SLIDER = 100;
const INT32	 MAX_RANDOM_HUE_SLIDER = 360;
const INT32   MIN_SPACING_INCR = -10;
const INT32   MAX_SPACING_INCR = 10;
const INT32	 MIN_PRESSURE_SLIDER =  0;
const INT32	 MAX_PRESSURE_SLIDER = 100;
const INT32    MAXSPACINGMULTIPLIER = 5;
const INT32	 MAXOFFSETMULTIPLIER = 3;
const INT32	 MIN_TRANSP_SLIDER	 = 0;
const INT32	 MAX_TRANSP_SLIDER	 = 200;
const double ExpFactor = 1.6;
const double InvFactor = 1/ExpFactor;

// Bodge alert - MinSliderSpacing should really be equal to the function shown, however
// rounding problems meant that this caused problems.
const double MinSliderSpacing = 75; //pow((double)MIN_BRUSH_SPACING, 1/ExpFactor);


const double MinOffset = 0;
const double MaxOffset = 300;


#define new CAM_DEBUG_NEW

const	UINT32		CBrushEditDlg::IDD   =  _R(IDD_BRUSHEDIT);
const	CDlgMode	CBrushEditDlg::Mode  =  MODELESS;
BOOL CBrushEditDlg::m_bIsOpen = FALSE;


/********************************************************************************************

>	CBrushEditDlg::CBrushEditDlg ()

	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for CBrushEditDlg
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CBrushEditDlg::CBrushEditDlg ()
	: DialogTabOp/*DialogOp*/( CBrushEditDlg::IDD, CBrushEditDlg::Mode )
{
	FirstTime = TRUE;
	
	EditingBrushDef = FALSE;
	
	m_Handle   = BrushHandle_NoBrush;

	PreviewWindow = NULL;  // NOTE we never use the preview window anymore!
	RenderType = RT_LINE;

	pDocUnitList = NULL;
	CurrentUserUnitType = NOTYPE;
	
	UsingRandomSpacing = FALSE;  
	UsingRandomOffset = FALSE;
	UsingRandomScaling = FALSE;

	EditGadgetLostFocus = 0;

	pAttrBrush_m = NULL;
}


/********************************************************************************************

>	CBrushEditDlg::~CBrushEditDlg ()

	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


CBrushEditDlg::~CBrushEditDlg ()
{
	if (OriginalData != NULL)
		delete (OriginalData);
	if (EditData != NULL)
		delete (EditData);
	if (PreviewWindow != NULL)
		DestroyWindow (PreviewWindow);
}


/********************************************************************************************

>	BOOL CBrushEditDlg::Init ()

	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	the init function to register us as an Op
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL CBrushEditDlg::Init ()
{
	return (
	
				RegisterOpDescriptor
				(
					0,									// Tool ID
 					_R(IDS_BRUSHEDITDLG),					// String resource ID
					CC_RUNTIME_CLASS (CBrushEditDlg),	// Runtime class
 					OPTOKEN_BRUSHEDIT_DLG,				// Token string
 					CBrushEditDlg::GetState,			// GetState function
					0,									// Help ID
					_R(IDBBL_BRUSHEDIT),					// Bubble ID
					_R(IDD_BARCONTROLSTORE),				// Resource ID
					_R(IDC_BRUSHEDIT),						// Control ID
					SYSTEMBAR_EDIT,						// Bar ID
					TRUE,								// Recieve system messages
					FALSE,								// Smart duplicate operation
					TRUE,								// Clean operation
					0,									// No vertical counterpart
					0,									// String for one copy only
					( DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC ) // Auto state flags
				)
			);
}


/********************************************************************************************

>	OpState	CBrushEditDlg::GetState (String_256* pHelpString,  OpDescriptor*)

	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		not used
	Outputs:	-
	Returns:	the OpState indicating whether or not this op is greyed 
	Purpose:	Determines whether or not we can launch this op.  We are allowed to launch in the
				following circumstances:
				- Any tool, one AttrBrushType in the selection
				- FreeHandTool, as above OR brush selected in the brush combo and no document selection

				Note also that we can only ever have one instance of the dialog open at once
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	CBrushEditDlg::GetState (String_256* pHelpString,  OpDescriptor*)
{
	static OpState DialogState;
	
	DialogState.Greyed = TRUE;

	// if we have a single brush in the selection then we're ok
	AttrBrushType* pAttrBrush = GetSingleSelectedBrushAttribute();
	
	if (pAttrBrush != NULL && pAttrBrush->GetBrushHandle() != BrushHandle_NoBrush)
		DialogState.Greyed = FALSE;

	// if we're in the freehand tool and we our current brush is not default then we're also ok
	if (Tool::GetCurrentID() == TOOLID_FREEHAND)
	{
		FreeHandTool* pTool = (FreeHandTool*)Tool::GetCurrent();
		if (pTool->GetBrushHandle() != BrushHandle_NoBrush)
			DialogState.Greyed = FALSE;
	}

	// we only ever want one instance of the dialog so check our static member
	if (m_bIsOpen)
		DialogState.Greyed = TRUE;

	return (DialogState);
}


/********************************************************************************************

>	void CBrushEditDlg::Do (OpDescriptor*)

	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		not used
	Outputs:	-
	Returns:	-
	Purpose:	launches the brush edit dialog, this method will determine which brush it is
				editing from the document selection.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CBrushEditDlg::Do (OpDescriptor*)
{
	Document * pDocument = Document::GetSelected();
	ERROR3IF(pDocument == NULL,"UnitPropertiesDlg::DoWithParam No selected document");

	if (pDocument)
		pDocUnitList = pDocument->GetDocUnitList();
	ERROR3IF(pDocUnitList == NULL,"UnitPropertiesDlg::DoWithParam No doc unit list attached to this doc yet");

	CurrentUserUnitType = pDocUnitList->GetPageUnits ();
	
	// this fn. is called from the ViewContextMenu without any params so here we must initialise it ourselves
	//pOwningGadget_m = FreeHandInfoBarOp::GetSpareGadget();
	pOwningGadget_m = FreeHandInfoBarOp::GetBrushGadget();
	if (pOwningGadget_m != NULL)
		pOwningGadget_m->SetBrushDialog(this);

	EditData = new BrushData;
	if (EditData == NULL)
		return;
	
	OriginalData = new BrushData;
	m_bDisableGadgets = TRUE;
	// we want to see if there is a brush in the selection, in which case grab its data so we
	// can initialise everything ok.   Otherwise keep everything disabled
	AttrBrushType* pAttrBrush = GetSingleSelectedBrushAttribute();
	if (pAttrBrush != NULL)
	{
		*EditData = pAttrBrush->GetBrushData();
		*OriginalData = *EditData;
		m_bDisableGadgets = FALSE;
		//initialise our opparam with the original data, just in case we want to do change all
		ChangeParam_m.SetValues(*EditData);
		String_256 Name = EditData->m_Name;
		SetName(&Name);
	}
	else
	{
		// If we are in the freehand tool then we can launch so long as the tool selected brush is not default
		if (Tool::GetCurrentID() == TOOLID_FREEHAND)
		{
			FreeHandTool* pTool = (FreeHandTool*)Tool::GetCurrent();
			BrushHandle Handle = pTool->GetBrushHandle();
			if (Handle != BrushHandle_NoBrush)
			{
				BrushDefinition* pDef = BrushComponent::FindDefinition(Handle);
				if (pDef != NULL)
				{
					// the definition allocates a new data object for us
					delete EditData;
					EditData = pDef->GetBrushData();
					EditData->m_BrushHandle = Handle;
					*OriginalData = *EditData;
					m_bDisableGadgets = FALSE;
					//initialise our opparam with the original data, just in case we want to do change all
					ChangeParam_m.SetValues(*EditData);
					EditingBrushDef = TRUE;
					String_256 Name = EditData->m_Name;
					SetName(&Name);
				}
			}
		}
	}
	
	Create ();
	Open ();
	m_bIsOpen = TRUE;
	m_bPreventUpdateSet = FALSE;
	m_SliderStarted = FALSE;
}


/********************************************************************************************

>	void CBrushEditDlg::DoWithParam (OpDescriptor*, OpParam* OwningGadgetParams)

	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		OwningGadgetParams - recast to get a BrushData object
	Outputs:	-
	Returns:	-
	Purpose:	launches the brush edit dialog, however this the BrushData to initialise the
				dialog is provided through the opparam.
				Note:  I don't think that we use this method anymore.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CBrushEditDlg::DoWithParam (OpDescriptor*, OpParam* OwningGadgetParams)
{
	pOwningGadget_m  =  reinterpret_cast<CBrushGadget*> (OwningGadgetParams->Param1);
	pOwningGadget_m->SetCBrushEditDlg (this);

	// if theres already one open the don't
	if (WindowID != NULL && ::IsWindowVisible(WindowID))
		return;

	EditData = new BrushData (*(reinterpret_cast<BrushData*> (OwningGadgetParams->Param2)));
	// if we're editing the definition then we had to create a brush data from scratch so delete 
	// it here
	if (EditingBrushDef)
	{
		BrushData* DeleteData = ((reinterpret_cast<BrushData*> (OwningGadgetParams->Param2)));
		delete DeleteData;
	}
	//EditData = reinterpret_cast<BrushData*> (OwningGadgetParams->Param2);
	OriginalData = new BrushData (*EditData);

	//initialise our opparam with the original data, just in case we want to do change all
	ChangeParam_m.SetValues(*EditData);

	m_bDisableGadgets = FALSE;

	// get the ID
	if (pOwningGadget_m != 0)
	{
		Id_m = pOwningGadget_m->GetGadgetID ();
	}

	if (pOwningGadget_m->GetEditingBrushDefinition ())
	{
		EditingBrushDef = TRUE;
		GetAttributeNodeFromDefinition();
	}

	Document * pDocument = Document::GetSelected();
	ERROR3IF(pDocument == NULL,"UnitPropertiesDlg::DoWithParam No selected document");

	if (pDocument)
		pDocUnitList = pDocument->GetDocUnitList();
	ERROR3IF(pDocUnitList == NULL,"UnitPropertiesDlg::DoWithParam No doc unit list attached to this doc yet");

	CurrentUserUnitType = pDocUnitList->GetPageUnits ();

	String_256 Title (_R(IDS_BRUSHEDITDLGTITLE));
	SetName (&Title);
	
	Create ();
	Open ();

	/// tell the owning gadget that this dialog has opened
	if (pOwningGadget_m != 0)
	{
		pOwningGadget_m->DialogHasOpened ();
	}
	m_bIsOpen = TRUE;
	m_bPreventUpdateSet = FALSE;
	m_SliderStarted = FALSE;
}


/********************************************************************************************

>	BOOL CBrushEditDlg::AddControlsToHelper()


	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all went well, FALSE if something went wrong
	Purpose:	We need to subclass our edit box controls so we're adding them to the helper here
	Errors:		-
	SeeAlso:	DialogControlHelper class (DlgCtHlp.h/.cpp)

********************************************************************************************/

BOOL CBrushEditDlg::AddControlsToHelper()
{
	// basically we just add 'em in one by one
	BOOL ok = AddDialogControlToHelper(_R(IDC_EDITBRUSHSPACING), _R(IDD_BRUSHEDITSPACING));
//	if (ok) ok = AddDialogControlToHelper(_R(IDC_EDITBRUSHSPACINGINCRCONST), _R(IDD_BRUSHEDITSPACING));
	if (ok) ok = AddDialogControlToHelper(_R(IDC_EDITPATHOFFSETVALUE), _R(IDD_BRUSHEDITOFFSET));
//	if (ok) ok = AddDialogControlToHelper(_R(IDC_EDITPATHOFFSETINCRCONST), _R(IDD_BRUSHEDITOFFSET));
//	if (ok) ok = AddDialogControlToHelper(_R(IDC_EDITSCALINGINCRCONST), _R(IDD_BRUSHEDITSCALING));
	if (ok) ok = AddDialogControlToHelper(_R(IDC_EDITROTATEANGLE), _R(IDD_BRUSHEDITEFFECTS));
	if (ok) ok = AddDialogControlToHelper(_R(IDC_EDITROTATEANGLEINCR), _R(IDD_BRUSHEDITEFFECTS));
	if (ok) ok = AddDialogControlToHelper(_R(IDC_EDITBRUSHTRANSP), _R(IDD_BRUSHEDITSEQUENCE));

	return ok;
}


/********************************************************************************************

>	BOOL CBrushEditDlg::RemoveControlsFromHelper()


	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	removing the edit controls from the helper
	Errors:		-
	SeeAlso:	DialogControlHelper class (DlgCtHlp.h/.cpp)

********************************************************************************************/

void CBrushEditDlg::RemoveControlsFromHelper()
{
	BOOL ok = RemoveDialogControlFromHelper(_R(IDC_EDITBRUSHSPACING), _R(IDD_BRUSHEDITSPACING));
//	if (ok) ok = RemoveDialogControlFromHelper(_R(IDC_EDITBRUSHSPACINGINCRCONST), _R(IDD_BRUSHEDITSPACING));
	if (ok) ok = RemoveDialogControlFromHelper(_R(IDC_EDITPATHOFFSETVALUE), _R(IDD_BRUSHEDITOFFSET));
//	if (ok) ok = RemoveDialogControlFromHelper(_R(IDC_EDITPATHOFFSETINCRCONST), _R(IDD_BRUSHEDITOFFSET));
//	if (ok) ok = RemoveDialogControlFromHelper(_R(IDC_EDITSCALINGINCRCONST), _R(IDD_BRUSHEDITSCALING));
	if (ok) ok = RemoveDialogControlFromHelper(_R(IDC_EDITROTATEANGLE), _R(IDD_BRUSHEDITEFFECTS));
	if (ok) ok = RemoveDialogControlFromHelper(_R(IDC_EDITROTATEANGLEINCR), _R(IDD_BRUSHEDITEFFECTS));
	if (ok) ok = RemoveDialogControlFromHelper(_R(IDC_EDITBRUSHTRANSP), _R(IDD_BRUSHEDITSEQUENCE));


	return;
}



/********************************************************************************************

>	MsgResult CBrushEditDlg::Message (Msg* Message)


	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Message proc for this dialog.  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult CBrushEditDlg::Message (Msg* Message)
{
	BOOL bClose = FALSE;
	if (IS_OUR_DIALOG_MSG (Message))
	{
		DialogMsg* Msg = static_cast<DialogMsg*>( Message );

		CDlgResID ThePage = Msg->PageID;		// the current page

		BOOL MessageAlreadyHandled = FALSE;		// due to the 'multi-pass' nature of the function
		INT32 Test = -1;
		switch (Msg->DlgMsg)
		{
			// we don't want the focus ....
			case DIM_TITLEFOCUSWARN:
			{
			//	DialogManager::DefaultKeyboardFocus();
			//	MessageAlreadyHandled = TRUE;

				// this message is sent because the dialog has moved
				BROADCAST_TO_ALL(ScreenChangeMsg());
			}
			break;

			// we don't want the focus ....
			case DIM_CREATE:
			{	
				// are we editing a brush definition?
				if (EditingBrushDef)
				{
					// is this the first time the dialog has been created?
					if (FirstTime)
					{
						HideShuffleIrrelevantButtons ();
					
						//ResizeDialogAndCreateBrushPreview ();

						FirstTime = FALSE;
					}
				}
				else
				{
					if (FirstTime)
					{
						HideShuffleIrrelevantButtons ();
						
						FirstTime = FALSE;
					}
				}
				DialogManager::DefaultKeyboardFocus();
			}
			break;

			case DIM_SET_ACTIVE:
			{
				ReInitPage(ThePage);
			}
			break;
			case DIM_COMMIT:
				// we have renamed the OK button to be Save As New so creat
				// a new brush here
				CreateNewBrush();
		
			break;
		

			case DIM_SOFT_COMMIT:
			{
				// soft commit is generated by the apply button, which we have renamed
				// we now wish to save this brush data as the brush definition.
				OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGEBRUSHDEF);
				if (pOpDesc != NULL)
				{
					// make a derived class OpParam from the member, and tell it which definition to edit
					ChangeBrushDefOpParam DefParam;
					DefParam = ChangeParam_m;
					DefParam.ChangeType = CHANGEBRUSH_ALL;
					DefParam.m_Handle = EditData->m_BrushHandle;
					DefParam.m_NewScaling = 1.0;
					pOpDesc->Invoke(&DefParam);

				}
				EnableGadget(_R(ID_APPLY_NOW), TRUE);
			}
			break;
			// close the dialog ....
			case DIM_CANCEL:
			{
				TRACEUSER( "Diccon", _T("Closing dialog, EditingBD = %d\n"), EditingBrushDef);
				
				if (EditingBrushDef = TRUE)
				{
					
					if (pAttrBrush_m != NULL)
						delete pAttrBrush_m;
				}
				m_bIsOpen = FALSE;
			
			
				Close ();
				bClose = TRUE;
				MessageAlreadyHandled = TRUE;
				
			}
			break;

		
			// handle sliders:
			case DIM_SLIDER_POS_CHANGING :
			{	
				if (!m_bPreventUpdateSet)
				{
					if (Tool::GetCurrentID() == TOOLID_FREEHAND)
					{
						FreeHandTool* pTool = (FreeHandTool*)Tool::GetCurrent();
						pTool->SetBrushUpdateState(UPDATE_NEVER);
					}
					m_bPreventUpdateSet = TRUE;
				}
				
				HandleSliderPosChanging (Msg);
			}
			break;

			// handle sliders:  
			case DIM_SLIDER_POS_SET:
			{
			
				if (m_bPreventUpdateSet)
				{
					if (Tool::GetCurrentID() == TOOLID_FREEHAND)
					{
						
						FreeHandTool* pTool = (FreeHandTool*)Tool::GetCurrent();
						pTool->SetBrushUpdateState(UPDATE_ONIDLE);
		
					}
					m_bPreventUpdateSet = FALSE;
				}
				
				HandleSliderPosSet (Msg);

			
			}
			break;
			
			// handle buttons
			case DIM_LFT_BN_CLICKED:
			{

				HandleButtonDown(Msg);
				
			}
			break;

			// now that we have subclassed our edit boxes this handles both combos and edit boxes
			case DIM_SELECTION_CHANGED:
			{
				switch (Msg->DlgMsgParam)
				{
					case ENTER_COMMIT:
					Test = 3;
					break;
					case TAB_COMMIT:
						Test = 4;
					break;
					default:

					break;
				}
				HandleSelectionChange (Msg);
			}
			break;



			// handle the redraw of our brush preview
			case DIM_REDRAW:
				///RenderBrushPreview (reinterpret_cast<ReDrawInfoType*>( Msg->DlgMsgParam ));
			break;

			// handle right clicking on brush preview:  alters the current preview image type
			case DIM_RGT_BN_DOWN:
			{
				CycleToNextPreviewStyle ();
			}
			
		}

		// dispatch message to relevant page handler ....
		if ( ThePage && !MessageAlreadyHandled)
		{
			switch (ThePage)
			{
				case _R(IDD_BRUSHEDITSPACING):
					HandleBrushEditSpacingMsg (Msg);
					break;
				case _R(IDD_BRUSHEDITOFFSET):
					HandleBrushEditOffsetMsg (Msg);
					break;
				case _R(IDD_BRUSHEDITSCALING):
					HandleBrushEditScalingMsg (Msg);
					break;
				case _R(IDD_BRUSHEDITEFFECTS):
					HandleBrushEditEffectMsg (Msg);
					break;
				case _R(IDD_BRUSHEDITSEQUENCE):
					HandleBrushEditSequenceMsg (Msg);
					break;
				case _R(IDD_BRUSHEDITFILL):
					HandleBrushFillMsg(Msg);
					break;
				default:
					ERROR3("Message from unknown tab dialog page");
			}
		}


	}
	else  // deal with non-dialog messages here
	{
			// this message tells us that the brushdefinition has changed so we need
		// to update our preview window
		if  (Message->IsKindOf(CC_RUNTIME_CLASS(BrushDefMsg)))
		{
			
			// only if we are editing the definition
			if (EditingBrushDef)
			{
				// deelete our attribute node, and ask the definition for a new one
				GetAttributeNodeFromDefinition();

				// refresh our window
				if (pOwningGadget_m->GetEditingBrushDefinition ())
				{
					if (PreviewWindow)
					{
						InvalidateRect (PreviewWindow, NULL, FALSE);
					}
				}
			}
		}
		if (MESSAGE_IS_A(Message, SelChangingMsg))
		{
			HandleDocSelectionChange();
		}
	}

	// Pass all unhandled messages to base class for default processing!
	MsgResult Result = DialogOp::Message (Message);

	if (bClose)
	{	
	//	DlgMgr->Delete (WindowID, this);
		// tell the owning gadget that this dialog has closed
		if (pOwningGadget_m != 0)
		{
			pOwningGadget_m->DialogHasClosed();
		}
	}

	return Result;
}



/********************************************************************************************

>	void CBrushEditDlg::HandleBrushEditSpacingMsg (DialogMsg* Msg)


	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		Msg - the message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Subhandler for messages from the spacing tab, except for the
				edit box and sliders, which have their own handlers.  
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void CBrushEditDlg::HandleBrushEditSpacingMsg (DialogMsg* Msg)
{
	TalkToPage (_R(IDD_BRUSHEDITSPACING));
	
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
			InitBrushSpacing ();
		break;
		/// edit box interaction messages
		case DIM_COMMIT :
		{
			HandleCommit (Msg);
			DialogManager::DefaultKeyboardFocus ();
		}
		break;
	}
}


/********************************************************************************************

>	void CBrushEditDlg::HandleBrushEditOffsetMsg (DialogMsg* Msg)


	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		Msg - the message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Subhandler for messages from the offset tab, except for the
				edit box and sliders, which have their own handlers.  
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CBrushEditDlg::HandleBrushEditOffsetMsg (DialogMsg* Msg)
{
	TalkToPage (_R(IDD_BRUSHEDITOFFSET));
	
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
			InitBrushOffset ();
		break;
		case DIM_COMMIT :
		{
			HandleCommit (Msg);
			DialogManager::DefaultKeyboardFocus ();
		}
		break;
	}
}


/********************************************************************************************

>	void CBrushEditDlg::HandleBrushEditScalingMsg (DialogMsg* Msg)


	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		Msg - the message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Subhandler for messages from the scaling tab, except for the
				edit box and sliders, which have their own handlers.  
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CBrushEditDlg::HandleBrushEditScalingMsg (DialogMsg* Msg)
{
	TalkToPage (_R(IDD_BRUSHEDITSCALING));
	
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
			InitBrushScaling ();
		break;
		case DIM_COMMIT :
		{
			HandleCommit (Msg);
			DialogManager::DefaultKeyboardFocus ();
		}
		break;
	}
}



/********************************************************************************************

>	void CBrushEditDlg::HandleBrushEditEffectMsg (DialogMsg* Msg)


	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		Msg - the message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Subhandler for messages from the rotation tab, except for the
				edit box and sliders, which have their own handlers.  
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CBrushEditDlg::HandleBrushEditEffectMsg (DialogMsg* Msg)
{
	TalkToPage (_R(IDD_BRUSHEDITEFFECTS));
	
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
			InitBrushEffects ();
		break;
		case DIM_COMMIT :
		{
			HandleCommit (Msg);
			DialogManager::DefaultKeyboardFocus ();
		}
		break;
	}
}



/********************************************************************************************

>	void CBrushEditDlg::HandleBrushEditSequenceMsg (DialogMsg* Msg)


	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		Msg - the message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Subhandler for messages from the transparency tab, except for the
				edit box and sliders, which have their own handlers.  
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CBrushEditDlg::HandleBrushEditSequenceMsg (DialogMsg* Msg)
{
	TalkToPage (_R(IDD_BRUSHEDITSEQUENCE));
	
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
			InitBrushSequence ();
		break;
		case DIM_COMMIT :
		{
			HandleCommit (Msg);
			DialogManager::DefaultKeyboardFocus ();
		}
		break;
	}
}



/********************************************************************************************

>	void CBrushEditDlg::HandleBrushFillMsg (DialogMsg* Msg)


	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		Msg - the message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Subhandler for messages from the fill tab, except for the
				edit box and sliders, which have their own handlers.  
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CBrushEditDlg::HandleBrushFillMsg (DialogMsg* Msg)
{
	TalkToPage (_R(IDD_BRUSHEDITFILL));
	
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
			InitBrushFill ();
		break;
		case DIM_COMMIT :
		{
			HandleCommit (Msg);
			DialogManager::DefaultKeyboardFocus ();
		}
		break;
	}
}


/********************************************************************************************

>	void CBrushEditDlg::InvokeVia (CBrushGadget& pInvokeWith, BrushData* pInvokeOn, AttrBrushType* pBrush, BOOL& bMany)


	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	I'm not entirely sure what this does, perhaps Chris will be able to elaborate,
				 I think they were once used as part of launching the dialog, but I think they are
				 no longer used.
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CBrushEditDlg::InvokeVia (CBrushGadget& pInvokeWith, BrushData* pInvokeOn, AttrBrushType* pBrush, BOOL& bMany)
{
	if (pInvokeOn == NULL)
	{
		ERROR3("Invalid inputs");
		return;
	}
	
	// for various reasons we want to disable the first operation
	m_bDisableOps = TRUE;
	OpDescriptor*  pOpDescriptor = OpDescriptor::FindOpDescriptor (OPTOKEN_BRUSHEDIT_DLG);
	if( pOpDescriptor != 0 )
	{
		OpParam  Param( reinterpret_cast<INT32>( &pInvokeWith ),  reinterpret_cast<INT32>( pInvokeOn ) );
		DoWithParam( pOpDescriptor, &Param );
	}
}


void CBrushEditDlg::InvokeVia (CBrushGadget& pInvokeWith, BrushHandle Handle)
{
	// we need to make sure our handle is valid
	Document* pDoc = Document::GetCurrent();
	if (pDoc == NULL)
		return;
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	if (pBrushComp == NULL)
	{
		ERROR3("No brush component in our document");
		return;
	}
	BrushDefinition* pBrushDef = pBrushComp->FindDefinition(Handle);
	if (pBrushDef == NULL)
		return;
	
	// set the flag so we know what we're doing
	EditingBrushDef = TRUE;
	
	//get some brush data from the definition
	BrushData* pInvokeOn = pBrushDef->GetBrushData();
	if (pInvokeOn == NULL)
		return;
	// for various reasons we want to disable the first operation
	m_bDisableOps = TRUE;
	m_Handle = Handle;

	OpDescriptor*  pOpDescriptor = OpDescriptor::FindOpDescriptor (OPTOKEN_BRUSHEDIT_DLG);
	if( pOpDescriptor != 0 )
	{
		OpParam  Param( reinterpret_cast<INT32>( &pInvokeWith ),  reinterpret_cast<INT32>( pInvokeOn ) );
		DoWithParam( pOpDescriptor, &Param );
	}
}

			

void CBrushEditDlg::InvokeVia (CBrushGadget& pInvokeWith, BrushData* pInvokeOn, AttrBrushType* pBrush, AttrBrushType* pOrigBrush, BOOL& bMany)
{
	if (pInvokeOn == NULL || pBrush == NULL)
	{
		ERROR3("Invalid inputs");
		return;
	}

	OpDescriptor*  pOpDescriptor = OpDescriptor::FindOpDescriptor (OPTOKEN_BRUSHEDIT_DLG);
	if( pOpDescriptor != 0 )
	{
		pAttrBrush_m = pBrush;
		pOrigAttrBrush_m = pOrigBrush;
		OpParam  Param( reinterpret_cast<INT32>( &pInvokeWith ),  reinterpret_cast<INT32>( pInvokeOn ) );
		DoWithParam( pOpDescriptor, &Param );
	}
}

BOOL CBrushEditDlg::RegisterYourPagesInOrderPlease ()
{
	BOOL ok = (	AddAPage (_R(IDD_BRUSHEDITSPACING)) &&
				AddAPage (_R(IDD_BRUSHEDITOFFSET)) &&
				AddAPage (_R(IDD_BRUSHEDITSCALING)) &&
				AddAPage (_R(IDD_BRUSHEDITEFFECTS)) &&
				AddAPage (_R(IDD_BRUSHEDITSEQUENCE)) &&
				AddAPage (_R(IDD_BRUSHEDITFILL))
			  );

	return (ok);
}



/********************************************************************************************

>	void CBrushEditDlg::HideShuffleIrrelevantButtons ()


	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Because CPropertySheets only let you have an OK, Apply,Cancel, and Help
				button, we must rename these for our own devious ends and swap their positions
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CBrushEditDlg::HideShuffleIrrelevantButtons ()
{
	TalkToPage (NULL);
	
	// say bye-bye to the ok button!
	//HideGadget (IDOK, TRUE);

	// now swap the position of the apply and cancel buttons ....

	TalkToPage (NULL);
	RECT rectApply;
	GetGadgetPosition (_R(ID_APPLY_NOW), &rectApply);	// Preview
	RECT rectCancel;
	GetGadgetPosition (IDCANCEL, &rectCancel);			// Cancel

	SetGadgetPosition (IDCANCEL, rectApply);
	SetGadgetPosition (_R(ID_APPLY_NOW), rectCancel);
	

	ChangeButtonText();

}



/********************************************************************************************

>	void CBrushEditDlg::ChangeButtonText()


	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Because CPropertySheets only let you have an OK, Apply,Cancel, and Help
				button, we must rename these for our own devious ends.
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CBrushEditDlg::ChangeButtonText()
{
	// change the text in the cancel button to 'Close'
	CWindowID OurID = GetReadWriteWindowID();

	HWND hButton = GetDlgItem(OurID, IDCANCEL);

	if (hButton != NULL)
	{
		String_32 CloseString;
		CloseString.Load(_R(IDS_BRUSH_CLOSEDLG));
		TCHAR* String = CloseString.operator TCHAR *();
		if (String != NULL)
			SetWindowText(hButton, String);
	}
	
	// change the text in the apply button to "save"
	HWND hApply = GetDlgItem(OurID, _R(ID_APPLY_NOW));

	if (hApply != NULL)
	{
		String_32 SaveString;
		SaveString.Load(_R(IDS_BRUSH_SAVE));
		TCHAR* String = SaveString.operator TCHAR *();
		if (String != NULL)
			SetWindowText(hApply, String);
	}
	
	// change text in the Ok button to "Save as"
	HWND hOK = GetDlgItem(OurID, IDOK);

	if (hOK != NULL)
	{
		String_32 SaveAsString;
		SaveAsString.Load(_R(IDS_BRUSH_SAVE_AS));
		TCHAR* String = SaveAsString.operator TCHAR *();
		if (String != NULL)
			SetWindowText(hOK, String);
	}	
}


/********************************************************************************************

>	void CBrushEditDlg::ResizeDialogAndCreateBrushPreview ()


	Author:		This comment by Diccon (function originally by ChrisS)
	Created:	Comment added 15/12/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Something to do with the brush preview window, which we no longer use.
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void CBrushEditDlg::ResizeDialogAndCreateBrushPreview ()
{
	RECT WindowRect, ClientRect;
	GetWindowRect (WindowID, &WindowRect);
	GetClientRect (WindowID, &ClientRect);

	INT32 NewPreviewSectionWidth = 100;

	MoveWindow (WindowID, WindowRect.left - NewPreviewSectionWidth/2, WindowRect.top,
				(WindowRect.right - WindowRect.left) + NewPreviewSectionWidth,
				(WindowRect.bottom - WindowRect.top), TRUE);

	// now lets create a render region for use as our preview brush area ....

	INT32 Offset = 2;
	RECT PreviewRect;

	RECT NewClientRect;
	GetClientRect (WindowID, &NewClientRect);

	PreviewRect.left = ClientRect.right; //+ Offset;
	PreviewRect.top = ClientRect.top + Offset;
	PreviewRect.right = (NewClientRect.right - ClientRect.right) - Offset;
	PreviewRect.bottom = ClientRect.bottom - 2*Offset;

	PreviewWindow = CreateWindow (REDRAWCLASS, "", WS_VISIBLE | WS_CHILD /*| WS_BORDER*/,
								  PreviewRect.left, PreviewRect.top, PreviewRect.right,
								  PreviewRect.bottom, (HWND) WindowID, (HMENU) 1,
								  (HINSTANCE) AfxGetInstanceHandle (), NULL);

	ASSERT (PreviewWindow);
}

void CBrushEditDlg::RenderBrushPreview (ReDrawInfoType* RedrawInfo)
{
	HWND DestinationWnd = WindowFromDC (RedrawInfo->pDC->GetSafeHdc ());
	INT32* RenderWindowID = (INT32*) GetProp (DestinationWnd, "WinID");

	AttrBrushType* CurrentBrush = pAttrBrush_m;
	
	// paths should not be made static (for safety) ....

	Path  CurvePath;

	CurvePath.Initialise (8);
	CurvePath.IsFilled  =  FALSE;
	CurvePath.IsStroked = TRUE;
	CurvePath.FindStartOfPath ();

	// avoid overhead of allocation each time ....
							
	static INT32 kWidth;				kWidth				=	RedrawInfo->dx;
	static INT32 kHalfWidth;			kHalfWidth			=	kWidth/2;
	static INT32 kHeight;			kHeight				=	RedrawInfo->dy;
	static INT32 kHalfHeight;		kHalfHeight			=	kHeight/2;

	switch (RenderType)
	{
		case RT_LINE:
		{
			DocCoord P1 (kHalfWidth, 0);
			DocCoord P2 (kHalfWidth, kHeight);

			CurvePath.InsertMoveTo (P1);
			CurvePath.InsertLineTo (P2);
		}
		break;
		case RT_S:
		{
			DocCoord StartPoint2	(15000, 15000);
			DocCoord MiddleCP1		(15000, kHalfHeight);
			DocCoord MiddlePoint	(kHalfWidth, kHalfHeight);
			DocCoord MiddleCP2		(kWidth-15000, kHalfHeight);
			DocCoord EndPoint2		(kWidth-15000, kHeight-15000);

			CurvePath.InsertMoveTo	(StartPoint2);
			CurvePath.InsertCurveTo	(StartPoint2, MiddleCP1, MiddlePoint);
			CurvePath.InsertCurveTo	(MiddlePoint, MiddleCP2, EndPoint2);
			CurvePath.InsertLineTo	(EndPoint2);
		}
		break;
	}

	// render a curved line ....

	DocRect  RenderRect( 0, 0,  kWidth, kHeight );

	RenderRegion*  pRenderRegion  =  CreateGRenderRegion( &RenderRect, RedrawInfo );

	static const StockColour  kBackgroundOutline  =  COLOUR_NONE;
	static const StockColour  kBackground         =  COLOUR_WHITE;
	
	if (CurrentBrush == NULL)
	{
		ERROR3("Er, CurrentBrush is NULL in RenderBrushPreview");
		return;
	}

	PathProcessorBrush* pPathProc = CurrentBrush->GetPathProcessor();
	BrushAttrValue* pVal = (BrushAttrValue*)CurrentBrush->GetAttributeValue();

	if( pRenderRegion != 0 )
	{
		pRenderRegion->SaveContext();

		/// set drawing quality
		Quality           QualityThing( Quality::QualityMax );
		QualityAttribute  AntiAliasQualityAttr( QualityThing );
		pRenderRegion->SetQuality( &AntiAliasQualityAttr, FALSE );

		pRenderRegion->SetFillColour( kBackground );
		pRenderRegion->DrawRect( &RenderRect );

		pRenderRegion->SetLineWidth (15000);
		pPathProc->ScaleToValue (15000);
		pRenderRegion->SetLineColour( kBackgroundOutline );

		pVal->FlushCache  ();
		CurrentBrush->Render (pRenderRegion);
		pRenderRegion->DrawPath (&CurvePath);

		pRenderRegion->RestoreContext();
		DestroyGRenderRegion( pRenderRegion );
	}
}

void CBrushEditDlg::CycleToNextPreviewStyle ()
{
	if (RenderType + 1 < RT_LAST)
	{
		//RenderType Adder = 1;
		RenderType = (RenderPreviewType) (RenderType + 1);
	}
	else
	{
		RenderType = RT_LINE;
	}

	InvalidateRect (PreviewWindow, NULL, FALSE);
}

static CGadgetID BrushSpacingGadgetIDs [] =		{
												//	_R(IDC_SPINBRUSHSPACING),            //0x5f84
													_R(IDC_EDITBRUSHSPACING),            //0x5f85
											
													_R(IDC_EDITBRUSHSPACINGINCRCONST),   //0x5f8b
													_R(IDC_SLIDERBRUSHSPACINGMAXRAND),   //0x5f89
													_R(IDC_BUTTONBRUSHSPACINGRANDSEED),  //0x5f8c
													_R(IDC_STATICBRUSHSPACING),			 //0x5fb6
													
													_R(IDC_STATICBRUSHSPACINGINCRCONST), //0x5fb8
													_R(IDC_STATICBRUSHSPACINGMAXRAND),	 //0x5fb9
													_R(IDC_SLIDERBRUSHSPACINGMULT),
													_R(IDC_SLIDERBRUSHSPACING1),
													_R(IDC_SLIDERBRUSHSPACINGINCR), 
													_R(IDC_STATICBRUSHSPACINGRAND1),
													_R(IDC_COMBOSEQUENCETYPE),           //0x5f9c
												};

static NumberOfBrushSpacingGadgets = sizeof (BrushSpacingGadgetIDs) / sizeof (CGadgetID);

void CBrushEditDlg::InitBrushSpacing (BOOL OnlyForUnitChange /*= FALSE*/)
{
	SetSpacingSlider();

//	SetSpacingIncrSlider();
	
	// Edit boxes have their own function
//	UpdateEditBox(_R(IDC_EDITBRUSHSPACINGINCRCONST));
	UpdateEditBox (_R(IDC_EDITBRUSHSPACING));

	SetGadgetRange (_R(IDC_SLIDERBRUSHSPACINGMAXRAND), MIN_RANDOM_SLIDER,  MAX_RANDOM_SLIDER);
	SetGadgetBitmaps (_R(IDC_SLIDERBRUSHSPACINGMAXRAND), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
	SetLongGadgetValue (_R(IDC_SLIDERBRUSHSPACINGMAXRAND), MAX_RANDOM_SLIDER - (EditData->m_BrushSpacingMaxRand) + MIN_RANDOM_SLIDER);
	SetRandomRangeText(_R(IDC_SLIDERBRUSHSPACINGMAXRAND));

	if (m_bDisableGadgets)
	{
		ReInitBrushSpacing();
		return;
	}
	
	
	// now have to share the randomize button between spacing and sequence
	if (EditData->m_BrushSpacingMaxRand == 0 && EditData->m_SequenceType != SEQ_RANDOM )			// no random spacing
	{
		
		UsingRandomSpacing = FALSE;
		EnableGadget (_R(IDC_BUTTONBRUSHSPACINGRANDSEED), FALSE);
		
	}
	else												// we have random spacing
	{
		UsingRandomSpacing = TRUE;
		EnableGadget (_R(IDC_BUTTONBRUSHSPACINGRANDSEED), TRUE);
	}	
	InitSequenceType ();
}

static CGadgetID BrushOffsetGadgetIDs [] =	{
												//_R(IDC_SPINPATHOFFSETVALUE),         //0x5f9d
												_R(IDC_EDITPATHOFFSETVALUE),         //0x5f9
												_R(IDC_EDITPATHOFFSETINCRCONST),     //0x5fa2
												_R(IDC_SLIDEROFFSETVALUEMAXRAND),    //0x5fa4
												_R(IDC_BUTTONOFFSETVALUERANDSEED),   //0x5fa5
												_R(IDC_COMBOPATHOFFSETTYPE),         //0x5f99
												_R(IDC_STATICPATHOFFSETVALUE),		 //0x5fbd
											
												_R(IDC_STATICPATHOFFSETINCRCONST),	 //0x5fbf
												_R(IDC_STATICPATHOFFSETTYPE),		 //0x5fc0
												_R(IDC_STATICOFFSETVALUEMAXRAND),	 //0x5fc1
												_R(IDC_SLIDERPATHOFFSETVAL),
												_R(IDC_SLIDEROFFSETINCRCONST)
											//	_R(IDC_BUTTONOFFSETTYPERANDSEED)
												
											};

static NumberOfBrushOffsetGadgets = sizeof (BrushOffsetGadgetIDs) / sizeof (CGadgetID);

void CBrushEditDlg::InitBrushOffset (BOOL OnlyForUnitChange /*= FALSE*/)
{
	SetOffsetValSlider();

	// do the addition slider
//	SetGadgetRange(_R(IDC_SLIDEROFFSETINCRCONST), MIN_SPACING_INCR, MAX_SPACING_INCR);
//	SetGadgetBitmaps(_R(IDC_SLIDEROFFSETINCRCONST), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
//	double PercentVal = GetDistanceAsPercentageOfBBox(EditData->m_PathOffsetIncrConst);
//	SetDoubleGadgetValue (_R(IDC_SLIDEROFFSETINCRCONST), MAX_SPACING_INCR - PercentVal + MIN_SPACING_INCR);
	
	// set up the random slider
	SetGadgetRange (_R(IDC_SLIDEROFFSETVALUEMAXRAND), MinOffsetSlider, MaxOffsetSlider);
	SetGadgetBitmaps (_R(IDC_SLIDEROFFSETVALUEMAXRAND), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
	SetRandomRangeText(_R(IDC_SLIDEROFFSETVALUEMAXRAND));


	if (m_bDisableGadgets)
	{
		ReInitBrushOffset();
		return;
	}
	EnableGadget (_R(IDC_BUTTONOFFSETVALUERANDSEED), FALSE);
	switch (EditData->m_PathOffsetType)
	{
		case OFFSET_NONE:
			UsingRandomOffset = FALSE;
			EnableGadget (_R(IDC_STATICPATHOFFSETVALUE), FALSE);
			EnableGadget (_R(IDC_SPINPATHOFFSETVALUE), FALSE);
			EnableGadget (_R(IDC_EDITPATHOFFSETVALUE), FALSE);
			EnableGadget (_R(IDC_STATICPATHOFFSETINCRCONST), FALSE);
			EnableGadget (_R(IDC_SPINPATHOFFSETINCRCONST), FALSE);
			EnableGadget (_R(IDC_EDITPATHOFFSETINCRCONST), FALSE);
			//EnableGadget (_R(IDC_SLIDEROFFSETVALUEMAXRAND), FALSE);
			EnableGadget (_R(IDC_SLIDERPATHOFFSETVAL), FALSE);
			EnableGadget (_R(IDC_SLIDEROFFSETINCRCONST), FALSE);
			//EnableGadget (_R(IDC_STATICOFFSETVALUEMAXRAND), FALSE);
			
		break;

		case OFFSET_RANDOM:
			EnableGadget (_R(IDC_EDITPATHOFFSETVALUE), FALSE);
			EnableGadget (_R(IDC_SLIDERPATHOFFSETVAL), FALSE);
			EnableGadget (_R(IDC_STATICPATHOFFSETVALUE), FALSE);
		break;
		
		default:
			
		break;
	}
	if (EditData->m_OffsetValueMaxRand > 0)
		EnableGadget (_R(IDC_BUTTONOFFSETVALUERANDSEED), TRUE);

	InitComboPathOffsetType ();
	
	SetLongGadgetValue (_R(IDC_SLIDEROFFSETVALUEMAXRAND), MaxOffsetSlider - (EditData->m_OffsetValueMaxRand));
}

static CGadgetID BrushScalingGadgetIDs [] =	{
												_R(IDC_EDITBRUSHSCALINGINCR),        //0x5f9a
												_R(IDC_SLIDERBRUSHSCALINGMAXRAND),   //0x5f94
												_R(IDC_BUTTONBRUSHSCALINGRANDSEED),   //0x5f95
												//_R(IDC_STATICBRUSHSCALING),			 //0x5fba
												_R(IDC_STATICBRUSHSCALINGINCR),		 //0x5fbb
												_R(IDC_STATICBRUSHSCALINGMAXRAND),	 //0x5fbc
												_R(IDC_SLIDERBRUSHSCALINGINCR),
												_R(IDC_SLIDERSCALINGINCRCONST),
												_R(IDC_EDITSCALINGINCRCONST),
												_R(IDC_STATICPERCENT),
												_R(IDC_STATICPERCENT2),
												_R(IDC_SLIDERSCALINGPRESSURE)
												
											};

static NumberOfBrushScalingGadgets = sizeof (BrushScalingGadgetIDs) / sizeof (CGadgetID);

void CBrushEditDlg::InitBrushScaling (BOOL OnlyForUnitChange /*= FALSE*/)
{
	// set up the random scaling slider
	SetGadgetRange (_R(IDC_SLIDERBRUSHSCALINGMAXRAND), MinScalingSlider, MaxScalingSlider);
	SetGadgetBitmaps (_R(IDC_SLIDERBRUSHSCALINGMAXRAND), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
	SetLongGadgetValue (_R(IDC_SLIDERBRUSHSCALINGMAXRAND), MaxScalingSlider - (EditData->m_BrushScalingMaxRand));
	SetRandomRangeText(_R(IDC_SLIDERBRUSHSCALINGMAXRAND));

	// set up the increment slider
////	SetGadgetRange (_R(IDC_SLIDERSCALINGINCRCONST), MinScalingIncrSlider, MaxScalingIncrSlider);
//	SetGadgetBitmaps (_R(IDC_SLIDERSCALINGINCRCONST), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
//	SetLongGadgetValue (_R(IDC_SLIDERSCALINGINCRCONST), (MaxScalingIncrSlider - (EditData->m_BrushScalingIncrConst)) + MinScalingIncrSlider);
//	SetDoubleGadgetValue(_R(IDC_EDITSCALINGINCRCONST), EditData->m_BrushScalingIncrConst);

	// setup the pressure slider
	SetGadgetRange (_R(IDC_SLIDERSCALINGPRESSURE), MIN_PRESSURE_SLIDER, MAX_PRESSURE_SLIDER);
	SetGadgetBitmaps (_R(IDC_SLIDERSCALINGPRESSURE), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
	SetLongGadgetValue (_R(IDC_SLIDERSCALINGPRESSURE), (MAX_PRESSURE_SLIDER - EditData->m_ScalingMaxPressure) + MIN_PRESSURE_SLIDER);
	SetRandomRangeText(_R(IDC_SLIDERSCALINGPRESSURE));

	SetStringGadgetValue(_R(IDC_STATICBRUSHSCALEINFO), _R(IDS_BRUSHSCALE));

	if (m_bDisableGadgets)
	{
		ReInitBrushScaling();
		return;
	}

	if (EditData->m_BrushScalingMaxRand == 0)				// no random scaling
	{
		UsingRandomScaling = FALSE;
		//EnableGadget (_R(IDC_SLIDERBRUSHSCALINGMAXRAND), FALSE);
		//EnableGadget (_R(IDC_STATICBRUSHSCALINGMAXRAND), FALSE);
		EnableGadget (_R(IDC_BUTTONBRUSHSCALINGRANDSEED), FALSE);
	}
	else													// we have random scaling
	{
		UsingRandomScaling = TRUE;
	//	EnableGadget (_R(IDC_STATICBRUSHSCALINGINCR), FALSE);
	//	EnableGadget (_R(IDC_SPINBRUSHSCALINGINCR), FALSE);
	//	EnableGadget (_R(IDC_EDITBRUSHSCALINGINCR), FALSE);
	}

	
}

static CGadgetID BrushEffectsGadgetIDs [] = {
												
												_R(IDC_CHECKROTATE),                 //0x5f8e
												_R(IDC_EDITROTATEANGLE),              //0x5f8f
												_R(IDC_EDITROTATEANGLEINCR),
												//_R(IDC_SLIDERROTATEINCR),
												_R(IDC_BUTTONROTATEANGLERANDSEED),
												_R(IDC_SLIDERROTATEANGLEMAXRAND),
												_R(IDC_SLIDERROTATEANGLEINCR),
												_R(IDC_SLIDERROTATEANGLE),
												_R(IDC_SLIDERROTATEPRESSURE)
												//_R(IDC_EDITROTATIONINCRPROP)
												
											};

static NumberOfBrushEffectsGadgets = sizeof (BrushEffectsGadgetIDs) / sizeof (CGadgetID);

void CBrushEditDlg::InitBrushEffects (BOOL OnlyForUnitChange /*= FALSE*/)
{
	// set the random slider
	SetGadgetRange (_R(IDC_SLIDERROTATEANGLEMAXRAND),	MIN_RANDOM_SLIDER, MAX_ROTATION_RANDOM_SLIDER);
	SetGadgetBitmaps (_R(IDC_SLIDERROTATEANGLEMAXRAND), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
	SetLongGadgetValue (_R(IDC_SLIDERROTATEANGLEMAXRAND), MAX_ROTATION_RANDOM_SLIDER - (EditData->m_RotationMaxRand) + MIN_RANDOM_SLIDER);
	SetRandomRangeText(_R(IDC_SLIDERROTATEANGLEMAXRAND));

	// set the rotate angle slider and edit field
	SetDoubleGadgetValue (_R(IDC_EDITROTATEANGLE), EditData->m_RotateAngle);
	SetGadgetBitmaps (_R(IDC_SLIDERROTATEANGLE), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
	SetGadgetRange (_R(IDC_SLIDERROTATEANGLE), MIN_ROTATION_SLIDER, MAX_ROTATION_SLIDER);
	SetDoubleGadgetValue (_R(IDC_SLIDERROTATEANGLE), (MAX_ROTATION_SLIDER - EditData->m_RotateAngle) + MIN_ROTATION_SLIDER);

	// set the angle increment slider and edit field
	SetDoubleGadgetValue (_R(IDC_EDITROTATEANGLEINCR), EditData->m_RotAngleIncrConst);
	SetGadgetBitmaps (_R(IDC_SLIDERROTATEANGLEINCR), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
	SetGadgetRange (_R(IDC_SLIDERROTATEANGLEINCR), MIN_ROTATION_INCR_SLIDER, MAX_ROTATION_INCR_SLIDER);
	SetDoubleGadgetValue (_R(IDC_SLIDERROTATEANGLEINCR), (MAX_ROTATION_INCR_SLIDER - EditData->m_RotAngleIncrConst) + MIN_ROTATION_INCR_SLIDER);

	
	if (m_bDisableGadgets)
	{
		ReInitBrushEffects();
		return;
	}


	if (EditData->m_RotationMaxRand == 0)
	{
		EnableGadget (_R(IDC_BUTTONROTATEANGLERANDSEED), FALSE);
	}


	if (EditData->m_bRotate)
	{
		SetLongGadgetValue (_R(IDC_CHECKROTATE), 1);
	}

	


}

/*!!!! SPECIAL NOTE the sequence combo has been moved to the spacing tab for various reasons.
the sequence tab now contains only transparency controls, obviously.
*/

static CGadgetID BrushSequenceGadgetIDs [] = {
												_R(IDC_SLIDERTRANSP),
												_R(IDC_SLIDERTRANSPPRESSURE),
												_R(IDC_EDITBRUSHTRANSP),
												_R(IDC_STATICTRANSPPRESSURE)
											};


static NumberOfBrushSequenceGadgets = sizeof (BrushSequenceGadgetIDs) / sizeof (CGadgetID);


void CBrushEditDlg::InitBrushSequence (BOOL OnlyForUnitChange /*= FALSE*/)
{
	if (m_bDisableGadgets)
	{
		ReInitBrushSequence();
		return;
	}


	// do the transparency sliders 
	SetGadgetBitmaps(_R(IDC_SLIDERTRANSP),  _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
	SetGadgetRange(_R(IDC_SLIDERTRANSP), MIN_TRANSP_SLIDER, MAX_TRANSP_SLIDER);
	SetLongGadgetValue(_R(IDC_SLIDERTRANSP), (MAX_TRANSP_SLIDER - EditData->m_BrushTransparency) + MIN_TRANSP_SLIDER);
	
	SetGadgetBitmaps(_R(IDC_SLIDERTRANSPPRESSURE),  _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
	SetGadgetRange(_R(IDC_SLIDERTRANSPPRESSURE), MIN_PRESSURE_SLIDER, MAX_PRESSURE_SLIDER);
	SetLongGadgetValue(_R(IDC_SLIDERTRANSPPRESSURE), (MAX_PRESSURE_SLIDER - EditData->m_TranspMaxPressure) + MIN_PRESSURE_SLIDER);

	UpdateEditBox(_R(IDC_EDITBRUSHTRANSP));
	SetRandomRangeText(_R(IDC_SLIDERTRANSPPRESSURE));
}

static CGadgetID BrushFillGadgetIDs [] = {
											_R(IDC_CHECKTILEING),
											_R(IDC_RADIOLOCALFILLALL),
											_R(IDC_RADIOLOCALFILLNAMED), 
											_R(IDC_RADIOLOCALFILLNONE),
											_R(IDC_SLIDERHUERANDOM),
											_R(IDC_SLIDERSATURATIONRANDOM),
											_R(IDC_BUTTONBRUSHFILLRANDOM),

												 };

static CGadgetID LocalFillRadioGadgets [] = {
												_R(IDC_RADIOLOCALFILLALL),
												_R(IDC_RADIOLOCALFILLNAMED), 
												_R(IDC_RADIOLOCALFILLNONE),
												NULL
											};

static NumberOfBrushFillGadgets = sizeof (BrushFillGadgetIDs) / sizeof (CGadgetID);
static NumberOfLocalFillRadios = sizeof (LocalFillRadioGadgets) / sizeof (CGadgetID);

void CBrushEditDlg::InitBrushFill (BOOL OnlyForUnitChange /*= FALSE*/)
{
	
	if (m_bDisableGadgets)
	{
		ReInitBrushFill();
		return;
	}

	// do the hue and saturation sliders
	SetGadgetBitmaps(_R(IDC_SLIDERSATURATIONRANDOM),  _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
	SetGadgetRange(_R(IDC_SLIDERSATURATIONRANDOM), MIN_RANDOM_SLIDER, MAX_RANDOM_SAT_SLIDER);
	SetLongGadgetValue(_R(IDC_SLIDERSATURATIONRANDOM), (MAX_RANDOM_SAT_SLIDER - EditData->m_BrushSatMaxRand) + MIN_RANDOM_SLIDER);
	SetRandomRangeText(_R(IDC_SLIDERSATURATIONRANDOM));
	
	SetGadgetBitmaps(_R(IDC_SLIDERHUERANDOM),  _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
	SetGadgetRange(_R(IDC_SLIDERHUERANDOM), MIN_RANDOM_SLIDER, MAX_RANDOM_HUE_SLIDER);
	SetLongGadgetValue(_R(IDC_SLIDERHUERANDOM), (MAX_RANDOM_HUE_SLIDER - EditData->m_BrushHueMaxRand) + MIN_RANDOM_SLIDER);
	SetRandomRangeText(_R(IDC_SLIDERHUERANDOM));

	if (EditData->m_BrushHueMaxRand > 0 || EditData->m_BrushSatMaxRand > 0)
		EnableGadget(_R(IDC_BUTTONBRUSHFILLRANDOM), TRUE);
	else
		EnableGadget(_R(IDC_BUTTONBRUSHFILLRANDOM), FALSE);

	if (EditData->m_bTile)
	{
		SetLongGadgetValue (_R(IDC_CHECKTILEING), 1);
	}
	else
	{
		SetLongGadgetValue (_R(IDC_CHECKTILEING), 0);
	}

	// use local options not available for the brush definitions
	if (!EditingBrushDef)
	{
		SetRadioGroupSelected(LocalFillRadioGadgets, _R(IDC_RADIOLOCALFILLNONE));
		if (EditData->m_bUseLocalFillColour)
		{
			SetRadioGroupSelected(LocalFillRadioGadgets, _R(IDC_RADIOLOCALFILLALL));
		}
		if (EditData->m_bUseNamedColour == FALSE)
		{
			SetRadioGroupSelected(LocalFillRadioGadgets, _R(IDC_RADIOLOCALFILLNAMED));
		}
	
	}
	else
	{
		
		for (INT32 i = 0; i < NumberOfLocalFillRadios - 1; i++)  // -1 because the last value is NULL
			EnableGadget(LocalFillRadioGadgets[i], FALSE);
		
	}

}


void CBrushEditDlg::ReInitBrushFill()
{
	BOOL Enable = !m_bDisableGadgets;

	for (INT32 i = 0; i < NumberOfBrushFillGadgets; i++)
	{
		EnableGadget (BrushSequenceGadgetIDs [i], Enable);
	}

	if (Enable)
		InitBrushFill ();

}


void CBrushEditDlg::InitComboPathOffsetType ()
{	
	DeleteAllValues (_R(IDC_COMBOPATHOFFSETTYPE));
	
	SetStringGadgetValue (_R(IDC_COMBOPATHOFFSETTYPE), _R(IDS_OFFSET_NONE), TRUE, 0);
	SetStringGadgetValue (_R(IDC_COMBOPATHOFFSETTYPE), _R(IDS_OFFSET_ALTERNATE), TRUE, 1);
	SetStringGadgetValue (_R(IDC_COMBOPATHOFFSETTYPE), _R(IDS_OFFSET_LEFT), TRUE, 2);
	SetStringGadgetValue (_R(IDC_COMBOPATHOFFSETTYPE), _R(IDS_OFFSET_RIGHT), TRUE, 3);
	//SetStringGadgetValue (_R(IDC_COMBOPATHOFFSETTYPE), _R(IDS_OFFSET_RANDOM), TRUE, 4);

	switch (EditData->m_PathOffsetType)
	{
		case OFFSET_NONE:
			SetStringGadgetValue (_R(IDC_COMBOPATHOFFSETTYPE), _R(IDS_OFFSET_NONE), FALSE, -1);
		break;
		case OFFSET_ALTERNATE:
			SetStringGadgetValue (_R(IDC_COMBOPATHOFFSETTYPE), _R(IDS_OFFSET_ALTERNATE), FALSE, -1);
		break;
		case OFFSET_LEFT:
			SetStringGadgetValue (_R(IDC_COMBOPATHOFFSETTYPE), _R(IDS_OFFSET_LEFT), FALSE, -1);
		break;
		case OFFSET_RIGHT:
			SetStringGadgetValue (_R(IDC_COMBOPATHOFFSETTYPE), _R(IDS_OFFSET_RIGHT), FALSE, -1);
		break;
		// We have gotten rid of the random option but some may remain from early files, so
		// display the default
		default:
			SetStringGadgetValue (_R(IDC_COMBOPATHOFFSETTYPE), _R(IDS_OFFSET_NONE), FALSE, -1);
		break;
	}
}

void CBrushEditDlg::InitSequenceType ()
{
	DeleteAllValues (_R(IDC_COMBOSEQUENCETYPE));
	
	SetStringGadgetValue (_R(IDC_COMBOSEQUENCETYPE), _R(IDS_SEQ_FORWARD), TRUE, 0);
	SetStringGadgetValue (_R(IDC_COMBOSEQUENCETYPE), _R(IDS_SEQ_BACKWARD), TRUE, 1);
	SetStringGadgetValue (_R(IDC_COMBOSEQUENCETYPE), _R(IDS_SEQ_MIRROR), TRUE, 2);
	SetStringGadgetValue (_R(IDC_COMBOSEQUENCETYPE), _R(IDS_SEQ_RANDOM), TRUE, 3);

	switch (EditData->m_SequenceType)
	{
		case SEQ_FORWARD:
			SetStringGadgetValue (_R(IDC_COMBOSEQUENCETYPE), _R(IDS_SEQ_FORWARD), FALSE, -1);
		break;
		case SEQ_BACKWARD:
			SetStringGadgetValue (_R(IDC_COMBOSEQUENCETYPE), _R(IDS_SEQ_BACKWARD), FALSE, -1);
		break;
		case SEQ_MIRROR:
			SetStringGadgetValue (_R(IDC_COMBOSEQUENCETYPE), _R(IDS_SEQ_MIRROR), FALSE, -1);
		break;
		case SEQ_RANDOM:
			SetStringGadgetValue (_R(IDC_COMBOSEQUENCETYPE), _R(IDS_SEQ_RANDOM), FALSE, -1);
		break;
	}

	BrushDefinition* pDef = BrushComponent::FindDefinition(EditData->m_BrushHandle);
	if (pDef)
	{
		UINT32 Num = pDef->GetNumBrushObjects();
		if (Num == 1)
			EnableGadget(_R(IDC_COMBOSEQUENCETYPE), FALSE);

	}
}

void CBrushEditDlg::ReInitBrushSpacing ()
{
	BOOL Enable = !m_bDisableGadgets;
	for (INT32 i = 0; i < NumberOfBrushSpacingGadgets; i++)
	{
		EnableGadget (BrushSpacingGadgetIDs [i], Enable);
	}
	if (Enable)
		InitBrushSpacing ();
}

void CBrushEditDlg::ReInitBrushOffset ()
{
	BOOL Enable = !m_bDisableGadgets;
	for (INT32 i = 0; i < NumberOfBrushOffsetGadgets; i++)
	{
		EnableGadget (BrushOffsetGadgetIDs [i], Enable);
	}

	if (Enable)
		InitBrushOffset ();
}

void CBrushEditDlg::ReInitBrushScaling ()
{
	BOOL Enable = !m_bDisableGadgets;
	for (INT32 i = 0; i < NumberOfBrushScalingGadgets; i++)
	{
		EnableGadget (BrushScalingGadgetIDs [i], Enable);
	}

	if (Enable)
		InitBrushScaling ();
}

void CBrushEditDlg::ReInitBrushEffects ()
{
	BOOL Enable = !m_bDisableGadgets;
	for (INT32 i = 0; i < NumberOfBrushEffectsGadgets; i++)
	{
		EnableGadget (BrushEffectsGadgetIDs [i], Enable);
	}
	
	if (Enable)
		InitBrushEffects ();
}

void CBrushEditDlg::ReInitBrushSequence ()
{
	BOOL Enable = !m_bDisableGadgets;

	for (INT32 i = 0; i < NumberOfBrushSequenceGadgets; i++)
	{
		EnableGadget (BrushSequenceGadgetIDs [i], Enable);
	}

	if (Enable)
		InitBrushSequence ();
}

void CBrushEditDlg::ReInitUnitDependantFields (CDlgResID ThePage)
{
	switch (ThePage)
	{
		case _R(IDD_BRUSHEDITSPACING):
			InitBrushSpacing (TRUE);
		break;
		case _R(IDD_BRUSHEDITOFFSET):
			InitBrushOffset (TRUE);
		break;
		case _R(IDD_BRUSHEDITSCALING):
			InitBrushScaling (TRUE);
		break;
		case _R(IDD_BRUSHEDITEFFECTS):
			InitBrushEffects (TRUE);
		break;
		case _R(IDD_BRUSHEDITSEQUENCE):
			InitBrushSequence (TRUE);
		break;
		case _R(IDD_BRUSHEDITFILL):
			InitBrushFill (TRUE);
		break;
		default:
			ERROR3("Unknown tab dialog page");
	}
}

void CBrushEditDlg::HandleCommit (DialogMsg* Msg)
{
	
	ReadEditBox (Msg);
	
	WriteSliders (Msg);

	BROADCAST_TO_CLASS (DialogMsg (0, Msg->DlgMsg, Id_m, reinterpret_cast<INT32> (EditData)),  DialogBarOp);
	
	LaunchOp();
}

void CBrushEditDlg::HandleSelectionChange (DialogMsg* Msg)
{
	ReadComboBox (Msg);
	
	// we are now routing edit box messages through Enter_Select so if we didn't get a result in
	// the combo boxes then try the edit boxes
	if (ChangeType_m == CHANGEBRUSH_NONE)
		ReadEditBox(Msg);

	BROADCAST_TO_CLASS (DialogMsg (0, Msg->DlgMsg, Id_m, reinterpret_cast<INT32> (EditData)),  DialogBarOp);
	// launch the changebrush op
	LaunchOp();
}

void CBrushEditDlg::HandleButtonDown (DialogMsg* Msg)
{
	// try reading the checkboxes first
	ReadCheckBox (Msg);

	// if it wasn't a checkbox then try the buttons
	if (ChangeType_m == CHANGEBRUSH_NONE)
		ReadButtons(Msg);

	// still no joy, try the radio buttons
	if (ChangeType_m == CHANGEBRUSH_NONE)
		ReadRadios(Msg);

	BROADCAST_TO_CLASS (DialogMsg (0, DIM_COMMIT/*Msg->DlgMsg*/, Id_m, reinterpret_cast<INT32> (EditData)),  DialogBarOp);
	LaunchOp();
}

void CBrushEditDlg::HandleSliderPosChanging (DialogMsg* Msg)
{
	ReadSlider (Msg);

	BROADCAST_TO_CLASS (DialogMsg (0, Msg->DlgMsg, Id_m, reinterpret_cast<INT32> (EditData)),  DialogBarOp);
	
	// ok, so in order to avoid generating an undoable op for each slider movement we will only
	// do so on the first movement, so that undo will take it back to its original position
	
	if (!m_SliderStarted) // our flag that indicates if this is the first slider movement or not
	{
		TRACEUSER( "Diccon", _T("Launching Op\n"));
		LaunchOp();
		m_SliderStarted = TRUE;
	}
	else if (ChangeType_m != CHANGEBRUSH_NONE && EditingBrushDef == FALSE)
	{
		ChangeParam_m.ChangeType = ChangeType_m;		
		ChangeBrushAction Action;

		Action.ChangeSelectionNoUndo(&ChangeParam_m);
	}
}

void CBrushEditDlg::HandleSliderPosSet (DialogMsg* Msg)
{
	ReadSlider (Msg);

	BROADCAST_TO_CLASS (DialogMsg (0, Msg->DlgMsg, Id_m, reinterpret_cast<INT32> (EditData)),  DialogBarOp);
	// don't launch an op, see above for comments
	if (ChangeType_m != CHANGEBRUSH_NONE && EditingBrushDef == FALSE)
	{
		ChangeParam_m.ChangeType = ChangeType_m;		
		ChangeBrushAction Action;

		Action.ChangeSelectionNoUndo(&ChangeParam_m);
	}
	m_SliderStarted = FALSE; 
}


void CBrushEditDlg::LaunchOp()
{

	// the op we launch differs slightly depending on whether we are editing a brush from 
	// the selection or the brush definition. If its from the selection then its easy
	if (ChangeType_m != CHANGEBRUSH_NONE) //// && !(*EditData == m_LastData))
	{
		if (EditingBrushDef == FALSE)
		{
			
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGEBRUSH);
			if (pOpDesc != NULL )
			{
				ChangeParam_m.ChangeType = ChangeType_m;		
				pOpDesc->Invoke(&ChangeParam_m);
				m_LastData = *EditData;
			}
			
		}
	
	}
	else
		TRACEUSER( "Diccon", _T("NOT LAUNCHING OP!!\n"));

	// reset our member
	ChangeType_m = CHANGEBRUSH_NONE;

	// enable the save button
	SetPropertyPageModified(TRUE);
}

BOOL CBrushEditDlg::GetAttributeNodeFromDefinition()
{
	// just a quick check to see if we're in the right place
	if (!EditingBrushDef)
		return FALSE;

	// we need to get hold of the brush component from the document
	Document* pDoc = Document::GetCurrent();
	if (pDoc == NULL)
		return FALSE;

	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	if (pBrushComp == NULL)
	{
		ERROR3("No brush component in our document");
		return FALSE;
	}
	
	// wipe our existing attribute
	if (pAttrBrush_m != NULL)
	{
		delete pAttrBrush_m;
		pAttrBrush_m = NULL;
	}

	// ask the component for a new one
	pAttrBrush_m = pBrushComp->CreateAttributeNode(m_Handle);

	// did we get it?
	if (pAttrBrush_m == NULL)
		return FALSE;

	return TRUE;
}


UINT32 CBrushEditDlg::GetNewRandomNumber(UINT32 OldNumber)
{
   // seed the random number generator
   srand( (unsigned)time( NULL ) );
	
   UINT32 Rand = (UINT32)rand();
   // make sure its not the same (unlikely)
   while (Rand == OldNumber)
	   Rand = (UINT32)rand();
	
   return Rand;
}

/*

						CHANGEBRUSH_NONE,
						CHANGEBRUSH_SPACING,
						CHANGEBRUSH_SPACING_MAXRAND,
						CHANGEBRUSH_SPACING_RANDSEED,
						CHANGEBRUSH_SCALING,
						CHANGEBRUSH_OFFSET_TYPE,
						CHANGEBRUSH_OFFSET_VAL,
						CHANGEBRUSH_TILED,
						CHANGEBRUSH_TANGENTIAL,
						CHANGEBRUSH_ROTATE_ANGLE,
						CHANGEBRUSH_NAME,
						CHANGEBRUSH_USELOCALFILLCOL,
						CHANGEBRUSH_REGEN,
						CHANGEBRUSH_MULTIPLE
*/
void CBrushEditDlg::ReadEditBox (DialogMsg* Msg)
{
	MILLIPOINT MilliPointNewVal;
	double DoubleNewVal;
	INT32 LongNewVal;
	BOOL Valid = FALSE;
	ChangeType_m = CHANGEBRUSH_NONE;
	switch (Msg->GadgetID)
	{
		// brush spacing stuff ....
		
		case _R(IDC_EDITBRUSHSPACING):
		{
			double MinSpacingSlider = 0;
			double MaxSpacingSlider = 0;
			GetSpacingRange(&MinSpacingSlider, &MaxSpacingSlider);
			DoubleNewVal = GetDoubleGadgetValue (_R(IDC_EDITBRUSHSPACING), MinSpacingSlider , MaxSpacingSlider);
			MILLIPOINT NewVal  = GetPercentageAsDistance(DoubleNewVal, WIDTH);
			DoubleNewVal = pow((double)NewVal, InvFactor);
		
			if (DoubleNewVal >= MinSpacingSlider && DoubleNewVal <= MaxSpacingSlider)
			{	
				if (EditData->m_BrushSpacing != NewVal)
				{
					
					EditData->m_BrushSpacing = NewVal;
					ChangeType_m = CHANGEBRUSH_SPACING;
					ChangeParam_m.m_NewSpacing = NewVal;
					ReInitBrushSpacing();
				}
			}
			else
			{
				//InformWarning(_R(IDS_BRUSHSPACING_INVALID));
				UpdateEditBox(_R(IDC_EDITBRUSHSPACING));
			}
		}
		break;
		
		case _R(IDC_EDITBRUSHSPACINGINCRCONST):
		{
			DoubleNewVal = GetDoubleGadgetValue(_R(IDC_EDITBRUSHSPACINGINCRCONST), MIN_SPACING_INCR, MAX_SPACING_INCR, _R(IDS_BRUSHINCRINVALID), &Valid);
			
			if (DoubleNewVal > MIN_SPACING_INCR && DoubleNewVal < MAX_SPACING_INCR && Valid)
			{
				// we need to convert the % value we got from the slider into millipoints
				MILLIPOINT Distance = GetPercentageAsDistance(DoubleNewVal, WIDTH);
				if (abs(Distance - EditData->m_BrushSpacingIncrConst) > 5)
				{
					EditData->m_BrushSpacingIncrConst = Distance;
					ChangeType_m = CHANGEBRUSH_SPACING_INCRCONST;
					ChangeParam_m.m_NewSpacingIncrConst = Distance;
					SetSpacingIncrSlider();
				}
			}
			else
			{
				//InformWarning(_R(IDS_BRUSHINCRINVALID));
				UpdateEditBox(_R(IDC_EDITBRUSHSPACINGINCRCONST));
			}
		}
		break;

		// brush offset stuff ....
		
		case _R(IDC_EDITPATHOFFSETVALUE):
		{
			DoubleNewVal = GetDoubleGadgetValue(_R(IDC_EDITPATHOFFSETVALUE), MinOffset, MaxOffset,0, &Valid );

			if (DoubleNewVal <= MaxOffset && DoubleNewVal >= MinOffset && Valid)
			{
				// convert the % val into millipoints
				MILLIPOINT Offset = GetPercentageAsDistance(DoubleNewVal, HEIGHT);
				if (abs(Offset - EditData->m_PathOffsetValue) > 5)
				{
					EditData->m_PathOffsetValue = Offset;
					ChangeType_m = CHANGEBRUSH_OFFSET_VAL;
					ChangeParam_m.m_NewPathOffsetVal = Offset;
					ReInitBrushOffset();
				}
			}
			else
			{
				UpdateEditBox(_R(IDC_EDITPATHOFFSETVALUE));
			}
		}
		break;

		case _R(IDC_EDITPATHOFFSETINCRCONST):
		{
			DoubleNewVal = GetDoubleGadgetValue(_R(IDC_EDITPATHOFFSETINCRCONST), MIN_SPACING_INCR, MAX_SPACING_INCR, _R(IDS_BRUSHINCRINVALID), &Valid);
			if (Valid && DoubleNewVal <= MAX_SPACING_INCR && DoubleNewVal >= MIN_SPACING_INCR)
			{
				MilliPointNewVal = GetPercentageAsDistance(DoubleNewVal, HEIGHT);
				if (EditData->m_PathOffsetIncrConst != MilliPointNewVal)
				{	
					EditData->m_PathOffsetIncrConst = MilliPointNewVal;
					ChangeParam_m.m_NewOffsetIncrConst = MilliPointNewVal;
					ChangeType_m = CHANGEBRUSH_OFFSET_INCRCONST;
					ReInitBrushOffset();
				}
			}
			else
			{
				//InformWarning(_R(IDS_BRUSHINCRINVALID));
				UpdateEditBox(_R(IDC_EDITPATHOFFSETINCRCONST));
			}
		}
		break;
		
		// brush scaling stuff ....

		case _R(IDC_EDITSCALINGINCRCONST):
		{
			DoubleNewVal = GetDoubleGadgetValue (_R(IDC_EDITSCALINGINCRCONST), MinScalingIncrSlider, MaxScalingIncrSlider, _R(IDS_BRUSHSCALINGINCRINVALID), &Valid);

			if (DoubleNewVal <= MaxScalingIncrSlider && DoubleNewVal >= MinScalingIncrSlider && Valid)
			{
				// we need to convert the % value we got from the slider into millipoints
				if (EditData->m_BrushScalingIncrConst  != DoubleNewVal)
				{
					EditData->m_BrushScalingIncrConst = DoubleNewVal;
					ReInitBrushScaling();
					ChangeType_m = CHANGEBRUSH_SCALING_INCRCONST;
					ChangeParam_m.m_NewScalingIncrConst = EditData->m_BrushScalingIncrConst;
				}
			}
			else
			{
				//InformWarning(_R(IDS_BRUSHSCALINGINCRINVALID));
				UpdateEditBox(_R(IDC_EDITSCALINGINCRCONST));
			}
		}
		break;

		case _R(IDC_EDITBRUSHSCALINGINCR):
			DoubleNewVal = GetDoubleGadgetValue (_R(IDC_EDITBRUSHSCALINGINCR), MinMultiSlider, MaxMultiSlider, _R(IDS_INCRPROP_INVALID), &Valid);
			if (! Valid || DoubleNewVal < MinMultiSlider || DoubleNewVal > MaxMultiSlider)
			{
				//InformWarning(_R(IDS_INCRPROP_INVALID));
				UpdateEditBox(_R(IDC_EDITBRUSHSCALINGINCR));
			}
			else
			{
				if (EditData->m_BrushScalingIncr != DoubleNewVal / 100)
				{
					EditData->m_BrushScalingIncr = DoubleNewVal / 100;
					ChangeType_m = CHANGEBRUSH_SCALING_INCR;
					ChangeParam_m.m_NewScalingIncr = DoubleNewVal / 100;
					ReInitBrushScaling();
				}
			}
		break;

		case _R(IDC_EDITROTATEANGLE):
			DoubleNewVal = GetDoubleGadgetValue(_R(IDC_EDITROTATEANGLE), 0, MAX_ANGLE_DEG, _R(IDS_BRUSHANGLE_INVALID), &Valid );
			if (! Valid || DoubleNewVal < 0 || DoubleNewVal > MAX_ANGLE_DEG)
			{
				//InformWarning(_R(IDS_BRUSHANGLE_INVALID));
				UpdateEditBox(_R(IDC_EDITROTATEANGLE));
			}
			else
			{
				if (EditData->m_RotateAngle != DoubleNewVal)
				{
					EditData->m_RotateAngle = DoubleNewVal;
					ChangeType_m = CHANGEBRUSH_ROTATE_ANGLE;
					ChangeParam_m.m_NewRotateAngle = DoubleNewVal;
					ReInitBrushEffects();
				}
			}
		break;
		case _R(IDC_EDITROTATEANGLEINCR):
			DoubleNewVal = GetDoubleGadgetValue(_R(IDC_EDITROTATEANGLEINCR), MIN_ANGLE_DEG , MAX_ANGLE_DEG, _R(IDS_BRUSHANGLE_INVALID), &Valid);
			if (!Valid || DoubleNewVal < MIN_ANGLE_DEG || DoubleNewVal > MAX_ANGLE_DEG)
			{
				//InformWarning(_R(IDS_BRUSHANGLE_INVALID));
				UpdateEditBox(_R(IDC_EDITROTATEANGLEINCR));
			}
			else 
			{
				if (EditData->m_RotAngleIncrConst != DoubleNewVal)
				{
					EditData->m_RotAngleIncrConst = DoubleNewVal ;
					ChangeType_m = CHANGEBRUSH_ROTATE_INCRCONST;
					ChangeParam_m.m_NewRotationIncrConst = DoubleNewVal ;
					ReInitBrushEffects();
				}
			}
			break;
		case _R(IDC_EDITBRUSHTRANSP):
			LongNewVal = GetLongGadgetValue(_R(IDC_EDITBRUSHTRANSP), MIN_TRANSP_SLIDER-100 , MAX_TRANSP_SLIDER-100, _R(IDS_BRUSHTRANSP_INVALID), &Valid);
			if (Valid)
			{
				if (EditData->m_BrushTransparency != LongNewVal + 100)
				{
					EditData->m_BrushTransparency = LongNewVal + 100;
					ChangeType_m = CHANGEBRUSH_TRANSP;
					ChangeParam_m.m_NewTransp = EditData->m_BrushTransparency;
					ReInitBrushSequence();
				}
				
					
			}
			UpdateEditBox(_R(IDC_EDITBRUSHTRANSP));
			break;

	}
}

void CBrushEditDlg::ReadComboBox (DialogMsg* Msg)
{
//	SequenceType NewSeqType;
//	PathOffset NewOffsetType;
	WORD Index = -1;
	switch (Msg->GadgetID)
	{
		case _R(IDC_COMBOPATHOFFSETTYPE):
			GetValueIndex(_R(IDC_COMBOPATHOFFSETTYPE), &Index);
			UsingRandomOffset = FALSE;
			
			switch (Index)
			{
				case -1:
					ERROR3("Invalid index");
				break;

				case OFFSET_NONE:
					EditData->m_PathOffsetType = OFFSET_NONE;
					if (EditData->m_OffsetValueMaxRand == 0)
						ChangeParam_m.m_NewPathOffsetType = OFFSET_NONE;
					else
						ChangeParam_m.m_NewPathOffsetType = OFFSET_RANDOM;
			
					ChangeType_m = CHANGEBRUSH_OFFSET_TYPE;
					ReInitBrushOffset ();
				break;

				case OFFSET_ALTERNATE:
					EditData->m_PathOffsetType = OFFSET_ALTERNATE;
					ChangeType_m = CHANGEBRUSH_OFFSET_TYPE;
					ChangeParam_m.m_NewPathOffsetType = OFFSET_ALTERNATE;
					ReInitBrushOffset ();
				break;

				case OFFSET_LEFT:
					EditData->m_PathOffsetType = OFFSET_LEFT;
					ChangeType_m = CHANGEBRUSH_OFFSET_TYPE;
					ChangeParam_m.m_NewPathOffsetType = OFFSET_LEFT;
					ReInitBrushOffset ();
				break;

				case OFFSET_RIGHT:
					EditData->m_PathOffsetType = OFFSET_RIGHT;
					ChangeType_m = CHANGEBRUSH_OFFSET_TYPE;
					ChangeParam_m.m_NewPathOffsetType = OFFSET_RIGHT;
					ReInitBrushOffset ();
				break;

				case OFFSET_RANDOM:
					UsingRandomOffset = TRUE;
					EditData->m_PathOffsetType = OFFSET_RANDOM;
					ChangeType_m = CHANGEBRUSH_OFFSET_TYPE;
					ChangeParam_m.m_NewPathOffsetType = OFFSET_RANDOM;
					ReInitBrushOffset ();
				break;

				default:
					ERROR3("Unrecognised offset type");
					ChangeType_m = CHANGEBRUSH_NONE;
				break;
			}
		break;

		case _R(IDC_COMBOSEQUENCETYPE):
		{
			GetValueIndex(_R(IDC_COMBOSEQUENCETYPE), &Index);
			switch(Index)
			{
				case SEQ_FORWARD:
					EditData->m_SequenceType = SEQ_FORWARD;
					ChangeType_m = CHANGEBRUSH_SEQUENCE;
					ChangeParam_m.m_NewSequenceType = SEQ_FORWARD;
				break;

				case SEQ_BACKWARD:
					EditData->m_SequenceType = SEQ_BACKWARD;
					ChangeType_m = CHANGEBRUSH_SEQUENCE;
					ChangeParam_m.m_NewSequenceType = SEQ_BACKWARD;
				break;

				case SEQ_MIRROR:
					EditData->m_SequenceType = SEQ_MIRROR;
					ChangeType_m = CHANGEBRUSH_SEQUENCE;
					ChangeParam_m.m_NewSequenceType = SEQ_MIRROR;
				break;

				case SEQ_RANDOM:
					EditData->m_SequenceType = SEQ_RANDOM;
					ChangeType_m = CHANGEBRUSH_SEQUENCE;
					ChangeParam_m.m_NewSequenceType = SEQ_RANDOM;
				break;

				default:
					ERROR3("Unrecognised sequence type");
					ChangeType_m = CHANGEBRUSH_NONE;
				break;
			}
		ReInitBrushSpacing();
		}
		break;

	default:
		ChangeType_m = CHANGEBRUSH_NONE;
	break;
	}
}

void CBrushEditDlg::ReadCheckBox (DialogMsg* Msg)
{	
//	BOOL Checked;

	switch (Msg->GadgetID)
	{
		case _R(IDC_CHECKTILEING):
			EditData->m_bTile = !(EditData->m_bTile);
			//GetBoolGadgetSelected (_R(IDC_CHECKTILEING),0, &Checked);
			//EditData->m_bTile = Checked;
			ChangeType_m = CHANGEBRUSH_TILED;
			ChangeParam_m.m_bNewTiled = EditData->m_bTile;//Checked;
			ReInitBrushFill();
		break;

		case _R(IDC_CHECKLOCALFILLCOLOUR):
			//GetBoolGadgetSelected (_R(IDC_CHECKLOCALFILLCOLOUR),0, &Checked);
			EditData->m_bUseLocalFillColour = !(EditData->m_bUseLocalFillColour);//Checked;
			ChangeType_m = CHANGEBRUSH_USELOCALFILLCOL;
			ChangeParam_m.m_bNewUseLocalFillColour = EditData->m_bUseLocalFillColour;//Checked;
			ReInitBrushFill();
		break;

		case _R(IDC_CHECKOVERRIDENAMED):
			EditData->m_bUseNamedColour = !(EditData->m_bUseNamedColour);
			ChangeType_m = CHANGEBRUSH_USENAMEDCOL;
			ChangeParam_m.m_bNewUseNamed = EditData->m_bUseNamedColour;
			ReInitBrushFill();
		break;

		case _R(IDC_CHECKROTATE):
			//GetBoolGadgetSelected (_R(IDC_CHECKROTATE),0, &Checked);
			EditData->m_bRotate = !(EditData->m_bRotate);//Checked;
			ChangeType_m = CHANGEBRUSH_TANGENTIAL;
			ChangeParam_m.m_bNewRotated = EditData->m_bRotate;//Checked;
		//	ReInitBrushEffects ();

			/*if (!(EditData->m_bRotate))
			{
				EditData->m_RotateAngle = DefaultRotationAngle;
			}*/
		break;

		default:
			//ERROR3 ("Unrecognised gadget"); // not necessarily an error, as it may have been a button
			ChangeType_m = CHANGEBRUSH_NONE;
		break;
	}
	
}

void CBrushEditDlg::ReadRadios(DialogMsg* Msg)
{
	if (Msg == NULL)
		return;

	switch (Msg->GadgetID)
	{
		case _R(IDC_RADIOLOCALFILLALL):
		{
			if (EditData->m_bUseLocalFillColour == FALSE)
			{
				EditData->m_bUseLocalFillColour = TRUE;
				// we need to set this to true to indicate that we are not overriding only named
				// colours, the semantics are a little off i'm afraid
				EditData->m_bUseNamedColour = TRUE;
				ChangeParam_m.m_bNewUseLocalFillColour = TRUE;
				ChangeType_m = CHANGEBRUSH_USELOCALFILLCOL;
			}

		}
		break;
		case _R(IDC_RADIOLOCALFILLNAMED):
		{
			// potentially a bit confusing this: if UseNamed == TRUE then we do not use
			// any of the local colours, if UseNamed == FALSE then we do
			if (EditData->m_bUseNamedColour == TRUE)
			{
				EditData->m_bUseNamedColour = FALSE;
				EditData->m_bUseLocalFillColour = FALSE;
				ChangeParam_m.m_bNewUseNamed = FALSE;
				ChangeType_m = CHANGEBRUSH_USENAMEDCOL;
			}

		}
		break;
		case _R(IDC_RADIOLOCALFILLNONE):
		{
			// find out which local option was selected and turn it off
			if (EditData->m_bUseLocalFillColour)
			{
				EditData->m_bUseLocalFillColour = FALSE;
				ChangeParam_m.m_bNewUseLocalFillColour = FALSE;
				ChangeType_m = CHANGEBRUSH_USELOCALFILLCOL;
			}
	
			if (EditData->m_bUseNamedColour == FALSE)
			{
				EditData->m_bUseNamedColour =	TRUE;
				ChangeParam_m.m_bNewUseNamed = TRUE;
				ChangeType_m = CHANGEBRUSH_USENAMEDCOL;
			}
			
		}
		break;

		default:
		break;
	}
}

void CBrushEditDlg::ReadSlider (DialogMsg* Msg)
{
	BOOL Valid;
	INT32 Result;
	
	ChangeType_m = CHANGEBRUSH_NONE; // initialise our op type to none
	
	switch (Msg->GadgetID)
	{
		case _R(IDC_SLIDERBRUSHSPACING1):
		{
			double MinSpacingSlider = 0;
			double MaxSpacingSlider = 0;

			GetSpacingRange(&MinSpacingSlider, &MaxSpacingSlider);

			INT32 IntMin = (INT32)(MinSpacingSlider);
			INT32 IntMax = (INT32)(MaxSpacingSlider);

			Result = GetLongGadgetValue (_R(IDC_SLIDERBRUSHSPACING1), IntMin, IntMax, 0, &Valid);
			Result = (IntMax - Result) + IntMin;
			
			if (Valid)
			{
				double ExpVal = pow(Result, ExpFactor);
				MILLIPOINT NewSpacing = (MILLIPOINT)ExpVal;
		
				EditData->m_BrushSpacing = NewSpacing;
				ChangeParam_m.m_NewSpacing = NewSpacing;
				ChangeType_m = CHANGEBRUSH_SPACING;
				SetSpacingSlider();
			
			
			}
		}
		break;
		case _R(IDC_SLIDERBRUSHSPACINGINCR):
		{
			double dblResult = GetDoubleGadgetValue(_R(IDC_SLIDERBRUSHSPACINGINCR), MIN_SPACING_INCR, MAX_SPACING_INCR, 0, &Valid);
			dblResult = (MAX_SPACING_INCR - dblResult) + MIN_SPACING_INCR;
			if (Valid)
			{
				// we need to convert the % value we got from the slider into millipoints
				MILLIPOINT Distance = GetPercentageAsDistance(dblResult, WIDTH);
		
				EditData->m_BrushSpacingIncrConst = Distance;
				ChangeParam_m.m_NewSpacingIncrConst = Distance;
				ChangeType_m = CHANGEBRUSH_SPACING_INCRCONST;
				UpdateEditBox(_R(IDC_EDITBRUSHSPACINGINCRCONST));
			
			}
		}			
		break;
		case _R(IDC_SLIDERBRUSHSPACINGMAXRAND):
		{
			Result = GetLongGadgetValue (_R(IDC_SLIDERBRUSHSPACINGMAXRAND), MIN_RANDOM_SLIDER, MAX_RANDOM_SLIDER, 0, &Valid);
			Result = MAX_RANDOM_SLIDER - Result;

			if (Valid)
			{
				EditData->m_BrushSpacingMaxRand = Result;
				
				if (!UsingRandomSpacing)
				{
					if (Result != MIN_RANDOM_SLIDER)
					{
						ReInitBrushSpacing ();
					}
				}
				else if (Result == MIN_RANDOM_SLIDER)
				{
					ReInitBrushSpacing ();
				}
				ChangeType_m  = CHANGEBRUSH_SPACING_MAXRAND;
				ChangeParam_m.m_NewSpacingMaxRand = Result;
				SetRandomRangeText(_R(IDC_SLIDERBRUSHSPACINGMAXRAND));
			}
		}
		break;
		case _R(IDC_SLIDERPATHOFFSETVAL):
		{
			double MinOffset = 0;
			double MaxOffset = 0;

			GetOffsetRange(&MinOffset, &MaxOffset);

			INT32 lMinOffset  = (INT32)MinOffset;
			INT32 lMaxOffset  = (INT32)MaxOffset;

			Result = GetLongGadgetValue(_R(IDC_SLIDERPATHOFFSETVAL), lMinOffset, lMaxOffset, 0, &Valid);
			Result = (lMaxOffset - Result) + lMinOffset;

			if (Valid)
			{
				double ExpVal = pow(Result, ExpFactor);
				MILLIPOINT NewOffset = (MILLIPOINT)ExpVal;
			
				EditData->m_PathOffsetValue = NewOffset;
				ChangeParam_m.m_NewPathOffsetVal = NewOffset;
				ChangeType_m = CHANGEBRUSH_OFFSET_VAL;
				ReInitBrushOffset();
				
			}
			
		}
		break;
		case _R(IDC_SLIDEROFFSETINCRCONST):
		{
			double NewIncr = GetDoubleGadgetValue(_R(IDC_SLIDEROFFSETINCRCONST), MIN_SPACING_INCR, MAX_SPACING_INCR, 0, &Valid);
			
			if (Valid)
			{
				NewIncr = (MAX_SPACING_INCR - NewIncr) + MIN_SPACING_INCR;
				MILLIPOINT Incr = GetPercentageAsDistance(NewIncr, HEIGHT);
				if (abs(EditData->m_PathOffsetIncrConst - Incr) > 20)
				{
					EditData->m_PathOffsetIncrConst = Incr;
					ChangeParam_m.m_NewOffsetIncrConst = Incr;
					ChangeType_m = CHANGEBRUSH_OFFSET_INCRCONST;
					UpdateEditBox(_R(IDC_EDITPATHOFFSETINCRCONST));
				}
			}
		}
		break;
	
		case _R(IDC_SLIDEROFFSETVALUEMAXRAND):
		{
			Result = GetLongGadgetValue (_R(IDC_SLIDEROFFSETVALUEMAXRAND), MinOffsetSlider, MaxOffsetSlider, 0, &Valid);
			Result = MaxOffsetSlider - Result;

			if (Valid)
			{
				EditData->m_OffsetValueMaxRand = Result;
			
				// if this is the first movement of the slider and our offset type is offset_none then
				// surreptitiously change this to offset_random
				if (EditData->m_PathOffsetType == OFFSET_NONE)
				{
					EditData->m_PathOffsetType = OFFSET_RANDOM;
					ChangeParam_m.m_NewPathOffsetType = OFFSET_RANDOM;
					ChangeType_m = CHANGEBRUSH_ALL;
				}
				else
					ChangeType_m = CHANGEBRUSH_OFFSET_MAXRAND;
				
				ChangeParam_m.m_NewOffsetValMaxRand = Result;
			}
			ReInitBrushOffset();
		}
		break;
		case _R(IDC_SLIDERBRUSHSCALINGMAXRAND):
		{
			Result = GetLongGadgetValue (_R(IDC_SLIDERBRUSHSCALINGMAXRAND), MinScalingSlider, MaxScalingSlider, 0, &Valid);
			Result = MaxScalingSlider - Result;

			if (Valid)
			{
				EditData->m_BrushScalingMaxRand = Result;

				if (!UsingRandomScaling)
				{
					if (Result != MinScalingSlider)
					{
						ReInitBrushScaling ();
					}
				}
				else if (Result == MinScalingSlider)
				{
					ReInitBrushScaling ();
				}
				SetRandomRangeText(_R(IDC_SLIDERBRUSHSCALINGMAXRAND));
				ChangeType_m = CHANGEBRUSH_SCALING_MAXRAND;
				ChangeParam_m.m_NewScalingMaxRand = Result;
			}
		}
		break;
		case _R(IDC_SLIDERROTATEANGLE):
		{
			double NewAngle = GetDoubleGadgetValue(_R(IDC_SLIDERROTATEANGLE), MIN_ROTATION_SLIDER, MAX_ROTATION_SLIDER, 0, &Valid);
			if (Valid)
			{
				NewAngle = (MAX_ROTATION_SLIDER - NewAngle) + MIN_ROTATION_SLIDER;
			
				// make it a whole number
				INT32 AngleRounded = (INT32)NewAngle;
				ChangeParam_m.m_NewRotateAngle = (double)AngleRounded;
				ChangeType_m = CHANGEBRUSH_ROTATE_ANGLE;
				EditData->m_RotateAngle = ChangeParam_m.m_NewRotateAngle;
				ReInitBrushEffects();
					
				
			}
		}
		break;
		case _R(IDC_SLIDERROTATEANGLEINCR):
		{
			double NewIncr = GetDoubleGadgetValue(_R(IDC_SLIDERROTATEANGLEINCR), MIN_ROTATION_INCR_SLIDER, MAX_ROTATION_INCR_SLIDER,
												  0, &Valid);
			if (Valid)
			{
				NewIncr = (MAX_ROTATION_INCR_SLIDER - NewIncr) + MIN_ROTATION_INCR_SLIDER;
			
				INT32 IncrRounded = (INT32)NewIncr;
				EditData->m_RotAngleIncrConst  =(double)IncrRounded;
				ChangeParam_m.m_NewRotationIncrConst = EditData->m_RotAngleIncrConst;
				ChangeType_m = CHANGEBRUSH_ROTATE_INCRCONST;
				ReInitBrushEffects();
				
			}
		}
		break;
		case _R(IDC_SLIDERROTATEANGLEMAXRAND):
		{
			Result = GetLongGadgetValue (_R(IDC_SLIDERROTATEANGLEMAXRAND), MIN_RANDOM_SLIDER, MAX_ROTATION_RANDOM_SLIDER, 0, &Valid);
			Result = (MAX_ROTATION_RANDOM_SLIDER - Result) + MIN_RANDOM_SLIDER;
			//TRACEUSER( "Diccon", _T("Rotation max rand = %d\n"), Result);
			if (Valid)
			{
				EditData->m_RotationMaxRand = Result;	
				ReInitBrushEffects ();
			
				ChangeType_m = CHANGEBRUSH_ROTATE_MAXRAND;
				ChangeParam_m.m_NewRotateMaxRand = Result;
				SetRandomRangeText(_R(IDC_SLIDERROTATEANGLEMAXRAND));
			}
		}
		break;
		case _R(IDC_SLIDERROTATEINCR):
		{
			Result = GetLongGadgetValue(_R(IDC_SLIDERROTATEINCR), MinMultiSlider, MaxMultiSlider, 0, &Valid);
			Result = (MaxMultiSlider - Result) + MinMultiSlider;
			if (Valid)
			{
			
				EditData->m_RotAngleIncrProp = (double)((double)Result / 100);
				ReInitBrushEffects();
				ChangeType_m = CHANGEBRUSH_ROTATE_INCRPROP;
				ChangeParam_m.m_NewRotationIncrProp = EditData->m_RotAngleIncrProp;
				
			}
		}
		break;
	
		case _R(IDC_SLIDERBRUSHSCALINGINCR):
		{
			Result = GetLongGadgetValue (_R(IDC_SLIDERBRUSHSCALINGINCR), MinMultiSlider, MaxMultiSlider, 0, &Valid);
			Result = (MaxMultiSlider - Result) + MinMultiSlider;
			if (Valid)
			{
			
				EditData->m_BrushScalingIncr = (double)Result / 100;
				ReInitBrushScaling();
				ChangeType_m = CHANGEBRUSH_SCALING_INCR;
				ChangeParam_m.m_NewScalingIncr = EditData->m_BrushScalingIncr;
				
			}
		}
		break;
		case _R(IDC_SLIDERSCALINGINCRCONST):
		{
			Result = GetLongGadgetValue(_R(IDC_SLIDERSCALINGINCRCONST), MinScalingIncrSlider, MaxScalingIncrSlider, 0, &Valid);
			Result = (MaxScalingIncrSlider - Result) + MinScalingIncrSlider;
			if (Valid)
			{
				if (EditData->m_BrushScalingIncrConst != Result)
				{
					EditData->m_BrushScalingIncrConst = (double)Result;
					ReInitBrushScaling();
					ChangeType_m = CHANGEBRUSH_SCALING_INCRCONST;
					ChangeParam_m.m_NewScalingIncrConst = (double)Result;
				}
			}
		}
		break;

		case _R(IDC_SLIDERSCALINGPRESSURE):
		{
			Result = GetLongGadgetValue(_R(IDC_SLIDERSCALINGPRESSURE), MIN_PRESSURE_SLIDER, MAX_PRESSURE_SLIDER, 0, &Valid);
			if (Valid)
			{
				Result = (MAX_PRESSURE_SLIDER - Result) + MIN_PRESSURE_SLIDER;
		
				EditData->m_ScalingMaxPressure = Result;
				ReInitBrushScaling();
				ChangeType_m = CHANGEBRUSH_SCALING_PRESSURE;
				ChangeParam_m.m_NewScalingMaxPressure = Result;
				SetRandomRangeText(_R(IDC_SLIDERSCALINGPRESSURE));
				
			}
		}
		break;
		
		case _R(IDC_SLIDERSATURATIONRANDOM):
		{
			Result = GetLongGadgetValue(_R(IDC_SLIDERSATURATIONRANDOM), MIN_RANDOM_SLIDER, MAX_RANDOM_SAT_SLIDER, 0, &Valid);
			if (Valid)
			{
				Result = (MAX_RANDOM_SAT_SLIDER - Result) + MIN_RANDOM_SLIDER;
		
				EditData->m_BrushSatMaxRand = Result;
				ChangeType_m = CHANGEBRUSH_SAT_MAXRAND;
				ChangeParam_m.m_NewSatMaxRand = Result;
				SetRandomRangeText(_R(IDC_SLIDERSATURATIONRANDOM));
				
				ReInitBrushFill();
			}
		}
		break;
		case _R(IDC_SLIDERHUERANDOM):
		{
			Result = GetLongGadgetValue(_R(IDC_SLIDERHUERANDOM), MIN_RANDOM_SLIDER, MAX_RANDOM_HUE_SLIDER, 0, &Valid);
			if (Valid)
			{
				Result = (MAX_RANDOM_HUE_SLIDER - Result) + MIN_RANDOM_SLIDER;
		
				EditData->m_BrushHueMaxRand = Result;
				ChangeType_m = CHANGEBRUSH_HUE_MAXRAND;
				ChangeParam_m.m_NewHueMaxRand = Result;
				SetRandomRangeText(_R(IDC_SLIDERHUERANDOM));
				
				ReInitBrushFill();
			}
		}
		break;
		case _R(IDC_SLIDERTRANSP):
		{
			Result = GetLongGadgetValue(_R(IDC_SLIDERTRANSP), MIN_TRANSP_SLIDER, MAX_TRANSP_SLIDER, 0, &Valid);
			if (Valid)
			{
				Result = (MAX_TRANSP_SLIDER - Result) + MIN_TRANSP_SLIDER;
		
				EditData->m_BrushTransparency = Result;
				ChangeType_m = CHANGEBRUSH_TRANSP;
				ChangeParam_m.m_NewTransp = Result;
				UpdateEditBox(_R(IDC_EDITBRUSHTRANSP));
				
			}
		}
		break;
		case _R(IDC_SLIDERTRANSPPRESSURE):
		{
			Result = GetLongGadgetValue(_R(IDC_SLIDERTRANSPPRESSURE), MIN_PRESSURE_SLIDER, MAX_PRESSURE_SLIDER, 0, &Valid);
			if (Valid)
			{
				Result = (MAX_PRESSURE_SLIDER - Result) + MIN_PRESSURE_SLIDER;
			
				EditData->m_TranspMaxPressure = Result;
				ChangeType_m = CHANGEBRUSH_TRANSP_PRESSURE;
				ChangeParam_m.m_NewTranspPressure = Result;
				ReInitBrushSequence();
				
			}
		}
		break;
	}
}


void CBrushEditDlg::ReadButtons(DialogMsg* Msg)
{

	switch (Msg->GadgetID)
	{
		case _R(IDC_BUTTONBRUSHSCALINGRANDSEED):
			ChangeType_m = CHANGEBRUSH_SCALING_RANDSEED;
			ChangeParam_m.m_NewScalingRandSeed = GetNewRandomNumber(0);
			EditData->m_BrushScalingRandSeed = ChangeParam_m.m_NewScalingRandSeed;
		break;

		case _R(IDC_BUTTONBRUSHSPACINGRANDSEED):
			ChangeType_m = CHANGEBRUSH_SPACING_RANDSEED;
			ChangeParam_m.m_NewSpacingRandSeed = GetNewRandomNumber(0);
			EditData->m_BrushSpacingRandSeed = ChangeParam_m.m_NewSpacingRandSeed;
			
			// We're changing sequence seed too
			ChangeParam_m.m_NewSequenceRandSeed = EditData->m_BrushSpacingRandSeed;
			EditData->m_SequenceRandSeed = ChangeParam_m.m_NewSequenceRandSeed;
		break;

		case _R(IDC_BUTTONOFFSETVALUERANDSEED):
			// bit of a change here, we only want to have one random button on the offset tab 
			// so this button now changes seeds for both offset value and offset type

			ChangeType_m = CHANGEBRUSH_OFFSET_SEEDS;
			ChangeParam_m.m_NewOffsetValRandSeed = GetNewRandomNumber(0);
			ChangeParam_m.m_NewOffsetTypeRandSeed = GetNewRandomNumber(0);
			EditData->m_OffsetValueRandSeed = ChangeParam_m.m_NewOffsetValRandSeed;
			EditData->m_OffsetTypeRandSeed = ChangeParam_m.m_NewOffsetTypeRandSeed;
		break;

		case _R(IDC_BUTTONBRUSHSEQUENCERANDSEED):
			ChangeType_m = CHANGEBRUSH_SEQUENCE_RANDSEED;
			ChangeParam_m.m_NewSequenceRandSeed = GetNewRandomNumber(0);
			EditData->m_SequenceRandSeed = ChangeParam_m.m_NewSequenceRandSeed;
		break;
		case _R(IDC_BUTTONROTATEANGLERANDSEED):
			ChangeType_m = CHANGEBRUSH_ROTATE_RANDSEED;
			ChangeParam_m.m_NewRotateRandSeed = GetNewRandomNumber(0);
			EditData->m_RotationRandSeed = ChangeParam_m.m_NewRotateRandSeed;
		break;
		case _R(IDC_BUTTONBRUSHFILLRANDOM):
			ChangeType_m = CHANGEBRUSH_FILL_SEEDS;
			ChangeParam_m.m_NewHueRandSeed = GetNewRandomNumber(0);
			ChangeParam_m.m_NewSatRandSeed = GetNewRandomNumber(0);
			EditData->m_BrushHueRandSeed = ChangeParam_m.m_NewHueRandSeed;
			EditData->m_BrushSatRandSeed = ChangeParam_m.m_NewSatRandSeed;
		break;
	
		default: //not necessarily an error
			break;
	}
}

void CBrushEditDlg::WriteSliders (DialogMsg* Msg)
{

}

void CBrushEditDlg::UpdateEditBox (CGadgetID GadgetToUpdate)
{	
	switch (GadgetToUpdate)
	{
		case _R(IDC_EDITBRUSHSPACING):
		{
			// work out the current actual spacing value
			double dSpacingValue = (double)EditData->m_BrushSpacing; // * EditData->m_BrushScaling;
			MILLIPOINT lSpacingValue = dSpacingValue;
			dSpacingValue = pow(dSpacingValue, 1/ExpFactor);
			INT32 iSpacingValue = (INT32)dSpacingValue;
		
			// get the spacing value as a % and set that in the edit field
			double SpacingAsPercent = GetDistanceAsPercentageOfBBox(lSpacingValue, WIDTH);
			SetDoubleGadgetValue (_R(IDC_EDITBRUSHSPACING), SpacingAsPercent);
		
		}
		break;
	/*	case _R(IDC_EDITBRUSHSPACINGINCRCONST):
		{
			// for the edit box we need to set it as a percentage
			double PercentVal = GetDistanceAsPercentageOfBBox(EditData->m_BrushSpacingIncrConst);
			SetDoubleGadgetValue(_R(IDC_EDITBRUSHSPACINGINCRCONST), PercentVal);
		}
		break;
	*/
		case _R(IDC_EDITPATHOFFSETVALUE):
		{
			double PercentVal = GetDistanceAsPercentageOfBBox(EditData->m_PathOffsetValue, HEIGHT);
			SetDoubleGadgetValue(_R(IDC_EDITPATHOFFSETVALUE), PercentVal);
		}
		break;

	/*	case _R(IDC_EDITPATHOFFSETINCRCONST):
		{
			double PercentVal = GetDistanceAsPercentageOfBBox(EditData->m_PathOffsetIncrConst);
			SetDoubleGadgetValue(_R(IDC_EDITPATHOFFSETINCRCONST), PercentVal);
		}		
		break;*/
		case _R(IDC_EDITBRUSHSCALING):
			SetDoubleGadgetValue (_R(IDC_EDITBRUSHSCALING), EditData->m_BrushScaling);
		break;
		case _R(IDC_EDITBRUSHSCALINGINCR):
			SetDoubleGadgetValue (_R(IDC_EDITBRUSHSCALINGINCR), EditData->m_BrushScalingIncr*100);	
		break;
	/*	case _R(IDC_EDITSCALINGINCRCONST):
			SetDoubleGadgetValue (_R(IDC_EDITSCALINGINCRCONST), EditData->m_BrushScalingIncrConst);
		break;
		*/
		case _R(IDC_EDITROTATEANGLEINCR):
			SetDoubleGadgetValue (_R(IDC_EDITROTATEANGLEINCR), EditData->m_RotAngleIncrConst);
		break;
		case _R(IDC_EDITROTATEANGLE):
			SetDoubleGadgetValue (_R(IDC_EDITROTATEANGLE), EditData->m_RotAngleIncrConst);
		break;
		case _R(IDC_EDITBRUSHTRANSP):
			SetLongGadgetValue(_R(IDC_EDITBRUSHTRANSP), EditData->m_BrushTransparency - 100);
		break;
		default:
		break;
	}
}

void CBrushEditDlg::ReInitialiseDialog (BrushData* pData)
{
	if (pData != NULL)					
	{
		if (EditData != NULL)
			delete EditData;
		EditData = new BrushData(*pData);
		if (EditData == NULL)
		{
			ERROR3("Argh we failed to allocate new brush data");
			return;
		}

		// we have to reinit all the pages one by one
		ReInitPage(_R(IDD_BRUSHEDITSPACING));
		ReInitPage(_R(IDD_BRUSHEDITOFFSET));
		ReInitPage(_R(IDD_BRUSHEDITSCALING));
		ReInitPage(_R(IDD_BRUSHEDITEFFECTS));
		ReInitPage(_R(IDD_BRUSHEDITSEQUENCE));
			
	} // end if data
}

void CBrushEditDlg::ReInitPage(CDlgResID PageID)
{
	// tell the page we're talking to it
	TalkToPage(PageID);
	// reinit our active page
	switch (PageID)
	{	
		case _R(IDD_BRUSHEDITSPACING):
			ReInitBrushSpacing();
			break;
		case _R(IDD_BRUSHEDITOFFSET):
			ReInitBrushOffset();
			break;
		case _R(IDD_BRUSHEDITSCALING):
			ReInitBrushScaling();
			break;
		case _R(IDD_BRUSHEDITEFFECTS):
			ReInitBrushEffects();
			break;
		case _R(IDD_BRUSHEDITSEQUENCE):
			ReInitBrushSequence();
			break;
		case _R(IDD_BRUSHEDITFILL):
			break;
		default:
			ERROR3("Message from unknown tab dialog page");
	}
	
}


// looks to see if there is one and only one brush attribute in the selection and returns it
AttrBrushType* CBrushEditDlg::GetSingleSelectedBrushAttribute()
{
	AttrBrushType* pAttrBrush = NULL;
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(AttrBrushType));
	BrushData Data; // initialises to default values
	INT32 NumItems = NodeList.GetCount();
	if (NumItems == 1)
	{
		NodeListItem* pItem = (NodeListItem*)NodeList.GetHead();
		if (pItem != NULL)
			pAttrBrush = (AttrBrushType*)pItem->pNode;
	}
	NodeList.DeleteAll();
	return pAttrBrush;
}

// deals with a selection change, essentially if we change from one brush to another
// then we refresh the dialog, if we change to no-brush then we disable everything
void CBrushEditDlg::HandleDocSelectionChange()
{
	AttrBrushType* pAttrBrush = GetSingleSelectedBrushAttribute();
	if (pAttrBrush == NULL)
	{
		m_bDisableGadgets = TRUE;
		String_32 Title = TEXT("");
		SetDialogTitle(Title);

		RefreshActivePage();
		return;
	}

	// lets get the data from the attribute and into our members
	if (EditData == NULL)
		EditData = new BrushData;

	if (EditData == NULL)
		ERROR3("Unable to allocate brush data in CBrushEditDlg::HandleDocSelectionChange");
	
	*EditData = pAttrBrush->GetBrushData();

	if (OriginalData == NULL)
		OriginalData = new BrushData;
	
	if (OriginalData == NULL)
		ERROR3("Unable to allocate brush data in CBrushEditDlg::HandleDocSelectionChange");

	*OriginalData = *EditData;
	m_LastData = *EditData;
	m_bDisableGadgets = FALSE;
	
	// set the name of the new brush
	SetDialogTitle(EditData->m_Name);
	
	//initialise our opparam with the original data, just in case we want to do change all
	ChangeParam_m.SetValues(*EditData);
	
	EditingBrushDef = FALSE;
	// now refresh the page that is currently active
	RefreshActivePage();
	
}

// finds out which page is currently active and ReInits it
void CBrushEditDlg::RefreshActivePage()
{
	CDlgResID ActivePageID = GetCurrentPageID();

	switch (ActivePageID)
	{
		case _R(IDD_BRUSHEDITSPACING):
			ReInitBrushSpacing();
			break;
		case _R(IDD_BRUSHEDITOFFSET):
			ReInitBrushOffset();
			break;
		case _R(IDD_BRUSHEDITSCALING):
			ReInitBrushScaling();
			break;
		case _R(IDD_BRUSHEDITEFFECTS):
			ReInitBrushEffects();
			break;
		case _R(IDD_BRUSHEDITSEQUENCE):
			ReInitBrushSequence();
		break;
		case _R(IDD_BRUSHEDITFILL):
			ReInitBrushFill();
		break;
		default: // maybe not an error
			break;
	}

}

// using settitlebarname has no effect as it gets the page window rather than 
// the property sheet, so use this instead
void CBrushEditDlg::SetDialogTitle(String_256 Title)
{
	// first get the window
	CWnd* pCWnd = CWnd::FromHandlePermanent(WindowID);
	
	if (pCWnd == NULL)
	{
		ERROR3("This is not a property sheet, the PageID should be NULL"); 
		return;
	}
	
	// Ok so Win must be a property sheet, let's make sure
	ERROR3IF(!(pCWnd->IsKindOf(RUNTIME_CLASS(OurPropSheet))), "Don't know what this window is");

	TCHAR* pName = Title.operator TCHAR*(); // cast the string to something MFC likes

	OurPropSheet* pPropertySheet = (OurPropSheet*)pCWnd;
	pPropertySheet->SetTitle(pName); 
}

// sets up the brush spacing slider to have the correct values etc.
void CBrushEditDlg::SetSpacingSlider()
{
	// first work out the maximum and minimum values for the spacing slider
	double MinSpacingSlider = 0; double MaxSpacingSlider = 0;

	GetSpacingRange(&MinSpacingSlider, &MaxSpacingSlider);

	INT32 IntMin = (INT32)(MinSpacingSlider);
	INT32 IntMax = (INT32)(MaxSpacingSlider);

	// set the range
	SetGadgetRange (_R(IDC_SLIDERBRUSHSPACING1), IntMin, IntMax);
	SetGadgetBitmaps( _R(IDC_SLIDERBRUSHSPACING1), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));

	// work out the current actual spacing value
	double dSpacingValue = (double)EditData->m_BrushSpacing; // * EditData->m_BrushScaling;
	dSpacingValue = pow(dSpacingValue, 1/ExpFactor);
	INT32 iSpacingValue = (INT32)dSpacingValue;
	SetLongGadgetValue (_R(IDC_SLIDERBRUSHSPACING1), (IntMax - iSpacingValue) + IntMin);
	
	UpdateEditBox(_R(IDC_EDITBRUSHSPACING));
}

void CBrushEditDlg::SetSpacingIncrSlider()
{
	SetGadgetRange(_R(IDC_SLIDERBRUSHSPACINGINCR), MIN_SPACING_INCR, MAX_SPACING_INCR);
	SetGadgetBitmaps (_R(IDC_SLIDERBRUSHSPACINGINCR), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));

	double PercentVal = GetDistanceAsPercentageOfBBox(EditData->m_BrushSpacingIncrConst, WIDTH);
	SetDoubleGadgetValue(_R(IDC_SLIDERBRUSHSPACINGINCR), (MAX_SPACING_INCR - PercentVal) + MIN_SPACING_INCR);
}


void CBrushEditDlg::SetOffsetValSlider()
{
	double MinSlider = 0; double MaxSlider = 0;

	GetOffsetRange(&MinSlider, &MaxSlider);

	INT32 IntMin = (INT32)MinSlider;
	INT32 IntMax = (INT32)MaxSlider;

	SetGadgetRange(_R(IDC_SLIDERPATHOFFSETVAL), IntMin, IntMax);
	SetGadgetBitmaps (_R(IDC_SLIDERPATHOFFSETVAL), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));

	double dOffsetVal = pow((double)EditData->m_PathOffsetValue, InvFactor);
	INT32 iOffsetVal = (INT32)dOffsetVal;

	SetLongGadgetValue(_R(IDC_SLIDERPATHOFFSETVAL), (IntMax - iOffsetVal) + IntMin);

	UpdateEditBox(_R(IDC_EDITPATHOFFSETVALUE));
}




// retrieves the range to set the spacing slider. This is calculated by finding the logarithm
// of the minimum spacing and 5x the bounding box size of the object
void CBrushEditDlg::GetSpacingRange(double* pMinimum, double* pMaximum)
{
	if (pMinimum == NULL || pMaximum == NULL)
	{
		ERROR3("Null pointers in CBrushEditDlg::GetSpacingRange");
		return;
	}

	*pMinimum = MinSliderSpacing;

	MILLIPOINT Width = GetSideOfBoundingBox(WIDTH);
	if (Width == 0)
	{
		ERROR3("Bounding rect is empty in CBrushEditDlg::GetSpacingRange");
		return;
	}

	Width = Width * MAXSPACINGMULTIPLIER;

	if (Width> MAX_BRUSH_SPACING)
		Width = MAX_BRUSH_SPACING;

	double ExpRootWidth = pow((double)Width, 1/ExpFactor);
	
	*pMaximum = ExpRootWidth;
}

// as above but for offsets
void CBrushEditDlg::GetOffsetRange(double* pMin, double* pMax)
{
	if (pMin == NULL || pMax == NULL)
	{
		ERROR3("Null pointers in CBrushEditDlg::GetOffsetRange");
		return;
	}

	*pMin = MinOffset;

	MILLIPOINT Height = GetSideOfBoundingBox(HEIGHT);
	if (Height == 0)
	{
	//	ERROR3("Bounding rect is empty in CBrushEditDlg::GetSpacingRange");
		return;
	}

	Height = Height * MAXOFFSETMULTIPLIER;

	if (Height > MAX_BRUSH_OFFSET)
		Height = MAX_BRUSH_OFFSET;

	double ExpRootHeight = pow((double)Height, 1/ExpFactor);
	
	*pMax = ExpRootHeight;
}

// pretty much what the title says
double CBrushEditDlg::GetDistanceAsPercentageOfBBox(MILLIPOINT Distance, RectSideInfo Side)
{
	MILLIPOINT RectSide = GetSideOfBoundingBox(Side);
	if (RectSide == 0)
	{
//		ERROR3("Bounding rect is empty in CBrushEditDlg::GetSpacingRange");
		return 0;
	}

	double Percent = (double)Distance / (double)RectSide;
	return Percent * 100;
}

MILLIPOINT CBrushEditDlg::GetSideOfBoundingBox(RectSideInfo RectSide)
{
	BrushDefinition* pBrushDef = BrushComponent::FindDefinition(EditData->m_BrushHandle);
	if (pBrushDef == NULL)
	{
	//	ERROR3("No brush definition in CBrushEditDlg::GetLongestSideOfBoundingBox");
		return 0;
	}
	
	DocRect BRect = pBrushDef->GetLargestBoundingBox();
	if (BRect.IsEmpty())
	{
		//ERROR3("Bounding rect is empty in CBrushEditDlg::GetSpacingRange");
		return 0;
	}

	MILLIPOINT Side = 0;
	switch (RectSide)
	{
		case WIDTH:
			Side = BRect.Width();
			break;
		case HEIGHT:
			Side = BRect.Height();
			break;
		case LONGEST:
			Side = BRect.Width() > BRect.Height() ? BRect.Width() : BRect.Height();
			break;
		default:
			ERROR3("Invalid side info passed to CBrushEditDlg::GetSideOfBoundingBox");
			Side = BRect.Width();
			break;
	}

	return Side;
}

MILLIPOINT CBrushEditDlg::GetPercentageAsDistance(double Percent, RectSideInfo RectSide)
{
	MILLIPOINT Side = GetSideOfBoundingBox(RectSide);

	MILLIPOINT ReturnVal = (double)Side * (Percent / 100);
	return ReturnVal;
}


// returns the text showing the range of random values for the current slider setting
// Note that SliderVal should be the value adjusted for position, i.e. the value that 
String_32 CBrushEditDlg::GetRandomRangeText(INT32 SliderVal)
{
	String_32 RangeText(" ");
	if (SliderVal < MIN_RANDOM_SLIDER || SliderVal > MAX_RANDOM_SLIDER)
	{
		ERROR3("Slider value is out of range in CBrushEditDlg::GetRandomRangeText");
		return RangeText;
	}
	UINT32 BaseVal = 100;
	UINT32 MinVal = 100 ;
	UINT32 MaxVal = 100 ;

	// the actual limits are worked out in the PPB static function
	PathProcessorBrush::GetRandomLimits(BaseVal, SliderVal, &MinVal, &MaxVal);

	// annoyingly we now need to recast our values as long
	String_32 MinString;
	String_32 MaxString;
	String_32 MiddleString = " - ";
	Convert::LongToString((INT32)MinVal, &MinString);
	Convert::LongToString((INT32)MaxVal, &MaxString);


	RangeText += MinString;
	RangeText += MiddleString;
	RangeText += MaxString;

	return RangeText;

}

// essentially the same as above except that after we get the limits we convert them into degrees
String_32 CBrushEditDlg::GetRotationRandomText(INT32 SliderVal)
{
	String_32 RangeText(" ");
	if (SliderVal < MIN_RANDOM_SLIDER || SliderVal > MAX_RANDOM_SLIDER)
	{
		ERROR3("Slider value is out of range in CBrushEditDlg::GetRandomRangeText");
		return RangeText;
	}
	UINT32 BaseVal = 100;
	UINT32 MinVal = 100 ;
	UINT32 MaxVal = 100 ;

	// the actual limits are worked out in the PPB static function
	PathProcessorBrush::GetRotationRandomLimits(BaseVal, SliderVal, &MinVal, &MaxVal);

	//double MinValDegrees = 
	// annoyingly we now need to recast our values as long
	String_32 MinString;
	String_32 MaxString;
	String_32 MiddleString = " - ";
	Convert::LongToString((INT32)MinVal, &MinString);
	Convert::LongToString((INT32)MaxVal, &MaxString);


	RangeText += MinString;
	RangeText += MiddleString;
	RangeText += MaxString;

	return RangeText;
}

// this function gets the current slider value and then sets the accompanying
// label with a string representing the possible random range
void CBrushEditDlg::SetRandomRangeText(CGadgetID SliderID)
{
	BOOL Valid = FALSE;
	INT32 SliderVal = 0;
	switch (SliderID)
	{
		case _R(IDC_SLIDERBRUSHSPACINGMAXRAND):
		{
			SliderVal = GetLongGadgetValue (_R(IDC_SLIDERBRUSHSPACINGMAXRAND), MIN_RANDOM_SLIDER, MAX_RANDOM_SLIDER, 0, &Valid);
			if (Valid)
			{
				SliderVal = (MAX_RANDOM_SLIDER - SliderVal) + MIN_RANDOM_SLIDER;
				String_32 RangeText = GetRandomRangeText(SliderVal);
				SetStringGadgetValue(_R(IDC_STATICBRUSHSPACINGRAND1), &RangeText);
			}
		}
		break;
		case _R(IDC_SLIDEROFFSETVALUEMAXRAND):
		{
			 SliderVal = GetLongGadgetValue(_R(IDC_SLIDEROFFSETVALUEMAXRAND), MIN_RANDOM_SLIDER, MAX_RANDOM_SLIDER, 0, &Valid);
			 if (Valid)
			 {
				SliderVal = (MAX_RANDOM_SLIDER - SliderVal) + MIN_RANDOM_SLIDER;
				String_32 RangeText = TEXT(""); //GetRandomRangeText(SliderVal);
				
				String_32 MinString = TEXT("0 - ");
				String_32 MaxString = TEXT("");;
				Convert::LongToString((INT32)SliderVal, &MaxString);

				RangeText += MinString;
				RangeText += MaxString;
				SetStringGadgetValue(_R(IDC_STATICOFFSETRAND), &RangeText);
			 }
		}
		break;
		case _R(IDC_SLIDERBRUSHSCALINGMAXRAND):
		{
			SliderVal = GetLongGadgetValue(_R(IDC_SLIDERBRUSHSCALINGMAXRAND), MinScalingSlider, MaxScalingSlider, 0, &Valid);
			if (Valid)
			{
				SliderVal = (MaxScalingSlider - SliderVal) + MinScalingSlider;
				String_32 RangeText = GetRandomRangeText(SliderVal);
				SetStringGadgetValue(_R(IDC_STATICSCALINGRAND), &RangeText);
			}
		}
		break;
		case _R(IDC_SLIDERROTATEANGLEMAXRAND):
		{
			// This is a little different as we go from 0 - Max degrees, where Max is determined by
			// SliderVal * 360
			SliderVal = GetLongGadgetValue(_R(IDC_SLIDERROTATEANGLEMAXRAND), MIN_RANDOM_SLIDER, MAX_ROTATION_RANDOM_SLIDER, 0, &Valid);
			if (Valid)
			{
				SliderVal = (MAX_ROTATION_RANDOM_SLIDER - SliderVal) + MIN_RANDOM_SLIDER;
				//SliderVal = (SliderVal * 2) + 100; 
				double MaxAngle = double(SliderVal); // * 360;
				//MaxAngle = (MaxAngle * 0.01) - 360;

				String_32 RangeText(""); 
				String_32 MinString;
				String_32 MaxString;
				String_32 MiddleString = " - ";
				Convert::LongToString(0, &MinString);
				Convert::LongToString(INT32(MaxAngle), &MaxString);

				RangeText = MinString;
				RangeText += MiddleString;
				RangeText += MaxString;
		
				SetStringGadgetValue(_R(IDC_STATICROTATIONRAND), &RangeText);
			}
		}
		break;
		case _R(IDC_SLIDERSCALINGPRESSURE):
		{
			SliderVal = GetLongGadgetValue(_R(IDC_SLIDERSCALINGPRESSURE), MIN_PRESSURE_SLIDER, MAX_PRESSURE_SLIDER, 0, &Valid);
			if (Valid)
			{
				INT32 lSlider = MAX_PRESSURE_SLIDER - (INT32)SliderVal;
				
				String_32 MinString = TEXT("");
				Convert::LongToString((INT32)SliderVal, &MinString);
				
				// our maximum is always 100%
				String_32 MaxString = TEXT(" - 100");

				MinString += MaxString;
				SetStringGadgetValue(_R(IDC_STATICSCALINGPRESS), &MinString);
			}
		}
		break;
		case _R(IDC_SLIDERSATURATIONRANDOM):
		{
			// this is a little different as it only goes from 0 - SliderVal;
			SliderVal = GetLongGadgetValue(_R(IDC_SLIDERSATURATIONRANDOM), MIN_RANDOM_SLIDER, MAX_RANDOM_SAT_SLIDER, 0, &Valid);
			if (Valid)
			{
				SliderVal = MAX_RANDOM_SAT_SLIDER - SliderVal;
				String_32 RangeText(""); 
				String_32 MinString;
				String_32 MaxString;
				String_32 MiddleString = " - ";
				Convert::LongToString(0, &MinString);
				Convert::LongToString(SliderVal, &MaxString);

				RangeText = MinString;
				RangeText += MiddleString;
				RangeText += MaxString;
			
				SetStringGadgetValue(_R(IDC_STATICSATRAND), &RangeText);
			}
		}
		break;
		case _R(IDC_SLIDERHUERANDOM):
		{
			SliderVal = GetLongGadgetValue(_R(IDC_SLIDERHUERANDOM), MIN_RANDOM_SLIDER, MAX_RANDOM_HUE_SLIDER, 0, &Valid);
			if (Valid)
			{
				SliderVal = MAX_RANDOM_HUE_SLIDER - SliderVal;
			///	String_32 RangeText = GetRandomText(SliderVal, MAX_RANDOM_HUE_SLIDER);
			//	SetStringGadgetValue(_R(IDC_STATICHUERAND), &RangeText);


				String_32 MinString = TEXT("0 - ");
				String_32 MaxString = TEXT("");
				Convert::LongToString((INT32)SliderVal, &MaxString);

				MinString += MaxString;
				SetStringGadgetValue(_R(IDC_STATICSCALINGPRESS), &MinString);
			
			}
		}
		break;
		case _R(IDC_SLIDERTRANSPPRESSURE):
		{
			SliderVal = GetLongGadgetValue(_R(IDC_SLIDERTRANSPPRESSURE), MIN_RANDOM_SLIDER, MAX_RANDOM_SLIDER, 0, &Valid);
			if (Valid)
			{
				SliderVal = MAX_RANDOM_SLIDER - SliderVal;
				String_32 RangeText = GetRandomText(SliderVal, MAX_RANDOM_SLIDER);
				SetStringGadgetValue(_R(IDC_STATICTRANSPPRESSURE), &RangeText);
			}
		}
		break;
		default:
		{
			// do nothing just yet
		}
		break;
	}
}

String_32 CBrushEditDlg::GetRandomText(INT32 Rand, INT32 MaxRand)
{
	INT32 Base = 100;
	INT32 MinVal = Base;
	INT32 MaxVal = Base;

	double RandomFactor = (double)Rand / 100;
	RandomFactor++; // = (RandomFactor * MaxRand);

	if (RandomFactor != 0)
	{
		MinVal = (INT32)((double)Base  / RandomFactor);
		MaxVal  =(INT32)((double)Base * RandomFactor);
	}


	String_32 RangeText("");

	// annoyingly we now need to recast our values as long
	String_32 MinString;
	String_32 MaxString;
	String_32 MiddleString = " - ";
	Convert::LongToString(MinVal, &MinString);
	Convert::LongToString(MaxVal, &MaxString);


	RangeText += MinString;
	RangeText += MiddleString;
	RangeText += MaxString;

	return RangeText;
}


// make a new brush and insert it into the brush component
void CBrushEditDlg::CreateNewBrush()
{
	// as we are basically making a copy of an existing definition we can use the nodetree
	// of the existing definition, so we need to obtain a pointer to it.
	BrushHandle ThisHandle = EditData->m_BrushHandle;

	// get the brush component from the document
	Document* pDoc = Document::GetCurrent();
	if (pDoc == NULL)
	{
		ERROR3("Document is NULL in CBrushEditDlg::BrushEditDlgOnOK");
		return;
	}

	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	if (pBrushComp == NULL)
	{
		ERROR3("Brush component is null in CBrushEditDlg::BrushEditDlgOnOK");
		return;
	}

	BrushDefinition* pBrushDef = pBrushComp->FindDefinition(ThisHandle);

	if (pBrushDef == NULL)
	{
		ERROR3("Brush definition is NULL in CBrushEditDlg::BrushEditDlgOnOK");
		return;
	}
	
	// ask the definition to copy itself
	BrushDefinition* pNewBrushDef = pBrushDef->Copy();

	if (pNewBrushDef == NULL)
		return;

	// copy our brush data to the definition
	pNewBrushDef->SetMembersFromData(*EditData);
	pNewBrushDef->SetBrushScaling(1.0);
	// find out how many brushes there are
	UINT32 LastBrush = pBrushComp->GetNumberOfBrushes() - 1;

	BrushHandle NewHandle = pBrushComp->AddNewItem(pNewBrushDef, TRUE);

	// tell the world who we are
	if (NewHandle > LastBrush)
		BROADCAST_TO_ALL(NewBrushMsg());
	else
		//ERROR3("Tried to make an identical brush");
		TRACEUSER( "Diccon", _T("Tried to make an identical brush\n"));

	EditData->m_BrushHandle = NewHandle;


}

