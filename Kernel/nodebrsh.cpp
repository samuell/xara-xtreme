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

// implementation of the NodeBrush class
#include "camtypes.h"
#include "nodebrsh.h"
//#include "group.h"
//#include "rndrgn.h"
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "blobs.h"
//#include "nodershp.h"
#include "ndbrshpt.h"
#include "ndbrshmk.h"
//#include "progress.h"
//#include "objchge.h"
//#include "markn.h"
//#include "opdrbrsh.h"
#include "opshadow.h"
//#include "opbevel.h"
#include "objchge.h"

class BlendRef;

CC_IMPLEMENT_DYNCREATE(NodeBrush, NodeGroup);
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW
#define BRUSH_MIN_SPACING 100
#define BRUSH_MAX_SPACING 10000000
#define BRUSH_DEFAULT_SPACING 10000


/*********************************************************************************************

>    NodeBrush::NodeBrush() 

     Author:	Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:	6/10/99
     Inputs:	
     Outputs:	
     Returns:	
     Purpose: 	This constructor creates a NodeBrush linked to no other
     Errors:    

**********************************************************************************************/
 
NodeBrush::NodeBrush(): NodeGroup()
{
	
}
	
/***********************************************************************************************

>   void NodeBrush::NodeBrush
	(
		Node* ContextNode,  
		AttachNodeDirection Direction,  
		BOOL Locked = FALSE, 
		BOOL Mangled = FALSE,  
		BOOL Marked = FALSE, 
		BOOL Selected = FALSE, 
	)

    Author:  Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/10/99
    Inputs: ContextNode: Pointer to a node which this node is to be attached to.     
    
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
			
    Outputs:   -
    Returns:   - 
    Purpose: This method initialises the node and links it to ContextNode in the
			 direction specified by Direction. All necessary tree links are
			 updated.     
			 
    Errors:  An assertion error will occur if ContextNode is NULL


***********************************************************************************************/

NodeBrush::NodeBrush(Node* ContextNode,  
					 AttachNodeDirection Direction,  
					 BOOL Locked, 
				 	 BOOL Mangled,  
					 BOOL Marked, 
					 BOOL Selected   
			   ):NodeGroup(ContextNode, Direction, Locked, Mangled, Marked, 
			 	Selected) 
{ 


} 


/*********************************************************************************************

>    NodeBrush::~NodeBrush() 

     Author:	Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:	6/10/99
     Inputs:	
     Outputs:	
     Returns:	
     Purpose: 	Destructor
     Errors:    

**********************************************************************************************/


NodeBrush::~NodeBrush()
{
	

}

/********************************************************************************************

>	DocRect NodeBrush::GetBlobBoundingRect()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Returns:	DocRect - The bounding rect of the node and its blobs
	Purpose:	This calls the base classes GetBlobBoundingRect(), and inflates the result by the width
				of a blob

********************************************************************************************/

DocRect NodeBrush::GetBlobBoundingRect()
{
	
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the base class blob bounding rectangle
	DocRect Rect = NodeGroup::GetBlobBoundingRect();

	// inflate it by the width of a blob (plus a bit)
	DocRect TempRect;
	GetApplication()->GetBlobManager()->GetBlobRect(Rect.lo,&TempRect);
	INT32 Delta = ((TempRect.hi.x - TempRect.lo.x)*1)/1;
	Rect.Inflate(Delta);

	return Rect;
#else
	return DocRect(0,0,0,0);
#endif

}

/********************************************************************************************

>	DocRect NodeBrusher::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		DontUseAttrs - TRUE if we should ignore the nodes attributes.
				Defaults to FALSE
				HitTest      - TRUE if being called during HitTest
	Returns:	The nodes bounding rect
	Purpose:	if the bounding rect is valid it is returned, if not, it is recalculated
				and then returned.
	SeeAlso:	NodeBrusher::GetBlobBoundingRect

********************************************************************************************/


DocRect	NodeBrush::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	/*
	// first get the bounding rect of the path
	DocRect Rect =  m_pNodeBrushPath->GetBoundingRect();
	
	// inflate by half the size of the brush ink object.

	NodeRenderableInk* pInk = m_pNodeBrushMaker->GetInkNode();
	
	if (pInk != NULL)
	{
		DocRect InkRect = pInk->GetBoundingRect();

		INT32 dx = InkRect.Width() / 2;
		INT32 dy = InkRect.Height() / 2;

		Rect.Inflate(dx, dy);
	}

	return Rect;
	*/
	// default docrect constructor makes empty rect
	DocRect Rect;
	if (m_pNodeBrushMaker != NULL)
		Rect =  m_pNodeBrushMaker->GetBoundingRect();
	
	return Rect;
}



/********************************************************************************************

>	void NodeBrush::Render (RenderRegion* pRender)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		pRender - The region to draw the blobs in
	Purpose:	Renders the nodebrushpath, if selected, otherwise nothing.  The main 
				rendering is performed by the nodebrushmaker

********************************************************************************************/

void NodeBrush::Render(RenderRegion* pRender)
{
	if (this->IsSelected())
	{
		if (m_pNodeBrushPath != NULL)
			m_pNodeBrushPath->RenderObjectBlobs(pRender);
	}
	if (m_pNodeBrushMaker != NULL)
		m_pNodeBrushMaker->Render(pRender);
}


/********************************************************************************************
>	virtual SubtreeRenderState NodeBrush::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/06/2004
	Inputs:		A Render Region to Render into.
	Returns:	PRE_FAILED for something went wrong,
				PRE_RENDER_CHILDREN for continue to render children,
				PRE_NO_RENDER_CHILDREN for DO NOT Render any of my children!!!!

	Purpose:	Enables Nodes to be able to Do Pre Print time alterations or even take over
				the control of the current Printing of their children.

********************************************************************************************/
SubtreeRenderState NodeBrush::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
	return SUBTREE_ROOTONLY;
}




/********************************************************************************************

>	void NodeBrush::RenderObjectBlobs(RenderRegion* pRender)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		pRender - The region to draw the blobs in
	Purpose:	Renders the object blobs

********************************************************************************************/

void NodeBrush::RenderObjectBlobs(RenderRegion* pRegion)
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// Find out about the groups bounding rect
	DocRect BoundingRect = GetBoundingRect();

	// Inflate the bounds by the width of a blob
	DocRect TempRect;
	GetApplication()->GetBlobManager()->GetBlobRect(BoundingRect.lo,&TempRect);
	INT32 Delta = ((TempRect.hi.x - TempRect.lo.x)*3)/4;
	BoundingRect.Inflate(Delta);

	// Find out where to draw the blobs
	DocCoord Low  = BoundingRect.LowCorner();
	DocCoord High = BoundingRect.HighCorner();

	// Set the colours of the blobs
	pRegion->SetFillColour(COLOUR_UNSELECTEDBLOB);
	pRegion->SetLineColour(COLOUR_NONE);

	// Draw all the blobs
	pRegion->DrawBlob(Low, BT_UNSELECTED);  
	pRegion->DrawBlob(High, BT_UNSELECTED); 
	pRegion->DrawBlob(DocCoord(Low.x, High.y), BT_UNSELECTED); 
	pRegion->DrawBlob(DocCoord(High.x, Low.y), BT_UNSELECTED); 

	// for some reason the NBP is never called, there is probably a 
	// proper fix for this but I don't have time right now, so render
	// the nodeblend path here
	m_pNodeBrushPath->RenderObjectBlobs(pRegion);


#endif 
}


/********************************************************************************************

>	void NodeBrush::RenderTinyBlobs(RenderRegion* pRender)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		pRender - The region to draw the blobs in
	Purpose:	Renders the object blobs

********************************************************************************************/

void NodeBrush::RenderTinyBlobs(RenderRegion* pRegion)
{
	// for some reason the NBP is never called, there is probably a 
	// proper fix for this but I don't have time right now, so render
	// the nodeblend path here
	m_pNodeBrushPath->RenderTinyBlobs(pRegion);

}


/********************************************************************************************

>	void NodeBrush::Transform(TransformBase& Trans)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		Trans - the transformation to perform
	Purpose:	transforms the brush

********************************************************************************************/

void NodeBrush::Transform(TransformBase& Trans)
{
	// The groups bounding rect is no longer valid
	InvalidateBoundingRect();

	// Transform all the children
	// See GroupCanTransformCached()
	NodeGroup::Transform(Trans);

}



/********************************************************************************************

  >	virtual BOOL NodeBrush::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/20/99
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the brush record to the filter
				
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBrush::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	return TRUE;
}


/********************************************************************************************

  >	virtual BOOL NodeBrush::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/20/99
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the brush record to the filter
				
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBrush::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	return TRUE;
}



/***********************************************************************************************

> Node* NodeBrush::SimpleCopy()  

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/10/99
	Inputs:     -  
    Outputs:    -
    Returns:    A copy of the node, or NULL if memory has run out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	            
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL.                                                                      
                                                                                 
**********************************************************************************************/

Node* NodeBrush::SimpleCopy()
{
	NodeBrush* pCopyOfNode = new NodeBrush();
	if (pCopyOfNode != NULL)
		CopyNodeContents(pCopyOfNode);
	return (pCopyOfNode);
}   

/***********************************************************************************************

>	void NodeBrush::CopyNodeContents(Node* pCopyOfNode)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		pCopyOfNode - The node to copy data to
	Outputs:	-
	Returns:	-
	Purpose:	Copies the data from this node to pCopyOfNode by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeGroup::CopyNodeContents

***********************************************************************************************/

void NodeBrush::CopyNodeContents(NodeBrush* pCopyOfNode)
{
	NodeGroup::CopyNodeContents(pCopyOfNode);

	// Copy member vars here

}
   


/***********************************************************************************************
>   void NodeBrush::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeBrush::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeBrush), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeBrush))
		CopyNodeContents((NodeBrush*)pNodeCopy);
}



/********************************************************************************************

>	virtual String NodeBrush::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		Plural: Flag indicating if the string description should be plural or
						singular. 
	Outputs:	-
	Retuns:		Description of the blend node 
	Purpose:	To return a description of the Brush object in either the singular or the 
				plural. This method is called by the DescribeRange method.
				
				The description will always begin with a lower case letter.   
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

String NodeBrush::Describe(BOOL Plural, BOOL Verbose) 
{     
	return m_BrushName;
}


/*********************************************************************************************

>    NodeBrushPath* NodeBrush::GetNodeBtushPath() 

     Author:	Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:	6/10/99
     Inputs:	
     Outputs:	
     Returns:	the nodebrushpath used by this brush
     Purpose: 	as above
**********************************************************************************************/

NodeBrushPath* NodeBrush::GetNodeBrushPath()
{
	return m_pNodeBrushPath;
}



/*********************************************************************************************

>    void NodeBrush: SetNodeBrushPath() 

     Author:	Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:	6/10/99
     Inputs:	
     Outputs:	
     Returns:	the nodebrushpath used by this brush
     Purpose: 	as above
**********************************************************************************************/

void NodeBrush::SetNodeBrushPath(NodeBrushPath* pPath)
{
	m_pNodeBrushPath = pPath;

}



/*********************************************************************************************

>    NodeBrushMaker* NodeBrush::GetNodeBtushMaker() 

     Author:	Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:	6/10/99
     Inputs:	
     Outputs:	
     Returns:	the nodebrushMaker used by this brush
     Purpose: 	as above
**********************************************************************************************/

NodeBrushMaker* NodeBrush::GetNodeBrushMaker()
{
	return m_pNodeBrushMaker;
}



/*********************************************************************************************

>    void NodeBrush: SetNodeBrushMaker() 

     Author:	Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:	6/10/99
     Inputs:	
     Outputs:	
     Returns:	the nodebrushMaker used by this brush
     Purpose: 	as above
**********************************************************************************************/

void NodeBrush::SetNodeBrushMaker(NodeBrushMaker* pMaker)
{
	m_pNodeBrushMaker = pMaker;

}

/*********************************************************************************************

>    MILLIPOINT NodeBrush: GetBrushSpacing() 

     Author:	Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:	19/11/99
     Inputs:	
     Outputs:	
     Returns:	the spacing between objects of this brush
     Purpose: 	as above
**********************************************************************************************/

MILLIPOINT NodeBrush::GetBrushSpacing()
{
	if (m_pNodeBrushMaker != NULL)
		return m_pNodeBrushMaker->GetBrushSpacing();
	else
	{
		ERROR3("NodeBrushMaker is NULL");
		return 0;
	}
}

/*********************************************************************************************

>    void NodeBrush: SetBrushSpacing(MILLIPOINT Spacing) 

     Author:	Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:	18/11/99
     Inputs:	Spacing to set
     Outputs:	
     Returns:	-
     Purpose: 	Sets the distance between brush objects used buy the nodebrushmaker
**********************************************************************************************/

void NodeBrush::SetBrushSpacing(MILLIPOINT Spacing)
{
	if (Spacing >= BRUSH_MIN_SPACING && Spacing >= BRUSH_MAX_SPACING)
	{
		ERROR3("Invalid spacing got passed to NodeBrush::SetBrushSpacing");		
		Spacing = BRUSH_DEFAULT_SPACING;
	}
			
	if (m_pNodeBrushMaker != NULL)
	{
		m_pNodeBrushMaker->SetBrushSpacing(Spacing);
	}
	else
		ERROR3("NodeBrushMaker is NULL");

}

/********************************************************************************************

>	BOOL NodeBrush::ResetBrushPath() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Outputs:    -
	Returns:	TRUE if successful, FALSE otherwise 
	Purpose:	Asks the nodebrushmaker to recalculate its number of steps, for use when 
				the nodebrushpath has been edited
				
********************************************************************************************/

BOOL NodeBrush::ResetBrushPath()
{
	// get the brushmaker to recalculate how many steps it needs
	if (m_pNodeBrushMaker != NULL)
	{
		m_pNodeBrushMaker->RecalculateMaxStep();
		m_pNodeBrushMaker->ClearListOfPointsToRender();
	}
	else
	{
		ERROR3("NodeBrushMaker is NULL");
		return FALSE;
	}
	return TRUE;

}

/********************************************************************************************

>	BOOL NodeBrush::ReInitialiseInk() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Outputs:    -
	Returns:	TRUE if successful, FALSE otherwise 
	Purpose:	Asks the nodebrushmaker to reinitialise itself, for use when the ink node has
				been altered, e.g. attribute changes.  Note that this must be called AFTER the
				attribute change in order to work.
			
********************************************************************************************/

BOOL NodeBrush::ReInitialiseInk()
{
	if (m_pNodeBrushMaker!= NULL)
	{
		NodeRenderableInk* pInk = m_pNodeBrushMaker->GetInkNode();
		if (pInk != NULL)
		{	
			//if (!m_pNodeBrushMaker->InitialiseInk(pInk))
			//	return FALSE;
			//else
			{
				// clear the cache in case we have any points left to render
				m_pNodeBrushMaker->ClearListOfPointsToRender();
				return TRUE;
			}
		}

	}
	ERROR3("NodeBrushMaker is NULL");
	return FALSE;

}

/********************************************************************************************

>	void NodeBrush::ChangeNodeBrushPath(NodeBrushPath * pNewPath) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pNewPath - the new path to set
	Outputs:    -
	Returns:	TRUE if successful, FALSE otherwise 
	Purpose:	Changes the nodebrushpath used by the brush

********************************************************************************************/

/* IMPLEMENTATION NOTE:
This function was written for use when the path was modified using the freehand or bezier tool.
These operations take care of hiding the old nodebrushpath for you, so it is not performed here.
If you wish to use this function for some other purpose then make sure you do something with 
the original nodebrushpath before swapping in a new one
*/

BOOL NodeBrush::ChangeNodeBrushPath(NodeBrushPath* pNewPath)
{
	if (pNewPath == NULL)
	{
		ERROR3("NodeBrushPath is NULL");
		return FALSE;;
	}

	m_pNodeBrushPath = pNewPath;
	if (m_pNodeBrushMaker != NULL)
		m_pNodeBrushMaker->InitialisePath(pNewPath);
	else
	{
		ERROR3("NodeBrushMaker is NULL");
		return FALSE;;
	}
	return TRUE;
}


/*********************************************************************************************

>	Node* NodeBrush::HasEditableChild(CCRuntimeClass* ChildClass, Node* pPreviousChild)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/11/99
     Inputs:    ChildClass		= the runtime class of the editable object
	 			pPreviousChild	= a pointer to the previous editable child 
	 							  returned by 'this' node, NULL if this is the first
								  call to this node.
     Outputs:   -
     Returns:   A node pointer, to an object which forms part of the editable surface of
	 			its parent (this node).
     Purpose:   This function returns our edit node (the NodeBrushPath)

**********************************************************************************************/

Node* NodeBrush::HasEditableChild(CCRuntimeClass* ChildClass, Node* pPreviousChild)
{
	if (ChildClass != CC_RUNTIME_CLASS(NodePath))
		return NULL;

	NodeBrushPath* pNodeBrushPath = GetNodeBrushPath();
	// check to see if this has already been asked for once
	if (((Node*)pNodeBrushPath) == pPreviousChild)
		return NULL;

	return pNodeBrushPath;
}


/*********************************************************************************************

>    NodeRenderableInk* NodeBrush: GetInkNode() 

     Author:	Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:	19/11/99
     Inputs:	
     Outputs:	
     Returns:	the ink node used by the nodebrushmaker to generate the blendpaths
     Purpose: 	as above
**********************************************************************************************/

NodeRenderableInk* NodeBrush::GetInkNode()
{
	if (m_pNodeBrushMaker != NULL)
	{
		return m_pNodeBrushMaker->GetInkNode();
	}
	return NULL;
}



/********************************************************************************************

> 	virtual BOOL NodeBrush::AllowOp(ObjChangeParam* pParam,BOOL SetOpPermissionState = TRUE)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/99
	Inputs:		pParam				 = describes the way an op wants to change the node
				SetOpPermissionState = if TRUE the Op permission state of this node will be set according to
										the outcome of the call
	Outputs:	-
	Returns:	TRUE means the node and all its parents are happy with this op, FALSE means don't do it
	Purpose:	In this instance, the func gives the brush a chance to stop an op from happening to one
				of its children.
				Currently the only permitted ops on children are those that change their appearance, .eg.
				change fill, line width etc.
						
				This must be called *before* the op is performed.
	SeeAlso:	Node::AllowOp(),GetOpPermission(),SetOpPermission();

********************************************************************************************/

BOOL NodeBrush::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState)
{
	ERROR2IF(pParam==NULL,FALSE,"NodeBlend::AllowOp() - pParam==NULL");

	// clean out the calling-child ptr, so it doesn't get passed around unintentionally.
	pParam->SetCallingChild(NULL);

	// decide if we allow it ... (and if we'll still exist after the op)
	BOOL allowed=TRUE;
	ObjChangeFlags Flags=pParam->GetChangeFlags();
	if (pParam->GetDirection()==OBJCHANGE_CALLEDBYCHILD)
	{
		if  (Flags.DeleteNode || Flags.MultiReplaceNode || Flags.MoveNode)
		{
			pParam->SetReasonForDenial(_R(IDS_BLEND_CANT_OP_ON_CHILDREN));
			allowed=FALSE;
		}
	}

	// if we allow it, (and our parents weren't calling us) see if our parents do ...
	if (allowed && pParam->GetDirection()!=OBJCHANGE_CALLEDBYPARENT && Parent!=NULL)
	{
		ObjChangeDirection OldDirection=pParam->GetDirection();
		pParam->SetCallingChild(this);
		pParam->SetDirection(OBJCHANGE_CALLEDBYCHILD);
		allowed=Parent->AllowOp(pParam,SetOpPermissionState);
		pParam->SetDirection(OldDirection);
	}

	if (allowed && pParam->GetDirection() == OBJCHANGE_CALLEDBYCHILD)
	{
		if (pParam->GetOpPointer())
		{
			if (pParam->GetOpPointer()->IS_KIND_OF(OpApplyShadow))
			{
				allowed = FALSE;
			}

PORTNOTE("other", "Removed use of OpCreateBevel from NodeBrush::AllowOp")
#if !defined(EXCLUDE_FROM_XARALX)
			if (pParam->GetOpPointer()->IS_KIND_OF(OpCreateBevel))
			{
				allowed = FALSE;
			}
#endif
		}
	}

	// if setting permisions ... (and possibly cause post-op code to be called)
	if (SetOpPermissionState)
	{
		if (allowed)
		{
			if (Parent!=NULL)
				Parent->SetOpPermission(PERMISSION_ALLOWED);

			// if post process required, ensure our OnChildChange is called (by setting permission on ourself),
			// insert an inverse action in the undo
			if (pParam->GetDirection()==OBJCHANGE_CALLEDBYCHILD || Flags.Attribute)
			{
				SetOpPermission(PERMISSION_ALLOWED);
PORTNOTE("other", "Removed use of ChangeBrushOpParam from NodeBrush::AllowOp")
#if !defined(EXCLUDE_FROM_XARALX)
				UndoableOperation* pOp=pParam->GetOpPointer();
			 	if (pOp!=NULL)
				{
					ChangeBrushOpParam BrushParam;
					BrushParam.ChangeType = CHANGEBRUSH_REGEN;
					ChangeBrushAction* pNewAction;
					if (allowed) allowed=(ChangeBrushAction::Init(pOp, pOp->GetUndoActionList(), this, 
										   &BrushParam, &pNewAction) != AC_FAIL);
					if (allowed) allowed=pOp->DoInvalidateNodeRegion(this,TRUE);
				}
#endif
			}
		}
		else
			SetOpPermission(PERMISSION_DENIED,TRUE);
	}

	// if the op was allowed, inform our children by calling their AllowOp()s
	// this must be done after we've inserted our undo actions so that any child undo actions will be undone first!
	if (allowed && pParam->GetDirection()!=OBJCHANGE_CALLEDBYCHILD)
		AllowOp_AccountForCompound(pParam, SetOpPermissionState);


	// return result (directly, or indirectly via a child AllowOp()) to op
	return allowed;
}


/********************************************************************************************

>	virtual ChangeCode NodeBlend::OnChildChange(ObjChangeParam* pParam)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/99
	Inputs:		pParam	= pointer to a object change parameter class
	Returns:	CC_OK		if we have successfully processed the change.
				CC_FAIL		if we cannot handle this particular change and must prevent the
							child from continuing
	Purpose:	This function should be overridden in derived object classes.
				Composite objects can use this function to respond to one of their children
				undergoing a change. They should return CC_FAIL whenever they are unable to
				cope with the change.

				Brushes simply regenerate themselves at the moment.

	SeeAlso:	WarnParentOfChange(),AllowOp();

********************************************************************************************/

ChangeCode NodeBrush::OnChildChange(ObjChangeParam* pParam)
{
PORTNOTE("other","NodeBrush::OnChildChange - ChangeBrushAction ChangeBrushOpParam")
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(pParam == NULL,CC_FAIL,"pParam == NULL");

	ObjChangeType cType  = pParam->GetChangeType();
	ObjChangeFlags Flags = pParam->GetChangeFlags();

	if (cType == OBJCHANGE_FINISHED)
	{
		UndoableOperation* pUndoOp = pParam->GetOpPointer();

		if (pUndoOp == NULL)
		{
			ERROR3("Couldn't get undo op");
			return CC_FAIL;
		}

		ChangeBrushAction* pAction;
		ChangeBrushOpParam OpParam;
		OpParam.ChangeType = CHANGEBRUSH_REGEN;
		
		BOOL ok = (ChangeBrushAction::Init(pUndoOp, pUndoOp->GetUndoActionList(), 
			this, &OpParam, &pAction) != AC_FAIL);
	
		if (ok) ok = pUndoOp->DoInvalidateNodeRegion(this,TRUE);

		if (ok)
			return CC_OK;
		else
			return CC_FAIL;
	}
#endif
	return CC_OK;
}
