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
// NodeSimpleShape class. This provides a base class for all simple primitve shapes
// eg, Rectangles and Ellipses.

/*
*/


#include "camtypes.h"
#include "nodeshap.h"
//#include "osrndrgn.h"
#include "nodepath.h"
#include "ops.h"
#include "document.h"
#include "snap.h"
#include "becomea.h"
#include "blobs.h"
#include "app.h"
#include "extender.h"

#include "ophist.h"

// Put my version Number into the About box
DECLARE_SOURCE( "$Revision: 662 $" );


CC_IMPLEMENT_DYNCREATE(NodeSimpleShape, NodeRenderableInk)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



/***********************************************************************************************

>	NodeSimpleShape::NodeSimpleShape(Node* 	ContextNode,
						AttachNodeDirection Direction,
						const DocRect& 		BoundingRect,
						BOOL				Locked = FALSE,
						BOOL 				Mangled = FALSE,
						BOOL 				Marked = FALSE,
						BOOL 				Selected = FALSE,
						BOOL 				Renderable = FALSE
						)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/93
    
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
	Note:		SetUpPath() must be called before the NodeSimpleShape is in a state in which it can be used.
                
    SeeAlso: SetUpPath   
    Errors: An assertion error will occur if ContextNode is NULL

***********************************************************************************************/

NodeSimpleShape::NodeSimpleShape(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ):NodeRenderableInk(ContextNode, Direction, Locked, Mangled, Marked, Selected )  
{                         
}                        
 
/*********************************************************************************************

>	NodeSimpleShape::NodeSimpleShape() 

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/93
	Purpose:	This constructor creates a NodeSimpleShape linked to no other with all status
				flags false and an uninitialized bounding rectangle.           
	Note:		SetUpPath() must be called before the NodeSimpleShape is in a state in which it can be 
				used. 		
	SeeAlso:	SetUpPath                                                        

**********************************************************************************************/
/* Technical Notes:   
	The default constructor is required so that SimpleCopy will work 
*/  

NodeSimpleShape::NodeSimpleShape(): NodeRenderableInk()
{
}

/***********************************************************************************************

>	virtual Node* NodeSimpleShape::SimpleCopy()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Returns:	Pointer to a Node
	Purpose:	Makes a copy of all the data in the node

***********************************************************************************************/

Node* NodeSimpleShape::SimpleCopy()
{
	// Make a new NodeSimpleShape and then copy things into it
	TRACE( _T("NodeSimpleShape::SimpleCopy() - Should not be called\n"));
	NodeSimpleShape* NodeCopy = new NodeSimpleShape();
    if (NodeCopy)
    	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}            


/***********************************************************************************************

>	void NodeSimpleShape::CopyNodeContents(NodeSimpleShape* NodeCopy)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Inputs:		NodeCopy - The node to copy
	Purpose:	Copies the data in the node by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeRenderableInk::CopyNodeContents

***********************************************************************************************/

void NodeSimpleShape::CopyNodeContents(NodeSimpleShape* NodeCopy)
{
	NodeRenderableInk::CopyNodeContents(NodeCopy);
	
	//Copy contents specific to derived class here
	if (NodeCopy->InkPath.Initialise(InkPath.GetNumCoords(),12))
	{
		// Copy the path data
		NodeCopy->InkPath.CopyPathDataFrom(&InkPath);

		// copy the parralleogram data
		NodeCopy->Parallel[0] = Parallel[0];
		NodeCopy->Parallel[1] = Parallel[1];
		NodeCopy->Parallel[2] = Parallel[2];
		NodeCopy->Parallel[3] = Parallel[3];
	}
}



/***********************************************************************************************
>   void NodeSimpleShape::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeSimpleShape::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeSimpleShape), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeSimpleShape))
		CopyNodeContents((NodeSimpleShape*)pNodeCopy);
}



/***********************************************************************************************

>	void NodeSimpleShape::ShowDebugTreeDetails() const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Purpose:	Displays debugging info of the tree
	SeeAlso:	NodeRenderableInk::ShowDebugTreeDetails

***********************************************************************************************/
#ifdef _DEBUG
void NodeSimpleShape::ShowDebugTreeDetails() const
{                     
	TRACE( _T("SimpleShape ")); 
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeRenderableInk::ShowDebugTreeDetails();	
}
#endif

/********************************************************************************************

>	void NodeSimpleShape::GetDebugDetails( StringBase* Str )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/93
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node

********************************************************************************************/

void NodeSimpleShape::GetDebugDetails( StringBase* Str )
{
#ifdef _DEBUG
	// Call base class
	NodeRenderableInk::GetDebugDetails( Str );
	
	InkPath.FindStartOfPath();
	String_256 TempStr;
		
	(*Str) += TEXT( "\r\nEllipse Path Data Dump\r\n" );

	TempStr._MakeMsg( TEXT(	"Parallelogram :-\r\n\t#1%ld,\t#2%ld\r\n"
							"\t#3%ld,\t#4%ld\r\n"
							"\t#5%ld,\t#6%ld\r\n"
							"\t#7%ld,\t#8%ld\r\n"),
						Parallel[0].x, Parallel[0].y,
						Parallel[1].x, Parallel[1].y,
						Parallel[2].x, Parallel[2].y,
						Parallel[3].x, Parallel[3].y );
	(*Str) += TempStr;

	DocRect BlobRect = GetBlobBoundingRect();
	TempStr._MakeMsg( TEXT("Blob Bounding Rect :-\r\n\t#1%ld,\t#2%ld\r\n\t#3%ld,\t#4%ld\r\n"),
					  BlobRect.lo.x, BlobRect.lo.y, BlobRect.hi.x, BlobRect.hi.y );
	(*Str) += TempStr;
	
	if ( InkPath.IsFilled )
		(*Str) += TEXT( "The Path is Filled\r\n" );

	(*Str) += TEXT( "\r\nNum\tType\tX Coord\tY Coord\r\n" );
	PathVerb* Verbs  = InkPath.GetVerbArray();
	DocCoord* Coords = InkPath.GetCoordArray();
//	PathFlags* Flags = InkPath.GetFlagArray();
	for (INT32 i=0; i<InkPath.GetNumCoords(); i++)
	{
		// Add the info to the string
		TempStr._MakeMsg( TEXT("#1%d.\t#2%d\t#3%ld,\t#4%ld\r\n"),
						  i, Verbs[i], Coords[i].x, Coords[i].y );
		(*Str) += TempStr;
	}
#endif
}



/***********************************************************************************************

>	BOOL NodeSimpleShape::SetUpPath(INT32 RequiredSize=12, INT32 BlockSize=12)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/5/93
	Inputs:		RequiredSize - The size of the initial block of memory to be allocated to the
				path.
				BlockSize - The size of the block asked for by the path object when it
				runs out of space. 
	Returns:	TRUE if the path was init'ed ok, FALSE otherwise
	Purpose:	To initialise the path used by the shape into a state that can be used,
				by allocating memory, setting up member variables properly.

***********************************************************************************************/

BOOL NodeSimpleShape::SetUpPath(INT32 RequiredSize, INT32 BlockSize)
{
	return (InkPath.Initialise(RequiredSize, BlockSize)); 
}



/********************************************************************************************

>	virtual void NodeSimpleShape::Transform( TransformBase& Trans )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/93
	Inputs:		Trans - The transform Object
	Purpose:	Transforms the shapes parallelogram and then re-builds the shape to fit it.
	SeeAlso:	NodeRenderableInk::Transform()

********************************************************************************************/

void NodeSimpleShape::Transform( TransformBase& Trans )
{
	// Transform the Shape
	Trans.Transform((DocCoord*)Parallel, 4);

	// re-create the path and update its bounding rectangle
	UpdateShape();

	// Mark the bounding rect as invalid
	InvalidateBoundingRect();

	// Transform all the children...
	TransformChildren(Trans);
}




/***********************************************************************************************

>	void NodeSimpleShape::CreateShape(DocRect ARect)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/93                          
	Inputs:		DocRect - representing the rectangle to be created.
	Purpose:	Should create a the shape to fit the current parallelogram. This version does
				nothing as there should always be a derived class version.

***********************************************************************************************/

void NodeSimpleShape::CreateShape(DocRect NewRect)
{
	// You are not supposed to be calling this. You should have called a
	// derived classes version of this function
	ENSURE(FALSE, "NodeSimpleShape::CreateShape - You called the base class version");
}

/***********************************************************************************************

>	void NodeSimpleShape::UpdateShape(DocRect ARect)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/93                          
	Inputs:		DocRect - representing the rectangle to be created.
	Purpose:	Should update the the shape to the current parallelogram. This version does
				nothing as there should always be a derived class version.

***********************************************************************************************/

void NodeSimpleShape::UpdateShape()
{
	// You are not supposed to be calling this. You should have called a
	// derived classes version of this function
	ENSURE(FALSE, "NodeSimpleShape::UpdateShape - You called the base class version");
}




/***********************************************************************************************

>	void NodeSimpleShape::Render(RenderRegion* pRender)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/93
	Inputs:		Pointer to a render region
	Purpose:	Will render the path contained within the object to the given render region

***********************************************************************************************/

void NodeSimpleShape::Render(RenderRegion* pRender)
{            
	// render the path
//#pragma message( __LOCMSG__ "Removed RenderRegion usage" )
	pRender->DrawPath(&InkPath);
}  



/********************************************************************************************

>	void NodeSimpleShape::RenderEorDrag( RenderRegion* pRender )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/93
	Inputs:		pRender - A Pointer to the current RenderRegion
	Purpose:	Renders a version of the path for EORed dragging of shapes.
	SeeAlso:	NodePath::Render; NodeRenderableInk::RenderEorDrag

********************************************************************************************/

void NodeSimpleShape::RenderEorDrag( RenderRegion* pRender )
{
	// Render an EORed version of the shape. For a simple object such
	// as this we can render it as normal
//#pragma message( __LOCMSG__ "Removed RenderRegion usage" )
	pRender->DrawPath(&InkPath);
}




/********************************************************************************************

>	void NodeSimpleShape::RenderObjectBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		pRender - the region to render the blobs to
	Purpose:	Renders the Object blobs for a Node Shape
	SeeAlso:	BlobManager

********************************************************************************************/

void NodeSimpleShape::RenderObjectBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Set the line colours etc as we need them
	pRender->SetLineColour(COLOUR_NONE);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// Render the blobs on the path
	pRender->DrawBlob(Parallel[0], BT_UNSELECTED);
	pRender->DrawBlob(Parallel[1], BT_UNSELECTED);
	pRender->DrawBlob(Parallel[2], BT_UNSELECTED);
	pRender->DrawBlob(Parallel[3], BT_UNSELECTED);
#endif
}



/********************************************************************************************

>	void NodeSimpleShape::RenderTinyBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		pRender - the region to render the blobs to
	Purpose:	Renders the Tiny blobs for a Node Shape
	SeeAlso:	BlobManager

********************************************************************************************/

void NodeSimpleShape::RenderTinyBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Set the line colours etc as we need them
	pRender->SetLineColour(COLOUR_NONE);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// Find out about the path that the shape is made from
	DocCoord* Coords = InkPath.GetCoordArray();

	// Render the blobs on the path
	if (InkPath.GetNumCoords()>0)
		pRender->DrawBlob(Coords[0], BT_UNSELECTED);
#endif
}



/********************************************************************************************

>	DocRect NodeSimpleShape::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/02/94
	Inputs:		DontUseAttrs - TRUE if we should ignore the nodes attributes.
				Defaults to FALSE
				HitTest      - TRUE if being called during HitTest
	Returns:	The nodes bounding rect
	Purpose:	if the bounding rect is valid it is returned, if not, it is recalculated
				and then returned.
	SeeAlso:	NodeSimpleShape::GetBlobBoundingRect

********************************************************************************************/

DocRect NodeSimpleShape::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	if (!IsBoundingRectValid || DontUseAttrs)
	{
		// Something to put the new bounding rectangle in
		DocRect NewBoundingRect;

		// Find out what the paths bounding rect is now
		if (!CalculatePathBoundingRect(InkPath, DontUseAttrs, &NewBoundingRect))
		{
			// GDraw failed to find out how big the bounding rect
			// we will have to make do with the bounding rect of the coords
			NewBoundingRect = InkPath.GetBoundingRect();
		}

		// we have a new bounding rect - decide what to do with it
		if (DontUseAttrs)
		{
			// but it is not the real bounding rect, so just return it
			return NewBoundingRect;
		}

		// Update the Nodes bounding rectangle
		BoundingRectangle = NewBoundingRect;

		// Mark the rect as valid
		IsBoundingRectValid = TRUE;
	}

	// return the current state of the bounding rect
	return BoundingRectangle;
}


/********************************************************************************************

>	DocRect NodeSimpleShape::GetBlobBoundingRect()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/02/94
	Returns:	DocRect - Returns the bounding rect of the path and its blobs
	Purpose:	This calculates the bounding box of the path and adds in the influence of
				the selection blobs. It does not consider if the blobs are visible or not,
				it just gives the bounding box that they would occupy if they were visible

********************************************************************************************/

DocRect NodeSimpleShape::GetBlobBoundingRect()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the Shapes bounding rectangle
	DocRect Rect = GetBoundingRect();

	// Find the blob manager
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();

	// And if we can find the current view, add on the size of a selection blob
	if (pBlobMgr!= NULL)
	{
		// Wee need to add in each of the blobs. there is a blob on each corner
		// of the parallelogram
		DocRect BlobSize;
		pBlobMgr->GetBlobRect(Parallel[0], &BlobSize);
		Rect = Rect.Union(BlobSize);
		
		// Next corner of the parallelogram
		pBlobMgr->GetBlobRect(Parallel[1], &BlobSize);
		Rect = Rect.Union(BlobSize);

		// and the next
		pBlobMgr->GetBlobRect(Parallel[2], &BlobSize);
		Rect = Rect.Union(BlobSize);

		// and the last one
		pBlobMgr->GetBlobRect(Parallel[3], &BlobSize);
		Rect = Rect.Union(BlobSize);
	}

	// Make sure we include the Bounds of our children
	IncludeChildrensBoundingRects(&Rect);

	// return the rectangle with the blobs included
	return Rect;
#else
	return DocRect(0,0,0,0);
#endif
}




/********************************************************************************************

>	virtual UINT32 NodeSimpleShape::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Returns:	The size of the node in bytes 
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodeSimpleShape::GetNodeSize() const 
{     
	return (sizeof(NodeSimpleShape)); 
}  



/********************************************************************************************

>	BOOL NodeSimpleShape::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/94
	Inputs:		PointerPos - The Location of the mouse pointer at the time of the click
				Click - The type of click received (single, double, drag etc)
				ClickMods - The modifiers to the click (eg shift, control etc )
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the Node to respond to clicks by selecting its blobs or starting
				drags etc.
				This functions should be overridden in the all the NodeRenderableInk classes
				so that this version never gets called. Eg the NodePath class might claim
				the click if it happened over one of its unselected blobs.

********************************************************************************************/

BOOL NodeSimpleShape::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	PORTNOTETRACE("other","NodeSimpleShape::OnClick - do nothing");

#if !defined(EXCLUDE_FROM_RALPH)
	// we only handle the click if we can confirm that object blobs are being displayed.
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr == NULL)
		return FALSE;
	if (!pBlobMgr->GetCurrentInterest().Object)
		return FALSE;

	INT32 ClickCorner;

	if (IsNearControlHandle(PointerPos, &ClickCorner))
	{
		// The click was over a control point, so start the drag and tell it the opposite corner
		if (Click==CLICKTYPE_DRAG)
			HandleBlobDrag(Parallel[ClickCorner], pSpread, (ClickCorner+2)%4 ); 
			
		// we have used that click up, so tell the world
		return TRUE;
	}
#endif
	// did not use the click
	return FALSE;
}



/********************************************************************************************

>	BOOL NodeSimpleShape::IsNearControlHandle(DocCoord Coord, INT32* CoordNum)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/94
	Inputs:		Coord - The Coordinate that we want to test
	Outputs:	CoordNum - The number of the control point that the coord was close to.
				This parameter is only changed it the function returns TRUE.
	Returns:	TRUE if the Coord was close to one of the control points, FALSE otherwise
	Purpose:	This function tests to see if the Coord passed in is close to any of the 
				control points in the Ellipse. If it is, the number of the control point
				is set in CoordNum and TRUE is returned. If not, the value of CoordNum
				is left unchanged and FALSE is returned.

********************************************************************************************/

BOOL NodeSimpleShape::IsNearControlHandle(DocCoord Coord, INT32* CoordNum)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the blob manager
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr==NULL)
		return FALSE;

	// Find out about the size of a blob
	DocRect BlobRect;
	pBlobMgr->GetBlobRect(Coord, &BlobRect);

	// Check to see if it is near any of the blobs
	for (INT32 i=0; i<4; i++)
	{
		// Check for collision with the control points
		if (BlobRect.ContainsCoord(Parallel[i]))
		{
			// Tell them which control point it waas over
			*CoordNum = i;

			// we have used that click up, so tell the world
			return TRUE;
		}
	}
#endif	
	// was not over any of the control points
	return FALSE;
}


/********************************************************************************************

>	void NodeSimpleShape::HandleBlobDrag(DocCoord& PointerPos, Spread* pSpread, INT32 FixedCorner)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/94
	Inputs:		PointerPos - The position the mouse pointer clicked
				pSpread - the spread that the click was on
				FixedCorner - The corner of the parallelogram that will stay fixed during
				the drag
	Purpose:	This is the base class version of this function, which does nothing. All the
				work should be done in the derived classes. These should do as follows.
				When a drag is started on one of the paths blobs this is called. It tries to
				fire up an operation that will perform a drag of all the selected points, 
				including all the EORing of the relavent parts of the curve. If it fails it
				will inform the user and not bother.

********************************************************************************************/

void NodeSimpleShape::HandleBlobDrag(DocCoord& PointerPos, Spread* pSpread, INT32 FixedCorner)
{
	ENSURE( FALSE, "NodeSimpleShape::HandleBlobDrag() called. Derived class should have been used");
}





/********************************************************************************************

>	virtual BOOL NodeSimpleShape::CanBecomeA(BecomeA* pBecomeA)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/94
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

BOOL NodeSimpleShape::CanBecomeA(BecomeA* pBecomeA)
{
	// The NodeSimpleShape can become a NodePath
	if (pBecomeA->BAPath())
	{
		pBecomeA->AddCount(1);

		return TRUE;
	}

	return FALSE;
}

		   
/********************************************************************************************

>	virtual BOOL NodeSimpleShape::DoBecomeA(CCRuntimeClass* InkClass, UndoableOperation* pOp) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/94
	Inputs:		pBecomeA = 	ptr to a class that contains all the info needed to become a new
							type of node.
	Outputs:	-
	Returns:	TRUE if the object has been transformed, FALSE if we run out of memory

	Purpose:	Transforms the object into another type of object. 
				Note: changed 7/10/94 by MarkN to take the pBecomeA param, so that more data could be passed
				to these functions in the future without causing massive rebuilds due to the editing of
				node.h
	SeeAlso:	NodeSimpleShape::CanBecomeA

********************************************************************************************/

BOOL NodeSimpleShape::DoBecomeA(BecomeA* pBecomeA)
{
	// Check for a NULL entry param
	ERROR2IF_PF(pBecomeA == NULL,FALSE,("pBecomeA is NULL"));

	// This lump checks that the Reason is one that we understand
	// It also makes sure that we don't have a NULL UndoOp ptr
	BOOL ValidReason = (pBecomeA->GetReason() == BECOMEA_REPLACE || pBecomeA->GetReason() == BECOMEA_PASSBACK);
	ERROR2IF_PF(!ValidReason,FALSE,("Unkown BecomeA reason %d",pBecomeA->GetReason()));

	// pBecomeA->Reason is one that we understand.

	BOOL 		Success = TRUE;			// Our success flag (Important that this defaults to TRUE)
	NodePath* 	pNewNodePath = NULL;	// Ptr to a new NodePath, if we get to make one.

	if (pBecomeA->BAPath())
	{
		// We need to create a new NodePath, no matter what the reason.
		
		// Allocate a new NodePath node
		ALLOC_WITH_FAIL(pNewNodePath, (new NodePath), pBecomeA->GetUndoOp()); 
		Success = (pNewNodePath != NULL);

		// Initialise the path
		if (Success) CALL_WITH_FAIL(pNewNodePath->InkPath.Initialise(InkPath.GetNumCoords(),12), pBecomeA->GetUndoOp(), Success);
		if (Success) CALL_WITH_FAIL(pNewNodePath->InkPath.CopyPathDataFrom(&InkPath), pBecomeA->GetUndoOp(), Success);

		// If Success is TRUE, then we now have a new NodePath object that contains this shape's path

		if (Success)
		{
		 	switch (pBecomeA->GetReason())
			{
		 		case BECOMEA_REPLACE :
				{
					// It's a BECOMEA_REPLACE, so replace this node with the new NodePath in an undoable way

					// Can't do it in an undoable way without an Undo Op
					ERROR2IF_PF(pBecomeA->GetUndoOp() == NULL,FALSE,("GetUndoOp() returned NULL"));

					// Firstly, hide this node
					NodeHidden* pNodeHidden; 
					Success = pBecomeA->GetUndoOp()->DoHideNode(this, TRUE, &pNodeHidden);

					if (Success)
					{
						// Insert the new NodePath into the tree, next to the hidden node
						pNewNodePath->AttachNode(pNodeHidden,NEXT);

						// Copy the node's attributes
						CALL_WITH_FAIL(CopyChildrenTo(pNewNodePath), pBecomeA->GetUndoOp(), Success); 

						if (Success)
						{
							// Set the bounds  
							pNewNodePath->InvalidateBoundingRect();
							pNewNodePath->SetSelected(IsSelected());

							// Create a hide node action to hide the node when we undo 
							HideNodeAction* UndoHideNodeAction;     
							Success = (HideNodeAction::Init(pBecomeA->GetUndoOp(),
											  		 pBecomeA->GetUndoOp()->GetUndoActionList(),
									 				 pNewNodePath, 
									 				 TRUE, 		 // Include subtree size 
							  		 				 ( Action**)(&UndoHideNodeAction))
							  		  				 != AC_FAIL);
						}
					}

					if (Success)
						pBecomeA->PassBack(pNewNodePath, this);
				}
				break;

				case BECOMEA_PASSBACK :
					Success = pBecomeA->PassBack(pNewNodePath,this);
				break;

				default:
					break;
			}
		}
	}

	if (!Success)
	{
		if (pNewNodePath != NULL)
		{
			// Delete all the NodePath's children (if it has any) and unlink it from the tree (if it's linked)
			// This is all done by CascadeDelete()
			pNewNodePath->CascadeDelete(); 
			delete pNewNodePath;
			pNewNodePath = NULL;
		}
	}

	return Success;
}



/***********************************************************************************************

> BOOL NodeSimpleShape::Snap(DocCoord* pDocCoord)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    20/9/94
	Inputs:     pDocCoord	= a coord in Spread coords
    Outputs:    
    Returns:    TRUE 	- the DocCoord has been snapped to the path.
    			FALSE	- the DocCoord has not been processed.
    			                                  		 
    Purpose:    Snaps to given coord to the nearest point on the path.  If it is not appropriate to snap
				the coord to the path (at the moment this means the coord is too far awawy), then FALSE is returned.
    Errors:        
	Scope:      public
	       
**********************************************************************************************/

BOOL NodeSimpleShape::Snap(DocCoord* pDocCoord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	BOOL Snapped = FALSE;

	MILLIPOINT SnapDist    = CSnap::GetSnapDist();
	MILLIPOINT SqrSnapDist = SnapDist*SnapDist;

	INT32 NearEl;
	double mu;
	double SqrDist = InkPath.SqrDistanceToPoint(*pDocCoord,&NearEl,&mu);

	if (SqrDist <= SqrSnapDist)
	{
		*pDocCoord = InkPath.ClosestPointTo(mu,NearEl);
		Snapped = TRUE;
	}

	return (Snapped);
#else
	return FALSE;
#endif
}

/***********************************************************************************************

> BOOL NodeSimpleShape::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    21/9/94
	Inputs:     pDocCoord	- the rectangle to snap
				StartDrag	- Start coord of drag
				EndDrag		- End coord of drag
    Outputs:    
    Returns:    TRUE 	- the DocRect been snapped to the grid.
    			FALSE	- the DocRect has not been processed.
    			                                  		 
    Purpose:    Snaps the given rect to the nearest position on the grid, preserving its width
    			and height.
    			The coords of the rect used for the snapping are determined by the PrevCoord and
				CurCoord coords supplied.  This is done to allow the user to control how a
				selection rectangle is snapped to the grid by the direction of his/her last mouse 
				movement.
				To force the bottom left hand corner of the rect to be snapped, 
				supply PrevCoord=(0,0) and CurCoord(-1,-1).
	Scope:      public
	       
**********************************************************************************************/

BOOL NodeSimpleShape::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	TRACEUSER( "MarkN", _T("NodeSimpleShape::Snap(DocRect)\n") );
#endif
	return FALSE;
}



/********************************************************************************************

>	BOOL NodeSimpleShape::SnapToCoords(DocCoord* pDocCoord)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		pDocCoord - The Coord to try and snap
	Outputs:	pDocCoord - The snapped verison of the coord
	Returns:	TRUE if the input coord was snapped, FALSE if not
	Purpose:	This function tries to magnetically snap the supplied coord to the shape.
				This version of the function tries to snap to coords at the centre of the
				shape and in the middle of each of the edges.
	SeeAlso:	NodeRenderableBounded::SnapToCoords

********************************************************************************************/

BOOL NodeSimpleShape::SnapToCoords(DocCoord* pDocCoord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Work out the coords to snap to
	DocCoord TestPoint;

	// Snap to the Centre of the Shape
	TestPoint.x = (Parallel[0].x + Parallel[2].x) / 2;
	TestPoint.y = (Parallel[0].y + Parallel[2].y) / 2;
	
	// Test to see if the points are close together, and update pDocCoord if they are
	if (IsMagneticallyClose(&TestPoint, pDocCoord))
		return TRUE;

	// Try and snap to the middle of each of the edges
	for (INT32 i=0; i<4; i++)
	{
		// Find test coord
		INT32 NextPoint = (i+1) % 4;
		TestPoint.x = (Parallel[i].x + Parallel[NextPoint].x) / 2;
		TestPoint.y = (Parallel[i].y + Parallel[NextPoint].y) / 2;
		
		// Test to see if the points are close together, and update pDocCoord if they are
		if (IsMagneticallyClose(&TestPoint, pDocCoord))
			return TRUE;
	}
#endif
	// Did not snap to anything.
	return FALSE;
}



/********************************************************************************************

>	double NodeSimpleShape::GetRotationAngle()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Outputs:	-
	Returns:	The rotation angle of this simple shape  NodeRenderableInk
	Purpose:	Returns the current angle of rotation of the simple shape.  This is calculated
				as the angle from the midpoint of the upper paralleogram line to the positive
				x-axis.
	SeeAlso:	NodeRenderableBounded::GetRotationAngle

********************************************************************************************/

double NodeSimpleShape::GetRotationAngle()
{
	DocCoord MidPoint((Parallel[0].x + Parallel[1].x)/2, (Parallel[0].y + Parallel[1].y)/2);

	DocRect Bounds(Parallel[0],Parallel[0]);
	Bounds.IncludePoint(Parallel[1]);
	Bounds.IncludePoint(Parallel[2]);
	Bounds.IncludePoint(Parallel[3]);
	DocCoord Centre((Bounds.LowCorner().x+Bounds.HighCorner().x)/2,
											(Bounds.LowCorner().y+Bounds.HighCorner().y)/2);

	DocCoord Offset = MidPoint - Centre;

	double Angle = atan2((double)Offset.y, (double)Offset.x);
	if (Angle == HUGE_VAL)
		Angle = 0.0;

	return Angle;
}



/********************************************************************************************

>	DocRect NodeSimpleShape::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/1999
	Inputs:		ExtParams		parameters describing the extension.
	Outputs:	
	Returns:	TRUE if this simple shape can be validly extended,
				FALSE otherwise.
	Purpose:	Tests to see whether this shape's centre-point is positioned so as to make
				an extend operation irreversible.
	Errors:		
	See also:	

********************************************************************************************/
DocRect NodeSimpleShape::ValidateExtend(const ExtendParams& ExtParams)
{
	DocCoord doccArray[1] = { FindExtendCentre() };
	DocRect drMinExtend = Extender::ValidateControlPoints(1, doccArray, ExtParams);

	// if we didn't invalidate the extension, we must call the base class
	// implementation, which will validate our children.
	if (drMinExtend.lo.x == INT32_MAX &&
		drMinExtend.lo.y == INT32_MAX &&
		drMinExtend.hi.x == INT32_MAX &&
		drMinExtend.hi.y == INT32_MAX)
		drMinExtend = Node::ValidateExtend(ExtParams);

	return drMinExtend;
}



/********************************************************************************************

>	void NodeSimpleShape::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/1999
	Inputs:		ExtParams		parameters describing the extension.
	Outputs:	this NodeSimpleShape will be extended in accordance with ExtParams.
	Returns:	
	Purpose:	Perform an Extend operation on this shape. Behaviour is as follows:

				* the shape extends separately along each axis.
				* if the shape is asked to stretch, it scales along the corresponding axes.
				* if the shape is asked to extend, it is translated as a whole, as described
																				by ExtParams.
	Errors:		
	See also:	class Extender

********************************************************************************************/
void NodeSimpleShape::Extend(const ExtendParams& ExtParams)
{
	// do the extension operations on ourself.
	TransformStretchObject(ExtParams);
	TransformTranslateObject(ExtParams);

	// do the base-class implementation to extend our children.
	Node::Extend(ExtParams);
}
