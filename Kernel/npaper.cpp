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

// NodeRenderablePaper class implementation

/*
//*/

#include "camtypes.h"
#include "npaper.h"
#include "errors.h"
//#include "mario.h"
#include "ensure.h"
#include "nodedoc.h"
#include "oilcoord.h"
  
CC_IMPLEMENT_DYNAMIC(NodeRenderablePaper, NodeRenderableBounded)

 
/***********************************************************************************************

>    NodeRenderablePaper::NodeRenderablePaper() 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:    -
     Outputs:   
     Returns:   -
			  
     Purpose: This constructor creates a NodeRenderablePaper linked to no other with all status
		 	  flags false and uninitialised bounding and pasteboard rectangles.            
			
     Errors:    

***********************************************************************************************/
 

NodeRenderablePaper::NodeRenderablePaper(): NodeRenderableBounded()
{
} 
  
  
/***********************************************************************************************

>   void NodeRenderablePaper::NodeRenderablePaper
	(
		Node* ContextNode,  
		AttachNodeDirection Direction,  
		BOOL Locked = FALSE, 
		BOOL Mangled = FALSE,  
		BOOL Marked = FALSE, 
		BOOL Selected = FALSE, 
	)

    Author:  Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created: 26/4/93             
    
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

NodeRenderablePaper::NodeRenderablePaper(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected   
			  ):NodeRenderableBounded(ContextNode, Direction, Locked, Mangled, Marked, 
			 	Selected) 
{ 
} 


/********************************************************************************************

>	BOOL NodeRenderablePaper::NeedsToRender(RenderRegion *pRender)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/94
	Inputs:		pRender - the render region in question (NULL if none)
	Returns:	FALSE => we never want to render paper objects while rendering ink objects.
	Purpose:	Indicate that we don't want to render paper objects in the ink loop.
	SeeAlso:	NodeRenderablePaper::NeedsToExport

********************************************************************************************/

SubtreeRenderState NodeRenderablePaper::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
//#pragma message( __LOCMSG__ "Removed RenderRegion usage" )
	if (pRender && pRender->RenderPaperAsInk())
		return SUBTREE_ROOTANDCHILDREN;

	return SUBTREE_NORENDER;
}




/********************************************************************************************

>	BOOL NodeRenderablePaper::IsPaper() const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/94
	Returns:	TRUE => we are a paper node.
	Purpose:	Indicate that we ARE paper! (Overrides virtual func in Node.)
	SeeAlso:	Node::IsPaper

********************************************************************************************/

BOOL NodeRenderablePaper::IsPaper() const
{
	return TRUE;
}




/********************************************************************************************

>	BOOL NodeRenderablePaper::NeedsToExport(RenderRegion *pRender, BOOL VisibleLayersOnly = FALSE,
											BOOL CheckSelected = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/94
	Inputs:		pRender - A pointer to the current export region (null if none)
				VisibleLayersOnly - TRUE => remove nodes which are on invisible layers
								   - FALSE => export everything
				CheckSelected - TRUE => we check if object selected and only export selected bjects
							  - FALSE => we don't bother checking for selection or not
	Returns:	FALSE => we never want to export NodeRenderablePaper objects.
	Purpose:	Indicate that we don't want to export this class of nodes.
	SeeAlso:	NodeRenderablePaper::NeedsToRender

********************************************************************************************/

BOOL NodeRenderablePaper::NeedsToExport(RenderRegion *pRender, BOOL VisibleLayersOnly, BOOL CheckSelected)
{
	return FALSE;
}

     
/***********************************************************************************************
> Node* NodeRenderablePaper::SimpleCopy() // Private method  

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:       
    Outputs:    
    Returns:    A copy of the node or NULL if memory runs out
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	
  	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   

**********************************************************************************************/
     
Node* NodeRenderablePaper::SimpleCopy()
{
	NodeRenderablePaper* NodeCopy; 
	NodeCopy = new NodeRenderablePaper();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);   
	return (NodeCopy);
}               


/***********************************************************************************************
>   void NodeRenderablePaper::CopyNodeContents(Node* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:     -     
    Outputs:    A copy of this node
   
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void NodeRenderablePaper::CopyNodeContents(NodeRenderablePaper* NodeCopy)
{                         
	ENSURE(NodeCopy != NULL,"Trying to copy a node's contents into a NULL node");  
	NodeRenderableBounded::CopyNodeContents(NodeCopy); 
 	NodeCopy->PasteboardRect = PasteboardRect;         
}       
   
                                             
/***********************************************************************************************
>   void NodeRenderablePaper::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeRenderablePaper::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeRenderablePaper), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeRenderablePaper))
		CopyNodeContents((NodeRenderablePaper*)pNodeCopy);
}



/***********************************************************************************************

>	virtual void NodeRenderablePaper::ChangePasteboardRect(const DocRect& PasteRect)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/93
	Inputs:		PasteRect: Rectangle representing the outer limits of the pasteboard 
						    
	Outputs:	-
	Returns:	-               
	
	Purpose:    To change the pasteboard rectangle of this node, and then if necessary
				recursively change the pasteboard rectangles of all its parents. 
				
				The method also changes the document extents in the NodeDocument node at 
				the root of the tree. 
				
	Errors:		An assertion failure will occur if the parent of a paper object is not a 
				paper object or the root of the tree is not a NodeDocument
	
	Scope:      protected

	SeeAlso:	NodeRenderablePaper::SetInitialPasteboardRect

***********************************************************************************************/

void NodeRenderablePaper::ChangePasteboardRect(const DocRect& PasteRect)  
{   
	// Check if the pasteboard rectangle should be changed
	if (PasteboardRect != PasteRect)
	{
		PasteboardRect = PasteRect;  // Set the pasteboard rectangle     
	                       
		// If the node has a parent then we will need to change its    
		// Pasteboard rectangle
		if (Parent != NULL)	  
		{
			// CombinedSiblingPasteboardRectangle will be the smallest rectangle which 
			// surrounds all sibling pasteboard rectangles.  
			DocRect CombinedSiblingPasteboardRectangle; 
		
			// Take the union of this nodes Pasteboard rectangle with all its siblings 
			// pasteboard rectangles. 
			Node* CurrentNode = this->FindParent()->FindFirstChild(); 
			while (CurrentNode != NULL)
			{       
				if (CurrentNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderablePaper)))	
				{
					CombinedSiblingPasteboardRectangle = CombinedSiblingPasteboardRectangle.Union(
						( ((NodeRenderablePaper*)CurrentNode)->GetPasteboardRect(FALSE)) ); 
				}			
				CurrentNode = CurrentNode->FindNext(); 		
			}       
		                              
            // The parent of a paper object should always be a paper object
            ENSURE(Parent->IsKindOf(CC_RUNTIME_CLASS(NodeRenderablePaper)), 
            	   "The parent of a paper object was not a paper object"); 
            
			// Call the routine recursively to set the parents pasteboard rectangle  
			((NodeRenderablePaper*)Parent)->ChangePasteboardRect(
				CombinedSiblingPasteboardRectangle);  
		}       
		else
		{
			// The root of the tree should be a NodeDocument
			ENSURE(this->IsKindOf(CC_RUNTIME_CLASS(NodeDocument)),
				   "When trying to set the document extents no\nNodeDocument node was found at the root of the tree");
			// Set the document extents
			((NodeDocument*)this)->SetExtents(); 
		}
	}   
}         

                                             
/***********************************************************************************************

>	virtual void NodeRenderablePaper::SetInitialPasteboardRect(const DocRect& PasteRect)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/93
	Inputs:		PasteRect: Rectangle representing the outer limits of the pasteboard 
						    
	Outputs:	-
	Returns:	-                                          
				
	Purpose:	To set the initial pasteboard rectangle of this node, and then change the 
				pasteboard rectangles of all its parents. 
	                
	            The method also changes the document extents in the NodeDocument node at 
				the root of the tree. 
				
	Errors:		An assertion failure will occur if the parent of a paper object is not a 
				paper object or the root of the tree is not a NodeDocument.  
				
	Scope:      protected
				
	SeeAlso:	NodeRenderablePaper::ChangePasteboardRectangle

***********************************************************************************************/

void NodeRenderablePaper::SetInitialPasteboardRect(const DocRect& PasteRect)  
{    
	// Check if the pasteboard rectangle should be changed
	if (PasteboardRect != PasteRect)
	{
		PasteboardRect = PasteRect;  // Set the pasteboard rectangle     
	                       
		// If the node has a parent then we will need to change its    
		// Pasteboard rectangle
		if (Parent != NULL)	  
		{
			// The parent of a paper object should always be a paper object
			ENSURE(Parent->IsKindOf(CC_RUNTIME_CLASS(NodeRenderablePaper)),
				   "The parent of a paper object was not a paper object!");   
			  
			// Union the parents pasteboard rectangle with this nodes pasteboard
			// rectangle to obtain the parents new pasteboard rectangle.   
            DocRect NewParentPasteboardRectangle = 
            	((NodeRenderablePaper*)Parent)->GetPasteboardRect(FALSE); 
            NewParentPasteboardRectangle = NewParentPasteboardRectangle.Union(PasteRect);     
                
    		// Call the routine recursively to set the parent's pasteboard rectangle     
			((NodeRenderablePaper*)Parent)->SetInitialPasteboardRect
				(NewParentPasteboardRectangle);   
		}   
		else
		{
			// The root of the tree should be a NodeDocument
			ENSURE(this->IsKindOf(CC_RUNTIME_CLASS(NodeDocument)), 
				   "The root of the document tree was not a NodeDocument node");    
			// Set the document extents
			((NodeDocument*)this)->SetExtents(); 
		}
	}   
}                          

/*********************************************************************************************

>    virtual DocRect NodeRenderablePaper::GetPasteboardRect(BOOL Pixelise = TRUE,
															View *pView = NULL) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:    Pixelise - whether or not to pixelise the pasteboard rectangle before
						   returning it.
				pView - the view to pixelise to (not used if Pixelise is FALSE).
     Outputs:   The objects pasteboard rectangle
     Returns:   -
			  
     Purpose:   For obtaining the objects pasteboard rectangle   
			
     Errors:    

**********************************************************************************************/

DocRect NodeRenderablePaper::GetPasteboardRect(BOOL Pixelise, View *pView) const
{
	DocRect temp = PasteboardRect;

	if (Pixelise)
	{
		// "Pixelise" the position of the spread.
		// Effectively, this ensures that the spread will be aligned to a whole pixel boundary
		// and allows both GDraw and GDI to consistently plot the same pixels when rendering
		// the same primitive
		if (pView != NULL)
		{
			temp.lo.Pixelise(pView);
			temp.hi.Pixelise(pView);
		}
		else
		{
			temp.lo.Pixelise();
			temp.hi.Pixelise();
		}
	}

	return (temp); 

//	return (PasteboardRect); 
}       


/********************************************************************************************

>	void* NodeRenderablePaper::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/93
	Inputs:		-
	Outputs:	Str: String giving debug info about the node
	Returns:	-
	Purpose:	For obtaining debug information about the Node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

     
void NodeRenderablePaper::GetDebugDetails(StringBase* Str) 
{
#ifdef _DEBUG
	NodeRenderableBounded::GetDebugDetails(Str);  
	
	String_256 TempStr; 
	if (!PasteboardRect.IsValid())
	{ 
		TempStr = TEXT("\r\nPasteboard Rectangle = *INVALID*\r\n"); 
	}
	else 
		TempStr._MakeMsg(TEXT("\r\nPasteboard Rectangle\r\n   Low(#1%ld, #2%ld)\r\n   High(#3%ld, #4%ld)\r\n"),    
					PasteboardRect.LowCorner().x,  
					PasteboardRect.LowCorner().y,
					PasteboardRect.HighCorner().x,  
					PasteboardRect.HighCorner().y);      
					
	(*Str)+=TempStr; 
#endif
}    
