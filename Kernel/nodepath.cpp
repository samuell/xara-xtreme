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
// The Renderable Path Code

/*
*/

#include "camtypes.h"
#include "nodepath.h"

#include <math.h>
#include "docview.h"

#include "becomea.h"
#include "blobs.h"
#include "contmenu.h"
#include "lineattr.h"
#include "ophist.h"
#include "opbreak.h"
#include "pathedit.h"
#include "snap.h"
#include "pathops.h"
#include "nodetxts.h"
#include "saveeps.h"

#include "camfiltr.h"
#include "cxftags.h"
#include "cxfile.h"
#include "cxfrec.h"
#include "rechpath.h"

//#include "simon.h"
//#include "rik.h"
//#include "jim.h"
//#include "tim.h"

#include "gclips.h"
//#include "mario.h"

#include "extender.h"
#include "ngcore.h"		// NameGallery, for stretching functionality

#include "gdraw.h"
//#include "rndrgn.h"
#include "brshattr.h"
#include "qualattr.h"

//#include "blendatt.h"
#include "nodebldr.h"
//#include "ndbldpth.h"
#include "fixmem.h"
#include "gblend.h"
#include "blndhelp.h"
#include "opdrbrsh.h"
#include "rsmooth.h"	//For retrosmoothing
#include "strkattr.h"	//For attrstroketype
#include "ppstroke.h"
#include "nodeclip.h"  // for pathbecomea
#include "attrmap.h"

// Put my version Number into the About box
DECLARE_SOURCE( "$Revision$" );

CC_IMPLEMENT_DYNCREATE( NodePath, NodeRenderableInk )

CC_IMPLEMENT_DYNAMIC(PathRecordHandler,CamelotRecordHandler)
CC_IMPLEMENT_DYNAMIC(PathFlagsRecordHandler,CamelotRecordHandler)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW
#define CONVERTSTROKES // comment this out to disable converting strokes for bevelling/contouring

/***********************************************************************************************

>   NodePath::NodePath(
	Node* ContextNode,  
	AttachNodeDirection Direction,  
	const DocRect& BoundingRect, 
	BOOL Locked = FALSE, 
	BOOL Mangled = FALSE,  
	BOOL Marked = FALSE, 
	BOOL Selected = FALSE, 
	BOOL Renderable = FALSE
	)

    Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
    Created:	26/4/93             
    
    Inputs:		ContextNode: Pointer to a node which this node is to be attached to.     
		
				Direction: 
			
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
			
	Note:		SetUpPath() must be called before the NodePath is in a state in which it can be 
				used. 		
                
    SeeAlso:	NodePath::SetUpPath()
    
    Errors:		An assertion error will occur if ContextNode is NULL

***********************************************************************************************/

NodePath::NodePath(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ):NodeRenderableInk(ContextNode, Direction, Locked, Mangled, Marked, Selected )  
{
	CurrentRotation = 0.0;
}                        
 
/*********************************************************************************************

>    NodePath::NodePath() 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   04/6/93
     Inputs:    -
     Outputs:   - 
     Returns:   -
			  
     Purpose:	This constructor creates a NodePath linked to no other with all status
     			flags false and an uninitialized bounding rectangle.           
			
     Note:    SetUpPath() must be called before the NodePath is in a state in which it can be 
			  used. 		
                
     SeeAlso: SetUpPath                                                        

**********************************************************************************************/
/* Technical Notes:   

The default constructor is required so that SimpleCopy will work 
*/  

NodePath::NodePath(): NodeRenderableInk()
{
	CurrentRotation = 0.0;
}



/********************************************************************************************

>	virtual BOOL NodePath::IsNodePath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Returns:	TRUE => This node is an instance of NodePath.
				FALSE => otherwise.
	Purpose:	Override the node virtual function IsNodePath() and return TRUE as we
				definitely are one.

********************************************************************************************/

BOOL NodePath::IsNodePath() const
{
	return TRUE;
}



/***********************************************************************************************

>	BOOL NodePath::SetUpPath( INT32 RequiredSize = 32, INT32 BlockSize = 32 )

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/5/93
	Inputs:		RequiredSize - The size of the initial block of memory to be allocated to the
				path.
				BlockSize - The size of the block asked for by the path object when it
				runs out of space. 
	Returns:	TRUE if the path was init'ed ok, FALSE otherwise
	Purpose:	To initialise the Nodepath into a state that can be used, by allocating memory,
				setting up member variables properly and inserting an EndPath element into
				the path

***********************************************************************************************/


BOOL NodePath::SetUpPath(INT32 RequiredSize, INT32 BlockSize)
{
	CurrentRotation = 0.0;
	return (InkPath.Initialise(RequiredSize, BlockSize)); 
}


/***********************************************************************************************

>	void NodePath::ShowDebugTreeDetails() const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Purpose:	Displays debugging info of the tree
	SeeAlso:	NodeRenderableInk::ShowDebugTreeDetails

***********************************************************************************************/
#ifdef _DEBUG
void NodePath::ShowDebugTreeDetails() const
{                     
	TRACE( _T("Path ")); 
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeRenderableInk::ShowDebugTreeDetails();	
	
	//InkPath.DumpWholePath();
}
#endif

/********************************************************************************************

>	void* NodeRenderable::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/93
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node

********************************************************************************************/

void NodePath::GetDebugDetails( StringBase* Str )
{
#ifdef _DEBUG
	// Call base class
	NodeRenderableInk::GetDebugDetails( Str );
	
	InkPath.FindStartOfPath();
	String_256 TempStr;
		
	(*Str) += TEXT( "\r\nPath Data Dump\r\n" );
	TempStr._MakeMsg( TEXT( "#1%ld bytes used\r\n"), InkPath.GetPathByteLength() );
	(*Str) += TempStr;

#if !defined(EXCLUDE_FROM_RALPH)
	DocRect BlobRect = GetBlobBoundingRect();
	TempStr._MakeMsg( TEXT("Blob Bounding Rect :-\r\n\t#1%ld,\t#2%ld\r\n\t#3%ld,\t#4%ld\r\n"),
					  BlobRect.lo.x, BlobRect.lo.y, BlobRect.hi.x, BlobRect.hi.y );
	(*Str) += TempStr;
#endif

	TempStr._MakeMsg(TEXT("The Path is #1%sFilled and #2%sStroked\r\n"), InkPath.IsFilled ? "" : "not ", InkPath.IsStroked ? "" : "not ");
	(*Str) += TempStr;

	(*Str) += TEXT( "\r\nNum\tType\tX Coord\tY Coord\r\n" );
	PathVerb* Verbs  = InkPath.GetVerbArray();
	DocCoord* Coords = InkPath.GetCoordArray();
	PathFlags* Flags = InkPath.GetFlagArray();
	INT32 numcoords = InkPath.GetNumCoords();
	for (INT32 i=0; i<numcoords; i++)
	{
		// Add the info to the string
		TempStr._MakeMsg( TEXT("#1%d.\t#2%d\t#3%ld,\t#4%ld\t"),
						  i, Verbs[i], Coords[i].x, Coords[i].y );
		
		// Add useful flags to the end of the string
		if (Flags[i].IsSmooth)
			TempStr += TEXT("S");

		if (Flags[i].IsRotate)
			TempStr += TEXT("R");

		if (Flags[i].IsEndPoint)
			TempStr += TEXT("E");

		if (Flags[i].IsSelected)
			TempStr += TEXT("(sel)");

		TempStr += TEXT("\r\n");
	
		(*Str) += TempStr;

	}
#endif
}


/********************************************************************************************

>	virtual UINT32 NodePath::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the path node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodePath::GetNodeSize() const 
{     
	return (sizeof(NodePath)+InkPath.GetPathByteLength());  
}  



/***********************************************************************************************

>	virtual Node* NodePath::SimpleCopy()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Returns:	Pointer to a Node
	Purpose:	Makes a copy of all the data in the node

***********************************************************************************************/

Node* NodePath::SimpleCopy()
{

	NodePath* NodeCopy = new NodePath();
    if (NodeCopy)
    	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}            


/***********************************************************************************************

>	void NodePath::CopyNodeContents( Node* NodeCopy)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Inputs:		NodeCopy - The node to copy
	Purpose:	Copies the data in the node by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeRenderableInk::CopyNodeContents

***********************************************************************************************/

void NodePath::CopyNodeContents( NodePath* NodeCopy)
{
	NodeRenderableInk::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here
	if (NodeCopy->InkPath.Initialise(InkPath.GetNumCoords(),12))
		NodeCopy->InkPath.CopyPathDataFrom(&InkPath);
}




/***********************************************************************************************
>   void NodePath::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodePath::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodePath), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodePath))
		CopyNodeContents((NodePath*)pNodeCopy);
}



/***********************************************************************************************

>	virtual void NodePath::Render(RenderRegion* pRRegion)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/93
	Inputs:		Pointer to a render region
	Purpose:	Will render the path contained within the object to the given render region

***********************************************************************************************/

void NodePath::Render(RenderRegion* pRender)
{
	// render the path.
	TRACEUSER( "Diccon", _T("Rendering nodepath\n"));
	pRender->DrawPath(&InkPath);
}



/********************************************************************************************

>	void NodePath::RenderEorDrag( RenderRegion* pRender )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/93
	Inputs:		pRender - A Pointer to the current RenderRegion
	Purpose:	Renders a version of the path for EORed dragging of shapes.
	SeeAlso:	NodePath::Render; NodeRenderableInk::RenderEorDrag

********************************************************************************************/

void NodePath::RenderEorDrag( RenderRegion* pRender )
{
	// Currently this function performs an identical task to the
	// Render() function
	
	// render the path
	pRender -> DrawPath( &InkPath );
}



/********************************************************************************************

>	void NodePath::RenderObjectBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRender - The region to render the blobs into
	Purpose:	Draws the paths object blobs into the render region supplied

********************************************************************************************/

void NodePath::RenderObjectBlobs(RenderRegion* pRender)
{
	// Find the parent spread
	Spread* pSpread = FindParentSpread();
	ENSURE(pSpread != NULL, "NodePath does not have a parent spread");
	
	// Render the blobs on the path
	if (pSpread!=NULL)
		InkPath.RenderPathBlobs(pRender);

	// New Diccon 13/6/2000 If we are a brush and we are selected then we want to 
	// render the path on top
	NodeAttribute* pAttr = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), &pAttr);
	if (pAttr!= NULL)
	{
		if (((AttrBrushType*)pAttr)->GetBrushHandle() != BrushHandle_NoBrush)
		{

			// --- If the quality is set low enough, strokes are just rendered as centrelines
			// "low enough" is defined as the same point that Blends show their middle parts
			// BLOCK
			{
				View* pRenderView = NULL;
				pRenderView = pRender->GetRenderView();
				if(pRenderView != NULL)
				{
					Quality viewQuality = pRenderView->RenderQuality;
					if (viewQuality.GetBlendQuality() == Quality::FullBlend)
					{
						pRender->SetLineColour(COLOUR_BEZIERLINE);
						pRender->SetFillColour(COLOUR_NONE);
						pRender->DrawPath(&InkPath);
					}
				}
			}
		}
	}
}

/********************************************************************************************

>	void NodePath::RenderPenBlobs(RenderRegion* pRender)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRender - The region to render the blobs into
	Purpose:	Draws the paths pen blobs into the render region supplied

********************************************************************************************/

void NodePath::RenderPenBlobs(RenderRegion* pRender)
{
	// Find the parent spread
	Spread* pSpread = FindParentSpread();
	ENSURE(pSpread != NULL, "NodePath does not have a parent spread");
	
	// Render the blobs on the path
	if (pSpread!=NULL)
		InkPath.RenderPathPenBlobs(pSpread);
}


/********************************************************************************************

>	void NodePath::RenderTinyBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		pRender - The region to render the blobs into
	Purpose:	Draws the paths Tiny blob into the render region supplied

********************************************************************************************/

void NodePath::RenderTinyBlobs(RenderRegion* pRender)
{
	// Set the line colours etc as we need them
	pRender->SetLineColour(COLOUR_NONE);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// Render the blobs on the path
	DocCoord* Coords = InkPath.GetCoordArray();
	pRender->DrawBlob(Coords[0], BT_UNSELECTED);
}



/********************************************************************************************

>	virtual ChangeCode NodePath::OnChildChange(ObjChangeParam* pParam)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Inputs:		pParam	= pointer to a object change parameter class
	Returns:	CC_OK	
				
	Purpose:	It is now necessary to know when the nodepath has been updated because
				if it has a brush applied to it the brush needs to clear its cache

********************************************************************************************/

ChangeCode NodePath::OnChildChange(ObjChangeParam* pParam)
{
	// look for an applied brush attribute
	NodeAttribute* pAttr = NULL;
	AttrBrushType* pAttrBrush = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), &pAttr);
	if (pAttr!= NULL)
	{
		pAttrBrush = (AttrBrushType*)pAttr;
		
		// make an undoable action
		UpdateBrushAction* pAction;
		UndoableOperation* pOp = pParam->GetOpPointer();
		if (pOp != NULL)
			UpdateBrushAction::Init(pOp, pOp->GetUndoActionList(), this, &pAction);
	}
	return CC_OK;
}

/********************************************************************************************

>	void NodePath::ClearSubSelection(Spread* pSpread)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/94
	Inputs:		pSpread - Pointer to the nodes parent spread
	Purpose:	Clears the paths sub-selection and re-draws the blobs to show this

********************************************************************************************/

void NodePath::ClearSubSelection(Spread* pSpread)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ENSURE( pSpread!=NULL, "Parent Spread was NULL in NodePath::ClearSubSelection" );

	if ((pSpread!=NULL) && (InkPath.IsSubSelection()))
	{
		InkPath.RenderPathBlobs(pSpread);
		InkPath.ClearSubSelection();
		InkPath.RenderPathBlobs(pSpread);
	}
#endif
}



/********************************************************************************************

>	virtual BOOL NodePath::OnClick( DocCoord PointerPos, ClickType Click, 
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
				This functions should be overridden in the all the NodeRenderableInk classes
				so that this verion never gets called. Eg the NodePath class might claim
				the click if it happened over one of its unselected blobs.

********************************************************************************************/

BOOL NodePath::OnClick( DocCoord PointerPos, ClickType Click, 
						ClickModifiers ClickMods, Spread *pSpread )
{
#ifndef STANDALONE

	DocRect    BlobRect;
	DocCoord*  Coords = InkPath.GetCoordArray();
	PathFlags* Flags  = InkPath.GetFlagArray();
//	PathVerb*  Verbs  = InkPath.GetVerbArray();

	// Should always be able to get selected view
	DocView *pDocView = DocView::GetSelected();
	ERROR3IF(pDocView == NULL, "NodePath::OnClick: Could not get selected DocView");
	if (pDocView == NULL)
		return FALSE;

	// we only handle the click if we can confirm that object blobs are being displayed.
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr == NULL)
		return FALSE;
	if (!pBlobMgr->GetCurrentInterest().Object)
		return FALSE;

	INT32 NumCoords = InkPath.GetNumCoords();
	INT32 i;
	if (InkPath.FindNearestPoint(	PointerPos, 
									POINTFLAG_ENDPOINTS | 
									POINTFLAG_CONTROLPOINTS | 
									POINTFLAG_ENDSFIRST,
									&i)
		)
	{
		// we had a hit on a blob, so do something with it
		if ( Click == CLICKTYPE_SINGLE )
		{
			// Clicks on control points have no effect
			// but clicks on endpoints do have an effect
			if (Flags[i].IsEndPoint)
			{
				if (ClickMods.Adjust && ClickMods.Constrain)
				{
					HandleBlobClick(Coords, Flags, i, NumCoords, TRUE, TRUE);
				}
				else
				{
					if ((!Flags[i].IsSelected) || ((Flags[i].IsSelected) && (ClickMods.Adjust)))						   
					{
						HandleBlobClick(Coords, Flags, i, NumCoords, ClickMods.Adjust, FALSE);
					}											   
					else
					{
						// In order to deselect all but the clicked on point and still have dragging of
						// multiple points we need to start the drag op on the click and handle a click
						// at the end of the drag
						if (Flags[i].IsSelected && !ClickMods.Menu)
						{
							OpNodePathEditBlob* pOpNodePath = new OpNodePathEditBlob;
							if (pOpNodePath == NULL)
								InformError();
							else
								pOpNodePath->DoStartDragEdit(this, Coords[i], pSpread);
						}
					}
				}
			}
			else
			{
				// Detected single click on Control Point
			}
		}

		if ( Click == CLICKTYPE_DOUBLE)
		{
			// A point has been double-clicked. Only if it's selected and an endpoint can we
			// do anything about it. What we do is the toggle smooth operation

			if (Flags[i].IsSelected && Flags[i].IsEndPoint)
			{
				// Try and create the toggle operation
				OpToggleSmooth* pOpToggle = new OpToggleSmooth;
				if (!pOpToggle)
				{
					// Inform the user that we are out of memory
					InformError();
				}
				else
				{
					// Call the function that actually does something
					pOpToggle->DoTogglePoint(this, i, pSpread, FALSE, !ClickMods.Constrain);	// invert constrain
				}
			}
		}

		// Check for drags, but only when the click isn't due to a menu click.
		if ( Click == CLICKTYPE_DRAG && !ClickMods.Menu )
		{
			
			//if (Flags[i].IsSelected)
			{
				if (Flags[i].IsEndPoint)
				{
					// Need to do a drag on the selected points,
					// so we had better start an operation
					OpNodePathEditBlob* pOpNodePath = new OpNodePathEditBlob;
					if (pOpNodePath == NULL)
					{
						// Inform the user that we are out of memory
						InformError();
					}
					else
					{
						// Start the drag operation and pass in the Anchor Point to the operation

						// The anchor point MUST be the true coord of the path point for
						// snapping to work correctly (Markn 30/9/94)

						pOpNodePath->DoStartDragEdit(this, Coords[i], pSpread);
					}
				}
				else
				{
					// This must be a drag on a control point, so start an operation to
					// handle that situation
					OpNodePathEditControlBlob* pOpNodePath = new OpNodePathEditControlBlob;
					if (pOpNodePath==NULL)
					{
						// Failed to get the mem I needed
						InformError();
					}
					else
					{
						// Start the drag operation and pass in the Anchor Point to the operation

						// The anchor point MUST be the true coord of the path point for
						// snapping to work correctly (Markn 30/9/94)

						pOpNodePath->DoStartDragEdit(this, Coords[i], pSpread, i);
					}
				}
			}
		}					

		return TRUE;
	}

#endif
	// we do not want to claim the click, so return FALSE
	return FALSE;
}



/********************************************************************************************

>	virtual BOOL NodePath::OnBlobPopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/04/95
	Inputs:		pSpread		The spread in which things are happening
				PointerPos	The Location of the mouse pointer at the time of the click
				pMenu		The menu to which items should be added
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the Node to respond to pop up menu clicks on blobs.

********************************************************************************************/

BOOL NodePath::OnBlobPopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect    BlobRect;
//	DocCoord*  Coords = InkPath.GetCoordArray();
//	PathFlags* Flags  = InkPath.GetFlagArray();
//	PathVerb*  Verbs  = InkPath.GetVerbArray();

	// Should always be able to get selected view
	DocView *pDocView = DocView::GetSelected();
	ERROR3IF(pDocView == NULL, "NodePath::OnClick: Could not get selected DocView");
	if (pDocView == NULL)
		return FALSE;

	// If there are no selected points we won't put up our menu because its
	// items are entirely to do with points (a useful thing in a Blobs menu!).
	if (!InkPath.IsSubSelection())
		return FALSE;

//	INT32 NumCoords = InkPath.GetNumCoords();
	INT32 i;
	if (InkPath.FindNearestPoint(	PointerPos, 
									POINTFLAG_ENDPOINTS | 
									POINTFLAG_CONTROLPOINTS | 
									POINTFLAG_ENDSFIRST,
									&i)
		)
	{
		pMenu->BuildCommand(TOOL_OPTOKEN_BEZTOOL, TRUE);

		pMenu->BuildCommand(OPTOKEN_MAKELINESOP);
		pMenu->BuildCommand(OPTOKEN_MAKECURVESOP, TRUE);

		pMenu->BuildCommand(OPTOKEN_DELETEPOINTSOP);
		pMenu->BuildCommand(OPTOKEN_BREAKATPOINTS, TRUE);

		pMenu->BuildCommand(OPTOKEN_SELECTALLPATHPOINTS);
		pMenu->BuildCommand(OPTOKEN_DESELECTALLPATHPOINTS, TRUE);

		return TRUE;
	}

#endif
	// we do not want to claim the click, so return FALSE
	return FALSE;
}



/********************************************************************************************

>	virtual BOOL NodePath::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/95
	Inputs:		pSpread		The spread in which things are happening
				PointerPos	The Location of the mouse pointer at the time of the click
				pMenu		The menu to which items should be added
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the Node to respond to pop up menu clicks on it (rather than its blobs).

********************************************************************************************/
BOOL NodePath::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)
{
#if !defined(EXCLUDE_FROM_RALPH)
	BOOL ok = TRUE;
	
	ok = ok && pMenu->BuildCommand(TOOL_OPTOKEN_BEZTOOL, TRUE);

	return ok;
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	void NodePath::HandleBlobClick(DocCoord* Coords, PathFlags* Flags, INT32 Pos, 
								   INT32 NumCoords, BOOL Adjust, BOOL Constrain)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/93
	Inputs:		Coords - The array of coordinates
				Flags - The array of flags
				Pos - The item click on
				Adjust - TRUE if this is an Adjust click
				Constrain - TRIE if this is a Constrain click.
	Outputs:	The values in the flags array are modified by this function
	Purpose:	This will handle the selection and deselection of points on a path. For curve
				segments all the Bezier control points etc will be drawn in. This routine will
				also spot closed paths and select endpoints as well
	Scope:		Private

********************************************************************************************/

void NodePath::HandleBlobClick(DocCoord* Coords, PathFlags* Flags, INT32 Pos, INT32 NumCoords,
																BOOL Adjust, BOOL Constrain)
{
#if !defined(EXCLUDE_FROM_RALPH)
	PathVerb* Verbs = InkPath.GetVerbArray();
	
	// clear the selection state of all the other selected objects in the tree (but not this one)
	if (!Adjust && !Constrain)
	{
		SelRange* Selected = GetApplication()->FindSelection();
		Node* pNode = Selected->FindFirst();
		while (pNode!=NULL)
		{
			if ((pNode!=this) && (pNode->IsKindOf(CC_RUNTIME_CLASS(NodePath))))
			{
				// This node needs to get rid of all its selected control points
				NodePath* pNodePath = (NodePath*) pNode;
				if (pNodePath->InkPath.IsSubSelection())
				{
					Spread* pSpread = pNodePath->FindParentSpread();
					ENSURE(pSpread!=NULL, "Node did not have a parent spread in NodePath::HandleBlobClick");
					if (pSpread!=NULL)
					{
						pNodePath->InkPath.RenderPathSelectedControlBlobs(pSpread);
						pNodePath->InkPath.ClearSubSelection();
						pNodePath->InkPath.RenderPathSelectedControlBlobs(pSpread);
					}
				}
			}

			// Get the next selected node
			pNode = Selected->FindNext(pNode);
		}
	}

	// Find out what the current selection state is, so that we can toggle it
	// Selection state will hold the value we want to set
	BOOL NewSelState;
	BOOL OldSelState = Flags[Pos].IsSelected;
	if (OldSelState)
		NewSelState = FALSE;
	else
		NewSelState = TRUE;

	Spread *pSpread = FindParentSpread();
	ENSURE(pSpread != NULL, "NodePath does not have a parent spread");

	// Remove the current blobs from this path before we try and change selection
	InkPath.RenderPathSelectedControlBlobs(pSpread);

	// If we're not adjusting, clear the subselection in this path
	if (!Adjust && !Constrain)
		InkPath.ClearSubSelection();

	// if we are adjusting and constraining then (de)select all the endpoints on the path
	if (Adjust && Constrain)
	{
		BOOL NewSelState = !Flags[Pos].IsSelected;
		for (INT32 loop = 0; loop < InkPath.GetNumCoords(); loop++)
		{
			Flags[loop].IsSelected = NewSelState;
		}
	}
	else
	{
		// Now change the selection of this point
		Flags[Pos].IsSelected = NewSelState;
		// If the previous point is a control point then deal with it
		if ((Pos>0) && (!Flags[Pos-1].IsEndPoint))
		{
			// Change the control point's selection state
			Flags[Pos-1].IsSelected = NewSelState;
		}

		// If the next point is a control point then deal with it
		if ((Pos+1<NumCoords) && (!Flags[Pos+1].IsEndPoint))
		{
			// Change the control point's selection state
			Flags[Pos+1].IsSelected = NewSelState;
		}
	
		// Check for this being the first element in a closed subpath
		// If this element is a moveto, and the end of the path has the
		// CLOSEFIGURE flag set, we should select the endpoint as well
		if (Verbs[Pos] == PT_MOVETO)
		{
			INT32 j;
			// This for loop will find either the end of the path, or the next moveto
			for (j=Pos+1;j<NumCoords && Verbs[j] != PT_MOVETO;j++);	// ; is intentional!
			j--;
			if (Verbs[j] & PT_CLOSEFIGURE)
			{
				//HandleBlobClick(Coords,Flags,j,NumCoords,TRUE);
				Flags[j].IsSelected = NewSelState;
				// If the previous point is a control point then deal with it
				if ((j>0) && (!Flags[j-1].IsEndPoint))
				{
					// Change the control point's selection state
					Flags[j-1].IsSelected = NewSelState;
				}
			}

		}
	}

	InkPath.RenderPathSelectedControlBlobs(pSpread);
#endif
}





/********************************************************************************************

>	virtual void NodePath::Transform( TransformBase& Trans )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/93
	Inputs:		Trans - The transform to be applied to the path
	Purpose:	Will Transform all the coords in the path with the transform 
				provided
	SeeAlso:	NodeRenderableInk::Transform()

********************************************************************************************/

void NodePath::Transform( TransformBase& Trans )
{
	// Change all the coords
	Trans.Transform( (DocCoord*)InkPath.GetCoordArray(), InkPath.GetNumCoords() );

	// and keep the bounding rectangle up to date.
	InvalidateBoundingRect();

	// Transform all the children...
	TransformChildren(Trans);

	// Also update our CurrentRotation variable

}


/********************************************************************************************

>	virtual String NodePath::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/93
	Inputs:		Plural: Flag indicating if the string description should be plural or
						singular. 
	Retuns:		Description of the object 
	Purpose:	To return a description of the NodePath object in either the singular or the 
				plural. This method is called by the DescribeRange method.
				The description will always begin with a lower case letter.   
	Errors:		A resource exception will be thrown if a problem occurs when loading the 
				string resource. 

********************************************************************************************/
              
String NodePath::Describe(BOOL Plural, BOOL Verbose) 
{     
	// if the first subpath is closed, this is a shape rather than a line
	if (InkPath.IsSubPathClosed(0))
	{
		if (Plural)
			return(String(_R(IDS_DESCRIBE_SHAPES)));  
		else
			return(String(_R(IDS_DESCRIBE_SHAPE))); 
	}
	else
	{
		if (Plural)
			return(String(_R(IDS_PATH_DESCRP)));  
		else
			return(String(_R(IDS_PATH_DESCRS))); 
	}
}; 




/********************************************************************************************

>	DocRect NodePath::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/02/94
	Inputs:		DontUseAttrs - TRUE if you don't want to use the nodes attrs to calculate
				the bounding rect (defaults will be used). Defaults to FALSE.
				HitTest      - TRUE if being called during HitTest
	Returns:	The paths bounding rectangle.
	Purpose:	returns the paths bounding rectangle and recalculates it if it is invalid
	SeeAlso:	NodePath::GetBlobBoundingRect; NodeRenderableBounded::GetBoundingRect

********************************************************************************************/

DocRect NodePath::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	// go and recalculate the bounding rect if it is not known
	if (!IsBoundingRectValid || DontUseAttrs)
	{
		// a rect to put the new version of the bounding rect into
		DocRect NewBoundingRect;

		// Find out what the paths bounding rect is now
		if (!CalculatePathBoundingRect(InkPath, DontUseAttrs, &NewBoundingRect))
		{
			// GDraw failed to find out how big the bounding rect
			// we will have to make do with the bounding rect of the coords
			NewBoundingRect = InkPath.GetBoundingRect();
		}

		// If the line is vertical/horizontal with zero width then we will have a 0 width/height rect
		// Inflate this so rectangle calculations work.
		if (NewBoundingRect.Width() == 0)
			NewBoundingRect.hi.x++;
		if (NewBoundingRect.Height() == 0)
			NewBoundingRect.hi.y++;

		// we have a new bounding rect - decide what to do with it
		if (DontUseAttrs)
		{
			// but it is not the real bounding rect, so just return it
			return NewBoundingRect;
		}
		else
		{
			// We need to go though the attributes applied to this path,
			// and see if any of them effect the bounding rect
			// (eg. ArrowHeads)
			CCAttrMap AttribMap(30);
			if (FindAppliedAttributes(&AttribMap))
			{
				void* pType;
				void* pVal;

				// iterating all (key, value) pairs
				CCAttrMap::iterator end = AttribMap.GetEndPosition();
				for( CCAttrMap::iterator Pos = AttribMap.GetStartPosition(); Pos != end; ++Pos )
				{
					// Get attr at position Pos
					pType = Pos->first;
					pVal = Pos->second;

					if (pVal != NULL)
					{
						if ( ((NodeAttribute*)pVal)->EffectsParentBounds() )
						{
							// Get the bounds of the attribute and Union it with
							// the path bounds
							DocRect AttrBounds = 
								((NodeAttribute*)pVal)->GetAttrBoundingRect(this, &AttribMap);
							NewBoundingRect = NewBoundingRect.Union(AttrBounds);
						}
					}
				}
			}

			// Update the Nodes bounding rectangle
			BoundingRectangle = NewBoundingRect;

			// Mark the rect as valid
			IsBoundingRectValid = TRUE;
		}
	}

	// return the current bounding rect
	return BoundingRectangle;
}



/********************************************************************************************

>	DocRect NodePath::GetBlobBoundingRect()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/02/94
	Returns:	DocRect - Returns the bounding rect of the path and its blobs
	Purpose:	This calculates the bounding box of the path and adds in the influence of
				the selection blobs. It does not consider if the blobs are visible or not,
				it just gives the bounding box that they would occupy if they were visible

********************************************************************************************/

DocRect NodePath::GetBlobBoundingRect()
{
	// Also, scan through all the points to find the bounds of the blobs
	PathVerb* Verbs = InkPath.GetVerbArray();
	PathFlags* Flags = InkPath.GetFlagArray();
	DocCoord* Coords = InkPath.GetCoordArray();
	INT32 NumCoords = InkPath.GetNumCoords();

	DocRect BlobRect;
	DocRect Rect;

	//Go get the blob manager and just return the bounding rect
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	if (BlobMgr==NULL)
		return GetBoundingRect();

	// loop through all the coords adding them to the bounding rect
	for (INT32 i=0; i<NumCoords; i++)
	{
		if (i==0)
		{
			// Set up initial rect to be just the first point
			Rect = DocRect(Coords[0].x, 	 Coords[0].y,
						   Coords[0].x + 1L, Coords[0].y + 1L);
		}
		else
		{
			// Expand the rectangle to include this point
			Rect.IncludePoint(Coords[i]);
		}
	}

	// Put blobs at the bottom-left and top-right corners, and include these in the rectangle
	BlobMgr->GetBlobRect(Rect.lo, &BlobRect);
	Rect = Rect.Union(BlobRect);
	BlobMgr->GetBlobRect(Rect.hi, &BlobRect);
	Rect = Rect.Union(BlobRect);

	// Now make sure we include any extra blobs dependent on the
	// blob type

	BlobStyle Blobs = BlobMgr->GetCurrentInterest(TRUE);
	if (Blobs.Pen)
	{
		// ok, if its the pen blobs, we need to do extra things over
		// and above the standard coord blobs. We need to add an extra
		// ghost point at the end of the path, if there is a curveto

		PathVerb LastVerb = Verbs[NumCoords-1];
		PathFlags LastFlag = Flags[NumCoords-1];

		if ((LastVerb == PT_BEZIERTO) && (LastFlag.IsRotate))
		{
			DocCoord Pt0 = Coords[NumCoords-2];
			DocCoord Pt1 = Coords[NumCoords-1];

			DocCoord GhostPt;
			GhostPt.x = Pt1.x - (Pt0.x - Pt1.x);
			GhostPt.y = Pt1.y - (Pt0.y - Pt1.y);

			BlobMgr->GetBlobRect(GhostPt,&BlobRect);
			Rect = Rect.Union(BlobRect);
		}
	}

	// Make sure we include the Bounds of our children
	IncludeChildrensBoundingRects(&Rect);

	// return the rectangle with the blobs included
	return Rect;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

/********************************************************************************************

>	virtual BOOL NodePath::CanBecomeA(BecomeA* pBecomeA)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/94
	Inputs:		pClass = runtime class to node you wish this node to become
				pNumObjects = ptr to place number of objects of type pClass that will be created (Note: can be NULL).
							  *pNumObects in undefined on entry
	Outputs:	-
	Returns:	TRUE if it can, FALSE if it can't 
	Purpose:	Interrogation routine to see if a node can be changed into a different node type.
				Returns TRUE if pClass is a NodePath, FALSE otherwise

				The number you put into pNumObjects (if it's not NULL) should exactly equal the total number
				of pClass objects you create.  It should NOT contain any additional objects you may produce
				such as group objects for containing the pClass object, or attributes.

				Also, the entry value of *pNumObjects cannot be assumed to be 0.

	Errors:		-
	SeeAlso:	NodePath::DoBecomeA()

********************************************************************************************/

BOOL NodePath::CanBecomeA(BecomeA* pBecomeA)
{
	if (pBecomeA->BAPath())
	{
		pBecomeA->AddCount(1);

		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	virtual BOOL NodePath::DoBecomeA(BecomeA* pBecomeA)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/94
	Inputs:		pBecomeA = 	ptr to info class containing everything a node needs to be able
							to become something else
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Actually tries to change the node into a different node type.
				This responds to BECOMEA_PASSBACK reason by making a duplicate NodePath and passing
				it to the pfnPassBack func provided in the pBecomeA class
	Errors:		-
	SeeAlso:	NodePath::CanBecomeA()

********************************************************************************************/

BOOL NodePath::DoBecomeA(BecomeA* pBecomeA)
{
	// Check for a NULL entry param
	ERROR2IF_PF(pBecomeA == NULL,FALSE,("pBecomeA is NULL"));

	// This lump checks that the Reason is one that we understand
	// It also makes sure that we don't have a NULL UndoOp ptr
	BOOL ValidReason = (pBecomeA->GetReason() == BECOMEA_REPLACE || pBecomeA->GetReason() == BECOMEA_PASSBACK);
	ERROR2IF_PF(!ValidReason,FALSE,("Unkown BecomeA reason %d",pBecomeA->GetReason()));

	BOOL 		Success = TRUE;			// Our success flag (Important that this defaults to TRUE)
	NodePath* 	pNewNodePath = NULL;	// Ptr to a new NodePath, if we get to make one.
	
	if (pBecomeA->BAPath())
	{
		switch (pBecomeA->GetReason())
		{
			case BECOMEA_REPLACE:
				// We do nothing as we are already a NodePath in the tree
				// except tell the caller where we are in the tree...
				pBecomeA->PassBack(this, this);
				break;

			case BECOMEA_PASSBACK :
			{
				// Make a copy of this NodePath
				CALL_WITH_FAIL(((pNewNodePath = (NodePath*)SimpleCopy()) != NULL), pBecomeA->GetUndoOp(), Success);

				// If successful, pass it back with my attribute map
				if (Success) Success = pBecomeA->PassBack(pNewNodePath,this);
			}
			break;
			default: break;
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
>  	virtual NodePath* NodePath::GetVariableWidthStrokePath()
    
	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    15-5-2000
	Inputs:     -  
    Outputs:    -
    Returns:    a nodepath containing the path that is generated by any variable width strokes that
				are applied to us, or NULL if there aren't any
    Purpose:    If we have a variable width stroke applied to us then this will get the path generated
				by that stroke.  This base class version returns NULL, overridden versions must supply
				their own outline path.

				In this version we simply use our inkpath

	See also:   NodePath::GetVariableWidthStrokePath, NodeRegularShape::GetVariableWidthStrokePath
				
	
***********************************************************************************************/

NodePath* NodePath::GetVariableWidthStrokePath()
{
	// first find out if we actually have a variable width applied to us, if not then we don't do anything
	AttrVariableWidth* pVarWidth = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrVariableWidth), (NodeAttribute**)&pVarWidth);
	if (pVarWidth && ((VariableWidthAttrValue*)pVarWidth->GetAttributeValue())->GetWidthFunction() == NULL)
		return NULL;

	// next find the stroke attribute that actually does the work
	AttrStrokeType* pStroke = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeType), (NodeAttribute**)&pStroke);
	
	NodePath* pRetNode = NULL;
	if (pStroke && pStroke->HasPathProcessor())
	{
		PathProcessorStroke* pPPS = pStroke->GetPathProcessor();
			
		pRetNode = pPPS->GetProcessedPath(&InkPath, this);
	}
		
	return pRetNode;	
}


/***********************************************************************************************
>  	virtual NodePath* NodePath::GetSmoothVariableWidthStrokePath()
    
	Author:     Priestley (Xara Group Ltd) <camelotdev@xara.com> ripped from Diccon
    Created:    18-11-2000
	Inputs:     -  
    Outputs:    -
    Returns:    a nodepath containing the path that is generated by any variable width strokes that
				are applied to us, or NULL if there aren't any
    Purpose:    If we have a variable width stroke applied to us then this will get the path generated
				by that stroke.  This base class version returns NULL, overridden versions must supply
				their own outline path.

				THIS FUNCTION IS THE SAME AS THAT ABOVE, BUT JUST RETURNS THE SMOOTHED PATH

	See also:   NodePath::GetVariableWidthStrokePath, NodeRegularShape::GetVariableWidthStrokePath
				
	
***********************************************************************************************/

NodePath* NodePath::GetSmoothVariableWidthStrokePath()
{
	// first find out if we actually have a variable width applied to us, if not then we don't do anything
	AttrVariableWidth* pVarWidth = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrVariableWidth), (NodeAttribute**)&pVarWidth);
	if (pVarWidth && ((VariableWidthAttrValue*)pVarWidth->GetAttributeValue())->GetWidthFunction() == NULL)
		return NULL;

	// next find the stroke attribute that actually does the work
	AttrStrokeType* pStroke = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeType), (NodeAttribute**)&pStroke);
	
	NodePath* pRetNode = NULL;
	if (pStroke && pStroke->HasPathProcessor())
	{
		PathProcessorStroke* pPPS = pStroke->GetPathProcessor();
			
		pRetNode = pPPS->GetSmoothProcessedPath(&InkPath, this);
	}
		
	return pRetNode;	
}



/***********************************************************************************************

>	NodePath * NodePath::MakeNodePathFromAttributes(MILLIPOINT Flatness, CCAttrMap * pAttrMap,
							BOOL bIncludeClosedPaths = FALSE, BOOL IncludeWidth = TRUE)

    Author:     David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
    Created:    6/12/99
	Inputs:     Flatness	-	The flatness to use in the stroking
				pAttrMap	-	The attribute map to use (if NULL then use the attribute map
								applied to this object)
    Outputs:     
    Returns:    A new node path, stroked + with all line attributes applied to it (i.e.
				line width, dash patterns, arrowheads etc)
	Purpose:	See returns
    Errors:        
	Scope:      public
	       
**********************************************************************************************/
NodePath * NodePath::MakeNodePathFromAttributes(double Flatness, CCAttrMap * pAttrMap,
												BOOL bIncludeClosedPaths, BOOL IncludeWidth)
{
	// Find the applied attributes
	AttrLineWidth * pAttrLineWidth = NULL;
	AttrStartArrow * pAttrStartArrow = NULL;
	AttrEndArrow * pAttrEndArrow = NULL;
	AttrJoinType * pAttrJoinType = NULL;
	AttrStartCap * pAttrStartCap = NULL;
	AttrDashPattern * pAttrDashPattern = NULL;
	AttrStrokeColour* pAttrStrokeColour = NULL;
	AttrStrokeType* pAttrStrokeType = NULL;
	AttrVariableWidth* pAttrVariableWidth = NULL;

	if (!pAttrMap)
	{
		// get all the attributes out of the node
		FindAppliedAttribute(CC_RUNTIME_CLASS(AttrLineWidth),(NodeAttribute **)(&pAttrLineWidth));
		FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStartArrow),(NodeAttribute **)(&pAttrStartArrow));
		FindAppliedAttribute(CC_RUNTIME_CLASS(AttrEndArrow),(NodeAttribute **)(&pAttrEndArrow));
		FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType),(NodeAttribute **)(&pAttrJoinType));
		FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStartCap),(NodeAttribute **)(&pAttrStartCap));	
		FindAppliedAttribute(CC_RUNTIME_CLASS(AttrDashPattern),(NodeAttribute **)(&pAttrDashPattern));	
		FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour),(NodeAttribute **)(&pAttrStrokeColour));	
		FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeType), (NodeAttribute **)(&pAttrStrokeType));
		FindAppliedAttribute(CC_RUNTIME_CLASS(AttrVariableWidth), (NodeAttribute **)(&pAttrVariableWidth));
	}
	else
	{
		// get all the attributes out of the attribute map
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrLineWidth), (void *&)pAttrLineWidth );
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStartArrow), (void *&)pAttrStartArrow );
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrEndArrow), (void *&)pAttrEndArrow);
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrJoinType), (void *&)pAttrJoinType);
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStartCap), (void *&)pAttrStartCap);
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrDashPattern), (void *&)pAttrDashPattern);
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStrokeColour), (void *&)pAttrStrokeColour);
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStrokeType), (void *&)pAttrStrokeType);
		pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrVariableWidth), (void *&)pAttrVariableWidth);

		if (!pAttrLineWidth)
			FindAppliedAttribute(CC_RUNTIME_CLASS(AttrLineWidth),(NodeAttribute **)(&pAttrLineWidth));
		
		if (!pAttrStartArrow)
			FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStartArrow),(NodeAttribute **)(&pAttrStartArrow));

		if (!pAttrEndArrow)
			FindAppliedAttribute(CC_RUNTIME_CLASS(AttrEndArrow), (NodeAttribute **)(&pAttrEndArrow));
		
		if (!pAttrJoinType)
			FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType),(NodeAttribute **)(&pAttrJoinType));
		
		if (!pAttrStartCap)
			FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStartCap),(NodeAttribute **)(&pAttrStartCap));
		
		if (!pAttrDashPattern)
			FindAppliedAttribute(CC_RUNTIME_CLASS(AttrDashPattern),(NodeAttribute **)(&pAttrDashPattern));	
		
		if (!pAttrStrokeColour)
			FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour),(NodeAttribute **)(&pAttrStrokeColour));	

		if (!pAttrStrokeType)
			FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeType), (NodeAttribute **)(&pAttrStrokeType));

		if(!pAttrVariableWidth)
			FindAppliedAttribute(CC_RUNTIME_CLASS(AttrVariableWidth), (NodeAttribute **)(&pAttrVariableWidth));
	}

// ********************************************************************************************
// Matt 20/10/2000
// At this point if stroke shapes are used, we may have a path consisting of a number of points
// joined by lines - we can represent this much more efficiently as a path of a fewer number of
// points joined by curves: This is achieved by smoothing the path with RetroSmooth
// ********************************************************************************************

	// Mark Howitt 18/12/00 - I`ve now made it check to see if the Stroke colour is transparent, as
	// the outline of the last line width is returned which is not correct.
	// When the outline is made NONE it does not contribute to the objects outline.
	BOOL IsTransparentStroke = FALSE;

	if(pAttrStrokeColour)
	{
		DocColour* pCol = pAttrStrokeColour->Value.GetStartColour();
		IsTransparentStroke = pCol->IsTransparent();
	}

	// If this nodepath belongs to an object with a stroke type attribute, then we should smooth it...
	if (pAttrStrokeType && !IsTransparentStroke)
	{
		// Check to see if the Stroke is the default type by checking the Variable width Attribute
		// Width function pointer!
		if(pAttrVariableWidth && ((VariableWidthAttrValue*)pAttrVariableWidth->GetAttributeValue())->GetWidthFunction() != NULL)
		{
//			double Smooth = 15.0;

#ifdef CONVERTSTROKES  // vector stroking in here

			PathProcessorStroke* pPPS = pAttrStrokeType->GetPathProcessor();
			if (pPPS)
			{
				BOOL OriginalIsClosed = InkPath.IsClosed();

				Path UnStrokedPath;
				UnStrokedPath.Initialise();

				if(OriginalIsClosed)
					UnStrokedPath.CloneFrom(InkPath);

				// path processor stroke always wants stroked unfilled path
				Path ClonePath;
				ClonePath.Initialise();
				ClonePath.CloneFrom(InkPath);

				ClonePath.IsFilled = FALSE;
				ClonePath.IsStroked = TRUE;

				// Path processor makes a nodepath for us
				NodePath* pNewNodePath = pPPS->GetSmoothProcessedPath(&ClonePath, this);

				if (pNewNodePath == NULL)
				{
					ERROR3("Urk - Cannae get yer stroak path lassie");
				}
				else
				{
					double SFlat = pNewNodePath->InkPath.CalculateFlatnessValueFromPath(500.0,2.0,375.0);
					double CFlat = UnStrokedPath.CalculateFlatnessValueFromPath(500.0,2.0,375.0);
					DWORD ClipRule = OriginalIsClosed ? 7 : 3;
					if(UnStrokedPath.ClipPathToPath(pNewNodePath->InkPath,&pNewNodePath->InkPath,ClipRule|CLIPPING_SOURCE_WINDING,20,SFlat,CFlat) < 0)
					{
						ERROR3("Urk - Cannae get yer stroak path lassie");
					}
				}

				return pNewNodePath;
			}
#else				
			if (RetroSmoothMe(Smooth))
			{
				// make a copy to return
				NodePath * pRetnNode = new NodePath;
				ERROR2IF(!pRetnNode,NULL,"Failed to create a new Path Node!");
				pRetnNode->InkPath.Initialise();
				pRetnNode->InkPath.CloneFrom(InkPath);
				return pRetnNode;
			}
#endif
		}
	}

// ********************************************************************************************
// End of RetroSmoothing...
// ********************************************************************************************

	// Make the return node and initialize it!
	NodePath * pRetnNode = new NodePath;
	ERROR2IF(!pRetnNode,NULL,"Failed to create a new Path Node!");
	pRetnNode->InkPath.Initialise();
	
	Path SrcPath;
	SrcPath.Initialise();
	SrcPath.CloneFrom(InkPath);

	DashPatternAttribute* pDashPattern = NULL;
	StartArrowAttribute* pStartArrow = NULL;
	LineWidthAttribute* pLineWidth = NULL;
	EndArrowAttribute* pEndArrow = NULL;
	JoinTypeAttribute* pJoinType = NULL;
	StartCapAttribute* pStartCap = NULL;

	// set up the attribute values
	if (pAttrLineWidth)
		pLineWidth = &(pAttrLineWidth->Value);

	if (pAttrStartArrow)
		pStartArrow = &(pAttrStartArrow->Value);

	if (pAttrEndArrow)
		pEndArrow = &(pAttrEndArrow->Value);

	if (pAttrJoinType)
		pJoinType = &(pAttrJoinType->Value);

	if (pAttrStartCap)
		pStartCap = &(pAttrStartCap->Value);

	if (pAttrDashPattern)
		pDashPattern = &(pAttrDashPattern->Value);

	// go through each path checking for non-closed paths
	INT32 StartIndex = 0;
	INT32 EndIndex = 0;

	Path SubPath;
	SubPath.Initialise();

	Path FlatPath;
	FlatPath.Initialise();

	UINT32 SubPathNum = 0;
	MILLIPOINT Width = 750;
	MILLIPOINT EndsWidth = 750;

	if(pLineWidth != NULL)
		Width = EndsWidth = pLineWidth->LineWidth;

	if(pAttrStrokeColour && pAttrStrokeColour->GetStartColour()->IsTransparent())
	{
		Width = 0;
		pDashPattern = NULL;
	}

	JoinStyles JoinS = JOIN_ROUND;

	if(pJoinType != NULL)
		JoinS = (pJoinType->JoinType == MitreJoin) ? JOIN_MITER : (pJoinType->JoinType == RoundJoin) ? JOIN_ROUND : JOIN_BEVEL;

	////////////////////////////////////////////////////////////////////////////////
	// Dash pattern - convert to Gavin-ness...
	DashType* pDashRec = NULL;
	DashType GavinDash;
	GavinDash.Length = 0;
	GavinDash.Offset = 0;
	GavinDash.Array[0] = 0;

	// Do do dash patten if there is one and the line width is greater than 0!
	if (pDashPattern && pDashPattern->DashPattern.Elements > 0 && pDashPattern->DashPattern.LineWidth > 0)
	{
		INT32 Length = pDashPattern->DashPattern.Elements;

		if (Length > 8) Length = 8;

		BOOL DoScale = pDashPattern->DashPattern.ScaleWithLineWidth;
		FIXED16 Scale = DoScale ? (double(Width) / double(pDashPattern->DashPattern.LineWidth)) : 1;

		GavinDash.Length = Length;
		GavinDash.Offset = LongMulFixed16(pDashPattern->DashPattern.DashStart, Scale);

		for (INT32 el = 0; el < Length; el++)
		{
			GavinDash.Array[el] = LongMulFixed16(pDashPattern->DashPattern.ElementData[el], Scale);
		}
	}

	// Set the dash pointer to the gavin dash structure!
	pDashRec = &GavinDash;

	////////////////////////////////////////////////////////////////////////////////

	// Make sure that we put the Width to a suitable value if we don`t require line widths in
	// the calculations
	if(!IncludeWidth && pAttrStrokeColour && !pAttrStrokeColour->GetStartColour()->IsTransparent())
		Width = 50;
	
	Path BlankPath;
	BlankPath.Initialise();

	Path StrokedPath;
	StrokedPath.Initialise();

	while (StartIndex < InkPath.GetNumCoords())
	{
		// get the end of the sub-path
		EndIndex = StartIndex;
		InkPath.FindEndElOfSubPath(&EndIndex);
		
		// only do this if the sub-path isn't closed
//		if (!InkPath.IsSubPathClosed(StartIndex) || bIncludeClosedPaths)
		if (!InkPath.IsClosed() || bIncludeClosedPaths)
		{		
			// we have found the last element of the sub-path
			// therefore make a new path
			SubPath.ClearPath();
			InkPath.MakePathFromSubPath(SubPathNum, &SubPath);
			FlatPath.ClearPath();
			FlatPath.CloneFrom(SubPath);
			
			// first, stroke the path			
			SrcPath.IsFilled = FALSE;
			SrcPath.IsStroked = FALSE;

			if (pStartCap && pJoinType)
			{
				SubPath.StrokePathToPath(Width,
					pStartCap->StartCap,
					pJoinType->JoinType,
					pDashRec,
					&StrokedPath,
					Flatness,
					FALSE);
			}
			else
			{
				SubPath.StrokePathToPath(Width,
					LineCapRound,
					RoundJoin,
					pDashRec,
					&StrokedPath,
					Flatness,
					FALSE);
			}

			// ensure the validity of the path
			Path ClipPath;
			ClipPath.Initialise();
			ClipPath.CloneFrom(StrokedPath);
			StrokedPath.ClearPath();

			if(ClipPath.ClipPathToPath(BlankPath, &StrokedPath, 6 | CLIPPING_CLIP_WINDING,30, Flatness, Flatness) < 2)
				StrokedPath.CloneFrom(ClipPath);
			
			// now, do the arrowheads
			INT32 Index = 0;
			DocCoord Centre;
			DocCoord Direction;
			Trans2DMatrix ArrowMatrix;
			
			Path ArrowPath;
			ArrowPath.Initialise();
			
			Path DestPathStartArrow;
			DestPathStartArrow.Initialise();
			
			Path DestPathEndArrow;
			DestPathEndArrow.Initialise();

			ClipPath.ClearPath();

			// do the start arrow
			if (pStartArrow)
			{
				if (pStartArrow->StartArrow.GetArrowPath())
				{
					Centre = FlatPath.GetCoordArray()[0];
					Direction = FlatPath.GetCoordArray()[1];
					
					pStartArrow->StartArrow.GetArrowMatrix(Centre, Direction,
										EndsWidth, &ArrowMatrix);
					
					// build the arrow's path and add it to the existing path
					ArrowPath.ClearPath();
					ArrowPath.CloneFrom(*pStartArrow->StartArrow.GetArrowPath());
					
					ArrowMatrix.Transform(ArrowPath.GetCoordArray(), ArrowPath.GetNumCoords());
					
					DestPathStartArrow.ClearPath();
					ClipPath.CloneFrom(StrokedPath);
					if(ClipPath.ClipPathToPath(ArrowPath,&StrokedPath, 7 | CLIPPING_SOURCE_WINDING,30, Flatness, Flatness) < 2)
					{
						TRACEUSER( "MarkH", _T("ClipFailed so copying path! NODEPATH\n"));
					}

					pRetnNode->InkPath.MergeTwoPaths(DestPathStartArrow);
				}
			}
			
			// do the end arrow
			if (pEndArrow)
			{
				if (pEndArrow->EndArrow.GetArrowPath())
				{
					Index = 0;

					Centre = FlatPath.GetCoordArray()[FlatPath.GetNumCoords() - 1];
					Direction.x = FlatPath.GetCoordArray()[FlatPath.GetNumCoords() - 1].x -
						FlatPath.GetCoordArray()[FlatPath.GetNumCoords() - 2].x;

					Direction.y = FlatPath.GetCoordArray()[FlatPath.GetNumCoords() - 1].y -
						FlatPath.GetCoordArray()[FlatPath.GetNumCoords() - 2].y;

					Direction.x = Centre.x - Direction.x;
					Direction.y = Centre.y - Direction.y;
					
					pEndArrow->EndArrow.GetArrowMatrix(Centre, Direction,
						EndsWidth, &ArrowMatrix);
					
					// build the arrow's path and add it to the existing path
					ArrowPath.ClearPath();
					ArrowPath.CloneFrom(*pEndArrow->EndArrow.GetArrowPath());
					
					ArrowMatrix.Transform(ArrowPath.GetCoordArray(), ArrowPath.GetNumCoords());
					
					DestPathEndArrow.ClearPath();
					ClipPath.CloneFrom(StrokedPath);
					if(ClipPath.ClipPathToPath(ArrowPath,&StrokedPath, 7 | CLIPPING_SOURCE_WINDING,30, Flatness, Flatness) < 2)
					{
						ArrowPath.CloneFrom(ClipPath);
						TRACEUSER( "MarkH", _T("ClipFailed so copying path! NODEPATH\n"));
					}

					pRetnNode->InkPath.MergeTwoPaths(DestPathEndArrow);
				}
			}

			if (pRetnNode->InkPath.GetBoundingRect().Union(StrokedPath.GetBoundingRect()).IsEmpty())
			{
				pRetnNode->InkPath.MergeTwoPaths(StrokedPath);
			}
			else
			{
				ClipPath.CloneFrom(pRetnNode->InkPath);
				pRetnNode->InkPath.ClearPath();
				if(StrokedPath.ClipPathToPath(ClipPath, &(pRetnNode->InkPath), 7 | CLIPPING_SOURCE_WINDING,30, Flatness, Flatness) < 2)
				{
					pRetnNode->InkPath.CloneFrom(ClipPath);
					TRACEUSER( "MarkH", _T("ClipFailed so copying path! NODEPATH\n"));
				}
			}
		}
		else
		{
			SubPath.ClearPath();

			// make sure that if we do require line widths to be taken into account then we need to stroke using the line width!
			if(IncludeWidth)
			{
				// First do the clipping to make sure the windings come out correct!
				SubPath.CloneFrom(InkPath);
				if(BlankPath.ClipPathToPath(SubPath, &StrokedPath, 3, 10, Flatness, Flatness) < 2)
				{
					StrokedPath.CloneFrom(SubPath);
					TRACEUSER( "MarkH", _T("ClipFailed so copying path! NODEPATH\n"));
				}

				// Now get the contour step for the full width!
				StrokedPath.InitializeContourValues(Width,JoinS,true,Flatness);
				if(StrokedPath.GetContourForStep(&pRetnNode->InkPath,1.0) < 2)
				{
					pRetnNode->InkPath.CloneFrom(StrokedPath);
					TRACEUSER( "MarkH", _T("ClipFailed so copying path! NODEPATH\n"));
				}
			}
			else
			{
				// Just clone the original!
				InkPath.MakePathFromSubPath(SubPathNum, &SubPath);
				pRetnNode->InkPath.MergeTwoPaths(SubPath);
			}
		}							

		StartIndex = EndIndex+1; 
		SubPathNum++;
	}

	if (pRetnNode)
	{
		pRetnNode->InkPath.IsFilled = TRUE;
		pRetnNode->InkPath.IsStroked = FALSE;
	}

	return pRetnNode;
}


/***********************************************************************************************

> BOOL NodePath::RetroSmoothMe(double Smoothness)

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    20/9/94
	Inputs:     Smoothness - the amount to smooth by
    Outputs:    
    Returns:    TRUE if everything went ok, FALSE if not.
    			                                  		 
    Purpose:    Use and abuse the retrosmoother to smooth our ink path.  The Retrosmoother is normally
				activated from the gadget in the shape editor tool however we are hijacking it here for our
				purposes.
    Errors:        
	Scope:      public
	       
**********************************************************************************************/

BOOL NodePath::RetroSmoothMe(double Smoothness)
{
	if (Smoothness <= 0)
	{
		ERROR3("Zero smoothing value in NodePath::RetroSmoothMe");
		return TRUE;
	}

	Spread *pSpread = Document::GetSelectedSpread();
	ERROR2IF(pSpread == NULL, FALSE, "No spread in NodePath::RetroSmoothMe");

	//Make a retro smoother and tell it not to bother with EOR rendering on top
	RetroSmooth rSmoother;
	rSmoother.Initialise();
	rSmoother.SetRenderFlag(false);

	// all path points must be selected
	InkPath.SetAllSubSelection();

	// set the smoother to work
	rSmoother.Changing(this, pSpread, Smoothness);
	BOOL ok = rSmoother.FinishedNoUndo(this);
	
	InkPath.ClearSubSelection();

	return ok;
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

/***********************************************************************************************

> BOOL NodePath::Snap(DocCoord* pDocCoord)

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

BOOL NodePath::Snap(DocCoord* pDocCoord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	return CSnap::SnapCoordToPath(pDocCoord, &InkPath);
#else
	return FALSE;
#endif
}



/***********************************************************************************************

> BOOL NodePath::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)

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

BOOL NodePath::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	TRACEUSER( "MarkN", _T("NodePath::Snap(DocRect)\n") );
#endif
	return FALSE;
}




/********************************************************************************************

>	BOOL NodePath::SnapToCoords(DocCoord* pDocCoord)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		pDocCoord - the point to magnetically snap
	Outputs:	pDocCoord - if the point was able to magnetically snap to a coord then
				it is changed to that coord
	Returns:	TRUE if it snapped to a coord, FALSE if not
	Purpose:	Snaps the point to all the magnetic points in a NodePath. These include
				all the endpoint in the path (ie MoveTos, LineTos and the last point in
				CurveTos). If it is close enough to any of these points in the path
				then the coord ischanged to match the point on the path and TRUE is returned.
				pDocCoord is not effected if it is not close enough to any of the points
				in the path
	SeeAlso:	MagneticPointRadius; NodeRenderableBounded::SnapToCoords

********************************************************************************************/

BOOL NodePath::SnapToCoords(DocCoord* pDocCoord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocCoord* Coords = InkPath.GetCoordArray();
	PathFlags* Flags = InkPath.GetFlagArray();
	INT32 NumCoords = InkPath.GetNumCoords();

	// loop through all the coords in the path
	for (INT32 i=0; i<NumCoords; i++)
	{
		// Only test the endpoints
		if (Flags[i].IsEndPoint)
		{
			// Now test the coord to see if it is close
			if (IsMagneticallyClose(&Coords[i], pDocCoord))
				return TRUE;
		}
	}
#endif
	// Did not find a match
	return FALSE;
}



/********************************************************************************************

>	double NodePath::GetRotationAngle()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Outputs:	-
	Returns:	The current angle of rotation of the path
	Purpose:	The overridden function for determining the current angle of rotation of this 
				path
	SeeAlso:	NodeRenderableBounded::GetRotationAngle

********************************************************************************************/

double NodePath::GetRotationAngle()
{
	return CurrentRotation;
}


/********************************************************************************************

>	virtual void NodePath::DeSelect(BOOL ReDraw, BOOL bDeselectChildren)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		ReDraw
	Outputs:	-
	Returns:	-
	Purpose:	When you select a NodePath we can to clear all the selection flags in the 
				path before deselecting the node.
	SeeAlso:	NodeRenderable::DeSelect

********************************************************************************************/

void NodePath::DeSelect(BOOL ReDraw, BOOL bDeselectChildren)
{
	if (ReDraw && GetApplication()->GetBlobManager()->GetCurrentInterest().Object)
		InkPath.RenderPathSelectedControlBlobs(FindParentSpread());

	InkPath.ClearSubSelection();

	// Now call the parent class
	NodeRenderableInk::DeSelect(ReDraw, bDeselectChildren);
}



/********************************************************************************************

>	virtual BOOL NodePath::IsPathAllowable()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the path is valid
				FALSE if is not valid
	Purpose:	This function following some operations on the path.  It allows the path (and
				objects derived from NodePath) to say wether the operation has left them
				in an invalid state.  For example, following a delete a closed path of one
				segment is not allowed.
				Further example, Mould paths can override this function to ensure there is
				at least four points on the path after a delete 
	SeeAlso:	-

********************************************************************************************/
BOOL NodePath::IsPathAllowable()
{
	INT32 NumCoords = InkPath.GetNumCoords();
	PathVerb* Verbs = NULL;
	PathFlags* Flags = NULL;
	DocCoord* Coords = NULL;
	InkPath.GetPathArrays(&Verbs, &Coords, &Flags);

	// It's a moveto/lineto combination
	if ( (NumCoords == 2) && (Verbs[1] & PT_CLOSEFIGURE) && ((Verbs[1] & ~PT_CLOSEFIGURE) == PT_LINETO) )
	{
		ERROR3IF(Verbs[0] != PT_MOVETO,"Path didn't start with a moveto!");
		return FALSE;
	}

	// It's a moveto/curveto combination
	if ( (NumCoords == 4) && (Verbs[3] & PT_CLOSEFIGURE) && ((Verbs[3] & ~PT_CLOSEFIGURE) == PT_BEZIERTO) ) 
	{
		ERROR3IF(Verbs[0] != PT_MOVETO,"Path didn't start with a moveto!");
		ERROR3IF(Verbs[1] != PT_BEZIERTO,"Path didn't continue with a Bezierto!");
		return FALSE;
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL NodePath::NeedsToExport(RenderRegion* pRender, BOOL VisibleLayersOnly = FALSE,
								 BOOL CheckSelected = FALSE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/95
	Inputs:		pRender - A pointer to the current export region
				VisibleLayersOnly - TRUE => remove nodes which are on invisible layers
								   - FALSE => export everything
				CheckSelected - TRUE => we check if object selected and only export selected bjects
							  - FALSE => we don't bother checking for selection or not
	Returns:	TRUE => please export me.
	Purpose:	Virtual function - this version currently returns false if the path is
				an immediate child of a text story otherwise its always exportable

********************************************************************************************/

BOOL NodePath::NeedsToExport(RenderRegion* pRender, BOOL VisibleLayersOnly, BOOL CheckSelected)
{
#ifdef DO_EXPORT
PORTNOTE("EPSRenderRegion", "Removed use of EPSRenderRegion")
#ifndef EXCLUDE_FROM_XARALX
	if (pRender->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)))
	{
		Node* pParent=FindParent();
		if (pParent!=NULL && IS_A(pParent,TextStory))
			return FALSE;
	}
#endif

	// If we have the check selection flag on then see if this node is:-
	// - selected or not = render it
	// - a child of the selection e.g. part of selected group where we are an item in the
	// 	 group and hence are not directly selected but still need to be exported
	// - a parent of the selected item e.g. selected inside item of group and we are at the
	//   group and hence need to include the group in the range 
	// Otherwise just return True as this is a path node and always needs to be exported
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
>	virtual BOOL NodePath::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState = TRUE,
														   BOOL DoPreTriggerEdit = TRUE)

 	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>; Karim 19/01/2000
	Created:	8/5/95
	Inputs:		pParam - pointer to the change parameter object
				SetOpPermissionState - TRUE to set the Nodes permission flags
				DoPreTriggerEdit	 - if TRUE then calls NameGallery::PreTriggerEdit.
									   *Must* be TRUE if the calling Op may make any nodes
									   change their bounds, eg move, line width, cut.
									   Use TRUE if unsure.
	Returns:	TRUE if the operation can proceede, FALSE to stop it
	Purpose:	To get node paths inside text objects to reformat themselves correctly
				when their paths change.
	Also see:	TextStory::AllowOp()
********************************************************************************************/

BOOL NodePath::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState,
											   BOOL DoPreTriggerEdit)
{
	// call its default AllowOp(); we never want to do a PreTriggerEdit for this.
	BOOL allowed = NodeRenderableInk::AllowOp(pParam, SetOpPermissionState, FALSE);

/*
 *	Karim 15/11/2000
 *	Commented this out, as I see no point in pretending to be an Op. *Any* parent of ours
 *	will have its AllowOp() called anyway, via our base class.
 *	
	// if allowed, and our parent is a textstory, call the TextStory's AllowOp() as though called directly by the op
	Node* pParent=FindParent();
	if (allowed && pParent!=NULL && IS_A(pParent,TextStory))
		allowed=pParent->AllowOp(pParam,SetOpPermissionState,DoPreTriggerEdit);
 */

	// if we're ok so far and were asked to do a PreTriggerEdit, then
	// determine whether the Op may change the bounds of some nodes.
	// If it may, then call NameGallery::PreTriggerEdit.
	if (allowed && DoPreTriggerEdit)
	{
		// if the Op is non-NULL then query its MayChangeNodeBounds() method.
		UndoableOperation* pChangeOp = pParam->GetOpPointer();
		if (pChangeOp != NULL && pChangeOp->MayChangeNodeBounds() && NameGallery::Instance())
		{
			allowed = NameGallery::Instance()->PreTriggerEdit(pChangeOp, pParam, this);
		}
	}
	if (SetOpPermissionState)
		SetOpPermission(PERMISSION_ALLOWED);

	return allowed;
}


/********************************************************************************************

>	virtual UINT32 NodePath::ChooseTagValue()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/96
	Inputs:		-
	Returns:	The tag value that should be used for saving this path
	Purpose:	Writes the path record to the filter
	SeeAlso:	This function chooses the tag to use based on the filled & stroked flags of the
				path

********************************************************************************************/

UINT32 NodePath::ChooseTagValue()
{
#ifdef DO_EXPORT
	UINT32 Tag = TAG_UNDEFINED;

	if (BaseCamelotFilter::WritePathsInRelativeFormat())
	{
		Tag = TAG_PATH_RELATIVE;
		if (InkPath.IsFilled && !InkPath.IsStroked) 
			Tag = TAG_PATH_RELATIVE_FILLED;
		else if (!InkPath.IsFilled && InkPath.IsStroked) 
			Tag = TAG_PATH_RELATIVE_STROKED;
		else if (InkPath.IsFilled && InkPath.IsStroked) 
			Tag = TAG_PATH_RELATIVE_FILLED_STROKED;
	}
	else
	{
		Tag = TAG_PATH;
		if (InkPath.IsFilled && !InkPath.IsStroked) 
			Tag = TAG_PATH_FILLED;
		else if (!InkPath.IsFilled && InkPath.IsStroked) 
			Tag = TAG_PATH_STROKED;
		else if (InkPath.IsFilled && InkPath.IsStroked) 
			Tag = TAG_PATH_FILLED_STROKED;
	}

	return Tag;
#else
	return TAG_UNDEFINED;
#endif
}



/********************************************************************************************

>	virtual INT32 NodePath::EstimateNodeComplexity (OpParam* details)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/09/2000

	Inputs:		details		any data that should be used for the calculation

	Outputs:	-

	Returns:	an estimate of the nodes complexity

	Purpose:	This function estimates a complexity value for the node.  The complexity
				value is based upon the total length of all paths in the node.

	See Also:	OpBlendNodes::DeterminBlendObjectsProcessorHit ()

********************************************************************************************/

INT32 NodePath::EstimateNodeComplexity (OpParam* details)
{
	return (InkPath.GetUsedSlots ());
}

/********************************************************************************************

>	virtual INT32 NodePath::CalcPathRecordSize()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/96
	Inputs:		-
	Returns:	The size of the record in bytes
	Purpose:	This func is the centre point at which the size of the record that will contain
				the path is calculated
	SeeAlso:	BaseCamelotFilter::WritePathsInRelativeFormat()

********************************************************************************************/

INT32 NodePath::CalcPathRecordSize()
{
	INT32 NumCoords = InkPath.GetNumCoords();

 	if (BaseCamelotFilter::WritePathsInRelativeFormat())
		return (sizeof(PathVerb)*NumCoords)+(sizeof(DocCoord)*NumCoords);
	else
		return sizeof(INT32)+(sizeof(PathVerb)*NumCoords)+(sizeof(DocCoord)*NumCoords);
}

/********************************************************************************************

>	virtual BOOL NodePath::WritePathToRecord(CamelotFileRecord* pRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/96
	Inputs:		pRecord = ptr to a record to write to
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Central func writes out the correct path record.
	SeeAlso:	BaseCamelotFilter::WritePathsInRelativeFormat()

********************************************************************************************/

BOOL NodePath::WritePathToRecord(CamelotFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL,FALSE,"NULL record ptr");

	if (BaseCamelotFilter::WritePathsInRelativeFormat())
		return pRecord->WritePathRelative(&InkPath);
	else
		return pRecord->WritePath(&InkPath);
}

/********************************************************************************************

>	virtual BOOL NodePath::WritePathRecord(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodePath::WritePathRecord(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	INT32 RecordSize = CalcPathRecordSize();

	BOOL ok = TRUE;

	// Which type of path is it to be?
	UINT32 Tag = ChooseTagValue();

	CamelotFileRecord Rec(pFilter,Tag,RecordSize);

	ok = Rec.Init();
	if (ok) ok = WritePathToRecord(&Rec);

	UINT32 RecordNumber;
	if (ok) RecordNumber = pFilter->WriteDefinitionRecord(&Rec);
	if (ok) ok = (RecordNumber != 0);
	if (ok) pFilter->AddPathRecordRefToList(this,RecordNumber);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL NodePath::WritePathRefRecord(BaseCamelotFilter* pFilter,UINT32 SrcPathRecNum,Matrix* pTransform);

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/96
	Inputs:		pFilter = ptr to the filter
				SrcPathRecNum = the record that contains the path that's similar to this path
				pTransform	  = How to transform the path in SrcPathRecNum to get this path bac
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the path reference record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodePath::WritePathRefRecord(BaseCamelotFilter* pFilter,UINT32 SrcPathRecNum,Matrix* pTransform)
{
	ERROR2IF(pTransform == NULL,FALSE,"NULL transform matrix ptr");

	BOOL ok = TRUE;

	UINT32 Tag  = TAG_PATHREF_TRANSFORM;
	INT32  Size = TAG_PATHREF_TRANSFORM_SIZE;

	CamelotFileRecord Rec(pFilter,Tag,Size);
	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteReference(SrcPathRecNum);
	if (ok) ok = Rec.WriteMatrixTrans(*pTransform,0,0);
	if (ok) ok = (pFilter->Write(&Rec) != 0);

	return ok;
}


/********************************************************************************************

  >	virtual BOOL NodePath::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodePath::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT

	BOOL ok = TRUE;
	UINT32 SrcPathRecNum;
	Matrix Transform;

	INT32 RecordSize = CalcPathRecordSize();

	// Only call FindSimilarPath() if the path is bigger than the path reference record.
	// there's no point in creating a reference if the ref is bigger than the actual path

	if (RecordSize > TAG_PATHREF_TRANSFORM_SIZE	&& pFilter->FindSimilarPath(this,&SrcPathRecNum,&Transform))
		ok = WritePathRefRecord(pFilter,SrcPathRecNum,&Transform);
	else
		ok = WritePathRecord(pFilter);

	return ok;

#else
	return FALSE;
#endif
}

/********************************************************************************************

  >	virtual BOOL NodePath::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodePath::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT

	// If it's the compact native filter (i.e. the non-minimal web format) OR the path
	// similarity check has been selected to apply to the native format,
	// do as you would for the web format.
	if (pFilter->IsCompactNativeFilter() || pFilter->GetNativeCheckSimilarPaths())
		return WritePreChildrenWeb(pFilter);
	else
		return WritePathRecord(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	BOOL NodePath::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Inputs:		pFilter
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Begin to write out you child records, in the native format

				This will write out a TAG_DOWN record, followed by a path flags record
	SeeAlso:	-

********************************************************************************************/

BOOL NodePath::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = pFilter->WriteZeroSizedRecord(TAG_DOWN);

	if (ok)
	{
		INT32 NumCoords = InkPath.GetNumCoords();
		INT32 RecordSize = sizeof(BYTE)*NumCoords;

		CamelotFileRecord Rec(pFilter,TAG_PATH_FLAGS,RecordSize);
		ok = Rec.Init();

		PathFlags* pFlags = InkPath.GetFlagArray();

		if (ok && pFlags != NULL)
		{
			BYTE Flags;
			for (INT32 i=0; ok && i < NumCoords;i++)
			{
				Flags = 0;

				if (pFlags[i].IsSmooth)		Flags |= TAG_PATH_FLAGS_SMOOTH;
				if (pFlags[i].IsRotate)		Flags |= TAG_PATH_FLAGS_ROTATE;
				if (pFlags[i].IsEndPoint)	Flags |= TAG_PATH_FLAGS_ENDPOINT;

				ok = Rec.WriteBYTE(Flags);
			}

			ok = pFilter->Write(&Rec);
		}

		if (ok)
		{
			if (pFilter->GetBoundsWriteLevel() >= BWL_ALL)
				ok = WriteBoundsRecord(pFilter);
		}
	}

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL NodePath::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Inputs:		pFilter
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	End writing out you child records, in the native format

				This will write out a TAG_UP record
	SeeAlso:	-

********************************************************************************************/

BOOL NodePath::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	return pFilter->WriteZeroSizedRecord(TAG_UP);
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	virtual BOOL NodePath::IsDifferent(Node *pOther)

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

				NodePath checks that the 2 InkPaths contain the same set of Coords
				and Verbs - currently IGNOREs everything else

********************************************************************************************/

BOOL NodePath::IsDifferent(Node *pOther)
{
	// First, check with the base class - this checks the classes are the same type
	if (NodeRenderableInk::IsDifferent(pOther))
		return(TRUE);

	ERROR3IF(GetRuntimeClass() != pOther->GetRuntimeClass(),
			"Node::IsDifferent base class method seems to have been broken");

	// Now, check the path info
	if (InkPath.GetNumCoords() != ((NodePath *)pOther)->InkPath.GetNumCoords())
		return(TRUE);

	// Check if all the coords and verbs are the same
	DocCoord *pCoord1 = InkPath.GetCoordArray();
	DocCoord *pCoord2 = ((NodePath *)pOther)->InkPath.GetCoordArray();

	PathVerb *pVerb1  = InkPath.GetVerbArray();
	PathVerb *pVerb2  = ((NodePath *)pOther)->InkPath.GetVerbArray();

	for (INT32 i = 0; i < InkPath.GetNumCoords(); i++)
	{
		if (pVerb1[i] != pVerb2[i] || pCoord1[i] != pCoord2[i])
			return(TRUE);
	}

	return(FALSE);
}



/********************************************************************************************

>	virtual BOOL NodePath::IsANodeBlendPath()

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		-
	Returns:	FALSE;
	Purpose:	virtual identifier function,will move to node.h/.cpp when i have time to wait 
				for the rebuild


********************************************************************************************/

BOOL NodePath::IsANodeBlendPath()
{
	return FALSE;
}



/********************************************************************************************

>	virtual DocRect NodePath::ValidateExtend(const ExtendParams& ExtParams) const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/1999
	Inputs:		ExtParams		parameters describing the extension.
	Outputs:	
	Returns:	TRUE if this path can validly extend,
				FALSE otherwise.
	Purpose:	Tests to see whether this path's control points are positioned so as to
				make an extend operation irreversible.
	Errors:		
	See also:	

********************************************************************************************/
DocRect NodePath::ValidateExtend(const ExtendParams& ExtParams)
{
	INT32 numPoints = InkPath.GetNumCoords();
	DocCoord* doccArray = InkPath.GetCoordArray();
	DocRect drMinExtend = Extender::ValidateControlPoints(numPoints, doccArray, ExtParams);

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

>	virtual void NodePath::Extend(const ExtendParams& ExtParams) const
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/1999
	Inputs:		ExtParams		parameters describing the extension.
	Outputs:	this NodePath will be extended in accordance with ExtParams.
	Returns:	
	Purpose:	Perform an Extend operation on this path. Behaviour is as follows:

				* the path extends separately along each axis.
				* if the path is asked to stretch, it scales along the corresponding axes.
				* if the path is asked to extend, its control points translate, as described
																				by ExtParams.
	Errors:		
	See also:	class Extender

********************************************************************************************/
void NodePath::Extend(const ExtendParams& ExtParams)
{
	TransformTranslateObject(ExtParams);
	TransformStretchObject(ExtParams);

	// do the base-class implementation to extend our children.
	Node::Extend(ExtParams);
}

/********************************************************************************************

>	virtual void NodePath::TransformTranslateObject(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/1999
	Inputs:		ExtParams		description of how this path should extend.
	Outputs:	This path's control points may be extended, depending on ExtParams' flags.
	Purpose:	Do an extend (as opposed to stretch) operation on this path, using ExtParams
				as the source of the extend data, together with the extend-centre of this
				path, defined in NodeRenderable::FindExtendCentre().

				This function does nothing unless ExtParams uses X_EXTEND or Y_EXTEND.
	See also:	The Extender class; FindExtendCentre().

********************************************************************************************/
void NodePath::TransformTranslateObject(const ExtendParams& ExtParams)
{
	// get the path's control points.
	INT32 numPoints = InkPath.GetNumCoords();
	DocCoord* doccArray = InkPath.GetCoordArray();

	// x-extension behaviour.
	if (ExtParams.fExtendFlags & X_EXTEND)
	{
		// translate the whole path by the offset from start- to end- centres.
		Trans2DMatrix baseXoffset(ExtParams.doccOffset.x, 0);
		Transform(baseXoffset);

		for (INT32 i = 0; i < numPoints; i ++)
		{
			if (doccArray[i].x > (ExtParams.doccEndCentre.x + ExtParams.xincExtendBuffer))
				doccArray[i].x += ExtParams.xinc;
			else if (doccArray[i].x < (ExtParams.doccEndCentre.x - ExtParams.xdecExtendBuffer))
				doccArray[i].x -= ExtParams.xdec;
		}
	}

	// y-extension behaviour.
	if (ExtParams.fExtendFlags & Y_EXTEND)
	{
		// translate the whole path by the offset from start- to end- centres.
		Trans2DMatrix baseYoffset(0, ExtParams.doccOffset.y);
		Transform(baseYoffset);

		for (INT32 i = 0; i < numPoints; i ++)
		{
			if (doccArray[i].y > (ExtParams.doccEndCentre.y + ExtParams.yincExtendBuffer))
				doccArray[i].y += ExtParams.yinc;
			else if (doccArray[i].y < (ExtParams.doccEndCentre.y - ExtParams.ydecExtendBuffer))
				doccArray[i].y -= ExtParams.ydec;
		}
	}
}



/********************************************************************************************

>	virtual DocRect NodePath::GetExtendTargetBounds(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/2000
	Inputs:		ExtParams		const ref to an ExtendParams class describing the extension.
	Outputs:	
	Returns:	The DocRect to treat as the bounding rect of this node when extending.
	Purpose:	Return a DocRect which contains the bounds of this node as defined and
				required by the Extending mechanism.

				We treat x- and y- directions separately, and we always return our true
				bounds, unless we're extending in a direction, in which case we return the
				bounds of our extend control points, which are currently all of our inkpath's
				control points.
	Errors:		
	See also:	SliceHelper::BoundingNodeSize().

********************************************************************************************/
DocRect NodePath::GetExtendTargetBounds(const ExtendParams& ExtParams)
{
	// think twice about using InkPath::GetTrueBoundingRect() here - using
	// NodePath::GetBoundingRect takes factors such as attributes into account.
	DocRect	drBounds		= GetBoundingRect();
	DocRect	drPointBounds	= InkPath.GetBoundingRect();
	if (ExtParams.fExtendFlags & X_EXTEND)
	{
		drBounds.lo.x = drPointBounds.lo.x;
		drBounds.hi.x = drPointBounds.hi.x;
	}
	if (ExtParams.fExtendFlags & Y_EXTEND)
	{
		drBounds.lo.y = drPointBounds.lo.y;
		drBounds.hi.y = drPointBounds.hi.y;
	}

	return drBounds;
}



/********************************************************************************************

>	static BOOL NodePath::CreateFromPath(NodePath** ppNewPath,	Path* pSrcPath = NULL,
																Operation* pOp = NULL);
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/2000
	Inputs:		ppNewPath	ptr to the ptr holding any new NodePath.

				pSrcPath	the path which the new NodePath will copy its data from.
							if NULL, then you just get back a fresh NodePath, ready for use.

				pOp			an Op ptr - if we run out of memory, then we can reclaim memory
							via this ptr; ok if this is NULL.

	Outputs:	*pNewPath holds a new NodePath, if successful, NULL otherwise.

	Returns:	TRUE if success, FALSE otherwise.

	Purpose:	Create and initialise a new NodePath, using the given path information if
				provided.

	Errors:		ERROR2 with FALSE if ppNewPath is NULL.

********************************************************************************************/
BOOL NodePath::CreateFromPath(NodePath** ppNewPath, Path* pSrcPath, Operation* pOp)
{
	ERROR2IF(ppNewPath == NULL, FALSE, "NodePath::CreateFromPath; called with NULL param!");

	NodePath* pNewPath = NULL;
	ALLOC_WITH_FAIL(pNewPath, new NodePath, pOp);
	if (pNewPath != NULL)
	{
		if (pSrcPath != NULL)
		{
			BOOL	ok = pNewPath->InkPath.Initialise(pSrcPath->GetNumCoords(), 1);
			if (ok)
				CALL_WITH_FAIL(pNewPath->InkPath.CopyPathDataFrom(pSrcPath), pOp, ok);

			if (!ok)
			{
				delete pNewPath;
				pNewPath = NULL;
			}
		}
		else
		{
			pNewPath->SetUpPath();
		}
	}

	*ppNewPath = pNewPath;
	return (pNewPath != NULL);
}


/********************************************************************************************

>	Path* NodePath::GetPathCopy()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2000

	Returns:	ptr to a copy of our path if successful,
				NULL otherwise.

	Purpose:	A quick'n'easy way to get a permanent copy of our path.

	Notes:		The copy is allocated on the heap, so it's *your* responsibility
				to dispose of it.

********************************************************************************************/
Path* NodePath::GetPathCopy()
{
	Path* pResult = new Path;
	if (pResult != NULL)
	{
		if (!pResult->Initialise(InkPath.GetNumCoords()) ||
			!pResult->CloneFrom(InkPath))
		{
			delete pResult;
			pResult = NULL;
		}
	}

	return pResult;
}



/********************************************************************************************

>	virtual BOOL NodePath::GetAreaDetails(XLONG* pxlArea, XLONG* pXLPerimeter)


	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/03/2005
	Outputs:	xlArea
				xlPerimeter
	Returns:	TRUE if calcs succeeded
	Purpose:	Work out 2-dimensional properties of this node

********************************************************************************************/
BOOL NodePath::GetAreaDetails(XLONG* pxlArea, XLONG* pxlPerimeter)
{
	BOOL bOK = TRUE;

	*pxlArea = InkPath.CalcArea();
	double dDistance = 0;
	bOK = InkPath.DistanceTo(InkPath.GetNumCoords()-1, 1, &dDistance);
	*pxlPerimeter = (INT32)dDistance;

	return TRUE;
}



//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------



/********************************************************************************************

>	PathRecordHandler::PathRecordHandler()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Inputs:		-
	Returns:	-
	Purpose:	Default constructor
	SeeAlso:	-

********************************************************************************************/

//PathRecordHandler::PathRecordHandler()
//{
//	pLastInsertedPath = NULL;
//}

/********************************************************************************************

>	BOOL PathRecordHandler::BeginImport()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Inputs:		-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Informs the handler that the filter's about to start importing
	SeeAlso:	-

********************************************************************************************/

//BOOL PathRecordHandler::BeginImport()
//{
//	pLastInsertedPath = NULL;
//	return TRUE;
//}

/********************************************************************************************

>	virtual UINT32* PathRecordHandler::GetTagList()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* PathRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {	TAG_PATH,
								TAG_PATH_FILLED,
								TAG_PATH_STROKED,
								TAG_PATH_FILLED_STROKED,
								TAG_PATH_RELATIVE,
								TAG_PATH_RELATIVE_FILLED,
								TAG_PATH_RELATIVE_STROKED,
								TAG_PATH_RELATIVE_FILLED_STROKED,
//								TAG_PATH_FLAGS,
								TAG_PATHREF_IDENTICAL,
								TAG_PATHREF_TRANSFORM,
								CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	virtual BOOL PathRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL PathRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	
	BOOL ok = TRUE;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_PATH:							ok = HandlePathRecord(pCXaraFileRecord,FALSE,FALSE);break;
		case TAG_PATH_FILLED:					ok = HandlePathRecord(pCXaraFileRecord,TRUE,FALSE);	break;
		case TAG_PATH_STROKED:					ok = HandlePathRecord(pCXaraFileRecord,FALSE,TRUE);	break;
		case TAG_PATH_FILLED_STROKED:			ok = HandlePathRecord(pCXaraFileRecord,TRUE,TRUE);	break;

		case TAG_PATH_RELATIVE:					ok = HandlePathRelativeRecord(pCXaraFileRecord,FALSE,FALSE);break;
		case TAG_PATH_RELATIVE_FILLED:			ok = HandlePathRelativeRecord(pCXaraFileRecord,TRUE,FALSE);	break;
		case TAG_PATH_RELATIVE_STROKED:			ok = HandlePathRelativeRecord(pCXaraFileRecord,FALSE,TRUE);	break;
		case TAG_PATH_RELATIVE_FILLED_STROKED:	ok = HandlePathRelativeRecord(pCXaraFileRecord,TRUE,TRUE);	break;

//		case TAG_PATH_FLAGS:					ok = HandlePathFlagsRecord(pCXaraFileRecord,GetLastInsertedPath());	break;

		case TAG_PATHREF_IDENTICAL:
		case TAG_PATHREF_TRANSFORM:				ok = HandlePathRefRecord(pCXaraFileRecord);		break;

		default:
			ok = FALSE;
			ERROR3_PF(("I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}


/********************************************************************************************

>	BOOL PathRecordHandler::HandlePathRecord(CXaraFileRecord* pCXaraFileRecord,BOOL Filled,BOOL Stroked)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
				Filled = TRUE if the path should be filled
				Stroked= TRUE if the path should be stroked
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a path record
	SeeAlso:	-

********************************************************************************************/

BOOL PathRecordHandler::HandlePathRecord(CXaraFileRecord* pCXaraFileRecord,BOOL Filled,BOOL Stroked)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR3IF(!IsTagInList(pCXaraFileRecord->GetTag()),"I don't handle this tag type");

	BOOL ok = FALSE;

	NodePath* pNodePath = new NodePath;

	if (pNodePath != NULL && pNodePath->SetUpPath())
	{
		ok = pCXaraFileRecord->ReadPath(&pNodePath->InkPath); 

		if (ok)
		{
			pNodePath->InkPath.IsFilled = Filled;	// Set the filled flag
			pNodePath->InkPath.IsStroked= Stroked;	// Set the stroked flag
			pNodePath->InkPath.InitialiseFlags();	// Init the path flags array to something sensible
		}
	}
			
	if (ok)	ok = InsertNode(pNodePath);
	if (ok) SetLastNodePathInserted(pNodePath);

	if (ok) AddPathRecordRefToList(pNodePath,pCXaraFileRecord->GetRecordNumber());

	return ok;
}

/********************************************************************************************

>	BOOL PathRecordHandler::HandlePathRelativeRecord(CXaraFileRecord* pCXaraFileRecord,BOOL Filled,BOOL Stroked)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
				Filled = TRUE if the path should be filled
				Stroked= TRUE if the path should be stroked
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a path relative record
	SeeAlso:	-

********************************************************************************************/

BOOL PathRecordHandler::HandlePathRelativeRecord(CXaraFileRecord* pCXaraFileRecord,BOOL Filled,BOOL Stroked)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR3IF(!IsTagInList(pCXaraFileRecord->GetTag()),"I don't handle this tag type");

	BOOL ok = FALSE;

	NodePath* pNodePath = new NodePath;

	if (pNodePath != NULL && pNodePath->SetUpPath())
	{
		ok = pCXaraFileRecord->ReadPathRelative(&pNodePath->InkPath); 

		if (ok)
		{
			pNodePath->InkPath.IsFilled = Filled;	// Set the filled flag
			pNodePath->InkPath.IsStroked= Stroked;	// Set the stroked flag
			pNodePath->InkPath.InitialiseFlags();	// Init the path flags array to something sensible
		}
	}
			
	if (ok)	ok = InsertNode(pNodePath);
	if (ok) SetLastNodePathInserted(pNodePath);

	if (ok) AddPathRecordRefToList(pNodePath,pCXaraFileRecord->GetRecordNumber());

	return ok;
}

/********************************************************************************************

>	BOOL PathRecordHandler::HandlePathRefRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a path reference record
	SeeAlso:	-

********************************************************************************************/

BOOL PathRecordHandler::HandlePathRefRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	ERROR3IF(pCXaraFileRecord->GetTag() != TAG_PATHREF_TRANSFORM,"I don't handle this tag type");

	BOOL ok = TRUE;

	UINT32	SrcRecNum;
	Matrix	Transform;

	if (ok) ok = pCXaraFileRecord->ReadUINT32(&SrcRecNum);
	if (ok) ok = pCXaraFileRecord->ReadMatrixTrans(&Transform,0,0);

	if (ok)
	{	 
		ok = FALSE; // Default to fail state

		NodePath* pSrcPath = FindPathRecordRefPath(SrcRecNum);
		if (pSrcPath != NULL)
		{
			NodePath* pNodePath = new NodePath;
			if (pNodePath != NULL && pNodePath->SetUpPath())
			{
				if (pNodePath->InkPath.MergeTwoPaths(pSrcPath->InkPath))
				{
					pNodePath->InkPath.IsFilled = pSrcPath->InkPath.IsFilled;
					pNodePath->InkPath.IsStroked= pSrcPath->InkPath.IsStroked;

					// See note in BaseCamelotFilter::FindSimilarPath() for an explaination of why we need 
					// to translate the path before transforming it.

					// Translate the source path so that it has the same coords as that stored in the file
					DocCoord Origin = GetCoordOrigin();
					{
						Matrix			TranslateMat(-Origin.x,-Origin.y);
						Trans2DMatrix	Translate(TranslateMat);
						pNodePath->Transform(Translate);
					}

					// Transform the path with the read in matrix
					Trans2DMatrix Trans(Transform);
					pNodePath->Transform(Trans);

					// Translate the source path back to it's origin position
					{
						Matrix			TranslateMat(Origin.x,Origin.y);
						Trans2DMatrix	Translate(TranslateMat);
						pNodePath->Transform(Translate);
					}

					ok = InsertNode(pNodePath);
					if (ok) SetLastNodePathInserted(pNodePath);
				}
			}
		}
	}

	return ok;
}

/********************************************************************************************

>	virtual void PathRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This is the base class record description generator.
				It doesn't do anything yet.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if XAR_TREE_DIALOG
void PathRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	char s[256];

	// Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	switch (pRecord->GetTag())
	{
		case TAG_PATH:
		case TAG_PATH_FILLED:
		case TAG_PATH_STROKED:
		case TAG_PATH_FILLED_STROKED:
			// Use standard base class func for describing the path textually
			DescribePath(pRecord,pStr);
			break;

		case TAG_PATH_RELATIVE:
		case TAG_PATH_RELATIVE_FILLED:
		case TAG_PATH_RELATIVE_STROKED:
		case TAG_PATH_RELATIVE_FILLED_STROKED:
			// Use standard base class func for describing the path textually
			DescribePathRelative(pRecord,pStr);
			break;

/*		case TAG_PATH_FLAGS:
		{
			UINT32 NumFlags  = pRecord->GetSize()*sizeof(BYTE);

			BYTE Flags;
			for (UINT32 i=0;i < NumFlags;i++)
			{
				pRecord->ReadBYTE(&Flags);

				sprintf(s,"Flags\r\n"); (*pStr) += s;
				sprintf(s,"Smooth:\t%d\r\n",	(Flags & TAG_PATH_FLAGS_SMOOTH)   != 0); (*pStr) += s;
				sprintf(s,"Rotate:\t%d\r\n",	(Flags & TAG_PATH_FLAGS_ROTATE)    != 0); (*pStr) += s;
				sprintf(s,"EndPoint:\t%d\r\n",	(Flags & TAG_PATH_FLAGS_ENDPOINT) != 0); (*pStr) += s;
				sprintf(s,"\r\n"); (*pStr) += s;

			}
		}
		break;
*/
		case TAG_PATHREF_TRANSFORM:
		{
			UINT32	SrcRecNum;
			Matrix	Transform;

			pRecord->ReadUINT32(&SrcRecNum);
			pRecord->ReadMatrix(&Transform);

			sprintf(s,"Src path rec num:\t%d\r\n",SrcRecNum); (*pStr) += s;

			FIXED16 abcd[4];
			INT32    ef[2];
			double  d;
			Transform.GetComponents(abcd,ef);

			d = abcd[0].MakeDouble(); sprintf(s,"a = %g\r\n",d); (*pStr) += s;
			d = abcd[1].MakeDouble(); sprintf(s,"b = %g\r\n",d); (*pStr) += s;
			d = abcd[2].MakeDouble(); sprintf(s,"c = %g\r\n",d); (*pStr) += s;
			d = abcd[3].MakeDouble(); sprintf(s,"d = %g\r\n",d); (*pStr) += s;

			sprintf(s,"e = %d\r\n",ef[0]); (*pStr) += s;
			sprintf(s,"f = %d\r\n",ef[1]); (*pStr) += s;		
		}
		break;
	}
}
#endif
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------


/********************************************************************************************

>	virtual UINT32* PathFlagsRecordHandler::GetTagList()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* PathFlagsRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {	TAG_PATH_FLAGS,
								CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	BOOL PathFlagsRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a path flags record

				NOTE: moved from PathRecordHandler class by Markn 12/5/99
	SeeAlso:	-

********************************************************************************************/

BOOL PathFlagsRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR3IF(pCXaraFileRecord->GetTag() != TAG_PATH_FLAGS,"I don't handle this tag type");

	BOOL ok = TRUE;

	NodePath* pNodePath = GetLastNodePathInserted();

	if (pNodePath != NULL)
	{
		UINT32 NumFlags  = pCXaraFileRecord->GetSize()*sizeof(BYTE);
		UINT32 NumCoords = pNodePath->InkPath.GetNumCoords();

		if (NumCoords == NumFlags)
		{
			PathFlags* pFlags = pNodePath->InkPath.GetFlagArray();

			BYTE Flags;
			for (UINT32 i=0; ok && i < NumFlags;i++)
			{
				ok = pCXaraFileRecord->ReadBYTE(&Flags);

				if (ok)
				{
					pFlags[i].IsSmooth	= (Flags & TAG_PATH_FLAGS_SMOOTH)	!= 0;
					pFlags[i].IsRotate	= (Flags & TAG_PATH_FLAGS_ROTATE)	!= 0;
					pFlags[i].IsEndPoint= (Flags & TAG_PATH_FLAGS_ENDPOINT) != 0;
				}
			}
		}
	}
	else
	{
		ERROR3("NULL node path ptr");	// Only complain a little in debug builds
	}

	return ok;
}

/********************************************************************************************

>	virtual void PathFlagsRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This is the base class record description generator.
				It doesn't do anything yet.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if XAR_TREE_DIALOG
void PathFlagsRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	char s[256];

	// Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	switch (pRecord->GetTag())
	{
		case TAG_PATH_FLAGS:
		{
			UINT32 NumFlags  = pRecord->GetSize()*sizeof(BYTE);

			BYTE Flags;
			for (UINT32 i=0;i < NumFlags;i++)
			{
				pRecord->ReadBYTE(&Flags);

				sprintf(s,"Flags\r\n"); (*pStr) += s;
				sprintf(s,"Smooth:\t%d\r\n",	(Flags & TAG_PATH_FLAGS_SMOOTH)   != 0); (*pStr) += s;
				sprintf(s,"Rotate:\t%d\r\n",	(Flags & TAG_PATH_FLAGS_ROTATE)    != 0); (*pStr) += s;
				sprintf(s,"EndPoint:\t%d\r\n",	(Flags & TAG_PATH_FLAGS_ENDPOINT) != 0); (*pStr) += s;
				sprintf(s,"\r\n"); (*pStr) += s;

			}
		}
		break;
	}
}
#endif
