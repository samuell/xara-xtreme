// $Id: brshbeca.cpp 662 2006-03-14 21:31:49Z alex $
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

// Implementation of BrushBecomeA classes

#include "camtypes.h"
#include "brshbeca.h"
#include "becomea.h"
#include "undoop.h"
#include "ink.h"
#include "attrmap.h"
#include "nodepath.h"
#include "nodebldr.h"
#include "group.h"
#include "ophist.h"

CC_IMPLEMENT_MEMDUMP(HandleBrushBecomeA, CC_CLASS_MEMDUMP);
CC_IMPLEMENT_MEMDUMP(BrushBecomeA, BecomeA);
CC_IMPLEMENT_MEMDUMP(SimpleBecomeA, BecomeA);
CC_IMPLEMENT_MEMDUMP(BrushBecomeAGroup, BecomeA);

/***********************************************************************************************

>	HandleBrushBecomeA::HandleBrushBecomeA()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor

***********************************************************************************************/

HandleBrushBecomeA::HandleBrushBecomeA()
{
	m_pContextNode    = NULL;
	m_pCreatedByNode  = NULL;
	m_pNodeGroup      = NULL; 
	m_pBecomeA		  = NULL;
	m_LastStep		  = -1;
	m_NumBlendPaths   = 0;
	m_bFirst		  = TRUE;
	m_bSecondary		= FALSE;
}


/***********************************************************************************************

>	HandleBrushBecomeA::~HandleBrushBecomeA()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor

***********************************************************************************************/

HandleBrushBecomeA::~HandleBrushBecomeA()
{
	if (m_pContextNode != NULL)
	{
		m_pContextNode->CascadeDelete();
		delete m_pContextNode;
		m_pContextNode = NULL;
	}
}


/***********************************************************************************************

>	BOOL HandleBrushBecomeA::Initialise(BecomeA* pBecomeA, Node* pContextNode, Node* pCreatedBy)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/2000
	Inputs:		pContextNode - the position in the tree to insert new nodes
				pCreatedBy - the node that created these objects
				pBecomeA - the BecomeA object that tells us what to do and receives any passbacks
	Outputs:	-
	Returns:	TRUE, unless you pass in  a duff pointer
	Purpose:	initialises the HBecA object ready for use

***********************************************************************************************/

BOOL HandleBrushBecomeA::Initialise(BecomeA* pBecomeA, Node* pContextNode, Node* pCreatedBy)
{
	ERROR2IF(pBecomeA == NULL, FALSE, "BecomeA pointer is NULL in HandleBrushBecomeA::Initialise");
	ERROR2IF(pCreatedBy == NULL, FALSE, "Parent node is NULL in HandleBrushBecomeA::Initialise");
	// if we are replacing a node in the tree then we need to ha
	if (pBecomeA->GetReason() == BECOMEA_REPLACE)
		ERROR2IF(pContextNode == NULL, FALSE, "Context Node is NULL in HandleBrushBecomeA::Initialise");

	m_pBecomeA = pBecomeA;
	m_pContextNode = pContextNode;
	m_pCreatedByNode = pCreatedBy;
	return TRUE;
}


/***********************************************************************************************

>	BOOL HandleBrushBecomeA::PassBack(BlendPath* pBlendPath, AttrMap* pAttrMap, UINT32 StepNum))

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/2000
	Inputs:		pBlendPath - a blendpath that has been generated by the brush	
				pAttrMap - the attributes that apply to the blendpath
				StepNum	  - the step counter of the brush
	Outputs:	-
	Returns:	TRUE, unless something goes wrong
	Purpose:	This takes a path and an attribute map generated by a PathProcessorBrush,
				and generates a NodePath object, if it wants to become a path object
				If pBecomeA->Reason == BECOMEA_REPLACE,  the NodePath and attributes are added to the tree.
				If pBecomeA->Reason == BECOMEA_PASSBACK, the NodePath is passed to the routine.

	SeeAlso:    This routine is based upon the HandleBecomeA routine in nodebldr.cpp
***********************************************************************************************/

BOOL HandleBrushBecomeA::Passback(Path* pPath, CCAttrMap* pAttrMap, UINT32 StepNum)
{
	ERROR2IF(pPath == NULL, FALSE, "pPath is NULL in HandleBecomeA::Passback");
	ERROR2IF(pAttrMap == NULL, FALSE, "pAttrMap is NULL in HandleBecomeA::Passback");
	ERROR2IF(m_pBecomeA == NULL, FALSE, "BecomeA is NULL in HandleBecomeA::Passback");

	BOOL ValidReason = (m_pBecomeA->GetReason() == BECOMEA_REPLACE || m_pBecomeA->GetReason() == BECOMEA_PASSBACK);
	ERROR3IF_PF(!ValidReason,("Unkown BecomeA reason %d",m_pBecomeA->GetReason()));
	if (!ValidReason) return FALSE;

	if (m_pBecomeA->GetReason()== BECOMEA_REPLACE)
		ERROR2IF(m_pContextNode == NULL, FALSE, "Context Node is NULL in HandleBrushBecomeA::Passback");

	BOOL Success = FALSE;
	NodePath* pNewNodePath = NULL;
	if (m_pBecomeA->BAPath())
	{
		// We need to create a new NodePath, no matter what the reason.
		
		// Allocate a new NodePath node
		ALLOC_WITH_FAIL(pNewNodePath, (new NodePath), m_pBecomeA->GetUndoOp());
		Success = (pNewNodePath != NULL);

		// Initialise the path
		if (Success) CALL_WITH_FAIL(pNewNodePath->InkPath.Initialise(pPath->GetNumCoords(),12), m_pBecomeA->GetUndoOp(), Success);
		if (Success) CALL_WITH_FAIL(pNewNodePath->InkPath.CopyPathDataFrom(pPath), m_pBecomeA->GetUndoOp(), Success);

		// If Success is TRUE, then we now have a new NodePath object that contains this shape's path

		// By default, attach the new path as the previous child of the context node
		Node* 				pAttachNode = m_pContextNode;
		AttachNodeDirection AttachDir = NEXT;
		
		// if this is the very first item then insert at first child
		if (m_bFirst)
		{
			AttachDir = LASTCHILD;
			m_bFirst = FALSE;
		}
	

		// By default, create a hide node action for the new path we're attaching to the tree
		// note - I've Changed my mind - because brushes can contain so many objects, by inserting an action 
		// for each one we're eating up tons of unneccessary space. Instead call HideNode when
		// you're done so ou only do it once.
		BOOL	HideNode	= TRUE;
		Node*	pNodeToHide	= pNewNodePath;


		// Should we be placing all created paths inside a group?
		if (MakeGroupForThisStep())
		{
			// Do we need to create a new group? I.e. is this the first path created for this step of the blend?
			if (StepNum == 1)
			{
				// Create a new group and place it in the tree next to the context node
				m_pNodeGroup 	= new NodeGroup;
				Success 	= (m_pNodeGroup != NULL);
				if (Success)
				{
					// attach group to our context node
					m_pNodeGroup->AttachNode(m_pContextNode,LASTCHILD);

					// Make a hide node action for this group
					HideNode = TRUE;
					pNodeToHide = m_pNodeGroup;
				}
			}
			else
			{
				pAttachNode = m_pNodeGroup;
				HideNode = FALSE;	// We created a hide node action when this group was created
			}
			
			// If we have a group node, make sure the new path gets placed in the tree as the
			// group's last child
			if (Success)
			{
				pAttachNode = m_pNodeGroup;
				AttachDir 	= LASTCHILD;
			}
		}
		else
			AttachDir = LASTCHILD;

		
		if (Success)
		{
		 	switch (m_pBecomeA->GetReason())
			{
		 		case BECOMEA_REPLACE :
				{
					// It's a BECOMEA_REPLACE, so put the new NodePath in the tree in an undoable way

					// Can't do it in an undoable way without an Undo Op
//					ERROR2IF_PF(m_pBecomeA->GetUndoOp() == NULL,FALSE,("GetUndoOp() returned NULL"));

					if (Success)
					{
						ERROR2IF(pAttachNode == NULL,FALSE,"The attach node is NULL!!!");

						// Insert the new NodePath into the tree
						pNewNodePath->AttachNode(pAttachNode,AttachDir);

						// Apply all the attributes to the new path
						pNewNodePath->ApplyAttributes(pAttrMap,TRUE);

						if (m_pBecomeA->GetUndoOp())
						{
							// Set the bounds  
							pNewNodePath->ValidateBoundingRect();
							pNewNodePath->InvalidateBoundingRect();

							// Create a hide node action to hide the node when we undo 
							if (HideNode)
							{
								ERROR2IF(pNodeToHide == NULL,FALSE,"pNodeToHide is NULL");

								HideNodeAction* UndoHideNodeAction;     
								Success = (HideNodeAction::Init(m_pBecomeA->GetUndoOp(),
											  			 m_pBecomeA->GetUndoOp()->GetUndoActionList(),
									 					 pNodeToHide,
									 					 TRUE, 		 // Include subtree size 
							  		 					 ( Action**)(&UndoHideNodeAction))
							  		  					 != AC_FAIL);
							}
						}
					}

					// We need to create a copy of the blended attributes to give to the caller
					// as they don't exist in the tree, and the ones in pAttrMap will be
					// deleted eventually.
					ERROR2IF(m_pCreatedByNode == NULL, FALSE, "Parent node is NULL in HandleBrushBecomeA::Passback");
					CCAttrMap* pNewAttrMap = pAttrMap->Copy(); // was causing memory leaks
					if (pNewAttrMap != NULL)
						m_pBecomeA->PassBack(pNewNodePath,(NodeRenderableInk*)m_pCreatedByNode,pNewAttrMap);
				}
				break;

				case BECOMEA_PASSBACK :
				{
					if (Success)
					{
						ERROR2IF(pAttachNode == NULL,FALSE,"The attach node is NULL!!!");

						// Insert the new NodePath into the tree
						pNewNodePath->AttachNode(pAttachNode,AttachDir);

						// Apply all the attributes to the new path
						pNewNodePath->ApplyAttributes(pAttrMap,TRUE);

						// Set the bounds  
						pNewNodePath->ValidateBoundingRect();
						pNewNodePath->InvalidateBoundingRect();
					}
					
					// We need to create a copy of the blended attributes to give to the caller
					// as they don't exist in the tree, and the ones in pAttrMap will be
					// deleted eventually.
					ERROR2IF(m_pCreatedByNode == NULL, FALSE, "Parent node is NULL in HandleBrushBecomeA::Passback");
					CCAttrMap* pNewAttrMap = pAttrMap->Copy(); // was causing memory leaks
					if (pNewAttrMap != NULL)
						Success = m_pBecomeA->PassBack(pNewNodePath,(NodeRenderableInk*)m_pCreatedByNode,pNewAttrMap);
					else
						Success = FALSE;
				}
				break;

				case BECOMEA_TEST :
					ERROR3("This case wasn't handled by the switch and generated a warning");
					break;
			} // end switch

		} //end if
		else //not success so delete our nodepath
		{
			if (pNewNodePath != NULL)
				delete pNewNodePath;
		}
	} // end if pBecomeA->BAPath()

	return Success;
}


/***********************************************************************************************

>	BOOL HandleBrushBecomeA::HideContextNode()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, etc.
	Purpose:	inserts an action to hide the context node

***********************************************************************************************/

BOOL HandleBrushBecomeA::HideContextNode()
{
	ERROR2IF(m_pBecomeA == NULL, FALSE, "BecomeA is NULL in HandleBecomeA::HideContextNode");
	ERROR2IF(m_pContextNode == NULL, FALSE, "Context node is NULL in HandleBecomeA::HideContextNode");
	ERROR2IF(m_pBecomeA->GetUndoOp() == NULL,FALSE,"GetUndoOp() returned NULL in HandleBecomeA::HideContextNode");

	UndoableOperation* pUndoOp = m_pBecomeA->GetUndoOp();

	return pUndoOp->DoHideNode(m_pContextNode, TRUE);


}


/***********************************************************************************************

>	BOOL HandleBrushBecomeA::HideCreatedByNode()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, etc.
	Purpose:	inserts an action to hide the context node

***********************************************************************************************/

BOOL HandleBrushBecomeA::HideCreatedByNode()
{
	ERROR2IF(m_pBecomeA == NULL, FALSE, "BecomeA is NULL in HandleBecomeA::HideContextNode");
	ERROR2IF(m_pCreatedByNode == NULL, FALSE, "Context node is NULL in HandleBecomeA::HideContextNode");
//	ERROR2IF(m_pBecomeA->GetUndoOp() == NULL,FALSE,"GetUndoOp() returned NULL in HandleBecomeA::HideContextNode");

	// set the context node selected
	if (m_pContextNode)
		m_pContextNode->SetSelected(TRUE);

	m_pContextNode = NULL; // so our destructor doesn't delete it

	UndoableOperation* pUndoOp = m_pBecomeA->GetUndoOp();

	if (!m_bSecondary)
	{
		if (pUndoOp)
			return pUndoOp->DoHideNode(m_pCreatedByNode, TRUE);

		m_pCreatedByNode->CascadeDelete();
		delete m_pCreatedByNode;
	}

	return(TRUE);
}



/***********************************************************************************************

>	BOOL HandleBrushBecomeA::FinishPassback()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, etc.
	Purpose:	Somewhat devious this, and to be used with care.  Basically after we have completed
				all our passbacks we have accumulated a nodegroup containiing all the brush objects
				converted to nodepaths.  This function indicates that we have finished and can then 
				passback this nodegroup to our becomeA object.
				This is a bit bad as we're never supposed to pass nodegroups back, so you must 
				make sure your becomeA object can handle it.
				It also nulls the group pointer so that it doesn't get deleted on destruction
***********************************************************************************************/

BOOL HandleBrushBecomeA::FinishPassback()
{
	ERROR2IF(m_pBecomeA == NULL, FALSE, "BecomeA is NULL in HandleBecomeA::FinishPassback");
	ERROR2IF(m_pContextNode == NULL, FALSE, "Context node is NULL in HandleBecomeA::FinishPassback");

	if (m_pBecomeA->GetReason() == BECOMEA_REPLACE)
	{
		// Select the newly created node
		if (m_pContextNode)
			m_pContextNode->SetSelected(TRUE);
	}

	if (m_pBecomeA->GetReason() == BECOMEA_REPLACE)
	{
		// Select the newly created node
		if (m_pContextNode)
			m_pContextNode->SetSelected(TRUE);
	}

	// We really shouldn't be passing this group back to anyone apart from our special becomeA
	if (!m_pBecomeA->IsBrushBecomeAGroup())
	{
		m_pContextNode = NULL;
		return TRUE;
	}

	BOOL ok = m_pBecomeA->PassBack((NodeRenderableInk*)m_pContextNode,(NodeRenderableInk*)m_pCreatedByNode,NULL);

	if (ok)
		m_pContextNode = NULL;

	return ok;
}



/*------------------------------------------------------------------------------------------------
--------------------------BrushBecomeA implementation---------------------------------------------
-------------------------------------------------------------------------------------------------*/


/***********************************************************************************************

>	BrushBecomeA::BrushBecomeA()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		Reason - our reason for being
				pClass - the class to generate
				pOp - the operation we are part of
				pBlendRef - the blendref that created us
	Outputs:	-
	Returns:	-
	Purpose:	constructor

***********************************************************************************************/

BrushBecomeA::BrushBecomeA(BecomeAReason Reason, CCRuntimeClass* pClass, UndoableOperation* pOp, 
						   BlendRef* pBlendRef) : BecomeA(Reason, pClass, pOp)
{
	m_pBlendRef = pBlendRef;
}


/***********************************************************************************************

>	bool BrushBecomeA::PassBack(NodeRenderableInk* pNewNode, NodeRenderableInk* pCreatedByNode, CCAttrMap* pAttrMap)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		pNewNode - the node that was generated
				pCreatedByNode - the node that created the node that was generated
				pAttrMap - the applied attributes
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	the money function, takes the nodepaths that are generated and makes blendpaths
				out of them.  Note that for this to work pNewNode MUST be a nodepath.

***********************************************************************************************/

BOOL BrushBecomeA::PassBack(NodeRenderableInk* pNewNode, NodeRenderableInk* pCreatedByNode, CCAttrMap* pAttrMap)
{
	ERROR2IF(pNewNode == NULL || pCreatedByNode == NULL, FALSE, "Input pointers are NULL in BrushBecomeA::PassBack");
	
	if (!pNewNode->IsNodePath())
	{
		ERROR3("New node is not NodePath in BrushBecomeA::PassBack");
		return FALSE;
	}

	Path* pInkPath = &(((NodePath*)pNewNode)->InkPath);

	//Node * pPathNode = (NodePath *)pNewNode;

	//INT32 NumCoords = pInkPath->GetNumCoords();
	//INT32 StartIndex=0,EndIndex=0;
	//UINT32 NumSubPaths=0;

	// Create a new NodePath for this subpath
	NodePath* pNewNodePath = new NodePath;
	BOOL ok = (pNewNodePath != NULL);
	if (ok)
	{
		// Copy the subpath in TempPath into the new NodePath's path object
		Path* pNewPath = &(pNewNodePath->InkPath);
		ok = pNewPath->Initialise(pInkPath->GetNumCoords());

		if (ok) ok = pNewPath->CopyPathDataFrom(pInkPath);

		if (ok)
		{
			// Create a new BlendPath obect
			BlendPath* pBlendPath = new BlendPath;
			ok = (pBlendPath != NULL);

			// Initialise the BlendPath object with the generated NodePath
			// and add the BlendPath to the BlendRef
			if (ok) ok = pBlendPath->Initialise(pNewNodePath, 0, pCreatedByNode, 1, 0, pAttrMap, FALSE);
			if (ok) ok = m_pBlendRef->AddBlendPath(pBlendPath);

		}
	}
	

	if (ok)
	{
		//pBlendRef->SetAWComplex(NumSubPaths > 1);	// This is the 'complex' flag used when exporting to AW EPS
		pNewNode->CascadeDelete();
		delete pNewNode;
		pNewNode = NULL;
	}
	
	return ok;
}



/*------------------------------------------------------------------------------------------------
--------------------------BrushBecomeA implementation---------------------------------------------
-------------------------------------------------------------------------------------------------*/


/***********************************************************************************************

>	SimpleBecomeA::SimpleBecomeA(BecomeAReason, CCRuntimeClass* pClass, UndoableOperation* pOp)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		Reason - our reason for being
				pClass - the class to generate
				pOp - the operation we are part of
	Outputs:	-
	Returns:	-
	Purpose:	constructor

***********************************************************************************************/

SimpleBecomeA::SimpleBecomeA(BecomeAReason Reason, CCRuntimeClass* pClass, UndoableOperation* pOp) 
							:BecomeA(Reason, pClass, pOp)
{
	m_pNodePath = NULL;
}


/***********************************************************************************************

>	BOOL SimpleBecomeA::PassBack(NodeRenderableInk* pNewNode, NodeRenderableInk* pCreatedByNode, CCAttrMap* pAttrMap)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		pNewNode - the node that was generated
				pCreatedByNode - the node that created the node that was generated
				pAttrMap - the applied attributes
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	the money function, takes the nodepath and assigns it to our member so it can 
				be retrieved using GetNodePath

***********************************************************************************************/

BOOL SimpleBecomeA::PassBack(NodeRenderableInk* pNewNode, NodeRenderableInk* pCreatedByNode, CCAttrMap* pAttrMap)
{
	ERROR2IF(pNewNode == NULL, FALSE, "Node is NULL in SimpleBecomeA::PassBack");
	ERROR2IF(!pNewNode->IsNodePath(), FALSE, "New node is not nodepath in SimpleBecomeA::PassBack");
	
	m_pNodePath = (NodePath*)pNewNode;

	return TRUE;
}



/***********************************************************************************************

>	NodePath* SimpleBecomeA::GetNodePath()
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		-
	Outputs:	-
	Returns:	the nodepath that we receieved from the passback
	Purpose:	as above
***********************************************************************************************/

NodePath* SimpleBecomeA::GetNodePath()
{
	return m_pNodePath;
}




/*------------------------------------------------------------------------------------------------
--------------------------BrushBecomeAGroup implementation---------------------------------------------
-------------------------------------------------------------------------------------------------*/


/***********************************************************************************************

>	BrushBecomeAGroup::BrushBecomeAGroup(BecomeAReason, CCRuntimeClass* pClass, UndoableOperation* pOp)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		Reason - our reason for being
				pClass - the class to generate
				pOp - the operation we are part of
	Outputs:	-
	Returns:	-
	Purpose:	constructor

***********************************************************************************************/

BrushBecomeAGroup::BrushBecomeAGroup(BecomeAReason Reason, CCRuntimeClass* pClass, UndoableOperation* pOp) 
							:BecomeA(Reason, pClass, pOp)
{
	m_pNodeGroup = NULL;
}



/***********************************************************************************************

>	NodeGroup* BrushBecomeAGroup::GetNodeGroup()
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		-
	Outputs:	-
	Returns:	the nodepath that we receieved from the passback
	Purpose:	as above, nulls the group so we don't delete it
***********************************************************************************************/

BrushBecomeAGroup::~BrushBecomeAGroup()
{
	if (m_pNodeGroup != NULL)
	{
		m_pNodeGroup->CascadeDelete();
		delete m_pNodeGroup;
	}
}

/***********************************************************************************************

>	BOOL BrushBecomeAGroup::PassBack(NodeRenderableInk* pNewNode, NodeRenderableInk* pCreatedByNode, CCAttrMap* pAttrMap)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		pNewNode - the node that was generated
				pCreatedByNode - the node that created the node that was generated
				pAttrMap - the applied attributes
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Curiously enough we're only interested in receiving nodegroups here

***********************************************************************************************/

BOOL BrushBecomeAGroup::PassBack(NodeRenderableInk* pNewNode, NodeRenderableInk* pCreatedByNode, 
								 CCAttrMap* pAttrMap)
{
	ERROR2IF(pNewNode == NULL || pCreatedByNode == NULL, FALSE, "Input pointers are NULL in BrushBecomeA::PassBack");
	
	// we don't care about the attr maps
	if (pAttrMap != NULL)
	{
		pAttrMap->DeleteAttributes();
		delete pAttrMap;
		pAttrMap = NULL;
	}

	if (pNewNode->IsAGroup())
	{
		if (m_pNodeGroup != NULL)
			ERROR3("Whats going on here? We've already got a nodegroup!");
		m_pNodeGroup = (NodeGroup*)pNewNode;
	}
	return TRUE;
}


/***********************************************************************************************

>	NodeGroup* BrushBecomeAGroup::GetNodeGroup()
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/99
	Inputs:		-
	Outputs:	-
	Returns:	the nodepath that we receieved from the passback
	Purpose:	as above, nulls the group so we don't delete it
***********************************************************************************************/

NodeGroup* BrushBecomeAGroup::GetNodeGroup()
{
	NodeGroup* Temp = m_pNodeGroup;
	m_pNodeGroup = NULL;
	return Temp;
}
