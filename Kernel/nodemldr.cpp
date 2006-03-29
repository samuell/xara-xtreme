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
// NodeMoulder implementation.

/*
*/

#include "camtypes.h"
#include "nodemldr.h"
//#include "mike.h"
#include "nodepath.h"
#include "nodebmp.h"
#include "document.h"
#include "moldshap.h"
#include "ndmldink.h"
#include "ndmldgrp.h"
#include "nodeattr.h"
//#include "nativeps.h"		// The old style EPS native filter, used in v1.1
//#include "cameleps.h"
#include "aw_eps.h"
//#include "oilprog.h"

#include "moldpers.h"	// For MouldPerspecive class definition
#include "fillattr.h" 	// For AttrFillGeometry::MakePerspective()

#include "extender.h"
//#include "ngcore.h"		// NameGallery, for stretching functionality

#include "strkattr.h"
#include "ophist.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNAMIC(NodeMoulder, NodeGroup )

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/***********************************************************************************************

>	NodeMoulder::NodeMoulder(Node* 	ContextNode,
						AttachNodeDirection Direction,
						const DocRect& 		BoundingRect,
						BOOL				Locked = FALSE,
						BOOL 				Mangled = FALSE,
						BOOL 				Marked = FALSE,
						BOOL 				Selected = FALSE,
						BOOL 				Renderable = FALSE
						)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
    
	Inputs:		ContextNode: Pointer to a node which this node is to be attached to.
				MonoOn Direction: MonoOff
				Specifies the direction in which the node is to be attached to the
				ContextNode. The values this variable can take are as follows:
								  
				PREV      : Attach node as a previous sibling of the context node
				NEXT      : Attach node as a next sibling of the context node
				FIRSTCHILD: Attach node as the first child of the context node
				LASTCHILD : Attach node as a last child of the context node

				BoundingRect: Bounding rectangle

				The remaining inputs specify the status of the node:
			
				Locked:     Is node locked ?
				Mangled:    Is node mangled ?
				Marked:     Is node marked ?
				Selected:   Is node selected ?

	Purpose:	This constructor initialises the nodes flags and links it to ContextNode in the
				direction specified by Direction. All necessary tree links are updated.

	Note:		Initialise() must be called before the NodeMoulder is in a state in which it can be used.
    SeeAlso: 	Initialise()   
   	Errors: 	An assertion error will occur if ContextNode is NULL

***********************************************************************************************/

NodeMoulder::NodeMoulder(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ):NodeGroup(ContextNode, Direction, Locked, Mangled, Marked, Selected )  
{                         
	pProviderNode = NULL;
}

/*********************************************************************************************

>	NodeMoulder::NodeMoulder() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Purpose:	This constructor creates a NodeMoulder linked to no other with all status
				flags false and an uninitialized bounding rectangle.           
 	Note:		Initialise() must be called before the NodeMoulder is in a state in which 
 				it can be used. 		
	SeeAlso:	Initialise()                                                        
 
**********************************************************************************************/
/* Technical Notes:   
	The default constructor is required so that SimpleCopy will work 
*/  

NodeMoulder::NodeMoulder(): NodeGroup()
{
	pProviderNode = NULL;
}


/*********************************************************************************************

>	NodeMoulder::~NodeMoulder() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Purpose:	Default destructor
 
**********************************************************************************************/

NodeMoulder::~NodeMoulder()
{
}


/*********************************************************************************************

>	BOOL NodeMoulder::Initialise(NodeRenderableInk* pContextNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pContextNode = pointer to node which will be moulded
	Returns:	TRUE if able to initialise the moulder object
				FALSE if not
	Purpose:	Initialise a moulder object with information necessary for it to perform
				a mould
 
**********************************************************************************************/

BOOL NodeMoulder::Initialise(NodeRenderableInk* pContextNode)
{
	ERROR2IF(pContextNode == NULL,FALSE,"pContext == NULL in NodeMoulder::Init");
	pProviderNode = pContextNode; 
	return (TRUE);
}


/********************************************************************************************

>	virtual String NodeMoulder::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		Plural: Flag indicating if the string description should be plural or
						singular. 
	Outputs:	-
	Retuns:		Description of the moulder node 
	Purpose:	To return a description of the Moulder object in either the singular or the 
				plural. This method is called by the DescribeRange method.
				
				The description will always begin with a lower case letter.   

********************************************************************************************/

String NodeMoulder::Describe(BOOL Plural, BOOL Verbose) 
{     
	if (Plural)
		return(String(_R(IDS_MOULDER_DESCRP)));  
	else
		return(String(_R(IDS_MOULDER_DESCRS))); 
}; 


/********************************************************************************************

>	NodePath* NodeMoulder::CreateMouldPath(	NodePath* pSourceNodePath,
											MouldGeometry* pGeometry
											UndoableOperation* pOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs		pSourceNodePath = pointer to a source node path object
				pGeometry		= pointer to a mould shape
				pOp				= pointer to an active operation or NULL if none
	Outputs:
	Returns:	a pointer to a node path if successfull.
				NULL if failed to create the object
	Purpose:	Creates a new Node path object containing a moulded version of the path held
				within the source node path and returns a pointer to it. This can then be
				inserted somewhere in the tree

********************************************************************************************/

NodePath* NodeMoulder::CreateMouldPath(	NodePath* pSourceNodePath, 
										MouldGeometry* pGeometry,
										UndoableOperation* pOp)
{							
	ERROR2IF(pSourceNodePath==NULL, NULL, "CreateMouldPath called with a null source pointer");
	ERROR2IF(pGeometry==NULL, NULL, "CreateMouldPath called with a null geometry pointer");

	BOOL ok;

	// ok we have the source node, so find the original path and mould it
	Path* pSourcePath = &(pSourceNodePath->InkPath);

	// create a node path receptor.
	NodePath* pDestinNodePath;
	ALLOC_WITH_FAIL( pDestinNodePath, new NodePath, pOp);
	if (!pDestinNodePath)
		return NULL;

	// allocate the node path some space
	CALL_WITH_FAIL((pDestinNodePath->SetUpPath(12,12)),pOp,ok);
	if (!ok)
	{
		delete pDestinNodePath;
		return NULL;
	}

	// create a destination path receptor for the mould
	Path* pDestinPath = &(pDestinNodePath->InkPath);
	CALL_WITH_FAIL((pGeometry->MouldPathToPath(pSourcePath, pDestinPath)),pOp,ok);
	if (!ok)
	{
		delete pDestinNodePath;
		return NULL;
	}

	// Set up the fill where necessary
	pDestinPath->IsFilled =	pSourcePath->IsFilled;

	return pDestinNodePath;
}



/********************************************************************************************

>	BOOL NodeMoulder::MouldPath(NodePath* pNodePath,
								MouldGeometry* pGeometry
								UndoableOperation* pOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs		pNodePath 	= pointer to a source node path object
				pGeometry	= pointer to a mould shape
				pOp			= pointer to an active operation or NULL if none
	Outputs:
	Returns:	TRUE if the path has been moulded correctly
				FALSE, then unable to mould the path, the originial is intact
	Purpose:	This function moulds the path contained within a NodPath to itself.
				It will use a temp path as a mould destination and copy this back over
				the source when successful.

********************************************************************************************/

BOOL NodeMoulder::MouldPath(NodePath* pNodePath, 
							MouldGeometry* pGeometry,
							UndoableOperation* pOp)
{							
	ERROR2IF(pNodePath==NULL, FALSE, "MouldPath called with a null source pointer");
	ERROR2IF(pGeometry==NULL, FALSE, "MouldPath called with a null geometry pointer");

	// ok we have the source node, so find the original path and mould it
	Path* pSourcePath = &(pNodePath->InkPath);

	// convert the source path into a destination path
	if (!pGeometry->MouldPathToPath(pSourcePath, pSourcePath))
		return FALSE;

	pNodePath->InvalidateBoundingRect();
	return TRUE;
}



/********************************************************************************************

>	NodeMouldBitmap* NodeMoulder::CreateMouldBitmap(NodeBitmap* pSourceNodeBlit, 
													MouldGeometry* pGeometry,
													UndoableOperation* pOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pSourceNodeBlit = pointer to a node bitmap
				pGeometry		= pointer to a mould shape to use for moulding
				pOp				= pointer to an active operation or NULL if none
	Purpose:	Creates a NodeMouldBitmap object which is used during rendering to
				render normal bitmaps through the mould.
	SeeAlso:	-
	Notes:

		We've found a source bitmap so we need to do something about moulding it.
		We either mould to a destination bitmap or create a NodeMouldBitmap node,
		which uses the bitmap and renders through the current mould. Eek

			Algorithm
			(1) Create NodeMouldBitmap node
			(2) Add the source node type to it
			(3) Add the source node object pointer to it
			(4) return ok;
	
		The idea here is hopefully simple. Currently bitmaps get rendered in
		Camelot as fill attributes. That means what really gets rendered is 
		a rectangular path, with a fill attribute set to the bitmap. We should
		attempt to simulate this but use one of our manifold paths instead. So
		when asked to render a NodeMouldBitmap object that we create here, the
		render function will simply take the original rectangle, mould it and
		render the result with a bitmap fill. I hope.
		Hmm, actually this may not be the best way. We need a MouldInk object
		but we cannot use a pointer to NodeBitmap due to the fact that we are
		in trouble if we copy the whole structure to another document.

********************************************************************************************/

NodeMouldBitmap* NodeMoulder::CreateMouldBitmap(NodeBitmap* pSourceNodeBlit, 
												MouldGeometry* pGeometry, 
												UndoableOperation* pOp)
{							
	ERROR2IF(pSourceNodeBlit==NULL, NULL, "CreateMouldBitmap() called with a null source pointer");
	ERROR2IF(pGeometry==NULL, NULL, "CreateMouldBitmap() called with a null geometry pointer");

	// ok we have the source node, so lets create a mould render object
	NodeMouldBitmap* pNodeMouldBitmap;
	ALLOC_WITH_FAIL( pNodeMouldBitmap, new NodeMouldBitmap, pOp);
	if (!pNodeMouldBitmap) 
		return NULL;

	return pNodeMouldBitmap;
}


/********************************************************************************************

>	BOOL NodeMoulder::DestroyMouldedObjects()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the moulder children were all destroyed successfully
				FALSE, if failed.
	Purpose:

********************************************************************************************/

BOOL NodeMoulder::DestroyMouldedObjects()
{
	// first need to add a record to the undo to rebuild the
	// moulded objects on a fail!
	Node* pChild = FindFirstChild();
	if (pChild!=NULL)
		DeleteChildren(pChild);

	return TRUE;
}



/********************************************************************************************

>	BOOL NodeMoulder::CreateMouldedObjects(	NodeMouldGroup* pGroup,
											MouldGeometry* pGeometry, 
											UndoableOperation* pOp) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pGroup		= pointer to a node mould group object
				pGeometry	= pointer to a node path shape to use for moulding
				pOp			= pointer to an active operation or NULL if none
	Outputs:
	Returns:	TRUE if all moulded objects have been created
				FALSE if unable to create some moulded objects
	Purpose:	Given a pointer to a moulder object, create all the moulded children
				using the source objects under the nodemouldgroup object

********************************************************************************************/

BOOL NodeMoulder::CreateMouldedObjects(NodeMouldGroup* pGroup, MouldGeometry* pGeometry, UndoableOperation* pOp)
{
	BOOL Success;

	if (pOp!=NULL)
		Success=CreateMouldObjsWithUndo(pGroup, pGeometry, pOp);
	else
		Success=CreateMouldObjsWithoutUndo(pGroup, pGeometry);

	return Success;
}


/********************************************************************************************

>	BOOL NodeMoulder::CreateMouldObjsWithUndo(	NodeMouldGroup* pGroup,
												MouldGeometry* pGeometry, 
												UndoableOperation* pOp) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pGroup		= pointer to a node mould group object
				pGeometry	= pointer to a node path shape to use for moulding
				pOp			= pointer to an active operation
	Outputs:
	Returns:	TRUE if all moulded objects have been created
				FALSE if unable to create some moulded objects
	Purpose:	Given a pointer to a moulder object, create all the moulded children
				using the source objects under the nodemouldgroup object

********************************************************************************************/

BOOL NodeMoulder::CreateMouldObjsWithUndo(NodeMouldGroup* pGroup, MouldGeometry* pGeometry, UndoableOperation* pOp)
{
	ERROR2IF(pGroup==NULL, FALSE, "CreateMouldedObjects() called with a null group pointer");
	ERROR2IF(pGeometry==NULL, FALSE, "CreateMouldedObjects() called with a null geometry pointer");

	Node *pNode, *qNode;

	// Stage 1
	// Copy all objects from the mould group to this new moulder object
	pNode = pGroup->FindFirstChild();
	while (pNode)
	{
		if (!IS_A(pNode,NodeHidden))
		{
			if (!pNode->CopyNode(this,LASTCHILD))
			{
				CascadeDelete();
				return FALSE;
			}
		}
		pNode=pNode->FindNext();
	}

	// Localise the current attributes. I do this because, although
	// most DoBecomeA() functions do their own localisation, bitmaps
	// see to apply AttrStrokeColour attributes to Pathified bitmaps
	// without localisation. This causes things like ConvertToShapes
	// produce MakeAttributeComplete found duplicate attr errors.
	if (!pOp->DoLocaliseCommonAttributes(this))
		return FALSE;

	// Stage 2
	// Get all objects to become paths or nodemouldbitmaps
	// Set up a BecomeA derived object, so that we can get everyone to become paths
	BecomeA ParamBecomeA(BECOMEA_REPLACE, CC_RUNTIME_CLASS(NodePath), pOp, FALSE);
	
	AttrStrokeType* pStroke = NULL;
	
	pNode = FindFirstChild();
	while (pNode)
	{
		qNode = pNode->FindNext();
		
		// bodge to deal with variable width strokes
		if (pNode->IsAnObject())
		{
			((NodeRenderableInk*)pNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeType), (NodeAttribute**)&pStroke);
			if (pStroke && pStroke->HasPathProcessor())
			{
				if (!pStroke->DoBecomeA(&ParamBecomeA, (NodeRenderableInk*)pNode))
					return FALSE;
			}
			else
				if (!pNode->DoBecomeA(&ParamBecomeA))
					return FALSE;

		}
		pStroke = NULL;
		pNode=qNode;
	}

	// As we've localised lets factor up. Make sure we do the factoring NONE globally.
	// We don't want to confuse any attributes.
	if (!pOp->DoFactorOutCommonChildAttributes(this))
		return FALSE;

	// Stage3
	// Process all objects, moulding each object as we go
	pNode = FindFirstChild();
    MouldTransform Transformer(pGeometry);
	if (!MouldAllChildren(pNode, pGeometry, Transformer, pOp))
		return FALSE;

	// Stage4
	// Deselect all the naughty selected objects inside ourselves
	pNode = FindFirstChild();
	while (pNode)
	{
		pNode->SetSelected(FALSE);
		pNode=pNode->FindNext();
	}

	// Stage5, make sure we know our bounds will be updated.
	InvalidateBoundingRect();
	return TRUE;
}



/********************************************************************************************

>	BOOL NodeMoulder::CreateMouldObjsWithoutUndo(	NodeMouldGroup* pGroup,
													MouldGeometry* pGeometry) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pGroup		= pointer to a node mould group object
				pGeometry	= pointer to a node path shape to use for moulding
	Outputs:
	Returns:	TRUE if all moulded objects have been created
				FALSE if unable to create some moulded objects
	Purpose:	Given a pointer to a moulder object, create all the moulded children
				using the source objects under the nodemouldgroup object

********************************************************************************************/

BOOL NodeMoulder::CreateMouldObjsWithoutUndo(NodeMouldGroup* pGroup, MouldGeometry* pGeometry)
{
	ERROR2IF(pGroup==NULL, FALSE, "CreateMouldedObjects() called with a null group pointer");
	ERROR2IF(pGeometry==NULL, FALSE, "CreateMouldedObjects() called with a null geometry pointer");

	Node *pNode;

	// Stage1
	// Copy the tree as shapes
	if (!pGroup->CopyChildrenAsShapes(this))
		return FALSE;

	// Stage2
	// Process all objects, moulding each object as we go
	pNode = FindFirstChild();
    MouldTransform Transformer(pGeometry);
	if (!MouldAllChildren(pNode, pGeometry, Transformer, NULL))
		return FALSE;

	// Stage 3
	// Factor out those attributes!
	if (!FactorOutCommonChildAttributes(FALSE,NULL))
		return FALSE;

	// Stage3
	// Deselect all the naughty selected objects inside ourselves
	pNode = FindFirstChild();
	while (pNode)
	{
		pNode->SetSelected(FALSE);
		pNode=pNode->FindNext();
	}

	// Stage4, make sure we know our bounds will be updated.
	InvalidateBoundingRect();
	return TRUE;
}



/********************************************************************************************

>	BOOL NodeMoulder::MouldAllChildren(	Node* pNode,
										MouldGeometry* pGeometry,
										TransformBase& Trans, 
										UndoableOperation* pOp) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/01/95
	Inputs:		pNode		= pointer to a node to start moulding from
				pGeometry	= pointer to a node path shape to use for moulding
				Trans		= reference to a mould transform object
				pOp			= pointer to an active operation or NULL if none
	Outputs:
	Returns:	TRUE if all objects have been moulded
				FALSE if unable to mould some objects
	Purpose:	Given a pointer to a node, begin moulding it, all its children and all
				its forward siblings and their children

********************************************************************************************/

BOOL NodeMoulder::MouldAllChildren(Node* pNode,
								   MouldGeometry* pGeometry,
								   TransformBase& Trans,
								   UndoableOperation* pOp)
{
	Node* qNode;

	while (pNode)
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodePath)))
		{
			// Ok, alter the path
			NodePath* pNodePath = (NodePath*)pNode;
			if (!MouldPath(pNodePath,pGeometry,pOp))
				return FALSE;
		}
		else
		{
			if (pNode->IsAnAttribute())
			{
				NodeAttribute* pAttr = (NodeAttribute*)pNode;

				if ( pAttr->IsAFillAttr() && IS_A(pGeometry,MouldPerspective) )
				{
					// Tell the fill attr to render perspectivised
					((AttrFillGeometry*)pAttr)->MakePerspective();
				}

				pAttr->Transform(Trans);
			}
		}

		qNode=pNode->FindFirstChild();
		pNode=pNode->FindNext();
		if (qNode)
		{	
			if (!MouldAllChildren(qNode, pGeometry, Trans, pOp))
				return FALSE;
		}
	}
	return TRUE;
}



/*
	if (pList !=NULL)
	{
		// we have a moulder list so now we can start creating moulded objects
		BOOL ok;

		MouldListObj* pMouldItem = pList->GetFirstMouldObj();
		NodeRenderableInk* pDestinNode;

		while (pMouldItem!=NULL)
		{
			switch (pMouldItem->GetMouldedType())
			{
				case MOULD_PATH:
				{
					// source object is a path, so we mould to a path
					NodePath* pSourceNodePath = (NodePath*)(pMouldItem->GetMouldedSource());
					pDestinNode = CreateMouldPath(pSourceNodePath, pGeometry, pOp);
					if (!pDestinNode) 
						return FALSE;

					if (pSourceNodePath->InkPath.IsStroked)
					{
						// This is nasty. We need to take the stroke applied to
						// this object and generate a path from it, which will
						// appear as another path in the tree. This will allow line
						// thicknesses etc to be transformed correctly. Can we stroke a
						// path to a buffer with a line attribute? Well currently I dont
						// really know how to do this so we'll leave it for now.

					}

				}
				break;

				case MOULD_BITMAP:
				{
					NodeBitmap* pSourceBitmap = (NodeBitmap*)(pMouldItem->GetMouldedSource());
					pDestinNode = CreateMouldBitmap(pSourceBitmap, pGeometry, pOp);
					if (!pDestinNode) 
						return FALSE; 
				}
				break;
			}

			// Now stick the new node inside us (ie the nodemoulder)
			CALL_WITH_FAIL(pOp->DoInsertNewNode(pDestinNode, this, LASTCHILD, FALSE, FALSE, FALSE, FALSE), pOp, ok);
			if (!ok)
			{
				delete pDestinNode;
				return FALSE;
			}

			// get the next mould item and mould it
			pMouldItem = pList->GetNextMouldObj(pMouldItem);
		}
	}
	return TRUE;
}

*/






 
/***********************************************************************************************

>	virtual Node* NodeMoulder::SimpleCopy()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Returns:	Pointer to a Node
	Purpose:	Makes a copy of all the data in the node

***********************************************************************************************/

Node* NodeMoulder::SimpleCopy()
{
	// Make a new NodeMoulder and then copy things into it
	NodeMoulder* pCopyOfNode = new NodeMoulder();
    if (pCopyOfNode)
    	CopyNodeContents(pCopyOfNode);
    
	return pCopyOfNode;
}            


/***********************************************************************************************

>	void NodeMoulder::CopyNodeContents(NodeMoulder* pCopyOfNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		pCopyOfNode - The node to copy data to
	Purpose:	Copies the data in this node to pCopyOfNode by first calling the 
				base class to get it to	copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeRenderableInk::CopyNodeContents

***********************************************************************************************/

void NodeMoulder::CopyNodeContents(NodeMoulder* pCopyOfNode)
{
	NodeRenderableInk::CopyNodeContents(pCopyOfNode);
	// Copy contents specific to derived class here
}


/***********************************************************************************************
>   void NodeMoulder::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeMoulder::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeMoulder), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeMoulder))
		CopyNodeContents((NodeMoulder*)pNodeCopy);
}



/***********************************************************************************************

>	void NodeMoulder::ShowDebugTreeDetails() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Purpose:	Displays debugging info of the tree
	SeeAlso:	NodeRenderableInk::ShowDebugTreeDetails

***********************************************************************************************/
#ifdef _DEBUG
void NodeMoulder::ShowDebugTreeDetails() const
{                     
	TRACE( _T("Moulder"));
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeRenderableInk::ShowDebugTreeDetails();	
}
#endif




/********************************************************************************************

>	void NodeMoulder::GetDebugDetails( StringBase* Str )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node

********************************************************************************************/

void NodeMoulder::GetDebugDetails( StringBase* Str )
{
#ifdef _DEBUG
	// Call base class
	NodeRenderableInk::GetDebugDetails( Str );
	
	String_256 TempStr;
		
	(*Str) += TEXT( "\r\nMoulder Data Dump\r\n" );

/*	TempStr._MakeMsg(	"Mould steps = #1%ld\n"
						"Index start = #2%ld\n"
						"Index end   = #3%ld\n\n"
						"",
						IndexStart,
						IndexEnd);

	*Str += TempStr;
*/
	DocRect BlobRect = GetBlobBoundingRect();
	TempStr._MakeMsg( TEXT("Blob Bounding Rect :-\r\n\t#1%ld,\t#2%ld\r\n\t#3%ld,\t#4%ld\r\n"),
					  BlobRect.lo.x, BlobRect.lo.y, BlobRect.hi.x, BlobRect.hi.y );
	(*Str) += TempStr;
#endif
}



/********************************************************************************************

>	virtual void NodeMoulder::Transform( TransformBase& Trans )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		Trans - The transform Object
	Purpose:	Transforms all the paths attached to this Moulder node
	SeeAlso:	NodeRenderableInk::Transform()

********************************************************************************************/

void NodeMoulder::Transform( TransformBase& Trans )
{
	// the moulder node bbox is now invalid
	InvalidateBoundingRect();
	// Transform all the children
	// See GroupCanTransformCached()
	NodeGroup::Transform(Trans);
}



/***********************************************************************************************

>	void NodeMoulder::Render(RenderRegion* pRender)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		Pointer to a render region
	Purpose:	Will render the  steps of this Moulder node

***********************************************************************************************/

void NodeMoulder::Render(RenderRegion* pRender)
{
//	pRender->SaveContext();
//	RenderMoulderedObjs(pRender);		// Only supply a render region, because we're only rendering
//	pRender->RestoreContext();
}  


/********************************************************************************************

>	DocRect NodeMoulder::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		DontUseAttrs - TRUE if we should ignore the nodes attributes.
				Defaults to FALSE
				HitTest      - TRUE if being called during HitTest
	Returns:	The nodes bounding rect
	Purpose:	if the bounding rect is valid it is returned, if not, it is recalculated
				and then returned.
	SeeAlso:	NodeMoulder::GetBlobBoundingRect

********************************************************************************************/

DocRect NodeMoulder::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	if (!IsBoundingRectValid || DontUseAttrs)
	{
		DocRect NewRect = GetChildBBox(DontUseAttrs);

		// just return this rectangle as it is not the nodes true bounding rect
		if (DontUseAttrs) 
			return NewRect;
			
		// copy the new docrect into the node bounding rect
		BoundingRectangle = NewRect;
		IsBoundingRectValid = TRUE;
	}

	// Return the resulting bounding rect
	return BoundingRectangle;
}



/********************************************************************************************

>	DocRect NodeMoulder::GetChildBBox(BOOL DontUseAttrs)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Inputs:		DontUseAttrs - TRUE if we should ignore the nodes attributes.
				Defaults to FALSE
	Returns:	The bounding rectangle of all the moulder children
	Purpose:	Calculates the bounding rectangle of the children of a moulder object.
				It does this by scanning the child objects and merging each bounded child
				bbox together

********************************************************************************************/

DocRect NodeMoulder::GetChildBBox(BOOL DontUseAttrs)
{
	// Find the moulders mangled object bounds
	DocRect Rect(0,0,0,0);
	Node* pNode = FindFirstChild();
	while (pNode!=NULL)
	{
		// if this node is bounded, union its bounds with the others
		if (pNode->IsBounded())
			Rect = Rect.Union(((NodeRenderableBounded*)pNode)->GetBoundingRect(DontUseAttrs));
		pNode = pNode->FindNext();
	}
	return Rect;
}




/********************************************************************************************

>	DocRect NodeMoulder::GetBlobBoundingRect()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Returns:	DocRect - Returns the bounding rect of the Moulder node and its blobs
	Purpose:	Scan through the Mouldered paths finding bounding boxes of Mouldered objects

********************************************************************************************/

DocRect NodeMoulder::GetBlobBoundingRect()
{
	return (GetBoundingRect());
}




/********************************************************************************************

>	virtual UINT32 NodeMoulder::GetNodeSize() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Returns:	The size of the node in bytes 
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodeMoulder::GetNodeSize() const 
{     
	return (sizeof(NodeMoulder)); 
}  



/********************************************************************************************

>	BOOL NodeMoulder::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		PointerPos 	- The Location of the mouse pointer at the time of the click
				Click 		- The type of click received (single, double, drag etc)
				ClickMods 	- The modifiers to the click (eg shift, control etc )
	Returns:	BOOL 		- TRUE if the node claims the click as its own and FALSE if it is
							  not interested in the click
	Purpose:	Allows the Node to respond to clicks by selecting its blobs or starting
				drags etc.

********************************************************************************************/

BOOL NodeMoulder::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	// we did not use the click, so let someone else try
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL NodeMoulder::CanBecomeA(BecomeA* pBecomeA) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/94
	Inputs:		InkClass: The class	of object
				pNumObjects = ptr to place number of objects of type pClass that will be created (Note: can be NULL).
							  *pNumObects in undefined on entry
	Returns:	TRUE if the node, or any of its children can transmogrify themselves to become 
				an InkClass object
	Purpose:	This function is used by the convert to shapes operation. It determines if 
				the node or any of its children can convert themselves into an InkClass object. 

				The number you put into pNumObjects (if it's not NULL) should exactly equal the total number
				of pClass objects you create.  It should NOT contain any additional objects you may produce
				such as group objects for containing the pClass object, or attributes.

				Also, the entry value of *pNumObjects cannot be assumed to be 0.

********************************************************************************************/

BOOL NodeMoulder::CanBecomeA(BecomeA* pBecomeA)
{
	// The NodeMoulder can become a NodePath
	if (pBecomeA->BAPath())
	{
		if (pBecomeA->IsCounting())
		{
			// Count the number of NodePaths that are children of this NodeMoulder
			Node* pNode = FindFirstChild();
			while (pNode != NULL)
			{
				pNode->CanBecomeA(pBecomeA);		// Increments count

				pNode = pNode->FindNext();
			}
		}

		return TRUE;
	}

	return FALSE;
}

		   
/********************************************************************************************

>	virtual BOOL NodeMoulder::DoBecomeA(BecomeA* pBecomeA)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		pBecomeA = 	ptr to a class that contains all the info needed to become a new
							type of node.
	Outputs:	-
	Returns:	TRUE if the object has been transformed, FALSE if we run out of memory

	Purpose:	Transforms the object into another type of object. 
				This converts all its children, and replaces itself in the tree with a NodeGroup
	Errors:		-
	SeeAlso:	Node::CanBecomeA

********************************************************************************************/

BOOL NodeMoulder::DoBecomeA(BecomeA* pBecomeA)
{
	// Check for a NULL entry param
	ERROR2IF_PF(pBecomeA == NULL,FALSE,("NodeMoulder::DoBecomeA() called with a NULL ptr"));

	switch (pBecomeA->GetReason())
	{
		case BECOMEA_REPLACE:
		{
			// If replacing the moulder with shapes, hide the moulder, create a group, and move all the children
			// so they become children of the new group node
			ERROR2IF(pBecomeA->GetUndoOp() == NULL,FALSE,"Trying to replace a NodeMoulder, but pUndoOp == NULL");
			UndoableOperation* pUndoOp = pBecomeA->GetUndoOp();
			BOOL ok;

			// If being repaced in the tree, deselect the node
			BOOL IsSelected=this->IsSelected();
			SetSelected(FALSE);

			// Pass on this message to our child ink nodes first
			Node* pNode = FindFirstChild();
			while (pNode != NULL)
			{
				Node* pThisNode = pNode;
				pNode = pNode->FindNext();
				pThisNode->DoBecomeA(pBecomeA);
			}

			// Allocate a new NodeGroup node
			NodeGroup* pNodeGroup;
			ALLOC_WITH_FAIL(pNodeGroup, (new NodeGroup), pUndoOp); 
			if (pNodeGroup == NULL)
				return FALSE;

			// Insert the NodeGroup where the NodeMoulder used to be
			if (!pUndoOp->DoInsertNewNode(pNodeGroup,this,NEXT,FALSE,FALSE,FALSE,FALSE))
				return FALSE;

			// if we were selected the lets select our group
			if (IsSelected)
			{
				// Select the group
				if (!pUndoOp->DoSelectNode(pNodeGroup,FALSE))
					return FALSE;
			}

			// localise common child attributes before moving objects!
			if (!pUndoOp->DoLocaliseCommonAttributes(this))
				return FALSE;

			// Now move each node in turn.
			pNode = FindLastChild();
			while (pNode != NULL)
			{                                                          
				// Find the next node to move before we shift up the children
				Node* pPrevNode = pNode->FindPrevious(); 
				if (pNode->IsAnObject())
				{
					CALL_WITH_FAIL(pUndoOp->DoMoveNode(pNode, pNodeGroup, FIRSTCHILD), pUndoOp, ok);
					if (!ok)
						return FALSE;
				}
				pNode = pPrevNode;  
			}

			// factor up on the group.
			if (!pUndoOp->DoFactorOutCommonChildAttributes(pNodeGroup))
				return FALSE;

			// Hide the moulder node
			NodeHidden* pNodeHidden; 
			if (!pUndoOp->DoHideNode(this, FALSE, &pNodeHidden, FALSE))
				return FALSE;                

			// and finally remove any nasty perspective fills from the result
			NodeMould::RemovePerspectiveFills(pNodeGroup, pUndoOp);
	
		}
		break;

		case BECOMEA_PASSBACK:
		{
			// We have been called to becomea passback on the moulder.
			// We cannot allow paths to be put in the tree with perspective fills
			// in them so we must hide the fills and replace them with standard ones
			// We should not do this on the clipboard however as undo/redo fails and
			// pasting clipboard contents wont work after!
			Document* pOwner = (Document*)FindOwnerDoc();
			if (pOwner == NULL || pOwner->IsNotAClipboard())
				NodeMould::RemovePerspectiveFills(this, pBecomeA->GetUndoOp());

			// Sequentially ask the children of the moulder to DoBecomeA
			// This is all that's required because the child objects are only passing back
			// the new node type, and NOT replacing themselves in the tree
			// This also ensures the receipient gets the list of paths in render order
			Node* pNode = FindFirstChild();
			while (pNode != NULL)
			{
				if (!pNode->DoBecomeA(pBecomeA))
					return FALSE;

				pNode = pNode->FindNext();
			}
		}
		break;

		default:
			ERROR3_PF(("Unknown BecomeA reason %d",pBecomeA->GetReason()));
			break;
	}


	// If this is a Perspective Mould, then we need to ensure
	// that any perspective fills are removed, as it is 
	// dangerous to allow the user to edit a shape containing
	// a perspectivised fill.
	return TRUE;
}







/***********************************************************************************************

> BOOL NodeMoulder::Snap(DocCoord* pDocCoord)

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	2/12/94
	Inputs:     pDocCoord	= a coord in Spread coords
    Outputs:    
    Returns:    TRUE 	- the DocCoord has been snapped .
    			FALSE	- the DocCoord has not been processed.
    			                                  		 
    Purpose:    Snaps to given coord to the nearest point on the Moulder node.
				Just returns FALSE currently
    Errors:        
	Scope:      public
	       
**********************************************************************************************/

//BOOL NodeMoulder::Snap(DocCoord* pDocCoord)
//{
//	return FALSE;
//}



/********************************************************************************************

>	BOOL NodeMoulder::NeedsToExport(RenderRegion* pRender, BOOL VisibleLayersOnly = FALSE,
									BOOL CheckSelected = FALSE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/95
	Inputs:		pRender - A pointer to the current export region
				VisibleLayersOnly - TRUE => remove nodes which are on invisible layers
								   - FALSE => export everything
				CheckSelected - TRUE => we check if object selected and only export selected bjects
							  - FALSE => we don't bother checking for selection or not
	Returns:	TRUE => please export me.
	Purpose:	Virtual function - this version will return FALSE for exporting to native
				EPS documents. We dont need the moulded objects in these docs.

********************************************************************************************/

BOOL NodeMoulder::NeedsToExport(RenderRegion* pRender, BOOL VisibleLayersOnly,
								BOOL CheckSelected)
{
#ifdef DO_EXPORT
	if (pRender==NULL)
		return TRUE;

	// dont export to native documents
	if (pRender->IS_KIND_OF(NativeRenderRegion))
		return FALSE;

	// If we have the check selection flag on then see if this moulder node is:-
	// - selected or not = render it
	// - a child of the selection e.g. part of selected group where we are an item in the
	// 	 group and hence are not directly selected but still need to be exported
	// - a parent of the selected item e.g. selected inside item of group and we are at the
	//   group and hence need to include the group in the range 
	// Otherwise just return True as this is a moulder node and always needs to be exported
	// unless of course some node overrides this or the test above fails.
	if (CheckSelected)
		return (IsSelected() || IsChildOfSelected() || IsParentOfSelected());
	else
		return TRUE;
#else
	return FALSE;
#endif
}


/*********************************************************************************************

>    void NodeMoulder::PreExportRender(RenderRegion* pRegion)

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	14/03/95
     Inputs:	pRegion = ptr to the export render region to export to
     Outputs:	
     Returns:	
     Purpose: 	Called before this node or any of its children have been rendered to the 
     			export region. This outputs the "start mould destin objects" command.
				Supports ArtWorks EPS and Camelot EPS
     Errors:    

**********************************************************************************************/
 
void NodeMoulder::PreExportRender(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
	if (pRegion->IS_KIND_OF(CamelotEPSRenderRegion))
	{
		PreExportCAMEPS(pRegion);
		return;
	} 

	if (pRegion->IS_KIND_OF(ArtWorksEPSRenderRegion))
	{
		PreExportAWEPS(pRegion);
		return;
	}
#endif
}

/*********************************************************************************************

>    BOOL NodeMoulder::ExportRender(RenderRegion* pRegion)

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	14/03/95
     Inputs:	pRegion = ptr to the export render region to export to
     Outputs:	
     Returns:	TRUE if ok, FALSE if something went wrong
     Purpose: 	Called after this node and all of its children have been rendered to the
     			export region. This outputs the "end mould destin objs" command.
				Supports ArtWorks EPS and Camelot EPS
     Errors:    

**********************************************************************************************/
 
BOOL NodeMoulder::ExportRender(RenderRegion* pRegion) 
{
#ifdef DO_EXPORT
	if (pRegion->IS_KIND_OF(CamelotEPSRenderRegion))
		return PostExportCAMEPS(pRegion);

	if (pRegion->IS_KIND_OF(ArtWorksEPSRenderRegion))
		return PostExportAWEPS(pRegion);
#endif	
	return FALSE;
}


/*********************************************************************************************

	BOOL NodeMoulder::Pre/Post ExportCAMEPS(RenderRegion* pRegion)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/95
	Purpose:	Export delimiter tokens for this particular object.
				The format is defined as
					csmp
						path description
					cemp

**********************************************************************************************/

void NodeMoulder::PreExportCAMEPS(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
	// Make sure this token goes out only in Native documents
	if (pRegion->IS_KIND_OF(NativeRenderRegion))
	{
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
		pDC->OutputToken("csdo");			// Camelot "start mould destin" token
		pDC->OutputNewLine();
	}
#endif
}


BOOL NodeMoulder::PostExportCAMEPS(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
	// Make sure this token goes out only in Native documents
	if (pRegion->IS_KIND_OF(NativeRenderRegion))
	{
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
		pDC->OutputToken("cedo");			// Camelot "end mould destin" token
		pDC->OutputNewLine();
		return TRUE;
	}
#endif
	// Otherwise render as paths (e.g. in EPS)
	return FALSE;
}


/*********************************************************************************************

	void NodeMoulder::Pre/Post ExportAWEPS(RenderRegion* pRegion)

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	14/03/95

**********************************************************************************************/

void NodeMoulder::PreExportAWEPS(RenderRegion* pRegion)
{
}

BOOL NodeMoulder::PostExportAWEPS(RenderRegion* pRegion)
{
	return FALSE;
}



/********************************************************************************************

>	virtual BOOL NodeMoulder::AllowOp(	ObjChangeParam* pParam,
										BOOL SetOpPermissionState = TRUE,
										DoPreTriggerEdit = TRUE)

 	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>; Karim 20/01/2000
	Created:	30/6/95
	Inputs:		pParam - pointer to the change parameter object
				SetOpPermissionState - TRUE to set the Nodes permission flags
				DoPreTriggerEdit	 - if TRUE then calls NameGallery::PreTriggerEdit.
									   *Must* be TRUE if the calling Op may make any nodes
									   change their bounds, eg move, line width, cut.
									   Use TRUE if unsure.

	Returns:	TRUE if the operation can proceede, FALSE to stop it
	Purpose:	Generic AllowOp() for all moulder nodes.

********************************************************************************************/

BOOL NodeMoulder::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState,
												  BOOL DoPreTriggerEdit)
{
	ERROR2IF(pParam==NULL,FALSE,"Node::AllowOp() - pParam==NULL");

	// if not called by a child AllowOp(), ensure AllowOp() called for all nodes in compound nodes,
	if (pParam->GetDirection()!=OBJCHANGE_CALLEDBYCHILD)
	{
		BOOL AnyAllowed=AllowOp_AccountForCompound(	pParam,
													SetOpPermissionState,
													DoPreTriggerEdit );
		// if called by a parent, just pass this result back
		if (pParam->GetDirection()==OBJCHANGE_CALLEDBYPARENT)
			return AnyAllowed;
	}
	else
	{
		// clean out the calling-child ptr, so it doesn't get passed around unintentionally.
		pParam->SetCallingChild(NULL);
	}

	// at this point we must have been called directly by the op or via a child AllowOp()

	// decide if we allow it, we dont allow very much at the moment, the only thing you can
	// actually do to children of a moulder is to copy them
	BOOL allowed=TRUE;
	ObjChangeFlags Flags=pParam->GetChangeFlags();
	if ((pParam->GetDirection()==OBJCHANGE_CALLEDBYCHILD) ||
		(pParam->GetDirection()==OBJCHANGE_CALLEDBYOP))
//		if (Flags.DeleteNode ||	Flags.ReplaceNode || Flags.MoveNode || Flags.Attribute || Flags.MultiReplaceNode || Flags.TransformNode)
		{
			pParam->SetReasonForDenial(_R(IDS_MOULD_CANT_OP_ON_CHILDREN));
			allowed=FALSE;
		}

	// if we allowed it, see if our parents do ...
	if (allowed && Parent!=NULL)
	{
		ObjChangeDirection OldDirection=pParam->GetDirection();
		pParam->SetCallingChild(this);
		pParam->SetDirection(OBJCHANGE_CALLEDBYCHILD);
		allowed=Parent->AllowOp(pParam,SetOpPermissionState,DoPreTriggerEdit);
		pParam->SetDirection(OldDirection);
	}

	// if setting permisions ...
	if (SetOpPermissionState)
	{
		// if allowed, mark parent accordingly, else mark child as denied and update parents
		if (allowed)
			Parent->SetOpPermission(PERMISSION_ALLOWED);
		else
			SetOpPermission(PERMISSION_DENIED,TRUE);
	}

	// if we're ok so far and were asked to do a PreTriggerEdit, then
	// determine whether the Op may change the bounds of some nodes.
	// If it may, then call NameGallery::PreTriggerEdit.
	if (allowed && DoPreTriggerEdit)
	{
		// if the Op is non-NULL then query its MayChangeNodeBounds() method.
		UndoableOperation* pChangeOp = pParam->GetOpPointer();
		if (pChangeOp != NULL && pChangeOp->MayChangeNodeBounds())
		{
PORTNOTE("other","Removed NameGallery usage")
#ifndef EXCLUDE_FROM_XARALX
			allowed = NameGallery::Instance()->PreTriggerEdit(pChangeOp, pParam, this);
#endif
		}
	}

	// return result (directly, or indirectly via a child AllowOp()) to op
	return allowed;
}


/********************************************************************************************

>	virtual ChangeCode NodeMoulder::OnChildChange(ObjChangeParam* pParam)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Inputs:		pParam	= pointer to a object change parameter class
	Returns:	CC_OK		if we have successfully processed the change.
				CC_FAIL		if we cannot handle this particular change and must prevent the
							child from continuing
	Purpose:	This function should be overridden in derived object classes.
				Composite objects can use this function to respond to one of their children
				undergoing a change. They should return CC_FAIL whenever they are unable to
				cope with the change.
	SeeAlso:	WarnParentOfChange();

********************************************************************************************/

ChangeCode NodeMoulder::OnChildChange(ObjChangeParam* pParam)
{
	return CC_OK;
}

/********************************************************************************************

>	virtual BOOL NodeMoulder::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/96
	Inputs:		pFilter = ptr to the filter to write to
	Returns:	TRUE is a record has been written, FALSE otherwise
	Purpose:	Writes out a records associated with this node

				NodeMoulders do NOT save anything out in the current version of Camelot.

********************************************************************************************/

BOOL NodeMoulder::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	return FALSE;
}

//------------------------------------------------------------------
// See comments for NodeMoulder::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
BOOL NodeMoulder::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	return WritePreChildrenWeb(pFilter);
}

/********************************************************************************************

>	virtual BOOL NodeMoulder::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/96
	Inputs:		pFilter = ptr to the filter to write to
	Returns:	TRUE if children can be written automatically, FALSE otherwise
	Purpose:	This allows a node to stop the filter from writing out the child nodes automatically.

				NodeMoulders do NOT save anything out in the current version of Camelot.
				Therefore, none of it's children can be saved out either.

********************************************************************************************/

BOOL NodeMoulder::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)
{
	return FALSE;
}

//------------------------------------------------------------------
// See comments for NodeMoulder::CanWriteChildrenNative(BaseCamelotFilter* pFilter)
BOOL NodeMoulder::CanWriteChildrenNative(BaseCamelotFilter* pFilter)
{
	return CanWriteChildrenWeb(pFilter);
}

