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

// Implementation of the Guides properties tab of the layer properties dialog box

/*

*/

#include "camtypes.h"

// WEBSTER - markn 15/1/97
// Don't need this tab in Webster
#ifndef WEBSTER

#include "layer.h"
#include "layerprp.h"
//#include "lyrprop.h"	// dialog/gadget ids
//#include "nev.h"		// error messages
#include "prpsgds.h"
#include "guides.h"
//#include "coldrop.h"
#include "fixmem.h"
#include "document.h"
//#include "markn.h"
#include "sglayer.h"
#include "colourix.h"
//#include "jason.h"
#include "layermsg.h"

CC_IMPLEMENT_DYNAMIC(GuidesPropertiesTab, 		LayerPropertyTabs)   
CC_IMPLEMENT_DYNCREATE(LayerColourAction,		Action)
CC_IMPLEMENT_MEMDUMP(OpChangeLayerColourParam,	OpParam)
CC_IMPLEMENT_DYNCREATE(OpChangeLayerColour,		UndoableOperation)

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	GuidesPropertiesTab::GuidesPropertiesTab() 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-	
	Outputs:	-
	Returns:	-
	Purpose:	GuidesPropertiesTab constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg; LayerPropertyTabs;

********************************************************************************************/

GuidesPropertiesTab::GuidesPropertiesTab()
{   
	GuideType 		= GUIDELINE_HORZ;
	pColourDropDown = NULL;
}   	     

/********************************************************************************************

>	GuidesPropertiesTab::~GuidesPropertiesTab()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	GuidesPropertiesTab destructor
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg; LayerPropertyTabs;

********************************************************************************************/
																				
GuidesPropertiesTab::~GuidesPropertiesTab()
{
PORTNOTE("other", "Disabled Colour Dropdown")
#ifndef EXCLUDE_FROM_XARALX
	if (pColourDropDown != NULL)
		delete pColourDropDown;
#endif
	GuidelineList.DeleteAll();
}        

/********************************************************************************************

>	BOOL GuidesPropertiesTab::Init()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	GuidesPropertiesTab initialisation routine
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg::Init; LayerPropertyTabs::Init;

********************************************************************************************/
																				
BOOL GuidesPropertiesTab::Init()
{   
	return TRUE;
}        

/******************************************************************************************

>	CDlgResID GuidesPropertiesTab::GetPageID()()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CDlgResID GuidesPropertiesTab::GetPageID()
{
	return _R(IDD_TAB_GUIDELINE_PROPERTIES);
}

/******************************************************************************************

>	BOOL GuidesPropertiesTab::IsPropertyRequired()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Returns:	Returns True if this tab is a program option.	  
	Purpose:	Allows the layer propertiesto determine if this tab is required.

******************************************************************************************/

BOOL GuidesPropertiesTab::IsPropertyRequired()
{
	return TRUE;
}


/******************************************************************************************

>	BOOL GuidesPropertiesTab::CommitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in this tab of the layer properties dialog box.
				Called when ok or apply now is pressed on the main dialog box.
	Errors:		-
	SeeAlso:	GuidesPropertiesTab::InitSection()

******************************************************************************************/

BOOL GuidesPropertiesTab::CommitSection()
{
TRACEUSER( "Neville", _T("GuidesPropertiesTab::CommitSection\n"));
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"GuidesPropertiesTab::CommitSection called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;			// Talk to page failed to return now


	return TRUE;
}

/********************************************************************************************

>	BOOL GuidesPropertiesTab::GreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents 
	Errors:		-
	SeeAlso:	GuidesPropertiesTab::UngreySection;

********************************************************************************************/

BOOL GuidesPropertiesTab::GreySection()
{
TRACEUSER( "Neville", _T("GreySection in GuidesPropertiesTab section\n"));
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"GuidesPropertiesTab::GreySection called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Only update if we are not already grey 
	if (GreyStatus == TRUE)
		return TRUE;

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(FALSE);
	
	GreyStatus = TRUE;

	return ok;
} 

/********************************************************************************************

>	BOOL GuidesPropertiesTab::UngreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document 
	Errors:		-
	SeeAlso:	GuidesPropertiesTab::UngreySection;

********************************************************************************************/

BOOL GuidesPropertiesTab::UngreySection()
{
TRACEUSER( "Neville", _T("UngreySection in GuidesPropertiesTab section\n"));
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"GuidesPropertiesTab::UngreySection called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());	// The GuidesPropertiesTab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Only update if we are not already ungrey 
	if (GreyStatus == FALSE)
		return TRUE;

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(TRUE);
	
	GreyStatus = FALSE;

	return ok;
}

/********************************************************************************************

>	BOOL GuidesPropertiesTab::ChangeControlStatus(const BOOL Status)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 
	Errors:		-
	SeeAlso:	GuidesPropertiesTab::GreySection;	GuidesPropertiesTab::UngreySection;

********************************************************************************************/

BOOL GuidesPropertiesTab::ChangeControlStatus(const BOOL Status)
{
	BOOL Enable;

	Layer* pLayer = GetGuideLayer();
	Enable = (pLayer != NULL) && Status;

	pPropertiesDlg->EnableGadget(_R(IDC_GUIDETAB_GUIDELINELIST),Enable);
	pPropertiesDlg->EnableGadget(_R(IDC_GUIDETAB_HORZ), 		Enable);
	pPropertiesDlg->EnableGadget(_R(IDC_GUIDETAB_VERT), 		Enable);
	pPropertiesDlg->EnableGadget(_R(IDC_GUIDETAB_COLOURLIST), 	Enable);
	pPropertiesDlg->EnableGadget(_R(IDC_GUIDETAB_COLOURTEXT),	Enable);

	pPropertiesDlg->EnableGadget(_R(IDC_GUIDETAB_NEW), 			Status);	// Always available

	INT32 SelCount = pPropertiesDlg->GetSelectedCount(_R(IDC_GUIDETAB_GUIDELINELIST));

	pPropertiesDlg->EnableGadget(_R(IDC_GUIDETAB_DELETE), 		Enable && (SelCount >= 1));
	pPropertiesDlg->EnableGadget(_R(IDC_GUIDETAB_PROPERTIES), 	Enable && (SelCount == 1));

	if (!Enable)
		pPropertiesDlg->DeleteAllValues(_R(IDC_GUIDETAB_GUIDELINELIST));

	return TRUE;
}			

/********************************************************************************************

>	BOOL GuidesPropertiesTab::UpdateSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. Should only init the tab if the page is present.  
				The document name allows the info field on the tab to be filled in correctly.
	Errors:		-
	SeeAlso:	GuidesPropertiesTab::GreySection; GuidesPropertiesTab::UngreySection; GuidesPropertiesTab::InitSection;

********************************************************************************************/

BOOL GuidesPropertiesTab::UpdateSection()
{
TRACEUSER( "Neville", _T("GuidesPropertiesTab::UpdateSection\n"));
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"GuidesPropertiesTab::UpdateSection called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;		// page not present


	// Update our controls here

	// We are happy with what happened
	return TRUE;
}

/********************************************************************************************

>	BOOL GuidesPropertiesTab::BuildGuidelineList(Layer* pLayer)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		pLayer = ptr layer that contains guidelines
	Outputs:	-
	Returns:	-
	Purpose:	Builds a list of the guidelines in the given layer
				Uses member var GuidelineList
	Errors:		-
	SeeAlso:	

********************************************************************************************/

BOOL GuidesPropertiesTab::BuildGuidelineList(Layer* pLayer)
{
	GuidelineList.DeleteAll();
	pPropertiesDlg->DeleteAllValues(_R(IDC_GUIDETAB_GUIDELINELIST));

	ERROR2IF(pLayer == NULL, FALSE,"pLayer is NULL");

	Node* pNode = pLayer->FindFirstChild(CC_RUNTIME_CLASS(NodeGuideline));
	while (pNode != NULL)
	{
		NodeGuideline* pGuideline = (NodeGuideline*) pNode;
		if (pGuideline->GetType() == GuideType)
		{
			GuidelineListItem* pItem = (GuidelineListItem*)GuidelineList.GetHead();
			while (pItem != NULL && (pItem->pGuideline->GetOrdinate() < pGuideline->GetOrdinate()))
				pItem = (GuidelineListItem*)GuidelineList.GetNext(pItem);

			GuidelineListItem* pNewItem = new GuidelineListItem(pGuideline);

			if (pNewItem != NULL)
			{
				if (pItem == NULL)
					GuidelineList.AddTail(pNewItem);
				else
					GuidelineList.InsertBefore(pItem,pNewItem);
			}
		}

		pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeGuideline));
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL GuidesPropertiesTab::ShowGuidelines(Layer* pLayer)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95
	Inputs:		pLayer = ptr layer that contains guidelines
	Outputs:	-
	Returns:	-
	Purpose:	Shows the guidelines in the guideline list
	Errors:		-
	SeeAlso:	

********************************************************************************************/

BOOL GuidesPropertiesTab::ShowGuidelines(Layer* pLayer)
{
	ERROR2IF(pLayer == NULL, FALSE,"pLayer is NULL");

	BuildGuidelineList(pLayer);

	GuidelineListItem* pItem = (GuidelineListItem*)GuidelineList.GetHead();
	while (pItem != NULL)
	{	
		NodeGuideline* pGuideline = pItem->pGuideline;

		MILLIPOINT Ordinate = NodeGuideline::ToUserOrdinate(pGuideline->FindParentSpread(),pGuideline->GetOrdinate(),GuideType);
		pPropertiesDlg->SetDimensionGadgetValue(_R(IDC_GUIDETAB_GUIDELINELIST),Ordinate,pGuideline);

		pItem = (GuidelineListItem*)GuidelineList.GetNext(pItem);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL GuidesPropertiesTab::ShowColours()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Shows the colours of the doc in the colour list
	Errors:		-
	SeeAlso:	GuidesPropertiesTab

********************************************************************************************/

BOOL GuidesPropertiesTab::ShowColours()
{
PORTNOTE("other", "Disabled Colour Dropdown")
#ifndef EXCLUDE_FROM_XARALX
	if (pColourDropDown != NULL)
	{
		IndexedColour* pIndexedColour = NULL;
		Layer* pLayer = GetGuideLayer();
		if (pLayer != NULL)
		{
			DocColour* pDocColour = pLayer->GetGuideColour();
			if (pDocColour != NULL)
			{
				pIndexedColour = pDocColour->FindParentIndexedColour();
				if (pIndexedColour->IsDeleted())
				{
					pLayer->SetGuideColour(NULL);
					pIndexedColour = NULL;
				}
			}
		}

		pColourDropDown->ClearAllSpecialEntries();

		if (pIndexedColour != NULL)
		{
			if (!pIndexedColour->IsNamed())
			{
				String_256 LocalColourText(_R(IDS_LOCALCOLOUR));
				DocColour LocalColour;
				LocalColour.MakeRefToIndexedColour(pIndexedColour);

				pColourDropDown->AddSpecialEntry(&LocalColourText,&LocalColour);
			}

			pColourDropDown->FillInColourList(pIndexedColour);
		}
		else
			pColourDropDown->FillInColourList(NULL,0);
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL GuidesPropertiesTab::ShowDetails()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Shows the details of the associated layer in this tab
	Errors:		-
	SeeAlso:	GuidesPropertiesTab

********************************************************************************************/

BOOL GuidesPropertiesTab::ShowDetails()
{
	Layer* pLayer = GetGuideLayer();
	if (pLayer != NULL)
	{
		ShowGuidelines(pLayer);
		ShowColours();

		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_GUIDETAB_HORZ),GuideType == GUIDELINE_HORZ);
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_GUIDETAB_VERT),GuideType == GUIDELINE_VERT);
	}

	ChangeControlStatus(!GreyStatus);

	return TRUE;
}

/********************************************************************************************

>	BOOL GuidesPropertiesTab::UpdateLayerSection()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Called when the layer-related section of the tab should be updated.
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg; LayerPropertyTabs;

********************************************************************************************/

BOOL GuidesPropertiesTab::UpdateLayerSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"GuidesPropertiesTab::UpdateLayerSection() called with no dialog pointer");
	return (pPropertiesDlg->TalkToPage(GetPageID()) && ShowDetails());
}

/********************************************************************************************

>	BOOL GuidesPropertiesTab::UpdateGuidelineSection()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Called when the guideline-related section of the tab should be updated.
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg; LayerPropertyTabs;

********************************************************************************************/

BOOL GuidesPropertiesTab::UpdateGuidelineSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"GuidesPropertiesTab::UpdateGuidelineSection() called with no dialog pointer");
	return (pPropertiesDlg->TalkToPage(GetPageID()) && ShowDetails());
}

/******************************************************************************************

>	virtual BOOL GuidesPropertiesTab::ColourListChanged(Document* pDoc)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	inputs:		pDoc = ptr to document that contains the changed list, or NULL if there's no longer a list
	Returns:	TRUE if successful, else FALSE
	Purpose:	Called when the colour list changes in some way.
				pDoc points to the doc that contains the list that's changed.
				If pDoc is NULL, then there is no longer a colour list (e.g. all docs closed)
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL GuidesPropertiesTab::ColourListChanged(Document* pDoc)
{
PORTNOTE("other", "Disabled Colour Dropdown")
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"GuidesPropertiesTab::UpdateGuidelineSection() called with no dialog pointer");
	ERROR2IF(pColourDropDown == NULL,FALSE,"ptr to 'colour drop down' is NULL");

	if (pPropertiesDlg->TalkToPage(GetPageID()))
	{
		BOOL Enable = (pDoc != NULL);

		pPropertiesDlg->EnableGadget(_R(IDC_GUIDETAB_COLOURLIST),Enable);
		pPropertiesDlg->EnableGadget(_R(IDC_GUIDETAB_COLOURTEXT),Enable);

		if (Enable)
			ShowColours();
		else
			pColourDropDown->ClearList();			
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL GuidesPropertiesTab::HandleMsg(DialogMsg* Msg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the messages for this layer property tab
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg; LayerPropertyTabs;

********************************************************************************************/

BOOL GuidesPropertiesTab::HandleMsg(DialogMsg* Msg)
{
TRACEUSER( "Neville", _T("GuidesPropertiesTab::HandleMsg\n"));
	ERROR2IF(Msg == NULL,FALSE,"GuidesPropertiesTab::Message null message received");
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"GuidesPropertiesTab::HandleMsg called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;		// no page present = handled message
	
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:	// Initialise controls
			GreyStatus = FALSE;	// we are ungreyed by default
			ok = InitSection();
			if (!ok)
				InformError();
			break;
		case DIM_LFT_BN_CLICKED:
			//LayerPropertyTabs::SetApplyNowState(TRUE);
			if (FALSE) {}
			else if (Msg->GadgetID == _R(IDC_GUIDETAB_HORZ))
			{
				if (GuideType != GUIDELINE_HORZ)
				{
					GuideType = GUIDELINE_HORZ;
					ShowDetails();
				}
			}
			else if (Msg->GadgetID == _R(IDC_GUIDETAB_VERT))
			{
				if (GuideType != GUIDELINE_VERT)
				{
					GuideType = GUIDELINE_VERT;
					ShowDetails();
				}
			}
			else if (Msg->GadgetID == _R(IDC_GUIDETAB_PROPERTIES))
			{
				PropertiesClicked();
				ShowDetails();
			}
			else if (Msg->GadgetID == _R(IDC_GUIDETAB_NEW))
			{
				NewClicked();
				ShowDetails();
			}
			else if (Msg->GadgetID == _R(IDC_GUIDETAB_DELETE))
			{	
				DeleteClicked();
				ShowDetails();
			}
			break;

		case DIM_SELECTION_CHANGED:
		case DIM_TEXT_CHANGED:
			//LayerPropertyTabs::SetApplyNowState(TRUE);
			if (FALSE) {}
			else if (Msg->GadgetID == _R(IDC_GUIDETAB_GUIDELINELIST))
			{
				ChangeControlStatus(!GreyStatus);
			}
			else if (Msg->GadgetID == _R(IDC_GUIDETAB_COLOURLIST))
			{
				ColourChanged(pPropertiesDlg->GetSelectedValueIndex(Msg->GadgetID));
				ShowDetails();
			}
			break;
		default:
			break;
	}
	return TRUE;
}  

/******************************************************************************************

>	BOOL GuidesPropertiesTab::ColourChanged(INT32 Index)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		Index = index of selected item (Values < 0 will return FALSE);
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Handles a change in the colour list selection
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL GuidesPropertiesTab::ColourChanged(INT32 Index)
{
PORTNOTE("other", "Disabled Colour Dropdown")
#ifndef EXCLUDE_FROM_XARALX
	if (pColourDropDown == NULL || Index < 0)
		return FALSE;
 
	IndexedColour* pNewColour = pColourDropDown->DecodeSelection(Index);

	Layer* pLayer = GetGuideLayer();
	if (pLayer != NULL)
	{
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGELAYERCOLOUR);
		ERROR3IF(pOpDesc == NULL,"FindOpDescriptor(OPTOKEN_CHANGELAYERCOLOUR) failed");

		if (pOpDesc != NULL)
		{
			OpChangeLayerColourParam ChangeColourParam(pDocument,pLayer,pNewColour);
			pOpDesc->Invoke(&ChangeColourParam);
		}
//		pLayer->SetGuideColour(pNewColour);
//		LayerSGallery::ForceRedrawLayer(pDocument,pLayer);
	}
#endif
	return TRUE;
}

/******************************************************************************************

>	BOOL GuidesPropertiesTab::DeleteClicked()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Allows you to delete the selected Guidelines
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL GuidesPropertiesTab::DeleteClicked()
{
	INT32* pIndexList = pPropertiesDlg->GetSelectedItems(_R(IDC_GUIDETAB_GUIDELINELIST));
	INT32 Count = GuidelineList.GetCount();

	INT32 i;

	BOOL ok = (pIndexList != NULL && Count > 0);
	NodeGuideline** pGuidelineList = NULL;

	if (ok)
	{
		pGuidelineList = (NodeGuideline**)CCMalloc(sizeof(NodeGuideline*)*(Count+1));

		ok = (pGuidelineList != NULL);

		if (ok)
		{
			for (i=0; (pIndexList[i] >= 0) && (i < Count);i++)
			{
				GuidelineListItem* pItem = (GuidelineListItem*)GuidelineList.FindItem(pIndexList[i]);
				if (pItem != NULL)
					pGuidelineList[i] = pItem->pGuideline;
				else
				{
					pGuidelineList[i] = NULL;
					ERROR3_PF(("List item at index [%d] is NULL",i));
				}
			}

			pGuidelineList[i] = NULL;
		}
	}
	else
		ok = FALSE;

	if (ok)
	{
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_GUIDELINE);
		ERROR3IF(pOpDesc == NULL,"FindOpDescriptor(OPTOKEN_GUIDELINE) failed");

		if (pOpDesc != NULL)
		{
			OpGuidelineParam GuidelineParam;

			GuidelineParam.Method 			= GUIDELINEOPMETHOD_DELETE;
			GuidelineParam.pGuidelineList 	= pGuidelineList;

			pOpDesc->Invoke(&GuidelineParam);
		}
	}

	if (pGuidelineList != NULL)
		CCFree(pGuidelineList);

	if (pIndexList != NULL)
		delete [] pIndexList;

	return ok;
}

/******************************************************************************************
>	BOOL GuidesPropertiesTab::NewClicked()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/95
	Returns:	FALSE if fails
	Purpose:	Allows you to create a new Guideline
******************************************************************************************/

BOOL GuidesPropertiesTab::NewClicked()
{
	// set guideline to be at user ordinate 0
	Spread*    pSpread  = Document::GetSelectedSpread();
	MILLIPOINT Ordinate = NodeGuideline::ToSpreadOrdinate(pSpread,0,GuideType);
	GuidelinePropDlg::SetNewGuidelineParams(GuideType,Ordinate);

	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NEWGUIDELINEPROPDLG);
	if (pOpDesc!=NULL)
		pOpDesc->Invoke();
	else
		ERROR3("GuidesPropertiesTab::NewClicked() - OpDescriptor::FindOpDescriptor(OPTOKEN_NEWGUIDELINEPROPDLG) - failed");

	return TRUE;
}

/******************************************************************************************
>	BOOL GuidesPropertiesTab::PropertiesClicked()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/95
	Returns:	FALSE if fails
	Purpose:	Opens up the guideline properties dlg for the first selected item in the guideline list box
******************************************************************************************/

BOOL GuidesPropertiesTab::PropertiesClicked()
{
	INT32 Index = pPropertiesDlg->GetFirstSelectedItem(_R(IDC_GUIDETAB_GUIDELINELIST));
	Layer* pLayer = GetGuideLayer();
	if (Index < 0 || pLayer == NULL) return FALSE;

	BOOL Valid;
	/*MILLIPOINT Ordinate =*/ pPropertiesDlg->GetDimensionGadgetValue(_R(IDC_GUIDETAB_GUIDELINELIST),pLayer,&Valid,Index);

	if (Valid)
	{
		GuidelineListItem* pItem = (GuidelineListItem*)GuidelineList.FindItem(Index);
		if (pItem!=NULL && pItem->pGuideline!=NULL)
		{
			GuidelinePropDlg::SetEditGuidelineParams(pItem->pGuideline);
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_EDITGUIDELINEPROPDLG);
			if (pOpDesc != NULL)
				pOpDesc->Invoke();
			else
				ERROR3("GuidesPropertiesTab::PropertiesClicked() - OpDescriptor::FindOpDescriptor(OPTOKEN_EDITGUIDELINEPROPDLG) failed");
		}
		else
			ERROR3_PF(("Unable to find guideline item at index (%d)",Index));
	}

	return TRUE;	
}


/******************************************************************************************

>	NodeGuideline* GuidesPropertiesTab::FindClosestGuideline(MILLIPOINT UserOrdinate)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95
	Inputs:		UserOrdinate = Ordinate of guideline in UserCoord space
	Outputs:	-
	Returns:	Ptr to nearest guideline to the given ordinate, or NULL if not found
	Purpose:	This hunts the current layer for guidelines of type GuideType, and returns the nearest
				one to the given ordinate of that type
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

/*
NodeGuideline* GuidesPropertiesTab::FindClosestGuideline(MILLIPOINT UserOrdinate)
{
	NodeGuideline* pNearestGuideline = NULL;
	MILLIPOINT SmallestDelta;

	Layer* pLayer = GetGuideLayer();
	if (pLayer != NULL)
	{
		Node* pNode = pLayer->FindFirstChild(CC_RUNTIME_CLASS(NodeGuideline));
		while (pNode != NULL)
		{
			NodeGuideline* pGuideline = (NodeGuideline*)pNode;

			if (pGuideline->GetType() == GuideType)
			{
				MILLIPOINT Ordinate = NodeGuideline::ToSpreadOrdinate(pGuideline->FindParentSpread(),UserOrdinate,GuideType);
				MILLIPOINT Delta = abs(pGuideline->GetOrdinate() - Ordinate);

				if ((pNearestGuideline == NULL) || (Delta < SmallestDelta))
				{
					pNearestGuideline = pGuideline;
					SmallestDelta = Delta;
				}					
			}

			pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeGuideline));
		}
	}

	return pNearestGuideline;
}
*/

/******************************************************************************************

>	BOOL GuidesPropertiesTab::InitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for this tab on the layer properties dialog box.
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg; LayerPropertyTabs;

******************************************************************************************/

BOOL GuidesPropertiesTab::InitSection()
{
TRACEUSER( "Neville", _T("GuidesPropertiesTab::InitSection\n"));
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"GuidesPropertiesTab::InitSection called with no dialog pointer");

PORTNOTE("other", "Disabled colour dropdown")
#ifndef EXCLUDE_FROM_XARALX
	pColourDropDown = new ColourDropDown;
	if (pColourDropDown != NULL)
		pColourDropDown->Init(pPropertiesDlg->GetReadWriteWindowID(),_R(IDC_GUIDETAB_COLOURLIST));
#endif

	ShowDetails();

	return TRUE;
}


//--------------------------------------------------------
//--------------------------------------------------------

/********************************************************************************************

>	LayerColourAction::LayerColourAction()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

LayerColourAction::LayerColourAction()
{
}


/********************************************************************************************

>	ActionCode LayerColourAction::Init(UndoableOperation* pOp,
										ActionList* pActionList,
										OpChangeLayerColourParam EntryParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		pOp  		= ptr to the operation to which this action belongs
				pActionList = ptr to action list to which this action should be added
				Param		= Contains all the info needed to change the layer's colour
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	The action that does the layer colour change biz
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode LayerColourAction::Init(	UndoableOperation* pOp,
									ActionList* pActionList,
									OpChangeLayerColourParam EntryParam)
{
	UINT32 ActSize = sizeof(LayerColourAction);

	LayerColourAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(LayerColourAction),(Action**)&pNewAction);

	if (Ac != AC_FAIL && pNewAction != NULL)
	{
		OpChangeLayerColourParam& Param = ((LayerColourAction*)pNewAction)->Param;

		Document* pDoc 	 = EntryParam.pDoc;
		Layer*    pLayer = EntryParam.pLayer;

		Param.pDoc 	 = pDoc;
		Param.pLayer = pLayer;

		if (pDoc != NULL && pLayer != NULL)
		{
			DocColour* pDocColour = pLayer->GetGuideColour();
			Param.pColour = pDocColour->FindParentIndexedColour();

			pLayer->SetGuideColour(EntryParam.pColour);
			LayerSGallery::ForceRedrawLayer(pDoc,pLayer);
			BROADCAST_TO_ALL(LayerMsg(pLayer,LayerMsg::GUIDELINES_CHANGED));
		}

		ERROR3IF(pDoc == NULL,  "pDoc is NULL");			
		ERROR3IF(pLayer == NULL,"pLayer is NULL");			
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode LayerColourAction::Execute();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  
				Puts the action's layer's visible state to the opposite setting.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode LayerColourAction::Execute()
{
	ActionCode Ac = AC_FAIL;

	if (pOperation->IS_KIND_OF(UndoableOperation))
	{
		Ac = LayerColourAction::Init((UndoableOperation*)pOperation,pOppositeActLst,Param);

	}
	else
	{
		ERROR3("LayerColourAction::Execute() called with op that's not an undoable op");
	}

	return Ac;
}

LayerColourAction::~LayerColourAction()
{
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------


/********************************************************************************************

>	OpChangeLayerColour::OpChangeLayerColour()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Purpose:	Default constructor

********************************************************************************************/

OpChangeLayerColour::OpChangeLayerColour()
{
	UndoIDS = _R(IDS_OPCHANGEGUIDELAYERCOLOUR);
}

/********************************************************************************************

>	OpChangeLayerColour::~OpChangeLayerColour()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Purpose:	Default destructor

********************************************************************************************/

OpChangeLayerColour::~OpChangeLayerColour()
{
}

/********************************************************************************************

>	void OpChangeLayerColour::DoWithParam(OpDescriptor* pOpDesc,OpParam* pOpParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		pOpDesc  = ptr to op descriptor
				pOpParam = ptr to the params (should be a OpChangeLayerColourParam)
	Outputs:	-
	Return:		-
	Purpose:	Does the changing of the layer's colour
				pOpParam should point to a OpChangeLayerColourParam

********************************************************************************************/

void OpChangeLayerColour::DoWithParam(OpDescriptor* pOpDesc,OpParam* pOpParam)
{
	OpChangeLayerColourParam* pParam = (OpChangeLayerColourParam*)pOpParam;

	ActionCode Ac = LayerColourAction::Init(this,&UndoActions,*pParam);

	if (Ac == AC_FAIL)
		FailAndExecute();

	End();
}

/********************************************************************************************

>	static BOOL OpChangeLayerColour::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		-
	Outputs:	-
	Return:		TRUE if initialised properly, FALSE otherwise
	Purpose:	Initialises the operation by registering an op descriptor for it

********************************************************************************************/

BOOL OpChangeLayerColour::Init()
{
	return (RegisterOpDescriptor(
								0, 
								0,
								CC_RUNTIME_CLASS(OpChangeLayerColour), 
								OPTOKEN_CHANGELAYERCOLOUR,
								OpChangeLayerColour::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								)
			);

}

/********************************************************************************************

>	static OpState OpChangeLayerColour::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		Description = place to put reason why you are greyed out
	Outputs:	-
	Return:		The state of this op
	Purpose:	Func for finding out whether this op is useable or not

********************************************************************************************/

OpState OpChangeLayerColour::GetState(String_256* Description, OpDescriptor*)
{
	OpState State;
	return State;
}

/********************************************************************************************

>	void OpChangeLayerColour::GetOpName(String_256* OpName)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		OpName = place to put name of op in
	Outputs:	-
	Return:		-
	Purpose:	Gets the name of the op for the undo/redo menu item

********************************************************************************************/

void OpChangeLayerColour::GetOpName(String_256* OpName)
{
	*OpName = String_256(UndoIDS);
}

#endif // WEBSTER
