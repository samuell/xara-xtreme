// $Id: ndbrshpt.cpp 662 2006-03-14 21:31:49Z alex $
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

// implementation of NodeBrushPath

#include "camtypes.h"
#include "ndbrshpt.h"
#include "pathproc.h"
#include "ndbrshmk.h"
#include "nodebrsh.h"
CC_IMPLEMENT_DYNCREATE(NodeBrushPath, NodeBlendPath);


/*********************************************************************************************

>    NodeBrushPath::NodeBrushPath() 

     Author:	Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:	6/10/99
     Purpose: 	This constructor creates a NodeBrushPath linked to no other, with all status
		  	  	flags false and an uninitialised bounding rectangle.           

**********************************************************************************************/
 
 
NodeBrushPath::NodeBrushPath(): NodeBlendPath()
{
	m_pCachedPointsAtDistance = NULL;
	// Reuse the "Destroy cache" call
	DestroyCachedInformation();
	m_bDrawingBrush = FALSE;
	m_pProcessPath = NULL;

}

/***********************************************************************************************

>	void NodeBrushPath::NodeBrushPath
	(
		Node* ContextNode,  
		AttachNodeDirection Direction,  
		BOOL Locked = FALSE, 
		BOOL Mangled = FALSE,  
		BOOL Marked = FALSE, 
		BOOL Selected = FALSE, 
	)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/10/99
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

NodeBrushPath::NodeBrushPath(Node* ContextNode,  
					 		 AttachNodeDirection Direction,  
					 		 BOOL Locked, 
				 	 		 BOOL Mangled,  
					 		 BOOL Marked, 
							 BOOL Selected   
			   ):NodeBlendPath(ContextNode, Direction, Locked, Mangled, Marked, Selected) 
{ 
	m_pCachedPointsAtDistance = NULL;
	// Reuse the "Destroy cache" call
	DestroyCachedInformation();
	m_bDrawingBrush = FALSE;
	m_pProcessPath = NULL;

} 

/*********************************************************************************************

>    NodeBrushPath::~NodeBrushPath() 

     Author:	Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:	6/10/99
     Purpose: 	Destructor for NodeBrushPath. 

**********************************************************************************************/
 
NodeBrushPath::~NodeBrushPath()
{
	// Reuse the "Destroy cache" call
	DestroyCachedInformation();

	if (m_pProcessPath != NULL)
	{
		delete m_pProcessPath;
		m_pProcessPath = NULL;
	}
}


/***********************************************************************************************

>	virtual Node* NodeBrushPath::SimpleCopy()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Returns:	Pointer to a Node
	Purpose:	Makes a copy of all the data in the node

***********************************************************************************************/

Node* NodeBrushPath::SimpleCopy()
{
	NodeBrushPath* NodeCopy = new NodeBrushPath();
    if (NodeCopy)
    	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}            


/***********************************************************************************************

>	void NodeBrushPath::CopyNodeContents(NodeBrushPath* NodeCopy)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:		NodeCopy - The node to copy
	Purpose:	Copies the data in the node by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeRenderableInk::CopyNodeContents

***********************************************************************************************/

void NodeBrushPath::CopyNodeContents( NodeBrushPath* NodeCopy)
{
	NodePath::CopyNodeContents( NodeCopy );
	//Copy contents specific to derived class here
}



/***********************************************************************************************
>   void NodeBrushPath::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeBrushPath::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeBrushPath), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeBrushPath))
		CopyNodeContents((NodeBrushPath*)pNodeCopy);
}



/********************************************************************************************

  >	virtual BOOL NodeBrushPath::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the nodebrushpath path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBrushPath::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = TRUE;
	/*
	INT32 NumCoords = InkPath.GetNumCoords();
	INT32 RecordSize = sizeof(INT32)+(sizeof(PathVerb)*NumCoords)+(sizeof(DocCoord)*NumCoords);

	CamelotFileRecord Rec(pFilter,TAG_BLEND_PATH,RecordSize);

	BOOL ok = Rec.Init();

	if (ok) ok = Rec.WritePath(&InkPath);
	if (ok) ok = pFilter->Write(&Rec);
	if (ok)
	{
		CamelotFileRecord FilledRec(pFilter, TAG_NODEBLENDPATH_FILLED, sizeof(INT32));
		
		ok = FilledRec.Init();
		if (ok)
		{
			INT32 Filled = InkPath.IsFilled;
			ok = FilledRec.WriteINT32(Filled);
			if (ok) ok = pFilter->Write(&FilledRec);
		}
	}
	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));
*/
	return ok;
}

/********************************************************************************************

  >	virtual BOOL NodeBrushPath::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the nodebrushpath path record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBrushPath::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	return WritePreChildrenWeb(pFilter);
}



/********************************************************************************************
>	void NodeBrushPath::SetPointAtDistance(MILLIPOINT Distance, DocCoord* pPoint)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		Distance = a length along the path
	Outputs:	pPoint = the coordinate of the point that distance along the path
	Returns:	-
	Purpose:	Sets the coordinate of a point a certain distance along this path
				in the cached points map
********************************************************************************************/

void NodeBrushPath::SetPointAtDistance(MILLIPOINT Distance, DocCoord Point)
{
	CPathPointInfo	PointInfo;

	if (m_pCachedPointsAtDistance == NULL)
	{
		m_pCachedPointsAtDistance = new CMap<MILLIPOINT,MILLIPOINT&,CPathPointInfo,CPathPointInfo&>;
//		if (m_pCachedPointsAtDistance != NULL)
//			m_pCachedPointsAtDistance->InitHashTable(1277);		// Init hash table size to a suitably large prime number
	}


	PointInfo.m_Point = Point;

	if (m_pCachedPointsAtDistance != NULL)
		m_pCachedPointsAtDistance->SetAt(Distance,PointInfo);

}



/********************************************************************************************
>	INT32 NodeBrushPath::GetNumberOfSteps()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		-
	Outputs:	-
	Returns:	the number of steps in the nodebrushpath, given the step distance
	Purpose:	as above
				
********************************************************************************************/

INT32 NodeBrushPath::GetNumberOfSteps()
{
	return m_NumSteps;
}



/********************************************************************************************
>	void NodeBrushPath::SetNumberOfSteps(INT32 Steps)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		Steps - the number to set
	Outputs:	-
	Returns:	-
	Purpose:	sets the number of steps in the nodeblendpath
				
********************************************************************************************/

void NodeBrushPath::SetNumberOfSteps(INT32 Steps)
{
	m_NumSteps = Steps;
}


/********************************************************************************************
>	void NodeBrushPath::StartedBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	informs the nodebrushpath that the brush is being drawn
				
********************************************************************************************/

void NodeBrushPath::StartedBrush()
{
//	ResetPathProcCache();
	m_bDrawingBrush = TRUE;
}



/********************************************************************************************
>	void NodeBrushPath::StoppedBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	informs the nodebrushpath that the brush has finished being drawn
				
********************************************************************************************/

void NodeBrushPath::StoppedBrush()
{
	m_bDrawingBrush = FALSE;
	ResetPathProcCache();
	if (m_pProcessPath != NULL)
	{
		delete m_pProcessPath;
		m_pProcessPath = NULL;
	}
}



/********************************************************************************************

>	DocRect NodeBrushPath::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/99
	Inputs:		DontUseAttrs - TRUE if you don't want to use the nodes attrs to calculate
				the bounding rect (defaults will be used). Defaults to FALSE.
				HitTest      - TRUE if being called during HitTest
	Returns:	bounding rectangle of the inkpath
	Purpose:	Returns the bounding rectangle of the coordinates of the path

********************************************************************************************/

DocRect NodeBrushPath::GetBoundingRect()
{
	//TRACEUSER( "Diccon", _T("Getting BRect, currently %d\n"), IsBoundingRectValid);
	return NodePath::GetBoundingRect();
}


/********************************************************************************************

>	void NodeBrushPath::AddPointToBoundingRect(DocCoord Point)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/99
	Inputs:		Point - coordinate to include in the bounding rect
	Returns:	-
	Purpose:	So that we can generate the bounding rect of the path as it is drawn 
				this function allows us to add a single point to the bounding rectangle

********************************************************************************************/

void NodeBrushPath::IncludePointInBoundingRect(DocCoord Point)
{
	BoundingRectangle.IncludePoint(Point);
}


/********************************************************************************************

>	void NodeBrushPath::Render( RenderRegion* pRender )

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/99
	Inputs:		pRender - A Pointer to the current RenderRegion
	Purpose:	If the brush is selected then the path is rendered, otherwise not

********************************************************************************************/

void NodeBrushPath::Render( RenderRegion* pRender )
{	
	pRender -> DrawPath( &InkPath );
}


/********************************************************************************************

>	void NodeBrushPath::RenderObjectBlobs(RenderRegion* pRender)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/99
	Inputs:		pRender - The region to render the blobs into
	Purpose:	Draws the actual path, so that it is only seen when selected

********************************************************************************************/

void NodeBrushPath::RenderObjectBlobs(RenderRegion* pRender)
{
	pRender -> DrawPath( &InkPath );
}


/********************************************************************************************

>	void NodeBrushPath::RenderTinyBlobs(RenderRegion* pRender)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/99
	Inputs:		pRender - The region to render the blobs into
	Purpose:	Draws the actual path, so that it is only seen when selected

********************************************************************************************/

void NodeBrushPath::RenderTinyBlobs(RenderRegion* pRender)
{
	//pRender -> DrawPath( &InkPath );
}


/********************************************************************************************

>	void NodeBrushPath::RenderEorDrag( RenderRegion* pRender )

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/99
	Inputs:		pRender - A Pointer to the current RenderRegion
	Purpose:	Renders a version of the path for EORed dragging of shapes.
	SeeAlso:	NodePath::Render; NodeRenderableInk::RenderEorDrag

********************************************************************************************/

void NodeBrushPath::RenderEorDrag( RenderRegion* pRender )
{
	pRender -> DrawPath( &InkPath );
}




/********************************************************************************************

>	virtual void NodeBrushPath::Transform( TransformBase& Trans )

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/99
	Inputs:		Trans - The transform to be applied to the path
	Purpose:	Will Transform all the coords in the path with the transform 
				provided
	SeeAlso:	NodeRenderableInk::Transform()

********************************************************************************************/

void NodeBrushPath::Transform(TransformBase &Trans) 
{
	// Change all the coords
	Trans.Transform( (DocCoord*)InkPath.GetCoordArray(), InkPath.GetNumCoords() );

	// and keep the bounding rectangle up to date.
	InvalidateBoundingRect();

	// Transform all the children...
	TransformChildren(Trans);

	// clear out the cache
	DestroyCachedInformation();	

	// as we are going to need to recalculate all the points try
	// to get the path processor back in action, don't worry if it fails though
	if (InitialisePathProcessor())
	{
		// we got it, so tell the NBM to ask for the cache
		Node* pParent = this->FindParent();
		if (pParent != NULL)
		{
			NodeBrushMaker* pMaker = (NodeBrushMaker*)pParent->FindFirstChild(CC_RUNTIME_CLASS(NodeBrushMaker));
			if (pMaker != NULL)
				pMaker->SetUsePathCache(FALSE);
		}
		else
		{
			delete m_pProcessPath;
			m_pProcessPath = NULL;
		}
	}
}

/********************************************************************************************
>	BOOL NodeBrushPath::GetPointAtDistance(MILLIPOINT Distance, DocCoord* pPoint, 
										   double* pTangent, BOOL UseCache = FALSE)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/99
	Inputs:		Distance = a length along the path
	Outputs:	pPoint = the coordinate of the point that distance along the path
				pTangent = tangent at this point (can be NULL)
				UseCache - whether or not to use the cached path processor (don't unless
				a) you're sure it hasn't been deleted and b) you only want consecutive points).

	Returns:	TRUE/FALSE for success/failure
	Purpose:	Gets the coordinate of a point a certain distance along this path
				Calls the underlying Path class function
				This is identical to the version in ndbldpth.cpp except that this uses
				the cached version of path::Getpointatdistance
********************************************************************************************/

BOOL NodeBrushPath::GetPointAtDistance(MILLIPOINT Distance, DocCoord* pPoint, 
									   double* pTangent, BOOL UseCache)
{
	ERROR2IF(pPoint == NULL,FALSE,"NULL pPoint param");

	BOOL Found = FALSE;


	CPathPointInfo	PointInfo;

	if (m_pCachedPointsAtDistance == NULL)
	{
		m_pCachedPointsAtDistance = new CMap<MILLIPOINT,MILLIPOINT&,CPathPointInfo,CPathPointInfo&>;
//		if (m_pCachedPointsAtDistance != NULL)
//			m_pCachedPointsAtDistance->InitHashTable(1277);		// Init hash table size to a suitably large prime number
	}

	if (m_pCachedPointsAtDistance != NULL)
	{
	
		Found = m_pCachedPointsAtDistance->Lookup(Distance,PointInfo);
		//if (Found)
	//	TRACEUSER( "Diccon", _T("Point %d, %d found in cache\n"), PointInfo.m_Point.x, PointInfo.m_Point.y);
	}

	if (!Found)
	{	

		// if we are drawing the brush then use the cached path processor, otherwise
		// use the normal one
		if (UseCache)
		{
	
			BOOL PointFound = FALSE;
			INT32 NumCoords;
			NumCoords = m_pProcessPath->GetCoordAndTangentWithCache( &PointInfo.m_Point, &PointInfo.m_Tangent, 
																	&PointFound, Distance ,&InkPath);
			if ((NumCoords != -1) && PointFound)
			{

				if (m_pCachedPointsAtDistance != NULL)
					m_pCachedPointsAtDistance->SetAt(Distance,PointInfo);
				Found = TRUE;	
			}	
		}
		else
		{
			if (InkPath.GetPointAtDistance(Distance,&PointInfo.m_Point,&PointInfo.m_Tangent))
			{
				if (m_pCachedPointsAtDistance != NULL)
					m_pCachedPointsAtDistance->SetAt(Distance,PointInfo);
		//		TRACEUSER( "Diccon", _T("Point %d, %d found from line\n"), PointInfo.m_Point.x, PointInfo.m_Point.y);
				Found = TRUE;
			}
		}
	}

	if (Found)
	{
		*pPoint = PointInfo.m_Point;
		if (pTangent)
			*pTangent = PointInfo.m_Tangent;
	}


//	if (Found == FALSE)
	//	TRACEUSER( "Diccon", _T("Not found at distance %d\n"), Distance);
	return Found;
}


/********************************************************************************************

>	 void NodeBrushPath::ResetPathProcCache()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/99
	Inputs:		-
	Purpose:	To reset the cache used by the inkpath member, call this if you are going to 
				alter the path in any way


********************************************************************************************/

void NodeBrushPath::ResetPathProcCache()
{
	//InkPath.ResetCoordsProcessed();

}


/********************************************************************************************

>	BOOL NodeBrushPath::InitialisePathProcessor()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/99
	Inputs:		-
	Purpose:	To create a new path processor object
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBrushPath::InitialisePathProcessor()
{
	if (m_pProcessPath != NULL)
	{
		delete m_pProcessPath;
		m_pProcessPath = NULL;
	}

	//m_pProcessPath = new ProcessPathDistance(64);

	//ERROR2IF(m_pProcessPath == NULL, FALSE, "Unable to allocate ProcessPathDistance object");

	return TRUE;
}
	

		
