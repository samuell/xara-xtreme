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

// Implementation of class NodeBrushMaker
#include "camtypes.h"
#include "ndbrshmk.h"
#include "attrmap.h"
//#include "progress.h"
#include "rndrgn.h"
#include "ndbrshpt.h"
#include "nodebrsh.h"
#include "docview.h"
#include "qualattr.h"
#include "fillattr.h"
#include "grndbmp.h"
//#include "camvw.h"
//#include "scrcamvw.h"
#include "grndrgn.h"
//#include "grndbrsh.h"
#include "quality.h"
#include "bitmap.h"
#include "quality.h"
#include "qualattr.h"

class QualityAttribute;

CC_IMPLEMENT_DYNCREATE(NodeBrushMaker, NodeRenderableInk)


#define DELPTR(p) if (p != NULL) { delete p; p = NULL; }


/***********************************************************************************************

>	NodeBrushMarker::NodeBrushMaker(Node* 	ContextNode,
						AttachNodeDirection Direction,
						const DocRect& 		BoundingRect,
						BOOL				Locked = FALSE,
						BOOL 				Mangled = FALSE,
						BOOL 				Marked = FALSE,
						BOOL 				Selected = FALSE,
						BOOL 				Renderable = FALSE
						)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:    6/10/99
    
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

	Note:		Initialise() must be called before the NodeBrushMaker is in a state in which it can be used.
    SeeAlso: 	Initialise()   
   	Errors: 	An assertion error will occur if ContextNode is NULL

***********************************************************************************************/

NodeBrushMaker::NodeBrushMaker(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ):NodeRenderableInk(ContextNode, Direction, Locked, Mangled, Marked, Selected )  
{                         
	ResetMembers();	
}

/*********************************************************************************************

>	NodeBrushMaker::NodeBrushMaker() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Purpose:	This constructor creates a NodeBrushMaker linked to no other with all status
				flags false and an uninitialized bounding rectangle.           
 	Note:		Initialise() must be called before the NodeBrushMaker is in a state in which it can be 
				used. 		
	SeeAlso:	Initialise()                                                        
 
**********************************************************************************************/
/* Technical Notes:   
	The default constructor is required so that SimpleCopy will work 
*/  

NodeBrushMaker::NodeBrushMaker(): NodeRenderableInk()
{
	ResetMembers();

}

/*********************************************************************************************

>	NodeBrushMaker::~NodeBrushMaker() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Purpose:	Default deconstructor
 
**********************************************************************************************/

NodeBrushMaker::~NodeBrushMaker()
{
	//delete m_pAttrMap;
	PORTNOTETRACE("other","NodeBrushMaker::~NodeBrushMaker - not deleting m_pGRender");
#ifndef EXCLUDE_FROM_XARALX
	DELPTR(m_pGRender);
#endif
	if (m_pBlendRef != NULL)
		DELPTR(m_pBlendRef);


}


/***********************************************************************************************

>	void NodeBrushMaker::ResetMembers()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	6/10/99
	Inputs:     -
	Returns     -
	Purpose:	Initialises the member variables

***********************************************************************************************/

void NodeBrushMaker::ResetMembers()
{
	m_BrushSpacing        = 0;
	m_BrushStep           = 0;
	m_MaxStep             = 0;
	m_CurrentDistance     = 0;
	
	m_PositionRandomness  = 0.0;
	m_AttrRandomness      = 0.0;
	m_SizeRandomness      = 0.0;
	m_AlignmentRandomness = 0.0;
	m_bRandomizeShape     = FALSE;
	m_JustTransformed     = FALSE;

	m_pBlendRef = NULL;
	m_pGRender = NULL;

	m_PathOffsetType       = OFFSET_NONE;
	m_PathOffsetValue      = 50000;

	m_bFirstRender		   = FALSE;

	m_bRotate              = FALSE;
	m_bTile                = TRUE;
	m_bUsePathCache        = TRUE;

	

}


/***********************************************************************************************

>	void NodeBrushMaker::InitialiseArray(UINT32 NumObjects)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	6/10/99
	Inputs:		Number of objects to size the array at
	Returns:	-
	Purpose:	Clears out the m_BrushRefPtrArray if it is not empty, and sets the size 

***********************************************************************************************/

void NodeBrushMaker::InitialiseBrushArray(UINT32 NumObjects)
{
	UINT32 i = 0;
	while (i < m_BrushRefPtrArray.size())
	{
		delete m_BrushRefPtrArray[i++];
	}
	m_BrushRefPtrArray.clear();

	m_BrushRefPtrArray.resize( NumObjects );

	// fill the array with NULL objects so that we can check later
	// to see if our allocations have worked
	i = 0;
	while (i < m_BrushRefPtrArray.size())
	{
		m_BrushRefPtrArray[i++] = NULL;
	}

}

/***********************************************************************************************

>	virtual Node* NodeBrushMaker::SimpleCopy()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	6/10/99
	Returns:	Pointer to a Node
	Purpose:	Makes a copy of all the data in the node

***********************************************************************************************/

Node* NodeBrushMaker::SimpleCopy()
{
	// Make a new NodeBrushMaker and then copy things into it
	NodeBrushMaker* pCopyOfNode = new NodeBrushMaker();
    if (pCopyOfNode)
    	CopyNodeContents(pCopyOfNode);
    
	return pCopyOfNode;
} 


/***********************************************************************************************

>	void NodeBrushMaker::CopyNodeContents(NodeBrushMaker* pCopyOfNode)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		pCopyOfNode - The node to copy data to
	Purpose:	Copies the data in this node to pCopyOfNode by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeRenderableInk::CopyNodeContents

***********************************************************************************************/

void NodeBrushMaker::CopyNodeContents(NodeBrushMaker* pCopyOfNode)
{
	NodeRenderableInk::CopyNodeContents(pCopyOfNode);
	
	// Copy contents specific to derived class here


}



/***********************************************************************************************
>   void NodeBrushMaker::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeBrushMaker::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeBrushMaker), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeBrushMaker))
		CopyNodeContents((NodeBrushMaker*)pNodeCopy);
}




/***********************************************************************************************

>	void NodeBrushMaker::ShowDebugTreeDetails() const

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Purpose:	Displays debugging info of the tree
	SeeAlso:	NodeRenderableInk::ShowDebugTreeDetails

***********************************************************************************************/
#ifdef _DEBUG
void NodeBrushMaker::ShowDebugTreeDetails() const
{                     
	// Display a bit of debugging info
	// For now, we will just call the base class version
	NodeRenderableInk::ShowDebugTreeDetails();	
}
#endif


/********************************************************************************************

>	void NodeBrushMaker::GetDebugDetails( StringBase* Str )

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node

********************************************************************************************/

void NodeBrushMaker::GetDebugDetails( StringBase* Str )
{
#ifdef _DEBUG
	// Call base class
	NodeRenderableInk::GetDebugDetails( Str );
	
	/*
	(*Str) += TEXT( "\r\nBrushMaker Data Dump\r\n" );

	char buf[500];
	sprintf(buf,		
						);

	*Str += buf;

	*Str += "--------- Start\r\n\r\n";
	GetDebugDetails(Str,m_pBlendRef);

	*/
#endif
}


/********************************************************************************************

>	void NodeBrushMaker::GetDebugDetails( StringBase* Str,BlendRef* pBlendRef )

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/95
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the blend reference

********************************************************************************************/

void NodeBrushMaker::GetDebugDetails( StringBase* Str, BlendRef* pBlendRef )
{
#ifdef _DEBUG
	String_256 TempStr;

	TempStr._MakeMsg( _T("Num blend paths = #1%d\r\n"), pBlendRef->GetNumBlendPaths() );
	*Str += TempStr;
	BlendPath* pBlendPath = pBlendRef->GetFirstBlendPath();
	while (pBlendPath != NULL)
	{
		Path*      pPath   = pBlendPath->GetPath();
		PathVerb*  pVerbs  = pPath->GetVerbArray();
		PathFlags* pFlags  = pPath->GetFlagArray();
		DocCoord*  pCoords = pPath->GetCoordArray();

		TempStr._MakeMsg( _T("Original Mapping = #1%d\r\n"), pBlendPath->GetOrigMapping() );
		*Str += TempStr;

		for (INT32 i=0; i<pPath->GetNumCoords(); i++)
		{
			// Add the info to the string
			TempStr._MakeMsg( TEXT("#1%d.\t#2%d\t#3%ld,\t#4%ld\t"),
							  i, pVerbs[i], pCoords[i].x, pCoords[i].y );
			(*Str) += TempStr;

			if (pFlags[i].IsEndPoint)
			{
				TempStr._MakeMsg( _T(": E\t") );
				(*Str) += TempStr;
			}

			*Str += _T("\r\n");
		}
		pBlendPath = pBlendRef->GetNextBlendPath(pBlendPath);
	}
#endif
}

      
    
/********************************************************************************************

>	virtual void NodeBrushMaker::Transform( TransformBase& Trans )

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		Trans - The transform Object
	Purpose:	Transforms all the paths attached to this blender node
	SeeAlso:	NodeRenderableInk::Transform()

********************************************************************************************/

void NodeBrushMaker::Transform( TransformBase& Trans )
{
	InvalidateBoundingRect();
	// transform the blendref object
	m_pBlendRef->Transform(Trans);

	// clear out the list of points to render
	m_ListOfPointsToRender.clear();
	
	// set the awful hacky flag
	m_JustTransformed = TRUE;
} 



/***********************************************************************************************

>	void NodeBrushMaker::Render(RenderRegion* pRender)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		Pointer to a render region
	Purpose:	Will render the blendpath objects on to the path of the brush

***********************************************************************************************/

void NodeBrushMaker::Render(RenderRegion* pRender)
{
	PORTNOTETRACE("other","NodeBrushMaker::Render - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// first check to see what we need to render
	DocRect RenderRect = pRender->GetClipRect();
	
	DocRect BrushRect = this->GetBoundingRect();

	DocRect TestRect = RenderRect.Intersection(BrushRect);

	// if there is no intersection we can just quit now
	
	if (!TestRect.IsIntersectedWith(RenderRect))
	{
		//TRACEUSER( "Diccon", _T("Brush not in render rect"));
		return;
	}
//	TRACEUSER( "Diccon", _T("\nBrushRect Top = %d, %d, Bot = %d, %d\n"), BrushRect.hix, BrushRect.hiy, BrushRect.lox, BrushRect.loy);

//	TRACEUSER( "Diccon", _T("\nRenderRect Top = %d, %d, Bot = %d, %d\n"), RenderRect.hix, RenderRect.hiy, RenderRect.lox, RenderRect.loy);

	// if we still have some points to render from last time then
	// go ahead and do those. if not then test to see if the
	// render region contains the brush, in which case we need all the
	// points, or if we just need to find out which points belong in the list
	
	if (m_ListOfPointsToRender.IsEmpty())
	{	
		//TRACEUSER( "Diccon", _T("Empty\n"));
		m_BrushStep = 0;
		if (RenderRect.ContainsRect(BrushRect))
			AddAllPointsToList();
		else
			AddPointsInRectToList(RenderRect);
	}

	// we only want to use the caching system once, so if we have just used it then
	// turn it off and tell the NBP we have stopped
	if (m_bUsePathCache == TRUE)
	{
		m_bUsePathCache = FALSE;
		m_pNodeBrushPath->StoppedBrush();
	}
	// we now have a list of points to render
	//TRACEUSER( "Diccon", _T("\nNum points = %d\n"), m_ListOfPointsToRender.GetCount());
	

	// set the quality level
	if (pRender->RRQuality.GetAntialiasQuality() != Quality::FullAntialias)
	{
		// Create a non-antialias quality attr to render
		Quality Qual;
		Qual.SetQuality(QUALITY_MAX);
		QualityAttribute *pQualAttrVal = new QualityAttribute(Qual);
		if (pQualAttrVal != NULL)
		{
			// Render the attribute, and mark it Temporary so it will be auto-deleted
			// when it is finished with.
			pRender->SetQuality(pQualAttrVal, TRUE);
		}
	}

	BOOL ContinueRender = TRUE;
	View* pView = pRender->GetRenderView();
	CPathPointInfo Point;
	m_AltValue = 0;
	m_bFirstRender = TRUE;
	while (ContinueRender && (m_ListOfPointsToRender.IsEmpty() == FALSE))
	{
		Point = m_ListOfPointsToRender.GetHead();
		m_ListOfPointsToRender.RemoveHead();
		RenderBrushObjectAtPoint(Point, pRender);
		
		// okay, so there was a problem in that when the node was transformed
		// it would not completely re-render itself because it kept timing out.
		// this is a hacky fix so that for the first render after the transformation
		// we force it to do the whole thing.
	//	if (!m_JustTransformed)
	//		ContinueRender = pView->CanContinue(pRender); 

	//	if (!ContinueRender)
	//		TRACEUSER( "Diccon", _T("TIMED OUT\n"); //"Continue = %d\n"), ContinueRender);
		if (m_AltValue == 0)
			m_AltValue = 1;
		else
			m_AltValue = 0;
		//m_bFirstRender = FALSE;
	}

	if (m_JustTransformed == TRUE)
		m_JustTransformed = FALSE;

	//TRACEUSER( "Diccon", _T("Finished render with %d objects in list\n"), m_ListOfPointsToRender.GetCount());
	
	if (m_ListOfPointsToRender.GetCount() == 0)
	{
		
		
	}
	else
	{
		
	}
	
	pRender->LockSubRenderState(FALSE);
#endif
}  





/********************************************************************************************

>	void NodeBrushMaker::RenderAttributes(RenderRegion* pRender, CCAttrMap* pAttrMap)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		pRender = ptr to render region
				pAttrMap - the attribute map to render
	Outputs:	-
	Returns:	-
	Purpose:	Renders the attributes to the given render region
	SeeAlso:	-

********************************************************************************************/

void NodeBrushMaker::RenderAttributes(RenderRegion* pRender, CCAttrMap* pAttrMap)
{
	PORTNOTETRACE("other","NodeBrushMaker::RenderAttributes - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Check entry params
	BOOL ok = (pRender != NULL && pAttrMap != NULL);
	ERROR3IF(!ok,"One or more NULL entry params");
	if (!ok) return;
	
	void* pType;
	void* pVal;
	NodeAttribute* pNodeAttr;
	// iterating all (key, value) pairs
	for (POSITION Pos = pAttrMap->GetStartPosition(); Pos != NULL;)
	{
		// Get attr at position Pos
		pAttrMap->GetNextAssoc(Pos,pType,pVal);

		// pVal is actually an attribute, so get a ptr to it and render it
		pNodeAttr = (NodeAttribute *)pVal;
		
		// we only need to render most of the attributes once
		if (m_bFirstRender == FALSE)
		{
			if (pNodeAttr->NeedsToRenderAtEachBrushStroke())
				pNodeAttr->Render(pRender);
		}
		else 
			pNodeAttr->Render(pRender);	
		
	}
#endif
}




/***********************************************************************************************

>	void NodeBrushMaker::RenderStep(INT32 StepNum, Spread* pSpread, DocCoord Point)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		StepNum - the step number to render
				pSpread - the current spread
				Point - the coordinate to render the step
	Purpose:	To draw the brush stroke to the screen as quickly as possible. This is 
				achieved by drawing the path and attributes to the render region, the blitting
				straight away.

***********************************************************************************************/

void NodeBrushMaker::RenderStep(INT32 StepNum, Spread* pSpread, CPathPointInfo Point)
{
	PORTNOTETRACE("other","NodeBrushMaker::RenderStep - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	//TRACEUSER( "Diccon", _T("Rendering Step %d\n"), StepNum);
	if (StepNum == 0)
		m_bFirstRender = TRUE;
	
	if (StepNum == 1)
		m_bFirstRender = FALSE;
	

	DocView* pView = DocView::GetSelected();
	DocRect ViewRect  =m_pGRenderDIB->GetClipRect();

	if (Point.m_Point.x > ViewRect.hix || Point.m_Point.x < ViewRect.lox)
		INT32 i = 1;
	if (Point.m_Point.y > ViewRect.hiy || Point.m_Point.y < ViewRect.loy)
		INT32 j = 1;

	if (pView == NULL)
	{
		ERROR3("No View!");
		return;
	}
	
	
	// declare variables to be used in loop
	DocRect BRect;
	DocRect ChangedRect;
	DocCoord CurrentPoint;
	Trans2DMatrix Trans;
	Trans2DMatrix Rotate;
	
	// get the head position of the offset list
	POSITION ListPosition; // = m_OffsetList.GetHeadPosition();
	BlendPathOffset ListOffset;  //m_OffsetList.GetNext(ListPosition);
	BlendPath* pBlendPath = m_pBlendRef->GetFirstBlendPath();

	CCAttrMap* pAttrMap = NULL;
	
	
	// pass alternating values into SetPosition so that it can do
	// alternating offsets
	UINT32 AltValue;
	if (StepNum % 2 == 0)
		AltValue = 0;
	else
		AltValue = 1;
	
	
	DocCoord CopyPoint;
	// the rendering loop
	//m_pGRenderDIB->StartRender();
	while (pBlendPath != NULL)
	{
		// get the correct transformation
		CopyPoint = SetPosition(Point,AltValue, &ListPosition);

		if (ListPosition != NULL)	
			ListOffset = m_OffsetList.GetNext(ListPosition);
		
		BRect = pBlendPath->m_pPath->GetBoundingRect();
		CurrentPoint = BRect.Centre();
		BRect.Translate((CopyPoint.x - CurrentPoint.x), (CopyPoint.y - CurrentPoint.y));
		Trans.SetTransform((CopyPoint.x - CurrentPoint.x), (CopyPoint.y - CurrentPoint.y));
		if (m_bRotate)
		{
			Rotate.SetTransform(CopyPoint, Point.m_Tangent);
			Trans.operator *=(Rotate);
		}
				
		// transform the points in the path
		Trans.Transform( (DocCoord*)pBlendPath->m_pPath->GetCoordArray(), pBlendPath->m_pPath->GetNumCoords());
		
		// get the attribute map
		pAttrMap = pBlendPath->FindAppliedAttributes();
		if (pAttrMap == NULL)
			ERROR3("No applied attributes");

		// transform the attribute map either to tile or not
		if (!m_bTile)
			pAttrMap->Transform(Trans);
		else
			pAttrMap->TransformForBrush(Trans);
		
		RenderAttributes(m_pGRenderDIB, pAttrMap);
		m_pGRenderDIB->DrawPath(pBlendPath->m_pPath);		
		

		////TRACEUSER( "Diccon", _T("Rendering %d at %d, %d\n"), StepNum, Point.x, Point.y);
		pBlendPath = m_pBlendRef->GetNextBlendPath(pBlendPath);
		
		ChangedRect = ChangedRect.Union(BRect);
		
	}
	//m_pGRenderDIB->StopRender();
	// set the changed bounding box for the render region so that when we come
	// to blit we only blit the pixels that have changed	
	m_pGRenderDIB->SetChangedBBox(ChangedRect);
//	m_pGRenderDIB->PlotBrushIntoBigBitmap(ChangedRect);
	m_pGRenderDIB->DrawToScreenNow();

	
//	//TRACEUSER( "Diccon", _T("Left render loop"));
#endif
} 

/********************************************************************************************

>	void NodeBrushMaker::RenderEorDrag( RenderRegion* pRender )

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		pRender - A Pointer to the current RenderRegion
	Purpose:	Renders a version of the blender node for EORed dragging of blends.
	SeeAlso:	NodePath::Render; NodeRenderableInk::RenderEorDrag

********************************************************************************************/

void NodeBrushMaker::RenderEorDrag( RenderRegion* pRender )
{
	
}



/********************************************************************************************

>	DocRect NodeBrushMaker::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		DontUseAttrs - TRUE if we should ignore the nodes attributes.
				Defaults to FALSE
				HitTest      - TRUE if being called during HitTest
	Returns:	The nodes bounding rect
	Purpose:	if the bounding rect is valid it is returned, if not, it is recalculated
				and then returned.
	SeeAlso:	NodeBrushMaker::GetBlobBoundingRect

********************************************************************************************/

DocRect NodeBrushMaker::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	DocRect Rect;
//	double Distance  = m_pNodeBrushPath->GetPathLength();
//	//TRACEUSER( "Diccon", _T("Path length %d\n"), Distance);
//	m_pNodeBrushPath->InvalidateBoundingRect();
	Rect = m_pNodeBrushPath->GetBoundingRect();
	Rect.Inflate(m_BrushBoundingRect.Width()/2, m_BrushBoundingRect.Height()/2);
	if (m_PathOffsetType != OFFSET_NONE)
		Rect.Inflate(m_PathOffsetValue, m_PathOffsetValue);
	return Rect;

}


/********************************************************************************************

>	DocRect NodeBrushMaker::GetBlobBoundingRect()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	DocRect - Returns the bounding rect of the brushmaker node and its blobs
	Purpose:

********************************************************************************************/

DocRect NodeBrushMaker::GetBlobBoundingRect()
{
	return (GetBoundingRect());
}




/********************************************************************************************

>	virtual UINT32 NodeBrushMaker::GetNodeSize() const

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	The size of the node in bytes 
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodeBrushMaker::GetNodeSize() const 
{     
	return (sizeof(NodeBrushMaker)); 
}  




/********************************************************************************************

>	BOOL NodeBrushMaker::SetBrushSpacing(double Spacing) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the spacing value to set
	Returns:	TRUE if successful, FALSE if the spacing is invalid 
	Purpose:	To set the spacing between brush objects
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBrushMaker::SetBrushSpacing(MILLIPOINT Spacing)
{
//	if (Spacing < MIN_BRUSH_SPACING || Spacing > MAX_BRUSH_SPACING)
//		return FALSE;
	
	m_BrushSpacing = Spacing;

	return TRUE;

}



/********************************************************************************************

>	MILLIPOINT NodeBrushMaker::GetBrushSpacing() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	the spacing between the brush objects 
	Purpose:	As above

********************************************************************************************/

MILLIPOINT NodeBrushMaker::GetBrushSpacing()
{
	return m_BrushSpacing;
}




/********************************************************************************************

>	PathOffset NodeBrushMaker::GetPathOffsetType() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	The type of pathoffset this brush is using 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

PathOffset NodeBrushMaker::GetPathOffsetType()
{
	return m_PathOffsetType;
}



/********************************************************************************************

>	void NodeBrushMaker::SetPathOffsetType(PathOffset Offset) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs		Offset - the offset type to set
	Returns:	- 
	Purpose:	to set the offset type for this brush
********************************************************************************************/
void NodeBrushMaker::SetPathOffsetType(PathOffset Offset)
{
	m_PathOffsetType = Offset;
}


/********************************************************************************************

>	MILLIPOINT NodeBrushMaker::GetPathOffsetValue() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Returns:	The value of the path offset used by this brush 
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

MILLIPOINT NodeBrushMaker::GetPathOffsetValue()
{
	return m_PathOffsetValue;
}



/********************************************************************************************

>	void NodeBrushMaker::SetPathOffsetValue(MILLIPOINT Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs		Value - the offset value to set
	Returns:	- 
	Purpose:	to set the offset distance for this brush
********************************************************************************************/
void NodeBrushMaker::SetPathOffsetValue(MILLIPOINT Value)
{
	m_PathOffsetValue = Value;
}



/********************************************************************************************

>	void NodeBrushMaker::SetNumberOfSteps(INT32 Steps) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the number of steps to set
	Returns:	-
	Purpose:	To set the number of steps
	SeeAlso:	-

********************************************************************************************/

void NodeBrushMaker::SetNumberOfSteps(INT32 Steps)
{
	m_MaxStep = Steps;

}


/********************************************************************************************

>	void NodeBrushMaker::SetRotated(BOOL Rotate) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Rotate - boolean value to set
	Returns:	-
	Purpose:	To set the rotate member
	SeeAlso:	-

********************************************************************************************/

void NodeBrushMaker::SetRotated(BOOL Rotate)
{
	m_bRotate = Rotate;
}


/********************************************************************************************

>	BOOL NodeBrushMaker::GetRotated() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		-
	Returns:	Current value of m_bRotate
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBrushMaker::GetRotated()
{
	return m_bRotate;
}



/********************************************************************************************

>	void NodeBrushMaker::SetUsePathCache(BOOL Value) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the value to set
	Returns:	- 
	Purpose:	To tell the nbm to ask the nbp to use its cached path processor to get 
				points along the path. This will speed up the first render after an alteration
				in the path.  However after the first rendering pass it should be set to false
				again because a) the points themselves will be cached and b)the cached processor
				will probably be deleted.
	SeeAlso:	-

********************************************************************************************/

void NodeBrushMaker::SetUsePathCache(BOOL Value)
{
	m_bUsePathCache = Value;

}

/********************************************************************************************

>	BOOL NodeBrushMaker::InitialiseInk(NodeRenderableInk* pInk, BlendRef* pBlendRef,Progress* pProgress ) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pInk - array of Ink nodes to make the brush objects from
				pBlendRef - blendref to make the brush from, default is  NULL
				pProgress - defaults to NULL
	Outputs:    -
	Returns:	TRUE if successful, otherwise FALSE
	Purpose:	To initialise the blendref, which creates the blendpaths that
				are placed along the NodeBrushPath to render the brush.  
				

********************************************************************************************/

BOOL NodeBrushMaker::InitialiseInk(NodeRenderableInk** ppInkNode, const UINT32 NumInkNodes, Progress* pProgress)
{
	ERROR2IF(*ppInkNode == NULL,FALSE,"pThisNode == NULL");
	ERROR2IF(NumInkNodes == 0, FALSE, "No ink nodes");	
	
	InitialiseBrushArray(NumInkNodes);
	
	UINT32 i = 0;
	while (i < NumInkNodes)
	{
		m_BrushRefPtrArray[i] = new BrushRef;
		ERROR2IF(m_BrushRefPtrArray[i] == NULL, FALSE, "Unable to allocate brushref object");
		if (!m_BrushRefPtrArray[i]->Initialise(ppInkNode[i]))
		{
			ERROR3("Failed to initialise brushref");
			InitialiseBrushArray(MAX_BRUSH_OBJECTS);
			return FALSE;
		}
		i++;
	}

	return TRUE;
}

#if 0 // not clear when all this was removed AB20060106
	m_pInkNode = *pInkNode;

	if (m_pBlendRef != NULL)
	{	
		delete m_pBlendRef;
		m_pBlendRef = NULL;
	}
	
	BOOL ok = TRUE;
	for (UINT32 i = 0; i < NumInkNodes; i++)
	{

		// allocate a new blendref
		m_pBlendRef = new BlendRef;
		if (ok) ok = (m_pBlendRef != NULL);
		if (ok)  ok = m_pBlendRef->Initialise(*pInkNode, -1, NULL, pProgress, FALSE, NULL);

		DocRect BRect = m_pBlendRef->GetBoundingRect();
		DocCoord InkCentre = BRect.Centre();

		m_OffsetList.RemoveAll();
	
		if (ok) 
		{
			// get the first blendpath as we need to pass it to the function
			// that retrieves the attributes
			BlendPath* pBlendPath = m_pBlendRef->GetFirstBlendPath();
			m_BrushBoundingRect = pBlendPath->m_pPath->GetBoundingRect();
			while (pBlendPath != NULL)
			{
				// copy the attributes of the blendref into our map
			/*	if (ok)
				{
					m_pAttrMap = m_pBlendRef->FindAppliedAttributes(pBlendPath);
					ok = (m_pAttrMap != NULL);
				}

				// find out how far each blendpath is offset from the centre of
				// the bounding box and record it.
				DocRect BlendPathRect = pBlendPath->m_pPath->GetBoundingRect();
				m_BrushBoundingRect = m_BrushBoundingRect.Union(BlendPathRect);
			
				DocCoord BlendPathCentre = BlendPathRect.Centre();
				BlendPathOffset ThisOffset;
				ThisOffset.m_XOffset = BlendPathCentre.x - InkCentre.x ;
				ThisOffset.m_YOffset = BlendPathCentre.y - InkCentre.y;
			//	TRACEUSER( "Diccon", _T("Offset = %d, %d\n"), ThisOffset.m_XOffset, ThisOffset.m_YOffset);
			//	m_OffsetList.AddTail(ThisOffset);
				pBlendPath = m_pBlendRef->GetNextBlendPath(pBlendPath);
			}
		}
	}
	
	POSITION ListPosition = m_OffsetList.GetHeadPosition();
	while (ListPosition != NULL)
	{
		BlendPathOffset Offset = m_OffsetList.GetNext(ListPosition);
		//TRACEUSER( "Diccon", _T("List offset %d, %d\n"), Offset.m_XOffset, Offset.m_YOffset);
	}
	

	return ok;

}
*/
#endif

/********************************************************************************************

>	void NodeBrushMaker::InitialisePath(NodeBrushPath* pBrushPath) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     NodeBrushPath
	Outputs:    -
	Returns:	TRUE if successful, otherwise FALSE
	Purpose:	To set the NodeBrushPath that the Brushmaker will use to render the brush 
				objects to

********************************************************************************************/

BOOL NodeBrushMaker::InitialisePath(NodeBrushPath* pNodeBrushPath)
{
	if (pNodeBrushPath == NULL)
		return FALSE;
	m_pNodeBrushPath = pNodeBrushPath;
	return TRUE;
}


/********************************************************************************************

>	NodeRenderableInk* NodeBrushMaker::GetInkNode() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Outputs:    -
	Returns:	TRUE if successful, FALSE if the start coordinate has not been set 
	Purpose:	To get the inknode used to make the brush
	SeeAlso:	-

********************************************************************************************/

NodeRenderableInk* NodeBrushMaker::GetInkNode()
{
	return m_pInkNode;
}


/********************************************************************************************

>	void NodeBrushMaker::RandomizeShape(Trans2DMatrix* pTransformMatrix) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Matrix containing current transformation
	Outputs:    matrix with added transformatio caused by randomization
	Returns:	-
	Purpose:	To add randomization to the blendpath to be rendered.  Transformations
				are calculated for the three different types of randomization possible
				and combined with the transformation matrix used to translate to the point
				along the nodebrushpath

********************************************************************************************/

void NodeBrushMaker::RandomizeShape(Trans2DMatrix* pTransMatrix)
{
	// err, not sure yet

}



/********************************************************************************************

>	void NodeBrushMaker::RandomizeAttributes() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Outputs:    -
	Returns:	-
	Purpose:	To add randomization to the attributes of the brush object

********************************************************************************************/

void NodeBrushMaker::RandomizeAttributes()
{

}



/********************************************************************************************

>	BOOL NodeBrushMaker::PrepareToRender() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/99
	Inputs:     pSpread - the current spread
	Outputs:    -
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets up the render region and renders the existing view into it.  Call this 
				before rendering any brush steps/

********************************************************************************************/

BOOL NodeBrushMaker::PrepareToRender(Spread* pSpread)
{
	PORTNOTETRACE("other","NodeBrushMaker::PrepareToRender - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	DocView* pView = DocView::GetSelected();

	if (pView == NULL)
	{
		ERROR3("No View!");
		return FALSE;
	}
	
	DocRect ViewRect = pView->GetDocViewRect(pSpread);

	if (!GetGRenderBitmap(&ViewRect, &m_pGRender, pSpread))
	{	
		ERROR3("Failed to get render region");
		return FALSE;
	}
	
	if (!DrawViewToRenderRegion(pSpread, m_pGRender))
	{
	//	//TRACEUSER( "Diccon", _T("Failed to draw view\n"));
		return FALSE;
	}


	m_pGRender->StartRender();
	return TRUE;
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	void NodeBrushMaker::FinishRender() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/99
	Inputs:     -
	Outputs:    -
	Returns:	-
	Purpose:	Informs the NBM that the 'creation' phase of rendering has finished.  Deletes
				the bitmap

********************************************************************************************/

void NodeBrushMaker::FinishRender()
{
	if (m_pGRenderDIB != NULL)
	{
	//	m_pGRender->StopRender();
	PORTNOTETRACE("other","NodeBrushMaker::FinishRender - not deleting m_pGRenderDIB");
#ifndef EXCLUDE_FROM_XARALX
		delete m_pGRenderDIB;
#endif
		m_pGRenderDIB = NULL;
	}
	m_bFirstRender = FALSE;
	
	if (m_pNodeBrushPath != NULL)
		m_pNodeBrushPath->StoppedBrush();

}

/********************************************************************************************

>	BOOL NodeBrushMaker::GetGRenderBitmap(DocRect* pRect, GRenderBitmap* pGRender) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pRect - the bounding rectangle of the render region we want to create
	Outputs:    pGRender - a new GRenderBitmap
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To return a GRenderDIB, if we don't already have one then create one, if we
				do then simply resize it to save time

********************************************************************************************/

BOOL NodeBrushMaker::GetGRenderBitmap(DocRect* pRenderRect, GRenderBrush** ppGRender, Spread* pSpread)
{
	PORTNOTETRACE("other","NodeBrushMaker::GetGRenderBitmap - do nothing");
#ifndef EXCLUDE_FROM_XARALX
// if it already points at something then delete it
	if (*ppGRender != NULL)
		delete *ppGRender;

	// now we need to make a new one

	// Lets setup a few variables need to create the new BMP
	View*	pView = View::GetCurrent();
	Matrix	Mat(1,0,0,1,0,0);
	FIXED16 Scale = 1;



	// Check to see if we`ve got an empty rect if so then return NULL!
	ERROR2IF(pRenderRect->IsEmpty(),FALSE,"We`ve got an empty DocRect");

	DocRect CopyRect = *pRenderRect;
	// Create a new GRenderBitmap region at 32BPP and 96 DPI
	GRenderBrush* pNewGRender = new GRenderBrush(CopyRect, Mat, Scale, 32, 96); // FALSE, 0, NULL, TRUE);

	// Check to see if the Render Region is valid!
	ERROR2IF(pNewGRender == NULL,FALSE,"Failed to create a GRenderBitmap in ");
	
	// Start to render into region!
	if(!pNewGRender->AttachDevice(pView, NULL, NULL))
	{
		ERROR3("Cannot attatch devices");
		
		delete pNewGRender;
		  pNewGRender = NULL;
		
		return FALSE;
	}
	

//	pNewGRender->m_DoCompression = TRUE;
	
	pNewGRender->InitAttributes();
	pNewGRender->RRQuality.SetQuality(QUALITY_MAX);
	pNewGRender->SetQualityLevel();
	pNewGRender->SetLineAttributes();
	pNewGRender->SetFillAttributes();
	pNewGRender->SetImmediateRender(TRUE);
	
	BOOL ok = pNewGRender->AttachDevice(pView, pView->GetRenderDC(), pSpread);
	if (ok)
	{
		pNewGRender->InitDevice();
		*ppGRender = pNewGRender;
		
	}
	return ok;
#endif
	return false;
}
/********************************************************************************************

>	BOOL NodeBrushMaker::DrawViewToRenderRegion(Spread* pSpread, RenderRegion* pRender) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pSpread - pointer to the current spread
				pRender - the region to render to
	Outputs:    -
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	To render whats currently in the tree onto the render region

********************************************************************************************/

BOOL NodeBrushMaker::DrawViewToRenderRegion(Spread* pSpread, RenderRegion* pRender)
{
	
	////TRACEUSER( "Diccon", _T("\nDrawing View\n"));
	if(pRender != NULL)
	{
		// get the view
		DocView* pDocView = DocView::GetCurrent();
		if (pDocView != FALSE)
		{
			
			// draw the view,	
			pDocView->ContinueRenderView(pRender,  pSpread, TRUE, FALSE);
			
			// render the brush so far
		
			Render(pRender);
		
		//	//TRACEUSER( "Diccon", _T("\nFinished Drawing View\n"));
			return TRUE;
		}
		else
			return FALSE;
	}
	else
		return FALSE;

}



/********************************************************************************************

>	BOOL NodeBrushMaker::GetStartCoord(DocCoord* Coord) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Outputs:    the starting coordinate of the brush
	Returns:	TRUE if successful, FALSE if the start coordinate has not been set 
	Purpose:	To get the starting coordinate of the brush
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBrushMaker::GetStartCoord(DocCoord* pCoord)
{
	*pCoord = m_StartCoord;

	return TRUE;
}



/********************************************************************************************

>	void NodeBrushMaker::AddPointsInRectToList(DocRect Rect) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Rect - the rect in which we look for brush objects
	Outputs:    -
	Returns:	- 
	Purpose:	To determine which of the brush objects intersect with Rect, these are then
				added to the list of points to render

********************************************************************************************/

void NodeBrushMaker::AddPointsInRectToList(DocRect Rect)
{
	
	//TRACEUSER( "Diccon", _T("\nAdding points in RECT\n"));
	
	MILLIPOINT DistanceToPoint = 0; 
	
	DocCoord TopRight;
	DocCoord PointToCheck;
	DocCoord BottomLeft;
	CPathPointInfo PathPoint;
	double Angle;
	BOOL ok;
	MILLIPOINT Width = m_BrushBoundingRect.Width() / 2;
	MILLIPOINT Height = m_BrushBoundingRect.Height() / 2;
	while (DistanceToPoint < (m_MaxStep * m_BrushSpacing))
	{
	
		ok = m_pNodeBrushPath->GetPointAtDistance(DistanceToPoint, &PointToCheck, &Angle, m_bUsePathCache );
		if (ok)
		{
			// we need to compare the bounding rect of a brush object centred at
			// PointToCheck.  m_BrushBoundingRect holds the correct dimensions of the brush
			
			
			TopRight.x = PointToCheck.x + Width;
			TopRight.y = PointToCheck.y + Height ;
			
			
			BottomLeft.x = PointToCheck.x - Width;
			BottomLeft.y = PointToCheck.y - Height;

			DocRect BrushRect(BottomLeft, TopRight);
			////TRACEUSER( "Diccon", _T("Point Rect  Top = %d, %d, bot = %d, %d\n"), BrushRect.hix, BrushRect.hiy, BrushRect.lox, BrushRect.loy);
			// is there an intersection? If so add this point to the list
			if (BrushRect.IsIntersectedWith(Rect))
			{
				PathPoint.m_Point = PointToCheck;
				PathPoint.m_Tangent = Angle;
				//TRACEUSER( "Diccon", _T("Adding point %d, %d\n"), PointToCheck.x, PointToCheck.y);
				m_ListOfPointsToRender.push_back( PathPoint );
			}
		}
		// get the next distance
		DistanceToPoint += m_BrushSpacing;
	}
}



/********************************************************************************************

>	void NodeBrushMaker::AddAllPointsToList() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Outputs:    -
	Returns:	- 
	Purpose:	Adds all the points in the brush to the list of things to be rendered

********************************************************************************************/

void NodeBrushMaker::AddAllPointsToList()
{
//	TRACEUSER( "Diccon", _T("\nAdding ALL points\n"));
	MILLIPOINT DistanceToPoint = 0;
	CPathPointInfo PathPoint;

	while (DistanceToPoint < (m_MaxStep * m_BrushSpacing))
	{
		double Angle;
		DocCoord Point; 
		BOOL ok = m_pNodeBrushPath->GetPointAtDistance(DistanceToPoint, &Point, &Angle, m_bUsePathCache);
		if (ok)
		{
			PathPoint.m_Point = Point;
			PathPoint.m_Tangent = Angle;
			m_ListOfPointsToRender.push_back( PathPoint );
		//	//TRACEUSER( "Diccon", _T("Adding point %d, %d\n"), Point.x, Point.y);
		}
		// get the next distance
		DistanceToPoint += m_BrushSpacing;
	}


}



/********************************************************************************************

>	void NodeBrushMaker::RenderBrushObjectAtPoint(CPathPointinfo Point, RenderRegion* pRender) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Point - co-ordinate where we wish to render the brush object
				pRender - the render region to render on to
	Outputs:    -
	Returns:	- 
	Purpose:	Renders a brush object at the point specified

********************************************************************************************/

void NodeBrushMaker::RenderBrushObjectAtPoint(CPathPointInfo Point,  RenderRegion* pRender)
{
	PORTNOTETRACE("other","NodeBrushMaker::RenderBrushObjectAtPoint - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	pRender->SaveContext();
	POSITION ListPosition = m_OffsetList.GetHeadPosition();
	
	Trans2DMatrix Trans;
	Trans2DMatrix Rotate;
	
	BlendPath* pBlendPath = m_pBlendRef->GetFirstBlendPath();
	CCAttrMap* pAttrMap = NULL;
	DocCoord CopyPoint;
	double RadToDeg = 1/ (2* PI);
	while (pBlendPath != NULL)
	{
		CopyPoint = SetPosition(Point, m_AltValue, &ListPosition);
		
		//TRACEUSER( "Diccon", _T("Rendering at  %d, %d\n"), Point.m_Point.x, Point.m_Point.y);
		
	

		// find out where the blendpath is at the moment, and transate it
		// to where we are now
		DocRect BRect = pBlendPath->m_pPath->GetBoundingRect();
		DocCoord CurrentPoint = BRect.Centre();
		Trans.SetTransform(CopyPoint.x  - CurrentPoint.x, CopyPoint.y - CurrentPoint.y );
		if (m_bRotate)
		{
			double RotAngle = 360 *(Point.m_Tangent * RadToDeg);
			Rotate.SetTransform(CopyPoint, RotAngle);

			//Trans.operator *=(Rotate);
		}
		// do randomisation if we need to
		if (m_bRandomizeShape)
			RandomizeShape(&Trans);
		
		if (m_AttrRandomness > 0)
			RandomizeAttributes();
		
		
		Trans.Transform( (DocCoord*)pBlendPath->m_pPath->GetCoordArray(), pBlendPath->m_pPath->GetNumCoords());
		Rotate.Transform( (DocCoord*)pBlendPath->m_pPath->GetCoordArray(), pBlendPath->m_pPath->GetNumCoords());
		pAttrMap = pBlendPath->FindAppliedAttributes();
		if (pAttrMap == NULL)
			ERROR3("No applied attributes");

		if (!m_bTile)
			pAttrMap->Transform(Trans);
		else
			pAttrMap->TransformForBrush(Trans);

		pAttrMap->Transform(Rotate);
		
		RenderAttributes(pRender, pAttrMap);
		pRender->DrawPath(pBlendPath->m_pPath);
		m_BrushStep++;
		//TRACEUSER( "Diccon", _T("Rendering Object %d at %d, %d\n"), m_BrushStep, Point.x, Point.y);
			
		pBlendPath = m_pBlendRef->GetNextBlendPath(pBlendPath);
	}
	pRender->RestoreContext();
#endif
}

/********************************************************************************************

>	DocCoord NodeBrushMaker::SetPosition(CPathPointInfo OriginalPoint, UINT32 AltValue,
										POSITION ListPosition) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     OriginalPoint - the point on the line at the correct distance for us to draw an object
				AltValue - if the offset it alternating then:
				0 = go right
				1 = go left
				ListPosition  - the current position in the offset CList
	Outputs:    POSITION is incremented.
	Returns:	the new position where we wish the object to be drawn 
	Purpose:	To determine where the next brush object should be drawn.  The position from the
				line can be altered by the following factors;
				
				 - m_OffsetList:  Mostly used for brushes made from compound nodes.  The list contains a
				translation for each BlendPath in m_Blendref showing how far it was from the centre of 
				the bounding box of the original object.

				- m_PathOffsetType and m_PathOffsetValue:  These translations map the point on one side of
				the path or the other, the value determines exactly how far from the path they will be.

				- m_PositionRandomness:  introduces a random element to the position of the object
	SeeAlso:	-

*********************************************************************/

DocCoord NodeBrushMaker::SetPosition(CPathPointInfo OriginalPoint, UINT32 AltValue, 
									 CListOfPointsToRender::iterator *ListPosition )
{
	BlendPathOffset PointOffset;

	if (*ListPosition != m_ListOfPointsToRender.end() )
	{
		//PointOffset = m_OffsetList.GetNext(*ListPosition);	
		OriginalPoint.m_Point.translate(PointOffset.m_XOffset, PointOffset.m_YOffset);
	}

	double theta;
	double NewX;
	double NewY;

	switch (m_PathOffsetType)
	{
		case OFFSET_NONE:
			break;
		case OFFSET_ALTERNATE:
			if (AltValue == 0)
				theta = OriginalPoint.m_Tangent - 90;
			else
				theta = OriginalPoint.m_Tangent + 90;
			
			NewX = m_PathOffsetValue * cos (theta);
			NewY = m_PathOffsetValue * sin (theta);
			OriginalPoint.m_Point.translate((INT32)NewX, (INT32)NewY);
			break;
		case OFFSET_RIGHT:
			theta = OriginalPoint.m_Tangent - 90;
			NewX = m_PathOffsetValue * cos (theta);
			NewY = m_PathOffsetValue * sin (theta);
			OriginalPoint.m_Point.translate((INT32)NewX, (INT32)NewY);
			break;
		case OFFSET_LEFT:
			theta = OriginalPoint.m_Tangent + 90;
			NewX = m_PathOffsetValue * cos (theta);
			NewY = m_PathOffsetValue * sin (theta);
			OriginalPoint.m_Point.translate((INT32)NewX, (INT32)NewY);
			break;
		case OFFSET_RANDOM:
			break;
		default:
			ERROR3("Unknown offset type");
			break;
	}

	return OriginalPoint.m_Point;
}









/********************************************************************************************

>	void NodeBrushMaker::SetStartCoord(DocCoord Coord) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     the starting coordinate of the brush
	Outputs:    -
	Returns:	- 
	Purpose:	To set the starting coordinate of the brush
	SeeAlso:	-

********************************************************************************************/

void NodeBrushMaker::SetStartCoord(DocCoord Coord)
{
	m_StartCoord = Coord;
}




/********************************************************************************************

>	void NodeBrushMaker::SetGRenderBitmap(GRenderBitmap* pGRenderBitmap) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pointer to the bitmap to set
	Outputs:    -
	Returns:	- 
	Purpose:	To set the pointer to the 'big' bitmap used to store the view
	SeeAlso:	-

********************************************************************************************/

void NodeBrushMaker::SetGRenderBitmap(GRenderBrush* pGRenderBitmap)
{
	m_pGRenderDIB = (GRenderBrush*)pGRenderBitmap;
}



/********************************************************************************************

>	void NodeBrushMaker::SetBlendRef(BlendRef* pBlendRef) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     pointer to the blendref
	Outputs:    -
	Returns:	- 
	Purpose:	To set the used to generate the brush objects
	SeeAlso:	-

********************************************************************************************/

void NodeBrushMaker::SetBlendRef(BlendRef* pBlendRef)
{
	if (pBlendRef == NULL)
	{
		ERROR3("Blend ref is NULL");
		return;
	}
	m_pBlendRef = pBlendRef;

	DocRect BRect = pBlendRef->GetBoundingRect();
	DocCoord InkCentre = BRect.Centre();

	m_OffsetList.clear();
	
//	if (ok) 
	{
		// get the first blendpath as we need to pass it to the function
		// that retrieves the attributes
		BlendPath* pBlendPath = m_pBlendRef->GetFirstBlendPath();
		if (pBlendPath == NULL)
		{
			ERROR3("First blendpath is NULL");
			return;
		}

		m_BrushBoundingRect = pBlendPath->m_pPath->GetBoundingRect();
		while (pBlendPath != NULL)
		{
			// find out how far each blendpath is offset from the centre of
			// the bounding box and record it.
			DocRect BlendPathRect = pBlendPath->m_pPath->GetBoundingRect();
			m_BrushBoundingRect = m_BrushBoundingRect.Union(BlendPathRect);
			
			DocCoord BlendPathCentre = BlendPathRect.Centre();
			BlendPathOffset ThisOffset;
			ThisOffset.m_XOffset = BlendPathCentre.x - InkCentre.x ;
			ThisOffset.m_YOffset = BlendPathCentre.y - InkCentre.y;
			//TRACEUSER( "Diccon", _T("Offset = %d, %d\n"), ThisOffset.m_XOffset, ThisOffset.m_YOffset);
			m_OffsetList.push_back( ThisOffset );
			pBlendPath = m_pBlendRef->GetNextBlendPath(pBlendPath);
		}
	}

	std::vector< BlendPathOffset >::iterator	ListPosition = m_OffsetList.begin();
	while (ListPosition != m_OffsetList.end())
	{
		BlendPathOffset Offset = *(ListPosition++);
		TRACEUSER( "Diccon", _T("List offset %d, %d\n"), Offset.m_XOffset, Offset.m_YOffset);
	}

}



/********************************************************************************************

>	void NodeBrushMaker::RecalculateMaxStep() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Outputs:    -
	Returns:	- 
	Purpose:	Recalculates the max step of the brush, call this when the path gets changed
********************************************************************************************/

void NodeBrushMaker::RecalculateMaxStep()
{	
	if (m_pNodeBrushPath != NULL)
	{
		INT32 NewMaxStep = (INT32)(m_pNodeBrushPath->GetPathLength() / m_BrushSpacing);
		if (NewMaxStep > 0)
		{
			m_MaxStep = NewMaxStep;
		}
		else
			ERROR3("Negative number of steps calculated");
	}
}




/********************************************************************************************

>	void NodeBrushMaker::ClearListOfPointsToRender() 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     -
	Outputs:    -
	Returns:	- 
	Purpose:	Clears out the cached list of points to render, ensures that all points will 
				be rendered on the next Render
********************************************************************************************/

void NodeBrushMaker::ClearListOfPointsToRender()
{
	m_ListOfPointsToRender.clear();
}
