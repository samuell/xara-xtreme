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
// Code for the Duplication of bars
// and the synchronisation of text across a bar


#include "camtypes.h"
//#include "sliceres.h"
//#include "simon.h"

#include "transop.h"
#include "spread.h"

#include "slice.h"

#include "cxfrech.h"
#include "userattr.h"
#include "tmpltatr.h"

#include "progress.h"
#include "fillval.h"
#include "fillattr.h"

#include "ngcore.h"
#include "ngitem.h"
#include "ngprop.h"
#include "ngsentry.h"

#include "slicehelper.h"
#include "opdupbar.h"
#include "slicetool.h"

#include "nodetxtl.h" // for textline def
#include "objchge.h" // for child change stuff
#include "sprdmsg.h"	// SpreadMsg

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE(OpDuplicateBar, CarbonCopyOp)
CC_IMPLEMENT_DYNCREATE(OpShortenBar, CarbonCopyOp)
CC_IMPLEMENT_DYNCREATE(OpDelBar, CarbonCopyOp)
CC_IMPLEMENT_DYNCREATE(OpShowState, UndoableOperation)


#define new CAM_DEBUG_NEW



/********************************************************************************************

>	OpClone::OpDuplicateBar() 

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/99
	Purpose:	OpBarCreation constructor
	Errors:		-
	SeeAlso:	CarbonCopyOp

********************************************************************************************/            
OpDuplicateBar::OpDuplicateBar(): CarbonCopyOp()								
{
	m_ExistingButtonsInBar = 0;
}



/********************************************************************************************

>	BOOL OpDuplicateBar::Init()

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/99
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpDuplicateBar initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/
BOOL OpDuplicateBar::Init()
{
	return	(RegisterOpDescriptor(0,
	 							_R(IDS_BARDUPLICATIONOP),
								CC_RUNTIME_CLASS(OpDuplicateBar),
 								OPTOKEN_BARDUPLICATIONOP,
 								OpDuplicateBar::GetState,
 								0,					// help ID 
 								0, //_R(IDBBL_BARDUPLICATIONOP),// bubble help
								0,					// resource ID
								0, //_R(IDC_BC_CREATE),		// control ID
								SYSTEMBAR_ILLEGAL,	// Bar ID
								TRUE, 				// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE) ));
}



/********************************************************************************************

>	OpState	OpDuplicateBar::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com> based on Jason
	Created:	30/9/99
	Returns:	The state of the OpDuplicateBar
	Purpose:	For finding the operations state.  

********************************************************************************************/
OpState	OpDuplicateBar::GetState(String_256* UIDescription, OpDescriptor *Bob)
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

>	void OpDuplicateBar::DoWithParam(OpDescriptor* token, OpParam* pOpParam)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/99
	Inputs:		OpParam - describes the bar to be dupicated
						- and the name to call the copy
	Outputs:	-
	Returns:	-
	Purpose:	Clones a bar, but gives the bar a new name and each button in the bar
				and new name. This is so each bar can work independantly or together.
				If they are just copied they would have the same bar IDs and the same
				button names which is not likely to be what the user would expect.
	Errors:		-
	SeeAlso:	OpBarCreation::DoWithParam, CarbonCopyOp::DoProcessing

********************************************************************************************/	
void OpDuplicateBar::DoWithParam(OpDescriptor* token, OpParam* pOpParam)
{
	if (DoStartSelOp(FALSE,TRUE))  // Try to record the selection state
	{
		// unpack the names of the bar to be copied and the new bars name from the params
		DuplicateBar(((OpParamBarDuplication *)pOpParam)->m_OldBarName,
					((OpParamBarDuplication *)pOpParam)->m_NewBarName,
					((OpParamBarDuplication *)pOpParam)->m_ButtonsToAdd,
					((OpParamBarDuplication *)pOpParam)->m_Spacing,
					((OpParamBarDuplication *)pOpParam)->m_IsHoriz);


		// end the op
		End();

		DialogBarOp::SetSystemStateChanged();
		DialogBarOp::UpdateStateOfAllBars(); 

	}
	else
	{
		FailAndExecute();
		End();
	}
}


/********************************************************************************************

>	void OpDuplicateBar::DuplicateBar(String_256 &OriginalBarName, String_256 &NewBarName)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/99
	Inputs:		OriginalBarName - the bar to be dupicated
				NewBarName		- the name to call the copy
				ButtonsToAdd	- 0 means it copies the bar over with an exact clone
								  any other number implies add this many buttons to the bar
								  must be less or equal to the number of buttons in the bar
								  to start with, since it duplicates the existing ones of
								  that number.
				Spacing			- In conjunction with the buttons to add only spaces out the
								  new buttons.
	Outputs:	-
	Returns:	-
	Purpose:	Clones a bar, but gives the bar a new name and each button in the bar
				and new name. This is so each bar can work independantly or together.
				If they are just copied they would have the same bar IDs and the same
				button names which is not likely to be what the user would expect.

********************************************************************************************/	
void OpDuplicateBar::DuplicateBar(String_256 &OriginalBarName, String_256 &NewBarName, INT32 ButtonsToAdd, INT32 Spacing, BOOL IsHoriz)
{

	// scan each of the special state layers for the original bar members
	// pionters to these template attributes are stored in the BarList

	// I don't want bar duplication at the moment thanks.
	// I just want to 
	ASSERT (ButtonsToAdd > 0);

	String_256 templayer;
	Layer * pLayer = NULL;

	List BarList;

	templayer.Load(_R(IDS_ROLLOVER_DEFAULT));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	Layer * pDefLayer = pLayer;
	if (pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, OriginalBarName);

	templayer.Load(_R(IDS_ROLLOVER_MOUSE));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if (pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, OriginalBarName);

	templayer.Load(_R(IDS_ROLLOVER_CLICKED));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if (pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, OriginalBarName);

	templayer.Load(_R(IDS_ROLLOVER_SELECTED));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if (pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, OriginalBarName);

	// calculate the dimensions of the bar
	// this is so we can work out where to place the bar copy
	// which is incidently placed either just to the right
	// or just bellow depending upon orientation

	NodeListItem *pNodeListItem = (NodeListItem *)BarList.GetHead();
	NodeListItem *pDelThisToo = NULL;

	// find the number of buttons in the bar and what they are called
	m_ExistingButtonsInBar = 0;
	SliceHelper::CountButtonsInBarScan(pDefLayer, (TemplateAttribute **) m_ppFoundButton, &m_ExistingButtonsInBar, OriginalBarName);

	templayer = ((TemplateAttribute *) m_ppFoundButton[0])->GetParam();

	DocRect BarDim;
	DocRect FirstButtonDim;

	BarDim.MakeEmpty();
	FirstButtonDim.MakeEmpty();

	while(pNodeListItem)
	{
		Node* pNode = pNodeListItem->pNode->FindParent();
		if (pNode)
		{
			BarDim = BarDim.Union(((NodeRenderableBounded *) pNode)->GetBoundingRect());
			if (templayer.CompareTo(((TemplateAttribute *)pNodeListItem->pNode)->GetParam()) == 0)
				FirstButtonDim = FirstButtonDim.Union(((NodeRenderableBounded *) pNode)->GetBoundingRect());
		}
		pNodeListItem = (NodeListItem *)BarList.GetNext(pNodeListItem);
	}
	
	// calc the transform matrix to be applied to each element when we copy it
	Trans2DMatrix Transformer;

	// test differences between the first button and the whole bar in with compared to height
	if (BarDim.hix - BarDim.lox + FirstButtonDim.hix - FirstButtonDim.lox > 
		BarDim.hiy - BarDim.loy + FirstButtonDim.hiy - FirstButtonDim.loy)
		Transformer.SetTransform(0, BarDim.loy - BarDim.hiy);
	else
		Transformer.SetTransform(BarDim.hix - BarDim.lox, 0);

	// find new names for the new buttons
	String_256 TempButtonName;
	INT32 i = 0, listbuttonno = 0;
	for (i = 0; i < MAX_BUTTONS_IN_A_BAR; i++)
	{
		m_ButtonNumberList[i] = i;
		if (i < max(m_ExistingButtonsInBar, ButtonsToAdd))
		{
			// get the next free button name and put it into listbuttonno
			SliceHelper::GetNextFreeButtonName(listbuttonno);

			m_ButtonNumberList[i] = listbuttonno;
		}
	}

	// copy each item referenced by a template attrib marked with this bar ID
	pNodeListItem = (NodeListItem *)BarList.GetHead();
	String_256 Name = "";

	// Prepare an ObjChangeParam so we can mark which nodes will allow this op to happen to them
	ObjChangeFlags cFlags;
	//cFlags.MultiReplaceNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	while(pNodeListItem)
	{
		pDelThisToo = pNodeListItem;
		Node *pNode = pNodeListItem->pNode;
		if (pNode && pNode->AllowOp(&ObjChange))
		{
			if (ButtonsToAdd == 0) // add the whole lot as a new bar
			{
				Node * NewNode = CopyBarMember(pNode, OriginalBarName, NewBarName, &Transformer);
				if (NewNode)
					NewNode->AllowOp(&ObjChange);
			}
			else
			{
				// add a few buttons on the end
				Name = ((TemplateAttribute *)pNode)->GetParam();

				// note it just copies the first button several times to make the new buttons
				if (Name.CompareTo(((TemplateAttribute *) m_ppFoundButton[0])->GetParam()) == 0)
					for (i = 0; i < ButtonsToAdd; i++)
					{
						// alter tranformer for each button
						if (IsHoriz)
							Transformer.SetTransform(BarDim.hix - BarDim.lox + (i+1)*(Spacing) + i*(FirstButtonDim.hix - FirstButtonDim.lox), 0);
						else
							Transformer.SetTransform(0, BarDim.loy - BarDim.hiy - ((i+1)*(Spacing) + i*( FirstButtonDim.hiy - FirstButtonDim.loy)));

						Node * NewNode = CopyBarMember(pNode, OriginalBarName, OriginalBarName, &Transformer, &i);
						if (NewNode)
							NewNode->AllowOp(&ObjChange);
					}
			}
		}

		pNodeListItem = (NodeListItem *)BarList.GetNext(pNodeListItem);

		// tidy up the list as we go along
		BarList.RemoveItem((NodeListItem *)pDelThisToo);
		delete pDelThisToo;
	}

	// touch the bar in question
	NameGallery * pNameGallery = NameGallery::Instance();
	if (pNameGallery)
	{
		pNameGallery->m_TouchedBar = SliceHelper::GetBarNumberFromBarName(NewBarName);
	}

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	UpdateChangedNodes(&ObjChange);


	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED));

}


/********************************************************************************************

>	void OpDuplicateBar::CopyBarMember(Node * pNode, const String_256 &OriginalBarName, const String_256 &NewBarName, Trans2DMatrix * pTransformer)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/99
	Inputs:		pNode			-	Ptr to the Template attribute that marks this 
									nodes parent as being a member of the bar
				OriginalBarName	-	The name of the bar that is being copied
				NewBarName		-	The name of the copy of the bar
				pTransformer	-	The transform matrix that is applied to all copies
				pKnownButtonIndex-	Ptr to an INT32 that if defined saves a bit of calculation
									since it is the index into the m_ButtonNumberList that
									stores the button number of the button being made.
									If not defined it gets this number from a scan based on
									the button name of this node being copied.
	Outputs:	-
	Returns:	-
	Purpose:	Copies a single member of a bar.
				Any template attributes that relate to the old original bar
				are replaced with references to the new bar.
				Also will generate new stretching attributes for a new button
				based on those of the source.
	Errors:		-

********************************************************************************************/	
Node* OpDuplicateBar::CopyBarMember(Node * pNode, const String_256 &OriginalBarName, const String_256 &NewBarName, Trans2DMatrix * pTransformer, const INT32 * pKnownButtonIndex)
{
	// Make a copy of the current node
	Node* pTheCopy;

	// Assumes that the parent of the wix attribute is the main node that needs
	// copying. This is a fair assumption being as that is where the bar creation
	// op puts it.
	Node* pCurrent = pNode->FindParent();
	BOOL CopiedOK = TRUE;

	String_256	Target = ""; // the button name of the selection
	String_256	Extender = ""; // the thing in this button that extends it
	DocRect rTarget;
	DocRect rExtender;
	BYTE ExtenderFlags = 0;
	INT32 RequiredLevel = 0;
	RequiredLevel = SliceHelper::FindTargetAndExtender(pCurrent, Target, Extender, RequiredLevel, &ExtenderFlags, &rTarget, &rExtender);

	// no longer unselect the old bar as this fn is only used for adding buttons from button 1 really now
	//pCurrent->SetSelected(FALSE);

	// where to insert this new node
	Node * pInsertPt = pCurrent->FindParent();
	while (!pInsertPt->IsLayer())
		pInsertPt = pInsertPt->FindParent();

	// create a copy of the node
	CALL_WITH_FAIL(pCurrent->NodeCopy(&pTheCopy), this, CopiedOK);
	if (!CopiedOK) return NULL; // couldn't copy -> fail

	// insert the copy into the tree
	DoInsertNewNode((NodeRenderableBounded *)pTheCopy, pInsertPt, LASTCHILD,
													 TRUE, 		// Do Invalidate region 
													 FALSE); 	// Don't Clear the selections
	// don't have any of these new nodes selected
	pTheCopy->SetSelected(FALSE);

	// clean the attributes of the copy.
	// remove all references to the old bar and
	// replace with references to the new bar.
	// Also update the name given to the button set.
	Node* pAttrib = pTheCopy->FindFirstChild();
	Node* pNext = NULL;

	BOOL goon = TRUE;
	while(pAttrib && goon)
	{
		pNext = pAttrib->FindNext();
		if (pAttrib->IsAnAttribute())
		{
			if (IS_A(pAttrib, TemplateAttribute))
			{
				String_256 ButtonName = ((TemplateAttribute *)pAttrib)->GetParam();
				String_256 AttrName = ((TemplateAttribute *)pAttrib)->GetQuestion();
				// found the actual button marker
				// since this has the marker of the bar name
				BOOL IsBarButton = (OriginalBarName.CompareTo(AttrName) == 0);
				BOOL IsExtender = (ButtonName.CompareTo(Extender) == 0);

				if (IsBarButton || IsExtender)
				{
					String_256 NewButtonName = "";
					
					if (pKnownButtonIndex)
						NewButtonName.MakeMsg(_R(IDS_BUTTONNAME), m_ButtonNumberList[*pKnownButtonIndex]);
					else
						NewButtonName = GetNewButtonName(ButtonName);

					String_256 ExtenderPostfix;
					ExtenderPostfix.Load(_R(IDS_EXTENDER_POSTFIX));
					String_256 NewExtenderName = NewButtonName;
					NewExtenderName += ExtenderPostfix;

					TemplateAttribute* pNewAttr = new TemplateAttribute(String_256(TEXT("ObjectName")),
																		IsBarButton ? NewBarName : "",
																		IsBarButton ? NewButtonName : NewExtenderName);

					if (pNewAttr) // create the node ok
					{
						pNewAttr->AttachNode(pTheCopy, FIRSTCHILD);

						if (IsBarButton)
						{
							if (!Target.IsEmpty() && RequiredLevel > 1)
								// create the properties for the button/Target that this extender extends
								SliceHelper::CreatePropertiesForSet(NewButtonName, NewBarName, TRUE, TRUE, FALSE, ExtenderFlags,
																		NewExtenderName, FALSE, &rTarget, &rExtender, this);
							else // create none stretching attribs for the button
								SliceHelper::CreatePropertiesForSet(NewButtonName, NewBarName, TRUE, FALSE, FALSE, 0,
																		TEXT(""), FALSE, NULL, NULL, this);

							// test to see if this button is in fact a trigger for something bigger
							// if the source is ButtonName then add NewButtonName to that list
							SliceHelper::MakeTriggerLikeExample(NewButtonName, ButtonName);
						}
						else // create the properties for the extender
						{
							SliceHelper::CreatePropertiesForSet(NewExtenderName, TEXT(""), FALSE, FALSE, FALSE, 0,
																	TEXT(""), FALSE, NULL, NULL, this);
							// test to see if this extender is in fact a trigger for something bigger
							// but not just the button we already know it to trigger
							// if the source is ButtonName then add NewButtonName to that list
							SliceHelper::MakeTriggerLikeExample(NewExtenderName, Extender, &Target);
						}

						DoHideNode(pAttrib, TRUE);
					}
				}
			}
		}

		pAttrib = pNext;
	}

	// transform the node into its new location in the bar
	((NodeRenderableBounded *)pTheCopy)->Transform(*pTransformer);

	return pTheCopy;
}

/********************************************************************************************

>	String_256 OpDuplicateBar::GetNewButtonName(String_256 &Name)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/99
	Inputs:		Name	-	The name of the button in the original bar
	Outputs:	-
	Returns:	The corresponding button in the new bar
	Purpose:	Generates names for the new bar.

********************************************************************************************/	
String_256 OpDuplicateBar::GetNewButtonName(String_256 &Name)
{
	String_256 TempButtonName;
	INT32 butno = -1;
	INT32 i = 0;
	for (i = 0; butno < 0 && i < m_ExistingButtonsInBar; i++)
	{
		if (Name.CompareTo(((TemplateAttribute *) m_ppFoundButton[i])->GetParam()) == 0)
			butno = i;
	}

	// return the button if we have found the extender
	if (butno < 0)
	{
		String_256 ExtenderPostfix;
		ExtenderPostfix.Load(_R(IDS_EXTENDER_POSTFIX));

		for (i = 0; butno < 0 && i < m_ExistingButtonsInBar; i++)
		{
			TempButtonName = ((TemplateAttribute *) m_ppFoundButton[i])->GetParam();
			TempButtonName += ExtenderPostfix;

			if (Name.CompareTo(TempButtonName) == 0)
				butno = i;
		}
	}

	if (butno < 0)
		butno = m_ExistingButtonsInBar;

	TempButtonName.MakeMsg(_R(IDS_BUTTONNAME), m_ButtonNumberList[butno]);
	return TempButtonName;
}







/********************************************************************************************

>	OpShortenBar::OpShortenBar() 

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/99
	Purpose:	OpShortenBar constructor
	Errors:		-
	SeeAlso:	CarbonCopyOp

********************************************************************************************/            
OpShortenBar::OpShortenBar(): CarbonCopyOp()								
{                              
}



/********************************************************************************************

>	BOOL OpShortenBar::Init()

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/99
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpShortenBar initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/
BOOL OpShortenBar::Init()
{
	return	(RegisterOpDescriptor(0,
	 							_R(IDS_BARDUPLICATIONOP),
								CC_RUNTIME_CLASS(OpShortenBar),
 								OPTOKEN_SHORTENBAROP,
 								OpShortenBar::GetState,
 								0,					// help ID 
 								0, //_R(IDBBL_BARDUPLICATIONOP),// bubble help
								0,					// resource ID
								0, //_R(IDC_BC_CREATE),		// control ID
								SYSTEMBAR_ILLEGAL,	// Bar ID
								TRUE, 				// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE) ));
}



/********************************************************************************************

>	OpState	OpShortenBar::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com> based on Jason
	Created:	15/10/99
	Returns:	The state of the OpShortenBar
	Purpose:	For finding the operations state.  

********************************************************************************************/
OpState	OpShortenBar::GetState(String_256* UIDescription, OpDescriptor *Bob)
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

>	void OpShortenBar::DoWithParam(OpDescriptor* token, OpParam* pOpParam)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/99
	Inputs:		OpParam - describes the bar to be dupicated
						- and the name to call the copy
	Outputs:	-
	Returns:	-
	Purpose:	Removes buttons from the bar so as to leave fewer.
	Errors:		-
	SeeAlso:	TrimBarDownToThisManyButtons
********************************************************************************************/	
void OpShortenBar::DoWithParam(OpDescriptor* token, OpParam* pOpParam)
{
	if (DoStartSelOp(FALSE,TRUE))  // Try to record the selection state
	{
		// unpack the names of the bar to be copied and the new bars name from the params
		TrimBarDownToThisManyButtons(((OpParamShortenBar *)pOpParam)->m_BarName,
									((OpParamShortenBar *)pOpParam)->m_NoOfButtons);

		// end the op
		End();

		DialogBarOp::SetSystemStateChanged();
		DialogBarOp::UpdateStateOfAllBars(); 

	}
	else
	{
		FailAndExecute();
		End();
	}
}

/********************************************************************************************

>	BOOL OpShortenBar::TrimBarDownToThisManyButtons(const StringBase &BarName, INT32 NewNoOfButtons)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/99
	Inputs:		BarName			- Name of the bar in for a trim
				NewNoOfButtons	- No of buttons that we want in this bar
	Outputs:	-
	Returns:	TRUE if it succeeds. FALSE if you try to increase the number of
				buttons by using this function or it fails in some way.
	Purpose:	Removes buttons from the bar so as to leave fewer.
	Errors:		-
********************************************************************************************/	
BOOL OpShortenBar::TrimBarDownToThisManyButtons(const StringBase &BarName, INT32 NewNoOfButtons)
{
	// find me the buttons in this bar
	String_256 templayer;
	templayer.Load(_R(IDS_ROLLOVER_DEFAULT));

	// count the buttons on the default layer
	// every button bar requires a member to be on the default layer
	// so there is no point in counting any other layer
	Layer * pDef = SliceHelper::FindLayerCalled(templayer);

	if (!pDef) return FALSE; // no default layer found

	INT32 NumberOfButtons = 0;

	// store of example attribs of each button in this bar
	// but we don't need this data so we can throw it away afterwards
	// MAX_BUTTONS_IN_A_BAR = 20
	TemplateAttribute ** ppFoundButton[MAX_BUTTONS_IN_A_BAR];
	DocRect ButtonRect[MAX_BUTTONS_IN_A_BAR];
	BYTE	Order[MAX_BUTTONS_IN_A_BAR];

	// scan down from the default layer counting the different button names
	// that all have the same bar name
	SliceHelper::CountButtonsInBarScan(pDef, (TemplateAttribute **) ppFoundButton, &NumberOfButtons, BarName);

	if (NumberOfButtons == 0 || NumberOfButtons <= NewNoOfButtons ) return FALSE;


	// build up a list of the buttons in the bar
	// so that we can shift them along
	// it must be better to use a list than keep on
	// scanning each time

	Layer * pLayer = NULL;

	List BarList;

	templayer.Load(_R(IDS_ROLLOVER_DEFAULT));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if (pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, BarName);

	templayer.Load(_R(IDS_ROLLOVER_MOUSE));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if (pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, BarName);

	templayer.Load(_R(IDS_ROLLOVER_CLICKED));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if (pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, BarName);

	templayer.Load(_R(IDS_ROLLOVER_SELECTED));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if (pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, BarName);

	// init stuff for the translation
	String_256	ButtonName = "";
	NodeListItem *pNodeListItem = NULL;
	NodeListItem *pDelThisToo = NULL;
	Node *pNode = NULL;
	Node *pParent = NULL;
	
	INT32 i = 0;

	// how big is each rect of the gallery?
	for (i = 0; i < NumberOfButtons; i++)
	{
		Order[i] = i; // init order

		ButtonName = ((TemplateAttribute *) ppFoundButton[i])->GetParam();
		pNodeListItem = (NodeListItem *)BarList.GetHead();
		ButtonRect[i].MakeEmpty();

		while(pNodeListItem)
		{
			pNode = pNodeListItem->pNode;
			if (pNode) 
			{
				pParent = pNode->FindParent();
				if (pParent && ButtonName.CompareTo(((TemplateAttribute *)pNode)->GetParam()) == 0)
				{
					// transform the node into its new location in the bar
					ButtonRect[i] = ButtonRect[i].Union(((NodeRenderableBounded *)pParent)->GetBoundingRect());
				}
			}
			// find the next item in the list
			pNodeListItem = (NodeListItem *)BarList.GetNext(pNodeListItem);
		}
	}

	// sort them by location
	for (i = 0; i < NumberOfButtons; i++)
	{
		for (INT32 j = i+1; j < NumberOfButtons; j++)
		{
			if (ButtonRect[Order[j]].hiy > ButtonRect[Order[i]].hiy)
			{
				BYTE t = Order[j];
				Order[j] = Order[i];
				Order[i] = t;
			}
			else
			if (ButtonRect[Order[j]].hiy == ButtonRect[Order[i]].hiy &&
				ButtonRect[Order[j]].lox < ButtonRect[Order[i]].lox)
			{
				BYTE t = Order[j];
				Order[j] = Order[i];
				Order[i] = t;
			}
		}
	}

	// tidy up the list
	pNodeListItem = (NodeListItem *)BarList.GetHead();
	pParent = NULL;
	// Prepare an ObjChangeParam so we can mark which nodes will allow this op to happen to them
	ObjChangeFlags cFlags(TRUE); // flag says deleting okay?
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);


	while(pNodeListItem)
	{
		pDelThisToo = pNodeListItem;

		// delete any node that is ordered after NewNoOfButtons
		// by finding out if the name of this item in the bar
		// is the same as any that occur after the required
		// number of buttons in the bar
		pParent = pNodeListItem->pNode->FindParent();
		if (pParent)
			{
				ButtonName = ((TemplateAttribute *)pNodeListItem->pNode)->GetParam();

				for (i = NewNoOfButtons; i < NumberOfButtons; i++)
				{
					if (ButtonName.CompareTo(((TemplateAttribute *) ppFoundButton[Order[i]])->GetParam()) == 0
						&& pParent->AllowOp(&ObjChange))
					{
						// We also need to invalidate the region of the node now that it has been transformed
						DoInvalidateNodeRegion((NodeRenderableBounded*) pParent, TRUE, FALSE);

						// remove the node from the excess button
						pParent->SetSelected(FALSE);
						DoHideNode(pParent, TRUE);
						
						i = NumberOfButtons; // shortcut out
					}
				}
			}

		pNodeListItem = (NodeListItem *)BarList.GetNext(pNodeListItem);

		// tidy up the list as we go along
		BarList.RemoveItem((NodeListItem *)pDelThisToo);
		delete pDelThisToo;
	}

	// touch the bar in question
	NameGallery * pNameGallery = NameGallery::Instance();
	if (pNameGallery)
	{
		pNameGallery->m_TouchedBar = SliceHelper::GetBarNumberFromBarName(BarName);
	}

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	UpdateChangedNodes(&ObjChange);
	
	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED));

	return TRUE;
}









// contructor
OpDelBar::OpDelBar(): CarbonCopyOp()								
{                              
}
/********************************************************************************************

>	BOOL OpDelBar::Init()

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/00
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpDelBar initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/
BOOL OpDelBar::Init()
{
	return	(RegisterOpDescriptor(0,
	 							_R(IDS_DEL_BAR),
								CC_RUNTIME_CLASS(OpDelBar),
 								OPTOKEN_DELBAROP,
 								OpDelBar::GetState,
 								0,					// help ID 
 								0,					// bubble help
								0,					// resource ID
								0,					// control ID
								SYSTEMBAR_ILLEGAL,	// Bar ID
								TRUE, 				// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE) ));
}



/********************************************************************************************

>	OpState	OpDelBar::GetState(String_256* UIDescription, OpDescriptor *Bob)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com> based on Jason
	Created:	3/5/00
	Purpose:	For finding the operations state.  

********************************************************************************************/
OpState	OpDelBar::GetState(String_256* UIDescription, OpDescriptor *Bob)
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

>	void OpDelBar::DoWithParam(OpDescriptor* token, OpParam* pOpParam)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/00
	Inputs:		OpParam - describes the bar to be deleted
						- and the layers to delete it from
	Outputs:	-
	Returns:	-
	Errors:		-
*******************************************************************************************/	
void OpDelBar::DoWithParam(OpDescriptor* token, OpParam* pOpParam)
{
	if (DoStartSelOp(FALSE,TRUE))  // Try to record the selection state
	{
		// Delete the bar
		DelBar(((OpParamDelBar *)pOpParam)->m_BarName, ((OpParamDelBar *)pOpParam)->m_State, this);

		// end the op
		End();

		DialogBarOp::SetSystemStateChanged();
		DialogBarOp::UpdateStateOfAllBars(); 

	}
	else
	{
		FailAndExecute();
		End();
	}
}

BOOL OpDelBar::DelBar(const StringBase &BarName, INT32 State, UndoableOperation * pOp)
{
	String_256 templayer;
	Layer * pLayer = NULL;

	// Prepare an ObjChangeParam so we can mark which nodes will allow this op to happen to them
	ObjChangeFlags cFlags;
	cFlags.DeleteNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,pOp);

	List BarList;

	templayer.Load(_R(IDS_ROLLOVER_DEFAULT));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	Layer * pDefLayer = pLayer;
	if ((State == 0 || State == 5 ) && pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, BarName);

	templayer.Load(_R(IDS_ROLLOVER_MOUSE));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if ((State == 1 || State == 5 ) && pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, BarName);

	templayer.Load(_R(IDS_ROLLOVER_CLICKED));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if ((State == 2 || State == 5 ) && pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, BarName);

	templayer.Load(_R(IDS_ROLLOVER_SELECTED));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if ((State == 3 || State == 5 ) && pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, BarName);
	
	templayer.Load(_R(IDS_BACK_BAR));
	pLayer = SliceHelper::FindLayerCalled(templayer);
	if ((State == 4 || State == 5 ) && pLayer)
		SliceHelper::BuildListOfNodesInBar(&BarList, pLayer, BarName);



	
	// loop through all nodes in the list to delete
	NodeListItem *pNodeListItem = (NodeListItem *)BarList.GetHead();

	while (pNodeListItem)
	{
		NodeListItem *pNextNodeListItem = (NodeListItem *)BarList.GetNext(pNodeListItem);

		Node * pNode = pNodeListItem->pNode->FindParent();

		if (pNode && pNode->AllowOp(&ObjChange))
		{
			pOp->DoInvalidateNodeRegion((NodeRenderableBounded*) pNode, TRUE, FALSE);
			pOp->DoHideNode(pNode, TRUE);
		}

		// tidy up the list as we go along
		BarList.RemoveItem((NodeListItem *)pNodeListItem);
		delete pNodeListItem;

		pNodeListItem = pNextNodeListItem;
	}

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,pOp);
	pOp->UpdateChangedNodes(&ObjChange);

	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED));

	return TRUE;
}



// contructor
OpShowState::OpShowState(): UndoableOperation()								
{                              
}
/********************************************************************************************

>	BOOL OpShowState::Init()

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/00
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpShowState initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/
BOOL OpShowState::Init()
{
	return	(RegisterOpDescriptor(0,
	 							_R(IDS_SHOWSTATEOP),
								CC_RUNTIME_CLASS(OpShowState),
 								OPTOKEN_SHOWSTATE,
 								OpShowState::GetState,
 								0,					// help ID 
 								0,					// bubble help
								0,					// resource ID
								0,					// control ID
								SYSTEMBAR_ILLEGAL,	// Bar ID
								TRUE, 				// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE) ));
}



/********************************************************************************************

>	OpState	OpShowState::GetState(String_256* UIDescription, OpDescriptor *Bob)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com> based on Jason
	Created:	6/7/00
	Purpose:	For finding the operations state.  

********************************************************************************************/
OpState	OpShowState::GetState(String_256* UIDescription, OpDescriptor *Bob)
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

>	void OpShowState::DoWithParam(OpDescriptor* token, OpParam* pOpParam)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/00
	Inputs:		OpParam - Param1 has the layer/state id in it
	Outputs:	-
	Returns:	-
	Errors:		-
*******************************************************************************************/	
void OpShowState::DoWithParam(OpDescriptor* token, OpParam* pOpParam)
{
	// Delete the bar
	ShowState(pOpParam->Param1, this);

	// end the op
	End();

	DialogBarOp::SetSystemStateChanged();
	DialogBarOp::UpdateStateOfAllBars(); 
}


/********************************************************************************************

>	void OpShowState::ShowState(INT32 ShowLayer, UndoableOperation * pOp)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/00
	Inputs:		ShowLayer	-	number of the specail layers to display 0 = mouseoff, 1 = mouseover, 2= clicked, 3 = selected, 4 = all the above
				pOp			-	Unodable Op to use, can be NULL in which case not undoable.
	Outputs:	-
	Returns:	-
*******************************************************************************************/	
void OpShowState::ShowState(INT32 ShowLayer, UndoableOperation * pOp)
{
	Spread* pSelSpread = Document::GetSelectedSpread();
	if (pSelSpread == NULL)
		return;

	String_32 StateName[4];

	StateName[DEFAULT].Load(_R(IDS_ROLLOVER_DEFAULT)); // = "Default";
	StateName[MOUSE].Load(_R(IDS_ROLLOVER_MOUSE)); // = "Mouse";
	StateName[CLICKED].Load(_R(IDS_ROLLOVER_CLICKED)); // = "Clicked";
	StateName[SELECTED].Load(_R(IDS_ROLLOVER_SELECTED)); // = "Selected";

	Layer * pLayer = pSelSpread->FindFirstLayer();

	Layer * pLayerTopLayer = NULL;
	BOOL ActiveLayerIsAState = FALSE;
	BOOL SetAnActiveLayer = FALSE;

	DocRect InvalidRect;

	InvalidRect.MakeEmpty();

	while (pLayer)
	{
		BOOL IsStateLayer = FALSE;
		INT32 i = 0;
		for (i = 0; i < 4 ; i++)
		{
			if (pLayer->GetLayerID().CompareTo(StateName[i]) == 0)
			{
				IsStateLayer = TRUE;
				if (ShowLayer == 4 /* show all layers at once */
					|| ShowLayer == i)
				{
					if (!pLayer->IsVisible())
					{
						if (pOp)
							SliceHelper::ShowLayer(TRUE, pLayer, pSelSpread, pOp);
						else
							pLayer->SetVisible(TRUE);

						InvalidRect = InvalidRect.Union(pLayer->GetBoundingRect());
					}
				}
				else
				{
					if (pLayer->IsVisible())
					{
						if (pOp)
							SliceHelper::ShowLayer(FALSE, pLayer, pSelSpread, pOp);
						else
							pLayer->SetVisible(FALSE);

						InvalidRect = InvalidRect.Union(pLayer->GetBoundingRect());
						// If layer has just about to become invisible,
						// remove the selections on the layer 
						//NodeRenderableInk::DeselectAllOnLayer(pLayer); 
					}
				}

				if (pLayer->IsActive())
					ActiveLayerIsAState = TRUE;
			}
		}

		if (pLayer->IsVisible() && !IsStateLayer)
			pLayerTopLayer = pLayer;

		pLayer = pLayer->FindNextLayer();
	}
}
