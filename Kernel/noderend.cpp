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

// Implementation of the following classes:
/*
	NodeRenderable
		NodeRenderableInk
		NodeRenderablePaper
*/ 
                   
/*
*/

#include "camtypes.h"                          

DECLARE_SOURCE("$Revision$");

//#include "ccobject.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "node.h"   - in camtypes.h [AUTOMATICALLY REMOVED]
#include "paper.h"
#include "dumbnode.h"  
//#include "docrect.h"    - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h"   
//#include "simon.h"
//#include "nodeattr.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rndrgn.h" 
//#include "bitmapcache.h"
//#include "resource.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "stockcol.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rendbits.h"
#include "nodepath.h"
#include "noderend.h"
#include "camelot.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h" // for _R(IDE_NOMORE_MORY)
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "chapter.h"
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodedoc.h"
#include "blobs.h"
#include "objreg.h"
#include "layer.h"
//#include "basedoc.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "progress.h"
#include "gdraw.h"
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "lineattr.h"
#include "grndrgn.h"			// for REAL_GDRAW
#include "snap.h"
#include "contmenu.h"
//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodetext.h"
#include "nodetxtl.h"
#include "attrmap.h"
#include "ralphdoc.h"
#include "extender.h"			// for ExtendParams
//#include "swfrndr.h"
//#include "bitmapcache.h"
//#include "capturemanager.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodeliveeffect.h"
#include "cxftags.h"
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "layermsg.h"	// LayerMsg::LayerReason::REDRAW_LAYER

//----------------------------------------------------------------------------------------------
// NodeRenderable methods
 
CC_IMPLEMENT_DYNAMIC(NodeRenderable, Node)           
CC_IMPLEMENT_DYNAMIC(NodeRenderableBounded, NodeRenderable)
CC_IMPLEMENT_DYNAMIC(NodeRenderableInk, NodeRenderableBounded)
CC_IMPLEMENT_DYNAMIC(NodeHidden, Node)
CC_IMPLEMENT_MEMDUMP(CopyBecomeA, BecomeA)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW    


/*********************************************************************************************

	Preference:	Point Radius
	Section:	Magnetic Options
	Range:		0 - lots
	Purpose:	This is the radius around a control point (such as the corner of a rectangle,
				or the end point in the middle of a path) that will allow other points to
				be magnetically attracted to it. This is measured in millipoints
	SeeAlso:	Line Radius

**********************************************************************************************/ 
INT32 NodeRenderableBounded::MagneticPointRadius = 750*6;		// 6 pixels at 96DPI


/*********************************************************************************************

	Preference:	Line Radius
	Section:	Magnetic Options
	Range:		0 - lots
	Purpose:	This is the radius around a path (such as the edges of a rectangle,
				or the whole of a path) that will allow other points to
				be magnetically attracted to it. This is measured in millipoints
	SeeAlso:	Point Radius

**********************************************************************************************/ 
INT32 NodeRenderableBounded::MagneticLineRadius = 750*4;		// 4 pixels at 96DPI



/*********************************************************************************************

	Preference:	EnableCacheing
	Section:	Rendering
	Range:		0 or 1
	Purpose:	Enable or disable bitmap cacheing of parts of the tree during GRenderRegion
				rendering
	SeeAlso:	-

**********************************************************************************************/ 
BOOL NodeRenderableBounded::bEnableCacheing = TRUE;


/*********************************************************************************************

	Preference:	ThrottleCacheing
	Section:	Rendering
	Range:		0 or 1
	Purpose:	TRUE - Throttle cacheing to be 5% of total rendering time or less
	SeeAlso:	-

**********************************************************************************************/ 
BOOL NodeRenderableBounded::bThrottleCacheing = TRUE;



/*********************************************************************************************

	Preference:	ShowCacheBlobs
	Section:	Rendering
	Range:		0 or 1
	Purpose:	TRUE - Show special markers when cached groups are selected
	SeeAlso:	-

**********************************************************************************************/ 
BOOL NodeRenderableBounded::bShowCacheBlobs = FALSE;



/*********************************************************************************************

>    NodeRenderable::NodeRenderable() 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:    -
     Outputs:   
     Returns:   -
			  
     Purpose: This constructor creates a NodeRenderable linked to no other with all status
		  	  flags false and an uninitialised bounding rectangle.           
			
     Errors:    

**********************************************************************************************/
 
 
NodeRenderable::NodeRenderable():Node()   
{     
	Flags.Renderable	= TRUE;
	Dragged				= FALSE;
}


/***********************************************************************************************

>   NodeRenderable::NodeRenderable(	Node* ContextNode,  
									AttachNodeDirection Direction,  
									BOOL Locked = FALSE, 
									BOOL Mangled = FALSE,  
									BOOL Marked = FALSE, 
									BOOL Selected = FALSE 
									)

    Author: Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:26/4/93             
    
    Inputs: ContextNode: Pointer to a node which this node is to be attached to.     
		
			Direction: 
			
				Specifies the direction in which the node is to be attached to the 
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
  
    Errors: An assertion error will occur if ContextNode is NULL

***********************************************************************************************/

NodeRenderable::NodeRenderable(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			  ): Node(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  
{                         
} 


/********************************************************************************************

>	BOOL NodeRenderable::NeedsToRender( RenderRegion *pRender )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/94
	Inputs:		pRender - A pointer to the current render region (NULL if none)
	Purpose:	Virtual function - this function will indicate to the caller whether or not
				we want to render the given node, according to the information passed in.
				For this class, it always returns TRUE.
	SeeAlso:	Node::NeedsToRender

********************************************************************************************/

SubtreeRenderState NodeRenderable::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
	return SUBTREE_ROOTANDCHILDREN;				// Else we allow the renderer into the subtree
}

/********************************************************************************************

>	BOOL NodeRenderable::NeedsToExport(RenderRegion *pRender, BOOL VisibleLayersOnly = FALSE,
									   BOOL CheckSelected = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/94
	Inputs:		pRender - A pointer to the current export region (NULL if none)
				VisibleLayersOnly - TRUE => remove nodes which are on invisible layers
								   - FALSE => export everything
				CheckSelected - TRUE => we check if object selected and only export selected bjects
							  - FALSE => we don't bother checking for selection or not
	Returns:	TRUE => we always want to export NodeRenderable objects.
	Purpose:	Indicate that we want to export this class of nodes.
	SeeAlso:	NodeRenderable::NeedsToRender

********************************************************************************************/

BOOL NodeRenderable::NeedsToExport(RenderRegion *pRender, BOOL VisibleLayersOnly, BOOL CheckSelected)
{
#ifdef DO_EXPORT
	// If we have the check selection flag on then see if this renderable node is:-
	// - selected or not = render it
	// - a child of the selection e.g. part of selected group where we are an item in the
	// 	 group and hence are not directly selected but still need to be exported
	// - a parent of the selected item e.g. selected inside item of group and we are at the
	//   group and hence need to include the group in the range 
	// Otherwise just return True as this is a renderable node and always needs to be exported
	// unless of course some node overrides this.
	if (CheckSelected)
		return (IsSelected() || IsChildOfSelected() || IsParentOfSelected());
	else
		return TRUE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	INT32 NodeRenderable::GetSizeOfExport(Filter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		pFilter - the export filter that will be used.
	Returns:	The number of nodes that will be exported (or an approximation for large
				objects, e.g. bitmaps).
	Purpose:	Find out how many nodes will be exported when this node is exported.
				The node can use the filter to find out how it will be exported, e.g.
				blends should return a value that reflects whether or not intermediate
				shapes will be saved out, etc.  Bitmaps and other large objects should
				return an approximation so that the progress display bar gives a smooth
				update.
				NB. This is virtual - the default implementation just returns 1 - only
					override if this is not accurate.
	SeeAlso:	Node::NeedsToExport; Node::ExportRender

********************************************************************************************/

INT32 NodeRenderable::GetSizeOfExport(Filter*)
{
	// Most nodes are just one node...
	return 1;
}

/********************************************************************************************

>	BOOL NodeRenderable::IsNodeRenderableClass()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Returns:	TRUE => This node is an instance of NodeRenderable or a derived class.
				FALSE => otherwise.
	Purpose:	Determine if a node is *derived* from the NodeRenderable class.
				NB. This has nothing to do with the node's renderable flag!
	SeeAlso:	Node::IsRenderable

********************************************************************************************/

BOOL NodeRenderable::IsNodeRenderableClass() const
{
	// We are a NodeRenderable, thanks.
	return TRUE;
}

/********************************************************************************************

>	BOOL NodeRenderable::ShouldBeRendered()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/07/95
	Returns:	TRUE => This node should be rendered
				FALSE => this node should not be rendered.
	Purpose:	Work out if the node should be rendered - basically all nodes except
				NodeRenderable nodes should be rendered.  However, some NodeRenderable
				derived classes should not actually be rendered - for example NodeMouldGroup
				hides the original un-moulded objects, which should definitely *not* be
				rendered.
				This is mainly used during printing where due to complications to do with
				banding and transparency, we cannot use NeedsToRender() to filter out
				such nodes, so we use this function instead.
				NB. This has nothing to do with the node's renderable flag!
	SeeAlso:	Node::IsNodeRenderable

********************************************************************************************/

BOOL NodeRenderable::ShouldBeRendered() const
{
	// By default we render nodes of this class, and derived nodes.
	return TRUE;
}


/********************************************************************************************

>	virtual void NodeRenderable::Transform( TransformBase& Transform )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/93
	Inputs:		Trans - The transform to apply to the objects
	Purpose:	This is the base class verison of the transform function.
				This is called to move, scale, rotate etc renderable objects.
				Each class derived from NodeRenderableInk should write a version
				of this function that manipulates all the control points in
				the object and updates its bounding box. See NodePath for an
				example.

********************************************************************************************/

void NodeRenderable::Transform( TransformBase& Trans )
{
	// By default, just transform all the children
	TransformChildren(Trans);
}



void NodeRenderable::TransformChildren(TransformBase& Trans)
{
	// We have to go through all the child nodes of this node and Transform
	// all the NodeRenderable objects

	Node* pCurrent = this -> FindFirstChild();

	while (pCurrent != NULL)
	{
		// Make sure it is a renderable ink object (not an attribute)
		if (pCurrent->IsKindOf( CC_RUNTIME_CLASS(NodeRenderable) ) )
			// Transform the child node
			((NodeRenderable*)pCurrent) -> Transform( Trans );

		// Show the hourglass
		if (ShowHourglass())												   
		{
			ContinueSlowJob(); 
		}


		// Find the next object in the group
		pCurrent = pCurrent -> FindNext();
	}
}

/********************************************************************************************

>	BOOL NodeRenderable::CanTransform()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Returns:	The base class always returns FALSE - i.e. NodeRenderables do not need to
				be transformed by default.
	Purpose:	Determine whether a renderable node can be transformed - if it does not,
				then no undo information needs to be stored for it.
	SeeAlso:	NodeRenderableInk::CanTransform

********************************************************************************************/

BOOL NodeRenderable::CanTransform()
{
	return FALSE;
}
                                   


/***********************************************************************************************
> Node* NodeRenderable::SimpleCopy()  

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	Inputs:     -  
    Outputs:    
    Returns:    A copy of the node, or NULL if memory has run out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	            
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL.                                                                      
                                                                                 
**********************************************************************************************/

Node* NodeRenderable::SimpleCopy()
{
	NodeRenderable* NodeCopy; 
	NodeCopy = new NodeRenderable();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);   
	return (NodeCopy);
}   


/***********************************************************************************************
>   void NodeRenderable::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeRenderable::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeRenderable), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeRenderable))
		CopyNodeContents(pNodeCopy);
}



/***********************************************************************************************
>   void NodeRenderable::CopyNodeContents(Node* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void NodeRenderable::CopyNodeContents(NodeRenderable* NodeCopy)
{               
	ENSURE(NodeCopy != NULL,"Trying to copy a node's contents into a NULL node"); 
	Node::CopyNodeContents(NodeCopy); 
}  

#if 1 //DEBUG_TREE

/********************************************************************************************

>	void* NodeRenderable::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/93
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node

********************************************************************************************/

     
void NodeRenderable::GetDebugDetails(StringBase* Str) 
{
	#if DEBUG_TREE
	Node::GetDebugDetails(Str);   
	#endif
}         

#endif

/********************************************************************************************

>	virtual void NodeRenderable::RenderFillBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/94
	Inputs:		pRender - The region to render the blobs into
	Purpose:	This function is the base class version of a blob rendering function, so it
				actually does nothing. The fill blobs are the blobs rendered by fills (eg
				the graduated fill should draw an arrow to show the start and end positions
				of the fill). A node that wants to render some fill blobs should write a
				replacement of this function.
	SeeAlso:	BlobManager

********************************************************************************************/

//void NodeRenderable::RenderFillBlobs(RenderRegion* pRender)
//{
//}



/********************************************************************************************

>	virtual void NodeRenderable::RenderEffectBlobs(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/2005
	Inputs:		pRender - The region to render the blobs into
	Purpose:	This function is the base class version of a blob rendering function, so it
				actually does nothing. The fill blobs are the blobs rendered by fills (eg
				the graduated fill should draw an arrow to show the start and end positions
				of the fill). A node that wants to render some fill blobs should write a
				replacement of this function.
	SeeAlso:	BlobManager

********************************************************************************************/

void NodeRenderable::RenderEffectBlobs(RenderRegion* pRender)
{
}



/********************************************************************************************

>	virtual void NodeRenderable::RenderArtisticBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/94
	Inputs:		pRender - The region to render the blobs into
	Purpose:	This function is the base class version of a blob rendering function, so it
				actually does nothing. Artistic blobs are used to display infomation about
				things such as pressure and other 'artistic' effects. If a node wants to
				render artistic blobs it should contain a version of this function
	SeeAlso:	BlobManager

********************************************************************************************/

void NodeRenderable::RenderArtisticBlobs(RenderRegion* pRender)
{
}




/********************************************************************************************

>	virtual void NodeRenderable::RenderObjectBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/94
	Inputs:		pRender - The region to render the blobs into
	Purpose:	This function is the base class version of a blob rendering function, so it
				actually does nothing. Object Blobs are the blobs that allow you to edit the
				shape of the node. For example, a path would display blobs on all its coords
				and a Rectangle may put a blob at each corner.
	SeeAlso:	BlobManager

********************************************************************************************/

void NodeRenderable::RenderObjectBlobs(RenderRegion* pRender)
{
}




/********************************************************************************************

>	virtual void NodeRenderable::RenderTinyBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/94
	Inputs:		pRender - The region to render the blobs into
	Purpose:	This function is the base class version of a blob rendering function, so it
				actually does nothing. Tiny blobs are displayed by each of the objects that
				is selected when in bounded region selection mode. Typically a Tiny Blob
				should be a single small blob.
	SeeAlso:	BlobManager

********************************************************************************************/

void NodeRenderable::RenderTinyBlobs(RenderRegion* pRender)
{
}



/********************************************************************************************

>	void NodeRenderable::RenderPenBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		pRender - the render region to draw your blobs into
	Purpose:	This function is the base class version of a blob rendering function, so it
				actually does nothing. Overide this function in each of the nodes that
				require Pen Blobs so draw what it needed.

********************************************************************************************/

void NodeRenderable::RenderPenBlobs(RenderRegion* pRender)
{
}



/********************************************************************************************

>	virtual void NodeRenderable::RenderToolObjectBlobs(RenderRegion* pRender)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26 March 2000
	Inputs:		pRender		the region to render the blobs into.

	Purpose:	This function is the base class version of a blob rendering function, so it
				actually does nothing.
				ToolObject blobs are displayed and used by a Node for interaction with a
				specific tool. They are similar to object blobs, but the big difference is
				that whereas object blobs are displayed regardless of the current tool
				(even if you can't use it to edit them), ToolObject blobs are not. The Node
				should always exercise final control over whether the blobs are active,
				depending on which tool is in use.

	SeeAlso:	BlobManager

********************************************************************************************/
void NodeRenderable::RenderToolObjectBlobs(RenderRegion* pRender)
{
}



/********************************************************************************************

>	virtual DocRect NodeRenderable::GetBlobBoundingRect()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/94
	Returns:	DocRect - The rectangle that contains all the nodes selection blobs.
	Purpose:	Calculates the bounding rectangle of the nodes blobs.This should always 
				be calculated on the fly as the view scale can change without the node 
				knowing, giving an incorrect result.

********************************************************************************************/

DocRect NodeRenderable::GetBlobBoundingRect()
{
	DocRect EmptyOne(0,0,0,0);
	return EmptyOne;
}

/********************************************************************************************

> 	virtual DocRect NodeRenderable::GetUnionBlobBoundingRect(BOOL bIncludeEffects = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/95
	Returns:	DocRect - The rectangle that contains all the nodes selection blobs.
	Purpose:	This simply returns the result of GetBlobBoundingRect() as the default action
				for the base class

********************************************************************************************/

DocRect NodeRenderable::GetUnionBlobBoundingRect(BOOL bIncludeEffects)
{
	return GetBlobBoundingRect();
}


/********************************************************************************************

>	virtual void NodeRenderable::Select(BOOL Redraw)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/94
	Inputs:		Redraw - TRUE if you want the blobs to be redrawn for you
	Purpose:	Selects the node

********************************************************************************************/

void NodeRenderable::Select(BOOL ReDraw)
{
	// Get the blob manager to draw some blobs if needed
	if (!IsSelected())
	{
		// It's possible that this node is inside a currently selected node
		// The rules of select inside say that a selected node cannot have
		// selected parents or children, so we must go deselect them.
		// Parents...
		Node* pNode = FindParent();
		while (pNode && pNode->IsNodeRenderableClass())
		{
			((NodeRenderable*)pNode)->DeSelect(ReDraw);
			pNode = pNode->FindParent();
		}

		// Children...
		// This node can only have selected children if it's marked as the parent
		// of selected children.
		if (IsParentOfSelected())
		{
			pNode = this->FindFirstDepthFirst();
			while (pNode)
			{
				if (pNode->IsNodeRenderableClass() && pNode!=this)
				{
					((NodeRenderable*)pNode)->DeSelect(ReDraw);
				}
				pNode = pNode->FindNextDepthFirst(this);
			}
		}

		SetSelected(TRUE);

		if (ReDraw)
		{
			// Find the blob manager
			BlobManager* BlobMgr = GetApplication()->GetBlobManager();
			ENSURE( BlobMgr!=NULL, "Blob Manager unexpected not there" );

			// Get it to render the blobs of this node
			BlobMgr->RenderMyBlobsOn(NULL, FindParentSpread(), this);

			AttrFillGeometry::LastRenderedMesh = NULL;
		}

		// Make this node selected and throw away the selection cache
		GetApplication()->UpdateSelection();
	}
}



/********************************************************************************************

>	virtual void NodeRenderable::DeSelect(BOOL ReDraw, BOOL bDeselectChildren)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/94
	Inputs:		ReDraw - TRUE if you want the blobs to be redrawn for you
	Purpose:	DeSelects the node

********************************************************************************************/

void NodeRenderable::DeSelect(BOOL ReDraw, BOOL bDeselectChildren)
{
	// Get the blob manager to draw some blobs if needed
	if (IsSelected())
	{
		if (ReDraw)
		{
			// Find the blob manager
			BlobManager* BlobMgr = GetApplication()->GetBlobManager();
			ENSURE( BlobMgr!=NULL, "Blob Manager unexpected not there" );

			// Get it to render the blobs of this node
			BlobMgr->RenderMyBlobsOff(NULL, FindParentSpread(), this);

			AttrFillGeometry::LastRenderedMesh = NULL;
		}

		// Make this node selected
		SetSelected(FALSE);
		GetApplication()->UpdateSelection();
	}
	else if (IsParentOfSelected() && bDeselectChildren)
	{
		Node* pNode = this->FindFirstDepthFirst();
		while (pNode)
		{
			if (pNode->IsNodeRenderableClass() && pNode!=this)
			{
				((NodeRenderable*)pNode)->DeSelect(ReDraw);
			}
			pNode = pNode->FindNextDepthFirst(this);
		}

		GetApplication()->UpdateSelection();
	}
}



/********************************************************************************************

>	virtual BOOL NodeRenderable::CopyComponentData(BaseDocument* SrcDoc, BaseDocument* NodesDoc) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/94
	Inputs:		SrcDoc:		The document from where this node was copied
				NodesDoc: 	The document where this node lives
	Outputs:	-
	Returns:	FALSE if the copy should be aborted, eg. the component data could not be
				copied accross because we ran out of memory.

	Purpose:	This virtual function will be called on the node after it has been copied from
				document SrcDoc. It should copy all neccessary DocComponent data from 
				SrcDoc to the document where the node lives (NodesDoc). 

	Errors:		-
	SeeAlso:	DocComponent::StartComponentCopy
	SeeAlso:	DocComponent::EndComponentCopy
	SeeAlso:	DocComponent::AbortComponentCopy


********************************************************************************************/

BOOL NodeRenderable::CopyComponentData(BaseDocument* SrcDoc,  BaseDocument* NodesDoc) 
{
	// Do nothing in the base class
	return TRUE;  
}		



/********************************************************************************************

>	virtual void NodeRenderable::TransformStretchObject(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/1999
	Inputs:		ExtParams		description of how this object should stretch.
	Outputs:	This node may be stretched, depending on the flags held within ExtParams.
	Purpose:	Perform a stretch on this Node, according to the contents of ExtParams.
				This function does nothing unless ExtParams uses X_STRETCH or Y_STRETCH.
	See also:	The Extender class.

********************************************************************************************/
void NodeRenderable::TransformStretchObject(const ExtendParams& ExtParams)
{
	// x-stretch - translate and stretch this Node.
	if (ExtParams.fExtendFlags & X_STRETCH)
	{
		Trans2DMatrix toOrigin(-ExtParams.doccScaleStart.x, 0);
		Matrix		matStretch( FIXED16( ExtParams.xscale ), FIXED16( 1.0 ) );
		Trans2DMatrix stretchX( matStretch );
		Trans2DMatrix fromOrigin(ExtParams.doccScaleEnd.x, 0);
		Transform(toOrigin);
		Transform(stretchX);
		Transform(fromOrigin);
	}

	// y-stretch - translate and stretch this Node.
	if (ExtParams.fExtendFlags & Y_STRETCH)
	{
		Trans2DMatrix toOrigin(0, -ExtParams.doccScaleStart.y);
		Matrix	matStretch( FIXED16(1.0), FIXED16(ExtParams.yscale) );
		Trans2DMatrix stretchY( matStretch );
		Trans2DMatrix fromOrigin(0, ExtParams.doccScaleEnd.y);
		Transform(toOrigin);
		Transform(stretchY);
		Transform(fromOrigin);
	}
}


													 
/********************************************************************************************

>	virtual void NodeRenderable::TransformTranslateObject(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/1999
	Inputs:		ExtParams		description of how this object should translate.
	Outputs:	This node may be translated, depending on the flags held within ExtParams.
	Purpose:	Translate this Node, according to the offsets and flags defined in ExtParams,
				and a reference centre, defined by NodeRenderable::FindExtendCentre().
				This function does nothing unless ExtParams uses X_EXTEND or Y_EXTEND.
	See also:	The Extender class; FindExtendCentre().

********************************************************************************************/
void NodeRenderable::TransformTranslateObject(const ExtendParams& ExtParams)
{
	DocCoord doccCentre = FindExtendCentre();

	// x-extension - we move the whole Node; behaviour determined by its centre.
	if (ExtParams.fExtendFlags & X_EXTEND)
	{
		doccCentre.x += ExtParams.doccOffset.x;

		// first, translate the object by the offset from start- to end- centres.
		Trans2DMatrix baseXoffset(ExtParams.doccOffset.x, 0);
		Transform(baseXoffset);

		// slight fudge here - we only move anything if we're outside a buffer-zone around
		// the centre (necessary as objects can be misaligned by a millipoint or two).
		if (doccCentre.x > ExtParams.doccEndCentre.x + ExtParams.xincExtendBuffer)
		{
			Trans2DMatrix translateX(ExtParams.xinc, 0);
			Transform(translateX);
		}
		else if (doccCentre.x < ExtParams.doccEndCentre.x - ExtParams.xdecExtendBuffer)
		{
			Trans2DMatrix translateX(-ExtParams.xdec, 0);
			Transform(translateX);
		}
	}

	// y-extension - we move the whole Node; behaviour determined by its centre.
	if (ExtParams.fExtendFlags & Y_EXTEND)
	{
		doccCentre.y += ExtParams.doccOffset.y;

		// first, translate the object by the offset from start- to end- centres.
		Trans2DMatrix baseYoffset(0, ExtParams.doccOffset.y);
		Transform(baseYoffset);

		// only extend if we're outside the centre's buffer-zone.
		if (doccCentre.y > ExtParams.doccEndCentre.y + ExtParams.yincExtendBuffer)
		{
			Trans2DMatrix translateY(0, ExtParams.yinc);
			Transform(translateY);
		}
		else if (doccCentre.y < ExtParams.doccEndCentre.y - ExtParams.ydecExtendBuffer)
		{
			Trans2DMatrix translateY(0, -ExtParams.ydec);
			Transform(translateY);
		}
	}
}



/********************************************************************************************

>	virtual void NodeRenderable::TransformTranslateNoStretchObject(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/1999
	Inputs:		ExtParams		description of how this object should translate/stretch.
	Outputs:	This node will be translated so that extend-centre matches up with where it
				would be if the node were stretched.
	Purpose:	Translate this node so that its extend-centre matches the position it would
				be in if the node were stretched. The node maintains its aspect ratio.
				This function does nothing unless ExtParams uses X_STRETCH or Y_STRETCH.
	See also:	The Extender class; FindExtendCentre().

********************************************************************************************/
void NodeRenderable::TransformTranslateNoStretchObject(const ExtendParams& ExtParams)
{
	DocCoord doccRef = FindExtendCentre();

	// x-stretch - we translate the Node by the same amount as doccRef under stretching.
	if (ExtParams.fExtendFlags & X_STRETCH)
	{
		Trans2DMatrix transX(	ExtParams.doccScaleEnd.x -
								ExtParams.doccScaleStart.x, 0	);
		Transform(transX);

		// extra translation is difference between stretched and unstretched pDoccRef.
		INT32 deltaX = (INT32)(	(ExtParams.xscale - 1.0) *
							(doccRef.x - ExtParams.doccScaleStart.x) );
		Trans2DMatrix trans2X(deltaX, 0);
		Transform(trans2X);
	}

	// y-stretch - we translate the Node by the same amount as doccRef under stretching.
	if (ExtParams.fExtendFlags & Y_STRETCH)
	{
		Trans2DMatrix transY(0, ExtParams.doccScaleEnd.y -
								ExtParams.doccScaleStart.y);
		Transform(transY);

		// extra translation is difference between stretched and unstretched pDoccRef.
		INT32 deltaY = (INT32)(	(ExtParams.yscale - 1.0) *
							(doccRef.y - ExtParams.doccScaleStart.y) );
		Trans2DMatrix trans2Y(0, deltaY);
		Transform(trans2Y);
	}
}



//-------------------------------------------------------------------------------------------
// NodeRenderableBounded methods



/********************************************************************************************

>	NodeRenderableBounded::NodeRenderableBounded() 

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Purpose:	Default constructor

********************************************************************************************/

NodeRenderableBounded::NodeRenderableBounded():NodeRenderable()   
{     
	IsBoundingRectValid = FALSE;
	Magnetic			= FALSE;
	MayBeCached			= FALSE;	// Definitely not cached
}


/***********************************************************************************************

>   NodeRenderableBounded::NodeRenderableBounded( Node* ContextNode, AttachNodeDirection Direction,  
									BOOL Locked = FALSE, BOOL Mangled = FALSE, BOOL Marked = FALSE, 
									BOOL Selected = FALSE)

    Author: Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:26/4/93                 
    Inputs: ContextNode: Pointer to a node which this node is to be attached to.     
			Direction: Specifies the direction in which the node is to be attached to the 
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
    Purpose: This method initialises the node and links it to ContextNode in the
			 direction specified by Direction. All necessary tree links are
			 updated. 
    Errors: An assertion error will occur if ContextNode is NULL

***********************************************************************************************/

NodeRenderableBounded::NodeRenderableBounded(Node* ContextNode, AttachNodeDirection Direction,
											 BOOL Locked, BOOL Mangled, BOOL Marked, BOOL Selected    
			  ): NodeRenderable(ContextNode, Direction, Locked, Mangled, Marked, Selected)
{                         
	IsBoundingRectValid = FALSE;
	Magnetic 			= FALSE;
	Dragged				= FALSE;
	MayBeCached			= FALSE;	// Definitely not cached
}


/*********************************************************************************************

>    NodeRenderableBounded::~NodeRenderableBounded() 

     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   25/06/2004
     Inputs:    -
     Outputs:   
     Returns:   -
			  
     Purpose:	Since this node is dieing make sure that any cached data associated with it
				is also removed...
			
     Errors:    

**********************************************************************************************/
			 
NodeRenderableBounded::~NodeRenderableBounded()
{
	if (MayBeCached)
	{
		// There could be some cached bitmaps associated with this node
		CBitmapCacheKey mykey(this, 42);
		CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
		if (pBitmapCache)
		{
			// Get rid of EVERYTHING - we are dying!
			pBitmapCache->RemoveAllOwnedBitmaps(mykey, FALSE, CACHEPRIORITY_PERMANENT);
		}
	}
}


/********************************************************************************************

>	void NodeRenderableBounded::Transform(TransformBase& Trans)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/94
	Inputs:		Trans - The transform
	Purpose:	Base class transform function.

********************************************************************************************/

void NodeRenderableBounded::Transform(TransformBase& Trans)
{
	// Just transform the children and invalidate the nodes bounding rect
	InvalidateBoundingRect();
	TransformChildren(Trans);
}



/********************************************************************************************

>	void NodeRenderableBounded::TransformChildren(TransformBase& Trans)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/94
	Inputs:		Trans - The transform that should be applied to all the children
	Purpose:	Transforms the Child nodes of this node. This allows fills etc to move
				with the objects.

********************************************************************************************/

void NodeRenderableBounded::TransformChildren(TransformBase& Trans)
{
	// We have to go through all the child nodes of this node and Transform
	// all the NodeRenderable objects
	Node* pCurrent = this -> FindFirstChild();

	while (pCurrent != NULL)
	{
		// Make sure it is a renderable ink object (not a hidden node)
		if (pCurrent->IS_KIND_OF(NodeRenderable))
		{
			// Transform the child node
			((NodeRenderable*)pCurrent)->Transform( Trans );

			// If this node has a bounding box then invalidate it
			if (pCurrent->IsBounded())
			{
				((NodeRenderableBounded*)pCurrent)->InvalidateBoundingRect();
			}
			
			// Show the hourglass
			if (ShowHourglass())
			{
				ContinueSlowJob(); 
			}
		}

		// Find the next object in the group
		pCurrent = pCurrent -> FindNext();
	}
}



/********************************************************************************************

>	virtual DocRect NodeRenderableBounded::GetBoundingRect(BOOL DontUseAttrs=FALSE,
															BOOL HitTest=FALSE)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/94
	Inputs:		DontUseAttrs - TRUE if you want to ignore all the nodes attributes
				FALSE by default.
				HitTest      - TRUE if being called during HitTest

	Returns:	The nodes bounding rectangle

	Purpose:	returns the nodes bounding rectangle. If the rectangle is valid the
				rectangle is simply returned, but is IsBoundingRectValid is FALSE (cos
				someone called InvaldateBoundingRect() on this node, or one of its
				children) then the bounding rect is first calculated and the flag reset,
				before it is returned. This is the base class version of this function
				and will build a bounding box out of the union of the nodes childrens
				bounding boxes. Your node MUST replace this function if this is not what you
				want it to do....

********************************************************************************************/

DocRect	NodeRenderableBounded::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	// if the bounding rect of this node is not valid then fill in something
	if (!IsBoundingRectValid || DontUseAttrs)
	{
		// just set it to be an empty rectangle
		DocRect BoundRect(0,0,0,0);
		
		Node* pNode = FindFirstChild();
		while (pNode!=NULL)
		{
			// Add in the bounding rect of this node with all the others
			if (pNode->IsBounded())
				BoundRect = BoundRect.Union(((NodeRenderableBounded*)pNode)->GetBoundingRect(DontUseAttrs));

			// And find the next node
			pNode = pNode->FindNext();
		}

		if (DontUseAttrs)
		{
			// We do not want to update the cached bounding rect here
			return BoundRect;
		}

		// Copy the unions into the nodes bounding rect param
		BoundingRectangle = BoundRect;

		// mark the bounding rect as valid
		IsBoundingRectValid = TRUE;
	}

	// and return the bounding rect
	return BoundingRectangle;
}

/********************************************************************************************

> 	virtual DocRect NodeRenderableBounded::GetUnionBlobBoundingRect(BOOL bIncludeEffects = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/95
	Returns:	DocRect - The rectangle that is the union of all the nodes selection blobs
						  and the node's bounding rectangle
	Purpose:	This simply returns the result of GetBlobBoundingRect() as the default action
				for the base class

********************************************************************************************/

DocRect NodeRenderableBounded::GetUnionBlobBoundingRect(BOOL bIncludeEffects)
{
	DocRect drBounds = GetBoundingRect().Union(GetBlobBoundingRect());

	if (!bIncludeEffects)
		return drBounds;

	// Search up the effects stack
//	NodeRenderableBounded* pEffect = this;
	Node* pParent = FindParent();
	while (pParent && !pParent->IsLayer())
	{
		if (pParent->IsEffect())
			drBounds = ((NodeEffect*)pParent)->GetBoundingRect().Union(((NodeEffect*)pParent)->GetBlobBoundingRect());

		pParent = pParent->FindParent();
	}

	return drBounds;
}


/********************************************************************************************

>	void NodeRenderableBounded::ValidateBoundingRect()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/94
	Purpose:	Marks this nodes bounding rect as valid again. You should not generally
				speaking be using this function. It was created so that new nodes could
				be sure of invalidating their parents bounding rects. DoInsertNewNode()
				is the only real case in which it can be used without express written
				permission from the UN.

********************************************************************************************/

void NodeRenderableBounded::ValidateBoundingRect()
{
	IsBoundingRectValid = TRUE;
}



/********************************************************************************************

>	void NodeRenderableBounded::InvalidateBoundingRectHelper()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/94
	Scope:		private
	Purpose:	Marks the bounding rect of this node as invalid, as well as marking this nodes
				parents as invalid as well. Next time a nodes GetBoundingRect() function is
				called the bounding rect will be recalculated if needed.

				This is the recursive helper function for NodeRenderableBounded::InvalidateBoundingRect


	SeeAlso		NodeRenderableBounded::InvalidateBoundingRect

********************************************************************************************/

void NodeRenderableBounded::InvalidateBoundingRectHelper()
{
	// Invalid this nodes bounding rect if we need to
	BoundingRectangle = DocRect(0,0,0,0);
	if (IsBoundingRectValid)
	{
		IsBoundingRectValid = FALSE;

		// and invalid this nodes parents bounding rect
		Node* pParent = FindParent();
		if (pParent!=NULL)
		{
			// Check that the tree is as expected
			ENSURE(pParent->IS_KIND_OF(NodeRenderableBounded), "Parent of a bounded node was not bounded");

			// sort of recurse up the tree invalidating the bounding rects as we go
			if (pParent->IsBounded())
				((NodeRenderableBounded*)pParent)->InvalidateBoundingRectHelper();
		}
	}
}

/********************************************************************************************

>	virtual void NodeRenderableBounded::InvalidateBoundingRect(BOOL InvalidateChildBounds = FALSE)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>	/ Recursion moved to new function by Simon
	Created:	11/9/94
	Inputs:		InvalidateChildBounds - When TRUE all children of the node have their
				bounds invalidated as well (eg. applying a line attribute to a group)

	Purpose:	Marks the bounding rect of this node as invalid, as well as marking this nodes
				parents as invalid as well. Next time a nodes GetBoundingRect() function is
				called the bounding rect will be recalculated if needed.

********************************************************************************************/

void NodeRenderableBounded::InvalidateBoundingRect(BOOL InvalidateChildBounds /* = FALSE */)
{
	// Invalid this nodes bounding rect if we need to
	if (IsBoundingRectValid)
	{
		InvalidateBoundingRectHelper();
		
		// Removed by Simon (31/03/95) I Can't see a reason to update the selection here !
		// Inform the range that it's invalid
		//SelRange* Selected = GetApplication()->FindSelection();
		//ERROR3IF(Selected == NULL, "Where's the selection gone ?"); 
		//Selected->UpdateBounds();

		if (InvalidateChildBounds)
		{
			// Invalidate the bounds of all children of the node
			Node* Current = FindFirstDepthFirst();
			while (Current != this)
			{
				ERROR3IF (Current == NULL, "invalid NULL Current");
				if (Current->IsBounded())
				{
					if (((NodeRenderableBounded*)Current)->IsBoundingRectValid)
					{
						((NodeRenderableBounded*)Current)->IsBoundingRectValid = FALSE;  
					}
				}
				Current = Current->FindNextDepthFirst(this); 
			} 
		}

	}
}



/********************************************************************************************

>	virtual DocRect NodeRenderableBounded::GetEffectStackBounds(BOOL* pbFoundEffects = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/2005
	Inputs:		-
	Returns:	rectangle containing bounds of topmost effect above this node
	Purpose:	Find the bounds of any effects which might be affected by changes
				to this node

********************************************************************************************/

DocRect NodeRenderableBounded::GetEffectStackBounds(BOOL* pbFoundEffects)
{
	DocRect drBounds = GetBoundingRect();

	if (pbFoundEffects)
		*pbFoundEffects = FALSE;

	Node* pParent = FindParent();
	while (pParent && !pParent->IsLayer())
	{
		if (pParent->IsEffect())
		{
			drBounds = ((NodeEffect*)pParent)->GetBoundingRect();
			if (pbFoundEffects)
				*pbFoundEffects = TRUE;
		}

		pParent = pParent->FindParent();
	}

	return drBounds;
}




/********************************************************************************************

>	BOOL NodeRenderableBounded::NeedsToRender( RenderRegion *pRender )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/94
	Inputs:		pRender - A pointer to the current render region (NULL if none)
	Returns:	TRUE  => This node should be rendered,
				FALSE => This node does not need to be rendered.
	Purpose:	Virtual function - this function will indicate to the caller whether or not
				we want to render the given node, according to the information passed in.
	SeeAlso:	Node::NeedsToRender

********************************************************************************************/

SubtreeRenderState NodeRenderableBounded::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
	if (pRender == NULL)							// If no render region supplied, assume we need to be rendered
		return SUBTREE_ROOTANDCHILDREN;

	// Go find out about my bounding rectangle
	DocRect Bounds = GetBoundingRect();
	DocRect ClipRect = pRender->GetClipRect();

	if (bClip && !ClipRect.IsIntersectedWith(Bounds))	// If not within the clipping rect then
		return SUBTREE_NORENDER;					// Don't render us or our children

//	if (RenderCached(pRender, ppNextNode))			// If we can find a cached bitmap for this node and render it
//		return SUBTREE_NORENDER;					// Then tell the renderer to move on without doing any more...

	return SUBTREE_ROOTANDCHILDREN;					// Else we must render ourselves and our children
}



/********************************************************************************************

>	BOOL NodeRenderableBounded::IsIntersectedWith( DocRect* pClipRect )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/94
	Inputs:		pClipRect
	Returns:	TRUE if this node is intersected witht he doc rect
	Purpose:	Determine if this node is intersected with the rectangle supplied.

********************************************************************************************/

BOOL NodeRenderableBounded::IsIntersectedWith( DocRect* pClipRect )
{
	// Go find out about my bounding rectangle
	DocRect Bounds = GetBoundingRect();

	// and return if we intersect
	return( pClipRect->IsIntersectedWith(Bounds));
}


/********************************************************************************************

>	virtual BOOL NodeRenderableBounded::IsBounded() const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/94
	Returns:	TRUE
	Purpose:	This function will return TRUE when called on NodeRenderableBounded nodes
				and those derived from it.
	SeeAlso:	Node::IsBounded

********************************************************************************************/

/*BOOL NodeRenderableBounded::IsBounded() const
{
	return TRUE;
}*/


/********************************************************************************************

>	BOOL NodeRenderableBounded::CanTakeAttributeType(CCRuntimeClass *pAttrClass)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/08/94
	Inputs:		pAttrClass - specifies the type of attribute being enquired about.
	Returns:	TRUE => object supports this kind of attribute.
				FALSE => object does not support this kind of attribute, and so it
						 should not be applied.
	Purpose:	Determine whether or not a bounded node can take a particular kind of
				attribute.  This is used for various objects, e.g. bitmap objects use it
				to accept transparent fill geometries but not normal ones, because they
				are always filled with the bitmap.
				Base class version always returns TRUE so by default an object supports
				all attribute types.

********************************************************************************************/

BOOL NodeRenderableBounded::CanTakeAttributeType(CCRuntimeClass*)
{
	return TRUE;
}

/***********************************************************************************************
> Node* NodeRenderableBounded::SimpleCopy()  

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
    Returns:    A copy of the node, or NULL if memory has run out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL.                                                                      
                                                                                 
**********************************************************************************************/

Node* NodeRenderableBounded::SimpleCopy()
{
	NodeRenderableBounded* NodeCopy; 
	NodeCopy = new NodeRenderableBounded();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);   
	return (NodeCopy);
}   



/***********************************************************************************************
>   void NodeRenderableBounded::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeRenderableBounded::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(pNodeCopy->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)), "PolyCopyNodeContents given wrong dest node type");

	if (pNodeCopy->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)))
		CopyNodeContents((NodeRenderableBounded*)pNodeCopy);
}



/***********************************************************************************************
>   void NodeRenderableBounded::CopyNodeContents(Node* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeRenderableBounded::CopyNodeContents(NodeRenderableBounded* NodeCopy)
{               
	ENSURE(NodeCopy != NULL,"Trying to copy a node's contents into a NULL node"); 
	NodeRenderable::CopyNodeContents(NodeCopy); 

	// Node Bounded stuff
	NodeCopy->BoundingRectangle 	= BoundingRectangle;
	NodeCopy->IsBoundingRectValid 	= IsBoundingRectValid;

	// The magnetic flag
	NodeCopy->Magnetic 				= Magnetic;
	NodeCopy->Dragged				= Dragged;
}  

/********************************************************************************************

>	void* NodeRenderableBounded::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/93
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node

********************************************************************************************/

void NodeRenderableBounded::GetDebugDetails(StringBase* Str) 
{
#ifdef _DEBUG
	NodeRenderable::GetDebugDetails(Str);   
	String_256 TempStr; 

	// Find out about the node up to date bounding rect
	DocRect Bounds = GetBoundingRect();
	if (!Bounds.IsValid())
	{ 
		TempStr = TEXT("\r\nBounding Rectangle = *INVALID*\r\n"); 
	}
	else
		TempStr._MakeMsg(TEXT("\r\nBounding Rectangle\r\n   Low(#1%ld, #2%ld)\r\n   High(#3%ld, #4%ld)\r\n"),    
							  Bounds.LowCorner().x,  
							  Bounds.LowCorner().y,
							  Bounds.HighCorner().x,  
							  Bounds.HighCorner().y); 
	(*Str)+=TempStr; 
#endif
}         

/********************************************************************************************

>	void NodeRenderableBounded::IncludeChildrensBoundingRects(DocRect* BoundingRect) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94
	Outputs:	DocRect is updated to include Childrens Bounding Rects.
	Purpose:	The child attribute Blob bounding rects are included in the BoundingRect.
	Note:		re-written by Ed 25/4/95
				Now only includes bounds of child attrs (rather than all NodeRenderableBounded)
				The assumption is that no child can be selected if it's parent is, so attr
				blobs of child nodes cannot be visible at the same time as their parents!

				re-written by Will 5/5/95
				Now uses 'FindAppliedAttributes' so it works for select inside.

********************************************************************************************/
     
void NodeRenderableBounded::IncludeChildrensBoundingRects(DocRect* pBounds) 
{
	// Ok, so this function should be defined in NodeRenderableInk, but that is
	// one heck of a change, so we'll just make sure we warn people if they try
	// something silly.
	ERROR3IF(!this->IS_KIND_OF(NodeRenderableInk), "IncludeChildrensBoundingRect called for non-ink node");
	
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the blob Manager
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr==NULL)
		return;

	// Since the only blobs that attributes have are Fill Blobs,
	// don't bother if the fill blobs are not visible
	BlobStyle VisibleBlobs = pBlobMgr->GetCurrentInterest(TRUE);
	if (VisibleBlobs.Fill)
	{
		CCAttrMap* pAttribMap = new CCAttrMap(30);
		if (pAttribMap == NULL)
			return;

		// Now find any attributes that are applied to this node.
		BOOL FoundAttrs = ((NodeRenderableInk*)this)->FindAppliedAttributes(pAttribMap, 5000, NULL, FALSE);

		if (FoundAttrs)
		{
			// iterating all (key, value) pairs
			for (CCAttrMap::iterator Pos = pAttribMap->GetStartPosition(); Pos != pAttribMap->GetEndPosition();)
			{
				void *pVal;
				CCRuntimeClass *pType;
				pAttribMap->GetNextAssoc(Pos,pType,pVal);

				NodeAttribute* pAttr = (NodeAttribute*)pVal;
				*pBounds=pBounds->Union( pAttr->GetBlobBoundingRect() );
			}
		}

		delete pAttribMap;
	}
#endif
}         

/***********************************************************************************************

> BOOL NodeRenderableBounded::Snap(DocCoord* pDocCoord)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    20/9/94
	Inputs:     pDocCoord = a coord in Spread coords
    Outputs:    
    Returns:    TRUE 	- the DocCoord has been snapped to the path.
    			FALSE	- the DocCoord has not been processed.
    			                                  		 
    Purpose:    ALWAYS RETURNS FALSE!
				This is just a virtual base function intended to be overridden.
    Errors:        
	Scope:      public
	       
**********************************************************************************************/

BOOL NodeRenderableBounded::Snap(DocCoord* pDocCoord)
{
	return (FALSE);
}

/***********************************************************************************************

> BOOL NodeRenderableBounded::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    21/9/94
	Inputs:     pDocCoord	- the rectangle to snap
				StartDrag	- Start coord of drag
				EndDrag		- End coord of drag
    Outputs:    
    Returns:    TRUE 	- the DocRect been snapped to the grid.
    			FALSE	- the DocRect has not been processed.
    			                                  		 
    Purpose:    ALWAYS RETURNS FALSE!
				This is just a virtual base function intended to be overridden.
	Scope:      public
	       
**********************************************************************************************/

BOOL NodeRenderableBounded::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
	return (FALSE);
}



/********************************************************************************************

>	BOOL NodeRenderableBounded::SnapToCoords(DocCoord* pDocCoord)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		pDocCoord - The DocCoord that needs snapping
	Outputs:	If the function returns TRUE then pDocCoord will have been modified to
				snap to a significant point on the node.
	Returns:	TRUE if the Coord was snapped.
				FALSE if the coord was not effected by this node
	Purpose:	This is a form of Magnetic snapping. It Snaps the Coordinates to the
				Significant points around a shape. eg. It would snap to the EndPoints along
				a path, the centre and corners of a rectangle etc.
				This snapping is more powerful than the normal magnetic snapping, so that
				these significant points can be snapped to more easily.
				This is the base class version of the function and does no snapping at all.
				It always returns FALSE. If you want your node to make use of this sort
				of snapping, override this function and use the MagneticPointRadius to
				decide if the DocCoord is close enough to be snapped.
	SeeAlso:	MagneticPointRadius; MagneticLineRadius; NodePath::SnapToCoords

********************************************************************************************/

BOOL NodeRenderableBounded::SnapToCoords(DocCoord* pDocCoord)
{
	return FALSE;
}


/********************************************************************************************

>	BOOL NodeRenderableBounded::Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Declare the preferences for the magnetic options.
				Now also declares the layers section rather than having a new explicit
				init in the layers section. 

********************************************************************************************/

BOOL NodeRenderableBounded::Init()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Declare the section that the magnetic options will live in
	BOOL Worked = Camelot.DeclareSection(TEXT("Magnetic Options"), 2);

	// declare the 2 prefs that there are
	if (Worked)
		Worked = Camelot.DeclarePref(TEXT("Magnetic Options"), TEXT("Point Radius"),
									&MagneticPointRadius, 0);

	if (Worked)
		Worked = Camelot.DeclarePref(TEXT("Magnetic Options"), TEXT("Line Radius"),
									&MagneticLineRadius, 0);

	// Declare the section that the preferences for layers will live in
	if (Worked)
		Worked = Camelot.DeclareSection(TEXT("Layers"), 2);

	// declare the 2 prefs that there are
	if (Worked)
		Worked = Camelot.DeclarePref(TEXT("Layers"), TEXT("ActiveLayerVisibleAndEditable"),
									&Layer::ActiveLayerVisibleAndEditable, 0, 1);

	if (Worked)
		Worked = Camelot.DeclarePref(TEXT("Layers"), TEXT("EnableLayerCacheing"),
									&Layer::EnableLayerCacheing, 0, 2);

	// Declare the section that the preferences for layers will live in
	if (Worked)
		Worked = Camelot.DeclareSection(TEXT("Rendering"), 4);

	if (Worked)
		Worked = Camelot.DeclarePref(TEXT("Rendering"), TEXT("Cacheing"),
									&bEnableCacheing, 0);

	if (Worked)
		Worked = Camelot.DeclarePref(TEXT("Rendering"), TEXT("ThrottleCacheing"),
									&bThrottleCacheing, 0);

	if (Worked)
		Worked = Camelot.DeclarePref(TEXT("Rendering"), TEXT("ShowCacheBlobs"),
									&bShowCacheBlobs, 0);

	// Return if it worked or not
	return Worked;
#else
	return TRUE;
#endif
}



/********************************************************************************************

>	BOOL NodeRenderableBounded::IsMagneticallyClose(DocCoord* pTestCoord, DocCoord* pOriginalCoord)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		pTestCoord - the coord we are testing the original against
				pOriginalCoord - The Coord that we are trying to snap to the test coord
	Outputs:	pOriginalCoord - If the 2 coords are close enough together (as defined by
				the MagneticPointRadius) then pOriginalCoord is changed to match pTestCoord
	Returns:	TRUE if it snapped the original coord to the test coord
	Purpose:	Tests to see if the 2 coordinates are close enough together to qualify as
				Magnetically close to each other. If they are, then pOriginalCoord is
				Modified to match pTestCoord and TRUE id returned. If not, then all the
				coords are left alone and FALSE is returned

********************************************************************************************/

BOOL NodeRenderableBounded::IsMagneticallyClose(DocCoord* pTest, DocCoord* pOriginal)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find out if anything is close to this coord
	if ((abs(pTest->x - pOriginal->x) < CSnap::GetSnapToCoordDist()) &&
		(abs(pTest->y - pOriginal->y) < CSnap::GetSnapToCoordDist()))
	{
		// They are close to each other
		// Snap the original coord to the test coord
		pOriginal->x = pTest->x;
		pOriginal->y = pTest->y;

		// return true to indicate what has happened
		return TRUE;
	}
#endif
	// They were not close to each other, so say so
	return FALSE;
}



/***********************************************************************************************

> BOOL NodeRenderableBounded::IsMagnetic()

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    22/9/94
	Inputs:     -
    Outputs:    -
    Returns:    TRUE 	- the node is magnetic
    			FALSE	- the node is not magnetic
    			                                  		 
    Purpose:    Returns that magnetic state of this node
	Scope:      public
	       
**********************************************************************************************/

BOOL NodeRenderableBounded::IsMagnetic()
{
	return Magnetic;
}
													 

/***********************************************************************************************

> void NodeRenderableBounded::SetMagneticState(BOOL state)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    22/9/94
	Inputs:     State - TRUE to become magnetic, FALSE otherwise
    Outputs:    -
    Returns:    -                                  		 
    Purpose:    Allows you to set the magnetic state of this node
	Scope:      public
	       
**********************************************************************************************/

void NodeRenderableBounded::SetMagneticState(BOOL state)
{
	Magnetic = state;
}


/***********************************************************************************************

> BOOL NodeRenderable::IsBeingDragged() const

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    12/12/2003
	Inputs:     -
    Outputs:    -
    Returns:    TRUE 	- the node is being dragged
    			                                  		 
    Purpose:    Returns a flag saying whether the node is currently being dragged or not
	Scope:      public
	       
**********************************************************************************************/

BOOL NodeRenderable::IsDragged() const
{
	return Dragged;
}
													 

/***********************************************************************************************

> void NodeRenderable::SetDraggedState(BOOL bNewState, BOOL bAndChildren)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    12/12/2003
	Inputs:     NewState - TRUE to become magnetic, FALSE otherwise
				bAndChildren - Recursively set all child nodes the same way
    Outputs:    -
    Returns:    -                                  		 
    Purpose:    Allows you to set the dragged state of this node
	Scope:      public
	       
**********************************************************************************************/

void NodeRenderable::SetDraggedState(BOOL bNewState, BOOL bAndChildren)
{
	Dragged = bNewState;
	if (bAndChildren)
	{
		NodeRenderable* pNode = (NodeRenderable*)FindFirstChild(CC_RUNTIME_CLASS(NodeRenderable));
		while (pNode)
		{
			pNode->SetDraggedState(bNewState, bAndChildren);
			pNode = (NodeRenderable*)pNode->FindNext(CC_RUNTIME_CLASS(NodeRenderable));
		}
	}
}


/***********************************************************************************************

> double NodeRenderableBounded::GetRotationAngle()

    Author:     Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
    Created:    24/1/95
	Inputs:     -
    Outputs:    -
    Returns:    The angle of rotation of the object.  This ranges from PI to -PI.  Zero is on
    			the positive x-axis.  (i.e. what atan2 returns)                                  		 
    Purpose:    Call this function to obtain the current angle of rotation of the node.  By
				default this is zero.  Nodes derived from NodeRenderableBounded should override
				this function and return the angle of rotation using their own particular method
	Scope:      public
	       
**********************************************************************************************/

double NodeRenderableBounded::GetRotationAngle()
{
	return 0.0;
}



/********************************************************************************************

>	virtual BOOL NodeRenderableBounded::IsDifferent(Node *pOther)

 	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/97

	Inputs:		pOther - The node to compare this one to

	Returns:	TRUE if this is considered different from pOther,
				FALSE if they are the same

	Purpose:	Determine if 2 nodes are considered different.

	Notes:		**** IMPORTANT - May not function well enough for your needs! ****

				This was written to allow the StrokeComponent class to merge stroke
				definitions which share identical clipart subtrees. Stroke subtrees
				are special in that they have had Make Shapes applied to them, and
				so only contain paths. Hence, I have only defined functions in
				Node, NodeRenderableBounded, and NodePath - most objects in the tree
				thus use base class functionality (are they of the same class, and
				do they have identical bounding boxes). This suffices for my needs,
				but you may need to implement this function for a lot more node
				types before it's of use to you.

********************************************************************************************/

BOOL NodeRenderableBounded::IsDifferent(Node *pOther)
{
	// First, check with the base class - this checks the classes are the same type
	if (NodeRenderable::IsDifferent(pOther))
		return(TRUE);

	ERROR3IF(GetRuntimeClass() != pOther->GetRuntimeClass(),
			"Node::IsDifferent base class method seems to have been broken");

	// Now check if the bounds of the nodes are the same
	if (BoundingRectangle != ((NodeRenderableBounded *)pOther)->BoundingRectangle)
		return(TRUE);

	return(FALSE);
}



/********************************************************************************************

>	virtual DocRect NodeRenderableBounded::GetExtendTargetBounds(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/2000
	Inputs:		ExtParams		const ref to an ExtendParams class describing the extension.
	Outputs:	
	Returns:	The DocRect to treat as the bounding rect of this node when extending.
	Purpose:	Return a DocRect which contains the bounds of this node as defined and
				required by the Extending mechanism.

				We treat x- and y- directions separately, and we always return our true
				bounds, unless we're extending in a direction, in which case we return the
				bounds of our extend control points. Default behaviour for this is to
				use the 'bounds' of our FindExtendCentre() coord.
	Errors:		
	See also:	SliceHelper::BoundingNodeSize().

********************************************************************************************/
DocRect NodeRenderableBounded::GetExtendTargetBounds(const ExtendParams& ExtParams)
{
	DocRect		drBounds	= GetBoundingRect();
	DocCoord	doccCentre	= FindExtendCentre();
	if (ExtParams.fExtendFlags & X_EXTEND)
		drBounds.lo.x = drBounds.hi.x = doccCentre.x;
	if (ExtParams.fExtendFlags & Y_EXTEND)
		drBounds.lo.y = drBounds.hi.y = doccCentre.y;

	return drBounds;
}



//----------------------------------------------------------------------------------------------
// NodeRenderableInk methods




/*********************************************************************************************

>	static BOOL NodeRenderableInk::Init()

     Author:    Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
     Created:   21/11/93
     Inputs:    -
     Outputs:   
     Returns:   TRUE if worked (in fact cannot fail).
     Purpose:	Reads preferences for NodeRenderableInk.
     Errors:    None

**********************************************************************************************/

BOOL NodeRenderableInk::Init()
{
#if !defined(EXCLUDE_FROM_RALPH)
	BOOL bOK = TRUE;
	bOK = bOK && Camelot.DeclareSection(TEXT("DebugFlags"), 10)
			  && Camelot.DeclarePref(TEXT("DebugFlags"), TEXT("SmartClicks"), &bUseSmartClicks, FALSE, TRUE)
			  && Camelot.DeclareSection(TEXT("Rendering"), 10)
			  && Camelot.DeclarePref(TEXT("Rendering"), TEXT("EyeDropperAverage"), &AverageHitColour, FALSE, TRUE);

	return bOK;
#else
	return TRUE;
#endif
}
			 
 
/*********************************************************************************************

>    NodeRenderableInk::NodeRenderableInk() 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:    -
     Outputs:   
     Returns:   -
			  
     Purpose: This constructor creates a NodeRenderableInk linked to no other, with all status
			  flags false, and an uninitialised bounding rectangle.           
			
     Errors:    

**********************************************************************************************/
			 
NodeRenderableInk::NodeRenderableInk(): NodeRenderableBounded()
{
	SafeToRender = FALSE;	// Not safe to progressively render during importing yet (see ReadPostChildren...)
	MayBeCached = FALSE;	// Definitely not cached
}

/***********************************************************************************************

>   void NodeRenderableInk::NodeRenderableInk(
	Node* ContextNode,  
	AttachNodeDirection Direction,  
	BOOL Locked = FALSE, 
	BOOL Mangled = FALSE,  
	BOOL Marked = FALSE, 
	BOOL Selected = FALSE, 
	)

    Author: Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:26/4/93             
    
    Inputs: ContextNode: Pointer to a node which this node is to be attached to.     
    
			Direction: 
			
				Specifies the direction in which this node is to be attached to the 
				ContextNode. The values this variable can take are as follows: 
								  
				PREV      : Attach node as a previous sibling of the context node
				NEXT      : Attach node as a next sibling of the context node
				FIRSTCHILD: Attach node as the first child of the context node
				LASTCHILD : Attach node as a last child of the context node                               
					      
			The remaining inputs specify the status of the node: 
			
			Locked:         Is node locked ?
			Mangled:    Is node mangled ?
			Marked:     Is node marked ?
			Selected:   Is node selected ?
			
    Outputs:   -
    Returns:   - 
    Purpose: This method initialises the node and links it to ContextNode in the
			 direction specified by Direction. All necessary tree links are
			 updated.                                               
			
    Errors: An assertion error will occur if ContextNode is NULL

***********************************************************************************************/

NodeRenderableInk::NodeRenderableInk(Node* ContextNode,  
									 AttachNodeDirection Direction,  
									 BOOL Locked, 
									 BOOL Mangled,  
									 BOOL Marked, 
									 BOOL Selected    
			  						):
NodeRenderableBounded(ContextNode, Direction, Locked, Mangled, Marked,Selected)
{
	SafeToRender = FALSE;	// Not safe to progressively render during importing yet (see ReadPostChildren...)
	MayBeCached = FALSE;	// Definitely not cached
} 


/*********************************************************************************************

>    virtual BOOL NodeRenderableInk::IsAnObject(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is an object i.e. a NodeRenderableInk
     Purpose:   For determining quickly if the node is an object
     Errors:    

**********************************************************************************************/
       
BOOL NodeRenderableInk::IsAnObject() const
{ 
	return TRUE; 	 // Cos it is
}



/*********************************************************************************************
>    virtual BOOL NodeRenderableInk::IsSetCandidate() const

     Author:    Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
     Created:   2/10/99
     Returns:   TRUE.
     Purpose:   Indicates that NodeRenderableInks are candidates for membership of Attribute
				gallery set.
**********************************************************************************************/
       
BOOL NodeRenderableInk::IsSetCandidate() const
{ 
	return TRUE;
}



/********************************************************************************************

>	BOOL NodeRenderableInk::CanTransform()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Returns:	This class's implementation always returns TRUE - i.e. NodeRenderableInks 
				should be transformed by default.
	Purpose:	Determine whether a renderable node can be transformed - if it does not,
				then no undo information needs to be stored for it.
	SeeAlso:	NodeRenderable::CanTransform

********************************************************************************************/

BOOL NodeRenderableInk::CanTransform()
{
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL NodeRenderableInk::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/05
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Begin to write out you child records, in the web format

				The base class will write out a TAG_DOWN record, but only if it has a child ptr

	SeeAlso:	WritePostChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/

BOOL NodeRenderableInk::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)
{
	return(WriteBeginChildRecordsNative(pFilter));
}

/********************************************************************************************

>	virtual BOOL NodeRenderableInk::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/05
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Begin to write out you child records, in the native format

				The base class will write out a TAG_DOWN record, but only if it has a child ptr

	SeeAlso:	WritePostChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/

BOOL NodeRenderableInk::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)
{
	BOOL bNeedDown = FALSE;
	BOOL ok = TRUE;

	Node* pChild = FindFirstChild();
	if (pChild != NULL)
	{
		// The node is considered to have children ONLY if at least one child is not a hidden node.
		if (!pChild->IsNodeHidden() || (pChild->FindNextNonHidden() != NULL))
		{
			bNeedDown = TRUE;
		}
	}

	BOOL bBounds = WillWriteBounds(pFilter);
	if (bNeedDown || bBounds)
	{
		ok = pFilter->WriteZeroSizedRecord(TAG_DOWN);
		if (ok && bBounds)
			ok = WriteBoundsRecord(pFilter);
	}

	return(ok);
}


/********************************************************************************************

>	virtual BOOL NodeRenderableInk::WriteBoundsRecord(BaseCamelotFilter* pFilter)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/05
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Write out a record containing the bounds of this object

********************************************************************************************/

BOOL NodeRenderableInk::WriteBoundsRecord(BaseCamelotFilter* pFilter)
{
	BOOL ok = TRUE;
	// Write out the record
	DocRect Rect = GetBoundingRect();

	CamelotFileRecord Rec(pFilter, TAG_OBJECTBOUNDS, TAG_OBJECTBOUNDS_SIZE);
	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(Rect.lo);
	if (ok) ok = Rec.WriteCoord(Rect.hi);
	if (ok) ok = pFilter->Write(&Rec);

	return(ok);
}


/********************************************************************************************

>	virtual BOOL NodeRenderableInk::WillWriteBounds(BaseCamelotFilter* pFilter)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/05
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if this node will write out a bounds record
	Purpose:	Determines if the down/up pair need to be written

********************************************************************************************/

BOOL NodeRenderableInk::WillWriteBounds(BaseCamelotFilter* pFilter)
{
	return(pFilter->GetBoundsWriteLevel() >= BWL_ALL);
}


/********************************************************************************************

>	virtual BOOL NodeRenderableInk::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Finished writing out you child records, in the web format

				The base class will write out a TAG_UP record, but only if it has a child ptr

	SeeAlso:	WritePostChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/

BOOL NodeRenderableInk::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)
{
	return WriteEndChildRecordsNative(pFilter);
}

/********************************************************************************************

>	virtual BOOL NodeRenderableInk::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Finished writing out you child records, in the native format

				The base class will write out a TAG_UP record, but only if it has a child ptr

	SeeAlso:	WritePostChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/

BOOL NodeRenderableInk::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)
{
	BOOL bNeedUp = WillWriteBounds(pFilter);

	Node* pChild = FindFirstChild();
	if (pChild != NULL)
	{
		// The node is considered to have children ONLY if at least one child is not a hidden node.
		if (!pChild->IsNodeHidden() || (pChild->FindNextNonHidden() != NULL))
			bNeedUp = TRUE;
	}

	if (bNeedUp)
		return pFilter->WriteZeroSizedRecord(TAG_UP);

	return TRUE;
}




/********************************************************************************************

>	virtual BOOL NodeRenderableBounded::RenderCached(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/2004
	Returns:	TRUE if the node managed to render a cached version of itself
					then pNextNode contains the pointer to the node to continue rendering from
				FALSE if the node did not render a cached version of itself and thus
					normal rendering is required
	Purpose:	Protected Helper function
				Allow this node to render itself using cached data if it has it and for that
				cached data to represent an arbitrary section of the document (usually a subtree
				but can be more)

				For this implementation we assume that our cached info will represent just 
				this subtree.

				Also start to capture the rendered info into a cached bitmap if possible
	SeeAlso:	NodeRenderableBounded::CaptureCached

********************************************************************************************/

BOOL NodeRenderableBounded::RenderCached(RenderRegion* pRender)
{
	// Standard implementation of RenderCached takes note of RenderRegion.BackmostChangedNode
	// and uses the global CBitmapCache object...
	//
	// First, try to find quick ways to do nothing...
	// If cacheing is disabled bale out now!
	if (!NodeRenderableBounded::bEnableCacheing)
		return FALSE;

//TRACEUSER("Phil", _T("NRB::RenderCached for %x %s\n"), this, GetRuntimeClass()->m_lpszClassName);
	// If the rendering quality is outline - no point showing cached bitmaps...
	if (pRender->RRQuality.GetFillQuality() <= Quality::Solid)
		return FALSE;

	// If the rendering system is doing black and white only then we can't sensibly
	// produce a monochrome version of the cached bitmap...
	if (pRender->IsVeryMono() || pRender->IsHitDetect() || pRender->IsPrinting() || pRender->IsWrappedRender())
		return FALSE;

	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache==NULL)
		return FALSE;

	// If this node is definitely not cached, skip the cache rendering stuff...
	DocRect cliprect = pRender->GetClipRect();
	DocRect bounds = GetBoundingRect();										// Assure new bounds are recalced if they were invalid
	BOOL bRendered = FALSE;
	if (MayBeCached)
	{
		// If render region tells us to, then we must remove all 24BPP cached bitmaps
		// (Opaque bitmaps could capture parts of the tree underneath them and they could have changed.
		// That's what the ReleaseCache() function is telling us.)
		CBitmapCacheKey inky(this, pRender->GetScaledPixelWidth());			//GetScaledPixelWidthDouble?

		Node* pBackmostChanged = pRender->GetBackmostChangedNode();
		if (pBackmostChanged)
		{
			// Only delete cached opaque bitmaps if they are above the lowest
			// changed node in this render (keep ones that are below because they are still valid)
			//
			// (Remember that the bounds of this object should already have been checked against
			// the clipping rectangle before this function is called, so we will only be chucking
			// out cached items that intersect the clipping rect.)
			//
			if (pBackmostChanged->IsUnder(this))
			{
				// Then we must remove all opaque bitmaps in the cache
				// associated with this node
//TRACEUSER("Phil", _T("NRB:RenderCached release transp bitmaps %x %s\n"), this, this->GetRuntimeClass()->m_lpszClassName);
				BOOL bRemoved = pBitmapCache->RemoveAllOwnedBitmaps(inky, TRUE);	// Only remove opaque bitmaps
				if (bRemoved) MayBeCached = FALSE;									// assume that is some were opaque, all were (and thus all have been removed)...
			}
		}

		// Look for a cached bitmap at the appropriate pixel size...
		CCachedBitmap cbmp;
		BOOL bFound = pBitmapCache->Lookup(inky, cbmp);
		if (bFound)
		{
			// If cached bitmap covers the current clipped object rect, we can use it...
			DocRect clippedboundrect = cliprect.Intersection(bounds);
			if (cbmp.GetCachedRect().ContainsRect(clippedboundrect))
			{
//TRACEUSER("Phil", _T("NRB::RenderCached Rendering cached bitmap for %x %s %d\n"), this, GetRuntimeClass()->m_lpszClassName, cbmp.pbmpInfo->bmiHeader.biBitCount);
				bRendered = pRender->RenderBits(cbmp.pbmpInfo, cbmp.pbmpBits, &cbmp.coord0, 3);
			}
		}
	}

	if (bRendered)
		return TRUE;

	// --------------------------------------------------------------------
	// Now we must decide whether we can afford to cache this node...
	//

	// If this is being (solid) dragged don't waste time trying to cache it
//	if (IsDragged())
//		return FALSE;

	// Any sort of drag prevents new cacheing (to keep drags running smoothly)
	if (Operation::GetCurrentDragOp())
		return FALSE;

//TRACEUSER("Phil", _T("NRB:RenderCached prep capture %x %s\n"), this, this->GetRuntimeClass()->m_lpszClassName);
	// Do the following optimisation only on unselected nodes...
	if (!IsSelected())
	{
		// This is a performance optimisation to reduce cacheing overhead...
		// If there are more than 2 nested captures already running, don't try another one...
		if (pRender->GetCaptureDepth() > 2)
			return FALSE;

		// We can use cache throttling to prevent this node being cached
		//
		// If we've run out of time to do cacheing then stop now.
		// if (time spent cacheing thus far is >5% of total rendering time) then don't do any more
//		Node* pParent = FindParent();
		if (NodeRenderableBounded::bThrottleCacheing &&
			pRender->GetStoredCount()*((INT64)20) > pRender->GetTotalCount() //&&
//			!IsSelected() &&			// Selected nodes can't be throttled
//			pParent &&					// Nodes without parents can't be trottled
//			pParent->IsLayer()			// Nodes whose parents are layers can't be throttled
			)
		{
//TRACEUSER("Phil", _T("NRB:RenderCached throttling prevents capture %x %s %I64d %I64d\n"), this, this->GetRuntimeClass()->m_lpszClassName, pRender->GetStoredCount()*(INT64)20, pRender->GetTotalCount());
			return FALSE;
		}
//TRACEUSER("Phil", _T("NRB:RenderCached throttling allows capture %x %s %I64d %I64d\n"), this, this->GetRuntimeClass()->m_lpszClassName, pRender->GetStoredCount()*(INT64)20, pRender->GetTotalCount());
	}

#if defined(_WIN32)
	// If we couldn't find or render a cached bitmap then try to cache a new one
	// Start timing of this cacheing overhead code
	LARGE_INTEGER countStart;
	LARGE_INTEGER countEnd;
	QueryPerformanceCounter(&countStart);
#endif
	
	double ScaledPixelWidth = pRender->GetScaledPixelWidth();

	// Work out how much of the object we propose to capture
	// (This may change during the Capture if we have to fall back to 24BPP opaque capture)
	DocRect viewrect = cliprect;
	DocRect CaptureRect = bounds;

	// Only do stuff if the cliprect is at least half the size of the object
	if (cliprect.Width()>(bounds.Width()/2) && cliprect.Height()>(bounds.Height()/2))
	{

		View* pView = pRender->GetRenderView();
		Spread* pSpread = pRender->GetRenderSpread();
		if (pView && pSpread)
		{
			viewrect = pView->GetDocViewRect(pSpread);
			pSpread->DocCoordToSpreadCoord(&viewrect);
			if (!viewrect.IsValid() || !viewrect.ContainsRect(cliprect))
				viewrect = cliprect;
		}
		CaptureRect = CaptureRect.Intersection(viewrect);				// Proposed capture area is obj x viewport

		if (CaptureRect.IsValid())
		{
			// If we're nearly going to capture the whole object
			// (Compare area of bounding rect with area of view rect so that bitmap sizes don't get out of control)
			XLONG areaBounds = (XLONG)bounds.Width()*(XLONG)bounds.Height();
			XLONG area2View = (XLONG)2*(XLONG)viewrect.Width()*(XLONG)viewrect.Height();
			if (areaBounds < area2View)
			{
				// Then grab the whole thing
				// (Bounding Rect is small enough for us to capture the whole object ignoring clipping rect)
				CaptureRect = bounds;
			}

			// Inflate by one pixel all round to allow for anti-aliasing effects at the edges
			CaptureRect.Inflate((INT32)ScaledPixelWidth);

			UINT32 flagCoverage = 0;
			if (CaptureRect.ContainsRect(bounds))
				flagCoverage = cfFULLCOVERAGE;

			// Only cache if it's worth it!
			if (CaptureRect.Width()>=ScaledPixelWidth*4 || CaptureRect.Height()>=ScaledPixelWidth*4)
			{
				// It's quicker to scan ahead looking for non-mix transparency than to find it
				// half way through rendering a bitmap...
//TRACEUSER("Phil", _T("NRB:RenderCached start capture %x %s\n"), this, this->GetRuntimeClass()->m_lpszClassName);
				CaptureFlags caFlags(flagCoverage | cfPIXWIDTHSCALE | (this->ContainsNonMixTransparency(pRender) ? cfGRABRENDERED : cfCOLLAPSETOMASTER | cfUNCLIP) );
				pRender->StartCapture(this, CaptureRect, CAPTUREINFO(ctNESTABLE, caFlags), TRUE, FALSE);
			}
		}
	}

#if defined(_WIN32)
	// Add up time spent doing cacheing work and store it in the render region
	INT64 countCacheing = pRender->GetStoredCount();
	QueryPerformanceCounter(&countEnd);
	countCacheing += countEnd.QuadPart-countStart.QuadPart;
	pRender->SetStoredCount(countCacheing);
#endif
	
	return FALSE;
}




/********************************************************************************************

>	virtual BOOL NodeRenderableBounded::CaptureCached(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/2004
	Returns:	TRUE if captured data was cached
	Purpose:	Protected Helper function
				Use the CaptureManager to capture the results of rendering, cache them
				and associate them with this Ink node
	SeeAlso:	NodeRenderableBounded::RenderCached

********************************************************************************************/

BOOL NodeRenderableBounded::CaptureCached(RenderRegion* pRender)
{
	// If cacheing is disabled bale out now!
	if (!NodeRenderableBounded::bEnableCacheing)
		return FALSE;

	Capture* pCapture = pRender->GetTopCapture();
	if (pCapture==NULL)											// If nothing was captured
		return FALSE;											// Then do nothing

	// Only stop the capture if we started it
	// (means we only capture whole subtrees at the mo.)
	if (pCapture->GetOwner()==this)
	{
#if defined(_WIN32)
		LARGE_INTEGER countStart;
		LARGE_INTEGER countEnd;
		QueryPerformanceCounter(&countStart);
#endif
		
		// End this capture:
		// Blit capture to screen
		// Retain bitmap because we will release it ourselves only if we fail to cache it
		BOOL bCached = FALSE;
		LPBITMAPINFO lpInfo = NULL;
		LPBYTE lpBits = NULL;
		DocRect CaptureRect = GetBoundingRect();				// Set maximum size we allow
		BOOL bFullCoverage = pCapture->HasFullCoverage();
		pRender->StopCapture(this, TRUE, FALSE, &lpInfo, &lpBits, &CaptureRect);

		// If the capture gave us back a bitmap, try to cache it
		if (lpInfo && lpBits && CaptureRect.IsValid())
		{
			// If we can't find the cache then all is lost!
			CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
			if (pBitmapCache)
			{
				double ScaledPixelWidth = pRender->GetScaledPixelWidth();
				CBitmapCacheKey inky(this, ScaledPixelWidth); //GetScaledPixelWidthDouble?

				CCachedBitmap cbmp;

				cbmp.pbmpBits = lpBits;
				cbmp.pbmpInfo = lpInfo;
				cbmp.SetCachedRect(CaptureRect);
				cbmp.nPriority = CACHEPRIORITY_TEMPBITMAP_LOW;
				cbmp.bFullCoverage = bFullCoverage;

				if (cbmp.IsValid())
				{
//TRACEUSER("Phil", _T("NRB:CaptureCached %x %s\n"), this, this->GetRuntimeClass()->m_lpszClassName);
					pBitmapCache->StoreBitmap(inky, cbmp);
					MayBeCached = TRUE;
					bCached = TRUE;
				}
			}
		}

		// If we failed to cache the captured bitmap then release it
		if (lpInfo!=NULL && lpBits!=NULL && !bCached)
		{
			FreeDIB(lpInfo, lpBits, NULL, FALSE);
		}

		INT64 countCacheing = pRender->GetStoredCount();
#if defined(_WIN32)
		QueryPerformanceCounter(&countEnd);
		countCacheing += countEnd.QuadPart-countStart.QuadPart;
#endif
		pRender->SetStoredCount(countCacheing);

	}

	return TRUE;
}




/********************************************************************************************

>	virtual BOOL NodeRenderableBounded::ReleaseCached(BOOL bAndParents = TRUE,
													  BOOL bAndChildren = TRUE,
													  BOOL bSelf = TRUE,
													  BOOL bAndDerived = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/2004
	Returns:	TRUE if captured data was cached
	Purpose:	Protected Helper function
				Use the CaptureManager to capture the results of rendering, cache them
				and associate them with this Ink node
	SeeAlso:	NodeRenderableInk::RenderCached, CaptureCached

********************************************************************************************/

BOOL NodeRenderableBounded::ReleaseCached(BOOL bAndParents, BOOL bAndChildren, BOOL bSelf, BOOL bAndDerived)
{
	if (MayBeCached)
	{
		BOOL bRemoveOpaqueBitmapsOnly = IsDragged();				// If we're being dragged, only remove opaque bmps
		BOOL bDidRemoveSome = FALSE;
		CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
		if (pBitmapCache!=NULL && bSelf)
		{
			CBitmapCacheKey inky(this, 42);
			bDidRemoveSome = pBitmapCache->RemoveAllOwnedBitmaps(inky, bRemoveOpaqueBitmapsOnly);
		}

		if (bDidRemoveSome && !bRemoveOpaqueBitmapsOnly)			// If we know we removed all cached bitmaps
			MayBeCached = FALSE;									// Then we can clear our maybe flag to optimise rendering
	}

	// If we should release our children's caches as well...
	if (bAndChildren)
	{
		Node* pChild = FindFirstChild();
		while (pChild)
		{
			if (pChild->IsBounded())
				((NodeRenderableBounded*)pChild)->ReleaseCached(FALSE, TRUE, TRUE, TRUE);

			pChild = pChild->FindNext();
		}
	}

	// If I can't be cached, neither can my parent...
	if (bAndParents && FindParent() && FindParent()->IsBounded())
		((NodeRenderableBounded*)FindParent())->ReleaseCached(TRUE, FALSE, TRUE, TRUE);

	return TRUE;
}




/********************************************************************************************

>	virtual void NodeRenderableBounded::CopyCached(NodeRenderableBounded* pCopyOfNode, double dRes, INT32 maxOption = 0)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/2004
	Returns:	-
	Purpose:	Copy fixed res cached bitmaps associated with this node to be associated
				with the specified new node
	SeeAlso:	NodeRenderableInk::RenderCached, CaptureCached

********************************************************************************************/

void NodeRenderableBounded::CopyCached(NodeRenderableBounded* pCopyOfNode, double dRes, INT32 maxOption)
{
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();

	INT32 i = 0;
	for (i = 0; i<=maxOption; i++)
	{
		// Copy the cached bitmap
		CCachedBitmap cbmp;
		CBitmapCacheKey inky(this, dRes, i);
		CBitmapCacheKey inkycopy(pCopyOfNode, dRes, i);
		BOOL bFound = pBitmapCache->Lookup(inky, cbmp);
		if (bFound)
		{
			cbmp = cbmp.Copy();
			pBitmapCache->StoreBitmap(inkycopy, cbmp);
		}
	}
	pCopyOfNode->MayBeCached = TRUE;
}




/********************************************************************************************

>	virtual void NodeRenderableBounded::CopyCached(NodeRenderableBounded* pCopyOfNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/2004
	Returns:	-
	Purpose:	Copy all cached bitmaps associated with this node to be associated
				with the specified new node
	SeeAlso:	NodeRenderableInk::RenderCached, CaptureCached

********************************************************************************************/

void NodeRenderableBounded::CopyCached(NodeRenderableBounded* pCopyOfNode)
{
	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();

	CBitmapCacheKey inky(this, 42);
	CCacheKeyMap::iterator pos = pBitmapCache->GetStartPosition();
	CCachedBitmap abitmap = pBitmapCache->FindNextOwnedBitmap(pos, inky);
	pCopyOfNode->MayBeCached = FALSE;
	while (abitmap.IsValid())
	{
		CCachedBitmap copybitmap = abitmap.Copy();
		CBitmapCacheKey copyinky(inky);
		copyinky.SetOwner(pCopyOfNode);
		pBitmapCache->StoreBitmap(copyinky, copybitmap);

		pCopyOfNode->MayBeCached = TRUE;

		abitmap = pBitmapCache->FindNextOwnedBitmap(pos, inky);
	}
}




/********************************************************************************************

>	virtual void NodeRenderableBounded::TransformCached(TransformBase& Trans, double dTestPixelWidth)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/2004
	Inputs:		Trans - Transformation
				dPixelWidth - PixelWidth to be tested specifically to control Trans.bHaveTransformedCached
	Returns:	-
	Purpose:	Transform all the cached bitmaps associated with this node
	SeeAlso:	NodeRenderableInk::RenderCached, CaptureCached

********************************************************************************************/

void NodeRenderableBounded::TransformCached(TransformBase& Trans, double dTestPixelWidth)
{
	// If cacheing is disabled bale out now!
	if (!NodeRenderableBounded::bEnableCacheing)
	{
		MayBeCached = FALSE;
		Trans.bHaveTransformedCached = FALSE;
		Trans.bTransformYourChildren = TRUE;
		return;
	}

	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();

	CBitmapCacheKey inky(this, 42);
	CCacheKeyMap::iterator pos = pBitmapCache->GetStartPosition();
	CCachedBitmap abitmap = pBitmapCache->FindNextOwnedBitmap(pos, inky);
	MayBeCached = abitmap.IsValid();						// Update MayBeCached here because we can
	BOOL bTransformedTested = FALSE;
	while (abitmap.IsValid())
	{
		abitmap.Transform(Trans);
		pBitmapCache->StoreBitmap(inky, abitmap);

		if (inky.GetPixelWidth() == dTestPixelWidth && abitmap.IsTransparent())
			bTransformedTested = TRUE;

		abitmap = pBitmapCache->FindNextOwnedBitmap(pos, inky);
	}

	// We can only continue to transform cached things if all our bitmaps are transparent (32BPP)
	// And if we actually had some cached data to transform
	if (!bTransformedTested)
	{
		Trans.bHaveTransformedCached = FALSE;
		Trans.bTransformYourChildren = TRUE;
	}
}




/********************************************************************************************

>	virtual void NodeRenderableBounded::RenderEffectAttributes(RenderRegion* pRegion)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/2005
	Returns:	-
	Purpose:	Render any effect attributes attached to this node
	SeeAlso:	-

********************************************************************************************/

void NodeRenderableBounded::RenderEffectAttributes(RenderRegion* pRegion)
{
	Node* pNode = FindLastChild(CC_RUNTIME_CLASS(NodeRenderableBounded));
	while (pNode)
	{
		pNode = pNode->FindNext();
		if (pNode && pNode->IsAnAttribute())
		{
			pNode->Render(pRegion);
		}
	}
}




/********************************************************************************************

>	virtual void NodeRenderableInk::RenderEorDrag( RenderRegion* pRender )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/93
	Inputs:		pRender - A pointer to a render region to draw in
	Purpose:	Each class derived from NodeRenderableInk should write a version
				of this member function. In most cases it will be the same as the
				Render() member function but some of the nodes (such as groups)
				need to perform special actions. The function is for drawing the
				object while it is being dragged about by the selector tool. It
				should do this with an effective wysiwyg level of 0 so that the
				drawing is fast.

********************************************************************************************/

void NodeRenderableInk::RenderEorDrag( RenderRegion* pRender )
{
	// This function does nothing. It is a virtual function and should be replaced by
	// derived class versions of it
}


/********************************************************************************************

>	void NodeRenderableInk::RenderEorDragChildren(RenderRegion * pRender)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/2000
	Inputs:		pRender		-	Render region to use
				
	Purpose:	Recursively calls RenderEorDrag for all children of this node,
				ignoring nodes which have ChildrenAreEorDragRenderedByMe() returning TRUE
	SeeAlso:	NodeRenderableInk::RenderEorDrag, 
	NodeRenderableInk::ChildrenAreEorDragRenderedByMe

********************************************************************************************/
void NodeRenderableInk::RenderEorDragChildren(RenderRegion * pRender)
{
	Node * pNode = FindFirstChild();

	while (pNode)
	{
		if (pNode->IsAnObject())
		{
			if (pNode->IsCompound())
			{
				if (((NodeRenderableInk *)pNode)->ChildrenAreEorDragRenderedByMe())
				{
					// just render this node
					((NodeRenderableInk *)pNode)->RenderEorDrag(pRender);
				}
				else
				{
					((NodeRenderableInk *)pNode)->RenderEorDragChildren(pRender);
					((NodeRenderableInk *)pNode)->RenderEorDrag(pRender);
				}
			}
			else
			{
				((NodeRenderableInk *)pNode)->RenderEorDrag(pRender);
			}
		}

		pNode = pNode->FindNext();
	}
}

/********************************************************************************************

>	void NodeRenderableInk::ClearSubSelection( BOOL ReDraw )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/93
	Inputs:		ReDraw - TRUE if the selection should be redrawn
	Purpose:	Goes through the object and clears the selection state of all its blobs. eg
				a selected path has blobs and each of these blobs can be selected. This
				clears the blobs selection BUT not the objects selection!

********************************************************************************************/

void NodeRenderableInk::ClearSubSelection( BOOL ReDraw )
{
	// Base class does nothing
}


/********************************************************************************************

>	void NodeRenderableInk::RenderFillBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/94
	Inputs:		pRender - The region to render the blobs into
	Purpose:	Draws the Fill blobs of an ink object. It does this by calling all the
				attributes that are its immediate children and asking them to render their
				Fill Blobs. If any of them happen to be items such as Graduated fills, then
				blobs will appear

********************************************************************************************/

//void NodeRenderableInk::RenderFillBlobs(RenderRegion* pRender)
//{
//}

void NodeRenderableInk::RenderAppliedFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	CCAttrMap* pAttribMap = new CCAttrMap(30);
	if (pAttribMap == NULL)
		return;

	// Find the topmost effect applied to this node
	// So that we can test whether it has preferential effect attributes
	Node* pEffectNode = FindParent();
	NodeEffect* pTopEffectNode = NULL;
	while (pEffectNode && pEffectNode->IsEffect())
	{
		// BODGE! we happen to know that shadow controllers can't have effect attrs applied to them
		// Would be better of there were some nice way to ask effect nodes about this.
		if (!pEffectNode->IsAShadowController())
			pTopEffectNode = (NodeEffect*)pEffectNode;
		pEffectNode = pEffectNode->FindParent();
	}

	// Now find any attributes that are applied to this node.
	BOOL FoundAttrs = FindAppliedAttributes(pAttribMap, 5000, NULL, FALSE);

	if (FoundAttrs)
	{
		// iterating all (key, value) pairs
		for (CCAttrMap::iterator Pos = pAttribMap->GetStartPosition(); Pos != pAttribMap->GetEndPosition();)
		{
			CCRuntimeClass *pType;
			void *pVal;
			pAttribMap->GetNextAssoc(Pos,pType,pVal);
			NodeAttribute* pAttr = (NodeAttribute*)pVal;

			// Only render fill blobs for the applied attribute if it can't
			// be overridden by a preferential effect attribute above...
			if (pTopEffectNode==NULL || (pTopEffectNode!=NULL && !pTopEffectNode->IsValidEffectAttr(pAttr)))
				pAttr->RenderFillBlobs(pRender);
		}
	}

	delete pAttribMap;
#endif
}


/********************************************************************************************

>	void NodeRenderableInk::RenderEffectBlobs(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/2005
	Inputs:		pRender - The region to render the blobs into
	Purpose:	Draws the Effect blobs of an ink object. It does this by calling all the
				attributes that are its immediate children and asking them to render their
				Fill Blobs. If any of them happen to be items such as Graduated fills, then
				blobs will appear

********************************************************************************************/

void NodeRenderableInk::RenderEffectBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	Node* pEffectNode = FindParent();
	Node* pTopEffectNode = NULL;

	while (pEffectNode && pEffectNode->IsEffect())
	{
		// BODGE! we happen to know that shadow controllers can't have effect attrs applied to them
		// Would be better of there were some nice way to ask effect nodes about this.
		if (!pEffectNode->IsAShadowController())
			pTopEffectNode = pEffectNode;
		pEffectNode = pEffectNode->FindParent();
	}

	if (pTopEffectNode && pTopEffectNode->IsBounded())
	{
		if (pTopEffectNode!=this)
			((NodeRenderableBounded*)pTopEffectNode)->RenderEffectBlobs(pRender);

		// Allow any effect attributes to render their blobs...
		NodeAttribute* pAttr = (NodeAttribute*)pTopEffectNode->FindLastChild(CC_RUNTIME_CLASS(NodeAttribute));
		while (pAttr && pAttr->IsEffectAttribute())
		{
			pAttr->RenderFillBlobs(pRender);

			pAttr = (NodeAttribute*)pAttr->FindPrevious(CC_RUNTIME_CLASS(NodeAttribute));
		}
	}
#endif
}


/********************************************************************************************

>	void NodeRenderableInk::RenderArtisticBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/94
	Inputs:		pRender - The region to render the blobs into
	Purpose:	Draws the Artistic blobs of an ink object. It does this by calling all the
				attributes that are its immediate children and asking them to render their
				Artistic Blobs. If any of them happen to be items such as Pressure attributes
				or some other artistic effect then blobs will appear.

********************************************************************************************/

void NodeRenderableInk::RenderArtisticBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	CCAttrMap* pAttribMap = new CCAttrMap(30);
	if (pAttribMap == NULL)
		return;

	// Now find any attributes that are applied to this node.
	BOOL FoundAttrs = FindAppliedAttributes(pAttribMap);

	if (FoundAttrs)
	{
		// iterating all (key, value) pairs
		for (CCAttrMap::iterator Pos = pAttribMap->GetStartPosition(); Pos != pAttribMap->GetEndPosition();)
		{
			CCRuntimeClass *pType;
			void *pVal;
			pAttribMap->GetNextAssoc(Pos,pType,pVal);

			NodeAttribute* pAttr = (NodeAttribute*)pVal;
			pAttr->RenderArtisticBlobs(pRender);
		}
	}

	delete pAttribMap;
#endif
}




/***********************************************************************************************
> Node* NodeRenderableInk::SimpleCopy() 

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
    Returns:    A copy of the node, or NULL if memory runs out
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
	Scope:      protected
**********************************************************************************************/

Node* NodeRenderableInk::SimpleCopy()
{
	NodeRenderableInk* NodeCopy; 
	NodeCopy = new NodeRenderableInk(); 
	if (NodeCopy) 
		NodeRenderableBounded::CopyNodeContents(NodeCopy);   
	return (NodeCopy);
}                   


/********************************************************************************************

>	virtual BOOL NodeRenderableInk::OnClick( DocCoord PointerPos, ClickType Click, 
									 ClickModifiers ClickMods )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		PointerPos - The Location of the mouse pointer at the time of the click
				Click - The type of click received (single, double, drag etc)
				ClickMods - The modifiers to the click (eg shift, control etc )
	Returns:	BOOL - TRUE if the node claims the click as its own and FLASE if it is
				not interested in the click
	Purpose:	Allows the Node to respond to clicks by selecting its blobs or starting
				drags etc.

				This function should be overridden in the all the NodeRenderableInk classes
				so that this verion never gets called. Eg the NodePath class might claim
				the click if it happened over one of its unselected blobs.

				Karim 07/04/2000
				With the advent of ToolObject blobs, this method can now be called when
				Object blobs are not showing. What this means is that when this method is
				called, you should *check* that conditions are right for you to respond.
				eg	if BlobManager's current interest does not include Object blobs, you
					may not want to do anything.

********************************************************************************************/

BOOL NodeRenderableInk::OnClick( DocCoord PointerPos, ClickType Click, 
								 ClickModifiers ClickMods, Spread *pSpread )
{
	// we do not want to claim the click, so return FALSE
	return FALSE;
}




/********************************************************************************************

>	virtual BOOL NodeRenderableInk::OnBlobPopUp(Spread* pSpread,
												DocCoord PointerPos,
												ContextMenu* pMenu
									 			)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95
	Inputs:		pSpread		The spread in which the action's occuring
				PointerPos	The Location of the mouse pointer at the time of the click
				pMenu		The menu to which items should be added
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the Node to respond to popup menu clicks by adding items to the popup
				menu if required. It also allows the node to prevent the menu appearing at all!
				This functions should be overridden in the all the NodeRenderableInk classes
				so that this verion never gets called. Eg the NodePath class might claim
				the click if it happened over one of its blobs.

********************************************************************************************/


BOOL NodeRenderableInk::OnBlobPopUp(Spread* pSpread,
									DocCoord PointerPos,
									ContextMenu* pMenu
							 		)
{
	// we do not want to claim the click, so return FALSE
	return FALSE;
}




/********************************************************************************************

>	virtual BOOL NodeRenderableInk::OnNodePopUp(Spread* pSpread,
												DocCoord PointerPos,
												ContextMenu* pMenu
									 			)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/95
	Inputs:		pSpread		The spread in which the action's occuring
				PointerPos	The Location of the mouse pointer at the time of the click
				pMenu		The menu to which items should be added
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the Node to respond to popup menu clicks by adding items to the popup
				menu if required. It also allows the node to prevent the menu appearing at all!
				This functions should be overridden in the all the NodeRenderableInk classes
				so that this verion never gets called. Eg the NodePath class might claim
				the click if it happened over one of its blobs.

********************************************************************************************/


BOOL NodeRenderableInk::OnNodePopUp(Spread* pSpread,
									DocCoord PointerPos,
									ContextMenu* pMenu
						 			)
{
	// we do not want to claim the click, so return FALSE
	return FALSE;
}




/********************************************************************************************

>	BOOL NodeRenderableInk::CalculatePathBoundingRect(Path& BoundPath, BOOL DontUseAttrs,
														  DocRect* pRect)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/94
	Inputs:		BoundPath - The path to calculate the bounding rectangle of
				DontUseAtrrs - TRUE if we do not want to apply the attributes to the path
				when calculating its bounds
	Outputs:	pRect - Will hold the paths bounding rect if everything worked
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Calculates the bounding rectangle of a path using GDraw. It will optionally
				go and find all the attributes that are applied to the path and tell GDraw
				about these as well (this is the normal way of using it)
				It can make up sone default attributes if needed as well. This will assume
				that the path is to be stroked as thin as possible.
	Errors:		GDraw can fail when calculating bounding rects, so beware
	SeeAlso:	NodeRenderableBounded::GetBoundingRect

********************************************************************************************/

BOOL NodeRenderableInk::CalculatePathBoundingRect(Path& BoundPath,
							BOOL DontUseAttrs, DocRect* pRect)
{
	// Check and initialise output.
	ERROR3IF(pRect == 0, "NodeRenderableInk::CalculatePathBoundingRect: null output");
	pRect->MakeEmpty();

	// Something to put the new bounding rect into
	DocRect NewBoundingRect;
	BOOL 	GDrawResult = TRUE;
	GDrawContext *GD = GRenderRegion::GetStaticDrawContext();

	if (GD == NULL)
		return(FALSE);

	// do we need to use the attrs
	if (DontUseAttrs)
	{
		// here we are not interested in the attributes
		// Find out what the paths bounding rect is now
		RECT			rectTmp;
		GDrawResult = GD->CalcStrokeBBox((POINT*)BoundPath.GetCoordArray(),
											BoundPath.GetVerbArray(), BoundPath.GetNumCoords(),
											&rectTmp,
											BoundPath.IsFilled, 0, CAPS_ROUND, JOIN_ROUND, NULL);
		if ( rectTmp.left>rectTmp.right || rectTmp.top>rectTmp.bottom )
			return false;
		NewBoundingRect = DocRect( rectTmp.left, rectTmp.top, rectTmp.right, rectTmp.bottom );
	}
	else
	{
		// First find all attributes applied to this node...
		// 30 is a default value - this will grow if it needs more space
		CCAttrMap AttribMap(30);
		if (FindAppliedAttributes(&AttribMap))
		{
			void* pLineColour = NULL;
			AttribMap.Lookup( CC_RUNTIME_CLASS(AttrStrokeColour),pLineColour);

			if (pLineColour && (*((AttrStrokeColour*)pLineColour)->GetStartColour() == COLOUR_NONE) ||
				!BoundPath.IsStroked)
			{
				// This path is not stroked, so we won't bother with any of the attributes
				// applied to it
				RECT	rectTmp;
				GDrawResult = GD->CalcStrokeBBox((POINT*)BoundPath.GetCoordArray(),
													BoundPath.GetVerbArray(), BoundPath.GetNumCoords(),
													&rectTmp,
													BoundPath.IsFilled, 0, CAPS_ROUND, JOIN_ROUND, NULL);
				if ( rectTmp.left>rectTmp.right || rectTmp.top>rectTmp.bottom )
					return false;
				NewBoundingRect = DocRect( rectTmp.left, rectTmp.top, rectTmp.right, rectTmp.bottom );

			}
			else
			{
				// Extract the info we need from the applied attributes
				void* pLineWidth = NULL;
				AttribMap.Lookup( CC_RUNTIME_CLASS(AttrLineWidth),pLineWidth);
				ENSURE(pLineWidth,"NodePath::GetBoundingRect can't find LineWidth");
	
				// if we did not get a line width, fail
				if (!pLineWidth) 
					return FALSE;

				// The Start cap
				void* pStartCap = NULL;
				AttribMap.Lookup( CC_RUNTIME_CLASS(AttrStartCap),pStartCap);
#ifdef _DEBUG
				if (pStartCap == 0)
					TRACE( _T("NodeRenderableInk::CalculatePathBoundingRect: can't find StartCap\n"));
#endif

				// The join style
				void* pJoinType = NULL;
				AttribMap.Lookup( CC_RUNTIME_CLASS(AttrJoinType),pJoinType);
#ifdef _DEBUG
				if (pJoinType == 0)
					TRACE( _T("NodeRenderableInk::CalculatePathBoundingRect: can't find JoinType\n"));
#endif
				if(pStartCap != NULL && pJoinType != NULL && pLineWidth != NULL)
				{
					// Now pass those values to Gavin along with the path for a bounds calculation
					RECT rectTmp;
					GDrawResult = GD->CalcStrokeBBox((POINT*)BoundPath.GetCoordArray(),
											BoundPath.GetVerbArray(), BoundPath.GetNumCoords(),
											&rectTmp,
											BoundPath.IsFilled,
											((AttrLineWidth*)pLineWidth)->Value.LineWidth,
											(CapStyles)((AttrStartCap*)pStartCap)->Value.StartCap,
											(JoinStyles)((AttrJoinType*)pJoinType)->Value.JoinType,
											NULL //Pointer to dash pattern attribute
											);
					if ( rectTmp.left>rectTmp.right || rectTmp.top>rectTmp.bottom )
						return false;
					NewBoundingRect = DocRect( rectTmp.left, rectTmp.top, rectTmp.right, rectTmp.bottom );
				}
			}
		}
	}

	// Make sure Gavin did not screw up.
	if (!NewBoundingRect.IsValid())
		// Set it to TRUE, as this means it went wrong in Gavin Talk.
		GDrawResult = TRUE;

	// see if GDraw worked (It returns FALSE when it works!)
	if (GDrawResult == FALSE)
	{
		// update the rectangle
		*pRect = NewBoundingRect;
		return TRUE;
	}

	// to get here, something must have failed
	return FALSE;
}




//////////////////////////////////////////////////////////////////////////////////////////////
// Selection Members

/********************************************************************************************

>	void NodeRenderableInk::DeselectAll(BOOL RenderBlobs = TRUE, BOOL InformImmediate = TRUE)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/93
	Inputs:		RenderBlobs: Should blobs be rendered
				InformImmediate - TRUE if the SelChanged message should be broadcast immediatly
								  FALSE if it should be saved until the end of the operation
	Purpose:	Deselects all the selected objects in the tree, leaving camelot with
				nothing selected. It also removes the EORed blobs of the objects that
				were selected at the time of the call

********************************************************************************************/

void NodeRenderableInk::DeselectAll(BOOL RenderBlobs, BOOL InformImmediate)
{
	// Find the selected objects in the tree;
	SelRange* Selected = GetApplication()->FindSelection();
	ENSURE( Selected!=NULL, "Selection object is null in DeselectAll()");

	// Find first selected node                                                    
	Node* pFirstSelectedNode = Selected->FindFirst();

	// If there is a selection, EOR blobs off, deselect nodes, and inform everybody
	if (pFirstSelectedNode != NULL)
	{
		// Go though and render all the EOR blobs off the screen
		if (RenderBlobs)
		{
			// Find the Blob Manager
			BlobManager* BlobMgr = GetApplication()->GetBlobManager();
			ENSURE( BlobMgr!=NULL, "Blob Manager unexpectedly not there.");

			// Render all the blobs
			BlobMgr->RenderOff(NULL, pFirstSelectedNode->FindParentSpread());
		}

		// and clear the object selection
		Node* pNode = pFirstSelectedNode;
		while(pNode)
		{
			// Deselect the object
			//ENSURE( pNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderable)), "Selected Node not renderable");
			//((NodeRenderable*)pNode)->DeSelect(FALSE);
			if (pNode->IsNodePath())
				((NodePath*)pNode)->InkPath.ClearSubSelection();

			/*
			// DMc
			// Deselect all nodes under node
			Node * pSubNode = pNode->FindFirstDepthFirst();

			while (pSubNode)
			{
				pSubNode->SetSelected(FALSE);

				if (pSubNode->IsNodePath())
				{
					((NodePath*)pSubNode)->InkPath.ClearSubSelection();
				}

				pSubNode = pSubNode->FindNextDepthFirst(pNode);
			}
			*/

			// Deselect the node itself
			pNode->SetSelected(FALSE);

			// Find the next node
			pNode = Selected->FindNext(pNode);
		}

		// Selection cache is no longer valid, so update and tell everyone that it has changed
		Selected->Update(InformImmediate);
	}
}

/********************************************************************************************

>	NodeRenderableInk::DeselectAllOnLayer(Layer* Layer) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/94
	Inputs:		Layer: The layer on which to deselect objects
	Purpose:	Same as DeselectAll except that it only deselects objects on Layer
	SeeAlso:	NodeRenderableInk::DeselectAll

********************************************************************************************/

void NodeRenderableInk::DeselectAllOnLayer(Layer* Layer) 
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(Layer == NULL,"Layer == NULL");
	if (Layer == NULL) return;

	ERROR3IF(Layer->FindParent() == NULL,"Layer has no parent");
	if (Layer->FindParent() == NULL) return;

	ERROR3IF(!IS_A(Layer->FindParent(),Spread),"The parent of a layer was not a spread"); 
	if (!IS_A(Layer->FindParent(),Spread)) return;
	
	// if the layer has no children, return immediately (otherwise the range screws things up by
	// giving nodes from other layers to deselect) - MarkN 11/1/95
	if (Layer->FindFirstChild() == NULL) return;

	// Find the layer's spread 
	Spread *pSpread = (Spread*)Layer->FindParent();

	// Set up a range of the selected object in this layer
//	RangeControl rc = { TRUE, FALSE };
	Range  SelectedRange(Layer->FindFirstChild(),NULL,RangeControl(TRUE,FALSE)); 
	Node* CurrentNode = SelectedRange.FindFirst(); 

	// See if there was anything selected in the first place
	if (CurrentNode != NULL)
	{
		// Go and find the Blob Manager
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		ENSURE( BlobMgr!=NULL, "There is no blob manager - Impossible!");
		if (BlobMgr==NULL)	return;

		// Go through all the regions and rub out all the blobs
		RenderRegion* pRegion = DocView::RenderOnTop( NULL, pSpread, ClippedEOR );
		while (pRegion)
		{
			// Find out the rect for this render region
			DocRect Rect = pRegion->GetRegionRect();
			if ( (Rect.IsValid()) && (!Rect.IsEmpty()) )
			{
				// Find the first node in the selection range
				CurrentNode = SelectedRange.FindFirst(); 

				// Find all the selected nodes in the tree
				while (CurrentNode != NULL)
				{
					// Rub the blobs off the screen for this node in this region
					ENSURE( CurrentNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderable)), "Node was not renderbale");
					BlobMgr->RenderMyBlobs(&Rect, pSpread, (NodeRenderable*)CurrentNode);

					// Find the next selected object on the layer 
					CurrentNode = SelectedRange.FindNext(CurrentNode);
				}
			}

			// Thats all the bits deselected for that render region, give me the next one please
			pRegion = DocView::GetNextOnTop( NULL );
		}
		
		// Now go though the selected nodes on the layer and deselect them 
	    CurrentNode = SelectedRange.FindFirst(); 
		while (CurrentNode != NULL)
		{
			// Make it unselected
			//((NodeRenderable*)CurrentNode)->DeSelect( FALSE );
			CurrentNode->SetSelected(FALSE);

			// and find the next one
			CurrentNode = SelectedRange.FindNext(CurrentNode);
		}

		// Selection cache is no longer valid
		GetApplication()->Selection->Update(TRUE);
	}
#endif
}



/********************************************************************************************
>	void NodeRenderableInk::SelectAllInRect(DocRect Rect, Spread* pSpread,
											NodeRenderableInk::SelStateAction st = SET)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> / JustinF
	Created:	15/9/93
	Inputs:		Rect - The Rectangle to act as a bounding box for the selection
				Spread - The spread on which to select the objects
				st	- either CLEAR, SET, or TOGGLE, which is what should be done to
					  the selection state of each object within the rectangle. 
	Purpose:	Selects all the objects that are wholly inside the supplied rectangle.
				It will draw in all the EORed blobs of the objects that it selects/deselects.
				rewritten by Simon so that objects on locked layers do not get selected
				also more optimal. 
				(st parameter etc added by JCF 1.11.94)
	SeeAlso:	NodeRenderableInk::DeselectAll
********************************************************************************************/

void NodeRenderableInk::SelectAllInRect(DocRect Rect, Spread* pSpread,
										NodeRenderableInk::SelStateAction st)
{
	// Make sure we are not being told garbage
	ERROR3IF(pSpread == NULL, "No spread in NodeRenderableInk::SelectAllInRect"); 
	ERROR3IF(pSpread->FindFirstLayer() == NULL, "No layer in NodeRenderableInk::SelectAllInRect");


	// Make all objects on modifiable layers within the given bounding box selected or
	// whatever.
	for (Layer* pLayer = pSpread->FindFirstLayer();
		 pLayer != NULL;
		 pLayer = pLayer->FindNextLayer())
	{
		if (!pLayer->IsLocked() && pLayer->IsVisible() &&
			pLayer->GetBoundingRect().IsIntersectedWith(Rect)) 
		{
			// The current layer is not locked so select all objects which intersect 
			// with rect 
/*			Range rng(pLayer->FindFirstChild(), NULL, RangeControl(TRUE, TRUE)); 
			for (Node* pNode = rng.FindFirst();
				 pNode != NULL;
				 pNode = rng.FindNext(pNode))
			{
				// make sure it is a Renderable Ink Node
				if (pNode->IsAnObject())
				{
					((NodeRenderableInk*)pNode)->SelectInRect(Rect, st);
				}
			}
*/
			Node* pNode = pLayer->FindFirstChild();
			while (pNode)
			{
				// make sure it is a Renderable Ink Node
				if (pNode->IsAnObject())
				{
					((NodeRenderableInk*)pNode)->SelectInRect(Rect, st);
				}

				pNode = pNode->FindNext();
			}

		}
	}

	// Update the selection cache
	GetApplication()->UpdateSelection();
}



/********************************************************************************************

>	virtual void NodeRenderableInk::SelectInRect(const DocRect& Rect, SelStateAction st)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14 April 2000
	Inputs:		Rect	const reference to the bounding rect to use.
				st		the selection state, ie SET/TOGGLE/CLEAR.
	Outputs:	This node may have its selection status changed, depending on the inputs.

	Purpose:	Helper method for the static fn SelectAllInRect() used by marquee select.
				This method changes the selection state of this node according to the given
				bounding rectangle and sel-state action. Most nodes will want to use the
				default behaviour, which is to select themselves if their bounds lie within
				the given bounding rectangle. If you want to do something special with the
				marquee select, then override this method.

	Notes:		This method is meant to be called solely from SelectAllInRect() - if you wish
				to call it from elsewhere, it *may* work as you expect, but it is a good idea
				that you check your assumptions on what preconditions are necessary.

	Errors:		ERROR3 in DEBUG if st holds an invalid state.
	See also:	SelectAllInRect().

********************************************************************************************/
void NodeRenderableInk::SelectInRect(const DocRect& Rect, SelStateAction st)
{
	// ok, check whether we lie within the given rect.
	if (Rect.ContainsRect(GetBoundingRect()))
	{
		switch (st)
		{
		case CLEAR:
			DeSelect(TRUE);
			break;

		case SET:
			Select(TRUE);
			break;

		case TOGGLE:
			if (IsSelected())
				DeSelect(TRUE);
			else
				Select(TRUE);
			break;

		default:
			ERROR3("NodeRenderableInk::SelectInRect; Unknown SelStateAction!\n");
			break;
		}
	}
}



/********************************************************************************************

>	void NodeRenderableInk::RedrawObject() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Purpose:	Invalidates the Bounding rectangle of the node, causing it to be redrawn. 

********************************************************************************************/

void NodeRenderableInk::RedrawObject() 
{
	// Find this nodes parent spread
	Spread* pSpread = FindParentSpread();

	// Find the owner document (could do it on this node, but leaping to spread
	// directly is slighlt quicker).
	BaseDocument *pDoc = pSpread->FindOwnerDoc();

	if ((pSpread != NULL) && (pDoc != NULL))
	{
		// Invalidate region (if we're in a real document)
		if (IS_A(pDoc, Document))
			((Document *) pDoc)->ForceRedraw(pSpread, GetBoundingRect(), FALSE, this);
		else
			// Not in a real document - ought to complain.
			// (If this ERROR3 proves to be a problem then could be removed - Tim)
			ERROR3("Trying to redraw an object that is in a BaseDocument!");
	}
}



/********************************************************************************************

>	void NodeRenderableInk::ApplyAttributeToObject(NodeAttribute* Attribute, BOOL Redraw); 
	
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		Attribute: The Attribute to apply to the object
				Redraw: Flag indicating if the object should be redrawn after the attribute is
				added. 
	Outputs:	-
	Returns:	-
	Purpose:	Applys Attribute to the selected node and then if Redraw is TRUE it 
				redraws the node.  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void NodeRenderableInk::ApplyAttributeToObject(NodeAttribute* Attribute, BOOL Redraw) 
{
	Attribute->AttachNode(this, FIRSTCHILD); 
	if (Redraw)
		RedrawObject(); // Redraw the node
}

/***********************************************************************************************

> BOOL NodeRenderableInk::ApplyAttributes(CCAttrMap* pAttribMap)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		pAttribMap 		  = ptr to the attributes to apply to this node
				RequiredAttrsOnly = TRUE is attrs should only be applied if object requires them
									as determined by the func RequiresAttrib()
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This generates copies of all the attributes in the map, and applies them to
				this node.

				If RequiredAttrsOnly is TRUE, then only the attrs in the map that are required
				by the node are applied.  RequiresAttrib() works this out for us.

***********************************************************************************************/

BOOL NodeRenderableInk::ApplyAttributes(CCAttrMap* pAttribMap,BOOL RequiredAttrsOnly)
{
	if (pAttribMap == NULL) return FALSE;

	BOOL ApplyAttr = TRUE;

	// iterating all (key, value) pairs
	for (CMapPtrToPtr::iterator Pos = pAttribMap->GetStartPosition(); Pos!=pAttribMap->GetEndPosition();)
	{
		CCRuntimeClass* pType;
		void* pVal;

		pAttribMap->GetNextAssoc(Pos,pType,pVal);

		NodeAttribute* pNodeAttr = (NodeAttribute *)pVal;

		if (pNodeAttr->CanBeAppliedToObject())
		{
			if (RequiredAttrsOnly)
				ApplyAttr = RequiresAttrib(pNodeAttr);

			if (ApplyAttr)
			{
				Node* pNode = pNodeAttr->SimpleCopy();
				if (pNode != NULL)
				{
					pNode->AttachNode(this,FIRSTCHILD);
					// Ilan
					// Supply nodes geometry to attributes which require it
					((NodeAttribute*)pNode)->LinkToGeometry(this);
				}
			}
		}
	}

	// This will remove all the redundant attributes that have been attached to the node
	NormaliseAttributes();

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL NodeRenderableInk::RequiresAttrib(CCRuntimeClass* AttribType, 
												   BOOL Search = FALSE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/94
	Inputs:		AttribType: The type of the attribute

				Search:		This flag only applies to compound objects. It specifies
							if we should search the compound to find out if any child
							requires the attribute. By default time is considered more
							important than truth and TRUE is returned on compound objects.

	Returns:	The answer to this question
	Purpose:	To determine if the node requires attribute AttribType to render itself

********************************************************************************************/

BOOL NodeRenderableInk::RequiresAttrib(CCRuntimeClass* AttribType, 
									   BOOL Search /* = FALSE */)
{
	List* ReqdAttribList;

	if (!IsCompound())
	{
		// Obtain the list of required attributes for this node
		ReqdAttribList = ObjectRegistry::GetRequiredAttribs(GetRuntimeClass()); 
		//Is AttribClass in the list ?
		NodeAttributeClassItem* AttrClassItem = (NodeAttributeClassItem*)(ReqdAttribList->GetHead());

		while (AttrClassItem != NULL)
		{
			if (AttrClassItem->AttributeClass == AttribType)
			{
				return TRUE;
			}
			AttrClassItem = (NodeAttributeClassItem*)(ReqdAttribList->GetNext(AttrClassItem));
		}
	}
	else
	{
		if (Search)
		{
			// Ask the kids if they require the attribute (they will ask their kids etc.)
			Node* pChild = FindFirstChild();
			NodeRenderableInk* pChildObject;
			while (pChild)
			{
				if (pChild->IsAnObject())
				{
					pChildObject = (NodeRenderableInk*)pChild;
					if (pChildObject->RequiresAttrib(AttribType, Search))
						return TRUE;
				}
				pChild = pChild->FindNext();
			} 
		}
		else
			return TRUE; // Assume the compound requires the attribute
	}
	return FALSE; // Not found (!IsCompound || IsCompound && Search cases )
} 


/********************************************************************************************

>	virtual BOOL NodeRenderableInk::CanAttrBeAppliedToMe(CCRuntimeClass* AttrType)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/95
	Inputs:		AttrType: The type of the attribute. It is a type, rather than an 
				instance because this function needs to be called from the Localise/Factor out 
				routines which operate on attribute types.

	Returns:	TRUE if an attribute of AttrType can be applied to this object, 
				FALSE otherwise. 

	Purpose:	This fn is not to be confused with RequiresAttrib, it determines if an 
				attribute of type AttrType can be directly applied to this object. The
				object is assumed to Require the attribute. 

				The base class fn returns TRUE. 

				Certain attributes cannot be directly applied to specific nodes (even though
				they require the attribute to render). eg. LineSpacing attributes cannot 
				be applied to text character objects. 

				This function is called by the GetObjectToApplyTo virtual function. 

	Errors:		-
	SeeAlso:	NodeRenderableInk::GetObjectToApplyTo

********************************************************************************************/

BOOL NodeRenderableInk::CanAttrBeAppliedToMe(CCRuntimeClass* AttrType)
{
	return TRUE;
}


/********************************************************************************************

>	virtual NodeRenderableInk* NodeRenderableInk::GetObjectToApplyTo(CCRuntimeClass* AttrType)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/95

	Inputs:		AttrType: The type of the attribute. It is a type, rather than an 
				instance because this function needs to be called from the Localise/Factor out 
				routines which operate on attribute types.

	Returns:	A pointer to an object that the attribute should be applied to. It returns
				NULL if there is no such object.

	Purpose:	Before applying an attribute to an object this function should be called to return
				the actual object the attribute should be applied to.

				This base class function calls CanBeAppliedToMe, and if the object says No
				then it tries it's parent etc. This system works for Line based text attributes
				which should never be applied to text characters.

				However if in future we meet a situation where things need to be done differently
				then this function can be overloaded.

	Errors:		-
	SeeAlso:	NodeRenderableInk::CanAttrBeAppliedToMe

********************************************************************************************/

NodeRenderableInk* NodeRenderableInk::GetObjectToApplyTo(CCRuntimeClass* AttrType)
{
	Node* pParent;
	NodeRenderableInk* pCurrentObject = this;
	do
	{
		if (pCurrentObject->CanAttrBeAppliedToMe(AttrType))
		{
			// if applying line level attr, force it to be the first line of the paragraph
			if (IS_A(pCurrentObject,TextLine) && TextLine::IsAttrTypeLineLevel(AttrType))
				pCurrentObject = ((TextLine*)pCurrentObject)->FindFirstLineOfParagraph();
			break;
		}

		// ok try the parent then
		pParent = pCurrentObject->FindParent(); 
		if (pParent && (pParent->IsAnObject()))
		{
			pCurrentObject = (NodeRenderableInk*)pParent;	
		}
		else
		{
			pCurrentObject = NULL; // No more to try
		}		
	} while (pCurrentObject); 

		
	return pCurrentObject;
}


/********************************************************************************************

>	virtual BOOL NodeRenderableInk::RequiresAttrib(NodeAttribute* pAttr, 
												   BOOL Search = FALSE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/94
	Inputs:		pAttr: 		An attribute
				Search:		This flag only applies to compound objects. It specifies
							if we should search the compound to find out if any child
							requires the attribute. By default time is considered more
							important than truth, and TRUE is returned on compound objects.
	Returns:	The answer to this question
	Purpose:	To determine if the node requires the pAttr to render itself

********************************************************************************************/

BOOL NodeRenderableInk::RequiresAttrib(NodeAttribute* pAttr, 
									   BOOL Search /*= FALSE*/)
{
	CCRuntimeClass* AttribType = pAttr->GetAttributeType(); 
	// Call fn which takes type
	return RequiresAttrib(AttribType, Search); 
} 

/********************************************************************************************

>	virtual CCRuntimeClass* NodeRenderableInk::GetCurrentAttribGroup()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Returns:	The default attribute group	(NodeRenderableInk)
	Purpose:	Every Ink object has an associated current attribute group, If an attribute 
				is applied to the object, and it needs to become current then the attribute 
				group specified will get updated.

********************************************************************************************/

CCRuntimeClass* NodeRenderableInk::GetCurrentAttribGroup()
{
	return (CC_RUNTIME_CLASS(NodeRenderableInk));
} 



/********************************************************************************************

>	NodeAttribute* NodeRenderableInk::GetChildAttrOfType(CCRuntimeClass* ReqdAttrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Inputs:		-
	Outputs:	-
	Returns:	If the node has an attribute with TYPE ReqdType then a pointer to the 
				attribute, else NULL.
	Purpose:	Scans the child attribute block of the node for an attribute with type 
				ReqdAttrib.
	Errors:		-
	SeeAlso:	NodeAttribute::GetAttributeType

********************************************************************************************/

NodeAttribute* NodeRenderableInk::GetChildAttrOfType(CCRuntimeClass* ReqdAttrib)
{  
/*	Node* Current = FindFirstChild();
	while (Current != NULL)
	{
		if (Current->IsAnAttribute())
		{
			if (((NodeAttribute*)Current)->GetAttributeType() == ReqdAttrib)
			{
				return ((NodeAttribute*)Current);
			}
		}
		else
		{
			if (Current->IsAnObject()) // Check for end of attribute block
			{
				return NULL;
			}
		}
		Current = Current->FindNext(); 
	}
	return NULL; // Not found
*/
	// It's important to do this scan in reverse render order so that closer scoped
	// attributes are found first. This helps report effect attributes correctly
	Node* Current = FindLastChild();
	while (Current != NULL)
	{
		if (Current->IsAnAttribute())
		{
			if (((NodeAttribute*)Current)->GetAttributeType() == ReqdAttrib)
			{
				NodeAttribute* pAttr = (NodeAttribute*)Current;
				if (pAttr->IsEffectAttribute() == IsValidEffectAttr(pAttr))
					return pAttr;
			}
		}
		Current = Current->FindPrevious(); 
	}
	return NULL; // Not found

}

/********************************************************************************************

>	BOOL NodeRenderableInk::HasAttrTypesOn(NodeRenderableInk* Object)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/95
	Purpose:	This function returns TRUE if the set of Object's child attribute types are 
				a subset of this objects child attribute types. 

********************************************************************************************/

BOOL NodeRenderableInk::HasAttrTypesOn(NodeRenderableInk* Object)
{
	AttrTypeSet OurAttrs;
	AttrTypeSet ObjectsAttrs;

	if (!AddChildAttrTypesToSet(&OurAttrs))
		return FALSE; 

	if (!(Object->AddChildAttrTypesToSet(&ObjectsAttrs)))
		return FALSE; 

	AttrTypeItem* pObjectsAttr = (AttrTypeItem*)ObjectsAttrs.GetHead();

	CCRuntimeClass* AttrType;
	while (pObjectsAttr)
	{
		AttrType = pObjectsAttr->AttributeType;
		if (!(OurAttrs.InSet(AttrType)))
			return FALSE; // sets are different
		pObjectsAttr = (AttrTypeItem*) ObjectsAttrs.GetNext(pObjectsAttr); 	
	} 

	return TRUE; 
}


/********************************************************************************************

>	NodeAttribute* NodeRenderableInk::FindAppliedAttribute(CCRuntimeClass* AttribType, BOOL bExcludeChildAttrs = FALSE, BOOL bStrict = TRUE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/95
	Inputs:		-
	Outputs:	-
	Returns:	The attribute of type 'AttribType' applied to this node.
	Purpose:	Finds the attribute of type 'AttribType' applied to this node.
	Errors:		-
	SeeAlso:	NodeRenderableInk::FindAppliedAttributes

********************************************************************************************/

BOOL NodeRenderableInk::FindAppliedAttribute(CCRuntimeClass* AttribType, NodeAttribute** ppAttr, BOOL bStrict)
{
	NodeAttribute* pAttr = FindAppliedAttribute(AttribType, FALSE, bStrict);
	if (ppAttr) *ppAttr = pAttr;
	return (pAttr!=NULL);
}

NodeAttribute* NodeRenderableInk::FindAppliedAttribute(CCRuntimeClass* AttribType, BOOL bExcludeChildAttrs, BOOL bStrict)
{
	// Made more efficient by Simon 09/06/95
//	*pAttrFound = ScanForNearestAttrib(AttribType, TRUE);
//	if (*pAttrFound)
//		return TRUE;
//	else
//		return FALSE;

	// Restored 06/01/2005 by Phil
	// FindFirst/PrevAppliedAttr have been made more efficient
	// And let's try to only have ONE way of scanning for applied attributes so we only
	// have to deal with ONE set of bugs...
	//
	//
	// Repeat the following loop for all attributes we encounter in the tree.
	NodeAttribute* pAttrib = NULL;
	if (bExcludeChildAttrs)
		pAttrib = NodeAttribute::FindPrevAppliedAttr(this);
	else
		pAttrib = NodeAttribute::FindFirstAppliedAttr(this);

	while (pAttrib)
	{
		if (pAttrib->GetAttributeType() == AttribType)
		{
			// If the attr is an effect attr
			// and it is in my child list
			// and I don't allow this type of attr to be an effect
			// Then continue looking
			// Else return it
//			if (!(pAttrib->FindParent()==this && pAttrib->IsEffectAttribute() && !this->IsValidEffectAttr(pAttrib)))
//				return pAttrib;
			if (pAttrib->FindParent()!=this)
				return pAttrib;

			if (bStrict)
			{
				if (pAttrib->IsEffectAttribute() == this->IsValidEffectAttr(pAttrib))
					return pAttrib;
			}
			else
				if (!(pAttrib->IsEffectAttribute() && !this->IsValidEffectAttr(pAttrib)))
					return pAttrib;
		}

		pAttrib = NodeAttribute::FindPrevAppliedAttr(pAttrib);
	}

	return NULL;
}


/********************************************************************************************

>	void NodeRenderableInk::DeleteAppliedAttributes() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function deletes all child attributes from the node which have the same 
				type and value as the attributes applied above the child level. It is useful for 
				Attribute optimisation work. 

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void NodeRenderableInk::DeleteAppliedAttributes()
{
	Node* pChild = FindFirstChild();
	Node* pFirstChild = pChild;

	// Temporarily unlink the children so that we can easily find applied attributes which
	// exclude the child attribs
	Child = NULL;
	
	NodeAttribute* pAppliedAttr;
	NodeAttribute* pChildAttr;
	while (pChild != NULL)
	{
		if (pChild->IsAnAttribute())
		{
			pChildAttr = (NodeAttribute*)pChild;
			if (FindAppliedAttribute((pChildAttr->GetAttributeType()), &pAppliedAttr))
			{
				if (pAppliedAttr)
				{
					// Test to see if the attributes have the same value
					if ((IS_SAME_CLASS(pAppliedAttr, pChildAttr )))
					{
						if ((*pAppliedAttr)==(*pChildAttr))
						{
							pChildAttr->CascadeDelete();		// Delete the attribute
							delete pChildAttr;
						}
					}
				}

			}
		}
		pChild = pChild->FindNext(); 
	}
	// Restore the children
	pChild = pFirstChild;	 		
} 

/// A function to return a set of the attribute types of all attribute children of
// the object. 

/********************************************************************************************

>	BOOL NodeRenderableInk::AddChildAttrTypesToSet(AttrTypeSet* pAttrTypeSet) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/95
	Inputs:		-
	Outputs:	pAttrTypeSet: The attribute types of all this nodes attributes are added to 
							  this set.
	Returns:	FALSE if we run out of memory
	Purpose:	This function adds the attribute types of all this nodes attributes to the
				AttrTypeSet. 

				If you are adding an object, which has localised attributes, to a compound node
				then this function is useful. The attrTypeSet will contain those attributes
				that need to be localised on the compound before the object is added, and those 
				that need to be factored out after. 

	Errors:		-
	SeeAlso:	-

********************************************************************************************/
		  
BOOL NodeRenderableInk::AddChildAttrTypesToSet(AttrTypeSet* pAttrTypeSet)
{
	Node* pChild;
	// Create a set of the caret's attribute types
	for (pChild = FindFirstChild(); 
		 pChild; 
		 pChild = pChild->FindNext())
	{
		if (pChild->IsAnAttribute())
		{
			// Add the attribute's type to the CaretsAttrTypes set
			if (!(pAttrTypeSet->AddToSet(((NodeAttribute*)pChild)->GetAttributeType())))
				return FALSE; // out of mem 
		}
	}
	return TRUE;
} 

/***********************************************************************************************

> BOOL NodeRenderableInk::CopyChildrenAsShapes(Node* pDestin)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		pDestin = a pointer to a tree node to place the shapes.
	Outputs:	-
	Returns:	TRUE if a copy of the children of this parent has been taken
				FALSE if failed.
	Purpose:	This function scans through all children of this object and asks them
				to convert themselves to shapes. The shapes are placed as children of the
				destination node. If the routine fails, all children of the destination will
				automatically be removed.

***********************************************************************************************/

BOOL NodeRenderableInk::CopyChildrenAsShapes(Node* pDestin)
{
	ERROR2IF(pDestin==NULL,FALSE,"pDestin==NULL in CopyChildrenAsShapes");

	// ok lets get the subtree to become a bunch of nodepaths
	CopyBecomeA ParamBecomeA(BECOMEA_PASSBACK, 
							 CC_RUNTIME_CLASS(NodePath),
							 NULL);
	
	// Set the context node
	ParamBecomeA.SetContextNode(pDestin);

	// now scan the children and get them to do their stuff
	Node* pNode = FindFirstChild();
	while (pNode!=NULL)
	{
		if (pNode->CanBecomeA(&ParamBecomeA))
		{
			if (!pNode->DoBecomeA(&ParamBecomeA))
			{
				pDestin->CascadeDelete();
				return FALSE;
			}
		}

		pNode=pNode->FindNext();
	}
	
	return TRUE;
}


/********************************************************************************************

> BOOL CopyBecomeA::PassBack(NodeRenderableInk* pNewNode,
							 NodeRenderableInk* pCreatedByNode,
							 CCAttrMap* pAttrMap)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		pNewNode 		= ptr to new generated node (should be a NodePath)
				pCreatedByNode 	= ptr to the node that generated this NodePath
				pAttrMap		= ptr attr map (NULL means get attrs applied to pCreatedByNode)
	Outputs:	-
	Returns:	-
	Purpose:	This is the func that receives the NodePaths generated by the last node asked
				to do becomea, it is a helper function associated with CopyChildrenAsShapes

********************************************************************************************/

BOOL CopyBecomeA::PassBack(NodeRenderableInk* pNewNode, NodeRenderableInk* pCreatedByNode, CCAttrMap* pAttrMap)
{
	ERROR2IF(pNewNode==NULL,FALSE,"pNewNode==NULL");
	ERROR2IF(pCreatedByNode==NULL,FALSE,"pCreateByNode==NULL");
	ERROR2IF(pContextNode == NULL,FALSE,"The context node is NULL!!!");

	// Insert the new NodePath into the tree
	pNewNode->AttachNode(pContextNode,LASTCHILD);

	if (pAttrMap!=NULL)
	{
		// Apply all the attributes to the new path
		BOOL ok = pNewNode->ApplyAttributes(pAttrMap);

/*
		// iterating all (key, value) pairs
		for (CCAttrMap::iterator Pos = pAttrMap->GetStartPosition(); Pos != NULL;)
		{
			void *pType, *pVal;
			pAttrMap->GetNextAssoc(Pos,pType,pVal);
			NodeAttribute* pNodeAttr = (NodeAttribute *)pVal;
			delete pNodeAttr;
		}
*/
		pAttrMap->DeleteAttributes();	// Delete the returned attrs, they're not needed
		delete pAttrMap;
		return ok;
	}
	else
	{
		// We need to get the attributes from the creater node.
		CCAttrMap AppliedAtts(30);
		if (!pCreatedByNode->FindAppliedAttributes(&AppliedAtts))
			return FALSE;

		if (!pNewNode->ApplyAttributes(&AppliedAtts))
			return FALSE;
	}	

	return TRUE;
}



BOOL NodeRenderableInk::ReadPostChildrenWeb(BaseCamelotFilter* pFilter)
{
	SafeToRender = NodeRenderableBounded::ReadPostChildrenWeb(pFilter);
	return SafeToRender;
}

BOOL NodeRenderableInk::ReadPostChildrenNative(BaseCamelotFilter* pFilter)
{
	return ReadPostChildrenWeb(pFilter);
}

BOOL NodeRenderableInk::AreYouSafeToRender()
{
	return SafeToRender;
}


/********************************************************************************************

>	virtual BOOL NodeRenderableInk::SetParentLayerAsEdited()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Returns:	True if worked ok, False otherwise
	Purpose:	Mark parent layer as edited. If an ink object as something done to it which
				changes its appearance then we need to mark the parent layer as edited.
				In this class we actually do the work as these are visible objects in the tree.

********************************************************************************************/

BOOL NodeRenderableInk::SetParentLayerAsEdited()
{
	// New 31/7/97 Neville
	// Mark the parent layer as edited as something has changed on it
	Layer * pLayer = (Layer*) FindParent(CC_RUNTIME_CLASS(Layer));
	if (pLayer)
	{
			// Note that this layer has been edited
			pLayer->SetEdited(TRUE);
#ifdef _DEBUG
			// Tell the frame gallery to update its display of the frame
			BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::REDRAW_LAYER));
#endif
	}

	return TRUE;
}



/********************************************************************************************

>	virtual BOOL NodeRenderableInk::IsSeeThrough(BOOL CheckIndirectAttrs)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13 September 2000

	Inputs:		CheckIndirectAttrs	whether or not to test those attributes applied
									to us living somewhere above us in the tree.

	Returns:	TRUE if you can still see some of the background after rendering the tree
				rooted at this node.

	Purpose:	Determines whether this node is see through or not.
				See through in this case does not necessarily mean transparent - eg we could
				be a no-colour-fill rectangle. This test is useful if you're going to render
				into a bitmap, which will then be post-processed and plonked on the document
				(eg Feathering). This way, you can hopefully avoid any performance hit from
				unnecessary rendering into an alpha-channel bitmap.

	Notes:		You should only need to set CheckIndirectAttrs when calling this fn
				on a sub-tree root-node. The recursive calls made by this fn pass FALSE,
				as we only need to check indirectly applied attrs once for a sub-tree.

********************************************************************************************/
BOOL NodeRenderableInk::IsSeeThrough(BOOL CheckIndirectAttrs)
{
	// we do the searches in this order:
	//
	//	1.	any attrs applied to us (not recursive).
	//	2.	ask any non-attribute children if they are see-through (recursive).
	//
	// we finish the search as soon as we get a positive result.

	BOOL bIsSeeThrough = FALSE;

	//	1.	Check our indirectly-applied attributes.
	if (CheckIndirectAttrs)
	{
		CCAttrMap* pAttrMap = CCAttrMap::MakeAppliedAttrMap(this);
		if (pAttrMap != NULL)
		{
			bIsSeeThrough = pAttrMap->IsSeeThrough();
			delete pAttrMap;
		}
	}

	//	2.	Check all our NodeRenderable children.
	//		Note that if we've checked indirect attrs,
	//		then we needn't bother re-testing any attributes.
	for (Node*	pKid = FindFirstChild();
				pKid != NULL && !bIsSeeThrough;
				pKid = pKid->FindNext() )
	{
		if (pKid->IsNodeRenderableClass())
		{
			if (CheckIndirectAttrs && pKid->IsAnAttribute())
			{
				// do nothing.
			}
			else
			{
				bIsSeeThrough = ((NodeRenderable*)pKid)->IsSeeThrough(FALSE);
			}
		}
	}

	return bIsSeeThrough;
}




/********************************************************************************************

>	virtual BOOL NodeRenderableBounded::ContainsNonMixTransparency(RenderRegion* pRegion)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/2004
	Inputs:		-
	Returns:	TRUE	- The node contains non-mix transparency attributes
				FALSE	- It doesn't
	Purpose:	Find out whether this node's subtree contains non-mix transparency.
				Useful for optimising cacheing because while you want things to be cached
				as 32BPP RGBT bitmaps most of the time, non-mix transparencies can't
				be captured that way and so some work ahs to be done to transform down
				to a 24BPP RGB bitmap - better to avoid that by calling this function.

********************************************************************************************/

BOOL NodeRenderableBounded::ContainsNonMixTransparency(RenderRegion* pRegion)
{
	for (Node*	pKid = FindFirstDepthFirst();
				pKid != NULL;
				pKid = pKid->FindNextDepthFirst(this) )
	{
		if (pKid->IsAnAttribute() && ((NodeAttribute*)pKid)->IsATranspFill())
		{
			AttrFillGeometry* pTransAttr = (AttrFillGeometry*) pKid;
			UINT32 ttype = pTransAttr->GetTranspType();
			ERROR3IF(ttype<TT_NoTranspType || ttype>TT_MAX, "Someone's trying to set an unknown transp type!");

//			if (!(ttype==TT_NoTranspType || ttype==TT_Mix))
			if (!pRegion->TranspTypeIsRGBTCompatible(ttype))
				return TRUE;
		}
	}

	return FALSE;
}




/********************************************************************************************

>	virtual BOOL NodeRenderableInk::ExportRender ( RenderRegion *pRender )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/00
	Inputs:		pRender	- A pointer to the render region being used for the export.
	Returns:	TRUE	- The node was exported using this function.
				FALSE	- There is no custom export code for this node type.
	Purpose:	Custom export code for nodes derived from NodeRenderableInk. So far there is
				only code for the VectorFileRenderRegion (which is a superclass for the EPS,
				CMX, and Flash render regions) to allow for the export of stroked shapes.

********************************************************************************************/

BOOL NodeRenderableInk::ExportRender ( RenderRegion *pRender )
{
	PORTNOTETRACE("other","NodeRenderableBounded::ExportRender - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// The return value.
	BOOL Result = FALSE;

	// Is this a FlashRenderRegion?
	if ( pRender->IsKindOf ( CC_RUNTIME_CLASS ( FlashRenderRegion ) ) )
	{
		// Cast the render region pointer to be a pointer to a vector file render region.
		FlashRenderRegion *pFlash = static_cast<FlashRenderRegion*> ( pRender );

		// Call the appropriate method in the vector file render region class.
		Result = pFlash->ExportRenderableNode ( this );
	}

	return Result;
#else
	return FALSE;
#endif
}

//------------------------------------------------------------------------------------------
//NodeHidden methods



/********************************************************************************************

>	NodeHidden::NodeHidden(Node* HiddenNode)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	NodeHidden constructor which hides HiddenNode
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

NodeHidden::NodeHidden(Node* HiddenNode): Node()
{  
	ENSURE(HiddenNode != NULL, "Trying to hide a NULL node"); 
	HiddenNd = HiddenNode;  		  	// Point to the node which is to be hidden    
	HiddenNode->IncHiddenCnt(); 	  	// Increase the count of the number of NodeHidden nodes which point 
									  	// to the node being hidden. 

	TAG tag = HiddenNd->GetTag();		// Copy tag so we can preserve it after the unlink from tree

	AttachNode(HiddenNode, NEXT); 
	HiddenNode->UnlinkNodeFromTree(); 	// Hide the node

	// Alert !
	// This is a bit scary but when doing things like complex deletions we need to find
	// the  parent of the hidden node when trying to factor out. I hope that this  
	// doesn't break to much !. 
	//HiddenNode->Parent = this;  

	HiddenNd->Tag = tag;				// Ensure the unlinked node has a preserved tag
}


/********************************************************************************************

>	Node* NodeHidden::ShowNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Inputs:		- 
	Outputs:	-
	Returns:	A pointer to the unhidden node 
	Purpose:	This static method will relink the hidden node into the tree. this  
				will be deleted. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Node* NodeHidden::ShowNode()
{
	Node* ShowNode = HiddenNd; // Pointer to hidden node 
	               
	//----------------------------------------------------------------------------
	// Relink the hidden node into the tree at the same position as the NodeHidden

	TAG tag = ShowNode->GetTag();		// Get the original tag
	ShowNode->AttachNode(this, NEXT);    
	ShowNode->Tag = tag;				// Restore tag value after relinking node into the tree

	ShowNode->DecHiddenCnt(); 		  // this hidden node no longer reffers to the node. 
	CascadeDelete();                  // Unlinks only the HideNode (It never has any children)
	delete (this); 
	return ShowNode; 
} 

/***********************************************************************************************
> Node* NodeHidden::SimpleCopy()  

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	Inputs:     -  
    Outputs:    
    Returns:    - 
		 
    Purpose:    Causes an ENSURE failure. A hidden node should never be copied 
	            
	Errors:                                                                          
                                                                                 
**********************************************************************************************/

Node* NodeHidden::SimpleCopy()
{
	ENSURE(FALSE, "NodeHidden invalid SimpleCopy called"); 
	return NULL; 
}   


/********************************************************************************************

>	virtual UINT32 NodeHidden::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes 	(this is the size of the NodeHidden only, it 
				does not include the size of the hidden subtree). 

	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodeHidden::GetNodeSize() const 
{     
	return (sizeof(NodeHidden)); 
}  

/********************************************************************************************

>	BOOL NodeHidden::IsNodeHidden()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/02/95
	Returns:	TRUE => This node is an instance of NodeHidden or a derived class.
				FALSE => otherwise.
	Purpose:	Determine if a node is *derived* from the NodeHidden class.

********************************************************************************************/

BOOL NodeHidden::IsNodeHidden() const
{
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL NodeHidden::IsOrHidesAnAttribute() const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/95
	Returns:	TRUE if this Hidden Node hides an Attribute, FALSE otherwise
	Purpose:	Enables to determine if this node is an Attribute or is hiding an attribute

********************************************************************************************/

BOOL NodeHidden::IsOrHidesAnAttribute() const
{
	// if it is NULL, then its not an attribute
	if (HiddenNd==NULL)
		return FALSE;

	// see if it is an attribute
	return (HiddenNd->IsAnAttribute());
}
