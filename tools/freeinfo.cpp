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
#include "freehand.h"
#include "freeinfo.h"
#include "range.h"
//#include "resource.h"
//#include "barsdlgs.h"
//#include "rikdlg.h"
//#include "rik.h"
//#include "justin2.h"
#include "pen.h"
//#include "freehres.h"
#include "opbevel.h"   // for the list of selected node
//#include "simon.h"
#include "opdrbrsh.h"
#include "brshcomp.h"	// for the brush spacing 
#include "brushmsg.h"
#include "brushdlg.h"	// for the brush edit dialog(s)
//#include "brdlgres.h"
#include "brshdata.h"
#include "sgstroke.h"
#include "strkattr.h"
#include "valfunc.h"
#include "ndoptmz.h"
#include "ppstroke.h"
#include "brpress.h"
#include "valfunc.h"
//#include "will.h" // for the 'many' string
#include "attrmgr.h"
#include "docvmsg.h"
#include "colmsg.h"

DECLARE_SOURCE( "$Revision$" );

// Declare stuff for the memory tracking
CC_IMPLEMENT_DYNCREATE( FreeHandInfoBarOp, InformationBarOp )
#define new CAM_DEBUG_NEW

PORTNOTE("other", "Disabled Brush editing")
#ifndef EXCLUDE_FROM_XARALX
CBrushGadget FreeHandInfoBarOp::BrushGadget;
#endif

// enumerate the valuefunctions that we are using in the stroke combo

// Just found out that if you don't enumerate these in the order in which you place them into
// the combo, then everything screws up big style! Matt 16/10/2000...
enum {
		CONSTANT,
		SS_Fallout,
		SS_Iron,
		ELLIPSE,
		SS_SlimBlip,
		BLIP,
		SS_Cigar,
		SS_Cigar2,
		SS_Cigar3,
		SS_Convex,
		RAMPS2,
		RAMPS, 
		SS_Concave,
		RAMPL2,
		RAMPL,
		TEARDROPCURVEDEND,
		TEARDROP,
		SS_Comet,
		SS_Petal,
		SS_Reed,
		SS_Meteor,
		SS_Torpedo,
		SS_Missile,
		SS_Goldfish,
		SS_Yacht,
		SS_Barb,
		SS_OceanLiner,
		PROPELLER,
		DOUBLERAMPS,
		BEVELENDS,
		SAWTOOTH,
		INTESTINE,
		DECAY
};

// used to indicate that our brush control needs to display the 'many' string
const BrushHandle BrushHandle_Many = BrushHandle_NoBrush - 1;

/********************************************************************************************

>	FreeHandInfoBarOp::FreeHandInfoBarOp()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Purpose:	Sets the info bars vars to NULL defaults

********************************************************************************************/

FreeHandInfoBarOp::FreeHandInfoBarOp()
{
	DlgResID = _R(IDD_FREEHANDTOOL);

	// Don't know about the tool and can not retro fit to start with
	pTool = NULL;
	CanRetroFit = FALSE;

	// The Controls hasve not been created yet
	ControlsExist = FALSE;

	m_pBrushList = NULL;

	m_DisplayBrush = BrushHandle_NoBrush;

/*	m_pStrokeList = NULL;

	m_DisplayStroke = StrokeHandle_NoStroke;
	*/

	m_SelectedStroke = CONSTANT; // we want to start by displaying the constant stroke
}


/********************************************************************************************

>	FreeHandInfoBarOp::~FreeHandInfoBarOp()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/2000
	Purpose:	destructor

********************************************************************************************/

FreeHandInfoBarOp::~FreeHandInfoBarOp()
{
	if (m_pBrushList != NULL)
	{
		m_pBrushList->clear();
		delete m_pBrushList;
		m_pBrushList = NULL;
	}

	/*
	if (m_pStrokeList != NULL)
	{
		m_pStrokeList->RemoveAll();
		delete m_pStrokeList;
		m_pStrokeList = NULL;
	}
	*/
}


/********************************************************************************************

>	MsgResult FreeHandInfoBarOp::Message(Msg* Message) 

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/94
	Inputs:		Message - A dialog message 
	Returns:	The result of the message
	Purpose:	Takes a look at the message and if it is for me it responds to it.
				Here the scroll bar for the freehand tool is Created and closed. It
				also responds to changes in the sliders position and tells the freehand
				tool to set its smoothing factor to something appropriate.

********************************************************************************************/

MsgResult FreeHandInfoBarOp::Message(Msg* Message) 
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		// Check if the message is a CANCEL
		if (Msg->DlgMsg == DIM_CANCEL)
		{
			// Mark the controls as no longer existing
			ControlsExist = FALSE;
			
			// Close the dialog 
			Close();
		}

		else if (Msg->DlgMsg == DIM_CREATE)
		{
			HandleCreate();
		}
		else
		{
			if (FALSE) {}
// WEBSTER - markn 25/4/97
// No pen presure stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
			else if (Msg->GadgetID == _R(IDC_FREEHANDPRESSURE))
			{
				if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
				{
					OpDescriptor *pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_TOGGLEPRESSURE);
					if (pOpDesc != NULL)
					{
						pOpDesc->Invoke();
						UpdateState();
					}
				}
			}
#endif // VECTOR_STROKING
			else if (Msg->GadgetID == _R(IDC_FREEHANDSLIDER))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SLIDER_POS_CHANGING:
					{
						HandleSliderPosChanging();
						break;
					}

					case DIM_SLIDER_POS_SET:
					{
						// Tell the tool that things are all finished with
						pTool->RetroFitFinished();
							break;
					}
					default:
						break;
				}
			}
PORTNOTE("other", "Removed CreateBrush and EditBrush")
#ifndef EXCLUDE_FROM_XARALX
			else if (Msg->GadgetID == _R(IDC_CREATEBRUSH))
			{
				pTool->CreateBrush();
				UpdateState();
			}
			else if (Msg->GadgetID == _R(IDC_EDITBRUSH))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_LFT_BN_CLICKED:
					{
						LaunchBrushEditDialog();
						break;
					}
					default:
						break;
				}
			}
#endif
			else if (Msg->GadgetID == _R(IDC_BRUSHTYPE))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED:
					{
						HandleBrushSelectChange();
						UpdateState();
						break;
					}
					default:
						break;
				}
			}
			else if (Msg->GadgetID == _R(IDC_STROKETYPE))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED:
					{
						HandleStrokeSelectChange();
						//UpdateState();
						break;
					}
					default:
						break;
				}
			}
		}
	}
	else
	{
		// Handle non dialog messages in here
		if (ControlsExist && Message->IsKindOf(CC_RUNTIME_CLASS(SelChangingMsg)))
		{
			// Yep, we had better go an see if the retro path field is still valid
			if (pTool!=NULL)
			{
				// Find out if we can still do the retro fitting stuff
				CanRetroFit = pTool->IsRetroPathStillValid();

				// set the string in the info bar
				SetRetroString();
			}
		}

		// the following is required because the brushes do NOT exist until the document has
		// been created!  We need to init brushes in three cases:

		// 1) THIS infobar is made active by the user - brushes inited via DIM_CREATE
		// 2) THIS infobar was the last used infobar when camelot quit - which means it will
		//	  be the first one created; occuring BEFORE the document has been created!
		// 3) Someone has edited the brush definition fron the line gallery, thereby changing the
		//	  appearance of the brushes.
		// we overcome this problem by detecting the documents BORNANDSTABLE and message
		
		if (ControlsExist) 
		{
			if (IsVisible ())		// BUT only if the infobar is visible!
			{
				if (Message->IsKindOf(CC_RUNTIME_CLASS(DocChangingMsg)))
				{
					Document* pDoc = Document::GetCurrent();
					BOOL EnableCombos = pDoc != NULL;
					if (!EnableCombos)
					{
						EnableGadget(_R(IDC_BRUSHTYPE), EnableCombos);
						EnableGadget(_R(IDC_STROKETYPE), EnableCombos);
						EnableGadget(_R(IDC_FREEHANDSLIDER), EnableCombos);
					}
					DocChangingMsg* pDocMsg = (DocChangingMsg*)Message;
					if (pDocMsg->State == DocChangingMsg::BORNANDSTABLE)
					{
						EnableGadget(_R(IDC_BRUSHTYPE), TRUE);
						EnableGadget(_R(IDC_STROKETYPE), TRUE);
						EnableGadget(_R(IDC_FREEHANDSLIDER), TRUE);
						if (m_pBrushList == NULL || m_pBrushList->size() == 0)
							InitBrushList();
						else
							ReInitBrushList();

						SetBrushListToControl ();

					}
				
				}
				else if (Message->IsKindOf(CC_RUNTIME_CLASS(BrushDefMsg)))
				{
					SetBrushListToControl();  // we've updated a brush definition so refresh
					SetHeadOfListSelected();  // the display

				}
			}
		}

		// if it is a brush selection message then pass the new handle on to the tool
		if (Message->IsKindOf(CC_RUNTIME_CLASS(BrushMsg)))
		 
		// only pass it on if a) our tool is the current tool and b)we have a current document
		{
			BrushHandle SelHandle = ((BrushMsg*)Message)->m_BrushSelected;
				
			if (pTool != NULL)
			{
				pTool->SetBrushUpdateState(UPDATE_ONIDLE);
				pTool->BrushSelected(SelHandle, FALSE);
			}
			
			// tell the list it has a new head
			if (SelHandle != BrushHandle_NoBrush)
				SetHeadOfList(SelHandle);

			m_DisplayBrush = SelHandle;
					
			// display the new list
			if (ControlsExist)
			{
				SetBrushListToControl();
				SetHeadOfListSelected();
			}
		}

		// if it is a stroke message then update our combo
		if (Message->IsKindOf(CC_RUNTIME_CLASS(StrokeMsg)))
			SetStrokeComboFromSelection();

		// likewise if it is a screen change message
		if (Message->IsKindOf(CC_RUNTIME_CLASS(ScreenChangeMsg)) || Message->IsKindOf(CC_RUNTIME_CLASS(CurrentAttrChangedMsg))
			|| Message->IsKindOf(CC_RUNTIME_CLASS(BrushDefMsg)))
		{
			if (pTool != NULL && HasWindow() 
				&& (Document::GetCurrent() != NULL))
			{
				pTool->ScreenChanged(TRUE); //((ScreenChangeMsg*)Message)->m_bClear);
				UpdateState();
			}
		}


		if (Message->IsKindOf(CC_RUNTIME_CLASS(DocViewMsg)))
		{
			DocViewMsg* pDVM = (DocViewMsg*)Message;
			if (pDVM->State == DocViewMsg::SELCHANGED)
				pTool->ScreenChanged(TRUE);
		}
	
		// if it is a new brush then we need to update our list
		if (Message->IsKindOf(CC_RUNTIME_CLASS(NewBrushMsg)))
		{
			AddBrush();
			if (pTool)
			{
				UINT32 BrushNum = BrushComponent::GetNumberOfBrushes() - 1;
				pTool->BrushSelected(BrushNum, FALSE);
			}
		}
	}

	// do we have a selection change message? 
	if (MESSAGE_IS_A(Message, SelChangingMsg))
	{
		Document* pDoc  = Document::GetCurrent();
		if (pDoc != NULL)
		{
			if (ControlsExist && IsVisible())
			{
				// we may have just undone a brush edit, if the brush edit dialog is open then we
				// want to make sure it has the uptodate data.
				BrushData Data = GetBrushDataFromSelection();
				if (Data.m_BrushHandle != BrushHandle_NoBrush)
				{
PORTNOTE("other", "Disabled Brush editing")
#ifndef EXCLUDE_FROM_XARALX
					if (BrushGadget.IsDialogOpen())
						BrushGadget.ReInitialiseDialog(&Data);
#endif	
				}
				// also make sure our combo reflects the selection
				SetBrushComboFromSelection();
				SetStrokeComboFromSelection();
			}
		}
		// ask the tool to refresh its cache, as blobs will have changed
		if (pTool != NULL && HasWindow()) 
			pTool->ScreenChanged(TRUE); 
	}

	if (MESSAGE_IS_A(Message, ColourChangingMsg))
	{
		ColourChangingMsg *TheMsg = (ColourChangingMsg *) Message;

		// handle the selected View's Colour Context is changing
		// e.g. if the show printer colours (plate) has changed
		if(TheMsg->State == ColourChangingMsg::SELVIEWCONTEXTCHANGE)
		{
			// ask the tool to refresh its cache
			// (so that the cache does not have the old colour in it
			//  and the drawing will be agains the correct (current)
			//  background!)
			if (pTool != NULL && HasWindow()) 
				pTool->ScreenChanged(TRUE);
		}
	}

	// Pass message on
	return (InformationBarOp::Message(Message)); 
}    

// NO LONGER USED!! Scheduled for demolition in five of your earth minutes, what do you mean 
// you've never been to alpha centauri??
void FreeHandInfoBarOp::InitBrushes ()
{
	// first get our brush list
	if (InitBrushList() == FALSE)
	{
		ToolInformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
		return;
	}
	// if we have no document then don't bother
	Document* pDoc  = Document::GetCurrent();
	if (pDoc != NULL)
	{
//#ifdef BUILDNEWBRUSHES
		DeleteAllValues(_R(IDC_BRUSHTYPE));
		
		// find out how many brushes there are
		UINT32 NumBrushes = BrushComponent::GetNumberOfBrushes ();

		for (UINT32 Index = 1; Index <= NumBrushes; Index++)
		{
			// ask the component to make a node for us
			
			AttrBrushType* pNewAttr = BrushComponent::CreateNode ((BrushHandle) Index-1);
		
			if (pNewAttr == NULL)
			{
				ToolInformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
				return;
			}

PORTNOTE("other", "Removed CustomComboBoxControlDataItem");
#ifndef EXCLUDE_FROM_XARALX
			CustomComboBoxControlDataItem * theItem = new CustomComboBoxControlDataItem ();
			if (theItem == NULL)
			{
				ToolInformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
				return;
			}
		
			theItem->itemID = Index;

			// make the custom combobox know about our brush ....
			theItem->itemBrush = (AttrBrushType*) pNewAttr;

			// and insert the data item into the cutsom combobox ....
			SetCustomComboGadgetValue ( _R(IDC_BRUSHTYPE), theItem, TRUE, 0);
#endif
			
		}
//#else
/*	
	UINT32 index = 1;
	CustomComboBoxControlDataItem* theItem = NULL;
		
	// --- Simple constant-width line ------------------------------------------
	VariableWidthAttrValue *pAttr;
	pAttr = new VariableWidthAttrValue(NULL);
	VarWidthItem* pItem = new VarWidthItem(pAttr, String_64(TEXT("Constant")));
	//NodeAttributepItem->CreateNewAttribute ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Constant";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Blip ----------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionBlip);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Blip")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Blip";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Ellipse -------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionEllipse);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Ellipse")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Ellipse";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Simple linear ramp --------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionRampL);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Concave 3")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Concave 3";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Simple linear ramp with non-zero end width --------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionRampL2);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Concave 2")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Concave 2";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Simple S-shaped ramp ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionRampS);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Convex 3")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Convex 3";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Simple S-shaped ramp with non-zero end width ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionRampS2);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Convex 2")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Convex 2";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Teardrop ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionTeardrop);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Raindrop")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Raindrop";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- TeardropCurvedEnd ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionTeardropCurvedEnd);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Dab")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Dab";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Double S-shaped ramp ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionDoubleRampS);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Bow Tie")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Bow Tie";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Propeller ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionPropeller);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Propeller")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Propeller";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- BevelEnds ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionBevelEnds);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Bevel Ends")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Bevel Ends";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Intestine ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionIntestine);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Intestine")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Intestine";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- SawTooth ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSawTooth);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Saw Tooth")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Saw Tooth";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Decay ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionDecay);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Decay")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Decay";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Reed ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Reed);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Reed")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Reed";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Meteor ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Meteor);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Meteor")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Meteor";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Petal ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Petal);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Petal")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Petal";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Comet ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Comet);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Comet")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Comet";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Barb ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Barb);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Barb")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Barb";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Concave ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Concave);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Concave")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Concave";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Convex ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Convex);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Convex")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Convex";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Iron ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Iron);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Iron")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Iron";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Torpedo ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Torpedo);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Torpedo")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Torpedo";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Missile ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Missile);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Missile")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Missile";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Goldfish ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Goldfish);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Goldfish")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Goldfish";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_OceanLiner ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_OceanLiner);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("OceanLiner")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "OceanLiner";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Yacht ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Yacht);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Yacht")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Yacht";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Cigar ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Cigar);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Cigar")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Cigar";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Fallout ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Fallout);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Fallout")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Fallout";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);
*/
//#endif
	}
}


/********************************************************************************************

>	void FreeHandInfoBarOp::AddBrush ()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	-
	Purpose:	Tells the infobar that a new brush has been added to the brush component.  This
				function finds the last created brush and selects it as the head of the list of
				brushes to be displayed in the brush combo

********************************************************************************************/

void FreeHandInfoBarOp::AddBrush ()
{
		UINT32 BrushNum = BrushComponent::GetNumberOfBrushes() - 1;
		
		// Simply add this to the head of the list
		if (m_pBrushList == NULL)
		{
			ERROR3("Brush list is null in FreeHandInfoBarOp::AddBrush");
		}
		SetHeadOfList((BrushHandle)BrushNum);
		m_DisplayBrush = 2;
		// display the new list
		if (ControlsExist)
		{
			SetBrushListToControl();
			SetHeadOfListSelected();
		}
		
}


/********************************************************************************************

>	void FreeHandInfoBarOp::AddBrush(BrushHandle Handle)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		Handle of the brush to add
	Returns:	-
	Purpose:	Overridden version of above function.  In this version we specify the handle of
				the brush to add, plus we do not select the new brush

********************************************************************************************/

void FreeHandInfoBarOp::AddBrush(BrushHandle Handle)
{
	// Simply add this to the head of the list
	if (m_pBrushList == NULL)
	{
		// we might get here on start-up before the list is generated so just leave
		return;
	}
	m_pBrushList->push_back(Handle);

	if (ControlsExist)
	{
		SetBrushListToControl();
		SetDisplayBrush();
	}
}

/********************************************************************************************

>	void FreeHandInfoBarOp::RemoveBrush(BrushHandle Handle)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		Handle - identifier of the brush to remove
	Returns:	-
	Purpose:	to remove the specified brush from the brush list

********************************************************************************************/

void FreeHandInfoBarOp::RemoveBrush(BrushHandle Handle)
{
	// first remove it from the list
	if (m_pBrushList != NULL)
	{
		m_pBrushList->remove(Handle);
	}

	// now we want to display the no-brush
	m_DisplayBrush = BrushHandle_NoBrush;
	
	if (ControlsExist)
	{
		SetBrushListToControl();
		SetDisplayBrush();
	}

	// tell the tool
	if (pTool != NULL)
	{
		pTool->SetBrushUpdateState(UPDATE_NOW);
		pTool->BrushSelected(BrushHandle_NoBrush, FALSE);
	}
}

/********************************************************************************************

>	void FreeHandInfoBarOp::HandleCreate()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	-
	Purpose:	Called when a DIM_CREATE message is received

********************************************************************************************/

void FreeHandInfoBarOp::HandleCreate()
{
	if (WindowID!=NULL)
	{
		// Set the state of the pressure button according to the Op's state.
		// Why buttons on infobars can't dynamically auto-connect to their ops is beyond me
		UpdateState();

		// Set the range of the control
		SetGadgetRange(_R(IDC_FREEHANDSLIDER), 0, 100, 5);
		SetGadgetBitmaps(_R(IDC_FREEHANDSLIDER), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));

		// and its position
		INT32 Smoothness = 50;
		if (pTool!=NULL)
			Smoothness = pTool->GetSmoothness();
		SetLongGadgetValue(_R(IDC_FREEHANDSLIDER), Smoothness);
	
		// Set the percentage string
		TCHAR Str[32];
		String_32 jcf(_R(IDS_PERCENT_FORMAT));
		camSnprintf(Str, 32, jcf, (INT32) Smoothness);
		String_32 PercentStr(Str);
		SetStringGadgetValue(_R(IDC_FREEHANDPERCENT), &PercentStr);
	
		// Set the Retro field (Will be empty initially)
		if (pTool!=NULL)
			CanRetroFit = pTool->IsRetroPathStillValid();
		SetRetroString();
		
		// only initialise the brush list once otherwise we upset the order
		if (m_pBrushList == NULL || m_pBrushList->size() == 0)
			InitBrushList();

		// create our brush list objects and put them in the control
		SetBrushListToControl();

		// make sure the combo shows the right thing
		SetBrushComboFromSelection();
//#ifdef BUILDNEWBRUSHES
		// set up the brush gadget
PORTNOTE("other", "Disabled Brush editing")
#ifndef EXCLUDE_FROM_XARALX
		BrushGadget.LinkControlButton (this, _R(IDC_EDITBRUSH), _R(IDBBL_BRUSHEDIT), _R(IDS_BRUSHEDITDLG));
		BrushGadget.DisableTimeStamping ();
#endif
//#endif	
	
	}
	// Mark the controls as existing
	ControlsExist = TRUE;
}

/********************************************************************************************

>	void FreeHandInfoBarOp::HandleBrushSelectChange()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	-
	Purpose:	Called when a DIM_SELECTION message is received from _R(IDC_BRUSHTYPE).  

	Notes:		10/10/2000 We have been through a few variants of this, but the current version
				is that when we select a brush from the drop down it is selected to draw with
				as well as being used to apply to the selection.
********************************************************************************************/

void FreeHandInfoBarOp::HandleBrushSelectChange()
{
//#ifdef BUILDNEWBRUSHES
	
	// if we don't have a document then forget it, though its not erroneous
	Document* pDoc = Document::GetCurrent();
	if (!pDoc)
		return;

	// Someone selected a new brush
	WORD Index;

	// find out which index they selected
	GetValueIndex (_R(IDC_BRUSHTYPE), &Index);

	BrushHandle SelHandle;
	
	// the zeroth index is always reserved for the 'no-brush' 
	if (Index == 0)
		SelHandle = BrushHandle_NoBrush;
	else
		SelHandle = GetBrushHandleFromControlIndex(Index-1);
	//TRACEUSER( "Diccon", _T("Brush Combo: Handle = %d, Index = %d\n"), SelHandle, Index);	

	// tell the tool our selection has changed
	if (pTool != NULL)
	{
		pTool->SetBrushUpdateState(UPDATE_ONIDLE);
		pTool->BrushSelected(SelHandle);
	}
			
	if (SelHandle != BrushHandle_NoBrush)
	{
		SetHeadOfList(SelHandle);
		SetBrushListToControl();
		m_DisplayBrush = SelHandle;
	//	TRACEUSER( "Diccon", _T("Brush Combo: Handle = %d, Index = %d\n"), SelHandle, Index);SetBrushListToControl();	
		SetHeadOfListSelected();
	}
	else
	{
		m_DisplayBrush = SelHandle;
	//	SetDisplayBrush();
	}
		
	

//#endif
}

/********************************************************************************************

>	void FreeHandInfoBarOp::HandleStrokeSelectChange()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	-
	Purpose:	Called when a DIM_SELECTION message is received from _R(IDC_STROKETYPE).  

********************************************************************************************/

void FreeHandInfoBarOp::HandleStrokeSelectChange()
{
	// if we don't have a document then forget it, though its not erroneous
	Document* pDoc = Document::GetCurrent();
	if (!pDoc)
		return;

	// Someone selected a new brush
	WORD Index;

	// find out which index they selected
	GetValueIndex (_R(IDC_STROKETYPE), &Index);	

	// what we are going to do is apply a stroke attribute and a variable width attribute at the 
	// same time via the attribute manager. This will require making a list

	List AttribsList;
	
	// create new list items
	NodeAttributePtrItem* pStrokeItem = new NodeAttributePtrItem;
	if (pStrokeItem == NULL)
	{
		ERROR3("Unable to create new stroke item in FreeHandInfoBarOp::HandleStrokeSelectChange");
		return;
	}
	

	NodeAttributePtrItem* pVarWidthItem = new NodeAttributePtrItem;
	if (pVarWidthItem == NULL)
	{
		ERROR3("Unable to create new variable width item in FreeHandInfoBarOp::HandleStrokeSelectChange");
		delete pStrokeItem;
		return;
	}

	NodeAttribute* pVarWidth = NULL;	
	NodeAttribute* pStrokeType = NULL;

	// now make a pathprocessor and a stroke attribute, note that if we apply a CONSTANT stroke we don't need to
	PathProcessorStroke* PPS = NULL;
	if (Index-1 != CONSTANT)
	{
		PPS = new PathProcessorStroke;
		if (PPS == NULL)
		{
			ERROR3("Unable to create new path processor in FreeHandInfoBarOp::HandleStrokeSelectChange");
			delete pStrokeItem;
			delete pVarWidthItem;
			return;
		}
	}

	StrokeTypeAttrValue NewStroke(PPS);
	pStrokeType = NewStroke.MakeNode();
	
	if (pStrokeType == NULL)
	{
		ERROR3("Unable to create new stroke attribute in FreeHandInfoBarOp::HandleStrokeSelectChange");
		delete pStrokeItem;
		delete pVarWidthItem;
		delete PPS;
		return;
	}

	pStrokeItem->NodeAttribPtr = pStrokeType;
	

	switch (Index-1)
	{
		case CONSTANT:
		{
			// Create a default variable width value (should get optimised out when applied to the doc)
			VariableWidthAttrValue VarWidth; // creates a CONSTANT variable width
			
			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pVarWidthItem;
				return;
			}
		}
		break;
		
		case RAMPL:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionRampL);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case RAMPL2:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionRampL2);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case RAMPS:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionRampS);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case RAMPS2:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionRampS2);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case BLIP:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionBlip);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case ELLIPSE:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionEllipse);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}

		}
		break;

		case TEARDROP:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionTeardrop);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case TEARDROPCURVEDEND:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionTeardropCurvedEnd);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case DOUBLERAMPS:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionDoubleRampS);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SAWTOOTH:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSawTooth);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case PROPELLER:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionPropeller);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case INTESTINE:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionIntestine);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case DECAY:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionDecay);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case BEVELENDS:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionBevelEnds);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)
			{
				pVarWidthItem->NodeAttribPtr = pVarWidth;
			}
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Yacht:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Yacht);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Iron:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Iron);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Reed:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Reed);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Meteor:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Meteor);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Petal:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Petal);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Comet:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Comet);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Fallout:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Fallout);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Torpedo:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Torpedo);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Missile:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Missile);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Convex:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Convex);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Concave:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Concave);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_SlimBlip:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_SlimBlip);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Cigar:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Cigar);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;
		case SS_Cigar2:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Cigar2);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Cigar3:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Cigar3);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_OceanLiner:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_OceanLiner);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Goldfish:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Goldfish);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		case SS_Barb:
		{
			// create our attrval with a valuefunction
			VariableWidthAttrValue VarWidth(new ValueFunctionSS_Barb);

			pVarWidth = VarWidth.MakeNode();

			if (pVarWidth)	{ pVarWidthItem->NodeAttribPtr = pVarWidth; }
			else
			{
				delete pStrokeItem;
				delete pVarWidthItem;
				delete pStrokeType;
				return;
			}
		}
		break;

		default:
		{
			ERROR3("Unrecognised stroke type in HandleStrokeSelectChange");
			delete pStrokeItem;
			delete pVarWidthItem;
			delete pStrokeType;
			m_SelectedStroke = CONSTANT;
			return;
		}
	}

	
	// now if the selection is empty then we want to update our selected stroke member
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel)
	{
		// recall that the indexes in the control are 1 greater than our enum constants
		if (pSel->IsEmpty())
			m_SelectedStroke = Index - 1;

		// update the selection bounds
		pSel->UpdateParentBoundsOfSelection();
	}


	// OK so now we should have two ListItems to add to the list, or only one if
	// its a CONSTANT stroke

	if (pStrokeItem != NULL)
		AttribsList.AddTail(pStrokeItem);

	if (pVarWidthItem != NULL)
		AttribsList.AddTail(pVarWidthItem);

	// apply those attributes!
	AttributeManager::AttributesSelected(AttribsList, _R(IDS_STROKEAPPLY_UNDO));


	// We don't need the list of attrs anymore
	NodeAttributePtrItem* pAttrItem = (NodeAttributePtrItem*)AttribsList.GetHead();
	while (pAttrItem)
	{
		delete (pAttrItem->NodeAttribPtr);
		pAttrItem->NodeAttribPtr = NULL;
		pAttrItem = (NodeAttributePtrItem*)AttribsList.GetNext(pAttrItem);
	}
	AttribsList.DeleteAll(); // tidyup	

}


/********************************************************************************************

>	void FreeHandInfoBarOp::HandleBrushSelectChange()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	-
	Purpose:	Called when a SLIDER_POS_CHANGING message is received 

********************************************************************************************/

void FreeHandInfoBarOp::HandleSliderPosChanging()
{
	// Find the current scrollers position
	TCHAR Str[32];
	BOOL Valid;
	INT32 Result = GetLongGadgetValue(_R(IDC_FREEHANDSLIDER), 0, 100, 0, &Valid);

	// Build the Percentage string and set it
	String_32 jcf(_R(IDS_PERCENT_FORMAT));
	camSnprintf(Str, 32, jcf, (INT32) Result);
	String_8 PercentStr(Str);
	SetStringGadgetValue(_R(IDC_FREEHANDPERCENT), &PercentStr);

	// Tell the freehand tool about the new smoothness
	if (Valid)
	{
		// Set the smoothness and re-fit the path if possible
		pTool->SetSmoothness(Result);
		pTool->RetroFitChanging();
	}

}


/********************************************************************************************

>	void FreeHandInfoBarOp::LaunchBrushEditDialog()
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	-
	Purpose:	launches the brush edit dialog based upon data from the selection

********************************************************************************************/

void FreeHandInfoBarOp::LaunchBrushEditDialog()
{

	// launch the dialog
	OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BRUSHEDIT_DLG);
	if (pDesc != NULL)
	{
		pDesc->Invoke();
	}
	
}

/********************************************************************************************

>	void FreeHandInfoBarOp::GetBrushDataFromSelection(BOOL* pSelectionEmpty = NULL)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Outputs:	pSelectionEmpty - flag is set if the selection is empty
	Returns:	brush data object representing a selected brush, you can tell if there are no
				real brushes selected by looking at the m_BrushHandle member of the returned data
	Purpose:	if there is one and only one brush in the selection the this function returns
				a brush data object representing its current state.  If there is any other number
				then a data item is still returned but it will not be valid

********************************************************************************************/

BrushData FreeHandInfoBarOp::GetBrushDataFromSelection(BOOL* pSelEmpty)
{
	// find all the brushes in the current selection
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(AttrBrushType));
	BrushData Data; // initialises to default values
	INT32 NumItems = NodeList.GetCount();
	
	// we can only retrieve a valid data item if there is a single item selected,
	// if there is any other number then we must return
	switch (NumItems)
	{
		case 0:
			if (pSelEmpty != NULL)
				*pSelEmpty = TRUE;
			NodeList.DeleteAll();
			return Data;
		case 1:
			if (pSelEmpty != NULL)
				*pSelEmpty = FALSE;
			break;
		default:
			if (pSelEmpty != NULL)
				*pSelEmpty = FALSE;
			NodeList.DeleteAll();
			return Data;
	}

	// we've got an item, so find its applied brush attribute
	NodeListItem * pItem = (NodeListItem *)NodeList.GetHead();

	if (pItem)
	{
		Node* pBrush = pItem->pNode;
		if (pBrush)
		{
			AttrBrushType* pAttrBrush = (AttrBrushType*)pBrush;
			if (pAttrBrush->GetBrushHandle() != BrushHandle_NoBrush)
			{
				Data = pAttrBrush->GetBrushData();
				
			}
		}
	}
	NodeList.DeleteAll();
	return Data;
}


/********************************************************************************************

>	void FreeHandInfoBarOp::GetBrushHandleFromSelection(BOOL* pSelEmpty = NULL)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Outputs:	pSelectionEmpty - flag is set if the selection is empty
	Returns:	handle of the brush in the selection.  Note that if there are many different
				brushes selected then BrushHandle_NoBrush will be returned
	Purpose:	To find out if there are any brushed objects in the selection

********************************************************************************************/

BrushHandle FreeHandInfoBarOp::GetBrushHandleFromSelection(BOOL* pSelEmpty)
{
	// find all the brushes in the current selection
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(AttrBrushType), TRUE);
	BrushData Data; // initialises to default values
	INT32 NumItems = NodeList.GetCount();

	BrushHandle SelHandle = BrushHandle_NoBrush;

	if (NumItems != 0)
	{
		if (pSelEmpty !=NULL)
			*pSelEmpty = FALSE;
		// we've got an item, so find its applied brush attribute
		NodeListItem * pItem = (NodeListItem *)NodeList.GetHead();
//		BrushHandle LastHandle = BrushHandle_NoBrush;
		BOOL bFirst = TRUE;

		AttrBrushType* pAttrBrush = NULL;
		Node* pBrush = NULL;
		while (pItem)
		{
			pBrush = pItem->pNode;
			if (pBrush)
			{
				pAttrBrush = (AttrBrushType*)pBrush;
				if (bFirst)
				{
					SelHandle = pAttrBrush->GetBrushHandle();
					bFirst = FALSE;
				}
				else
				{
					// if we have different brushes then show the 'many' string
					if (pAttrBrush->GetBrushHandle() != SelHandle)
					{
						SelHandle = BrushHandle_Many;
						break;
					}
				}
			}
			pItem = (NodeListItem *)NodeList.GetNext(pItem);
		}
	}
	else
	{
		// see if we actually have anything in the selection
		SelRange* pSel = GetApplication()->FindSelection();
		if (pSel && pSel->IsEmpty())
		{
			if (pSelEmpty != NULL)
				*pSelEmpty = TRUE;
		}
	}
	NodeList.DeleteAll();
	return SelHandle;
}
/********************************************************************************************

>	BOOL FreeHandInfoBarOp::InitBrushList()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	true, unless we failed to get a new
	Purpose:	creates the brush list if it does not exist, if it already exists then we just 
				empty it. Then we fill it with brush handles in the order that they exist in
				the brush component.
********************************************************************************************/

BOOL FreeHandInfoBarOp::InitBrushList()
{
	if (m_pBrushList == NULL)
		m_pBrushList = new BrushList;

	if (m_pBrushList == NULL)
		return FALSE;

	m_pBrushList->clear();

	UINT32 NumBrushes = BrushComponent::GetNumberOfBrushes();

	for (UINT32 Index = 0; Index < NumBrushes; Index++)
	{
		BrushHandle Handle = Index;
		m_pBrushList->push_back(Handle);
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL FreeHandInfoBarOp::ReInitBrushList()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	true, unless something goes wrong
	Purpose:	This takes care of a situation where we might add a number of brushes at once
				and want to update the brush list without forgetting the order of the list.
				This kind of thing arises when we load in a new document.
				Anyway what we do is compare the number of brushes with the number of items in the 
				list, and if there is a mismatch we add the new ones to the end of the list
********************************************************************************************/

BOOL FreeHandInfoBarOp::ReInitBrushList()
{
	if (m_pBrushList == NULL)
		return InitBrushList();

	UINT32 NumBrushes = BrushComponent::GetNumberOfBrushes();
	UINT32 NumListBrushes = m_pBrushList->size();

	while (NumListBrushes < NumBrushes)
	{
		m_pBrushList->push_back(NumListBrushes);
		NumListBrushes++;
	}
	return TRUE;

}

/********************************************************************************************

>	BOOL FreeHandInfoBarOp::SetBrushListToControl()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	true, unless we failed to get a new
	Purpose:	Goes through the brush list in order, creating attribute nodes from the brush
				handles and setting them into the combo.
********************************************************************************************/

BOOL FreeHandInfoBarOp::SetBrushListToControl()
{
//#ifdef BUILDNEWBRUSHES
	if (m_pBrushList == NULL)
		return TRUE; // not an error, we just can't do it

	// if we have no document then don't bother
	Document* pDoc  = Document::GetCurrent();
	if (pDoc == NULL)
		return TRUE;

	// wipe our existing control
	DeleteAllValues(_R(IDC_BRUSHTYPE));
	
	// need a pointer of type CustomComboBoxControlDataItem to insert items into
	// a custom combobox ....
//	CustomComboBoxControlDataItem* theItem = NULL;

//	UINT32 Counter = 1; // the combo's indexes start at 1
	
	BrushHandle Handle = BrushHandle_NoBrush;
	BrushList::const_iterator iter;

	for (iter = m_pBrushList->begin(); iter !=m_pBrushList->end(); iter++)
	{
		// get the next handle
		Handle = *iter;
		
		// ask the component to make a node for us
		if (Handle != BrushHandle_NoBrush)
		{
			AttrBrushType* pNewAttr = BrushComponent::CreateNode (Handle);
			
			if (pNewAttr != NULL)
			{
PORTNOTE("other", "Removed CustomComboCoxControlDataItem");
#ifndef EXCLUDE_FROM_XARALX
				// make a new data item
				theItem = new CustomComboBoxControlDataItem ();
				if (theItem == NULL)
				{
					ToolInformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
					return FALSE;
				}
			
				theItem->itemID = Counter++;

				// make the custom combobox know about our brush ....
				theItem->itemBrush = (AttrBrushType*) pNewAttr;

				// and insert the data item into the cutsom combobox ....
				SetCustomComboGadgetValue ( _R(IDC_BRUSHTYPE), theItem, TRUE, 0);

				theItem = NULL;
#endif
			}
		}
		else
		{
			ERROR3("Default brush handle found in list in FreeHandInfoBarOp::SetBrushListToControl");
		}
	}
//#else

	return SetStrokesToControl() ;
}

/********************************************************************************************

>	BOOL FreeHandInfoBarOp::SetStrokesToControl()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	true, unless we failed to get a new
	Purpose:	Creates stroke shape items and adds them to the stroke combo
********************************************************************************************/

BOOL FreeHandInfoBarOp::SetStrokesToControl()
{
	DeleteAllValues(_R(IDC_STROKETYPE));

PORTNOTE("other", "Removed CustomComboBoxControlDataItem")
#ifndef EXCLUDE_FROM_XARALX
	
	UINT32 index = 1;

	CustomComboBoxControlDataItem* theItem = NULL;
		
	// --- Simple constant-width line ------------------------------------------
	VariableWidthAttrValue *pAttr;
	pAttr = new VariableWidthAttrValue(NULL);
	VarWidthItem* pItem = new VarWidthItem(pAttr, String_64(TEXT("Constant")));
	//NodeAttribute* pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Constant"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Fallout ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Fallout);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Fallout")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Fallout";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Iron ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Iron);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Iron")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Iron";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Ellipse -------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionEllipse);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Ellipse")));
	//pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Ellipse"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_SlimBlip ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_SlimBlip);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Slim Blip")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Slim Blip";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Blip ----------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionBlip);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Blip")));
	//pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Blip"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Cigar ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Cigar);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Cigar")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Cigar";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Cigar2 ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Cigar2);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Cigar 2")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Cigar 2";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Cigar3 ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Cigar3);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Cigar 3")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Cigar 3";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Convex ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Convex);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Convex")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Convex";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Simple S-shaped ramp with non-zero end width ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionRampS2);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Convex 2")));
	//pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Convex 2"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Simple S-shaped ramp ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionRampS);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Convex 3")));
	//pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Convex 3"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Concave ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Concave);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Concave")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Concave";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Simple linear ramp with non-zero end width --------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionRampL2);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Concave 2")));
	//pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Concave 2"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Simple linear ramp --------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionRampL);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Concave 3")));
	//pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Concave 3"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Teardrop CurvedEnd------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionTeardropCurvedEnd);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Dab")));
	//pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Dab"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Teardrop ------------------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionTeardrop);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Raindrop")));
	//pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Raindrop"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Comet ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Comet);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Comet")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Comet";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Petal ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Petal);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Petal")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Petal";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Reed ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Reed);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Reed")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Reed";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Meteor ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Meteor);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Meteor")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Meteor";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Torpedo ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Torpedo);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Torpedo")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Torpedo";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Missile ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Missile);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Missile")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Missile";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Goldfish ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Goldfish);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Goldfish")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Goldfish";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Yacht ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Yacht);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Yacht")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Yacht";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_Barb ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_Barb);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Barb")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Barb";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Smooth Stroke SS_OceanLiner ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSS_OceanLiner);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Ocean Liner")));
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = "Ocean Liner";
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Propeller ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionPropeller);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Propeller")));
	//pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Propeller"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Double S-shaped ramp ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionDoubleRampS);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Bow Tie")));
	//pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Bow Tie"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- BevelEnds ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionBevelEnds);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Bevel Ends")));
	//pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Bevel Ends"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- SawTooth ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionSawTooth);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Saw Tooth")));
	//pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Saw Tooth"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Intestine ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionIntestine);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Intestine")));
	//pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Intestine"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);

	// --- Decay ------------------------------------------------
	pAttr = new VariableWidthAttrValue(new ValueFunctionDecay);
	pItem = new VarWidthItem(pAttr, String_64(TEXT("Decay")));
	//pAttr2 = (NodeAttribute*) pItem->CreateNode ();
	theItem = new CustomComboBoxControlDataItem ();
	theItem->itemID = index++;
	theItem->itemStroke = (VarWidthItem*) pItem;
	theItem->itemName = String_64(TEXT("Decay"));
	if (pAttr == NULL || pItem == NULL || theItem == NULL)
		goto ExitFalse;
	SetCustomComboGadgetValue ( _R(IDC_STROKETYPE), theItem, TRUE, 0);


	SetStrokeComboFromSelection();
	
	return TRUE;

ExitFalse: // clean up anything we managed to allocate
	if (pAttr)
		delete pAttr;
	if (pItem)
		delete pItem;
	if (theItem)
		delete theItem;

	return FALSE;

#else
	return TRUE;
#endif

}



/********************************************************************************************

>	void FreeHandInfoBarOp::SetHeadOfListSelected()
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	-
	Purpose:	asks the brush combo to make its top item appear selecte
********************************************************************************************/

void FreeHandInfoBarOp::SetHeadOfListSelected()
{
//#ifdef BUILDNEWBRUSHES
	// if we have no document then don't bother
	Document* pDoc  = Document::GetCurrent();
	if (pDoc == NULL)
		return;
	if (m_pBrushList == NULL)
	{
		ERROR3("Brush list is NULL in FreeHandInfoBarOp::SetHeadOfListSelected");
		return;
	}
	
	// on start up we have not yet put anything in the list so make sure we're not empty,
	// however this may not be cause for an error
	if (!m_pBrushList->empty())
	{
		BrushHandle Handle = m_pBrushList->front();
	
		SetCustomComboGadgetValue(_R(IDC_BRUSHTYPE), NULL, 0, 2 );
		m_DisplayBrush = Handle;
		
	}
//#endif
}


/********************************************************************************************

>	void FreeHandInfoBarOp::SetDisplayBrush()
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	-
	Purpose:	display the current display brush in the combo window (use this when display brush
				is different from the head of the brush list)
********************************************************************************************/

void FreeHandInfoBarOp::SetDisplayBrush()
{
//#ifdef BUILDNEWBRUSHES	
	// we need to find the index for our brush
	INT32 Index = GetControlIndexFromBrushHandle(m_DisplayBrush);
	
	switch (Index)
	{
		case -1:
			SetCustomComboGadgetValue ( _R(IDC_BRUSHTYPE), NULL, TRUE, -2);
		//	TRACEUSER( "Diccon", _T("Display default brush\n"));
			break;
		case 0: // the zeroth index is the same as the head of our selected brushes list
			SetHeadOfListSelected();
		//	TRACEUSER( "Diccon", _T("Displayed head of list\n"));
			break;
		default:
			SetCustomComboGadgetValue(_R(IDC_BRUSHTYPE), NULL, 0, Index + 2);  // control indexes are +2 from the index into our list
		//	TRACEUSER( "Diccon", _T("Displayed display brush\n"));
			break;
	}
//#endif
}


/********************************************************************************************

>	void FreeHandInfoBarOp::SetBrushComboFromSelection()
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	-
	Purpose:	examines the selection and sets the appropriate brush in the brush combo

********************************************************************************************/

void FreeHandInfoBarOp::SetBrushComboFromSelection()
{
	/* there are a few different scenarios here:
	   1) we have a single brush in the selection, in which case show it in the combo
	   2) we have many different brushes in the selection, in which case show our most
	   recently selected drawing brush
	   3) we have no brushes in the selection, but the selection is not empty, in this case
	   show a default line
	   4) the selection is empty, in which case we show the selected drawing brush.
	*/

	BOOL bSelectionEmpty = FALSE;
	BrushHandle SelHandle = GetBrushHandleFromSelection(&bSelectionEmpty);

	switch (SelHandle)
	{
		case BrushHandle_NoBrush: // display our currently selected brush for drawing
		{
			// if the selection is empty then show our drawing brush, if however we just don't
			// have any brushes in the selection then display the no-brush
			if (bSelectionEmpty)
			{
				SetDisplayBrush(); 	
			}
			else
			{
				// temporarily change the display brush and reflect it in the control
				BrushHandle Temp = m_DisplayBrush;
				m_DisplayBrush = SelHandle;
				SetDisplayBrush();			
				// restore the original display brush
				m_DisplayBrush = Temp;
			}
		}
		break;
		case BrushHandle_Many: // display the 'many' string
		{
			// the only way to do this seems to be by sending a string to the control
			String_256 Many;
			Many.Load(_R(IDS_FILLTOOL_MANY));
			
			// call custom controls interface ....
			SelectCustomComboGadgetValueOnString (_R(IDC_BRUSHTYPE), &Many);
			
		}
		break;

		default: // display the brush from the selection
		{
			// temporarily change the display brush and reflect it in the control
			BrushHandle Temp = m_DisplayBrush;
			m_DisplayBrush = SelHandle;
			SetDisplayBrush();			
			// restore the original display brush
			m_DisplayBrush = Temp;
		}
		break;
	}

}


/********************************************************************************************

>	void FreeHandInfoBarOp::SetStrokeComboFromSelection()
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	-
	Purpose:	examines the selection and sets the appropriate stroke in the stroke combo.
				If there is no selection, or a multiple selection with different strokes then
				we display the last selected stroke

********************************************************************************************/

void FreeHandInfoBarOp::SetStrokeComboFromSelection()
{
	Document* pDoc = Document::GetCurrent();
	
	// if we have no document then don't bother
	if (pDoc == NULL)
		return;

	// find out how many items there are in the selection
	SelRange* pSel = GetApplication()->FindSelection();
//	INT32 NumSelItems = 0;
	INT32 SelectedStroke = m_SelectedStroke; // this is the index that we will end up setting to the control

	if (pSel)
	{
		Node* pNode = pSel->FindFirst();
		
		// declare some vars that we need in the loop
		AttrVariableWidth* pVarWidth = NULL;
		VariableWidthAttrValue* pVal = NULL;
		ValueFunction* pFunk = NULL;
		ValueFunctionID ID = ValueFunctionID_None;

		BOOL bFirst = TRUE;
		BOOL bDifferent = FALSE;
		INT32 NumItems = 0;

		// for each node check to get its applied variable width attribute, and then the value function
		// of the attribute.  If its the first node then record it, otherwise just check to see if its different
		// and if so then break out
		
		while (pNode)
		{
			NumItems++;
			
			// if its a noderenderable ink then check its applied attrvarwidth
			if (pNode->IsAnObject())
			{
				// find the attribute
				((NodeRenderableInk*)pNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrVariableWidth),
											(NodeAttribute**)&pVarWidth);

				if (pVarWidth)
				{
					// get the variable width function from the attribute (can be null)
					pVal = (VariableWidthAttrValue*)pVarWidth->GetAttributeValue();
					pFunk = pVal->GetWidthFunction();

					// its the first one
					if (bFirst)
					{
						if (pFunk)
							ID = pFunk->GetUniqueID();
						
						// get the index into our combo corresponding to this ID
						SelectedStroke = GetStrokeIndexFromValueFunctionID(ID);

						bFirst = FALSE;
					}
					else
					{
						// check to see if it is different, if so set the flag and quit
						if (pFunk && (pFunk->GetUniqueID() != ID))
						{
							bDifferent = TRUE;
							break;
						}
					}
				}

			}
			pNode = pSel->FindNext(pNode);
		}

		// if there were no items then display the current attribute
		if (NumItems == 0)
		{
			// get the current attribute from the attribute manager
			AttributeManager* pAttrMgr = &(pDoc->GetAttributeMgr());
			if (pAttrMgr == NULL)
			{
				ERROR3("Unable to get attribute manager!");
				return;
			}
			
			// may as well reuse this var
			pVarWidth = NULL;
			pVarWidth = (AttrVariableWidth*)pAttrMgr->GetCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), 
											CC_RUNTIME_CLASS(AttrVariableWidth));
			if (pVarWidth)
			{
				VariableWidthAttrValue* pVal = (VariableWidthAttrValue*)pVarWidth->GetAttributeValue();
				ValueFunction* pFunk = pVal->GetWidthFunction();
				if (pFunk)
				{
					SelectedStroke = GetStrokeIndexFromValueFunctionID(pFunk->GetUniqueID());
				}
				else
				{
					SelectedStroke = CONSTANT;
				}
			}
		}
		// if we have different strokes we need to say 'many', (what choo talkin' bout Willis!?)
		if (bDifferent)
		{
			// the only way to do this seems to be by sending a string to the control
			String_256 Many;
			Many.Load(_R(IDS_FILLTOOL_MANY));
			
			// call custom controls interface ....
			SelectCustomComboGadgetValueOnString (_R(IDC_STROKETYPE), &Many);
			return;
		}

	}
	// now set the index to the control
	SetCustomComboGadgetValue(_R(IDC_STROKETYPE), NULL, TRUE, SelectedStroke + 1);
}



/********************************************************************************************

>	INT32 FreeHandInfoBarOp::GetStrokeIndexFromValueFunctionID(ValueFunctionID ID)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		ID of the value function
	Returns:	the corresponding index of this stroke into the stroke combo, if we get an 
				unrecognised ID to be on the safe side we will return CONSTANT

	Purpose:	Bit of a last minute bodge this, as we are trying to implement a small subset
				of the value functions into the stroke combo at fixed indexes.  We will map
				the ValueFunctionID used by the ValueFunctions to the index into the combo

********************************************************************************************/

INT32 FreeHandInfoBarOp::GetStrokeIndexFromValueFunctionID(ValueFunctionID ID)
{
	INT32 ReturnVal = CONSTANT;  // if we get an invalid ID we will just stick to this
	switch (ID)
	{
		case ValueFunctionID_None:
			break;
		case ValueFunctionID_Constant:
			break;
		case ValueFunctionID_RampS:
			ReturnVal = RAMPS;
			break;	
		case ValueFunctionID_RampS2:
			ReturnVal = RAMPS2;
			break;	
		case ValueFunctionID_Teardrop:
			ReturnVal = TEARDROP;
			break;
		case ValueFunctionID_Ellipse:
			ReturnVal = ELLIPSE;
			break;
		case ValueFunctionID_Blip:
			ReturnVal = BLIP;
			break;
		case ValueFunctionID_RampL:
			ReturnVal = RAMPL;
			break;
		case ValueFunctionID_RampL2:
			ReturnVal = RAMPL2;
			break;
		case ValueFunctionID_DoubleRampS:
			ReturnVal = DOUBLERAMPS;
			break;	
		case ValueFunctionID_TeardropCurvedEnd:
			ReturnVal = TEARDROPCURVEDEND;
			break;
		case ValueFunctionID_SawTooth:
			ReturnVal = SAWTOOTH;
			break;	
		case ValueFunctionID_Propeller:
			ReturnVal = PROPELLER;
			break;	
		case ValueFunctionID_Intestine:
			ReturnVal = INTESTINE;
			break;	
		case ValueFunctionID_Decay:
			ReturnVal = DECAY;
			break;	
		case ValueFunctionID_BevelEnds:
			ReturnVal = BEVELENDS;
			break;	
		case ValueFunctionID_SS_Yacht:
			ReturnVal = SS_Yacht;
			break;	
		case ValueFunctionID_SS_Iron:
			ReturnVal = SS_Iron;
			break;	
		case ValueFunctionID_SS_Reed:
			ReturnVal = SS_Reed;
			break;	
		case ValueFunctionID_SS_Meteor:
			ReturnVal = SS_Meteor;
			break;	
		case ValueFunctionID_SS_Petal:
			ReturnVal = SS_Petal;
			break;	
		case ValueFunctionID_SS_Comet:
			ReturnVal = SS_Comet;
			break;	
		case ValueFunctionID_SS_Fallout:
			ReturnVal = SS_Fallout;
			break;	
		case ValueFunctionID_SS_Torpedo:
			ReturnVal = SS_Torpedo;
			break;	
		case ValueFunctionID_SS_Missile:
			ReturnVal = SS_Missile;
			break;	
		case ValueFunctionID_SS_Convex:
			ReturnVal = SS_Convex;
			break;	
		case ValueFunctionID_SS_Concave:
			ReturnVal = SS_Concave;
			break;
		case ValueFunctionID_SS_SlimBlip:
			ReturnVal = SS_SlimBlip;
			break;	
		case ValueFunctionID_SS_Cigar:
			ReturnVal = SS_Cigar;
			break;	
		case ValueFunctionID_SS_Cigar2:
			ReturnVal = SS_Cigar2;
			break;	
		case ValueFunctionID_SS_Cigar3:
			ReturnVal = SS_Cigar3;
			break;	
		case ValueFunctionID_SS_OceanLiner:
			ReturnVal = SS_OceanLiner;
			break;	
		case ValueFunctionID_SS_Goldfish:
			ReturnVal = SS_Goldfish;
			break;	
		case ValueFunctionID_SS_Barb:
			ReturnVal = SS_Barb;
			break;	

		default:
			ERROR3("Illegal ValueFunction ID");
			break;
	}

	return ReturnVal;
}


/********************************************************************************************

>	BrushHandle FreeHandInfoBarOp::GetBrushHandleFromControlIndex(BrushHandle Index)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		Index - the index selected from _R(IDC_BRUSHTYPE)
	Returns:	the brush handle in our 'most popular brushes' list
	Purpose:	Retrieves the brushhandle of the Indexth item in the list
********************************************************************************************/

BrushHandle FreeHandInfoBarOp::GetBrushHandleFromControlIndex(BrushHandle Index)
{
	BrushHandle RetVal = BrushHandle_NoBrush;
	if (m_pBrushList != NULL)
	{
		size_t i = (size_t)Index; // No, I don't know why it's declared to take a BrushHandle - bizarre - amb
		if (i<m_pBrushList->size())
		{
			BrushList::const_iterator iter = m_pBrushList->begin();
			for (size_t n=0; n<i; n++) iter++;
			return *iter;
		}
	}
	return RetVal;
}


/********************************************************************************************

>	INT32 FreeHandInfoBarOp::GetControlIndexFromBrushHandle(BrushHandle Handle)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		Handle - the brush handle that we want to locate
	Returns:	the position of Handle within our brush list, or - 1 if its not there
	Purpose:	Retrieves the list position of the handle
********************************************************************************************/

INT32 FreeHandInfoBarOp::GetControlIndexFromBrushHandle(BrushHandle Handle)
{
	INT32 Retval = -1;
	if (m_pBrushList != NULL)
	{
		INT32 Counter = 0;
		BrushList::const_iterator iter;
		for (iter = m_pBrushList->begin(); iter != m_pBrushList->end(); iter++)
		{
			if (*iter == Handle)
			{
				Retval = Counter;
				break;
			}
			Counter++;
		}
	}
	return Retval;
}

/********************************************************************************************

>	void FreeHandInfoBarOp::SetHeadOfList(BrushHandle Handle)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		handle - the handle to set at the head of the list
	Returns:	-
	Purpose:	Sets the given handle to the head of the 'most popular brush' list, also
				goes through the rest of the list and removes other instances of it, so
				as not to duplicate.
********************************************************************************************/

void FreeHandInfoBarOp::SetHeadOfList(BrushHandle Handle)
{
	if (Handle == BrushHandle_NoBrush)
	{
		ERROR3("Attempting to set BrushHandle_NoBrush to head of list in FreeHandInfoBarOp::SetHeadOfList");
		return;
	}
	
	if (m_pBrushList == NULL)
	{
		ERROR3("Brush list is NULL in FreeHandInfoBarOp::SetHeadOfList");
		return;
	}
	
	// go through the list replacing existing instances of Handle
	m_pBrushList->remove(Handle);
	// set Handle to the head of the list
	m_pBrushList->push_front(Handle);
	
}



/********************************************************************************************

>	void FreeHandInfoBarOp::SetDefaultLineInCombo()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	-
	Purpose:	Sets the bitmap showing in the brush combo to be that of the default stroke
				!!! Note - not actually working at the moment
********************************************************************************************/

void FreeHandInfoBarOp::SetDefaultStrokeInCombo()
{
//#ifdef BUILDNEWBRUSHES
	SetCustomComboGadgetValue(_R(IDC_BRUSHTYPE), NULL, 0, 1);
//#endif
}

/********************************************************************************************

>	void FreeHandInfoBarOp::SetToolActiveState(BOOL IsToolActive)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		IsToolActive - TRUE if the tool is becoming active, FALSE if it not
	Purpose:	Allows the info bar to know if it is valid or not

********************************************************************************************/

void FreeHandInfoBarOp::SetToolActiveState(BOOL IsToolActive)
{
	// Set the control state
	ControlsExist = IsToolActive;
}



/********************************************************************************************

>	void FreeHandInfoBarOp::SetRetroState(BOOL CanRetroFit)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Inputs:		CanRetroFit - TRUE if we can currently retro fit the path, FALSE if not
	Purpose:	Sets the Flag CanRetroFit and gets the infobar to redraw the static
				text item that says this.

********************************************************************************************/

void FreeHandInfoBarOp::SetRetroState(BOOL CanFit)
{
	// Set the Retro field to be empty by default
	CanRetroFit = CanFit;

	// Set the string in the info bar now
	SetRetroString();
}


/********************************************************************************************

>	void FreeHandInfoBarOp::SetRetroString()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Purpose:	Sets the static text item in the freehand info bar to say either 'Retro' or
				nothing, depending on the state of the flag CanRetroFit.
	SeeAlso:	FreeHandInfoBarOp::SetRetroState

********************************************************************************************/

void FreeHandInfoBarOp::SetRetroString()
{
	// go see if there is an info bar to use
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
			String_8 RetroWord(_T(""));
			SetStringGadgetValue(_R(IDC_FREEHANDRETRO), &RetroWord);
		}
	}
}



/********************************************************************************************

>	virtual void FreeHandInfoBarOp::UpdateState()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97

	Purpose:	Sets the state of button on the bar to keep them up to date

********************************************************************************************/

void FreeHandInfoBarOp::UpdateState()
{
	// Don't try to set our controls when they're not there!
	if (!ControlsExist)
		return;

PORTNOTE("other", "Disabled Brush editing")
#ifndef EXCLUDE_FROM_XARALX
	// see if we're allowed to launch the brush dialog
	OpState State = CBrushEditDlg::GetState(NULL, NULL);
//#ifdef BUILDNEWBRUSHES
	if (State.Greyed == FALSE) // && !BrushGadget.IsDialogOpen())
		EnableGadget(_R(IDC_EDITBRUSH), TRUE);
	else
		EnableGadget(_R(IDC_EDITBRUSH), FALSE);
//#endif
#endif
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel != NULL)
	{
//#ifdef BUILDNEWBRUSHES
		if (pSel->FindFirst() == NULL)
			EnableGadget(_R(IDC_CREATEBRUSH), FALSE);
		else
			EnableGadget(_R(IDC_CREATEBRUSH), TRUE);
//#endif
	}

// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	// Set the state of the pressure button according to the Op's state.
	// Why buttons on infobars can't dynamically auto-connect to their ops is beyond me
	OpDescriptor *pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_TOGGLEPRESSURE);
	if (pOpDesc != NULL)
	{
		String_256 Why;
		OpState State = pOpDesc->GetOpsState(&Why);
		
		// stop this damn thing from falling over when there is no infobar ....
		
		CWindowID hGadget = DialogManager::GetGadget(GetReadWriteWindowID (), _R(IDC_FREEHANDPRESSURE));
		
		if (hGadget)
		{
			SetBoolGadgetSelected(_R(IDC_FREEHANDPRESSURE), (State.Ticked) ? 1 : 0);
			EnableGadget(_R(IDC_FREEHANDPRESSURE), (State.Greyed) ? 0 : 1);
		}
	}	
#endif // VECTOR_STROKING



}

