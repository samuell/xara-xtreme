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

#include "camtypes.h" 

#include "opfeathr.h"
#include "fthrattr.h"
//#include "undoop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "resource.h" 
//#include "opdesc.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "simon.h"
//#include "assoc.h"
#include "bubbleid.h"
//#include "mario.h"
//#include "barsdlgs.h"
//#include "feather.h"
#include "nodedoc.h"		// IS_KIND_OF(NodeDocument)
//#include "rikdlg.h"			// _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER)
//#include "biasres.h"
#include "biasdlg.h"
#include "objchge.h"		// ObjChange and AllowOp stuff

#include "optsmsgs.h"		// OptionsChangingMsg

//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "grndrgn.h"

// not supported yet
#include "nodebev.h"
#include "nodeshad.h"
#include "nbevcont.h"
#include "nodecntr.h"
#include "ncntrcnt.h"
#include "nodetext.h"
#include "selector.h"		// for class SelectorTool
#include "opbevel.h"
#include "offscrn.h"
#include "opliveeffects.h"
#include "nodeliveeffect.h"

CC_IMPLEMENT_DYNCREATE(OpChangeFeatherSize, UndoableOperation)
CC_IMPLEMENT_DYNCREATE(OpChangeFeatherProfile, UndoableOperation)
CC_IMPLEMENT_DYNCREATE(ChangeFeatherProfileAction, Action)
#ifdef FEATHER_EFFECT
CC_IMPLEMENT_DYNCREATE(ChangeFeatherEffectProfileAction, Action)
CC_IMPLEMENT_DYNCREATE(ChangeFeatherEffectAction, Action)
#endif
CC_IMPLEMENT_DYNCREATE(RegenerateFeatherContourAction, Action)
CC_IMPLEMENT_DYNAMIC(OpChangeFeatherSizeParam, OpParam)
CC_IMPLEMENT_DYNAMIC(FeatherProfileOpParam, OpParam)
CC_IMPLEMENT_DYNAMIC(ChangeFeatherSizeSliderOpDesc, UndoableOpDescriptor)
CC_IMPLEMENT_DYNAMIC(ChangeFeatherProfileOpDesc, UndoableOpDescriptor)

#define new CAM_DEBUG_NEW

CBiasGainGadget		ChangeFeatherProfileOpDesc::m_BiasGainGadget;

const MILLIPOINT	MaxFeatherSize = ((const MILLIPOINT)(MAX_FEATHERSIZE_MP));
const MILLIPOINT	ChangeFeatherSizeSliderOpDesc::MinSlider = 0;
const MILLIPOINT	ChangeFeatherSizeSliderOpDesc::MaxSlider = ((const MILLIPOINT) (MaxFeatherSize / 2 ));
const double		ChangeFeatherSizeSliderOpDesc::SliderChangeRate = 1.5;

INT32					OpChangeFeatherSize::s_iEditStackPos = STACKPOS_INVALID;
ListRange*			OpChangeFeatherSize::s_pEditRange = NULL;



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										Changing feather size
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ChangeFeatherSizeSliderOpDesc::ChangeFeatherSizeSliderOpDesc(
						 	UINT32 toolID,                    // Tool (Module) Identifier
						 	UINT32 txID,                      // String Resource ID
						 	CCRuntimeClass* Op,				// pointer to the Op's runtime class object
						 	TCHAR* tok,						// pointer to the token string
						 	pfnGetState gs,					// pointer to the GetState function
						 	UINT32 helpId,				// help identifier 
						 	UINT32 bubbleID,				// string resource for bubble help
						 	UINT32 resourceID,			// resource ID
						 	UINT32 controlID,				// control ID within resource
						 	BOOL ReceiveMessages, 		
						 	BOOL Smart, 
						 	BOOL Clean,
				 			UINT32 AutoStateFlags
						   )
	: UndoableOpDescriptor(toolID, txID, Op, tok, gs, helpId, bubbleID, 
						 resourceID, controlID, ReceiveMessages, Smart,
						 Clean, 0, AutoStateFlags )
{
	m_pOpChangeFeatherSize = NULL;
}

/********************************************************************************************

>	void ChangeFeatherSizeSliderOpDesc::OnControlCreate(OpDescControlCreateMsg* CreateMsg)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/2000

	Purpose:	This function is called when the feathering bar is about to be shown.
				This function gets pointers to all of the the controls within the bar
				(after an indirect fashion) and stores these.  These pointers can then be
				used within the other feathering OpDescs' to gain direct access ....

********************************************************************************************/

void ChangeFeatherSizeSliderOpDesc::OnControlCreate(OpDescControlCreateMsg* CreateMsg)
{
	DialogOp* pDlg = CreateMsg->pDlgOp;
	CGadgetID GadgetID = CreateMsg->SetGadgetID;

	BOOL IsHorizontal = TRUE;
	if	(pDlg->IsKindOf(CC_RUNTIME_CLASS(DialogBarOp)))
			IsHorizontal = ((DialogBarOp *) pDlg)->IsHorizontal();
	pDlg->SetGadgetRange (GadgetID, MinSlider, MaxSlider);
	if(IsHorizontal)
		pDlg->SetGadgetBitmaps (GadgetID, _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
	else
		pDlg->SetGadgetBitmaps (GadgetID, _R(IDB_QUALITYBASEVERT), _R(IDB_QUALITYSLIDERVERT));

	SetFeatherSizeForCurrentSel();
}

/********************************************************************************************
>	MsgResult ChangeFeatherSizeSliderOpDesc::Message(Msg* Msg)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/2000

	Purpose:	Needed to override this in order to respond to OpMsg messages
				(ie to update ui controls when undo of any of the feather ops occurs)
********************************************************************************************/
MsgResult ChangeFeatherSizeSliderOpDesc::Message(Msg* pMessage)
{
	if (MESSAGE_IS_A(pMessage, OpMsg))
	{
		/*
		BEGIN,			// An operation is about to be performed
		END, 			// An operation has successfully ended
		BEFORE_UNDO,	// Sent prior to the operation being undone
		AFTER_UNDO,		// Sent after the operation has been undone
		BEFORE_REDO,	// Sent prior to the operation being redone
		AFTER_REDO		// Sent after the operation has been redone
		*/

		OpMsg* pMsg = (OpMsg*) pMessage;
		switch (pMsg->MsgType)
		{
			case OpMsg::AFTER_UNDO:
			case OpMsg::AFTER_REDO:
				if( /*pMsg->pOp->IS_KIND_OF(OpFeather) ||
					pMsg->pOp->IS_KIND_OF(OpUnFeather) ||*/
					pMsg->pOp->IS_KIND_OF(OpChangeFeatherSize) )
				{
					SetFeatherSizeForCurrentSel();
				}
				break;
			default:
				break;
		}
	}
	
	return UndoableOpDescriptor::Message(pMessage);
}

/********************************************************************************************
>	BOOL ChangeFeatherSizeSliderOpDesc::OnCommonAttrsChangedMsg()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/2000

	Purpose:	Updating UI controls in response to selection changes
********************************************************************************************/
BOOL ChangeFeatherSizeSliderOpDesc::OnCommonAttrsChangedMsg()
{
	// Not sure if sel changes from one feathered object to another will register
	// an attr change
	// Using this in place of OnSelChangingMsg (same as OpLineWidthOpDesc)
	return SetFeatherSizeForCurrentSel();
}



BOOL ChangeFeatherSizeSliderOpDesc::OnOptionsChangingMsg(OptionsChangingMsg* Msg)
{
	OptionsChangingMsg *OpsChangeMsg = (OptionsChangingMsg *) Msg;
	if( OpsChangeMsg->State == OptionsChangingMsg::NEWUNITS)
	{
		// ensure we don't have a cached feather
		SelRange* pSelRange = GetApplication()->FindSelection();
		if (pSelRange != NULL)
			pSelRange->AttrsHaveChanged();

		return SetFeatherSizeForCurrentSel();
	}

	return TRUE;
}



BOOL ChangeFeatherSizeSliderOpDesc::OnSelChangingMsg(SelChangingMsg::SelectionState State)
{
	// if nodes have been added/removed from the selection, then ensure that
	// we abort any drag currently in progress.
	// to prevent responding to messages sent from SlideFinished(), copy the ptr first.
//	if (State == SelChangingMsg::SelectionState::SELECTIONCHANGED)
//	{
//		if (m_pOpChangeFeatherSize != NULL)
//		{
//			OpChangeFeatherSize* pOp = m_pOpChangeFeatherSize;
//			m_pOpChangeFeatherSize = NULL;
//			pOp->SlideFinished(FALSE);
//		}
//	}


	return TRUE;
}


// update the feather controls when the selection changes.
BOOL ChangeFeatherSizeSliderOpDesc::SetFeatherSizeForCurrentSel()
{
	MILLIPOINT	MpFeather = 0;
	String_256	StrFeather("");
	SelRange::CommonAttribResult Result = Range::ATTR_NONE;
	AttrFeather* pAttr = NULL;

	if (Document::GetSelected() != NULL) 
	{
		// If we have a specific context within which we should be operating
		// use that
		Range* pLevelRange = OpChangeFeatherSize::GetEditContext();
		if (pLevelRange==NULL)
		{
			// If not, go get a context from the current selection
			SelRange* pSelRange = GetApplication()->FindSelection();
			pLevelRange = pSelRange->GetTopClassRange(CC_RUNTIME_CLASS(NodeFeatherEffect), FALSE, TRUE);
		}

		if (pLevelRange)
		{
			Node* pNode = pLevelRange->FindFirst();
			while (pNode)
			{
				if (pNode->IsEffect() && ((NodeEffect*)pNode)->IsFeatherEffect())
				{
					NodeFeatherEffect* pEffect = (NodeFeatherEffect*)pNode;
					if (Result==Range::ATTR_COMMON && pEffect->GetFeatherSize() != MpFeather)
					{
						Result = Range::ATTR_MANY;
						break;
					}

					if (Result==Range::ATTR_NONE && pEffect->GetFeatherSize() != 0)
					{
						Result = Range::ATTR_COMMON;
						MpFeather = pEffect->GetFeatherSize();
					}
				}
				else
				if (pNode->IsAnObject())	// sanity check.
				{
					pAttr = (AttrFeather*)pNode->FindFirstChild(CC_RUNTIME_CLASS(AttrFeather));
					if (pAttr)
					{
						if (Result==Range::ATTR_COMMON && pAttr->Value.GetFeatherSize() != MpFeather)
						{
							Result = Range::ATTR_MANY;
							break;
						}

						if (Result==Range::ATTR_NONE && pAttr->Value.GetFeatherSize() != 0)
						{
							Result = Range::ATTR_COMMON;
							MpFeather = pAttr->Value.GetFeatherSize();
						}
					}
				}

				pNode = pLevelRange->FindNext(pNode);
			}
		}

		// -------------------------------------------------
		// Show the user the result
		switch (Result)
		{
			case Range::ATTR_COMMON:
				ConvertSizeToUnits(StrFeather, MpFeather);
				break;

			case Range::ATTR_NONE:
				MpFeather	= 0;
				StrFeather	= String_256(_R(IDS_NONE));
				break;

			case Range::ATTR_MANY:
				MpFeather	= 0;
				StrFeather	= String_256(_R(IDS_MANY));
				break;
		}
	}

	// Set the string in all gadgets associated with the OpDescriptor
	return UpdateAllFeatherControls(&StrFeather, MpFeather);
}



/********************************************************************************************

>	BOOL ChangeFeatherSizeSliderOpDesc::UpdateAllFeatherControls(String_256* Str,
																MILLIPOINT InverseSliderVal)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com> (tidied up and header'ed)
	Created:	24 August 2000
	Inputs:		Str					string value to insert into the edit control.
				InverseSliderVal	value used to set the slider bar.

	Returns:	FALSE if nothing went wrong,
				TRUE otherwise.

	Purpose:	Update all feather UI controls.
				Str == NULL or InverseSliderVal < 0 are valid inputs - they just do nothing.

	Errors:		ERROR3 with FALSE if InverseSliderVal > MaxSlider.

	Notes:		Note that the cc_CustomEdit control may get updated TWICE - once for the
				edit text value and again for the slider position. This is because the
				control does not understand the mapping between the two, or have access
				to the selection to determine that "Many" objects are selected.

********************************************************************************************/
BOOL ChangeFeatherSizeSliderOpDesc::UpdateAllFeatherControls(String_256* Str,
															MILLIPOINT InverseSliderVal)
{
	// Create a list for the dialogue manager to put gadget ID's on.
	List* pGadgetList = new List;
	if (pGadgetList == NULL)
	{
		InformError(_R(IDE_NOMORE_MEMORY));
		return FALSE;
	}

	if (BuildGadgetList(pGadgetList))
	{
		GadgetListItem*	GadgetItem;

		for (GadgetItem = (GadgetListItem*) pGadgetList->GetHead(); 
			 GadgetItem != 0; 
			 GadgetItem	= (GadgetListItem*) pGadgetList->GetNext(GadgetItem))
		{
			UINT32		GadgetID = GadgetItem->gidGadgetID;
			DialogOp*	pDlg	 = GadgetItem->pDialogOp;

			/* Remove until custom slider implemented
			if (Str != NULL)
				pDlg->SetStringGadgetValue(GadgetID, Str, FALSE, -1);
			*/

			// set the position of a cc_Slider or the slider component of a cc_CustomEdit...
TRACEUSER("amb", _T("UpdateAllFeatherContrls: Set slider to %d in [%d .. %d]"), InverseSliderVal, MinSlider, MaxSlider);
			SetSliderValue(InverseSliderVal, pDlg, GadgetID);
		}
	}

	pGadgetList->DeleteAll();
	delete pGadgetList;

	return TRUE;
}



/********************************************************************************************

>	INT32 ChangeFeatherSizeSliderOpDesc::GetSliderValue(DialogOp* pDlg, CGadgetID SliderID)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26 September 2000
	Inputs:		pDlg		required to access the slider control.
				SliderID	the ID of the slider control.

	Returns:	The feather value to use in millipoints, derived from the slider's value, or
				-1 if unsuccessful.

********************************************************************************************/
INT32 ChangeFeatherSizeSliderOpDesc::GetSliderValue(DialogOp* pDlg, CGadgetID SliderID)
{
	BOOL Valid;
	INT32 SliderVal = pDlg->GetLongGadgetValue(SliderID, MinSlider, MaxSlider, 0, &Valid);

	if (!Valid)
		return -1;

	SliderVal = (INT32)(MaxSlider * pow(SliderVal / (double)MaxSlider, SliderChangeRate));

	return SliderVal;
}



/********************************************************************************************

>	void ChangeFeatherSizeSliderOpDesc::SetSliderValue(	INT32 SliderVal,
														DialogOp* pDlg, CGadgetID SliderID	)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26 September 2000
	Inputs:		SliderVal	the feather value in millipoints to set the slider for.
				pDlg		required to access the slider control.
				GadgetID	the ID of the slider control.

	Outputs:	The feather slider is set so that it represents the given feather size.

********************************************************************************************/
void ChangeFeatherSizeSliderOpDesc::SetSliderValue(	INT32 SliderVal,
													DialogOp* pDlg, CGadgetID SliderID	)
{
	static const double InverseChangeRate = 1 / SliderChangeRate;

	if (SliderVal < 0)
		return;

	SliderVal = (INT32)(MaxSlider * pow(SliderVal / (double)MaxSlider, InverseChangeRate));
	pDlg->SetLongGadgetValue(SliderID, SliderVal);
}




/********************************************************************************************

>	void ChangeFeatherSizeSliderOpDesc::ConvertSizeToUnits(String_256& StrSize, MILLIPOINT size)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 August 2000
	Inputs:		strSize		the text string to receive the converted value.
				size		the value to convert.

	Purpose:	Convert the given value to appropriate units, returning the result as a text
				string.

	Errors:		ERROR3 if anything goes wrong.

********************************************************************************************/
void ChangeFeatherSizeSliderOpDesc::ConvertSizeToUnits(String_256& StrSize, MILLIPOINT size)
{
	Spread*		pSpread		= NULL;
	DimScale*	pDimScale	= NULL;
	Document*	pDoc = Document::GetSelected();
	if (pDoc != NULL)
	{
		pSpread = pDoc->GetSelectedSpread();
		if (pSpread != NULL)
		{
			pDimScale = DimScale::GetPtrDimScale(pSpread);
			if (pDimScale != NULL)
			{
				pDimScale->ConvertToUnits(size, &StrSize);
			}
		}
	}

	ERROR3IF(pDimScale == NULL,
			"ChangeFeatherSizeSliderOpDesc::ConvertSizeToUnits; error during method setup!");
}



/********************************************************************************************

>	void ChangeFeatherSizeSliderOpDesc::OnSelectionChange(	OpDescControlMsg* SelChangedMsg,
															List* GadgetList )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/2000

	Purpose:	This function is called when the feather size edit box (which is
				'linked' to ChangeFeatherSizeSliderOpDesc) value is changed by the user.  It
				reads the new value from the edit box, and sets our slider accordingly.
				The edit box/slider will be reset to their previous values if the new value
				is not valid.

********************************************************************************************/

void ChangeFeatherSizeSliderOpDesc::OnSelectionChange(OpDescControlMsg* SelChangedMsg, List* GadgetList)
{
PORTNOTE("feather", "Feather Slider OnSelectionChange disabled since it doesn't have an edit field at the moment and gets upset by wxEVT_SCROLL messages")
#if !defined(EXCLUDE_FROM_XARALX)
    DialogOp* pDlg = SelChangedMsg->pDlgOp;
    CGadgetID SetGadgetID = SelChangedMsg->SetGadgetID;

	BOOL bSuccess;
	MILLIPOINT Result = pDlg->GetUnitGadgetValue(SetGadgetID, PIXELS, MinSlider, MaxFeatherSize,
												0, &bSuccess);

	if (bSuccess)
	{
		MILLIPOINT mp = (Result > MaxSlider) ? MaxSlider : Result;
		OpChangeFeatherSize* pOp = new OpChangeFeatherSize();
		if (pOp != NULL)
		{
			OpChangeFeatherSizeParam param(mp, OpChangeFeatherSize::GetEditContext());
			pOp->DoWithParam(NULL, &param);
			pOp->DoSlide(mp);
			pOp->SlideFinished();
			UpdateAllFeatherControls((String_256*)NULL, mp);
		}

		else
			bSuccess = FALSE;
	}

	if (!bSuccess)
		SetFeatherSizeForCurrentSel();
#endif
}

/********************************************************************************************

>	void ChangeFeatherSizeSliderOpDesc::OnSliderSet(OpDescControlMsg* SelChangedMsg,
													List* GadgetList)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/2000

	Purpose:	This function is called when the user stops dragging the feather size slider.
				It finishes of the resize-feather operation which was started when the user
				began dragging the slider.

********************************************************************************************/

void ChangeFeatherSizeSliderOpDesc::OnSliderSet(OpDescControlMsg* SelChangedMsg)
{
	// we can't tell our op to tidy itself up if it doesn't exist.
	if (m_pOpChangeFeatherSize == NULL)
		return;

	// tell our op to finish changing the sizes.
	// to stop ourselves responding to any messages sent from SlideFinished,
	// use a copied ptr to our Op to call it.
	OpChangeFeatherSize* pOp = m_pOpChangeFeatherSize;
	m_pOpChangeFeatherSize = NULL;
	pOp->SlideFinished();

	// reinitialise ourself, ready for the next slide.
	LastOnscreenPixelSize = -1;
}

/********************************************************************************************

>	void ChangeFeatherSizeSliderOpDesc::OnSliderChanging(OpDescControlMsg* SliderChangingMsg)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/2000

	Purpose:	This function is called when the feather size slider (which is
				'linked' to ChangeFeatherSizeSliderOpDesc) value is changed by the user.
				It reads the new value from the slider, and sets our edit box accordingly.

********************************************************************************************/

void ChangeFeatherSizeSliderOpDesc::OnSliderChanging(OpDescControlMsg* SliderChangingMsg)
{
	DialogOp* pDlg = SliderChangingMsg->pDlgOp;
	CGadgetID SetGadgetID = SliderChangingMsg->SetGadgetID;
	INT32 Result = GetSliderValue(pDlg, SetGadgetID);
	if (Result < 0)
		return;

	const double dpi = (double)GRenderRegion::GetDefaultDPI();
	const double MillipointsPerPixel = (dpi <= 0) ? 750.0 : MILLIPOINTS_PER_INCH / dpi;

	MILLIPOINT mp = Result;

	double	fOnscreenPix = (mp / MillipointsPerPixel);
			fOnscreenPix *=	GetCurrentViewScale().MakeDouble();

	if (fOnscreenPix > 1)
		mp = ConvertMeasurementToMillipointsAtCurrentViewScale((INT32)(fOnscreenPix + 0.5), PIXELS);

//	if (OnscreenPix == LastOnscreenPixelSize)
	if (mp == LastOnscreenPixelSize)
		return;

	// if we haven't already started a size change, then do so.
	if (m_pOpChangeFeatherSize == NULL)
	{
		m_pOpChangeFeatherSize = new OpChangeFeatherSize;
		if (m_pOpChangeFeatherSize != NULL)
		{
			OpChangeFeatherSizeParam param(mp, OpChangeFeatherSize::GetEditContext());
			m_pOpChangeFeatherSize->DoWithParam(NULL, &param);
		}
		else
			return;
	}

	// ok, we're currently in the middle of a slide,
	// so pass our slide information over to our op.
//	MILLIPOINT mp = ConvertMeasurementToMillipointsAtCurrentViewScale(OnscreenPix, PIXELS);
	m_pOpChangeFeatherSize->DoSlide(mp);

	// update our last pixel size.
//	LastOnscreenPixelSize = OnscreenPix;
	LastOnscreenPixelSize = mp;

	// update all the feather controls.
	String_256 Str;
	ConvertSizeToUnits(Str, mp);
	UpdateAllFeatherControls(&Str, mp);
}

/********************************************************************************************

>	void ChangeFeatherSizeSliderOpDesc::OnSliderCancelled(OpDescControlMsg* SliderChangingMsg)

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/11/2005

	Purpose:	This function is called when the feather size slider (which is
				'linked' to ChangeFeatherSizeSliderOpDesc) is cancelled by the user.

********************************************************************************************/

void ChangeFeatherSizeSliderOpDesc::OnSliderCancelled(OpDescControlMsg* SliderChangingMsg)
{
	// to prevent responding to messages sent from SlideFinished(), copy the ptr first.
	if (m_pOpChangeFeatherSize != NULL)
	{
		OpChangeFeatherSize* pOp = m_pOpChangeFeatherSize;
		m_pOpChangeFeatherSize = NULL;
		pOp->SlideFinished(FALSE);
		SetFeatherSizeForCurrentSel();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL OpChangeFeatherSize::Init()
{
	OpDescriptor* OpDesc = new ChangeFeatherSizeSliderOpDesc(0,
											 _R(IDS_FEATHERSIZEOP),
											 CC_RUNTIME_CLASS(OpChangeFeatherSize),
											 OPTOKEN_FEATHERSIZE,
											 OpChangeFeatherSize::GetState,			// NB Operations GetState
											 0,
											 _R(IDBBL_FEATHERSIZEOP),
											 SYSTEMBAR_FEATHER,
											 _R(IDC_FEATHERSIZE_CUSTOMEDIT),
											 TRUE,
											 FALSE,
											 FALSE,
											 (GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION | DONT_GREY_WHEN_SELECT_INSIDE ) );

	ERROR2IF(!OpDesc, FALSE, _R(IDE_NOMORE_MEMORY));

	return TRUE;
}




/********************************************************************************************
>	BOOL OpChangeFeatherSize::DeInit()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/2005
	Purpose:	Deallocate anything grabbed in Init or grabbed statically
********************************************************************************************/

BOOL OpChangeFeatherSize::DeInit()
{
	if (s_pEditRange)
	{
		delete s_pEditRange;
		s_pEditRange = NULL;
	}

	return TRUE;
}




OpState OpChangeFeatherSize::GetState(String_256* Description, OpDescriptor*)
{
	// Always enabled.
	// Wherever a feather attr can't be applied we can apply a feather effect...

	OpState OpSt;

	OpSt.Greyed = FALSE;

	return OpSt;
}




/********************************************************************************************

>	static void		OpChangeFeatherSize::SetEditContext(INT32 iStackPos = STACKPOS_INVALID, ListRange* pEditRange = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/05/2005
	Inputs:		iStackPos	- stack level index
				pEditRange	- list of nodes in that stack level
	Purpose:	Tell the feather UI that it must edit a specific set of feather nodes - not
				find it's own set from the selection.
	See also:	DoSlide(), SlideFinished().

********************************************************************************************/
void OpChangeFeatherSize::SetEditContext(INT32 iStackPos, ListRange* pEditRange)
{
	if (s_pEditRange && s_pEditRange != pEditRange)
	{
		delete s_pEditRange;
		s_pEditRange = NULL;
	}

	s_iEditStackPos = iStackPos;
	if (pEditRange)
		s_pEditRange = new ListRange(pEditRange);
	else
		s_pEditRange = NULL;

	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::EFFECTSTACKCHANGED));
}




/********************************************************************************************

>	ListRange* OpChangeFeatherSize::GetEditContext()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	Pointer to range 
	Purpose:	Get the current edit context for the feather UI controls
	See also:	DoSlide(), SlideFinished().

********************************************************************************************/
ListRange* OpChangeFeatherSize::GetEditContext()
{
	if (s_iEditStackPos==STACKPOS_INVALID || s_pEditRange==NULL)
		return NULL;

	if (s_pEditRange->MatchesSelectionEffectLevel(s_iEditStackPos))
		return s_pEditRange;

	return NULL;
}




/********************************************************************************************

>	void OpChangeFeatherSize::DoWithParam(OpDescriptor*, OpParam*)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/08/2000

	Inputs:		This method's parameters are unused.

	Purpose:	Called to initiate this action.
				Starts the process of resizing/applying feathers to the selection.

	Notes:		This method works in tandem with DoSlide() and SlideFinished().
				You must always call SlideFinished() at some point after calling this method.
				To perform any useful function, you should also call DoSlide() at least
				once in between this method and SlideFinished().

	See also:	DoSlide(), SlideFinished().

********************************************************************************************/
void OpChangeFeatherSize::DoWithParam(OpDescriptor* pOpDesc, OpParam* pParam)
{
	/////////////////////////////////////////////////////////////////////////////////////////
	// What we do:
	//	For all AllowOp()'d nodes in the selection:
	//		1. Undoably hide any existing feather attribute.
	//		2. Apply a new feather attribute with default settings or same as the old attr,
	//		3. adding it to a list of feather attrs to update.
	//
	BOOL bAddNewFeathers = TRUE;
	MILLIPOINT size = 0;
	ListRange* pLevelRange = NULL;
	ListRange* pLocalRange = NULL;
	BOOL bGotSelRange = FALSE;
	if (pParam && IS_A(pParam, OpChangeFeatherSizeParam))
	{
		OpChangeFeatherSizeParam* pCFSParam = (OpChangeFeatherSizeParam*)pParam;
		bAddNewFeathers = pCFSParam->bAddNewFeathers;
		size = pCFSParam->size;
		pLevelRange = pCFSParam->pLevelRange;
	}

	ObjChangeFlags cFlags;
	cFlags.Attribute = TRUE;

	SelRange* pSelRange = GetApplication()->FindSelection();
	pSelRange->MakePartialSelectionWhole(FALSE, TRUE);

	// Since the selection may change during this Op we must get a permanent copy of
	// the PostProcessor stack
//	INT32 iStackPos = STACKPOS_TOP;
//	EffectsStack* pStack = pSelRange->GetEffectsStack(TRUE, TRUE);
//	ListRange* pLevelRange = pStack->GetLevelRange(&iStackPos);

	// Since the selection may change during this Op we must get a permanent copy of
	// the level range we are working on
	if (pLevelRange==NULL)
	{
		SelRange* pSelRange = GetApplication()->FindSelection();
		pLevelRange = pSelRange->GetTopClassRange(CC_RUNTIME_CLASS(NodeFeatherEffect), FALSE, TRUE);
		pLocalRange = new ListRange(pLevelRange);
		pLevelRange = pLocalRange;
		bGotSelRange = TRUE;
	}

	DoInvalidateNodesRegions(*pLevelRange, TRUE, FALSE, FALSE, FALSE);

	Node* pSelNode = NULL;
	for (	pSelNode = pLevelRange->FindFirst();
			pSelNode != NULL;
			pSelNode = pLevelRange->FindNext(pSelNode)	)
	{
		if (!pSelNode->IsAnObject())
			continue;

		// What a mess! The following logic allows feather effects to
		// be applied to effects, EXCEPT when those effects are shadows
		NodeRenderableInk* pInkNode = (NodeRenderableInk*)pSelNode;
		AttrFeather* pFthr = (AttrFeather*)pInkNode->FindFirstChild(CC_RUNTIME_CLASS(AttrFeather));
		ListRange* pStack = NULL;
		Node* pTopEffectNode = NULL;
		if (bAddNewFeathers && pFthr==NULL && !pInkNode->IsKindOf(CC_RUNTIME_CLASS(NodeFeatherEffect)))
		{
			pStack = EffectsStack::GetEffectsStackFromNode(pInkNode);
			if (pStack)
			{
				pTopEffectNode = pStack->FindLast();
				while (pTopEffectNode && pTopEffectNode->IsAShadowController())
				{
					pTopEffectNode = pStack->FindPrev(pTopEffectNode);
				}
				delete pStack;
			}
		}
		if (bAddNewFeathers && pFthr==NULL && pTopEffectNode!=NULL && pTopEffectNode->IsAnObject())
			pInkNode = (NodeRenderableInk*)pTopEffectNode;

#ifdef FEATHER_EFFECT
		// For each node in the top level range
		// If the node is a postprocessor we must apply a feather effect
		// Otherwise we can apply a legacy style feather effect
		NodeCompound* pCompound = NULL;
		if (pInkNode->IsCompoundClass())
			pCompound = (NodeCompound*)pInkNode;
		if (pInkNode->IsEffect() ||
			(pCompound && (pCompound->HasEffectAttrs() || pCompound->ContainsEffects()))
			)
		{
			// ----------------------------------------------------
			// Feather Effect
			//
			ObjChangeParam ObjChange(OBJCHANGE_STARTING, cFlags, pInkNode, this, OBJCHANGE_CALLEDBYOP);
			if (pInkNode->AllowOp(&ObjChange))
			{
				Node* pEffect = NULL;
				if (pInkNode->IsEffect() && ((NodeEffect*)pInkNode)->IsFeatherEffect())
				{
					// Existing Feather Effect
					NodeFeatherEffect* pFeatherNode = (NodeFeatherEffect*)pInkNode;
					pEffect = OpChangeFeatherSize::DoReplaceFeatherEffectNode( this,
																		pFeatherNode,
																		pFeatherNode->GetFeatherSize(),
																		pFeatherNode->GetProfile());
				}
				else if (bAddNewFeathers)
				{
PORTNOTE("effect", "Removed LiveEffect usage")
#ifndef EXCLUDE_FROM_XARALX
					// New Feather Effect
					pEffect = OpLiveEffect::DoApplyFeatherEffectNode(this,
																	 pInkNode,
																	 POSTPRO_ID_FEATHER,
																	 POSTPRO_DISPLAYNAME_FEATHER,
																	 10 * 750,
																	 CProfileBiasGain());
#endif
				}

				if (pEffect==NULL && bAddNewFeathers)
				{
					FailAndExecute();
					return;
				}

				if (pEffect)
					m_NewFeatherAttrs.AddHead(new NodeListItem(pEffect));
			}
		}
		else
#endif
		{
			// ----------------------------------------------------
			// Feather Legacy Attribute
			//
			// ensure that this is a featherable node
			// (which currently can only be derivatives of NodeRenderableInk).
			ObjChangeParam ObjChange(	OBJCHANGE_STARTING, cFlags, pInkNode, this,
										OBJCHANGE_CALLEDBYOP );
			if (pInkNode->AllowOp(&ObjChange))
			{
				// undoably hide any existing feather attr.
				FeatherAttrValue* pOldFeatherVal = NULL;
				if (pFthr != NULL)
				{
					pOldFeatherVal = &pFthr->Value;
					DoHideNode(pFthr, FALSE);
				}

				// non-undoably attach a new feather attr to the selected node.
				if (pFthr || bAddNewFeathers)
				{
					AttrFeather* pNewFthr = new AttrFeather();
					pNewFthr->AttachNode(pInkNode, FIRSTCHILD);
					if (pNewFthr != NULL)
					{
						if (pOldFeatherVal != NULL)
							pNewFthr->Value.SimpleCopy(pOldFeatherVal);
						else
							pNewFthr->Value.SetFeatherSize(0);

						m_NewFeatherAttrs.AddHead(new NodeListItem(pNewFthr));
					}
				}
			}
		}
	}

	// If we didn't use a supplied edit context
	// Then this would be a good time to clear any existing edit context that existed
	if (bGotSelRange)
	{
		SetEditContext();
		if (pLocalRange)
		{
			delete pLocalRange;
			pLocalRange = NULL;
		}
	}
}



/********************************************************************************************

>	virtual void OpChangeFeatherSize::DoSlide(INT32 SlideValue)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 August 2000
	Inputs:		SlideValue	the value on our slider - we'll use it directly for our feather
				size.

	Purpose:	Called by our OpDescriptor's OnSliderChanging() method.
				Changes all feathers on our list, and causes them to redraw.

	Notes:		There is not time at the mo' (is there ever?), but it may be an idea to
				build these methods into a rudimentery slider or similar framework, like
				the drag framework, which would make implementing slider UI a lot easier,
				and remove lots of bodges.

	See also:	SlideFinished(), DoWithParam().

********************************************************************************************/
void OpChangeFeatherSize::DoSlide(INT32 SlideValue)
{
	/////////////////////////////////////////////////////////////////////////////////////////
	// What we do:
	//	1. Change the feather sizes for all feathers in our list with a valid parent.
	//	2. Cause a redraw of the invalid region, so the user gets immediate feedback.
	//

	// after each slide change, we'll need to invalidate the feathered region
	// so that the feathers re-render themselves correctly.
	DocRect drInvalidRegion(0, 0, 0, 0);

	NodeListItem*	pListItem		= NULL;
	NodeListItem*	pNextListItem	= (NodeListItem*)m_NewFeatherAttrs.GetHead();

	AttrFeather*	pFthrAttr		= NULL;
	Node*			pFeatheredNode	= NULL;

	// Prevent slow renderers from doing their stuff while we try to
	// show the results of the feather op interactively.
	Operation::SetQuickRender(TRUE, this);

	while (pNextListItem != NULL)
	{
		pListItem		= pNextListItem;
		pNextListItem	= (NodeListItem*)m_NewFeatherAttrs.GetNext(pListItem);
		Node* pNode		= pListItem->pNode;
#ifdef FEATHER_EFFECT
		if (pNode->IsEffect() && ((NodeEffect*)pNode)->IsFeatherEffect())
		{
			NodeFeatherEffect* pEffect = (NodeFeatherEffect*)pNode;
			pEffect->SetFeatherSize(SlideValue);

			drInvalidRegion = drInvalidRegion.Union(pEffect->GetBoundingRect());
			drInvalidRegion = drInvalidRegion.Union(pEffect->GetEffectStackBounds());	// There might be effects above us which will re-render after this change
			pEffect->ReleaseCached(TRUE, FALSE, TRUE, FALSE);		// Parents and Self
		}
		else
#endif
		{
			pFthrAttr = (AttrFeather*)pNode;
			pFeatheredNode = pFthrAttr->FindParent();

			if (pFeatheredNode != NULL && pFeatheredNode->IsAnObject())
			{
				pFthrAttr->Value.SetFeatherSize(SlideValue);
				pFeatheredNode = pFthrAttr->FindParent();
				drInvalidRegion = drInvalidRegion.Union(
					((NodeRenderableInk*)pFeatheredNode)->GetBoundingRect()
				);
				drInvalidRegion = drInvalidRegion.Union(((NodeRenderableInk*)pFeatheredNode)->GetEffectStackBounds());	// There might be effects above us which will re-render after this change
				((NodeRenderableInk*)pFeatheredNode)->ReleaseCached(TRUE, FALSE, TRUE, FALSE);		// Parents only
			}
		}
	}

	// force a redraw of all the feathered nodes.
	if (!drInvalidRegion.IsEmpty())
	{
		Document* pDoc = Document::GetSelected();
		if (pDoc != NULL)
		{
			Spread* pSpread	= pDoc->GetSelectedSpread();
			if (pSpread != NULL)
			{
				pDoc->ForceRedraw(pSpread, drInvalidRegion);
				GetApplication()->ServiceRendering();
			}
		}
	}

	// Re-enable slow renderers
	Operation::SetQuickRender(FALSE, this);
}



/********************************************************************************************

>	virtual void OpChangeFeatherSize::SlideFinished(BOOL SuccessfulFinish = TRUE)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 August 2000

	Inputs:		SuccessfulFinish	whether or not the finish is successful; if the slide
									was aborted, then this value should be set to FALSE.

	Purpose:	Called by our OpDescriptor's OnSliderSet() method.
				Finishes off a feather-size slider drag. Removes any zero-size feathers on
				our list from the tree, and adds a hide-node action for legitimate feathers,
				so they hide on undo. Also records an action to invalidate the feathered
				nodes, for undo.

				If we're asked to abort, then we will FailAndExecute(), which will roll back 
				all changes made and restore the tree to the state it was in before the
				slider drag took place.

	Notes:		There is not time at the mo' (is there ever?), but it may be an idea to
				build these methods into a rudimentery slider or similar framework, like
				the drag framework, which would make implementing slider UI a lot easier,
				and remove lots of bodges.

	See also:	DoSlide(), DoWithParam().

********************************************************************************************/
void OpChangeFeatherSize::SlideFinished(BOOL SuccessfulFinish)
{
	/////////////////////////////////////////////////////////////////////////////////////////
	// What we do:
	//	1. Check the size of all feathers in our list.
	//		zero size	=>	remove and delete it from the tree.
	//		ok size		=>	record a hide-node action so it will be hidden on undo.
	//
	//	2. Undoably invalidate the region covered by all validly feathered nodes.
	//

	// we need to undoably invalidate a region of the document.
	DocRect drInvalidRegion(0, 0, 0, 0);

	NodeListItem* pListItem = (NodeListItem*)m_NewFeatherAttrs.GetHead();
	AttrFeather* pFthrAttr = NULL;
	while (pListItem != NULL)
	{
		Node* pNode		= pListItem->pNode;
PORTNOTE("effect", "Removed LiveEffect usage")
#ifndef EXCLUDE_FROM_XARALX
#ifdef FEATHER_EFFECT
		if (pNode->IsEffect() && ((NodeEffect*)pNode)->IsFeatherEffect())
		{
			NodeFeatherEffect* pEffect = (NodeFeatherEffect*)pNode;
			if (pEffect->GetFeatherSize()==0)
				OpLiveEffect::DoDeletePostProcessor(this, pEffect);

			ReleaseFeatherCache(pEffect, drInvalidRegion);
		}
		else
#endif
#endif
		{
			pFthrAttr = (AttrFeather*)pNode;
			Node* pFeatheredNode = pFthrAttr->FindParent();

			// remove all zero-size feathers from the selection.
			if (pFthrAttr->Value.GetFeatherSize() == 0)
			{
				pFthrAttr->UnlinkNodeFromTree();
				delete pFthrAttr;
			}
			else
			{
				// all remaining feathers should have a hide-node-action given to them,
				// so that when the user hits 'undo', they are hidden.
				HideNodeAction* pUndoHideNodeAction = NULL;
				ActionCode ac = HideNodeAction::Init(this,
													&UndoActions,
													pFthrAttr,
													FALSE,		// don't include subtree size
													(Action**)&pUndoHideNodeAction,
													FALSE);		// don't tell subtree when undone
				if (ac == AC_FAIL)
				{
					pFthrAttr->UnlinkNodeFromTree();
					delete pFthrAttr;
				}
			}

			if (pFeatheredNode != NULL && pFeatheredNode->IsAnObject())
			{
				ReleaseFeatherCache((NodeRenderableInk*)pFeatheredNode, drInvalidRegion);
			}
		}

		pListItem = (NodeListItem*)m_NewFeatherAttrs.GetNext(pListItem);
	}

	// invalidate the feathered region.
	// Note that this is needed to allow nodes which were rendering themselves in a fast mode
	// during slider dragging (see Get/SetQuickRender) are now able to render themselves
	// fully now that the drag is over
	if (!drInvalidRegion.IsEmpty())
	{
		Document* pDoc = Document::GetSelected();
		if (pDoc != NULL)
		{
			Spread* pSpread = pDoc->GetSelectedSpread();
			if (pSpread != NULL)
				DoInvalidateRegion(pSpread, drInvalidRegion);
		}
	}

	// update any changed nodes.
	ObjChangeFlags ocf;
	ocf.Attribute = TRUE;
	ObjChangeParam ocp(OBJCHANGE_FINISHED, ocf, NULL, this, OBJCHANGE_CALLEDBYOP);
	UpdateChangedNodes(&ocp);

	m_NewFeatherAttrs.DeleteAll();

	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::EFFECTSTACKCHANGED)); 

	if (!SuccessfulFinish)
		FailAndExecute();

	End();
}




/********************************************************************************************

>	static void OpChangeFeatherSize::ReleaseFeatherCache(NodeRenderableInk* pNode, DocRect& drRegion)
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/05/2005
	Inputs:		pNode - pointer to node (either FeatherEffect or node above Feather Attr)
				drRegion - Region rectangle to be updated
	Outputs:	drRegion - Updated region rectangle
	Returns:	-
	Purpose:	Encapsulate common bounding rect and cache control for feather attribute changing

********************************************************************************************/
void OpChangeFeatherSize::ReleaseFeatherCache(NodeRenderableInk* pNode, DocRect& drRegion)
{
	ERROR3IF(pNode==NULL, "ReleaseFeatherCache passed NULL node ptr");

	// T'would be better if ReleaseCached returned a rectangle that needs to
	// be invalidated to recover all the cached data...
	if (pNode->IsEffect())
		pNode->ReleaseCached(TRUE, FALSE, FALSE, FALSE);
	else
		pNode->ReleaseCached(TRUE, FALSE, TRUE, TRUE);

	NodeRenderableInk* pBoundsNode = pNode;
	ListRange* pStack = EffectsStack::GetEffectsStackFromNode(pNode);
	if (pStack)
	{
		pBoundsNode = (NodeEffect*)pStack->FindLast();
		delete pStack;
	}

	drRegion = drRegion.Union(pBoundsNode->GetBoundingRect());
}




/********************************************************************************************

>	static NodeFeatherEffect* OpChangeFeatherSize::DoReplaceFeatherEffectNode(UndoableOperation* pOp,
														  NodeFeatherEffect* pSel,
														  MILLIPOINT FeatherSize,
														  CProfileBiasGain Profile)
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/2005
	Inputs:		pSel - pointer to node to insert Feather Effect above
				strDispayName - The display name of the effect we're going to apply
	Outputs:	-
	Returns:	Pointer to newly created NodeFeatherEffect or NULL
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
#ifdef FEATHER_EFFECT
NodeFeatherEffect* OpChangeFeatherSize::DoReplaceFeatherEffectNode(UndoableOperation* pOp,
											  NodeFeatherEffect* pNode,
											  MILLIPOINT FeatherSize,
											  CProfileBiasGain Profile)
{
	ENSURE(pNode, "DoApplyLiveEffect not given a useful node");

	// Read the current state of the node
	MILLIPOINT OldFeatherSize = pNode->GetFeatherSize();
	CProfileBiasGain OldFeatherProfile = pNode->GetProfile();

	// Setup the action to undo this Operation
	ChangeFeatherEffectAction* pAct = NULL;
	if (ChangeFeatherEffectAction::Init(pOp, 
		 								pOp->GetUndoActionList(),
										pNode,
										OldFeatherSize,
										OldFeatherProfile,
										&pAct	) != AC_OK )
	{
		pOp->FailAndExecute();
		pOp->End();
		return NULL;
	}

	// Now we know the action was allocated OK, change the data structure...
	pNode->SetFeatherSize(FeatherSize);
	pNode->SetProfile(Profile);

	return pNode;
}
#endif



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										Changing feather profile
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ChangeFeatherProfileOpDesc::ChangeFeatherProfileOpDesc(
						 	UINT32 toolID,                    // Tool (Module) Identifier
						 	UINT32 txID,                      // String Resource ID
						 	CCRuntimeClass* Op,				// pointer to the Op's runtime class object
						 	TCHAR* tok,						// pointer to the token string
						 	pfnGetState gs,					// pointer to the GetState function
						 	UINT32 helpId,				// help identifier 
						 	UINT32 bubbleID,				// string resource for bubble help
						 	UINT32 resourceID,			// resource ID
						 	UINT32 controlID,				// control ID within resource
						 	BOOL ReceiveMessages, 		
						 	BOOL Smart, 
						 	BOOL Clean,
				 			UINT32 AutoStateFlags
						   )
	: UndoableOpDescriptor(toolID, txID, Op, tok, gs, helpId, bubbleID, 
						 resourceID, controlID, ReceiveMessages, Smart,
						 Clean, 0, AutoStateFlags )
{
	m_bProfileChanging		= FALSE;
	m_bProfileInitialChange = TRUE;
}

/********************************************************************************************

>	void ChangeFeatherProfileOpDesc::OnControlCreate(OpDescControlCreateMsg* CreateMsg)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/2000

	Purpose:	This function is called after ChangeFeatherSizeSliderOpDesc::OnControlCreate ()
				has been called.  This function initialises the biasgain gadget that appears
				on our feathering bar.
	See Also:	ChangeFeatherSizeSliderOpDesc::OnControlCreate

********************************************************************************************/

void ChangeFeatherProfileOpDesc::OnControlCreate(OpDescControlCreateMsg* CreateMsg)
{
	DialogOp* pDlg = CreateMsg->pDlgOp;
	CGadgetID GadgetID = CreateMsg->SetGadgetID;

	// link the profile gadget to its controlling button/infobar ....	
	m_BiasGainGadget.Init(pDlg, GadgetID, _R(IDBBL_BIASGAIN), _R(IDS_BIASGAINDLG));
	m_BiasGainGadget.ToggleFillProfile();
}



MsgResult ChangeFeatherProfileOpDesc::Message(Msg* pMessage)
{
	if (MESSAGE_IS_A(pMessage, OpDescControlMsg))
	{
		OpDescControlMsg* pOpDescCtrlMsg = reinterpret_cast<OpDescControlMsg*>(pMessage);
		
		if (pOpDescCtrlMsg->DlgMsg == DIM_PROFILE_CHANGED)
			OnProfileChanged(pOpDescCtrlMsg);
		else if (pOpDescCtrlMsg->DlgMsg == DIM_PROFILE_CHANGING)
			OnProfileChanging(pOpDescCtrlMsg);
	}
	
	return UndoableOpDescriptor::Message(pMessage);
}



/********************************************************************************************

>	void ChangeFeatherProfileOpDesc::OnSliderSet(OpDescControlMsg* SelChangedMsg)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/2000

	Purpose:	Process the profile chenged message (it's send from BiasGain dialog
				when the combobox is changed or when a slider is set (probably after dragging).

********************************************************************************************/
void ChangeFeatherProfileOpDesc::OnProfileChanged(OpDescControlMsg* SelChangedMsg)
{
	// ILAN:  Although it is possible for me to ammend the SelChangedMsg message so that the new
	// profile is part of it (i.e.  like how the other profiles work); it is not worth me doing
	// this just to get the following working + it would also slow things down somewhat ....

	CProfileBiasGain NewBiasGain = m_BiasGainGadget.GetCurrentDialogProfile();

	if (m_bProfileChanging)
	{
		// expect SliderChanging to be true before a set is called. Already saved initial 
		// state in first Op initiated by OpSliderChanging()
		m_bProfileInitialChange = FALSE;
	}
	else
	{
		m_bProfileInitialChange = TRUE;
	}
	
	m_bProfileChanging = FALSE;

	FeatherProfileOpParam Param(NewBiasGain, OpChangeFeatherSize::GetEditContext());
	OpChangeFeatherProfile* Op = new OpChangeFeatherProfile();
	Op->DoWithParam(this, &Param);
}

/********************************************************************************************

>	void ChangeFeatherProfileOpDesc::OnSliderChanging(OpDescControlMsg* SliderChangingMsg)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/2000

	Purpose:	Handles profile changing message. Usually sent by BiasGain dialog when user
				drags a slider (after the user finished dragging, DIM_PROFILE_CHANGED message
				is sent).

********************************************************************************************/
void ChangeFeatherProfileOpDesc::OnProfileChanging(OpDescControlMsg* SliderChangingMsg)
{
	// ILAN:  Although it is possible for me to ammend the SelChangedMsg message so that the new
	// profile is part of it (i.e.  like how the other profiles work); it is not worth me doing
	// this just to get the following working + it would also slow things down somewhat ....
	
	CProfileBiasGain NewBiasGain = m_BiasGainGadget.GetCurrentDialogProfile();

	if (m_bProfileChanging)
	{
		// Successive slider changing message
		// Don't add to undo history
		m_bProfileInitialChange = FALSE;
	}
	else
	{
		// Store selections initial feather states so that it can be saved in undo list
		// once all sliding has finished (ie. in OnProfileChanged())
		m_bProfileChanging		= TRUE;
		m_bProfileInitialChange = TRUE;
	}

	FeatherProfileOpParam Param(NewBiasGain, OpChangeFeatherSize::GetEditContext());
	OpChangeFeatherProfile* Op = new OpChangeFeatherProfile();
	Op->DoWithParam(this, &Param);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL OpChangeFeatherProfile::Init()
{
	OpDescriptor* OpDesc = new ChangeFeatherProfileOpDesc(0,
											 _R(IDS_FEATHERPROFILEOP),
											 CC_RUNTIME_CLASS(OpChangeFeatherProfile),
											 OPTOKEN_FEATHERPROFILE,
											 OpChangeFeatherProfile::GetState,			// NB Operations GetState
											 0,
											 _R(IDBBL_FEATHERPROFILEOP),
											 SYSTEMBAR_FEATHER,
											 _R(IDC_FEATHERPROFILE_GDGT),
											 TRUE,
											 FALSE,
											 FALSE,
											 (GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION | DONT_GREY_WHEN_SELECT_INSIDE ) );

	ERRORIF(!OpDesc, FALSE, _R(IDE_NOMORE_MEMORY));

	return TRUE;
}




/********************************************************************************************

>	OpState OpChangeFeatherProfile::GetState(String_256* UIDescription, OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/2005
	Purpose:	Return availability of OpChangeFeatherProfile

********************************************************************************************/

OpState OpChangeFeatherProfile::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;

	SelRange::CommonAttribResult status = GetProfileFromSelection2(TRUE, NULL);
	if (status == Range::ATTR_NONE)
	{
		OpSt.Greyed = TRUE;
		String_256 DisableReason(_R(IDS_NO_FEATHERED_OBJECTS));
	   	*UIDescription = DisableReason;	           
	}

	return OpSt;
}




/********************************************************************************************

>	void OpChangeFeatherProfile::Do(OpDescriptor* pOp)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/2000

	Purpose:	This function is called when the user clicks our linked profile button
				(yahoo!).  This function does one of two things:

				1)	Gets the current feathering profile (if there is one); and invokes the
					profile dialog with it.
				2)	Closes the profile dialog down.

	See Also:	InformationBarOp::GetProfileFromSelection() for tips of implementing the
				remainder of this function.

	NOTE:		At present one should NOT implement OpChangeFeatherProfile::DoWithParam ()

********************************************************************************************/

void OpChangeFeatherProfile::Do(OpDescriptor* pOp)
{
	if (!(ChangeFeatherProfileOpDesc::m_BiasGainGadget.IsDialogOpen ()))
	{
		CBiasGainDlg* pDialog_m  =  new CBiasGainDlg();
		CProfileBiasGain CommonProfile;
		SelRange::CommonAttribResult status = GetProfileFromSelection2(FALSE, &CommonProfile);

		// I don't why this isn't working like the other profile gadgets do, BUT 
		// force the controlling button to stay down!

		// AUGH!!!!!!  why on earth do I get a divide by zero error from the dialog if I call 
		// SetBoolGadgetSelected?  well sod it, the button can damn well stay up!

		//pDlg->SetLongGadgetValue (FeatherProfile->GetUniqueGadgetID (), 1);

		//ChangeFeatherProfileOpDesc::m_BiasGainGadget.GetOwningBar ()->SetBoolGadgetSelected (_R(IDC_FEATHERPROFILE_GDGT), TRUE);
		
		//ChangeFeatherProfileOpDesc::SetBoolGadgetSelected (_R(IDC_FEATHERPROFILE_GDGT), TRUE);
		
		pDialog_m->InvokeVia(ChangeFeatherProfileOpDesc::m_BiasGainGadget, &CommonProfile, status==Range::ATTR_MANY);
	}
	else
	{
		//ChangeFeatherProfileOpDesc::m_BiasGainGadget.GetOwningBar ()->SetBoolGadgetSelected (_R(IDC_FEATHERPROFILE_GDGT), FALSE);
		ChangeFeatherProfileOpDesc::m_BiasGainGadget.CloseDialog();
		//ChangeFeatherProfileOpDesc::SetBoolGadgetSelected (_R(IDC_FEATHERPROFILE_GDGT), FALSE);
	}
}

void ChangeFeatherProfileOpDesc::SetBoolGadgetSelected (CGadgetID Gadget, BOOL IsSelected)
{
	/*OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FEATHERPROFILE);
	if (pOpDesc!=NULL)
	{
		// Found the opdescriptor. Now find all the gadgets associated with it
		List Gadgets;
		if (pOpDesc->BuildGadgetList(&Gadgets))
		{
			// Found some. Set the controls accordingly
			GadgetListItem* pGadgetItem = (GadgetListItem*) Gadgets.GetHead();

			while (pGadgetItem != NULL)
			{
				// Set the gadget
				pGadgetItem->pDialogBarOp->SetBoolGadgetSelected(pGadgetItem->gidGadgetID,
																	IsSelected);
				// Find the next gadget
				pGadgetItem = (GadgetListItem*) Gadgets.GetNext(pGadgetItem);
			}
	
			// Clean out the list
			Gadgets.DeleteAll();
		}
	}*/
}


/********************************************************************************************

>	static SelRange::CommonAttribResult OpChangeFeatherProfile::GetProfileFromSelection2(BOOL bFastScan,
																						  CProfileBiasGain* pCommonProfile)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/2005
	Inputs:		-
	Outputs:	Common profile if there is one
	Returns:	Enum indicating None, Common or Many Profiles
	Purpose:	See InformationBarOp::GetProfileFromSelection() for a description of this
				function.
				See BlendInfoBarOp::GetProfileFromSelection for multi-gadget eg.

*********************************************************************************************/

SelRange::CommonAttribResult OpChangeFeatherProfile::GetProfileFromSelection2(BOOL bFastScan,
																					  CProfileBiasGain* pCommonProfile)
{	
	// we must now examine all our nodes ....
	
	// If we have a specific context within which we should be operating
	// use that
	Range* pLevelRange = OpChangeFeatherSize::GetEditContext();
	if (pLevelRange==NULL)
	{
		// If not, go get a context from the current selection
		SelRange* pSelRange = GetApplication()->FindSelection();
		pLevelRange = pSelRange->GetTopClassRange(CC_RUNTIME_CLASS(NodeFeatherEffect), FALSE, TRUE);
	}
	if (pLevelRange==NULL)
		return Range::ATTR_NONE;
	
	BOOL bFoundFeather = FALSE;
	CProfileBiasGain FirstProfile;
	AttrFeather* pAttr = NULL;
	SelRange::CommonAttribResult status = Range::ATTR_NONE;
	MonotonicTime timer;

	Node* pNode = pLevelRange->FindFirst();
	while (pNode && !(bFastScan && timer.Elapsed(100)))
	{
#ifdef FEATHER_EFFECT
		if (pNode->IsEffect() && ((NodeEffect*)pNode)->IsFeatherEffect())
		{
			// -------------------------------------------------------------------------
			// Feather effect case:
			NodeFeatherEffect* pEffect = (NodeFeatherEffect*)pNode;
			if (!bFoundFeather)
			{
				bFoundFeather = TRUE;
				FirstProfile = pEffect->GetProfile();
				status = Range::ATTR_COMMON;
			}
			else
			{
				if (!(FirstProfile == pEffect->GetProfile()))
				break;
			}
		}
		else
#endif
		{
			// -------------------------------------------------------------------------
			// Feather attribute case:
			// Feather attributes don't get optimised so we know we should find it it in the child list of the selected item
			pAttr = (AttrFeather*)pNode->FindFirstChild(CC_RUNTIME_CLASS(AttrFeather));

			if (pAttr)
			{
				if (!bFoundFeather)
				{
					bFoundFeather = TRUE;
					FirstProfile = pAttr->Value.GetProfile();
					status = Range::ATTR_COMMON;
				}
				else
				{
					if (!(FirstProfile == pAttr->Value.GetProfile()))
					break;
				}
			}
			else
			{
				// node is using default feather - got two cases to handle here
				// Or may be INSIDE a feathered Node!!! So checking default value is pointless!
			}
		}

		pNode = pLevelRange->FindNext(pNode);
	}

	// If we exited the loop while there were still more nodes to process
	// We must have run out of time or aborted early because we detected the "many" case
	if (pNode)
	{
		status = Range::ATTR_MANY;
		if (pCommonProfile)
			*pCommonProfile = *(new CProfileBiasGain());		// Reset to defaults
	}
	else
	{
		if (pCommonProfile)
			*pCommonProfile = FirstProfile;
	}

	return status;
}




/********************************************************************************************

>	void OpChangeFeatherProfile::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Change all selected feather profiles
	Note:		It's important that the basic logic of this function is the same as
				that in GetProfileFromSelection2

*********************************************************************************************/

void OpChangeFeatherProfile::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	ObjChangeFlags cFlags;
	cFlags.Attribute = TRUE;
	DocRect InvalidRegion(0,0,0,0);	// To prevent individual node redraws we union all bounding
									// rects of nodes which get feathered and then generate a
									// single redraw action (DoInvalidateNodeRegion)
	CProfileBiasGain NewProfile;
	ListRange* pLevelRange = NULL;
	AttrFeather* pAttr = NULL;

	if (pParam && IS_A(pParam, FeatherProfileOpParam))
	{
		FeatherProfileOpParam* pProfileParam = (FeatherProfileOpParam*)pParam;
		pLevelRange = pProfileParam->pLevelRange;
		NewProfile = pProfileParam->P;
	}

	if (pLevelRange==NULL)
	{
		SelRange* pSelRange = GetApplication()->FindSelection();
		pLevelRange = pSelRange->GetTopClassRange(CC_RUNTIME_CLASS(NodeFeatherEffect), FALSE, TRUE);
	}

	// Since the selection may change during this Op we must get a permanent copy of
	// the level range we are working on
	ListRange* pLocalRange = new ListRange(pLevelRange);
	if (pLocalRange==NULL)
	{
		FailAndExecute();
		End();
		return;
	}

	if (((ChangeFeatherProfileOpDesc*)pOp)->m_bProfileChanging)
	{
		// Prevent slow renderers from doing their stuff while we try to
		// show the results of the feather op interactively.
		Operation::SetQuickRender(TRUE, this);
	}

	DoInvalidateNodesRegions(*pLocalRange, TRUE, FALSE, FALSE, FALSE);

	Node* pNode = pLocalRange->FindFirst();
	while (pNode)
	{
#ifdef FEATHER_EFFECT
		if (pNode->IsEffect() && ((NodeEffect*)pNode)->IsFeatherEffect())
		{
			// -------------------------------------------------------------------------
			// Feather effect case:
			NodeFeatherEffect* pEffect = (NodeFeatherEffect*)pNode;
			CProfileBiasGain undoProf = pEffect->GetProfile();

			// Setup the action to undo this Operation
			ChangeFeatherEffectProfileAction* pAct = NULL;
			if (ChangeFeatherEffectProfileAction::Init(this, 
		 												&UndoActions,
														pEffect,
														undoProf,
														&pAct	) != AC_OK )
			{
				FailAndExecute();
				End();
				return;
			}

			// Only set the new profile if the action was successfully allocated
			pEffect->SetProfile(NewProfile);

			// union feathered node bounding rect so only one redraw per feather op
			OpChangeFeatherSize::ReleaseFeatherCache(pEffect, InvalidRegion);

		}
		else
#endif
		{
			// -------------------------------------------------------------------------
			// Feather attribute case:
			// Feather attributes don't get optimised so we know we should find it it in the child list of the selected item
			pAttr = (AttrFeather*)pNode->FindFirstChild(CC_RUNTIME_CLASS(AttrFeather));

//			if (!pAttr)
//			{
//				// if there is no feather - see if the parent node is a compound; and if so,
//				// see if it has been feathered ....
//
//				if (pNode->FindParent ()->IsController ())
//				{
//					pAttr = pNode->FindParent ()->FindFirstChild(CC_RUNTIME_CLASS(AttrFeather));
//				}
//			}
//
			if (pAttr)
			{
				ObjChangeParam ObjChange(OBJCHANGE_STARTING, cFlags, pNode, this);
				if (pNode->AllowOp(&ObjChange))
				{
					if (!DoChangeFeatherProfile(&pAttr->Value, NewProfile))
					{
						FailAndExecute();
						End();
						return;
					}

					// union feathered node bounding rect so only one redraw per feather op
					if (pNode->IsAnObject())
					{
						OpChangeFeatherSize::ReleaseFeatherCache((NodeRenderableInk*)pNode, InvalidRegion);
					}

					// send the objchange message to trigger any OnChildChange messages
					ObjChange.Define(OBJCHANGE_FINISHED, cFlags, pNode, this);
					UpdateChangedNodes(&ObjChange);
				}
			}
		}

		pNode = pLocalRange->FindNext(pNode);
	}

	delete pLocalRange;
	pLocalRange = NULL;

	// send through the redraw
	if (!(InvalidRegion.hi.x==0) || !(InvalidRegion.lo.x==0) || !(InvalidRegion.hi.y==0) || !(InvalidRegion.lo.y==0))
	{
		Document* SelectedDoc = Document::GetSelected();
		ENSURE(SelectedDoc,"No selected doc.");
		Spread* pSpread = SelectedDoc->GetSelectedSpread();
		ENSURE(pSpread,"No selected spread.");

		DoInvalidateRegion(pSpread,InvalidRegion);
	}
	else
	{
		// no nodes feathered
		SucceedAndDiscard();
	}

	if (!((ChangeFeatherProfileOpDesc*)pOp)->m_bProfileInitialChange)
	{
		// don't want op in op hist
		// nb Action::Init modifies the ophist size in anticipation of the op being added
		// to the history list. Hence modify the size
		SucceedAndDiscard();
	}

	if (((ChangeFeatherProfileOpDesc*)pOp)->m_bProfileChanging)
	{
		// Prevent slow renderers from doing their stuff while we try to
		// show the results of the feather op interactively.
		Operation::SetQuickRender(FALSE, this);
	}

	// bodge/hack alert!  I found it necessary to add some code to the selector tool to prevent
	// profile dilaogs from closing down when changing the selection.  Unfortunately, when you
	// change the profile within the feathers profile dialog - this same code is called; which
	// attempts to close down the dialog.  It must not do this!  I traced the offending call
	// (which invokes OpChangeFeatherProfile::OnSelChangingMsg ()) to being the following one
	// to End ().
	SelectorTool::SetNormalClick_CheckProfileDialog (TRUE);
	End();
	SelectorTool::SetNormalClick_CheckProfileDialog (FALSE);
}




/********************************************************************************************

>	BOOL OpChangeFeatherProfile::OnSelChangingMsg(SelChangingMsg::SelectionState State)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Update the Profile dialog when the selection changes

*********************************************************************************************/

BOOL OpChangeFeatherProfile::OnSelChangingMsg(SelChangingMsg::SelectionState State)
{
	if (ChangeFeatherProfileOpDesc::m_BiasGainGadget.IsDialogOpen ())
	{	
		CProfileBiasGain CommonProfile;
		SelRange::CommonAttribResult status = GetProfileFromSelection2(FALSE, &CommonProfile);
		if (status == Range::ATTR_MANY)
		{
			// handles case where we click on another blend
			ChangeFeatherProfileOpDesc::m_BiasGainGadget.ReInitialiseDialog(&CommonProfile, TRUE);
		}
		else if (status == Range::ATTR_COMMON)
		{	
			// handles case where we click on another blend
			ChangeFeatherProfileOpDesc::m_BiasGainGadget.ReInitialiseDialog(&CommonProfile, FALSE);
		}
		else
		{
			// CGS:  wo there!  when selecting another object, everything is deselected - and then just that node
			// selected.  This is a problem, since if we proceed regardless, the profile dialog closes (when it
			// possibly shouldn't have).  As such, I now check for this occurring ....
			if (!SelectorTool::GetNormalClick_CheckProfileDialog())
			{
				// user click on no object at all - close the profile dialog ....
				ChangeFeatherProfileOpDesc::m_BiasGainGadget.CloseDialog();
			}
			else
				ChangeFeatherProfileOpDesc::m_BiasGainGadget.ReInitialiseDialog(&CommonProfile, FALSE);
		}
	}
	
	return (TRUE);
}




/********************************************************************************************

>	BOOL OpChangeFeatherProfile::DoChangeFeatherProfile(FeatherAttrValue* pFeather, CProfileBiasGain &newProf)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Change one feather profile

*********************************************************************************************/

BOOL OpChangeFeatherProfile::DoChangeFeatherProfile(FeatherAttrValue* pFeather, CProfileBiasGain &newProf)
{
	CProfileBiasGain undoProf = pFeather->GetProfile();

	if (!pFeather->ChangeFeatherProfile(newProf))
	{
		// ChangeFeatherProfileAction doesn't need to alloc any memory so shouldn't ever fail
		return FALSE;
	}

	ChangeFeatherProfileAction* pAct = NULL;

	// Setup the action to undo this Operation
	if (!ChangeFeatherProfileAction::Init(	this, 
		 									&UndoActions,
											pFeather,
											undoProf,
											&pAct	) == AC_OK )
	{
		return FALSE;
	}

	return TRUE;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ChangeFeatherProfileAction::ChangeFeatherProfileAction()
{
	m_pFeather = NULL;
}

/********************************************************************************************
>	ActionCode ChangeFeatherProfileAction::Execute()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/99
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.
	Errors:		-
	SeeAlso:	Action::Init()
********************************************************************************************/
ActionCode ChangeFeatherProfileAction::Execute()
{
	ActionCode Act = AC_FAIL;
	ChangeFeatherProfileAction* pAction;

	// First time an instance of this Action is executed is when called during and
	// UndoList->ExecuteBackwards inside an OpUndo invoked OpHist->GetTail()->Undo (where
	// the tail OpHist item is an OpChangeFeatherProfile. SHEW!
	// However may also be called by a Redo as we are about to stick an instance into the
	// Redo list
	CProfileBiasGain UndoProf = m_pFeather->GetProfile();
	if (m_pFeather->ChangeFeatherProfile(m_LastProfile))
		Act = ChangeFeatherProfileAction::Init(	pOperation, 
										pOppositeActLst,
										m_pFeather,
										UndoProf,
										&pAction );

	if (ChangeFeatherProfileOpDesc::m_BiasGainGadget.IsDialogOpen())
	{
		// only way I can think of to get the profile dialog to reinialise itself is
		// to fake a OnSelChangingMsg message ....

		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::SELECTIONCHANGED, FALSE));
	}

	return Act;
}

/********************************************************************************************
>	ActionCode ChangeFeatherProfileAction::Init(	Operation* 	pOp,
												ActionList* pActionList,
												FeatherAttrValue* 	pFeather,
												INT32 FeatherSize,
												ChangeFeatherProfileAction** 	ppNewAction )
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	= ptr to Operations action list to which this action should be added
				pFeather		= ptr to a feather attribute
				FeatherSize		= New feather size for pFeather
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	SeeAlso:	Action::Init()
	Notes:		Doesn't redraw the Node (either before or after the change).
				Bounding Box doesn't change so you don't need to remove blobs before and after
				this action - ie DoInvalidateNodeBounds(no blob) on either side of this
********************************************************************************************/
ActionCode ChangeFeatherProfileAction::Init( Operation* pOp,
							ActionList* pActionList,
							FeatherAttrValue* pFeather,
							CProfileBiasGain FeatherProfile,
							ChangeFeatherProfileAction** ppNewAction )
{
	UINT32 ActSize = sizeof(ChangeFeatherProfileAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(ChangeFeatherProfileAction),(Action**)ppNewAction);

	if (Ac != AC_FAIL)
	{
		(*ppNewAction)->m_pFeather = pFeather;
		(*ppNewAction)->m_LastProfile = FeatherProfile;
	}

	return Ac;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef FEATHER_EFFECT
/********************************************************************************************
>	ActionCode ChangeFeatherEffectProfileAction::Init(	Operation* 	pOp,
														ActionList* pActionList,
														NodeFeatherEffect* 	pFeatherEffect,
														CProfileBiasGain FeatherProfile,
														ChangeFeatherEffectProfileAction** ppNewAction )
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/2005
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	= ptr to Operations action list to which this action should be added
				pFeatherEffect	= ptr to a feather effect
				FeatherSize		= New feather size for pFeather
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	SeeAlso:	Action::Init()
	Notes:		Doesn't redraw the Node (either before or after the change).
				Bounding Box doesn't change so you don't need to remove blobs before and after
				this action - ie DoInvalidateNodeBounds(no blob) on either side of this
********************************************************************************************/
ActionCode ChangeFeatherEffectProfileAction::Init(	Operation* pOp,
													ActionList* pActionList,
													NodeFeatherEffect* pFeatherEffect,
													CProfileBiasGain FeatherProfile,
													ChangeFeatherEffectProfileAction** ppNewAction )
{
	UINT32 ActSize = sizeof(ChangeFeatherEffectProfileAction);

	ActionCode Ac = Action::Init(pOp,
								 pActionList,
								 ActSize,
								 CC_RUNTIME_CLASS(ChangeFeatherEffectProfileAction),
								 (Action**)ppNewAction);
	if (Ac != AC_FAIL)
	{
		(*ppNewAction)->m_pFeatherEffect = pFeatherEffect;
		(*ppNewAction)->m_LastProfile = FeatherProfile;
	}

	return Ac;
}


/********************************************************************************************
>	ActionCode ChangeFeatherEffectProfileAction::Execute()
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/2005
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.
	Errors:		-
	SeeAlso:	Action::Init()
********************************************************************************************/
ActionCode ChangeFeatherEffectProfileAction::Execute()
{
	ActionCode Act = AC_FAIL;
	ChangeFeatherEffectProfileAction* pAction;

	// First time an instance of this Action is executed is when called during and
	// UndoList->ExecuteBackwards inside an OpUndo invoked OpHist->GetTail()->Undo (where
	// the tail OpHist item is an OpChangeFeatherProfile. SHEW!
	// However may also be called by a Redo as we are about to stick an instance into the
	// Redo list
	CProfileBiasGain UndoProf = m_pFeatherEffect->GetProfile();

	Act = ChangeFeatherEffectProfileAction::Init(pOperation, 
												 pOppositeActLst,
												 m_pFeatherEffect,
												 UndoProf,
												 &pAction );

	if (Act==AC_OK)
		m_pFeatherEffect->SetProfile(m_LastProfile);

	if (ChangeFeatherProfileOpDesc::m_BiasGainGadget.IsDialogOpen())
	{
		// only way I can think of to get the profile dialog to reinialise itself is
		// to fake a OnSelChangingMsg message ....

		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::SELECTIONCHANGED, FALSE));
	}

	return Act;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************
>	ActionCode ChangeFeatherEffectAction::Init(	Operation* 	pOp,
												ActionList* pActionList,
												NodeFeatherEffect* pFeatherEffect,
												MILLIPOINT FeatherSize,
												CProfileBiasGain FeatherProfile,
												ChangeFeatherEffectAction** ppNewAction )
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/2005
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	= ptr to Operations action list to which this action should be added
				pFeatherEffect	= ptr to a feather effect
				FeatherSize		= New feather size for pFeather
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	SeeAlso:	Action::Init()
	Notes:		Doesn't redraw the Node (either before or after the change).
				Bounding Box doesn't change so you don't need to remove blobs before and after
				this action - ie DoInvalidateNodeBounds(no blob) on either side of this
********************************************************************************************/
ActionCode ChangeFeatherEffectAction::Init(	Operation* pOp,
											ActionList* pActionList,
											NodeFeatherEffect* pFeatherEffect,
											MILLIPOINT FeatherSize,
											CProfileBiasGain FeatherProfile,
											ChangeFeatherEffectAction** ppNewAction )
{
	UINT32 ActSize = sizeof(ChangeFeatherEffectAction);

	ActionCode Ac = Action::Init(pOp,
								 pActionList,
								 ActSize,
								 CC_RUNTIME_CLASS(ChangeFeatherEffectAction),
								 (Action**)ppNewAction);
	if (Ac != AC_FAIL)
	{
		(*ppNewAction)->m_pFeatherEffect = pFeatherEffect;
		(*ppNewAction)->m_LastFeatherSize = FeatherSize;
		(*ppNewAction)->m_LastProfile = FeatherProfile;
	}

	return Ac;
}


/********************************************************************************************
>	ActionCode ChangeFeatherEffectAction::Execute()
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/2005
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.
	Errors:		-
	SeeAlso:	Action::Init()
********************************************************************************************/
ActionCode ChangeFeatherEffectAction::Execute()
{
	ActionCode Act = AC_FAIL;
	ChangeFeatherEffectAction* pAction;

	CProfileBiasGain UndoProf = m_pFeatherEffect->GetProfile();
	MILLIPOINT UndoSize = m_pFeatherEffect->GetFeatherSize();

	Act = ChangeFeatherEffectAction::Init(pOperation, 
										  pOppositeActLst,
										  m_pFeatherEffect,
										  UndoSize,
										  UndoProf,
										  &pAction );

	if (Act==AC_OK)
	{
		m_pFeatherEffect->SetFeatherSize(m_LastFeatherSize);
		m_pFeatherEffect->SetProfile(m_LastProfile);
	}

	if (ChangeFeatherProfileOpDesc::m_BiasGainGadget.IsDialogOpen())
	{
		// only way I can think of to get the profile dialog to reinitialise itself is
		// to fake a OnSelChangingMsg message ....
		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::SELECTIONCHANGED, FALSE));
//		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::EFFECTSTACKCHANGED)); 
	}

	return Act;
}
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RegenerateFeatherContourAction::RegenerateFeatherContourAction()
{
	m_pFeather = NULL;
	m_pContour = NULL;
}

RegenerateFeatherContourAction::~RegenerateFeatherContourAction()
{
	// must free path on document deletion when actions slaughtered
	if (m_pContour)
	{
		delete m_pContour;
	}
}

/********************************************************************************************
>	ActionCode RegenerateFeatherContourAction::Execute()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/99
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.
	Errors:		-
	SeeAlso:	Action::Init()
********************************************************************************************/
ActionCode RegenerateFeatherContourAction::Execute()
{
	ActionCode Act = AC_FAIL;
	RegenerateFeatherContourAction* pAction;

	// First time an instance of this Action is executed is when called during and
	// UndoList->ExecuteBackwards inside an OpUndo invoked OpHist->GetTail()->Undo (where
	// the tail OpHist item is an OpChangeFeatherProfile. SHEW!
	// However may also be called by a Redo as we are about to stick an instance into the
	// Redo list
	Path* UndoPath = m_pFeather->GetOuterContour();
	m_pFeather->SetOuterContour(m_pContour);
	m_pContour = NULL;		// prevent ~RegenerateFeatherContourAction from deleting path

	Act = RegenerateFeatherContourAction::Init(	pOperation, 
												pOppositeActLst,
												m_pFeather,
												UndoPath,
												&pAction );

	if (ChangeFeatherProfileOpDesc::m_BiasGainGadget.IsDialogOpen())
	{
		// only way I can think of to get the profile dialog to reinialise itself is
		// to fake a OnSelChangingMsg message ....

		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::SELECTIONCHANGED, FALSE));
	}

	return Act;
}

/********************************************************************************************
>	ActionCode RegenerateFeatherContourAction::Init(	Operation* 	pOp,
												ActionList* pActionList,
												FeatherAttrValue* 	pFeather,
												INT32 FeatherSize,
												RegenerateFeatherContourAction** 	ppNewAction )
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	= ptr to Operations action list to which this action should be added
				pFeather		= ptr to a feather attribute
				FeatherSize		= New feather size for pFeather
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	SeeAlso:	Action::Init()
	Notes:		Doesn't redraw the Node (either before or after the change).
				Bounding Box doesn't change so you don't need to remove blobs before and after
				this action - ie DoInvalidateNodeBounds(no blob) on either side of this
********************************************************************************************/
ActionCode RegenerateFeatherContourAction::Init( Operation* pOp,
							ActionList* pActionList,
							FeatherAttrValue* pFeather,
							Path* pFeatherContour,
							RegenerateFeatherContourAction** ppNewAction )
{
	UINT32 ActSize = sizeof(RegenerateFeatherContourAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(RegenerateFeatherContourAction),(Action**)ppNewAction);

	if (Ac != AC_FAIL && *ppNewAction!=NULL)
	{
		(*ppNewAction)->m_pFeather = pFeather;
		(*ppNewAction)->m_pContour = pFeatherContour;
	}

	return Ac;
}
