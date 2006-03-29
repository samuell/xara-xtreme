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
	$Header: /Camelot/kernel/OpBarCreation.cpp 44    26/05/05 15:49 Luke $
*/

#include "camtypes.h"
#include "opbarcreation.h"
//#include "simon.h"
//#include "resource.h"

#include "transop.h"
#include "spread.h"

#include "slice.h"

#include "cxfrech.h"
#include "userattr.h"
#include "tmpltatr.h"

#include "progress.h"
#include "fillval.h"
#include "fillattr.h"


// bevel stuff for playing with light angles
#include "nbevcont.h"
#include "attrbev.h"
#include "opbevel.h"

// name set stuff
#include "ngcore.h"
#include "ngitem.h"
#include "ngprop.h"
#include "ngsentry.h"

#include "layermsg.h"	// the layer messaging
#include "sprdmsg.h"	// SpreadMsg

#include "slicehelper.h"
//#include "sliceres.h"

#include "fillramp.h"	// for ColRampItem
#include "objchge.h" // for the allow op flags

#include "extender.h"
#include "nodetxts.h"

#include "nodetxts.h"
#include "nodetxtl.h"
#include "nodetext.h"
#include "nodecntr.h"

#include "page.h"

#include "extender.h"

// need to know about shadows and bevels since they size funny for the GetNodeBounding()
#include "nodeshad.h"

#include "opdupbar.h" // for the showlayer stuff

#include "nodeblnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE(OpBarCreation, CarbonCopyOp)

#define new CAM_DEBUG_NEW


/********************************************************************************************

>	OpClone::OpBarCreation() 

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Purpose:	OpBarCreation constructor
	Errors:		-
	SeeAlso:	CarbonCopyOp

********************************************************************************************/            
OpBarCreation::OpBarCreation(): CarbonCopyOp()								
{                              
}



/********************************************************************************************

>	BOOL OpBarCreation::Init()

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpBarCreation initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/
BOOL OpBarCreation::Init()
{
	return	(RegisterOpDescriptor(0,
	 							_R(IDS_BARCREATIONOP),
								CC_RUNTIME_CLASS(OpBarCreation),
 								OPTOKEN_BARCREATIONOP,
 								OpBarCreation::GetState,
 								0,					// help ID 
 								_R(IDBBL_BARCREATIONOP),// bubble help
								0,					// resource ID
								0, //_R(IDC_BC_CREATE),		// control ID
								SYSTEMBAR_ILLEGAL,	// Bar ID
								TRUE, 				// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC) ));
}



/********************************************************************************************

>	OpState	OpBarCreation::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com> based on Jason
	Created:	27/8/99
	Returns:	The state of the OpBarCreation
	Purpose:	For finding the operations state.  

********************************************************************************************/
OpState	OpBarCreation::GetState(String_256* UIDescription, OpDescriptor *Bob)
{
	OpState OpSt;

	// if we don't allow it
	OpSt.Greyed = TRUE;
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread && !pSpread->FindActiveLayer()->IsFrame())
		OpSt.Greyed = FALSE;

	return(OpSt);   
}

/********************************************************************************************

>	void OpBarCreation::DoWithParam(OpDescriptor* token, OpParam* pOpParam)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Inputs:		OpParam - describes the number of buttons, what layers to create, any mutation
	Outputs:	-
	Returns:	-
	Purpose:	Creates a navigation rollover bar in the drawing by copying the selection
				several times unto named layers, and naming each copy of the selection.
	Errors:		-
	SeeAlso:	OpBarCreation::DoWithParam, CarbonCopyOp::DoProcessing

********************************************************************************************/	
void OpBarCreation::DoWithParam(OpDescriptor* token, OpParam* pOpParam)
{
	// start the op
	if (pOpParam != NULL && DoStartSelOp(FALSE,TRUE))
	{
		// do the biz
		// create or edit the bar depending on the params
		BOOL ok = TRUE;
		
		// touch the bar in question
		NameGallery * pNameGallery = NameGallery::Instance();
		if (pNameGallery)
		{
			pNameGallery->m_TouchedBar = SliceHelper::GetBarNumberFromBarName(((OpParamBarCreation*)pOpParam)->m_BarName);
		}

		if (((OpParamBarCreation*)pOpParam)->m_DelExistingState)
		{
			INT32 NewLayerNo = 0;

			if (((OpParamBarCreation*)pOpParam)->m_WantMouse)
				NewLayerNo = 1;
			else if (((OpParamBarCreation*)pOpParam)->m_WantClicked)
				NewLayerNo = 2;
			else if (((OpParamBarCreation*)pOpParam)->m_WantSelected)
				NewLayerNo = 3;

			// delete elements in this state before we create it
			OpDelBar::DelBar(((OpParamBarCreation*)pOpParam)->m_BarName, NewLayerNo, this);
		}

		if (((OpParamBarCreation*)pOpParam)->m_WantBackBar)
		{
			// want to build the bar and the backbar at once!!!
			if (((OpParamBarCreation*)pOpParam)->m_WantDefault)
				ok = CreateBarAndBackBar(pOpParam); // generates its own warnings if required
			else
			{
				ok = CreateBackBarFromSelection(pOpParam);
				if (ok)
					InformMessage(_R(IDS_BACKBAR_CREATED_OK));
				else
					InformWarning(_R(IDS_FAILLED_MAKE_BACKBAR));
			}
		}
		else
		{
			if (((OpParamBarCreation*)pOpParam)->m_FromSelection)
				ok = CreateOrEditBar(pOpParam);
			else
				ok = CreateFromDefaultState(pOpParam);

			if (!ok)
				InformWarning(_R(IDS_FAILLED_MAKE_BAR));
		}

		if (!ok)
			FailAndExecute();

		// end the op
		End();

		// update the bars
		DialogBarOp::SetSystemStateChanged();
		DialogBarOp::UpdateStateOfAllBars(); 
	}
	else
	{
		// give up and go home
		FailAndExecute();
		End();
	}
}

	
/********************************************************************************************

>	BOOL OpBarCreation::CreateOrEditBar (OpParam* pOpParam)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99 - re-writen 17/4/00
	Inputs:		OpParam - describes the number of buttons, what layers to create, any mutation
	Outputs:	-
	Returns:	TRUE on success, FALSE on failure
	Purpose:	Creates a navigation rollover bar in the drawing by copying the selection
				several times unto named layers, and naming each copy of the selection.
	Errors:		-
	SeeAlso:	OpBarCreation::DoWithParam

********************************************************************************************/	
BOOL OpBarCreation::CreateOrEditBar (OpParam* pOpParam)
{
	String_256 ActiveLayerStr(_R(IDS_ROLLOVER_DEFAULT));
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		ActiveLayerStr = pSpread->FindActiveLayer()->GetLayerID();
	}

	OpParamBarCreation* pBarParam = (OpParamBarCreation*)pOpParam;
	if (pBarParam->m_NoOfButtons > MAX_BUTTONS_IN_A_BAR)
		return FALSE; // too many buttons requested

	// get the name of this bar
	m_BarName = pBarParam->m_BarName;
	INT32 BarNo = SliceHelper::GetBarNumberFromBarName(m_BarName);

	// set up the layer names
	m_RolloverName[DEFAULT].Load(_R(IDS_ROLLOVER_DEFAULT)); // = "Default";
	m_RolloverName[MOUSE].Load(_R(IDS_ROLLOVER_MOUSE)); // = "Mouse";
	m_RolloverName[CLICKED].Load(_R(IDS_ROLLOVER_CLICKED)); // = "Clicked";
	m_RolloverName[SELECTED].Load(_R(IDS_ROLLOVER_SELECTED)); // = "Selected";
	m_RolloverName[BACKBAR].Load(_R(IDS_BACK_BAR)); // = "Back Bar";

	INT32 NewLayerNo = 0;

	if (pBarParam->m_WantMouse)
		NewLayerNo = 1;
	else if (pBarParam->m_WantClicked)
		NewLayerNo = 2;
	else if (pBarParam->m_WantSelected)
		NewLayerNo = 3;

	List SelectionList; // list of nodes we are copying to each point
	List DelList;		// list of nodes we no longer have any use for
	Node * pNode = NULL;

	// get the selection
	Range Sel(*(GetApplication()->FindSelection()));

	// set the range flags so it includes shadow and bevel manager nodes
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	// remove any quickshape objects from the selection
	if (Extender::ConvertQuickShapesInSelRangeToPaths(this, &Sel))
	{
		Sel = *(GetApplication()->FindSelection());
		rg = Sel.GetRangeControlFlags();
		rg.PromoteToParent = TRUE;
		Sel.Range::SetRangeControl(rg);
	}

	if (Sel.IsEmpty())
	{
		InformWarning(_R(IDS_WARNING_NO_SEL_TEMPLATE));
		return FALSE;
	}
	
	// Prepare an ObjChangeParam so we can mark which nodes will allow this op to happen to them
	ObjChangeFlags cFlags;
	cFlags.MultiReplaceNode = TRUE;
	cFlags.RegenerateNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	// Mark nodes that will allow this to happen, and error if no nodes will let it happen
	if (!Sel.AllowOp(&ObjChange))
		return FALSE; // op not allowed

	// modify the selection to the allowed constraints
	// use the selection list from now on
	CleanSelection (&Sel, &SelectionList, &DelList, NewLayerNo);

	if (SelectionList.IsEmpty())
	{
		InformWarning(_R(IDS_WARNING_NO_SEL_TEMPLATE));
		return FALSE;
	}

	// create the new layer
	Layer * pNewLayer = AddLayer(m_RolloverName[NewLayerNo], NewLayerNo);
	
	if (!pNewLayer)
		return FALSE;

	// mark the existing back bar for deletion if not in the selection
	SliceHelper::BuildListOfNodesInBar(&DelList, pNewLayer, m_BarName);

	// the deletion list should be of the actual nodes not the attribs as these
	// can get removed in the process
	NodeListItem * pNodeListItem = (NodeListItem *)DelList.GetHead();

	while(pNodeListItem)
	{
		Node *pParent = pNodeListItem->pNode;

		// if it is the template attrib we are looking at
		// we mean the node on which it is attached
		if (IS_A(pParent, TemplateAttribute))
		{
			pParent = pParent->FindParent();
			pNodeListItem->pNode = pParent;
		}

		// if it is under a bevel or shadow or something look at the top node of the compound thing
		while (!IS_A(pParent->FindParent(), NodeGroup) && !pParent->FindParent()->IsLayer())
		{
			pParent = pParent->FindParent();
			pNodeListItem->pNode = pParent;
		}

		pNodeListItem = (NodeListItem *)DelList.GetNext(pNodeListItem);
	}

	// get the size of the selection
	DocRect SelRect;
	SelRect.MakeEmpty();
	DocRect FullSelRect;
	FullSelRect.MakeEmpty();
	
	// localise all the attribs in the selection list
	pNodeListItem = (NodeListItem *)SelectionList.GetHead();

	while(pNodeListItem)
	{
		// localise attribs for this node
		DoLocaliseForAttrChange((NodeRenderableInk*) pNodeListItem->pNode, (AttrTypeSet *)NULL, (ObjectSet*) NULL);

		// calc the size of this node as part of the size of the selection
		SelRect = SelRect.Union(SliceHelper::BoundingNodeSize(pNodeListItem->pNode));
		if (IS_A(pNodeListItem->pNode, NodeShadowController) || IS_A(pNodeListItem->pNode, TextStory))
			FullSelRect = FullSelRect.Union(SliceHelper::BoundingNodeSize(pNodeListItem->pNode));
		else
			FullSelRect = FullSelRect.Union(((NodeRenderableBounded*)(pNodeListItem->pNode))->GetBoundingRect());

		pNodeListItem = (NodeListItem *)SelectionList.GetNext(pNodeListItem);
	}

	// find the center of the selection
	DocCoord SelCentre = SelRect.Centre();

	// not used by this function but set it up for later
	m_BarSelectionRect = SelRect;
	m_FinishedBarRect.MakeEmpty();


	// Matt - 20/12/2000
	// We wish you a Merry Christmas, We wish you a Merry Christmas, We wish you a Merry Christmas and a Happy New Year... That'll be 10p guv...
	// Find out whereabouts the centre of each button ON THIS LAYER was previously if we have a user positioned bar or LiveStretching is OFF
	// This is so that in these cases, when the user chooses 'Set New Design' we know the location and do not reposition/misalign things !!!
	BOOL LiveStretching = FALSE;
	BOOL IgnoreSettings = FALSE;
	INT32 m_ExistingButtonsInBar = 0;
	DocCoord oldbuttoncentres[MAX_BUTTONS_IN_A_BAR];
	TemplateAttribute ** m_ppFoundButton[MAX_BUTTONS_IN_A_BAR];
	for (INT32 temp = 0; temp < MAX_BUTTONS_IN_A_BAR; temp++)
	{
		m_ppFoundButton[temp] = NULL;
	}

	SliceHelper::CountButtonsInBarScan(pNewLayer, (TemplateAttribute **) m_ppFoundButton, &m_ExistingButtonsInBar, m_BarName);

	NodeBarProperty* pNodeBarProperty = (NodeBarProperty*) Document::GetCurrent()->GetSetSentinel()->FindBarProperty();
	BarDataType NewBarData = pNodeBarProperty->Bar(BarNo);
	if (pNodeBarProperty  && BarNo < pNodeBarProperty->HowMany())
	{
		LiveStretching = NewBarData.IsLive;
	}

	if (!LiveStretching || !NewBarData.RequiresShuffle)
	{
		// We need to get all nodes that are on this layer, for each of the buttons of this bar (in turn) - then we need to find out their bounding box in order
		// to record their centre position - this may take a while...

		for (INT32 i = 0; i < pBarParam->m_NoOfButtons; i++)
		{
			List * pList = new List;
			DocRect ButtonRect;
			ButtonRect.MakeEmpty();

			if (!((TemplateAttribute *) m_ppFoundButton[i]))
			{
				IgnoreSettings = TRUE;
				i = pBarParam->m_NoOfButtons;
			}
			else
			{
				SliceHelper::BuildListOfNodesInButton(pList, pNewLayer, ((TemplateAttribute *) m_ppFoundButton[i])->GetParam());

				NodeListItem * pNodeListItem = NULL;
				if (pList)
				{
					pNodeListItem = (NodeListItem *)pList->GetHead();
				}

				while(pNodeListItem)
				{
					// localise attribs for this node
					DoLocaliseForAttrChange((NodeRenderableInk*) pNodeListItem->pNode, (AttrTypeSet *)NULL, (ObjectSet*) NULL);
					ButtonRect = ButtonRect.Union(SliceHelper::BoundingNodeSize(pNodeListItem->pNode));

					pNodeListItem = (NodeListItem *)pList->GetNext(pNodeListItem);
				}

				oldbuttoncentres[i] = ButtonRect.Centre();

				pList->DeleteAll();
				delete pList;
			}
		}
	}




	// *** Calculate the properties needed to be added to the buttons for stretching *** 

	// set up the target and extender strs by scanning the selection
	String_256	Target = ""; // the button name of the selection
	String_256	Extender = ""; // the thing in this button that extends it
	BOOL		MakeItStretch = TRUE; // nothing stretched before - but I want it to stretch using button text
	BOOL		TextStoryInSelection = FALSE;
	BYTE		ExtenderFlags = 0; // the way the buttons will extend

	DocRect TargetRect;
	DocRect ExtenderRect;
	DocRect TextRect;
	DocRect NoTextRect;

	// find the Target, extender and if there are any text stories in the selection
	pNodeListItem = (NodeListItem *)SelectionList.GetHead();
	Node * pTextStory = NULL;

	INT32 RequiredLevel = 0;
	while (pNodeListItem && RequiredLevel < 2)
	{
		Node * pCurrent = pNodeListItem->pNode;

		RequiredLevel = SliceHelper::FindTargetAndExtender(pCurrent, Target, Extender, RequiredLevel, &ExtenderFlags,
															&TargetRect, &ExtenderRect);
		pTextStory = SliceHelper::FindNextOfClass(pCurrent, pCurrent, CC_RUNTIME_CLASS(TextStory), TRUE);
		if (pTextStory)
		{
			TextStoryInSelection = TRUE;
			TextRect = TextRect.Union(SliceHelper::BoundingNodeSize(pCurrent));

			// warn if there are any grouped text stories and fail
			while (!pTextStory->IsLayer())
			{
				if (IS_A(pTextStory, NodeGroup)) //cant call ->IsAGroup() since shadows say yes!
				{
					InformWarning(_R(IDS_WARNING_GROUPED_TEXT));
					SelectionList.DeleteAll();
					DelList.DeleteAll();
					return FALSE;
				}
				pTextStory = pTextStory->FindParent();
			}

		}
		else
			NoTextRect = NoTextRect.Union(SliceHelper::BoundingNodeSize(pCurrent));


		pNodeListItem = (NodeListItem *)SelectionList.GetNext(pNodeListItem);
	}

	// for this level we copy the stretch data across rather than "make it stretch"
	// or if there is no text story then it we can't use the "make it stretch"
	if (RequiredLevel > 1 || !TextStoryInSelection || NoTextRect.IsEmpty())
		MakeItStretch = FALSE;

//	if (MakeItStretch)
//	{
//		NoTextRect = NoTextRect.Union(TextRect); // since the text trigger is also part of the target!
//	}


	// *** calculate the translation to build up the bar into the correct location ***

	// data class to store temp data needed to build up this bar
	class ButtonDataType
	{
	public:
		DocCoord Translation;
		String_256	Name;
		BOOL HaveCreatedProperty;
		TextStory * pOriginalStory;
		String_256 Storytext;
		DocRect OriginalTextStoryDims;
		DocRect NewTextStoryDims;
	} ButtonData[MAX_BUTTONS_IN_A_BAR];

	INT32 ButtonsInitialised = 0;

	// ask the name gallery what buttons do we already have defined
	// get their names and work out the translations to line up a new state
	NameGallery * pNameGallery = NameGallery::Instance();

	pNameGallery->FastUpdateNamedSetSizes(); // make sure we are using the most up-to-date data
	
	SGUsedNames* pNames = pNameGallery->GetUsedNames();
	SGNameItem* pNameGalleryItem = pNames ? (SGNameItem*) pNames->GetChild() : NULL;

		
	while (pNameGalleryItem)
	{
		if (pNameGalleryItem->m_BarNumber == BarNo && !pNameGalleryItem->IsEmpty() && !pNameGalleryItem->IsABackBar())
		{
			// set the name
			pNameGalleryItem->GetNameText(&(ButtonData[ButtonsInitialised].Name));
			ButtonData[ButtonsInitialised].HaveCreatedProperty = FALSE;

			// Matt 19/12/2000
			// If the LiveStretching check box is turned on, then we should reposition the buttons relative to the centre of their set bounds.
			// If not, then we should position them central to where their set bounds on THIS layer are...
			// This is so that you can change some colours (etc) on one state, click 'SetNewDesign' and be sure that your button isn't
			// about to realign itself centrally to its set (when this may not have been appropriate)
			BOOL LiveStretching = FALSE;
			NodeBarProperty* pNodeBarProperty = (NodeBarProperty*) Document::GetCurrent()->GetSetSentinel()->FindBarProperty();
			BarDataType NewBarData = pNodeBarProperty->Bar(BarNo);
			if (pNodeBarProperty  && BarNo < pNodeBarProperty->HowMany())
			{
				LiveStretching = NewBarData.IsLive;
			}

			// If LiveStretching is not enabled or the bar does not require shuffling (ie user positioned)
			if ((!LiveStretching || !NewBarData.RequiresShuffle) && !IgnoreSettings)
			{
				// Unfortunately, I now want to know where the old version of this button was... I need to know this so that I can position the new buttons with
				// their new designs in the same place as the original ones so as to minimise on-screen fun... Also, we should use this in the case of User Positioned
				// buttons so that we don't turn them into Vertical Bars everytime the user clicks 'Set New Design' - not very user positioned!
				ButtonData[ButtonsInitialised].Translation.x = oldbuttoncentres[ButtonsInitialised].x - SelCentre.x;
				ButtonData[ButtonsInitialised].Translation.y = oldbuttoncentres[ButtonsInitialised].y - SelCentre.y;
			}
			else
			{
				// position to new button state on top of the other button states
				ButtonData[ButtonsInitialised].Translation.x = pNameGalleryItem->GetSetBounds().Centre().x - SelCentre.x;
				ButtonData[ButtonsInitialised].Translation.y = pNameGalleryItem->GetSetBounds().Centre().y - SelCentre.y;
			}

			ButtonsInitialised++;

		}
		pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
	}

	// init buttons that haven't been seen yet working out their new names and their locations
	INT32 listbuttonno = 0;
	for (;ButtonsInitialised < pBarParam->m_NoOfButtons; ButtonsInitialised++)
	{
		// get the next free button name and put it into listbuttonno
		SliceHelper::GetNextFreeButtonName(listbuttonno, &(ButtonData[ButtonsInitialised].Name));
		ButtonData[ButtonsInitialised].HaveCreatedProperty = FALSE;

		if (ButtonsInitialised == 0)
		{
			ButtonData[ButtonsInitialised].Translation.x = 0;
			ButtonData[ButtonsInitialised].Translation.y = 0;

		}
		else
		{
			if (pBarParam->m_IsVertical)
			{
				ButtonData[ButtonsInitialised].Translation.x = ButtonData[ButtonsInitialised-1].Translation.x;

				ButtonData[ButtonsInitialised].Translation.y = ButtonData[ButtonsInitialised-1].Translation.y
					- pBarParam->m_Spacing - SelRect.Height();

				// place the new button on the same subpixel level as the first button?
				if (pBarParam->m_Spacing == 0 || pBarParam->m_Spacing >= 3750)
					ButtonData[ButtonsInitialised].Translation.y = (ButtonData[ButtonsInitialised].Translation.y /750)*750;
			}
			else
			{
				ButtonData[ButtonsInitialised].Translation.x = ButtonData[ButtonsInitialised-1].Translation.x
					+ pBarParam->m_Spacing + SelRect.Width();

				ButtonData[ButtonsInitialised].Translation.y = ButtonData[ButtonsInitialised-1].Translation.y;

				// place the new button on the same subpixel level as the first button?
				if (pBarParam->m_Spacing == 0 || pBarParam->m_Spacing >= 3750)
					ButtonData[ButtonsInitialised].Translation.x = (ButtonData[ButtonsInitialised].Translation.x /750)*750;
			}
		}
	}

	// find a ptr to the original text stories in this bar
	for (ButtonsInitialised = 0; ButtonsInitialised < pBarParam->m_NoOfButtons; ButtonsInitialised++)
	{
		ButtonData[ButtonsInitialised].pOriginalStory = SliceHelper::FindNextTextStoryToSync( NULL,
												pNewLayer,
												NULL,
												ButtonData[ButtonsInitialised].Name,
												"",
												TRUE);

		if (ButtonData[ButtonsInitialised].pOriginalStory)
			ButtonData[ButtonsInitialised].Storytext = ButtonData[ButtonsInitialised].pOriginalStory->GetStoryAsString();


		ButtonData[ButtonsInitialised].OriginalTextStoryDims.MakeEmpty();
		ButtonData[ButtonsInitialised].NewTextStoryDims.MakeEmpty();
	}

	// activate the layer we are about to stuff things on
	SliceHelper::ShowLayer(TRUE, pNewLayer, pSpread, this);
	LayerSGallery::MakeActiveLayer(pNewLayer, FALSE); // dont tell as this will be restored

	// add items directly after the layer node as its first child
	Node * pTail = pNewLayer;
	AttachNodeDirection TailAttachDirection = FIRSTCHILD;

	pNodeListItem = NULL;
	Trans2DMatrix Transformer;


	// for each button we want to produce
	INT32 but;
	for (but = 0; but < pBarParam->m_NoOfButtons; but++)
	{
		// copy the selection from where it is to the new layer
		pNodeListItem = (NodeListItem *)SelectionList.GetHead();

		// set the transformer to position the button
		Transformer.SetTransform(	ButtonData[but].Translation.x,
									ButtonData[but].Translation.y);

		while (pNodeListItem)
		{
			pNode = pNodeListItem->pNode;
			if (pNode)
			{
				// add this node into the tree on the layer defined in list order
				// Make a copy of the current node
				Node* pTheCopy;
				BOOL ok;
				
				CALL_WITH_FAIL(pNode->NodeCopy(&pTheCopy), this, ok);

				if (ok && !DoInsertNewNode((NodeRenderableBounded *)pTheCopy, pTail, TailAttachDirection,
										 TRUE, 		// Do Invalidate region 
										 FALSE)) 	// Don't Clear the selections
					{
						ok = FALSE;
					}

				if (ok)
				{
					// don't have any of these new nodes selected
					pTheCopy->SetSelected(but == 0);

					// move it to the new location
					((NodeRenderableBounded *)pTheCopy)->Transform(Transformer);
					pTheCopy->AllowOp(&ObjChange);
					DoInvalidateNodeRegion((NodeRenderableBounded*) pTheCopy, TRUE, FALSE);

					// not used by this function but recorded by the object
					m_FinishedBarRect = m_FinishedBarRect.Union(SliceHelper::BoundingNodeSize(pTheCopy));

					// sync the text as it goes in
					if (ButtonData[but].pOriginalStory && !ButtonData[but].Storytext.IsEmpty())
					{
						TextStory * pStory = (TextStory *) SliceHelper::FindNextOfClass(pTheCopy, pTheCopy, CC_RUNTIME_CLASS(TextStory), TRUE);
						while (pStory)
						{
							// work out how to reposition this newly sync'ed text
							Node * pNodeSetSentinel = Document::GetSelected()->GetSetSentinel();
							NodeBarProperty * pNodeBarProperty = (NodeBarProperty*) ((NodeSetSentinel *)pNodeSetSentinel)->FindBarProperty();
							INT32 alignment = pNodeBarProperty->Bar(BarNo).SameSize;

							// where is the defining position of the text story before the change?
							DocRect OldRect = SliceHelper::BoundingNodeSize(pStory);
							BOOL changed = SliceHelper::SyncTextStories(pStory, ButtonData[but].pOriginalStory, this);
							if (changed)
							{
								DocRect NewRect = SliceHelper::BoundingNodeSize(pStory);
								// make the new and old texts be centred in the same place
								INT32 tx = OldRect.Centre().x - NewRect.Centre().x;
								INT32 ty = OldRect.Centre().y - NewRect.Centre().y;

								if (alignment == 1)
									tx = OldRect.lox - NewRect.lox;
								else
								if (alignment == 2)
									tx = OldRect.hix - NewRect.hix;
								
								Trans2DMatrix TextTransformer(tx, ty);
								pStory->Transform(TextTransformer);

								if (alignment == 3) // only store the data if they are different sizes
								{
									NewRect.Translate(tx,ty);
									ButtonData[but].NewTextStoryDims = ButtonData[but].NewTextStoryDims.Union(NewRect);
									ButtonData[but].OriginalTextStoryDims = ButtonData[but].OriginalTextStoryDims.Union(OldRect);
								}
							}
							// translate the altered text story to have the same defining position
							pStory = (TextStory *) SliceHelper::FindNextOfClass(pStory, pTheCopy, CC_RUNTIME_CLASS(TextStory));
						}
					}

					// Look at the wix attribs it has been given
					if (!Target.IsEmpty()) // only if we know the target name
					{
						TemplateAttribute * pTemplateAttribute = (TemplateAttribute *) SliceHelper::FindNextOfClass(pTheCopy, pTheCopy, CC_RUNTIME_CLASS(TemplateAttribute));
						while (pTemplateAttribute)
						{
							TemplateAttribute * pNextTemplateAttribute = (TemplateAttribute *) SliceHelper::FindNextOfClass(pTemplateAttribute, pTheCopy, CC_RUNTIME_CLASS(TemplateAttribute));
							// found a target
							if (Target.CompareTo(pTemplateAttribute->GetParam()) == 0)
							{
								// hide this, it is replaced by the ApplyNameAttr call bellow
								// which will use the buttons new name
								//NodeListItem * pItem = new NodeListItem(pTemplateAttribute);
								//DelList.AddTail(pItem);
								DoHideNode(pTemplateAttribute, TRUE);
							}
							else if (!Extender.IsEmpty() && Extender.CompareTo(pTemplateAttribute->GetParam()) == 0)
							{
								// found a node that matches the extender object in the selection
								// replace this with a new button name that matches the buttons
								// new name but has the word extender stuffed after it.
								// We also need to add to the node sentinel the fact that
								// NewExtenderName stretches ButtonName in the same way that
								// Extender stretched Target
								String_32	ExtenderPostfix;
								ExtenderPostfix.Load(_R(IDS_EXTENDER_POSTFIX));
								String_256	NewExtenderName = ButtonData[but].Name;
								NewExtenderName += ExtenderPostfix;
								// apply the extender name, but not the bar name to it
								ApplyNameAttr(&NewExtenderName, pTheCopy, FALSE);

								//NodeListItem * pItem = new NodeListItem(pTemplateAttribute);
								//DelList.AddTail(pItem);
								DoHideNode(pTemplateAttribute, TRUE);

								if (!ButtonData[but].HaveCreatedProperty)
								{
									TRACE( _T("redef from existing data\n"));

									DocRect TempTarget(TargetRect);
									TempTarget.Translate(	ButtonData[but].Translation.x,
															ButtonData[but].Translation.y);
									DocRect TempExtender(ExtenderRect);
									TempExtender.Translate(	ButtonData[but].Translation.x,
															ButtonData[but].Translation.y);

									// create the properties for the button/Target that this extender extends
									SliceHelper::CreatePropertiesForSet(ButtonData[but].Name, m_BarName, TRUE, ExtenderFlags != 0, FALSE, ExtenderFlags,
																		NewExtenderName, FALSE, &TempTarget, &TempExtender,
																		this);

									// create default properties for the extender
									SliceHelper::CreatePropertiesForSet(NewExtenderName, TEXT(""),  FALSE, FALSE, FALSE, 0,
																		TEXT(""), FALSE, NULL, NULL, this);

									ButtonData[but].HaveCreatedProperty = TRUE;
								}
							}
							pTemplateAttribute = pNextTemplateAttribute;
						}
					}

					// make it stretch if it didn't before
					if (MakeItStretch)
					{
						if (IS_A(pTheCopy,TextStory) || SliceHelper::FindNextOfClass(pTheCopy, pTheCopy, CC_RUNTIME_CLASS(TextStory)))
						{
							// found a node that matches the extender object in the selection
							// replace this with a new button name that matches the buttons
							// new name but has the word extender stuffed after it.
							// We also need to add to the node sentinel the fact that
							// NewExtenderName stretches ButtonName in the same way that
							// Extender stretched Target
							String_32	ExtenderPostfix;
							ExtenderPostfix.Load(_R(IDS_EXTENDER_POSTFIX));
							String_256	NewExtenderName = ButtonData[but].Name;
							NewExtenderName += ExtenderPostfix;
							// apply the extender name, but not the bar name to it
							ApplyNameAttr(&NewExtenderName, pTheCopy, FALSE);

							if (!ButtonData[but].HaveCreatedProperty)
							{
								DocRect TempTarget(NoTextRect);
								TempTarget.Translate(	ButtonData[but].Translation.x,
														ButtonData[but].Translation.y);
								DocRect TempExtender(TextRect);
								TempExtender.Translate(	ButtonData[but].Translation.x,
														ButtonData[but].Translation.y);

								// create the properties for the button/Target that this extender extends
								SliceHelper::CreatePropertiesForSet(ButtonData[but].Name, m_BarName, TRUE, pBarParam->m_ExtendFlags != 0, FALSE,
																	pBarParam->m_ExtendFlags, NewExtenderName, FALSE,
																	&TempTarget, &TempExtender, this);

								// create default properties for the extender
								SliceHelper::CreatePropertiesForSet(NewExtenderName, m_BarName, FALSE, FALSE, FALSE, 0,
																	TEXT(""), FALSE, NULL, NULL, this);

								ButtonData[but].HaveCreatedProperty = TRUE;
							}
						}
					}

					// give it the button's name
					ApplyNameAttr(&ButtonData[but].Name, pTheCopy);

					// no extender?
					// then we should add the default properties to the button
					// in the node sentry too
					if (Extender.IsEmpty() && !ButtonData[but].HaveCreatedProperty && !MakeItStretch)
					{
						SliceHelper::CreatePropertiesForSet(ButtonData[but].Name, m_BarName, TRUE, FALSE, FALSE,
															0, TEXT(""), FALSE, NULL, NULL, this);
						ButtonData[but].HaveCreatedProperty = TRUE;
					}

					// factor out common attribs
					if (pTheCopy->IsCompound() && !pTheCopy->IsABlend())
						DoFactorOutCommonChildAttributes((NodeRenderableInk*) pTheCopy);
					else
						DoFactorOutAfterAttrChange((NodeRenderableInk*) pTheCopy, (AttrTypeSet *)NULL);

					// attach the next node after this one
					TailAttachDirection = NEXT;
					pTail = pTheCopy;
				}
			}

			// and for the next item in the list
			pNodeListItem = (NodeListItem *)SelectionList.GetNext(pNodeListItem);
		}
	}

	// clean up the selection list
	pNodeListItem = (NodeListItem *)SelectionList.GetHead();
	NodeListItem *pNodeListItemToDel = NULL;

	while (pNodeListItem)
	{
		pNodeListItemToDel = pNodeListItem;
		pNodeListItem = (NodeListItem *)SelectionList.GetNext(pNodeListItem);
		SelectionList.RemoveItem((NodeListItem *)pNodeListItemToDel);
		delete pNodeListItemToDel;
	}

	// delete the existing back bar nodes
	pNodeListItem = (NodeListItem *)DelList.GetHead();

	while(pNodeListItem)
	{
		Node *pParent = pNodeListItem->pNode;

		BOOL HideIt = TRUE;
		NodeListItem * pPreviousNodeListItem = (NodeListItem *)DelList.GetHead();

		//TRACEUSER( "SimonK", _T("considering %s %d\n"), (LPCTSTR) pParent->GetRuntimeClass()->m_lpszClassName, pParent);

		// test for deleting this item twice - a double delete will crash!!!
		while (pPreviousNodeListItem && pPreviousNodeListItem != pNodeListItem && HideIt)
		{
			//TRACEUSER( "SimonK", _T("testing %s %d\n"), (LPCTSTR) pPreviousNodeListItem->pNode->GetRuntimeClass()->m_lpszClassName, pPreviousNodeListItem->pNode);
			if (pParent == pPreviousNodeListItem->pNode)
			{
				HideIt = FALSE; // since it has already been hidden by us
				//TRACEUSER( "SimonK", _T("MATCHED\n"));
			}

			pPreviousNodeListItem = (NodeListItem *)DelList.GetNext(pPreviousNodeListItem);
		}

		// hide the node if we haven't already done so
		if (HideIt)
		{
			DoInvalidateNodeRegion((NodeRenderableBounded*) pParent, TRUE, FALSE);
			pParent->SetSelected(FALSE);
			DoHideNode(pParent, TRUE);
		}

		pNodeListItem = (NodeListItem *)DelList.GetNext(pNodeListItem);
	}


	pNodeListItem = (NodeListItem *)DelList.GetHead();
	pNodeListItemToDel = NULL;

	while(pNodeListItem)
	{
		pNodeListItemToDel = pNodeListItem;
		pNodeListItem = (NodeListItem *)DelList.GetNext(pNodeListItem);
		DelList.RemoveItem((NodeListItem *)pNodeListItemToDel);
		delete pNodeListItemToDel;
	}

	// sort out any buttons just created that need to be extended due to the text
	// having been swapped. This usually happens if during the "redefine state" op
	// added 25/8/00 by sjk

	for (but = 0; but < pBarParam->m_NoOfButtons; but++)
	{
		NodeSetSentinel * pNodeSetSentinel = Document::GetSelected()->GetSetSentinel();
		if (ButtonData[but].OriginalTextStoryDims != ButtonData[but].NewTextStoryDims)
		{
			NodeSetProperty * pProp = pNodeSetSentinel->FindPropertyNode(ButtonData[but].Name);
			NamedStretchProp* pStretchProp = pProp ? (NamedStretchProp*) pProp->GetProperty(NamedStretchProp::nIndex) : NULL;

			if (pStretchProp)
			{
				// found an altered text story
				DocRect ButtonRect = TargetRect;
				if (ButtonRect.IsEmpty() || !ButtonRect.IsValid())
					ButtonRect = SelRect;

				ButtonRect.Translate(	ButtonData[but].Translation.x,
										ButtonData[but].Translation.y);

				Node * pNode = SliceHelper::FindNextNameNode(pNewLayer, pNewLayer);

				while (pNode)
				{
					Node * pParent = pNode->FindParent();
					if (!IS_A(pParent, TextStory) && ButtonData[but].Name.CompareTo(((TemplateAttribute *)pNode)->GetParam()) == 0)
						// do the extend
						Extender::Extend((NodeRenderableInk*) (pParent),
							pBarParam->m_ExtendFlags,
							pStretchProp->GetRefUnionTriggerBounds(),
							pStretchProp->GetRefTargetBounds(),
							ButtonData[but].NewTextStoryDims,
							ButtonData[but].OriginalTextStoryDims,
							ButtonRect,
							NULL, // no restrictions on extending
							TRUE
							);

					pNode = SliceHelper::FindNextNameNode(pNode, pNewLayer);
				}
			}
		}
	}

	// set the active layer to the layer that was active before
	// if that is possible
	Layer* pActiveLayer = SliceHelper::FindLayerCalled(ActiveLayerStr);
	if (pActiveLayer != 0)
		LayerSGallery::MakeActiveLayer(pActiveLayer, FALSE); // dont tell as we are putting it back

	// inform that there may well be more/less layers about to be visible or not
	BROADCAST_TO_ALL(LayerMsg(pActiveLayer, LayerMsg::LayerReason::LAYER_VISIBILITY_CHANGED));

	// the selection will have changed - after all we just deleted it
	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED));
	GetApplication()->UpdateSelection();

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	UpdateChangedNodes(&ObjChange);

	return TRUE;
}

/********************************************************************************************

>	void OpBarCreation::Do(OpDescriptor*)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Creates a navigation rollover bar in the drawing by copying the selection
				several times unto named layers, and naming each copy of the selection.
	Errors:		-
	SeeAlso:	OpBarCreation::DoWithParam, CarbonCopyOp::Do

********************************************************************************************/	
void OpBarCreation::Do(OpDescriptor* token)
{   
	OpParamBarCreation BarParam;
	DoWithParam(token, &BarParam);
}


/********************************************************************************************

>	Layer * OpBarCreation::AddLayer(String_256 LayerName)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Inputs:		Name of the Layer to create
	Outputs:	Pointer to the newly created layer
	Returns:	-
	Purpose:	Create a new named layer

********************************************************************************************/
Layer * OpBarCreation::AddLayer(String_256 LayerName, INT32 NewLayerNo)
{
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread == NULL)
		return NULL;

	// check to see if we need to add one first 
	Layer * pStandardLayer = SliceHelper::FindLayerCalled(LayerName);
	if (pStandardLayer)
	{
		LayerSGallery::MakeActiveLayer(pStandardLayer, FALSE); // dont tell, but we must put this back
		return pStandardLayer;
	}

	// find out where to add it and create it now
	Layer* 				pLayer 		= NULL;
	Node*				pContextNode= pSpread;
	AttachNodeDirection	AttDir		= FIRSTCHILD;
	LayerStatus			NewStatus;
	NewStatus.StringLayerID = LayerName;

	// avoid the page - make the layers after the page node
	Node * pNode = pSpread->FindFirstChild();
	while (pNode)
	{
		// after the page node
		if (IS_A(pNode, Page))
		{
			pContextNode = pNode;
			AttDir = NEXT;
		}
		// and after any background nodes
		else if (pNode->IS_KIND_OF(Layer))
		{
			if (((Layer *)pNode)->IsBackground())
			{
				pContextNode = pNode;
				AttDir = NEXT;
			}
		}

		pNode = pNode->FindNext();
	}

	// order the special layers
	BOOL FoundHigherLayer = FALSE;
	INT32 LayerToFind = NewLayerNo-1;
	while (!FoundHigherLayer && LayerToFind >= DEFAULT)
	{
		pStandardLayer = SliceHelper::FindLayerCalled(m_RolloverName[LayerToFind]);
		if (pStandardLayer)
		{
			FoundHigherLayer = TRUE; // found the layer to stick it just previous to

			pContextNode = pStandardLayer;
			AttDir		= PREV;
		}
		else
		{
			LayerToFind--;
		}
	}

	// create the new layer and stick it in the tree
	ALLOC_WITH_FAIL(pLayer, (new Layer()), this); 	      
	if (pLayer == NULL) return NULL;

	// Set the new layer's status  
	pLayer->SetLayerStatus(NewStatus); 

	if (DoInsertNewNode(pLayer,pContextNode,AttDir,FALSE))
	{
		LayerSGallery::MakeActiveLayer(pLayer, FALSE); // dont tell but we must put this back
	}

	return pLayer;
}

/********************************************************************************************

>	BOOL OpBarCreation::ApplyNameAttr(String_256 * pNameAttr, Node * pNode, BOOL UseBarName)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Inputs:		The object name and the node to apply it to
	Returns:	TRUE unless a error occurred
	Purpose:	To apply the names Button1, Button2, etc to the different buttons
				so that the image slicing/rollover stuff works okay.
				NB. it uses the question part of the wix attribute to store the
				bar name. This then gets saved out in the wix files but will be
				destroyed by Merlin if wixxed up.

********************************************************************************************/	
BOOL OpBarCreation::ApplyNameAttr(String_256 * pNameAttr, Node * pNode, BOOL UseBarName)
{
	TemplateAttribute* pAttr = new TemplateAttribute(	String_256(TEXT("ObjectName")),
														UseBarName ? m_BarName : "",
														*pNameAttr);

	if (!pAttr) return FALSE;

	// add the attrib into the tree
	pAttr->AttachNode(pNode, FIRSTCHILD);

	return TRUE;
}


/********************************************************************************************

>	void OpBarCreation::FindAttribColours(Node * pAttrNode, INT32 PercentColourChange, INT32 State)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Inputs:		The Node to test and manipulate (it will test all its children too and their
				children recursively - so use with care).
				PercentColourChange - is a relative amount to change the colours by (100 does nothing)
				State = (DEFAULT = 0, MOUSE = 1, CLICKED = 2, SELECTED = 3) used to gadge the
				state of the button being modified.
	Returns:	-
	Purpose:	Modifies the colours of objects for the "suggest design" mode
				Will also rotate bevels for the SELECTED state.

********************************************************************************************/	
void OpBarCreation::FindAttribColours(Node * pAttrNode, INT32 PercentColourChange, INT32 State)
{
	if (pAttrNode->IsAnAttribute())
	{
		// change the colour of the attribute
		if (PercentColourChange != 100 && pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrFlatColourFill)))
		{
			DocColour* pdoccol = ((AttrFlatColourFill *)pAttrNode)->GetStartColour();
			ShiftColourValue(pdoccol, PercentColourChange);
		}
		// this else clause *must* come before that which tests for AttrFillGeometry!
		else if (PercentColourChange != 100 && pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrFillGeometry)))
		{
			ColRampItem* pColItem;
			ColourRamp* pColRamp = ((AttrFillGeometry*)pAttrNode)->GetColourRamp();
			if (pColRamp != NULL)
			{
				pColItem = pColRamp->GetFirstCol();
				while (pColItem != NULL)
				{
					ShiftColourValue(pColItem->GetColourAddr(), PercentColourChange);
					pColItem = pColRamp->GetNextCol(pColItem);
				}
			}

			DocColour* pdoccol = ((AttrFillGeometry *)pAttrNode)->GetStartColour();
			ShiftColourValue(pdoccol, PercentColourChange);
			pdoccol = ((AttrFillGeometry *)pAttrNode)->GetEndColour();
			ShiftColourValue(pdoccol, PercentColourChange);
			pdoccol = ((AttrFillGeometry *)pAttrNode)->GetEndColour2();
			ShiftColourValue(pdoccol, PercentColourChange);
			pdoccol = ((AttrFillGeometry *)pAttrNode)->GetEndColour3();
			ShiftColourValue(pdoccol, PercentColourChange);
		}
		// rotate the bevel light angle for selected states
		else if (State == SELECTED && pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrBevelLightAngle)))
		{
			// assumes the parent of the bevel attrib is the bevel controller
			INT32 angle = 135;
			Node * pParent = pAttrNode->FindParent();
			if (pParent->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)))
			{
				angle = (INT32)((NodeBevelController *)pAttrNode->FindParent())->m_LightAngle + 180;
				if (angle > 360) angle -= 360;
			}
			((AttrBevelLightAngle *)pAttrNode)->SetValue(angle);
		}
	}
	else // find anything else interesting?
	{
		Node * pChildNode = pAttrNode->FindFirstChild();

		while (pChildNode)
		{
			// recursive call
			FindAttribColours(pChildNode, PercentColourChange, State);
			pChildNode = pChildNode->FindNext();
		}
	}
}


/********************************************************************************************

>	void OpBarCreation::ShiftColourValue(DocColour* pdoccol, INT32 PercentColourChange)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Inputs:		ptr to the doc colour - changing this changes the colour
				PercentColourChange - amount of colour to change
	Returns:	-
	Purpose:	Modifies the colours of objects for the "suggest design" mode
				Uses HSV colour model. Drops or raises V. When this goes out of bounds
				it lowers s too.

********************************************************************************************/	
void OpBarCreation::ShiftColourValue(DocColour* pdoccol, INT32 PercentColourChange)
{
	if (!pdoccol || pdoccol->IsTransparent())
		return;

	INT32 h,s,v;

	pdoccol->GetHSVValue(&h, &s, &v);


	// usual lighter and darker code
	if (PercentColourChange != 0)
	{
		v -= PercentColourChange - 100;
		if (v < 0)
		{
			s -= v*2;
			if (s > 255) s = 255;
			v = 0;
		}
		else
		if (v > 255)
		{
			s -= (v-255)*2;
			if (s < 0) s = 0;
			v = 255;
		}
	}
	else // spin the hue!!!
	{
		h += 35; // arbitry amount that you can see the difference with
		if (h > 255)
			h -= 255;
	}
	pdoccol->SetHSVValue(h, s, v);

}


/********************************************************************************************
>	NodeBarProperty* GetBarData()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/00
	Returns:	Current document's NodeBarProperty, if any.
	SeeAlso:	BarDataType; NodeBarProperty; NodeSetSentinel
********************************************************************************************/

NodeBarProperty* GetBarData()
{
	return (NodeBarProperty*) Document::GetCurrent()->GetSetSentinel()->FindBarProperty();
}


/********************************************************************************************
>	BOOL OpBarCreation::CreateFromDefaultState (OpParam* pOpParam)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> K
	Created:	25/3/00
	Purpose:	Pretty much just has to copy the contents of this bar from the default
				layer to the layer prescribed. All bar properties and stetching etc should
				already have been set up from creating the MouseOff state/layer

	Returns:	TRUE if all went well
********************************************************************************************/
BOOL OpBarCreation::CreateFromDefaultState (OpParam* pOpParam)
{
	// work out which is the active layer so we can set this back when
	// we are finished playing around with them all
	String_256 ActiveLayerStr(_R(IDS_ROLLOVER_DEFAULT));
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		ActiveLayerStr = pSpread->FindActiveLayer()->GetLayerID();
	}

	OpParamBarCreation* pBarParam = (OpParamBarCreation*)pOpParam;

	// get the name of this bar
	m_BarName = pBarParam->m_BarName;

	// set up the layer names
	m_RolloverName[DEFAULT].Load(_R(IDS_ROLLOVER_DEFAULT)); // = "Default";
	m_RolloverName[MOUSE].Load(_R(IDS_ROLLOVER_MOUSE)); // = "Mouse";
	m_RolloverName[CLICKED].Load(_R(IDS_ROLLOVER_CLICKED)); // = "Clicked";
	m_RolloverName[SELECTED].Load(_R(IDS_ROLLOVER_SELECTED)); // = "Selected";
	m_RolloverName[BACKBAR].Load(_R(IDS_BACK_BAR)); // = "Back Bar";

	INT32 NewLayerNo = 0;

	if (pBarParam->m_WantMouse)
		NewLayerNo = 1;
	else if (pBarParam->m_WantClicked)
		NewLayerNo = 2;
	else if (pBarParam->m_WantSelected)
		NewLayerNo = 3;

	// create the new layer
	Layer * pNewLayer = AddLayer(m_RolloverName[NewLayerNo], NewLayerNo);
	
	if (!pNewLayer)
		return FALSE;

	// make the new layer visible
	SliceHelper::ShowLayer(TRUE, pNewLayer, pSpread, this);

	Layer * pLayer = NULL;

	List BarList;

	pLayer = SliceHelper::FindLayerCalled(m_RolloverName[DEFAULT]);

	if (!pLayer)
		return FALSE;

	// Prepare an ObjChangeParam so we can mark which nodes will allow this op to happen to them
	ObjChangeFlags cFlags;
	cFlags.RegenerateNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	// list all of this bar that is on the default layer
	SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, m_BarName);

	// copy each item referenced by a template attrib marked with this bar ID
	NodeListItem * pNodeListItem = (NodeListItem *)BarList.GetHead();
	NodeListItem * pDelThisToo = NULL;

	Node * pTail = pNewLayer;
	AttachNodeDirection	TailAttachDirection	= FIRSTCHILD;

	while(pNodeListItem)
	{
		pDelThisToo = pNodeListItem;
		Node *pNode = pNodeListItem->pNode->FindParent();
		if (pNode)
		{
			// add this node into the tree on the layer defined in list order
			// Make a copy of the current node
			Node* pTheCopy;
			BOOL ok = TRUE;
			
			if (ok)
				CALL_WITH_FAIL(pNode->NodeCopy(&pTheCopy), this, ok);

			if (ok && pBarParam->m_SuggestDesign)
			{
				INT32 ColShift = 100;
				if (NewLayerNo == MOUSE)
					ColShift = 40;
				else if (NewLayerNo == SELECTED)
					ColShift = 150;
				else if (NewLayerNo == CLICKED)
					ColShift = 0; // this plays with the hue instead

				FindAttribColours(pTheCopy, ColShift, NewLayerNo);
			}

			if (ok && !DoInsertNewNode((NodeRenderableBounded *)pTheCopy, pTail, TailAttachDirection,
									 TRUE, 		// Do Invalidate region 
									 FALSE)) 	// Don't Clear the selections
				{
					ok = FALSE;
				}

			if (ok)
			{
				// if we are making the selected version check that we didn't have a default light angle
				// on the bevel that we would have missed. If we did miss it add in a reverse of the 
				// default light angle
				if (NewLayerNo == SELECTED && pBarParam->m_SuggestDesign)
				{
					// was there a bevel? With out a bev light angle?
					Node * pBev = SliceHelper::FindNextOfClass(pTheCopy, pTheCopy, CC_RUNTIME_CLASS(NodeBevelController), TRUE );
					if (pBev && !SliceHelper::FindNextOfClass(pTheCopy, pTheCopy, CC_RUNTIME_CLASS(AttrBevelLightAngle) ))
					{
						// then give it a bev light angle
						AttrBevelLightAngle* pNewLightAttr = new AttrBevelLightAngle();
						if (pNewLightAttr)
						{
							pNewLightAttr->Value.m_LightAngle = 120; // reverse of the default
							// add the attrib into the tree
							pNewLightAttr->AttachNode(pTheCopy, FIRSTCHILD);

						}
					}
				}

				// don't have any of these new nodes selected
				pTheCopy->SetSelected(FALSE);

				// make it regenerate
				pTheCopy->AllowOp(&ObjChange);

				// attach the next node after this one
				TailAttachDirection = NEXT;
				pTail = pTheCopy;
			}
		}

		pNodeListItem = (NodeListItem *)BarList.GetNext(pNodeListItem);

		// tidy up the list as we go along
		BarList.RemoveItem((NodeListItem *)pDelThisToo);
		delete pDelThisToo;
	}

	// set the active layer to the layer that was active before
	// if that is possible
	Layer* pActiveLayer = SliceHelper::FindLayerCalled(ActiveLayerStr);
	if (pActiveLayer != 0)
		LayerSGallery::MakeActiveLayer(pActiveLayer, FALSE); // dont tell as we are putting this back

	// show just the new layer we have made
	ShowState(NewLayerNo);

	// inform that there may well be more/less layers about to be visible or not
	BROADCAST_TO_ALL(LayerMsg(pActiveLayer, LayerMsg::LayerReason::LAYER_VISIBILITY_CHANGED));
	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED));

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	UpdateChangedNodes(&ObjChange);


	// the selection will have changed - after all we just deleted it
	GetApplication()->UpdateSelection();
	return TRUE;
}


/********************************************************************************************
>	BOOL OpBarCreation::CreateBackBarFromSelection (OpParam* pOpParam)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> K
	Created:	5/4/00
	Purpose:	Creates the back bar from the selection
	Returns:	TRUE if all went well
********************************************************************************************/
BOOL OpBarCreation::CreateBackBarFromSelection (OpParam* pOpParam, BOOL Extend)
{
	String_256 ActiveLayerStr(_R(IDS_ROLLOVER_DEFAULT));
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		ActiveLayerStr = pSpread->FindActiveLayer()->GetLayerID();
	}

	OpParamBarCreation* pBarParam = (OpParamBarCreation*)pOpParam;

	// get the name of this bar
	m_BarName = pBarParam->m_BarName;

	// work out the name of the back bar
	String_256 BackBarName = "";
	BackBarName.MakeMsg(_R(IDS_BACKBARNAME), SliceHelper::GetBarNumberFromBarName(m_BarName) +1);

	// set up the layer names
	m_RolloverName[DEFAULT].Load(_R(IDS_ROLLOVER_DEFAULT)); // = "Default";
	m_RolloverName[MOUSE].Load(_R(IDS_ROLLOVER_MOUSE)); // = "Mouse";
	m_RolloverName[CLICKED].Load(_R(IDS_ROLLOVER_CLICKED)); // = "Clicked";
	m_RolloverName[SELECTED].Load(_R(IDS_ROLLOVER_SELECTED)); // = "Selected";
	m_RolloverName[BACKBAR].Load(_R(IDS_BACK_BAR)); // = "Back Bar";

	List SelectionList;
	List DelList;

	// get the selection
	Range Sel(*(GetApplication()->FindSelection()));

	// set the range flags so it includes shadow and bevel manager nodes
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	// remove any quickshape objects from the selection
	if (Extender::ConvertQuickShapesInSelRangeToPaths(this, &Sel))
	{
		Sel = *(GetApplication()->FindSelection());
		rg = Sel.GetRangeControlFlags();
		rg.PromoteToParent = TRUE;
		Sel.Range::SetRangeControl(rg);
	}

	if (Sel.IsEmpty())
	{
		InformWarning(_R(IDS_WARNING_NO_SEL_BACKBAR));
		return FALSE;
	}
	
	// Prepare an ObjChangeParam so we can mark which nodes will allow this op to happen to them
	ObjChangeFlags cFlags;
	cFlags.MultiReplaceNode = TRUE;
	cFlags.RegenerateNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	// Mark nodes that will allow this to happen, and error if no nodes will let it happen
	if (!Sel.AllowOp(&ObjChange))
		return FALSE; // op not allowed

	// get the size of the selection
	DocRect SelRect; // = Sel.GetBoundingRect();
	SelRect.MakeEmpty();

	// modify the selection to the allowed constraints
	// use the selection list from now on
	CleanSelection (&Sel, &SelectionList, &DelList, BACKBAR);
	if (SelectionList.IsEmpty())
	{
		InformWarning(_R(IDS_WARNING_NO_SEL_BACKBAR));
		return FALSE;
	}

	// define the SelRect from the selection list
	NodeListItem * pNodeListItem = (NodeListItem *)SelectionList.GetHead();
	while(pNodeListItem)
	{
		// localise attribs for this node
		DoLocaliseForAttrChange((NodeRenderableInk*) pNodeListItem->pNode, (AttrTypeSet *)NULL, (ObjectSet*) NULL);

		// calc the size of this node as part of the size of the selection
		SelRect = SelRect.Union(SliceHelper::BoundingNodeSize(pNodeListItem->pNode));

		pNodeListItem = (NodeListItem *)SelectionList.GetNext(pNodeListItem);
	}

	// create a barBar layer if there isn't one
	Layer * pNewLayer = AddLayer(m_RolloverName[BACKBAR], BACKBAR);

	// mark the existing back bar for deletion if not in the selection
	SliceHelper::BuildListOfNodesInBar(&DelList, pNewLayer, m_BarName);

	// activate the layer we are about to stuff things on
	SliceHelper::ShowLayer(TRUE, pNewLayer, pSpread, this);

	LayerSGallery::MakeActiveLayer(pNewLayer, FALSE); // dont tell as we will put this back

	// add items directly after the layer node as its first child
	Node * pTail = pNewLayer;
	AttachNodeDirection TailAttachDirection = FIRSTCHILD;

	// copy the selection from where it is to the backbar layer
	pNodeListItem = (NodeListItem *)SelectionList.GetHead();
	Node * pNode;

	while (pNodeListItem)
	{
		pNode = pNodeListItem->pNode;
		if (pNode)
		{
			// add this node into the tree on the layer defined in list order
			// Make a copy of the current node
			Node* pTheCopy;
			BOOL ok;
			
			CALL_WITH_FAIL(pNode->NodeCopy(&pTheCopy), this, ok);

			if (ok && !DoInsertNewNode((NodeRenderableBounded *)pTheCopy, pTail, TailAttachDirection,
									 TRUE, 		// Do Invalidate region 
									 FALSE)) 	// Don't Clear the selections
				{
					ok = FALSE;
				}

			if (ok)
			{
				// have all of these new nodes selected
				pTheCopy->SetSelected(TRUE);

				// delete any wix attribs that are on this node
				TemplateAttribute * pTemplateAttribute = (TemplateAttribute *) SliceHelper::FindNextOfClass(pTheCopy, pTheCopy, CC_RUNTIME_CLASS(TemplateAttribute));
				while (pTemplateAttribute)
				{
					TemplateAttribute * pNextTemplateAttribute = (TemplateAttribute *) SliceHelper::FindNextOfClass(pTemplateAttribute, pTheCopy, CC_RUNTIME_CLASS(TemplateAttribute));
					// found a bar element
					if (SliceHelper::GetBarNumberFromBarName(pTemplateAttribute->GetQuestion()) >= 0)
					{
						// hide this, it is replaced by the ApplyNameAttr call bellow
						// which will use the buttons new name
						DoHideNode(pTemplateAttribute, TRUE);
					}

					pTemplateAttribute = pNextTemplateAttribute;
				}

				// if it is not correctly named add the name now
				ApplyNameAttr(&BackBarName, pTheCopy);

				// if extending extend this backbar item on the fly!
				if (Extend)
				{
					// do the extend
					Extender::Extend((NodeRenderableInk*) (pTheCopy),
						pBarParam->m_ExtendFlags,
						m_BarSelectionRect,
						SelRect,
						m_FinishedBarRect,
						m_BarSelectionRect,
						SelRect,
						NULL, // no restrictions on extending
						TRUE
						);
					
					// wake up blends They appear to need this extra wake up call otherwise they
					// do not redraw to their new extended dimensions
					if (pTheCopy->IsABlend())
						((NodeBlend*)(pTheCopy))->Reinit(FALSE);
				}

				// factor out common attribs
				if (pTheCopy->IsCompound() && !pTheCopy->IsABlend())
					DoFactorOutCommonChildAttributes((NodeRenderableInk*) pTheCopy);
				else
					DoFactorOutAfterAttrChange((NodeRenderableInk*) pTheCopy, (AttrTypeSet *)NULL);

				// attach the next node after this one
				TailAttachDirection = NEXT;
				pTail = pTheCopy;
			}
		}

		// and for the next item in the list
		pNodeListItem = (NodeListItem *)SelectionList.GetNext(pNodeListItem);
	}


	// add the back bar properties, which means it is extended by every member of the bar
	SliceHelper::CreatePropertiesForSet(BackBarName, m_BarName, FALSE, pBarParam->m_ExtendFlags != 0, TRUE, pBarParam->m_ExtendFlags,
				TEXT(""), FALSE, &SelRect, Extend ? &m_BarSelectionRect : NULL, this);

	// clean up the selection list
	pNodeListItem = (NodeListItem *)SelectionList.GetHead();
	NodeListItem *pNodeListItemToDel = NULL;

	while (pNodeListItem)
	{
		pNodeListItemToDel = pNodeListItem;
		pNodeListItem = (NodeListItem *)SelectionList.GetNext(pNodeListItem);
		SelectionList.RemoveItem((NodeListItem *)pNodeListItemToDel);
		delete pNodeListItemToDel;
	}

	// delete the existing back bar nodes
	pNodeListItem = (NodeListItem *)DelList.GetHead();
	pNodeListItemToDel = NULL;

	while(pNodeListItem)
	{
		pNodeListItemToDel = pNodeListItem;
		Node *pParent = pNodeListItem->pNode;

		// if it is the template attrib we are looking at
		// we mean the node on which it is attached
		if (IS_A(pParent, TemplateAttribute))
			pParent = pParent->FindParent();

		if (pParent->FindParent())
		{
			DoInvalidateNodeRegion((NodeRenderableBounded*) pParent, TRUE, FALSE);
			pParent->SetSelected(FALSE);
			DoHideNode(pParent, TRUE);
		}

		pNodeListItem = (NodeListItem *)DelList.GetNext(pNodeListItem);
		DelList.RemoveItem((NodeListItem *)pNodeListItemToDel);
		delete pNodeListItemToDel;
	}

	// set the active layer to the layer that was active before
	// if that is possible
	Layer* pActiveLayer = SliceHelper::FindLayerCalled(ActiveLayerStr);
	if (pActiveLayer != 0) 
	{
		LayerSGallery::MakeActiveLayer(pActiveLayer, FALSE); // don't tell as we are putting it back
	}

	// inform that there may well be more/less layers about to be visible or not
	BROADCAST_TO_ALL(LayerMsg(pActiveLayer, LayerMsg::LayerReason::LAYER_VISIBILITY_CHANGED));

	// the selection will have changed - after all we just deleted it
	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED));
	GetApplication()->UpdateSelection();

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	UpdateChangedNodes(&ObjChange, pSpread);
	return TRUE;
}


/********************************************************************************************
>	BOOL OpBarCreation::CleanSelection (Range * pSel, List * pSelectionList, List * pDeletionList)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> K
	Created:	30/3/00
	Purpose:	Has a look at the selection making it into a node list called selection
				if it is on a state layer that is not the current state layer it should be
				excluded from the selection as this must be an accident
				if the item is in the selection but not in any state layer
				it needs to be in the selection so it can be copied and added to the deletion list
				so it can be removed from play.

	Returns:	TRUE if all went well
********************************************************************************************/
BOOL OpBarCreation::CleanSelection (Range * pSel, List * pSelectionList, List * pDeletionList, INT32 LayerNumToCreate)
{
	Layer * pLayer = NULL;
	Node * pCurrent = pSel->FindFirst();
	Node * pParent = NULL;
	Spread* pSpread = Document::GetSelectedSpread();
	BOOL AllOk = TRUE;
	String_256 ButtonBeingEdited = "";

	// for each node in the selection
	while (pCurrent)
	{
		// find the layer on which it resides
		pParent = pCurrent;
		while (!pParent->IsLayer())
			pParent = pParent->FindParent();

		pLayer = (Layer *)pParent;

		BOOL ok = FALSE;

		// use nodes from the layer we are going to be adding to
		if (LayerNumToCreate >=0 && LayerNumToCreate <=4)
		{
			if (pLayer->GetLayerID() == m_RolloverName[LayerNumToCreate])
				ok = TRUE;
		}

		if (!ok)
		{	
			ok = TRUE;
			// test the newly found layer against the state layers
			// if it is not on a state layer it should be in the sel and del lists
			for (INT32 i = 0; i < 5; i++)
			{
				if (pLayer->GetLayerID() == m_RolloverName[i])
				{
					ok = FALSE;
					AllOk = FALSE;
				}
			}

			if (!ok)
			{
				// it could be a normal shape drawn on the wrong layer
				// so if it is not tagged with a bar tag make it ok
				ok = TRUE;

				// of course we need to find the template attribs first

				Node * pAttr = SliceHelper::FindNextOfClass(pCurrent,pCurrent, CC_RUNTIME_CLASS(TemplateAttribute));

				while (pAttr)
				{
					if (!SliceHelper::GetBarName((TemplateAttribute *)pAttr).IsEmpty())
					{
						ok = FALSE;
						break;
					}

					pAttr = SliceHelper::FindNextOfClass(pAttr,pCurrent, CC_RUNTIME_CLASS(TemplateAttribute));
				}
			}

			if (ok) // add to the deletion list - its on Layer 1 or whatever
			{
				NodeListItem * pItem = new NodeListItem(pCurrent);
				pDeletionList->AddTail(pItem);
			}
		}
		else // drawn on the layer you are going to copy to? 
			//  still want to delete your original drawing as it wont have the tags required
		{
			// only want to copy from a single button
			// if it is from a different button do not add it to the selection or
			// delete it
			// of course we need to find the template attribs first

			Node * pAttr = SliceHelper::FindNextOfClass(pCurrent,pCurrent, CC_RUNTIME_CLASS(TemplateAttribute));
			BOOL FromThisBar = TRUE;

			while (pAttr)
			{
				if (!SliceHelper::GetBarName((TemplateAttribute *)pAttr).IsEmpty())
				{
					// the source is from another bar
					if (m_BarName.CompareTo(SliceHelper::GetBarName((TemplateAttribute *)pAttr)) != 0)
					{
						FromThisBar = FALSE;
					}

					String_256 AttrSet = SliceHelper::GetSetNameFromAttrib(pAttr);
					if (ButtonBeingEdited.IsEmpty())
					{
						ButtonBeingEdited = AttrSet;
						break;
					}
					else if (ButtonBeingEdited == AttrSet || AttrSet.IsEmpty())
					{
						break;
					}
					else
						ok = FALSE; // it is of another bar name set
					break;
				}

				pAttr = SliceHelper::FindNextOfClass(pAttr,pCurrent, CC_RUNTIME_CLASS(TemplateAttribute));
			}
			
			if (ok && FromThisBar) // delete it from here
			{
				NodeListItem * pItem = new NodeListItem(pCurrent);
				pDeletionList->AddTail(pItem);
			}
		}

		if (ok)
		{ // add to the selection list
			NodeListItem * pItem = new NodeListItem(pCurrent);
			pSelectionList->AddTail(pItem);
		}

		// check the next item on the list
		pCurrent = pSel->FindNext(pCurrent);
	}

	return AllOk;
}


/********************************************************************************************

>	void OpBarCreation::ShowState(INT32 ShowLayer)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/00
	Returns:	-
	Purpose:	Makea sure that that just the state is shown, out of the specail states
				Updates the screen drawing and informs other dlgs/galleries such as
				the layer gallery.

********************************************************************************************/
void OpBarCreation::ShowState(INT32 ShowLayer)
{
	OpShowState::ShowState(ShowLayer, this);
}

BOOL OpBarCreation::CreateBarAndBackBar (OpParam* pOpParam)
{

	// unselect the back object and note it down to make into a back bar
	Node * pBackObj = NULL;

	// get the selection
	Range Sel(*(GetApplication()->FindSelection()));

	// set the range flags so it includes shadow and bevel manager nodes
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	if (Sel.Count() < 2)
		return FALSE;
	else
	{
		pBackObj = Sel.FindFirst();
		pBackObj->SetSelected(FALSE);
	}


	BOOL ok = CreateOrEditBar(pOpParam);

	if (!ok)
	{
		InformWarning(_R(IDS_FAILLED_MAKE_BAR));
		return FALSE;
	}

	// deselect everything
	NodeRenderableInk::DeselectAll(TRUE);

	// select everything in the group not the group itself
	// dont bother with this now as we deal with true groups extending
/*	if (pBackObj->IsCompound())
	{
		pBackObj = pBackObj->FindFirstChild();
		while (pBackObj)
		{
			if (!pBackObj->IsNodeHidden() && !pBackObj->IsAnAttribute())
				pBackObj->SetSelected(TRUE);

			pBackObj = pBackObj->FindNext();
		}
	}
	else // else select the node
*/		pBackObj->SetSelected(TRUE);

	ok = CreateBackBarFromSelection(pOpParam, TRUE);

	if (!ok)
		InformWarning(_R(IDS_FAILLED_MAKE_BACKBAR));

	return ok;
}

