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

/*
*/

#include "camtypes.h"
#include "ndmldink.h"
#include "nodemldr.h"
#include "moldshap.h"
#include "nodebmp.h"
//#include "mike.h"
//#include "mario.h"
#include "nodemold.h"
#include "bitmap.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNAMIC( NodeMouldBitmap, NodeRenderableInk )  

#define new CAM_DEBUG_NEW



/*********************************************************************************************

>    NodeMouldBitmap::NodeMouldBitmap() 

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	19/12/94
     Inputs:	
     Outputs:	
     Returns:	
     Purpose: 	This constructor creates a NodeMouldBitmap linked to no other, with all status
		  	  	flags false and an uninitialised bounding rectangle.           
     Errors:    

**********************************************************************************************/
 
NodeMouldBitmap::NodeMouldBitmap(): NodeRenderableInk()
{
}


/***********************************************************************************************

>	void NodeMouldBitmap::NodeMouldBitmap
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

NodeMouldBitmap::NodeMouldBitmap(Node* ContextNode,  
					 AttachNodeDirection Direction,  
					 BOOL Locked, 
				 	 BOOL Mangled,  
					 BOOL Marked, 
					 BOOL Selected   
			   ):NodeRenderableInk(ContextNode, Direction, Locked, Mangled, Marked,	Selected) 
{ 
} 


/*********************************************************************************************

>    NodeMouldBitmap::~NodeMouldBitmap() 

     Author:	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:	5/12/94
     Purpose: 	default destructor for the NodeMouldBitmap class

**********************************************************************************************/
 
NodeMouldBitmap::~NodeMouldBitmap()
{
}



/***********************************************************************************************

>	void NodeMouldBitmap::Render(RenderRegion* pRender)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		Pointer to a render region
	Purpose:	Will construct a moulded path from the renderable reference nodes bounding
				rectangle and render the reference node into it.
				This currently only makes sense for bitmap nodes. These are usually rendered
				as bitmap fills inside a rectangle. If we can generate the destination path
				by moulding the NodeBitmap bounding rectangle, we should be able to render
				the bitmap into the result.

***********************************************************************************************/

void NodeMouldBitmap::Render(RenderRegion* pRegion)
{            
	// find the mould root object
	/*
	NodeMould* pNodeMould = (NodeMould*)this->FindParent(CC_RUNTIME_CLASS(NodeMould));
	MouldGeometry* pMouldGeom = pNodeMould->GetGeometry();
	if (pMouldGeom)
		pMouldGeom->MouldBitmapRender(&TheBitmap, Parallel, pRegion);
	*/
}


/********************************************************************************************

>	virtual String NodeMouldBitmap::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		Plural: Flag indicating if the string description should be plural or
						singular. 
	Outputs:	-
	Retuns:		Description of the mould node 
	Purpose:	To return a description of the NodeMouldBitmap object in either the singular
				or the plural. This method is called by the DescribeRange method.
				The description will always begin with a lower case letter.   
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

String NodeMouldBitmap::Describe(BOOL Plural, BOOL Verbose) 
{     
	if (Plural)
		return(String(_R(IDS_MOULDINK_DESCRP)));  
	else
		return(String(_R(IDS_MOULDINK_DESCRS))); 
}; 




/***********************************************************************************************

> Node* NodeMouldBitmap::SimpleCopy()  

    Author:     Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	19/12/94
	Inputs:     -  
    Outputs:    -
    Returns:    A copy of the node, or NULL if memory has run out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	            
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL.                                                                      
                                                                                 
**********************************************************************************************/

Node* NodeMouldBitmap::SimpleCopy()
{
	NodeMouldBitmap* pCopyOfNode = new NodeMouldBitmap();
	ERROR1IF(pCopyOfNode == NULL,NULL,_R(IDE_NOMORE_MEMORY)); 
	CopyNodeContents(pCopyOfNode);
	return (pCopyOfNode);
}   


/***********************************************************************************************

>	void NodeMouldBitmap::CopyNodeContents(NodeMouldBitmap* pCopyOfNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pCopyOfNode - The node to copy data to
	Outputs:	-
	Returns:	-
	Purpose:	Copies the data from this node to pCopyOfNode by first calling the base class 
				to get it to copy its stuff, and then copying its own stuff

***********************************************************************************************/

void NodeMouldBitmap::CopyNodeContents(NodeMouldBitmap* pCopyOfNode)
{
	ERROR3IF(pCopyOfNode == NULL,"NodeMouldBitmap::CopyNodeContents was asked to copy into a NULL pointer");

	// Copy from the base class
	NodeRenderableInk::CopyNodeContents(pCopyOfNode);

}

   
/***********************************************************************************************
>   void NodeMouldBitmap::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeMouldBitmap::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeMouldBitmap), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeMouldBitmap))
		CopyNodeContents((NodeMouldBitmap*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 NodeMouldBitmap::GetNodeSize() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
********************************************************************************************/

UINT32 NodeMouldBitmap::GetNodeSize() const 
{     
	return (sizeof(NodeMouldBitmap)); 
}  
