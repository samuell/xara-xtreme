// $Id: moldedit.cpp 662 2006-03-14 21:31:49Z alex $
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
// This file contains a library of mould functions defined in the mould lib class.
// This class can be derived from by other operations outside the mould tool to
// provide mould functionality.

/*
*/

#include "camtypes.h"
//#include "resource.h"
#include "moldedit.h"
#include "ops.h"
#include "app.h"
#include "nodemold.h"
#include "nodemldr.h"
#include "ndmldgrp.h"
#include "ndmldpth.h"
#include "docview.h"
#include "document.h"
#include "moldpers.h"
#include "pathedit.h"
//#include "mike.h"
#include "helpuser.h"
#include "progress.h"
#include "keypress.h"
#include "objchge.h"
#include "ophist.h"

class Node;

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE( OpMouldLibSel,SelOperation )
CC_IMPLEMENT_DYNCREATE( OpMouldLib,UndoableOperation )
CC_IMPLEMENT_DYNCREATE( OpDragOrigin, SelOperation )
CC_IMPLEMENT_DYNCREATE( InvalidateRectAction, Action )
CC_IMPLEMENT_DYNCREATE( RecordGeometryAction, Action )
CC_IMPLEMENT_DYNCREATE( SaveDetachAction, Action )
CC_IMPLEMENT_DYNCREATE( RedrawBoundsAction, Action )
CC_IMPLEMENT_DYNCREATE( BuildMouldAction, Action )
CC_IMPLEMENT_DYNCREATE( StartRebuildMouldAction, Action )
CC_IMPLEMENT_DYNCREATE( EndRebuildMouldAction, Action )

#define new CAM_DEBUG_NEW


/********************************************************************************************

>	OpMouldLibSel::OpMouldLibSel() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	OpMouldLibSel() constructor
	SeeAlso:	-

********************************************************************************************/

OpMouldLibSel::OpMouldLibSel()
{
	// Dummy constructor
}


/********************************************************************************************

>	BOOL OpMouldLibSel::CreateNewMould(Path* pShape,
									  MouldSpace m_space,
									  BOOL FitSelection,
									  BOOL LockAspect)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	11/12/94
	Inputs:		pShape		= A path to use as a mould shape
				mSpace		= The mould space to create, currently only envelope and
							  perspective spaces are defined
				ScaleToFit	= If TRUE then the shape will be scaled to fit over the 
							  bounds of the objects.
				LockAspect	= If TRUE then the paths current dimensions are scaled so that
							  the minor dimension fits over pBounds
	Outputs:	-
	Returns:	TRUE if CreateNewMould has successfully created the mould object and children
				FALSE if failed.
	Purpose:	Creates a complete mould object from the selected objects. Certain rules are
				used for creating moulds when moulds are already selected. If the selection
				contains mould objects only, then each mould will have its geometry changed
				to that of m_space. If however there are a selection of unlike objects, they
				will all be placed in a new mould whose geometry is specified in this function
				call.

**********************************************************************************************/

BOOL OpMouldLibSel::CreateNewMould(Path* pShape, MouldSpace m_space, BOOL FitSelection, BOOL LockAspect)
{
	ERROR3IF(pShape==NULL,"CreateNewMould called with a NULL shape pointer");

	// Obtain the current selections 
	SelRange* Selected = GetApplication()->FindSelection();
	if (!Selected)
		return FALSE;

	// Trash the old selection bounds
	Range Rnge = (*Selected);
	if (!DoInvalidateNodesRegions(Rnge,FALSE,FALSE))
		return FALSE;

	// Remember the selection before the operation
	if (!DoStartSelOp(FALSE,TRUE))
		return FALSE;

	// Check that we are actually allowed to do this operation
	ObjChangeFlags cFlags;
	cFlags.MultiReplaceNode=TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	// Mark nodes that will allow this to happen, and error if no nodes will let it happen
	if (!Selected->AllowOp(&ObjChange))
	{
		ERROR3("AllowOp() returned FALSE, i.e. op should have been greyed out");
		return FALSE;
	}

	// using this list create or replace the mould bits
	BOOL ok = DoCreateOrReplaceMould(Selected, pShape, m_space, FitSelection, LockAspect);
	if (!ok)
		return FALSE;

	// Update all the changed nodes, i.e. tell all the parents of the children that have been effected
	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	return UpdateChangedNodes(&ObjChange);
}



/********************************************************************************************

>	BOOL OpMouldLibSel::DoCreateOrReplaceMould(	SelRange* pSelected,
												Path* pShape,
												MouldSpace m_space,
												BOOL FitSelection,
												BOOL LockAspect)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/03/95
	Inputs:		pShape		= A path to use as a mould shape
				mSpace		= The mould space to create, currently only envelope and
							  perspective spaces are defined
				ScaleToFit	= If TRUE then the shape will be scaled to fit over the 
							  bounds of the objects.
				LockAspect	= If TRUE then the paths current dimensions are scaled so that
							  the minor dimension fits over pBounds
	Returns:	TRUE if the job has been done
				FALSE if not
	Purpose:

********************************************************************************************/

BOOL OpMouldLibSel::DoCreateOrReplaceMould( SelRange* pSelected,
											Path* pShape,
											MouldSpace m_space,
											BOOL FitSelection,
											BOOL LockAspect)
{
	BOOL ok;

	// Create a list of all the selected nodes
	List* pNodeList = pSelected->MakeListOfNodes();
	if (pNodeList==NULL)
		return FALSE;

	// Get the bounds of the selection
	DocRect Bounds = pSelected->GetBoundingRect();

	if (AllMouldObjects(pNodeList))
	{
		// Ohh arr, we need to replace all selected mould shapes, here goes!!
		ok = DoReplaceAllMoulds(pNodeList, pShape, m_space, FitSelection, LockAspect);
	}
	else
	{		
		// call the mould builder
		NodeMould* pMould = DoCreateMould(pShape, m_space, pNodeList, &Bounds, FitSelection, LockAspect);
		ok = (pMould!=NULL);

		if (ok)
		{
			// Recalculate the mould's bounding box
			pMould->InvalidateBoundingRect();
			GetApplication()->UpdateSelection();
			ok = DoInvalidateNodeRegion(pMould, TRUE);
		}
	}

	// delete all elements in our compiled list
	while (!pNodeList->IsEmpty())
		delete (NodeListItem*)(pNodeList->RemoveHead());
	delete pNodeList;

	return ok;
}


/********************************************************************************************

>	BOOL OpMouldLibSel::AllMouldObjects(List* pNodeList) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/03/95
	Inputs:		pNodeList = a list of object pointers to objects to mould.
	Returns:	TRUE if the list contains mould objects only, at the top level.
				FALSE if there are other object types.
	Purpose:	Scans the list of nodes provided. If any objects which are not node moulds
				are found, false is returned

********************************************************************************************/

BOOL OpMouldLibSel::AllMouldObjects(List* pNodeList)
{
	NodeListItem* CurItem = (NodeListItem*)(pNodeList->GetHead());
	if (!CurItem) return FALSE;

	while (CurItem)
	{
		if (CurItem->pNode->GetRuntimeClass() != CC_RUNTIME_CLASS(NodeMould))
			return FALSE;
		CurItem = (NodeListItem*)(pNodeList->GetNext(CurItem));
	}
	return TRUE;
}



/********************************************************************************************

>	NodeMould* OpMouldLibSel::DoCreateMould(Path* pShape, 
										 	MouldSpace mSpace,
										 	List* NodeList,
											DocRect* pBounds,
										 	BOOL ScaleToFit,
										 	BOOL LockAspect)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pShape		= A path to use as a mould shape
				mSpace		= The mould space to create, currently only envelope and
							  perspective spaces are defined
				NodeList 	= A list of tree object nodes to place in the Mould
				pBounds		= The bounding box of the objects in the node list
				ScaleToFit	= If TRUE then the shape will be scaled to fit over the 
							  bounds of the objects.
				LockAspect	= If TRUE then the paths current dimensions are scaled so that
							  the minor dimension fits over pBounds
	Returns:	TRUE if DoCreateMould has successfully created the mould object and children
				FALSE if failed.
	Purpose:	Creates a complete mould object positioned on the next pointer of the
				last node in the list. All nodes in the list will move inside this new
				mould object.

********************************************************************************************/

NodeMould* OpMouldLibSel::DoCreateMould(Path* pShape, 
									 	MouldSpace mSpace,
									 	List* NodeList,
									 	DocRect* pBounds,
									 	BOOL ScaleToFit,
									 	BOOL LockAspect)
{
	ERROR2IF(pShape==NULL, NULL, "OpMouldLibSel::DoCreateMould() called with a NULL shape pointer");
	ERROR2IF(mSpace==MOULDSPACE_UNDEFINED, NULL, "pMouldLib::DoCreateMould() called with an undefined space type");
	ERROR2IF(NodeList==NULL, NULL, "OpMouldLibSel::DoCreateMould() called with a NULL list pointer");

	// initialise various locals
	BOOL ok;

	NodeListItem* CurItem = (NodeListItem*)(NodeList->GetTail());
	if (!CurItem)
		return NULL;

	Node* Context = CurItem->pNode;
	if (!Context)
		return NULL; 

	// create a mould parent and insert it in the tree
	NodeMould* pMouldParent;
	ALLOC_WITH_FAIL( pMouldParent, new NodeMould, this);
	if (!pMouldParent)
		return NULL;

	// give the parent mould object a shape and mould space to work with and stick it
	// in the tree
			CALL_WITH_FAIL(pMouldParent->CreateGeometry(mSpace), this, ok);
	if (ok) CALL_WITH_FAIL(DoInsertNewNode(pMouldParent, Context, NEXT, FALSE, TRUE, TRUE, FALSE), this, ok);
	if (!ok)
	{	
		delete pMouldParent;
		return NULL;
	}

	// check the shape we've been given is fine and lovely	
	UINT32 errID;
	if (!pMouldParent->GetGeometry()->Validate(pShape, errID))
	{
		SetNextMsgHelpContext(errID);
		ERROR1(NULL, errID);
	}

	// build a destination scale rectangle
	DocRect DRect, *pRect;
	pRect = FindScaleRect(pBounds, &DRect, ScaleToFit, LockAspect);

	// Create and add a mould shape object
	NodeMouldPath* pNodeMPath = pMouldParent->AddNewMouldShape(pShape,pRect,this);
	if (!pNodeMPath)
		return NULL;

	// set the geometry using this new mould shape
	ok = pMouldParent->GetGeometry()->Define(&pNodeMPath->InkPath, pBounds);
	if (!ok)
		return NULL;

	// now create the mould group object
	NodeMouldGroup* pMouldGroup = pMouldParent->AddNewMouldGroup(this);
	if (!pMouldGroup)
		return NULL;

	// and fill it with our object list
	if (!pMouldParent->FillMouldGroup(pMouldGroup, NodeList, this))
		return NULL;

	// Now build the moulder object and its list structure
	NodeMoulder* pMoulder = pMouldParent->AddNewMoulder(pMouldParent, LASTCHILD, this);
	if (!pMoulder)
		return NULL;

	// create all the moulded objects inside the moulder
	if (!pMoulder->CreateMouldedObjects(pMouldGroup, pMouldParent->GetGeometry(), this)) 
		return NULL;

	return pMouldParent; 
}




/********************************************************************************************

	DocRect* OpMouldLibSel::FindScaleRect(	DocRect* pSRect,
											DocRect* pDRect,
											BOOL ScaleToFit,
											BOOL KeepSquare)
 
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		pSRect = a pointer to a source rectangle
				pDRect = a pointer to a destination rectangle
				ScaleToFit	= If TRUE then pDRect will contain pSRect
				KeepSquare	= If TRUE then pDRect is converted to have equal width and
							  height using its max dimension 
	Returns:	a pointer to pDRect or null if pDRect has not been affected
	Purpose:	Internal scaling rectangle calculator. Using the flags scaletofit, lock
				a destination rectangle is build from the souce. If both flags are false
				a null pointer will be returned else a pointer to pDRect.

********************************************************************************************/

DocRect* OpMouldLibSel::FindScaleRect(DocRect* pSRect, DocRect* pDRect, BOOL ScaleToFit, BOOL KeepSquare)
{
	if (ScaleToFit)
	{
		(*pDRect)=(*pSRect);

		// form a square rectangle if lock aspect is on.
		if (KeepSquare)
		{
			INT32 wid = pDRect->Width();
			INT32 hgt = pDRect->Height();
			INT32 max = (hgt>wid) ? hgt : wid;

			INT32 cx = (pDRect->lo.x + pDRect->hi.x)/2;
			INT32 cy = (pDRect->lo.y + pDRect->hi.y)/2;

			pDRect->lo.x = cx-max/2;
			pDRect->lo.y = cy-max/2;
			pDRect->hi.x = cx+max/2;
			pDRect->hi.y = cy+max/2;
		}
		
		return pDRect;
	}
	return NULL;
}


/********************************************************************************************

>	static BOOL OpMouldLibSel::IsAMouldSelected(NodeMould** pNodeMould)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		pNodeMould, a return pointer (can be null)
	Outputs:	pNodeMould points to the selected mould object if one exists
	Returns:	TRUE if there is a selected mould object
				FALSE if not
	Purpose:	Scans the selection hunting for a selected mould object.
				It will return a pointer to the object if found and if the return pointer
				is none null.
				Hopefully this will be replaced at some point by Selection->FindObject(type)

********************************************************************************************/

BOOL OpMouldLibSel::IsAMouldSelected(NodeMould** pNodeMould)
{
	BOOL Sel = FALSE;
	Node* pNode =NULL;
	if (Document::GetCurrent() != NULL)
	{
		SelRange* Selected = GetApplication()->FindSelection();
		pNode = Selected->FindFirst();
		while ((pNode!=NULL) && (!Sel))
		{
			if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeMould))
				Sel=TRUE;
			else
				pNode = Selected->FindNext(pNode);
		}
	}
	if (Sel && pNodeMould)
		*pNodeMould = (NodeMould*)pNode;

	return Sel;
}




/********************************************************************************************

>	BOOL OpMouldLibSel::DoRemoveAllMould(List* pNodeList)
 
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		pNodeList = pointer to a list of selected nodes to process
	Returns:	TRUE if successfully removed all the mould objects
				FALSE if failed.
	Purpose:	Removes the mould from around an object.
				To do this it 

********************************************************************************************/

BOOL OpMouldLibSel::DoRemoveAllMoulds(List* pNodeList)
{
	BOOL ok, constrain, putonclip;
	INT32 count = 0;

	NodeListItem* CurItem = (NodeListItem*)(pNodeList->GetHead());
	if (!CurItem) 
		return FALSE;

	constrain=KeyPress::IsConstrainPressed();

	while (CurItem)
	{
		if (CurItem->pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeMould))
		{
			NodeMould* pMould = (NodeMould*)(CurItem->pNode);

			count++;
			putonclip = (constrain && (count==1));

			CALL_WITH_FAIL(DoRemoveMould(pMould, putonclip),this,ok);
			if (!ok) 
				return FALSE;
		}
		CurItem = (NodeListItem*)(pNodeList->GetNext(CurItem));
	}
	return TRUE;
}




/********************************************************************************************

>	BOOL OpMouldLibSel::DoRemoveMould(NodeMould* pMould, BOOL PutOnClip)
 
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		pMould	= pointer to a mould object
	Returns:	TRUE if successfully removed the mould object
				FALSE if failed.
	Purpose:	Removes the mould from around an object.
				To do this it 

********************************************************************************************/

BOOL OpMouldLibSel::DoRemoveMould(NodeMould* pMould, BOOL PutOnClip)
{
	ERROR2IF(pMould==NULL, FALSE, "OpMouldBuild::DoCreateMould() called with a NULL shape pointer");

	// Ask the mould if it's OK to do the op.
	// If not, pretend that everything went OK, because we wouldn't have got called unless at least
	// one selected mould could be removed.  This is ensured by OpRemoveMould::GetState
	ObjChangeFlags cFlags;
	cFlags.MultiReplaceNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,pMould,this);
	if (!pMould->AllowOp(&ObjChange))
		return TRUE;

	// Make sure each child contains the correct attributes.
	//if (!DoLocaliseCommonAttributes(pMould))
//		return FALSE;

	if (PutOnClip)
	{
		NodeMouldPath* pMouldPath = pMould->GetPathShape();
		if (pMouldPath!=NULL)
		{
			// RangeControl Cntrl = Selected + Unselected + don't cross layers  
			Range CopyRange(pMouldPath,pMouldPath,RangeControl(TRUE,TRUE,FALSE));
			if (!DoCopyNodesToClipboard(CopyRange))
				return FALSE;
		}
	}

 	NodeMouldGroup* pNodeMouldGroup = pMould->FindMouldGroup();
	if (!pNodeMouldGroup) 
		return FALSE;

	// now make sure the nodemouldgroups kids have localised attributes
	if (!DoLocaliseCommonAttributes(pNodeMouldGroup))
		return FALSE;

	// Now localise all those unsafe attributes at the top.
	if (!LocaliseMouldAttributes(pMould))
		return FALSE;

	if (!HideAllMouldAttrs(pMould))
		return FALSE;

	Spread* pSpread = pMould->FindParentSpread();

	Node* pNode = pNodeMouldGroup->FindLastChild();
	while (pNode!=NULL)
	{
		Node* pPrevNode = pNode->FindPrevious();

		if (pNode->IS_KIND_OF(NodeRenderableInk))
		{
			if (!DoSelectNode((NodeRenderableInk*)pNode,pSpread))
				return FALSE;

			if (!DoInvalidateNodeRegion((NodeRenderableInk*)pNode,TRUE))
				return FALSE;
		}

		if (pNode->IsAnObject())
		{
			if (!DoMoveNode(pNode, pMould, NEXT))
				return FALSE;
		}
		
		pNode = pPrevNode;  
	}

	if (!DoDeselectNode(pMould, pSpread))
		return FALSE;

	if (!DoInvalidateNodeRegion(pMould, TRUE))
		return FALSE;

	// Hide the mould node
	if (!DoHideNode(pMould, FALSE, NULL))
		return FALSE;                

	// inform the selection something has changed?
	GetApplication()->UpdateSelection();

	return TRUE;
}


/********************************************************************************************

	BOOL OpMouldLibSel::LocaliseMouldAttributes(NodeMould* pMould)
 
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		pMould	= pointer to a mould object
	Returns:	TRUE if successfully localised the mould attributes
				FALSE if failed.
	Purpose:	

********************************************************************************************/

BOOL OpMouldLibSel::LocaliseMouldAttributes(NodeMould* pMould)
{
	ERROR2IF(pMould==NULL, FALSE, "LocaliseMouldAttributes() given a null mould pointer");

 	NodeMouldGroup* pNodeMouldGroup = pMould->FindMouldGroup();
	if (!pNodeMouldGroup) 
		return FALSE;

	Node* pNode = pNodeMouldGroup->FindFirstChild();
	while (pNode)
	{
		if (pNode->IsAnObject())
		{
			NodeRenderableInk* pInkNode = (NodeRenderableInk*)pNode;
			if (!LocaliseObjectAttrs(pMould, pInkNode))
				return FALSE;
		}
		pNode=pNode->FindNext();
	}
	return TRUE;
}



/********************************************************************************************

	BOOL OpMouldLibSel::LocaliseObjectAttrs(NodeRenderableInk* pParent, NodeRenderableInk* pInkNode)
 
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		pMould	= pointer to a mould object
	Returns:	TRUE if successfully localised the mould attributes
				FALSE if failed.
	Purpose:	

********************************************************************************************/

BOOL OpMouldLibSel::LocaliseObjectAttrs(NodeRenderableInk* pParent, NodeRenderableInk* pInkNode)
{
	CCRuntimeClass* AttrType;
	BOOL Exists; 
	NodeAttribute* pCurAttr;
	Node* pCloneAttr;
	Node* Current;

	ERROR2IF(pInkNode==NULL, FALSE, "LocaliseObjectAttrs() given a null object pointer");

	Current = pParent->FindFirstChild();
	while (Current)
	{
		if (Current->IsAnAttribute())
		{
			pCurAttr = (NodeAttribute*)Current;
			// Some attributes cannot be applied to objects
			if (pCurAttr->CanBeAppliedToObject())
			{
				AttrType = pCurAttr->GetAttributeType();

				if (pInkNode->RequiresAttrib(pCurAttr))
				{
					// Does the node already have this child attribute?
					Exists = (pInkNode->GetChildAttrOfType(AttrType) != NULL);
					ERROR3IF(Exists, "LocaliseMouldAttributes: Duplicate attr found !"); 

					if (!Exists)
					{
						pCloneAttr = pCurAttr->SimpleCopy();
						if (pCloneAttr)
						{
							pCloneAttr->AttachNode(pInkNode, FIRSTCHILD, TRUE, FALSE);

							HideNodeAction* UndoHideNodeAction; 

							if (HideNodeAction::Init(this,  
													 GetUndoActionList(),
													 pCloneAttr, 
													 TRUE,
													 (Action**)(&UndoHideNodeAction)
													 ) == AC_FAIL)
							{   
								pCloneAttr->UnlinkNodeFromTree();
								delete pCloneAttr;
								pCloneAttr=NULL;
								return FALSE; 
							}

						}
					}
				}
			}
		}
		Current = Current->FindNext();
	}
	return TRUE;
}




/********************************************************************************************

	BOOL OpMouldLibSel::HideAllMouldAttrs(NodeMould* pMould)
 
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		pMould	= pointer to a mould object
	Returns:	TRUE if successfully hidden all the mould attributes
				FALSE if failed.
	Purpose:	

********************************************************************************************/

BOOL OpMouldLibSel::HideAllMouldAttrs(NodeMould* pMould)
{
	ERROR2IF(pMould==NULL, FALSE, "HideAllMouldAttrs() given a null mould pointer");

	Node* pNode = pMould->FindFirstChild();
	while (pNode)
	{
		if (pNode->IsAnAttribute())
		{
			NodeAttribute* pAttr = (NodeAttribute*)pNode;
			if (!DoHideNode(pAttr, TRUE))
				return FALSE;
		}
		pNode=pNode->FindNext();
	}
	return TRUE;
}




/********************************************************************************************

>	BOOL OpMouldLibSel::DoReplaceAllMoulds(	List* pNodeList,
											Path* pShape,
											MouldSpace mSpace,
											BOOL ScaleToFit,
											BOOL LockAspect)
 	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		pNodeList	= pointer to list of nodes to process
				pNewShape	= A path to use as a mould shape
				mSpace		= The mould space to create, currently only envelope and
						  	  perspective spaces are defined
	Returns:	TRUE if successfully replaced all mould shapes with the new shape.
				FALSE if failed.
	Purpose:	Scans all nodes in the node list. If moulds are found, their shapes will
				be replaced with pShape.

********************************************************************************************/

BOOL OpMouldLibSel::DoReplaceAllMoulds(	List* pNodeList,
										Path* pShape,
										MouldSpace mSpace,
										BOOL ScaleToFit,
										BOOL LockAspect)
{
	ERROR2IF(pNodeList==NULL, FALSE, "DoReplaceAllMoulds() given a null nodelist pointer");
	
	NodeListItem* CurItem = (NodeListItem*)(pNodeList->GetHead());
	if (!CurItem)
		return FALSE;

	while (CurItem)
	{
		if (CurItem->pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeMould))
		{
			NodeMould* pMould = (NodeMould*)(CurItem->pNode);
			if (!DoInvalidateNodeRegion(pMould, TRUE))
				return FALSE;

			if (!DoReplaceMould(pMould, pShape, mSpace, ScaleToFit, LockAspect))
				return FALSE;

			// Recalculate the mould's bounding box
			pMould->InvalidateBoundingRect();

			if (!DoInvalidateNodeRegion(pMould, TRUE))
				return FALSE;
		}
		CurItem = (NodeListItem*)(pNodeList->GetNext(CurItem));
	}

	GetApplication()->UpdateSelection();

	return TRUE;
}


/********************************************************************************************

>	BOOL OpMouldLibSel::DoReplaceMould(	NodeMould* pMould,
										Path* pShape,
										MouldSpace mSpace,
										BOOL ScaleToFit,
										BOOL LockAspect)
 	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		pMould		= pointer to a mould object
				pNewShape	= A path to use as a mould shape
				mSpace		= The mould space to create, currently only envelope and
						  	  perspective spaces are defined
	Returns:	TRUE if successfully removed the mould object
				FALSE if failed.
	Purpose:	Replaces one mould shape with another definition

********************************************************************************************/

BOOL OpMouldLibSel::DoReplaceMould(	NodeMould* pMould,
									Path* pNewShape,
									MouldSpace mSpace,
									BOOL ScaleToFit,
									BOOL LockAspect )
{
	ERROR2IF(pMould==NULL, FALSE, "DoReplaceMould() called with no mould pointer");
	ERROR2IF(pNewShape==NULL, FALSE, "DoReplaceMould() called with a NULL shape pointer");

	// cache the old geometry
	MouldGeometry* pGeometry=pMould->GetGeometry();
	if (!pGeometry)
		return FALSE;

	DocRect Bounds, DRect, *pRect;
	Bounds = pGeometry->GetSourceRect();

	if ((pMould->StartSaveContext(this, REC_REBUILD | REC_GEOMETRY)) == CC_FAIL)
		return FALSE;

	// create a new one (will automatically delete the old)
	if (!pMould->CreateGeometry(mSpace))
		return FALSE;

	// cache a pointer to the new geometry
	pGeometry=pMould->GetGeometry();
	if (!pGeometry)
		return FALSE;

	// check the shape we've been given is fine and lovely	
	UINT32 errID;
	if (!pGeometry->Validate(pNewShape,errID))
	{
		ERROR1(FALSE, errID);
	}

	// build a destination scale rectangle
	pRect = FindScaleRect(&Bounds, &DRect, ScaleToFit, LockAspect);

	// Find the current mould path object
	NodeMouldPath* pNodeMPath = pMould->GetPathShape();	
	if (!pNodeMPath)
		return FALSE;

	// Hide the mould path node
	if (!DoHideNode(pNodeMPath, FALSE, NULL))
		return FALSE;                

	// Create and add a mould shape object
	pNodeMPath = pMould->AddNewMouldShape(pNewShape,pRect,this);
	if (!pNodeMPath)
		return FALSE;

	// set the geometry using this new mould shape
	if (!pGeometry->Define(&(pNodeMPath->InkPath), &Bounds))
		return FALSE;

	// finally generate some new mould shapes.
	if (pMould->RemouldAll(this)==CC_FAIL)
		return FALSE;

	// Ilan 7/5/00
	// Inform geom linked attrs of the change. Nb outside the normal AllowOp mechanism
	NodeAttribute* pNA = pMould->FindFirstGeometryLinkedAttr();
	while(pNA)
	{
		pNA->LinkedNodeGeometryHasChanged(this);
		pNA = pNA->FindNextGeometryLinkedAttr();
	}

	if ((pMould->EndSaveContext(this, REC_REBUILD | REC_GEOMETRY)) == CC_FAIL)
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL OpMouldLibSel::DoRecordGeometry(NodeMould* pMould)
 
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		pMould	= pointer to a mould object
	Returns:	TRUE if successfully recorded the mould geometry
				FALSE if failed.
	Purpose:	Asks the geometry to record its own internal data and then records the
				type of geometry on the undo, ready to be replaced.

********************************************************************************************/

BOOL OpMouldLibSel::DoRecordGeometry(NodeMould* pMould)
{
	ActionCode Act;

	// call the actions static init function to get the action going.
	Act = RecordGeometryAction::DoRecord(this,pMould);
	if (Act==AC_FAIL)
		return FALSE;

	return TRUE;
}




/********************************************************************************************

>	OpMouldLib::OpMouldLib() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	OpMouldLib() constructor
	SeeAlso:	-

********************************************************************************************/

OpMouldLib::OpMouldLib()
{
	// Dummy constructor
}


/********************************************************************************************

>	BOOL OpMouldLib::DoRotateMould(NodeMould* pMould)

 	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		pMould		= pointer to a mould object
	Returns:	TRUE if successfully rotated the geometry around by one path element
	Purpose:	Simply rotates the orientation of all objects within the geometry.

********************************************************************************************/
/*
BOOL OpMouldLib::DoRotateMould(NodeMould* pMould)
{
	ERROR2IF(pMould==NULL, FALSE, "DoRotateMould() called with no mould pointer");

	MouldGeometry* pGeometry=pMould->GetGeometry();
	NodeMouldPath* pMouldPath=pMould->GetPathShape();
	if (!pMouldPath || !pGeometry)
		return FALSE;

	// cache the actual path shape
	Path* pEditPath = &(pMouldPath->InkPath);

	// record the current path data, ie its flags,verbs & coords
	if (SavePathArraysAction::DoRecord(this, &UndoActions, pEditPath) == AC_FAIL)
		return FALSE;

	// record the current mould shape on the undo and any blob rectangles
	if (pMould->SaveContext(this)==CC_FAIL)
		return FALSE;

	// rotate the path around
	INT32 index=0;
	if (!pEditPath->FindNextEndPoint(&index))
		return FALSE;
	if (!pEditPath->ChangeStartElement(index))
		return FALSE;

	// set the geometry using this new mould shape
	if (!pGeometry->Define(pEditPath, NULL))
		return FALSE;

	// finally generate some new mould shapes.
	if (pMould->RemouldAll(this)==CC_FAIL)
		return FALSE;

	return TRUE;
}
*/

BOOL OpMouldLib::DoRotateMould(NodeMould* pMould)
{
	ERROR2IF(pMould==NULL, FALSE, "DoRotateMould() called with no mould pointer");

	MouldGeometry* pGeometry=pMould->GetGeometry();
	NodeMouldPath* pMouldPath=pMould->GetPathShape();
	if (!pMouldPath || !pGeometry)
		return FALSE;

	// save the bits we need on the undo
	if ((pMould->StartSaveContext(this, REC_REBUILD | REC_GEOMCONTEXT | REC_PATHARRAYS)) == CC_FAIL)
		return FALSE;

	Path* pEditPath = &(pMouldPath->InkPath);
	// rotate the path around
	INT32 index=0;
	if (!pEditPath->FindNextEndPoint(&index))
		return FALSE;
	if (!pEditPath->ChangeStartElement(index))
		return FALSE;

	// set the geometry using this new mould shape
	if (!pGeometry->Define(pEditPath, NULL))
		return FALSE;

	// finally generate some new mould shapes.
	if (pMould->RemouldAll(this)==CC_FAIL)
		return FALSE;

	// Ilan 7/5/00
	// Inform geom linked attrs of the change. Nb outside the normal AllowOp mechanism
	NodeAttribute* pNA = pMould->FindFirstGeometryLinkedAttr();
	while(pNA)
	{
		pNA->LinkedNodeGeometryHasChanged(this);
		pNA = pNA->FindNextGeometryLinkedAttr();
	}

	if ((pMould->EndSaveContext(this, REC_REBUILD | REC_GEOMCONTEXT | REC_PATHARRAYS)) == CC_FAIL)
		return FALSE;

	return TRUE;
}



/********************************************************************************************

>	RecordGeometryAction::RecordGeometryAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95
	Purpose:	RecordGeometryAction constructor

********************************************************************************************/

RecordGeometryAction::RecordGeometryAction()
{
	pSavedMould=NULL;
	pSavedGeometry=NULL;
}

RecordGeometryAction::~RecordGeometryAction()
{
	if (pSavedGeometry!=NULL)
	{
		delete pSavedGeometry;
		pSavedGeometry=NULL;
	}
}


/********************************************************************************************

>	static ActionCode RecordGeometryAction::DoRecord(Operation* const pOp, 
													 NodeMould* const pMould)	

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/03/95  
	Inputs:		pOp		= The operation to which the action should be added
				pMould	= a pointer to a mould object we wish to record the geometry of
 	Outputs:	-
	Returns:	ActionCode:	 one of AC_OK, AC_NORESTORE, AC_FAIL
				           
	Purpose:	This simple action saves away the current mould space (geometry) defined by
				this mould object. The mould space may then be deleted externally and 
				replaced with another. During undo the new space will be recorded and the 
				old space rebuilt.

********************************************************************************************/

ActionCode RecordGeometryAction::DoRecord(Operation* const pOp, 
										  NodeMould* const pMould) 
{
	ERROR2IF(pMould==NULL,AC_FAIL,"Mould pointer is NULL in RecordGeometryAction()");
	RecordGeometryAction* GeomAction;
    // Attempt to initialise the next action    
    return RecordGeometryAction::Init(pOp, pOp->GetUndoActionList(), pMould, (Action**)(&GeomAction));
}




/********************************************************************************************

>	static ActionCode RecordGeometryAction::Init(Operation* const pOp, 
										  		 ActionList* pActionList, 	
										  		 MouldGeometry* pGeometry,
										  		 Action** NewAction)	

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/03/95  
	Inputs:		pOp: 		 The operation to which the action should be added
				pActionList: The action list in the operation object
				pMould:		 A pointer to a mould object whose geometry we will record.
 	Outputs:    NewAction:   A pointer to the action if it could be allocated. 
	Returns:	ActionCode:	 one of AC_OK, AC_NORESTORE, AC_FAIL
				           
	Purpose:	This simple action saves away the current mould space (geometry) defined by
				this mould object. The mould space may then be deleted externally and 
				replaced with another. During undo the new space will be recorded and the 
				old space rebuilt.

********************************************************************************************/

ActionCode RecordGeometryAction::Init(Operation* const pOp, 
					   				  ActionList* pActionList, 	
					                  NodeMould* const pMould,
					                  Action** NewAction)	
{		
	ERROR2IF(pMould==NULL,AC_FAIL,"Mould pointer is NULL in RecordGeometryAction()");
	MouldGeometry* pGeometry = pMould->GetGeometry();
	ERROR2IF(pGeometry==NULL, AC_FAIL, "There was no defined geometry in RecordGeometryAction");

	// find the size of the geometry we need to save
	ActionCode Ac = (Action::Init(pOp,
					 			  pActionList,
					 			  sizeof(RecordGeometryAction), 
					 			  CC_RUNTIME_CLASS(RecordGeometryAction), 
					 			  NewAction));
	if (*NewAction != NULL)
	{
		((RecordGeometryAction*)(*NewAction))->pSavedMould = pMould;
		((RecordGeometryAction*)(*NewAction))->pSavedGeometry = pGeometry;
	}

	return (Ac); 
} 



/********************************************************************************************

>	ActionCode RecordGeometryAction::Execute()   

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/03/95
	Inputs:		-
	Returns:	ActionCode indicating if the action was successfully executed or not
	Purpose:	Executes the InvalidateRectAction which invalidates a rectangular region
				of the current document.
    
********************************************************************************************/

ActionCode RecordGeometryAction::Execute()
{
	RecordGeometryAction* GeomAction;  
    ActionCode Ac;  
    // Attempt to initialise the next action    
    Ac = RecordGeometryAction::Init(pOperation, pOppositeActLst, pSavedMould, (Action**)(&GeomAction));
	
	if (Ac!=AC_FAIL)
	{   
		// set the geometry using this new mould shape
		if (!pSavedMould->SetGeometry(pSavedGeometry))
			return AC_FAIL;

		// Make sure the destructor doesn't vape it
		pSavedGeometry=NULL;
	}
	return Ac;
}   





/********************************************************************************************

>	InvalidateRectAction::InvalidateRectAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95
	Purpose:	InvalidateRectAction constructor

********************************************************************************************/

InvalidateRectAction::InvalidateRectAction()
{
	InvalidSpread=NULL;
}



/********************************************************************************************

>	static ActionCode InvalidateRectAction::DoRecord(Operation* const pOp, 
													 const DocRect& Rect, 
													 Spread* pSpread)	

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95  
	Inputs:		pOp		= The operation to which the action should be added
				Rect	= The rectangle to save
				pSpread	= The rectangles spread 
 	Outputs:	-
	Returns:	ActionCode:	 one of AC_OK, AC_NORESTORE, AC_FAIL
				           
	Purpose:	This simple action saves away a rectangle on the undo, which will be 
				invalidated during execution of undo and redo.

********************************************************************************************/

ActionCode InvalidateRectAction::DoRecord(	Operation* const pOp, 
											const DocRect& Rect, 
											Spread* pSpread)
{
	InvalidateRectAction* InvRctAct;  
    // Attempt to initialise the next action    
    return InvalidateRectAction::Init(pOp, pOp->GetUndoActionList(), Rect, pSpread, (Action**)(&InvRctAct));
}


/********************************************************************************************

>	static ActionCode InvalidateRectAction::Init(Operation* const pOp, 
										  		 ActionList* pActionList, 	
										  		 const DocRect& Rect,
										  		 Spread* pSpread,
										  		 Action** NewAction)	

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95  
	Inputs:		pOp: 		 The operation to which the action should be added
				pActionList: The action list in the operation object
				pSpread:	 The rectangles spread 
 	Outputs:    NewAction:   A pointer to the action if it could be allocated. 
	Returns:	ActionCode:	 one of AC_OK, AC_NORESTORE, AC_FAIL
				           
	Purpose:	This simple action invalidates a rectangular region.	

********************************************************************************************/
 
ActionCode InvalidateRectAction::Init(Operation* const pOp, 
					   				  ActionList* pActionList, 	
					                  const DocRect& Rect, 
					                  Spread* pSpread,  
					                  Action** NewAction)	
{		
	ActionCode Ac = (Action::Init(pOp,
					 			  pActionList,
					 			  sizeof(InvalidateRectAction), 
					 			  CC_RUNTIME_CLASS(InvalidateRectAction), 
					 			  NewAction));
	if (*NewAction != NULL)
	{
		((InvalidateRectAction*)(*NewAction))->InvalidRect = Rect;
		((InvalidateRectAction*)(*NewAction))->InvalidSpread = pSpread; 
	}
	return (Ac); 
} 



/********************************************************************************************

>	ActionCode InvalidateRectAction::Execute()   

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95
	Inputs:		-
	Returns:	ActionCode indicating if the action was successfully executed or not
	Purpose:	Executes the InvalidateRectAction which invalidates a rectangular region
				of the current document.
    
********************************************************************************************/

ActionCode InvalidateRectAction::Execute()
{
	InvalidateRectAction* InvRctAct;  
    ActionCode Ac;  
    // Attempt to initialise the next action    
    Ac = InvalidateRectAction::Init(pOperation, pOppositeActLst, InvalidRect, InvalidSpread, (Action**)(&InvRctAct));
	
	if (Ac!=AC_FAIL)
	{   
		// The action was successfully initialised   
		Document* pDocument = Document::GetCurrent();
		ENSURE(pDocument!=NULL, "There was no current document in InvalidateRectAction" );
		if (pDocument!=NULL)
			pDocument->ForceRedraw(InvalidSpread, InvalidRect);

	}
	return Ac;
}   


/********************************************************************************************

>	RedrawBoundsAction::RedrawBoundsAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94

********************************************************************************************/

RedrawBoundsAction::RedrawBoundsAction()
{
	ChangedNode = NULL;
}


ActionCode RedrawBoundsAction::DoRecord(Operation* pOp, NodeMould* WhichNode, BOOL Redraw)
{
	RedrawBoundsAction* RecAction;
	ActionCode Act = RedrawBoundsAction::Init(pOp, 
											  pOp->GetUndoActionList(),
											  WhichNode,
											  (Action**)&RecAction);
	if (Act != AC_FAIL && Redraw)
		RedrawNode(pOp, WhichNode);
	return Act;
}


ActionCode RedrawBoundsAction::Init(Operation* pOp,
									ActionList* pActionList,
									NodeMould* WhichNode,
									Action** NewAction)
{
	UINT32 ActSize = sizeof(RedrawBoundsAction);
	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(RedrawBoundsAction), NewAction);
	if ((Ac == AC_OK) && (*NewAction != NULL))
		((RedrawBoundsAction*)*NewAction)->ChangedNode = WhichNode;

	return Ac;
}


ActionCode RedrawBoundsAction::Execute()
{
	RedrawBoundsAction* ReAction;
	ActionCode Act;
	Act = RedrawBoundsAction::Init(pOperation, pOppositeActLst, ChangedNode, (Action**)(&ReAction));
	if (Act != AC_FAIL)
		RedrawNode(pOperation, ChangedNode);
	return Act;
}


void RedrawBoundsAction::RedrawNode(Operation* pOp, NodeMould* pNode)
{
	Document* pDoc = pOp->GetWorkingDoc();
	ERROR3IF( pDoc == NULL, "There was no current document when undoing RecordMouldBounds" );
	Spread* pSpread = pNode->FindParentSpread();
	DocRect Invalid = pNode->GetChildrensBounds();
	pDoc->ForceRedraw( pSpread, Invalid, FALSE, pNode);
}





/********************************************************************************************

>	SaveDetachAction::SaveDetachAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95
	Purpose:	SaveDetachAction constructor

********************************************************************************************/

SaveDetachAction::SaveDetachAction()
{
	pCurrMould=NULL;
	CurrDetached=FALSE;
}


/********************************************************************************************

>	static ActionCode SaveDetachAction::Save(Operation* const pOp, 
											 NodeMould* const pMould)	

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95  
	Inputs:		pOp		= The operation to which the action should be added
				pMould  = The mould object whose data we are saving
 	Outputs:	-
	Returns:	ActionCode:	 one of AC_OK, AC_NORESTORE, AC_FAIL
				           
	Purpose:	This simple action saves away a rectangle on the undo, which will be 
				invalidated during execution of undo and redo.

********************************************************************************************/

ActionCode SaveDetachAction::Save(Operation* const pOp, 
								  NodeMould* const pMould) 
{
	SaveDetachAction* SvDetAct;  
    // Attempt to initialise the next action    
    return SaveDetachAction::Init(pOp, pOp->GetUndoActionList(), pMould, (Action**)(&SvDetAct));
}



/********************************************************************************************

>	static ActionCode SaveDetachAction::Init(Operation* const pOp,
 												 ActionList* pActionList, 
 								  		 
										  		 Action** NewAction)	

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95  
	Inputs:		pOp: 		 The operation to which the action should be added
				pActionList: The action list in the operation object
				pSpread:	 The rectangles spread 
 	Outputs:    NewAction:   A pointer to the action if it could be allocated. 
	Returns:	ActionCode:	 one of AC_OK, AC_NORESTORE, AC_FAIL
				           
	Purpose:	An action to save away the state of the detach flag.	

********************************************************************************************/
 
ActionCode SaveDetachAction::Init(	Operation* const pOp, 
					   				ActionList* pActionList, 	
					                NodeMould* const pMould,   
									Action** NewAction)	
{		
	ActionCode Ac = (Action::Init(pOp,
					 			  pActionList,
					 			  sizeof(SaveDetachAction), 
					 			  CC_RUNTIME_CLASS(SaveDetachAction), 
					 			  NewAction));
	if (*NewAction != NULL)
	{
		((SaveDetachAction*)(*NewAction))->pCurrMould = pMould;
		((SaveDetachAction*)(*NewAction))->CurrDetached = pMould->IsDetached();
	}
	return (Ac); 
} 



/********************************************************************************************

>	ActionCode SaveDetachAction::Execute()   

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95
	Inputs:		-
	Returns:	ActionCode indicating if the action was successfully executed or not
	Purpose:	Executes the SaveDetachAction
    
********************************************************************************************/

ActionCode SaveDetachAction::Execute()
{
	SaveDetachAction* MyAct;  
    ActionCode Ac = SaveDetachAction::Init(	pOperation,
    									    pOppositeActLst,
    							    		pCurrMould,
    							    		(Action**)(&MyAct));
	
	if (Ac!=AC_FAIL)
	{
		if ((pCurrMould->IsDetached())!=CurrDetached)
			pCurrMould->ToggleDetachFlag();
	}
	return Ac;
}   






/********************************************************************************************

>	OpDragOrigin::OpDragOrigin()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/95
	Purpose:	Constructs an Operation that allows the user to drag vanishing points of
				a perspective mould

********************************************************************************************/

OpDragOrigin::OpDragOrigin()
{
	StartSpread 	= NULL;
	pEditMould		= NULL;
	pEditMouldPath	= NULL;
	pEditPath		= NULL;
	pEditGeometry	= NULL;
}

/********************************************************************************************

>	BOOL OpDragOrigin::Declare()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the op is correctly registered.
	Purpose:	Registers the rotation centre drag Operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpDragOrigin::Init()
{
	return RegisterOpDescriptor(0, /* _R(IDS_DRAGOriginOP), */ 
								_R(IDS_DRAGORIGINOP),
								CC_RUNTIME_CLASS(OpDragOrigin),
								OPTOKEN_DRAGVANISHPOINT, 
								OpDragOrigin::GetState,
								0,
								_R(IDBBL_DRAGORIGINOP));
}



/********************************************************************************************

>	OpState OpDragOrigin::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns a default OpState.
	Purpose:	Controls whether the rotation-centre mouse drag operation is available or
				not.  Currently, is always available (the internal logic of the selector
				tool & info-bar handle this really).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpDragOrigin::GetState(String_256*, OpDescriptor*)
{
	OpState os;
	return os;
}


/********************************************************************************************

>	virtual void OpDragOrigin::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to this type of operation.

********************************************************************************************/

void OpDragOrigin::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_DRAGVPOINT));
}

/********************************************************************************************

>	void OpDragOrigin::DoDragVanishPoint(Spread* pSpread, 
										 const DocCoord& dcPos,
										 ClickModifiers mods,
										 NodeMould* pMould,
										 BOOL WhichBlob)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/95
	Inputs:		pSpread	= the spread containing the rotation centre blob
				dcPos	= the initial position of the blob
				mods	= the mouse button click modifiers (eg ctrl click etc)
				pMould	= a pointer to the mould we will drag
	Outputs:	-
	Returns:	-
	Purpose:	Starts a drag of one of the perspective moulds vanishing points.
				Note, a lot of effort has been put into the redraw of perspectives when
				dragging vanishing points. We must make sure the vanishing point rectangles
				are redrawn separately to the actual mould contents. If not it is v likely
				that you will get a huge rectangle as a result of the union. Redrawing this
				area is very bad.
				Also note, we do not drag a copy of the envelope or perspective control
				polygon but the control polygon itself. This means we must make sure when
				we scroll we prevent the mould object from attempting to eor its blobs on,
				this has the tendancy to leave large amounts of unsightly eor all over the
				place.

********************************************************************************************/

void OpDragOrigin::DoDragVanishPoint(Spread* pSpread, 
									 const DocCoord& dcPos,
									 ClickModifiers mods,
									 NodeMould* pMould,
									 BOOL WhichBlob)
{
	ERROR3IF(pMould==NULL,"DoDragVanishPoint() given a null mould object");
	DocCoord v0,v1;

	// Remember the starting positions etc and initiate a drag.
	StartSpread = pSpread;
	pEditMould	= pMould;

	// try to find the geometry
	MouldGeometry* pGeometry = pMould->GetGeometry();
	if (!pGeometry->IsKindOf(CC_RUNTIME_CLASS(MouldPerspective)))
	{
		ERROR3("DoDragVanishPoint() called on an illegal geometry");
		goto Failed;
	}

	pEditGeometry = (MouldPerspective*)pGeometry;

	// read the current vanishing point positions for this mould.
	pEditGeometry->GetVanishingPoints(v0,v1);
	WhichBlob ? (Blob=v1) : (Blob=v0);

 	// get hold of the moulds nodepath object
	pEditMouldPath = pMould->GetPathShape();
	if (!pEditMouldPath)
		goto Failed;

	// cache the actual path shape
	pEditPath = &(pEditMouldPath->InkPath);

	// record an action to redraw the node in its current state during undo/redo
	// we use our own special action to do this RecordBoundsAction uses the nasty
	// GetUnionBlobBounds() which unions the vanishing points in there too.
	// PS.. Pass FALSE into this function to avoid redraw at this stage
	if (RedrawBoundsAction::DoRecord(this,pEditMould,FALSE)==AC_FAIL)
		goto Failed;
	
	// Save the current rectangular bounds to redraw after the dragop has finished
	OpDragOrigin::EditRect = pMould->GetChildrensBounds();

	// record the current path data, ie its flags,verbs & coords
	if ((pEditMould->StartSaveContext(this, REC_REBUILD | REC_BLOBS | REC_GEOMCONTEXT | REC_PATHARRAYS)) == CC_FAIL)
		goto Failed;

/*	if (SavePathArraysAction::DoRecord(this, &UndoActions, pEditPath) == AC_FAIL)
		goto Failed;

	// record the current mould shape on the undo and any blob rectangles
	if (pEditMould->SaveContext(this)==CC_FAIL)
		goto Failed;
*/
	// turn the display blobs off to prevent the mould from drawing them on
	// during a scroll.
	pEditMould->DisableBlobs();

	// start off the drag and get ready
	StartDrag(DRAGTYPE_AUTOSCROLL);
	return;

Failed:
	FailAndExecute();
	End();
}



/********************************************************************************************

>	void OpDragOrigin::DragPointerMove(DocCoord dcPos, ClickModifiers, Spread* pSpread, BOOL bSolidDrag)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95
	Inputs:		dcPos	= the current mouse position
				pSpread	= the spread containing the mouse
	Outputs:	-
	Returns:	-
	Purpose:	Called while the vanishing point is being dragged around.  Erases it
				from its old position and redraws it at its new position.

********************************************************************************************/

void OpDragOrigin::DragPointerMove(DocCoord dcPos, ClickModifiers mods, Spread* pSpread, BOOL bSolidDrag)
{	
	// If the mouse has moved outside the spread the drag was started on then we must
	// account for this.
	if (pSpread != StartSpread)
		dcPos = MakeRelativeToSpread(StartSpread, pSpread, dcPos);

	// Erase the old blobs.
	RenderDragBlobs(CalcBlobClipRect(), StartSpread, bSolidDrag);

	if (pEditGeometry->MoveVanishingPoint(Blob,dcPos))
	{
		Blob=dcPos;
		// Update the new shape in the path.
		DocCoord* pCoords = pEditPath->GetCoordArray();
		pEditGeometry->CopyShape(pCoords);
	}	

	// Draw on the new blobs
	RenderDragBlobs(CalcBlobClipRect(), StartSpread, bSolidDrag);
}




/********************************************************************************************

>	void OpDragOrigin::DragFinished(DocCoord PointerPos, 
									ClickModifiers ClickMods,
									Spread* pSpread,
									BOOL success,
									BOOL bSolidDrag)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95
	Inputs:		PointerPos	= final position of the mouse
				ClickMods	= the click modifiers.
				pSpread		= spread containing the mouse
				success		= whether the drag was cancelled or not
	Outputs:	-
	Returns:	-
	Purpose:	Update the perspective mould object with the final resting place of this
				mould vanishing point.

********************************************************************************************/
void OpDragOrigin::DragFinished(DocCoord dcPos,	    
								ClickModifiers mods,
								Spread* pSpread,
								BOOL success,
								BOOL bSolidDrag)
{
	EndDrag();
	Document* pDoc = Document::GetCurrent();
	NodeAttribute* pNA = NULL;

	// Make sure the nodes blob rendering is enabled again
	pEditMould->EnableBlobs();

	// Act on the termination status of the drag operation.
	if (!success)
	{
		// Get rid of what ever new stuff we have, the unwind handler will
		// cause our original stuff to be replaced.
		RenderDragBlobs(CalcBlobClipRect(), StartSpread, bSolidDrag);
		goto Failed;
	}

	// Turn that hourglass on
	BeginSlowJob();

	// Make sure we invalidate the edit object
	pEditMouldPath->InvalidateBoundingRect();

	// Force a redraw on the original position. 
	// Note, we cannot do this before we start dragging as EORing goes mad!
	if (pDoc)
		pDoc->ForceRedraw( StartSpread, OpDragOrigin::EditRect, FALSE, pEditMould);

	if (pEditMould->RemouldAll(this)==CC_FAIL)
		goto Failed;

	// Ilan 7/5/00
	// Inform geom linked attrs of the change. Nb outside the normal AllowOp mechanism
	pNA = pEditMould->FindFirstGeometryLinkedAttr();
	while(pNA)
	{
		pNA->LinkedNodeGeometryHasChanged(this);
		pNA = pNA->FindNextGeometryLinkedAttr();
	}

	if ((pEditMould->EndSaveContext(this, REC_REBUILD | REC_BLOBS | REC_GEOMCONTEXT | REC_PATHARRAYS)) == CC_FAIL)
		goto Failed;

	// record an action to redraw the state of the mould (but not vanishpoints)
	if (RedrawBoundsAction::DoRecord(this,pEditMould,TRUE)==AC_FAIL)
		goto Failed;

	// record the vanish points separately (important to avoid unionblobbounds()!)
	if (pEditGeometry->RecordBlobs(this,StartSpread)==CC_FAIL)
		goto Failed;

	End();
	return;

Failed:
	FailAndExecute();
	End();
	return;
}


/********************************************************************************************

>	void OpDragOrigin::RenderDragBlobs(DocRect Clip, Spread* pSpread, BOOL bSolidDrag)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/95
	Inputs:		Clip	= clipping rectangle for output
				pSpread	= the spread to render into
	Outputs:	-
	Returns:	-
	Purpose:	Draws the drag path and new vanishing point positions

********************************************************************************************/

void OpDragOrigin::RenderDragBlobs(DocRect Clip, Spread* pSpread, BOOL bSolidDrag)
{
	RenderRegion* pRegion = DocView::RenderOnTop(&Clip, pSpread, ClippedEOR);
	while (pRegion != NULL)
	{
		// Get the edit path to render itself
		pEditMouldPath->RenderObjectBlobs(pRegion);
		pEditGeometry->RenderControlBlobs(pRegion);

		pRegion = DocView::GetNextOnTop(&Clip);
	}
}


/********************************************************************************************

>	DocRect OpDragOrigin::CalcBlobClipRect()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		-
	Outputs:	-
	Returns:	A clipping rectangle for use by RenderDragBlobs.
	Purpose:	Constructs a rectangle that surrounds the objects we are eoring

********************************************************************************************/

DocRect OpDragOrigin::CalcBlobClipRect()
{
	// get the current vanish point clip rects
	DocRect Rect0 = pEditGeometry->GetBlobBoundingRect();
	DocRect	Rect1 = pEditMouldPath->GetBlobBoundingRect();

	return Rect0.Union(Rect1);
}







/********************************************************************************************

>	StartRebuildMouldAction::StartRebuildMouldAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95
	Purpose:	RebuildMouldAction constructor

********************************************************************************************/

StartRebuildMouldAction::StartRebuildMouldAction()
{
	pRebuildMould=NULL;
}



/********************************************************************************************

>	static ActionCode StartRebuildMouldAction::DoRecord(Operation* const pOp, NodeMould* pMould)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95  
	Inputs:		pOp		= The operation to which the action should be added
				pMould	= A pointer to a mould to rebuild
 	Outputs:	-
	Returns:	ActionCode:	 one of AC_OK, AC_NORESTORE, AC_FAIL
	Purpose:	

********************************************************************************************/

ActionCode StartRebuildMouldAction::DoRecord(Operation* const pOp, NodeMould* pMould)
{
	StartRebuildMouldAction* Act;  
    // Attempt to initialise the next action    
    return StartRebuildMouldAction::Init(pOp, pOp->GetUndoActionList(), pMould, (Action**)(&Act));
}


/********************************************************************************************

>	static ActionCode StartRebuildMouldAction::Init(	Operation* const pOp, 
										  		 ActionList* pActionList, 	
										  		 NodeMould* pMould,
										  		 Action** NewAction)	

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95  
	Inputs:		pOp: 		 The operation to which the action should be added
				pActionList: The action list in the operation object
				pMould:		 A pointer to a mould to rebuild
 	Outputs:    NewAction:   A pointer to the action if it could be allocated. 
	Returns:	ActionCode:	 one of AC_OK, AC_NORESTORE, AC_FAIL
				           
	Purpose:	This simple action invalidates a rectangular region.	

********************************************************************************************/
 
ActionCode StartRebuildMouldAction::Init(Operation* const pOp, 
				   					ActionList* pActionList, 	
							  		NodeMould* pMould,
					                Action** NewAction)	
{		
	ActionCode Ac = (Action::Init(pOp,
					 			  pActionList,
					 			  sizeof(StartRebuildMouldAction), 
					 			  CC_RUNTIME_CLASS(StartRebuildMouldAction), 
					 			  NewAction));
	if (*NewAction != NULL)
	{
		((StartRebuildMouldAction*)(*NewAction))->pRebuildMould = pMould;
	}
	return (Ac); 
} 



/********************************************************************************************

>	ActionCode RebuildMouldAction::Execute()   

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95
	Inputs:		-
	Returns:	ActionCode indicating if the action was successfully executed or not
	Purpose:	Executes the RebuildMouldAction which invalidates a rectangular region
				of the current document.
    
********************************************************************************************/

ActionCode StartRebuildMouldAction::Execute()
{
	EndRebuildMouldAction* Act;  
    ActionCode Ac;  
    // Attempt to initialise the next action    
    Ac = EndRebuildMouldAction::Init(pOperation, pOppositeActLst, pRebuildMould, (Action**)(&Act));
	
	if (Ac!=AC_FAIL)
	{   
		if (!pRebuildMould->IsDetached())
		{
			// The action was successfully initialised   
			NodeMoulder* pMoulder = pRebuildMould->FindFirstMoulder();
			NodeMouldGroup* pMouldGroup = pRebuildMould->FindMouldGroup();

			if (pMoulder!=NULL && pMouldGroup!=NULL)
			{
				pMoulder->DestroyMouldedObjects();
				if (!pMoulder->CreateMouldedObjects(pMouldGroup, pRebuildMould->GetGeometry(), NULL)) 
				{
					pMoulder->DestroyMouldedObjects();
					return AC_FAIL;
				}
			}
		}
	}
	return Ac;
}   



/********************************************************************************************

>	EndRebuildMouldAction::EndRebuildMouldAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95
	Purpose:	RebuildMouldAction constructor

********************************************************************************************/

EndRebuildMouldAction::EndRebuildMouldAction()
{
	pRebuildMould=NULL;
}


/********************************************************************************************

>	static ActionCode EndRebuildMouldAction::DoRecord(Operation* const pOp, NodeMould* pMould)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95  
	Inputs:		pOp		= The operation to which the action should be added
				pMould	= A pointer to a mould to rebuild
 	Outputs:	-
	Returns:	ActionCode:	 one of AC_OK, AC_NORESTORE, AC_FAIL
	Purpose:	

********************************************************************************************/

ActionCode EndRebuildMouldAction::DoRecord(Operation* const pOp, NodeMould* pMould)
{
	EndRebuildMouldAction* Act;  
    // Attempt to initialise the next action    
    return EndRebuildMouldAction::Init(pOp, pOp->GetUndoActionList(), pMould, (Action**)(&Act));
}

/********************************************************************************************

>	static ActionCode EndRebuildMouldAction::Init(	Operation* const pOp, 
										  		 ActionList* pActionList, 	
										  		 NodeMould* pMould,
										  		 Action** NewAction)	

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95  
	Inputs:		pOp: 		 The operation to which the action should be added
				pActionList: The action list in the operation object
				pMould:		 A pointer to a mould to rebuild
 	Outputs:    NewAction:   A pointer to the action if it could be allocated. 
	Returns:	ActionCode:	 one of AC_OK, AC_NORESTORE, AC_FAIL
	Purpose:

********************************************************************************************/
 
ActionCode EndRebuildMouldAction::Init(Operation* const pOp, 
				   					ActionList* pActionList, 	
							  		NodeMould* pMould,
					                Action** NewAction)	
{		
	ActionCode Ac = (Action::Init(pOp,
					 			  pActionList,
					 			  sizeof(EndRebuildMouldAction), 
					 			  CC_RUNTIME_CLASS(EndRebuildMouldAction), 
					 			  NewAction));
	if (*NewAction != NULL)
	{
		((EndRebuildMouldAction*)(*NewAction))->pRebuildMould = pMould;
	}
	return (Ac); 
} 



/********************************************************************************************

>	ActionCode EndRebuildMouldAction::Execute()   

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95
	Inputs:		-
	Returns:	ActionCode indicating if the action was successfully executed or not
	Purpose:	Executes the RebuildMouldAction which invalidates a rectangular region
				of the current document.
    
********************************************************************************************/

ActionCode EndRebuildMouldAction::Execute()
{
	StartRebuildMouldAction* Act;  
    // Attempt to initialise the next action    
    return StartRebuildMouldAction::Init(pOperation, pOppositeActLst, pRebuildMould, (Action**)(&Act));
}   







/********************************************************************************************

>	BuildMouldAction::BuildMouldAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/95
	Purpose:	BuildMouldAction constructor

********************************************************************************************/

BuildMouldAction::BuildMouldAction()
{
	pBuildMould=NULL;
}


/********************************************************************************************
********************************************************************************************/

ActionCode BuildMouldAction::DoRecord(Operation* const pOp, NodeMould* pMould)
{
	EndRebuildMouldAction* Act;  
    // Attempt to initialise the next action    
    return BuildMouldAction::Init(pOp, pOp->GetUndoActionList(), pMould, (Action**)(&Act));
}


/********************************************************************************************
********************************************************************************************/
 
ActionCode BuildMouldAction::Init(Operation* const pOp, 
				   					ActionList* pActionList, 	
							  		NodeMould* pMould,
					                Action** NewAction)	
{		
	ActionCode Ac = (Action::Init(pOp,
					 			  pActionList,
					 			  sizeof(BuildMouldAction), 
					 			  CC_RUNTIME_CLASS(BuildMouldAction), 
					 			  NewAction));
	if (*NewAction != NULL)
	{
		((BuildMouldAction*)(*NewAction))->pBuildMould = pMould;
	}
	return (Ac); 
} 


/********************************************************************************************
********************************************************************************************/

ActionCode BuildMouldAction::Execute()
{
	ActionCode Ac = AC_OK;
	OperationStatus OpStat = pOperation->OpStatus;
	BuildMouldAction Act;

    Ac = BuildMouldAction::Init(pOperation, pOppositeActLst, pBuildMould, (Action**)(&Act));

	if (Ac != AC_FAIL)
	{
		NodeMoulder* pMoulder = pBuildMould->FindFirstMoulder();
		NodeMouldGroup* pMouldGroup = pBuildMould->FindMouldGroup();

		if (pMoulder!=NULL && pMouldGroup!=NULL)
		{
			// DO, UNDO, REDO destroy all moulder objects
			pMoulder->DestroyMouldedObjects();

			// for DO and REDO we need to recreate them
			if (OpStat==DO || OpStat==REDO)
			{
				if (!pMoulder->CreateMouldedObjects(pMouldGroup, pBuildMould->GetGeometry(), NULL)) 
				{
					// failed to recreate so toast em.
					pMoulder->DestroyMouldedObjects();
					Ac = AC_FAIL;
				}
			}
		}
	}

	return Ac;
}

