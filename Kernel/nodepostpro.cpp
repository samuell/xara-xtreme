// $Id: nodepostpro.cpp 662 2006-03-14 21:31:49Z alex $
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
*/

#include "camtypes.h"
#include "app.h"
#include "node.h"
#include "nodepostpro.h"

// Resource headers
//#include "resource.h"
//#include "mario.h"
#include "lineattr.h"
#include "fillattr.h"

DECLARE_SOURCE( "$Revision: 662 $" );

CC_IMPLEMENT_DYNCREATE(NodeEffect, NodeCompound)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/***********************************************************************************************

>	NodeEffect::NodeEffect(Node* 	ContextNode,
						AttachNodeDirection Direction,
						const DocRect& 		BoundingRect,
						BOOL				Locked = FALSE,
						BOOL 				Mangled = FALSE,
						BOOL 				Marked = FALSE,
						BOOL 				Selected = FALSE,
						BOOL 				Renderable = FALSE
						)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
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
				direction specified by Direction. All neccesary tree links are updated.
	Note:		SetUpShape() must be called before the NodeRegularShape is in a state in which
				it can be used.
    SeeAlso:	NodeRegularShape::SetUpShape
    Errors:		An ENSURE will occur if ContextNode is NULL

***********************************************************************************************/
NodeEffect::NodeEffect(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ) : NodeCompound(ContextNode, Direction, Locked, Mangled, Marked, Selected )
{                         
	m_strPostProID = String(_T(""));
}                        
 
/*********************************************************************************************

>	NodeEffect::NodeEffect() 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Purpose:	
	Note:		
	SeeAlso:	

**********************************************************************************************/
NodeEffect::NodeEffect() : NodeCompound()
{
	m_strPostProID = String(_T(""));
}

/*********************************************************************************************

>	NodeEffect::~NodeEffect() 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Purpose:	Destructor
	Note:		
	SeeAlso:	

**********************************************************************************************/
NodeEffect::~NodeEffect()
{
}




/********************************************************************************************

>	SubtreeRenderState NodeEffect::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Inputs:		pRender - The region render into (or use as context for decisions about subtree)
				ppNextNode - Address of node pointer for next node to render or run to after this
				bClip - flag indicating whether to clip or not
	Purpose:	Do clever stuff on the way into a subtree, possibly modifying rendering
				behaviour.

********************************************************************************************/

SubtreeRenderState NodeEffect::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
	if (pRender == NULL)							// If no render region supplied, assume we need to be rendered
		return SUBTREE_ROOTANDCHILDREN;

	// Go find out about my bounding rectangle
	DocRect BoundingRect = GetBoundingRect();
	DocRect ClipRect = pRender->GetClipRect();

	if (bClip && !ClipRect.IsIntersectedWith(BoundingRect))	// If not within the clipping rect then
		return SUBTREE_NORENDER;					// Don't render us or our children

	// Ask Helper function to set up cacheing for me...
// AMB wonders whethere this was commented out during porting - 20051212
//	if (RenderCached(pRender))						// If we can find a cached bitmap for this node and render it
//	{
//		return SUBTREE_NORENDER;					// Then tell the renderer to move on without doing any more...
//	}

	return SUBTREE_ROOTANDCHILDREN;					// Else we must render ourselves and our children
}



/********************************************************************************************

>	void NodeEffect::RenderAfterSubtree(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Inputs:		pRender - The region to render into
	Purpose:	Capture the group as a cached bitmap

********************************************************************************************/

void NodeEffect::RenderAfterSubtree(RenderRegion* pRegion)
{
	// Deal with group transparency capture
	// Call Helper function to run all my cacheing functionality for me...
//	CaptureCached(pRegion);
}




/***********************************************************************************************
> Node* NodeEffect::SimpleCopy()  

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    09/09/2004
	Inputs:     -  
    Outputs:    
    Returns:    A copy of the node, or NULL if memory has run out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	            
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL.                                                                      
                                                                                 
**********************************************************************************************/

Node* NodeEffect::SimpleCopy()
{
	NodeEffect* NodeCopy; 
	NodeCopy = new NodeEffect();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);   		
	return (NodeCopy);
}   



   
/***********************************************************************************************

>	void NodeEffect::CopyNodeContents(Node* pCopyOfNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/09/2004
	Inputs:		pCopyOfNode - The node to copy data to
	Outputs:	-
	Returns:	-
	Purpose:	Copies the data from this node to pCopyOfNode by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeGroup::CopyNodeContents

***********************************************************************************************/

void NodeEffect::CopyNodeContents(NodeEffect* pCopyOfNode)
{
	NodeCompound::CopyNodeContents(pCopyOfNode);

	// Copy member vars here
	pCopyOfNode->m_strPostProID = m_strPostProID;
}




/***********************************************************************************************
>   void NodeEffect::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    09/09/2004
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeEffect::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeEffect), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeEffect))
		CopyNodeContents((NodeEffect*)pNodeCopy);
}




/********************************************************************************************

>	void NodeEffect::IsValidEffectAttr(CCRuntimeClass* pAttrClass) const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/2005
	Inputs:		Attribute class
	Returns:	TRUE if the attr class is a valid effect attr
	Purpose:	Determine whether this type of attribute can be an effect attribute
				On this node at this time.

********************************************************************************************/

/*BOOL NodeEffect::IsValidEffectAttr(CCRuntimeClass* pAttrClass) const
{
	return (AttrFillGeometry::s_bGroupTransparency &&
			(pAttr->IsATranspFill() ||
			 pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrTranspChange)) ||
			 pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeTranspChange)
			 )
			);
}*/

	
	
	
/********************************************************************************************

>	void NodeEffect::IsValidEffectAttr(NodeAttribute* pAttr) const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/2005
	Inputs:		Attribute
	Returns:	TRUE if the attr is a valid effect attr
	Purpose:	Determine whether this attribute instance can be an effect attribute
				On this node at this time.

********************************************************************************************/

BOOL NodeEffect::IsValidEffectAttr(NodeAttribute* pAttr) const
{
	CCRuntimeClass* pAttrType = pAttr->GetAttributeType();

	return (pAttr->IsATranspFill() ||
			pAttrType == CC_RUNTIME_CLASS(AttrTranspFillGeometry) || //->IsKindOf(CC_RUNTIME_CLASS(AttrTranspChange)) ||
			pAttrType == CC_RUNTIME_CLASS(AttrStrokeTransp) || //pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeTranspChange)) ||
			pAttrType == CC_RUNTIME_CLASS(AttrTranspFillMapping)
			);
}




