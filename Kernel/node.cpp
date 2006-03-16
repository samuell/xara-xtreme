// $Id: node.cpp 662 2006-03-14 21:31:49Z alex $
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
                  

// Implementation of the Node class
                  
// Check in comments



#include "camtypes.h"                          
//#include "mario.h"    // for _R(IDE_NOMORE_MEMORY)    
#include "spread.h"
#include "chapter.h"
#include "nodedoc.h"
#include "page.h"
#include "progress.h"
#include "insertnd.h"
#include "lineattr.h"
#include "objchge.h"
#include "nodepath.h"
#include "ndoptmz.h"
#include "dbugtree.h"
#include "camfiltr.h"
#include "cxftags.h"
#include "nodetxts.h"

#include "extender.h"
//#include "ngcore.h"		// NameGallery, for stretching functionality
#include "group.h"

#ifdef RALPH
#include "ralphcri.h"
#endif
   

MILLIPOINT Node::PixelWidth;   
MILLIPOINT Node::PixelHeight; 
BOOL Node::HourglassOn = FALSE; // When TRUE certain slow routines will call ContinueSlowJob

DECLARE_SOURCE("$Revision: 662 $");
CC_IMPLEMENT_DYNAMIC(Node, CCObject)
      
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW    
						    
	  
/***********************************************************************************************

>    Node::Node(void)

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    -
     Outputs:   -
     Returns:   - (Constructor)
     Purpose:   This constructor creates a node linked to no other with all status flags
				FALSE.  Also updates the Node counter of the current Document.
     Errors:    An assertion failure will occur if there is no current document
     Notes:     This method relies on the current document having been set in the Document
     			object.   

***********************************************************************************************/


Node::Node()
{
	Previous=Next=Child=Parent=NULL; 
	Flags.Locked=Flags.Mangled=Flags.Marked=Flags.Selected=Flags.Renderable = FALSE;   
	Flags.SelectedChildren = FALSE;
	SetOpPermission(PERMISSION_UNDEFINED);	
	HiddenRefCnt = 0; 
		 
	// Has no TAG because it is not in a document yet.
	Tag = TAG_NOT_IN_DOC; 
}
 

/***********************************************************************************************

>	Node::Node( Node* ContextNode,  
				AttachNodeDirection Direction, 
				BOOL Locked = FALSE, 
				BOOL Mangled = FALSE,  
				BOOL Marked = FALSE, 
				BOOL Selected = FALSE, 
				BOOL Renderable = FALSE)

    Author: Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:26/4/93             
    
    Inputs: ContextNode: Pointer to a node which this  node is to be attached to.     
    
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
			Renderable: Is node renderable ?
			Compound:	Can node contain non attribute children (eg. a group)
			
    Outputs:   -
    Returns:   - 
    Purpose: This constructor initialises a node and links it to ContextNode in the
		     direction specified by Direction. All necessary tree links are
			 updated. For example if a node is inserted as a FirstChild of ContextNode, 
			 and Context node already has children, then the current first child of context 
			 node will become the next sibling of the new first child node.
			 
			 Also updates the Node counter of the current document.  
   
    Errors: An assertion failure will occur if the ContextNode is NULL
    
***********************************************************************************************/
/*
Technical notes:

This method Initializes the flags to the values specified, and then calls a method
to attach a node to ContextNode, in the direction specified by Direction. 

***********************************************************************************************/                
 

Node::Node(Node* ContextNode,  
		        AttachNodeDirection Direction, 
		        BOOL Locked, 
				BOOL Mangled,  
		    	BOOL Marked, 
				BOOL Selected, 
	 			BOOL Renderable
			   )
{                             
	// Defensive programming
	ENSURE(ContextNode != NULL,"Trying to attach a node to a NULL node");              

	// Initialize the flags
	Flags.Locked = Locked; 
	Flags.Mangled = Mangled; 
	Flags.Marked = Marked; 
	Flags.Selected = Selected; 
	Flags.SelectedChildren = FALSE;
	Flags.Renderable = Renderable;
	SetOpPermission(PERMISSION_UNDEFINED);	
	Child=NULL;                         // New node has no children
	AttachNode(ContextNode,Direction);  
			 
	// Produce unique TAG for object 
	//ENSURE(Document::GetCurrent() != NULL,"Unable to produce a unique TAG for the node" 
	//	"\nbeing constructed because the current document is NULL"); 
	
	BaseDocument* pDoc = ContextNode->FindOwnerDoc();
	if (pDoc != NULL)
	{
		// Get a tag for this node and update the count of nodes in the document.
		Tag = pDoc->NewTag();
		pDoc->IncNodeCount();
	}
	else 
	{
		Tag = TAG_NOT_IN_DOC; // Has no TAG
	}

	HiddenRefCnt = 0;

	if (Selected) 
	{
		// Inform the Selection SelRange that the selection has changed
		// Pass in 'this' to let it know that I am the most recently selected node
		SelRange *Selection = GetApplication()->FindSelection();
		if (Selection != NULL)
			Selection->Update(FALSE, this);
	}
} 




/********************************************************************************************

>	virtual Node::~Node()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys a Node object.  The base class updates the node counter of the
				current document, and ensures that the SelRange doesn't have a cached pointer
				to 'this' as its 'Last Selected Node'.
	Errors:		-
	SeeAlso:	Document::DecCurrentNodeCount

********************************************************************************************/

Node::~Node()
{
	ERROR3IF(Child != NULL,"Deleting a node that has a ptr to child nodes");

	// Ensure that the Selection SelRange doesn't still keep a cached pointer to me
	SelRange *Selection = GetApplication()->FindSelection();
	if (Selection != NULL)
		Selection->ClearLastSelectedNodeIfItIs(this);

}

                   
/***********************************************************************************************

>	virtual void Node::Render(void)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/93
	Inputs:		RendRegion: Render region to render into
	Outputs:	-
	Returns:	-   
	Purpose:    For rendering a node
		
***********************************************************************************************/

void Node::Render( RenderRegion* pRender ) {};   



/********************************************************************************************

>	void Node::RenderTreeAtomic(RenderRegion* pRender)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/09/2000
	Inputs:		pRender		the RenderRegion to render into.

	Purpose:	Renders the tree rooted at this node into the given RenderRegion.

	Notes:		!!! THIS FUNCTION IS NOT RECURSIVE AS OF 23/06/2004 - Phil !!!

				*** THIS FUNCTION IS CURRENTLY RECURSIVE ***

				*** THIS FUNCTION IS CURRENTLY ATOMIC ***
				So don't call it willy-nilly, as you will ruin Camelot's interruptible
				background rendering functionality.

				Rendering is depth-first, with PreRenderChildren() called on all nodes.

				This function currently goes *down* the tree, so it will miss any attributes
				applied to this node via its parent. To get around this, do:

				NodeRenderableInk* pParent = (NodeRenderableInk*)pNodeToRender->FindParent();
				if (pParent != NULL)
				{
					CCAttrMap* pAttrMap = CCAttrMap::MakeAppliedAttrMap(pParent);
					if (pAttrMap != NULL)
					{
						pAttrMap->Render(pRenderRegion);
						delete pAttrMap;
					}
				}
				pNodeToRender->RenderTreeAtomic(pRenderRegion);

				Misconceptions: Render() does not render a tree - only one node.
								RenderAppliedAttributes() cannot be used generally - it is
								only designed for and used in the hit-test code.

	Errors:		pRender is checked for NULL in DEBUG builds.

********************************************************************************************/
void Node::RenderTreeAtomic(RenderRegion* pRender)
{
//#pragma message( __LOCMSG__ "Node::RenderTreeAtomic - do nothing" )
//	TRACE( _T("Warning - Node::RenderTreeAtomic called\n") );
	ERROR3IF(pRender == NULL, "Node::RenderTreeAtomic; NULL pRender parameter not allowed!");

	pRender->RenderTree(this, FALSE);
}



/********************************************************************************************

>	BOOL Node::NeedsToRender(RenderRegion *pRender)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/94
	Inputs:		pRender - the render region in question (NULL if none)
	Returns:	TRUE  => This node should be rendered,
				FALSE => This node does not need to be rendered.
	Purpose:	Decide whether this node needs to be rendered, given the information
				provided by the parameters.
				This is a virtual function which defaults to returning FALSE - i.e. the
				node never needs to be rendered.  This function should be overridden for
				any renderable nodes, so that they use the info provided to determine
				whether or not they need to be rendered.
	SeeAlso:	Node::Render

********************************************************************************************/

BOOL Node::NeedsToRender(RenderRegion* pRender)
{
	// NeedsToRender has been deprecated and replaced by RenderSubtree
	// It is retained in this base class for those algorithms that still have to use
	// NeedsToRender and FindNextForClippedInkRender
	//
	// Now implemented by calling RenderSubtree
	//
	SubtreeRenderState state = RenderSubtree(pRender);
	ERROR2IF(state==SUBTREE_JUMPTO, FALSE, "Complex render state return to BOOL NeedsToRender\n");

	return (state==SUBTREE_ROOTONLY || state==SUBTREE_ROOTANDCHILDREN);
}


/*********************************************************************************************

>    void Node::SetRender(BOOL Status, BOOL bAndChildren=FALSE) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    val: Status of node's renderable flag
				Recursion flag
     Outputs:   -
     Returns:   -
     Purpose:   To set Nodes renderable status (TRUE/FALSE)
     Errors:    -

**********************************************************************************************/
      

void Node::SetRender(BOOL Status, BOOL bAndChildren)
{
	Flags.Renderable = Status;

	if (bAndChildren)
	{
		Node* pNode = FindFirstChild();
		while (pNode)
		{
			pNode->SetRender(Status, bAndChildren);
			pNode = pNode->FindNext();
		}
	}
}     
	


/********************************************************************************************

>	void Node::PreExportRender( RenderRegion* pRender )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/03/94
	Inputs:		pRender - the render region we are exporting to.
	Purpose:	Perform any rendering required when exporting to a file, and this node
				is being 'passed by' during the tree searching.  For example, a group
				node being exported to EPS would output a "start group" token, and then
				its ExportRender function would output a "end group" token.
				By default, it does nothing. Nodes wishing to do special export processing
				should override this function (and ExportRender).
	SeeAlso:	Node::ExportRender; Node::NeedsToExport

********************************************************************************************/

void Node::PreExportRender( RenderRegion* pRender )
{
}

/********************************************************************************************

>	BOOL Node::ExportRender( RenderRegion* pRender )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/03/94
	Inputs:		pRender - the render region we are exporting to.
	Returns:	TRUE  => Rendered (exported) ok; don't call Render().
				FALSE => Exporting is the same as normal rendering; nothing has been
		 				 rendered, so call the default Render() function.
	Purpose:	Perform special export rendering of an object, if it is different to the
				way the object would be rendered normally (e.g. graduated fill attributes,
				and so on).
				By default, the base class function returns FALSE, so by default all
				objects will be exported by calling the Render() function unless they
				override this function.
	SeeAlso:	Node::PreExportRender; Node::NeedsToExport

********************************************************************************************/

BOOL Node::ExportRender( RenderRegion* pRender )
{
	// By default, use normal rendering when exporting.
	return FALSE;
}

/********************************************************************************************

>	BOOL Node::NeedsToExport(RenderRegion* pRender, BOOL VisibleLayersOnly = FALSE,
							 BOOL CheckSelected = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/94
	Inputs:		pRender - the render region we are exporting to (NULL if none)
				VisibleLayersOnly - TRUE => remove nodes which are on invisible layers
								   - FALSE => export everything
				CheckSelected - TRUE => we check if object selected and only export selected bjects
							  - FALSE => we don't bother checking for selection or not
	Returns:	TRUE  => Yes, export this node.
				FALSE => Do not export this node.
	Purpose:	To indicate whether a given node needs to be rendered when exporting the
				document tree to a foreign file format.  The base class always returns
				FALSE; if derived nodes wish to be exported they must override it and
				return TRUE.
	SeeAlso:	Node::NeedsToRender.

********************************************************************************************/

BOOL Node::NeedsToExport(RenderRegion* pRender, BOOL VisibleLayersOnly, BOOL CheckSelected)
{
	return FALSE;
}



/********************************************************************************************

>	virtual String Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/93
	Inputs:		Plural: Flag indicating if the string description should be plural or
						singular.
				Verbose: Flag indicating if a shortended or full description is required. 
						 Most classes will ignore this, but some (eg. Bitmaps) will just
						 give shortended desciptions more appropriate for menus.
	Outputs:	-
	Retuns:		Description of the object 
	Purpose:	To return a description of the Node object in either the singular or the 
				plural. This method is called by the DescribeRange method.
				
				The description will always begin with a lower case letter.   
				
	Errors:		A resource exception will be thrown if a problem occurs when loading the 
				string resource. 
	SeeAlso:	-

********************************************************************************************/
/*
	Technical Notes:
	
	The String resource identifiers should be of the form: ID_<Class>_DescriptionS for the 
	singular description and ID_<Class>_DescriptionP for the plural. 
*/              
              
String Node::Describe(BOOL Plural, BOOL Verbose) 
{     
	ENSURE (FALSE,"The illegal function Node::Describe was called\n"); 
	
	return( _T("") ); // Just to keep the compiler happy
}; 

/***********************************************************************************************
    
							     
>   void Node::AttachNode(	Node* ContextNode, 
							AttachNodeDirection Direction,
							BOOL fCheckTransparent = TRUE,
							BOOL InvalidateBounds = TRUE)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93             
    
    Inputs:     ContextNode: Pointer to a node to which this node is to be attached     
    
				Direction: 
			
				Specifies the direction in which this node is to be attached to the 
				ContextNode. The values this variable can take are as follows: 
								  
				PREV      : Attach node as a previous sibling of the context node
				NEXT      : Attach node as a next sibling of the context node
				FIRSTCHILD: Attach node as the first child of the context node
				LASTCHILD : Attach node as a last child of the context node
				
				fCheckTransparent	whether the insertion routine should test if the
									node to be inserted requires transparency mode to
									render, and if so, to prompt the user about this.                               
    			InvalidateBounds  	When TRUE the parents bounds + all its childrens bounds
									will be invalidated if this is neccessary.
    Outputs:   -
    Returns:   - 
    Purpose:    This method attaches the node to a context node in the direction specified 
				by Direction. All necessary tree links are updated. 
   
    Errors:     An assertion failure will occur if ContextNode is NULL   
    
    Scope:      private 

***********************************************************************************************/
							 
void Node::AttachNode(	Node* ContextNode,
						AttachNodeDirection Direction, 
						BOOL fCheckTransparent,
						BOOL InvalidateBounds) 
{  
	ERROR3IF(IsAnAttribute() && (!(((NodeAttribute*)this)->CanBeAppliedToObject())) &&
			 ContextNode->IsAnObject(), 
		"Trying to attach invalid attribute to this object");
		 
    ENSURE(ContextNode != NULL,"Trying to attach a node to a NULL node");              
	switch (Direction)
	{
		case PREV:        // Attach node as a previous sibling of the ContextNode  
			AttachAsPreviousNode(ContextNode); 
			break; 
		case NEXT:         // Attach node as a next sibling of the ContextNode
			AttachAsNextNode(ContextNode);                                 
			break; 
		case FIRSTCHILD:   // Attach node as the first child of the Context node
			AttachAsFirstChildNode(ContextNode);       
			break;
		case LASTCHILD:    // Attach node as the last child of the Context node
			AttachAsLastChildNode(ContextNode);            
			break;  
	}

	// If an attribute is being attached which affects the parents bounds then we need to
	// invalidate it's parent and all children of the parent!.
	
	if (InvalidateBounds)
	{
		Node* pParent = FindParent();
		if (pParent != NULL)
		{ 
			if (pParent->IsBounded())
			{ 
				BOOL AffectsParentsChildren = (this->IsAnAttribute() && ((NodeAttribute*)this)->EffectsParentBounds());
				if (AffectsParentsChildren)
				{
					((NodeRenderableBounded*)pParent)->InvalidateBoundingRect(TRUE);
				}
			}
		}
	}

	// Set the tags for this node and any children
	BaseDocument* pDoc = ContextNode->FindOwnerDoc();
	SetTags(pDoc);
}     
    

/***********************************************************************************************

>	void Node::SetTags(BaseDocument *pOwnerDoc)

    Author:  Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created: 3/11/95
    Inputs:  pOwnerDoc - the document that this node is in; NULL if not in a document.
    Purpose: Sets the tag of this node and all its descendants to a legal value, according
			 to which document it is in, if any.

***********************************************************************************************/

void Node::SetTags(BaseDocument *pOwnerDoc)
{
	// Set the tag for this node according to the document.
	if (pOwnerDoc != NULL)
	{
		// Only set the tag if it is not already a legal tag.
		if (Tag == TAG_NOT_IN_DOC)
		{
			Tag = pOwnerDoc->NewTag();
			pOwnerDoc->IncNodeCount();
		}
	}
	else 
	{
		// Ensure this is marked as not being in a document.
		Tag = TAG_NOT_IN_DOC;
	}

	// Now set the tags of any children of this node - start with the first child of this node.
	Node *pChild = FindFirstChild();

	while (pChild != NULL)
	{
		// Set the tags of this child and its children.
		pChild->SetTags(pOwnerDoc);

		// Do the next child.
		pChild = pChild->FindNext();
	}
}
      
/***********************************************************************************************

>	void Node::AttachAsPreviousNode(Node* ContextNode) 

    Author:  Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created: 25/4/93             
    
    Inputs:  ContextNode: Pointer to a node to which the new node is to be attached as a
			 previous sibling     
    Outputs: -
    Returns: - 
    Purpose: Links the node as a previous sibling of ContextNode.   
    
    Errors:  An assertion failure will occur if ContextNode is NULL 
    
    Scope:   private 

***********************************************************************************************/
/*
Technical notes:

 When linking the node as a previous sibling of the context node there are a number of  
 cases to consider. 
 
 1. The context node has no existing previous sibling  
	1.1 The context node has a parent  
	1.2 The context node has no parent 
 2. The context node has an existing previous sibling   

***********************************************************************************************/                
 
      
void Node::AttachAsPreviousNode(Node* ContextNode)
{                                
    ENSURE(ContextNode != NULL,"Trying to attach a node to a NULL node");              
	
	// Check if ContextNode already has a Previous sibling 
	if (ContextNode->Previous == NULL)  // ContextNode has no previous sibling                                       
	{
		Previous = NULL; // The new node has no previous sibling
		   
		// If ContextNode has a parent then ContextNode will currently be the 
		// first child of the parent because Previous == NULL   
		if (ContextNode->Parent != NULL) 
		{                                           
			// Make the new node the first child of ContextNode
			ContextNode->Parent->Child = this; 
		}
	}
	else // Context node has a previous sibling     
	{
		// The new node's Previous sibling becomes the context node's previous sibling
		Previous = ContextNode->Previous;    
		// The previous siblings next sibling becomes the new node
		Previous->Next = this; 
	}        
	ContextNode->Previous = this;  // Attach new node as previous sibling
	Next = ContextNode;                // New node's next sibling is ContextNode
	Parent = ContextNode->Parent;  // New nodes parent same as the ContextNode.  
}    

      
/***********************************************************************************************

>   void Node::AttachAsNextNode(Node* ContextNode) 

    Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:   25/4/93             
                                                                         *
    Inputs:    ContextNode: Pointer to a node to which the new node is to be attached as a
			   next sibling     
    Outputs:   -
    Returns:   - 
    Purpose:   Links the node as a next sibling of ContextNode. 
    Errors:    An assertion failure will occur if ContextNode is NULL
    Scope:     Private

***********************************************************************************************/
/*
Technical notes:

 When linking the node as a next sibling of the context node there are two   
 cases to consider. 
 
 1. The context node has no existing next sibling
 2. The context node has an existing next sibling   
	
***********************************************************************************************/                
  
       
void Node::AttachAsNextNode(Node* ContextNode)
{                     
	ENSURE(ContextNode != NULL,"Trying to attach a node to a NULL node");              
    
	// Check if ContextNode already has a Next sibling
	if (ContextNode->Next == NULL)  // Context node has no next sibling  
		Next = NULL; // New node has no next sibling
	else  
	{   
		// The new node's next sibling = the context node's next sibling
		Next = ContextNode->Next; 
		// The next sibling's previous sibling becomes the new node
		Next->Previous = this; 
	}          
	ContextNode->Next = this; // Attach new node as next sibling
	Previous = ContextNode;   // New node's previous sibling is ContextNode
	Parent = ContextNode->Parent; // New node's parent same as the ContextNode
}
					   

/***********************************************************************************************

>   void Node::AttachAsFirstChildNode(Node* ContextNode) 

    Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:   25/4/93             
    
    Inputs:    ContextNode: Pointer to a node to which the new node is to be attached as a
			   first child     
    Outputs:   -
    Returns:   - 
    Purpose:   Links the node as a first child of ContextNode.  
    Errors:    An assertion failure will occur if ContextNode is NULL                                    
    Notes:     This is a private method
	Scope:     private

***********************************************************************************************/
/*
Technical notes:

 When linking the node as a first child of the context node there are two   
 cases to consider. 
 
 1. The context node has no existing first child
 2. The context node has an existing first child  
	
***********************************************************************************************/                
  
					   
void Node::AttachAsFirstChildNode(Node* ContextNode)
{                   
	ENSURE(ContextNode != NULL,"Trying to attach a node to a NULL node");              

	// Check if ContextNode already has a Child
	if (ContextNode->Child == NULL)    
		Next = NULL; // If not then new node has no next sibling
	else
	{   
		// Context node's first child becomes the new node's next sibling
		Next = ContextNode->Child; 
		// The next sibling's previous sibling becomes the new node
		Next->Previous = this; 
	}          
	ContextNode->Child = this; // Attach new node as first child of context node
	Previous = NULL;      // A first child cannot have a previous sibling
	Parent = ContextNode; // New node's parent is ContextNode       
}                                 
  
      
/***********************************************************************************************

>   void Node::AttachAsLastChildNode(Node* ContextNode) 

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    25/4/93             
    
    Inputs:     ContextNode: Pointer to a node to which the new node is to be attached as a
				last child     
    Outputs:    -
    Returns:    - 
    Purpose:    Links the node as a last child of ContextNode. 
    Errors:     An assertion failure will occur if ContextNode is NULL
    Scope:      private
    

***********************************************************************************************/
/*
Technical notes:

 When linking the node as a last child of the context node there are two   
 cases to consider: 
 
 1. The context node has no existing children 
	In this case we can attach the node as a first child
 2. The context node has existing children
	In this case we can find the current last child, and attach the node to it as a next sibling . 
	
***********************************************************************************************/                
				   

void Node::AttachAsLastChildNode(Node* ContextNode)
{               
	ENSURE(ContextNode != NULL,"Trying to attach a node to a NULL node");              
	if (!ContextNode)
		return; // Don't just blow up

	if (ContextNode->Child == NULL)      // The context node has no children   
		AttachAsFirstChildNode(ContextNode);      // Add node as first child of context node                            
	else
	{                                             
		// Search for the current last child of Context node
		Node* np; 
		for(np=ContextNode->Child; (np->Next != NULL); np = np->Next);   
	    // Add node as the next node of the current last child of ContextNode
		AttachAsNextNode(np);    
	}
}                              

/***********************************************************************************************

>    void Node::CascadeDelete(void) 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   26/4/93
     Purpose:   This method removes the node from the tree and deletes all its child nodes 
     			(including hidden nodes). 

				"IT DOES NOT DELETE THE NODE ITSELF (only its children)"
							
     			
				The following before and after diagram illustrates this form of deletion:
				       
				       
     Notes:             Before:-    
		    
		 
	MonoOn	    
			.-----.
			| N1  |
			|     |
			.-----.            
			   |
			   V
			.-----.      .-----.      .-----.
			|N2   |----->|N3   |----->| N4  |      
			|     |      |     |      |     |     
			.-----.      .-----.      .-----.      
							|
							V
				  	 	 .-----.      .-----.     
				   	     |N5   |----->|N6   |     
				         |     |      |     |     
				         .-----.      .-----.     
			                |
							V
	      			     .-----.
		                 |N7   |
						 |     |
	      			     .-----.
					
	MonoOff				
				     
	    
			After deleting node N3 
     
	MonoOn
			.-----.
			|N1   |
			|     |
			.-----.            
			   |
			   V
			.-----.      .-----.     
			|N2   |----->|N4   |     
			|     |      |     |
			.-----.      .-----.   
		      
	MonoOff	
			
			
	 Errors:    -
***********************************************************************************************/
/*
Technical notes:

The routine calls methods to firstly delete the children of the node, and then to unlink the 
node from the tree.      
 
***********************************************************************************************/                
  
			       
void Node::CascadeDelete(void) 
{
	ERROR3IF(GetHiddenCnt() != 0, "Trying to delete a node with a hidden ref cnt"); 
	// Delete all the node's children
	DeleteChildren(Child);
	// Unlink the node from the tree
	UnlinkNodeFromTree();
}  

/***********************************************************************************************

>    void Node::DeleteChildren(Node* FirstChild) 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   26/4/93
     Inputs:    FirstChild: Pointer to the first child to be deleted
     Outputs:   -
     Returns:   - 
     Purpose:   Deletes the children and all the children's children etc. 
				starting from FirstChild. 

				Certain nodes are not deleted but simply unlinked, these are the nodes which are
				pointed at by the OperationHistory.

				NodeHidden nodes (ShowNodeActions point at them)
				Nodes with a HiddenCnt not equal to 0 - HiddenNodes point at them




	 Errors:    -
     Scope:     private 		 
***********************************************************************************************/
/*
Technical notes:

The implementation is recursive: 
		
 For each child
	IF the child has children THEN delete the child's children
	delete the child               
	

***********************************************************************************************/                
  

void Node::DeleteChildren(Node* FirstChild)
{          
	Node* CurrentNode = FirstChild;

	if (ShowHourglass())
	{
		ContinueSlowJob(); 
	}

	BaseDocument *pOwnerDoc = NULL;

	if (FirstChild != NULL)
		FirstChild->FindOwnerDoc();

	while (CurrentNode != NULL)   // While there are children left to process
	{                               
		Node* NextNode = CurrentNode->Next;  
		// It is not the job of DeleteChildren to delete HiddenNodes or nodes pointed to by hidden nodes
		// They should be considered to be in the OperationHistory
		if ( (!CurrentNode->IsNodeHidden()) && (CurrentNode->GetHiddenCnt() == 0) )
		{
			if (CurrentNode->Child != NULL)  // There are children to delete      
				DeleteChildren(CurrentNode->Child);

			// Unlink from tree before deleting it
			CurrentNode->UnlinkNodeFromTree(pOwnerDoc); // Nice and safe !!!!

			delete CurrentNode;              // Current node has no children so can be deleted
		}
		else
			CurrentNode->UnlinkNodeFromTree(pOwnerDoc);

		CurrentNode = NextNode;          // Process next sibling

		if (ShowHourglass())
		{
			ContinueSlowJob(); 
		}

	} //end while
	Child = NULL; 
}                      


/***********************************************************************************************

>    void Node::UnlinkNodeFromTree(BaseDocument *pOwnerDoc == NULL)

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   26/4/93
     Inputs:    pOwnerDoc - pointer to the document that this node is in.  This is used to
							update the node count.  If it is NULL, then the tree is scanned
							upwards to find the owner document instead.  This is merely a
							performance enhancement, so that if a lot of nodes are being
							deleted, then we don't have to scan the tree all the time.

							This parameter is passed in automatically by Node functions which
							delete many nodes in one go, such as:

								Node::CascadeDelete
								Node::DeleteChildren
								...
								etc.

     Purpose:   Unlinks the node from the tree.
	 SeeAlso:	Node::FindOwnerDoc
		 
***********************************************************************************************/
/*
Technical notes:  

When unlinking the node there are a number of cases to consider:

1. The Node has a previous node     
 
2. The Node has no previous node 
	2.1 The node has a parent
	2.2 The node has no parent  

3. The Node has a next node
4. The Node has no next Node

***********************************************************************************************/                
  
void Node::UnlinkNodeFromTree(BaseDocument *pOwnerDoc)
{
	// Find the owner document if we don't have one
	if (pOwnerDoc == NULL)
		pOwnerDoc = FindOwnerDoc();

	// Update the node count if this node is actually in a document
	if (pOwnerDoc != NULL)
		pOwnerDoc->DecNodeCount();

	// Inform classes that may be holding pointers to this node
PORTNOTE("other","Removed RenderRegionList usage")
#ifndef EXCLUDE_FROM_XARALX
	RenderRegionList* pRList = GetApplication()->GetRegionList();
	if (pRList)
		pRList->HandleNodeDeletion(this);
#endif
	if (pOwnerDoc && pOwnerDoc->IsKindOf(CC_RUNTIME_CLASS(Document)))
		((Document*)pOwnerDoc)->HandleNodeDeletion(this);

	// Now do the actual unlinking
	if (Previous == NULL) // Node must be a first child or root
	{
		if (Parent != NULL) // Node is a first child
			// The parent's new first child is the next sibling of the node being deleted
			Parent->Child = Next; 
	}
	else     
    	//The previous node's next sibling becomes the node's Next sibling   
		Previous->Next = Next;                                                
    
	if (Next != NULL)   
		//The next node's previous sibling becomes the node's Previous sibling                          
		Next->Previous = Previous;    

	// Remove nodes links into the tree	           
	Next = NULL; 
	Previous = NULL; 
	Parent = NULL; 
}



/***********************************************************************************************

>   BOOL Node::CopyNode(Node* Destination, 
						AttachNodeDirection Direction) 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   28/4/93
	
	 Inputs:    Destination: The destination node the copied node is to be attached to. 
	     
				Direction: 
			
				Specifies the direction in which the copied node is to be attached to the 
				Destination node. The values this variable can take are as follows: 
								  
				PREV      : Attach node as a previous sibling of the destination node
				NEXT      : Attach no de as a next sibling of the destination node
				FIRSTCHILD: Attach node as the first child of the destination node
				LASTCHILD : Attach node as a last child of the destination node      
				
    Outputs:   	
    Returns:   	FALSE if memory ran out during the copy. 
    			Otherwise TRUE. 
    			 
    Purpose:    This method creates a 'deep' copy of the node and then attaches it to the 
				destination node. The Direction input specifies how the node is to be 
		 		attached to the Destination node. The following before and after diagram 
				illustrates this method.      
			 
    Notes:       Before:-                                    
		
	MonoOn					     
			.-----.      .-----.      .-----.
			|N1   |----->|N2   |----->| N3  |      
			|     |      |     |      |     |     
			.-----.      .-----.      .-----.      
							|
							V
					     .-----.      .-----.
					     |N4   |----->|N5   |
					     |     |      |     |
				    	 .-----.      .-----.          
				     
	MonoOff
					
		 After copying node N2 to destination N2 as a next sibling:-     
		 
			   
	MonoOn									     
			.-----.      .-----.              .-----.      .-----.
			|N1   |----->|N2   |------------->| N2C |----->|N3   | 
			|     |      |     |              |     |      |     |
			.-----.      .-----.              .-----.      .-----.
							|                    |
							V                    V
					     .-----.      .-----. .-----.      .-----.
					     |N4   |----->|N5   | |N4C  |----->|N5C  |
					     |     |      |     | |     |      |     |
					     .-----.      .-----. .-----.      .-----.   
	MonoOff		   
    
	Errors:     If memory is exhausted whilst copying, the routine returns FALSE after 
				deleting any partially copied subtree.				 
		 
***********************************************************************************************/
/*
Technical notes:  

This method makes a call to the recursive CopyChildren to create a copy of the subtree, and 
then attaches the subtree to the destination node. 

***********************************************************************************************/                

BOOL Node::CopyNode(Node* DestinationNode, AttachNodeDirection Direction)
{     
	// Get a copy of the subtree  
	if (ShowHourglass())
	{
		ContinueSlowJob(); 
	}

	Node* NewNode;
	if (!NodeCopy(&NewNode))
		return FALSE;

	// Attach the copied node to the Destination node                                                       
	NewNode->AttachNode(DestinationNode, Direction);
	return (TRUE);
}


/********************************************************************************************

>	BOOL Node::NodeCopy(Node** NodeCopy)   

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> / Mike
	Created:	4/3/94
	Inputs:		-
	Outputs:	NodeCopy: A deep copy of the node if Successful
	Returns:	- 
    Purpose:    This method outputs a 'deep' copy of the node. It is the same as CopyNode 
				except that the copy is not linked into the tree. 

	Errors:		-
	SeeAlso:	Node::CopyNode

********************************************************************************************/

BOOL Node::NodeCopy(Node** ppNodeCopy)   
{     
	Node* NewNode = SimpleCopy();   			  
	if (NewNode == FALSE)    
		return (FALSE);    						  // Out of memory
	
	if (!CopyChildren(Child, NewNode, ccALL))     // Out of memory 
	{        
		DeleteChildren(NewNode->Child);   
		delete NewNode; 
		return (FALSE); 
	}   

	*ppNodeCopy = NewNode;
	return (TRUE); 
}        


/********************************************************************************************

>	BOOL Node::CloneNode(Node** ppNodeCopy, BOOL bLightweight)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/2005
	Inputs:		bLightweight - Only copy what you really need,
							   Make references to original large data items instead of copying them
							   if possible
	Outputs:	NodeCopy: A deep copy of the node if Successful
	Returns:	- 
    Purpose:    This method outputs a 'deep' copy of the node. It is the same as CopyNode 
				except that the copy is not linked into the tree. 
				Override if yor class can interpret the lightweight flag

	Errors:		-
	SeeAlso:	Node::CopyNode

********************************************************************************************/

BOOL Node::CloneNode(Node** ppNodeCopy, BOOL bLightweight)
{
	return NodeCopy(ppNodeCopy);
}


/********************************************************************************************

>	BOOL Node::CopyComplexRange(Range& RangeToCopy)   

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/95
	Inputs:		RangeToCopy - identifies the range of nodes to copy
	Outputs:	
	Returns:	- 
    Purpose:    Copy the series of nodes described by RangeToCopy and attach them to
				onto the last child position of this object.
   	Errors:		-
	SeeAlso:	Node::CopyNode

********************************************************************************************/

BOOL Node::CopyComplexRange(Range& RangeToCopy)
{
	BOOL Failed=FALSE;		
	Node* Current = RangeToCopy.FindFirst();
	Node* NewNode;

	std::list<Node *>	ListOfNodes(30);
	
	while (Current!=NULL)
	{
		CopyType ctype = Current->GetCopyType();
		switch (ctype)
		{
			case SIMPLECOPY:
			{
				Failed = !(Current->CopyNode(this, LASTCHILD));

				// kludge, should be part of post-copy system which does not exist!!
				// unwrap any text story on the clipboard
				if (IS_A(Current,TextStory))
				{
					Node* pLastChild = this->FindLastChild();
					if (IS_A(pLastChild,TextStory))
						Failed = !((TextStory*)pLastChild)->UnWrapStory(NULL);
					else
					{
						ERROR2RAW("Node::CopyComplexRange() - did not find the story");
						Failed = TRUE;
					}
				}

				break;
			}

			case COMPLEXCOPY:
			{
				INT32 success = Current->ComplexCopy(COPYOBJECT, RangeToCopy, &NewNode);
				// success=-1 then error
				// success= 0 then ignore
				// success= 1 then attach NewNode
				if (success>0)
				{
					NewNode->AttachNode(this, LASTCHILD); 
					// we need to save a list of these nodes for a call back
					ListOfNodes.push_back( Current );
					ListOfNodes.push_back( NewNode );
				}				
				
				Failed=(success<0);
				break;
			}
				
			default:
			{
				ERROR2RAW("Illegal copy type returned to CopyComplexRange()");
				Failed=TRUE;
				break;
			}
		}

		// if we haven't copied properly, tidy up and exit the loop
		if (Failed)
			break;

		Current = RangeToCopy.FindNext(Current); 
	}

	// Now call the nodes complex copy functions again to say we've done something.
	// But only call those we have already!

	while( !ListOfNodes.empty() )
	{
		Node		   *pCopy   = (Node *)ListOfNodes.back(); ListOfNodes.pop_back();
		Node		   *pRecall = (Node *)ListOfNodes.back(); ListOfNodes.pop_back();

		if( pRecall && pCopy && pRecall->GetCopyType() == COMPLEXCOPY )
			pRecall->ComplexCopy( COPYFINISHED, RangeToCopy, &pCopy );
	}

/*
	if (Current!=NULL)
		Current=RangeToCopy.FindNext(Current);

	Node* Recall = RangeToCopy.FindFirst();
	while ((Recall!=Current) && (Recall!=NULL))
	{
		if (Recall->GetCopyType()==COMPLEXCOPY)
			Recall->ComplexCopy(COPYFINISHED, RangeToCopy, &NewNode);
		Recall = RangeToCopy.FindNext(Recall);
	}
 */

	return (!Failed);
}








/***********************************************************************************************
    
>   BOOL Node::CopyChildren(Node* FirstChild,
							Node* NewParent,
							CopyControlFlags CopyFlags=ccALL)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>	/ Mike
    Created:    28/4/93
	
	Inputs:     FirstChild: The first child of the subtree to be copied 
		        NewParent: The parent to which all copied children will be attached 
				CopyFlags: The copy control flags (defaults to ALLOBJECTS)
			
    Outputs:    Attaches children of the subtree to NewParent.      
    
    Returns:    FALSE if we run out of memory during a copy
    			Otherwise TRUE

    Purpose:    This private method makes a copy of all the subtree's children and attaches 
				them to NewParent (the root of the new subtree). It is a helper method for
				CopyTree. 

	Errors:     If a memory allocation error occurs then FALSE is returned. 
    
    Scope:      private 					     
***********************************************************************************************/
/*
Technical notes:  

The algorithm recursively copies each child,and each of the child's children etc. 
					   
The routine returns the partially copied tree when a memory error occurs  rather
than deleting it. The reason for this is that the deletion is easier to perform in the 
calling function because of the recursive nature of CopyChildren. 

					  
***********************************************************************************************/                

BOOL Node::CopyChildren(Node* FirstChild, Node* NewParent, CopyControlFlags CopyFlags)
{   
	if (ShowHourglass())
	{
		ContinueSlowJob(); 
	}
	BOOL CopyObject=FALSE;

	// Find out the destination document, if any.
	BaseDocument* pDoc = NewParent->FindOwnerDoc();

	NewParent->Child = NULL;  					// Just in case no nodes are copied

	if (FirstChild != NULL)						// New parent has children
	{                                                        
		Node* CurrentChild = FirstChild; 
		Node* LastNewChild = NULL; 
		while (CurrentChild != NULL)			// While there are children left to copy
		{   
			// We don't copy hidden nodes or the insertion node.
			if (!CurrentChild->IsNodeHidden() && !CurrentChild->IsAnInsertionNode())
			{
				switch (CopyFlags)
				{
					case ccALL:
						CopyObject=TRUE;
						break;
					case ccLOCKED:
						CopyObject=((CurrentChild->IsLocked())!=0);
						break;
					case ccMANGLED:
						CopyObject=((CurrentChild->IsMangled())!=0);
						break;
					case ccMARKED:
						CopyObject=((CurrentChild->IsMarked())!=0);
						break;
					case ccSELECTED:
						CopyObject=((CurrentChild->IsSelected())!=0);
						break;
					case ccRENDERABLE:
						CopyObject=((CurrentChild->IsRenderable())!=0);
						break;
				}

				if (CopyObject)
				{
					Node* NewChild = CurrentChild->SimpleCopy(); // Copy current node  (virtual method)

				    if (NewChild == NULL)			   // Out of memory    
				    	return FALSE;
		    
					// Attach the NewChild node to the previous or parent node
					if (LastNewChild != NULL)          // Child is not a first child
						LastNewChild->Next = NewChild; 
					else                               // Child is a first child
						NewParent->Child = NewChild;   // NewParent's first child is the NewChild        
	    
			    	NewChild->Parent = NewParent;      // The parent of the new child node is NewParent
					NewChild->Previous = LastNewChild; // The previous sibling of the new child

					// Karim 16/01/01 - GLA's must be re-linked up when they are copied.
					if (NewChild->IsAnAttribute() && ((NodeAttribute*)NewChild)->IsLinkedToNodeGeometry())
						((NodeAttribute*)NewChild)->LinkToGeometry(NewParent);

					// Ensure that this new node has the correct tag.
					if (pDoc != NULL)
					{
						NewChild->Tag = pDoc->NewTag();
						pDoc->IncNodeCount();
					}
					else 
					{
						// Ensure this is marked as not being in a document.
						NewChild->Tag = TAG_NOT_IN_DOC;
					}

					if (CurrentChild->Child != NULL)   // There are children to copy   
						if(!CopyChildren(CurrentChild->Child,NewChild,CopyFlags))// Recursively copy child's children 
							return (FALSE);			   // Out of memory     
					             
					LastNewChild = NewChild;           // Need to remember this for next/prev links
				}
			}
			CurrentChild = CurrentChild->Next; // Process next sibling
			
			if (ShowHourglass())
			{
				ContinueSlowJob(); 
			}

		}
	}
	return TRUE; // No errors occurred        
}                


/*
    
BOOL Node::CopyChildren(Node* FirstChild, Node* NewParent)
{   
	if (ShowHourglass())
	{
		ContinueSlowJob(); 
	}

	// Find out the destination document, if any.
	BaseDocument* pDoc = NewParent->FindOwnerDoc();


	NewParent->Child == NULL;  // Just in case no nodes are copied

	if (FirstChild != NULL)    // New parent has children
	{                                                        
		Node* CurrentChild = FirstChild; 
		Node* LastNewChild = NULL; 
		while (CurrentChild != NULL)          // While there are children left to copy
		{   
			// We don't copy hidden nodes
			if (CurrentChild->GetRuntimeClass() != CC_RUNTIME_CLASS(NodeHidden)) 
			{
				Node* NewChild = CurrentChild->SimpleCopy(); // Copy current node  (virtual method)
			    if (NewChild == NULL) // Out of memory    
			    {    
			    	return (FALSE);  
			    }        
		    
				// Attach the NewChild node to the previous or parent node
				if (LastNewChild != NULL)         // Child is not a first child
					LastNewChild->Next = NewChild; 
				else                              // Child is a first child
				{        
					NewParent->Child = NewChild;  // NewParent's first child is the NewChild        
				} // EndElse              
	    
		    	NewChild->Parent = NewParent;      // The parent of the new child node is NewParent
				NewChild->Previous = LastNewChild; // The previous sibling of the new child

				// Ensure that this new node has the correct tag.
				if (pDoc != NULL)
				{
					NewChild->Tag = pDoc->NewTag();
					pDoc->IncNodeCount();
				}
				else 
				{
					// Ensure this is marked as not being in a document.
					NewChild->Tag = TAG_NOT_IN_DOC;
				}

				if (CurrentChild->Child != NULL)   // There are children to copy   
					if(!CopyChildren(CurrentChild->Child,NewChild))// Recursively copy child's children 
						return (FALSE); // Out of memory     
					             
				LastNewChild = NewChild;           // Need to remember this for next/prev links
			}
			CurrentChild = CurrentChild->Next; // Process next sibling
			
			if (ShowHourglass())
			{
				ContinueSlowJob(); 
			}

		} //end while   
	} //end else      
	return (TRUE); // No errors occurred        
}                

*/



/********************************************************************************************

>	BOOL Node::CopyChildrenTo(Node* DestinationNode, CopyControlFlags CopyFlags=ccALL)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/94
	Inputs:		DestinationNode = a pointer to the node to copy this nodes children to
				CopyFlags = copy control flags indicating what flagged nodes to copy
							defaults to ccALL meaning copy every node.
	Outputs:	-
	Returns:	FALSE if we run out of memory during a copy
    			Otherwise TRUE.
 
	Purpose:	This function copies the node's children to DestinationNode	which should 
				initially have no children.

				If we run out of memory during a copy all copied nodes are deleted
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Node::CopyChildrenTo(Node* DestinationNode, CopyControlFlags CopyFlags)
{
	Node* FirstChild = FindFirstChild(); 
	if (FirstChild != NULL)
	{ 
		if (!CopyChildren(FirstChild, DestinationNode, CopyFlags))
		{
			DeleteChildren(DestinationNode->FindFirstChild());   
			return (FALSE); 
		} 
	}
	return TRUE;
} 



/********************************************************************************************

>	virtual CopyType Node::GetCopyType()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		-
	Returns:	A copy type describing how to copy this object
    Purpose:    This function returns a type describing how this object is to be copied.
				The fuction is called from the low level copy operation CopyObjects.
				There are two options at present, these being SIMPLECOPY and COMPLEXCOPY.
				SIMPLECOPY indicates that the node can be copied by a call to its virtual
				function SimpleCopy() followed by a deep copy of all its children.
				COMPLEXCOPY however indicates that the node needs to do its own thing when
				copying and must be called via the ComplexCopy() virtual function. This
				virtual will likely return a tree of copied objects rather than just a
				copy of itself.

********************************************************************************************/

CopyType Node::GetCopyType()
{
	return SIMPLECOPY;
}

  
/***********************************************************************************************

>	Node* Node::SimpleCopy()

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if we are out of memory 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes of Node  
	       
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
	
    Scope:      Protected
								     
***********************************************************************************************/
/*
Technical notes:   

This function needs to be virtual so that it can return a correct object type.
The method delegates the copying of the Nodes contents to the CopyNodeContents method.  
										    
The method templates below show how the SimpleCopy and CopyNodeContents methods should be 
defined in each derived class of Node.                                                                                     

virtual Node* <Node derived class>::SimpleCopy()
{                    
	Node* NodeCopy = new <Node derived class>();  
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(nodeCopy); 
	return (nodeCopy)
}            

void <Node derived class>::CopyNodeContents(<Node derived class>* nodeCopy)
{
	<Base class>::CopyNodeContents(nodeCopy)
	
	//Copy contents specific to derived class here                                    
} 

***********************************************************************************************/                
   
Node* Node::SimpleCopy()
{     
	Node* NodeCopy;   
	// Will need to handle this 
	NodeCopy = new Node();  // Allocate a new node	
	ERRORIF( NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL ); 
	CopyNodeContents(NodeCopy);    
	return (NodeCopy); 
}        

/***********************************************************************************************

>   void Node::CopyNodeContents(Node* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
   	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void Node::CopyNodeContents(Node* NodeCopy)
{                         
	ENSURE(NodeCopy != NULL,"Trying to copy node contents to\n"
							"a node pointed to by a NULL pointer"); 
	NodeCopy->Flags = Flags;        
	// All tree pointers are NULL
// Phil, 7/1/2004
// DO NOT blat the tree pointers because the node may already be in the tree!!!
//	NodeCopy->Previous=NodeCopy->Next=NodeCopy->Parent=NodeCopy->Child = NULL;              
}       


/***********************************************************************************************

>   Node* Node::PublicCopy()

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    17/11/99
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
   	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

Node* Node::PublicCopy()
{
	return SimpleCopy();
}


/********************************************************************************************

>	virtual INT32 Node::ComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		Stage		- COPYOBJECT   if we should make a copy
							- COPYFINISHED once the entire copy operation has completed
				RangeToCopy - Describes the range which is currently being copied.
				pOutput		- Depends on the Stage parameter
							  if Stage=
							  COPYOBJECT
							    Then the node pointer pOutput points at NULL. It should be
								set on exit to point at the copied object or tree of objects
							  COPYFINISHED
							  	Then the node pointer pOutput points at the resulting copy
								from COPYOBJECT inserted into the destination tree.
	Outputs:	pOutput
	Returns:	-1	= The routine failed to make a copy.
				 0	= No copy has been made for this node.
				+1	= pOutput points to the copy created.

    Purpose:    This virtual function is useful for nodes needing to control how they are
    			to be copied. It is overridden currently by text characters and line 
    			objects. These objects cannot exist outside the context of a text story.
				Hence when copying a character object, the parent line object and text
				story object must be copied along with it. 
				To do this we must give an object the ability to copy whatever it needs
				to along with itself. In order to do that, the object needs to override
				this virtual function and the associated GetCopyType() function.
				( Normally nodes which are not associated with a particular grouping will
				not need to override either of these functions. The copy mechanism defaults
				to using SimpleCopy(), whereby every node is copied as is ).
    			
    			Copy stages:
       			If the copy stage is COPYOBJECT, the node has been called to copy itself 
       			and do what ever it needs to to	make a sensible copy of other items such 
       			as attributes. The caller (CopyObjects) will not deep copy this node 
       			(as this is a complex copy and it expects the handler to know what its doing).
				If the copy stage is COPYFINISHED, the node should tidy up any bits it has
				left over after a copy. Some nodes set bits to indicate that a complex copy
				is taking place. This allows them to ignore multiple calls to complex copy
				if the bit is set.

				Note: Imagine you override this function for a particular node pN which
				exists as a selectable item inside a common parent cP. Now, for you to do
				so obviously means that pN cannot exist outside cP and pN can be selected
				without pN being selected. Not to override this function would ofcourse
				allow pN to be copied to the clipboard by itself which may be hazardous.
				So overriding the function will allow you to make a copy of cP and pN
				together, when the function is called. You do not need to override this
				function inside the class pN. When pN is selected a SimpleCopy() copies
				it and all its children automatically ensuring validity. Its soley for use
				by children that need their parents!

	SeeAlso:	Node::GetCopyType(), InternalClipboard::CopyObjects()

********************************************************************************************/

INT32 Node::ComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput)
{
	return -1;
}

   
/***********************************************************************************************

>   void Node::MoveNode(Node* Destination, 
						AttachNodeDirection Direction) 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   28/4/93
	
	 Inputs:    Destination: The destination node which the node is to be attached to. 
	     
				Direction: 
			
				Specifies the direction in which the node is to be attached to the 
				Destination node. The values this variable can take are as follows: 
								  
				PREV      : Attach node as a previous sibling of the destination node
				NEXT      : Attach node as a next sibling of the destination node
				FIRSTCHILD: Attach node as the first child of the destination node
				LASTCHILD : Attach node as a last child of the destination node                           
				
				Note: The destination node should not be the same as, or any child of node. 
			
    Outputs:    -
    Returns:    - 
    Purpose: 	This method moves a node from its existing position in the tree to a 
			 	new node position specified by Destination. The Direction input specifies 
			 	how the node is to be attached to the Destination node. The following 
			 	before and after diagram illustrates this method.      
			 
			 
    Notes:   Before:-   
    
    MonoOn                                 
							     
			.-----.      .-----.      .-----.
			|N1   |----->|N2   |----->| N3  |      
			|     |      |     |      |     |     
			.-----.      .-----.      .-----.      
							|
							V
				    	 .-----.      .-----.
					     |N4   |----->|N5   |
					     |     |      |     |
					     .-----.      .-----.          
		
	MonoOff		     
					
		 After moving node N2 to destination N3 as a first child:-     
		 
		
	MonoOn	      
			.-----.      .-----.      
			|N1   |----->| N3  |      
			|     |      |     |      
			.-----.      .-----.      
							|
							V  
						 .-----.      
					     |N2   |
					     |     |
				    	 .-----.
							|
							V
					     .-----.      .-----.
				    	 |N4   |----->|N5   |
				    	 |     |      |     |
				     	 .-----.      .-----.          

     
    MonoOff 
     
	Errors: An assertion error will occur if the DestinationNode is NULL
		 
***********************************************************************************************/
/*
Technical notes:

The method makes calls to firstly unlink the node from its current position in the 
tree, and then to attach it to its new position in the tree. 

***********************************************************************************************/                
    

void Node::MoveNode(Node* DestinationNode, 
			    AttachNodeDirection Direction) 
{     
	ENSURE(DestinationNode != NULL,"Trying to copy a node to a"
								   "node pointed to by a NULL pointer"); 

	UnlinkNodeFromTree(); // Remove the node from the tree    
	AttachNode(DestinationNode, Direction); 
}                       

/********************************************************************************************

>	virtual UINT32 Node::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of a node, in concrete classes derived from Node. 
	Errors:		If this function has not been overloaded in a non-abstract class then an 
				ENSURE failure will occur.              
				
			
	SeeAlso:	-

********************************************************************************************/

UINT32 Node::GetNodeSize() const 
{     
	//ENSURE(FALSE, "Pseudo pure virtual GetSize() function called");  
	//return (0); 
	return sizeof(*this);
}  

/********************************************************************************************

>	UINT32 Node::GetSubtreeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the subtree in bytes 
	Purpose:	For finding the size of this subtree 
				
	SeeAlso:	-

********************************************************************************************/

UINT32 Node::GetSubtreeSize()  
{   
	UINT32 Total = 0; 
	Node* Current = this->FindFirstDepthFirst();		 
	while (Current != NULL)
	{
		Total += Current->GetNodeSize(); 
		Current = Current->FindNextDepthFirst(this); 
	} 
	return Total; 
}  

/*********************************************************************************************

>    BOOL Node::IsChildOfSelected() const

     Author:    Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/9/95
     Returns:   TRUE if the node has a selected parent.
     			FALSE otherwise
     Purpose:   For finding out whether this node has a selected parent.
	 			Note: If a group (node) of a number of objects is selected then only the group
	 			node itself has the selected flag set. Any children of this group, i.e. objects
	 			in the group would also be deemed selected but would not have the selected bit set.
				This routine is based on the routine View::IsPrintableNodeSelected and just 
				searches up until it finds a layer. If by then it has found a selected flag	then
				its parent must be selected and hence it is.
	 SeeAlso:	View::IsPrintableNodeSelected;

**********************************************************************************************/
       
BOOL Node::IsChildOfSelected()           
{ 
	// We must check the selected status of this node.
	// Even if this node is not selected, its parent might be, so we
	// scan upwards until we find a layer, or a selected node.
	Node *pTestNode = this;
	while ((pTestNode != NULL) && 
		   (!pTestNode->IsSelected()) && 
		   (!pTestNode->IsLayer()))
	{
		pTestNode = pTestNode->FindParent();
	}

	// Ok, what did we find?
	return ((pTestNode != NULL) && pTestNode->IsSelected());
}  

/*********************************************************************************************

>    BOOL Node::IsUnder(Node* pTestNode, bFirstCall = TRUE) const

     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   02/08/2004
	 Inputs:	pTestNode - pointer to node to test against
	 Outputs:	-
     Returns:   TRUE if this node is "under" the specified node.
     			FALSE otherwise
     Purpose:   For finding out whether this node is under another node. Where "under" is
				defined as being before it in rendering order.
				To find the answer quickly without scanning the entire tree, we will find
				the common root node of both this and pNode and find out whether the child
				links from that node lead down to this node first or pTestNode first.
				(Because if my parent is under the test node then so am I)
				Do this recursively so that state is held on the stack, not in the tree
				Clever, huh?
				There is some strictly unneccessary work done in finding the common
				ancestor in this implementation but it's the best compromise that is both
				reasonably fast and completely stateless.
	 SeeAlso:	-

**********************************************************************************************/
       
BOOL Node::IsUnder(Node* pTestNode, BOOL bFirstCall) const
{
	ENSURE(this, "IsUnder called on NULL");
	if (this==NULL)
		return FALSE;

	if (bFirstCall && this==pTestNode)
		return FALSE;

	Node* pMyParent = FindParent();
	Node* pTestParent = pTestNode->FindParent();
	Node* pChild = pTestNode;

	ENSURE(pTestNode, "IsUnder called with NULL pointer to test");
	ENSURE(this!=pTestNode, "IsUnder called with pointer to this");
	ENSURE(pMyParent, "IsUnder can't find parent of this");

//	ENSURE(HiddenRefCnt==0, "IsUnder called on hidden node(s)");

	if (pTestNode == pMyParent)						// If the test node is one of my ancestors
		return TRUE;								// Then I am under it (in render order)

	// Scan up from the test node, checking for the common ancestor
	while (pTestParent)
	{
//		ENSURE(pTestParent->HiddenRefCnt==0, "IsUnder called on hidden node(s)");

		if (pTestParent == pMyParent)
		{
			// Found the common ancestor!
			// Now compare positions in the sibling list
// This commented out code is the naive version
//			Node* pNode = (Node*)this;
//			while (pNode && pNode != pChild)
//				pNode = pNode->FindNext();
//
//			return (pNode!=NULL);					// If we found the test node then we are under it
//													// If we didn't, then we are above it

			// This is a more optimised version
			// We can quickly detect whether testnode is
			// first in sibling list
			if (pChild==pTestParent->FindFirstChild())	// If the test node is the first child
				return (pChild==this);					// Then we're under it if we're the test node
														// And above it if we're not the test node

			// Work in both directions at the same time because, typically
			// the test node is very close to this node, so we're likely to hit
			// it quickly in one direction or the other...
			Node* pNodeF = (Node*)this;				// Forwards
			Node* pNodeB = (Node*)this;				// Backwards
			while ((pNodeF || pNodeB) && pNodeF != pChild && pNodeB!=pChild)
			{
				pNodeF ? pNodeF = pNodeF->FindNext() : NULL;
				pNodeB ? pNodeB = pNodeB->FindPrevious() : NULL;
			}

			return (pNodeF==pChild);				// If we found the test node in the forward direction
													// Then we are underneath it, otherwise we are on top
		}

		pChild = pTestParent;
		pTestParent = pTestParent->FindParent();
	}

	// If the common ancestor wasn't our immediate parent
	// Then test higher ancestors recursively
	// (If our ancestor is under the test node then we are too!)
	return pMyParent->IsUnder(pTestNode, FALSE);
}  

/*********************************************************************************************

>    virtual BOOL Node::IsAnObject(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is an object i.e. a NodeRenderableInk, the base class returns
	 			FALSE.
     Purpose:   For determining quickly if the node is an object
     Errors:    

**********************************************************************************************/
       
/*BOOL Node::IsAnObject() const
{ 
	return FALSE; 	 // Cos it's not (overridden in NodeRenderableInk)
}*/

/*********************************************************************************************

>    BOOL Node::IsCompound(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node can contain non NodeAttribute children, eg. NodeGroup, 
     			the base class returns FALSE.
     Purpose:   For finding the status of the node's Compound flag
     Errors:    

**********************************************************************************************/
       
/*BOOL Node::IsCompound() const
{ 
	return FALSE;  
}*/

/*********************************************************************************************

>    BOOL Node::IsAnAttribute(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a NodeAttribute, the base class returns FALSE.
     Purpose:   For finding the status of the node's Compound flag
     Errors:    

**********************************************************************************************/
       
/*BOOL Node::IsAnAttribute() const
{ 
	return FALSE;  
}*/


/********************************************************************************************

>	virtual BOOL Node::IsOrHidesAnAttribute() const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/95
	Returns:	FALSE
	Purpose:	Allows you to see if this node is a NodeAttribute or is Hiding a NodeAttribute
				without having to use the expensive runtime checking

********************************************************************************************/

/*BOOL Node::IsOrHidesAnAttribute() const
{
	return FALSE;
}*/



/*********************************************************************************************

>    BOOL Node::IsPaper(void) const

     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   30/11/94
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a paper node, the base class returns FALSE.
     Purpose:   For finding if a node is a paper node.
     Errors:    

**********************************************************************************************/
       
/*BOOL Node::IsPaper() const
{ 
	return FALSE;  
}*/

/*********************************************************************************************

>    BOOL Node::IsLayer(void) const

     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   30/11/94
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is a layer node, the base class returns FALSE.
     Purpose:   For finding if a node is a layer node.
     Errors:    

**********************************************************************************************/
       
/*BOOL Node::IsLayer() const
{ 
	return FALSE;  
}*/

/*********************************************************************************************

>    BOOL Node::IsSpread(void) const

     Author:    Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   15/02/95
     Returns:   TRUE if the node is a spread node, the base class returns FALSE.
     Purpose:   For finding if a node is a spread node.

**********************************************************************************************/
       
/*BOOL Node::IsSpread() const
{ 
	return FALSE;  
}*/


/*********************************************************************************************

>    BOOL Node::IsChapter(void) const

     Author:    Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
     Created:   28/04/95
     Returns:   TRUE if the node is a chapter node, the base class returns FALSE.
     Purpose:   For finding if a node is a chapter node.

**********************************************************************************************/
       
/*BOOL Node::IsChapter() const
{ 
	return FALSE;  
}*/


/********************************************************************************************

>	BOOL Node::IsNodeRenderableClass()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Returns:	TRUE => This node is an instance of NodeRenderable or a derived class.
				FALSE => otherwise.
	Purpose:	Determine if a node is *derived* from the NodeRenderable class.
				NB. This has nothing to do with the node's renderable flag!
	SeeAlso:	Node::IsRenderable

********************************************************************************************/

/*BOOL Node::IsNodeRenderableClass() const
{
	return FALSE;
}*/



/********************************************************************************************

>	BOOL Node::ShouldBeRendered()

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

BOOL Node::ShouldBeRendered() const
{
	return FALSE;
}

/********************************************************************************************

>	BOOL Node::IsNodeHidden()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/02/95
	Returns:	TRUE => This node is an instance of NodeHidden or a derived class.
				FALSE => otherwise.
	Purpose:	Determine if a node is *derived* from the NodeHidden class.

********************************************************************************************/

/*BOOL Node::IsNodeHidden() const
{
	return FALSE;
}*/

/********************************************************************************************

>	BOOL Node::IsNodeDocument()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/95
	Returns:	TRUE => This node is an instance of NodeDocument.
				FALSE => otherwise.
	Purpose:	Determine if a node is a NodeDocument object.

********************************************************************************************/

/*BOOL Node::IsNodeDocument() const
{
	return FALSE;
}*/



/********************************************************************************************

>	BOOL Node::IsNodePath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Returns:	TRUE => This node is an instance of NodePath.
				FALSE => otherwise.
	Purpose:	Determine if a node is a NodePath object.

********************************************************************************************/

/*BOOL Node::IsNodePath() const
{
	return FALSE;
}*/



/********************************************************************************************

>	BOOL Node::IsARegularShape()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/95
	Returns:	TRUE => This node is an instance of NodeRegularShape
				FALSE => otherwise.
	Purpose:	Determine if a node is a QuickShape object.

********************************************************************************************/

/*BOOL Node::IsARegularShape() const
{
	return FALSE;
}*/



/********************************************************************************************

>	BOOL Node::IsAVisibleTextNode()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/02/95
	Returns:	TRUE => This node is a VisibleTextNode.
				FALSE => otherwise.
	Purpose:	Determine if a node is a visible text node.

********************************************************************************************/

/*BOOL Node::IsAVisibleTextNode() const
{
	return FALSE;
}*/

/********************************************************************************************
>	BOOL Node::IsAnAbstractTextChar()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/02/95
	Returns:	TRUE  => This node is a AbstractTextChar node.
				FALSE => otherwise.
	Purpose:	Determine if a node is an AbstractTextChar node.
********************************************************************************************/

/*BOOL Node::IsAnAbstractTextChar() const
{
	return FALSE;
}*/

/********************************************************************************************
>	BOOL Node::IsATextChar()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Returns:	TRUE  => This node is a kind of TextChar node.
				FALSE => otherwise.
	Purpose:	Determine if a node is a (or is derived from) TextChar.
********************************************************************************************/
/*BOOL Node::IsATextChar() const
{
	return FALSE;
}*/

/********************************************************************************************
>	BOOL Node::IsABaseTextClass()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/04/95
	Returns:	TRUE  => This node is a kind of BaseTextClass node.
				FALSE => otherwise.
	Purpose:	Determine if a node is a (or is derived from) BaseTextClass.
********************************************************************************************/
/*BOOL Node::IsABaseTextClass() const
{
	return FALSE;
}*/

/********************************************************************************************
>	BOOL Node::IsATypeface()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/04/95
	Returns:	TRUE  => This node is a kind of AttrTxtFontTypeface.
				FALSE => otherwise.
	Purpose:	Determine if a node is a (or is derived from) AttrTxtFontTypeface.
********************************************************************************************/
/*BOOL Node::IsATypeface() const
{
	return FALSE;
}*/

/********************************************************************************************

>	NodePath* Node::IsAnEditablePath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Inputs:		pSelected = A pointer to the selected node to check
	Outputs:	-
	Returns:	A pointer to an editable node path (or derived) object
				NULL if there isn't one.
	Purpose:	Finds the node to edit (if it exists). This node could actually be 'this'
				node one of its children which the node wants to be editable.

********************************************************************************************/

NodePath* Node::IsAnEditablePath()
{
	Node* pNode = this;
 	if (pNode->IsNodePath())
 		return ((NodePath*)pNode);

 	return ((NodePath*) pNode->HasEditableChild(CC_RUNTIME_CLASS(NodePath), NULL));
}



/********************************************************************************************
>	virtual BOOL Node::IsABitmap() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Returns:	TRUE if this node is a bitmap, FALSE if not (the default)
	Purpose:	Quicker way to test if this node is a bitmap.
********************************************************************************************/

/*BOOL Node::IsABitmap() const
{
	return FALSE;
}*/



/*********************************************************************************************
>    BOOL Node::IsAFillAttr(void) const

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/2/95
     Returns:   TRUE if the node is an AttrFillGeometry, will return TRUE
     Purpose:   Virtual function for determining if the node is a Fill attribute
**********************************************************************************************/
       
/*BOOL Node::IsAFillAttr() const
{ 
	return FALSE;  
} */



/*********************************************************************************************
>    BOOL Node::IsABitmapFill(void) const

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/2/95
     Returns:   TRUE if the node is an AttrBitmapFill, will return TRUE
     Purpose:   Virtual function for determining if the node is a Fill attribute
**********************************************************************************************/
       
/*BOOL Node::IsABitmapFill() const
{ 
	return FALSE;  
} */


/*********************************************************************************************
>    BOOL Node::IsABitmapColourFill(void) const

     Author:    Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
     Created:   01/06/2004
     Returns:   TRUE if the node is an AttrBitmapFill, will return TRUE
     Purpose:   Virtual function for determining if the node is a Fill attribute
**********************************************************************************************/
       
/*BOOL Node::IsABitmapColourFill() const
{ 
	return FALSE;  
} */



/*********************************************************************************************
>    BOOL Node::IsABitmapTranspFill(void) const

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/2/95
     Returns:   TRUE if the node is an AttrBitmapTranspFill, will return TRUE
     Purpose:   Virtual function for determining if the node is a Fill attribute
**********************************************************************************************/
       
/*BOOL Node::IsABitmapTranspFill() const
{ 
	return FALSE;  
} */



/********************************************************************************************
>	virtual BOOL Node::IsAnObjectName() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Returns:	TRUE if this node is an object-name attribute (ie. a Wix TemaplateAttribute
				with an 'ObjectName' key), FALSE if not (the default).
	Purpose:	Quicker way to test if this node is an object-name attribute.
	SeeAlso:	TemplateAttribute::IsAnObjectName
********************************************************************************************/

/*BOOL Node::IsAnObjectName() const
{
	return FALSE;
}*/



/********************************************************************************************
>	virtual BOOL Node::IsUserAttribute() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Returns:	TRUE if this node is an AttrUser, FALSE if not (the default).
	Purpose:	Quicker way to test if this node is an AttrUser.
	SeeAlso:	AttrUser::IsUserAttribute
********************************************************************************************/

/*BOOL Node::IsUserAttribute() const
{
	return FALSE;
}*/



/********************************************************************************************
>	virtual BOOL Node::IsSetCandidate() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Returns:	TRUE if this node is can be a member of a set in the Attribute gallery
				(by default FALSE).
	Purpose:	Quicker way to test if this node is worth considering during Name gallery
				scans.
	SeeAlso:	NodeSetSentinel::IsSetCandidate; NodeRenderableInk::IsSetCandidate
********************************************************************************************/

/*BOOL Node::IsSetCandidate() const
{
	return FALSE;
}*/



/********************************************************************************************

>	BOOL Node::IsANodeMould() const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/2000
	Purpose:	Virtual function to determine whether this node is a NodeMould.

********************************************************************************************/
/*BOOL Node::IsANodeMould() const
{
	return FALSE;
}*/



/*********************************************************************************************

>    virtual BOOL Node::IsABrush()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   29/11/99
     Inputs:    -
     Outputs:   -
     Returns:	FALSE
     Purpose:   Virtual function for determing whether this node is a brush attribute
     Errors:    
	 See Also;  brshattr.cpp

**********************************************************************************************/
       
/*BOOL Node::IsABrush() const
{
	return FALSE;
}*/


/*********************************************************************************************

>    BOOL NodeAttribute::IsBrushed()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   29/11/99
     Inputs:    -
     Outputs:   -
     Returns:	TRUE if this node has a brush attribute node as one of its children
     Purpose:   as above
     Errors:    
	 See Also;  brshattr.cpp

**********************************************************************************************/
       
BOOL Node::IsBrushed() const
{
	NodeAttribute* pAttr = FindFirstAttr( &Node::IsABrush );
	return (pAttr != NULL);
}

/********************************************************************************************

>	virtual BOOL Node::SupportsClipboardFormat(InternalClipboardFormat *Format) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95

	Returns:	TRUE  = This node supports the given format
				FALSE = This node does not support the format (or the format is unknown)

	Purpose:	Determine if a node supports a given internal data type. This is used
				by the clipboard when exporting to other applications in order to
				determine if certain data types can be supplied.

				e.g. The basic formats include:
					Vector		- this is ALWAYS assumed to be available (Vector format
								  includes every Node, e.g. export in Camelot .art format)

					Text		- As well as paths, some objects can provide text chars

					Bitmap		- Bitmap fills can render a filled object or supply the
								  bitmap used for filling with.

				See InternalClipboardFormat (kernel\cliptype.h) for more details

	Notes:		The base class does not bother checking the format and just returns
				FALSE, as the only format supported by default is "Vector", which is 

	SeeAlso:	InternalClipboardFormat

********************************************************************************************/

BOOL Node::SupportsClipboardFormat(InternalClipboardFormat *Format) const
{
	// By default, nodes can supply Vector format export only
	return(FALSE);
}

/********************************************************************************************

>	BOOL Node::DiscardsAttributeChildren() const 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/95
	Returns:	TRUE if the node discards its attribute children
				FALSE if it does not (default)

	Purpose:	This virtual was added because we need to know about nodes which can 
				discard their child attributes, an example is the CaretNode. If an 
				attribute gets applied to such a node then no undo info can be generated
				
				scenario
				~~~~~~~~	

				Apply attribute to caret generating undo
				Move caret, so deleting all initial attrs
				undo

				The undo tries to find the applied attribute so that it can hide it, but
				it does not exist anymore (BANG!)
				 						   				
				This base class function returns FALSE

	Notes:		Phil, 24/09/2005
				The usage of this function has changed subtly today. It used to prevent
				both attribute optimisation and undo recording on Caret nodes.
				As of today it just prevents undo recording - attribute optimisation
				goes ahead like normal on Caret nodes so that they behave more consistently.


********************************************************************************************/

BOOL Node::DiscardsAttributeChildren() const 
{
	return (FALSE);
}

/********************************************************************************************

>	KernelBitmap *Node::EnumerateBitmaps(UINT32 Count)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/17/95
	Inputs:		Count - the bitmap to get (see Purpose).
	Returns:	The KernelBitmap in use by the node, or NULL if no more are used.
	Purpose:	Find out what bitmaps, if any, are used by this node.

				The base class returns NULL always, so you over-ride this in any node classes
				that use bitmaps.

				This function supports nodes that use more than one bitmap - you call this
				function repeatedly and keep incrementing the Count parameter that you pass
				in each time by 1.  You should stop calling it when it returns NULL, as this
				indicates that no more bitmaps are used by this node.
				Count should start off as 0 for the first call.  Note that this function
				can (and often will) return NULL for the first call, as many nodes don't
				use bitmaps, obviously.

	SeeAlso:	KernelBitmap

********************************************************************************************/

KernelBitmap *Node::EnumerateBitmaps(UINT32 Count)
{
	return NULL;
}



/*********************************************************************************************

>    Node* Node::HasEditableChild(CCRuntimeClass* ChildClass, Node* pPreviousChild)

     Author:    Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:   20/3/95
     Inputs:    ChildClass		= the runtime class of the editable object
	 			pPreviousChild	= a pointer to the previous editable child 
	 							  returned by 'this' node, NULL if this is the first
								  call to this node.
     Outputs:   -
     Returns:   A node pointer, to an object which forms part of the editable surface of
	 			its parent (this node).
     Purpose:   This function allows compound objects to provide editable children.
       			It also allows tools to ask an object what other nodes form part of
	 			its editable surface if the node itself cannot be editable. This allows
				complex objects such as moulds and text objects, who have a single parent
				above many objects (some of which are editable), to provide a way for tools
				to get at them. 
				A tool scanning the selection may be looking to edit certain objects. 
				If a selected object it comes accross is not of the correct type, the tool
				can also ask it whether it contains any editable objects of type(ChildClass).
				If the node overrides this virtual function it can provide a strangly shapes
				editable surface.
				Note, do not confuse this with select inside, where objects have been
				explicitly selected by the user. In that case the parent would never be
				asked for editable children as it does not form part of the selection under 
				a select inside action.
     Errors:    -

**********************************************************************************************/

Node* Node::HasEditableChild(CCRuntimeClass* ChildClass, Node* pPreviousChild)
{
	return NULL;
}



/*********************************************************************************************

>    virtual void Node::SetSelected(BOOL Status) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93                                                                                                                       
     Inputs:    Status: Status of node's selected flag
     Outputs:   -
     Returns:   -
     Purpose:   To set Nodes selected status (TRUE/FALSE)
	 			This routine also deals with "select-inside" by making sure all the
				SelectedChildren flags are set appropriately.
				Note:	Selecting a node prevents it from being a parent of selected nodes
						This routine therefore clears the selection of any children!
				Note2:	Calling this routine clears any selection from children of this
						node and marks it to no longer be the parent of selected children.
						This is done because selected nodes can't have sel children and because
						unselecting a node implies that you thought it was selected in which case
						it shouldn't have selected children.
     Errors:    -

**********************************************************************************************/

void Node::SetSelected(BOOL fStatus)       
{
	// If selecting or deselecting a node which is currently the parent of selected nodes
	// then it can no longer have any selected children. The rules of select inside say
	// that selected nodes cannot have selected children.
	if (Flags.SelectedChildren)			// If node was previously a parent of selected nodes
		DeSelectSubtree();				// Then clear any selected children

	Flags.SelectedChildren = FALSE;  	// Flag that this node doesn't have selected children

	if (Flags.Selected != fStatus) 
	{
		Flags.Selected = fStatus;		// Flag that this node is now selected

		// Inform the Selection SelRange that the selection has changed
		// If selected, pass in 'this' to let it know that I am the most recently selected node,
		// else pass in NULL to leave things as they were
		SelRange* Selection = GetApplication()->FindSelection();
		if (Selection != 0) Selection->Update(FALSE, fStatus ? this : 0);

	/*
		Removed by Simon.M for Mark.N - the assumption made is invalid

		#ifdef _DEBUG
		// This piece of code checks that if you are selecting a node, none of its ascendents are selected.
		// Debug builds will inform you of this, retail builds will ignore it as it is viewed as such
		// a highly illegal state that all bugs causing this to happen should have be ironed out (shyeh,right!)
		// (MarkN 27/10/94)
		if (Status)
		{
			Node* pNode = Parent;
			while (pNode)
			{
				ERROR3IF(pNode->Flags.Selected,"Selecting a node when an ascendent is selected");
				pNode = pNode->Parent;
			}
		}
		#endif
	*/
		// Now that we've altered the Selected and SelectedChildren flags:
		// If this node is not in the normal selection surface (direct children of a layer)
		// then we must go and set the SelectedChildren flags properly...
		if (FindParent() == 0 || FindParent()->IsLayer())
			return;		// Is in the normal selection surface so exit quickly!

		// Is NOT in the normal selection surface so go fix up the SelectedChildren flags...
		if (fStatus)
		{
			// This node has been selected while "inside" another one.
			// If this node is not in the top layer of normal selection then it must be inside
			// a compound node. So, we must go up the parent links setting their SelectedChildren
			// flags until we reach the normal selection layer (the child list of a Layer node).
			for (Node* pParent = FindParent();
				 pParent != 0 && !pParent->IsLayer();
				 pParent = pParent->FindParent())
			{
				pParent->Flags.SelectedChildren = TRUE;		// Flag that parent has selected children
			}
				
			// This error check may be a bit over-zealous. I suspect that nodes can be deselected
			// while they are in subtrees which are temporarilly unlinked from the tree (hidden nodes???)
//			ERROR3IF(pParent==NULL,"Node doesn't seem to be in a layer!");
		}
		else
		{
			// This node has been deselected while "inside" another one.
			// If there are any other selected of parentsofselected nodes in the sibling list
			// with this one then leave the parent marked as ParentOfSelected
			// Otherwise clear it.
			// And do the same test for it's parent, etc., etc...
			for (Node* pParent = FindParent();
				 pParent != 0 && !pParent->IsLayer();
				 pParent = pParent->FindParent())
			{
				Node   *p;
				for ( p = pParent->FindFirstChild(); p != 0; p = p->FindNext())
					if (p->IsSelected() || p->HasSelectedChildren())
						break;

				pParent->Flags.SelectedChildren = (p != 0);

//				if ( pParent->HasSelectedChildren() )	 // If our parent still has selected children
//					break;								 // Then stop the loop NOW because we don't need
														 // to alter this parent's flag, or any of it's parents.
														 //		Else
//				pParent->Flags.SelectedChildren = FALSE; // Flag that parent no longer has selected children.
			
			}

			// This error check may be a bit over-zealous. I suspect that nodes can be deselected
			// while they are in subtrees which are temporarilly unlinked from the tree (hidden nodes???)
//			ERROR3IF(pParent==NULL,"Node doesn't seem to be in a layer!");
		}
	}
}     



			     
/*********************************************************************************************

>	BOOL Node::DeSelectSubtree()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/10/94                                                                                                                       
	Inputs:   	-
	Outputs:  	-
	Returns:	TRUE if it succeeded
				FALSE otherwise
	Purpose:	To deselect all selected nodes below this node.
				This function also ensures that the SelectedChildren flags are cleared properly.
				It is self-recursive!
				Helper function for SetSelected
	Scope:		Private
	Errors:		-
	SeeAlso:	Node::SetSelected

**********************************************************************************************/

BOOL Node::DeSelectSubtree()
{
	ERROR2IF(this==NULL,FALSE,"DeSelectSubtree called on NULL node.");	// Precondition

	Node* pNode = Child;
	while (pNode)
	{
		pNode->Flags.Selected = FALSE;				// Deselect this node directly

		if (pNode->Flags.SelectedChildren)			// If this node has selected children
		{
			pNode->DeSelectSubtree();				// Then deselect anything in its subtree
		}

		pNode->Flags.SelectedChildren = FALSE;		// Flag that this node no longer has sel children
		pNode = pNode->Next;
	}

	return TRUE;
}

			     
/*********************************************************************************************

>	BOOL Node::HasSelectedChildren() const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/10/94                                                                                                                       
	Inputs:   	-
	Outputs:  	-
	Returns:	TRUE if this node has selected or ParentOfSelected nodes in it's child list
				FALSE otherwise
	Purpose:	To find out whether a node has got selected children or children that themselves
	 			have selected children...
				Helper function for SetSelected
	Scope:		Private
	Errors:		-
	SeeAlso:	Node::SetSelected

**********************************************************************************************/

BOOL Node::HasSelectedChildren() const
{
	ERROR2IF(this==NULL,FALSE,"HasSelectedChildren called on NULL node.");	// Precondition

//	BOOL	has = FALSE;
	Node*	pNode = Child;
	while (pNode!=NULL)										// While there are children to process
	{
		if (pNode->Flags.Selected ||
			pNode->Flags.SelectedChildren)					// See if this child is Sel or SelChildren
			return TRUE;									// If so, then return TRUE immediately!
		pNode = pNode->Next;								// Else go look for next child
	}

	return FALSE;											// If no children Sel or SelChildren then
															// Return FALSE.
}

			     
/********************************************************************************************

>	virtual BOOL Node::IsBounded() const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/94
	Returns:	FALSE
	Purpose:	This function will return TRUE when called on NodeRenderableBounded nodes
				and those derived from it. This is the base class version that returns FALSE
	SeeAlso:	NodeRenderableBounded::IsBounded

********************************************************************************************/

/*BOOL Node::IsBounded()
{
	return FALSE;
}*/
				      
  
/*********************************************************************************************

>    Node* Node::FindLastChild(BOOL ExcludeInsertionNode = FALSE) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    ExcludeInsertionNode: When TRUE we find the last child excluding the 
     								  InsertionNode
     Outputs:   -
     Returns:   Pointer to the node's last child
     			NULL if the node has no children  
     Purpose:   For finding a node's last child 
	 SeeAlso:	InsertionNode
     Errors:    

**********************************************************************************************/
       
Node* Node::FindLastChild(BOOL ExcludeInsertionNode) const
{    
	Node* n = Child; // First child   
	if (n != NULL)
	{
		while ((n->Next) != NULL)
			n = n->Next;              

		if ((ExcludeInsertionNode) && (IS_A(n, InsertionNode)))
		{
			n = n->Previous; 
		}
	}
	return(n); 
}                      
 
/********************************************************************************************

>	Node* Node::FindParent(CCRuntimeClass* ParentClass)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/94
	Inputs:		ParentClass: The type of parent that you want to find
	Outputs:	-
	Returns:	The parent of the node with class ParentClass
				In the retail build NULL is returned if the parent could not be found 

	Purpose:	This function follows the node's parent links until it finds a node 
				of class ParentClass. If no such node is found then an ENSURE will 
				occur. 

	Errors:		If the parent node could not be found then ENSURE is called. 
	SeeAlso:	-

********************************************************************************************/
Node* Node::FindParent(CCRuntimeClass* ParentClass) const
{
	Node* Current = this->FindParent(); 
	while (Current != NULL)
	{
		if (Current->GetRuntimeClass() == ParentClass)
		{
			return Current; 
		} 	
		Current = Current->FindParent(); // Get the next parent
	}
	//ENSURE(FALSE, "Could not find parent of correct type"); 
	return NULL; 
}


/********************************************************************************************

>	Node* Node::FindNext(CCRuntimeClass* Class)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/94
	Inputs:		Class: The node class to look for
	Outputs:	-
	Returns:	The next of the node with class Class

	Purpose:	This function follows the node's next links until it finds a node 
				derived from  Class. NULL is returned if no node is found

	SeeAlso:	-

********************************************************************************************/
Node* Node::FindNext(CCRuntimeClass* Class) const
{
	Node* Current = this->FindNext(); 
	while (Current != NULL)
	{
		if (Current->IsKindOf(Class))
		{
			return Current; 
		} 	
		Current = Current->FindNext(); 
	}
	return NULL; 
}

/********************************************************************************************

>	Node* Node::FindPrevious(CCRuntimeClass* Class)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/94
	Inputs:		Class: The node class to look for
	Outputs:	-
	Returns:	The Previous of the node with class Class

	Purpose:	This function follows the node's Previous links until it finds a node 
				derived from Class. NULL is returned if no node is found

	SeeAlso:	-

********************************************************************************************/
Node* Node::FindPrevious(CCRuntimeClass* Class) const
{
	Node* Current = this->FindPrevious(); 
	while (Current != NULL)
	{
		if (Current->IsKindOf(Class))
		{
			return Current; 
		} 	
		Current = Current->FindPrevious(); 
	}
	return NULL; 
}


/********************************************************************************************
>	Node* Node::FindFirstChild(CCRuntimeClass* Class)

 	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/95
	Inputs:		Class - class of first child to find
	Returns:	pointer to first child of specified type else NULL
	Purpose:	Find first child of specified type
********************************************************************************************/
Node* Node::FindFirstChild(CCRuntimeClass* Class) const
{
	Node* pNode = FindFirstChild(); 
	while (pNode)
	{
		if (pNode->IsKindOf(Class))
			return pNode; 
		pNode = pNode->FindNext(); 
	}
	return NULL; 
}



/********************************************************************************************
>	Node* Node::FindLastChild(CCRuntimeClass* Class)

 	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/95
	Inputs:		Class - class of first child to find
	Returns:	pointer to last child of specified type else NULL
	Purpose:	Find last child of specified type
********************************************************************************************/
Node* Node::FindLastChild(CCRuntimeClass* Class) const
{
	Node* pNode = FindFirstChild(); 
	Node* pNodeOfGivenClass = NULL;
	while (pNode)
	{
		if (pNode->IsKindOf(Class))
			pNodeOfGivenClass = pNode;
		pNode = pNode->FindNext(); 
	}
	return pNodeOfGivenClass; 
}


/********************************************************************************************
>	NodeRenderableInk* Node::FindNextInk() const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/08/2000
	Purpose:	Find the next ink node.
	Notes:		Analogous to FindNext(CC_RUNTIME_CLASS(NodeRenderableInk)),
				just faster and neater.

	See also:	FindNext(), IsAnObject().
********************************************************************************************/
NodeRenderableInk* Node::FindNextInk() const
{
	Node* pInk = FindNext();
	while (pInk != NULL && !pInk->IsAnObject())
		pInk = pInk->FindNext();

	return (NodeRenderableInk*)pInk;
}


/********************************************************************************************
>	NodeRenderableInk* Node::FindPreviousInk() const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/08/2000
	Purpose:	Find the previous ink node.
	Notes:		Analogous to FindPrevious(CC_RUNTIME_CLASS(NodeRenderableInk)),
				just faster and neater.

	See also:	FindPrevious(), IsAnObject().
********************************************************************************************/
NodeRenderableInk* Node::FindPreviousInk() const
{
	Node* pInk = FindPrevious();
	while (pInk != NULL && !pInk->IsAnObject())
		pInk = pInk->FindPrevious();

	return (NodeRenderableInk*)pInk;
}


/********************************************************************************************
>	NodeRenderableInk* Node::FindFirstChildInk() const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/08/2000
	Purpose:	Find the first child ink-node.
	Notes:		Analogous to FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk)),
				just faster and neater.

	See also:	FindFirstChild(), IsAnObject().
********************************************************************************************/
NodeRenderableInk* Node::FindFirstChildInk() const
{
	Node* pInk = FindFirstChild();
	while (pInk != NULL && !pInk->IsAnObject())
		pInk = pInk->FindNext();

	return (NodeRenderableInk*)pInk;
}


/********************************************************************************************
>	NodeRenderableInk* Node::FindLastChildInk() const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/08/2000
	Purpose:	Find the last child ink-node.
	Notes:		Analogous to FindLastChild(CC_RUNTIME_CLASS(NodeRenderableInk)),
				just faster and neater.

	See also:	FindLastChild(), IsAnObject().
********************************************************************************************/
NodeRenderableInk* Node::FindLastChildInk() const
{
	Node* pInk = FindLastChild();
	while (pInk != NULL && !pInk->IsAnObject())
		pInk = pInk->FindPrevious();

	return (NodeRenderableInk*)pInk;
}


/********************************************************************************************

>	BaseDocument *Node::FindOwnerDoc()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/15/95
	Returns:	Pointer to the Document that owns this node, or 
				NULL if the node is not in a document.
	Purpose:	Find out which document this node is in.  This is used to maintain an
				accurate node count for each document.

********************************************************************************************/

BaseDocument *Node::FindOwnerDoc() const
{
	// Find the document this node is attached to - go up the tree until we reach the
	// NodeDocument object or we run out of parents (i.e. it's an orphan sub-tree waiting to
	// go into a document.
	const Node *pNode = this;
	while ((pNode != NULL) && (!pNode->IsNodeDocument()))
		pNode = pNode->FindParent();

	if (pNode != NULL)
		return ((NodeDocument *) pNode)->GetParentDoc();
	else
		return NULL;
}



/********************************************************************************************

>	BOOL Node::IsFamily(CCRuntimeClass* ccrcType, Node* pAncestor) const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19 April 2000
	Inputs:		ccrcType		the type to test for.
				pAncestor		the grandparent node we're looking for.
	Outputs:	
	Returns:	TRUE if one of our parent nodes is of the given type, and either _it_, or
				one of its parents, is the given ancestor node.
				FALSE otherwise.

	Purpose:	Basically, we're looking for a child-parent-grandparent relationship.
				We're the child and our parent can also be the grandparent.

	Notes:		If pAncestor points at us, then this fails the test, so you get FALSE.

	Errors:		In debug, we complain if any input parameter is NULL,
				in release we just return FALSE.

********************************************************************************************/
BOOL Node::IsFamily(CCRuntimeClass* ccrcType, Node* pAncestor) const
{
	// fairly naive algorithm - just get the next ccrcType node and check its parents against
	// pAncestor. could probably improve slightly by checking for ccrcType while we do the
	// grandparent check.

	// input validation.
	if (ccrcType == NULL || pAncestor == NULL)
	{
		ERROR3("Node::IsFamily; NULL input parameters!");
		return FALSE;
	}

	// local variables.
	Node* pParent		= NULL;
	Node* pGrandParent	= NULL;

	// loop on all our parents of type ccrcType.
	pParent = FindParent(ccrcType);
	while (pParent != NULL && pGrandParent != pAncestor)
	{
		// loop up the tree from pParent, looking for pAncestor.
		pGrandParent = pParent;
		while (pGrandParent != NULL && pGrandParent != pAncestor)
			pGrandParent = pGrandParent->FindParent();

		// if we haven't found a grandparent, then find the next parent of type ccrcTest.
		if (pGrandParent == NULL)
			pParent = pParent->FindParent(ccrcType);
	}

	// we were successful if we found a grandparent.
	return (pGrandParent != NULL);
}



// -------------------------------------------------------------------------------------------
// Find NonHidden methods

/*********************************************************************************************

>   Node* Node::FindNextNonHidden(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    -
     Outputs:   -
     Returns:   Pointer to the first next sibling node which is not a NodeHidden 
     Purpose:   For finding the next sibling of a node (ignoring all NodeHidden nodes). 
                
                This routine should only be used if it is vital that a NodeHidden is 
                not returned. It is much slower than Node::FindNext. 
     			  
     SeeAlso:	Node::FindNext
     Errors:    

**********************************************************************************************/

Node* Node::FindNextNonHidden(void) const
{
	Node* Current = Next;     
	if (Current != NULL)
	{
		while (Current->IsNodeHidden())
		{
			Current = Current->Next; 
			if (Current == NULL) break; 		
		} 
	}    
	return (Current); 
}
  
/*********************************************************************************************

>   Node* Node::FindPrevNonHidden(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    -
     Outputs:   -
     Returns:   Pointer to the first previous sibling node which is not a NodeHidden 
     Purpose:   For finding the previous sibling of a node (ignoring all NodeHidden nodes). 
                  
                This routine should only be used if it is vital that a NodeHidden is 
                not returned. It is much slower than Node::FindPrevious. 
     			  
     SeeAlso:	Node::FindPrevious
     Errors:    

**********************************************************************************************/

Node* Node::FindPrevNonHidden(void) const 
{
	Node* Current = Previous;     
	if (Current != NULL)
	{
		while (Current->IsNodeHidden())
		{
			Current = Current->Previous; 
			if (Current == NULL) break; 		
		} 
	}    
	return (Current); 	
}  


 
/*********************************************************************************************

>   static Node* Node::DocFindFirstDepthFirst(BaseDocument *pDocToSearch)

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:    pDocToSearch - the document to look in.
     Returns:   The first node in a depth first traversal of the document tree  
				or NULL if the tree is empty, except for the start node. 
     Purpose:   To find the first node in a depth first traversal of the document tree.
  	 SeeAlso:   Node::DocFindNextDepthFirst
     Errors:    If the document pointer is NULL => ERROR2

**********************************************************************************************/  
/*
Technical notes:

The document tree must always have a start node which is a previous sibling of the 
documents root. This will always be the first node to process in a depth first traversal 
of the tree.  

***********************************************************************************************/                


Node* Node::DocFindFirstDepthFirst(BaseDocument *pDocToSearch)
{
	// Make sure we have a document to search.
	ERROR2IF(pDocToSearch == NULL, NULL, "No document to search in DocFindFirstDepthFirst()");

	// Get the first node!
	return pDocToSearch->GetFirstNode();
}

/*********************************************************************************************

>    Node* Node::DocFindNextDepthFirst(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:    -  
     Outputs:   - 
     Returns:   The next node in the depth first traversal of the document tree or NULL
				if the traversal is complete.    
			
     Purpose:   To find the next node in the depth first traversal of the current document 
                tree.                                                                

	 SeeAlso:   Node::DocFindFirstDepthFirst
	 		
     Errors:    - 
     

**********************************************************************************************/  


Node* Node::DocFindNextDepthFirst(void) 
{                 
	Node* CurrentNode = this; 
				       
	if (CurrentNode->Next != NULL) 
	{             
		CurrentNode = CurrentNode->Next;           
		
		// Find leftmost child node
		while(CurrentNode->Child != NULL)
			CurrentNode = CurrentNode->Child; 
		   
		return (CurrentNode);
		
	}
	else 
		return (CurrentNode->Parent);
}


/*********************************************************************************************

>    Node* Node::FindFirstDepthFirst(void) 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:    None
     Outputs:   -
     Returns:   The first node in a depth first traversal of this subtree. 
			
     Purpose:   To find the first node in a depth first traversal of this subtree
  				
  	 			
  	 SeeAlso:   Node::FindNextDepthFirst
  
     Errors:    An assertion failure will occur if the current document is NULL
     
 

**********************************************************************************************/  


Node* Node::FindFirstDepthFirst(void)
{
	// Find the leftmost child node
	Node* Current = this; 
	while (Current->FindFirstChild() != NULL) 
	{
		Current = Current->FindFirstChild(); 
	}  
	return (Current);    
}

/*********************************************************************************************

>    Node* Node::FindNextDepthFirst(Node* Subtree) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93

     Inputs:    Subtree: The root of the subtree 
     Outputs:   - 
     Returns:   The next node in a depth-first traversal of the subtree, or NULL if there
	 			are no more nodes. 
			
			
     Purpose:   To find the next node in the depth first traversal of the subtree.                                                                

	 SeeAlso:   Node::FindFirstDepthFirst
	 		
     Errors:    - 
     

**********************************************************************************************/  


Node* Node::FindNextDepthFirst(Node* Subtree)
{   
	if (this != Subtree)		// There are more nodes in the subtree
	{              
		Node* CurrentNode = this; 
				       
		if (CurrentNode->Next != NULL) 	   // Has this node got any siblings 
		{             
			CurrentNode = CurrentNode->Next;           
		
			// Find leftmost child node
			while(CurrentNode->Child != NULL)
			{
				CurrentNode = CurrentNode->Child; 
			}
		   
			return (CurrentNode);
		}
		else 
		{
			return (CurrentNode->Parent);
		}
	}
	return (NULL); 
}




/*********************************************************************************************

>    Node* Node::FindFirstPreorder()
     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   15/06/2005
     Inputs:    -
     Outputs:   - 
     Returns:   The First node in a preorder traversal of the subtree, or NULL if there
	 			are no more nodes.
     Purpose:   To find the next node in a preorder traversal of the subtree.                                                                
	 SeeAlso:   Node::FindFirstDepthFirst

**********************************************************************************************/  

Node* Node::FindFirstPreorder()
{
	return this;
}




/*********************************************************************************************

>    Node* Node::FindNextPreorder(Node* pRoot = NULL, BOOL bSkipSubtree = FALSE)
     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   15/06/2005
     Inputs:    -
     Outputs:   - 
     Returns:   The Next node in a preorder traversal of the subtree, or NULL if there
	 			are no more nodes.
     Purpose:   To find the next node in a preorder traversal of the subtree.                                                                
	 SeeAlso:   Node::FindFirstDepthFirst

**********************************************************************************************/  

Node* Node::FindNextPreorder(Node* pRoot, BOOL bSkipSubtree)
{
	Node* pNode = this;

	// We have visited this, so visit its children, then visit siblings and finally parents siblings
	if (pNode->FindFirstChild() && !bSkipSubtree)
		return pNode->FindFirstChild();

	if (pNode==pRoot)
		return NULL;

	if (pNode->FindNext())
		return pNode->FindNext();

	do
	{
		pNode = pNode->FindParent();
		if (pNode==NULL || pNode==pRoot)
			return NULL;
	}
	while (pNode->FindNext()==NULL);

	return pNode->FindNext();
}




/********************************************************************************************

>	Spread *Node::FindParentSpread()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/93
	Returns:	Parent spread node, or NULL if no such parent can be found..
	Purpose:	Find the spread node to which this node is ultimately attached.
	Errors:		ENSUREs if no parent spread is found.

********************************************************************************************/

Spread *Node::FindParentSpread()
{
	ERROR2IF(this==NULL, NULL, "NULL node in Node::FindParentSpread");
	ENSURE(!IsKindOf(CC_RUNTIME_CLASS(Spread)), "A spread cannot contain a spread");

	Node *pNode = this;

	// Search up the tree for the parent spread
	while ((pNode != NULL) && !pNode->IsSpread())
		pNode = pNode->Parent;

	ENSURE(pNode != NULL, "No parent spread found in Node::FindParentSpread");

	return (Spread *) pNode;
}




/*********************************************************************************************

>    static Chapter* Node::FindFirstChapter(BaseDocument *pDocToSearch)

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   18/5/93
     Inputs:    pDocToSearch - the document that should be searched for the first chapter.
     Returns:   If the document tree has any chapter nodes then a pointer to the first is 
     			returned,  else NULL is returned. 
			
     Purpose:   To find the first chapter in the document tree.  
	 		 
     Errors:    An assertion failure will occur if:
     
     			The current document has not been set in the Document class 
     			The document tree's first node is not a StartDocument
                The first sibling of the StartDocument is not a NodeDocument
     	
     SeeAlso:   Node::FindNextChapter
     			
     Notes:	    This method requires that the current document has been set in the Document
  	 			class.
	
**********************************************************************************************/  
/*
Technical notes:    

***********************************************************************************************/                
							     
							       
Chapter* Node::FindFirstChapter(BaseDocument *pDocToSearch) 
{
	// Sanity check
	ERROR2IF(pDocToSearch == NULL, NULL, "NULL document in FindFirstChapter()");

	// Make sure that the document is not toast
	ERROR3IF(!pDocToSearch->GetFirstNode()->FindNext()->IsKindOf(CC_RUNTIME_CLASS(NodeDocument)),
			 "A NodeDocument node was not found at the root of the document tree"); 

	// Get the start node
	Node* StartChapterList = pDocToSearch->GetFirstNode()->FindNext()->FindFirstChild(); 
	if (StartChapterList == NULL)  
		return NULL; 
	else
	{
		if (StartChapterList->IsChapter())
			return ((Chapter*) StartChapterList);
		else
			return(StartChapterList->FindNextChapter());  
	}
}
 
/*********************************************************************************************

>   Chapter* Node::FindNextChapter(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   25/5/93                          
     Inputs:    - 
     Outputs:   - 
     Returns:   If the document tree has any more chapter nodes then a pointer to the next
     			chapter node is returned, else NULL is returned. 
			
     Purpose:   To find the next chapter in the document tree.  
	 		 

**********************************************************************************************/  
							   
Chapter* Node::FindNextChapter(void) 
{                              
	const Node* CurrentNode = this->FindNext();   

	while (CurrentNode != NULL)
	{                                        
		if (CurrentNode->IsChapter())
			return ((Chapter*)CurrentNode); 
		CurrentNode = CurrentNode->FindNext(); 
	}	  
	        
	return (NULL); // No chapter found
}               


  

/*********************************************************************************************

>   NodeRenderablePaper* Node::FindNextForClippedPaperRender(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   18/5/93                          
	 Returns:   If the document tree has any more paper renderable nodes which require 
				rendering then a pointer to the next to render  is returned, else NULL is 
				returned. 
			
     Purpose:   To traverse the document tree to find the next paper renderable node which 
				needs to be rendered, i.e. its pasteboard rectangle intersects with the current
				clipping rectangle.
						     
				The document tree is searched depth first, visiting parent paper renderables 
				before their children. It is a sort of depth first preorder traversal.  
	
	 SeeAlso:   Node::FindFirstForClippedPaperRender			                                                  
				                                                  
**********************************************************************************************/  
							   
NodeRenderablePaper* Node::FindNextForClippedPaperRender(void) 
{                              
	Node *pNode = this;

	// If the node is a spread, then search its children for a paper object
	if (pNode->IsSpread())
	{
		pNode = pNode->FindFirstChild();
		if (pNode->IsPaper())
			return (NodeRenderablePaper *) pNode; // Found a paper object
	}

	// Search this node's siblings for another paper object
	do 
	{
		pNode = pNode->FindNext();
	}
	while ((pNode != NULL) && !pNode->IsPaper());

	// Return pointer to next page, or NULL if no more paper objects left to render.
	return ((NodeRenderablePaper *) pNode);
}                               
	
/*********************************************************************************************

>    Node * Node::FindFirstForClippedInkRender( DocRect* pClipRect, 
 										   		RenderRegion* pRender, 
 										   		BOOL ExcludeLockedLayers) 	   

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>, Modified by Tim & Rik for coordinate changes

     Created:   18/5/93	  
	 Inputs: 	pClipRect: 			 The clipping rectangle 
	 			pRender: 			 The render region 
				ExcludeLockedLayers: When true objects on locked layers do not get rendered 
									 (used for hit detection)

	 Outputs:
     Returns:   If the document tree has any NoderRenderableInk or NodeAttribute nodes which 
				require rendering then a pointer to the first is returned,  else NULL is 
				returned. 
			
     Purpose:   To traverse the document tree depth first to find the first NodeRenderableInk 
				or NodeAttribute node which needs to be rendered, i.e. in the case of a 
				NodeRenderableInk node its bounding rectangle intersects with the current 
				clipping rectangle (and it's not on a hidden layer).  

				This routine can only be called for a spread node 
     
     SeeAlso:   Node::FindNextForClipperInkRender           
     
     Errors:    An assertion failure will occur if the render region is NULL or if this node 
	 			is not a spread.
                       
                    
**********************************************************************************************/  
// Note ExcludeLockedLayers should go when these routines are no longer used for hit detection !

Node* Node::FindFirstForClippedInkRender( DocRect* pClipRect, 
 										   RenderRegion* pRender, 
 										   BOOL ExcludeLockedLayers,
										   Node* pStopNode) 	   
{                                                   
	ENSURE(pRender != NULL, "Render region NULL in FindFirstForClippedInkRender"); 
	ENSURE( IsKindOf(CC_RUNTIME_CLASS(Spread)), "Oh No, that's not a spread in FindFirstForClippedInkRender" );
	// Let FindNextForClippedInkRender do all the work 
	return FindNextForClippedInkRender(pClipRect, pRender, ExcludeLockedLayers, pStopNode);
}

/********************************************************************************************

>	Node *Node::FindChildToRender(DocRect *const pClipRect, RenderRegion *const pRender)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/94
	Inputs:		pClipRect - the clip rectangle to check against.
				pRender - the render region in question.
	Returns:	The child node to render, or NULL if none are suitable.
	Purpose:	Find the first child of this node that needs rendering.
				This follows the children (assuming they intersect with the clipping
				rectangle), and finds (via a depth first search) the first child that
				should be rendered according to our rules.
				The node should be either:
				
				 (a) a NodeAttrbute, OR 
				 (b) a NodeRenderable that intersects the clipping region.

				All other nodes (e.g. NodeHidden) are not rendered.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
// Note ExcludeLockedLayers should go when these routines are no longer used for hit detection !

Node *Node::FindChildToRender(DocRect *const pClipRect, RenderRegion *const pRender, BOOL ExcludeLockedLayers,
							  Node* pStopNode)
{
//#pragma message( __LOCMSG__ "Node::FindChildToRender - do nothing" )
//	TRACE( _T("Warning - Node::FindChildToRender called\n") );
	ENSURE(this != NULL, "FindChildToRender called on NULL pointer");

	Node *pNode = this;

	SubtreeRenderState state = pNode->RenderSubtree(pRender);
	if (state!=SUBTREE_ROOTANDCHILDREN && !pNode->IsAnAttribute())
	{
		pRender->SaveContext();
		return pNode;
	}

	// If this node has any children, save the context first.
	Node *pChild = pNode->Child;

	if (pChild != NULL)
		pRender->SaveContext();

	do
	{
		if (pChild != NULL)
		{
			// Should it be rendered?
			if (pChild->NeedsToRender(pRender)) //(Flags.Required)
			{
				// See if this node has any suitable children to render.
				Node *pChildToRender = pChild->FindChildToRender(pClipRect, pRender, ExcludeLockedLayers);

				if (pChildToRender != NULL)
					// This child has a child of its own to render so return it.
					return pChildToRender;
				else
					// This child has no children to render, so just render this child.
					return pChild; 
			}

		}
		else
			// This node has no children.
			return NULL;

		// No luck - try the next child.
		pChild = pChild->Next;

	} while (pChild != NULL);// && pChild != pStopNode );

	// No child nodes found to render
	ENSURE(pRender != NULL, "NULL Render region found !!");
	return NULL;
}




/*********************************************************************************************

>   Node* Node::FindNextForClippedInkRender( DocRect* pClipRect, 
										 RenderRegion *pRender,  
										 BOOL ExcludeLockedLayers) 	      

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>, Modifioed by Tim & Rik for coordinate conversion
     Created:   18/5/93                          
     
     Inputs: 	pClipRect: 			 The clipping rectangle 
	 			pRender: 			 The render region 
				ExcludeLockedLayers: When true objects on locked layers do not get rendered 
									 (used for hit detection)
    
	 Outputs:   - 
	 Returns:   If the document tree has any more NodeRenderableInk or NodeAttribute nodes 
				which require rendering then a pointer to the next to render  is returned, 
				else NULL is returned. 
			
     Purpose:   To traverse the document tree depth first to find the next NodeRenderableInk 
				or NodeAttribute node which needs to be rendered, i.e. in the case of a 
				NodeRenderableInk its bounding rectangle intersects with the current clipping 
				rectangle.                                                           
				
	 SeeAlso::  Node::FindFirstForClippedInkRender

     Errors:    An assertion failure will occur if LastNode is NULL. 

**********************************************************************************************/  
// Note ExcludeLockedLayers should go when these routines are no longer used for hit detection !
	                               
Node* Node::FindNextForClippedInkRender( DocRect* pClipRect, 
										 RenderRegion *pRender,  
										 BOOL ExcludeLockedLayers,
										 Node* pStopNode) 	      
{              
//#pragma message( __LOCMSG__ "Node::FindNextForClippedInkRender - do nothing" )
//	TRACE( _T("Warning - Node::FindNextForClippedInkRender called\n") );
	ENSURE(pRender != NULL, "Render region NULL in FindNextForClippedInkRender"); 
	Node* pNode = this; 
	BOOL IsSpread = pNode->IsSpread();

	do
	{
		// If this node (which has been rendered) has any children, then those children have
		// also been rendered, but they have just gone out of scope, so restore the attribute
		// context.
		if (!IsSpread && (pNode->Child != NULL))
		{
			pRender->RestoreContext();
		}


		// If this Node is non-NULL
		while (IsSpread || (pNode->Next != NULL))
		{
			// If this node is not a spread, move to the next sibling.
			if (!IsSpread)
				pNode = pNode->Next;

			if (IsSpread || pNode->NeedsToRender(pRender)) //Flags.Required)
			{
				// Follow the child links (saving the context as we go) to the deepest child 
				// of this node, and then return this node for rendering.
				// NB. If no children, then this code just returns the current node to render.
				Node *pChild = pNode->FindChildToRender(pClipRect, pRender, ExcludeLockedLayers, pStopNode);

				if (pChild != NULL)
					return pChild;
				else if (!IsSpread)
					return pNode;
				else
					// Spread has nothing at all to render
					return NULL;
			}

			// Definitely not rendering a spread anymore
			IsSpread = FALSE;
			ENSURE(!pNode->IsKindOf(CC_RUNTIME_CLASS(Spread)),
					"Node should not be a spread!");
		}

		// No siblings left to render (i.e. that intersect the clipping rectangle), so
		// return to the parent.
		pNode = pNode->Parent;

		ENSURE(pNode != NULL, "NULL parent found in FindNextForClippedInkRender");

		// If the parent is NodeRenderableInk or a NodeAttribute (ie renderable), then return it.
		if (pNode->IsAnObject() || pNode->IsAnAttribute())
			// We want to render this node.
			return pNode;

	} while (!pNode->IsSpread());

	// We found no more renderable nodes in this spread, so stop looking.
	// If the spread had any children, then FindChildToRender would have called
	// SaveContext(), so we call RestoreContext() to balance it.
	if (pNode->Child != NULL)
		pRender->RestoreContext();

	// All done.
	return NULL;
}


/*********************************************************************************************

>    Node *Node::FindFirstForUnclippedInkRender(RenderRegion* pRender)

     Author:    Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/6/95
	 Inputs: 	pRender: The render region 
     Returns:   If the document tree has any NoderRenderableInk or NodeAttribute nodes which 
				require rendering then a pointer to the first is returned,  else NULL is 
				returned. 
     Purpose:   To traverse the document tree depth first to find the first NodeRenderableInk 
				or NodeAttribute node which needs to be rendered, regardless of the clip
				rectangle of the render region (and it's not on a hidden layer).  

				This routine can only be called for a spread node 
     SeeAlso:   Node::FindNextForUnclippedInkRender
     Errors:    An assertion failure will occur if the render region is NULL or if this node 
	 			is not a spread.
                       
                    
**********************************************************************************************/  

Node *Node::FindFirstForUnclippedInkRender(RenderRegion* pRender)
{                                                   
	ENSURE(pRender != NULL, "Render region NULL in FindFirstForUnclippedInkRender"); 
	ENSURE( IsKindOf(CC_RUNTIME_CLASS(Spread)), "Oh No, that's not a spread in FindFirstForUnclippedInkRender" );
	// Let FindNextForUnclippedInkRender do all the work 
	return FindNextForUnclippedInkRender(pRender);
}

/********************************************************************************************

>	Node *Node::FindChildToRenderUnclipped(RenderRegion *const pRender)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/6/95
	Inputs:		pRender - the render region in question.
	Returns:	The child node to render, or NULL if none are suitable.
	Purpose:	Find the first child of this node that needs rendering.
				This follows the children (assuming they intersect with the clipping
				rectangle), and finds (via a depth first search) the first child that
				should be rendered according to our rules.
				The node should be either:
				
				 (a) a NodeAttribute, OR 
				 (b) a NodeRenderableBounded.

				All other nodes (e.g. NodeHidden) are not rendered.  Hidden layers are 
				bypassed.

********************************************************************************************/

Node *Node::FindChildToRenderUnclipped(RenderRegion *const pRender)
{
//#pragma message( __LOCMSG__ "Node::FindChildToRenderUnclipped - do nothing" )
//	TRACE( _T("Warning - Node::FindChildToRenderUnclipped called\n") );
	ENSURE(this != NULL, "FindChildToRenderUnclipped called on NULL pointer");

	Node *pNode = this;

//	INT32 DoFindChildren = PRE_RENDER_CHILDREN;

	SubtreeRenderState state = pNode->RenderSubtree(pRender, NULL, FALSE);		// Don't clip!
	if (state!=SUBTREE_ROOTANDCHILDREN && !pNode->IsAnAttribute())
	{
		pRender->SaveContext();
		return pNode;
	}

	// If this node has any children, save the context first.
	Node *pChild = pNode->Child;

	if (pChild != NULL)
	{
		pRender->SaveContext();
	}

	do
	{
		if (pChild != NULL)
		{
			// Should it be rendered?
			BOOL NeedsToRender = pChild->ShouldBeRendered();

			// Special case for layers - don't do hidden layers!
			if (pChild->IsLayer())
			{
				NeedsToRender = pChild->NeedsToRender(pRender);								// Clipped!
			}

			if (NeedsToRender)
			{
				// See if this node has any suitable children to render.
				Node *pChildToRender = pChild->FindChildToRenderUnclipped(pRender);

				if (pChildToRender != NULL)
					// This child has a child of its own to render so return it.
					return pChildToRender;
				else
					// This child has no children to render, so just render this child.
					return pChild; 
			}
		}
		else
			// This node has no children.
			return NULL;

		// No luck - try the next child.
		pChild = pChild->Next;

	} while (pChild != NULL);

	// No child nodes found to render
	ENSURE(pRender != NULL, "NULL Render region found !!");
	return NULL;
}




/*********************************************************************************************

>   Node* Node::FindNextForUnclippedInkRender(RenderRegion *pRender)

     Author:    Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/6/95
     Inputs: 	pRender: The render region 
	 Returns:   If the document tree has any more NodeRenderableInk or NodeAttribute nodes 
				which require rendering then a pointer to the next to render  is returned, 
				else NULL is returned. 
     Purpose:   To traverse the document tree depth first to find the next NodeRenderableInk 
				or NodeAttribute node which needs to be rendered, ignoring the clipping
				rectangle of the clip region.  (i.e. all renderable nodes except those on
				hidden layers).
	 SeeAlso::  Node::FindFirstForUnclippedInkRender
     Errors:    An assertion failure will occur if LastNode is NULL. 

**********************************************************************************************/  

Node* Node::FindNextForUnclippedInkRender(RenderRegion *pRender)
{              
//#pragma message( __LOCMSG__ "Node::FindNextForUnclippedInkRender - do nothing" )
//	TRACE( _T("Warning - Node::FindNextForUnclippedInkRender called\n") );
	ENSURE(pRender != NULL, "Render region NULL in FindNextForUnclippedInkRender"); 
	Node* pNode = this; 

	BOOL IsSpread = pNode->IsSpread();

	do
	{
		// If this node (which has been rendered) has any children, then those children have
		// also been rendered, but they have just gone out of scope, so restore the attribute
		// context.
		if (!IsSpread && (pNode->Child != NULL))
			pRender->RestoreContext();

		// If this Node is non-NULL
		while (IsSpread || (pNode->Next != NULL))
		{
			// If this node is not a spread, move to the next sibling.
			if (!IsSpread)
				pNode = pNode->Next;

			// Should it be rendered?
			BOOL NeedsToRender = IsSpread || pNode->ShouldBeRendered();

			// Special case for layers - don't do hidden layers!
			if (pNode->IsLayer())
				NeedsToRender = pNode->NeedsToRender(pRender);			// Clipped!!!

			if (NeedsToRender)
			{
				// Follow the child links (saving the context as we go) to the deepest child 
				// of this node, and then return this node for rendering.
				// NB. If no children, then this code just returns the current node to render.
				Node *pChild = pNode->FindChildToRenderUnclipped(pRender);

				if (pChild != NULL)
					return pChild;
				else if (!IsSpread)
					return pNode;
				else
					// Spread has nothing at all to render
					return NULL;
			}

			// Definitely not rendering a spread anymore
			IsSpread = FALSE;
			ENSURE(!pNode->IsKindOf(CC_RUNTIME_CLASS(Spread)),
					"Node should not be a spread!");
		}

		// No siblings left to render (i.e. that intersect the clipping rectangle), so
		// return to the parent.
		pNode = pNode->Parent;

		ENSURE(pNode != NULL, "NULL parent found in FindNextForClippedInkRender");

		// If the parent is NodeRenderableInk or a NodeAttribute (ie renderable), then return it.
		if (pNode->IsAnObject() || pNode->IsAnAttribute())
			// We want to render this node.
			return pNode;

	} while (!pNode->IsSpread());

	// We found no more renderable nodes in this spread, so stop looking.
	// If the spread had any children, then FindChildToRender would have called
	// SaveContext(), so we call RestoreContext() to balance it.
	if (pNode->Child != NULL)
		pRender->RestoreContext();

	// All done.
	return NULL;
}



/***********************************************************************************************

>	Chapter* Node::FindEnclosingChapter(DocCoord* ChapterPos, XLONG* ChapterDepth);  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/93
	Inputs:		- 
	Outputs:	ChapterPos:   The top left hand corner of the chapters pasteboard rectangle 
				ChapterDepth: The Sum of the lengths of all preceding chapters   
				
	Returns:	If this node is a chapter or a node contained within a chapter then 
					the chapter node is returned.
				else 
					NULL is returned (In the non DEBUG version). 
				
	Purpose:    For finding the enclosing chapter of a node, the position of the start of the
				enclosing chapter, and the chapters depth.   

	Errors:		An assertion failure will occur if the node is not a chapter itself, or if 
				the node is not enclosed within a chapter.

***********************************************************************************************/
 
Chapter* Node::FindEnclosingChapter(DocCoord* ChapterPos, XLONG* ChapterDepth)  
{                                              
	Node* CurrentNode = this;    
	while (CurrentNode != NULL) 
	{                                   
		if (CurrentNode->IsChapter())
		{                                       
			// We have found the enclosing chapter
			Chapter* EnclosingChapter = (Chapter*)CurrentNode;                                     
			                    
			// Chapter position is the top left hand corner of the chapters pasteboard                 
			ChapterPos->x = (EnclosingChapter->GetPasteboardRect().LowCorner().x); 
			ChapterPos->y = (EnclosingChapter->GetPasteboardRect().HighCorner().y); 
			
			// Calculate the ChapterDepth
			*ChapterDepth = 0;     
			BaseDocument *pOwnerDoc = EnclosingChapter->FindOwnerDoc();
			Chapter* CurrentChapter = Node::FindFirstChapter(pOwnerDoc);
			
			while (CurrentChapter != EnclosingChapter)
			{      
				ENSURE(CurrentChapter != NULL, ")CurrentChapter=NULL"); // This should always be true
				(*ChapterDepth) += CurrentChapter->GetPasteboardRect().Height(); 
				CurrentChapter = CurrentChapter->FindNextChapter(); 		
			}        
			return  (EnclosingChapter); 
		} 
		else
			CurrentNode = CurrentNode->FindParent(); 
	}    
	ENSURE(FALSE,"Trying to find the enclosing chapter\n"
				 "of a node which has no enclosing chapter"); 
	return (NULL); 
}



/********************************************************************************************

>	Node* Node::FindParentOfSelected()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/10/94
	Inputs:		-
	Outputs:	-
	Returns:	The root node of the subtree containing this "selected-inside" node.
				If this node is not "selected-inside" then NULL is returned.
	Purpose:	To find the node at the normal selection surface from one of its
				selected-inside children. Note that this routine can be used to test whether
				a node is "selected-inside" or not by testing whether the Node* returned is
				NULL or not. (NULL means not selected-inside.)

********************************************************************************************/

Node* Node::FindParentOfSelected()
{
	ERROR2IF(this==NULL,NULL,"FindParentOfSelected called on NULL node.");

	Node* pParentOf = NULL;
	Node* pParent = Parent;
	while ( pParent!=NULL && !pParent->IsLayer() )
	{
		pParentOf = pParent;					// Record last parent before we reached the Layer
		pParent = pParent->Parent;				// Find it's parent.
	}
	ERROR3IF(pParent==NULL,"Node doesn't seem to be in a layer!");

	return pParentOf;
}


/********************************************************************************************
>	Node* Node::FindFirstGeometryLinkedAttr()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/00
	Inputs:		-
	Outputs:	-
	Returns:	NodeAttribute child of this node which is linked to this nodes geometry
				NULL if no geometry linked attributes are attached to this node
	Purpose:	Used primarily to inform the geomoetry linked attribute that this node
				has had some operation performed on it - direclty (via op) or indirectly (via parent
				or child nodes) - which has altered its geometry (ie inkpath or shape onscreen)
********************************************************************************************/
NodeAttribute* Node::FindFirstGeometryLinkedAttr()
{
	Node* pCurr = FindFirstChild();
	while(pCurr)
	{
		if(pCurr->IsAnAttribute())
			if(((NodeAttribute*)pCurr)->IsLinkedToNodeGeometry())
				return (NodeAttribute*) pCurr;

		pCurr = pCurr->FindNext();
	}

	return NULL;
}

/********************************************************************************************
>	Node* Node::FindNextGeometryLinkedAttr()
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/00
	Inputs:		-
	Outputs:	-
	Returns:	NodeAttribute child of this node which is linked to this nodes geometry
				NULL if no geometry linked attributes are attached to this node
	Purpose:	Used in conjunction with FindFirstGeometryLinkedAttr find all geometry
				linked attributesm in order to inform them of a change to this nodes shape
********************************************************************************************/
NodeAttribute* Node::FindNextGeometryLinkedAttr()
{
	Node* pCurr = FindNext();
	while(pCurr)
	{
		if(pCurr->IsAnAttribute())
			if(((NodeAttribute*)pCurr)->IsLinkedToNodeGeometry())
				return (NodeAttribute*) pCurr;

		pCurr = pCurr->FindNext();
	}

	return NULL;
}

#ifdef _DEBUG                             
/*********************************************************************************************

>    void Node::ShowDebugTree(void) const 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    - 
     Outputs:   -
     Returns:   -
     Purpose:   This method displays the camelot tree for debugging purposes, it should
     			be called from the StartDocument node.      
     			
     SeeAlso:   Node::ShowDebugTreeDetails
     Errors:    -

**********************************************************************************************/
      

void Node::ShowDebugTree(void) const
{                        
#if DEBUG_TREE
	enum {LevelIndentSpaces = 10};
	BYTE IndentString[LevelIndentSpaces+1];
    
    // Initialize the Indent string
	for(INT32 i=0; i<LevelIndentSpaces; i++)
		IndentString[i] = ' '; 
	IndentString[LevelIndentSpaces] = 0; 
	 
	ShowDebugTreeHelper(this,0,IndentString); 
#endif
}

/*********************************************************************************************

>    void Node::ShowDebugTreeHelper(const Node* const Root, 
								   INT32 TreeLevel, 
								   BYTE* IndentString) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93 
     
     Inputs:    Root:           Root node of the subtree to be output
				TreeLevel:      Current level in the camelot tree
				IndentString:   String containing number of spaces to indent for each level
								in the tree.
			 
     Outputs:   -
     Returns:   -
     Purpose:   To show a subtree of the Camelot tree 
			
     Errors:    -     
     Scope:     private

**********************************************************************************************/
      

void Node::ShowDebugTreeHelper(const Node* const Root, 
							   INT32 TreeLevel, 
							   BYTE* IndentString) const
{
#if DEBUG_TREE
	const Node* CurrentNode = Root; // The current node position in the tree                    

	do
	{
		// Indent to the correct tree level       
		for(INT32 i=0; i<TreeLevel;i++)
			TRACE( _T("%s"),(const char*)IndentString); 
	     
		CurrentNode->ShowDebugTreeDetails(); 
		TRACE( _T("\n"));    // Move to next output line
					 
		// If the current node has children then show them
		if (CurrentNode->Child != NULL)
			ShowDebugTreeHelper(CurrentNode->Child, TreeLevel + 1, IndentString);
    
		CurrentNode = CurrentNode->Next; // Next sibling
	} while (CurrentNode != NULL);
#endif
}                
	

/*********************************************************************************************

>    void Node::ShowDebugTreeDetails() const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   29/4/93 
     
     Inputs:    - 
			 
     Outputs:   -
     Returns:   -
     Purpose:   This virtual function shows a line describing the node for use when displaying 
				the debug tree. 
			
     Errors:    -

**********************************************************************************************/
			
			
void Node::ShowDebugTreeDetails() const
{
#if DEBUG_TREE
	TRACE( _T("Tag = %lu, Flgs( "), Tag);  
	if (Flags.Selected) 
		TRACE( _T("SELECTED ")); 
	if (Flags.Renderable)
		TRACE( _T("RENDERABLE ")); 
	if (Flags.Locked)
		TRACE( _T("LOCKED ")); 
	if (Flags.Mangled)
		TRACE( _T("MANGLED ")); 
	if (Flags.Marked)
		TRACE( _T("MARKED ")); 
	TRACE( _T(")"));     
#endif
}        



/********************************************************************************************

>	void* Node::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/93
	Inputs:		-
	Outputs:	Str: String giving debug info about the node
	Returns:	-
	Purpose:	For obtaining debug information about the Node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

     
void Node::GetDebugDetails(StringBase* pStr) 
{          
#if DEBUG_TREE
	String_256 TempStr; 
	TempStr._MakeMsg(TEXT("Tag = #1%lu\r\n"), (UINT32) GetTag());

	*pStr += TempStr; 
	*pStr += TEXT("\r\n         Node Flags ( "); 

	if (IsSelected())			*pStr += TEXT("SELECTED "); 
	if (IsRenderable())			*pStr += TEXT("RENDERABLE "); 
	if (IsLocked())  			*pStr += TEXT("LOCKED ");
	if (IsMangled()) 			*pStr += TEXT("MANGLED "); 
	if (IsMarked())  			*pStr += TEXT("MARKED ");

	*pStr += TEXT(")\r\nSelect-inside Flags ( ");
	
	if (IsParentOfSelected())	*pStr += TEXT("PARENT ");
	if (IsChildOfSelected())	*pStr += TEXT("CHILD ");
	
	*pStr += TEXT(")\r\n");
#endif   
}           

#endif  // _DEBUG
 
/********************************************************************************************

>	void Node::IncHiddenCnt(void)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Increments the count of the number of NodeHidden nodes which reffer to the 
				node.  This routine should only be called from the NodeHidden methods. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
 
void Node::IncHiddenCnt(void)
{                               
	HiddenRefCnt++; 
}                   

/********************************************************************************************

>	void Node::DecHiddenCnt(void)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Decrements the count of the number of NodeHidden nodes which reffer to the 
				node. This routine should only be called from the NodeHidden methods
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Node::DecHiddenCnt(void)
{
	ENSURE((HiddenRefCnt != 0), "Trying to decrement a zero Hidden Node refference count"); 
	HiddenRefCnt--; 
} 
               
/********************************************************************************************

>	UINT32 Node::GetHiddenCnt(void)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The number of hidden nodes which refer to the node. 
	Purpose:	For finding out how many hidden nodes reffer to the node. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 Node::GetHiddenCnt(void)
{
	return (HiddenRefCnt); 
} 




/********************************************************************************************

>	const TCHAR* Node::Name() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Returns:	A character string which contains some text which names this node.
	Purpose:	Useful for debugging - actually, it only exists in the debug build.

********************************************************************************************/

#ifdef _DEBUG

const TCHAR* Node::Name() const
{
	return GetRuntimeClass()->GetClassName();
}

#endif



/***********************************************************************************************

>    virtual BOOL Node::HidingNode()

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   9/11/94
	 Returns:   TRUE if all was ok. FALSE if an error occured.
     Purpose:   This virtual function is called whenever the node is hidden.
	 			It allows the node do things like 'optimise' itself to use less memory or
	 			send a message to let others know it's is being hidden etc.

				ALWAYS CALL THE BASE CLASS' FUNCTION FROM YOUR OVERRIDDEN FUNCTION.

***********************************************************************************************/

BOOL Node::HidingNode()
{
	return TRUE;
}

/***********************************************************************************************

>    virtual BOOL Node::ShowingNode()

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   9/11/94
	 Returns:   TRUE if all was ok. FALSE if an error occured (eg Out of memory).
     Purpose:   This virtual function is called whenever the node is re-shown after being 
     			Hidden.
	 			It allows the node to reconstruct itself if it was optimised or
	 			send a message to let others know it is back etc.

				ALWAYS CALL THE BASE CLASS' FUNCTION FROM YOUR OVERRIDDEN FUNCTION.

***********************************************************************************************/

BOOL Node::ShowingNode()
{
	return TRUE;
}


/********************************************************************************************

>	virtual INT32 Node::ComplexHide(UndoableOperation* pOp, Node* pNextInRange)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		pOp			  - a pointer to an undoable operation
				pNextInRange  - a pointer to the next node in the range

	Returns:	-1	= The routine failed to hide this node.
				 0	= Ignored, this object does not support complex hide operations, so
				      carry on and hide the node as normal.
				+1	= The node and possibly various others have been hidden correctly.

    Purpose: This function can be overridden by complex objects who wish to gain more
			 control in what happens when they are hidden. For instance some nodes may
			 not want to be hidden at all. If this function is not overridden, the normal
			 action will be used, which is to use DoHideNode() on the node. It may be that
			 some complex parents may wish to retain various sets of nodes within themselves
			 and allow others to be hidden. To inherit this facility in your node, you
			 simply need to override this virtual function and handle your own hiding of
			 the node. 

			 An example of the use of this function is inside the TextObject.
			 TextCharacters can be hidden but EOLNodes and Carets cannot, although they regularly
			 appear in the selection range. Hence the text objects members override this
			 function and ask the textstory to perform the complex hide operation. It will be
			 called on every selected child and should only hide itself when the last child 
			 is called. It can easily work this out by using the pNextInRange pointer which may
			 be null or not a member of this text story. Only at this stage are nodes in the
			 story hidden to avoid corrupting the external range scan. 
			 Hidding nodes which should appear next in range is never a good idea.

	SeeAlso: VisibleTextStory::ComplexHide()

********************************************************************************************/

INT32 Node::ComplexHide(UndoableOperation* pOp, Node* pNextInRange)
{
	// Always ignore this call in the base class
	return 0;
}



/********************************************************************************************

>	void Node::SetParentDangerous(Node* NewParent)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Inputs:		NewParent: The node's new parent
	Outputs:	-
	Returns:	-
	Purpose:	Sets the parent of the node to NewParent. 
				
				*****************************************************************************
				This function has a very special purpose and is very Dangerous to use. 
				So don't use it unless you are very sure what you are doing. 
				***************************************************************************** 

	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void Node::SetParentDangerous(Node* NewParent)
{
	Parent = NewParent; 	
}
 
void Node::SetNextDangerous(Node* NewNext)
{
	Next = NewNext; 	
}

void Node::SetPreviousDangerous(Node* NewPrev)
{
	Previous = NewPrev; 	
}

void Node::SetChildDangerous(Node* NewChild)
{
	Child = NewChild; 	
}


/********************************************************************************************
>	virtual BOOL Node::NeedsTransparency() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this Node requires transparency mode to render properly.  By
				default this returns FALSE.
	Purpose:	Can be overriden in derived classes to return TRUE, indicating that the
				user may be prompted concerning transparency mode when such a node is
				inserted in the tree.
	Errors:		-
	SeeAlso:	Node::AttachNode
********************************************************************************************/

BOOL Node::NeedsTransparency() const
{
	return FALSE;		// by default we don't need to be in transparency mode
}


/********************************************************************************************

>	BOOL Node::ChildrenNeedTransparency()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/15/95
	Returns:	TRUE if any of this node's descendants need transparency to be displayed;
				FALSE if not.
	Purpose:	Used when adding sub-trees to a document (e.g. file load, import).  It scans
				the node's children to determine if any of them need transparency in order
				to be displayed correctly.
				NB. The node itself is not checked; only its children.
	SeeAlso:	Node::NeedsTransparency

********************************************************************************************/

BOOL Node::ChildrenNeedTransparency()
{
#ifdef RALPH
	// Can't scan children when progressively loading/rendering in ralph
	return TRUE;
#else
	// Start with the first child of this node.
	Node *pChild = FindFirstChild();

	while (pChild != NULL)
	{
		// Does this child (or any of its children) need transparency?
		if (pChild->NeedsTransparency() || pChild->ChildrenNeedTransparency())
			// Yes - found some transparency
			return TRUE;

		// Try the next child.
		pChild = pChild->FindNext();
	}

	// No transparency found
	return FALSE;
#endif
}


/********************************************************************************************

>	ChangeCode Node::WarnParentOfChange(ObjChangeParam* pParam, BOOL AllParents)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Inputs:		pParam		= pointer to a object change parameter class
				AllParents	= if true then tell all parents of the change, else just tell
							  the immediate parent.
							  Defaults to TRUE
	Returns:	CC_OK		if all warned parents have verified the change
				CC_FAIL		if unable to go on at all		
	Purpose:	When an object changes, this function should be used to inform its parent(s)
				of the change. If AllParents is TRUE it will call every parent, passing the 
				object change parameter	to their OnChildChange() virtual function.

********************************************************************************************/

ChangeCode Node::WarnParentOfChange(ObjChangeParam* pParam, BOOL AllParents)
{
	ERROR1IF(pParam==NULL,CC_FAIL,"Node::WarnParentOfChange() called with a null change reason");
	ERROR1IF(pParam->GetChangeType()==OBJCHANGE_UNDEFINED,CC_FAIL,"Node::WarnParentOfChange() passed an illegal Change block");

	// find the immediate parent
	Node* pParent = FindParent();
	ChangeCode Code = CC_OK;
	while (pParent)
	{
		// call the parents 'child change' function 
		Code = pParent->OnChildChange(pParam);
		if (Code == CC_FAIL)
			return Code;
		// now itterate up the tree if necessary
		if (AllParents)
			pParent=pParent->FindParent();
		else
			pParent=NULL;
	}
	return Code;
}

/********************************************************************************************

>	virtual ChangeCode Node::OnChildChange(ObjChangeParam* pParam)

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

ChangeCode Node::OnChildChange(ObjChangeParam* pParam)
{
	return CC_OK;
}

/********************************************************************************************

> 	virtual OpPermissionState Node::GetOpPermission()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/02/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns the op perission state for this node.
	Purpose:	Use this to access the node's permission flags to find out if the current op
				is doable to this node.

					PERMISSION_UNDEFINED means no op has explicitly asked this node for permission so go ahead
					PERMISSION_DENIED	 means the current op should not touch this node.
					PERMISSION_ALLOWED   means the nice op asked and the node said YES!

				Range uses this value to decide whether a node is in the range.  A node is not in the range is its
				permission value is PERMISSION_DENIED, otherwise it is in the range (RangeControlFlags permitting that is)

				This state should be set via the AllowOp() function, using the most appropriate out of Node and Range's 
				varients.

	SeeAlso:	SetOpPermission();

********************************************************************************************/

OpPermissionState Node::GetOpPermission()
{
	if (!Flags.OpPermission1)
	{
		if (!Flags.OpPermission2)
			return PERMISSION_DENIED;
		else
			return PERMISSION_ALLOWED;
	}
	else
		return PERMISSION_UNDEFINED;
}

/********************************************************************************************

> 	virtual void Node::SetOpPermission(OpPermissionState NewState,BOOL AndParents = FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/02/95
	Inputs:		NewState 	= the state to set the permission to
				AndParents 	= if TRUE, set the same permission for your parent, parent's parent, etc
	Outputs:	-
	Returns:	-
	Purpose:	Use this to set the node's permission flags.

				Note: 	Once you set a node's permission to PERMISION_ALLOWED, you can only subsequently set it to
						PERMISSION_UNDEFINED. i.e. if you set it to PERMISSION_ALLOWED, then try and set it to 
						PERMISSION_DENIED, the node's permission state will stay as PERMISSION_ALLOWED.

	SeeAlso:	GetOpPermission();

********************************************************************************************/

void Node::SetOpPermission(OpPermissionState NewState,BOOL AndParents)
{
	OpPermissionState CurrentState = GetOpPermission();

	switch (NewState)
	{
		case PERMISSION_DENIED:
			if (CurrentState != PERMISSION_ALLOWED)
			{
				Flags.OpPermission1 = FALSE;
				Flags.OpPermission2 = FALSE;
			}
			break;

		case PERMISSION_ALLOWED:
			Flags.OpPermission1 = FALSE;
			Flags.OpPermission2 = TRUE;
			break;

		case PERMISSION_UNDEFINED:
			Flags.OpPermission1 = TRUE;
			Flags.OpPermission2 = FALSE;
			break;

		default:
			ERROR3_PF(("Unknown OpPermissionState (%d)",NewState));
			break;
	}

	if (AndParents)
	{
		if (Parent != NULL)
			Parent->SetOpPermission(NewState,AndParents);
	}			
}



/********************************************************************************************

> 	virtual BOOL Node::AllowOp(ObjChangeParam* pParam,	BOOL SetOpPermissionState = TRUE,
														BOOL DoPreTriggerEdit = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> 19/01/2000
	Created:	3/02/95
	Inputs:		pParam				 = describes the way an op wants to change the node
				SetOpPermissionState = if TRUE the Op permission state of this node will be set according to
										the outcome of the call
				DoPreTriggerEdit	 = if TRUE then calls NameGallery::PreTriggerEdit.
									   *Must* be TRUE if the calling Op may make any nodes
									   change their bounds, eg move, line width, cut.
									   Use TRUE if unsure.
	Outputs:	-
	Returns:	TRUE means the node and all its parents are happy with this op, FALSE means don't do it
	Purpose:	This is the way to ask a node if you can do an op to it.

				The ObjChangeParam class contains flags that describe how it will change the node

				For example, the op could change the node's appearence (e.g. attr application, editing a path), 
				replace the node with another node (e.g. because it uses hidden nodes to hide the original and put another
				node in its place, or "make shapes"), delete the node (e.g. the delete and cut ops), etc.

				This function gives the node a chance to say NO.  It also gives the parents a chance to say no too.
				E.g. a blend node will allow itself to be deleted, but it will NOT allow a child of itself to be deleted).

				This call should only be made on selected, or parents of selected, nodes.  It makes a decision as a 
				straight node if it is selected.  It makes a decision as a parent if it has selected children.

				E.g. NodeBlend::AllowOp(...op delete...)
					if the node is selected, then it will return TRUE (parents permitting), i.e. I can be deleted
					if the node is a parent of selected it will return FALSE (i.e. can't delete children of blends).

				So when the node is selected, you are asking the node if you can do the op to it.  When the node is
				a parent of a selected node, you are asking if you can do the op to one of its children.

				********

				If the 'SetOpPermissionState' param is TRUE, the following indented lines applies:
					The node's op permission state is set according to the result of this function.

					If TRUE  is returned, then the node's op permission state will be left unchanged.
						AND the parent's op permission state will be set to PERMISSION_ALLOWED

					if FALSE is returned, then the node's op permission state will be PERMISSION_DENIED,
						AND all it's parents (up to the layer) will be set to PERMISSION_DENIED

					Also, all parents of this node are called via their AllowOp() func with the same state 
					as this node.  This means that after this call, you can guarantee that all of its parents will
					have either a PERMISSION_DENIED or PERMISSION_ALLOWED state.

					Note: Even if this node tries to set all it's parents to have a PERMISSION_DENIED state, if any
					of its parents have previously been set to PERMISSION_ALLOWED they will remain in that state (see
					SetOpPermission()). Why? Well, it is possible for a parent node to have one child with a 
					PERMISSION_DENIED and another child with a PERMISSION_ALLOWED.  It this state the parent MUST be
					in state PERMISSION_ALLOWED, because at least one of its children will allow the op to happen to it.

					So, after this call:
						 	The op permission state for this node will be either left unchanged (and
							therefore remain PERMISSION_UNDEFINED), or PERMISSION_DENIED.

							The parent's op permission state will be either PERMISSION_ALLOWED, or PERMISSION_DENIED.

					This is so UndoableOperation::UpdateChangedNodes() will only call OnChildChange() on parent nodes,
					because it only calls that func for nodes that have an op permission state of PERMISSION_ALLOWED.

				********

	SeeAlso:	GetOpPermission(),SetOpPermission();

********************************************************************************************/

BOOL Node::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState, BOOL DoPreTriggerEdit)
{
	ERROR2IF(pParam==NULL,FALSE,"Node::AllowOp() - pParam==NULL");

	// if not called by a child AllowOp(), ensure AllowOp() called for all nodes in compound nodes,
	if (pParam->GetDirection()!=OBJCHANGE_CALLEDBYCHILD)
	{
		BOOL AnyAllowed=AllowOp_AccountForCompound(	pParam,
													SetOpPermissionState,
													DoPreTriggerEdit );
		// check for geometry linked attributes
		// NB this handles the passing of AllowOp messages to GeomLinkedAttrs on CALLEDBYPARENT
		// and CALLEDBYOP.
		// NB2 compounds which don't implement their own AllowOps will call
		// AccountForCompound from here so ensure that nodes don't inform
		// geom linked attrs of the change twice
		if(!IS_KIND_OF(NodeGroup))
		{
			UndoableOperation* pChangeOp = pParam->GetOpPointer();
			BOOL InformGeomLinkedAttrs = SetOpPermissionState && pChangeOp && pChangeOp->MayChangeNodeBounds();
			if(InformGeomLinkedAttrs)
			{
				NodeAttribute* pNA = FindFirstGeometryLinkedAttr();
				while(pNA)
				{
					pNA->LinkedNodeGeometryHasChanged(pChangeOp);
					pNA = pNA->FindNextGeometryLinkedAttr();
				}
			}
		}

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

	// decide if we allow it ... err we do!
	BOOL allowed = TRUE;

	// if we allowed it, see if our parents do ...
	if (Parent != NULL)
	{
		ObjChangeDirection OldDirection = pParam->GetDirection();
		pParam->SetCallingChild(this);
		pParam->SetDirection(OBJCHANGE_CALLEDBYCHILD);
		allowed = Parent->AllowOp(pParam,SetOpPermissionState,DoPreTriggerEdit);
		pParam->SetDirection(OldDirection);
	}

	// if setting permisions ...
	if (SetOpPermissionState)
	{
		// if allowed, mark parent accordingly, else mark child as denied and update parents
		if (!allowed)
			SetOpPermission(PERMISSION_DENIED,TRUE);
		else
			if (Parent != 0) Parent->SetOpPermission(PERMISSION_ALLOWED);
	}

	if (allowed)
	{
		UndoableOperation* pChangeOp = pParam->GetOpPointer();
		// check for geometry linked attributes (for CALLEDBYCHILD direction)
		if(IsCompound() && pParam->GetDirection()==OBJCHANGE_CALLEDBYCHILD)
		{
			BOOL InformGeomLinkedAttrs = SetOpPermissionState && pChangeOp && pChangeOp->MayChangeNodeBounds();
			if(InformGeomLinkedAttrs)
			{
				NodeAttribute* pNA = FindFirstGeometryLinkedAttr();
				while(pNA)
				{
					pNA->LinkedNodeGeometryHasChanged(pChangeOp);
					pNA = pNA->FindNextGeometryLinkedAttr();
				}
			}		
		}

		// if we're ok so far and were asked to do a PreTriggerEdit, then
		// determine whether the Op may change the bounds of some nodes.
		// If it may, then call NameGallery::PreTriggerEdit.
		if(DoPreTriggerEdit)
		{
			// if the Op is non-NULL then query its MayChangeNodeBounds() method.
			if (pChangeOp != NULL && pChangeOp->MayChangeNodeBounds())
			{
PORTNOTE("other","Removed NameGallery usage")
#ifndef EXCLUDE_FROM_XARALX
				allowed = NameGallery::Instance()->PreTriggerEdit(pChangeOp, pParam, this);
#endif
			}
		}
	}

	// return result (directly, or indirectly via a child AllowOp()) to op
	return allowed;
}

/********************************************************************************************
> 	virtual BOOL Node::AllowOp_AccountForCompound(	ObjChangeParam* pParam,
													BOOL SetOpPermissionState=TRUE,
													BOOL DoPreTriggerEdit = TRUE )

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>; Karim 20/01/2000
	Created:	5/5/95
	Inputs:		pParam				 = describes the way an op wants to change the node
				SetOpPermissionState = if TRUE the OpPermission of nodes should be set
				DoPreTriggerEdit	 = if TRUE then NameGallery::PreTriggerEdit is called.
									   *Must* be TRUE if the calling Op may make any nodes
									   change their bounds, eg move, line width, cut.
									   Use TRUE if unsure.

	Returns:	TRUE if ANY of the objects in the coumpond node have allowed the op
	Purpose:	if this is a compound node, ensures all child nodes have their AllowOp() called.

				This ensures that any pre-op code in AllowOp()s will be done
				eg inserting insert dormat format action for TextStories

				This also ensures post-op code in OnChildChange()s is called
				(if the OpPermission is set on affected nodes AND PARENTS)
				eg. format redraw for TextStories

				This fixes the bug where attributes applied to a group/Mould/Blend containing
				a TextStory cause the text story to be formatted correctly

	Note:		Optimisation - currently only done for attribute changes and transformations.
				Karim 18/05/2000 - Also now done when Flags.RegenerateNode is set.
********************************************************************************************/

BOOL Node::AllowOp_AccountForCompound(ObjChangeParam* pParam, BOOL SetOpPermissionState,
															  BOOL DoPreTriggerEdit)
{
	BOOL AnyAllowed=FALSE;

	// 'optimised' to only do the biz if an attr changes, it's a transform or a regeneration.
	ObjChangeFlags Flags=pParam->GetChangeFlags();
	if (IsCompound() && (Flags.Attribute || Flags.TransformNode || Flags.RegenerateNode) )
	{
		ObjChangeDirection OldDirection=pParam->GetDirection();
		pParam->SetDirection(OBJCHANGE_CALLEDBYPARENT);
		Node* pNode=FindFirstChild();
		UndoableOperation* pChangeOp = pParam->GetOpPointer();
		BOOL InformGeomLinkedAttrs = SetOpPermissionState && pChangeOp && pChangeOp->MayChangeNodeBounds();

		while (pNode!=NULL)
		{
			if (pNode->IsAnObject())
				AnyAllowed |= pNode->AllowOp(pParam, SetOpPermissionState, DoPreTriggerEdit);
			else
			{
				if(pNode->IsAnAttribute() && ((NodeAttribute*)pNode)->IsLinkedToNodeGeometry())
					if(InformGeomLinkedAttrs)
						((NodeAttribute*)pNode)->LinkedNodeGeometryHasChanged(pChangeOp);
			}

			pNode=pNode->FindNext();
		}
		pParam->SetDirection(OldDirection);
	}

	// if setting flags and any child allowed it, set this permission allowed
	if (AnyAllowed && SetOpPermissionState)
		SetOpPermission(PERMISSION_ALLOWED);

	return AnyAllowed;
}


/********************************************************************************************

> 	void Node::InsertChainSimple(Node* ContextNode, AttachNodeDirection Direction)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29 03 95
	Inputs:		ContextNode - the node to attach under or after
				Direction - where to attach it
	Outputs:	-
	Returns:	-
	Purpose:	Inserts a chain of nodes at contextnode. A chain is an unconnected set of
				siblings. This function should be called from the first node (left most) in
				the chain.
				It doesn't do anything fancy - in fact, little more than changing a few
				pointers here and there.
	SeeAlso:	AttachNode();

********************************************************************************************/

void Node::InsertChainSimple(Node* ContextNode, AttachNodeDirection Direction)
{
	ERROR3IF(Previous != 0, "Node is not first in the chain");

	if(Direction == FIRSTCHILD)
	{
		Node *N = this;	// will be the last chain in node
		Node *Last = NULL;

		// shoot though the chain setting parent pointers and find the last one
		while(N != 0)
		{
			N->Parent = ContextNode;
			Last = N;
			N = N->FindNext();
		}

		Last->Next = ContextNode->Child;
		ContextNode->Child = this;
	} else if(Direction == LASTCHILD)
	{
		if(ContextNode->Child != 0)
		{
			Node *End = ContextNode->FindLastChild();
		
			ERROR3IF(End->Next != 0, "Last child has next pointer");		

			End->Next = this;
			Previous = End;
		} else {
			ContextNode->Child = this;
		}

		Node *N = this;

		// shoot though the chain setting parent pointers
		while(N != 0)
		{
			N->Parent = ContextNode;
			N = N->FindNext();
		}
	} else {
		ERROR3("Direction not implemented yet in Node::InsertChainSimple");
	}
}



/********************************************************************************************
> 	virtual BOOL Node::PostImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/04/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	This function is called after a document is imported.  Nodes should override
				this function to do any post-import processing.
	SeeAlso:	Node::PostDuplicate
********************************************************************************************/
BOOL Node::PostImport()
{
	return TRUE;
}



/********************************************************************************************
> 	virtual BOOL Node::PostDuplicate(UndoableOperation* pOp)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/10/96
	Inputs:		pOp - pointer to the operation that created the copy.
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	This function is called after this node has been created via a copy/duplicate
				type operation.  Nodes should override this function to do any post-processing.
	SeeAlso:	Node::PostImport
********************************************************************************************/
BOOL Node::PostDuplicate(UndoableOperation* pOp)
{
	return TRUE;
}



/********************************************************************************************

	void TextStory::MarkNodeAndChildren()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		-
    Purpose:    Mark this node and all its children

********************************************************************************************/

void Node::MarkNodeAndChildren()
{
	SetMarker(TRUE);
	MarkChildren();
}

/********************************************************************************************

	void TextStory::MarkChildren(Node* pNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		pNode = A pointer to a node
    Purpose:    Mark all children of this node

********************************************************************************************/

void Node::MarkChildren()
{
	Node* pNode = FindFirstChild();
	while (pNode!=NULL)
	{
		pNode->SetMarker(TRUE);
		pNode->MarkChildren();
		pNode=pNode->FindNext();
	}	
}


/********************************************************************************************

>	void Node::ClearMarks()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		-
    Purpose:    Clear marks from this node and all its children

********************************************************************************************/

void Node::ClearMarks()
{
	SetMarker(FALSE);
	ClearChildMarks();
}


/********************************************************************************************

>	void Node::ClearChildMarks()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		-
    Purpose:    Clear marks from all the children of this node

********************************************************************************************/

void Node::ClearChildMarks()
{
	Node* pNode = FindFirstChild();
	while (pNode!=NULL)
	{
		pNode->SetMarker(FALSE);
		pNode->ClearChildMarks();
		pNode=pNode->FindNext();
	}	
}



/********************************************************************************************

>	virtual BOOL Node::CanBecomeA(BecomeA* pBecomeA)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/94
	Inputs:		pClass 		= runtime class to node you wish this node to become
				pNumObjects = ptr to place number of objects of type pClass that will be created (Note: can be NULL).
							  *pNumObects in undefined on entry
	Outputs:	-
	Returns:	TRUE if it can, FALSE if it can't 
				Always returns FALSE
	Purpose:	Interrogation routine to see if a node can be changed into a different node type.

				The number you put into pNumObjects (if it's not NULL) should exactly equal the total number
				of pClass objects you create.  It should NOT contain any additional objects you may produce
				such as group objects for containing the pClass object, or attributes.

				Also, the entry value of *pNumObjects cannot be assumed to be 0.

	Errors:		-
	SeeAlso:	Node::DoBecomeA()

********************************************************************************************/

BOOL Node::CanBecomeA(BecomeA* pBecomeA)
{
	return FALSE;
}



/********************************************************************************************

>	virtual BOOL Node::DoBecomeA(BecomeA* pBecomeA)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/94
	Inputs:		pBecomeA = 	ptr to info class containing everything a node needs to be able
							to become something else
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
				Always returns TRUE
	Purpose:	Actually tries to change the node into a different node type.
	Errors:		-
	SeeAlso:	Node::CanBecomeA()

********************************************************************************************/

BOOL Node::DoBecomeA(BecomeA* pBecomeA)
{
	// Always return TRUE so that this routine won't upset the caller.
	// Even though the CanBecomeA() function returns FALSE, this does NOT guarantee that DoBecomeA() 
	// won't get called. 
	
	// E.g. Group::CanBecomeA() will return TRUE if ANY of its children return TRUE from their 
	// CanBecomeA() virt func.  When Group::DoBecomeA() is called, all the children's
	// DoBecomeA() funcs are called, so if they returned FALSE by default, the op will be aborted (this is bad).

	return TRUE;
}



/****************************************************************************

>	void Node::RemoveAttrTypeFromSubtree(CCRuntimeClass* AttrType, Node* pExceptThis)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/2005

	Inputs:		AttrType	- pointer to a CCRuntimeClass
				pExceptThis	- pointer to a Node
	Purpose:	Searches the subtree and every attribute which has type AttrType
				is deleted.

****************************************************************************/

void Node::RemoveAttrTypeFromSubtree(CCRuntimeClass* AttrType, Node* pExceptThis)
{
	// Traverse the subtree depth first
	Node* Current = FindFirstDepthFirst();
	Node* Next;
	while (Current != NULL)
	{
		Next =  Current->FindNextDepthFirst(this);
		// Determine if the Current node is to be hidden
		if (Current!=pExceptThis && Current->IsKindOf(CC_RUNTIME_CLASS(NodeAttribute)))
		{
			{ 
				if (((NodeAttribute*)Current)->GetAttributeType() == AttrType)
				{
					Current->CascadeDelete();
					delete Current;
				}
			}
		}
		Current = Next;
	}
}


#ifdef _DEBUG
/********************************************************************************************
>	void Node::DT()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/96
	Purpose:	Dump the document tree containing this node
********************************************************************************************/

void Node::DT()
{
#if DEBUG_TREE
	Node* pNode = this;
	while (pNode->FindParent())
		pNode = pNode->FindParent();
	DebugTreeDlg::DumpSubTree(pNode);
#endif
}

/********************************************************************************************
>	void Node::DST()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/96
	Purpose:	Dump the subtree starting at this node in the output stream
********************************************************************************************/

void Node::DST()
{
#if DEBUG_TREE
	DebugTreeDlg::DumpSubTree(this);
#endif
}

/********************************************************************************************
>	void Node::DST1(INT32 FormParent=0)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/96
	Inputs:		FromParent - levels to ascend before dumping sub tree
	Purpose:	Dump the subtree starting at this node in the output stream
********************************************************************************************/

void Node::DST1(INT32 FromParent)
{
#if DEBUG_TREE
	DebugTreeDlg::DumpSubTree(this,FromParent);
#endif
}

/********************************************************************************************
>	void Node::DST2(INT32 FormParent=0, INT32 MaxDepth=9999)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/96
	Inputs:		FromParent - levels to ascend before dumping sub tree
				MaxDepth   - max depth to descend to
	Purpose:	Dump the subtree starting at this node in the output stream
********************************************************************************************/

void Node::DST2(INT32 FromParent, INT32 MaxDepth)
{
#if DEBUG_TREE
	DebugTreeDlg::DumpSubTree(this,FromParent,MaxDepth);
#endif
}
#endif

/********************************************************************************************
>	virtual SubtreeRenderState Node::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/06/2004
	Inputs:		A Render Region to Render into.
	Returns:	PRE_FAILED for something went wrong,
				PRE_RENDER_CHILDREN for continue to render children,
				PRE_NO_RENDER_CHILDREN for DO NOT Render any of my children!!!!

	Purpose:	Enables Nodes to be able to Do Pre Print time alterations or even take over
				the control of the current Printing of their children.

********************************************************************************************/
SubtreeRenderState Node::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
	return SUBTREE_ROOTANDCHILDREN;
}

/********************************************************************************************

  >	virtual BOOL Node::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if the node has written out a record to the filter
				FALSE otherwise
	Purpose:	Writes out a record that represents the node.

				This function is called before any of the node's children are written to the filter.

				If the node writes out a record successfully to the file, it should return TRUE.

				If the node chooses not to write itself to the filter (e.g. because it is not appropriate
				for this filter), then this function should return FALSE.

				If an error occurs, the function should call pFilter->GotError(), and return FALSE.

				If the node does not want its child nodes to be written automatically, you should make
				sure you override CanWriteChildrenWeb() so it returns FALSE.
				
				(e.g. this node may choose to save its sub tree itself, so it can choose which of its
				child nodes should be saved out)

	SeeAlso:	CanWriteChildrenWeb(), WritePostChildrenWeb()

********************************************************************************************/

BOOL Node::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	return FALSE;
}

// See header for WritePreChildrenWeb().  The same information applies
BOOL Node::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	return FALSE;
}


/********************************************************************************************

  >	virtual BOOL Node::WritePostChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Gives the node a change to write data to the filter after its children
				have been automatically written out.

				This function will get called even if CanWriteChildrenWeb() has returned FALSE

	SeeAlso:	CanWriteChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/

BOOL Node::WritePostChildrenWeb(BaseCamelotFilter* pFilter)
{
	return TRUE;
}

// See header for WritePostChildrenWeb().  The same information applies
BOOL Node::WritePostChildrenNative(BaseCamelotFilter* pFilter)
{
	return TRUE;
}

/********************************************************************************************

  >	virtual BOOL Node::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok to automatically write out the children, FALSE otherwise
	Purpose:	If the node does not mind the filter automatically writing out its
				child nodes, then return TRUE.

				If, however, you wish to prevent this (e.g. because you want to save out 
				your child nodes yourself, in some selective fashion), then return FALSE

	SeeAlso:	WritePostChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/

BOOL Node::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)
{
	return TRUE;
}

// See header for CanWriteChildrenWeb.  The same information applies
BOOL Node::CanWriteChildrenNative(BaseCamelotFilter* pFilter)
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL Node::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Begin to write out you child records, in the web format

				The base class will write out a TAG_DOWN record, but only if it has a child ptr

	SeeAlso:	WritePostChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/

BOOL Node::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)
{
	Node* pChild = FindFirstChild();
	if (pChild != NULL)
	{
		// The node is considered to have children ONLY if at least one child is not a hidden node.
		if (!pChild->IsNodeHidden() || (pChild->FindNextNonHidden() != NULL))
			return pFilter->WriteZeroSizedRecord(TAG_DOWN);
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL Node::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Begin to write out you child records, in the native format

				The base class will write out a TAG_DOWN record, but only if it has a child ptr

	SeeAlso:	WritePostChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/

BOOL Node::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)
{
	Node* pChild = FindFirstChild();
	if (pChild != NULL)
	{
		// The node is considered to have children ONLY if at least one child is not a hidden node.
		if (!pChild->IsNodeHidden() || (pChild->FindNextNonHidden() != NULL))
			return pFilter->WriteZeroSizedRecord(TAG_DOWN);
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL Node::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Finished writing out you child records, in the web format

				The base class will write out a TAG_UP record, but only if it has a child ptr

	SeeAlso:	WritePostChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/

BOOL Node::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)
{
	Node* pChild = FindFirstChild();
	if (pChild != NULL)
	{
		// The node is considered to have children ONLY if at least one child is not a hidden node.
		if (!pChild->IsNodeHidden() || (pChild->FindNextNonHidden() != NULL))
			return pFilter->WriteZeroSizedRecord(TAG_UP);
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL Node::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Finished writing out you child records, in the native format

				The base class will write out a TAG_UP record, but only if it has a child ptr

	SeeAlso:	WritePostChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/

BOOL Node::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)
{
	Node* pChild = FindFirstChild();
	if (pChild != NULL)
	{
		// The node is considered to have children ONLY if at least one child is not a hidden node.
		if (!pChild->IsNodeHidden() || (pChild->FindNextNonHidden() != NULL))
			return pFilter->WriteZeroSizedRecord(TAG_UP);
	}

	return TRUE;
}

/********************************************************************************************

  >	virtual BOOL Node::ReadPostChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/96
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	This function is called during the importing of the file.

				A node gets called via this function after all its children have been imported.
				This allows complex nodes (such as moulds) to do some post-import work, which can
				only be done after all its children have been read in and created.

	SeeAlso:	CanWriteChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/

BOOL Node::ReadPostChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef RALPH
	// We don't want to be rendering at the same time as doing this, because we get
	// blobby text problems - fun, but bad !
	RalphCriticalSection RalphCS;
#endif

	return PostImport();

	//return TRUE;
}

// See header for ReadPostChildrenWeb().  The same information applies
BOOL Node::ReadPostChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef RALPH
	// We don't want to be rendering at the same time as doing this, because we get
	// blobby text problems - fun, but bad !
	RalphCriticalSection RalphCS;
#endif
	return PostImport();

	//return TRUE;
}


/********************************************************************************************

>	virtual BOOL Node::AreYouSafeToRender()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/96
	Inputs:		-
	Returns:	TRUE means yes, FALSE means no
	Purpose:	This function is used by the progressive rendering system & the new file format
				to determine whether the node is renderable or not

				The base class assumes that the node IS safe to render, as these are the most common


	SeeAlso:	CanWriteChildrenWeb(), WritePreChildrenWeb()

********************************************************************************************/

BOOL Node::AreYouSafeToRender()
{
	return TRUE;
}

/********************************************************************************************

>	virtual void Node::CountChildNodes(UINT32* pChildCount, UINT32* pChildAttrCount);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		pChildCount = place to put the number of child nodes (can be NULL)
				pChildAttrCount = place to put the number of child attribute nodes (can be NULL)
	Returns:	-
	Purpose:	This scans the child sibling list of this node, counting the number of nodes
				it finds and the number of attributes

				If does **not** do any extra tree traversal - only immediate children of this
				node are counted

				NOTE: Hidden nodes are not counted

	SeeAlso:	-

********************************************************************************************/

void Node::CountChildNodes(UINT32* pChildCount, UINT32* pChildAttrCount)
{
	UINT32 ChildCount = 0;
	UINT32 ChildAttrCount = 0;

	Node* pNode = FindFirstChild();
	while (pNode != NULL)
	{
		if (!pNode->IsNodeHidden())
		{
			ChildCount++;
			if (pNode->IsAnAttribute())
				ChildAttrCount++;
		}
		pNode = pNode->FindNext();
	}

	if (pChildCount != NULL)
		*pChildCount = ChildCount;

	if (pChildAttrCount != NULL)
		*pChildAttrCount = ChildAttrCount;
}

/********************************************************************************************

>	virtual BOOL Node::IsNodeInSubtree(Node* pChildNode)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/07/2000
	Inputs:		pChildNode	the node to test.

	Returns:	TRUE	if the given node lies in the subtree beneath this node.
				FALSE	otherwise or if the given node is NULL or if it *is* this node.

	Purpose:	Checks the parents of the given node, to see whether it lies within this
				node's subtree.

********************************************************************************************/
BOOL Node::IsNodeInSubtree(Node* pChildNode)
{
	// we return FALSE if pChildNode is this node, or is NULL.
	if (pChildNode == this || pChildNode == NULL)
		return FALSE;

	// check each of pNode's parents.
	while (pChildNode != NULL && pChildNode != this)
		pChildNode = pChildNode->FindParent();

	// we only return TRUE if we hit this Node, travelling up from pChildNode.
	return (pChildNode == this);
}

/********************************************************************************************

>	virtual BOOL Node::HasThisChildAttr(NodeAttribute* pAttr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		pAttr = ptr to an attribute
	Returns:	TRUE if it does contain an exact replica, FALSE otherwise
	Purpose:	This scans the child sibling list of this node, to see if it contains an attribute
				that's exactly the same as the given one.

				If does **not** do any extra tree traversal - only immediate child attrs of this
				node compared

	SeeAlso:	-

********************************************************************************************/

BOOL Node::HasThisChildAttr(NodeAttribute* pAttr)
{
	ERROR2IF(pAttr == NULL,FALSE,"NULL attr ptr");

	Node* pNode = FindFirstChild();
	while (pNode != NULL)
	{
		if (pNode->IsAnAttribute())
		{
			NodeAttribute* pOtherAttr = (NodeAttribute*)pNode;

			if (pAttr->GetAttributeType() == pOtherAttr->GetAttributeType())
			{
				if (*pAttr == *pOtherAttr)
					return TRUE;
			}
		}

		pNode = pNode->FindNext();
	}

	return FALSE;
}

/********************************************************************************************

>	virtual BOOL Node::AreChildAttrsIdentical(Node* pOtherNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		pOtherNode = ptr to the node to compare against
	Returns:	TRUE if attrs are identical, FALSE otherwise
	Purpose:	This scans the child sibling list of this node, to see of it contains the same attributes
				as pOtherNode

				If does **not** do any extra tree traversal - only immediate child attrs of this
				node and the other node compared

	SeeAlso:	-

********************************************************************************************/

BOOL Node::AreChildAttrsIdentical(Node* pOtherNode)
{
	ERROR2IF(pOtherNode == NULL,FALSE,"NULL node ptr");

	// Count the number of child attrs in this and the other nodes.
	UINT32 ChildAttrCount,OtherChildAttrCount;
	CountChildNodes(NULL,&ChildAttrCount);
	pOtherNode->CountChildNodes(NULL,&OtherChildAttrCount);

	// If they don't have the same number of child attrs, then they can't have an identical set
	if (ChildAttrCount != OtherChildAttrCount)
		return FALSE;

	Node* pNode = FindFirstChild();
	while (pNode != NULL)
	{
		if (pNode->IsAnAttribute())
		{
			if (!pOtherNode->HasThisChildAttr((NodeAttribute*)pNode))
				return FALSE;
		}

		pNode = pNode->FindNext();
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL Node::CopyChildAttrs(Node* pDestNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		pDestNode = ptr to node to copy attrs to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	This scans the child sibling list of this node, and copies all the child attributes,
				applying the copy as a child to the destination node.

	SeeAlso:	-

********************************************************************************************/

BOOL Node::CopyChildAttrs(Node* pDestNode)
{
	ERROR2IF(pDestNode == NULL,FALSE,"NULL dest node ptr");

	Node* pLastInsert = NULL;
	Node* pSrcNode = FindFirstChild();
	while (pSrcNode != NULL)
	{
		if (pSrcNode->IsAnAttribute())
		{
			Node* pCopy = pSrcNode->SimpleCopy();
			if (pCopy == NULL)
				return(FALSE);

			if (pLastInsert == NULL)
			{
				pCopy->AttachNode(pDestNode,FIRSTCHILD);
			}
			else
			{
				pCopy->AttachNode(pLastInsert,NEXT);
			}
			pLastInsert = pCopy;
		}

		pSrcNode = pSrcNode->FindNext();
	}

	return TRUE;
}



/********************************************************************************************

>	virtual BOOL Node::IsDifferent(Node *pOther)

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

BOOL Node::IsDifferent(Node *pOther)
{
	// The base class merely checks that the 2 objects are of the same class
	if (GetRuntimeClass() != pOther->GetRuntimeClass())
		return(TRUE);

	// If they're the same type, then as far as we know, they're the same.
	// Derived classes will override this function to do a better check.
	return(FALSE);
}


/********************************************************************************************

>	virtual BOOL Node::SetParentLayerAsEdited()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Returns:	True if worked ok, False otherwise
	Purpose:	Mark parent layer as edited. If an ink object as something done to it which
				changes its appearance then we need to mark the parent layer as edited.
				In this baseclass version do nothing.

********************************************************************************************/

BOOL Node::SetParentLayerAsEdited()
{
	return TRUE;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//	Attribute scanning for the Attribute gallery.


/********************************************************************************************
>	NodeAttribute* Node::FindFirstAttr(TypeFunc pfnTest) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95
	Inputs:		pfnTest			---		pointer to a member-function of Node:-
										typedef BOOL (Node::*TypeFunc)() const;
										that returns TRUE for the attribute being
										searched for and FALSE otherwise
	Returns:	The first attribute of the given type that applies to pInk, or null
				if there isn't one.  Call NodeAttribute::FindNextAttr on the returned
				attribute to find the next attribute in scope and so on.
	SeeAlso:	NodeAttribute::FindNextAttrClass
********************************************************************************************/

NodeAttribute* Node::FindFirstAttr(TypeFunc pfnTest) const
{
	// Begin searching from the last child of pInk, or if there isn't one, from pInk itself.
	const Node* pNode = FindLastChild();
	if (pNode == 0) pNode = this;

	// Search for the class in the previous sibling nodes, then the parent's siblings,
	// then the parent siblings of the parent of ... etc.
	while (!(pNode->*pfnTest)())
	{
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
	}

	// Return the attribute, or null if none was found.
	return (NodeAttribute*) pNode;
}



/********************************************************************************************

>	virtual DocRect Node::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	
	Returns:	TRUE if extending this Node would be a valid operation, ie by passing
				appropriate inverse extend parameters, the Node could be un-extended
				back to its original state.
				FALSE if the operation would be irreversible.
	Purpose:	Tests the reversibility of an Extend operation applied to this node.

				Note that where appropriate, this function asks the Node's children if they
				may validly extend, only returning TRUE if the extension is completely ok
				with everyone.

				Also note that if a node is of a non-extending type, it should normally
				consider itself a 'valid extender', as doing nothing is completely reversible
				(it should of course still check its children).
	Errors:		
	See also:	Node::IsTypeExtendible(), Node::Extend().

********************************************************************************************/
DocRect Node::ValidateExtend(const ExtendParams& ExtParams)
{
	DocRect drMinExtend(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX), drThisMinExtend;
	for (	Node* pChildNode = FindFirstChild();
			pChildNode != NULL;
			pChildNode = pChildNode->FindNext()	)
	{
		drThisMinExtend = pChildNode->ValidateExtend(ExtParams);
		if (drMinExtend.lo.x > drThisMinExtend.lo.x) drMinExtend.lo.x = drThisMinExtend.lo.x;
		if (drMinExtend.lo.y > drThisMinExtend.lo.y) drMinExtend.lo.y = drThisMinExtend.lo.y;
		if (drMinExtend.hi.x > drThisMinExtend.hi.x) drMinExtend.hi.x = drThisMinExtend.hi.x;
		if (drMinExtend.hi.y > drThisMinExtend.hi.y) drMinExtend.hi.y = drThisMinExtend.hi.y;
	}
	return drMinExtend;
}



/********************************************************************************************

>	virtual void Node::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	This node and some of its children may have their dimensions altered.
	Returns:	
	Purpose:	Perform an Extend operation on this Node, and its children if appropriate.
				Default implementation just calls Extend() on its children.
	Errors:		
	See also:	

********************************************************************************************/
void Node::Extend(const ExtendParams& ExtParams)
{
	for (	Node* pChildNode = FindFirstChild();
			pChildNode != NULL;
			pChildNode = pChildNode->FindNext()	)
	{
		pChildNode->Extend(ExtParams);
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////
// AttrTypeSet methods


/********************************************************************************************

>	BOOL AttrTypeSet::AddToSet(CCRuntimeClass* AttrType)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Returns:	FALSE if we run out of memory
	Purpose:	Adds AttrType to the set
	SeeAlso:	AttrTypeSet::InSet
	SeeAlso:	AttrTypeSet::CopySet


********************************************************************************************/

BOOL AttrTypeSet::AddToSet(CCRuntimeClass* AttrType)
{
	ERROR3IF(AttrType == NULL, "AddToSet: AttrType is NULL"); 
	// Determine if the AttrType is already in the set
	AttrTypeItem* pAttrType = (AttrTypeItem*) GetHead();
	while (pAttrType != NULL)
	{
		if (pAttrType->AttributeType == AttrType)
		{
			return TRUE; // Already in set so return
		}
		pAttrType =  (AttrTypeItem*) GetNext(pAttrType);
	}
	
	// The attribute type is not in the set so let's add it
	pAttrType = new  AttrTypeItem;
	if (!pAttrType)
		return FALSE; 	// out of memory (error has been set)

	pAttrType->AttributeType = AttrType;

	AddHead(pAttrType); // Add attribute to the head of the list
	 
	return TRUE; 
}

/********************************************************************************************

>	BOOL AttrTypeSet::AddToSet(List& Attributes)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Inputs:		A list of attributes (each item must be a NodeAttributePtrItem)
	Returns:	FALSE if we run out of memory. 
	Purpose:	Adds the Types of all attributes in the Attributes list 
	SeeAlso:	AttrTypeSet::InSet
	SeeAlso:	AttrTypeSet::CopySet


********************************************************************************************/

BOOL AttrTypeSet::AddToSet(List& Attributes)
{
	NodeAttributePtrItem* pAttrItem = (NodeAttributePtrItem*)(Attributes.GetHead());
	NodeAttribute* pAttr;
	while (pAttrItem)
	{
		pAttr = pAttrItem->NodeAttribPtr;
		ERROR3IF(!pAttr, "Should be an attribute"); 
		if (!AddToSet(pAttr->GetAttributeType()))
		{
			return FALSE;
		} 
		pAttrItem = (NodeAttributePtrItem*)(Attributes.GetNext(pAttrItem));
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL AttrTypeSet::CopySet()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		-
	Outputs:	-
	Returns:	A copy of the set, or NULL if we run out of memory
	Purpose:	Makes a copy of the attribute type set
	Errors:		-
	SeeAlso:	AttrTypeSet::AddToSet
	SeeAlso:	AttrTypeSet::InSet

********************************************************************************************/

AttrTypeSet* AttrTypeSet::CopySet()
{
	AttrTypeSet* pSetCopy; 
	pSetCopy = new AttrTypeSet;
	if (!pSetCopy)
		return NULL; // Out of memory

	// Copy each item in turn
	AttrTypeItem* pItemCopy;
	AttrTypeItem* pCurrent = (AttrTypeItem*)GetHead(); 
	while (pCurrent)
	{
		pItemCopy = new AttrTypeItem;
		if (!pItemCopy)
		{
			// Tidyup
			pSetCopy->DeleteAll();
			delete pSetCopy;
			return NULL;  
		}
		pItemCopy->AttributeType = pCurrent->AttributeType;
		pSetCopy->AddTail(pItemCopy); 
		pCurrent = (AttrTypeItem*)GetNext(pCurrent); 	
	}
	return pSetCopy;
}


/********************************************************************************************
>	BOOL AttrTypeSet::InSet(CCRuntimeClass* AttrType)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		AttrType: The type of an attribute
	Outputs:	-
	Returns:	TRUE if AttrType is in the set
	Purpose:	To determine if AttrType is in the set	
	Errors:		-
	SeeAlso:	AttrTypeSet::CopySet
	SeeAlso:	AttrTypeSet::AddToSet
********************************************************************************************/

BOOL AttrTypeSet::InSet(CCRuntimeClass* AttrType)
{
	AttrTypeItem* pCurrent = (AttrTypeItem*)GetHead(); 
	while (pCurrent)
	{
		if (pCurrent->AttributeType == AttrType)
			return TRUE; 
		pCurrent = (AttrTypeItem*)GetNext(pCurrent); 	
	}
	return FALSE; 
}  		  

