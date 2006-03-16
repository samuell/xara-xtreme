// $Id: nodemold.cpp 662 2006-03-14 21:31:49Z alex $
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
// NodeMould implementation. This file controls the tree node 'mould' which
// currently includes envelope and perspective mould shapes. Mould creation,
// rendering, deletion etc

// Comments
//
// A NodeMould contains all the usual information pertaining to tree nodes.
// Various virtual functions inherited from Node are used to render and
// format the node. The guts however, is the class MouldShape, which contains
// the actual shape 'vector space' into which objects, normally living in
// Euclidean 2d, are transformed. See the class MouldShape for more information
// about these shape geometries.

/*
*/


#include "camtypes.h"
#include "nodemold.h"
#include "nodemldr.h"
#include "ndmldgrp.h"
#include "ndmldpth.h"
//#include "mike.h"
#include "moldshap.h"
#include "moldenv.h"
#include "moldpers.h"
//#include "mario.h"
#include "document.h"
#include "ops.h"
#include "range.h"
#include "undoop.h"
#include "nodebmp.h"
#include "nodepath.h"
#include "trans2d.h"
#include "objchge.h"
#include "docview.h"
#include "spread.h"
#include "aw_eps.h"
//#include "nativeps.h"		// The old style EPS native filter, used in v1.1
//#include "moldtool.h"
#include "fillattr.h" 	// For AttrFillGeometry::RemovePerspective()
//#include "resource.h" 	// For _R(IDS_OK)/CANCEL
//#include "will.h"
//#include "tim.h"
//#include "contmenu.h"
#include "pathedit.h"
#include "moldedit.h"

#include "cxftags.h"
#include "cxfdefs.h"
#include "cxfrec.h"
#include "camfiltr.h"
#include "rechmld.h"
#include "app.h"
#include "blobs.h"		// for BlobManager.
#include "slicehelper.h"

#include "ophist.h"


DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNAMIC( NodeMould, NodeGroup )  
CC_IMPLEMENT_DYNCREATE( RecordChangeCodesAction, Action )
CC_IMPLEMENT_DYNAMIC(MouldRecordHandler, CamelotRecordHandler)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/*********************************************************************************************

>    NodeMould::NodeMould() 

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	5/12/94
     Purpose: 	This constructor creates a NodeMould linked to no other, with all status
		  	  	flags false and an uninitialised bounding rectangle.           

**********************************************************************************************/
 
NodeMould::NodeMould(): NodeGroup()
{
	NodeMould::InitialiseVars();
}


/***********************************************************************************************

>	void NodeMould::NodeMould
	(
		Node* ContextNode,  
		AttachNodeDirection Direction,  
		BOOL Locked = FALSE, 
		BOOL Mangled = FALSE,  
		BOOL Marked = FALSE, 
		BOOL Selected = FALSE, 
	)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	5/12/94
    Inputs: 	ContextNode: Pointer to a node which this node is to be attached to.     
    
				Direction: 
			
					Specifies the direction in which this node is to be attached to the 
					ContextNode. The values this variable can take are as follows: 
									  
					PREV      : Attach node as a previous sibling of the context node
					NEXT      : Attach node as a next sibling of the context node
					FIRSTCHILD: Attach node as the first child of the context node
					LASTCHILD : Attach node as a last child of the context node                               
					      
				The remaining inputs specify the status of the node: 
			
				Locked:     Is node locked ?
				Mangled:    Is node mangled ?
				Marked:     Is node marked ?
				Selected:   Is node selected ?
			
    Outputs: -
    Returns: - 
    Purpose: 	This method initialises the node and links it to ContextNode in the
			 	direction specified by Direction. All necessary tree links are
			 	updated.     
			 
    Errors:  	An assertion error will occur if ContextNode is NULL


***********************************************************************************************/

NodeMould::NodeMould(Node* ContextNode,  
					 AttachNodeDirection Direction,  
					 BOOL Locked, 
				 	 BOOL Mangled,  
					 BOOL Marked, 
					 BOOL Selected   
			   ):NodeGroup(ContextNode, Direction, Locked, Mangled, Marked, 
			 	Selected) 
{ 
	NodeMould::InitialiseVars();
} 

/*********************************************************************************************

>    NodeMould::~NodeMould() 

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	5/12/94
     Purpose: 	This destructor calls the parent class destructor and then checks whether
     			it is necessary to delete the mould geometry class. If so the shape geometry
				is deleted.

**********************************************************************************************/
 
NodeMould::~NodeMould()
{
	if (pMouldGeometry != NULL)
	{
		delete pMouldGeometry;
		pMouldGeometry = NULL;
	}
}




/********************************************************************************************

	void NodeMould::InitialiseVars()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/03/95
	Purpose:	Common place to set up the classes private variables

********************************************************************************************/

void NodeMould::InitialiseVars()
{
	pMouldGeometry=NULL;
	DetachedGeometry=FALSE;
	OnCC_CRC=0;
	OnCC_Width=0;
	OnCC_Height=0;
	RenderBlobs=TRUE;
}



/********************************************************************************************

>	BOOL NodeMould::IsANodeMould() const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/2000
	Purpose:	Virtual function to determine whether this node is a NodeMould.

********************************************************************************************/
BOOL NodeMould::IsANodeMould() const
{
	return TRUE;
}



/**********************************************************************************************

> BOOL NodeMould::CreateGeometry(MouldSpace mSpace)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Inputs:		mSpace	= the type of mould to create. Currently use Envelope or Perspective
						  enum values.
						  It can also be NULL if you wish to use the shape as is.
	Returns:	TRUE 	if the geometry has been created successfully.
				FALSE 	then unable to create the geometry object (out of memory)

	Purpose:	Changes the mould space used by this mould to that described by mSpace.
				Currently the defined spaces are
					MOULDSPACE_ENVELOPE
					MOULDSPACE_PERSPECTIVE

***********************************************************************************************/

BOOL NodeMould::CreateGeometry(MouldSpace mSpace)
{
	// now build the new vector space polymorphically
	MouldGeometry* pNewGeom=NULL;

	switch (mSpace)
	{
		case MOULDSPACE_ENVELOPE:
		{
			MouldEnvelope* mNewSpace = new MouldEnvelope;
			if (!mNewSpace)
				return FALSE;
			if (!mNewSpace->Initialise())
			{
				delete mNewSpace;
				return FALSE;
			}
			pNewGeom = mNewSpace;
			break;
		}

		case MOULDSPACE_ENVELOPE2X2:
		{
			MouldEnvelope2x2* mNewSpace = new MouldEnvelope2x2;
			if (!mNewSpace)
				return FALSE;
			if (!mNewSpace->Initialise())
			{
				delete mNewSpace;
				return FALSE;
			}
			pNewGeom = mNewSpace;
			break;
		}

		case MOULDSPACE_PERSPECTIVE:
		{
			MouldPerspective* mNewSpace = new MouldPerspective;
			if (!mNewSpace)
				return FALSE;
			pNewGeom = mNewSpace;
			break;
		}
		default: break;
	}

	if (!pNewGeom)
		return FALSE;

	// Assign the class geometry pointer the new shape
	// Note, we do not record the mouldspace in our class because
	// we dont really need to. The shape is an abstract class with
	// concrete geometries derived from it and hence we can call a shape
	// function and the run time binder will call the correct class func.

	pMouldGeometry = pNewGeom;
	return TRUE;
}



/********************************************************************************************

	BOOL NodeMould::SetGeometry(MouldGeometry* pGeometry)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/03/95
	Purpose:	Not to be used.

********************************************************************************************/

BOOL NodeMould::SetGeometry(MouldGeometry* pGeometry)
{
	ERROR2IF(pGeometry==NULL, FALSE, "Null pointer passed to NodeMould::SetGeometry()");
	pMouldGeometry=pGeometry;
	return TRUE;		
}


/********************************************************************************************

>	virtual String NodeMould::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		Plural: Flag indicating if the string description should be plural or
						singular. 
	Outputs:	-
	Retuns:		Description of the mould node 
	Purpose:	To return a description of the Mould object in either the singular or the 
				plural. This method is called by the DescribeRange method.
				
				The description will always begin with a lower case letter.   

********************************************************************************************/

String NodeMould::Describe(BOOL Plural, BOOL Verbose) 
{     
	if (Plural)
		return(String(_R(IDS_MOULD_DESCRP)));  
	else
		return(String(_R(IDS_MOULD_DESCRS))); 
}; 




/***********************************************************************************************

> Node* NodeMould::SimpleCopy()  

    Author:     Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	5/12/94
	Inputs:     -  
    Outputs:    -
    Returns:    A copy of the node, or NULL if memory has run out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	            
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL.                                                                      
                                                                                 
**********************************************************************************************/

Node* NodeMould::SimpleCopy()
{
	NodeMould* pCopyOfNode = new NodeMould();
	ERROR2IF(pCopyOfNode == NULL,NULL,_R(IDE_NOMORE_MEMORY)); 
	CopyNodeContents(pCopyOfNode);
	return (pCopyOfNode);
}   


/***********************************************************************************************

>	void NodeMould::CopyNodeContents(Node* pCopyOfNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		pCopyOfNode - The node to copy data to
	Outputs:	-
	Returns:	-
	Purpose:	Copies the data from this node to pCopyOfNode by first calling the base class 
				to get it to copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeGroup::CopyNodeContents

***********************************************************************************************/

void NodeMould::CopyNodeContents(NodeMould* pCopyOfNode)
{
	ERROR3IF(pCopyOfNode==NULL,"NodeMould::CopyNodeContents() was asked to copy into a NULL pointer");
	NodeGroup::CopyNodeContents(pCopyOfNode);

	// Make sure we copy all our personal data here
	pCopyOfNode->DetachedGeometry = DetachedGeometry;

	// Need to copy the shape and adjust our pointer.
	MouldGeometry* pNewGeom = (MouldGeometry*) pMouldGeometry->MakeCopy();

	// if the shape is NULL ie we couldn't build it what do we do?
	if (pNewGeom)
		pCopyOfNode->pMouldGeometry = pNewGeom;

}

   
/***********************************************************************************************
>   void NodeMould::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeMould::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeMould), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeMould))
		CopyNodeContents((NodeMould*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 NodeMould::GetNodeSize() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodeMould::GetNodeSize() const 
{     
	return (sizeof(NodeMould)); 
}  



/********************************************************************************************

>	virtual DocRect NodeMould::GetBlobBoundingRect()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/94
	Returns:	DocRect - Returns the bounding rect of the path and its blobs
	Purpose:	This calculates the bounding box of the mould and adds in the influence of
				the selection blobs. It does not consider if the blobs are visible or not,
				it just gives the bounding box that they would occupy if they were visible
				Note, the mould blob bounds are simply made up of the nodemouldpath bounds
				and thats yer lot.

********************************************************************************************/

DocRect NodeMould::GetBlobBoundingRect()
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect Rect;
	// Get our blob bounds
	if (pMouldGeometry)
		Rect = pMouldGeometry->GetBlobBoundingRect();

	// Note I Should really be using NodeRenderableBounded::IncludeChildrensBoundingRects()
	// But sadly I cannot, simply due to the fact that it doesn't work!!!
	// When given a null rectangle it gets totally confused. 

	// Make sure we include the Bounds of our children
	NodeMould::IncludeChildrensBoundingRects(Rect);

	// return the rectangle with the blobs included
	return Rect;
#else
	return DocRect(0,0,0,0);
#endif
}



/********************************************************************************************

>	DocRect NodeMould::GetRenderBounds()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/94
	Returns:	DocRect - the bounding rect of the mould
	Purpose:	Return the bounding rect of the mould. This rectangle includes only
				those items in the mould which are visible and hence will be invalidated
				by the OS. Items such as the NodeMouldGroup (which are of course invisible)
				do not get involved in the renderbounds

********************************************************************************************/

DocRect NodeMould::GetRenderBounds()
{
	DocRect Rect;
	if (pMouldGeometry)
		Rect = pMouldGeometry->GetBlobBoundingRect();

	Rect = Rect.Union(GetChildrensBounds());
	return Rect;
}


DocRect NodeMould::GetChildrensBounds()
{
	DocRect Rect;
	NodeMould::IncludeChildrensBoundingRects(Rect);
	return Rect;
/*	DocRect Rect,BlobRect;
	Node* pNode = FindFirstChild();
	while (pNode)
	{
		if (!(pNode->IsNodeHidden()))
		{
			if (pNode->IsNodeRenderableClass())
			{
				if (!(pNode->IsKindOf(CC_RUNTIME_CLASS(NodeMouldGroup))))
				{
					BlobRect=((NodeRenderable*)pNode)->GetBlobBoundingRect();
					Rect=Rect.Union(BlobRect);
				}
			}
		}
		pNode=pNode->FindNext();
	}
	return Rect;*/
}


/********************************************************************************************

>	void NodeMould::IncludeChildrensBoundingRects(DocRect& BoundingRect) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/95
	Outputs:	BoundingRect is updated to include the childrens bounding rects.
	Purpose:	The childrens Blob bounding rects are added to the BoundingRect.

********************************************************************************************/
     
void NodeMould::IncludeChildrensBoundingRects(DocRect& BoundingRect) 
{
	Node* pNode;
	DocRect BlobRect;

	// Start scanning the children
	pNode = this->FindFirstChild();
	while (pNode!=NULL)
	{
		if (!pNode->IsNodeHidden())
		{
			if (pNode->IsAnObject())
			{
				BlobRect=((NodeRenderable*)pNode)->GetBlobBoundingRect();
				BoundingRect=BoundingRect.Union(BlobRect);
			}
		}
		pNode=pNode->FindNext();
	}
}



/********************************************************************************************

>	BOOL NodeMould::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						     Spread* pSpread )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		PointerPos 	- The Location of the mouse pointer at the time of the click
				Click 		- The type of click received (single, double, drag etc)
				ClickMods 	- The modifiers to the click (eg shift, control etc )
	Returns:	TRUE 	- if the node claims the click as its own
				FALSE 	- if it is not interested in the click
	Purpose:	Allows the Node to respond to clicks by selecting its blobs or starting
				drags etc.
				Here we check for clicks over a moulds shape. If a blob has been clicked
				on we start a new drag operation to alter the shape of the mould.
		
********************************************************************************************/

BOOL NodeMould::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
#if !defined(EXCLUDE_FROM_RALPH)
	// we only handle the click if we can confirm that object blobs are being displayed.
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr == NULL)
		return FALSE;
	if (!pBlobMgr->GetCurrentInterest().Object)
		return FALSE;

	// pass the click on to the shape handler if there is one.
	if (pMouldGeometry)
	{
		if (pMouldGeometry->OnClick(PointerPos, Click, ClickMods, pSpread, this))
			return TRUE;
	}
	 
	// if none try the actual manifold shape itself.
	NodeMouldPath* pPathShape = GetPathShape();
	if (pPathShape)
	{
		if (pPathShape->OnClick(PointerPos, Click, ClickMods, pSpread))
			return TRUE;
	}
#endif
	return FALSE;
}


/***********************************************************************************************

>	BOOL NodeMould::OnMouseMove(DocCoord PointerPos, Spread* pSpread, ClickModifiers ClickMods)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		PointerPos 	- The Location of the mouse pointer
				Click 		- The type of click received (single, double, drag etc)
				ClickMods 	- The modifiers to the click (eg shift, control etc )
	Outputs:	-
	Returns:	TRUE 	- if the node has processed this event
				FALSE 	- if not
	Purpose:	A very unusual function to find in a node. This must exist so that the mould
				tool can ask the mould object what cursor to show as the mouse moves around.
				The function is called for each selected mould object when the mould tool is
				current.
				The nub of the problem is that a mould object can basically be made up of
				virtually anything. We actually need to ask the mould shape handler inside
				the mould object what type of cursor it wants to show and what sort of
				resouced message it would like to display. This cannot be determined from
				outside the object.

***********************************************************************************************/

BOOL NodeMould::OnMouseMove(DocCoord PointerPos, Spread* pSpread, ClickModifiers ClickMods, INT32* ctype, INT32* msgres)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// check for movement over our mould path end points and get
	// the caller to set the correct status bar and cursor stuff
	if (OverMouldEndPoint(PointerPos,pSpread))
	{
		*ctype=3;
		*msgres=3;
		return TRUE;
	}

	// pass the mouse move on to the shape handler if there is one.
	if (pMouldGeometry)
	{
		if (pMouldGeometry->OnMouseMove(PointerPos, pSpread, ClickMods, ctype, msgres))
			return TRUE;
	}

#endif
	// otherwise, nothing to do with us matey
	return FALSE;
}


/********************************************************************************************

>	void NodeMould::OverMouldEndPoint(DocCoord PointerPos, Spread* pSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/95
	Inputs:		PointerPos	= coordinate of mouse move,
				pSpread		= pointer to spread containing coord
	Outputs:	-
	Returns:	TRUE if the coord is over one of the path end points.
				FALSE if not
	Purpose:	This routine checks for the mouse being over end points of our mould path.

********************************************************************************************/

BOOL NodeMould::OverMouldEndPoint(DocCoord coord, Spread* pSpread)
{
	// find the actual moulding path object inside ourselves.
	Path* pPath = GetPath();
	if (pPath)
	{
		INT32 Closest=0;
		if (pPath->FindNearestPoint(coord, POINTFLAG_ENDPOINTS | POINTFLAG_CONTROLPOINTS, &Closest))
			return TRUE;
	}
	return FALSE;
}



/********************************************************************************************

>	virtual BOOL NodeMould::CanBecomeA(BecomeA* pBecomeA)

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

BOOL NodeMould::CanBecomeA(BecomeA* pBecomeA)
{
	// The NodeMoulder can become a NodePath
	if (pBecomeA->BAPath())
	{
		if (pBecomeA->IsCounting())
		{
			// We need to count how many paths it will create
			// We do this by adding the values received by each child NodeMoulder
			NodeMoulder* pNodeMoulder = FindFirstMoulder();
			while (pNodeMoulder != NULL)
			{
				pNodeMoulder->CanBecomeA(pBecomeA);				// Increments count

				pNodeMoulder = FindNextMoulder(pNodeMoulder);
			}
		}

		return TRUE;
	}

	return FALSE;
}



/********************************************************************************************

>	virtual BOOL NodeMould::DoBecomeA(BecomeA* pBecomeA)

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

BOOL NodeMould::DoBecomeA(BecomeA* pBecomeA)
{
//#pragma message( __LOCMSG__ "NodeMould::DoBecomeA - do nothing" )
//	TRACE( _T("Warning - NodeMould::DoBecomeA called\n") );
	BOOL ok;

	switch (pBecomeA->GetReason())
	{
		case BECOMEA_REPLACE:
		{
//			ERROR2IF(pBecomeA->GetUndoOp() == NULL,FALSE,"Trying to replace a NodeMould, but pUndoOp == NULL");
			UndoableOperation* pUndoOp = pBecomeA->GetUndoOp();

			// If replacing the mould with shapes, hide the mould, create a group, and move all the children
			// so they become children of the new group node

			NodeMoulder* pMoulder = FindFirstMoulder();
			if (pMoulder!=NULL)
			{
				BOOL IsSelected = this->IsSelected();
				SetSelected(FALSE);

				// Localise only the current children so undo factors out only these
				AttrTypeSet AttrTypes;
				if (!CreateAttrSetFromChildren(AttrTypes))
					return FALSE;

				// localise common child attributes before moving objects!
				if (pUndoOp)
				{
					if (!pUndoOp->DoLocaliseCommonAttributes(this, FALSE, FALSE, &AttrTypes))
						return FALSE;

					if (!pUndoOp->DoLocaliseCommonAttributes(pMoulder))
						return FALSE;
				}
				else
				{
					if (!LocaliseCommonAttributes(FALSE, FALSE, &AttrTypes))
						return FALSE;

					if (!pMoulder->LocaliseCommonAttributes())
						return FALSE;
				}

				// Pass on this message to our child ink nodes first
				Node* pNode = pMoulder->FindFirstChild();
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

				// Insert the NodeGroup next to the NodeMould 
				if (pUndoOp)
				{
					if (!pUndoOp->DoInsertNewNode(pNodeGroup,this,NEXT,FALSE,FALSE,FALSE,FALSE))
						return FALSE;

					// if we were selected the lets select our group
					if (IsSelected)
					{
						// Select the group
						if (!pUndoOp->DoSelectNode(pNodeGroup,FALSE))
							return FALSE;
					}
				}
				else
				{
					pNodeGroup->AttachNode(this,NEXT);
				}

				// Now move each node in turn.
				pNode = pMoulder->FindLastChild();
				while (pNode != NULL)
				{                                                          
					// Find the next node to move before we shift up the children
					Node* pPrevNode = pNode->FindPrevious(); 
					if (pNode->IsAnObject())
					{
						if (pUndoOp)
						{
							CALL_WITH_FAIL(pUndoOp->DoMoveNode(pNode, pNodeGroup, FIRSTCHILD), pUndoOp, ok);
							if (!ok)
								return FALSE;
						}
						else
						{
							pNode->MoveNode(pNodeGroup, FIRSTCHILD);
						}
					}
					pNode = pPrevNode;  
				}

				// Karim 15/05/2000 - this new loop copies non-optimising attrs applied to
				// the NodeMould object, into the new group. Note that although CopyNode()
				// is NOT undoable, things should be fine as the insertion of the group *was*.
				pNode = FindLastChild();
				while (pNode != NULL)
				{
					Node* pPrevNode = pNode->FindPrevious();
					if (	pNode->IsAnAttribute() &&
							!((NodeAttribute*)pNode)->ShouldBeOptimized() )
					{
						CALL_WITH_FAIL(pNode->CopyNode(pNodeGroup, FIRSTCHILD), pUndoOp, ok)
						if (!ok)
							return FALSE;
					}
					pNode = pPrevNode;  
				}

				// factor up on the group.
				if (pUndoOp)
				{
					if (!pUndoOp->DoFactorOutCommonChildAttributes(pNodeGroup))
						return FALSE;
				}
				else
				{
					if (!pNodeGroup->FactorOutCommonChildAttributes())
						return FALSE;
				}

				// Remove any nasty perspective fills from the result
				NodeMould::RemovePerspectiveFills(pNodeGroup, pUndoOp);

				// Hide the mould node
				if (pUndoOp)
				{
					if (!pUndoOp->DoHideNode(this, FALSE, NULL, FALSE))
						return FALSE;
				}
				else
				{
					CascadeDelete();
					delete this;
				}
			}
		}
		break;

		case BECOMEA_PASSBACK:
		{
			// Sequentially ask the children of the blend to DoBecomeA
			// This is all that's required because the child objects are only passing back
			// the new node type, and NOT replacing themselves in the tree
			// This also ensures the receipient gets the list of paths in render order
			Node* pNode = FindFirstChild();
			while (pNode != NULL)
			{
				Node* pThisNode = pNode;
				pNode = pNode->FindNext();
				if (IS_A(pThisNode,NodeMoulder))
				{
					if (!pThisNode->DoBecomeA(pBecomeA))
						return FALSE;
				}
			}
		}
		break;

		default:
			ERROR3_PF(("Unknown BecomeA reason %d",pBecomeA->GetReason()));
			break;
	}
	
	return TRUE;
}




/********************************************************************************************

>	void NodeMoulder::RemovePerspectiveFills(Node* pParetNode, UndoableOperation* pUndoOp) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/6/95
	Inputs:		pParentNode - a pointer to a node to start removing from
				pUndoOp		- a pointer to the currently active undo operation. can be NULL
	Returns:	-
	Purpose:	This function scans the children of a given parent for perspectivised 
				bitmap fills. It replaces these attributes with standard bitmap fill atts
				in order to avoid perspective bitmap fills from escaping during a make
				shapes

********************************************************************************************/

void NodeMould::RemovePerspectiveFills(Node* pParentNode, UndoableOperation* pUndoOp)
{
//	BOOL WarnedUser = FALSE;

	// Found the NodeMoulder, so scan it's children, looking for perspectivised Attributes
	Node* pMoulderChild = pParentNode->FindFirstChild();
	Node* pNext;

	while (pMoulderChild != NULL)
	{
		Node* pThisChild = pMoulderChild;
		pMoulderChild = pMoulderChild->FindNext();

		// Scan the nodes children for Perspective fills
		Node* pChild = pThisChild->FindFirstDepthFirst();
		while (pChild)
		{
			pNext = pChild->FindNextDepthFirst(pThisChild);

			if (pChild->IsAnAttribute() && ((NodeAttribute*)pChild)->IsAFillAttr())
			{
				if (((AttrFillGeometry*)pChild)->IsPerspective())
				{
					// Remove any perspectiven
					((AttrFillGeometry*)pChild)->RemovePerspective(pUndoOp);
				}
			}
			pChild=pNext;
		}
	}
}



/********************************************************************************************

>	BOOL NodeMould::CreateAttrSetFromChildren(AttrTypeSet& AttSet)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/95
	Inputs:		-
	Outputs:	AttSet 	  = a set of attributes which are children of the compound node
	Returns:	TRUE - if the set was built correctly
				FALSE - if not
	Purpose:	Builds a set of AttrTypeSet set of attributes from the children of a node

********************************************************************************************/

BOOL NodeMould::CreateAttrSetFromChildren(AttrTypeSet& AttSet)
{
	Node* qNode = FindFirstChild();
	while (qNode && qNode->IsAnAttribute())
	{
		if (!AttSet.AddToSet(((NodeAttribute*)qNode)->GetAttributeType()))
			return FALSE;
		qNode=qNode->FindNext();
	}
	return TRUE;
}




/********************************************************************************************

>	virtual void NodeMould::RenderObjectBlobs(RenderRegion* pRender)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/94
	Inputs:		pRender - The region to draw the blobs in
	Purpose:	This renders all the blobs associated with this mould node
				This node doesn't do any rendering itself. All it does is ask the shape
				object to render itself.
	
********************************************************************************************/

void NodeMould::RenderObjectBlobs(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// We need to ask the mould shape to render its edit blobs.
	if (!RenderBlobs)
		return;

	if (pMouldGeometry)
		pMouldGeometry->RenderControlBlobs(pRegion);

	// We also need to ask the mould manifold to render its blobs too.
	NodeMouldPath* pShape = GetPathShape();
	if (pShape)
		pShape->RenderObjectBlobs(pRegion);
#endif
}



/********************************************************************************************

>	void NodeMould::RenderTinyBlobs(RenderRegion* pRender)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/95
	Inputs:		pRender - The region to render the blobs into
	Purpose:	Draws the paths Tiny blob into the render region supplied

********************************************************************************************/

void NodeMould::RenderTinyBlobs(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	NodeMoulder* pMoulder = FindFirstMoulder();
	if (pMoulder!=NULL)
	{
		Node* pNode = pMoulder->FindLastChild();
		while (pNode != NULL && !pNode->IsAnObject())
		{
			// pNode was not a ink object, so get the next one
			pNode = pNode->FindPrevious();
		}

		if (pNode)
		{
			// Render the tiny blob of that object
			((NodeRenderableInk*)pNode)->RenderTinyBlobs(pRegion);
		}
	}
#endif
}



/********************************************************************************************

>	void NodeMould::RedrawMould()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		-
	Purpose:	Forces a redraw over the mould object
	
********************************************************************************************/

void NodeMould::RedrawMould()
{
	Spread* pSpread = FindParentSpread();

	if (pSpread != NULL)
	{
		BaseDocument* pDoc = pSpread->FindOwnerDoc();

		if ((pDoc != NULL) && (IS_A(pDoc, Document)))
		{
			DocRect Invalid = GetBlobBoundingRect();
			((Document *) pDoc)->ForceRedraw( pSpread, Invalid, FALSE, this);
		}
	}
}

/***********************************************************************************************

>	void NodeMould::ShowDebugTreeDetails() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/94
	Purpose:	Displays debugging info of the tree
	SeeAlso:	NodeRenderableInk::ShowDebugTreeDetails

***********************************************************************************************/

#ifdef _DEBUG
void NodeMould::ShowDebugTreeDetails() const
{                     
	TRACE( _T("Mould "));
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeRenderableInk::ShowDebugTreeDetails();	
}
#endif




/********************************************************************************************

>	void NodeMould::GetDebugDetails( StringBase* Str )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/94
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node

********************************************************************************************/

void NodeMould::GetDebugDetails( StringBase* Str )
{
#ifdef _DEBUG
	// Call base class
	NodeRenderableInk::GetDebugDetails( Str );
	
	String_256 TempStr;
		
	(*Str) += TEXT( "\r\nMould Data Dump\r\n" );

	DocRect BlobRect = GetBlobBoundingRect();
	TempStr._MakeMsg( TEXT("Blob Bounding Rect :-\r\n\t#1%ld,\t#2%ld\r\n\t#3%ld,\t#4%ld\r\n"),
					  BlobRect.lo.x, BlobRect.lo.y, BlobRect.hi.x, BlobRect.hi.y );
	(*Str) += TempStr;

	// call the mould shape debug function
	if (pMouldGeometry)
		pMouldGeometry->GetDebugDetails( Str );
#endif
}

/********************************************************************************************

>	virtual void NodeMould::Transform( TransformBase& Trans )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94
	Inputs:		Trans - A transformation object
	Purpose:	Transforms the mould object and all its children.

********************************************************************************************/

void NodeMould::Transform( TransformBase& Trans )
{
	// Transform all the children
	NodeRenderableInk::TransformChildren(Trans);

	// once transformed get the shape controller to transform its stuff
	if (pMouldGeometry)
	{
		Path* pPath = GetPath();
		NodeMouldGroup* pMouldGroup = FindMouldGroup();
		if (pPath && pMouldGroup)
		{
			DocRect Rect = pMouldGroup->GetChildrensBounds(TRUE);
			pMouldGeometry->Transform(pPath, &Rect, Trans);
		}
	}
}


/********************************************************************************************

>	MouldSpace NodeMould::Describe()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94
	Inputs:
	Returns:	The current mould space defined for this mould object. ie what type of
				mould geometry it uses, eg envelope, perspective etc.
	Purpose:	Find out what type of geometry is defined for this mould object.

********************************************************************************************/

MouldSpace NodeMould::DescribeGeometry()
{
	if (pMouldGeometry)
		return pMouldGeometry->Describe();

	return MOULDSPACE_UNDEFINED;
}



/********************************************************************************************

>	Path* NodeMould::GetPath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/01/95
	Inputs:		-
	Returns:	A pointer to the path object 
					inside the first node mould shape object 
						inside the NodeMould!
				NULL if unable to find the object
	Purpose:	A useful function to find the actual path used for moulding
				As this is used quite a lot its been 'function' alised. 

********************************************************************************************/

Path* NodeMould::GetPath()
{
	NodeMouldPath* pNodeMPath = GetPathShape();
	if (pNodeMPath)
		return (&(pNodeMPath->InkPath));
	return NULL;
}

/********************************************************************************************

>	NodeMouldPath* NodeMould::GetPathShape()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94
	Inputs:		-
	Returns:	A pointer to the first node mould path object inside the NodeMould
				NULL if unable to find the object
	Purpose:	A useful function to find the first node mould shape object within a NodeMould
				object. As this is used quite a lot its been 'function'alised. 

********************************************************************************************/

NodeMouldPath* NodeMould::GetPathShape()
{
	// try to find the first node moulder object
	Node* pNode = FindFirstChild();
	ERROR2IF(pNode == NULL,NULL, "No children found in MouldObject during NodeMould::GetPathShape()");

	// check for the right class
	if (pNode->GetRuntimeClass() != CC_RUNTIME_CLASS(NodeMouldPath))
		pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeMouldPath));

	// there's no geometry object inside this mould so we still can't do anything.
	ERROR2IF(pNode==NULL,NULL, "Can't find first NodeMouldPath as a child of NodeMould");

	return (NodeMouldPath*)pNode;
}



/********************************************************************************************

>	NodeMoulder* NodeMould::FindFirstMoulder(BOOL errorcheck=TRUE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94
	Inputs:
	Returns:	A pointer to the first node moulder object inside the NodeMould
				NULL if unable to find the object
	Purpose:	A useful function to find the first node moulder object within a NodeMould
				object. As this is used quite a lot its been 'function'alised. 

********************************************************************************************/

NodeMoulder* NodeMould::FindFirstMoulder(BOOL errorcheck)
{
	// try to find the first node moulder object
	Node* pNode = FindFirstChild();
	
	if (pNode==NULL)
	{
		if (errorcheck)
			ERROR2(NULL,"No children found in MouldObject during NodeMould::FindFirstMoulder()");
		return NULL;
	}

	// check for the right class
	if (pNode->GetRuntimeClass() != CC_RUNTIME_CLASS(NodeMoulder))
		pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeMoulder));

	// there's no moulder object inside this mould so we still can't do anything.
	if (pNode==NULL)
	{
		if (errorcheck)
			ERROR2(NULL,"Can't find first NodeMoulder as a child of NodeMould");
		return NULL;
	}

	// do a type conversion	for speed
	NodeMoulder* pMoulder = (NodeMoulder*)pNode;

	return pMoulder;
}

/********************************************************************************************

>	NodeMoulder* NodeMould::FindNextMoulder(NodeMoulder* pNodeMoulder)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/96
	Inputs:		pNodeMoulder = ptr to current node moulder
	Returns:	A pointer to the next node moulder object inside the NodeMould
				NULL if unable to find the object
	Purpose:	A useful function to find the next node moulder object within a NodeMould
				object.

********************************************************************************************/

NodeMoulder* NodeMould::FindNextMoulder(NodeMoulder* pNodeMoulder)
{
	ERROR2IF(pNodeMoulder == NULL,NULL,"pNodeMoulder == NULL");

	NodeMoulder* pNextNodeMoulder = (NodeMoulder*)pNodeMoulder->FindNext();

	while (pNextNodeMoulder != NULL && !IS_A(pNextNodeMoulder,NodeMoulder))
		pNextNodeMoulder = (NodeMoulder*)pNextNodeMoulder->FindNext();

	return pNextNodeMoulder;
}

/********************************************************************************************

>	INT32 NodeMould::CountMoulders()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/95
	Inputs:
	Returns:	The number of moulder objects within this mould
	Purpose:	Count the number of moulder objects which have been created within this
				mould. This does not include hidden moulder objects of course. 

********************************************************************************************/

INT32 NodeMould::CountMoulders()
{
	INT32 Count=0;
	Node* pNode = FindFirstChild();
	ERROR2IF(pNode == NULL,0, "No children found in MouldObject during NodeMould::CountMoulders()");
	while (pNode!=NULL)
	{
		// check for the right class
		if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeMoulder))
			Count+=1;

		pNode=pNode->FindNext();
	}
	return Count;
}


/********************************************************************************************

>	NodeMouldGroup* NodeMould::FindMouldGroup(BOOL errorcheck=TRUE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94
	Inputs:
	Returns:	A pointer to the first node mould group object inside the NodeMould
				NULL if unable to find the object
	Purpose:	A useful function to find the mould group node within a NodeMould
				object. As this is used quite a lot its been 'function'alised. 

********************************************************************************************/

NodeMouldGroup* NodeMould::FindMouldGroup(BOOL errorcheck)
{
	// try to find the first child node
	Node* pNode = FindFirstChild();

	if (pNode==NULL)
	{
		if (errorcheck)
			ERROR2(NULL,"No children found in MouldObject during NodeMould::FindMouldGroup()");
		return NULL;
	}

	// check for the right class
	if (pNode->GetRuntimeClass() != CC_RUNTIME_CLASS(NodeMouldGroup))
		pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeMouldGroup));

	// there's no moulder object inside this mould so we still can't do anything.
	
	if (pNode==NULL)
	{
		if (errorcheck)
			ERROR2(NULL,"Can't find first NodeMouldGroup as a child of NodeMould");
		return NULL;
	}

	// do a type conversion	for speed
	NodeMouldGroup* pMouldGrp = (NodeMouldGroup*)pNode;

	return pMouldGrp;
}


/********************************************************************************************

>	BOOL NodeMould::CreateAllMoulderObjects(UndoableOperation* pOp) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pOp = pointer to an operation (can be null)
	Returns:	TRUE if all moulder objects have rebuilt themselves
				FALSE if one failed.
	Purpose:	Scan through the children of a mould object, getting all NodeMoulders
				to regenerate their moulded objects.

********************************************************************************************/

BOOL NodeMould::CreateAllMoulderObjects(UndoableOperation* pOp)
{
	// Scan inside the mould object for the first moulder
	NodeMoulder* pMoulder = FindFirstMoulder();
	if (!pMoulder)
		// there's no moulder object inside this mould so we still can't do anything.
		return FALSE;

	// first find the mould group 
	NodeMouldGroup* pMouldGroup = FindMouldGroup();
	if (!pMouldGroup) 
		// there's no mould group inside this mould so we still can't do anything.
		return FALSE;

	while (pMoulder)
	{
		// create moulds for this moulder
		if (!pMoulder->CreateMouldedObjects(pMouldGroup, pMouldGeometry, pOp))
			return FALSE;
		// skip to the next moulder object
		pMoulder = (NodeMoulder*) pMoulder->FindNext(CC_RUNTIME_CLASS(NodeMoulder));
	}
	return TRUE;
}



/********************************************************************************************

>	NodeMoulder* NodeMould::CreateNewMoulder(UndoableOperation* pOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pOp = A pointer to an active operation (can be null)
	Returns:	a pointer to the moulder object (not in the tree).
				NULL if failed to create this moulder.
	Purpose:	Creates a NodeMoulder object and returns a pointer to it
	SeeAlso:	-

********************************************************************************************/

NodeMoulder* NodeMould::CreateNewMoulder(UndoableOperation* pOp)
{
//#pragma message( __LOCMSG__ "NodeMould::CreateNewMoulder - do nothing" )
//	TRACE( _T("Warning - NodeMould::CreateNewMoulder called\n") );
//	return NULL;
	BOOL ok;

	// first find the mould group 
	NodeMouldGroup* pMouldGroup = FindMouldGroup();
	if (!pMouldGroup) return NULL;

	// Now create a moulder object
	NodeMoulder* pMoulder;
	ALLOC_WITH_FAIL( pMoulder, new NodeMoulder, pOp);
	if (!pMoulder) return NULL;

	// get the moulder to initialise itself
	CALL_WITH_FAIL(pMoulder->Initialise(pMouldGroup), pOp, ok);
	if (!ok)
	{
		delete pMoulder;
		return NULL;
	}

	// its built so lets return it
	return pMoulder;
}


/********************************************************************************************

>	NodeMoulder* NodeMould::AddNewMoulder(	Node* pContextNode,
											AttachNodeDirection Direction,
 											UndoableOperation* pOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pContextNode =	The node to which NewNode is to be attached
				Direction	 =	The direction the new node should be attached to the
								context node. 
				pOp			 =	A pointer to an active operation (can be null)
	Returns:	a pointer to the moulder object (not in the tree).
				NULL if failed to create this moulder.
	Purpose:	Creates a NodeMoulder object and sticks it in the tree at the specified
				position
	SeeAlso:	-

********************************************************************************************/

NodeMoulder* NodeMould::AddNewMoulder(	Node* pContextNode,
										AttachNodeDirection Direction,
 										UndoableOperation* pOp)
{
//#pragma message( __LOCMSG__ "NodeMould::AddNewMoulder - do nothing" )
//	TRACE( _T("Warning - NodeMould::AddNewMoulder called\n") );
//	return NULL;
	BOOL ok;

	// create a new moulder object
	NodeMoulder* pMoulder = CreateNewMoulder(pOp);
	if (pMoulder==NULL)
		return NULL;

	if (pOp!=NULL)
	{
		// now insert it in the tree in an undoable way
		CALL_WITH_FAIL(pOp->DoInsertNewNode(pMoulder, pContextNode, Direction, FALSE, FALSE, FALSE, FALSE), pOp, ok);	
	}
	else
	{
		// all right then, insert it in a none undoable way
		pMoulder->AttachNode(pContextNode, Direction); 
		pMoulder->SetSelected(FALSE); 
		ok = TRUE;
	}

	// if failed get rid of what we have
	if (!ok)
	{
		delete pMoulder;
		return NULL;
	}

	return pMoulder;
}




/********************************************************************************************

>	NodeMouldPath* NodeMould::CreateNewMouldShape(Path* pShape, 
											  	  DocRect* const pDestin,
											  	  UndoableOperation* pOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pPath 	= pointer to a path to use as a shape
				pDestin = a rectangular area to place the shape over.
						  If this is NULL the path is used as is. 
				pOp		 = A pointer to an active operation (can be null)
	Returns:	a pointer to the created node mould path (not in the tree)
				NULL if unable to create the node
	Purpose:	Creates a node mould path object using the path pointer given and the 
				destination rectangle described. The objects pointer is returned
	SeeAlso:	-

********************************************************************************************/

NodeMouldPath* NodeMould::CreateNewMouldShape(	Path* pShape,
										 		DocRect* const pDestin,
										 		UndoableOperation* pOp)
{
//#pragma message( __LOCMSG__ "NodeMould::CreateNewMouldShape - do nothing" )
//	TRACE( _T("Warning - NodeMould::CreateNewMouldShape called\n") );
//	return NULL;
	ERROR2IF(pShape==NULL, NULL, "NodeMould::CreateNewMouldShape() called with a null path");

	// Note, we do not record the mouldspace in our class because
	// we dont really need to. The shape is an abstract class with
	// concrete geometries derived from it and hence we can call a shape
	// function and the run time binder will call the correct class func.

	BOOL ok;

	// build the new vector space polymorphically
	NodeMouldPath* pNodeMPath;
	ALLOC_WITH_FAIL( pNodeMPath, new NodeMouldPath, pOp);
	if (!pNodeMPath)
		return NULL;

	Path* pPath = (&(pNodeMPath->InkPath));

	CALL_WITH_FAIL(pPath->Initialise(pShape->GetNumCoords(), 12), pOp, ok);
	if (!ok)
	{
		delete pNodeMPath;
		return NULL;
	}

	CALL_WITH_FAIL(pPath->CopyPathDataFrom(pShape), pOp, ok);
	if (!ok)
	{
		delete pNodeMPath;
		return NULL;
	}

	// make sure our path has no fill bit
	pPath->IsFilled=FALSE;

	if (pDestin)
	{
		// we have been given a destination rectangle to map the path shape onto,
		// so first we need the source bbox
		DocRect Source = pPath->GetBoundingRect();

		// Build a transform matrix and apply it to this mould
		Matrix Mat(Source,*pDestin);
		Trans2DMatrix Trans(Mat);
		Trans.Transform( (DocCoord*)pPath->GetCoordArray(), pPath->GetNumCoords() );
	}

	return pNodeMPath;
}



/********************************************************************************************

>	NodeMouldPath* NodeMould::AddNewMouldShape(Path* pShape, 
										  	   DocRect* const pDestin,
											   UndoableOperation* pOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pPath 	= pointer to a path to use as a shape
				pDestin = a rectangular area to place the shape over.
						  If this is NULL the path is used as is. 
				pOp		= A pointer to an active operation (can be null)
	Returns:	a pointer to the created node mould path (inserted into the tree)
				NULL if unable to create the node
	Purpose:	Creates a node mould path object using the path pointer given and the 
				destination rectangle described. The object is inserted as the first child 
				of this	node mould object and its pointer is returned
	SeeAlso:	-

********************************************************************************************/

NodeMouldPath* NodeMould::AddNewMouldShape(	Path* pShape,
										 	DocRect* const pDestin,
										 	UndoableOperation* pOp)
{
	ERROR2IF(pShape==NULL, NULL, "NodeMould::AddNewMouldShape() called with a null path");

	NodeMouldPath* pNodeMPath = CreateNewMouldShape(pShape, pDestin, pOp);
	if (pNodeMPath==NULL)
		return NULL;

	NodeMouldGroup* pGroup = FindMouldGroup(FALSE);
	NodeMoulder* pMoulder = FindFirstMoulder(FALSE);

	Node* pContext = this;
	AttachNodeDirection Dir = LASTCHILD;

	if (pMoulder!=NULL)
	{
		pContext = pMoulder;
		Dir = PREV;
	}

	if (pGroup!=NULL)
	{
		pContext = pGroup;
		Dir = PREV;
	}

	BOOL ok = TRUE;

	if (pOp!=NULL)
		ok = pOp->DoInsertNewNode(pNodeMPath, pContext, Dir, FALSE, FALSE, FALSE, FALSE);
	else
	{
		pNodeMPath->AttachNode(pContext, Dir); 
		pNodeMPath->SetSelected(FALSE); 
	}
	
	if (!ok)
	{
		delete pNodeMPath;
		return NULL;
	}

	return pNodeMPath;
}
	

/********************************************************************************************

>	NodeMouldGroup* NodeMould::CreateNewMouldGroup(UndoableOperation* pOp) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pOp		 = A pointer to an active operation (can be null)
	Returns:	a pointer to the created mould group (not in the tree)
				NULL if unable to create the node
	Purpose:	Creates a MouldGroup object and returns a pointer to it
	SeeAlso:	-

********************************************************************************************/

NodeMouldGroup* NodeMould::CreateNewMouldGroup(UndoableOperation* pOp)
{
//#pragma message( __LOCMSG__ "NodeMould::CreateNewMouldGroup - do nothing" )
//	TRACE( _T("Warning - NodeMould::CreateNewMouldGroup called\n") );
//	return NULL;
	// Create a mould group object
	NodeMouldGroup*	pMouldGroup;
	ALLOC_WITH_FAIL( pMouldGroup, new NodeMouldGroup, pOp);
	return pMouldGroup;
}


/********************************************************************************************

>	NodeMouldGroup* NodeMould::AddNewMouldGroup(UndoableOperation* pOp) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pOp	= A pointer to an active operation (can be null)
	Returns:	a pointer to the created mould group (inserted into the tree)
				NULL if unable to create the node
	Purpose:	Creates a MouldGroup object inside the MouldParent and moves all the
				objects specified by NodeList into the group. Only object which can
				become either a path or a nodebitmap are actually moved.
	SeeAlso:	-

********************************************************************************************/

NodeMouldGroup* NodeMould::AddNewMouldGroup(UndoableOperation* pOp)
{
//#pragma message( __LOCMSG__ "NodeMould::AddNewMouldGroup - do nothing" )
//	TRACE( _T("Warning - NodeMould::AddNewMouldGroup called\n") );
//	return NULL;
	BOOL ok;

	// Create a mould group object
	NodeMouldGroup*	pMouldGroup = CreateNewMouldGroup(pOp);
	if (pMouldGroup==NULL)
		return NULL;

	if (pOp!=NULL)
	{
		// Now stick the mould group inside the mould parent, but dont alter the selection
		CALL_WITH_FAIL(pOp->DoInsertNewNode(pMouldGroup, this, LASTCHILD, FALSE, FALSE, FALSE, FALSE), pOp, ok);	
	}
	else
	{
		// all right then, insert it in a none undoable way
		pMouldGroup->AttachNode(this, LASTCHILD); 
		pMouldGroup->SetSelected(FALSE); 
		ok = TRUE;
	}

	// if failed get rid of what we have
	if (!ok)
	{
		delete pMouldGroup;
		return NULL;
	}

	return pMouldGroup;
}



/********************************************************************************************

>	BOOL NodeMould::FillMouldGroup(	NodeMouldGroup* pMouldGroup,
									List* NodeList,
									UndoableOperation* pOp) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		NodeList = A pointer to a list of nodes to move
				pOp		 = A pointer to an active operation
	Returns:	TRUE if filled the group
				NULL if unable transfer all necessary nodes
	Purpose:	Moves all the objects specified by NodeList into the mould group.
				Only objects which can become either a path or a nodebitmap are actually
				moved.
	SeeAlso:	-

********************************************************************************************/

BOOL NodeMould::FillMouldGroup(	NodeMouldGroup* pMouldGroup, 
								List* NodeList,
								UndoableOperation* pOp)
{
//#pragma message( __LOCMSG__ "NodeMould::FillMouldGroup - do nothing" )
//	TRACE( _T("Warning - NodeMould::FillMouldGroup called\n") );
//	return false;
	BOOL ok;
	Node* pObj;

	// find the parent spread object
	Spread*	pParentSpread=FindParentSpread();
	if (pParentSpread==NULL)
		return FALSE;

	Node* pDNode = (Node*)pMouldGroup;
	INT32 count = 0;

	BecomeA PathTester(BECOMEA_TEST, CC_RUNTIME_CLASS(NodePath));
	BecomeA BitmapTester(BECOMEA_TEST, CC_RUNTIME_CLASS(NodeBitmap));

	NodeListItem* CurItem = (NodeListItem*)(NodeList->GetHead());
	while (CurItem)
	{
		pObj = CurItem->pNode;

		if (
		    (pObj->CanBecomeA(&PathTester)) ||
			(pObj->CanBecomeA(&BitmapTester))
		   )
		{
			// Deselect the node first
			if (pObj->IsAnObject())
			{
				CALL_WITH_FAIL(pOp->DoDeselectNode((NodeRenderableInk*)pObj,pParentSpread), pOp, ok);
				if (!ok)
					return FALSE;
			}
			CALL_WITH_FAIL(pOp->DoMoveNode(pObj, pDNode, LASTCHILD), pOp, ok);
			if (!ok)
				return FALSE;

			count+=1;
		}
		CurItem = (NodeListItem*)(NodeList->GetNext(CurItem));
	}

	if (!pOp->DoFactorOutCommonChildAttributes(pMouldGroup))
		return FALSE;

	// remove the template attribute "names" from the MouldGroup
	// as if they are left in this the node group can be selected and
	// it should never be selected in the tree.
	// this should be recreated when the mould is removed - which it isn't currently
	Node * pNode = SliceHelper::FindNextNameNode(pMouldGroup, pMouldGroup);
	while (pNode)
	{
		pOp->DoHideNode(pNode, FALSE);
		pNode = SliceHelper::FindNextNameNode(pMouldGroup, pMouldGroup);
	}

	// if there's no objects at all fail
	return (count>0);
}



/*********************************************************************************************

>    Node* NodeMould::HasEditableChild(CCRuntimeClass* ChildClass, Node* pPreviousChild)

     Author:    Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:   20/3/95
     Inputs:    ChildClass		= the runtime class of the editable object
	 			pPreviousChild	= a pointer to the previous editable child 
	 							  returned by 'this' node, NULL if this is the first
								  call to this node.
     Outputs:   -
     Returns:   A node pointer, to an object which forms part of the editable surface of
	 			its parent (this node).
     Purpose:   This function returns our edit node (the nodemouldpath) which forms part of
     			the mould objects editable surface. 

**********************************************************************************************/

Node* NodeMould::HasEditableChild(CCRuntimeClass* ChildClass, Node* pPreviousChild)
{
	if (ChildClass != CC_RUNTIME_CLASS(NodePath))
		return NULL;

	NodeMouldPath* pNodeMPath = GetPathShape();
	// check to see if this has already been asked for once
	if (((Node*)pNodeMPath)==pPreviousChild)
		return NULL;

	return GetPathShape();
}


/*********************************************************************************************

>    void NodeMould:ToggleDetachFlag()

     Author:    Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:   20/3/95
     Inputs:    -
     Returns:	-
	 Purpose:	Switch the logical value of the detached mould flag inside this mould object

**********************************************************************************************/

void NodeMould::ToggleDetachFlag()
{
	DetachedGeometry = !DetachedGeometry;
}





/********************************************************************************************

>	virtual ChangeCode NodeMould::OnChildChange(ObjChangeParam* pParam)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Inputs:		pParam	= pointer to a object change parameter class
	Returns:	CC_OK	if we have successfully processed the change.
				CC_FAIL	if we cannot handle this particular change and must prevent the
						child from continuing
	Purpose:	An object has or is currently being edited inside our mould so we need to
				perform actions related to this change. The parameter block passed to us will
				define what to do (if we recognise it). Otherwise we will simply perform the
				default action of remoulding ourselves on receiving a HasChanged reason.

********************************************************************************************/

ChangeCode NodeMould::OnChildChange(ObjChangeParam* pParam)
{
#if defined(EXCLUDE_FROM_RALPH)
	ERROR3("NodeMould::OnChildChange being called");
	return (ChangeCode)CC_OK;
#else
	ERROR2IF(pParam==NULL,CC_FAIL,"NodeMould::OnChildChange() called with a null parameter");

	// check for a specific derived change type
 	if (pParam->IS_KIND_OF(ObjChangePathEdit))
	{
		// see if its our shape path thats being altered
		Node* pChangedObj = pParam->GetChangeObj();
		NodeMouldPath* pShape = GetPathShape();
		if (pChangedObj==pShape)
			return HandleMouldShapeChange((ObjChangePathEdit*)pParam);
	}

	// now read the parameter block types
	ObjChangeType pType = pParam->GetChangeType();
	ObjChangeFlags cFlags = pParam->GetChangeFlags();
	ChangeCode Chge = CC_OK;

	// otherwise its not our mould shape thats changed so
	// simply perform a remould
	if (pType==OBJCHANGE_FINISHED)
	{
		// if this change is a copy the do nothing.
		if (!cFlags.CopyNode)
		{
			// Node* pChangedObj = pParam->GetChangeObj();
			NodeMouldPath* pShape = GetPathShape();
			NodeMouldGroup* pNodeMGroup = FindMouldGroup();
			UndoableOperation* UndoOp = pParam->GetOpPointer();
	
			if ((pNodeMGroup!=NULL) && (pShape!=NULL) && (UndoOp!=NULL))
			{
				// Has the mould path changed shape?
				INT32 newCRC = pShape->InkPath.CalcCRC();
		
				// Have the bounds of the source objects changed?
				DocRect Rect = pNodeMGroup->GetChildrensBounds(FALSE);
				INT32 rw = Rect.Width();
				INT32 rh = Rect.Height();

				BOOL ch = ((newCRC!=OnCC_CRC) || (rw!=OnCC_Width) || (rh!=OnCC_Height));

				if (ch)
				{
					if (newCRC!=OnCC_CRC)
					{
						// The mould path has changed so we better check it
						UINT32 errID;
						if (!pMouldGeometry->Validate( &(pShape->InkPath), errID ))
							return CC_FAIL;
					}

					Chge=RecordChangeCodes(UndoOp);
					if (Chge==CC_OK)
					{
						// Set the new CRC.
						OnCC_CRC=newCRC;
						OnCC_Width=rw;
						OnCC_Height=rh;

						Chge = StartSaveContext(UndoOp,REC_REBUILD | REC_GEOMCONTEXT);
						if (Chge==CC_OK)
						{
							pMouldGeometry->Define(&(pShape->InkPath),&Rect);
							Chge = RemouldAll(UndoOp);
							if (Chge==CC_OK)
								Chge = EndSaveContext(UndoOp,1+4);
						}
					}
				}
			}
		}
	}

	return Chge;	
#endif
}


/********************************************************************************************

>	ChangeCode NodeMould::StartSaveContext(UndoableOperation* pUndoOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Inputs:		pUndoOp	= pointer to an active undoable operation
	Returns:	CC_OK	if we have successfully saved the mould context
				CC_FAIL	if we have failed to save the context. We've possible run out of memory
	Purpose:	Tries to save the current state of the mould shape on the undo. If you want
				to edit the mould in any way you should use this function to record various
				parts of the mould before going ahead and changing any data. The corresponding
				EndSaveContext should be called after you have altered any data

********************************************************************************************/

ChangeCode NodeMould::StartSaveContext(UndoableOperation* pUndoOp, REC_TYPE rectype)
{
	ChangeCode Chge=CC_OK;

#if !defined(EXCLUDE_FROM_RALPH)
	if (pUndoOp!=NULL)
	{
		// create an action to rebuild the mould
//		if ((rectype & REC_REBUILD) && (Chge==CC_OK))
//		{
//			Chge = ConvertCode(StartRebuildMouldAction::DoRecord(pUndoOp, this));
//		}

		// record the current path data, ie its flags,verbs & coords
		if ((rectype & REC_PATHARRAYS) && (Chge==CC_OK))
		{
			NodeMouldPath* pMouldPath=GetPathShape();
			if (pMouldPath)
			{
				Path* pEditPath = &(pMouldPath->InkPath);
				Chge = ConvertCode(SavePathArraysAction::DoRecord(pUndoOp, pUndoOp->GetUndoActionList(), pEditPath));
			}
		}
		
		// record the current blob positions
		if ((rectype & REC_BLOBS) && (Chge==CC_OK))	
		{
			Spread* pSpread = FindParentSpread();
			if (pMouldGeometry!=NULL)
				Chge=pMouldGeometry->RecordBlobs(pUndoOp, pSpread);
		}

		if ((rectype & REC_GEOMETRY) && (Chge==CC_OK))
		{
			// record the entire geometry object on the undo
			Chge = ConvertCode(RecordGeometryAction::DoRecord(pUndoOp,this));
		}

		// record the mould geometry context on the undo.
		if ((rectype & REC_GEOMCONTEXT) && (Chge==CC_OK))
		{
			if (pMouldGeometry!=NULL)
				Chge=pMouldGeometry->RecordContext(pUndoOp);
		}
	}
#endif
	return Chge;
}


/********************************************************************************************

>	ChangeCode NodeMould::EndSaveContext(UndoableOperation* pUndoOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Inputs:		pUndoOp	= pointer to an active undoable operation
						  NULL if none
	Returns:	CC_OK	if we have successfully saved the mould context
				CC_FAIL	if we have failed to save the context. We've possible run out of memory
	Purpose:	Tries to save the current state of the mould shape on the undo.

********************************************************************************************/

ChangeCode NodeMould::EndSaveContext(UndoableOperation* pUndoOp, REC_TYPE rectype)
{
	ChangeCode Chge=CC_OK;
#if !defined(EXCLUDE_FROM_RALPH)

	if (pUndoOp!=NULL)
	{
		if (NodeMould::pMouldGeometry)
		{
//			if (rectype & REC_REBUILD)
//			{
//				Chge = ConvertCode(EndRebuildMouldAction::DoRecord(pUndoOp, this));
//			}

			// no revese actions are required for geometry recording or blob recording
		}
	}	
#endif
	return Chge;
}




/***********************************************************************************************

>	virtual ChangeCode NodeMould::RecordChangeCodes(UndoableOperation* pOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		pOp	= pointer to a running undoable operation
	Outputs:	-
	Purpose:	Record the CRC codes held within the mould object

***********************************************************************************************/

ChangeCode NodeMould::RecordChangeCodes(UndoableOperation* pOp)
{
	if (pOp!=NULL)
	{
		RecordChangeCodesAction* CCodeAction;
		ActionCode Act;

		// call the actions static init function to get the action going.
		Act = RecordChangeCodesAction::Init(pOp, pOp->GetUndoActionList(), this, (Action**)(&CCodeAction));

		if (Act == AC_FAIL)
			return CC_FAIL;
		if (Act == AC_NORECORD)
			return CC_NORECORD;
	}
	return CC_OK;
}


/***********************************************************************************************

>	ChangeCode NodeMould::ConvertCode(ActionCode Act)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		Action code
	Returns:	Change code
	Purpose:	Convert a action code to a change code.

***********************************************************************************************/

ChangeCode NodeMould::ConvertCode(ActionCode Act)
{
	switch (Act)
	{
		case AC_FAIL: 		return CC_FAIL;
		case AC_NORECORD:	return CC_NORECORD;
		case AC_OK: 		return CC_OK;
	}
	return CC_OK;
}



/********************************************************************************************

>	ChangeCode NodeMould::HandleMouldShapeChange(ObjChangePathEdit* pParam)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Inputs:		pParam	= pointer to a object change parameter class
	Returns:	CC_OK	if we have successfully processed the change.
				CC_FAIL	if we cannot handle this particular change and must prevent the
						child from continuing
	Purpose:	The user is
					about to edit,
					in the action of editing,
					or has edited, 
				our mould shape. 
				We as a parent need to sort out whether they are allowed to continue.
				If so we need to perform various actions dependent on what stage the
				change is in.
				AboutToEdit: we need to decide whether there is a good reason to prevent
							 the edit from taking place
				Editing:	 we need to eor our edit blobs on when necessary
				HasEdited:	 we need to perform a remould of all children.

********************************************************************************************/

ChangeCode NodeMould::HandleMouldShapeChange(ObjChangePathEdit* pParam)
{
#if defined(EXCLUDE_FROM_RALPH)
	ERROR3("NodeMould::HandleMouldShapeChange called");
	return (ChangeCode)CC_OK;
#else
	ObjChangeType pType = pParam->GetChangeType();
	ObjChangeFlags pFlags = pParam->GetChangeFlags();
	ChangeCode Chge = CC_OK;
	BOOL ok;

	if (pMouldGeometry)
	{
		switch (pType)
		{
			case OBJCHANGE_STARTING:
				Chge=StartSaveContext(pParam->GetOpPointer(), REC_REBUILD | REC_BLOBS | REC_GEOMCONTEXT);
				pParam->ChangeMask.ClaimAll();
				pMouldGeometry->DisableControlBlobs();
				break;

			case OBJCHANGE_RENDERCURRENTBLOBS:
				pMouldGeometry->RenderDragBlobs(pParam->pChangeSpread);
				break;

			case OBJCHANGE_RENDERCHANGEDBLOBS:
				ok=pMouldGeometry->Define(pParam->pChangePath,NULL);
				pMouldGeometry->RenderDragBlobs(pParam->pChangeSpread);
				if (!ok)
					Chge=CC_FAIL;
				break;

			case OBJCHANGE_FINISHED:
			{
				pMouldGeometry->EnableControlBlobs();
				if (Chge==CC_OK)
				{
					Spread* pSpread = FindParentSpread();
					Chge=pMouldGeometry->RecordBlobs(pParam->GetOpPointer(), pSpread);
					if (Chge==CC_OK)
					{
						Chge = RemouldAll(pParam->GetOpPointer());
						if (Chge==CC_OK)
							Chge=EndSaveContext(pParam->GetOpPointer(), REC_REBUILD | REC_BLOBS | REC_GEOMCONTEXT);
					}
				}
			}
			break;

			case OBJCHANGE_FAILED:
				pMouldGeometry->RenderDragBlobs(pParam->pChangeSpread);
				pMouldGeometry->EnableControlBlobs();
				RedrawMould();
				break;

			default:
				break;
		}
	}
	return Chge;
#endif
}




	
/********************************************************************************************

>	ChangeCode NodeMould::RemouldAll(UndoableOperation* pUndoOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Inputs:		pUndoOp	= pointer to an active undoable operation
						  NULL if none
	Returns:	CC_OK	if we have successfully remoulded all objects
				CC_FAIL	if we have failed to remould. We've possible run out of memory
	Purpose:	Creates a new set of moulded objects, now the mould shape has changed 

********************************************************************************************/

ChangeCode NodeMould::RemouldAll(UndoableOperation* pUndoOp)
{
#if defined(EXCLUDE_FROM_RALPH)
	ERROR3("NodeMould::RemouldAll called");
#else
	BOOL ok;

	// if we're in detached mode, exit now.
	if (DetachedGeometry)
		return CC_OK;

	NodeMouldGroup* pMouldGroup = FindMouldGroup();
	if (pMouldGroup==NULL)
		return CC_FAIL;

	NodeMoulder* pMoulder = FindFirstMoulder();
	while (pMoulder!=NULL)
	{
		// Find the next moulder object
		Node* pNext = FindNext(CC_RUNTIME_CLASS(NodeMoulder));

		// Now build the moulder object 
		NodeMoulder* pNewMoulder = AddNewMoulder(pMoulder,NEXT,pUndoOp);
		if (!pNewMoulder)
			return CC_FAIL;

		CALL_WITH_FAIL(pUndoOp->DoHideNode(pMoulder,TRUE,NULL,FALSE), pUndoOp, ok);
		if (!ok)
			return CC_FAIL;

		if (!pNewMoulder->CreateMouldedObjects(pMouldGroup, GetGeometry(), pUndoOp)) 
			return CC_FAIL;

		pMoulder=(NodeMoulder*)pNext;
	}

	// now, inform all parents of a child change
	// go to all parents, sending child change messages
	Node * pParent = NULL;

	// DMc
	// do the child change message
	ObjChangeFlags Flags;
	ObjChangeParam EditObjChange(OBJCHANGE_FINISHED, Flags, this, pUndoOp, OBJCHANGE_CALLEDBYOP);

	pParent = this->FindParent();

	while (pParent)
	{
		pParent->OnChildChange(&EditObjChange);

		pParent = pParent->FindParent();
	}	

	// Update the selection range
	Camelot.UpdateSelection();

#endif
	return CC_OK;
}


/*

ChangeCode NodeMould::RemouldAll(UndoableOperation* pUndoOp)
{
	// if we're in detached mode, exit now.
	if (DetachedGeometry)
		return CC_OK;

	NodeMouldGroup* pMouldGroup = FindMouldGroup();
	NodeMoulder* pMoulder = FindFirstMoulder();

	if ((pMouldGroup!=NULL) && (pMoulder!=NULL))
	{
		pMoulder->DestroyMouldedObjects();

		if (!pMoulder->CreateMouldedObjects(pMouldGroup, GetGeometry(), NULL)) 
		{
			pMoulder->DestroyMouldedObjects();
			return CC_FAIL;
		}
	}

	return CC_OK;
}
*/




/*********************************************************************************************

>    void NodeMould::PreExportRender(RenderRegion* pRegion)

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	14/03/95
     Inputs:	pRegion = ptr to the export render region to export to
     Outputs:	
     Returns:	
     Purpose: 	Called before this node or any of its children have been rendered to the 
     			export region. This outputs the "start mould" command.
				Supports ArtWorks EPS and Camelot EPS
     Errors:    

**********************************************************************************************/
 
void NodeMould::PreExportRender(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
	if (pRegion->IS_KIND_OF(NativeRenderRegion))
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

>    BOOL NodeMould::ExportRender(RenderRegion* pRegion)

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	14/03/95
     Inputs:	pRegion = ptr to the export render region to export to
     Outputs:	
     Returns:	TRUE if ok, FALSE if something went wrong
     Purpose: 	Called after this node and all of its children have been rendered to the
     			export region. This outputs the "end mould" command.
				Supports ArtWorks EPS and Camelot EPS
     Errors:    

**********************************************************************************************/
 
BOOL NodeMould::ExportRender(RenderRegion* pRegion) 
{
#ifdef DO_EXPORT
	if (pRegion->IS_KIND_OF(NativeRenderRegion))
		return PostExportCAMEPS(pRegion);

	if (pRegion->IS_KIND_OF(ArtWorksEPSRenderRegion))
		return PostExportAWEPS(pRegion);
#endif
	return FALSE;
}



void NodeMould::PreExportCAMEPS(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
	MouldSpace mSpace = DescribeGeometry();
	EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();

	pDC->OutputValue(INT32(MOULD_EPS_VERSION));	// The current mould format flag
	switch (mSpace)
	{
		case MOULDSPACE_ENVELOPE:
			if (IsDetached())
				InformWarning(_R(IDS_SAVE_WHILE_DETACHEDE), _R(IDS_OK));
			pDC->OutputToken("csev");			// Camelot "start envelope" token

			break;

		case MOULDSPACE_PERSPECTIVE:
			if (IsDetached())
				InformWarning(_R(IDS_SAVE_WHILE_DETACHEDP), _R(IDS_OK));
			pDC->OutputToken("cspr");			// Camelot "start perspective" token
			break;

		default:
			break;
	}
	pDC->OutputNewLine();

	// output the threshold extension token. We don't need to output this
	// if the threshold is still 1024 ie the old value. The importer will
	// default to this if there's no threshold token in the file.
	INT32 Threshold = GetGeometry()->GetThreshold();
	if (Threshold != MOULD_V1THRESHOLD)
	{
		pDC->OutputValue((INT32)TAG_MOULDTHRESHOLD);
		pDC->OutputToken("cso");
		pDC->OutputNewLine();
		pDC->OutputValue(Threshold);
		pDC->OutputToken("cmth");
		pDC->OutputNewLine();
		pDC->OutputToken("ceo");
		pDC->OutputNewLine();
	}
#endif
}




BOOL NodeMould::PostExportCAMEPS(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
	MouldSpace mSpace = DescribeGeometry();
	EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();

	switch (mSpace)
	{
		case MOULDSPACE_ENVELOPE:
			pDC->OutputToken("ceev");			// Camelot "end envelope" token
			break;
		case MOULDSPACE_PERSPECTIVE:
			pDC->OutputToken("cepr");			// Camelot "end perspective" token
			break;
		default:
			break;
	}
	pDC->OutputNewLine();
#endif

	return TRUE;
}


void NodeMould::PreExportAWEPS(RenderRegion* pRegion)
{
}

BOOL NodeMould::PostExportAWEPS(RenderRegion* pRegion)
{
	return TRUE;
}



/********************************************************************************************

> 	virtual BOOL NodeMould::PostImport()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	This function is called after a document is imported.  Nodes should override
				this function to do any post-import processing.
	SeeAlso:	-

********************************************************************************************/

BOOL NodeMould::PostImport()
{
//#pragma message( __LOCMSG__ "NodeMould::PostImport - do nothing" )
//	TRACE( _T("Warning - NodeMould::PostImport called\n") );

	// if theres already some moulded objects then fine....
	if (CountMoulders()>0)
		return TRUE;

	// find the created objects we need to set the geometry up
	NodeMouldPath* pNodeMPath = GetPathShape();
	NodeMouldGroup* pNodeMGroup = FindMouldGroup();
	
	if (pNodeMGroup==NULL || pNodeMPath==NULL)
		return FALSE;

	// Get the kids bounding rects
	DocRect Rect = pNodeMGroup->GetChildrensBounds(FALSE);
	// We need to set the geometry of our mould using the imported data
	// set the geometry using this new mould shape
	if (!GetGeometry()->Define(&(pNodeMPath->InkPath), &Rect))
		return FALSE;

	// Create a new moulder object
	NodeMoulder* pMoulder = AddNewMoulder(this, LASTCHILD, NULL);
	if (pMoulder==NULL)
		return FALSE;

	// and finally create all the moulded objects inside the moulder
	if (!pMoulder->CreateMouldedObjects(pNodeMGroup, GetGeometry(), NULL))
	{
		// Vape the whole lot.
		pMoulder->CascadeDelete();
		return FALSE;
	}

	// Force a redraw over the extent of the mould (for importing)
	// Document* pDoc=Document::GetSelected();
	// Naughty Naughty. You cannot rely on a static (?*&$) variable to tell you
	// what is selected. Why not just find your parent?
	Spread * pSpread = FindParentSpread();
	Document * pDoc = NULL;
	if (pSpread != NULL)
		pDoc = pSpread->FindParentDocument();
	if (pDoc!=NULL)
	{
		DocRect Rect = pNodeMPath->GetBlobBoundingRect();
		pDoc->ForceRedraw(pSpread, Rect, TRUE, this);
	}
	else
		ERROR3("NodeMold::PostImport() - pDoc==NULL");

	// Mark this nodes bounds as invalid
	InvalidateBoundingRect();

	// All is well and lovely
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL NodeMould::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/96
	Inputs:		pFilter = ptr to the filter to write to
	Returns:	TRUE is a record has been written, FALSE otherwise
	Purpose:	Writes out a records associated with this node

				It either writes out an envelope or perspective record, depending on the geometry.

				The only piece of data that needs to be saved out with a node mould is the
				error threshold.

********************************************************************************************/

BOOL NodeMould::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	ERROR2IF(pMouldGeometry == NULL,FALSE,"No mould geometry");

	UINT32 Tag  = TAG_UNDEFINED;
	UINT32 Size = 0;

	switch (NodeMould::DescribeGeometry())
	{
		case MOULDSPACE_ENVELOPE:		Tag = TAG_MOULD_ENVELOPE;	 Size = TAG_MOULD_ENVELOPE_SIZE;    break;
		case MOULDSPACE_PERSPECTIVE:	Tag = TAG_MOULD_PERSPECTIVE; Size = TAG_MOULD_PERSPECTIVE_SIZE; break;

		default:
			ERROR3("Unkown mould space type");
			pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));
			return FALSE;
	}

	INT32 Threshold = pMouldGeometry->GetThreshold();

	CXaraFileRecord Rec(Tag,Size);

	BOOL ok = Rec.Init();

	if (ok) ok = Rec.WriteINT32(Threshold);
	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//------------------------------------------------------------------
// See comments for NodeMould::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
BOOL NodeMould::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	DocRect NodeMould::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/26/00
	Purpose:	NodeMould uses Node's base-class implementation, as we don't want to inherit
				NodeGroup's extend behaviour - see Node::ValidateExtend() for details.
	See also:	Node::ValidateExtend().

********************************************************************************************/
DocRect NodeMould::ValidateExtend(const ExtendParams& ExtParams)
{
	return Node::ValidateExtend(ExtParams);
}



/********************************************************************************************

>	void NodeMould::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/26/00
	Purpose:	NodeMould uses Node's base-class implementation, as we don't want to inherit
				NodeGroup's extend behaviour - see Node::Extend() for details.
	See also:	Node::Extend().

********************************************************************************************/
void NodeMould::Extend(const ExtendParams& ExtParams)
{
	Node::Extend(ExtParams);

	// once we've done the extension, we redefine our path.
	// I know it's bad, but I don't know what this does and I'm just copying the code
	// from NodeMould::Transform. This code does not cause the mould to visibly change
	// shape, that bit happens when the contents of the NodeMoulder are Extended.
	if (pMouldGeometry)
	{
		Path* pPath = GetPath();
		NodeMouldGroup* pMouldGroup = FindMouldGroup();
		if (pPath && pMouldGroup)
		{
			DocRect Rect = pMouldGroup->GetChildrensBounds(TRUE);
			pMouldGeometry->Define(pPath, &Rect);
		}
	}
}



/********************************************************************************************

>	DocRect NodeMould::GetExtendTargetBounds(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/2000
	Purpose:	NodeMould uses NodeRenderableBounded's base-class implementation, as we don't
				want to inherit NodeGroup's extend behaviour - see base class for details.
	See also:	NodeRenderableBounded::Extend().

********************************************************************************************/
DocRect NodeMould::GetExtendTargetBounds(const ExtendParams& ExtParams)
{
	return NodeRenderableBounded::GetExtendTargetBounds(ExtParams);
}



//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/********************************************************************************************

>	virtual UINT32* MouldRecordHandler::GetTagList()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/96
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* MouldRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {	TAG_MOULD_ENVELOPE,
								TAG_MOULD_PERSPECTIVE,
								TAG_MOULD_GROUP,
								TAG_MOULD_PATH,
								TAG_MOULD_BOUNDS,	// WEBSTER - markn 10/2/97 - Mould bounds record
								CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	virtual BOOL MouldRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL MouldRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	BOOL ok = TRUE;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_MOULD_ENVELOPE:
		case TAG_MOULD_PERSPECTIVE:
			ok = HandleMouldRecord(pCXaraFileRecord);
			break;

		case TAG_MOULD_GROUP:
			ok = HandleMouldGroupRecord(pCXaraFileRecord);
			break;

		case TAG_MOULD_PATH:
			ok = HandleMouldPathRecord(pCXaraFileRecord);
			break;

		// WEBSTER - markn 10/2/97
		// Mould bounds record
		case TAG_MOULD_BOUNDS:
			break;

		default:
			ok = FALSE;
			ERROR3_PF(("I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}


/********************************************************************************************

>	BOOL MouldRecordHandler::HandleMouldRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a envelope or perspective mould record
	SeeAlso:	-

********************************************************************************************/

BOOL MouldRecordHandler::HandleMouldRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 Tag = pCXaraFileRecord->GetTag();

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_MOULD_ENVELOPE && Tag != TAG_MOULD_PERSPECTIVE,FALSE,"I don't handle this tag type");

	NodeMould* pMould = new NodeMould;
	if (pMould == NULL)
		return FALSE;

	// Read in the data
	INT32 Threshold;
	BOOL ok = pCXaraFileRecord->ReadINT32(&Threshold);

	if (ok)
	{
		MouldSpace mSpace=MOULDSPACE_UNDEFINED;

		// Find out what type of geometry to use.
		switch (Tag)
		{
			case TAG_MOULD_ENVELOPE:    mSpace = MOULDSPACE_ENVELOPE;    break;
			case TAG_MOULD_PERSPECTIVE: mSpace = MOULDSPACE_PERSPECTIVE; break;
			default: ERROR3("Unknown geometry"); break;
		}
		
		// Create the correct geometry.
		ok = pMould->CreateGeometry(mSpace);

		if (ok)
		{
			// Find the geometry and set the threshold - then we can stick the mould node in the tree.
			MouldGeometry* pGeometry = pMould->GetGeometry();
			if (pGeometry != NULL)
			{
				pGeometry->SetThreshold(Threshold);
				ok = InsertNode(pMould);
			}
			else
				ok = FALSE;
		}
	}

	return ok;
}

/********************************************************************************************

>	BOOL MouldRecordHandler::HandleMouldGroupRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a mould group record
	SeeAlso:	-

********************************************************************************************/

BOOL MouldRecordHandler::HandleMouldGroupRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_MOULD_GROUP,FALSE,"I don't handle this tag type");

	NodeMouldGroup* pMouldGroup = new NodeMouldGroup;

	if (pMouldGroup != NULL)
		return InsertNode(pMouldGroup);

	return FALSE;
}

/********************************************************************************************

>	BOOL MouldRecordHandler::HandleMouldPathRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a mould path record
	SeeAlso:	-

********************************************************************************************/

BOOL MouldRecordHandler::HandleMouldPathRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_MOULD_PATH,FALSE,"I don't handle this tag type");

	BOOL ok = FALSE;

	NodeMouldPath* pNodeMouldPath = new NodeMouldPath;

	if (pNodeMouldPath != NULL && pNodeMouldPath->SetUpPath())
	{
		ok = pCXaraFileRecord->ReadPath(&pNodeMouldPath->InkPath); 
		if (ok) pNodeMouldPath->InkPath.InitialiseFlags();	// Init the path flags array to something sensible
	 	if (ok)	ok = InsertNode(pNodeMouldPath);
		if (ok) SetLastNodePathInserted(pNodeMouldPath);	// Set it as last path inserted.
	}

	return ok;
}

/********************************************************************************************

>	virtual void MouldRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for the mould records.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if XAR_TREE_DIALOG
void MouldRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	char s[256];

	//	 Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	switch (pRecord->GetTag())
	{
		case TAG_MOULD_ENVELOPE:
		case TAG_MOULD_PERSPECTIVE:
		{
			INT32 Threshold;

			pRecord->ReadINT32(&Threshold);

			_stprintf(s,"Threshold = %d\r\n",Threshold);
			(*pStr) += s;
		}
		break;

		case TAG_MOULD_PATH:
			// Use standard base class func for describing the path textually
			DescribePath(pRecord,pStr);
			break;
	}
}
#endif

/********************************************************************************************

>	virtual BOOL NodeMould::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Inputs:		pSpread		The spread in which things are happening
				PointerPos	The Location of the mouse pointer at the time of the click
				pMenu		The menu to which items should be added
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the Mould object to respond to pop up menu clicks on itself.

********************************************************************************************/

BOOL NodeMould::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)
{
//WEBSTER-ranbirr-01/12/96	
#ifndef WEBSTER
	BOOL ok = TRUE;
//	WEBSTER-ranbirr-01/12/96	
#ifndef WEBSTER
	
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	ok = ok && pMenu->BuildCommand(TOOL_OPTOKEN_MOULD, 	TRUE);
//	ok = ok && pMenu->BuildCommand(OPTOKEN_DETACHMOULD, FALSE);
//	ok = ok && pMenu->BuildCommand(OPTOKEN_ROTATEMOULD, FALSE);
//	ok = ok && pMenu->BuildCommand(OPTOKEN_COPYMOULD, 	TRUE);
//	ok = ok && pMenu->BuildCommand(OPTOKEN_RECTANGULARENVELOPE,		FALSE);
//	ok = ok && pMenu->BuildCommand(OPTOKEN_RECTANGULARPERSPECTIVE,	TRUE);
#endif
#endif //webster

	return ok;
#else //webster
	return FALSE;
#endif //webster
}





/********************************************************************************************

	RecordChangeCodesAction::RecordChangeCodesAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Purpose:	Constructor for the action to undo Perspective modification

********************************************************************************************/

RecordChangeCodesAction::RecordChangeCodesAction()
{
	pCRCMould=NULL;
	CRCcode=0;
	CRCWidth=0;
	CRCHeight=0;
}


/********************************************************************************************

	RecordChangeCodesAction::~RecordChangeCodesAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Purpose:	Destructor for the action to undo Perspective modification

********************************************************************************************/

RecordChangeCodesAction::~RecordChangeCodesAction()
{
}


/********************************************************************************************

>	ActionCode RecordChangeCodesAction::Init(Operation* pOp,
											 ActionList* pActionList,
											 NodeMould* pMould,
											 Action** NewAction)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		pOp 		= a pointer to the operation to which this action belongs
				pActionList = is the action list to which this action should be added
				pMould		= a pointer to a NodeMould object

	Outputs:	NewAction 	= a pointer to a pointer to an action, allowing the function to 
							  return a pointer to the created action.
	Returns:	ActionCode, 
					= AC_OK			if the action was created correctly
					= AC_NORECORD	if no memory to undo/redo but go ahead anyway without undo
					= AC_FAIL		stop the operation

	Purpose:	This is the function which creates an instance of this action. If there is
				no room in the undo buffer (which is determined by the base class Init 
				function called within)	the function will either return AC_NORECORD which
				means the operation can continue, but no undo information needs to be stored,
				or AC_OK which means the operation should continue AND record undo information.
				If the function returns AC_FAIL, there was not enough memory to record the 
				undo information, and the user has decided not to continue with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode RecordChangeCodesAction::Init(Operation* pOp,
										 ActionList* pActionList,
										 NodeMould* pMould,
										 Action** NewAction)
{
	ActionCode Ac = AC_FAIL;
	if (pMould!=NULL)
	{
		UINT32 ActSize = sizeof(RecordChangeCodesAction);
		Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(RecordChangeCodesAction), NewAction);
		if (Ac==AC_OK)
		{
			RecordChangeCodesAction* pAct = ((RecordChangeCodesAction*)*NewAction);
			if (pAct)
			{
				pAct->pCRCMould	= pMould;
				pAct->CRCcode 	= pMould->OnCC_CRC;
				pAct->CRCWidth 	= pMould->OnCC_Width;
				pAct->CRCHeight = pMould->OnCC_Height;
			}
		}
	}
	return Ac;
}

/********************************************************************************************

>	ActionCode RecordChangeCodesAction::Execute()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, 
					= AC_OK			if the action was created correctly
					= AC_NORECORD	if no memory to undo/redo but go ahead anyway without undo
					= AC_FAIL		stop the operation
	Purpose:	This is the virtual function that is called when the action is executed
				by the Undo/Redo system. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ActionCode RecordChangeCodesAction::Execute()
{
	// try to create a redo record
	RecordChangeCodesAction* NewAction;
	ActionCode Ac;
	Ac = RecordChangeCodesAction::Init(pOperation, pOppositeActLst, pCRCMould, (Action**)(&NewAction));
	
	if (pCRCMould)
	{
		pCRCMould->OnCC_CRC 	= CRCcode;
		pCRCMould->OnCC_Width 	= CRCWidth;
		pCRCMould->OnCC_Height 	= CRCHeight;
	}
		
	return Ac;
}
