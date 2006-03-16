// $Id: nodeattr.cpp 662 2006-03-14 21:31:49Z alex $
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
#include "nodeattr.h"
//#include "mario.h"
//#include "simon.h"
#include "errors.h"
#include "ensure.h"
//#include "doccolor.h"
#include "colcomp.h"
#include "basedoc.h"
#include "blendatt.h"
#include "ink.h"		// for EffectsParentBounds(NodeRenderableInk*)
#include "nodedoc.h"
#include "ops.h"
#include "undoop.h"
#include "layer.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNAMIC(NodeAttribute, NodeRenderable)

/********************************************************************************************

>	NodeAttribute::NodeAttribute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for Node Attribute class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

NodeAttribute::NodeAttribute() : NodeRenderable()
{
}

/********************************************************************************************

>	NodeAttribute::NodeAttribute(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			  ): Node(ContextNode, Direction, Locked, Mangled, Marked, Selected, TRUE)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Who knows ?
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

NodeAttribute::NodeAttribute(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			  ): NodeRenderable(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 


/*********************************************************************************************

>    BOOL NodeAttribute::IsAnAttribute(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a NodeAttribute, will return TRUE
     Purpose:   Virtual function for determining if the node is an attribute
     Errors:    

**********************************************************************************************/
       
BOOL NodeAttribute::IsAnAttribute() const
{ 
	return TRUE;  
} 


/********************************************************************************************

>	virtual BOOL NodeAttribute::IsOrHidesAnAttribute() const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/95
	Returns:	TRUE, as this is an attribute
	Purpose:	To see if this node is an attribute, or is hiding an attribute (NodeHiddens)

********************************************************************************************/

BOOL NodeAttribute::IsOrHidesAnAttribute() const
{
	return TRUE;
}


/*********************************************************************************************

>    BOOL NodeAttribute::IsAValueChange(void) const

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/2/95
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is an AttrValueChange, will return TRUE
     Purpose:   Virtual function for determining if the node is a value change attribute
     Errors:    

**********************************************************************************************/
       
BOOL NodeAttribute::IsAValueChange() const
{ 
	return FALSE;  
} 

/*********************************************************************************************

>    BOOL NodeAttribute::IsAColourFill(void) const

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/2/95
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a AttrColourFill, will return TRUE
     Purpose:   Virtual function for determining if the node is a Colour Fill attribute
     Errors:    

**********************************************************************************************/
       
BOOL NodeAttribute::IsAColourFill() const
{ 
	return FALSE;  
} 

/*********************************************************************************************

>    BOOL NodeAttribute::IsATranspFill(void) const

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/2/95
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a NodeAttribute, will return TRUE
     Purpose:   Virtual function for determining if the node is an attribute
     Errors:    

**********************************************************************************************/
       
BOOL NodeAttribute::IsATranspFill() const
{ 
	return FALSE;  
} 

/*********************************************************************************************

>    BOOL NodeAttribute::IsAStrokeColour(void) const

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/2/95
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a NodeAttribute, will return TRUE
     Purpose:   Virtual function for determining if the node is an attribute
     Errors:    

**********************************************************************************************/
       
BOOL NodeAttribute::IsAStrokeColour() const
{ 
	return FALSE;  
} 

/*********************************************************************************************

>    BOOL NodeAttribute::IsAStrokeTransp(void) const

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/2/95
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a NodeAttribute, will return TRUE
     Purpose:   Virtual function for determining if the node is an attribute
     Errors:    

**********************************************************************************************/
       
BOOL NodeAttribute::IsAStrokeTransp() const
{ 
	return FALSE;  
} 

/*********************************************************************************************

>    BOOL NodeAttribute::IsAFlatFill(void) const

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/2/95
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a NodeAttribute, will return TRUE
     Purpose:   Virtual function for determining if the node is an attribute
     Errors:    

**********************************************************************************************/
       
BOOL NodeAttribute::IsAFlatFill() const
{ 
	return FALSE;  
} 
/*********************************************************************************************

>    BOOL NodeAttribute::IsAGradFill(void) const

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/2/95
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a NodeAttribute, will return TRUE
     Purpose:   Virtual function for determining if the node is an attribute
     Errors:    

**********************************************************************************************/
       
BOOL NodeAttribute::IsAGradFill() const
{ 
	return FALSE;  
} 

/*********************************************************************************************

>    BOOL NodeAttribute::IsABitmapFill(void) const

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/2/95
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a NodeAttribute, will return TRUE
     Purpose:   Virtual function for determining if the node is an attribute
     Errors:    

**********************************************************************************************/
       
BOOL NodeAttribute::IsABitmapFill() const
{ 
	return FALSE;  
} 
/*********************************************************************************************

>    BOOL NodeAttribute::IsAFractalFill(void) const

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/2/95
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a NodeAttribute, will return TRUE
     Purpose:   Virtual function for determining if the node is an attribute
     Errors:    

**********************************************************************************************/
       
BOOL NodeAttribute::IsAFractalFill() const
{ 
	return FALSE;  
} 

/*********************************************************************************************

>    BOOL NodeAttribute::IsALineWidthAttr() const
     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/2/95
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a line width attribute, will return TRUE
     Purpose:   Virtual function for determining if the node is a line width attribute
     Errors:    

*************************************************************************************************/

BOOL NodeAttribute::IsALineWidthAttr() const
{
	return FALSE;
}

/*********************************************************************************************

>    virtual BOOL NodeAttribute::NeedsToRenderAtEachBrushStroke()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   29/11/99
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if this attribute should be rendered at every step of a brush stroke
     Purpose:   So that don't have to keep re-rendering attributes whilst drawing a brush, this
				identifies whether or not the attribute need to be rendered at each step, 
				e.g. radial fills.
     Errors:    
	 See Also;  Brush code (ndbrshmk.cpp)

**********************************************************************************************/
       
BOOL NodeAttribute::NeedsToRenderAtEachBrushStroke() const
{
	return FALSE;
}



/*********************************************************************************************

>    BOOL NodeAttribute::EffectsParentBounds() const

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   4/4/95
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node will effect the bounds of it's parent.
     Purpose:   Virtual function for determining if the node will effect it's parent bounds.
	 			eg. ArrowHeads.
	 SeeAlso:	NodeAttribute::GetAttrBoundingRect

**********************************************************************************************/
       
BOOL NodeAttribute::EffectsParentBounds()
{
	return FALSE;
}

/*********************************************************************************************

>    DocRect NodeAttribute::GetAttrBoundingRect(NodeRenderableInk* pParent)

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   4/4/95
     Inputs:    pParent, the parent Ink node that this attribute will effect.
     Outputs:   -
     Returns:   The Bounding rect of the attribute (NULL rectangle is it doesn't have any).
     Purpose:   Virtual function for determining the bounding rect of an attribute.
	 			eg. ArrowHeads.
	 SeeAlso:	NodeAttribute::EffectsParentBounds

**********************************************************************************************/
       
DocRect NodeAttribute::GetAttrBoundingRect(NodeRenderableInk* pParent, CCAttrMap* pAttribMap)
{
	return DocRect(0,0,0,0);
}

/********************************************************************************************

>	BOOL NodeAttribute::NeedsToRender( RenderRegion *pRender )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/94
	Inputs:		pRender - A pointer to the current render region
	Returns:	TRUE  => This node should be rendered,
				FALSE => This node does not need to be rendered.
	Purpose:	Virtual function - this version always returns TRUE, which indicates to
				the caller that we always want to render nodes of this type if we have
				encountered them.
	SeeAlso:	Node::NeedsToRender

********************************************************************************************/

SubtreeRenderState NodeAttribute::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
	// If we've found an attribute node, then we always want to render it.
	return SUBTREE_ROOTONLY;
}

/********************************************************************************************

>	BOOL NodeAttribute::NeedsToExport(RenderRegion* pRender, BOOL VisibleLayersOnly = FALSE,
									  BOOL CheckSelected = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/94
	Inputs:		pRender - A pointer to the current export region
				VisibleLayersOnly - TRUE => remove nodes which are on invisible layers
								   - FALSE => export everything
				CheckSelected - TRUE => we check if object selected and only export selected bjects
							  - FALSE => we don't bother checking for selection or not
	Returns:	TRUE => please export me.
	Purpose:	Virtual function - this version always returns TRUE, because we always
				want to export attributes.
	SeeAlso:	NodeAttribute::NeedsToRender

********************************************************************************************/

BOOL NodeAttribute::NeedsToExport(RenderRegion* pRender, BOOL VisibleLayersOnly,
								  BOOL CheckSelected)
{
	return TRUE;
}

/***********************************************************************************************
> Node* NodeAttribute::SimpleCopy() 

    Author:     Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:    21/6/93
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 

	Scope:      protected       
***********************************************************************************************/

Node* NodeAttribute::SimpleCopy()
{
	NodeAttribute* NodeCopy = new NodeAttribute();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 NodeAttribute::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		Atrib: The attribute to compare 
	Outputs:	-
	Returns:	-
	Purpose:	This function is slightly weird. Unlike most definitions of operator== it's
				virtual. 

				To see why it's required, consider the following 
               
				NodeAttribute* A,B; 

				A = X; 
				B = Y; 

				where X and Y are pointers to instances of derived 
				classes of NodeAttribute. 

				if X->GetRuntimeClass() == Y->GetRuntimeClass() then we should be 
				able to compare X and Y. But if we had defined a normal operator== 
				fn then we find we cannot do this, because there is no way of casting 
				to a type which is unknown at compile time. 

				The implementation of operator== in all derived classes of NodeAttribute
				should look something like this. 

				INT32 DerivedClass::operator==(const NodeAttribute& Attrib)
				{
					DerivedClass* Attr = (DerivedClass*) &Attrib;  

					// Compare member vars with Attr
				}

				Before calling this function you must be sure that the attributes
				have the same runtime class. 

				The function needs to be defined in all derived classes of NodeAttribute

	Errors:		If this function is called on an instance of a derived class of
				NodeAttribute, and the function has not been defined for the derived
				class, then an ENSURE failure will result. 
				 
	SeeAlso:	-

********************************************************************************************/

INT32 NodeAttribute::operator==(const NodeAttribute& Attrib)
{
	ENSURE(FALSE, "the pure virtual operator== fn has not been defined"); 
	return (FALSE); 
}



/********************************************************************************************

>	virtual BOOL NodeAttribute::IsDifferent(Node *pOther)

 	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/97

	Inputs:		pOther - The node to compare this one to

	Returns:	TRUE if this is considered different from pOther,
				FALSE if they are the same

	Purpose:	Determine if 2 nodes are considered different.

	Notes:		NodeAttribute uses its own (and derived class) operator== method
				to determine if the nodes are different. This will work for ANY
				derived attribute class.

********************************************************************************************/

BOOL NodeAttribute::IsDifferent(Node *pOther)
{
	// First, check with the base class - this checks the classes are the same type
	if (NodeRenderable::IsDifferent(pOther))
		return(TRUE);

	ERROR3IF(GetRuntimeClass() != pOther->GetRuntimeClass(),
			"Node::IsDifferent base class method seems to have been broken");

	// Check if different using the operator== which is defined from NodeAttribute onwards
	if (*this == *((NodeAttribute *)pOther) )
		return(FALSE);

	return(TRUE);
}



/********************************************************************************************

>	virtual UINT32 NodeAttribute::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute, this base 
				class function returns the resource _R(IDS_ATTRIBUTE).

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 NodeAttribute::GetAttrNameID(void)  
{
	return (_R(IDS_ATTRIBUTE)); 
}               

/***********************************************************************************************
>   void NodeAttribute::CopyNodeContents(NodeAttribute* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void NodeAttribute::CopyNodeContents( NodeAttribute* NodeCopy)
{
	NodeRenderable::CopyNodeContents( NodeCopy );
} 


#ifdef _DEBUG
/***********************************************************************************************

>	void NodeAttribute::ShowDebugTreeDetails() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Displays debugging info of the tree

***********************************************************************************************/

void NodeAttribute::ShowDebugTreeDetails() const
{
	// Display a bit of debugging info
	// For now, we will just call the base class version
	Node::ShowDebugTreeDetails();	
}
#endif


/********************************************************************************************

>	DocColour *NodeAttribute::EnumerateColourFields(UINT32 Context)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/07/94
	Inputs:		Context - indicates which colour field should be returned next.
	Returns:	Pointer to the next colour field - this may well be altered by the caller.
	Purpose:	Enumerates all the colour fields used by an attribute.  This enables the
				colour manager to scan the tree and process any attribute it finds to carry
				out operations such as turning indexed colours into immediate colours, and
				so on.
				The Context parameter indicates which colour field should be returned 
				next.  It is 0 initially, then it will be 1, then 2, etc.  The NodeAttribute
				object should use this to deduce how far it has got through its colour
				fields, and return the next one, or NULL if there are no more.
				The order in which the colours are returned is not important, as long
				as EVERY colour in the attribute is returned ONCE and ONLY once.  Other
				than this stipulation, the NodeAttribute can amuse itself in whatever
				foul and depraved manner it deems appropriate.

********************************************************************************************/

DocColour *NodeAttribute::EnumerateColourFields(UINT32 Context)
{
	// No colours in a standard NodeAttribute.
	return NULL;
}

/********************************************************************************************

>	virtual BOOL NodeAttribute::CopyComponentData(BaseDocument* SrcDoc,  BaseDocument* NodesDoc) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/94
	Inputs:		SrcDoc:   The document from where this node was copied
				NodesDoc: The document where this node lives 
	Outputs:	-
	Returns:	FALSE if unable to copy data
	Purpose:	If the attribute contains any DocColours which are indexed then a copy
				of the indexed colour is made and added to the NodesDoc
				ColourListComponent.
	Errors:		-
	SeeAlso:	NodeRenderable::CopyComponentData

********************************************************************************************/


BOOL NodeAttribute::CopyComponentData(BaseDocument* SrcDoc, BaseDocument* NodesDoc)
{
	// Ask the base class to copy its data
	if (!NodeRenderable::CopyComponentData(SrcDoc, NodesDoc))
	{
		return FALSE; // Failed
	}
	// Get the colour list component
	ColourListComponent *pComponent = 
		(ColourListComponent *) NodesDoc->GetDocComponent(CC_RUNTIME_CLASS(ColourListComponent));

	ENSURE (pComponent != NULL, "Could not find ColourListComponent");

	// Copy across all DocColours
	for (INT32 i=0;;i++)
	{
		DocColour* pDocCol = EnumerateColourFields(i); 
		if (pDocCol == NULL)
		{
			// there are no more colours to copy
			break;
		}
		// Copy the colour across 
		if (pComponent->CopyColourAcross(pDocCol) == CCCOPY_FAILED)
		{
			return FALSE; // Failed to copy colour info
		} 
	}
	return TRUE; 
}
 



/********************************************************************************************
>	static NodeAttribute* NodeAttribute::FindFirstAppliedAttr(Node* pContextNode, Node* pRoot = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/01/2005
	Inputs:		-
	Outputs:	-
	Returns:	Pointer to attribute which is closest in scope to the context node.
	Purpose:	Find the attribute which is closest in attribute scope to the context node.
				Use in conjunction with FindPrevAppliedAtt.
				Note!
				This routine and FindPrevAppliedAttr will return "Effect Attributes" that
				only apply to pContextNode, and not its children. You should test for them
				using IsEffectAttribute outside this function if you don't want to find them.
	Errors:		-
	SeeAlso:	NodeAttribute::FindPrevAppliedAtt
********************************************************************************************/

NodeAttribute* NodeAttribute::FindFirstAppliedAttr(Node* pContextNode, Node* pRoot)
{
	ERROR2IF(pContextNode == NULL, NULL, "FindFirstAppliedAttr() called on NULL");
	Node* pNode = NULL;

	// Begin the attribute search from the right-most child of "this" node.
	// (So that Effect Attributes are picked up)
	pNode = pContextNode->FindLastChild();
	if (pNode == NULL) pNode = pContextNode;

	// If the very first node is an attribute, great, return that...
	if (pNode->IsAnAttribute() && !pNode->IsAClipViewAttr())
		return ((NodeAttribute*) pNode);

	// Otherwise must search in reverse render order for an attribute...
	return FindPrevAppliedAttr(pNode, pRoot);
}




/********************************************************************************************
>	static NodeAttribute* NodeAttribute::FindPrevAppliedAttr(Node* pContextNode, Node* pRoot = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/01/2005
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the attribute which occurs "previous" to this node.
	Purpose:	Finds the previous attribute in the tree, from this node, by searching
				backwards and up (along Previous and Parent links), or null if there is no
				such attribute.
				Note!
				This routine and FindFirstAppliedAttr will return "Effect Attributes" that
				only apply to pContextNode, and not its children. You should test for them
				using IsEffectAttribute outside this function if you don't want to find them.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

NodeAttribute* NodeAttribute::FindPrevAppliedAttr(Node* pContextNode, Node* pRoot)
{
	ERROR2IF(pContextNode == NULL, NULL, "FindPrevAppliedAttr() called on NULL");

	// We are going to start from this attribute node.
	// All attributes are collected together as the first nodes in a sub-tree, so we can
	// call FindPrevious here as the previous node will either be NULL or another attribute.
	Node* pNode = pContextNode;
	
	// Search for the next attribute
	do
	{
		// Get the previous node
		if (pNode->FindPrevious())
			pNode = pNode->FindPrevious();
		else
		{
			// Move up to sibling list above
			pNode = pNode->FindParent();

			// --------------------------------------
			// Optimisation!
			// There are often a small number of attributes at the start of a large sibling
			// list. So it's often quicker to skip to the start and then skip forward from there
			// (This clause can be removed without changing the basic operation of this
			// function.)
			if (pNode==pRoot) return NULL;

			// Quick test to see whether attr is readily available in new sibling list
			// (Assume that paper node tree section is quicker to scan the obvious way)
			Node* pTestNode = pNode->FindPrevious();
			if (!(pNode->IsPaper() || (pTestNode && pTestNode->IsOrHidesAnAttribute())))
			{
				Node* pAttr = NULL;											// Init the attr pointer we're searching for
				pTestNode = pNode;
				while (pTestNode && pAttr==NULL)							// Start looping up through sibling lists til we find an attr
				{
					Node* pParent = pTestNode->FindParent();				// Find first node in this sibling list
					pTestNode = pParent ? pParent->FindFirstChild() : NULL;

					while (pTestNode && pTestNode->IsOrHidesAnAttribute())	// Loop through nodes in this sib list until find a non-attr
					{
						pAttr = pTestNode;									// Remember the last attr we've found
						pTestNode = pTestNode->FindNext();					// Test next node in this sibling list
					}

					pTestNode = pParent;									// Find parent sibling list
				}

				if (pAttr)													// If we found an attribute
					pNode = pAttr;											// return it
																			// (otherwise leave pNode unchanged and allow normal
																			//  algorithm to work out how to handle it)
			}
			// --------------------------------------
		}

		if (pNode && pNode->IsAnAttribute() && !pNode->IsAClipViewAttr())
			return ((NodeAttribute*) pNode);
	}
	while (pNode!=pRoot);

	return NULL;
}


/********************************************************************************************
>	BOOL NodeAttribute::HasEquivalentDefaultValue(BOOL bAppearance = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/03/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node has a value equivalent to the relevant 
				FALSE otherwise
	Purpose:	Determine whether this attribute has the default value or not
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL NodeAttribute::HasEquivalentDefaultValue(BOOL bAppearance)
{
	AttributeValue* pDefaultAttrVal = NULL;
//	CCRuntimeClass* AttrType = GetAttributeType();

	// Look for the default attr directly
	// (But this doesn't work for those attrs which are grouped together
	//	and share a common default value - e.g. fills)
	AttrIndex attrid = GetAttributeIndex();
	if (attrid!=ATTR_BAD_ID && attrid<ATTR_FIRST_FREE_ID)
		pDefaultAttrVal = AttributeManager::GetDefaultAttributeVal(attrid);

	if (pDefaultAttrVal && !GetAttributeValue()->IsDifferent(pDefaultAttrVal))
	{
		// Just allow the node to be hidden
		return TRUE;
	}

	return FALSE;
}




/********************************************************************************************
>	BOOL NodeAttribute::Blend(BlendAttrParam* pBlendParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/94
	Inputs:		pBlendParam = ptr to a blend attr class object
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This will blend this attribute using the data held in pBlendParam.
				This base version does this:
					If the blend ratio <= 50%, this attr is copied
					If the blend ratio >  50%, the other attr is copied
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL NodeAttribute::Blend(BlendAttrParam* pBlendParam)
{
	// Check NULL entry param
	ERROR3IF(pBlendParam == NULL,"pBlendParam == NULL");
	if (pBlendParam == NULL) return FALSE;

	// Make a copy of this node, and make this the blended attribute
	NodeAttribute* pBlendedAttr = NULL;
	NodeAttribute* pAttrToCopy  = NULL;

	if (pBlendParam->GetBlendRatio() <= 0.5)
		pAttrToCopy = this;
	else
		pAttrToCopy = pBlendParam->GetOtherAttr();

	if (pAttrToCopy != NULL)
	{
		pBlendedAttr = (NodeAttribute*) pAttrToCopy->SimpleCopy();
		pBlendParam->SetBlendedAttr(pBlendedAttr);
	}

	// Return TRUE if we were able to make a copy of this node
	return (pBlendedAttr != NULL);
}


/********************************************************************************************

>	BOOL NodeAttribute::IsADefaultAttr()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/95
	Returns:	TRUE if attribute is a default attribute. i.e. it's parent is a NodeDocument
	Purpose:	Simple but useful fn to determine if the attribute (which must be in the tree) 
				is a default attribute

********************************************************************************************/


BOOL NodeAttribute::IsADefaultAttr()
{ 
	return (IS_A(FindParent(), NodeDocument)); 
}


/********************************************************************************************

>	BOOL NodeAttribute::IsEffectAttribute() const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/2005
	Returns:	TRUE if attribute is an Effect attribute. i.e. it's not in the usual place
				in the tree, it's at the end of a child list with bounded nodes ahead of it.

********************************************************************************************/

BOOL NodeAttribute::IsEffectAttribute() const
{ 
//	Node* pSibling = FindPrevious(CC_RUNTIME_CLASS(NodeRenderableBounded));
	Node* pSibling = FindPrevious();
	while (pSibling && !pSibling->IsBounded())
		pSibling = pSibling->FindPrevious();

	return (pSibling!=NULL);
}


/********************************************************************************************

>	BOOL NodeAttribute::ShouldBecomeCurrent()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96
	Returns:	TRUE if this attribute should become current.

				FALSE if the user must deliberately apply this attribute to an object
				every time he wants to use it.

	Purpose:	The attribute manager calls this function before making an attribute current.

				There are some attributes - like this URL Hot Link attribute I'm about
				to put in - that the user probably won't want to become current.
				For example, if a user puts a HotLink on an object and happens to have
				the Give Other Objects Most Recent Attributes option on, all his new objects are
				going to have HotLinks. That's not good.

				So those attributes will override this function and return FALSE.

********************************************************************************************/


BOOL NodeAttribute::ShouldBecomeCurrent()
{ 
	//Yes, we'd like this attribute to be passed to other objects.
	return TRUE; 
}



/********************************************************************************************
>	NodeAttribute* NodeAttribute::FindNextAttr(TypeFunc pfnTest) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95
	Inputs:		pfnTest			---		pointer to a member-function of Node:-
										typedef BOOL (Node::*TypeFunc)() const;
										that returns TRUE for the attribute being
										searched for and FALSE otherwise
	Returns:	The next attribute of the given type from this one that applies to the
				object Node::FindFirstAttr was called for, or null if there
				isn't one.
	Purpose:	Finds the next attribute of the given type that applies to the object.
	SeeAlso:	Node::FindFirstAttr
********************************************************************************************/

NodeAttribute* NodeAttribute::FindNextAttr(TypeFunc pfnTest) const
{
	// Search for the class in the previous sibling nodes, then the parent's siblings,
	// then the parent siblings of the parent of ... etc.
	const Node* pNode = this;
	do {
		// Work out the next node in the search order.
		if (pNode->FindPrevious() != 0)
		{
			// The next node is the previous sibling.
			pNode = pNode->FindPrevious();
		}
		else
		{
			// The next node is the parent, if there is one.
			pNode = pNode->FindParent();
			if (pNode == 0) break;
		}
	} while (!(pNode->*pfnTest)());

	// Return the attribute, or null if none was found.
	return (NodeAttribute*) pNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// New extensions to the attribute system - initially created to support feathering attribute
/////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************
>	virtual BOOL NodeAttribute::IsLinkedToNodeGeometry()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/00
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	This test determines whether the attribute is linked to the geometry of the 
				objects it has been applied to. This condition is an important precursor before
				performing extra initialisation tests, and passing operation messages around to
				ensure that the attribute is up to date.
 ********************************************************************************************/
BOOL NodeAttribute::IsLinkedToNodeGeometry()
{
	return FALSE;
}


/********************************************************************************************
>	virtual BOOL NodeAttribute::IsLinkedToThisNode(Node* pNode)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/00
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	This test determines whether the attribute is linked to the geometry of the 
				supplied object
 ********************************************************************************************/
BOOL NodeAttribute::IsLinkedToThisNode(Node* pNode)
{
	return FALSE;
}

/********************************************************************************************
>	virtual BOOL NodeAttribute::PostDynCreateInit(CCAttrMap* pMap, Path* InkPath, 
												   CCRuntimeClass* pCreatorClass)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/00
	Inputs:		pMap		- the attr map which contains the dynamically generated attribute 
							  we are trying to initialise
				InkPath		- the inkpath we are going to render in this blend/contour step
							- NB also have a Node* version below
				pCreatorClass	-	Allows the attribute to modify it's behaviour depending on
									which complex node has asked to create a dynamic attr
	Outputs:	
	Returns:	
	Purpose:	Blends and contours create attribute maps which are dynamically blended
				when the compound is rendered.
				Certain attributes cannot function correctly without having links to the
				objects which they apply to.
				eg feather attributes need the path outline of the objects in order to create
				a silhouette image from which the feather bitmap is created
				In order for these attributes to do their job they require an extra initialisation
				step after dynamic creation.
				NB only required when NodeAttributes in an CCAttrMap is copied or blended (ie 
				dynamically created)
				NOT required if the attribute is going to be inserted into the tree before being 
				rendered (because then it can acquire all the information it needs, about the node
				it is attached to, via the NodeAttribute which contains in - NB this assertion
				requires the associated NodeAttribute's Render() function to feed the attribute
				this information before asking it to render (ie by calling NodeAttribute::Render())).
	Errors:		May return FALSE if memory allocations fail.
				In this case the attribute will not be able to acheive it's affect.
				The caller must be careful not to ask the attribute to render (eg by removing it from the
				CCAttrMap before calling pMap->Render())
	SeeAlso:	NodeAttribute::PostDynCreateInit(CCAttrMap* pMap, ___Node* pNode____,
				CCRuntimeClass* pCreatorClass)
 ********************************************************************************************/
BOOL NodeAttribute::PostDynCreateInit(CCAttrMap* pMap,
									   Path* InkPath,
									   CCRuntimeClass* pCreatorClass)
{
	return TRUE;
}

/********************************************************************************************
>	virtual BOOL NodeAttribute::PostDynCreateInit(	CCAttrMap* pMap,
											   Node* pNode,
											   CCRuntimeClass* pCreatorClass)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/00
	Inputs:		as above except:-
				pNode	- This variety allows the caller to supply the attr with a node which it
				would like the attribute to apply itself to
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	SeeAlso:	NodeAttribute::PostDynCreateInit(CCAttrMap* pMap, ___Path* InkPath____,
				CCRuntimeClass* pCreatorClass)
 ********************************************************************************************/
BOOL NodeAttribute::PostDynCreateInit(CCAttrMap* pMap,
									   Node* pNode,
									   CCRuntimeClass* pCreatorClass)
{
	return TRUE;
}

/********************************************************************************************
>	virtual void NodeAttribute::PostDynCreateDeInit()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/00
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Free memory allocated by PostDynCreateInit(..) functions
	Errors:		No memory needs to be allocated so cannot fail.
	SeeAlso:	-
********************************************************************************************/
void NodeAttribute::PostDynCreateDeInit()
{
}

/********************************************************************************************
>	virtual BOOL NodeAttribute::LinkedNodeGeometryHasChanged(UndoableOperation* pOp)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/00
	Inputs:		pOp - the operation that has caused the children nodes to change their
					  geometry
	Outputs:	
	Returns:	
	Purpose:	Attributes which are linked to the geometry of the objects which they affect,
				need to receive notification when these objects are changed by other
				operations
				eg if a group of objects is featherd, then the feather attribute applied at 
				group level needs to be informed when any operations are performed on the
				children which will change their outline
	Tech Notes:	NB NB NB
				This function gets called as part of the AllowOp process where an operation
				informs the rest of the tree that it is about to make a modification of some
				kind.
				Because of the structure of the AllowOp process we can only pass in a pointer to
				the operation which is causing the change
				It is important that you understand where this function is being used and also
				to use the Op pointer to make any changes UndoAble
				Order in which this function gets called is dependent on where the attribute
				appears in the tree ie ensure that work you do in this function is not order
				dependent.
				Also, this is called during the permission stage of the operation, ie it is
				merely trying to find out if it can go ahead and perform its actions,
				ie NO CHANGES HAVE BEEN MADE YET!
				This means you have to treat this function as a flag to reinit yourself at a
				later stage - ie delaying recalculation till the next time you are asked to
				render is one way to ensure all changes made by pOp have been performed.
	Errors:		may need to allocate memory so can fail (return FALSE)
	SeeAlso:	Node::AllowOp(..) and other AllowOp's
 ********************************************************************************************/
BOOL NodeAttribute::LinkedNodeGeometryHasChanged(UndoableOperation* pOp)
{
	return TRUE;
}

/********************************************************************************************
>	virtual void NodeAttribute::NewlyCreatedDefaultAttr(NodeDocument* pNode)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/00
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Attributes which are linked to the geometry of the objects which they affect,
				may not be able to perform a useful function if attached directly to the 
				NodeDocument node (ie a default attribute). This function allows a default
				attribute of this type to perform an extra initialisation step so that the
				Render() function can handle 
	Errors:		
	SeeAlso:	-
 ********************************************************************************************/
void NodeAttribute::NewlyCreatedDefaultAttr(NodeDocument* pNode)
{
}

// geometry linked attributes should use the nodes inkpath as required
BOOL NodeAttribute::LinkToGeometry(Node* pContext)
{
	return TRUE;
}

/********************************************************************************************
>	virtual BOOL NodeAttribute::ContainsAttributeValue(AttributeValue* pVal)
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/00
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	To check if this NodeAttribute contains a the AttributeValue
				NB needs to be implemented in a manner specific to the way in which
				the derived NodeAttribute class stores the AttributeValue
	Errors:		
	SeeAlso:	OffscreenAttrValue::DoesOffscreenBmpRequireTransp
				AttrFeather::ContainsAttributeValue
 ********************************************************************************************/
BOOL NodeAttribute::ContainsAttributeValue(AttributeValue* pVal)
{
	return FALSE;
}
