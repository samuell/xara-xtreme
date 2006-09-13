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
// FreeHand Tool Info bar operations

/*
*/

#include "camtypes.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h"
//#include "resource.h"
#include "bevinfo.h"
//#include "bevres.h"
#include "attrbev.h"
//#include "range.h"
//#include "resource.h"
//#include "barsdlgs.h"
#include "nbevcont.h"
#include "opbevel.h"
//#include "range.h"
//#include "rikdlg.h"
//#include "rik.h"
//#include "justin2.h"
#include "pen.h"
//#include "unittype.h"
//#include "attrmgr.h"
//#include "attrbev.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "blobs.h"
#include "bevtool.h"
#include "selall.h"
#include "cutop.h"
#include "csrstack.h"
#include "optsmsgs.h"
#include "gdraw2.h"
//#include "biasgain.h"
//#include "mario.h"
#include "ncntrcnt.h"
#include "lineattr.h"
//#include "simon.h"
//#include "camvw.h"
#include "nodebev.h"

DECLARE_SOURCE( "$Revision$" );

// Declare stuff for the memory tracking
CC_IMPLEMENT_DYNCREATE( BevelInfoBarOp, InformationBarOp )
#define new CAM_DEBUG_NEW

struct BevelComboMap
{
	// the ID for the string in the combo
	UINT32 StringID;

	// the number passed to the beveler to determine the type of bevel
	INT32 TypeNum;
} ;

// no. of bevel types including NONE!
INT32 NumBevelTypes = 16;

BevelComboMap BevelTypes[] = 
{
	{ _R(IDS_BEVELNONE), -1 },
	{ _R(IDS_BEVELFLAT), (INT32)BEVEL_FLAT },
	{ _R(IDS_BEVELSMOOTH_1), (INT32)BEVEL_SMOOTH_1 } ,
	{ _R(IDS_BEVELSMOOTH_2), (INT32)BEVEL_SMOOTH_2 } ,
	{ _R(IDS_BEVELMESA_1), (INT32)BEVEL_MESA_1 },
	{ _R(IDS_BEVELMESA_2), (INT32)BEVEL_MESA_2 },
	{ _R(IDS_BEVELROUND), (INT32)BEVEL_ROUND },
	{ _R(IDS_BEVELHALFROUND), (INT32)BEVEL_HALFROUND}, 
	{ _R(IDS_BEVELFRAME), (INT32)BEVEL_FRAME },
	{ _R(IDS_BEVELPOINT_1), (INT32)BEVEL_POINT_1 },
	{ _R(IDS_BEVELPOINT_2A), (INT32)BEVEL_POINT_2a },
	{ _R(IDS_BEVELPOINT_2B), (INT32)BEVEL_POINT_2b },
	{ _R(IDS_BEVELRUFFLE_2A), (INT32)BEVEL_RUFFLE_2a },
	{ _R(IDS_BEVELRUFFLE_2B), (INT32)BEVEL_RUFFLE_2b },
	{ _R(IDS_BEVELRUFFLE_3A), (INT32)BEVEL_RUFFLE_3a },
	{ _R(IDS_BEVELRUFFLE_3B), (INT32)BEVEL_RUFFLE_3b }
} ;

// slider bar types next
INT32 NumSliderTypes = 4;
const UINT32 SliderTypes[] =
{
	_R(IDS_BEVELDEPTH),
	_R(IDS_BEVELCONTRAST),
	_R(IDS_BEVELLIGHTANGLE),
	_R(IDS_BEVELLIGHTTILT)
} ;

// inner/outer combo definition
INT32 NumInnerTypes = 2;
const UINT32 InnerTypes[] =
{
	_R(IDS_BEVELINNER),
	_R(IDS_BEVELOUTER)
} ;

// constraints for the slider
#define BEVEL_DEPTH_MIN 0
#define BEVEL_DEPTH_MAX 250000
#define BEVEL_CONTRAST_MIN 0
#define BEVEL_CONTRAST_MAX 100
#define BEVEL_LIGHTANGLE_MIN 0
#define BEVEL_LIGHTANGLE_MAX 360
#define BEVEL_LIGHTTILT_MIN 0
#define BEVEL_LIGHTTILT_MAX 90

// static for the current bevel info bar
BevelInfoBarOp * BevelInfoBarOp::pCurrentBar = NULL;

///////////////////////////////////////////////////////////
// slider profile
CProfileBiasGain BevelSliderProfile((AFp)0.7, (AFp)0.0);

/********************************************************************************************

>	BevelInfoBarOp::BevelInfoBarOp()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Purpose:	Sets the info bars vars to NULL defaults

********************************************************************************************/

BevelInfoBarOp::BevelInfoBarOp()
{
	DlgResID = _R(IDD_BEVELINFOBAR);
	// The Controls hasve not been created yet
	ControlsExist = FALSE;
	
	// variable to stop controls from flickering
	m_bControlsEnabled = FALSE;

	// slider values
	m_Depth = 0;
	m_LightAngle = 0;
	m_Contrast = 100;
	m_SliderComboIndex = 1;
	m_bComboChanged = FALSE;
	m_bAmDragging = FALSE;
	m_pDragBlob = NULL;
	m_BlobAngle = 0;
	m_bUserSet = FALSE;
	m_UserIndent = FALSE;
	m_bButtonIsInner = FALSE;
	m_bButtonIsOuter = FALSE;
	m_bAmDraggingSlider = FALSE;
	pTool = NULL;
	CanRetroFit = FALSE;
	m_LightTilt = 0;
	m_LastDragWidth = 0;
	m_BetweenViews = FALSE;
}




/********************************************************************************************

>	MsgResult BevelInfoBarOp::Message(Msg* Message) 

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/94
	Inputs:		Message - A dialog message 
	Returns:	The result of the message
	Purpose:	Takes a look at the message and if it is for me it responds to it.
				Here the scroll bar for the freehand tool is Created and closed. It
				also responds to changes in the sliders position and tells the freehand
				tool to set its smoothing factor to something appropriate.

********************************************************************************************/

MsgResult BevelInfoBarOp::Message(Msg* Message) 
{
//	NodeBevelController * pBevel = NULL;
	INT32 value = 0;

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		// Check if the message is a CANCEL
		if (Msg->DlgMsg == DIM_CANCEL)
		{
			// Close the dialog 
			Close();
			m_bComboChanged = FALSE;
		}
		else if (Msg->DlgMsg == DIM_CREATE)
		{
			m_BetweenViews = FALSE;
			// Initialise the controls
			// go see if there is an info bar to use
			if (WindowID!=NULL)
			{
				// Mark the controls as existing
				ControlsExist = TRUE;

				String_64 Str;

				// set the combos					
//				SetLongGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT), m_SliderComboIndex, FALSE, -1);
				SetLongGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT), 0, FALSE, -1);

				// first, clear the combo
				DeleteAllValues( _R(IDC_BEVEL_SLIDER_COMBO));
				
				// now the slider combo
				for ( INT32 i=0; i<NumSliderTypes; i++ )
				{
					Str.Load( SliderTypes[i] );
					SetStringGadgetValue(_R(IDC_BEVEL_SLIDER_COMBO), Str, TRUE, i);
				}
				
				SetComboListLength(_R(IDC_BEVEL_SLIDER_COMBO));

				LoadBevelTypeComboStrings(FALSE);
				LoadBevelDirectionComboStrings(FALSE);

				UpdateControls();
				UpdateJoinTypeControls();
				
				SetGadgetBitmaps(_R(IDC_BEVELSLIDER), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));

				m_bComboChanged = FALSE;
			}

		}
		else
		{
			// Messages to all the controls, handled indivdually
			if (ControlsExist)
			{
				if (WindowID!=NULL)
				{
					if (FALSE) {}
					else if (Msg->GadgetID == _R(IDC_BEVEL_TYPE_COMBO))
					{
						if (Msg->DlgMsg == DIM_SELECTION_CHANGED)
						{
							// ensure values are updated
							HandleBevelTypeChanged();
						}
					}
					else if (Msg->GadgetID == _R(IDC_BEVEL_SLIDER_COMBO))
					{	
						if (Msg->DlgMsg == DIM_SELECTION_CHANGED)
						{
							// ensure values are updated
							SliderUpdate(FALSE);
						}
					}
						/*
					else if (Msg->GadgetID == _R(IDC_BEVEL_INNER_COMBO))
					{
						if (Msg->DlgMsg == DIM_SELECTION_CHANGED)
						{
							// ensure values are updated
							HandleBevelDirectionChanged();
					
						}
					}
						*/
					else if (Msg->GadgetID == _R(IDC_BEVELSLIDER))
					{
						// on drop of slider, change the bevel (if there is one selected !)
						if (Msg->DlgMsg == DIM_SLIDER_POS_SET)
						{
							if (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) == 0)
							{
								pTool->m_pDragBevelOp->RenderDragBlobs(m_LastDragWidth,
									Document::GetSelectedSpread());
							}
							
							HandleSliderChanged();
							m_bAmDraggingSlider = FALSE;
						}
						else if (Msg->DlgMsg == DIM_SLIDER_POS_CHANGING)
						{
							// start the drag on the tool's drag info bar so we
							// can render blobs
							if (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) == 0)
							{
								if (!m_bAmDraggingSlider)
								{
									pTool->m_pDragBevelOp->SetupDragInfo();
									m_bAmDraggingSlider = TRUE;
								}
								else
								{
									// if we've already dragged, then remove the drag
									// blobs which are present
									pTool->m_pDragBevelOp->RenderDragBlobs(m_LastDragWidth,
										Document::GetSelectedSpread());
								}
							}
										
							// is there a 'many' setting ?
							m_bAmDraggingSlider = TRUE;
							if (pTool->AreManyBevelLightAngles() &&
								GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) == 2)
							{
								// set all the blob angles to be the same, and update
								MsgResult Result = InformationBarOp::Message(Message);
								SetGadgetRange(_R(IDC_BEVELSLIDER), BEVEL_LIGHTANGLE_MIN, BEVEL_LIGHTANGLE_MAX);
				
								INT32 Angle = GetLongGadgetValue(_R(IDC_BEVELSLIDER),
														BEVEL_LIGHTANGLE_MIN,
														BEVEL_LIGHTANGLE_MAX);

								// change the angle for the slider stuff
								// Angle = BEVEL_LIGHTANGLE_MAX - Angle;

								// change the blob angles
								pTool->ChangeBlobAngles(Angle);
								
								// update the slider/edit control
								SliderUpdate(TRUE);

								return Result;
							}
							if (pTool->AreManyBevelLightTilts() &&
								GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) == 3)
							{
								// set all the blob angles to be the same, and update
								MsgResult Result = InformationBarOp::Message(Message);
								SetGadgetRange(_R(IDC_BEVELSLIDER), BEVEL_LIGHTTILT_MIN, BEVEL_LIGHTTILT_MAX);
				
								INT32 Angle = GetLongGadgetValue(_R(IDC_BEVELSLIDER),
														BEVEL_LIGHTTILT_MIN,
														BEVEL_LIGHTTILT_MAX);

								// change the angle for the slider stuff
								// Angle = BEVEL_LIGHTTILT_MAX - Angle;

								// change the blob angles
								pTool->ChangeBlobTilts(Angle);
								
								// update the slider/edit control
								SliderUpdate(TRUE);

								return Result;
							}
							else
							{
								SliderUpdate(TRUE);
								
								if (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) == 3)
								{
									RedrawBlobs(FALSE, TRUE);
								}
								else
								{
									RedrawBlobs(TRUE, FALSE);
								}

								// render the width drag blobs if necessary
								if (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) == 0)
								{
									INT32 Width = m_Depth;

									if(!m_bButtonIsInner)
										Width = -Width;

									pTool->m_pDragBevelOp->RenderDragBlobs(Width,Document::GetSelectedSpread());
									m_LastDragWidth = Width;
								}
							}

							// repaint the editable control now
							PaintGadgetNow(_R(IDC_BEVEL_SLIDER_EDIT));
						}
					}	
					else if (Msg->GadgetID == _R(IDC_BEVEL_SLIDER_EDIT))
					{
						if (Msg->DlgMsg == DIM_SELECTION_CHANGED)
						{
							if (ConvertStringToValue(_R(IDC_BEVEL_SLIDER_EDIT), value))
							{
								
								if (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) == 0)
								{
									m_Depth = value;
								}
								else if (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) == 1)
								{
									m_Contrast = value;
								}
								else if (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) == 2)
								{
									m_LightAngle = value;
								}
								else if (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) == 3)
								{
									if (value < 0 || value > 90)
									{
										SliderUpdate(FALSE);
										goto sliderout;
									}

									m_LightTilt = value;
								}
							}
							
							SliderUpdate(FALSE);
							HandleSliderChanged();
							sliderout: {}
						}
					}
					else if (Msg->GadgetID == _R(IDC_BTN_BEVELOUTER))
					{
						if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
						{
							if (!m_bButtonIsOuter)
							{
								// make all outer
								HandleBevelDirectionsToOuter();
							}
							else
							{
								UpdateControls();
							}
						}
					}
					else if (Msg->GadgetID == _R(IDC_BTN_BEVELINNER))
					{
						if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
						{
							if (!m_bButtonIsInner)
							{
								// make all outer
								HandleBevelDirectionsToInner();
							}
							else
							{
								UpdateControls();
							}
						}
					}
					else if (Msg->GadgetID == _R(IDC_BEVELJOINTYPEMITRE))
					{
						if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
						{
							HandleJoinTypeMitreClicked();
						}
					}
					else if (Msg->GadgetID == _R(IDC_BEVELJOINTYPEROUND))
					{
						if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
						{
							HandleJoinTypeRoundClicked();
						}
					}
					else if (Msg->GadgetID == _R(IDC_BEVELJOINTYPEBEVEL))
					{
						if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
						{
							HandleJoinTypeBevelClicked();
						}
					}
				}
			}
		}
	}
	else
	{
		if (Tool::GetCurrentID() == TOOLID_BEVELTOOL)
		{
			if (MESSAGE_IS_A(Message, DocViewMsg))
			{
				// re-render the blobs
				DocViewMsg* msg = (DocViewMsg*) Message;

				// Render the tool blobs off the old view just before it changes
				if ((msg->State == DocViewMsg::SELABOUTTOCHANGE || msg->State == DocViewMsg::SELCHANGED ) 
					&& msg->pOldDocView && !GetApplication()->CamelotIsDying())
				{
					// Do additional checks here to avoid ensures!
					CCamView* pOilView = msg->pOldDocView->GetConnectionToOilView();
					BlobManager* pBlobManager = GetApplication()->GetBlobManager();
					ENSURE(pBlobManager, "Can't get BlobManager");

					PORTNOTE("other", "Removed IsScreenCamView test");
					if (pOilView!=NULL /*&& pOilView->IsScreenCamView()*/)
					{
						// handle the view will/has changed msgs and do NOT process and selection changes msgs between times (sjk 22/11/00)
						if (msg->State == DocViewMsg::SELABOUTTOCHANGE)
						{
							pBlobManager->RenderToolBlobsOff(pTool, msg->pOldDocView->GetVisibleSpread(), NULL);
							pTool->ClearToolBlobList();
							//TRACE( _T("View about to change\n"));
							m_BetweenViews = TRUE;
						}
						else
						{
							pTool->SetupSelectionBlobs();
							pBlobManager->RenderToolBlobsOn(pTool, msg->pOldDocView->GetVisibleSpread(), NULL);
							//TRACE( _T("View has just changed\n"));
							m_BetweenViews = FALSE;
						}
					}
				}
			}
			else if (MESSAGE_IS_A(Message, OpMsg))
			{
				if (((OpMsg *)Message)->MsgType == OpMsg::AFTER_UNDO ||
					((OpMsg *)Message)->MsgType == OpMsg::AFTER_REDO)
				{
					// cope with blob rendering
					if (pTool && BevelTool::AmActive())
					{
						// render blobs back on
						pTool->SetBlobRendering(TRUE);
						pTool->SetupBlobList();
						pTool->RenderToolBlobs(Document::GetSelectedSpread(), NULL);
					}
				}
				else if (((OpMsg *)Message)->MsgType == OpMsg::BEFORE_UNDO ||
					((OpMsg *)Message)->MsgType == OpMsg::BEFORE_REDO)
				{
					// cope with blob rendering
					if (pTool && BevelTool::AmActive())
					{
						// render off blobs
						pTool->RenderToolBlobs(Document::GetSelectedSpread(), NULL);
						pTool->SetBlobRendering(FALSE);
					}
				}
				else if (((OpMsg *)Message)->pOp->IsKindOf(CC_RUNTIME_CLASS(SelectAllOp)))
				{
					// TRACEUSER( "DavidM", _T("Select all op\n"));
					pTool->InvalidateToolBlobs();
				}
				else if (((OpMsg *)Message)->pOp->IsKindOf(CC_RUNTIME_CLASS(SelectNoneOp)))
				{
					pTool->InvalidateToolBlobs();
				}
				else if (((OpMsg *)Message)->pOp->IsKindOf(CC_RUNTIME_CLASS(OpDelete)))
				{
					pTool->InvalidateToolBlobs();
				}
			}
			else if (MESSAGE_IS_A(Message, OptionsChangingMsg))
			{
				OptionsChangingMsg* Msg = (OptionsChangingMsg*)Message;

				if(Msg->State == OptionsChangingMsg::NEWUNITS)
				{		
					// update all controls
					UpdateControls();
				}
			}
			else if (MESSAGE_IS_A(Message, SelChangingMsg))
			{
				UpdateControls();
				SelChangingMsg* pSelChangeMsg = (SelChangingMsg*)Message;
				if (pSelChangeMsg->State == SelChangingMsg::SELECTIONCHANGED && pTool && !m_BetweenViews)
				{	
//					Spread * pSpread = Document::GetSelectedSpread();
					
					if (!pSelChangeMsg->InDifferentDocument)
					{
						pTool->InvalidateToolBlobs();
/*						pTool->RenderToolBlobsOff(pSpread, NULL);
						pTool->ClearToolBlobList();
						pTool->SetupBlobList();
						// render the blobs back on
						pTool->RenderToolBlobsOn(pSpread, NULL);
*/					}

				}

				// Karim 12/09/2000
				// Update our join-type buttons for certain selection changes.
				if (pSelChangeMsg->State == SelChangingMsg::SELECTIONCHANGED ||
					pSelChangeMsg->State == SelChangingMsg::NONCOLOURATTCHANGED)
					UpdateJoinTypeControls();
					EnableDisableJoinTypeControls();
			}
		}
	}

	return (InformationBarOp::Message(Message)); 
}    


/********************************************************************************************

>	void BevelInfoBarOp::SliderUpdate()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/99
	Inputs:		-
	Purpose:	Updates the variables in me depending on the slider position

********************************************************************************************/
void BevelInfoBarOp::SliderUpdate(BOOL bSetVariables, BOOL bTestMany)
{
	if (!ControlsExist)
		return;

	List ContourList;
	BevelTools::BuildListOfSelectedNodes(&ContourList, CC_RUNTIME_CLASS(NodeContourController));
	
	if (!ContourList.IsEmpty())
	{
		ContourList.DeleteAll();
		EnableGadget(_R(IDC_BEVEL_SLIDER_COMBO), FALSE);
		EnableGadget(_R(IDC_BEVELSLIDER), FALSE);
		EnableGadget(_R(IDC_BEVEL_SLIDER_EDIT), FALSE);
		EnableGadget(_R(IDC_BEVEL_TYPE_COMBO), FALSE);
		return;
	}

	TRACEUSER( "DavidM", _T("Slider update !\n"));

	INT32 value = 0;

	String_256 Str;

	TRACEUSER( "DavidM", _T("Slider update\n"));

//	TCHAR unit[] = _T("Units");

	if (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) < 0 ||
		GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) > NumSliderTypes-1)
	{
		SetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO), 1);
	}

	// set the variables from the slider's position
	if (bSetVariables)
	{
		switch (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)))
		{
		case 0:
			// bevel depth update
			SetGadgetRange(_R(IDC_BEVELSLIDER), BEVEL_DEPTH_MIN, BEVEL_DEPTH_MAX);
			
			value = GetSliderDepth();
			m_Depth = value;
			ConvertValueToString(Str,
								 value,
								 PIXELS);
			// Str += _T(" pix");
			SetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT),
								 Str);
			m_SliderComboIndex = 0;
			
			break;
		case 1:
			// bevel contrast update
			SetGadgetRange(_R(IDC_BEVELSLIDER), BEVEL_CONTRAST_MIN, BEVEL_CONTRAST_MAX);
			value = GetLongGadgetValue(_R(IDC_BEVELSLIDER), BEVEL_CONTRAST_MIN,
				BEVEL_CONTRAST_MAX);
			// value = BEVEL_CONTRAST_MAX - value;
			m_Contrast = value;
			SetLongGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT), value);
			Str = GetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT));
			Str += _T(" %");
			SetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT),
								 Str);

			m_SliderComboIndex = 1;

			if (pTool)
				pTool->ChangeContrastAndReRender(m_Contrast);
		
			break;
		case 2:
			// bevel light angle update
			SetGadgetRange(_R(IDC_BEVELSLIDER), BEVEL_LIGHTANGLE_MIN, BEVEL_LIGHTANGLE_MAX);
			value = GetLongGadgetValue(_R(IDC_BEVELSLIDER), BEVEL_LIGHTANGLE_MIN,
				BEVEL_LIGHTANGLE_MAX);

			// value = BEVEL_LIGHTANGLE_MAX - value;

			m_LightAngle = value;
			SetLongGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT), value);
			Str = GetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT));
			Str += _T(" ");
			SetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT),
								 Str);

			m_SliderComboIndex = 2;
			
			break;
		case 3:
			// bevel light tilt update
			SetGadgetRange(_R(IDC_BEVELSLIDER), BEVEL_LIGHTTILT_MIN, BEVEL_LIGHTTILT_MAX);
			value = GetLongGadgetValue(_R(IDC_BEVELSLIDER), BEVEL_LIGHTTILT_MIN,
				BEVEL_LIGHTTILT_MAX);
			// value = BEVEL_LIGHTTILT_MAX - value;
			m_LightTilt = value;
			SetLongGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT), value);
			Str = GetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT));
			Str += _T(" ");
			SetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT),
								 Str);

			m_SliderComboIndex = 3;
			break;
		}
	}
	else
	{
		// set the slider's position from the variables
		switch (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)))
		{
		case 0:
			// bevel depth update
			// now, check for multiple depths of bevels
			// if so, then set the value appropriately
			SetGadgetRange(_R(IDC_BEVELSLIDER), BEVEL_DEPTH_MIN, BEVEL_DEPTH_MAX);

			SetSliderDepth(abs(m_Depth));
			
			if (pTool->AreManyBevelIndents() && bTestMany)
			{
				Str.Load(_R(IDS_BEVELSTRING_MANY));
			}
			else
			{
				ConvertValueToString(Str,
					m_Depth,
					PIXELS);
			}
			
			SetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT),
								 Str);

			PaintGadgetNow(_R(IDC_BEVELSLIDER));
			PaintGadgetNow(_R(IDC_BEVEL_SLIDER_EDIT));		

			m_SliderComboIndex = 0;
			
			break;
		case 1:
			// bevel contrast update
			SetGadgetRange(_R(IDC_BEVELSLIDER), BEVEL_CONTRAST_MIN, BEVEL_CONTRAST_MAX);
			value = m_Contrast;
			//value = BEVEL_CONTRAST_MAX - value;
				
			SetLongGadgetValue(_R(IDC_BEVELSLIDER), value);

			
			if (pTool->AreManyBevelContrasts() && bTestMany)
			{
				Str.Load(_R(IDS_BEVELSTRING_MANY));
			}
			else
			{
				SetLongGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT), m_Contrast);
				Str = GetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT));
				Str += _T(" %");

				// find all selected bevel contrast 
			}

			SetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT),
								 Str);

			m_SliderComboIndex = 1;

			PaintGadgetNow(_R(IDC_BEVELSLIDER));
			PaintGadgetNow(_R(IDC_BEVEL_SLIDER_EDIT));		
			
			break;
		case 2:
			// bevel light angle update
			SetGadgetRange(_R(IDC_BEVELSLIDER), BEVEL_LIGHTANGLE_MIN, BEVEL_LIGHTANGLE_MAX);

			value = m_LightAngle;
				
			SetLongGadgetValue(_R(IDC_BEVELSLIDER), value);
			
			if (pTool->AreManyBevelLightAngles() && bTestMany)
			{
				Str.Load(_R(IDS_BEVELSTRING_MANY));
			}
			else
			{			
				SetLongGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT), BEVEL_LIGHTANGLE_MAX - value);
				Str = GetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT));
				Str += _T(" ");
			}
			
			SetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT),
								 Str);

			m_SliderComboIndex = 2;			

			PaintGadgetNow(_R(IDC_BEVELSLIDER));
			PaintGadgetNow(_R(IDC_BEVEL_SLIDER_EDIT));		
			
			break;
		case 3:
			// bevel light angle update
			SetGadgetRange(_R(IDC_BEVELSLIDER), BEVEL_LIGHTTILT_MIN, BEVEL_LIGHTTILT_MAX);

			value = m_LightTilt;
			// value = BEVEL_LIGHTTILT_MAX - value;
				
			SetLongGadgetValue(_R(IDC_BEVELSLIDER), value);
			
			if (pTool->AreManyBevelLightTilts() && bTestMany)
			{
				Str.Load(_R(IDS_BEVELSTRING_MANY));
			}
			else
			{			
				SetLongGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT), m_LightTilt);
				Str = GetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT));
				Str += _T(" ");
			}
			
			SetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT),
								 Str);

			m_SliderComboIndex = 3;			

			PaintGadgetNow(_R(IDC_BEVELSLIDER));
			PaintGadgetNow(_R(IDC_BEVEL_SLIDER_EDIT));		
			
			break;
		}
	}

	// activate & deactivate gadgets dending on bevel type
	Document* pDoc = Document::GetCurrent();

	if (pDoc)
	{
		if (GetApplication()->FindSelection()->IsEmpty())
		{
			EnableGadget(_R(IDC_BEVEL_SLIDER_COMBO), FALSE);
			EnableGadget(_R(IDC_BEVELSLIDER), FALSE);
			EnableGadget(_R(IDC_BEVEL_SLIDER_EDIT), FALSE);
			EnableGadget(_R(IDC_BEVEL_TYPE_COMBO), FALSE);
		}
		else
		{
			// enable the join-type controls whenever the selection is non-empty.

			if (NodeBevelController::SelectionHasBevelNode() ||
				NodeBevelController::SelectionHasBevelInkNode())
			{
				EnableGadget(_R(IDC_BEVEL_SLIDER_COMBO), TRUE);
				EnableGadget(_R(IDC_BEVELSLIDER), TRUE);
				EnableGadget(_R(IDC_BEVEL_SLIDER_EDIT), TRUE);
				EnableGadget(_R(IDC_BEVEL_TYPE_COMBO), TRUE);
			}
			else
			{
				EnableGadget(_R(IDC_BEVEL_SLIDER_COMBO), TRUE);

				if (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) == 0)
				{
					EnableGadget(_R(IDC_BEVELSLIDER), TRUE);
					EnableGadget(_R(IDC_BEVEL_SLIDER_EDIT), TRUE);
				}
				else
				{
					EnableGadget(_R(IDC_BEVELSLIDER), FALSE);
					EnableGadget(_R(IDC_BEVEL_SLIDER_EDIT), FALSE);
				}

				if (OpCreateBevel::GetState(NULL, NULL).Greyed)
				{
					EnableGadget(_R(IDC_BEVEL_TYPE_COMBO), FALSE);
				}
				else
				{
					EnableGadget(_R(IDC_BEVEL_TYPE_COMBO), TRUE);
				}
			}
		}
	}
	else
	{
		EnableGadget(_R(IDC_BEVEL_SLIDER_COMBO), FALSE);
		EnableGadget(_R(IDC_BEVELSLIDER), FALSE);
		EnableGadget(_R(IDC_BEVEL_SLIDER_EDIT), FALSE);
		EnableGadget(_R(IDC_BEVEL_TYPE_COMBO), FALSE);
	}

	// do bubble help
	switch (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)))
	{
	case 0:
		SetGadgetHelp(_R(IDC_BEVELSLIDER), _R(IDBBL_BEVELSLIDERDEPTH), _R(IDS_BEVELSLIDERDEPTHID));
		SetGadgetHelp(_R(IDC_BEVEL_SLIDER_EDIT), _R(IDBBL_BEVELSLIDERDEPTH), _R(IDS_BEVELSLIDERDEPTHID));
		break;
	case 1:
		SetGadgetHelp(_R(IDC_BEVELSLIDER), _R(IDBBL_BEVELSLIDERCONTRAST), _R(IDS_BEVELSLIDERCONTRASTID));
		SetGadgetHelp(_R(IDC_BEVEL_SLIDER_EDIT), _R(IDBBL_BEVELSLIDERCONTRAST), _R(IDS_BEVELSLIDERCONTRASTID));
		break;
	case 2:
		SetGadgetHelp(_R(IDC_BEVELSLIDER), _R(IDBBL_BEVELSLIDERLIGHTANGLE), _R(IDS_BEVELSLIDERLIGHTANGLEID));
		SetGadgetHelp(_R(IDC_BEVEL_SLIDER_EDIT), _R(IDBBL_BEVELSLIDERLIGHTANGLE), _R(IDS_BEVELSLIDERLIGHTANGLEID));
		break;
	case 3:
		SetGadgetHelp(_R(IDC_BEVELSLIDER), _R(IDBBL_BEVELSLIDERTILT), _R(IDS_BEVELSLIDERTILTID));
		SetGadgetHelp(_R(IDC_BEVEL_SLIDER_EDIT), _R(IDBBL_BEVELSLIDERTILT), _R(IDS_BEVELSLIDERTILTID));
		break;
	}

	SetGadgetHelp(_R(IDC_BEVEL_TYPE_COMBO), _R(IDBBL_BEVELTYPECOMBO), _R(IDS_BEVELTYPECOMBO));
	SetGadgetHelp(_R(IDC_BEVEL_SLIDER_COMBO), _R(IDBBL_BEVELSLIDERCOMBO), _R(IDS_BEVELSLIDERCOMBO));
}

/********************************************************************************************

>	void BevelInfoBarOp::DoBevel()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/99
	Inputs:		-
	Purpose:	Does the business with the bevelling !

********************************************************************************************/
void BevelInfoBarOp::DoBevel()
{
	// create a bevel
	BevelInfo BI;

	OpDescriptor* pOpDesc = NULL;

	if(m_Depth == 0)
	{
		SetSelectedValueIndex(_R(IDC_BEVEL_TYPE_COMBO),0);
	}

	BI.m_BevelType = BevelTypes[GetSelectedValueIndex(_R(IDC_BEVEL_TYPE_COMBO))].TypeNum;
	BI.m_bBevelTypeChanged = TRUE;
	BI.m_LightAngle = m_LightAngle;
	BI.m_Indent = m_Depth;

	if (pTool->AreManyBevelTypes())
	{
		// ok, 'many' is selected so alter the type accordingly
		BI.m_BevelType = BevelTypes[GetSelectedValueIndex(_R(IDC_BEVEL_TYPE_COMBO)) - 1].TypeNum;
	}

	TRACEUSER( "DavidM", _T("Bevel type change %d\n"), BI.m_BevelType);

	BlobManager* BlobMgr = GetApplication()->GetBlobManager();

	BI.m_bOuter = GetBoolGadgetSelected(_R(IDC_BTN_BEVELOUTER));
	BI.m_Contrast	= m_Contrast;
	BI.m_bCreateNewBevels = TRUE;

	Spread * pSpread = Document::GetSelectedSpread();

	if (BI.m_BevelType >= 0)
	{
		pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpCreateBevel));
		
		if (pTool && pSpread && BlobMgr)
		{
			pTool->RenderToolBlobs(pSpread, NULL);
			pTool->SetBlobRendering(FALSE);
		}
		
		if (pOpDesc)
		{
			pOpDesc->Invoke(&BI);
		}

		if (pTool && pSpread && BlobMgr)
		{
			pTool->SetupBlobList();
			pTool->SetBlobRendering(TRUE);
			pTool->RenderToolBlobs(pSpread, NULL);
		}
	}
	else
	{
		// do a remove bevel operation !
		// tell my tool to remove its blobs
		pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpRemoveBevel));
		
		// tell my tool to update its blobs
	
		if (pTool && pSpread && BlobMgr)
		{
			// turn off all the tool blobs
			pTool->RenderToolBlobs(pSpread, NULL);
			pTool->SetBlobRendering(FALSE);
		}	
	
		if (pOpDesc)
		{
			pOpDesc->Invoke();
		}

		if (pTool && pSpread && BlobMgr)
		{
			pTool->SetBlobRendering(TRUE);
			pTool->SetupBlobList();
			pTool->RenderToolBlobs(pSpread, NULL);
		}	
	
	}

	UpdateControls();
}

/********************************************************************************************

>	void BevelInfoBarOp::SetToolActiveState(BOOL IsToolActive)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		IsToolActive - TRUE if the tool is becoming active, FALSE if it not
	Purpose:	Allows the info bar to know if it is valid or not

********************************************************************************************/

void BevelInfoBarOp::SetToolActiveState(BOOL IsToolActive)
{
	// Set the control state
	m_bComboChanged = FALSE;
	ControlsExist = IsToolActive;
		
}



/********************************************************************************************

>	void BevelInfoBarOp::SetRetroState(BOOL CanRetroFit)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Inputs:		CanRetroFit - TRUE if we can currently retro fit the path, FALSE if not
	Purpose:	Sets the Flag CanRetroFit and gets the infobar to redraw the static
				text item that says this.

********************************************************************************************/

void BevelInfoBarOp::SetRetroState(BOOL CanFit)
{
	// Set the Retro field to be empty by default
	CanRetroFit = CanFit;

	// Set the string in the info bar now
	SetRetroString();
}


/********************************************************************************************

>	void BevelInfoBarOp::SetRetroString()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Purpose:	Sets the static text item in the freehand info bar to say either 'Retro' or
				nothing, depending on the state of the flag CanRetroFit.
	SeeAlso:	BevelInfoBarOp::SetRetroState

********************************************************************************************/

void BevelInfoBarOp::SetRetroString()
{
	// go see if there is an info bar to use
	/*
	if (WindowID!=NULL)
	{
		if (CanRetroFit)
		{
			// Make the word Retro appear
			String_64 RetroWord;
			if (RetroWord.Load(_R(IDS_FHANDRETROTEXT)))
			{
				// Set the contents of the field to indicate retro is possible
				SetStringGadgetValue(_R(IDC_FREEHANDRETRO), &RetroWord);
			}
		}
		else
		{
			// make the word retro not appear
			String_8 RetroWord("");
			SetStringGadgetValue(_R(IDC_FREEHANDRETRO), &RetroWord);
		}
	}
	*/
}

/********************************************************************************************

>	void BevelInfoBarOp::UpdateControls(BOOL bResetSliderCombo)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/99

	Purpose:	Updates the controls on the info bar

********************************************************************************************/

void BevelInfoBarOp::UpdateControls()
{
	// Check to see if we`ve got any contours in the selection!
	BOOL HaveGotContours = FALSE;
	List ContourList;
	BevelTools::BuildListOfSelectedNodes(&ContourList, CC_RUNTIME_CLASS(NodeContourController));
	
	if (!ContourList.IsEmpty())
	{
		ContourList.DeleteAll();
		HaveGotContours = TRUE;
	}

	if (!ControlsExist || !pTool)
		return;

	// these two MUST be done in this order otherwise camelot falls over on entry
	if (!HasWindow())
		return;

	TRACEUSER( "DavidM", _T("Update controls\n"));

	NodeBevel			* pBevel	= NULL;
//	NodeAttribute		* pNode		= NULL;
	AttrBevelIndent		* pIndent	= NULL;
	AttrBevelLightAngle * pLightAngle = NULL;
	AttrBevelType		* pType		= NULL;
	AttrBevelContrast	* pContrast	= NULL;
	AttrBevelLightTilt * pTilt = NULL;

	// Obtain the current selections 
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.SetRangeControl(rg);

	// the attribute list
	List NodeList;
	NodeListItem * pItem = NULL;

	// build a list of all the bevel attribute nodes in the selection
	BevelTools::BuildListOfSelectedNodes(&NodeList,
							CC_RUNTIME_CLASS(NodeBevel), TRUE);

	AttrBevelIndent		* pCurrentIndent			= NULL;
	AttrBevelLightAngle * pCurrentLightAngle		= NULL;
	AttrBevelContrast	* pCurrentContrast			= NULL;
//	AttrBevelType		* pCurrentType				= NULL;
	AttrBevelLightTilt	* pCurrentTilt				= NULL;

//	NodeBevelController * pController = NULL;

	Document* pDoc = Document::GetCurrent();

	if (NodeList.IsEmpty() || HaveGotContours)
	{
		// means we need to do the default attributes
		if (pDoc)
		{
			// reset the 'many'
			LoadBevelTypeComboStrings(FALSE);
			LoadBevelDirectionComboStrings(FALSE);
			
			// has the user set the variables ??

			pCurrentIndent = (AttrBevelIndent *)pDoc->GetAttributeMgr().GetCurrentAttribute(
				CC_RUNTIME_CLASS(NodeRenderableInk),
				CC_RUNTIME_CLASS(AttrBevelIndent));
			
			if (pCurrentIndent)
			{
				m_Depth = pCurrentIndent->Value.m_Indent;
				
				// just do this the first time
				m_bUserSet = TRUE;
				m_UserIndent = m_Depth;
			}
		
			if (m_Depth < 0)
				m_Depth = - m_Depth;
			
			pCurrentLightAngle = (AttrBevelLightAngle *)pDoc->GetAttributeMgr().GetCurrentAttribute(
							CC_RUNTIME_CLASS(NodeRenderableInk),
							CC_RUNTIME_CLASS(AttrBevelLightAngle));

			if (pCurrentLightAngle)
			{
				m_LightAngle = pCurrentLightAngle->Value.m_LightAngle;
			}
			
			SetSelectedValueIndex(_R(IDC_BEVEL_TYPE_COMBO), 
				0);
			
			pCurrentContrast = (AttrBevelContrast *)pDoc->GetAttributeMgr().GetCurrentAttribute(
				CC_RUNTIME_CLASS(NodeRenderableInk),
				CC_RUNTIME_CLASS(AttrBevelContrast));
			
			if (pCurrentContrast)
			{
				m_Contrast = pCurrentContrast->Value.m_Contrast;
			}

			pCurrentTilt = (AttrBevelLightTilt *)pDoc->GetAttributeMgr().GetCurrentAttribute(
				CC_RUNTIME_CLASS(NodeRenderableInk),
				CC_RUNTIME_CLASS(AttrBevelLightTilt));

			if (pCurrentTilt)
			{
				m_LightTilt = pCurrentTilt->Value.m_LightTilt;
			}
		}

		UpdateInnerOuterButtons(FALSE, FALSE, TRUE);
		//UpdateJoinTypeButtons () ....
		EnableGadget(_R(IDC_BEVELJOINTYPEMITRE), FALSE);
		EnableGadget(_R(IDC_BEVELJOINTYPEROUND), FALSE);
		EnableGadget(_R(IDC_BEVELJOINTYPEBEVEL), FALSE);
	}
	else
	{
		m_bControlsEnabled = TRUE;
		
		pIndent		= NULL;
		pLightAngle = NULL;
		pContrast	= NULL;
		pType		= NULL;

		// get the head of the list
		pItem = (NodeListItem *)NodeList.GetHead();
		
		// get all the applied attributes of the first node
		if (pItem && (!pIndent || !pLightAngle || !pType || !pContrast))
		{
			if (pItem->pNode)
			{
				pBevel = (NodeBevel *)pItem->pNode;
				
				pBevel->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelIndent),
					(NodeAttribute **)(&pIndent));

				pBevel->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelLightAngle),
					(NodeAttribute **)(&pLightAngle));

				pBevel->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelContrast),
					(NodeAttribute **)(&pContrast));

				pBevel->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelType),
					(NodeAttribute **)(&pType));

				pBevel->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelLightTilt),
					(NodeAttribute **)(&pTilt));
			}
			
			pItem = (NodeListItem *)NodeList.GetNext(pItem);
		}
		
		// set up the gadgets
		if (pType)
		{
			// have we a 'many' setting ?
			if (pTool->AreManyBevelTypes())
			{
				LoadBevelTypeComboStrings(TRUE);
				SetSelectedValueIndex(_R(IDC_BEVEL_TYPE_COMBO), 0);
			}
			else
			{
				LoadBevelTypeComboStrings(FALSE);
				for (INT32 i = 0 ; i < NumBevelTypes; i++)
				{
					if (pType->Value.m_Type == BevelTypes[i].TypeNum)
						SetSelectedValueIndex(_R(IDC_BEVEL_TYPE_COMBO), i);
				}
			}
		}
		else
		{
			// no type found so set to "none"
			LoadBevelTypeComboStrings(FALSE);
			SetSelectedValueIndex(_R(IDC_BEVEL_TYPE_COMBO), 0);
		}
		
		if (pIndent)
		{
			if (pTool->AreManyBevelDirections())
			{
				UpdateInnerOuterButtons(TRUE, FALSE, FALSE);
			}
			else
			{				
				if (pIndent->Value.m_Indent < 0)
				{
					UpdateInnerOuterButtons(FALSE, TRUE, FALSE);
				}
				else
				{
					UpdateInnerOuterButtons(FALSE, FALSE, FALSE);
				}
			}
			
			SetLongGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT), abs(pIndent->Value.m_Indent), FALSE, -1);
		}
		
		SetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO), m_SliderComboIndex);
		
		if (pIndent)
		{
			m_Depth = abs(pIndent->Value.m_Indent);
		}
		else
		{
			m_Depth = 0;
		}
		
		if (pLightAngle)
		{
			// take it from the attribute
//			m_LightAngle = pLightAngle->Value.m_LightAngle;
			m_LightAngle = BEVEL_LIGHTANGLE_MAX - pLightAngle->Value.m_LightAngle;
		}
		
		if (pContrast)
		{
			m_Contrast = pContrast->Value.m_Contrast;
		}
		else
		{
			m_Contrast = 0;
		}

		if (pTilt)
		{
			m_LightTilt = pTilt->Value.m_LightTilt;
		}
		else
		{
			m_LightTilt = 45;
		}
	}

	// clean up the list
	NodeList.DeleteAll();

	SliderUpdate(FALSE);
}

/********************************************************************************************

>	virtual void BevelInfoBarOp::UpdateState()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97

	Purpose:	Sets the state of button on the bar to keep them up to date

********************************************************************************************/

void BevelInfoBarOp::UpdateState()
{
	// Don't try to set our controls when they're not there!
	TRACEUSER( "DavidM", _T("BevelInfoBarOp::UpdateState\n"));

	if (!ControlsExist)
		return;

	UpdateControls();

	return;

}


/********************************************************************************************

>	BOOL SoftShadowInfoBarOp::ConvertValueToString(String_256 &In, const INT32 value, UnitType type, String_256 &unit)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/12/96
	Inputs:		The value to convert to a string (In)
	Outputs:	-
	Returns:	-
	Purpose:	TRUE if the function was able to convert the INT32 to a string

********************************************************************************************/

BOOL BevelInfoBarOp::ConvertValueToString(String_256 &In, const INT32 value, UnitType type)
{
	BOOL Converted = FALSE;	// True when a value has been converted
	
	Spread * CurrentSpread = Document::GetSelectedSpread();

	if (CurrentSpread)
	{
		// Get the dimension scaling object (units) associated with the given spread
		// and convert to its units.
		DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
		pDimScale->ConvertToUnits((INT32)value , &In);
		Converted = TRUE;
	}

	if (In.Length() == 0)
	{
		In = _T("0");
	}

	return Converted;
		
}


/********************************************************************************************

>	BOOL BevelInfoBarOp::ConvertStringToValue(CGadgetID ID, INT32 &Value)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/99
	Inputs:		The value to convert to a string (In)
	Outputs:	-
	Returns:	-
	Purpose:	TRUE if the function was able to convert the INT32 to a string
	See Also:	ShadowBarOp::ConvertStringToValue()

********************************************************************************************/

BOOL BevelInfoBarOp::ConvertStringToValue(CGadgetID ID, INT32 &Value)
{
	String_64 Str;
	Str.Load(_R(IDS_BEVELSTRING_MANY));

	BOOL Valid = FALSE;	// True when a value has been converted

	Spread * CurrentSpread = Document::GetSelectedSpread();

	DimScale* pDimScale = NULL;
	String_128 FieldContents;
	String_128 NumberString;
	INT32 i = 0;
	BOOL bUpdate = FALSE;

	// find out if it reads 'many'
	FieldContents = GetStringGadgetValue(ID, &Valid);

	if (Str == FieldContents)
		return FALSE;

	if (CurrentSpread != NULL)
	{
		// Get the dimension scaling object (units) associated with the given spread
		// and convert to its units.
		switch (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)))
		{
		case 0:
			// depth
			pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
			//String_128 FieldContents = GetStringGadgetValue(_R(IDC_SOFTSHADOWCENTRE_EDIT_X), &Valid);
			FieldContents = GetStringGadgetValue(ID, &Valid);
			Valid = pDimScale->ConvertToMillipoints(FieldContents, &Value);

			if (Value > BEVEL_DEPTH_MAX)
				Value = BEVEL_DEPTH_MAX;
			break;
		case 1:
			FieldContents = GetStringGadgetValue(ID, &Valid);
			
			for (i = 0 ; i < FieldContents.Length(); i++)
			{
				if (FieldContents[i] >= '0' && FieldContents[i] <= '9')
				{
					NumberString += FieldContents[i];
				}
				else
				{
					i = FieldContents.Length();
				}
			}
			
			SetStringGadgetValue(ID, NumberString);
			Value = GetLongGadgetValue(ID, BEVEL_CONTRAST_MIN, BEVEL_CONTRAST_MAX);
			SetStringGadgetValue(ID, FieldContents);
			break;
		case 2:
		case 3:
			FieldContents = GetStringGadgetValue(ID, &Valid);
			
			for (i = 0 ; i < FieldContents.Length(); i++)
			{
				if (FieldContents[i] >= '0' && FieldContents[i] <= '9')
				{
					NumberString += FieldContents[i];
				}
				else
				{
					// deal with a negative angle
					if (i == 0 && FieldContents[i] == '-')
					{
						NumberString += FieldContents[i];
					}
					else
					{						
						i = FieldContents.Length();
					}
				}
			}
			
			SetStringGadgetValue(ID, NumberString);
			Value = GetLongGadgetValue(ID, -1000, 1000);
			
			while (Value < BEVEL_LIGHTANGLE_MIN)
			{
				bUpdate = TRUE;
				Value += 360;
			}
			
			while (Value > BEVEL_LIGHTANGLE_MAX)
			{
				bUpdate = TRUE;
				Value -= 360;
			}
			
			if (!bUpdate)
			{
				SetStringGadgetValue(ID, FieldContents);
			}
			else
			{
				SetLongGadgetValue(ID, Value);
				NumberString = GetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT));
				NumberString += _T(" ");
				SetStringGadgetValue(_R(IDC_BEVEL_SLIDER_EDIT),
					NumberString);
			}		
			
			break;
		}
	}
	else
	{ 
		//TODO ??
	}
	
	return Valid;
}

/********************************************************************************************

>	void BevelInfoBarOp::HandleBevelTypeChanged()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Updates the bevel type
	
********************************************************************************************/

void BevelInfoBarOp::HandleBevelTypeChanged()
{
	if (!pTool)
		return;
	
	if(m_Depth == 0)
	{
		m_Depth = 3000;
		SetBoolGadgetSelected(_R(IDC_BTN_BEVELOUTER),TRUE);
	}

	DoBevel();
}

/********************************************************************************************

>	void BevelInfoBarOp::DoBevelAttrChange(BevelInfo * pBevParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Applies the attributes to the selection
	
********************************************************************************************/
void BevelInfoBarOp::DoBevelAttrChange(BevelInfo * pBevParam)
{
	// use the create bevel (which is kinda an universal function) to change all the
	// indents in the selection
	Spread * pSpread = Document::GetSelectedSpread();

	pBevParam->m_bCreateNewBevels = FALSE;

	// render the tool blobs off
	if (pTool && pSpread)
	{
		pTool->RenderToolBlobs(pSpread, NULL);
		pTool->SetBlobRendering(FALSE);
	}		

	OpDescriptor * pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpCreateBevel));
	
	if (pOpDesc)
	{
		pOpDesc->Invoke(pBevParam);
	}	

	// render them back on
	// render the tool blobs off
	if (pTool && pSpread)
	{
		pTool->SetBlobRendering(TRUE);
		pTool->SetupBlobList();
		pTool->RenderToolBlobs(pSpread, NULL);
	}		

	
}

/********************************************************************************************

>	void BevelInfoBarOp::HandleSliderChanged()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Updates the bevel type
	
********************************************************************************************/

void BevelInfoBarOp::HandleSliderChanged()
{
	// has the edit field got 'many' in it ? if so, exit
	String_64 Str;

	// find out if the selection is null
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.SetRangeControl(rg);

	// update sliders & member variables first
	BevelInfo BI;
	
	SetUpBevelInfo(&BI);

//	AttrBevelIndent     *pIndent      = NULL;
//	AttrBevelType	    *pType	      = NULL;
//	AttrBevelContrast   * pContrast   = NULL;
//	AttrBevelLightAngle * pLightAngle = NULL;

	switch (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)))
	{
	case 0:
		// are there no bevels in the selection ? if so, make it an outer flat bevel
		if (!(NodeBevelController::SelectionHasBevelNode() ||
			NodeBevelController::SelectionHasBevelInkNode()) ||
			m_Depth == 0)
		{
			if(m_Depth > 0 && BevelTypes[GetSelectedValueIndex(_R(IDC_BEVEL_TYPE_COMBO))].TypeNum == -1)
			{
				SetSelectedValueIndex(_R(IDC_BEVEL_TYPE_COMBO),1);
				UpdateInnerOuterButtons(FALSE,TRUE,FALSE);
			}

			DoBevel();
			return;
		}
		
		// the depth/indent setting
		BI.m_bBevelIndentChanged = TRUE;
		m_bUserSet = TRUE;
		m_UserIndent = BI.m_Indent;
		
		if (BI.m_bOuter)
		{
			m_UserIndent = -m_UserIndent;
		}
		
		break;
	case 1:
		// the contrast setting
		BI.m_bBevelContrastChanged = TRUE;
		break;
	case 2:
		BI.m_bBevelLightAngleChanged = TRUE;
		break;
	case 3:
		BI.m_bBevelLightTiltChanged = TRUE;
		break;
	default:
		ERROR3("Bevel infobar slider combo has invalid value");
		return;
	}	
	
	// render the blobs off
//	Spread * pSpread = Document::GetSelectedSpread();

	DoBevelAttrChange(&BI);	
}

/********************************************************************************************

>	void BevelInfoBarOp::RedrawBlobs(BOOL bAngle, BOOL bTilt)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/99
	Inputs:		bAngle		=	To do change in light angles
				bTilt		=	To do change in tilts
	Outputs:	-
	Returns:	-
	Purpose:	Redraws the object blobs
	
********************************************************************************************/
void BevelInfoBarOp::RedrawBlobs(BOOL bAngle, BOOL bTilt)
{
	if (bAngle)
	{	
		if (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) != 2)
			return;
		
		INT32 LightAngle = 0;
		LightAngle = GetLongGadgetValue(_R(IDC_BEVELSLIDER), BEVEL_LIGHTANGLE_MIN,
			BEVEL_LIGHTANGLE_MAX);

		// LightAngle = BEVEL_LIGHTANGLE_MAX - LightAngle;
		
		if (pTool)
		{
			pTool->ChangeBlobAngles(LightAngle);
		}
	}
	else if (bTilt)
	{
		if (GetSelectedValueIndex(_R(IDC_BEVEL_SLIDER_COMBO)) != 3)
			return;

		INT32 Tilt = 0;

		Tilt = GetLongGadgetValue(_R(IDC_BEVELSLIDER), BEVEL_LIGHTTILT_MIN,
			BEVEL_LIGHTTILT_MAX);
		// Tilt = BEVEL_LIGHTTILT_MAX - Tilt;
		
		if (pTool)
		{
			pTool->ChangeBlobTilts(Tilt);
		}
	}
}

/********************************************************************************************

>	void BevelInfoBarOp::HandleBevelDirectionsToOuter()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Updates the bevel direction
	
********************************************************************************************/
void BevelInfoBarOp::HandleBevelDirectionsToOuter()
{
		// update my variables first
	BevelInfo BI;
	
	SetUpBevelInfo(&BI);

	BI.m_bBevelDirectionChanged = TRUE;
	BI.m_bOuter = TRUE;

	// check for zero selection
	Range Sel(*(GetApplication()->FindSelection()));	
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.SetRangeControl(rg);

	AttrBevelIndent * pIndent = NULL;
	if (Sel.IsEmpty())
	{
		// do the default attribute
		pIndent = new AttrBevelIndent;

		if (!pIndent)
		{
			ERROR3("Can't create bevel indent");
			return;
		}

		pIndent->Value.m_Indent = BI.m_Indent;

		m_bUserSet = TRUE;
		m_UserIndent = BI.m_Indent;

		if (BI.m_bOuter)
		{
			pIndent->Value.m_Indent = -BI.m_Indent;
			m_UserIndent = -m_UserIndent;
		}

		AttributeManager::AttributeSelected(pIndent);

		return;
	}

	// get the operation descriptor for the bevel change
	DoBevelAttrChange(&BI);

}

/********************************************************************************************

>	void BevelInfoBarOp::HandleBevelDirectionsToInner()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Updates the bevel direction
	
********************************************************************************************/
void BevelInfoBarOp::HandleBevelDirectionsToInner()
{
	// update my variables first
	BevelInfo BI;
	
	SetUpBevelInfo(&BI);

	BI.m_bBevelDirectionChanged = TRUE;
	BI.m_bOuter = FALSE;

	// check for zero selection
	Range Sel(*(GetApplication()->FindSelection()));	
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.SetRangeControl(rg);

	AttrBevelIndent * pIndent = NULL;
	if (Sel.IsEmpty())
	{
		// do the default attribute
		pIndent = new AttrBevelIndent;
		if (!pIndent)
		{
			ERROR3("Can't create bevel indent");
			return ;
		}

		pIndent->Value.m_Indent = BI.m_Indent;

		m_bUserSet = TRUE;
		m_UserIndent = BI.m_Indent;

		if (BI.m_bOuter)
		{
			pIndent->Value.m_Indent = -BI.m_Indent;
			m_UserIndent = -m_UserIndent;
		}

		AttributeManager::AttributeSelected(pIndent);

		return;
	}

	// get the operation descriptor for the bevel change
	DoBevelAttrChange(&BI);
}




/********************************************************************************************

>	void BevelInfoBarOp::HandleBevelDirectionChanged()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Updates the bevel direction
	
********************************************************************************************/

void BevelInfoBarOp::HandleBevelDirectionChanged()
{
	// check for 'many' being selected
	/*
	if (pTool->AreManyBevelDirections())
	{
		if (GetSelectedValueIndex(_R(IDC_BEVEL_INNER_COMBO)) == 0)
		{
			// do nothing - '<Many>' has been selected again
			return;
		}
	}
	*/
	
	// update my variables first
	BevelInfo BI;
	
	SetUpBevelInfo(&BI);

	BI.m_bBevelDirectionChanged = TRUE;

	// check for zero selection
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.SetRangeControl(rg);

	AttrBevelIndent * pIndent = NULL;
	if (Sel.IsEmpty())
	{
		// do the default attribute
		pIndent = new AttrBevelIndent;

		if (!pIndent)
		{
			ERROR3("Can't create bevel indent");
			return;
		}

		pIndent->Value.m_Indent = BI.m_Indent;

		m_bUserSet = TRUE;
		m_UserIndent = BI.m_Indent;

		if (BI.m_bOuter)
		{
			pIndent->Value.m_Indent = -BI.m_Indent;
			m_UserIndent = -m_UserIndent;
		}

		AttributeManager::AttributeSelected(pIndent);

		return;
	}

	// get the operation descriptor for the bevel change
	DoBevelAttrChange(&BI);
}



/********************************************************************************************

>	void BevelInfoBarOp::HandleJoinTypeMitreClicked()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08 September 2000

	Outputs:	We ensure that all objects in the selection have a mitre join type.

	Purpose:	Handles the user clicking on the mitre join-type button on our infobar.

	Notes:		This method assumes that an object must have a join type;
				ie	if none of the other join types apply to the selection,
					then the whole selection must have this join type, so there
					is no point in reapplying it in this case.

********************************************************************************************/
void BevelInfoBarOp::HandleJoinTypeMitreClicked()
{
	BOOL fRound = GetBoolGadgetSelected(_R(IDC_BEVELJOINTYPEROUND));
	BOOL fBevel = GetBoolGadgetSelected(_R(IDC_BEVELJOINTYPEBEVEL));

	// only bother changing to a mitre join type if any selected objects
	// use the other join types.
	if (fRound || fBevel)
		ChangeJoinType(MitreJoin);
	else
		SetBoolGadgetSelected(_R(IDC_BEVELJOINTYPEMITRE), TRUE);
}



/********************************************************************************************

>	void BevelInfoBarOp::HandleJoinTypeRoundClicked()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08 September 2000

	Outputs:	We ensure that all objects in the selection have a round join type.

	Purpose:	Handles the user clicking on the round join-type button on our infobar.

	Notes:		This method assumes that an object must have a join type;
				ie	if none of the other join types apply to the selection,
					then the whole selection must have this join type, so there
					is no point in reapplying it in this case.

********************************************************************************************/
void BevelInfoBarOp::HandleJoinTypeRoundClicked()
{
	BOOL fMitre = GetBoolGadgetSelected(_R(IDC_BEVELJOINTYPEMITRE));
	BOOL fBevel = GetBoolGadgetSelected(_R(IDC_BEVELJOINTYPEBEVEL));

	// only bother changing to a mitre join type if any selected objects
	// use the other join types.
	if (fMitre || fBevel)
		ChangeJoinType(RoundJoin);
	else
		SetBoolGadgetSelected(_R(IDC_BEVELJOINTYPEROUND), TRUE);
}



/********************************************************************************************

>	void BevelInfoBarOp::HandleJoinTypeBevelClicked()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08 September 2000

	Outputs:	We ensure that all objects in the selection have a bevel join type.

	Purpose:	Handles the user clicking on the bevel join-type button on our infobar.

	Notes:		This method assumes that an object must have a join type;
				ie	if none of the other join types apply to the selection,
					then the whole selection must have this join type, so there
					is no point in reapplying it in this case.

********************************************************************************************/
void BevelInfoBarOp::HandleJoinTypeBevelClicked()
{
	BOOL fRound = GetBoolGadgetSelected(_R(IDC_BEVELJOINTYPEROUND));
	BOOL fMitre = GetBoolGadgetSelected(_R(IDC_BEVELJOINTYPEMITRE));

	// only bother changing to a mitre join type if any selected objects
	// use the other join types.
	if (fRound || fMitre)
		ChangeJoinType(BevelledJoin);
	else
		SetBoolGadgetSelected(_R(IDC_BEVELJOINTYPEBEVEL), TRUE);
}



/********************************************************************************************

>	void BevelInfoBarOp::ChangeJoinType(JointType jt)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11 September 2000
	Inputs:		jt	the join type to apply to the selection.

	Outputs:	An attribute describing the given join type is applied to the selection.

	Purpose:	Apply the given join type to the selection.

	Errors:		Shows a warning if we run out of memory.

	See also:	AttributeManager::AttributeSelected().

********************************************************************************************/
void BevelInfoBarOp::ChangeJoinType(JointType jt)
{
	// Create the appropriate join type attribute.
	AttrJoinType* pJoinAttr = new AttrJoinType;
	if (pJoinAttr == NULL)
	{
		InformWarning(_R(IDS_UNDO_MEMORY_FAILURE), _R(IDS_OK));
		return;
	}
	pJoinAttr->Value.JoinType = jt;

	// Let the attribute manager apply it to the selection.
	AttributeManager::AttributeSelected(pJoinAttr);
}



/********************************************************************************************

>	void BevelInfoBarOp::UpdateJoinTypeControls()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11 September 2000

	Outputs:	The join type buttons on the infobar are updated to reflect the join types
				contained in the selection.

	Purpose:	Update our infobar join type buttons.
				This method is currently called only in response to a SelChanging message.

********************************************************************************************/
void BevelInfoBarOp::UpdateJoinTypeControls()
{
// DEBUG:
//	TRACEUSER( "Karim", _T("BevelInfoBarOp::UpdateJoinTypeControls\n"));

	if (!ControlsExist || !HasWindow())
		return;

	// the check for join types is possibly time-consuming, as we may need to scan the
	// selection if there are multiple join types in use on selected objects.

	// Determine if there is a common attribute.
	NodeAttribute* pAttr;
	SelRange::CommonAttribResult eResult;
	eResult = GetApplication()->
				 FindSelection()->
					 FindCommonAttribute(CC_RUNTIME_CLASS(AttrJoinType), &pAttr);

	// initialise all button-states to FALSE - they will be set to TRUE appropriately below.
	BOOL fMitre = FALSE;
	BOOL fRound = FALSE;
	BOOL fBevel = FALSE;

	switch (eResult)
	{
		// uncheck all buttons.
		case SelRange::ATTR_NONE:
			break;

		// only check the common join type.
		case SelRange::ATTR_COMMON:
			if (pAttr != NULL)
			{
				fMitre = ( ((AttrJoinType*)pAttr)->Value.JoinType == MitreJoin );
				fRound = ( ((AttrJoinType*)pAttr)->Value.JoinType == RoundJoin );
				fBevel = ( ((AttrJoinType*)pAttr)->Value.JoinType == BevelledJoin );
			}
			break;

		// test the selection for all common line attr types.
		case SelRange::ATTR_MANY:
			{
				// scan the selection, quitting as soon as all three states get set.
				SelRange* pSel = GetApplication()->FindSelection();
				for (Node*	pSelNode =	pSel->FindFirst();
							pSelNode !=	NULL && !(fMitre && fRound && fBevel);
							pSelNode =	pSel->FindNext(pSelNode))
				{
					if (!pSelNode->IsAnObject())
						continue;

					((NodeRenderableInk*)pSelNode)->
						FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType), &pAttr);
					if (pAttr != NULL)
					{
						fMitre |= ( ((AttrJoinType*)pAttr)->Value.JoinType == MitreJoin );
						fRound |= ( ((AttrJoinType*)pAttr)->Value.JoinType == RoundJoin );
						fBevel |= ( ((AttrJoinType*)pAttr)->Value.JoinType == BevelledJoin );
					}
				}
			}
			break;

		// error - unrecognised return option from FindCommonAttribute().
		default:
			ERROR3("BevelInfoBarOp::UpdateJoinTypeControls; invalid result from FindCommonAttribute()");
			break;
	}

	SetBoolGadgetSelected(_R(IDC_BEVELJOINTYPEMITRE), fMitre);
	SetBoolGadgetSelected(_R(IDC_BEVELJOINTYPEROUND), fRound);
	SetBoolGadgetSelected(_R(IDC_BEVELJOINTYPEBEVEL), fBevel);
}



/********************************************************************************************

>	void BevelInfoBarOp::EnableDisableJoinTypeControls()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12 September 2000
	Purpose:	Enable/disable the join type buttons on the info bar,
				according to the contents of the selection.

********************************************************************************************/
void BevelInfoBarOp::EnableDisableJoinTypeControls()
{
	// our join type buttons act only upon the selection.
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel == NULL || pSel->IsEmpty())
	{
		EnableGadget(_R(IDC_BEVELJOINTYPEMITRE), FALSE);
		EnableGadget(_R(IDC_BEVELJOINTYPEROUND), FALSE);
		EnableGadget(_R(IDC_BEVELJOINTYPEBEVEL), FALSE);
	}
	else
	{
		EnableGadget(_R(IDC_BEVELJOINTYPEMITRE), TRUE);
		EnableGadget(_R(IDC_BEVELJOINTYPEROUND), TRUE);
		EnableGadget(_R(IDC_BEVELJOINTYPEBEVEL), TRUE);
	}
}



/********************************************************************************************

>	void BevelInfoBarOp::SetUpBevelInfo(BevelInfo * pInfo)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Initialises the bevel info class based on the dialog's settings
	
********************************************************************************************/
void BevelInfoBarOp::SetUpBevelInfo(BevelInfo * pInfo)
{
	pInfo->m_BevelType	= BevelTypes[GetSelectedValueIndex(_R(IDC_BEVEL_TYPE_COMBO))].TypeNum;

	pInfo->m_LightAngle = m_LightAngle;
	pInfo->m_Contrast	= m_Contrast;
	pInfo->m_Indent		= m_Depth;
	pInfo->m_Tilt		= m_LightTilt;

	INT32 DirectionIndex = 0 ;

	if (m_bButtonIsOuter)
		DirectionIndex = 1;

	if (pTool->AreManyBevelTypes())
	{
		pInfo->m_BevelType	= BevelTypes[GetSelectedValueIndex(_R(IDC_BEVEL_TYPE_COMBO))-1].TypeNum;
	}

	// have we a many situation ?
	if (pTool->AreManyBevelDirections())
	{
		DirectionIndex -= 1;
	}

	// depth is negative for outer bevels (in the attribute)
	if (DirectionIndex == 0)
	{
		pInfo->m_bOuter = FALSE;
	}
	else
	{
		pInfo->m_bOuter = TRUE;
	}
}

/********************************************************************************************

>	void BevelInfoBarOp::BeginDrag(BevelBlob * pBlob)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	starts the drag on a particular blob
	Notes:		Called by the BevelTool when the user begins a drag on a bevel blob
				arrowhead
	
********************************************************************************************/
void BevelInfoBarOp::BeginDrag(BevelBlob * pBlob)
{
	if (!pBlob)
	{
		ERROR3("BevelInfoBarOp::BeginDrag - blob passed in is NULL");
		return;
	}
	
	m_pDragBlob = pBlob;
	m_bAmDragging = TRUE;
	m_BlobAngle = pBlob->GetAngle();

	StartDrag( DRAGTYPE_AUTOSCROLL );
}

/********************************************************************************************

>	void BevelInfoBarOp::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Deals with pointer movement messages whilst dragging
	
********************************************************************************************/

void BevelInfoBarOp::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)
{
	// first, render my blob off
	if (!pTool || !m_pDragBlob)
		return;

	// now, get the blob to recalculate its angle
	m_pDragBlob->CalculateAngle(PointerPos, ClickMods.Constrain);

	m_pDragBlob->RenderNodes();

	Document * pDoc = Document::GetCurrent();

	if (pDoc)
	{
		pDoc->ForceRedraw(pSpread, m_pDragBlob->GetRect());
	}
	
	GetApplication()->ServiceRendering();
	
	// update our controls
//	m_LightAngle = m_pDragBlob->GetAngle();
	m_LightAngle = BEVEL_LIGHTANGLE_MAX - m_pDragBlob->GetAngle();

	SliderUpdate(FALSE);
}

/********************************************************************************************

>	void BevelInfoBarOp::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Finishes or quits the dragging
	
********************************************************************************************/

void BevelInfoBarOp::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	if (!pTool || !m_pDragBlob)
	{
		EndDrag();
		return;
	}
	
//	AttrBevelLightAngle * pAttr = NULL;
//	NodeListItem * pItem = NULL;

	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();

	if (!pBlobMgr)
	{
		ERROR3("DragFinished - no blob manager\n");
		return;
	}

	OpDescriptor * pOpDesc = NULL;

	OpChangeLightAnglesParam Param;
	
	if (!Success)
	{
		// user clicked ESC or some other reason for the drag to not complete properly
		// restore the blob's angle
		pTool->RenderBlob(m_pDragBlob);
		m_pDragBlob->SetAngle(m_BlobAngle);

		// re-render the bitmaps back to what they were
		NodeListItem * pItem = (NodeListItem *)m_pDragBlob->GetList()->GetHead();

		Document * pDoc = Document::GetCurrent();

		while (pItem)
		{
			((NodeBevelController *)pItem->pNode)->RegenerateNode(NULL, FALSE);
			((NodeBevelController *)pItem->pNode)->ReleaseCached();

			pItem = (NodeListItem *)m_pDragBlob->GetList()->GetNext(pItem);
		}
		
		if (pDoc)
		{
			if (pSpread)
			{
				pDoc->ForceRedraw(pSpread, 
					m_pDragBlob->GetRect());
			}
			else
			{
				pSpread = Document::GetSelectedSpread();

				pDoc->ForceRedraw(pSpread, 
					m_pDragBlob->GetRect());
			}
		}		

		pTool->RenderBlob(m_pDragBlob);

		// GetApplication()->ServiceRendering();
	}
	else
	{
		// success ! replace all attributes in the blob's list with the new angle
		// get the operation descriptor for the bevel change
		pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpChangeBevelLightAngle));

		Param.pNodes		 = m_pDragBlob->GetList();
		Param.NewLightAngle	 = m_pDragBlob->GetAngle();
		Param.OldLightAngle	 = m_BlobAngle;

		if (pTool)
		{
			pTool->RenderToolBlobs(pSpread, NULL);
			pTool->SetBlobRendering(FALSE);
		}

		// invoke the op
		if (pOpDesc)
		{
			pOpDesc->Invoke(&Param);
		}
		
		if (pTool)
		{
			pTool->SetBlobRendering(TRUE);
			pTool->SetupBlobList();
			pTool->RenderToolBlobs(pSpread, NULL);
		}
	}

	m_bAmDragging = FALSE;

	EndDrag();
}

/********************************************************************************************

>	void BevelInfoBarOp::LoadBevelTypeComboStrings(BOOL bMany)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Loads the values into the bevel type combo
	
********************************************************************************************/

void BevelInfoBarOp::LoadBevelTypeComboStrings(BOOL bMany)
{
	// first, clear the combo
	DeleteAllValues( _R(IDC_BEVEL_TYPE_COMBO));
	
	// now load in the combo text entries
	String_64 Str;

	INT32 Offset = 0;

	// have we a many ??
	if (bMany)
	{
		Str.Load( _R(IDS_BEVELSTRING_MANY) );
		SetStringGadgetValue(_R(IDC_BEVEL_TYPE_COMBO), Str, TRUE, 0);
		Offset = 1;
	}
				
	// load the bevel types
	for ( INT32 i=0; i<NumBevelTypes; i++ )
	{
		Str.Load( BevelTypes[i].StringID );
		SetStringGadgetValue(_R(IDC_BEVEL_TYPE_COMBO), Str, TRUE, i + Offset);
	}
				
	SetComboListLength(_R(IDC_BEVEL_TYPE_COMBO));
}
				
/********************************************************************************************

>	void BevelInfoBarOp::LoadBevelDirectionComboStrings(BOOL bMany)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Loads the values into the bevel type combo
	
********************************************************************************************/

void BevelInfoBarOp::LoadBevelDirectionComboStrings(BOOL bMany)
{
	// first, clear the combo
	/*
	DeleteAllValues( _R(IDC_BEVEL_INNER_COMBO));
	
	// now load in the combo text entries
	String_64 Str;

	INT32 Offset = 0;

	// have we a many ??
	if (bMany)
	{
		Str.Load( _R(IDS_BEVELSTRING_MANY) );
		SetStringGadgetValue(_R(IDC_BEVEL_INNER_COMBO), &Str, TRUE, 0);
		Offset = 1;
	}
				
	// load the direction combo strings
	for (INT32 i=0; i<NumInnerTypes; i++)
	{
		Str.Load( InnerTypes[i] );
		SetStringGadgetValue(_R(IDC_BEVEL_INNER_COMBO), &Str, TRUE, i + Offset);
	}
				
	SetComboListLength(_R(IDC_BEVEL_INNER_COMBO));				
	*/
}

/********************************************************************************************

>	void BevelInfoBarOp::UpdateInnerOuterButtons(BOOL bMany, BOOL bOuter, BOOL bNoSelection)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Updates the inner / outer buttons on the info bar
	
********************************************************************************************/

void BevelInfoBarOp::UpdateInnerOuterButtons(BOOL bMany, BOOL bOuter, BOOL bNoSelection)
{
	m_bButtonIsInner = FALSE;
	m_bButtonIsOuter = FALSE;

	if (bNoSelection)
	{
		EnableGadget(_R(IDC_BTN_BEVELOUTER), FALSE);
		EnableGadget(_R(IDC_BTN_BEVELINNER), FALSE);
		SetBoolGadgetSelected(_R(IDC_BTN_BEVELOUTER), FALSE);
		SetBoolGadgetSelected(_R(IDC_BTN_BEVELINNER), FALSE);
	}
	else if (bMany)
	{
		EnableGadget(_R(IDC_BTN_BEVELOUTER), TRUE);
		EnableGadget(_R(IDC_BTN_BEVELINNER), TRUE);

		SetBoolGadgetSelected(_R(IDC_BTN_BEVELOUTER), FALSE);
		SetBoolGadgetSelected(_R(IDC_BTN_BEVELINNER), FALSE);
	}
	else if (bOuter)
	{
		EnableGadget(_R(IDC_BTN_BEVELOUTER), TRUE);
		EnableGadget(_R(IDC_BTN_BEVELINNER), TRUE);

		SetBoolGadgetSelected(_R(IDC_BTN_BEVELOUTER), TRUE);
		SetBoolGadgetSelected(_R(IDC_BTN_BEVELINNER), FALSE);
		m_bButtonIsOuter = TRUE;
	}
	else
	{
		EnableGadget(_R(IDC_BTN_BEVELOUTER), TRUE);
		EnableGadget(_R(IDC_BTN_BEVELINNER), TRUE);

		SetBoolGadgetSelected(_R(IDC_BTN_BEVELOUTER), FALSE);
		SetBoolGadgetSelected(_R(IDC_BTN_BEVELINNER), TRUE);
		m_bButtonIsInner = TRUE;
	}
}


/********************************************************************************************

>	MILLIPOINT BevelInfoBarOp::GetSliderDepth()


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/99
	Inputs:		
	Outputs:	Depth in millipoints
	Returns:	-
	Purpose:	Sets the slider for depth settings (logarithmic)
	
********************************************************************************************/
MILLIPOINT BevelInfoBarOp::GetSliderDepth()
{
	SetGadgetRange(_R(IDC_BEVELSLIDER), BEVEL_DEPTH_MIN, BEVEL_DEPTH_MAX);
	MILLIPOINT Value = GetLongGadgetValue(_R(IDC_BEVELSLIDER), BEVEL_DEPTH_MIN,
		BEVEL_DEPTH_MAX);
	
	// make the value appear between 0 and 1
	if (Value > BEVEL_DEPTH_MAX)
		Value = BEVEL_DEPTH_MAX;

	double dValue = Value - BEVEL_DEPTH_MIN;
	dValue /= (double)(BEVEL_DEPTH_MAX - BEVEL_DEPTH_MIN);
	// dValue = 1.0 - dValue;

	// change the profile
	BevelSliderProfile.SetBias(-BevelSliderProfile.GetBias());
	double dSliderValue = BevelSliderProfile.MapZeroToOne((AFp)dValue);
	BevelSliderProfile.SetBias(-BevelSliderProfile.GetBias());
		
	dSliderValue *= (double)(BEVEL_DEPTH_MAX - BEVEL_DEPTH_MIN);
	dSliderValue += BEVEL_DEPTH_MIN;

	return (INT32)dSliderValue;
}

/********************************************************************************************

>	MILLIPOINT BevelInfoBarOp::SetSliderDepth(MILLIPOINT Depth)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/99
	Inputs:		
	Outputs:	Depth in millipoints
	Returns:	-
	Purpose:	Sets the slider for depth settings (logarithmic)
	
********************************************************************************************/
void BevelInfoBarOp::SetSliderDepth(MILLIPOINT Value)
{
	// make the value appear between 0 and 1
	if (Value > BEVEL_DEPTH_MAX)
		Value = BEVEL_DEPTH_MAX;

	if (Value < BEVEL_DEPTH_MIN)
		Value = BEVEL_DEPTH_MIN;

	double dValue = Value - BEVEL_DEPTH_MIN;
	dValue /= (double)(BEVEL_DEPTH_MAX - BEVEL_DEPTH_MIN);

	// change the profile
	double dSliderValue = 0;
	
	dSliderValue = BevelSliderProfile.MapZeroToOne((AFp)(dValue));
	// dSliderValue = 1.0 - dSliderValue;
		
	dSliderValue *= (double)(BEVEL_DEPTH_MAX - BEVEL_DEPTH_MIN);
	dSliderValue += BEVEL_DEPTH_MIN;

	SetGadgetRange(_R(IDC_BEVELSLIDER), BEVEL_DEPTH_MIN, BEVEL_DEPTH_MAX);

	SetLongGadgetValue(_R(IDC_BEVELSLIDER), (INT32)dSliderValue);
}





				








