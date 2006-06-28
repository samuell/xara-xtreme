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
// The Graduated Fill Tool

// This is a random comment inserted just to change the size of the file
// so that Will might have more luck checking it out.

// includes
#include "camtypes.h"

#include "colourix.h"
#include "csrstack.h"
#include "osrndrgn.h"
//#include "viewrc.h"
#include "blobs.h"
//#include "will.h"
//#include "markn.h"
//#include "rikdlg.h"
#include "oilfiles.h"

#include "filltool.h"
#include "opgrad.h"
#include "toollist.h"
//#include "resource.h"

#include "bitmpinf.h"
//#include "wbitmap.h"
#include "ophist.h"
#include "keypress.h"
#include "opnudge.h"
#include "fillndge.h"

//#include "justin2.h"
//#include "will2.h"
#include "bmpcomp.h"	// BitmapListComponent
#include "bubbleid.h"

//#include "gerry.h"		// new fill type strings (only things in the file)
//#include "biasres.h"
#include "nodeshad.h"
//#include "grptrans.h"
#include "objchge.h"
#include "layer.h"
//#include "simon.h"
#include "opliveeffects.h"
#include "effects_stack.h"
#include "nodeliveeffect.h"

// Revision Number
DECLARE_SOURCE( "$Revision$" );

// The Names of the tools
TCHAR*		GradFillTool::FamilyName = _T("Fill Tools");
TCHAR* 		GradFillTool::ToolName = _T("Graduated Fill Tool");
TCHAR*		GradFillTool::Purpose = _T("Graduated Filling of Objects");
TCHAR*		GradFillTool::Author = _T("Will");

TCHAR*		TranspTool::FamilyName = _T("Fill Tools");
TCHAR* 		TranspTool::ToolName = _T("Transparency Tool");
TCHAR*		TranspTool::Purpose = _T("Alter the transparency of objects");
TCHAR*		TranspTool::Author = _T("Will");

INT32 GradInfoBarOp::CurrentGeometryIndex;
INT32 GradInfoBarOp::CurrentMappingIndex;
INT32 GradInfoBarOp::CurrentEffectIndex;

INT32 TranspInfoBarOp::CurrentGeometryIndex;
INT32 TranspInfoBarOp::CurrentTransTypeIndex;
INT32 TranspInfoBarOp::CurrentMappingIndex;
INT32 TranspInfoBarOp::CurrentTypeIndex;
INT32 TranspInfoBarOp::CurrentBitmapIndex;

GradInfoBarOp* GradFillTool::pGradInfoBarOp;
TranspInfoBarOp* GradFillTool::pTranspInfoBarOp;

UINT32 GradFillTool::LastStatusID;
BOOL GradFillTool::CurrentTool;

TranspInfoBarOp* TranspTool::pTranspInfoBarOp;

UINT32 TranspTool::LastStatusID;
BOOL TranspTool::CurrentTool;

BOOL GradFillTool::AllowFillNudges = TRUE;

BOOL GradFillTool::NudgeFills = FALSE;
BOOL TranspTool::NudgeFills = FALSE;

static INT32 TempGeometryIndex = -1;

CC_IMPLEMENT_MEMDUMP( GradFillTool, Tool_v1 )
CC_IMPLEMENT_MEMDUMP( TranspTool, Tool_v1 )
CC_IMPLEMENT_DYNCREATE( GradInfoBarOp,   InformationBarOp )
CC_IMPLEMENT_DYNCREATE( TranspInfoBarOp,   InformationBarOp )

CC_IMPLEMENT_DYNCREATE( AttrRemoveTransp,  AttrTranspChange )
CC_IMPLEMENT_DYNCREATE( AttrRemoveStrokeTransp,  AttrValueChange )
CC_IMPLEMENT_DYNCREATE( AttrMakeFlatTransp,  AttrTranspChange )

CC_IMPLEMENT_DYNCREATE( FillProfileOpParam, OpParam)
CC_IMPLEMENT_DYNCREATE( OpChangeFillProfile, SelOperation)
CC_IMPLEMENT_DYNCREATE( ChangeFillProfileAction, Action)
CC_IMPLEMENT_DYNCREATE( TranspFillProfileOpParam, OpParam)
CC_IMPLEMENT_DYNCREATE( OpChangeTranspFillProfile, SelOperation)
CC_IMPLEMENT_DYNCREATE( ChangeTranspFillProfileAction, Action)


#define	new	CAM_DEBUG_NEW

/********************************************************************************************

	Preference:	DoFillMeshCheck
	Section:	DebugFlags
	Range:		TRUE or FALSE
	Purpose:	Enables or disables the check for Similar Fill meshes overwriting each other.

********************************************************************************************/



/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//			Implementation of class FillTools

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************

>	BOOL FillTools::BuildListOfSelectedNodes(List *pList,
											 const CCRuntimeClass * pClass,
											 BOOL bPromoteToParents = TRUE)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/2000
	Inputs:		
	Purpose:	Builds a list of all given attribute nodes in the selection

********************************************************************************************/

/*BOOL FillTools::bScanForTransparency = FALSE;

BOOL FillTools::BuildListOfSelectedNodes(List *pList,
										 const CCRuntimeClass* pClass,
										 BOOL bPromoteToParents)
{
	if (!pList)
		return FALSE;

	if (pClass == (CC_RUNTIME_CLASS (AttrFillGeometry)))
	{
		bScanForTransparency = FALSE;
	}
	else if (pClass == (CC_RUNTIME_CLASS (AttrTranspFillGeometry)))
	{
		bScanForTransparency = TRUE;
	}
	else
	{
		ERROR3("Someone's forgotten to read my notes about this function!");
		return (FALSE);
	}
	
	// run through the selection adding all nodes to the list
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = bPromoteToParents;
	Sel.Range::SetRangeControl(rg);

	if (Sel.IsEmpty())
		return FALSE;

	Node * pNode = Sel.FindFirst(FALSE);

	NodeListItem * pItem = NULL;

	while (pNode)
	{
		GetAllNodesUnderNode(pNode, pList, pClass);

		pNode = Sel.FindNext(pNode, FALSE);
	}

	if (pList->IsEmpty())
		return FALSE;

	return TRUE;
}
*/



// It's important that all parts of Camelot find attributes in the same ways - otherwise
// the UI might be showing one set of things and the code changing a completely different
// set!
//
// This function uses the applied attr functions to determine which attributes are in
// scope in the selection instead of assuming that all attrs under the selected node
// are relevant...
//
/*BOOL FillTools::BuildListOfSelectedAttrs(List* pList,
										 const CCRuntimeClass* pClass,
										 BOOL bPromoteToParents)
{
	ERROR3IF(pList==NULL, "BuildListOfAttrs called with no list");
	if (!pList)
		return FALSE;

	// run through the selection adding all nodes to the list
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = bPromoteToParents;
	Sel.Range::SetRangeControl(rg);

	if (Sel.IsEmpty())
		return FALSE;

	Node* pNode = Sel.FindFirst();
	NodeListItem* pItem = NULL;

	while (pNode)
	{
		if (pNode->IsAnObject())
		{
			NodeAttribute* pAttr = ((NodeRenderableInk*)pNode)->FindAppliedAttribute((CCRuntimeClass*)pClass);
			if (pAttr && !pAttr->IsADefaultAttr())
			{
				pItem = new NodeListItem;
				if (pItem)
				{
					pItem->pNode = pAttr;
					pList->AddTail(pItem);
				}
				else
				{
					pList->DeleteAll();
					break;
				}
			}
		}

		pNode = Sel.FindNext(pNode);
	}

	if (pList->IsEmpty())
		return FALSE;

	return TRUE;
}
*/



/********************************************************************************************

>	BOOL FillTools::GetAllNodesUnderNode(const Node * pNode, List * pList,
										 const CCRuntimeClass * pClass, INT32 depth = 0)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/2000
	Inputs:		The node to recurse down from, and the list to add found attributes to
	Purpose:	Recurses down the tree building a list of all light attribute nodes

********************************************************************************************/

/*BOOL FillTools::GetAllNodesUnderNode(const Node * pNode, List * pList,
									 const CCRuntimeClass * pClass, INT32 depth)
{
	if (!pNode || !pList)
		return FALSE;

	const Node * pStartNode = pNode;

	// NOTHING exists under a locked (destructive) effect!
	if (pNode->IsBitmapEffect() && ((NodeBitmapEffect*)pNode)->IsLockedEffect())
		return TRUE;

	Node * pCurNode = pNode->FindFirstChild();

	Node * pChild = NULL;
	NodeListItem * pItem = NULL;

	if (!pCurNode)
	{
		// starting conditions
		if (pStartNode->IsKindOf(CC_RUNTIME_CLASS(AttrFillGeometry)) &&
			depth == 0 &&
			IsNodeTrueFill(pStartNode) &&
			!IsNodeInList(pList, pStartNode)
			)
		{
			pItem = new NodeListItem;

			if (pItem)
			{
				pItem->pNode = (Node *)pStartNode;
				pList->AddTail(pItem);

				return TRUE;
			}
		}

		return FALSE;
	}
	
	// step through the node's siblings
	while (pCurNode)
	{	
		// check sub-tree first
		GetAllNodesUnderNode(pCurNode, pList, pClass, depth + 1);
		
		// check this node is of the correct class, and it also has a parent
		if (pCurNode->IsKindOf(CC_RUNTIME_CLASS (AttrFillGeometry)) && IsNodeTrueFill (pCurNode) && pCurNode->FindParent() &&
			!pCurNode->IsNodeHidden() && !IsNodeInList(pList, pCurNode))
		{
			pItem = new NodeListItem;

			if (!pItem)
			{
				ERROR3("Can't create NodeListItem");
				return FALSE;
			}

			pItem->pNode = pCurNode;
			pList->AddTail(pItem);
		}

		// move onto next sibling
		pCurNode = pCurNode->FindNext();
	}

	// check original node
	if (pStartNode->IsKindOf(CC_RUNTIME_CLASS(AttrFillGeometry)) &&
		IsNodeTrueFill(pStartNode) &&
		pStartNode->FindParent() &&
		!pStartNode->IsNodeHidden() &&
		!IsNodeInList(pList, pStartNode)
		)
	{
		pItem = new NodeListItem;

		if (!pItem)
		{
			ERROR3("Can't create NodeListItem");
			return FALSE;
		}

		pItem->pNode = (Node *)pStartNode;
		pList->AddTail(pItem);
	}

	return TRUE;
}
*/


/********************************************************************************************

>	BOOL FillTools::IsNodeInList(List * pList, Node * pNode)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/2000
	Inputs:		A list of NodeListItems and the node to search for
	Purpose:	Returns TRUE if the node is in the list

********************************************************************************************/

BOOL FillTools::IsNodeInList(List * pList, const Node * pNode)
{
	NodeListItem * pItem = (NodeListItem * )pList->GetHead();

	while (pItem)
	{
		if (pItem->pNode == pNode)
			return TRUE;

		pItem = (NodeListItem * )pList->GetNext(pItem);
	}

	return FALSE;
}



/********************************************************************************************

>	BOOL FillTools::IsNodeTrueFill (const Node * pNode)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/2000
	Inputs:		A ptr to the current node
	Purpose:	Returns TRUE if the node is a gradient fill

********************************************************************************************/

/*BOOL FillTools::IsNodeTrueFill (const Node * pNode)
{
	if (bScanForTransparency == FALSE)
	{
		if ((((AttrFillGeometry*) pNode)->IsAGradFill ()) && (!(((AttrFillGeometry*) pNode)->IsATranspFill ())))
		{
			return (TRUE);
		}
		else
		{
			return (FALSE);
		}
	}
	else //(bScanForTransparency == TRUE)
	{
		if ((((AttrFillGeometry*) pNode)->IsAGradFill ()) && (((AttrFillGeometry*) pNode)->IsATranspFill ()))
		{
			return (TRUE);
		}
		else
		{
			return (FALSE);
		}
	}
}
*/

BOOL FillTools::GetSelectedAttrList(List* pList,
									const CCRuntimeClass* pClass,
									BOOL bGradFill,
									BOOL bTranspFill,
									EffectsStack* pUseThisStack)
{
	ERROR2IF(pList==NULL, FALSE, "GetSelectedAttrList passed NULL list pointer");
	ERROR2IF(pClass==NULL, FALSE, "GetSelectedAttrList passed NULL class pointer");

	EffectsStack* pStack = NULL;

	if (pUseThisStack != NULL)
		pStack = pUseThisStack;
	else
		pStack = GetApplication()->FindSelection()->GetEffectsStack();	// From cache

	ENSURE(pStack, "Can't find PostProStack");

	INT32 iStackPos = STACKPOS_TOP;
	Range* pLevelRange = pStack->GetLevelRange(&iStackPos, FALSE);		// Don't escape old controllers when scanning for attributes
	if (pLevelRange == NULL)
		return FALSE;

	Node* pNode = pLevelRange->FindFirst();
	while (pNode)
	{
		if (pNode->IsAnObject())
		{
			NodeAttribute* pAttr = ((NodeRenderableInk*)pNode)->FindAppliedAttribute((CCRuntimeClass*)pClass);
			if (pAttr && !pAttr->IsADefaultAttr() && pAttr->IsAGradFill()==bGradFill && pAttr->IsATranspFill()==bTranspFill)
			{
				NodeListItem * pItem = new NodeListItem;
				if (pItem)
				{
					pItem->pNode = pAttr;
					pList->AddTail(pItem);
				}
				else
				{
					pList->DeleteAll();
					return FALSE;
				}
			}
		}

		pNode = pLevelRange->FindNext(pNode);
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//			Implementation of class GradFillTool (graduated fill tool)

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************

>	GradFillTool::GradFillTool()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Purpose:	Constructs a Fill Tool.

********************************************************************************************/

GradFillTool::GradFillTool()
{
	ClickStart = DocCoord(0,0);
	IsSelection = FALSE;
	StartSpread = NULL;
	pGradFillCursor = NULL;
	CurrentCursorID = 0;

	NudgeFills = FALSE;
}

/********************************************************************************************

>	GradFillTool::~GradFillTool()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Purpose:	Destructor. Does nothing.

********************************************************************************************/

GradFillTool::~GradFillTool()
{
	// Dummy destructor
}

/********************************************************************************************

>	BOOL GradFillTool::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	GradFillTool::GradFillTool

********************************************************************************************/

BOOL GradFillTool::Init()
{
	BOOL ok = TRUE;

	ok = OpChangeFillProfile::Declare();

#if 0
	if (ok)
	{
		// Is everything ok so far ?
		CCResTextFile 		file;				// Resource File
		GradInfoBarOpCreate BarCreate;			// Object that creates GradInfoBarOp objects

			 	ok = file.open(_R(IDM_GRAD_BAR), _R(IDT_INFO_BAR_RES));		// Open resource
		if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);	// Read and create info bar
		if (ok) file.close();									 	// Close resource

		ENSURE(ok,"Unable to load gradbar.ini from resource\n"); 
		String_32 str = String_32(_R(IDS_FILLTOOL_FILLINFOBARNAME));
		DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);
	}
#endif

	if (ok)
	{
		// Info bar now exists.  Now get a pointer to it
		pGradInfoBarOp = new GradInfoBarOp(_R(IDD_GRADFILLDLG));

		if (pGradInfoBarOp)
		{
			// Set our default menu items
			pGradInfoBarOp->CurrentGeometryIndex = FGMENU_LINEAR;
			pGradInfoBarOp->CurrentMappingIndex = FMMENU_SIMPLE;
			pGradInfoBarOp->CurrentEffectIndex = FEMENU_FADE;
		} else
			ok=FALSE;

		ENSURE(ok,"Couldn't find grad tool info bar");
	}

	return (ok);
}

/********************************************************************************************

>	void GradFillTool::Describe(void *InfoPtr)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		InfoPtr - A pointer to a tool info block. It is passed cast to void* as
				the version of the tool is unknown at this point. Later versions of the
				Tool class may have more items in this block, that this tool will not use
	Outputs:	InfoPtr - The structure pointed to by InfoPtr will have had all the info
				that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void GradFillTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info -> InfoVersion = 1;
	
	Info -> InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info -> Version = 1;
	Info -> ID      = GetID();
	Info -> TextID   = _R(IDS_GRADFILL_TOOL);
	Info -> BubbleID = _R(IDBBL_FILL_TOOL);

	Info -> Family  = FamilyName;
	Info -> Name    = ToolName;
	Info -> Purpose = Purpose;
	Info -> Author  = Author;

	Info -> InfoBarDialog = 0;
}

// The EventHandlers

/********************************************************************************************

>	virtual void GradFillTool::SelectChange(BOOL isSelected)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/07/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates/destroys/pushes/pops the rectangle tool's cursor.
	Errors:		Debug warning if creating the cursor fails.
	SeeAlso:	-

********************************************************************************************/

void GradFillTool::SelectChange(BOOL isSelected)
{
	if (isSelected)
	{
		// This tool has just been selected.  Create an appropriate cursor, in this case
		// we can use the operating system-provided crosshair cursor.
		pGradFillCursor  = new Cursor(this, _R(IDC_GRADTOOLCURSOR));
		pGradPointCursor = new Cursor(this, _R(IDC_GRADPOINTCURSOR));

		// Did the cursor create ok ?
		if (!pGradFillCursor || !pGradFillCursor->IsValid())
		{
			return;
		}

		// Did the cursor create ok ?
		if (!pGradPointCursor || !pGradPointCursor->IsValid())
		{
			return;
		}

		pCurrentCursor = pGradFillCursor;
		CurrentCursorID = CursorStack::GPush(pGradFillCursor, FALSE);	// Push cursor, but don't display now

		// Create and display the tool's info bar
		CurrentTool = TRUE;
		AttrFillGeometry::SetTranspMeshesVisible(FALSE);

		// Which blobs do I want displayed
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			// Decide which blobs to display
			BlobStyle MyBlobs;
			MyBlobs.Tiny = TRUE;
			MyBlobs.Fill = TRUE;

			// tell the blob manager
			BlobMgr->ToolInterest(MyBlobs);
		}

		// Re-Count the number of selected fill control points
		AttrFillGeometry::SetSelectionCount(AttrFillGeometry::CountSelectionControlPoints());

      	pGradInfoBarOp->Create();
		pGradInfoBarOp->EnableControls ();

		if (AttrFillGeometry::SelectionCount > 0)
			EnableFillNudge();

		TempGeometryIndex = -1;
		//CurrentGeometryIndex = ;

		SelRange *Selected = GetApplication()->FindSelection();

		// There is a selection if there is a document, and it has some selected object(s)
		IsSelection = (Document::GetCurrent() != NULL && Selected->Count() > 0);

		if (IsSelection)
		{
			SelectionBox  = Selected->GetBoundingRect();

			Node* Current = Selected->FindFirst();
			if (Current != NULL)
			{
				Spread *pSpread = Current->FindParentSpread();
				RenderToolBlobs(pSpread, NULL);
			}
		}
	}
	else
	{
//		pGradInfoBarOp->ResetHiddenGadgetStates ();
		
		DisableFillNudge();

		// Deselection - destroy the tool's cursor, if there is one.
		if (pGradFillCursor)
		{
			CursorStack::GPop(CurrentCursorID);
			delete pGradFillCursor;
			delete pGradPointCursor;

			pCurrentCursor = NULL;
			CurrentCursorID = 0;
		}

		pGradInfoBarOp->CloseProfileDialog (pGradInfoBarOp->m_BiasGainGadget);

     	pGradInfoBarOp->Delete();

		CurrentTool = FALSE;

		// If needed, draw the [tool blobs? OK, who forgot to finish the comment?!]
		if (IsSelection && Document::GetCurrent()!=NULL)
		{
			SelRange *Selected = GetApplication()->FindSelection();
			Node *Current = Selected->FindFirst();

			if (Current!=NULL)
			{
				Spread *pSpread = Current->FindParentSpread();
				RenderToolBlobs(pSpread, NULL);
			}
		}

		// ensure any tool object blobs are removed.
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			BlobStyle bsRemoves;
			bsRemoves.ToolObject = TRUE;
			BlobMgr->RemoveInterest(bsRemoves);
		}
	}
}

/********************************************************************************************

>	void GradFillTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )


	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The Coords (in DocCoords) of the point where the mouse button
				was clicked
				Click -	Describes the type of click that was detected. 
				ClickMods -	Indicates which buttons caused the click and which modifers were
				pressed at the same time
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Click event for the Selector Tool. It starts up a Selector 
				Operation.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void GradFillTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

	// See if there is already a drag going on
	if (Operation::GetCurrentDragOp()!=NULL)
		return;

	if ( AttrFillGeometry::CheckAttrClick(PointerPos, Click, ClickMods, pSpread) )
	{
		// nastiness ALERT !!!!!

		// since we can now double click to insert a multi-stage fill blob, we also MUST
		// select that blob once it is created.  ONLY problem is that we cannot do this
		// (since it dosen't exist) UNTIL we have redrawn the screen ....
		
		if ( Click == CLICKTYPE_DOUBLE )
		{
			SelRange *Selected = GetApplication()->FindSelection();
			Node *Current = Selected->FindFirst();

			if (Current!=NULL)
			{
				Spread *pSpread2 = Current->FindParentSpread();
				RenderToolBlobs(pSpread2, NULL);
				
				// the only sensible way to select the new blob is to fake a mouse click on it ....
				
				AttrFillGeometry::CheckAttrClick(PointerPos, CLICKTYPE_SINGLE, ClickMods, pSpread2);

				// now fake an onmousemove to get our cursor/status line to change as well!
				
				OnMouseMove (PointerPos, pSpread, ClickMods);
			}
		}
		
		return;		// An Attribute claimed the click
	}

	// Make sure this click is one that we want
	if ( Click == CLICKTYPE_SINGLE )
	{
		StartPos = PointerPos;
		StartSpread = pSpread;
		DoubleClicked = FALSE;
	}

	if ( Click == CLICKTYPE_UP )
	{
		StartPos = DocCoord(0,0);
		StartSpread = NULL;
		DoubleClicked = FALSE;
	}

	if ( Click == CLICKTYPE_DOUBLE )
	{
		DoubleClicked = TRUE;		
	}

	if ( Click == CLICKTYPE_DRAG )
	{
		if (StartSpread == NULL)
			return;

		// Just what we wanted - Someone is dragging the mouse about
		// We need to make an operation to perform the drag with
//		OpCreateFill* pOp = new OpCreateFill;
		OpEditFill* pOp = new OpEditFill;
		if (pOp == NULL)
		{
			// Inform the person doing the clicking that life is not looking so good
			InformError();
		}
		else
		{
			AttrFillGeometry* Fill = NULL;
			// Start the drag operation and pass in the Anchor Point to the push operation

			if (ClickMods.Adjust)
			{
				Fill = new AttrRadialColourFill;
				if (Fill != NULL)
					((AttrRadialFill*)Fill)->MakeCircular();
			}
#ifndef WEBSTER
            else if (DoubleClicked)
			{
				Fill = new AttrConicalColourFill;
			}
#endif //WEBSTER
			else
			{
				switch (pGradInfoBarOp->CurrentGeometryIndex)
				{
				 	case (FGMENU_FLAT):
						// DMc changed - for the multiple colour fills
						Fill = new AttrLinearColourFill;
						break;

				 	case (FGMENU_LINEAR):
						Fill = new AttrLinearColourFill;
						break;

				 	case (FGMENU_CIRCULAR):
						Fill = new AttrRadialColourFill;
						if (Fill != NULL)
							((AttrRadialFill*)Fill)->MakeCircular();
						break;

				 	case (FGMENU_RADIAL):
						Fill = new AttrRadialColourFill;
						break;

				 	case (FGMENU_CONICAL):
						Fill = new AttrConicalColourFill;
						break;

				 	case (FGMENU_SQUARE):
						Fill = new AttrSquareColourFill;
						break;

				 	case (FGMENU_THREECOL):
						Fill = new AttrThreeColColourFill;
						break;

				 	case (FGMENU_FOURCOL):
						Fill = new AttrFourColColourFill;
						break;

				 	case (FGMENU_BITMAP):
						Fill = new AttrBitmapColourFill;

	   					Fill->AttachBitmap(NULL);
						Fill->SetTesselation(pGradInfoBarOp->CurrentMappingIndex+1);
						break;

				 	case (FGMENU_FRACTAL):
						Fill = new AttrFractalColourFill;
						Fill->SetTesselation(pGradInfoBarOp->CurrentMappingIndex+1);
						break;

				 	case (FGMENU_NOISE):
						Fill = new AttrNoiseColourFill;
						Fill->SetTesselation(pGradInfoBarOp->CurrentMappingIndex+1);
						break;

					default:
						// Er .. Dunno what kind of fill this is ?
						Fill = new AttrLinearColourFill;
						break;
				}
			}
	
			if (Fill == NULL)
			{
				InformError();
				return;
			}

			pOp->DoCreate(StartPos, StartSpread, Fill);
		}

		StartPos = DocCoord(0,0);
		StartSpread = NULL;
		DoubleClicked = FALSE;
		return;
	}

	// call the base class ....

	DragTool::OnClick (PointerPos, Click, ClickMods, pSpread);
}

/********************************************************************************************

>	void GradFillTool::OnMouseMove(DocCoord Pos, Spread* pSpread, ClickModifiers ClickMods)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/07/94
	Inputs:		Pos - The current position of the mouse.
				pSpread - The current spread.
	Purpose:	Called whenever the mouse position changes.
				We use this to update the cursor and status messages.

********************************************************************************************/

void GradFillTool::OnMouseMove(DocCoord Pos, Spread* pSpread, ClickModifiers ClickMods)
{
	// Setup defaults
	Cursor* pCursor = pGradFillCursor;
	String_256 Status;

	if (ClickMods.Adjust)
		TempGeometryIndex = FGMENU_CIRCULAR;
	else
		TempGeometryIndex = -1;

	GetCursorAndStatus(Pos, pSpread, &pCursor, &Status);

	if (pCursor != pCurrentCursor)
	{
		// We're using the wrong shape!! 
		pCurrentCursor = pCursor;
		CursorStack::GSetTop(pCurrentCursor, CurrentCursorID);
	}
		
	// Display the status text
	GetApplication()->UpdateStatusBarText(&Status);
}

/********************************************************************************************

> 	BOOL GradFillTool::OnKeyPress(KeyPress* pKeyPress)

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	29/3/94
	Inputs:		pKeyPress = Ptr to a platform-indy key press object
    Outputs:    -
    Returns:   	TRUE  - the key press was processed, so do NOT pass on
				FALSE - not processed, so let others have a bash
    Purpose:    Checks for fill tool keys.
	Errors:		-
                   			                                     
********************************************************************************************/

BOOL GradFillTool::OnKeyPress(KeyPress* pKeyPress)
{
	if (pKeyPress->GetVirtKey() == CAMKEY(TAB) &&
		!pKeyPress->IsRelease() &&
		!pKeyPress->IsRepeat())
	{
		// Toggle the selection state of all visible fill control points
		ToggleControlPoints(pKeyPress->IsAdjust());

		return TRUE;
	}

	if ( IsFillNudgeEnabled() &&
		(pKeyPress->GetVirtKey() == CAMKEY(UP) ||
		 pKeyPress->GetVirtKey() == CAMKEY(DOWN) ||
		 pKeyPress->GetVirtKey() == CAMKEY(LEFT) ||
		 pKeyPress->GetVirtKey() == CAMKEY(RIGHT)) )
	{
		// If we are nudging, then stop the Attribute manager from sending
		// tons of messages, until the nudge stops.
		if (pKeyPress->IsRelease())
			AttributeManager::SendMessages = TRUE;
		else
			AttributeManager::SendMessages = FALSE;

		// Pass on the nudge keys
		return FALSE;
	}
	return FALSE;
}

/********************************************************************************************

> 	void GradFillTool::ToggleControlPoints(BOOL Reverse)

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	29/3/94
	Inputs:		Reverse - if true the points will cycle in the opposite direction
	Purpose:    Toggles the selection state of all visible fill control blobs.
                   			                                     
********************************************************************************************/

void GradFillTool::ToggleControlPoints(BOOL Reverse)
{
	// Switch control points
	AttrFillGeometry::LastRenderedStartBlob = DocCoord(0,0);
	AttrFillGeometry::LastRenderedEndBlob = DocCoord(0,0);
	AttrFillGeometry::LastRenderedEnd2Blob = DocCoord(0,0);
	AttrFillGeometry::LastRenderedEnd3Blob = DocCoord(0,0);

	AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr();

	// Return if there aren't any
	if (pAttrNode == NULL)
		return;

	// We only toggle if there are some points selected
	BOOL DoToggle = AttrFillGeometry::FillSelectionCount() > 0;

	while (pAttrNode != NULL)
	{
		if (pAttrNode->IsAColourFill())
		{
			if (DoToggle)
			{
				// We're going to toggle the selected control points.
				if (pAttrNode->GetSelectionCount() > 0)
				{
					pAttrNode->CycleSelection(Reverse);
				}
			}
			else
			{
				// There were no points selected, so we'll just
				// select all the start points.
				pAttrNode->SelectBlob(FILLCONTROL_STARTPOINT);
			}
		}

		// Check the next fill
		pAttrNode = AttrFillGeometry::FindNextSelectedAttr();
	}

	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED)); 
}

/********************************************************************************************

> 	void GradFillTool::EnableFillNudge()

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	29/3/94
    Purpose:    Enables the fill control point nudging.
	Errors:		-
                   			                                     
********************************************************************************************/

void GradFillTool::EnableFillNudge()
{
	if (!GradFillTool::AllowFillNudges || NudgeFills)
		return;
	
	//Alias all the nudge ops to the fill specific ones.
	OpFillNudge::NudgeColours(TRUE);
	
	OpDescriptor* pOpDesc;

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP1) ;
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeUp1),NULL,0);
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP5);  
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeUp5),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP10);  
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeUp10),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPFIFTH);
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeUpFifth),NULL,0);
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPPIXEL1);
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeUpPixel1),NULL,0);
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPPIXEL10);
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeUpPixel10),NULL,0);

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN1);  
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeDown1),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN5);  
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeDown5),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN10);  
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeDown10),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNFIFTH);
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeDownFifth),NULL,0);
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNPIXEL1);
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeDownPixel1),NULL,0);
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNPIXEL10);
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeDownPixel10),NULL,0);

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT1);  
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeLeft1),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT5);  
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeLeft5),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT10);  
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeLeft10),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTFIFTH);
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeLeftFifth),NULL,0);
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTPIXEL1);
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeLeftPixel1),NULL,0);
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTPIXEL10);
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeLeftPixel10),NULL,0);

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT1);  
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeRight1),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT5);  
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeRight5),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT10);  
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeRight10),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTFIFTH);
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeRightFifth),NULL,0);
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTPIXEL1);
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeRightPixel1),NULL,0);
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTPIXEL10);
	if (pOpDesc) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeRightPixel10),NULL,0);
	
	NudgeFills = TRUE;
}

/********************************************************************************************

> 	void GradFillTool::DisableFillNudge()

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	29/3/94
    Purpose:    Disables the fill control point nudging.
	Errors:		-
                   			                                     
********************************************************************************************/

void GradFillTool::DisableFillNudge()
{
	if (!GradFillTool::AllowFillNudges || !NudgeFills)
		return;

	NudgeFills = FALSE;

	//Un-alias all the nudge ops back like they were.
	
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP1);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP5);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP10);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPFIFTH);
	pOpDesc->RemoveAlias();
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPPIXEL1);
	pOpDesc->RemoveAlias();
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPPIXEL10);
	pOpDesc->RemoveAlias();

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN1);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN5);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN10);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNFIFTH);
	pOpDesc->RemoveAlias();
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNPIXEL1);
	pOpDesc->RemoveAlias();
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNPIXEL10);
	pOpDesc->RemoveAlias();

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT1);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT5);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT10);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTFIFTH);
	pOpDesc->RemoveAlias();
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTPIXEL1);
	pOpDesc->RemoveAlias();
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTPIXEL10);
	pOpDesc->RemoveAlias();


	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT1);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT5);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT10);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTFIFTH);
	pOpDesc->RemoveAlias();
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTPIXEL1);
	pOpDesc->RemoveAlias();
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTPIXEL10);
	pOpDesc->RemoveAlias();
}

/********************************************************************************************

> 	BOOL GradFillTool::IsFillNudgeEnabled()

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	29/3/94
    Returns:   	TRUE  - Fill nudging is disabled
				FALSE - Fill nudging is enabled
    Purpose:    Checks for to see if the fill nudging is currently enabled or not.
	Errors:		-
                   			                                     
********************************************************************************************/

BOOL GradFillTool::IsFillNudgeEnabled()
{
	return NudgeFills;
}

/*****************************************************************************

>	virtual BOOL GradFillTool::GetStatusLineText(String_256* ptext, Spread* pSpread,
												 DocCoord DocPos, ClickModifiers ClickMods);

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Inputs:		pSpread   - pointer to spread under mouse (else NULL)
				DocPos    - position of mouse in doc (in spread coords)
				ClickMods - mouse click modifiers
	Outputs:	ptext - text for status line
	Returns:	TRUE if outputting valid text
	Purpose:	generate up-to-date text for the status line (called on idles)

*****************************************************************************/

BOOL GradFillTool::GetStatusLineText(String_256* ptext, Spread* pSpread, 
									 DocCoord DocPos, ClickModifiers ClickMods)
{
	ERROR3IF(ptext==NULL,"GradFillTool::GetStatusLineText() - ptext passed as null");

	if (ClickMods.Adjust)
		TempGeometryIndex = FGMENU_CIRCULAR;
	else
		TempGeometryIndex = -1;

	GetCursorAndStatus(DocPos, pSpread, NULL, ptext);

	return TRUE;
}

/********************************************************************************************

>	void GradFillTool::GetCursorAndStatus(DocCoord Pos, Spread* pSpread, 
										  Cursor** pCursor, String_256* pStatus)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Inputs:		Pos - The current position of the mouse.
	Purpose:	Get the correct cursor and status message for this mouse position.

********************************************************************************************/

void GradFillTool::GetCursorAndStatus(DocCoord Pos, Spread* pSpread, 
									  Cursor** pCursor, String_256* pStatus)
{
	// Setup defaults
	UINT32 Status;
	Cursor* pNewCursor = pGradFillCursor;

	AttrFillGeometry::DoCheckOnFillRampMesh = FALSE;		// disable this feature

	// Is the mouse over any of our fill control points ?
	if ( AttrFillGeometry::CheckForFillControlHit(Pos, &Status) )
	{
		// Status will have been updated

		// Change the cursor, to indicate the mouse is over a control point
		pNewCursor = pGradPointCursor;
	}
	else
	{
		INT32 Index = pGradInfoBarOp->CurrentGeometryIndex;

		if (TempGeometryIndex != -1)
			Index = TempGeometryIndex;

		// Check our menu status
		switch (Index)
		{      
			// If fill controls are selected then we indicate 
			// that a click will deselect them.
			// Otherwise we just show what kind of fill will
			// be created when the user drags.

		 	case (FGMENU_FLAT):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_FS_CREATELINEAR_S);		
				else                                    
                	Status = _R(IDS_FS_CREATELINEAR);
                break;

		 	case (FGMENU_LINEAR):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_FS_CREATELINEAR_S);		
				else                                    
                	Status = _R(IDS_FS_CREATELINEAR);
                break;

		 	case (FGMENU_CIRCULAR):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_FS_CREATECIRCLE_S);
				else
                	Status = _R(IDS_FS_CREATECIRCLE);
				break;

		 	case (FGMENU_RADIAL):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_FS_CREATEELLIP_S);
				else
                	Status = _R(IDS_FS_CREATEELLIP);
				break;

		 	case (FGMENU_CONICAL):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_FS_CREATECONICAL_S);
				else
                	Status = _R(IDS_FS_CREATECONICAL);
				break;

		 	case (FGMENU_SQUARE):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_FS_CREATESQUARE_S);
				else
                	Status = _R(IDS_FS_CREATESQUARE);
				break;

		 	case (FGMENU_THREECOL):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_FS_CREATETHREECOL_S);
				else
                	Status = _R(IDS_FS_CREATETHREECOL);
				break;

		 	case (FGMENU_FOURCOL):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_FS_CREATEFOURCOL_S);
				else
                	Status = _R(IDS_FS_CREATEFOURCOL);
				break;

		 	case (FGMENU_BITMAP):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_FS_CREATEBITMAP_S);
				else
                	Status = _R(IDS_FS_CREATEBITMAP);
				break;

		 	case (FGMENU_FRACTAL):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_FS_CREATEFRACTAL_S);
				else
                	Status = _R(IDS_FS_CREATEFRACTAL);
				break;

		 	case (FGMENU_NOISE):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_FS_CREATENOISE_S);
				else
                	Status = _R(IDS_FS_CREATENOISE);
				break;

			default:
				// Er .. Dunno what kind of fill this is ?
				Status = _R(IDS_FS_CREATELINEAR);
				break;
		}
	}

	if (pStatus != NULL)
	{
		pStatus->Load(Status);
		LastStatusID = Status;
	}

	if (pCursor != NULL)
		*pCursor = pNewCursor;

	AttrFillGeometry::DoCheckOnFillRampMesh = TRUE;			// enable again
}

/********************************************************************************************

>	static void GradTool::DisplayStatusBarHelp(UINT32 StatusID)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		StatusID = ID of status help string
	Outputs:	-
	Returns:	-
	Purpose:	Displays the given status help string in the status bar
	SeeAlso:	-

********************************************************************************************/

void GradFillTool::DisplayStatusBarHelp(UINT32 StatusID)
{
	String_256 StatusMsg("");
	StatusMsg.Load(StatusID);
	GetApplication()->UpdateStatusBarText(&StatusMsg);
	LastStatusID = StatusID;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//			Implementation of class TranspTool (Transparency Tool)

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************

>	TranspTool::TranspTool()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Constructs a Transparency Tool.

********************************************************************************************/

TranspTool::TranspTool()
{
	ClickStart = DocCoord(0,0);
	IsSelection = FALSE;
	StartSpread = NULL;
	pTranspCursor = NULL;
	CurrentCursorID = 0;

	CurrentTool = FALSE;
	NudgeFills = FALSE;
}

/********************************************************************************************

>	TranspTool::~TranspTool()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Destructor. Does nothing.

********************************************************************************************/

TranspTool::~TranspTool()
{
	// Dummy destructor
}

/********************************************************************************************

>	BOOL TranspTool::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	TranspTool::TranspTool

********************************************************************************************/

BOOL TranspTool::Init()
{
	BOOL ok = TRUE;

	ok = OpChangeTranspFillProfile::Declare();

#if 0
	if (ok)
	{
		CCResTextFile 		file;				// Resource File
		TranspInfoBarOpCreate BarCreate;		// Object that creates TranspInfoBarOp objects

			 	ok = file.open(_R(IDM_TRANSP_BAR), _R(IDT_INFO_BAR_RES));		// Open resource
		if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);	// Read and create info bar
		if (ok) file.close();									 	// Close resource

		ENSURE(ok,"Unable to load transbar.ini from resource\n"); 

		// Info bar now exists.  Now get a pointer to it
		String_32 str = String_32(_R(IDS_FILLTOOL_TRANINFOBARNAME));
		DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);

	}
#endif
	if (ok)
	{
		pTranspInfoBarOp = new TranspInfoBarOp(_R(IDD_TRANFILLDLG));

		if (pTranspInfoBarOp)
		{
			// Set our default menu items
			pTranspInfoBarOp->CurrentGeometryIndex = FGMENU_LINEARTRANSP;
			pTranspInfoBarOp->CurrentMappingIndex = FMMENU_SIMPLE;
			pTranspInfoBarOp->CurrentTypeIndex = FTMENU_REFLECT;
		} else
			ok=FALSE;

		ENSURE(ok,"Unable to load transbar.ini from resource\n"); 

	}

	return (ok);
}

/********************************************************************************************

>	void TranspTool::Describe(void *InfoPtr)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		InfoPtr - A pointer to a tool info block. It is passed cast to void* as
				the version of the tool is unknown at this point. Later versions of the
				Tool class may have more items in this block, that this tool will not use
	Outputs:	InfoPtr - The structure pointed to by InfoPtr will have had all the info
				that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void TranspTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info -> InfoVersion = 1;
	
	Info -> InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info -> Version = 1;
	Info -> ID      = GetID();
	Info -> TextID   = _R(IDS_TRANSP_TOOL);
	Info -> BubbleID = _R(IDBBL_TRANSP_TOOL);

	Info -> Family  = FamilyName;
	Info -> Name    = ToolName;
	Info -> Purpose = Purpose;
	Info -> Author  = Author;

	Info -> InfoBarDialog = 0;
}

// The EventHandlers

/********************************************************************************************

>	virtual void TranspTool::SelectChange(BOOL isSelected)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/07/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates/destroys/pushes/pops the rectangle tool's cursor.
	Errors:		Debug warning if creating the cursor fails.
	SeeAlso:	-

********************************************************************************************/

void TranspTool::SelectChange(BOOL isSelected)
{
	if (isSelected)
	{
		// This tool has just been selected.  Create an appropriate cursor, in this case
		// we can use the operating system-provided crosshair cursor.
		pTranspCursor  = new Cursor(this, _R(IDC_TRANSPTOOLCURSOR));
		pTranspPointCursor = new Cursor(this, _R(IDC_TRANSPPOINTCURSOR));

		// Did the cursor create ok ?
		if (!pTranspCursor || !pTranspCursor->IsValid())
		{
			return;
		}

		// Did the cursor create ok ?
		if (!pTranspPointCursor || !pTranspPointCursor->IsValid())
		{
			return;
		}

		pCurrentCursor = pTranspCursor;
		CurrentCursorID = CursorStack::GPush(pTranspCursor, FALSE);	// Push cursor, but don't display now

		// Create and display the tool's info bar
		CurrentTool = FALSE;
		AttrFillGeometry::SetTranspMeshesVisible(FALSE);

		BlobManager* BlobMgr = GetApplication()->GetBlobManager();

		BlobStyle MyBlobs = BlobMgr->GetCurrentInterest();
		if (MyBlobs.Fill)
		{
			MyBlobs.Fill = FALSE;
			MyBlobs.Effect = FALSE;
			BlobMgr->ToolInterest(MyBlobs);
		}

		CurrentTool = TRUE;
 		AttrFillGeometry::SetTranspMeshesVisible(TRUE);

		SelRange *Selected = GetApplication()->FindSelection();

		BlobStyle ToolBlobs;
		ToolBlobs.Fill = TRUE;
		ToolBlobs.Tiny = TRUE;
		ToolBlobs.Effect = TRUE;
		BlobMgr->ToolInterest(ToolBlobs);

		// Re-Count the number of selected fill control points
		AttrFillGeometry::SetSelectionCount(AttrFillGeometry::CountSelectionControlPoints());

       	pTranspInfoBarOp->Create();

		if (AttrFillGeometry::SelectionCount > 0)
			EnableFillNudge();

		TempGeometryIndex = -1;

		// There is a selection if there is a document, and it has some selected object(s)
		IsSelection = (Document::GetCurrent() != NULL && Selected->Count() > 0);

		if (IsSelection)
		{
			SelectionBox  = Selected->GetBoundingRect();

			Node* Current = Selected->FindFirst();
			if (Current != NULL)
			{
				Spread *pSpread = Current->FindParentSpread();
				RenderToolBlobs(pSpread, NULL);
			}
		}
	}
	else
	{
		DisableFillNudge();

		// Deselection - destroy the tool's cursor, if there is one.
		if (pTranspCursor)
		{
			CursorStack::GPop(CurrentCursorID);
			delete pTranspCursor;
			delete pTranspPointCursor;

			pCurrentCursor = NULL;
			CurrentCursorID = 0;
		}

       	pTranspInfoBarOp->CloseProfileDialog(pTranspInfoBarOp->m_BiasGainGadget);
		
		pTranspInfoBarOp->Delete();

		CurrentTool = TRUE;
		AttrFillGeometry::SetTranspMeshesVisible(TRUE);

		BlobManager* BlobMgr = GetApplication()->GetBlobManager();

		BlobStyle MyBlobs = BlobMgr->GetCurrentInterest();
		MyBlobs.Fill = FALSE;
		MyBlobs.Effect = FALSE;
		BlobMgr->ToolInterest(MyBlobs);

		CurrentTool = FALSE;
		AttrFillGeometry::SetTranspMeshesVisible(FALSE);

		// If needed, draw the [tool blobs? OK, who forgot to finish the comment?!]
		if (IsSelection && Document::GetCurrent()!=NULL)
		{
			SelRange *Selected = GetApplication()->FindSelection();
			Node *Current = Selected->FindFirst();

			if (Current!=NULL)
			{
				Spread *pSpread = Current->FindParentSpread();
				RenderToolBlobs(pSpread, NULL);
			}
		}

		// ensure any tool object blobs are removed.
		BlobStyle bsRemoves;
		bsRemoves.ToolObject = TRUE;
		BlobMgr->RemoveInterest(bsRemoves);
	}
}

/********************************************************************************************

>	void TranspTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )


	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The Coords (in DocCoords) of the point where the mouse button
				was clicked
				Click -	Describes the type of click that was detected. 
				ClickMods -	Indicates which buttons caused the click and which modifers were
				pressed at the same time
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Click event for the Selector Tool. It starts up a Selector 
				Operation.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers
	Note:		WEBSTER-Martin-22/11/96 disallow editting and creation of transparency types
				we don't want Webster to have.

********************************************************************************************/

void TranspTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

	// See if there is already a drag going on
	if (Operation::GetCurrentDragOp()!=NULL)
		return;

	if ( AttrFillGeometry::CheckAttrClick(PointerPos, Click, ClickMods, pSpread) )
	{	
		return;		// An Attribute claimed the click
	}

	// Make sure this click is one that we want
	if ( Click == CLICKTYPE_SINGLE )
	{
		StartPos = PointerPos;
		StartSpread = pSpread;
		DoubleClicked = FALSE;
	}

	// Make sure this click is one that we want
	if ( Click == CLICKTYPE_UP )
	{
		StartPos = DocCoord(0,0);
		StartSpread = NULL;
		DoubleClicked = FALSE;
	}

	if ( Click == CLICKTYPE_DOUBLE )
	{
		DoubleClicked = TRUE;		
	}

	if ( Click == CLICKTYPE_DRAG )
	{
		// is there a shadow node in the selection ? if so, don't start a drag
		// DMc - check for nodes which transparency can't be applied to
		BOOL allowed = TRUE;

		Range Sel(*(GetApplication()->FindSelection()));

		INT32 Count = Sel.Count();
		INT32 ShadCount = 0;

		Node * pNode = Sel.FindFirst(FALSE);
		
		while (pNode)
		{
			if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeShadow)))
			{
				allowed = FALSE;
				((NodeShadow*)pNode)->DeSelect(TRUE);
				ShadCount ++;
			}

			pNode = Sel.FindNext(pNode, FALSE);
		}

		if (!allowed && Count == ShadCount)
		{
			return;
		}

		if (!allowed)
		{
			GetApplication()->UpdateSelection();
		}

		if (StartSpread == NULL)
			return;

		// Just what we wanted - Someone is dragging the mouse about
		// We need to make an operation to perform the drag with
//		OpCreateFill* pOp = new OpCreateFill;
		OpEditFill* pOp = new OpEditFill;
		if (pOp == NULL)
		{
			// Inform the person doing the clicking that life is not looking so good
			InformError();
		}
		else
		{
			AttrFillGeometry* Fill = NULL;
			// Start the drag operation and pass in the Anchor Point to the push operation

			if (ClickMods.Adjust)
			{
				Fill = new AttrRadialTranspFill;
				if (Fill != NULL)
					((AttrRadialFill*)Fill)->MakeCircular();
			}
#ifndef WEBSTER
			else if (DoubleClicked)
			{
				//this could never be executed anyway even in Camelot - MAB 20/03/97
				Fill = new AttrConicalTranspFill;
			}
#endif //WEBSTER
			else
			{
				switch (pTranspInfoBarOp->CurrentGeometryIndex)
				{
				 	case (FGMENU_NOTRANSP):
						Fill = new AttrLinearTranspFill;
						break;

				 	case (FGMENU_FLATTRANSP):
						Fill = new AttrLinearTranspFill;
						break;

				 	case (FGMENU_LINEARTRANSP):
						Fill = new AttrLinearTranspFill;
						break;

				 	case (FGMENU_CIRCULARTRANSP):
						Fill = new AttrRadialTranspFill;
						if (Fill != NULL)
							((AttrRadialFill*)Fill)->MakeCircular();
						break;

					case (FGMENU_RADIALTRANSP):
						Fill = new AttrRadialTranspFill;
						break;

				 	case (FGMENU_CONICALTRANSP):
						Fill = new AttrConicalTranspFill;
						break;

				 	case (FGMENU_SQUARETRANSP):
						Fill = new AttrSquareTranspFill;
						break;

				 	case (FGMENU_THREECOLTRANSP):
						Fill = new AttrThreeColTranspFill;
						break;

				 	case (FGMENU_FOURCOLTRANSP):
						Fill = new AttrFourColTranspFill;
						break;

				 	case (FGMENU_BITMAPTRANSP):
					{
						Fill = new AttrBitmapTranspFill;

						KernelBitmap* Default = NULL;
						Default = KernelBitmap::MakeKernelBitmap();
						if (Default == NULL)
						{
							InformError();
							delete Fill;
							return;
						}

						Fill->AttachBitmap(Default);
						Fill->SetTesselation(pTranspInfoBarOp->CurrentMappingIndex+1);
						break;
					}

				 	case (FGMENU_FRACTALTRANSP):
						Fill = new AttrFractalTranspFill;
						Fill->SetTesselation(pTranspInfoBarOp->CurrentMappingIndex+1);
						break;

				 	case (FGMENU_NOISETRANSP):
						Fill = new AttrNoiseTranspFill;
						Fill->SetTesselation(pTranspInfoBarOp->CurrentMappingIndex+1);
						break;

					default:
						// Er .. Dunno what kind of fill this is ?
						Fill = new AttrLinearTranspFill;
						break;
				}
			}
			
			if (Fill == NULL)
			{
				InformError();
				delete pOp;
				return;
			}

			Fill->SetTranspType(pTranspInfoBarOp->GetTranspType());

//			pOp->DoDrag(Fill, StartSpread, StartPos);
			pOp->DoCreate(StartPos, StartSpread, Fill);
		}

		StartPos = DocCoord(0,0);
		StartSpread = NULL;
		DoubleClicked = FALSE;
		return;
	}

	// call the base class ....

	DragTool::OnClick (PointerPos, Click, ClickMods, pSpread);
}

/********************************************************************************************

>	void TranspTool::OnMouseMove(DocCoord Pos, Spread* pSpread, ClickModifiers ClickMods)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/07/94
	Inputs:		Pos - The current position of the mouse.
				pSpread - The current spread.
	Purpose:	Called whenever the mouse position changes.
				We use this to update the cursor and status messages.

********************************************************************************************/

void TranspTool::OnMouseMove(DocCoord Pos, Spread* pSpread, ClickModifiers ClickMods)
{
	// Setup defaults
	Cursor* pCursor = pTranspCursor;
	String_256 Status;

	if (ClickMods.Adjust)
		TempGeometryIndex = FGMENU_CIRCULARTRANSP;
	else
		TempGeometryIndex = -1;

	GetCursorAndStatus(Pos, pSpread, &pCursor, &Status);

	if (pCursor != pCurrentCursor)
	{
		// We're using the wrong shape!! 
		pCurrentCursor = pCursor;
		CursorStack::GSetTop(pCurrentCursor, CurrentCursorID);
	}
		
	// Display the status text
	GetApplication()->UpdateStatusBarText(&Status);
}

/********************************************************************************************

> 	BOOL TranspTool::OnKeyPress(KeyPress* pKeyPress)

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	29/3/94
	Inputs:		pKeyPress = Ptr to a platform-indy key press object
    Outputs:    -
    Returns:   	TRUE  - the key press was processed, so do NOT pass on
				FALSE - not processed, so let others have a bash
    Purpose:    Checks for fill tool keys.
	Errors:		-
                   			                                     
********************************************************************************************/

BOOL TranspTool::OnKeyPress(KeyPress* pKeyPress)
{
	if (pKeyPress->GetVirtKey() == CAMKEY(TAB) &&
		!pKeyPress->IsRelease() &&
		!pKeyPress->IsRepeat())
	{
		// Toggle the selection state of all visible fill control points
		ToggleControlPoints(pKeyPress->IsAdjust());

		return TRUE;
	}

	if ( IsFillNudgeEnabled() &&
		(pKeyPress->GetVirtKey() == CAMKEY(UP) ||
		 pKeyPress->GetVirtKey() == CAMKEY(DOWN) ||
		 pKeyPress->GetVirtKey() == CAMKEY(LEFT) ||
		 pKeyPress->GetVirtKey() == CAMKEY(RIGHT)) )
	{
		// If we are nudging, then stop the Attribute manager from sending
		// tons of messages, until the nudge stops.
		if (pKeyPress->IsRelease())
			AttributeManager::SendMessages = TRUE;
		else
			AttributeManager::SendMessages = FALSE;

		// Pass on the nudge keys
		return FALSE;
	}
	return FALSE;
}

/********************************************************************************************

> 	void TranspTool::ToggleControlPoints(BOOL Reverse)

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	29/3/94
	Inputs:		Reverse - if TRUE the points cycle backwards
	Purpose:    Toggles the selection state of all visible fill control blobs.
	Notes:		Changed by Gerry (8/96) to use new CycleSelection call

********************************************************************************************/

void TranspTool::ToggleControlPoints(BOOL Reverse)
{
	// Switch control points
	AttrFillGeometry::LastRenderedStartBlob = DocCoord(0,0);
	AttrFillGeometry::LastRenderedEndBlob = DocCoord(0,0);
	AttrFillGeometry::LastRenderedEnd2Blob = DocCoord(0,0);
	AttrFillGeometry::LastRenderedEnd3Blob = DocCoord(0,0);

	AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));

	// Return if there aren't any
	if (pAttrNode == NULL)
		return;

	// We only toggle if there are some points selected
	BOOL DoToggle = AttrFillGeometry::FillSelectionCount() > 0;

	while (pAttrNode != NULL)
	{
		if (pAttrNode->IsATranspFill())
		{
			if (DoToggle)
			{
				// We're going to toggle the selected control points.
				if (pAttrNode->GetSelectionCount() > 0)
				{
					pAttrNode->CycleSelection(Reverse);
				}
			}
			else
			{
				// There were no points selected, so we'll just
				// select all the start points.
				pAttrNode->SelectBlob(FILLCONTROL_STARTPOINT);
			}
		}

		// Check the next fill
		pAttrNode = AttrFillGeometry::FindNextSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));
	}

	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED)); 
}

/********************************************************************************************

> 	void TranspTool::EnableFillNudge()

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	29/3/94
    Purpose:    Enables the fill control point nudging.
	Errors:		-
                   			                                     
********************************************************************************************/
void TranspTool::EnableFillNudge()
{
	if (!GradFillTool::AllowFillNudges || NudgeFills)
		return;

	//Alias all the nudge ops to the fill specific ones.
	OpFillNudge::NudgeColours(FALSE);

	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP1);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeUp1),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP5);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeUp5),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP10);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeUp10),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPFIFTH);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeUpFifth),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPPIXEL1);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeUpPixel1),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPPIXEL10);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeUpPixel10),NULL,0);  

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN1);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeDown1),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN5);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeDown5),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN10);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeDown10),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNFIFTH);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeDownFifth),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNPIXEL1);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeDownPixel1),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNPIXEL10);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeDownPixel10),NULL,0);  

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT1);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeLeft1),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT5);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeLeft5),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT10);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeLeft10),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTFIFTH);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeLeftFifth),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTPIXEL1);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeLeftPixel1),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTPIXEL10);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeLeftPixel10),NULL,0);  

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT1);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeRight1),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT5);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeRight5),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT10);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeRight10),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTFIFTH);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeRightFifth),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTPIXEL1);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeRightPixel1),NULL,0);  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTPIXEL10);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpFillNudgeRightPixel10),NULL,0);  


	NudgeFills = TRUE;
}

/********************************************************************************************

> 	void TranspTool::DisableFillNudge()

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	29/3/94
    Purpose:    Disables the fill control point nudging.
	Errors:		-
                   			                                     
********************************************************************************************/

void TranspTool::DisableFillNudge()
{
	if (!GradFillTool::AllowFillNudges || !NudgeFills)
		return;

	NudgeFills = FALSE;

	//Un-alias all the nudge ops back like they were.
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP1);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP5);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP10);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPFIFTH);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPPIXEL1);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPPIXEL10);  
	pOpDesc->RemoveAlias();  

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN1);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN5);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN10);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNFIFTH);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNPIXEL1);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNPIXEL10);  
	pOpDesc->RemoveAlias();  

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT1);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT5);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT10);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTFIFTH);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTPIXEL1);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTPIXEL10);  
	pOpDesc->RemoveAlias();  

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT1);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT5);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT10);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTFIFTH);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTPIXEL1);  
	pOpDesc->RemoveAlias();  
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTPIXEL10);  
	pOpDesc->RemoveAlias();  
}

/********************************************************************************************

> 	BOOL TranspTool::IsFillNudgeEnabled()

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	29/3/94
    Returns:   	TRUE  - Fill nudging is disabled
				FALSE - Fill nudging is enabled
    Purpose:    Checks for to see if the fill nudging is currently enabled or not.
	Errors:		-
                   			                                     
********************************************************************************************/

BOOL TranspTool::IsFillNudgeEnabled()
{
	return NudgeFills;
}

/*****************************************************************************

>	virtual BOOL TranspTool::GetStatusLineText(String_256* ptext, Spread* pSpread,
												 DocCoord DocPos, ClickModifiers ClickMods);

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Inputs:		pSpread   - pointer to spread under mouse (else NULL)
				DocPos    - position of mouse in doc (in spread coords)
				ClickMods - mouse click modifiers
	Outputs:	ptext - text for status line
	Returns:	TRUE if outputting valid text
	Purpose:	generate up-to-date text for the status line (called on idles)

*****************************************************************************/

BOOL TranspTool::GetStatusLineText(String_256* ptext, Spread* pSpread, 
									 DocCoord DocPos, ClickModifiers ClickMods)
{
	ERROR3IF(ptext==NULL,"TranspTool::GetStatusLineText() - ptext passed as null");

	if (ClickMods.Adjust)
		TempGeometryIndex = FGMENU_CIRCULARTRANSP;
	else
		TempGeometryIndex = -1;

	GetCursorAndStatus(DocPos, pSpread, NULL, ptext);

	return TRUE;
}

/********************************************************************************************

>	void TranspTool::GetCursorAndStatus(DocCoord Pos, Spread* pSpread, 
										  Cursor** pCursor, String_256* pStatus)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Inputs:		Pos - The current position of the mouse.
	Purpose:	Get the correct cursor and status message for this mouse position.

********************************************************************************************/

void TranspTool::GetCursorAndStatus(DocCoord Pos, Spread* pSpread, 
									  Cursor** pCursor, String_256* pStatus)
{
	// Setup defaults
	UINT32 Status;
	Cursor* pNewCursor = pTranspCursor;

	// Is the mouse over any of our fill control points ?
	if ( AttrFillGeometry::CheckForFillControlHit(Pos, &Status) )
	{
		// Status will have been updated

		// Change the cursor, to indicate the mouse is over a control point
		pNewCursor = pTranspPointCursor;
	}
	else
	{
		INT32 Index = pTranspInfoBarOp->CurrentGeometryIndex;

		if (TempGeometryIndex != -1)
			Index = TempGeometryIndex;

		// Check our menu status
		switch (Index)
		{      
			// If fill controls are selected then we indicate 
			// that a click will deselect them.
			// Otherwise we just show what kind of fill will
			// be created when the user drags.

		 	case (FGMENU_NOTRANSP):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_TS_CREATELINEAR_S);		
				else                                    
                	Status = _R(IDS_TS_CREATELINEAR);
                break;

		 	case (FGMENU_FLATTRANSP):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_TS_CREATELINEAR_S);		
				else                                    
                	Status = _R(IDS_TS_CREATELINEAR);
                break;

		 	case (FGMENU_LINEARTRANSP):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_TS_CREATELINEAR_S);		
				else                                    
                	Status = _R(IDS_TS_CREATELINEAR);
                break;

		 	case (FGMENU_CIRCULARTRANSP):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_TS_CREATECIRCLE_S);
				else
                	Status = _R(IDS_TS_CREATECIRCLE);
				break;

		 	case (FGMENU_RADIALTRANSP):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_TS_CREATEELLIP_S);
				else
                	Status = _R(IDS_TS_CREATEELLIP);
				break;

		 	case (FGMENU_CONICALTRANSP):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_TS_CREATECONICAL_S);
				else
                	Status = _R(IDS_TS_CREATECONICAL);
				break;

		 	case (FGMENU_SQUARETRANSP):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_TS_CREATESQUARE_S);
				else
                	Status = _R(IDS_TS_CREATESQUARE);
				break;

		 	case (FGMENU_THREECOLTRANSP):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_TS_CREATETHREECOL_S);
				else
                	Status = _R(IDS_TS_CREATETHREECOL);
				break;

		 	case (FGMENU_FOURCOLTRANSP):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_TS_CREATEFOURCOL_S);
				else
                	Status = _R(IDS_TS_CREATEFOURCOL);
				break;

		 	case (FGMENU_BITMAPTRANSP):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_TS_CREATEBITMAP_S);
				else
                	Status = _R(IDS_TS_CREATEBITMAP);
				break;

		 	case (FGMENU_FRACTALTRANSP):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_TS_CREATEFRACTAL_S);
				else
                	Status = _R(IDS_TS_CREATEFRACTAL);
				break;
		 	
			case (FGMENU_NOISETRANSP):
				if ( AttrFillGeometry::FillSelectionCount() > 0 )
                	Status = _R(IDS_TS_CREATENOISE_S);
				else
                	Status = _R(IDS_TS_CREATENOISE);
				break;

			default:
				// Er .. Dunno what kind of fill this is ?
				Status = _R(IDS_TS_CREATELINEAR);
				break;
		}
	}

	if (pStatus != NULL)
	{
		pStatus->Load(Status);
		LastStatusID = Status;
	}

	if (pCursor != NULL)
		*pCursor = pNewCursor;
}

/********************************************************************************************

>	static void TranspTool::DisplayStatusBarHelp(UINT32 StatusID)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		StatusID = ID of status help string
	Outputs:	-
	Returns:	-
	Purpose:	Displays the given status help string in the status bar
	SeeAlso:	-

********************************************************************************************/

void TranspTool::DisplayStatusBarHelp(UINT32 StatusID)
{
	String_256 StatusMsg("");
	StatusMsg.Load(StatusID);
	GetApplication()->UpdateStatusBarText(&StatusMsg);
	LastStatusID = StatusID;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//			Implementation of class GradInfoBarOp (graduated fill tool infobar)

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************

>	struct FGMItem 

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/06/97
	Purpose:	Associate the FillGeometry enum with the resource id of the text that gets
				displayed on the menu.
	SeeAlso:	TGMItem, FillGeometry enum

********************************************************************************************/

struct FGMItem
{
	FillGeometry Geometry;
	unsigned StringID;
};

/********************************************************************************************

>	const FGMItem FillGeometryMenu[] 

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/06/97
	Purpose:	Array of 'FGMItem's that make up the geometry menu. We need to keep track of
				this because list combo boxes can only return the position of the selected
				item, not what it actually is. All this fuctionality should be in the gadgets
				not here.
	SeeAlso:	FillGeometry enum, TransparencyGeometryMenu

********************************************************************************************/

const FGMItem FillGeometryMenu[] =
{
	{ FGMENU_FLAT,		_R(IDS_FILLTOOL_FLATFILL) },
	{ FGMENU_LINEAR,	_R(IDS_FILLTOOL_LINEAR) },
	{ FGMENU_CIRCULAR,	_R(IDS_FILLTOOL_CIRCULAR) },
	{ FGMENU_RADIAL,	_R(IDS_FILLTOOL_ELLIPTICAL) },
#ifndef WEBSTER
	{ FGMENU_CONICAL,	_R(IDS_FILLTOOL_CONICAL) },
	{ FGMENU_SQUARE,	_R(IDS_FILLTOOL_SQUARE) },
	{ FGMENU_THREECOL,	_R(IDS_FILLTOOL_THREECOL) },
	{ FGMENU_FOURCOL,	_R(IDS_FILLTOOL_FOURCOL) },
#endif //WEBSTER
	{ FGMENU_BITMAP,	_R(IDS_FILLTOOL_BITMAP) },
	{ FGMENU_FRACTAL,	_R(IDS_FILLTOOL_FRACTAL) },
#ifndef WEBSTER
	{ FGMENU_NOISE,		_R(IDS_FILLTOOL_NOISE) }
#endif //WEBSTER
};

/********************************************************************************************

>	const INT32 FillGeometryMenuCount 

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/06/97
	Purpose:	Number of items on the FillGeometryMenu menu.
	SeeAlso:	FillGeometryMenu

********************************************************************************************/

const INT32 FillGeometryMenuCount = sizeof(FillGeometryMenu) / sizeof(FillGeometryMenu[0]);

/********************************************************************************************

>	MsgResult GradInfoBarOp::Message(Msg* Message) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Inputs:		Message: The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Gradfill info bar dialog message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult GradInfoBarOp::Message(Msg* Message) 
{
	if (!GradFillTool::IsCurrentTool() || !IsVisible())
	{
		return (InformationBarOp::Message(Message));
	}

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		// Check if the message is a CANCEL
		if (Msg->DlgMsg == DIM_CANCEL)
		{
			Close(); // Close the dialog 
		}
		else if (Msg->DlgMsg == DIM_CREATE)
		{
			// Initialise the controls 
			InitControls();
		}
		else
		{
			if (Msg->GadgetID == _R(IDC_GEOMETRY))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED :
					{
						ShowCommonType (FALSE);		// specifying false means that this
													// gets (but does not show) CurrentGeometryIndex
						
						// Someone selected a new Fill Geometry
						INT32 Index;
						GetValueIndex(_R(IDC_GEOMETRY),&Index);

						BOOL AllowFractal = FALSE;	// we need to still allow the user to
													// select the same fractal fill
													// again and again - since we apply a new
													// seed on each occaison ....

						if (Index == FGMENU_FRACTAL)
						{
							if (CurrentGeometryIndex == Index)
							{
								AllowFractal = TRUE;
							}
						}

						if (Index == FGMENU_NOISE)
						{
							if (CurrentGeometryIndex == Index)
							{
								AllowFractal = TRUE;
							}
						}
						
						if ((CurrentGeometryIndex != Index) || (AllowFractal))
						{
							// Change the Fill Geometry Here
							CurrentGeometryIndex = FillGeometryMenu[Index].Geometry;

							// CGS - need to deselect all mult-stage fill blobs before
							// chnaging the fill type - otherwise we get redraw problems ....
							// Karim 30/11/2000 - rewritten to not use BevelTools,
							// which is slower and carries memory-leak risks.
							Range* pSelRange = GetApplication()->FindSelection();
							if (pSelRange != NULL)
							{
								for ( Node*	pSelNode =	pSelRange->FindFirst();
											pSelNode !=	NULL;
											pSelNode =	pSelRange->FindNext(pSelNode) )
								{
									if (pSelNode->IsAnAttribute() &&
										pSelNode->IS_KIND_OF(AttrFillGeometry))
									{
										FillRamp* pRamp = ((AttrFillGeometry*)pSelNode)->GetFillRamp();
										if (pRamp != NULL)
											pRamp->DeselectAll();
									}
								}
							}
							
							ChangeFillType();
							EnableControls();
						}
					}
					break;

					default:
						break;
				}
			}

			else if (Msg->GadgetID == _R(IDC_MAPPING))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED:
					{
						ShowCommonMapping (FALSE);		// specifying false means that this
														// gets (but does not show) CurrentMappingIndex
						
						// Someone selected a new Fill Mapping
						INT32 Index;  
						GetValueIndex(_R(IDC_MAPPING),&Index);

						if (CurrentMappingIndex != Index)
						{
							// Change the Fill Mapping Here
							CurrentMappingIndex = Index;
							ChangeFillMapping();
						}
					}
					break;

					default:
						break;
				}
			}

			else if (Msg->GadgetID == _R(IDC_EFFECT))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED:
					{
						ShowCommonEffect (FALSE);	// specifying false means that this
													// gets (but does not show) CurrentEffectIndex
						
						// Someone selected a new Fill Effect
						INT32 Index;  
						GetValueIndex(_R(IDC_EFFECT),&Index);

						if (CurrentEffectIndex != Index)
						{
							// Change the Fill Effect Here
							CurrentEffectIndex = Index;
							ChangeFillEffect();
						}
					}
					break;

					default:
						break;
				}
			}

			else if (Msg->GadgetID == _R(IDC_BITMAPEFFECT))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED:
					{
						// Someone selected a new bitmap Fill Effect
						INT32 Index;
						GetValueIndex (_R(IDC_BITMAPEFFECT), &Index);

						//if (CurrentEffectIndex != Index)
						//{
							// Change the bitmap Fill Effect Here
							// (we use CurrentEffectIndex because of the days when
							// _R(IDC_EFFECT) handled this as well; thus we avoid having
							// to change all of the code that relates to this
							CurrentEffectIndex = Index;
							ChangeBitmapName ();
						//}
					}
					break;

					default:
						break;
				}
			}

			else if (Msg->GadgetID == _R(IDC_BIASGAIN))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_LFT_BN_CLICKED:
						HandleProfileButtonClick (m_BiasGainGadget, _R(IDC_BIASGAIN));
						break;

					default:
						ProfileSelectionChange( Msg, Msg->GadgetID );
						break;
				}
			}

			else if (Msg->GadgetID == _R(IDC_SELPOINT))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED:
					{
						if (AttrFillGeometry::SelectionCount == 0)
						{
							if (Mode == BITMAPFILL || 
								Mode == FRACTALFILL || 
								Mode == NOISEFILL ) 
							{
								ChangeDPI();
							}
						}
					}
					break;
					default:
						break;
				}
			}

			else if (Msg->GadgetID == _R(IDC_SELCOLOUR))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED:
					{
						if (AttrFillGeometry::SelectionCount == 0)
						{
							if (Mode == FRACTALFILL)
								ChangeFractalGrain();
							if (Mode == NOISEFILL)
								ChangeNoiseScale();
						}
					}
					break;

					default:
						break;
				}
			}
		}
	}

	if (MESSAGE_IS_A(Message, SelChangingMsg) ||
		MESSAGE_IS_A(Message, CurrentAttrChangedMsg))
	{
		// The selection has changed in some way
		// Re-Count the number of selected fill control points
		AttrFillGeometry::SetSelectionCount(AttrFillGeometry::CountSelectionControlPoints());

		if (AttrFillGeometry::SelectionCount > 0)
		{
			GradFillTool::EnableFillNudge();
		}
		else
		{
			GradFillTool::DisableFillNudge();
		}

		HandleProfileSelChangingMsg (m_BiasGainGadget, _R(IDC_BIASGAIN));

		ShowInfo();
/*
		SelChangingMsg* Msg = (SelChangingMsg*)Message;
		switch (Msg->State)
		{
		 	case SelChangingMsg::COLOURATTCHANGED:
				// The colour of an attribute has changed
				TRACEUSER( "Will", _T("ColourAttChanged\n"));
				InitControls();
				break;

		 	case SelChangingMsg::NONCOLOURATTCHANGED:
				// The selection state or control points of an attribute have changed
				TRACEUSER( "Will", _T("NonColourAttChanged\n"));
				InitControls();
				break;
		}
*/
	}

	// do we have a selection change message? 
	if (MESSAGE_IS_A(Message, SelChangingMsg))
	{
		//ShowInfo();		// done in above call anyway - so not needed here ....
		HandleProfileSelChangingMsg (m_BiasGainGadget, _R(IDC_BIASGAIN));
		ShowInfo();
	}

	if (MESSAGE_IS_A(Message, BitmapListChangedMsg) &&
		Document::GetSelected() != NULL)
	{	
		InitBitmapName();
		ShowInfo();
	}
	
	// Pass the message on
	return (InformationBarOp::Message(Message));
}    



/********************************************************************************************

>	void GradInfoBarOp::ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Purpose:	See InformationBarOp::ChangeProfile () for an explanation of this function.
	See Also:	InformationBarOp::ChangeProfile ()

*********************************************************************************************/

void GradInfoBarOp::ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID)
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FILLPROFILE);
	if (pOpDesc != NULL)
	{			
		BOOL FireOp = TRUE;

		// we only want to generate one bit of undo information - so decided whether
		// we have to fire the above op, or whether we just 'pump' the values into
		// our nodes (thereby nolonger generating infinite undo information) ....

		Operation* pLastOp = NULL;

		if (Profile->GetGeneratesInfiniteUndo ())	// only do if they didn't select a preset profile
		{
			pLastOp = Document::GetSelected()->GetOpHistory().FindLastOp();
		}

		if (pLastOp)
		{
			if (pLastOp->GetRuntimeClass() == CC_RUNTIME_CLASS(OpChangeFillProfile))
			{
				FireOp = FALSE;
			}
		}

		if (FireOp == TRUE)
		{
			FillProfileOpParam Param;
			Param.Profile = *Profile;
		
			pOpDesc->Invoke(&Param);
		}
		else
		{
			// we don't need/want any undo information - so just change the value ....
			
			//OpChangeFillProfile Op;
			ChangeFillProfileAction Action;
			Action.ChangeFillProfileWithNoUndo (/*&Op,*/ *Profile);
		}
	}
}



/********************************************************************************************

>	virtual CProfileBiasGain* GetProfileFromSelection(CGadgetID GadgetID, INT32* Index, BOOL* bAllSameType)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Inputs:		The GadgetID of the CBiasGainGadget that we are dealing with.
	Outputs:	bMany - returned as TRUE if we have MANY profiles selected.
				bAllSameType - returned as TRUE if objects within selection are all of the
				same type.
	returns		Ptr to common CProfileBiasGain, or NULL if there is NOT one.
	Purpose:	See InformationBarOp::GetProfileFromSelection () for a description of this
				function.

*********************************************************************************************/

CProfileBiasGain* GradInfoBarOp::GetProfileFromSelection(CGadgetID GadgetID, BOOL* bMany, BOOL* bAllSameType)
{
	BOOL ok = (GadgetID == _R(IDC_BIASGAIN));

	ERROR2IF(ok==FALSE, FALSE, "Invalid gadgetID passed");
	
	UINT32 TotalNumberSelected = (GetApplication()->FindSelection()->Count ());
	
	// get the list of all the shadows
	List ShadowList;
//	FillTools::BuildListOfSelectedNodes(&ShadowList, CC_RUNTIME_CLASS(AttrFillGeometry));
	FillTools::GetSelectedAttrList(&ShadowList, CC_RUNTIME_CLASS(AttrFillGeometry), TRUE, FALSE);

	AttrFillGeometry* pFirstNodeShadow = NULL;
	FillGeometryAttribute* pFirstFillGeoAttr = NULL;

	//CProfileBiasGain Profile;
	CProfileBiasGain* pFirstProfile = NULL;

	/*if (TotalNumberSelected != (UINT32) ShadowList.GetCount ())
	{
		// totals differ - so the user MUST have selected someother type of node as well
		*bAllSameType = FALSE;
		ShadowList.DeleteAll();
		return (NULL);
	}*/

	NodeListItem * pItem = (NodeListItem *)ShadowList.GetHead();

	while (pItem)
	{
		if (pFirstNodeShadow == NULL)
		{
			pFirstNodeShadow = (AttrFillGeometry*) pItem->pNode;
			pFirstFillGeoAttr = (FillGeometryAttribute*) pFirstNodeShadow->GetAttributeValue ();

			pFirstProfile = pFirstFillGeoAttr->GetProfilePtr ();

			if (pFirstNodeShadow->GetFillRamp () != NULL)
			{
				*bAllSameType = FALSE;
			}
		}
		else
		{		
			if (((AttrFillGeometry*)pItem->pNode)->GetFillRamp () != NULL)
			{
				*bAllSameType = FALSE;
			}

			CProfileBiasGain* pOtherProfile = NULL;

			pOtherProfile = ((FillGeometryAttribute*) ((AttrFillGeometry*)pItem->pNode)->GetAttributeValue ())->GetProfilePtr ();

			if (pOtherProfile)
			{
				if (*pFirstProfile == *pOtherProfile)
				{
					// all ok
				}
				else
				{
					*bMany = TRUE;
				}
			}
		}

		pItem = (NodeListItem *)ShadowList.GetNext(pItem);
	}

	if (TotalNumberSelected != (UINT32) ShadowList.GetCount ())
	{
		// totals differ - so the user MUST have selected someother type of node as well
		
		if ((pFirstProfile) && (*bMany == FALSE))
		{
			// scan each node in the list, and see if they have the attribute applied ....

			SelRange* range = GetApplication()->FindSelection();

			NodeRenderableInk* pNode = (NodeRenderableInk*) range->FindFirst ();
			NodeAttribute* pAttr;

			while (pNode)
			{
				if (pNode->FindAppliedAttribute (CC_RUNTIME_CLASS (AttrFillGeometry), &pAttr))
				{
					pNode = (NodeRenderableInk*) (range->FindNext (pNode));
				}
				else
				{
					// the node ain't got one

					*bAllSameType = FALSE;
					ShadowList.DeleteAll();
					return (NULL);
				}
			}
		}
		else
		{	
			*bAllSameType = FALSE;
			ShadowList.DeleteAll();
			return (NULL);
		}
	}
	if (m_BiasGainGadget.GetUseFillProfile () == FALSE)		// biasgain dialog is configured
	{														// as an 'object' spacing
		ShadowList.DeleteAll();
		if (*bMany == TRUE)
		{
			return (NULL);
		}
		else
		{
			return (pFirstProfile);
		}
	}
	else		// biasgain dialog is configured as an interpolation (between two colours)
	{
		// so we had best attempt to get them ....
		if (*bMany == TRUE)				// don't matter in this case anyway
		{
			ShadowList.DeleteAll();
			return (NULL);
		}
		else
		{
			// BUT it does here ....
			// NOTE:  we can only be 'context' sensitive for a single fill within the
			// selection - so lets just choose the first one that we came accross ....

			if (pFirstNodeShadow)
			{	
				m_BiasGainGadget.SetStartColour (pFirstNodeShadow->GetStartColour ());
				m_BiasGainGadget.SetEndColour (pFirstNodeShadow->GetEndColour ());

				// we also need to find ourselves an effect type (i.e.  fade, rainbow or alt rainbow)

				String_64 Str;

				CommonAttrEffect = SelRange::ATTR_NONE;
				Str.Load(_R(IDS_FILLTOOL_NONE));

				if (Selection != NULL)
				{
					// Try and find a common Attribute within the selection.
					// All objects in the selection must have an Identical fill for this
					// to find anything.
					CommonAttrEffect = Selection->FindCommonAttribute(CC_RUNTIME_CLASS(AttrFillEffect), 
																(NodeAttribute**)&CommonEffect);
					CCRuntimeClass* EffectType;

					// Try and find a common Attribute Type within the selection.
					// The fills need not be identical. Just of the same type.
					SelRange::CommonAttribResult CommonType = 
							Selection->FindCommonAttributeType(CC_RUNTIME_CLASS(AttrFillEffect), 
																&EffectType);

					if (CommonAttrEffect == SelRange::ATTR_NONE &&
						CommonEffect != NULL)
					{
						CommonAttrEffect = SelRange::ATTR_COMMON;
					}

					if (CommonType == SelRange::ATTR_NONE &&
						EffectType != NULL)
					{
						CommonType = SelRange::ATTR_COMMON;
					}

					FillEffectAttribute* TheEffect = NULL;
    
					// Make the menu show any Common Fill Type
					// or failing that, 'None' or 'Many'.
					if (CommonType == SelRange::ATTR_COMMON)
					{	
		 				if (EffectType == CC_RUNTIME_CLASS(AttrFillEffectFade))
						{
							TheEffect = new FillEffectFadeAttribute ();
						}
						else if (EffectType == CC_RUNTIME_CLASS(AttrFillEffectRainbow))
						{
							TheEffect = new FillEffectRainbowAttribute ();
						}
						else if (EffectType == CC_RUNTIME_CLASS(AttrFillEffectAltRainbow))
						{
							TheEffect = new FillEffectAltRainbowAttribute ();
						}
					}

					if (CommonEffect != NULL)
						m_BiasGainGadget.SetFillEffect (TheEffect);

			//		delete (TheEffect);
				}
				// else we can't alter the fill effect

				ShadowList.DeleteAll();
			}
			
			return (pFirstProfile);
		}
	}
}



/********************************************************************************************

>	void GradInfoBarOp::InitControls()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Purpose:	Inits all the controls in the info bar.
				Called immediately after the bar is created when the gradfill tool
				becomes the current tool.
	SeeAlso:	-

********************************************************************************************/

void GradInfoBarOp::InitControls()
{
	// Don't do anything if the Info bar isn't displayed
	if (!GradFillTool::IsCurrentTool() || !IsVisible()) return;

	DeleteAllValues(_R(IDC_GEOMETRY));
	DeleteAllValues(_R(IDC_EFFECT));
	DeleteAllValues(_R(IDC_BITMAPEFFECT));
	DeleteAllValues(_R(IDC_MAPPING));

	String_64 Str;

	// Setup the Menu Item text entries
	for ( INT32 i=0; i<FillGeometryMenuCount; i++ )
	{
		Str.Load( FillGeometryMenu[i].StringID );
		SetStringGadgetValue( _R(IDC_GEOMETRY), Str, TRUE, FillGeometryMenu[i].Geometry );
	}

	// Ensure the drop-downs are the correct length
	SetComboListLength(_R(IDC_GEOMETRY));

	SetGadgetWritable(_R(IDC_GEOMETRY), FALSE);
	SetGadgetWritable(_R(IDC_MAPPING), FALSE);
	SetGadgetWritable(_R(IDC_EFFECT), FALSE);
	SetGadgetWritable(_R(IDC_SELPOINT), FALSE);
	SetGadgetWritable(_R(IDC_SELCOLOUR), FALSE);

	SetGadgetHelp(_R(IDC_GEOMETRY),		_R(IDBBL_FILLTOOL_FILLTYPE),		_R(IDS_FILLTOOL_FILLTYPE));
	SetGadgetHelp(_R(IDC_EFFECT), 		_R(IDBBL_FILLTOOL_FILLEFFECT),		_R(IDS_FILLTOOL_FILLEFFECT));
	SetGadgetHelp(_R(IDC_BITMAPEFFECT), _R(IDBBL_FILLTOOL_BITMAPNAME),		_R(IDS_FILLTOOL_BITMAPNAME));
	SetGadgetHelp(_R(IDC_MAPPING), 		_R(IDBBL_FILLTOOL_FILLTILING),		_R(IDS_FILLTOOL_FILLTILING));
	SetGadgetHelp(_R(IDC_SELPOINT),		_R(IDBBL_FILLTOOL_FILLHANDLE),		_R(IDS_FILLTOOL_FILLHANDLE));

	// Default to the first items in each list
	SelGeometryIndex = 0;
	SelMappingIndex = 0;
	SelEffectIndex = 0;
	SelPointIndex = 0;

	CommonGeometry = NULL;
	CommonMapping = NULL;
	CommonEffect = NULL;

	MappingDisabled = FALSE;
	EffectDisabled = FALSE;
	AllowForceToSimpleMapping = TRUE;

	Mode = NOFILL;

	m_BiasGainGadget.Init(this, _R(IDC_BIASGAIN), _R(IDBBL_BIASGAIN),  _R(IDS_BIASGAINDLG));
	m_BiasGainGadget.ToggleFillProfile ();

	ShowInfo();
}           

/********************************************************************************************

>	void GradInfoBarOp::ShowInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/95
	Purpose:	Shows Info on the current selection.
	SeeAlso:	-

********************************************************************************************/

void GradInfoBarOp::ShowInfo()
{
	// Don't do anything if the Info bar isn't displayed
	if (!GradFillTool::IsCurrentTool() || !IsVisible()) return;

	// Now have a look at the selected objects
	Selection = GetApplication()->FindSelection();

	OldMode = Mode;

	ShowCommonType();

	// Now show different info depending on the type ...
	// (Well we will do eventually)

	switch (Mode)
	{
		case FLATFILL:
			ShowFlatInfo();
			break;

		case GRADFILL:
		case GRADREPEATFILL:
			ShowGraduatedInfo();
			break;

		case BITMAPFILL:
			ShowBitmapInfo();
			break;

		case FRACTALFILL:
			ShowFractalInfo();
			break;

		case NOISEFILL:
			ShowNoiseInfo();
			break;

		default:
			ShowGraduatedInfo();
			break;
	}

	EnableControls();
}

/********************************************************************************************

>	void GradInfoBarOp::InitEffect()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/95
	Purpose:	Initialise the Effect Control.
	SeeAlso:	-

********************************************************************************************/

void GradInfoBarOp::InitEffect()
{
	DeleteAllValues(_R(IDC_EFFECT));
	EnableGadget(_R(IDC_EFFECT), TRUE);
	EffectDisabled = FALSE;

	String_64 Str;

	Str.Load(_R(IDS_FILLTOOL_FADE));
	SetStringGadgetValue(_R(IDC_EFFECT),Str,FALSE, FEMENU_FADE);
	Str.Load(_R(IDS_FILLTOOL_RAINBOW));
	SetStringGadgetValue(_R(IDC_EFFECT),Str,FALSE, FEMENU_RAINBOW);
	Str.Load(_R(IDS_FILLTOOL_ALTRAINBOW));
	SetStringGadgetValue(_R(IDC_EFFECT),Str,TRUE, FEMENU_ALTRAINBOW);

	SetComboListLength(_R(IDC_EFFECT));
}

/********************************************************************************************

>	void GradInfoBarOp::InitMapping()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/95
	Purpose:	Initialise the Mapping Control.
	SeeAlso:	-

********************************************************************************************/

void GradInfoBarOp::InitMapping()
{
	DeleteAllValues(_R(IDC_MAPPING));
	EnableGadget(_R(IDC_MAPPING), TRUE);
	MappingDisabled = FALSE;

	String_64 Str;

	Str.Load(_R(IDS_FILLTOOL_MAPSIMPLE));
	SetStringGadgetValue(_R(IDC_MAPPING),Str,FALSE, FMMENU_SIMPLE);
	Str.Load(_R(IDS_FILLTOOL_MAPREPEAT));
	SetStringGadgetValue(_R(IDC_MAPPING),Str,TRUE, FMMENU_REPEATING);

//	SetSelectedValueIndex(_R(IDC_MAPPING), 0);
	SetComboListLength(_R(IDC_MAPPING));
}

/********************************************************************************************

>	void GradInfoBarOp::InitBitmapName()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/95
	Purpose:	Initialise the Bitmap Name Control.
	SeeAlso:	-

********************************************************************************************/

void GradInfoBarOp::InitBitmapName()
{
	DeleteAllValues(_R(IDC_BITMAPEFFECT));
	EnableGadget (_R(IDC_BITMAPEFFECT), TRUE);
	
	EffectDisabled = FALSE;

	Document* pDoc = Document::GetSelected();

	// Get the bitmap list
	BitmapList* Bitmaps = NULL;
	if (pDoc) Bitmaps = pDoc->GetBitmapList();

	if (Bitmaps == NULL)
		return;

	String_256 Str;

	if (Bitmaps->GetCount() > 0)
	{
		INT32 Index = 0;

		ListItem* pBmp = Bitmaps->GetHead();

		DeleteAllValues ( _R(IDC_BITMAPEFFECT) );

		while (pBmp != NULL)
		{
			if (!((KernelBitmap*)pBmp)->HasBeenDeleted())	// Ignore deleted bitmaps
			{
				Str = ((KernelBitmap*)pBmp)->ActualBitmap->GetName();

PORTNOTETRACE("other","GradInfoBarOp::InitBitmapName() - removed CustomComboBox handler");
#ifndef EXCLUDE_FROM_XARALX
				CustomComboBoxControlDataItem* theItem = new CustomComboBoxControlDataItem ();
				theItem->itemName = Str;
				theItem->itemID = Index;

				KernelBitmap* bitmap = ((KernelBitmap*)pBmp);

				// make the custom combobox know about our bitmap ....
				theItem->oilItemBitmap = (KernelBitmap*) bitmap;

				// and insert the data item into the cutsom combobox ....
				SetCustomComboGadgetValue( _R(IDC_BITMAPEFFECT), theItem, TRUE, 0);
#else
				SetStringGadgetValue(_R(IDC_BITMAPEFFECT), Str, TRUE, Index);
#endif
				Index++;
			}

			pBmp = Bitmaps->GetNext(pBmp);
		}

		SetComboListLength(_R(IDC_BITMAPEFFECT));
	}
	//else
	//{
		// (not sure about this for a custom box)
		
		//Str.Load(_R(IDS_FILLTOOL_DEFAULTNAME));
		//SetStringGadgetValue(_R(IDC_EFFECT),&Str,TRUE, 0);
	//}
}


/********************************************************************************************

>	void GradInfoBarOp::InitTesselate()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/95
	Purpose:	Initialise the Tesselation Control.
	SeeAlso:	-

********************************************************************************************/

void GradInfoBarOp::InitTesselate()
{
	DeleteAllValues(_R(IDC_MAPPING));
	EnableGadget(_R(IDC_MAPPING), TRUE);
	MappingDisabled = FALSE;

	String_64 Str;

	Str.Load(_R(IDS_FILLTOOL_TESS_SIMPLE));
	SetStringGadgetValue(_R(IDC_MAPPING),Str,FALSE, FTMENU_SIMPLE);
	Str.Load(_R(IDS_FILLTOOL_TESS_REPEAT));
	SetStringGadgetValue(_R(IDC_MAPPING),Str,FALSE, FTMENU_REPEAT);
	Str.Load(_R(IDS_FILLTOOL_TESS_REPEATINV));
	SetStringGadgetValue(_R(IDC_MAPPING),Str,TRUE, FTMENU_REPEATINV);

	SetComboListLength(_R(IDC_MAPPING));
}

/********************************************************************************************

>	BOOL GradInfoBarOp::SetGadgetString(CGadgetID Gadget, StringBase* StrValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/95
	Purpose:	Set the string displayed by a Gadget.
				This version only updates the gadget if it has changed.
	SeeAlso:	-

********************************************************************************************/

BOOL GradInfoBarOp::SetGadgetString(CGadgetID Gadget, StringBase* StrValue)
{
	if (Gadget == _R(IDC_EFFECT) && EffectDisabled)
		return TRUE;

	if (Gadget == _R(IDC_MAPPING) && MappingDisabled)
		return TRUE;

	if (GetStringGadgetValue(Gadget, NULL, -1) != *StrValue)
  	{
		return SetStringGadgetValue(Gadget, *StrValue, FALSE, -1);
	}

	return TRUE;
}

/********************************************************************************************

>	void GradInfoBarOp::ShowFlatInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Purpose:	Show info on a flat fill.

********************************************************************************************/

void GradInfoBarOp::ShowFlatInfo()
{
	if (OldMode != FLATFILL)
	{	
		InitMapping();
		InitEffect();

		SetGadgetHelp(_R(IDC_EFFECT), 		_R(IDBBL_FILLTOOL_FILLEFFECT),		_R(IDS_FILLTOOL_FILLEFFECT));
		SetGadgetHelp(_R(IDC_MAPPING), 		_R(IDBBL_FILLTOOL_FILLTILING),		_R(IDS_FILLTOOL_FILLTILING));
		SetGadgetHelp(_R(IDC_SELPOINT),		_R(IDBBL_FILLTOOL_FILLHANDLE),		_R(IDS_FILLTOOL_FILLHANDLE));
		//SetGadgetHelp(_R(IDC_SELCOLOUR),	_R(IDBBL_FILLTOOL_FILLCOLOUR),		_R(IDS_FILLTOOL_FILLCOLOUR));
	}

	ShowCommonEffect();
	ShowCommonMapping();
	ShowControlPointInfo();
}

/********************************************************************************************

>	void GradInfoBarOp::ShowGraduatedInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Purpose:	Show info on a graduated fill.

********************************************************************************************/

void GradInfoBarOp::ShowGraduatedInfo()
{
	if (OldMode != GRADFILL && OldMode != GRADREPEATFILL)
	{	
		InitMapping();
		InitEffect();

		SetGadgetHelp(_R(IDC_EFFECT), 		_R(IDBBL_FILLTOOL_FILLEFFECT),		_R(IDS_FILLTOOL_FILLEFFECT));
		SetGadgetHelp(_R(IDC_MAPPING), 		_R(IDBBL_FILLTOOL_FILLTILING),		_R(IDS_FILLTOOL_FILLTILING));
		SetGadgetHelp(_R(IDC_SELPOINT),		_R(IDBBL_FILLTOOL_FILLHANDLE),		_R(IDS_FILLTOOL_FILLHANDLE));
		//SetGadgetHelp(_R(IDC_SELCOLOUR),	_R(IDBBL_FILLTOOL_FILLCOLOUR),		_R(IDS_FILLTOOL_FILLCOLOUR));
	}

	ShowCommonEffect();
	ShowCommonMapping();
	ShowControlPointInfo();
}

/********************************************************************************************

>	void GradInfoBarOp::ShowBitmapInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Purpose:	Show info on a bitmap fill.

********************************************************************************************/

void GradInfoBarOp::ShowBitmapInfo()
{
	if (OldMode != BITMAPFILL)
	{
		InitTesselate ();
		InitEffect ();				// we need to do this because of the possibility of contoned bitmaps ....
		InitBitmapName ();

		SetGadgetHelp(_R(IDC_BITMAPEFFECT), _R(IDBBL_FILLTOOL_BITMAPNAME),		_R(IDS_FILLTOOL_BITMAPNAME));
		SetGadgetHelp(_R(IDC_MAPPING), 		_R(IDBBL_FILLTOOL_FILLTILING),		_R(IDS_FILLTOOL_FILLTILING));
		//SetGadgetHelp(_R(IDC_SELCOLOUR),	_R(IDBBL_FILLTOOL_FILLCOLOUR),		_R(IDS_FILLTOOL_FILLCOLOUR));
	}

    if (AttrFillGeometry::SelectionCount == 0)
	{
		SetGadgetHelp(_R(IDC_SELPOINT),  _R(IDBBL_FILLTOOL_BITMAPRES),  _R(IDS_FILLTOOL_BITMAPRES));
		SetGadgetWritable(_R(IDC_SELPOINT), TRUE);
	}
	else
	{
		SetGadgetHelp(_R(IDC_SELPOINT),  _R(IDBBL_FILLTOOL_FILLHANDLE), _R(IDS_FILLTOOL_FILLHANDLE));
		SetGadgetWritable(_R(IDC_SELPOINT), FALSE);
	}

	ShowCommonTesselate();
	ShowCommonEffect ();
	ShowCommonBitmapName();
	ShowControlPointInfo();

	if (AttrFillGeometry::SelectionCount == 0)
		ShowCommonBitmapDpi();
}



/********************************************************************************************

>	void GradInfoBarOp::ShowFractalInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Purpose:	Show info on a fractal fill.

********************************************************************************************/

void GradInfoBarOp::ShowFractalInfo()
{
	if (OldMode != FRACTALFILL)
	{	
//		InitTransType ();
		InitTesselate();
		InitEffect();

		SetGadgetHelp(_R(IDC_EFFECT), 	 _R(IDBBL_FILLTOOL_FILLEFFECT), _R(IDS_FILLTOOL_FILLEFFECT));
		SetGadgetHelp(_R(IDC_MAPPING), 	 _R(IDBBL_FILLTOOL_FILLTILING), _R(IDS_FILLTOOL_FILLTILING));
	}

    if (AttrFillGeometry::SelectionCount == 0)
	{
		SetGadgetHelp(_R(IDC_SELPOINT),  _R(IDBBL_FILLTOOL_FRACTALRES),  _R(IDS_FILLTOOL_FRACTALRES));
		SetGadgetHelp(_R(IDC_SELCOLOUR), _R(IDBBL_FILLTOOL_GRAININESS), _R(IDS_FILLTOOL_GRAININESS));
		SetGadgetWritable(_R(IDC_SELPOINT), TRUE);
		SetGadgetWritable(_R(IDC_SELCOLOUR), TRUE);
	}
	else
	{
		SetGadgetHelp(_R(IDC_SELPOINT),  _R(IDBBL_FILLTOOL_FILLHANDLE), _R(IDS_FILLTOOL_FILLHANDLE));
		SetGadgetHelp(_R(IDC_SELCOLOUR), _R(IDBBL_FILLTOOL_FILLCOLOUR), _R(IDS_FILLTOOL_FILLCOLOUR));
		SetGadgetWritable(_R(IDC_SELPOINT), FALSE);
		SetGadgetWritable(_R(IDC_SELCOLOUR), FALSE);
	}

//	ShowCommonTranspType();
	ShowCommonTesselate();
	ShowCommonEffect();

    if (AttrFillGeometry::SelectionCount == 0)
	{
		ShowCommonBitmapDpi();
		ShowCommonFractalGrain();
	}
	else
	{
		ShowControlPointInfo();
	}
}

/********************************************************************************************

>	void GradInfoBarOp::ShowNoiseInfo()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/97
	Purpose:	Show info on a noise fill.

********************************************************************************************/

void GradInfoBarOp::ShowNoiseInfo()
{
	if (OldMode != NOISEFILL)
	{	
		InitTesselate();
		InitEffect();

		SetGadgetHelp(_R(IDC_EFFECT), 		_R(IDBBL_FILLTOOL_FILLEFFECT),		_R(IDS_FILLTOOL_FILLEFFECT));
		SetGadgetHelp(_R(IDC_MAPPING), 		_R(IDBBL_FILLTOOL_FILLTILING),		_R(IDS_FILLTOOL_FILLTILING));
	}

    if (AttrFillGeometry::SelectionCount == 0)
	{
		SetGadgetHelp(_R(IDC_SELPOINT),  _R(IDBBL_FILLTOOL_NOISERES),   _R(IDS_FILLTOOL_NOISERES));
		SetGadgetHelp(_R(IDC_SELCOLOUR), _R(IDBBL_FILLTOOL_NOISESCALE), _R(IDS_FILLTOOL_NOISESCALE));
		SetGadgetWritable(_R(IDC_SELPOINT), TRUE);
		SetGadgetWritable(_R(IDC_SELCOLOUR), TRUE);
	}
	else
	{
		SetGadgetHelp(_R(IDC_SELPOINT),  _R(IDBBL_FILLTOOL_FILLHANDLE), _R(IDS_FILLTOOL_FILLHANDLE));
		SetGadgetHelp(_R(IDC_SELCOLOUR), _R(IDBBL_FILLTOOL_FILLCOLOUR), _R(IDS_FILLTOOL_FILLCOLOUR));
		SetGadgetWritable(_R(IDC_SELPOINT), FALSE);
		SetGadgetWritable(_R(IDC_SELCOLOUR), FALSE);
	}

	ShowCommonTesselate();
	ShowCommonEffect();

    if (AttrFillGeometry::SelectionCount == 0)
	{
		ShowCommonBitmapDpi();
		ShowCommonNoiseScale();
	}
	else
	{
		ShowControlPointInfo();
	}
}

/********************************************************************************************

>	void GradInfoBarOp::ShowCommonType(BOOL ShowDataAndNotSet = TRUE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (altered by Chris Snook)
	Created:	25/8/94
	Inputs:		if ShowDataAndNotSet is FALSE, then data members will be set (for controlling
				menu selection logic) and data will not be shown in the relevant control.
	Purpose:	Show any Common Fill Type in the selection.

********************************************************************************************/

void GradInfoBarOp::ShowCommonType(BOOL ShowDataAndNotSet)
{
	String_64 Str;

	CommonAttr = SelRange::ATTR_NONE;
	Str.Load(_R(IDS_FILLTOOL_NONE));

	if (Selection != NULL)
	{
		Document* pCurrentDoc = Document::GetCurrent();
		if (Document::GetSelected())
			Document::GetSelected()->SetCurrent();

		// Try and find a common Attribute within the selection.
		// All objects in the selection must have an Identical fill for this
		// to find anything.
		CommonAttr = Selection->FindCommonAttribute(CC_RUNTIME_CLASS(AttrFillGeometry), 
													(NodeAttribute**)&CommonGeometry);
		CCRuntimeClass* GeometryType;

		// Try and find a common Attribute Type within the selection.
		// The fills need not be identical. Just of the same type.
		SelRange::CommonAttribResult CommonType = 
				Selection->FindCommonAttributeType(CC_RUNTIME_CLASS(AttrFillGeometry), 
												   &GeometryType);

		if (CommonAttr == SelRange::ATTR_NONE &&
			CommonGeometry != NULL)
		{
			CommonAttr = SelRange::ATTR_COMMON;
		}

		if (CommonType == SelRange::ATTR_NONE &&
			GeometryType != NULL)
		{
			CommonType = SelRange::ATTR_COMMON;
		}
    
		// Make the menu show any Common Fill Type
		// or failing that, 'None' or 'Many'.
		if (CommonType == SelRange::ATTR_COMMON)
		{	
		 	if (GeometryType == CC_RUNTIME_CLASS(AttrFlatColourFill))
			{
				Str.Load(_R(IDS_FILLTOOL_FLATFILL));
				Mode = FLATFILL;
				CurrentGeometryIndex = FGMENU_FLAT;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrLinearColourFill))
			{
				Str.Load(_R(IDS_FILLTOOL_LINEAR));
				Mode = GRADFILL;
				CurrentGeometryIndex = FGMENU_LINEAR;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrCircularColourFill))
			{
				Str.Load(_R(IDS_FILLTOOL_CIRCULAR));
				Mode = GRADFILL;
				CurrentGeometryIndex = FGMENU_CIRCULAR;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrRadialColourFill))
			{
				Str.Load(_R(IDS_FILLTOOL_ELLIPTICAL));
				Mode = GRADFILL;
				CurrentGeometryIndex = FGMENU_RADIAL;
    		}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrConicalColourFill))
			{
				Str.Load(_R(IDS_FILLTOOL_CONICAL));
				Mode = GRADFILL;
				CurrentGeometryIndex = FGMENU_CONICAL;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrSquareColourFill))
			{
				Str.Load(_R(IDS_FILLTOOL_SQUARE));
				Mode = GRADFILL;
				CurrentGeometryIndex = FGMENU_SQUARE;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrThreeColColourFill))
			{
				Str.Load(_R(IDS_FILLTOOL_THREECOL));
				Mode = GRADREPEATFILL;
				CurrentGeometryIndex = FGMENU_THREECOL;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrFourColColourFill))
			{
				Str.Load(_R(IDS_FILLTOOL_FOURCOL));
				Mode = GRADREPEATFILL;
				CurrentGeometryIndex = FGMENU_FOURCOL;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrBitmapColourFill))
			{
				Str.Load(_R(IDS_FILLTOOL_BITMAP));
				Mode = BITMAPFILL;
				CurrentGeometryIndex = FGMENU_BITMAP;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrFractalColourFill))
			{
				Str.Load(_R(IDS_FILLTOOL_FRACTAL));
				Mode = FRACTALFILL;
				CurrentGeometryIndex = FGMENU_FRACTAL;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrNoiseColourFill))
			{
				Str.Load(_R(IDS_FILLTOOL_NOISE));
				Mode = NOISEFILL;
				CurrentGeometryIndex = FGMENU_NOISE;
			}
		}
		else
		{
			if (CommonAttr == SelRange::ATTR_MANY)
			{
				// There are many different types of fill selected
				Str.Load(_R(IDS_FILLTOOL_MANY));
				Mode = MANYFILLS;
				CurrentGeometryIndex = FGMENU_MANY;
			}
		}
		
		if (pCurrentDoc)
			pCurrentDoc->SetCurrent();
	}

	// Update the Menus
	if (ShowDataAndNotSet)
	{
		SetGadgetString(_R(IDC_GEOMETRY),&Str);
	}
}



/********************************************************************************************

>	void GradInfoBarOp::ShowCommonMapping()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com> (altered by Chris Snook)
	Created:	22/8/96
	Inputs:		if ShowDataAndNotSet is FALSE, then data members will be set (for controlling
				menu selection logic) and data will not be shown in the relevant control.
	Purpose:	Show any Common Fill Mapping in the selection.

********************************************************************************************/

void GradInfoBarOp::ShowCommonMapping(BOOL ShowDataAndNotSet)
{
	String_64 Str;

	INT32 CommTess = FindCommonTesselate();

	switch (CommTess)
	{
		case -1:
			Str.Load(_R(IDS_FILLTOOL_MANY));
			CurrentMappingIndex = -2;//FMMENU_REPEATING;
			break;

		case 0:
			Str.Load(_R(IDS_FILLTOOL_NONE));
			CurrentMappingIndex = -1;//FMMENU_REPEATING;
			break;

		case 1:
			Str.Load(_R(IDS_FILLTOOL_MAPSIMPLE));
			CurrentMappingIndex = FMMENU_SIMPLE;
			break;

		case 2:
//Mark Howitt, 8/10/97. If were using the new grad fills and repeat is set to 2, this doesn`t mean repeating
//						grad fills. Make sure it only shows it as simple! 
#ifdef NEW_FEATURES
			if(Mode == GRADFILL)
			{
				Str.Load(_R(IDS_FILLTOOL_MAPSIMPLE));
				CurrentMappingIndex = FMMENU_SIMPLE;
				break;
			}
			else
			{
				Str.Load(_R(IDS_FILLTOOL_MAPREPEAT));
				CurrentMappingIndex = FMMENU_REPEATING;
				break;
			}
		case 4:		// Must be a repeating grad fill!
#endif
		case 3:
			Str.Load(_R(IDS_FILLTOOL_MAPREPEAT));
			CurrentMappingIndex = 2;//FMMENU_REPEATING;
			break;
	}

	if (ShowDataAndNotSet)
	{
		SetGadgetString(_R(IDC_MAPPING),&Str);
	}
}

/********************************************************************************************

>	void GradInfoBarOp::ShowCommonEffect()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (altered by Chris Snook)
	Created:	25/8/94
	Inputs:		if ShowDataAndNotSet is FALSE, then data members will be set (for controlling
				menu selection logic) and data will not be shown in the relevant control.
	Purpose:	Show any Common Fill Type in the selection.

********************************************************************************************/

void GradInfoBarOp::ShowCommonEffect(BOOL ShowDataAndNotSet)
{
	String_64 Str;

	CommonAttrEffect = SelRange::ATTR_NONE;
	Str.Load(_R(IDS_FILLTOOL_NONE));

	if (Selection != NULL)
	{
		// Try and find a common Attribute within the selection.
		// All objects in the selection must have an Identical fill for this
		// to find anything.
		CommonAttrEffect = Selection->FindCommonAttribute(CC_RUNTIME_CLASS(AttrFillEffect), 
													(NodeAttribute**)&CommonEffect);
		CCRuntimeClass* EffectType;

		// Try and find a common Attribute Type within the selection.
		// The fills need not be identical. Just of the same type.
		SelRange::CommonAttribResult CommonType = 
				Selection->FindCommonAttributeType(CC_RUNTIME_CLASS(AttrFillEffect), 
												    &EffectType);

		if (CommonAttrEffect == SelRange::ATTR_NONE &&
			CommonEffect != NULL)
		{
			CommonAttrEffect = SelRange::ATTR_COMMON;
		}

		if (CommonType == SelRange::ATTR_NONE &&
			EffectType != NULL)
		{
			CommonType = SelRange::ATTR_COMMON;
		}
    
		// Make the menu show any Common Fill Type
		// or failing that, 'None' or 'Many'.
		if (CommonType == SelRange::ATTR_COMMON)
		{	
		 	if (EffectType == CC_RUNTIME_CLASS(AttrFillEffectFade))
			{
				Str.Load(_R(IDS_FILLTOOL_FADE));
				CurrentEffectIndex = FEMENU_FADE;
			}
			else if (EffectType == CC_RUNTIME_CLASS(AttrFillEffectRainbow))
			{
				Str.Load(_R(IDS_FILLTOOL_RAINBOW));
				CurrentEffectIndex = FEMENU_RAINBOW;
			}
			else if (EffectType == CC_RUNTIME_CLASS(AttrFillEffectAltRainbow))
			{
				Str.Load(_R(IDS_FILLTOOL_ALTRAINBOW));
				CurrentEffectIndex = FEMENU_ALTRAINBOW;
			}
		}
		else
		{
			if (CommonType == SelRange::ATTR_MANY)
			{
				// There are many different types of fill selected
				Str.Load(_R(IDS_FILLTOOL_MANY));
				CurrentEffectIndex = -1;
			}
		}
	}

	// Update the Menus
	if (ShowDataAndNotSet)
	{
		SetGadgetString(_R(IDC_EFFECT),&Str);
	}
}


/********************************************************************************************

>	void GradInfoBarOp::ShowCommonTesselate()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Show any Common Fill Tesselation in the selection.

********************************************************************************************/

void GradInfoBarOp::ShowCommonTesselate()
{
	String_64 Str;

	INT32 CommTess = FindCommonTesselate();

	switch (CommTess)
	{
		case -1:
			Str.Load(_R(IDS_FILLTOOL_MANY));
			CurrentMappingIndex = FTMENU_REPEAT;
			break;

		case 0:
			Str.Load(_R(IDS_FILLTOOL_NONE));
			CurrentMappingIndex = FTMENU_REPEAT;
			break;

		case 1:
			Str.Load(_R(IDS_FILLTOOL_TESS_SIMPLE));
			CurrentMappingIndex = FTMENU_SIMPLE;
			break;

		case 2:
			Str.Load(_R(IDS_FILLTOOL_TESS_REPEAT));
			CurrentMappingIndex = FTMENU_REPEAT;
			break;

		case 3:
			Str.Load(_R(IDS_FILLTOOL_TESS_REPEATINV));
			CurrentMappingIndex = FTMENU_REPEATINV;
			break;
	}

	SetGadgetString(_R(IDC_MAPPING),&Str);
	EnableGadget(_R(IDC_MAPPING), TRUE);
}

/********************************************************************************************

>	INT32 GradInfoBarOp::FindCommonTesselate()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Find any Common Fill Tesselation in the selection.

********************************************************************************************/

INT32 GradInfoBarOp::FindCommonTesselate()
{
	INT32 CommonTess = 0;
	Node* pNode;
	NodeAttribute* pChild;

	if (Selection->Count() == 0)
	{
		AttrFillMapping* pCurrentMapping = GetCurrentMapping();

		if (pCurrentMapping != NULL)
		{
			return pCurrentMapping->GetRepeat();
		}

		return 0;
	}

	// Are there any selected Objects ?
	if (Selection != NULL)
	{
		// Find the first Object
		pNode = Selection->FindFirst();
		while (pNode != NULL)
		{
			BOOL FoundAttr = ((NodeRenderableInk*)pNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillMapping), &pChild);
			
			if (FoundAttr)
			{
				INT32 Tess = ((AttrFillMapping*)pChild)->GetRepeat();

				if (CommonTess == 0)
					CommonTess = Tess;

				if (Tess != CommonTess)
			   	{
					return -1; // Many
				}
			}

			pNode = Selection->FindNext(pNode);
		}
	}

	//if (CommonTess == 0)
	//	return RT_Repeating;

	return CommonTess;
}

/********************************************************************************************

>	void GradInfoBarOp::ShowCommonBitmapName()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Show any Common Bitmap Name in the selection.

********************************************************************************************/

void GradInfoBarOp::ShowCommonBitmapName()
{
	String_64 Str;

	Str = FindCommonBitmapName();
	// call custom controls interface ....
PORTNOTETRACE("other","GradInfoBarOp::InitBitmapName() - removed CustomComboBox handler");
#ifndef EXCLUDE_FROM_XARALX
	SelectCustomComboGadgetValueOnString(_R(IDC_BITMAPEFFECT), &Str);
#else
	SetGadgetString(_R(IDC_BITMAPEFFECT), &Str);
#endif
}

/********************************************************************************************

>	String_256 GradInfoBarOp::FindCommonBitmapName()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Find any Common Bitmap Name in the selection.

********************************************************************************************/

String_256 GradInfoBarOp::FindCommonBitmapName()
{
	String_256 CommonName;
	CommonName.Load(_R(IDS_FILLTOOL_NONE));

	String_256 Name;
	Name.Load(_R(IDS_FILLTOOL_NONE));
	
	String_256 None;
	None.Load(_R(IDS_FILLTOOL_NONE));

	String_256 Many;
	Many.Load(_R(IDS_FILLTOOL_MANY));

	if (Selection->Count() == 0)
	{
		AttrFillGeometry* pCurrentFill = GetCurrentGeometry();

		if (pCurrentFill && pCurrentFill->IsABitmapFill())
		{
			KernelBitmap* pBitmap = pCurrentFill->GetBitmap();
			if (pBitmap)
			{
				OILBitmap* pOilBitmap = pBitmap->ActualBitmap;
				if (pOilBitmap)
				{
					return pOilBitmap->GetName();
				}
			}
		}

		return None;
	}

	// Find the first Fill Attribute in the selection
	AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr();

	// Return if there aren't any
	if (pAttrNode == NULL)
		return None;

	while (pAttrNode != NULL)
	{
		if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrBitmapColourFill)))
		{
			Name = None;
			KernelBitmap* pBitmap = pAttrNode->GetBitmap();
			if (pBitmap)
			{
				OILBitmap* pOilBitmap = pBitmap->ActualBitmap;
				if (pOilBitmap)
				{
					Name = pOilBitmap->GetName();
				}
			}

			if (CommonName == None)
				CommonName = Name;

			if (Name != CommonName)
		   	{
				return Many;
			}
		}

		// Check the next fill
		pAttrNode = AttrFillGeometry::FindNextSelectedAttr();
	}

	return CommonName;
}

/********************************************************************************************

>	void GradInfoBarOp::ShowCommonBitmapDpi()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Show any Common Bitmap Dpi in the selection.

********************************************************************************************/

void GradInfoBarOp::ShowCommonBitmapDpi()
{
	INT32 dpi = FindCommonBitmapDpi();

	String_64 DpiStr;

	if (dpi == 0)
	{
		DpiStr.Load(_R(IDS_FILLTOOL_NODPI));
	}

	if (dpi == -1)
	{
		DpiStr.Load(_R(IDS_FILLTOOL_MANYDPI));
	}

	if (dpi > 0)
	{
		// use the conversion functions rather than trying to do it ourselves
		Convert::LongToString(dpi, &DpiStr);
		DpiStr += String_64(_R(IDS_FILLTOOL_DPI));
	}

	SetGadgetString(_R(IDC_SELPOINT),&DpiStr);
	SetGadgetWritable(_R(IDC_SELPOINT), TRUE);
}

/********************************************************************************************

>	void GradInfoBarOp::SetGadgetWritable(INT32 id, BOOL enable)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		id, the 'IDC_?' of the control.
				enable, TRUE to allow the control to be typed into. FALSE to make it
						read only.
	Purpose:	Sets the state of the 'Read Only' flag of an edit field or combo box.

********************************************************************************************/

void GradInfoBarOp::SetGadgetWritable(INT32 id, BOOL enable)
{
	PORTNOTETRACE("other","GradInfoBarOp::SetGadgetWritable - disabled");
#ifndef EXCLUDE_FROM_XARALX
	// Get the window handle of the gadget, from the gadget ID
	HWND gadget = ::GetDlgItem(WindowID, id);

	// See if it's got a child window (it may be a Combo Box)
	HWND hEdit = ::ChildWindowFromPoint(gadget, CPoint(1,1));

	if (hEdit)				// Was there a child window ?
		gadget = hEdit;		// Yes, so send the message to it

 	if (enable)
	{
		::SendMessage(gadget, EM_SETREADONLY, FALSE, 0);	// Clear the Read Only Flag
	}
	else
	{
		::SendMessage(gadget, EM_SETREADONLY, TRUE, 0);		// Set the Read Only Flag
	}
#endif
}

/********************************************************************************************

>	BOOL GradInfoBarOp::GetGadgetWritable(INT32 id)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/11/99
	Inputs:		id, the 'IDC_?' of the control.
	Returns:	TRUE if writable, FALSE otherwise
	Purpose:	Returns whether the control is writable (i.e.  read only, or not).
				void GradInfoBarOp::EnableControls() needs to make use of this to control
				its 'dynamic' toolbar correctly (both of which were also written by me).

********************************************************************************************/

BOOL GradInfoBarOp::GetGadgetWritable (INT32 id)
{
	PORTNOTETRACE("other","GradInfoBarOp::SetGadgetWritable - disabled");
#ifndef EXCLUDE_FROM_XARALX
	// Get the window handle of the gadget, from the gadget ID
	HWND gadget = ::GetDlgItem (WindowID, id);

	// See if it's got a child window (it may be a Combo Box)
	HWND hEdit = ::ChildWindowFromPoint (gadget, CPoint(1,1));

	if (hEdit)				// Was there a child window ?
		gadget = hEdit;		// Yes, so send the message to it

	if (GetWindowLong (gadget, GWL_STYLE) & ES_READONLY)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
#else
	return TRUE;
#endif
}

/********************************************************************************************

>	INT32 GradInfoBarOp::FindCommonBitmapDpi()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Find any Common Bitmap Dpi in the selection.

********************************************************************************************/

INT32 GradInfoBarOp::FindCommonBitmapDpi()
{
	if (Selection->Count() == 0)
	{
		AttrFillGeometry* pCurrentFill = GetCurrentGeometry();

		if (pCurrentFill && pCurrentFill->IsABitmapFill())
		{
			return pCurrentFill->GetDPI();
		}

		return 0;
	}

	// Find the first Fill Attribute in the selection
	AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr();

	// Return if there aren't any
	if (pAttrNode == NULL)
		return 0;

	INT32 CommonDpi = 0;
	INT32 Dpi = 0;

	while (pAttrNode != NULL)
	{
		if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrBitmapFill)))
		{
			if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrTextureColourFill)) ||
				pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrBitmapColourFill))  )
			{
				Dpi = pAttrNode->GetDPI();

				if (CommonDpi == 0)
					CommonDpi = Dpi;

				if (Dpi != CommonDpi)
			   	{
					return -1; // Many
				}
			}
		}

		// Check the next fill
		pAttrNode = AttrFillGeometry::FindNextSelectedAttr();
	}

	return CommonDpi;
}


/********************************************************************************************

>	void GradInfoBarOp::ShowCommonFractalGrain()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Show any Common Fractal Graininess in the selection.

********************************************************************************************/

void GradInfoBarOp::ShowCommonFractalGrain()
{
	double Grain = FindCommonFractalGrain();

	String_64 GrainStr;

	if (Grain == 0)
		GrainStr.Load(_R(IDS_FILLTOOL_NOGRAIN));
	if (Grain == -1)
		GrainStr.Load(_R(IDS_FILLTOOL_MANYGRAIN));
	if (Grain > 0)
		Convert::DoubleToString(Grain, &GrainStr, 1);

	SetGadgetString(_R(IDC_SELCOLOUR),&GrainStr);
	SetGadgetWritable(_R(IDC_SELCOLOUR), TRUE);
}

/********************************************************************************************

>	FIXED16 GradInfoBarOp::FindCommonFractalGrain()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Find any Common Fractal Graininess in the selection.

********************************************************************************************/

double GradInfoBarOp::FindCommonFractalGrain()
{
	if (Selection->Count() == 0)
	{
		AttrFillGeometry* pCurrentFill = GetCurrentGeometry();

		if (pCurrentFill && pCurrentFill->IsAFractalFill())
		{
			return ((FractalFillAttribute*)pCurrentFill->GetAttributeValue())->Graininess.MakeDouble();
		}

		return 0;
	}

	// Find the first Fill Attribute in the selection
	AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr();

	// Return if there aren't any
	if (pAttrNode == NULL)
		return 0;

	double CommonGrain = 0;

	while (pAttrNode != NULL)
	{
		if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrFractalColourFill)))
		{
			double Grain = ((FractalFillAttribute*)pAttrNode->GetAttributeValue())->Graininess.MakeDouble();

			if (CommonGrain == 0)
				CommonGrain = Grain;

			if (Grain != CommonGrain)
		   	{
				return -1; // Many
			}
		}

		// Check the next fill
		pAttrNode = AttrFillGeometry::FindNextSelectedAttr();
	}

	return CommonGrain;
}

/********************************************************************************************

>	void GradInfoBarOp::ShowCommonNoiseScale()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/97
	Purpose:	Show any Common Noise scale in the selection

********************************************************************************************/

void GradInfoBarOp::ShowCommonNoiseScale()
{
	double Scale = FindCommonNoiseScale();

	String_64 ScaleStr;

	if (Scale == 0)  
		ScaleStr.Load(_R(IDS_FILLTOOL_NOGRAIN));			// evaluates to 'None'
	if (Scale == -1) 
		ScaleStr.Load(_R(IDS_FILLTOOL_MANYGRAIN));			// evaluates to 'Many'
	if (Scale > 0)	 
		Convert::DoubleToString(Scale, &ScaleStr, 1);

	SetGadgetString(_R(IDC_SELCOLOUR),&ScaleStr);
	SetGadgetWritable(_R(IDC_SELCOLOUR), TRUE);
}

/********************************************************************************************

>	double GradInfoBarOp::FindCommonNoiseScale()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/97
	Purpose:	Find any common noise scale in the selection.

********************************************************************************************/

double GradInfoBarOp::FindCommonNoiseScale()
{
	if (Selection->Count() == 0)
	{
		AttrFillGeometry* pCurrentFill = GetCurrentGeometry();

		if (pCurrentFill && IS_A(pCurrentFill, AttrNoiseColourFill))
		{
			return ((NoiseFillAttribute*)pCurrentFill->GetAttributeValue())->GetGraininess().MakeDouble();
		}

		return 0;
	}

	// Find the first Fill Attribute in the selection
	AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr();

	// Return if there aren't any
	if (pAttrNode == NULL)
		return 0;

	double CommonScale = 0;

	while (pAttrNode != NULL)
	{
		if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrNoiseColourFill)))
		{
			double Scale = ((NoiseFillAttribute*)pAttrNode->GetAttributeValue())->GetGraininess().MakeDouble();

			if (CommonScale == 0)
				CommonScale = Scale;

			if (Scale != CommonScale)
		   	{
				return -1; // Many
			}
		}

		// Check the next fill
		pAttrNode = AttrFillGeometry::FindNextSelectedAttr();
	}

	return CommonScale;
}

/********************************************************************************************

>	void GradInfoBarOp::ShowControlPointInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Purpose:	Show Info about selected control points.

********************************************************************************************/

void GradInfoBarOp::ShowControlPointInfo()
{
	String_64 PointStr;
	String_64 ColourStr;

	// Look though the selection for any selected control points
	ScanSelectionForControlPoints(&PointStr, &ColourStr);

	// PointStr and ColourStr will be updated if we find any

	// Show the selected control points state
	SetStringGadgetValue(_R(IDC_SELPOINT), PointStr);
	SetStringGadgetValue(_R(IDC_SELCOLOUR), ColourStr);

	if ((Mode != BITMAPFILL && 
		 Mode != FRACTALFILL && 
		 Mode != NOISEFILL ) || AttrFillGeometry::SelectionCount > 0)
	{
		SetGadgetString(_R(IDC_SELPOINT),&PointStr);
	}

	SetGadgetString(_R(IDC_SELCOLOUR),&ColourStr);
}

/********************************************************************************************

>	void GradInfoBarOp::ScanSelectionForControlPoints(String_64* PointString, 
														String_64* ColourString)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Purpose:	Look for selected control points within the selection.

********************************************************************************************/

void GradInfoBarOp::ScanSelectionForControlPoints(String_64* PointString, 
												  String_64* ColourString)
{
	*PointString = _T("");
	*ColourString = _T("");

	AttrFillGeometry* pAttrNode;
	UINT32 count = Selection->Count();

	// Find the first Fill Attribute in the selection
	if (count == 0)
	{
		pAttrNode = GetCurrentGeometry();
	}
	else
	{
		pAttrNode = AttrFillGeometry::FindFirstSelectedAttr();
	}

	AllowForceToSimpleMapping = TRUE;
	
	// Return if there aren't any
	if (pAttrNode == NULL)
	{
		(*PointString).Load(_R(IDS_FILLTOOL_NONE));
		(*ColourString).Load(_R(IDS_FILLTOOL_NONE));
		return;
	}
	
	INT32 SelCount = 0;
	BOOL CommonColour = TRUE;
	BOOL CommonPoint  = TRUE;

	BOOL AllFlat = TRUE;
	BOOL AnyNeedSimple = FALSE;

	while (pAttrNode != NULL)
	{
		if (AttrFillGeometry::FillSelectionCount() > 0)
		{
			if (!(pAttrNode->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrFlatColourFill)))
			{
				// Get the selection state and colours of this fill
				String_64 PointStr = GetSelectedPoint(pAttrNode);
				String_64 ColourStr = GetSelectedColour(pAttrNode);

				String_64 Str;
				Str.Load(_R(IDS_FILLTOOL_NONE));

				// Where there any points selected ?
				if (PointStr != Str)
				{
					// Update and keep a count
					*PointString = PointStr;
					SelCount++;

					if (*PointString == String_64(_T("")))
						*PointString = PointStr;

					if (*PointString != PointStr)
						CommonPoint = FALSE;

					if (*ColourString == String_64(_T("")))
                    	*ColourString = ColourStr;

					if (*ColourString != ColourStr)
						CommonColour = FALSE;
	
					AllFlat = FALSE;
				}
			}
		}
		else 
		{
			if (pAttrNode->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrFlatColourFill))
			{
				if ( pAttrNode->GetStartColour() != NULL )
				{
					(*PointString).Load(_R(IDS_FILLTOOL_FLATFILL));

					if (*ColourString == String_64(_T("")))
		            	*ColourString = GetColourName(*pAttrNode->GetStartColour());

					if (*ColourString != GetColourName(*pAttrNode->GetStartColour()))
						CommonColour = FALSE;

					SelCount++;
				}
			}
			else
			{
				if (SelCount > 0)
					AllFlat = FALSE;
			}
		}

		// If this is a colour fill attribute and doesn't need simple mapping then set FALSE
		if (pAttrNode->IsAColourFill() && pAttrNode->NeedsForceToSimpleMapping())
			AnyNeedSimple = TRUE;
			
		// Check the next fill
		if (count > 0)
			pAttrNode = AttrFillGeometry::FindNextSelectedAttr();
		else
			pAttrNode = NULL;
	}

	if (AnyNeedSimple)
		AllowForceToSimpleMapping = FALSE;

	if (AttrFillGeometry::FillSelectionCount() == 0 && !AllFlat)
	{
		(*PointString).Load(_R(IDS_FILLTOOL_MANY));
	}

	if (SelCount == 0)
	{
		(*PointString).Load(_R(IDS_FILLTOOL_NONE));
		(*ColourString).Load(_R(IDS_FILLTOOL_NONE));
	}

	// Was there more than one fill with points selected ?
	if (!CommonPoint)
	{
		// Status indicating many points selected
		(*PointString).Load(_R(IDS_FILLTOOL_MANY));
	}

	if (!CommonColour)
	{
		(*ColourString).Load(_R(IDS_FILLTOOL_MANY));
	}

/*
	*PointString = "";
	*ColourString = "";

	// Find the first Fill Attribute in the selection
	AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr();

	// Return if there aren't any
	if (pAttrNode == NULL)
		return;
	
	INT32 SelCount = 0;
	BOOL CommonPoint = TRUE;
	BOOL CommonColour = TRUE;

	while (pAttrNode != NULL)
	{
		// Get the selection state and colours of this fill
		String_64 PointStr = GetSelectedPoint(pAttrNode);
		String_64 ColourStr = GetSelectedColour(pAttrNode);

		// Where there any points selected ?
		String_64 Str;
		Str.Load(_R(IDS_FILLTOOL_NONE));

		if (PointStr != Str)
		{
			if (*PointString == String_64(""))
			{
				// Update and keep a count
				*PointString = PointStr;
				*ColourString = ColourStr;
				SelCount++;
			}					
			else
			{
				if (CommonPoint && *PointString != PointStr)
				{
				 	CommonPoint = FALSE;
					(*PointString).Load(_R(IDS_FILLTOOL_MANY));
				}

				if (CommonColour && *ColourString != ColourStr)
				{
				 	CommonColour = FALSE;
					(*ColourString).Load(_R(IDS_FILLTOOL_MANY));
				}
			}
		
			if (!CommonPoint && !CommonColour)
				return;
		}

		// Check the next fill
		pAttrNode = AttrFillGeometry::FindNextSelectedAttr();
	}

	if (SelCount == 0)
	{
		(*PointString).Load(_R(IDS_FILLTOOL_NONE));
		(*ColourString).Load(_R(IDS_FILLTOOL_NONE));
	}
*/
}

/********************************************************************************************

>	String_64 GradInfoBarOp::GetSelectedPoint(AttrFillGeometry* pGeometry)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Outputs:	-
	Returns:	A String_64, "None", "Many", or "Start Colour", "End Colour" etc.
	Purpose:	Gets a textual description of a fills contol point selection state.

********************************************************************************************/

String_64 GradInfoBarOp::GetSelectedPoint(AttrFillGeometry* pGeometry)
{
	String_64 Str;

	// how many	happen to be selected at the moment?
	UINT32 SelCount = pGeometry->GetSelectionCount();

	// if there's nothing selected then we're done
	if (SelCount<1)
	{
		Str.Load(_R(IDS_FILLTOOL_NONE));
		return Str;
	}

	// more than one?	
	if (SelCount>1)
	{
		Str.Load(_R(IDS_FILLTOOL_MANY));
		return Str;
	}

	// ramp blobs have negative indexes
	INT32 i = pGeometry->GetFirstSelectedIndex();
	if (ISA_RAMPINDEX(i))
	{
		Str.Load(_R(IDS_FILLTOOL_RAMPCOL));
		return Str;
	}

	// ok its an end blob so which one is it?
	switch (i)
	{
	  	case FILLCONTROL_STARTPOINT:
			Str.Load(_R(IDS_FILLTOOL_STARTCOL));
			break;

		case FILLCONTROL_ENDPOINT:
	  	case FILLCONTROL_SECONDARYPOINT:
			Str.Load(_R(IDS_FILLTOOL_ENDCOL));
			break;

		case FILLCONTROL_ENDPOINT2:
			Str.Load(_R(IDS_FILLTOOL_ENDCOL2));
			break;

		case FILLCONTROL_ENDPOINT3:
			Str.Load(_R(IDS_FILLTOOL_ENDCOL3));
			break;
		
		default:
			Str.Load(_R(IDS_FILLTOOL_NONE));
			break;
	}

	return Str;
}

/*
String_64 GradInfoBarOp::GetSelectedPoint(AttrFillGeometry* pGeometry)
{
	BOOL SelState[5];
	String_64 Str;

	// Get the selection state of this fills control points
	pGeometry->GetSelectionState(SelState, 5);

	INT32 SelCount = 0;
	for (INT32 i=0; i<5; i++)
	{
		// Is this point selected ?
		if (SelState[i])
		{
			// Get a description of the Point
			switch (i)
			{
	  			case FILLCONTROL_STARTPOINT:
					Str.Load(_R(IDS_FILLTOOL_STARTCOL));
					break;

				case FILLCONTROL_ENDPOINT:
					Str.Load(_R(IDS_FILLTOOL_ENDCOL));
					break;

				case FILLCONTROL_ENDPOINT2:
					Str.Load(_R(IDS_FILLTOOL_ENDCOL2));
					break;

				case FILLCONTROL_ENDPOINT3:
					Str.Load(_R(IDS_FILLTOOL_ENDCOL3));
					break;

	  			case FILLCONTROL_SECONDARYPOINT:
					Str.Load(_R(IDS_FILLTOOL_ENDCOL));
					break;

			}
			// Keep a count of how many points were selected
			SelCount++;
		}
	}

	if (SelCount == 0)
		Str.Load(_R(IDS_FILLTOOL_NONE));	// No control points were selected

	if (SelCount > 1)
		Str.Load(_R(IDS_FILLTOOL_MANY));	// Many control points were selected

	return Str;
}
*/

/********************************************************************************************

>	String_64 GradInfoBarOp::GetSelectedColour(AttrFillGeometry* pGeometry)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Outputs:	-
	Returns:	A String_64, "None", "Many", or "Colour Name".
	Purpose:	Gets a textual description of a fills contol point colour.

********************************************************************************************/

String_64 GradInfoBarOp::GetSelectedColour(AttrFillGeometry* pGeometry)
{
	String_64 Str;

	// fill the string with nothing at the moment
	Str.Load(_R(IDS_FILLTOOL_NONE));
	// how many	happen to be selected at the moment?
	UINT32 SelCount = pGeometry->GetSelectionCount();
	
	// if there's nothing selected then we're done
	if (SelCount<1)
		return Str;

	// single selected end point?
	if (SelCount==1)
	{
		// then find the name of the colour
		DocColour *pColour = pGeometry->GetFirstSelectedColour();
		if (pColour)
			Str=GetColourName(*pColour);
		return Str;	
	}

	// more that one selected blob so go figure.
	Str.Load(_R(IDS_FILLTOOL_MANY));
	return Str;
}

/*
String_64 GradInfoBarOp::GetSelectedColour(AttrFillGeometry* pGeometry)
{
	BOOL SelState[5];
	String_64 Str;

	// Get the selection state of this fills control points
	pGeometry->GetSelectionState(SelState, 5);

	INT32 SelCount = 0;
	for (INT32 i=0; i<5; i++)
	{
		// Is this point selected ?
		if (SelState[i])
		{
			// Get a description of the Points Colour
			switch (i)
			{
	  			case FILLCONTROL_STARTPOINT:
					if (pGeometry->GetStartColour())
						Str = GetColourName(*(pGeometry->GetStartColour()));
					else
						Str.Load(_R(IDS_FILLTOOL_NONE));
					break;

				case FILLCONTROL_ENDPOINT:
	  			case FILLCONTROL_SECONDARYPOINT:
					if (pGeometry->GetEndColour())
						Str = GetColourName(*(pGeometry->GetEndColour()));
					else
						Str.Load(_R(IDS_FILLTOOL_NONE));
					break;

				case FILLCONTROL_ENDPOINT2:
					if (pGeometry->GetEndColour2())
						Str = GetColourName(*(pGeometry->GetEndColour2()));
					else
						Str.Load(_R(IDS_FILLTOOL_NONE));
					break;

				case FILLCONTROL_ENDPOINT3:
					if (pGeometry->GetEndColour3())
						Str = GetColourName(*(pGeometry->GetEndColour3()));
					else
						Str.Load(_R(IDS_FILLTOOL_NONE));
					break;
			}
			// Keep a count of how many points were selected
			SelCount++;
		}
	}

	if (SelCount == 0)
		Str.Load(_R(IDS_FILLTOOL_NONE));

	if (SelCount > 1)
		Str.Load(_R(IDS_FILLTOOL_MANY));

	return Str;
}
*/

/********************************************************************************************

>	String_64 GradInfoBarOp::GetColourName(DocColour Col)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Returns:	-
	Purpose:	Gets a name of a colour.
	SeeAlso:	-

********************************************************************************************/

String_64 GradInfoBarOp::GetColourName(DocColour &Col)
{
	String_64 Str;
	Str.Load(_R(IDS_FILLTOOL_NONE));

	IndexedColour* IndexCol = Col.FindParentIndexedColour();

	// Is this an Index Colour ?
	if (IndexCol != NULL)
	{
		// Yep, so get it's name
		Str = *(IndexCol->GetName());
	}
	else
	{
		// Check for a colour that we know about
		if (Col == COLOUR_NONE)
		{
			Str.Load(_R(IDS_FILLTOOL_NOCOLOUR));
		}
		else if (Col == COLOUR_WHITE)
		{
			Str.Load(_R(IDS_FILLTOOL_WHITE));
		}
		else if (Col == COLOUR_BLACK)
		{
			Str.Load(_R(IDS_FILLTOOL_BLACK));
		}
		else
		{
//			Str.Load(_R(IDS_FILLTOOL_LOCAL));
			Str.Load(_R(IDS_FILLTOOL_LOCALCOLOUR));
		}
	}

	return Str;
}

/********************************************************************************************

>	void GradInfoBarOp::EnableControls()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (rewritten by Chris Snook)
	Created:	3/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Enables or disables the controls depending on the current context, i.e.
				the controls are disabled if there is no selection, etc.
				Actually more complicated than this now, cause my function calls now hide/show
				various controls; and dynamically reshuffle the infobar.

********************************************************************************************/

void GradInfoBarOp::EnableControls()
{
	if (Document::GetSelected() == NULL)
	{	
		EnableGadget(_R(IDC_GEOMETRY),		FALSE);
		EnableGadget(_R(IDC_MAPPING),		FALSE);
		EnableGadget(_R(IDC_EFFECT),		FALSE);
		EnableGadget(_R(IDC_SELPOINT),		FALSE);
		EnableGadget(_R(IDC_BITMAPEFFECT),	FALSE);
		EnableGadget(_R(IDC_BIASGAIN),		FALSE);
		EnableGadget(_R(IDC_SELCOLOUR),		FALSE);
		
		//HideDisableGadgetAndResuffleBar (GetDlgItem ((HWND) GetReadWriteWindowID (), _R(IDC_BITMAPEFFECT)));
		//HideDisableGadgetAndResuffleBar (GetDlgItem ((HWND) GetReadWriteWindowID (), _R(IDC_BIASGAIN)));
		//HideDisableGadgetAndResuffleBar (GetDlgItem ((HWND) GetReadWriteWindowID (), _R(IDC_SELCOLOUR)));
	}
	else
	{
		EnableGadget (_R(IDC_GEOMETRY), TRUE);

		if (Mode != BITMAPFILL) { EnableGadget(_R(IDC_BITMAPEFFECT), FALSE); }
		
		if ((Mode != FRACTALFILL) && (Mode != NOISEFILL)) { EnableGadget(_R(IDC_SELCOLOUR),	FALSE); }

		// fill profile exceptions ....

		BOOL EnableBiasGain = TRUE;		// only for fills that have not been multistage!
		BOOL EnableBiasGain2 = FALSE;	// only for bitmap fills that are not contoned!
		BOOL EnableAffect = FALSE;		// only for bitmap fills that are not contoned!

		List FillList;
//		FillTools::BuildListOfSelectedNodes(&FillList, CC_RUNTIME_CLASS(AttrFillGeometry));
//		FillTools::BuildListOfSelectedAttrs(&FillList, CC_RUNTIME_CLASS(AttrFillGeometry));
		FillTools::GetSelectedAttrList(&FillList, CC_RUNTIME_CLASS(AttrFillGeometry), TRUE, FALSE);

		AttrFillGeometry* pNodeFill = NULL;

		if (!(FillList.IsEmpty()))
		{
			NodeListItem * pItem = (NodeListItem *)FillList.GetHead();

			while (pItem)
			{
				pNodeFill = (AttrFillGeometry*) pItem->pNode;

				// firstly check for multistage fills ....
				
				if (pNodeFill->GetFillRamp () != NULL)
				{
				//	if (pNodeFill->GetFillRamp ()->GetCount () > 0)
				//	{
						EnableBiasGain = FALSE;		// only for fills that have not been multistaged!
				//	}
				}

				// secondly check for a contoned bitmap fill
				
				if ((pNodeFill->IsABitmapFill ()) && (pNodeFill->GetStartColour () != NULL) && (pNodeFill->GetEndColour () != NULL))
				{
					EnableBiasGain2 = TRUE;
					EnableAffect = TRUE;
				}
			
				pItem = (NodeListItem *)FillList.GetNext(pItem);

				//if (EnableBiasGain == FALSE)
				//{
				//	pItem = NULL;				// breakout as soon as possible !!!!
				//}
			}

			FillList.DeleteAll();
		}
		
		switch (Mode)
		{	
			// I'm NOT sure if the NOFILL case is ever called for this tool (never fired off
			// the MessageBox during testing) - so for safety sake, we had best handle it ....
			case NOFILL:
				EnableGadget(_R(IDC_MAPPING),		FALSE);
				EnableGadget(_R(IDC_EFFECT),		FALSE);
				EnableGadget(_R(IDC_BIASGAIN),		FALSE);
				EnableGadget(_R(IDC_SELPOINT),		FALSE);
			break;
			
			case MANYFILLS:
			case FLATFILL:
				// implicitly handles (FGMENU_FLATTRANSP)
				// nothing else to enable ....
				EnableGadget (_R(IDC_MAPPING), FALSE);
				EnableGadget (_R(IDC_EFFECT), FALSE);
				EnableGadget (_R(IDC_BIASGAIN), FALSE);
				EnableGadget (_R(IDC_SELPOINT), FALSE);
				EnableGadget (_R(IDC_SELCOLOUR), FALSE);
				
				// CGS:  don't ask me why this different to the transparency infobar, BUT for somereason,
				// unless  I do the following test - the damn dialog is always closed!
				// NOTE:  this is the code that closes the profile dialog when you have it open for a single
				// fill; and then shift select a different fill ....
				if (Mode == MANYFILLS)
					CloseProfileDialog (m_BiasGainGadget);		// cause fill type cannot be profiled!
			break;

			case GRADFILL:
			case GRADREPEATFILL:
				switch (CurrentGeometryIndex)
				{
					case FGMENU_LINEAR:
					case FGMENU_CIRCULAR:
					case FGMENU_RADIAL:
					// the following is really fg_diamond ....
					case FGMENU_SQUARE:
						EnableGadget (_R(IDC_MAPPING), TRUE);
						// if (2 colour)
						EnableGadget (_R(IDC_BIASGAIN), EnableBiasGain);
						EnableGadget (_R(IDC_SELPOINT), FALSE);
						EnableGadget (_R(IDC_SELCOLOUR), FALSE);
					break;
					
					case FGMENU_CONICAL:
						EnableGadget (_R(IDC_MAPPING), FALSE);
						//if (2 colour)
						EnableGadget (_R(IDC_BIASGAIN), EnableBiasGain);
						EnableGadget (_R(IDC_SELPOINT), FALSE);
						EnableGadget (_R(IDC_SELCOLOUR), FALSE);
					break;
					
					case FGMENU_THREECOL:
					case FGMENU_FOURCOL:
						EnableGadget (_R(IDC_MAPPING), TRUE);
						EnableGadget (_R(IDC_EFFECT), FALSE);
						EnableGadget (_R(IDC_BIASGAIN), FALSE);
						EnableGadget (_R(IDC_SELPOINT), FALSE);
						EnableGadget (_R(IDC_SELCOLOUR), FALSE);
					break;

				}
			break;

			case BITMAPFILL:
				EnableGadget (_R(IDC_MAPPING), TRUE);
				EnableGadget (_R(IDC_EFFECT), EnableAffect);//FALSE);
				EnableGadget (_R(IDC_BITMAPEFFECT), TRUE);
				//if (2 colour)
				EnableGadget (_R(IDC_BIASGAIN), EnableBiasGain2);

				if (GetGadgetWritable (_R(IDC_SELPOINT))) { EnableGadget (_R(IDC_SELPOINT), FALSE); }
				else { EnableGadget (_R(IDC_SELPOINT), TRUE); }
				
				EnableGadget (_R(IDC_SELCOLOUR), FALSE);
			break;

			case FRACTALFILL:
			case NOISEFILL:
				EnableGadget (_R(IDC_MAPPING), TRUE);
				//if (2 colour)
				EnableGadget (_R(IDC_BIASGAIN), TRUE);

				if (GetGadgetWritable (_R(IDC_SELPOINT))) { EnableGadget (_R(IDC_SELPOINT), FALSE); }
				else { EnableGadget (_R(IDC_SELPOINT), TRUE); }
				
				if (GetGadgetWritable (_R(IDC_SELCOLOUR))) { EnableGadget (_R(IDC_SELCOLOUR), FALSE); }
				else
				{
					EnableGadget (_R(IDC_SELCOLOUR), TRUE);		// for selection bug !!!!

					EnableGadget (_R(IDC_SELCOLOUR), TRUE);
				}
			break;

			default:
			break;
		}

		if ((Mode != BITMAPFILL) && (Mode != NOFILL) && (Mode != MANYFILLS) && (Mode != FLATFILL)
			&& (CurrentGeometryIndex != FGMENU_THREECOL) && (CurrentGeometryIndex != FGMENU_FOURCOL))
		{
			EnableGadget (_R(IDC_EFFECT), TRUE);
		}
	}
}           

/********************************************************************************************

>	void GradInfoBarOp::ChangeFillType()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Purpose:	Called when a new fill type is selected from the menu.
	Notes:		NeedsForceToSimpleMapping added for new fill types (Gerry 22/8/96)

********************************************************************************************/

void GradInfoBarOp::ChangeFillType()
{
	// Get the current fill type, and pass a new fill of that type into
	// the attribute manager, as a mutator.
	AttrFillGeometry* Mutator = MakeFillMutator();

	if (Mutator == NULL)
	{
		InformError();
		return;
	}

	// If the mutator requires a simple mapping but the current fill doesn't
	// then we need to set ForceToSimple to TRUE
	// Need to check this before we select the attribute

	//BOOL ForceToSimple = Mutator->NeedsForceToSimpleMapping() && AllowForceToSimpleMapping;

	// This call deletes the Mutator so I'll set the pointer to NULL
	AttributeManager::AttributeSelected(NULL, Mutator);		// cause this gives us our undoable !!!!
	Mutator = NULL;

	//if (ForceToSimple)
	//{
//		CurrentMappingIndex = FMMENU_SIMPLE;
//		ChangeFillMapping ();					// we push two bits of undo here, but this is
												// unavoidable at present ....
	//}
}

/********************************************************************************************

>	void GradInfoBarOp::ChangeFillMapping()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/94
	Purpose:	Called when a new fill mapping or tesselation is selected from the menu.

********************************************************************************************/

void GradInfoBarOp::ChangeFillMapping()
{
	if (Mode != BITMAPFILL && 
		Mode != FRACTALFILL && 
		Mode != GRADREPEATFILL &&
// Mark Howitt, 6/10/97. Enable Gradual fill repeating/simple switching.
#ifdef NEW_FEATURES
		Mode != GRADFILL &&			// also include gradual fills
#endif
		Mode != NOISEFILL)
	{
		return;
	}

	INT32 Tesselation;

	switch (CurrentMappingIndex)
	{
	 	case (FTMENU_SIMPLE):
			Tesselation = 1;
			break;
   	
	 	case (FTMENU_REPEAT):
//Mark Howitt, 8/10/97. Check to see if gradfill is repeating and then set repeat value to 4 to show it`s special
#ifdef NEW_FEATURES
			if(Mode==GRADFILL)
			{
				Tesselation = 4;
			}
			else
#endif
			{
				Tesselation = 2;
			}
			break;

	 	case (FTMENU_REPEATINV):
			Tesselation = 3;
			break;

		default:
			Tesselation = 1;
			break;
	}

/*
	AttrBitmapTessChange* TessChange = new AttrBitmapTessChange;
	if (TessChange == NULL)
	{
		InformError();
		return;
	}

	TessChange->SetTesselation(Tesselation);

	AttributeManager::AttributeSelected(NULL, TessChange);
*/

	AttrFillMappingLinear* NewMapping = new AttrFillMappingLinear;
	if (NewMapping == NULL)
	{
		InformError();
		return;
	}

	NewMapping->SetRepeat(Tesselation);
	AttributeManager::AttributeSelected(NewMapping);
}

/********************************************************************************************

>	void GradInfoBarOp::ChangeFillEffect()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/94
	Purpose:	Called when a new fill effect is selected from the menu.

********************************************************************************************/

void GradInfoBarOp::ChangeFillEffect()
{
	AttrFillEffect* FillEffect = NULL;

	switch (CurrentEffectIndex)
	{
	 	case (FEMENU_FADE):
			FillEffect = new AttrFillEffectFade;
			break;
   	
	 	case (FEMENU_RAINBOW):
			FillEffect = new AttrFillEffectRainbow;
			break;

	 	case (FEMENU_ALTRAINBOW):
			FillEffect = new AttrFillEffectAltRainbow;
			break;

		default:
			FillEffect = new AttrFillEffectFade;
			break;
	}

	if (FillEffect == NULL)
	{
		InformError();
		return;
	}

	AttributeManager::AttributeSelected(FillEffect);
}

/********************************************************************************************

>	void GradInfoBarOp::ChangeBitmapName()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	Called when a new bitmap is selected from the menu.

********************************************************************************************/

void GradInfoBarOp::ChangeBitmapName()
{
	AttrValueChange* NewBitmap = new AttrBitmapChange;
	if (NewBitmap == NULL)
	{
		InformError();
		return;
	}

	Document* pDoc = Document::GetSelected();

	// Get the bitmap list
	BitmapList* Bitmaps = NULL;
	if (pDoc) Bitmaps = pDoc->GetBitmapList();

	if (Bitmaps == NULL)
		return;

	INT32 BmpIndex = -1;
	ListItem* pBmp = Bitmaps->GetHead();

	while (pBmp != NULL)
	{
		if (!((KernelBitmap*)pBmp)->HasBeenDeleted())
			BmpIndex++;

		if (BmpIndex == CurrentEffectIndex)
			break;

		pBmp = Bitmaps->GetNext(pBmp);
	}

	((AttrBitmapChange*)NewBitmap)->AttachBitmap((KernelBitmap*)pBmp);
	AttributeManager::AttributeSelected(NULL, NewBitmap);
}


/********************************************************************************************

>	void GradInfoBarOp::ChangeDPI()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/94
	Purpose:	Called when the dpi of a bitmap is changed by the user.

********************************************************************************************/

void GradInfoBarOp::ChangeDPI()
{
	BOOL Valid;
	String_256 Str = GetStringGadgetValue(_R(IDC_SELPOINT), &Valid);

	if (Valid)
	{
		TRACEUSER( "Will", _T("String = %s\n"),(TCHAR*)Str);

		INT32 Length = Str.Length();
		TCHAR* StrText = (TCHAR*)Str;

		INT32 FirstNum = -1;
		INT32 Count = 0;

		for (INT32 i = 0; i < Length; ++i)
		{
			TCHAR chr = StrText[i];
			if (FirstNum == -1 && StringBase::IsNumeric(chr))
				FirstNum = i;

			if (FirstNum != -1)
			{
				if (StringBase::IsNumeric(chr))
					Count++;
				else
					break;
			}
		}

		String_256 DpiStr;
		INT32 Dpi;

		if (FirstNum != -1 && Count > 0)
		{
			Str.Mid(&DpiStr, FirstNum, Count);
			Convert::StringToLong(DpiStr, &Dpi);
			TRACEUSER( "Will", _T("Dpi = %d\n"),Dpi);

			if (Dpi == 0)
				Dpi = 1;

			AttrValueChange* NewDpi = new AttrBitmapDpiChange;
			if (NewDpi == NULL)
			{
				InformError();
				return;
			}

			((AttrBitmapDpiChange*)NewDpi)->SetDPI(Dpi);

			AttributeManager::AttributeSelected(NULL, NewDpi);
		}
		else
		{
			TRACEUSER( "Will", _T("That was rubbish\n"));
			ShowCommonBitmapDpi();
		}
	}
	else
		TRACEUSER( "Will", _T("String was invalid\n"));
}

/********************************************************************************************

>	void GradInfoBarOp::ChangeFractalGrain()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Purpose:	Called when the Graininess of a fractal is changed by the user.

********************************************************************************************/

void GradInfoBarOp::ChangeFractalGrain()
{
	BOOL Valid;
	String_256 Str = GetStringGadgetValue(_R(IDC_SELCOLOUR), &Valid);

	if (Valid)
	{
		TRACEUSER( "Will", _T("String = %s\n"),(TCHAR*)Str);

		double Grain = 1.0;
		UnitType Units;

		if (Convert::StringToComponents(Str, &Grain, &Units))
		{
			TRACEUSER( "Will", _T("Graininess = %f\n"),Grain);

			if (Grain < 0.1)
				Grain = 0.1;

			if (Grain > 16384)
				Grain = 16384;

			AttrValueChange* NewGrain = new AttrFractalGrainChange;
			if (NewGrain == NULL)
			{
				InformError();
				return;
			}

			((FractalFillAttribute*)NewGrain->GetAttributeValue())->Graininess = FIXED16(Grain);

			AttributeManager::AttributeSelected(NULL, NewGrain);
		}
		else
		{
			TRACEUSER( "Will", _T("That was rubbish\n"));
			ShowCommonFractalGrain();
		}
	}
}

/********************************************************************************************

>	void GradInfoBarOp::ChangeNoiseScale()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/95
	Purpose:	Called when the Scale of a noise fill is changed by the user.

********************************************************************************************/

void GradInfoBarOp::ChangeNoiseScale()
{
	BOOL Valid;
	String_256 Str = GetStringGadgetValue(_R(IDC_SELCOLOUR), &Valid);

	if (Valid)
	{
		TRACEUSER( "Will", _T("String = %s\n"),(TCHAR*)Str);

		double Scale = 1.0;
		UnitType Units;

		if (Convert::StringToComponents(Str, &Scale, &Units))
		{
			TRACEUSER( "Mike", _T("Noise scale = %f\n"),Scale);

			if (Scale < 1)
				Scale = 1;

			if (Scale > 100)
				Scale = 100;

			AttrValueChange* NewScale = new AttrNoiseScaleChange;
			if (NewScale == NULL)
			{
				InformError();
				return;
			}

			FIXED16 Grain = FIXED16(Scale);
			((NoiseFillAttribute*)NewScale->GetAttributeValue())->SetGraininess(Grain);

			AttributeManager::AttributeSelected(NULL, NewScale);
		}
		else
		{
			TRACEUSER( "Will", _T("That was rubbish\n"));
			ShowCommonNoiseScale();
		}
	}
}

/********************************************************************************************

>	AttrFillGeometry* GradInfoBarOp::MakeFillMutator()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Purpose:	Make a fill that will be used to Mutate another fill, changing it's type 
				while retaining it's existing points and colours.
				The fill type returned depends on the fill type menu state.

********************************************************************************************/

AttrFillGeometry* GradInfoBarOp::MakeFillMutator()
{
	AttrFillGeometry* Fill = NULL;
	KernelBitmap* Default;

	switch (CurrentGeometryIndex)
	{
	 	case (FGMENU_FLAT):
			Fill = new AttrFlatColourFill;
			break;

	 	case (FGMENU_LINEAR):
			Fill = new AttrLinearColourFill;
			break;
   	
	 	case (FGMENU_CIRCULAR):
			Fill = new AttrRadialColourFill;
			if (Fill != NULL)
				((AttrRadialFill*)Fill)->MakeCircular();
			break;
   	
	 	case (FGMENU_RADIAL):
			Fill = new AttrRadialColourFill;
			if (Fill != NULL)
				((AttrRadialFill*)Fill)->MakeElliptical();
			break;
   	
	 	case (FGMENU_CONICAL):
			Fill = new AttrConicalColourFill;
			break;
   	
	 	case (FGMENU_SQUARE):
			Fill = new AttrSquareColourFill;
			break;
   	
	 	case (FGMENU_THREECOL):
			Fill = new AttrThreeColColourFill;
			CloseProfileDialog (m_BiasGainGadget);		// cause fill type cannot be profiled!
			break;
   	
	 	case (FGMENU_FOURCOL):
			Fill = new AttrFourColColourFill;
			CloseProfileDialog (m_BiasGainGadget);		// cause fill type cannot be profiled!
			break;
   	
	 	case (FGMENU_BITMAP):
			Fill = new AttrBitmapColourFill;
			CloseProfileDialog (m_BiasGainGadget);		// cause fill type cannot be profiled!

			Default = KernelBitmap::MakeKernelBitmap();
			if (Default == NULL)
			{
				delete Fill;
				return NULL;
			}

			Fill->AttachBitmap(Default);
			break;

	 	case (FGMENU_FRACTAL):
			Fill = new AttrFractalColourFill;
			break;

		case (FGMENU_NOISE):
			Fill = new AttrNoiseColourFill;
			break;

		default:
			// Er .. Dunno what kind of fill this is ?
			Fill = new AttrFlatColourFill;
			break;
	}

	return Fill;
}

/********************************************************************************************

>	AttrFillGeometry* GradInfoBarOp::GetCurrentGeometry()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Purpose:	Gets the current Fill Geometry from the Attribute manager.

********************************************************************************************/

AttrFillGeometry* GradInfoBarOp::GetCurrentGeometry()
{
	CCRuntimeClass* CurrentAttribGroup = 
 		Tool::GetCurrent()->Parent->m_ToolInfo.CurrentAttributeGroup;

	Document* CurrentDoc = Document::GetSelected();

	if (CurrentDoc == NULL)
		return NULL; // We are not going to be able to do anything if there is no document

	return (AttrFillGeometry*)CurrentDoc->GetAttributeMgr()
								.GetCurrentAttribute(CurrentAttribGroup, 
											CC_RUNTIME_CLASS(AttrFillGeometry));
}


/********************************************************************************************

>	AttrFillMapping* GradInfoBarOp::GetCurrentMapping()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Purpose:	Gets the current Fill Geometry from the Attribute manager.

********************************************************************************************/

AttrFillMapping* GradInfoBarOp::GetCurrentMapping()
{
	CCRuntimeClass* CurrentAttribGroup = 
 		Tool::GetCurrent()->Parent->m_ToolInfo.CurrentAttributeGroup;

	Document* CurrentDoc = Document::GetSelected();

	if (CurrentDoc == NULL)
		return NULL; // We are not going to be able to do anything if there is no document

	return (AttrFillMapping*)CurrentDoc->GetAttributeMgr()
								.GetCurrentAttribute(CurrentAttribGroup, 
											CC_RUNTIME_CLASS(AttrFillMapping));
}


















/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//			Implementation of class TranspInfoBarOp (transparency tools infobar)

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************

>	struct TGMItem 

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/06/97
	Purpose:	Associate the TransparencyGeometry enum with the resource id of the text that
				gets displayed on the menu.
	SeeAlso:	FGMItem, TransparencyGeometry enum

********************************************************************************************/

struct TGMItem
{
	TransparencyGeometry Geometry;
	unsigned StringID;
};

/********************************************************************************************

>	const TGMItem TransparencyGeometryMenu[] 

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/06/97
	Purpose:	Array of 'TGMItem's that make up the geometry menu. We need to keep track of
				this because list combo boxes can only return the position of the selected
				item, not what itd actually is. All this fuctionality should be in the gadgets
				not here.
	SeeAlso:	TransparencyGeometry enum, FillGeometryMenu

********************************************************************************************/

const TGMItem TransparencyGeometryMenu[] =
{
	{ FGMENU_NOTRANSP,			_R(IDS_FILLTOOL_NOTRANSP) },
	{ FGMENU_FLATTRANSP,		_R(IDS_FILLTOOL_FLATTRANSP) },
	{ FGMENU_LINEARTRANSP,		_R(IDS_FILLTOOL_LINEAR) },
	{ FGMENU_CIRCULARTRANSP,	_R(IDS_FILLTOOL_CIRCULAR) },
	{ FGMENU_RADIALTRANSP,		_R(IDS_FILLTOOL_ELLIPTICAL) },
#ifndef WEBSTER
	{ FGMENU_CONICALTRANSP,		_R(IDS_FILLTOOL_CONICAL) },
	{ FGMENU_SQUARETRANSP,		_R(IDS_FILLTOOL_SQUARE) },
	{ FGMENU_THREECOLTRANSP,	_R(IDS_FILLTOOL_THREECOLTRANS) },
	{ FGMENU_FOURCOLTRANSP,		_R(IDS_FILLTOOL_FOURCOLTRANS) },
#endif //WEBSTER
	{ FGMENU_BITMAPTRANSP,		_R(IDS_FILLTOOL_TEXTURE) },
	{ FGMENU_FRACTALTRANSP,		_R(IDS_FILLTOOL_FRACTAL) },
#ifndef WEBSTER
	{ FGMENU_NOISETRANSP,		_R(IDS_FILLTOOL_NOISE) }
#endif //WEBSTER
};

/********************************************************************************************

>	const INT32 TransparencyGeometryMenuCount 

	Author:		Martin_Bell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/06/97
	Purpose:	Number of items on the TransparencyGeometryMenu menu.
	SeeAlso:	TransparencyGeometryMenu

********************************************************************************************/

const INT32 TransparencyGeometryMenuCount = sizeof(TransparencyGeometryMenu) / sizeof(TransparencyGeometryMenu[0]);

/********************************************************************************************

>	MsgResult TranspInfoBarOp::Message(Msg* Message) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Inputs:		Message: The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Transpfill info bar dialog message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult TranspInfoBarOp::Message(Msg* Message) 
{
	if (!TranspTool::IsCurrentTool() || !IsVisible())
    	return (InformationBarOp::Message(Message));

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		// Check if the message is a CANCEL
		if (Msg->DlgMsg == DIM_CANCEL)
		{
			Close(); // Close the dialog 
		}
		else if (Msg->DlgMsg == DIM_CREATE)
		{
			// Initialise the controls 
			SliderDragged = FALSE;
			InitControls();
		}
		else
		{
			if (Msg->GadgetID == _R(IDC_GEOMETRY))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED :
					{
						ShowCommonType (FALSE);		// specifying false means that this
													// gets (but does not show) CurrentGeometryIndex
						
						// Someone selected a new Fill Geometry
						INT32 Index;
						GetValueIndex(_R(IDC_GEOMETRY),&Index);

						// pointless doing all that work if the user has selected
						// the currently applied transparency geometry ....

						if (CurrentGeometryIndex != Index)
						{
							// Change the Fill Geometry Here
							CurrentGeometryIndex = TransparencyGeometryMenu[Index].Geometry;
							ChangeFillType();
							EnableControls();
						}
					}
					break;

					default:
						break;
				}
			}

			else if (Msg->GadgetID == _R(IDC_TRANSPTYPE))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED:
					{
						ShowCommonTranspType (FALSE);		// specifying false means that this
															// gets (but does not show) CurrentTransTypeIndex
						
						// Someone selected a new Fill Mapping
						INT32 Index;  
						GetValueIndex (_R(IDC_TRANSPTYPE), &Index);

						if (CurrentTransTypeIndex != Index)
						{
							if (Index >= 0 && Index < FTMENU_MAX)
							{
								// Change the Fill Mapping Here
								CurrentTransTypeIndex = Index;
								EnableControls ();
								ChangeTranspType ();
							}
							else
							{
								ShowInfo();
							}
						}
					}
					break;

					default:
						break;
				}
			}

			else if (Msg->GadgetID == _R(IDC_MAPPING))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED:
					{
						ShowCommonMapping (FALSE);	// specifying false means that this
													// gets (but does not show) CurrentMappingIndex
						
						// Someone selected a new Fill Mapping
						INT32 Index;  
						GetValueIndex(_R(IDC_MAPPING),&Index);

						if (CurrentMappingIndex != Index)
						{
							// Change the Fill Mapping Here
							CurrentMappingIndex = Index;
							ChangeFillMapping();
						}
					}
					break;

					default:
						break;
				}
			}

			else if (Msg->GadgetID == _R(IDC_BITMAPEFFECT))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED:
					{
						// Someone selected a new bitmap Fill Effect
						INT32 Index;
						GetValueIndex (_R(IDC_BITMAPEFFECT), &Index);

						//if (CurrentBitmapIndex != Index)
						//{
							// Change the bitmap Fill Effect Here
							INT32 OldBitmapIndex = CurrentBitmapIndex;
						
							CurrentBitmapIndex = Index;
							
							if (!ChangeBitmapName ())
							{
								CurrentBitmapIndex = OldBitmapIndex;
								ShowInfo();
							}
						//}
					}
					break;											       

					default:
						break;
				}
			}

			else if (Msg->GadgetID == _R(IDC_BIASGAIN))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_LFT_BN_CLICKED:
						HandleProfileButtonClick (m_BiasGainGadget, _R(IDC_BIASGAIN));
						break;

					default:
						ProfileSelectionChange( Msg, Msg->GadgetID );
						break;
				}
			}

			else if (Msg->GadgetID == _R(IDC_SELPOINT))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED:
					{
						if (AttrFillGeometry::SelectionCount == 0)
						{
							if (Mode == BITMAPFILL || 
								Mode == FRACTALFILL ||
								Mode == NOISEFILL
								)
							{
								ChangeDPI();
							}
						}
					}
					break;											       

					default:
						break;
				}
			}

			else if (Msg->GadgetID == _R(IDC_SELCOLOUR))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SELECTION_CHANGED:
					{
						//if (AttrFillGeometry::SelectionCount == 0)
						//{
						//	if (Mode == BITMAPFILL || 
						//		Mode == FRACTALFILL || 
						//		Mode == NOISEFILL ) 
						//	{
							//	ChangeTransparencyValue();
						//	}
						//}

						if (Mode==FRACTALFILL && AttrFillGeometry::SelectionCount==0)
						{
							ChangeFractalGrain (TRUE);
						}
						else if (Mode==NOISEFILL && AttrFillGeometry::SelectionCount==0)
						{
							ChangeNoiseScale (TRUE);
						}
						else
						{
							ChangeTransparencyValue ();
						}
					}
					break;

					default:
						break;
				}
			}

			else if (Msg->GadgetID == _R(IDC_TRANSPSLIDE))
			{
				switch (Msg->DlgMsg)
				{
					case DIM_SLIDER_POS_CHANGING:
					{
						GetApplication ()->LockOutControlHelp ();
						// Find the current scrollers position
						TCHAR Str[32];
						BOOL Valid;
						INT32 Result = GetLongGadgetValue(_R(IDC_TRANSPSLIDE), 0, SliderMax, 0, &Valid);

						if (LastSliderPos == Result)
							break;

						double Grain = 1.0;
						BOOL flag = TRUE;

						if (AttrFillGeometry::SelectionCount == 0)
						{
							if (Mode == FRACTALFILL)
							{
								Grain = GetDoubleGrain(Result);	
								flag = FALSE;
							}
							if (Mode == NOISEFILL)
							{
								Grain = GetDoubleScale(Result);
								flag = FALSE;
							}

							if (!flag)
							{
								if (Mode == FRACTALFILL)
								{
									camSnprintf(Str, 31, _T("%.1f"), Grain);
								}
							}
						}

						if (flag)
						{
							double Transp = (double) Result;
							//Result = ((SliderMax - Result) * 100) / SliderMax;

							// Build the Percentage string and set it
							//String_32 jcf(_R(IDS_PERCENT_FORMAT));
							//wsprintf(Str, jcf, Result);

							DisplayTransparencyPercentString ((double) Transp);	// and display it

							if (GetApplication()->FindSelection()->Count() <= 64)
							{
								// Don't let transp get to Zero, or the 'Repeat'
								// op will get confused
								// if (Transp == 0) Transp = 1;

								AttributeManager::SendMessages = FALSE;
								SetTranspValue(static_cast<UINT32>(Transp), TRUE, !SliderDragged);
							}
						}
						else
						{
							if (Mode == FRACTALFILL)
							{
								String_32 PercentStr(Str);
								SetStringGadgetValue(_R(IDC_SELCOLOUR), PercentStr);
							}
							else	// MUST be NOISEFILL
							{
								String_64 Str;

								Convert::DoubleToString( Grain, &Str, 1 );
		
								Str.MakePercent ((TCHAR*) Str);

								String_64 PercentStr(Str);
								SetGadgetString(_R(IDC_SELCOLOUR), &PercentStr);
							}
						}

						LastSliderPos = Result;

						SliderDragged = TRUE;
					}
					break;

					case DIM_SLIDER_POS_IDLE:
					{
						// Slider control generates an "IDLE" event after processing
						// a run of WM_MOUSEWHEEL messages... Oh dear...
						//if (SliderDragged)
						{
							GetApplication ()->LockOutControlHelp ();
							// Find the current scrollers position
							TCHAR Str[32];
							BOOL Valid;
							INT32 Result = GetLongGadgetValue(_R(IDC_TRANSPSLIDE), 0, SliderMax, 0, &Valid);
							// if (LastSliderPos == Result)
							//	break;

							LastSliderPos = Result;
						
							double val = 1.0;
							BOOL flag = TRUE;

							if (AttrFillGeometry::SelectionCount == 0)
							{
								if (Mode == FRACTALFILL)
								{
									val = GetDoubleGrain(Result);	
									flag = FALSE;
								}
								if (Mode == NOISEFILL)
								{
									val = GetDoubleScale(Result);
									flag = FALSE;
								}

								if (!flag)
								{
									if (Mode == FRACTALFILL)
									{
										camSnprintf(Str, 31, _T("%.1f"), val);
									}
								}
							}
							
							if (flag)
							{
								double Transp = (double) Result;
								//Result = ((SliderMax - Result) * 100) / SliderMax;

								// Build the Percentage string and set it
								//String_32 jcf(_R(IDS_PERCENT_FORMAT));
								//wsprintf(Str, jcf, Result);

								DisplayTransparencyPercentString ((double) Transp);	// and display it

								if (GetApplication()->FindSelection()->Count() <= 64)
								{
									// Don't let transp get to Zero, or the 'Repeat'
									// op will get confused
									// if (Transp == 0) Transp = 1;

									AttributeManager::SendMessages = FALSE;
									// SetTranspValue(static_cast<UINT32>(Transp), TRUE);
									// Treat Idle events as SET events because of the way the damn Slider control handles mouse wheel events...
									SetTranspValue(static_cast<UINT32>(Transp));
									SliderDragged = FALSE;
								}
							}
							else
							{
								if (Mode == FRACTALFILL)
								{
									String_32 PercentStr(Str);
									SetStringGadgetValue(_R(IDC_SELCOLOUR), PercentStr);
								}
								else	// MUST be NOISEFILL
								{
									String_64 Str;

									Convert::DoubleToString( val, &Str, 1 );
		
									Str.MakePercent ((TCHAR*) Str);

									String_64 PercentStr(Str);
									SetGadgetString(_R(IDC_SELCOLOUR), &PercentStr);
								}
							}
						}
					}
					break;

					case DIM_SLIDER_POS_SET:
					{
						// Find the current scrollers position
						BOOL Valid;
						INT32 Result = GetLongGadgetValue(_R(IDC_TRANSPSLIDE), 0, SliderMax, 0, &Valid);
						LastSliderPos = Result;

						Operation::SetQuickRender(FALSE);

						// Tell the freehand tool about the new smoothness
						if (Valid)
						{
							AttributeManager::SendMessages = TRUE;

							if (Mode==FRACTALFILL && AttrFillGeometry::SelectionCount==0)
								ChangeFractalGrain();
							else if (Mode==NOISEFILL && AttrFillGeometry::SelectionCount==0)
								ChangeNoiseScale();
							else
								SetTranspValue(Result);

							GetApplication ()->UnlockControlHelp ();
						}

						SliderDragged = FALSE;
						break;
					}

					default:
						break;
				}
			}
	
		}
	}

	if (MESSAGE_IS_A(Message, SelChangingMsg) ||
		MESSAGE_IS_A(Message, CurrentAttrChangedMsg))
	{
		// The selection has changed in some way

		// Re-Count the number of selected fill control points
		AttrFillGeometry::SetSelectionCount(AttrFillGeometry::CountSelectionControlPoints());

		if (AttrFillGeometry::SelectionCount > 0)
		{
			TranspTool::EnableFillNudge();
		}
		else
		{
			TranspTool::DisableFillNudge();
		}

		HandleProfileSelChangingMsg (m_BiasGainGadget, _R(IDC_BIASGAIN));

		ShowInfo();
/*
		SelChangingMsg* Msg = (SelChangingMsg*)Message;
		switch (Msg->State)
		{
		 	case SelChangingMsg::COLOURATTCHANGED:
				// The colour of an attribute has changed
				InitControls();
				break;

		 	case SelChangingMsg::NONCOLOURATTCHANGED:
				// The selection state or control points of an attribute have changed
				InitControls();
				break;
		}
*/
	}
	
	// Pass the message on
	return (InformationBarOp::Message(Message));
}


/********************************************************************************************

>	void TranspInfoBarOp::ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Purpose:	See InformationBarOp::ChangeProfile () for an explanation of this function.
	See Also:	InformationBarOp::ChangeProfile ()

*********************************************************************************************/

void TranspInfoBarOp::ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID)
{
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_TRANSPFILLPROFILE);
	if (pOpDesc != NULL)
	{
		BOOL FireOp = TRUE;

		// we only want to generate one bit of undo information - so decided whether
		// we have to fire the above op, or whether we just 'pump' the values into
		// our nodes (thereby nolonger generating infinite undo information) ....

		Operation* pLastOp = NULL;

		if (Profile->GetGeneratesInfiniteUndo ())	// only do if they didn't select a preset profile
		{
			pLastOp = Document::GetSelected()->GetOpHistory().FindLastOp();
		}

		if (pLastOp)
		{
			if (pLastOp->GetRuntimeClass() == CC_RUNTIME_CLASS(OpChangeTranspFillProfile))
			{
				FireOp = FALSE;
			}
		}

		if (FireOp == TRUE)
		{
			TranspFillProfileOpParam Param;
			Param.Profile = *Profile;
		
			pOpDesc->Invoke(&Param);
		}
		else
		{
			// we don't need/want any undo information - so just change the value ....
			
			//OpChangeFillProfile Op;
			ChangeTranspFillProfileAction Action;
			Action.ChangeTranspFillProfileWithNoUndo (/*&Op,*/ *Profile);
		}
		
		/*AFp BiasValue = Profile->GetBias();
		AFp GainValue = Profile->GetGain();
		
		TranspFillProfileOpParam Param;
		Param.Profile.SetBias(BiasValue);
		Param.Profile.SetGain(GainValue);
		
		pOpDesc->Invoke(&Param);*/
	}
}



/********************************************************************************************

>	virtual CProfileBiasGain* GetProfileFromSelection(CGadgetID GadgetID, INT32* Index, BOOL* bAllSameType)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Inputs:		The GadgetID of the CBiasGainGadget that we are dealing with.
	Outputs:	bMany - returned as TRUE if we have MANY profiles selected.
				bAllSameType - returned as TRUE if objects within selection are all of the
				same type.
	returns		Ptr to common CProfileBiasGain, or NULL if there is NOT one.
	Purpose:	See InformationBarOp::GetProfileFromSelection () for a description of this
				function.

*********************************************************************************************/

CProfileBiasGain* TranspInfoBarOp::GetProfileFromSelection(CGadgetID GadgetID, BOOL* bMany, BOOL* bAllSameType)
{
	BOOL ok = (GadgetID == _R(IDC_BIASGAIN));

	ERROR2IF(ok==FALSE, FALSE, "Invalid gadgetID passed");
	
	UINT32 TotalNumberSelected = (GetApplication()->FindSelection()->Count ());

	// get the list of all the shadows
//	List ShadowList;
//	FillTools::BuildListOfSelectedAttrs(&ShadowList, CC_RUNTIME_CLASS(AttrTranspFillGeometry));
	List AttrList;
	ok = FillTools::GetSelectedAttrList(&AttrList, CC_RUNTIME_CLASS(AttrTranspFillGeometry), TRUE, TRUE);
	if (!ok)
		return NULL;

	AttrFillGeometry* pFirstNodeShadow = NULL;
	FillGeometryAttribute* pFirstFillGeoAttr = NULL;

	CProfileBiasGain* pFirstProfile = NULL;

//	NodeListItem * pItem = (NodeListItem *)ShadowList.GetHead();
	NodeListItem * pItem = (NodeListItem *)AttrList.GetHead();

	while (pItem)
	{
		if (pFirstNodeShadow == NULL)
		{
			pFirstNodeShadow = (AttrFillGeometry*) pItem->pNode;
			pFirstFillGeoAttr = (FillGeometryAttribute*) pFirstNodeShadow->GetAttributeValue ();

			pFirstProfile = pFirstFillGeoAttr->GetProfilePtr ();
		}
		else
		{		
			CProfileBiasGain* pOtherProfile = NULL;
			
			pOtherProfile = ((FillGeometryAttribute*) ((AttrFillGeometry*)pItem->pNode)->GetAttributeValue ())->GetProfilePtr ();

			if (pOtherProfile)
			{
				if (*pFirstProfile == *pOtherProfile)
				{
					// all ok
				}
				else
				{
					*bMany = TRUE;
				}
			}
		}

//		pItem = (NodeListItem *)ShadowList.GetNext(pItem);
		pItem = (NodeListItem *)AttrList.GetNext(pItem);
	}

//	if (TotalNumberSelected != (UINT32) ShadowList.GetCount ())
	if (TotalNumberSelected != (UINT32) AttrList.GetCount ())
	{
		// totals differ - so the user MUST have selected someother type of node as well
		
		if ((pFirstProfile) && (*bMany == FALSE))
		{
			// scan each node in the list, and see if they have the attribute applied ....

			SelRange* range = GetApplication()->FindSelection();

			NodeRenderableInk* pNode = (NodeRenderableInk*) range->FindFirst();
			NodeAttribute* pAttr;

			while (pNode)
			{
//				if (pNode->FindAppliedAttribute (CC_RUNTIME_CLASS (AttrTranspFillGeometry), &pAttr))
				pAttr = pNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry));
				if (pAttr)
				{
					pNode = (NodeRenderableInk*) (range->FindNext(pNode));
				}
				else
				{
					// the node ain't got one

					*bAllSameType = FALSE;
//					ShadowList.DeleteAll();
					AttrList.DeleteAll();
					return (NULL);
				}
			}
		}
		else
		{	
			*bAllSameType = FALSE;
//			ShadowList.DeleteAll();
			AttrList.DeleteAll();
			return (NULL);
		}
	}
	
	// I've assumed that m_BiasGainGadget.UseTranspFillProfile () has already been called ....
	// (which in this code it will have been).
	
	if (*bMany == TRUE)
	{
//		ShadowList.DeleteAll();
		AttrList.DeleteAll();
		return (NULL);
	}
	else
	{
		// NOTE:  we can only be 'context' sensitive for a single fill within the
		// selection - so lets just choose the first one that we came accross ....
		
		if (pFirstNodeShadow)
		{
			m_BiasGainGadget.SetStartTransp (pFirstNodeShadow->GetStartTransp ());
			m_BiasGainGadget.SetEndTransp (pFirstNodeShadow->GetEndTransp ());
		}

//		ShadowList.DeleteAll();
		AttrList.DeleteAll();

		return (pFirstProfile);
	}
}

/********************************************************************************************

>	void TranspInfoBarOp::InitControls()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Purpose:	Inits all the controls in the info bar.
				Called immediately after the bar is created when the Transpfill tool
				becomes the current tool.
	SeeAlso:	-
	Note:		WEBSTER-Martin-26/11/86 removed initiallization of things not in XWP

********************************************************************************************/

void TranspInfoBarOp::InitControls()
{
	// Don't do anything if the Info bar isn't displayed
	if (!TranspTool::IsCurrentTool() || !IsVisible()) return;

//	if (SliderDragged)
//		return;

	SetGadgetRange(_R(IDC_TRANSPSLIDE), 0, SliderMax);

	DeleteAllValues(_R(IDC_GEOMETRY));
	DeleteAllValues(_R(IDC_TRANSPTYPE));
	DeleteAllValues(_R(IDC_MAPPING));
	DeleteAllValues(_R(IDC_BITMAPEFFECT));

	SetGadgetHelp(_R(IDC_GEOMETRY), 	_R(IDBBL_TRANSPTOOL_TRANSPSHAPE), 	_R(IDS_TRANSPTOOL_TRANSPSHAPE));
	SetGadgetHelp(_R(IDC_TRANSPTYPE), 	_R(IDBBL_TRANSPTOOL_TRANSPTYPE), 	_R(IDS_TRANSPTOOL_TRANSPTYPE));
	SetGadgetHelp(_R(IDC_MAPPING), 	 	_R(IDBBL_TRANSPTOOL_TRANSPTILING), 	_R(IDS_TRANSPTOOL_TRANSPTILING));
	SetGadgetHelp(_R(IDC_BITMAPEFFECT), _R(IDBBL_FILLTOOL_BITMAPNAME),		_R(IDS_FILLTOOL_BITMAPNAME));
	SetGadgetHelp(_R(IDC_SELPOINT),  	_R(IDBBL_TRANSPTOOL_TRANSPHANDLE),  _R(IDS_TRANSPTOOL_TRANSPHANDLE));
	SetGadgetHelp(_R(IDC_TRANSPSLIDE), 	_R(IDBBL_TRANSPTOOL_TRANSP), 		_R(IDS_TRANSPTOOL_TRANSP));
	SetGadgetHelp(_R(IDC_SELCOLOUR), 	_R(IDBBL_TRANSPTOOL_TRANSP), 		_R(IDS_TRANSPTOOL_TRANSP));

	String_64 Str;

	// Setup the Menu Item text entries
	for ( INT32 i=0; i<TransparencyGeometryMenuCount; i++ )
	{
		Str.Load( TransparencyGeometryMenu[i].StringID );
		SetStringGadgetValue( _R(IDC_GEOMETRY), Str, TRUE, TransparencyGeometryMenu[i].Geometry );
	}

	Str.Load(_R(IDS_FILLTOOL_PAINT));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,FALSE, FTMENU_REFLECT);
	Str.Load(_R(IDS_FILLTOOL_STAIN));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,FALSE, FTMENU_SUB);
	Str.Load(_R(IDS_FILLTOOL_BLEACH));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_ADD);
	Str.Load(_R(IDS_FILLTOOL_CONTRAST));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_CON);
	Str.Load(_R(IDS_FILLTOOL_SATURATE));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_SAT);
	Str.Load(_R(IDS_FILLTOOL_DARKEN));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_DARK);
	Str.Load(_R(IDS_FILLTOOL_LIGHTEN));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_LIGHT);
	Str.Load(_R(IDS_FILLTOOL_BRIGHTNESS));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_BRI);
	Str.Load(_R(IDS_FILLTOOL_LUMINOSITY));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_LUM);
	Str.Load(_R(IDS_FILLTOOL_HUE));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_HUE);

	Str.Load(_R(IDS_FILLTOOL_TESS_SIMPLE));
	SetStringGadgetValue(_R(IDC_MAPPING),Str,FALSE, FTMENU_SIMPLE);
	Str.Load(_R(IDS_FILLTOOL_TESS_REPEAT));
	SetStringGadgetValue(_R(IDC_MAPPING),Str,TRUE, FTMENU_REPEAT);
	Str.Load(_R(IDS_FILLTOOL_TESS_REPEATINV));
	SetStringGadgetValue(_R(IDC_MAPPING),Str,TRUE, FTMENU_REPEATINV);

	// Ensure the drop-downs are the correct length
	SetComboListLength(_R(IDC_GEOMETRY));
	SetComboListLength(_R(IDC_TRANSPTYPE));
	SetComboListLength(_R(IDC_MAPPING));

	SetGadgetWritable(_R(IDC_GEOMETRY), FALSE);
	SetGadgetWritable(_R(IDC_TRANSPTYPE), FALSE);
	SetGadgetWritable(_R(IDC_MAPPING), FALSE);

	SetGadgetWritable(_R(IDC_SELPOINT), FALSE);
	SetGadgetWritable(_R(IDC_SELCOLOUR), TRUE);

	SliderMax = 255;
	SetGadgetRange(_R(IDC_TRANSPSLIDE), 0, SliderMax, 1);
	SetGadgetBitmaps(_R(IDC_TRANSPSLIDE), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));

	// Default to the first items in each list
	SelGeometryIndex = FGMENU_LINEARTRANSP;
	SelMappingIndex = 0;

	CommonGeometry = NULL;
	CommonMapping = NULL;

	MappingDisabled = FALSE;
	AllowForceToSimpleMapping = TRUE;

	TypeDisabled = TRUE;

	LastSliderPos = -1;	

	Mode = NOFILL;

	m_BiasGainGadget.Init(this, _R(IDC_BIASGAIN), _R(IDBBL_BIASGAIN),  _R(IDS_BIASGAINDLG));
	m_BiasGainGadget.ToggleTranspFillProfile ();

	ShowInfo();
}           

/********************************************************************************************

>	void TranspInfoBarOp::ShowInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/95
	Purpose:	Show Transparency Info on the current selection.
	SeeAlso:	-

********************************************************************************************/

void TranspInfoBarOp::ShowInfo()
{
	// Don't do anything if the Info bar isn't displayed
	if (!TranspTool::IsCurrentTool() || !IsVisible()) return;

	// Now have a look at the selected objects
	Selection = GetApplication()->FindSelection();

	// Re-Count the number of selected fill control points
	AttrFillGeometry::SetSelectionCount(AttrFillGeometry::CountSelectionControlPoints());

	OldMode = Mode;

	ShowCommonType();

	// Now show different info depending on the type ...
	// (Well we will do eventually)

	switch (Mode)
	{
		case NOFILL:
			ShowFlatInfo();
			break;

		case FLATFILL:
			ShowFlatInfo();
			break;

		case GRADFILL:
		case GRADREPEATFILL:
			ShowGraduatedInfo();
			break;

		case BITMAPFILL:
			ShowBitmapInfo();
			break;

		case FRACTALFILL:
			ShowFractalInfo();
			break;

		case NOISEFILL:
			ShowNoiseInfo();
			break;

		default:
			ShowGraduatedInfo();
			break;
	}

	EnableControls();
}

/********************************************************************************************

>	void TranspInfoBarOp::InitMapping()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/96
	Purpose:	Initialise the Mapping Control.
	SeeAlso:	-

********************************************************************************************/

void TranspInfoBarOp::InitMapping()
{
	DeleteAllValues(_R(IDC_MAPPING));
	EnableGadget(_R(IDC_MAPPING), TRUE);
	MappingDisabled = FALSE;

	String_64 Str;

	Str.Load(_R(IDS_FILLTOOL_MAPSIMPLE));
	SetStringGadgetValue(_R(IDC_MAPPING), Str,FALSE, FMMENU_SIMPLE);
	Str.Load(_R(IDS_FILLTOOL_MAPREPEAT));
	SetStringGadgetValue(_R(IDC_MAPPING), Str,TRUE, FMMENU_REPEATING);

	SetComboListLength(_R(IDC_MAPPING));
}

/********************************************************************************************

>	void TranspInfoBarOp::InitBitmapName()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (ported to transparency tool by Chris Snook 11/10/99)
	Created:	8/5/95
	Purpose:	Initialise the Bitmap Name Control.
	SeeAlso:	-

********************************************************************************************/

void TranspInfoBarOp::InitBitmapName()
{
	DeleteAllValues(_R(IDC_BITMAPEFFECT));
	EnableGadget (_R(IDC_BITMAPEFFECT), TRUE);

	Document* pDoc = Document::GetSelected();

	// Get the bitmap list
	BitmapList* Bitmaps = NULL;
	if (pDoc) Bitmaps = pDoc->GetBitmapList();

	if (Bitmaps == NULL)
		return;

	String_256 Str;

	if (Bitmaps->GetCount() > 0)
	{
		INT32 Index = 0;

		ListItem* pBmp = Bitmaps->GetHead();

		DeleteAllValues ( _R(IDC_BITMAPEFFECT) );

		while (pBmp != NULL)
		{
			if (!((KernelBitmap*)pBmp)->HasBeenDeleted())	// Ignore deleted bitmaps
			{
				Str = ((KernelBitmap*)pBmp)->ActualBitmap->GetName();

PORTNOTETRACE("other","SelectorTool::ResetDefaults - SelectorInfoBarOp usage");
#ifndef EXCLUDE_FROM_XARALX
				CustomComboBoxControlDataItem* theItem = new CustomComboBoxControlDataItem ();
				theItem->itemName = Str;
				theItem->itemID = Index;

				KernelBitmap* bitmap = ((KernelBitmap*)pBmp);

				// make the custom combobox know about our bitmap ....
				theItem->oilItemBitmap = (KernelBitmap*) bitmap;

				// and insert the data item into the cutsom combobox ....
				SetCustomComboGadgetValue ( _R(IDC_BITMAPEFFECT), theItem, TRUE, 0);
#else
				SetStringGadgetValue(_R(IDC_BITMAPEFFECT), Str, TRUE, Index);
#endif
				Index++;
			}

			pBmp = Bitmaps->GetNext(pBmp);
		}

		SetComboListLength(_R(IDC_BITMAPEFFECT));
	}
	//else
	//{
		// (not sure about this for a custom box)
		
		//Str.Load(_R(IDS_FILLTOOL_DEFAULTNAME));
		//SetStringGadgetValue(_R(IDC_EFFECT),&Str,TRUE, 0);
	//}
}

/********************************************************************************************

>	void TranspInfoBarOp::InitTransType ()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/95
	Purpose:	Initialise the Transparency Type Control.
	SeeAlso:	-

********************************************************************************************/

void TranspInfoBarOp::InitTransType ()
{
	DeleteAllValues (_R(IDC_TRANSPTYPE));
	EnableGadget (_R(IDC_TRANSPTYPE), TRUE);

	String_64 Str;

	Str.Load(_R(IDS_FILLTOOL_PAINT));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE), Str,FALSE, FTMENU_REFLECT);
	Str.Load(_R(IDS_FILLTOOL_STAIN));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,FALSE, FTMENU_SUB);
	Str.Load(_R(IDS_FILLTOOL_BLEACH));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_ADD);
	Str.Load(_R(IDS_FILLTOOL_CONTRAST));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_CON);
	Str.Load(_R(IDS_FILLTOOL_SATURATE));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_SAT);
	Str.Load(_R(IDS_FILLTOOL_DARKEN));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_DARK);
	Str.Load(_R(IDS_FILLTOOL_LIGHTEN));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_LIGHT);
	Str.Load(_R(IDS_FILLTOOL_BRIGHTNESS));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_BRI);
	Str.Load(_R(IDS_FILLTOOL_LUMINOSITY));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_LUM);
	Str.Load(_R(IDS_FILLTOOL_HUE));
	SetStringGadgetValue(_R(IDC_TRANSPTYPE),Str,TRUE, FTMENU_HUE);

	SetComboListLength(_R(IDC_TRANSPTYPE));
}


/********************************************************************************************

>	void TranspInfoBarOp::InitTesselate()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/95
	Purpose:	Initialise the Tesselation Control.
	SeeAlso:	-

********************************************************************************************/

void TranspInfoBarOp::InitTesselate()
{
	DeleteAllValues(_R(IDC_MAPPING));
	EnableGadget(_R(IDC_MAPPING), TRUE);
	MappingDisabled = FALSE;

	String_64 Str;

	Str.Load(_R(IDS_FILLTOOL_TESS_SIMPLE));
	SetStringGadgetValue(_R(IDC_MAPPING),Str,FALSE, FTMENU_SIMPLE);
	Str.Load(_R(IDS_FILLTOOL_TESS_REPEAT));
	SetStringGadgetValue(_R(IDC_MAPPING),Str,TRUE, FTMENU_REPEAT);
	Str.Load(_R(IDS_FILLTOOL_TESS_REPEATINV));
	SetStringGadgetValue(_R(IDC_MAPPING),Str,TRUE, FTMENU_REPEATINV);

	SetComboListLength(_R(IDC_MAPPING));
}

/********************************************************************************************

>	void TranspInfoBarOp::DisplayTransparencyPercentString (double value)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99
	Inputs:		value - the (double) value that is to be converted/displayed as percent.
				To get the correct results, the value supplied for value MUST be in the
				range 0-255; and NOT 0-100!
	Purpose:	Sorts out the transparency value that is actually displayed (as a string).
				It actually calculates the percentage value that has been/ is being applied.
				This is calculated in 0.5% intervals.
	SeeAlso:	-

********************************************************************************************/

void TranspInfoBarOp::DisplayTransparencyPercentString (double value)
{
	if (value == (double) 1) { value = 0; }			// cause of the way the slider is bodged
	
	double TranspVal = ((double) value * 100) / (double) SliderMax;

	UINT32 Rounded = (UINT32) TranspVal;
	double Diff = TranspVal - Rounded;

	if (Diff < 0.5)
	{
		if (Diff < 0.25) { TranspVal = (double) Rounded; }
		else { TranspVal = (double) Rounded + 0.5; }
	}
	else
	{
		if (Diff < 0.75) { TranspVal = (double) Rounded + 0.5; }
		else { TranspVal = (double) ++Rounded; }
	}

	String_64 Str;

	Convert::DoubleToString( TranspVal, &Str, 1 );

	if (Str.FindNextChar ((TCHAR) '.') == -1)
	{
		// need to wack a .0 in ....

		Str += _T(".0");
	}
	
	Str.MakePercent ((TCHAR*) Str);

	String_64 PercentStr(Str);
	SetGadgetString(_R(IDC_SELCOLOUR), &PercentStr);
}

/********************************************************************************************

>	BOOL TranspInfoBarOp::SetGadgetString(CGadgetID Gadget, StringBase* StrValue)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/95
	Purpose:	Set the string displayed by a Gadget.
				This version only updates the gadget if it has changed.
	SeeAlso:	-

********************************************************************************************/

BOOL TranspInfoBarOp::SetGadgetString(CGadgetID Gadget, StringBase* StrValue)
{
	if (Gadget == _R(IDC_EFFECT) && TypeDisabled)
		return TRUE;

	if (Gadget == _R(IDC_MAPPING) && MappingDisabled)
		return TRUE;

	if (GetStringGadgetValue(Gadget, NULL, -1) != *StrValue)
  	{
		return SetStringGadgetValue(Gadget, *StrValue, FALSE, -1);
	}

	return TRUE;
}

/********************************************************************************************

>	void TranspInfoBarOp::ShowFlatInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Purpose:	Show info on a flat fill.

********************************************************************************************/

void TranspInfoBarOp::ShowFlatInfo()
{
	if (OldMode != FLATFILL && OldMode != NOFILL)
	{
		InitTransType ();
		InitTesselate();
//		InitType();

		SetGadgetHelp(_R(IDC_TRANSPTYPE), 	_R(IDBBL_TRANSPTOOL_TRANSPTYPE), 	_R(IDS_TRANSPTOOL_TRANSPTYPE));
		SetGadgetHelp(_R(IDC_MAPPING), 	 	_R(IDBBL_TRANSPTOOL_TRANSPTILING), 	_R(IDS_TRANSPTOOL_TRANSPTILING));
		SetGadgetHelp(_R(IDC_SELPOINT),  	_R(IDBBL_TRANSPTOOL_TRANSPHANDLE),  _R(IDS_TRANSPTOOL_TRANSPHANDLE));
		SetGadgetHelp(_R(IDC_TRANSPSLIDE), 	_R(IDBBL_TRANSPTOOL_TRANSP), 		_R(IDS_TRANSPTOOL_TRANSP));
		SetGadgetHelp(_R(IDC_SELCOLOUR), 	_R(IDBBL_TRANSPTOOL_TRANSP), 		_R(IDS_TRANSPTOOL_TRANSP));

		// Reset the slider back to its proper range as if we have been using the fractal fill
		// the the slider range will be large and we will always set 0 tranasparency i.e.
		// dragging the slider always gives 0
		SliderMax = 255;
		SetGadgetRange(_R(IDC_TRANSPSLIDE), 0, SliderMax, 1);
	}

	ShowCommonTesselate();
	ShowCommonTranspType();
	ShowControlPointInfo();
}

/********************************************************************************************

>	void TranspInfoBarOp::ShowGraduatedInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Purpose:	Show info on a graduated fill.

********************************************************************************************/

void TranspInfoBarOp::ShowGraduatedInfo()
{
	if (OldMode != GRADFILL && OldMode != GRADREPEATFILL)
	{
		InitTransType ();
		InitMapping();
//		InitType();

		SetGadgetHelp(_R(IDC_TRANSPTYPE), 	_R(IDBBL_TRANSPTOOL_TRANSPTYPE), 	_R(IDS_TRANSPTOOL_TRANSPTYPE));
		SetGadgetHelp(_R(IDC_MAPPING), 	 	_R(IDBBL_TRANSPTOOL_TRANSPTILING), 	_R(IDS_TRANSPTOOL_TRANSPTILING));
		SetGadgetHelp(_R(IDC_SELPOINT),  	_R(IDBBL_TRANSPTOOL_TRANSPHANDLE),  _R(IDS_TRANSPTOOL_TRANSPHANDLE));
		SetGadgetHelp(_R(IDC_TRANSPSLIDE), 	_R(IDBBL_TRANSPTOOL_TRANSP), 		_R(IDS_TRANSPTOOL_TRANSP));
		SetGadgetHelp(_R(IDC_SELCOLOUR), 	_R(IDBBL_TRANSPTOOL_TRANSP), 		_R(IDS_TRANSPTOOL_TRANSP));

		// Reset the slider back to its proper range as if we have been using the fractal fill
		// the the slider range will be large and we will always set 0 tranasparency i.e.
		// dragging the slider always gives 0
		SliderMax = 255;
		SetGadgetRange(_R(IDC_TRANSPSLIDE), 0, SliderMax, 1);
	}

	ShowCommonMapping();
	ShowCommonTranspType();
	ShowControlPointInfo();
}


/********************************************************************************************

>	void TranspInfoBarOp::ShowBitmapInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Purpose:	Show info on a bitmap fill.

********************************************************************************************/

void TranspInfoBarOp::ShowBitmapInfo()
{
	if (OldMode != BITMAPFILL)
	{
		InitTesselate();
		InitTransType ();
//		InitType();
		InitBitmapName();

		SetGadgetHelp(_R(IDC_TRANSPTYPE), 	_R(IDBBL_TRANSPTOOL_TRANSPTYPE), 	_R(IDS_TRANSPTOOL_TRANSPTYPE));
		SetGadgetHelp(_R(IDC_MAPPING), 	 	_R(IDBBL_TRANSPTOOL_TRANSPTILING), 	_R(IDS_TRANSPTOOL_TRANSPTILING));
		SetGadgetHelp(_R(IDC_BITMAPEFFECT), _R(IDBBL_FILLTOOL_BITMAPNAME),		_R(IDS_FILLTOOL_BITMAPNAME));
	}

    if (AttrFillGeometry::SelectionCount == 0)
	{
		SetGadgetWritable(_R(IDC_SELPOINT), TRUE);
		
		SetGadgetHelp(_R(IDC_SELPOINT),		_R(IDBBL_FILLTOOL_FRACTALRES),		_R(IDS_FILLTOOL_FRACTALRES));
		//SetGadgetHelp(_R(IDC_TRANSPSLIDE), 	_R(IDBBL_TRANSPTOOL_GRAININESS), 	_R(IDS_TRANSPTOOL_GRAININESS));
		//SetGadgetHelp(_R(IDC_SELCOLOUR),  	_R(IDBBL_TRANSPTOOL_GRAININESS),  	_R(IDS_TRANSPTOOL_GRAININESS));
		SetGadgetHelp(_R(IDC_TRANSPSLIDE), 	_R(IDBBL_TRANSPTOOL_TRANSP), 		_R(IDS_TRANSPTOOL_TRANSP));
		SetGadgetHelp(_R(IDC_SELCOLOUR), 	_R(IDBBL_TRANSPTOOL_TRANSP), 		_R(IDS_TRANSPTOOL_TRANSP));

		//EnableGadget (_R(IDC_TRANSPSLIDE), FALSE);
		//EnableGadget (_R(IDC_SELCOLOUR), FALSE);
	}
	else
	{
		SetGadgetWritable(_R(IDC_SELPOINT), FALSE);
		SetGadgetHelp(_R(IDC_SELPOINT),  	_R(IDBBL_TRANSPTOOL_TRANSPHANDLE),  _R(IDS_TRANSPTOOL_TRANSPHANDLE));

		SetGadgetHelp(_R(IDC_TRANSPSLIDE), 	_R(IDBBL_TRANSPTOOL_TRANSP), 		_R(IDS_TRANSPTOOL_TRANSP));
		SetGadgetHelp(_R(IDC_SELCOLOUR), 	_R(IDBBL_TRANSPTOOL_TRANSP), 		_R(IDS_TRANSPTOOL_TRANSP));

		SliderMax = 255;
	}

	SetGadgetRange(_R(IDC_TRANSPSLIDE), 0, SliderMax, 1);

	ShowCommonTesselate();
	ShowCommonTranspType();
	ShowCommonBitmapName();

    if (AttrFillGeometry::SelectionCount == 0)
		ShowCommonBitmapDpi();

	ShowControlPointInfo();
}


/********************************************************************************************

>	void TranspInfoBarOp::ShowNoiseInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Purpose:	Show info on a noise transparency

********************************************************************************************/

void TranspInfoBarOp::ShowNoiseInfo()
{
	if (OldMode != NOISEFILL)
	{
		InitTransType ();
		InitTesselate();
		//InitType();

		SetGadgetHelp(_R(IDC_TRANSPTYPE), 	_R(IDBBL_TRANSPTOOL_TRANSPTYPE), 	_R(IDS_TRANSPTOOL_TRANSPTYPE));
		SetGadgetHelp(_R(IDC_MAPPING), 	 	_R(IDBBL_TRANSPTOOL_TRANSPTILING), 	_R(IDS_TRANSPTOOL_TRANSPTILING));
	}

    if (AttrFillGeometry::SelectionCount == 0)
	{
		SetGadgetWritable(_R(IDC_SELPOINT), TRUE);

		SetGadgetHelp(_R(IDC_SELPOINT),		_R(IDBBL_FILLTOOL_FRACTALRES),		_R(IDS_FILLTOOL_FRACTALRES));
		SetGadgetHelp(_R(IDC_TRANSPSLIDE), 	_R(IDBBL_TRANSPTOOL_NOISESCALE), 	_R(IDS_TRANSPTOOL_NOISESCALE));
		SetGadgetHelp(_R(IDC_SELCOLOUR),  	_R(IDBBL_TRANSPTOOL_NOISESCALE),  	_R(IDS_TRANSPTOOL_NOISESCALE));

		SliderMax = GetScalePosMax();
	}
	else
	{
		SetGadgetWritable(_R(IDC_SELPOINT), FALSE);

		SetGadgetHelp(_R(IDC_SELPOINT),  	_R(IDBBL_TRANSPTOOL_TRANSPHANDLE),  _R(IDS_TRANSPTOOL_TRANSPHANDLE));
		SetGadgetHelp(_R(IDC_TRANSPSLIDE), 	_R(IDBBL_TRANSPTOOL_TRANSP), 		_R(IDS_TRANSPTOOL_TRANSP));
		SetGadgetHelp(_R(IDC_SELCOLOUR), 	_R(IDBBL_TRANSPTOOL_TRANSP), 		_R(IDS_TRANSPTOOL_TRANSP));
		
		SliderMax = 255;
	}

	SetGadgetRange(_R(IDC_TRANSPSLIDE), 0, SliderMax, 1);

	ShowCommonTesselate();
	ShowCommonTranspType();

    if (AttrFillGeometry::SelectionCount == 0)
	{
    	ShowCommonBitmapDpi();
    	ShowCommonNoiseScale();
	}
	else
		ShowControlPointInfo();
}

/********************************************************************************************

>	void TranspInfoBarOp::ShowFractalInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Purpose:	Show info on a fractal transparency

********************************************************************************************/

void TranspInfoBarOp::ShowFractalInfo()
{
	if (OldMode != FRACTALFILL)
	{
		InitTransType ();
		InitTesselate();
//		InitType();

		//SetGadgetHelp(_R(IDC_TRANSPTYPE), 	_R(IDBBL_TRANSPTOOL_TRANSPTYPE), 	_R(IDS_TRANSPTOOL_TRANSPTYPE));
		SetGadgetHelp(_R(IDC_MAPPING), 	 	_R(IDBBL_TRANSPTOOL_TRANSPTILING), 	_R(IDS_TRANSPTOOL_TRANSPTILING));
	}

    if (AttrFillGeometry::SelectionCount == 0)
	{
		SetGadgetWritable(_R(IDC_SELPOINT), TRUE);
		
		SetGadgetHelp(_R(IDC_SELPOINT),		_R(IDBBL_FILLTOOL_FRACTALRES),		_R(IDS_FILLTOOL_FRACTALRES));
		SetGadgetHelp(_R(IDC_TRANSPSLIDE), 	_R(IDBBL_TRANSPTOOL_GRAININESS), 	_R(IDS_TRANSPTOOL_GRAININESS));
		SetGadgetHelp(_R(IDC_SELCOLOUR),  	_R(IDBBL_TRANSPTOOL_GRAININESS),  	_R(IDS_TRANSPTOOL_GRAININESS));

		SliderMax = GetGrainPosMax();
	}
	else
	{
		SetGadgetWritable(_R(IDC_SELPOINT), FALSE);

		SetGadgetHelp(_R(IDC_SELPOINT),  	_R(IDBBL_TRANSPTOOL_TRANSPHANDLE),  _R(IDS_TRANSPTOOL_TRANSPHANDLE));
		SetGadgetHelp(_R(IDC_TRANSPSLIDE), 	_R(IDBBL_TRANSPTOOL_TRANSP), 		_R(IDS_TRANSPTOOL_TRANSP));
		SetGadgetHelp(_R(IDC_SELCOLOUR), 	_R(IDBBL_TRANSPTOOL_TRANSP), 		_R(IDS_TRANSPTOOL_TRANSP));

		SliderMax = 255;
	}

	SetGadgetRange(_R(IDC_TRANSPSLIDE), 0, SliderMax, 1);

	ShowCommonTesselate();
	ShowCommonTranspType();

    if (AttrFillGeometry::SelectionCount == 0)
	{
		ShowCommonBitmapDpi();
		ShowCommonFractalGrain();
	}
	else
		ShowControlPointInfo();
}

/********************************************************************************************

>	void TranspInfoBarOp::ShowCommonType()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (altered by Chris Snook)
	Created:	25/8/94
	Inputs:		if ShowDataAndNotSet is FALSE, then data members will be set (for controlling
				menu selection logic) and data will not be shown in the relevant control.
	Purpose:	Show any Common Fill Type in the selection.

********************************************************************************************/

void TranspInfoBarOp::ShowCommonType(BOOL ShowDataAndNotSet)
{
	CommonAttr = SelRange::ATTR_NONE;
	String_64 Str;
	Str.Load(_R(IDS_FILLTOOL_NONE));
	String_64 TStr;
	TStr.Load(_R(IDS_FILLTOOL_NONE));

	if (Selection != NULL)
	{
		Document* pCurrentDoc = Document::GetCurrent();
		if (Document::GetSelected())
			Document::GetSelected()->SetCurrent();

EffectsStack* pStack = Selection->GetEffectsStack();
INT32 iStackPos = STACKPOS_TOP;
Range* pLevel = pStack->GetLevelRange(&iStackPos, FALSE);		// Don't escape old controllers, apply attr to base nodes
		
		// Try and find a common Attribute within the selection.
		// All objects in the selection must have an Identical fill for this
		// to find anything.
//		CommonAttr = Selection->FindCommonAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry), 
//													(NodeAttribute**)&CommonGeometry);
		CommonAttr = pLevel->FindCommonAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry), 
													(NodeAttribute**)&CommonGeometry,
													TRUE);
		CCRuntimeClass* GeometryType;

		// Try and find a common Attribute Type within the selection.
		// The fills need not be identical. Just of the same type.
//		SelRange::CommonAttribResult CommonType = 
//				Selection->FindCommonAttributeType(CC_RUNTIME_CLASS(AttrTranspFillGeometry), 
//												    &GeometryType);
		SelRange::CommonAttribResult CommonType = 
				pLevel->FindCommonAttributeType(CC_RUNTIME_CLASS(AttrTranspFillGeometry), 
												    &GeometryType,
													FALSE,
													TRUE);

		if (CommonAttr == SelRange::ATTR_NONE &&
			CommonGeometry != NULL)
		{
			CommonAttr = SelRange::ATTR_COMMON;
		}

		if (CommonType == SelRange::ATTR_NONE &&
			GeometryType != NULL)
		{
			CommonType = SelRange::ATTR_COMMON;
		}
    
		// Make the menu show any Common Fill Type
		// or failing that, 'None' or 'Many'.
		if (CommonType == SelRange::ATTR_COMMON)
		{	
		 	if (GeometryType == CC_RUNTIME_CLASS(AttrFlatTranspFill))
			{
				Str.Load(_R(IDS_FILLTOOL_FLATTRANSP));
				Mode = FLATFILL;
				CurrentGeometryIndex = FGMENU_FLATTRANSP;

				if (CommonAttr == SelRange::ATTR_COMMON)
				{
					if ( (CommonGeometry->GetTranspType() == TT_Mix) && ((*CommonGeometry->GetStartTransp()) == 0) )
					{
						Str.Load(_R(IDS_FILLTOOL_NONE));
						Mode = NOFILL;
						CurrentGeometryIndex = 0;
					}
				}
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrLinearTranspFill))
			{
				Str.Load(_R(IDS_FILLTOOL_LINEAR));
				Mode = GRADFILL;
				CurrentGeometryIndex = FGMENU_LINEARTRANSP;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrCircularTranspFill))
			{
				Str.Load(_R(IDS_FILLTOOL_CIRCULAR));
				Mode = GRADFILL;
				CurrentGeometryIndex = FGMENU_CIRCULARTRANSP;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrRadialTranspFill))
			{
				Str.Load(_R(IDS_FILLTOOL_ELLIPTICAL));
				Mode = GRADFILL;
				CurrentGeometryIndex = FGMENU_RADIALTRANSP;
    		}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrConicalTranspFill))
			{
				Str.Load(_R(IDS_FILLTOOL_CONICAL));
				Mode = GRADFILL;
				CurrentGeometryIndex = FGMENU_CONICALTRANSP;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrSquareTranspFill))
			{
				Str.Load(_R(IDS_FILLTOOL_SQUARE));
				Mode = GRADFILL;
				CurrentGeometryIndex = FGMENU_SQUARETRANSP;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrThreeColTranspFill))
			{
				Str.Load(_R(IDS_FILLTOOL_THREECOLTRANS));
				Mode = GRADREPEATFILL;
				CurrentGeometryIndex = FGMENU_THREECOLTRANSP;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrFourColTranspFill))
			{
				Str.Load(_R(IDS_FILLTOOL_FOURCOLTRANS));
				Mode = GRADREPEATFILL;
				CurrentGeometryIndex = FGMENU_FOURCOLTRANSP;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrBitmapTranspFill))
			{
				Str.Load(_R(IDS_FILLTOOL_TEXTURE));
				Mode = BITMAPFILL;
				CurrentGeometryIndex = FGMENU_BITMAPTRANSP;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrFractalTranspFill))
			{
				Str.Load(_R(IDS_FILLTOOL_FRACTAL));
				Mode = FRACTALFILL;
				CurrentGeometryIndex = FGMENU_FRACTALTRANSP;
			}
			else if (GeometryType == CC_RUNTIME_CLASS(AttrNoiseTranspFill))
			{
				Str.Load(_R(IDS_FILLTOOL_NOISE));
				Mode = NOISEFILL;
				CurrentGeometryIndex = FGMENU_NOISETRANSP;
			}
		}
		else
		{
			if (CommonAttr == SelRange::ATTR_MANY)
			{
				// There are many different types of fill selected
				Str.Load(_R(IDS_FILLTOOL_MANY));
				Mode = MANYFILLS;
				CurrentGeometryIndex = FGMENU_MANYTRANSP;
			}
		}

		if (pCurrentDoc)
			pCurrentDoc->SetCurrent();
	}

	// Update the Menus
	if (ShowDataAndNotSet)
	{
		SetGadgetString(_R(IDC_GEOMETRY),&Str);
	}
}

/********************************************************************************************

>	void GradInfoBarOp::ShowCommonTranspType ()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (ported to filltool by Chris Snook 11/10/99)
	Created:	25/8/94
	Inputs:		if ShowDataAndNotSet is FALSE, then data members will be set (for controlling
				menu selection logic) and data will not be shown in the relevant control.
	Purpose:	Show any Common Transparency type in the selection.

********************************************************************************************/

void TranspInfoBarOp::ShowCommonTranspType (BOOL ShowDataAndNotSet)
{
	String_64 TStr;
	TStr.Load (_R(IDS_FILLTOOL_NONE));

	INT32 TType = FindCommonTranspType ();
	switch (TType)
	{
		case -1:
			TStr.Load(_R(IDS_FILLTOOL_MANY));
			CurrentTransTypeIndex = -2;
			break;

		case 0:
			TStr.Load(_R(IDS_FILLTOOL_NONE));
			CurrentTransTypeIndex = -1;
			break;

		case 1:
			TStr.Load(_R(IDS_FILLTOOL_PAINT));
			CurrentTransTypeIndex = FTMENU_REFLECT;
			break;

		case 2:
			TStr.Load(_R(IDS_FILLTOOL_STAIN));
			CurrentTransTypeIndex = FTMENU_SUB;
			break;

		case 3:
			TStr.Load(_R(IDS_FILLTOOL_BLEACH));
			CurrentTransTypeIndex = FTMENU_ADD;
			break;

		case 13:
			TStr.Load(_R(IDS_FILLTOOL_CONTRAST));
			CurrentTransTypeIndex = FTMENU_CON;
			break;

		case 16:
			TStr.Load(_R(IDS_FILLTOOL_SATURATE));
			CurrentTransTypeIndex = FTMENU_SAT;
			break;

		case 19:
			TStr.Load(_R(IDS_FILLTOOL_DARKEN));
			CurrentTransTypeIndex = FTMENU_DARK;
			break;
		
		case 22:
			TStr.Load(_R(IDS_FILLTOOL_LIGHTEN));
			CurrentTransTypeIndex = FTMENU_LIGHT;
			break;

		case 25:
			TStr.Load(_R(IDS_FILLTOOL_BRIGHTNESS));
			CurrentTransTypeIndex = FTMENU_BRI;
			break;

		case 28:
			TStr.Load(_R(IDS_FILLTOOL_LUMINOSITY));
			CurrentTransTypeIndex = FTMENU_LUM;
			break;

		case 31:
			TStr.Load(_R(IDS_FILLTOOL_HUE));
			CurrentTransTypeIndex = FTMENU_HUE;
			break;
	}

	if (ShowDataAndNotSet)
	{
		SetGadgetString(_R(IDC_TRANSPTYPE),&TStr);
	}
}

/********************************************************************************************

>	UINT32 TranspInfoBarOp::FindCommonTranspType()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (ported to filltool by Chris Snook 11/10/99)
	Created:	14/9/94
	Returns:	0 = No Common type, >0 = Common Transparency Type
	Purpose:	Tries to find a common transparency type within the selection.
				This now makes use of camelots new AttrPaintingMode (which I also wrote).

********************************************************************************************/

INT32 TranspInfoBarOp::FindCommonTranspType()
{
	if (Selection->Count() == 0)
	{
		AttrFillGeometry* pCurrentFill = GetCurrentGeometry();

		if (pCurrentFill != NULL)
		{
			return pCurrentFill->GetTranspType();
		}

		return 0;
	}

	AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));

	// Return if there aren't any
	if (pAttrNode == NULL)
		return 0;

	UINT32 TType = 0;
	
	while (pAttrNode != NULL)
	{
		if (pAttrNode->GetAttributeType() == CC_RUNTIME_CLASS(AttrTranspFillGeometry))
		{
			UINT32 NodeTType = pAttrNode->GetTranspType();
			
			if (TType == 0)
				TType = NodeTType;

			if (TType != NodeTType)
				return -1;
		}

		// Check the next fill
		pAttrNode = AttrFillGeometry::FindNextSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));
	}

	return TType;
}


/********************************************************************************************

>	void GradInfoBarOp::ShowCommonMapping()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com> (altered by Chris Snook)
	Created:	22/8/96
	Inputs:		if ShowDataAndNotSet is FALSE, then data members will be set (for controlling
				menu selection logic) and data will not be shown in the relevant control.
	Purpose:	Show any Common Fill Mapping in the selection.

********************************************************************************************/

void TranspInfoBarOp::ShowCommonMapping(BOOL ShowDataAndNotSet)
{
	String_64 Str;

	INT32 CommTess = FindCommonTesselate();

	switch (CommTess)
	{
		case -1:
			Str.Load(_R(IDS_FILLTOOL_MANY));
			CurrentMappingIndex = -2;//FMMENU_REPEATING;
			break;

		case 0:
			Str.Load(_R(IDS_FILLTOOL_NONE));
			CurrentMappingIndex = -1;//FMMENU_REPEATING;
			break;

		case 1:
			Str.Load(_R(IDS_FILLTOOL_MAPSIMPLE));
			CurrentMappingIndex = FMMENU_SIMPLE;
			break;

		case 2:
//Mark Howitt, 14/10/97. Lets make sure repeating trans grad fills when set to 2 are actually simple!
#ifdef NEW_FEATURES
			if(Mode == GRADFILL)
			{
				Str.Load(_R(IDS_FILLTOOL_MAPSIMPLE));
				CurrentMappingIndex = FMMENU_SIMPLE;
				break;
			}
			else
			{
				Str.Load(_R(IDS_FILLTOOL_MAPREPEAT));
				CurrentMappingIndex = FMMENU_REPEATING;
				break;
			}
		case 4:		// Must be a repeating trans grad fill!
#endif
		case 3:
			Str.Load(_R(IDS_FILLTOOL_MAPREPEAT));
			CurrentMappingIndex = 2;//FMMENU_REPEATING;
			break;
	}

	if (ShowDataAndNotSet)
	{
		SetGadgetString(_R(IDC_MAPPING),&Str);
	}
}

/********************************************************************************************

>	void TranspInfoBarOp::ShowCommonTesselate()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Show any Common Fill Tesselation in the selection.

********************************************************************************************/

void TranspInfoBarOp::ShowCommonTesselate()
{
	String_64 Str;

	INT32 CommTess = FindCommonTesselate();

	switch (CommTess)
	{
		case -1:
			Str.Load(_R(IDS_FILLTOOL_MANY));
			CurrentMappingIndex = FTMENU_REPEAT;
			break;

		case 0:
			Str.Load(_R(IDS_FILLTOOL_NONE));
			CurrentMappingIndex = FTMENU_REPEAT;
			break;

		case 1:
			Str.Load(_R(IDS_FILLTOOL_TESS_SIMPLE));
			CurrentMappingIndex = FTMENU_SIMPLE;
			break;

		case 2:
			Str.Load(_R(IDS_FILLTOOL_TESS_REPEAT));
			CurrentMappingIndex = FTMENU_REPEAT;
			break;

		case 3:
			Str.Load(_R(IDS_FILLTOOL_TESS_REPEATINV));
			CurrentMappingIndex = FTMENU_REPEATINV;
			break;
	}

	SetGadgetString(_R(IDC_MAPPING),&Str);
//	SetStringGadgetValue(_R(IDC_MAPPING),&Str, FALSE, -1);
	EnableGadget(_R(IDC_MAPPING), TRUE);
}

/********************************************************************************************

>	INT32 TranspInfoBarOp::FindCommonTesselate()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Find any Common Fill Tesselation in the selection.

********************************************************************************************/

INT32 TranspInfoBarOp::FindCommonTesselate()
{
	INT32 CommonTess = 0;
	Node* pNode;
	NodeAttribute* pChild;

	if (Selection->Count() == 0)
	{
		AttrTranspFillMapping* pCurrentMapping = GetCurrentMapping();

		if (pCurrentMapping != NULL)
		{
			return pCurrentMapping->GetRepeat();
		}

		return 0;
	}

	// Are there any selected Objects ?
	if (Selection != NULL)
	{
EffectsStack* pStack = Selection->GetEffectsStack();
INT32 iStackPos = STACKPOS_TOP;
Range* pLevel = pStack->GetLevelRange(&iStackPos, FALSE);		// Don't escape old controllers, apply attr to base nodes
		// Find the first Object
/*		pNode = Selection->FindFirst();
		while (pNode != NULL)
		{
			BOOL FoundAttr = ((NodeRenderableInk*)pNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillMapping), &pChild);
			
			if (FoundAttr)
			{
				INT32 Tess = ((AttrTranspFillMapping*)pChild)->GetRepeat();

				if (CommonTess == 0)
					CommonTess = Tess;

				if (Tess != CommonTess)
			   	{
					return -1; // Many
				}
			}

			pNode = Selection->FindNext(pNode);
		}
*/
		pNode = pLevel->FindFirst();
		while (pNode != NULL)
		{
			pChild = ((NodeRenderableInk*)pNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillMapping));
			
			if (pChild)
			{
				INT32 Tess = ((AttrTranspFillMapping*)pChild)->GetRepeat();

				if (CommonTess == 0)
					CommonTess = Tess;

				if (Tess != CommonTess)
			   	{
					return -1; // Many
				}
			}

			pNode = pLevel->FindNext(pNode);
		}

	}

	//if (CommonTess == 0)
	//	return RT_Repeating;

	return CommonTess;
}

/********************************************************************************************

>	void TranspInfoBarOp::ShowCommonBitmapName()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Show any Common Bitmap Name in the selection.

********************************************************************************************/

void TranspInfoBarOp::ShowCommonBitmapName()
{
	String_64 Str;

	Str = FindCommonBitmapName();
PORTNOTETRACE("other","TranspInfoBarOp::InitBitmapName() - removed CustomComboBox handler");
#ifndef EXCLUDE_FROM_XARALX
	SelectCustomComboGadgetValueOnString(_R(IDC_BITMAPEFFECT), &Str);
#else
	SetGadgetString(_R(IDC_BITMAPEFFECT), &Str);
#endif
}

/********************************************************************************************

>	String_64 TranspInfoBarOp::FindCommonBitmapName()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Find any Common Bitmap Name in the selection.

********************************************************************************************/

String_64 TranspInfoBarOp::FindCommonBitmapName()
{
	String_256 CommonName;
	CommonName.Load(_R(IDS_FILLTOOL_NONE));

	if (Selection->Count() == 0)
	{
		AttrFillGeometry* pCurrentFill = GetCurrentGeometry();

		//AttrTranspFillGeometry* pCurrentFill = GetCurrentGeometry();

		// NOTE:  we can call either IsATranspFill or IsABitmapFill here ....
		// BOTH return TRUE, the only difference is that IsABitmapFill is a base
		// class member function (AttrBitmapFill::) and IsATranspFill is a 'true'
		// class function (i.e.  defined and implemented in AttrBitmapTranspFill)
		// BUT, lets stick with IsABitmapFill (for clarity) - cause after all,
		// were dealing with bitmaps here ....

		if (pCurrentFill && pCurrentFill->/*IsATranspFill*/IsABitmapFill())
		{
			CommonName = pCurrentFill->GetBitmap()->ActualBitmap->GetName();
		}

		return CommonName;
	}

	String_256 Name;
	Name.Load(_R(IDS_FILLTOOL_NONE));
	
	String_256 None;
	None.Load(_R(IDS_FILLTOOL_NONE));

	String_256 Many;
	Many.Load(_R(IDS_FILLTOOL_MANY));

	// Find the first Fill Attribute in the selection (of the right type!)
	// (observe the CC_RUNTIME_CLASS(AttrTranspFillGeometry) 
	AttrFillGeometry* pAttrNode = AttrTranspFillGeometry::FindFirstSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));

	// Return if there aren't any
	if (pAttrNode == NULL)
		return None;

	while (pAttrNode != NULL)
	{
		if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrBitmapTranspFill)))
		{
			Name = pAttrNode->GetBitmap()->ActualBitmap->GetName();

			if (CommonName == None)
				CommonName = Name;

			if (Name != CommonName)
		   	{
				return Many;
			}
		}

		// Check the next fill (of the right type!)
		pAttrNode = AttrTranspFillGeometry::FindNextSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));
	}

	return CommonName;
}

/********************************************************************************************

>	void TranspInfoBarOp::ShowCommonBitmapDpi()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Show any Common Bitmap Dpi in the selection.

********************************************************************************************/

void TranspInfoBarOp::ShowCommonBitmapDpi()
{
	INT32 dpi = FindCommonBitmapDpi();

	TCHAR Str[16];
	String_64 DpiStr;

	if (dpi == 0)
	{
		DpiStr.Load(_R(IDS_FILLTOOL_NODPI));
	}

	else if (dpi == -1)
	{
		DpiStr.Load(_R(IDS_FILLTOOL_MANYDPI));
	}

	else if (dpi > 0)
	{
		String_256 jcf(_R(IDS_FILLTOOL_DPI_FORMAT));
		camSnprintf(Str, 15, jcf, (INT32) dpi);
		DpiStr = Str;
	}

	SetGadgetString(_R(IDC_SELPOINT), &DpiStr);
}

void TranspInfoBarOp::SetGadgetWritable(INT32 id, BOOL enable)
{
	PORTNOTETRACE("other","TranspInfoBarOp::SetGadgetWritable disabled");
#ifndef EXCLUDE_FROM_XARALX
	// Bodge code to try and make a gadget writable on the fly
	HWND gadget = ::GetDlgItem(WindowID, id);
	HWND hEdit = ::ChildWindowFromPoint(gadget, CPoint(1,1));

	if (hEdit)
		gadget = hEdit;
 
 	if (enable)
		::SendMessage(gadget, EM_SETREADONLY, FALSE, 0);
	else
		::SendMessage(gadget, EM_SETREADONLY, TRUE, 0);
#endif
}

/********************************************************************************************

>	BOOL TranspInfoBarOp::GetGadgetWritable(INT32 id)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/11/99
	Inputs:		id, the 'IDC_?' of the control.
	Returns:	TRUE if writable, FALSE otherwise
	Purpose:	Returns whether the control is writable (i.e.  read only, or not).
				void TranspInfoBarOp::EnableControls() needs to make use of this to control
				its 'dynamic' toolbar correctly (both of which were also written by me).

********************************************************************************************/

BOOL TranspInfoBarOp::GetGadgetWritable (INT32 id)
{
	PORTNOTETRACE("other","TranspInfoBarOp::GetGadgetWritable disabled");
#ifndef EXCLUDE_FROM_XARALX
	// Get the window handle of the gadget, from the gadget ID
	HWND gadget = ::GetDlgItem (WindowID, id);

	// See if it's got a child window (it may be a Combo Box)
	HWND hEdit = ::ChildWindowFromPoint (gadget, CPoint(1,1));

	if (hEdit)				// Was there a child window ?
		gadget = hEdit;		// Yes, so send the message to it

	if (GetWindowLong (gadget, GWL_STYLE) & ES_READONLY)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
#else
	return TRUE;
#endif
}

/********************************************************************************************

>	INT32 TranspInfoBarOp::FindCommonBitmapDpi()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Find any Common Bitmap Dpi in the selection.

********************************************************************************************/

INT32 TranspInfoBarOp::FindCommonBitmapDpi()
{
	if (Selection->Count() == 0)
	{
		AttrFillGeometry* pCurrentFill = GetCurrentGeometry();

		if (pCurrentFill && pCurrentFill->IsABitmapFill())
		{
			return pCurrentFill->GetDPI();
		}

		return 0;
	}

	// Find the first Fill Attribute in the selection
	AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));

	// Return if there aren't any
	if (pAttrNode == NULL)
		return 0;

	INT32 CommonDpi = 0;
	INT32 Dpi = 0;

	while (pAttrNode != NULL)
	{
		if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrBitmapFill)))
		{
			if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrTextureTranspFill)) ||
				pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrBitmapTranspFill))  )
			{
				Dpi = pAttrNode->GetDPI();

				if (CommonDpi == 0)
					CommonDpi = Dpi;

				if (Dpi != CommonDpi)
			   	{
					return -1; // Many
				}
			}
		}

		// Check the next fill
		pAttrNode = AttrFillGeometry::FindNextSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));
	}

	return CommonDpi;
}

/********************************************************************************************

>	void TranspInfoBarOp::ShowCommonFractalGrain()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Show any Common Fractal Graininess in the selection.

********************************************************************************************/

void TranspInfoBarOp::ShowCommonFractalGrain()
{
	double Grain = FindCommonFractalGrain();

	TCHAR Str[16];
	String_64 GrainStr;

	if (Grain == 0)
		GrainStr.Load(_R(IDS_FILLTOOL_NOGRAIN));
	if (Grain == -1)
		GrainStr.Load(_R(IDS_FILLTOOL_MANYGRAIN));
	if (Grain > 0)
	{
		camSnprintf(Str, 15, _T("%.1f"), Grain);
		GrainStr = Str;
	}

	INT32 GrainPos = GetGrainPos(Grain);

	SetGadgetString(_R(IDC_SELCOLOUR),&GrainStr);
	SetLongGadgetValue(_R(IDC_TRANSPSLIDE), GrainPos);

	// Build the Percentage string and set it
	camSnprintf(Str, 15, _T("%.1f"), Grain);
	String_8 PercentStr(Str);
	SetGadgetString(_R(IDC_SELCOLOUR), &PercentStr);
}

/********************************************************************************************

>	FIXED16 TranspInfoBarOp::FindCommonFractalGrain()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Find any Common Fractal Graininess in the selection.

********************************************************************************************/

double TranspInfoBarOp::FindCommonFractalGrain()
{
	if (Selection->Count() == 0)
	{
		AttrFillGeometry* pCurrentFill = GetCurrentGeometry();

		if (pCurrentFill && pCurrentFill->IsAFractalFill())
		{
			return ((FractalTranspFillAttribute*)pCurrentFill->GetAttributeValue())->Graininess.MakeDouble();
		}

		return 0;
	}

	// Find the first Fill Attribute in the selection
	AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));

	// Return if there aren't any
	if (pAttrNode == NULL)
		return 0;

	double CommonGrain = 0;

	while (pAttrNode != NULL)
	{
		if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrFractalTranspFill)))
		{
			double Grain = ((FractalTranspFillAttribute*)pAttrNode->GetAttributeValue())->Graininess.MakeDouble();

			if (CommonGrain == 0)
				CommonGrain = Grain;

			if (Grain != CommonGrain)
		   	{
				return -1; // Many
			}
		}

		// Check the next fill
		pAttrNode = AttrFillGeometry::FindNextSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));
	}

	return CommonGrain;
}

/********************************************************************************************

>	void TranspInfoBarOp::ShowCommonNoiseScale()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Show any Common Fractal Graininess in the selection.

********************************************************************************************/

void TranspInfoBarOp::ShowCommonNoiseScale()
{
	double Scale = FindCommonNoiseScale();

	BOOL convert = FALSE;

	TCHAR Str[16];
	String_64 ScaleStr;

	if (Scale == 0)
	{
		ScaleStr.Load(_R(IDS_FILLTOOL_NOGRAIN));
	}
	if (Scale == -1)
	{
		ScaleStr.Load(_R(IDS_FILLTOOL_MANYGRAIN));
	}
	if (Scale > 0)
	{	
		camSnprintf(Str, 15, _T("%.1f"), Scale);
		ScaleStr = Str;
		convert = TRUE;
	}

	INT32 ScalePos = GetScalePos(Scale);

	SetGadgetString(_R(IDC_SELCOLOUR),&ScaleStr);
	SetLongGadgetValue(_R(IDC_TRANSPSLIDE), ScalePos);

	// Build the Percentage string and set it

	if (convert)
	{
		String_64 Str;

		Convert::DoubleToString( Scale, &Str, 1 );
		
		Str.MakePercent ((TCHAR*) Str);

		String_64 PercentStr(Str);
		SetGadgetString(_R(IDC_SELCOLOUR), &PercentStr);
	}
	else
	{
	
		camSnprintf(Str, 15, _T("%.1f"), Scale);
		String_8 PercentStr(Str);
		SetGadgetString(_R(IDC_SELCOLOUR), &PercentStr);
	}
}

/********************************************************************************************

>	FIXED16 TranspInfoBarOp::FindCommonNoiseScale()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Find any Common Noise Scale in the selection.

********************************************************************************************/

double TranspInfoBarOp::FindCommonNoiseScale()
{
	if (Selection->Count() == 0)
	{
		AttrFillGeometry* pCurrentFill = GetCurrentGeometry();

		if (pCurrentFill && IS_A(pCurrentFill, AttrNoiseTranspFill))
		{
			return ((FractalTranspFillAttribute*)pCurrentFill->GetAttributeValue())->GetGraininess().MakeDouble();
		}

		return 0;
	}

	// Find the first Fill Attribute in the selection
	AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));

	// Return if there aren't any
	if (pAttrNode == NULL)
		return 0;

	double CommonScale = 0;

	while (pAttrNode != NULL)
	{
		if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrNoiseTranspFill)))
		{
			double Scale = ((FractalTranspFillAttribute*)pAttrNode->GetAttributeValue())->GetGraininess().MakeDouble();

			if (CommonScale == 0)
				CommonScale = Scale;

			if (Scale != CommonScale)
		   	{
				return -1; // Many
			}
		}

		// Check the next fill
		pAttrNode = AttrFillGeometry::FindNextSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));
	}

	return CommonScale;
}

/********************************************************************************************

>	void TranspInfoBarOp::ShowControlPointInfo()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Purpose:	Show Info about selected control points.

********************************************************************************************/

void TranspInfoBarOp::ShowControlPointInfo()
{	
	String_64 PointStr;
	INT32 Transp;

	// Look though the selection for any selected control points
	ScanSelectionForControlPoints(&PointStr, &Transp);

	// PointStr and ColourStr will be updated if we find any

	if (Transp > 255) Transp = 255;
	if (Transp < 0)   Transp = 0;

	// Show the selected control points state
	if ((Mode != BITMAPFILL && 
		 Mode != FRACTALFILL &&
		 Mode != NOISEFILL) ||	AttrFillGeometry::SelectionCount > 0)
	{	
		SetGadgetString(_R(IDC_SELPOINT), &PointStr);
	}

	//TRACEUSER( "Chris", _T("Set slider to Trans = %d\n"), Transp);
	SetLongGadgetValue(_R(IDC_TRANSPSLIDE), Transp);
	//double TempNewVal = ((double) Transp * 100) / (double) SliderMax;
	DisplayTransparencyPercentString ((double) Transp);	// and display it

	//INT32 Percent = (Transp * 100) / SliderMax;
	//String_64 Str;

	/*double TempNewVal = ((double) Transp * 100) / (double) SliderMax;
	String_64 Str;

	Convert::DoubleToString( TempNewVal, &Str, 1 );

	if (Str.FindNextChar ((TCHAR) '.') == -1)
	{
		// need to wack a .0 in ....

		Str += ".0";
	}
	
	Str.MakePercent ((TCHAR*) Str);

	String_64 PercentStr(Str);
	SetGadgetString(_R(IDC_SELCOLOUR), &PercentStr);*/
}

/********************************************************************************************

>	void TranspInfoBarOp::ScanSelectionForControlPoints(String_64* PointString, 
														String_64* ColourString)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Purpose:	Look for selected control points within the selection.

********************************************************************************************/

void TranspInfoBarOp::ScanSelectionForControlPoints(String_64* PointString, INT32* Transp)
{
	*PointString = _T("");
	*Transp = -1;

	// Find the first Fill Attribute in the selection
	AttrFillGeometry* pAttrNode;
	UINT32 count = Selection->Count();

	// Find the first Fill Attribute in the selection
	if (count == 0)
	{
		pAttrNode = GetCurrentGeometry();
	}
	else
	{
		pAttrNode = AttrFillGeometry::FindFirstSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));
	}

	AllowForceToSimpleMapping = TRUE;

	// Return if there aren't any
	if (pAttrNode == NULL)
	{
		(*PointString).Load(_R(IDS_FILLTOOL_NONE));
		*Transp = 0;
		return;
	}
	
	INT32 SelCount = 0;
	BOOL CommonTransp = TRUE;
	BOOL CommonPoint  = TRUE;

	BOOL AllFlat = TRUE;
	BOOL AnyNeedSimple = FALSE;

	while (pAttrNode != NULL)
	{
		if (AttrFillGeometry::FillSelectionCount() > 0)
		{
			if (!(pAttrNode->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrFlatTranspFill)))
			{
				// Get the selection state and colours of this fill
				String_64 PointStr = GetSelectedPoint(pAttrNode);
				UINT32 HandleTransp  = GetSelectedTransp(pAttrNode);

				String_64 Str;
				Str.Load(_R(IDS_FILLTOOL_NONE));

				// Where there any points selected ?
				if (PointStr != Str)
				{
					// Update and keep a count
					*PointString = PointStr;
					SelCount++;

					if (*PointString == String_64(_T("")))
						*PointString = PointStr;

					if (*PointString != PointStr)
						CommonPoint = FALSE;

					if (*Transp == -1)
                    	*Transp = HandleTransp;

					if (*Transp != INT32(HandleTransp))
						CommonTransp = FALSE;
	
					AllFlat = FALSE;
				}
			}
		}
		else 
		{
			if (pAttrNode->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrFlatTranspFill))
			{
				if ( !((pAttrNode->GetTranspType() == TT_Mix) && ((*pAttrNode->GetStartTransp()) == 0)) )
				{
					(*PointString).Load(_R(IDS_FILLTOOL_FLATTRANSP));

					if (*Transp == -1)
		            	*Transp = *pAttrNode->GetStartTransp();

					if (*Transp != INT32(*pAttrNode->GetStartTransp()))
						CommonTransp = FALSE;

					SelCount++;
				}
			}
			else
			{
				if (SelCount > 0)
					AllFlat = FALSE;
			}
		}

		// If this is a transp fill attribute and doesn't need simple mapping then set FALSE
		if (pAttrNode->IsATranspFill() && pAttrNode->NeedsForceToSimpleMapping())
			AnyNeedSimple = TRUE;
			
		// Check the next fill
		if (count > 0)
			pAttrNode = AttrFillGeometry::FindNextSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));
		else
			pAttrNode = NULL;
	}

	if (AnyNeedSimple)
		AllowForceToSimpleMapping = FALSE;

	if (AttrFillGeometry::FillSelectionCount() == 0 && !AllFlat)
	{
		(*PointString).Load(_R(IDS_FILLTOOL_MANY));
	}

	if (SelCount == 0)
	{
		// Status indicating many points selected
		(*PointString).Load(_R(IDS_FILLTOOL_NONE));
	}

	// Was there more than one fill with points selected ?
	if (!CommonPoint)
	{
		// Status indicating many points selected
		(*PointString).Load(_R(IDS_FILLTOOL_MANY));
	}

	if (!CommonTransp)
	{
		*Transp = 0;
	}
}

/********************************************************************************************

>	String_64 TranspInfoBarOp::GetSelectedPoint(AttrFillGeometry* pGeometry)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Outputs:	-
	Returns:	A String_64, "None", "Many", or "Start Colour", "End Colour" etc.
	Purpose:	Gets a textual description of a fills contol point selection state.

********************************************************************************************/

String_64 TranspInfoBarOp::GetSelectedPoint(AttrFillGeometry* pGeometry)
{
	String_64 Str;

	// how many	happen to be selected at the moment?
	UINT32 SelCount = pGeometry->GetSelectionCount();

	// if there's nothing selected then we're done
	if (SelCount<1)
	{
		Str.Load(_R(IDS_FILLTOOL_NONE));
		return Str;
	}

	// more than one?	
	if (SelCount>1)
	{
		Str.Load(_R(IDS_FILLTOOL_MANY));
		return Str;
	}

	// ramp blobs have negative indexes
	INT32 i = pGeometry->GetFirstSelectedIndex();
	if (ISA_RAMPINDEX(i))
	{
		Str.Load(_R(IDS_FILLTOOL_RAMPTRANSP));
		return Str;
	}

	// ok its an end blob so which one is it?
	switch (i)
	{
	  	case FILLCONTROL_STARTPOINT:
			Str.Load(_R(IDS_FILLTOOL_STARTTRANS));
			break;

		case FILLCONTROL_ENDPOINT:
	  	case FILLCONTROL_SECONDARYPOINT:
			Str.Load(_R(IDS_FILLTOOL_ENDTRANS));
			break;

		case FILLCONTROL_ENDPOINT2:
			Str.Load(_R(IDS_FILLTOOL_ENDTRANS2));
			break;

		case FILLCONTROL_ENDPOINT3:
			Str.Load(_R(IDS_FILLTOOL_ENDTRANS3));
			break;

		default:
			Str.Load(_R(IDS_FILLTOOL_NONE));
			break;
	}

	return Str;
}


/*
String_64 TranspInfoBarOp::GetSelectedPoint(AttrFillGeometry* pGeometry)
{
	BOOL SelState[5];
	String_64 Str;

	// Get the selection state of this fills control points
	pGeometry->GetSelectionState(SelState, 5);

	INT32 SelCount = 0;
	for (INT32 i=0; i<5; i++)
	{
		// Is this point selected ?
		if (SelState[i])
		{
			// Get a description of the Point
			switch (i)
			{
	  			case FILLCONTROL_STARTPOINT:
					Str.Load(_R(IDS_FILLTOOL_STARTTRANS));
					break;

				case FILLCONTROL_ENDPOINT:
					Str.Load(_R(IDS_FILLTOOL_ENDTRANS));
					break;

				case FILLCONTROL_ENDPOINT2:
					Str.Load(_R(IDS_FILLTOOL_ENDTRANS2));
					break;

				case FILLCONTROL_ENDPOINT3:
					Str.Load(_R(IDS_FILLTOOL_ENDTRANS3));
					break;

	  			case FILLCONTROL_SECONDARYPOINT:
					Str.Load(_R(IDS_FILLTOOL_ENDTRANS));
					break;
			}
			// Keep a count of how many points were selected
			SelCount++;
		}
	}

	if (SelCount == 0)
		Str.Load(_R(IDS_FILLTOOL_NONE));	// No control points were selected

	if (SelCount > 1)
		Str.Load(_R(IDS_FILLTOOL_MANY));  	// Many control points were selected

	return Str;
}
*/

/********************************************************************************************

>	String_64 TranspInfoBarOp::GetSelectedColour(AttrFillGeometry* pGeometry)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Outputs:	-
	Returns:	A String_64, "None", "Many", or "Colour Name".
	Purpose:	Gets a textual description of a fills contol point colour.

********************************************************************************************/

UINT32 TranspInfoBarOp::GetSelectedTransp(AttrFillGeometry* pGeometry)
{
	UINT32* pStartTransp = pGeometry->GetStartTransp();
	if (pStartTransp == NULL)
		return 0;	// Not a transparency fill !

	UINT32* pEndTransp = pGeometry->GetEndTransp();
	UINT32 SelCount = pGeometry->GetSelectionCount();

	// if there's nothing selected then we're done
	if (SelCount<1)
		return 0;

	if (SelCount>1)
	{
		if (pEndTransp!=NULL && ((*pStartTransp) != (*pEndTransp)))
			return 0;
	}

	// ok, only one thing selected so go get it
	pStartTransp = pGeometry->GetFirstSelectedTransp();
	if (pStartTransp)
		return (*pStartTransp);

	return 0;
}

/********************************************************************************************

>	void TranspInfoBarOp::EnableControls()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (rewritten by Chris Snook)
	Created:	3/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Enables or disables the controls depending on the current context, i.e.
				the controls are disabled if there is no selection, etc.
				Actually more complicated than this now, cause my function calls now hide/show
				various controls; and dynamically reshuffle the infobar.

********************************************************************************************/

void TranspInfoBarOp::EnableControls()
{
	// DMc - check for nodes which transparency can't be applied to
	BOOL allowed = TRUE;

	SelRange * pSel = GetApplication()->FindSelection();

	Node * pNode = pSel->FindFirst(FALSE);

	while (pNode && allowed)
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeShadow)))
		{
			allowed = FALSE;
		}

		pNode = pSel->FindNext(pNode, FALSE);
	}
	
	if (Document::GetSelected() == NULL || !allowed)
	{
		EnableGadget (_R(IDC_GEOMETRY),		FALSE);
		EnableGadget (_R(IDC_MAPPING),		FALSE);
		EnableGadget(_R(IDC_TRANSPTYPE),	FALSE);
		EnableGadget (_R(IDC_BITMAPEFFECT), FALSE);
		EnableGadget (_R(IDC_BIASGAIN),		FALSE);
		EnableGadget (_R(IDC_SELPOINT),		FALSE);
		EnableGadget (_R(IDC_TRANSPSLIDE),	FALSE);
		EnableGadget (_R(IDC_SELCOLOUR),	FALSE);
		//EnableGadget(_R(IDC_GRADCOORDS),	FALSE);
	}
	else
	{

		EnableGadget (_R(IDC_GEOMETRY), TRUE);
		EnableGadget (_R(IDC_TRANSPTYPE),	TRUE);
		EnableGadget (_R(IDC_TRANSPSLIDE), TRUE);
		EnableGadget (_R(IDC_SELCOLOUR), TRUE);
				
		switch (Mode)
		{
			case NOFILL:

				EnableGadget (_R(IDC_MAPPING), FALSE);
				EnableGadget (_R(IDC_BITMAPEFFECT), FALSE);
				EnableGadget (_R(IDC_BIASGAIN), FALSE);
				EnableGadget (_R(IDC_SELPOINT), FALSE);
				EnableGadget (_R(IDC_TRANSPSLIDE), TRUE);
				EnableGadget (_R(IDC_SELCOLOUR), TRUE);
			break;
			
			case MANYFILLS:
			case FLATFILL:
				// nothing else to enable ....
				EnableGadget (_R(IDC_MAPPING), FALSE);
				EnableGadget (_R(IDC_BITMAPEFFECT), FALSE);
				EnableGadget (_R(IDC_BIASGAIN), FALSE);
				EnableGadget (_R(IDC_SELPOINT), FALSE);

				if (Mode == MANYFILLS)
				{
					EnableGadget (_R(IDC_TRANSPSLIDE), FALSE);
					EnableGadget (_R(IDC_SELCOLOUR), FALSE);
				}

				CloseProfileDialog (m_BiasGainGadget);		// cause fill type cannot be profiled!
			break;

			case GRADFILL:
			case GRADREPEATFILL:
				switch (CurrentGeometryIndex)
				{
					case FGMENU_LINEARTRANSP:
					case FGMENU_CIRCULARTRANSP:
					case FGMENU_RADIALTRANSP:
					// the following is really fg_diamond ....
					case FGMENU_SQUARETRANSP:

						EnableGadget (_R(IDC_MAPPING), TRUE);
						EnableGadget (_R(IDC_BITMAPEFFECT), FALSE);

						//if (2 colour)
						//{
							EnableGadget (_R(IDC_BIASGAIN), TRUE);
						//}
						//else
						//{
						//	EnableGadget (_R(IDC_BIASGAIN), FALSE);
						//}

						EnableGadget (_R(IDC_SELPOINT), FALSE);
					break;
					
					case FGMENU_CONICALTRANSP:
						EnableGadget (_R(IDC_MAPPING), FALSE);
						EnableGadget (_R(IDC_BITMAPEFFECT), FALSE);

						//if (2 colour)
						//{
							EnableGadget (_R(IDC_BIASGAIN), TRUE);
						//}
						//else
						//{
						//	EnableGadget (_R(IDC_BIASGAIN), FALSE);
						//}
						
						EnableGadget (_R(IDC_SELPOINT), FALSE);
					break;
					
					case FGMENU_THREECOLTRANSP:
					case FGMENU_FOURCOLTRANSP:
						EnableGadget (_R(IDC_MAPPING), TRUE);
						EnableGadget (_R(IDC_BITMAPEFFECT), FALSE);
						EnableGadget (_R(IDC_BIASGAIN), FALSE);
						EnableGadget (_R(IDC_SELPOINT), FALSE);
					break;
				}
			break;

			case BITMAPFILL:
				EnableGadget (_R(IDC_MAPPING), TRUE);
				EnableGadget (_R(IDC_BITMAPEFFECT), TRUE);

				if (AttrFillGeometry::SelectionCount == 0)
				{
					EnableGadget (_R(IDC_TRANSPSLIDE), FALSE);
					EnableGadget (_R(IDC_SELCOLOUR), FALSE);
				}
				
				//if (2 colour)
				//{
					//EnableGadget (_R(IDC_BIASGAIN), TRUE);
				//}
				//else
				//{
					EnableGadget (_R(IDC_BIASGAIN), TRUE);//FALSE);
				//}

				//EnableGadget (_R(IDC_SELPOINT), FALSE);

				if (GetGadgetWritable (_R(IDC_SELPOINT)))
				{
					EnableGadget (_R(IDC_SELPOINT), FALSE);
				}
				else
				{
					EnableGadget (_R(IDC_SELPOINT), TRUE);
				}
			break;

			case FRACTALFILL:
			case NOISEFILL:
				EnableGadget (_R(IDC_MAPPING), TRUE);
				EnableGadget (_R(IDC_BITMAPEFFECT), FALSE);

				//if (2 colour)
				//{
					//EnableGadget (_R(IDC_BIASGAIN), TRUE);
				//}
				//else
				//{
					EnableGadget (_R(IDC_BIASGAIN), TRUE);//FALSE);
				//}

				if (GetGadgetWritable (_R(IDC_SELPOINT)))
				{
					EnableGadget (_R(IDC_SELPOINT), FALSE);
				}
				else
				{
					EnableGadget (_R(IDC_SELPOINT), TRUE);
				}
				
				/*if (GetGadgetWritable (_R(IDC_SELCOLOUR)))
				{
					EnableGadget (_R(IDC_SELCOLOUR), FALSE);
				}
				else
				{
					EnableGadget (_R(IDC_SELCOLOUR), TRUE);
				}*/
			break;

			default:
			break;
		}


/*		BOOL enable = TRUE;
		EnableGadget(_R(IDC_GEOMETRY),		enable);

//		if (Mode == NOFILL)
//			enable = FALSE;

		if (TypeDisabled == enable)
		{
			TypeDisabled = !enable;
			//EnableGadget(_R(IDC_TRANSPTYPE),	enable);
		}

		enable = FALSE;
		if (Mode == BITMAPFILL || 
			Mode == FRACTALFILL || 
			Mode == GRADREPEATFILL ||
//Mark Howitt, 14/10/97. Implement repeating transparent grad fills, except conical
#ifdef NEW_FEATURES
			(Mode == GRADFILL && CurrentGeometryIndex != FGMENU_CONICALTRANSP) ||
#endif
			Mode == NOISEFILL)
		{
			enable = TRUE;
		}

		if (MappingDisabled == enable)
		{
			MappingDisabled = !enable;
			EnableGadget(_R(IDC_MAPPING),		enable);
		}

//		if (CommonAttr != SelRange::ATTR_NONE)
			enable = TRUE;
//		else
//	    	enable = FALSE;

		EnableGadget(_R(IDC_SELPOINT),		enable);
		EnableGadget(_R(IDC_TRANSPSLIDE),	enable);
	//	EnableGadget(_R(IDC_SELPOINT2),		enable);
	//	EnableGadget(_R(IDC_SELCOLOUR),		enable);
	//	EnableGadget(_R(IDC_SELCOLOURBLOB),	enable);

	//	enable = FALSE;
	//	EnableGadget(_R(IDC_GRADCOORDS),	enable);
	//	EnableGadget(_R(IDC_GRADCOORDS2),	enable);*/
	}

}           

/********************************************************************************************

>	void TranspInfoBarOp::SetTranspValue(UINT32 Transp, BOOL bIntermediateStep = FALSE, BOOL bFirstStep = FALSE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/94
	Purpose:	Called when a the transparency slider is changed.

********************************************************************************************/

void TranspInfoBarOp::SetTranspValue(UINT32 Transp, BOOL bIntermediateStep, BOOL bFirstStep)
{
/*	AttrValueChange* NewTransp = new AttrTranspChange;
	if (NewTransp == NULL)
	{
		InformError();
		return;
	}

	NewTransp->SetStartTransp(&Transp);

	Operation::SetQuickRender(bIntermediateStep);

		AttributeManager::AttributeSelected(NULL, NewTransp, bIntermediateStep, bFirstStep);

	Operation::SetQuickRender(FALSE);
*/

	if (bFirstStep)
	{
		// ---------------------------------------------
		// Drag starting
		ERROR3IF(m_pAttrApplyOp!=NULL, "Why do we have a drag op already?\n");
		if (m_pAttrApplyOp==NULL)
		{
			m_pDragTransp = new AttrTranspChange;
			if (m_pDragTransp == NULL)
				return;

			m_pDragTransp->SetStartTransp(&Transp);

			m_pAttrApplyOp = new OpApplyAttrInteractive;
			m_pAttrApplyOp->DoDragAttr(m_pDragTransp);				// Drag starting
			ERROR3IF(m_pAttrApplyOp!=Operation::GetCurrentDragOp(),
					 "DoDragAttr should have set itself as the current drag op");
		}
	}
	else if (bIntermediateStep)
	{
		// ---------------------------------------------
		// Drag running
		m_pDragTransp->SetStartTransp(&Transp);

		ERROR3IF(m_pAttrApplyOp==NULL, "Why don't we have a drag op yet?\n");
		if (m_pAttrApplyOp && m_pAttrApplyOp==Operation::GetCurrentDragOp())
			m_pAttrApplyOp->DragAttrChanged(m_pDragTransp);			// Drag running

	}
	else
	{
		// ---------------------------------------------
		// Drag ending (or single-click or too many objects to drag interactively)
		if (m_pAttrApplyOp==NULL)
		{
			m_pDragTransp = new AttrTranspChange;
			if (m_pDragTransp == NULL)
				return;

			m_pDragTransp->SetStartTransp(&Transp);

			m_pAttrApplyOp = new OpApplyAttrInteractive;
			m_pAttrApplyOp->DoDragAttr(m_pDragTransp);				// Drag starting
		}

		m_pDragTransp->SetStartTransp(&Transp);

		ERROR3IF(m_pAttrApplyOp==NULL, "Why don't we have a drag op yet?\n");
		if (m_pAttrApplyOp)
			if (m_pAttrApplyOp==Operation::GetCurrentDragOp())
			{
				m_pAttrApplyOp->DragAttrFinished(m_pDragTransp);		// Drag over
				// Don't delete the op - it lives in the undo history now!
				// delete m_pAttrApplyOp;
				// But do release ownership so that we can start another one
				m_pAttrApplyOp = NULL;
			}
			else
			{
				// Drag op must have been cancelled and deleted already (because
				// CurrentDragOp is NULL)
				ShowInfo();						// Update UI
				m_pAttrApplyOp = NULL;			// Release invalid pointer
			}

		if (m_pDragTransp)
		{
			delete m_pDragTransp;
			m_pDragTransp = NULL;
		}
	}
}

/********************************************************************************************

>	void TranspInfoBarOp::ChangeFillType()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Purpose:	Called when a new fill type is selected from the menu.

********************************************************************************************/

//if (Selection->Count() == 0)
	//{
	//	AttrFillGeometry* pCurrentFill = GetCurrentGeometry();

		//AttrTranspFillGeometry* pCurrentFill = GetCurrentGeometry();

		// NOTE:  we can call either IsATranspFill or IsABitmapFill here ....
		// BOTH return TRUE, the only difference is that IsABitmapFill is a base
		// class member function (AttrBitmapFill::) and IsATranspFill is a 'true'
		// class function (i.e.  defined and implemented in AttrBitmapTranspFill)
		// BUT, lets stick with IsABitmapFill (for clarity) - cause after all,
		// were dealing with bitmaps here ....

	//	if (pCurrentFill && pCurrentFill->/*IsATranspFill*/IsABitmapFill())
	/*	{
			CommonName = pCurrentFill->GetBitmap()->ActualBitmap->GetName();
		}

		return CommonName;
	}

	String_256 Name;
	Name.Load(_R(IDS_FILLTOOL_NONE));
	
	String_256 None;
	None.Load(_R(IDS_FILLTOOL_NONE));

	String_256 Many;
	Many.Load(_R(IDS_FILLTOOL_MANY));

	// Find the first Fill Attribute in the selection (of the right type!)
	// (observe the CC_RUNTIME_CLASS(AttrTranspFillGeometry) 
	AttrFillGeometry* pAttrNode = AttrTranspFillGeometry::FindFirstSelectedAttr (CC_RUNTIME_CLASS(AttrTranspFillGeometry));

	// Return if there aren't any
	if (pAttrNode == NULL)
		return None;

	while (pAttrNode != NULL)
	{
		if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrBitmapTranspFill)))
		{
			Name = pAttrNode->GetBitmap()->ActualBitmap->GetName();

			if (CommonName == None)
				CommonName = Name;

			if (Name != CommonName)
		   	{
				return Many;
			}
		}

		// Check the next fill (of the right type!)
		pAttrNode = AttrTranspFillGeometry::FindNextSelectedAttr(CC_RUNTIME_CLASS(AttrTranspFillGeometry));
	}

	return CommonName;*/

void TranspInfoBarOp::ChangeFillType()
{
	if (CurrentGeometryIndex == FGMENU_NOTRANSP)
	{
		AttrFillGeometry* NoTransp = new AttrRemoveTransp;
		if (NoTransp == NULL)

		{
			InformError();
			return;
		}

		AttributeManager::AttributeSelected(NULL, NoTransp);
	}	
	else if (CurrentGeometryIndex == FGMENU_FLATTRANSP)
	{
		AttrFillGeometry* FlatTransp = new AttrMakeFlatTransp;
		if (FlatTransp == NULL)
		{
			InformError();
			return;
		}

		AttributeManager::AttributeSelected(NULL, FlatTransp);
	}	
	else
	{
		// Get the current fill type, and pass a new fill of that type into
		// the attribute manager, as a mutator.
		AttrFillGeometry* Mutator = MakeFillMutator();
		if (Mutator == NULL)
		{
			InformError();
			return;
		}

		// If the mutator requires a simple mapping but the current fill doesn't
		// then we need to set ForceToSimple to TRUE

		//BOOL ForceToSimple = Mutator->NeedsForceToSimpleMapping() && AllowForceToSimpleMapping;

		// This call deletes the Mutator so I'll set the pointer to NULL
		AttributeManager::AttributeSelected(NULL, Mutator);
		Mutator = NULL;

		//if (ForceToSimple)
		//{
	//		CurrentMappingIndex = FMMENU_SIMPLE;
	//		ChangeFillMapping ();			// we push two bits of undo here, but this is
											// unavoidable at present ....
		//}
	}

	if (CurrentGeometryIndex != FGMENU_NOTRANSP)
		UpdateBrushAttributes();
}

/********************************************************************************************

>	void TranspInfoBarOp::ChangeFillMapping()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/94
	Purpose:	Called when a new fill mapping or tesselation is selected from the menu.

********************************************************************************************/

void TranspInfoBarOp::ChangeFillMapping()
{
	if (Mode != BITMAPFILL && 
	    Mode != FRACTALFILL && 
		Mode != GRADREPEATFILL &&
//Mark Howitt, 14/10/97. Exclude transparent grad fills as well
#ifdef NEW_FEATURES
		Mode != GRADFILL &&
#endif
		Mode != NOISEFILL)
	{
		return;
	}

	INT32 Tesselation;

	switch (CurrentMappingIndex)
	{
	 	case (FTMENU_SIMPLE):
			Tesselation = 1;
			break;
   	
	 	case (FTMENU_REPEAT):
//Mark Howitt, 14/10/97. Repeating grad fills are now 4.
#ifdef NEW_FEATURES
			if(Mode == GRADFILL)
			{
				Tesselation = 4;
			}
			else
#endif
			{
				Tesselation = 2;
			}
			break;

	 	case (FTMENU_REPEATINV):
			Tesselation = 3;
			break;

		default:
			Tesselation = 1;
			break;
	}

	AttrTranspFillMappingLinear* NewMapping = new AttrTranspFillMappingLinear;
	if (NewMapping == NULL)
	{
		InformError();
		return;
	}

	NewMapping->SetRepeat(Tesselation);
	AttributeManager::AttributeSelected(NewMapping, NULL);
}


/********************************************************************************************

>	void TranspInfoBarOp::UpdateBrushAttributes()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/2000
	Purpose:	Called when a new transparency is selected.  This function goes through all
				brush attributes in the selection and tells them to use the local transparency
				rather than their cached one.

********************************************************************************************/

void TranspInfoBarOp::UpdateBrushAttributes()
{
	List BrushList;
//	BevelTools::BuildListOfSelectedNodes(&BrushList, CC_RUNTIME_CLASS(AttrBrushType));
	FillTools::GetSelectedAttrList(&BrushList, CC_RUNTIME_CLASS(AttrBrushType), FALSE, FALSE);

	NodeListItem* pItem = (NodeListItem*)BrushList.GetHead();
	AttrBrushType* pAttrBrush = NULL;
	while (pItem != NULL)
	{
		pAttrBrush = (AttrBrushType*)(pItem->pNode);
		if (pAttrBrush != NULL)
			pAttrBrush->SetUseLocalTransp(TRUE);

		pItem = (NodeListItem*)BrushList.GetNext(pItem);
	}

	BrushList.DeleteAll();
}

/********************************************************************************************

>	void TranspInfoBarOp::ChangeBitmapName()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (ported to the transparency tool by Chris Snook 12/10/99)
	Created:	14/12/94
	Purpose:	Called when a new bitmap is selected from the menu.

********************************************************************************************/

BOOL TranspInfoBarOp::ChangeBitmapName()
{
	NodeAttribute *Attrib = new AttrBitmapTranspFill;
	if (Attrib == NULL)
	{
		InformError();
		return (FALSE);
	}

	Document* pDoc = Document::GetSelected();

	// Get the bitmap list
	BitmapList* Bitmaps = NULL;
	if (pDoc) Bitmaps = pDoc->GetBitmapList();

	if (Bitmaps == NULL)
		return (FALSE);

	INT32 BmpIndex = -1;
	ListItem* pBmp = Bitmaps->GetHead();

	while (pBmp != NULL)
	{
		if (!((KernelBitmap*)pBmp)->HasBeenDeleted())
			BmpIndex++;

		if (BmpIndex == CurrentBitmapIndex)
			break;

		pBmp = Bitmaps->GetNext(pBmp);
	}

	KernelBitmap* pkBmp = (KernelBitmap*) pBmp;

	// CGS:  check for a 32-bit bitmap being applied here as a transparency, since we convert
	// this permantly to 24-bit we should ask the user if they wish to continue ....

	if (pkBmp->ActualBitmap->GetBitmapInfoHeader()->biBitCount == 32)
	{
		// Ask the user wether or not they want to use a 24bit copy of the BMP or use the Default Bitmap?
		//InformWarning(_R(IDS_BFX_BMP_CONVERT_MSG),_R(IDS_OK),0,0,0,1,2);

		// Load and build the question text.
		String_256 QueryString(_R(IDS_QUERYTRANSP321));
					
		// The only way of bringing up a box with a string in it
		Error::SetError(0, QueryString, 0);
		INT32 DlgResult = InformMessage(FALSE, _R(IDS_YES), _R(IDS_NO));
		Error::ClearError();

		switch (DlgResult)
		{
			case 1:				// YES
								// apply attribute ....
			break;
			case 2:				// NO
				delete (Attrib);
				Attrib = NULL;
				return (FALSE);			// break out of this stuff!
		}
	}

	((AttrBitmapChange*)Attrib)->AttachBitmap((KernelBitmap*)pBmp);
	AttributeManager::AttributeSelected(NULL, Attrib);

	return (TRUE);
}

/********************************************************************************************

>	void TranspInfoBarOp::ChangeTranspType()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Purpose:	Called when a new fill type is selected from the menu.

********************************************************************************************/

void TranspInfoBarOp::ChangeTranspType()
{
	// Get the current fill type, and pass a new fill of that type into
	// the attribute manager, as a mutator.
	AttrFillGeometry* TypeChange = new AttrTranspTypeChange();

	
	//AttrPaintingMode* TypeChange = new AttrPaintingMode ();
	//AttrTranspTypeChange* TypeChnage = new AttrTranspTypeChange ();
	if (TypeChange == NULL)
	{
		InformError();
		return;
	}

	UINT32 TType = GetTranspType() - 1;
	UINT32 Index = 0;
	
	switch(TType)
	{
	case FTMENU_REFLECT:
	case FTMENU_SUB:
	case FTMENU_ADD:
		{
			Index = TType + TT_Mix;
			break;
		}
	case FTMENU_CON:
		{
			Index = TT_CONTRAST;
			break;
		}
	case FTMENU_SAT:
		{
			Index = TT_SATURATION;
			break;
		}
	case FTMENU_DARK:
		{
			Index = TT_DARKEN;
			break;
		}
	case FTMENU_LIGHT:
		{
			Index = TT_LIGHTEN;
			break;
		}
	case FTMENU_BRI:
		{
			Index = TT_BRIGHTNESS;
			break;
		}
	case FTMENU_LUM:
		{
			Index = TT_LUMINOSITY;
			break;
		}
	case FTMENU_HUE:
		{
			Index = TT_HUE;
			break;
		}
	default:
		{
			Index = TT_NoTranspType;
		}
	}

	TypeChange->SetTranspType(Index);

	//TypeChange->SetValue (Index);

	//AttributeManager::AttributeSelected(TypeChange, NULL);
	AttributeManager::AttributeSelected(NULL, TypeChange);


/*
	// And also change the line transparency type
	TypeChange = new AttrStrokeTranspTypeChange();
	if (TypeChange == NULL)
	{
		InformError();
		return;
	}

	TypeChange->SetTranspType(GetTranspType());

	AttributeManager::AttributeSelected(NULL, TypeChange);
*/
}


/********************************************************************************************

>	void TranspInfoBarOp::ChangeDPI()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/94
	Purpose:	Called when the dpi of a bitmap is changed by the user.

********************************************************************************************/

void TranspInfoBarOp::ChangeDPI()
{
	BOOL Valid;
	String_256 Str = GetStringGadgetValue(_R(IDC_SELPOINT), &Valid);

	if (Valid)
	{
		TRACEUSER( "Will", _T("String = %s\n"),(TCHAR*)Str);

		INT32 Length = Str.Length();
		TCHAR* StrText = (TCHAR*)Str;

		INT32 FirstNum = -1;
		INT32 Count = 0;

		for (INT32 i = 0; i < Length; ++i)
		{
			TCHAR chr = StrText[i];
			if (FirstNum == -1 && StringBase::IsNumeric(chr))
				FirstNum = i;

			if (FirstNum != -1)
			{
				if (StringBase::IsNumeric(chr))
					Count++;
				else
					break;
			}
		}

		String_256 DpiStr;
		INT32 Dpi;

		if (FirstNum != -1 && Count > 0)
		{
			Str.Mid(&DpiStr, FirstNum, Count);
			Convert::StringToLong(DpiStr, &Dpi);
			TRACEUSER( "Will", _T("Dpi = %d\n"),Dpi);

			if (Dpi == 0)
				Dpi = 1;

			AttrValueChange* NewDpi = new AttrBitmapDpiChange;
			if (NewDpi == NULL)
			{
				InformError();
				return;
			}

			NewDpi->MutateTranspFills(TRUE);
			((AttrBitmapDpiChange*)NewDpi)->SetDPI(Dpi);

			AttributeManager::AttributeSelected(NULL, NewDpi);
		}
		else
		{
			TRACEUSER( "Will", _T("That was rubbish\n"));
			ShowCommonBitmapDpi();
		}
	}
	else
		TRACEUSER( "Will", _T("String was invalid\n"));
}

/********************************************************************************************

>	void TranspInfoBarOp::ChangeTransparencyValue()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/99
	Purpose:	Called when the transparency value is changed by the user.

********************************************************************************************/

void TranspInfoBarOp::ChangeTransparencyValue()
{
	BOOL Valid;
	String_256 Str = GetStringGadgetValue(_R(IDC_SELCOLOUR), &Valid);

	if (Valid)
	{
		INT32 Length = Str.Length();
		TCHAR* StrText = (TCHAR*)Str;

		INT32 FirstNum = -1;
		INT32 Count = 0;

		BOOL FoundDp = FALSE;

		for (INT32 i = 0; i < Length; ++i)
		{
			TCHAR chr = StrText[i];
			if (FirstNum == -1 && StringBase::IsNumeric(chr))
				FirstNum = i;

			if (FirstNum != -1)
			{
				if (StringBase::IsNumeric(chr))
				{
					Count++;
					
					//if (FoundDp == TRUE) { break; }
				}
				else if ((chr == '.') && (FoundDp == FALSE))
				{
					FoundDp = TRUE;
					Count++;
				}
				else
					break;
			}
		}

		String_256 TranspValStr;
		double TranspVal;

		if (FirstNum != -1 && Count > 0)
		{
			Str.Mid(&TranspValStr, FirstNum, Count);
			Convert::StringToDouble(TranspValStr, &TranspVal);
			
			// check for out of range stuff ....

			if (TranspVal < 0.0) { TranspVal = 0.0; }
			if (TranspVal > 100.0) { TranspVal = 100.0; }
			
			// convert data ....

			UINT32 Rounded = (UINT32) TranspVal;
			double Diff = TranspVal - Rounded;

			if (Diff < 0.5)
			{
				if (Diff < 0.25) { TranspVal = (double) Rounded; }
				else { TranspVal = (double) Rounded + 0.5; }
			}
			else
			{
				if (Diff < 0.75) { TranspVal = (double) Rounded + 0.5; }
				else { TranspVal = (double) ++Rounded; }
			}

			double AccurateNewVal = ((TranspVal * (double) SliderMax) / 100.0) + 0.5;

			SetTranspValue (static_cast<UINT32>(AccurateNewVal));
		}
		else
		{
			ShowControlPointInfo ();
		}
	}
}

/********************************************************************************************

>	void TranspInfoBarOp::ChangeFractalGrain()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Purpose:	Called when the Graininess of a fractal is changed by the user.

********************************************************************************************/

void TranspInfoBarOp::ChangeFractalGrain (BOOL useEditbox /*= FALSE*/)
{
	if (!useEditbox)
	{
		BOOL Valid;
		INT32 Result = GetLongGadgetValue(_R(IDC_TRANSPSLIDE), 0, SliderMax, 0, &Valid);

		if (Valid)
		{
			double Grain = GetDoubleGrain(Result);	

			TRACEUSER( "Will", _T("Graininess = %f\n"),Grain);

			if (Grain < 0.1)
				Grain = 0.1;

			if (Grain > 16384)
				Grain = 16384;

			AttrValueChange* NewGrain = new AttrFractalGrainChange;
			if (NewGrain == NULL)
			{
				InformError();
				return;
			}
			
			NewGrain->MutateTranspFills(TRUE);
			((FractalFillAttribute*)NewGrain->GetAttributeValue())->Graininess = FIXED16(Grain);

			AttributeManager::AttributeSelected(NULL, NewGrain);
		}
	}
	else
	{
		BOOL Valid;
		String_256 Str = GetStringGadgetValue(_R(IDC_SELCOLOUR), &Valid);

		if (Valid)
		{
			INT32 Length = Str.Length();
			TCHAR* StrText = (TCHAR*)Str;

			INT32 FirstNum = -1;
			INT32 Count = 0;

			BOOL FoundDp = FALSE;

			for (INT32 i = 0; i < Length; ++i)
			{
				TCHAR chr = StrText[i];
				if (FirstNum == -1 && StringBase::IsNumeric(chr))
					FirstNum = i;

				if (FirstNum != -1)
				{
					if (StringBase::IsNumeric(chr))
					{
						Count++;
						
						//if (FoundDp == TRUE) { break; }
					}
					else if ((chr == '.') && (FoundDp == FALSE))
					{
						FoundDp = TRUE;
						Count++;
					}
					else
						break;
				}
			}

			String_256 TranspValStr;
			double TranspVal;

			if (FirstNum != -1 && Count > 0)
			{
				Str.Mid(&TranspValStr, FirstNum, Count);
				Convert::StringToDouble(TranspValStr, &TranspVal);
				
				// check for out of range stuff ....

				if (TranspVal < 0.0) { TranspVal = 0.0; }
				if (TranspVal > 128.0) { TranspVal = 128.0; }
				
				// convert data ....

				UINT32 Rounded = (UINT32) TranspVal;
				double Diff = TranspVal - Rounded;

				if (Diff < 0.5)
				{
					if (Diff < 0.25) { TranspVal = (double) Rounded; }
					else { TranspVal = (double) Rounded + 0.5; }
				}
				else
				{
					if (Diff < 0.75) { TranspVal = (double) Rounded + 0.5; }
					else { TranspVal = (double) ++Rounded; }
				}

				//double AccurateNewVal = ((TranspVal * (double) 128) / 128.0) + 0.5;
				//double AccurateNewVal = TranspVal; //+ 0.5;

				//UINT32 newVal = static_cast<UINT32>(AccurateNewVal);

				AttrValueChange* NewGrain = new AttrFractalGrainChange;
				if (NewGrain == NULL)
				{
					InformError();
					return;
				}
				
				NewGrain->MutateTranspFills(TRUE);
				((FractalFillAttribute*)NewGrain->GetAttributeValue())->Graininess = FIXED16(/*(INT32)*/ TranspVal);

				AttributeManager::AttributeSelected(NULL, NewGrain);
			}
			else
			{
				ShowFractalInfo ();
			}		
		}
	}
}

/********************************************************************************************

>	void TranspInfoBarOp::ChangeNoiseScale()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Purpose:	Called when the scale of a transparency is changed by the user.

********************************************************************************************/

void TranspInfoBarOp::ChangeNoiseScale (BOOL useEditbox /*= FALSE*/)
{
	if (!useEditbox)
	{
		BOOL Valid;
		INT32 Result = GetLongGadgetValue(_R(IDC_TRANSPSLIDE), 0, SliderMax, 0, &Valid);

		if (Valid)
		{
			double Scale = GetDoubleScale(Result);

			TRACEUSER( "Mike", _T("Scale = %f\n"),Scale);

			if (Scale < 0)
				Scale = 0;

			if (Scale > 100)
				Scale = 100;

			AttrValueChange* NewScale = new AttrNoiseScaleChange;
			if (NewScale == NULL)
			{
				InformError();
				return;
			}
			
			FIXED16 Grain = FIXED16(Scale);
			NewScale->MutateTranspFills(TRUE);
			((NoiseFillAttribute*)NewScale->GetAttributeValue())->SetGraininess(Grain);

			AttributeManager::AttributeSelected(NULL, NewScale);
		}
	}
	else
	{
		BOOL Valid;
		String_256 Str = GetStringGadgetValue(_R(IDC_SELCOLOUR), &Valid);

		if (Valid)
		{
			INT32 Length = Str.Length();
			TCHAR* StrText = (TCHAR*)Str;

			INT32 FirstNum = -1;
			INT32 Count = 0;

			BOOL FoundDp = FALSE;

			for (INT32 i = 0; i < Length; ++i)
			{
				TCHAR chr = StrText[i];
				if (FirstNum == -1 && StringBase::IsNumeric(chr))
					FirstNum = i;

				if (FirstNum != -1)
				{
					if (StringBase::IsNumeric(chr))
					{
						Count++;
						
						//if (FoundDp == TRUE) { break; }
					}
					else if ((chr == '.') && (FoundDp == FALSE))
					{
						FoundDp = TRUE;
						Count++;
					}
					else
						break;
				}
			}

			String_256 TranspValStr;
			double TranspVal;

			if (FirstNum != -1 && Count > 0)
			{
				Str.Mid(&TranspValStr, FirstNum, Count);
				Convert::StringToDouble(TranspValStr, &TranspVal);
				
				// check for out of range stuff ....

				if (TranspVal < 0.0) { TranspVal = 0.0; }
				if (TranspVal > 100.0) { TranspVal = 100.0; }
				
				// convert data ....

				UINT32 Rounded = (UINT32) TranspVal;
				double Diff = TranspVal - Rounded;

				if (Diff < 0.5)
				{
					if (Diff < 0.25) { TranspVal = (double) Rounded; }
					else { TranspVal = (double) Rounded + 0.5; }
				}
				else
				{
					if (Diff < 0.75) { TranspVal = (double) Rounded + 0.5; }
					else { TranspVal = (double) ++Rounded; }
				}

				double AccurateNewVal = ((TranspVal * (double) SliderMax) / 100.0) + 0.5;

				UINT32 newVal = static_cast<UINT32>(AccurateNewVal);
				
				//	SetTranspValue (static_cast<UINT32>(AccurateNewVal));

				AttrValueChange* NewScale = new AttrNoiseScaleChange;
				if (NewScale == NULL)
				{
					InformError();
					return;
				}
				
				FIXED16 Grain = FIXED16((INT32) newVal);
				NewScale->MutateTranspFills(TRUE);
				((NoiseFillAttribute*)NewScale->GetAttributeValue())->SetGraininess(Grain);

				AttributeManager::AttributeSelected(NULL, NewScale);
			}
			else
			{
				ShowNoiseInfo ();
			}		
		}
	}
}

/********************************************************************************************

>	AttrFillGeometry* TranspInfoBarOp::MakeFillMutator()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Purpose:	Make a fill that will be used to Mutate another fill, changing it's type 
				while retaining it's existing points and colours.
				The fill type returned depends on the fill type menu state.

********************************************************************************************/

AttrFillGeometry* TranspInfoBarOp::MakeFillMutator()
{
	AttrFillGeometry* Fill = NULL;
	KernelBitmap* Default;

	switch (CurrentGeometryIndex)
	{
	 	case (FGMENU_FLATTRANSP):
			Fill = new AttrFlatTranspFill;
			break;

	 	case (FGMENU_LINEARTRANSP):
			Fill = new AttrLinearTranspFill;
			break;

	 	case (FGMENU_RADIALTRANSP):
			Fill = new AttrRadialTranspFill;
			if (Fill != NULL)
				((AttrRadialFill*)Fill)->MakeElliptical();
			break;

	 	case (FGMENU_CIRCULARTRANSP):
			Fill = new AttrRadialTranspFill;
			if (Fill != NULL)
				((AttrRadialFill*)Fill)->MakeCircular();
			break;

	 	case (FGMENU_CONICALTRANSP):
			Fill = new AttrConicalTranspFill;
			break;

	 	case (FGMENU_SQUARETRANSP):
			Fill = new AttrSquareTranspFill;
			break;

	 	case (FGMENU_THREECOLTRANSP):
			Fill = new AttrThreeColTranspFill;
			CloseProfileDialog (m_BiasGainGadget);		// cause fill type cannot be profiled!
			break;

	 	case (FGMENU_FOURCOLTRANSP):
			Fill = new AttrFourColTranspFill;
			CloseProfileDialog (m_BiasGainGadget);		// cause fill type cannot be profiled!
			break;

	 	case (FGMENU_BITMAPTRANSP):
			Fill = new AttrBitmapTranspFill;

			Default = KernelBitmap::MakeKernelBitmap();
			if (Default == NULL)
			{
				delete Fill;
				return NULL;
			}

			Fill->AttachBitmap(Default);
			break;

	 	case (FGMENU_FRACTALTRANSP):
			Fill = new AttrFractalTranspFill;
			break;

	 	case (FGMENU_NOISETRANSP):
			Fill = new AttrNoiseTranspFill;
			break;

		default:
			// Er .. Dunno what kind of fill this is ?
			Fill = new AttrFlatTranspFill;
			break;
	}

	return Fill;
}

/********************************************************************************************

>	AttrFillGeometry* TranspInfoBarOp::GetCurrentGeometry()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Purpose:	Gets the current Fill Geometry from the Attribute manager.

********************************************************************************************/

AttrFillGeometry* TranspInfoBarOp::GetCurrentGeometry()
{
	CCRuntimeClass* CurrentAttribGroup = 
 		Tool::GetCurrent()->Parent->m_ToolInfo.CurrentAttributeGroup;

	Document* CurrentDoc = Document::GetSelected();

	if (CurrentDoc == NULL)
		return NULL; // We are not going to be able to do anything if there is no document

	return (AttrFillGeometry*)CurrentDoc->GetAttributeMgr()
								.GetCurrentAttribute(CurrentAttribGroup, 
											CC_RUNTIME_CLASS(AttrTranspFillGeometry));
}

/********************************************************************************************

>	AttrPaintingMode* GradInfoBarOp::GetCurrentPaintingMode ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/99
	Purpose:	Gets the current painting mode from the Attribute manager.

********************************************************************************************/

/*AttrPaintingMode* TranspInfoBarOp::GetCurrentPaintingMode ()
{
	CCRuntimeClass* CurrentAttribGroup = 
 		Tool::GetCurrent()->Parent->ToolInfo.CurrentAttributeGroup;

	Document* CurrentDoc = Document::GetSelected();

	if (CurrentDoc == NULL)
		return NULL; // We are not going to be able to do anything if there is no document

	return (AttrPaintingMode*)CurrentDoc->GetAttributeMgr()
								.GetCurrentAttribute(CurrentAttribGroup, 
											CC_RUNTIME_CLASS(AttrPaintingMode));
}*/


/********************************************************************************************

>	AttrTranspFillMapping* TranspInfoBarOp::GetCurrentMapping()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Purpose:	Gets the current Fill Geometry from the Attribute manager.

********************************************************************************************/

AttrTranspFillMapping* TranspInfoBarOp::GetCurrentMapping()
{
	CCRuntimeClass* CurrentAttribGroup = 
 		Tool::GetCurrent()->Parent->m_ToolInfo.CurrentAttributeGroup;

	Document* CurrentDoc = Document::GetSelected();

	if (CurrentDoc == NULL)
		return NULL; // We are not going to be able to do anything if there is no document

	return (AttrTranspFillMapping*)CurrentDoc->GetAttributeMgr()
								.GetCurrentAttribute(CurrentAttribGroup, 
											CC_RUNTIME_CLASS(AttrTranspFillMapping));
}

double GetDoubleGrain(INT32 Pos)
{
	return ( 128.0 * double(Pos)/double(GetGrainPosMax()) );
}

INT32 GetGrainPos(double Grain)
{
	return INT32(Grain * double(GetGrainPosMax()) / 128.0 );
}

INT32 GetGrainPosMax()
{
	return (1<<16);
}

double GetDoubleScale(INT32 Pos)
{
	return ( 100.0 * double(Pos)/double(GetScalePosMax()) );
}

INT32 GetScalePos(double Scale)
{
	return INT32(Scale * double(GetScalePosMax()) / 100.0 );
}

INT32 GetScalePosMax()
{
	return (100);
}




/********************************************************************************************

>	AttrFillGeometry* AttrRemoveTransp::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (modified by Chris Snook)
	Created:	1/4/94
	Inputs:		The fill that is to be changed.
	Purpose:	Removes all transparency from a fill.

********************************************************************************************/

AttrFillGeometry* AttrRemoveTransp::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
	AttrFillGeometry* NewFill = new AttrFlatTranspFill;
	if (NewFill == NULL)
		return NULL;

	UINT32 TypeNone = TT_Mix;
	UINT32 NoTransp = 0;

	NewFill->SetTranspType(TypeNone);
	NewFill->SetStartTransp(&NoTransp);

	if (GetApplication()->GetBlobManager()->GetCurrentInterest().Fill && 
		FillToMutate->IsVisible() &&
		FillToMutate->FindParent() &&
		FillToMutate->FindParent()->IsSelected())
	{
		FillToMutate->RenderFillBlobs();
	}

	// Return the Mutated Fill
	return NewFill;
}

/********************************************************************************************

>	virtual NodeAttribute* AttrRemoveTransp::GetOtherAttrToApply(BOOL* IsMutate)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/95
	Returns:	The secondary attribute to apply, or NULL if none to apply
	Purpose:	Some attributes require a secondary atribute to be changed when they are
				changed.  This routine obtains a pointer to the secondary attribute to
				apply.

********************************************************************************************/

NodeAttribute* AttrRemoveTransp::GetOtherAttrToApply(BOOL* IsMutate)
{
	ERROR3IF(IsMutate == NULL, "NULL flag pointer passed to GetOtherAttrToApply");

	// Remove Transparency also need to remove line transparency
	NodeAttribute* OtherAttr = new AttrRemoveStrokeTransp;
	if (OtherAttr == NULL)
		return NULL;

	*IsMutate = TRUE;

	return OtherAttr;
}

/********************************************************************************************

>	AttrFillGeometry* AttrRemoveStrokeTransp::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/94
	Inputs:		The fill that is to be changed.
	Purpose:	Removes all transparency from a fill.

********************************************************************************************/

AttrFillGeometry* AttrRemoveStrokeTransp::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
    AttrFillGeometry* NewFill = new AttrStrokeTransp;
	if (NewFill == NULL)
		return NULL;

	UINT32 TypeNone = TT_Mix;
	UINT32 NoTransp = 0;

	NewFill->SetTranspType(TypeNone);
	NewFill->SetStartTransp(&NoTransp);

	// Return the Mutated Fill
	return NewFill;
}

/********************************************************************************************

>	AttrFillGeometry* AttrMakeFlatTransp::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/94
	Inputs:		The fill that is to be changed.
	Purpose:	Forces a 50% flat transparency.

********************************************************************************************/

AttrFillGeometry* AttrMakeFlatTransp::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
    AttrFillGeometry* NewFill = new AttrFlatTranspFill;
	if (NewFill == NULL)
		return NULL;

	UINT32 Transp = *FillToMutate->GetStartTransp();

	if (FillToMutate->GetTranspType() == TT_Mix &&
		*FillToMutate->GetStartTransp() == 0)
	{
		Transp = 128;
	}

	NewFill->SetTranspType(FillToMutate->GetTranspType());
	NewFill->SetStartTransp(&Transp);

	// WEBSTER-Martin-02/12/96
//#ifdef WEBSTER
	// Make sure a FlatTransparency can change its value bearing in mind the user can't
	// change the blob selection
//	NewFill->SelectBlob(FILLCONTROL_STARTPOINT);
	//NewFill->SetBlobState(FILLCONTROL_STARTPOINT, TRUE);
	//AttrFillGeometry::SelectionCount = AttrFillGeometry::CountSelectionControlPoints();
//#endif //WEBSTER

	if (GetApplication()->GetBlobManager()->GetCurrentInterest().Fill && 
		FillToMutate->IsVisible() &&
		FillToMutate->FindParent() &&
		FillToMutate->FindParent()->IsSelected())
	{
		FillToMutate->RenderFillBlobs();
	}

	// Return the Mutated Fill
	return NewFill;
}

/********************************************************************************************

>	virtual NodeAttribute* AttrMakeFlatTransp::GetOtherAttrToApply(BOOL* IsMutate)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/95
	Returns:	The secondary attribute to apply, or NULL if none to apply
	Purpose:	Some attributes require a secondary atribute to be changed when they are
				changed.  This routine obtains a pointer to the secondary attribute to
				apply.

********************************************************************************************/

NodeAttribute* AttrMakeFlatTransp::GetOtherAttrToApply(BOOL* IsMutate)
{
	ERROR3IF(IsMutate == NULL, "NULL flag pointer passed to GetOtherAttrToApply");

	// A transparency change also needs to set the Stroke Transparency
	NodeAttribute* OtherAttr = new AttrStrokeTranspChange;
	if (OtherAttr == NULL)
		return NULL;

	UINT32 Transp = 128;

	((AttrStrokeTranspChange *)OtherAttr)->SetStartTransp(&Transp);

	*IsMutate = TRUE;

	return OtherAttr;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//			Implementation of class OpChangeFillProfile

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************

>	OpChangeFillProfile::OpChangeFillProfile()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Inputs:		-
	Purpose:	Construct an object of this type.

********************************************************************************************/

OpChangeFillProfile::OpChangeFillProfile()
{
}



/********************************************************************************************

>	OpChangeFillProfile::~OpChangeFillProfile()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Inputs:		-
	Purpose:	Destruct an object of this type.

********************************************************************************************/

OpChangeFillProfile::~OpChangeFillProfile()
{
}



/********************************************************************************************

>	BOOL OpChangeFillProfile::Declare()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations.

********************************************************************************************/

BOOL OpChangeFillProfile::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_FILLPROFILE),
								CC_RUNTIME_CLASS(OpChangeFillProfile), 
								OPTOKEN_FILLPROFILE,
								OpChangeFillProfile::GetState));
}



/********************************************************************************************

>	void OpChangeFillProfile::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Returns:	-
	Purpose:	Saves the fill profile at this point, so it can be restored on an undo.

********************************************************************************************/

void OpChangeFillProfile::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	// Start a slow job
	//BeginSlowJob();
	DoStartSelOp(TRUE);

	// run through the selection changing all profiles
	CProfileBiasGain Profile;

	// recast the op param
	FillProfileOpParam * pProfileParam = (FillProfileOpParam *)pParam;

	List ShadowList;
// Why does this change ALL profiles - even those deep down in the selection???
//	FillTools::BuildListOfSelectedNodes(&ShadowList, CC_RUNTIME_CLASS(AttrFillGeometry));
	FillTools::GetSelectedAttrList(&ShadowList, CC_RUNTIME_CLASS(AttrFillGeometry), TRUE, FALSE);

	NodeListItem * pItem = (NodeListItem *)ShadowList.GetHead();

	CProfileBiasGain OldGain;

	while (pItem)
	{
		if (pItem->pNode)
		{
			// initialise the action
			ChangeFillProfileAction * pAction;

			ChangeFillProfileAction::Init(this, GetUndoActionList(), (AttrFillGeometry *)pItem->pNode, pProfileParam->Profile,
				&pAction);

			// Inform the effected parents of the change

			ObjChangeFlags cFlags;
			cFlags.Attribute = TRUE;		// Needed to make Blends re-calc themselves

			ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

			/*(AttrFillGeometry *)*/ (pItem->pNode)->AllowOp(&ObjChange);

			ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,NULL);
			UpdateAllChangedNodes(&ObjChange);
		}

		pItem = (NodeListItem *)ShadowList.GetNext(pItem);
	}

	ShadowList.DeleteAll();

	End();
}



/********************************************************************************************

>	OpState OpChangeFillProfile::GetState(String_256* Description, OpDescriptor*)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Returns:	Ungreyed, Unticked
	Purpose:	Determines the state of the op.

********************************************************************************************/

OpState OpChangeFillProfile::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//			Implementation of class ChangeFillProfileAction

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************

>	ChangeFillProfileAction::ChangeFillProfileAction()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Inputs:		-
	Purpose:	Construct an object of this type.

********************************************************************************************/

ChangeFillProfileAction::ChangeFillProfileAction()
{
	m_pNodeFill = NULL;
}



/********************************************************************************************

>	ChangeFillProfileAction::~ChangeFillProfileAction()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Inputs:		-
	Purpose:	Destruct an object of this type.

********************************************************************************************/

ChangeFillProfileAction::~ChangeFillProfileAction()
{
}



/********************************************************************************************

>	ActionCode ChangeFillProfileAction::Init(Operation* pOp,
										ActionList* pActionList,
										AttrFillGeometry* pThisNodeFill,
										CProfileBiasGain &Profile,
										ChangeFillProfileAction** NewAction,
										BOOL bReverse = TRUE,
										BOOL bCache = FALSE)
{

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	= ptr to action list to which this action should be added
				pThisNodeFill	= ptr to NodeFill to change 
				Profile			= New profile to apply to pThisNodeFill
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeFillProfileAction::Init(Operation* pOp,
										ActionList* pActionList,
										AttrFillGeometry* pThisNodeFill,
										CProfileBiasGain &Profile,
										ChangeFillProfileAction** NewAction,
										BOOL bReverse /*= TRUE*/,
										BOOL bCache /*= FALSE*/)
{
	UINT32 ActSize = sizeof(ChangeFillProfileAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize, CC_RUNTIME_CLASS(ChangeFillProfileAction), (Action**)NewAction);

	if (Ac != AC_FAIL)
	{
		Document * pDoc = Document::GetCurrent();
		
		// reverse the action
		if (bReverse)
		{
			pActionList->RemoveItem(*NewAction);
			pActionList->AddHead(*NewAction);
		}
		
		(*NewAction)->m_pNodeFill = pThisNodeFill;
		FillGeometryAttribute* temp =  (FillGeometryAttribute*) pThisNodeFill->GetAttributeValue ();
		(*NewAction)->m_LastProfile = temp->GetProfile ();

		NodeRenderableInk* pNode = (NodeRenderableInk*) pThisNodeFill->FindParent ();

		// set the values, invalidate and regenerate

		temp->SetProfile(Profile);

		pDoc->ForceRedraw(pThisNodeFill->FindParentSpread(), 
						  pNode->GetBoundingRect(), FALSE, pNode);
	}

	return Ac;
}



/********************************************************************************************

>	void ChangeFillProfileAction::ChangeFillProfileWithNoUndo (Operation* pOp, CProfileBiasGain &Profile)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/2000
	Inputs:		Profile - the profile that is to be applied directly (i.e.  applied with no undo)
	Purpose:	When applying fill profiles, we only want to generate one bit of undo information.
				This function allows us to meet this requirement (the one bit of undo information
				is generated via OpChangeFillProfile::DoWithParam ()).  This function is ONLY
				called from within GradInfoBarOp::ChangeProfile () - after
				OpChangeFillProfile::DoWithParam () has been called.
	Errors:		-
	SeeAlso:	GradInfoBarOp::ChangeProfile (), OpChangeFillProfile::DoWithParam ().

********************************************************************************************/

void ChangeFillProfileAction::ChangeFillProfileWithNoUndo (CProfileBiasGain &Profile)
{
	Document * pDoc = Document::GetCurrent();

	List ShadowList;
// Why does this change ALL profiles - even those deep down in the selection???
//	FillTools::BuildListOfSelectedNodes(&ShadowList, CC_RUNTIME_CLASS(AttrFillGeometry));
	FillTools::GetSelectedAttrList(&ShadowList, CC_RUNTIME_CLASS(AttrFillGeometry), TRUE, FALSE);

	NodeListItem * pItem = (NodeListItem *)ShadowList.GetHead();

	CProfileBiasGain OldGain;

	while (pItem)
	{
		if (pItem->pNode)
		{
			AttrFillGeometry* pThisNodeFill = (AttrFillGeometry *)pItem->pNode;
					
			FillGeometryAttribute* temp =  (FillGeometryAttribute*) pThisNodeFill->GetAttributeValue ();
			temp->SetProfile(Profile);

			NodeRenderableInk* pNode = (NodeRenderableInk*) pThisNodeFill->FindParent ();
			Node* pParent = pNode->FindParent ();

			// decide the best option for causing a redraw
			
			if (IS_A(pParent, Layer))			// a simple node is selected
			{
				pDoc->ForceRedraw(pThisNodeFill->FindParentSpread(), 
								  pNode->GetBoundingRect(), FALSE, pNode);
			}
			else
			{
				ObjChangeFlags cFlags;
				cFlags.Attribute = TRUE;		// Needed to make Blends re-calc themselves

				ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

				/*(AttrFillGeometry *)*/ (pItem->pNode)->AllowOp(&ObjChange);

				ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,NULL);
					
				Operation TempOp;

				TempOp.UpdateAllChangedNodes(&ObjChange);
			}
		}

		pItem = (NodeListItem *)ShadowList.GetNext(pItem);
	}

	ShadowList.DeleteAll();

	if (pDoc->GetOpHistory ().CanRedo ())
	{
		// then we need to clear out the redo information - since we are now 'before' it ....
		pDoc->GetOpHistory ().DeleteRedoableOps ();

		// and update the state of things ....
		DialogBarOp::SetSystemStateChanged();
		DialogBarOp::UpdateStateOfAllBars();
	}
}



/********************************************************************************************

>	ActionCode ChangeFillProfileAction::Execute()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the fill profile in
				m_pNodeFill to its previous one, after creating another action to
				record the current fill profile of m_pNodeFill in.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeFillProfileAction::Execute()
{
	ActionCode Act;
	ChangeFillProfileAction* pAction;

	Act = ChangeFillProfileAction::Init(	pOperation, 
											pOppositeActLst,
											m_pNodeFill,
											m_LastProfile,
											&pAction,
											TRUE,
											FALSE);
	
	return (Act);
}



/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//			Implementation of class OpChangeTranspFillProfile

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************

>	OpChangeTranspFillProfile::OpChangeTranspFillProfile()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Inputs:		-
	Purpose:	Construct an object of this type.

********************************************************************************************/

OpChangeTranspFillProfile::OpChangeTranspFillProfile()
{
}



/********************************************************************************************

>	OpChangeTranspFillProfile::~OpChangeTranspFillProfile()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Inputs:		-
	Purpose:	Destruct an object of this type.

********************************************************************************************/

OpChangeTranspFillProfile::~OpChangeTranspFillProfile()
{
}



/********************************************************************************************

>	BOOL OpChangeTranspFillProfile::Declare()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations.

********************************************************************************************/

BOOL OpChangeTranspFillProfile::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_TRANSPFILLPROFILE),
								CC_RUNTIME_CLASS(OpChangeTranspFillProfile), 
								OPTOKEN_TRANSPFILLPROFILE,
								OpChangeTranspFillProfile::GetState));
}



/********************************************************************************************

>	void OpChangeTranspFillProfile::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Returns:	-
	Purpose:	Saves the transparency profile at this point, so it can be restored on
				an undo.

********************************************************************************************/

void OpChangeTranspFillProfile::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	// Start a slow job
	//BeginSlowJob();
	DoStartSelOp(TRUE);

	// run through the selection changing all profiles
	CProfileBiasGain Profile;

	// recast the op param
	FillProfileOpParam * pProfileParam = (FillProfileOpParam *)pParam;

	List ShadowList;
// Why does this change ALL profiles - even those deep down in the selection???
//	FillTools::BuildListOfSelectedNodes(&ShadowList, CC_RUNTIME_CLASS(AttrTranspFillGeometry));
	FillTools::GetSelectedAttrList(&ShadowList, CC_RUNTIME_CLASS(AttrTranspFillGeometry), TRUE, TRUE);

	NodeListItem * pItem = (NodeListItem *)ShadowList.GetHead();

	CProfileBiasGain OldGain;

	while (pItem)
	{
		if (pItem->pNode)
		{
			// initialise the action
			ChangeFillProfileAction * pAction;

			ChangeFillProfileAction::Init(this, GetUndoActionList(), (AttrFillGeometry *)pItem->pNode, pProfileParam->Profile,
				&pAction);

			// Inform the effected parents of the change

			ObjChangeFlags cFlags;
			cFlags.Attribute = TRUE;		// Needed to make Blends re-calc themselves

			ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

			/*(AttrFillGeometry *)*/ (pItem->pNode)->AllowOp(&ObjChange);

			ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,NULL);
			UpdateAllChangedNodes(&ObjChange);
		}

		pItem = (NodeListItem *)ShadowList.GetNext(pItem);
	}

	ShadowList.DeleteAll();

	End();
}



/********************************************************************************************

>	OpState OpChangeTranspFillProfile::GetState(String_256* Description, OpDescriptor*)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Returns:	Ungreyed, Unticked
	Purpose:	Determines the state of the op.

********************************************************************************************/

OpState OpChangeTranspFillProfile::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return (Blobby);
}



/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//			Implementation of class ChangeTranspFillProfileAction

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************

>	ChangeTranspFillProfileAction::ChangeTranspFillProfileAction()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Inputs:		-
	Purpose:	Construct an object of this type.

********************************************************************************************/

ChangeTranspFillProfileAction::ChangeTranspFillProfileAction()
{
	m_pNodeTranspFill = NULL;
}



/********************************************************************************************

>	ChangeTranspFillProfileAction::~ChangeTranspFillProfileAction()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Inputs:		-
	Purpose:	Destruct an object of this type.

********************************************************************************************/

ChangeTranspFillProfileAction::~ChangeTranspFillProfileAction()
{
}



/********************************************************************************************

>	ActionCode ChangeTranspFillProfileAction::Init(Operation* pOp,
											   ActionList* pActionList,
											   AttrFillGeometry* pThisNodeTranspFill,
											   CProfileBiasGain &Profile,
											   ChangeTranspFillProfileAction** NewAction,
											   BOOL bReverse = TRUE,
											   BOOL bCache = FALSE)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	= ptr to action list to which this action should be added
				pThisNodeTranspFill	= ptr to NodeTranspFill to change 
				Profile			= New profile to apply to pThisNodeTranspFill
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeTranspFillProfileAction::Init(Operation* pOp,
											   ActionList* pActionList,
											   AttrFillGeometry* pThisNodeTranspFill,
											   CProfileBiasGain &Profile,
											   ChangeTranspFillProfileAction** NewAction,
											   BOOL bReverse /*= TRUE*/,
											   BOOL bCache /*= FALSE*/)
{
	UINT32 ActSize = sizeof(ChangeTranspFillProfileAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize, CC_RUNTIME_CLASS(ChangeTranspFillProfileAction), (Action**)NewAction);

	if (Ac != AC_FAIL)
	{
		Document * pDoc = Document::GetCurrent();
		
		// reverse the action
		if (bReverse)
		{
			pActionList->RemoveItem(*NewAction);
			pActionList->AddHead(*NewAction);
		}
		
		(*NewAction)->m_pNodeTranspFill = pThisNodeTranspFill;
		FillGeometryAttribute* temp =  (FillGeometryAttribute*) pThisNodeTranspFill->GetAttributeValue ();
		(*NewAction)->m_LastProfile = temp->GetProfile ();

		NodeRenderableInk* pNode = (NodeRenderableInk*) pThisNodeTranspFill->FindParent ();

		// set the values, invalidate and regenerate

		temp->SetProfile(Profile);

		pDoc->ForceRedraw(pThisNodeTranspFill->FindParentSpread(), 
				pNode->GetBoundingRect(), FALSE, pNode);
	}

	return Ac;
}



/********************************************************************************************

>	void ChangeTranspFillProfileAction::ChangeTranspFillProfileWithNoUndo (CProfileBiasGain &Profile)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/2/2000
	Inputs:		Profile - the profile that is to be applied directly (i.e.  applied with no undo)
	Purpose:	When applying transp fill profiles, we only want to generate one bit of undo information.
				This function allows us to meet this requirement (the one bit of undo information
				is generated via OpChangeTranspFillProfile::DoWithParam ()).  This function is ONLY
				called from within TranspInfoBarOp::ChangeProfile () - after
				OpChangeTranspFillProfile::DoWithParam () has been called.
	Errors:		-
	SeeAlso:	GradInfoBarOp::ChangeProfile (), OpChangeTranspFillProfile::DoWithParam ().

********************************************************************************************/

void ChangeTranspFillProfileAction::ChangeTranspFillProfileWithNoUndo (CProfileBiasGain &Profile)
{
	Document * pDoc = Document::GetCurrent();

	List ShadowList;
// Why does this change ALL profiles - even those deep down in the selection???
//	FillTools::BuildListOfSelectedNodes(&ShadowList, CC_RUNTIME_CLASS(AttrTranspFillGeometry));
	FillTools::GetSelectedAttrList(&ShadowList, CC_RUNTIME_CLASS(AttrTranspFillGeometry), TRUE, TRUE);

	NodeListItem * pItem = (NodeListItem *)ShadowList.GetHead();

	CProfileBiasGain OldGain;

	while (pItem)
	{
		if (pItem->pNode)
		{
			AttrFillGeometry* pThisNodeFill = (AttrFillGeometry *)pItem->pNode;
					
			FillGeometryAttribute* temp =  (FillGeometryAttribute*) pThisNodeFill->GetAttributeValue ();
			temp->SetProfile(Profile);

			NodeRenderableInk* pNode = (NodeRenderableInk*) pThisNodeFill->FindParent ();
			Node* pParent = pNode->FindParent ();

			// decide the best option for causing a redraw
			
			if (IS_A(pParent, Layer))			// a simple node is selected
			{
				pDoc->ForceRedraw(pThisNodeFill->FindParentSpread(),
								  pNode->GetBoundingRect(), FALSE, pNode);
			}
			else
			{
				ObjChangeFlags cFlags;
				cFlags.Attribute = TRUE;		// Needed to make Blends re-calc themselves

				ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

				/*(AttrFillGeometry *)*/ (pItem->pNode)->AllowOp(&ObjChange);

				ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,NULL);
					
				Operation TempOp;

				TempOp.UpdateAllChangedNodes(&ObjChange);
			}
		}

		pItem = (NodeListItem *)ShadowList.GetNext(pItem);
	}

	ShadowList.DeleteAll();

	if (pDoc->GetOpHistory ().CanRedo ())
	{
		// then we need to clear out the redo information - since we are now 'before' it ....
		pDoc->GetOpHistory ().DeleteRedoableOps ();

		// and update the state of things ....
		DialogBarOp::SetSystemStateChanged();
		DialogBarOp::UpdateStateOfAllBars();
	}
}



/********************************************************************************************

>	ActionCode ChangeTranspFillProfileAction::Execute()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the transparency profile in
				m_pNodeTranspFill to its previous one, after creating another action to
				record the current transparency profile of m_pNodeTranspFill in.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeTranspFillProfileAction::Execute()
{
	ActionCode Act;
	ChangeFillProfileAction* pAction;

	Act = ChangeFillProfileAction::Init(	pOperation, 
											pOppositeActLst,
											m_pNodeTranspFill,
											m_LastProfile,
											&pAction,
											TRUE,
											FALSE);
	
	return (Act);
}
