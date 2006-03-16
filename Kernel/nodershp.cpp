// $Id: nodershp.cpp 662 2006-03-14 21:31:49Z alex $
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
#include "nodershp.h"

// Code headers
#include "app.h"
#include "becomea.h"
#include "blobs.h"
//#include "cameleps.h"
//#include "nativeps.h"		// The old style EPS native filter, used in v1.1
//#include "swfrndr.h"		// For the Flash render region class.
//#include "contmenu.h"
#include "docview.h"
#include "fillval.h"
#include "nodepath.h"
//#include "pathedit.h"
#include "ophist.h"
//#include "resource.h"
#include "rndrgn.h"
#include "shapeops.h"
#include "snap.h"
#include "tool.h"
#include "tranform.h"
#include "trans2d.h"

// Headers for the new native format.
#include "cxfdefs.h"
#include "cxfrgshp.h"

// Resource headers.
//#include "mario.h"	 
//#include "peter.h"
//#include "rik.h"
//#include "simon.h"

// For ImagemapRenderRegion.
#include "filtirr.h"

#include "extender.h"

// for the bounding box calculation.
#include "attrmap.h"

// for the variable width path fn.
#include "strkattr.h"
#include "ppstroke.h"
#include "gclip.h"
#include "gclips.h"
#include "rsmooth.h"
#include "brshattr.h"


DECLARE_SOURCE( "$Revision: 662 $" );


CC_IMPLEMENT_DYNCREATE(NodeRegularShape, NodeRenderableInk)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

static const double CurveFactor = 0.552;

/***********************************************************************************************

>	NodeRegularShape::NodeRegularShape(Node* 	ContextNode,
						AttachNodeDirection Direction,
						const DocRect& 		BoundingRect,
						BOOL				Locked = FALSE,
						BOOL 				Mangled = FALSE,
						BOOL 				Marked = FALSE,
						BOOL 				Selected = FALSE,
						BOOL 				Renderable = FALSE
						)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
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
NodeRegularShape::NodeRegularShape(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ) : NodeRenderableInk(ContextNode, Direction, Locked, Mangled, Marked, Selected )
{                         
	InitialiseMemberVars();
}                        
 


/*********************************************************************************************

>	NodeRegularShape::NodeRegularShape() 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Purpose:	This constructor creates a NodeRegularShape linked to no other.
	Note:		SetUpShape() should be called to change the NodeRegularShape before use!
	SeeAlso:	NodeRegularShape::SetUpShape                                                        

**********************************************************************************************/
NodeRegularShape::NodeRegularShape() : NodeRenderableInk()
{
	InitialiseMemberVars();
}



/*********************************************************************************************

>	void NodeRegularShape::InitialiseMemberVars()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/96
	Inputs:		-
	Outputs:	Initalised member variables
	Returns:	-
	Purpose:	One consistant place to initialise all the member variables of a NodeRegularShape

**********************************************************************************************/
void NodeRegularShape::InitialiseMemberVars()
{
	Circular = FALSE;	
	Stellated = FALSE;	
	PrimaryCurvature = FALSE;	
	StellationCurvature = FALSE;
	NumSides = 6;			
	StellRadiusToPrimary = 0.5;
	PrimaryCurveToPrimary = 0.2;
	StellCurveToStell = 0.2;
	StellOffsetRatio = 0.0;
	CachedRenderPath = NULL;
	PathCacheInvalid = TRUE;
}



/*********************************************************************************************

>	NodeRegularShape::~NodeRegularShape() 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Purpose:	Distructor to delete the cached render path
	SeeAlso:	-

**********************************************************************************************/
NodeRegularShape::~NodeRegularShape()
{
	DeleteCachedPath();
}



/***********************************************************************************************

>	virtual Node* NodeRegularShape::SimpleCopy()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Returns:	Pointer to a Node or NULL if there was not enough memory for the new node
	Purpose:	Makes a copy of all the data in the node

***********************************************************************************************/

Node* NodeRegularShape::SimpleCopy()
{
	// Make a new NodeRegularShape and then copy things into it
	NodeRegularShape* NodeCopy = new NodeRegularShape();
    if (NodeCopy != NULL)
    	CopyNodeContents(NodeCopy);

	return NodeCopy;
}            



/***********************************************************************************************

>	void NodeRegularShape::CopyNodeContents(NodeRegularShape* NodeCopy)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		NodeCopy - The node to copy the data into
	Purpose:	Copies the data in the node by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
				Note : Copies FROM this TO NodeCopy
	SeeAlso:	NodeRenderableInk::CopyNodeContents
	Errors:		ERROR3 if passed a NULL pointer.

***********************************************************************************************/

void NodeRegularShape::CopyNodeContents(NodeRegularShape* NodeCopy)
{
	ERROR3IF(NodeCopy == NULL,"NodeRegularShape::CopyNodeContents was asked to copy into a NULL pointer");

	// If the dest shape has any cached object pointers, delete them before copying over
	delete NodeCopy->CachedRenderPath;
	
	// Copy from the base class
	NodeRenderableInk::CopyNodeContents(NodeCopy);
	
	//Copy contents specific to derived class here
	NodeCopy->NumSides = NumSides;			    
	NodeCopy->Circular = Circular;			    
	NodeCopy->Stellated = Stellated;			
	NodeCopy->PrimaryCurvature = PrimaryCurvature;	    
	NodeCopy->StellationCurvature = StellationCurvature;	
	NodeCopy->StellRadiusToPrimary = StellRadiusToPrimary;	
	NodeCopy->PrimaryCurveToPrimary = PrimaryCurveToPrimary;	
	NodeCopy->StellCurveToStell = StellCurveToStell;    
	NodeCopy->StellOffsetRatio = StellOffsetRatio;
	NodeCopy->UTCentrePoint = UTCentrePoint;
	NodeCopy->UTMajorAxes = UTMajorAxes;
	NodeCopy->UTMinorAxes = UTMinorAxes;
	NodeCopy->CachedRenderPath = NULL;
	NodeCopy->PathCacheInvalid = TRUE;
	NodeCopy->TransformMatrix = TransformMatrix;

	// When copying the paths we could run out of memory (so?)
	if (NodeCopy->EdgePath1.Initialise(EdgePath1.GetNumCoords(),1) 
		&& NodeCopy->EdgePath2.Initialise(EdgePath2.GetNumCoords(),1) )
	{
		NodeCopy->EdgePath1.CopyPathDataFrom(&EdgePath1);
		NodeCopy->EdgePath2.CopyPathDataFrom(&EdgePath2);
	}
}



/***********************************************************************************************
>   void NodeRegularShape::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeRegularShape::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeRegularShape), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeRegularShape))
		CopyNodeContents((NodeRegularShape*)pNodeCopy);
}



/***********************************************************************************************

>	void NodeRegularShape::ShowDebugTreeDetails() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Purpose:	Displays debugging info of the tree
	SeeAlso:	NodeRenderableInk::ShowDebugTreeDetails

***********************************************************************************************/
#ifdef _DEBUG
void NodeRegularShape::ShowDebugTreeDetails() const
{                     
	// Display a bit of debugging info
	// For now, we will just call the base class version
	TRACEALL( _T("NodeRegularShape  ") );
	NodeRenderableInk::ShowDebugTreeDetails();	
}
#endif



/********************************************************************************************

>	void NodeRegularShape::GetDebugDetails( StringBase* Str )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node (for

********************************************************************************************/

void NodeRegularShape::GetDebugDetails( StringBase* Str )
{
#if DEBUG_TREE
	// Call base class
	NodeRenderableInk::GetDebugDetails( Str );
	
	String_256 TempStr;
	String_256 TempStr2;
		
	(*Str) += TEXT( "\r\nRegular Path Data Dump\r\n" );

	DocRect BlobRect = GetBlobBoundingRect();
	TempStr._MakeMsg( TEXT("Blob Bounding Rect :\r\n\t#1%ld,\t#2%ld\r\n\t#3%ld,\t#4%ld\r\n"),
					  BlobRect.lo.x, BlobRect.lo.y, BlobRect.hi.x, BlobRect.hi.y );
	(*Str) += TempStr;

	TempStr._MakeMsg( TEXT("#1%d sides\r\n"),NumSides);
	(*Str) += TempStr;

	TempStr._MakeMsg( TEXT("Set Flags:"));
	if (Circular)
	{
		TempStr2._MakeMsg( TEXT(" Circular"));
		TempStr += TempStr2;
	}
	if (Stellated)
	{
		TempStr2._MakeMsg( TEXT(" Stellated"));
		TempStr += TempStr2;
	}
	if (PrimaryCurvature)
	{
		TempStr2._MakeMsg( TEXT(" PrimaryCurvature"));
		TempStr += TempStr2;
	}
	if (StellationCurvature)
	{
		TempStr2._MakeMsg( TEXT(" StellationCurvature"));
		TempStr += TempStr2;
	}
	if (!(Circular || Stellated || PrimaryCurvature	|| StellationCurvature))
	{
		TempStr2._MakeMsg( TEXT(" None! "));
		TempStr += TempStr2;
	}
	TempStr2._MakeMsg( TEXT("\r\n\r\n"));
	TempStr += TempStr2;
	(*Str) += TempStr;

	TCHAR				floatStr[20];
	tsprintf( floatStr, 20, _T("%f"), StellRadiusToPrimary );
	TempStr._MakeMsg( TEXT("Stellation Radius :\t#1%s\r\n"), floatStr);
	(*Str) += TempStr;
	tsprintf( floatStr, 20, _T("%f"), StellOffsetRatio );
	TempStr._MakeMsg( TEXT("Stellation Offset :\t#1%s\r\n\r\n"), floatStr);
	(*Str) += TempStr;
	tsprintf( floatStr, 20, _T("%f"), PrimaryCurveToPrimary);
	TempStr._MakeMsg( TEXT("Primary Curvature :\t#1%s\r\n"), floatStr);
	(*Str) += TempStr;
	tsprintf( floatStr, 20, _T("%f"), StellCurveToStell);
	TempStr._MakeMsg( TEXT("Stellation Curvature :\t#1%s\r\n"), floatStr);
	(*Str) += TempStr;

	TempStr._MakeMsg( TEXT("\r\nUntransformed"));
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("Centre Point :\t#1%ld,\t#2%ld\r\n"), UTCentrePoint.x, UTCentrePoint.y);
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("Major axes :\t#1%ld,\t#2%ld\r\n"), UTMajorAxes.x, UTMajorAxes.y);
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("Minor axes :\t#1%ld,\t#2%ld\r\n"), UTMinorAxes.x, UTMinorAxes.y);
	(*Str) += TempStr;
	
	TempStr._MakeMsg( TEXT("\r\nTransformed"));
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("Centre Point :\t#1%ld,\t#2%ld\r\n"), GetCentrePoint().x, GetCentrePoint().y);
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("Major axes :\t#1%ld,\t#2%ld\r\n"), GetMajorAxes().x, GetMajorAxes().y);
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("Minor axes :\t#1%ld,\t#2%ld\r\n"), GetMinorAxes().x, GetMinorAxes().y);
	(*Str) += TempStr;

	fixed16 billy[4];
	INT32	bobby[2];
	TransformMatrix.GetComponents(billy, bobby);

	TempStr._MakeMsg( TEXT("\r\nMatrix\r\n"));	 
	(*Str) += TempStr;
	tsprintf( floatStr, 20, _T("%f,%f"), billy[0].MakeDouble(), billy[1].MakeDouble());
	TempStr._MakeMsg( TEXT("a, b :\t#1%s\r\n"), floatStr);
	(*Str) += TempStr;
	tsprintf( floatStr, 20, _T("%f,%f"), billy[2].MakeDouble(), billy[3].MakeDouble());
	TempStr._MakeMsg( TEXT("c, d :\t#1%s\r\n"), floatStr);
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("e, f :\t#1%ld,\t#2%ld\r\n"), bobby[0], bobby[1]);
	(*Str) += TempStr;

	(*Str) += TEXT( "\r\nPath 1\r\nNum\tType\tX Coord\tY Coord\r\n" );
	PathVerb* Verbs  = EdgePath1.GetVerbArray();
	DocCoord* Coords = EdgePath1.GetCoordArray();
	PathFlags* Flags = EdgePath1.GetFlagArray();
	INT32					i;
	for( i = 0; i<EdgePath1.GetNumCoords(); i++)
	{
		// Add the info to the string
		TempStr._MakeMsg( TEXT("#1%d.\t#2%d\t#3%ld,\t#4%ld\r\n"),
						  i, Verbs[i], Coords[i].x, Coords[i].y );
		(*Str) += TempStr;
	}

	(*Str) += TEXT( "\r\nPath 2\r\nNum\tType\tX Coord\tY Coord\r\n" );
	Verbs  = EdgePath2.GetVerbArray();
	Coords = EdgePath2.GetCoordArray();
	Flags = EdgePath2.GetFlagArray();
	for (i=0; i<EdgePath2.GetNumCoords(); i++)
	{
		// Add the info to the string
		TempStr._MakeMsg( TEXT("#1%d.\t#2%d\t#3%ld,\t#4%ld\r\n"),
						  i, Verbs[i], Coords[i].x, Coords[i].y );
		(*Str) += TempStr;
	}

	Path* RenderPath = NULL;
	if (BuildShapePath(&RenderPath))
	{
		// Now do the complete path shape
		(*Str) += TEXT( "\r\nRender path\r\nNum\tType\tX Coord\tY Coord\r\n" );
		Verbs  = RenderPath->GetVerbArray();
		Coords = RenderPath->GetCoordArray();
		Flags = RenderPath->GetFlagArray();
		for (i=0; i<RenderPath->GetNumCoords(); i++)
		{
			// Add the info to the string
			TempStr._MakeMsg( TEXT("#1%d.\t#2%d\t#3%ld,\t#4%ld\r\n"),
							  i, Verbs[i], Coords[i].x, Coords[i].y );
			(*Str) += TempStr;
		}
	}
	else
	{
		TempStr._MakeMsg( TEXT("Failed to build render path") );
		(*Str) += TempStr;
	}
#endif
}



/***********************************************************************************************

>	BOOL NodeRegularShape::SetUpShape()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		- 
	Returns:	TRUE if the shape was init'ed ok, FALSE otherwise
	Purpose:	To initialise the paths used by the shape into a state that can be used,
				by allocating memory, setting up member variables properly.

***********************************************************************************************/

BOOL NodeRegularShape::SetUpShape()
{
	if (EdgePath1.Initialise(4, 1) && EdgePath2.Initialise(4, 1))
	{
		DocCoord Fred1(0,0);
		DocCoord Fred2(72000,0);
		EdgePath1.SetPathPosition(0);
		EdgePath1.InsertMoveTo(Fred1);
		EdgePath1.InsertLineTo(Fred2);
		EdgePath2.SetPathPosition(0);
		EdgePath2.InsertMoveTo(Fred1);
		EdgePath2.InsertLineTo(Fred2);
		return TRUE;
	}
	else
		return FALSE;
}



/********************************************************************************************

>	virtual void NodeRegularShape::Transform( TransformBase& Trans )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		Trans - The transform Object
	Purpose:	Transforms the shape.
	SeeAlso:	NodeRenderableInk::Transform()

********************************************************************************************/
void NodeRegularShape::Transform( TransformBase& Trans )
{
	if (IS_A(&Trans, Trans2DMatrix))
	{
		TransformMatrix *= ((Trans2DMatrix&)Trans).GetMatrix();
	}
	else
	{
 		// Transform the Shape
		Trans.Transform(&UTCentrePoint, 1);
		Trans.Transform(&UTMajorAxes, 1);
		Trans.Transform(&UTMinorAxes, 1);
	}

	EmergencyFixShape();

	// Mark the bounding rect as invalid							
	InvalidateBoundingRect();
	InvalidateCache();

	DocRect dr = GetBoundingRect();
	
	// Transform all the children...
	TransformChildren(Trans);

	InvalidateBoundingRect();
	InvalidateCache();
	dr = GetBoundingRect();
}



/***********************************************************************************************

>	BOOL NodeRegularShape::BuildShapePath(Path** RenderPath)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		RenderPath - Pointer to an pointer to a Path object.
	Outputs:	RenderPath will point to a path that describes the whole regular shape.  This can
				then be used to render the shape or EOR its outline.
	Returns:	FALSE if the path could not be created (eg out of memory).  TRUE if RenderPath
				points to a valid path.
	Purpose:	Will build up a path representing the path from the edge paths stored in the
				NodeRegularShape object.
				NOTES: This rountine needs to be as quick as possible as it is called all the
				time anyone does anything involving the path.  It may be necessary to hold
				the render path in the shape and only regenerate it when required.
	SeeAlso:	RegShape.doc
	Errors:		SetError will have been called if FALSE is returned.

***********************************************************************************************/

BOOL NodeRegularShape::BuildShapePath(Path** RenderPath)
{ 
	if ((CachedRenderPath == NULL) || PathCacheInvalid)
	{
		DeleteCachedPath();

		CachedRenderPath = new (Path);
		if (CachedRenderPath == NULL)
			return FALSE;

		BOOL Success = TRUE;
	
		if (IsCircular())
		{
			Success = BuildEllipsePath(CachedRenderPath);
		}
		else
		{
			DocCoord*	pArray;
			Success = BuildPolygonPoints(&pArray);
			if (Success)
				Success = BuildPolygonPath(CachedRenderPath, pArray);
			if (pArray != NULL)
				delete [] pArray;
		}

		if (Success)
		{
			DocCoord* Coords = CachedRenderPath->GetCoordArray();
			TransformMatrix.transform((Coord*)Coords, CachedRenderPath->GetNumCoords()); 

			*RenderPath = CachedRenderPath;
			PathCacheInvalid = FALSE;
			return TRUE;
		}
		else
		{
			DeleteCachedPath();
			return FALSE;
		}
	}
	else
	{
		*RenderPath = CachedRenderPath;
		return TRUE;
	}
}



/********************************************************************************************

>	void NodeRegularShape::Render ( RenderRegion* pRender )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/00
	Inputs:		pRender	- Pointer to a render region.
	Purpose:	Creates a path from the renderable shape's characterisitics, which it passes
				to the DrawPath method of the render region that it's exporting to.

********************************************************************************************/
void NodeRegularShape::Render ( RenderRegion* pRender )
{
//#pragma message( __LOCMSG__ "NodeRegularShape::Render - do nothing" )
//	TRACE( _T("Warning - NodeRegularShape::Render called\n") );
	Path* pRenderPath = NULL;

	// Attempt to build a shape path.
	if ( BuildShapePath ( &pRenderPath ) )
	{
		// Write the path out to the file.
		pRender->DrawPath ( pRenderPath, NULL, GetPathShape () );
	}
}  



/********************************************************************************************

>	void NodeRegularShape::RenderEorDrag( RenderRegion* pRender )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		pRender - A Pointer to the current RenderRegion
	Purpose:	Renders a version of the shape for EORed dragging.
	SeeAlso:	NodeRenderableInk::RenderEorDrag

********************************************************************************************/
void NodeRegularShape::RenderEorDrag( RenderRegion* pRender )
{
//#pragma message( __LOCMSG__ "NodeRegularShape::RenderEorDrag - do nothing" )
//	TRACE( _T("Warning - NodeRegularShape::RenderEorDrag called\n") );
	Path* RenderPath = NULL;
	
	if (BuildShapePath(&RenderPath))
		pRender->DrawPath(RenderPath);
}



/********************************************************************************************

>	void NodeRegularShape::RenderObjectBlobs(RenderRegion* pRender)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		pRender - the region to render the blobs to
	Purpose:	Renders the Object blobs for a NodeRegularShape
	SeeAlso:	BlobManager

********************************************************************************************/
void NodeRegularShape::RenderObjectBlobs(RenderRegion* pRender)
{
	pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// Draw a blob at the centre point
	DocRect BlobSize;
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr != NULL)
	{
		DocCoord Point = GetCentrePoint();
		pBlobMgr->GetBlobRect(Point, &BlobSize);

		pRender->DrawLine(DocCoord(BlobSize.hi.x, BlobSize.hi.y), DocCoord(BlobSize.lo.x, BlobSize.lo.y));
		pRender->DrawLine(DocCoord(BlobSize.lo.x, BlobSize.hi.y), DocCoord(BlobSize.hi.x, BlobSize.lo.y));
		pRender->DrawPixel(DocCoord(BlobSize.hi.x, BlobSize.lo.y));
		pRender->DrawPixel(DocCoord(BlobSize.lo.x, BlobSize.lo.y));
	}

	pRender->SetLineColour(COLOUR_NONE);
	// Now draw blobs at the primary and stellation points, and their curvature points.
	if (!IsCircular())
	{
		DocCoord*	Array;
		INT32 Points = 0;
		BOOL Success = BuildPolygonPoints(&Array, &Points);
		UINT32 Counter = 0;
		if (Success)
		{
			TransformMatrix.transform((Coord*)Array, Points);
			for (UINT32 loop = 0; loop < NumSides; loop++)
			{
				// Skip along to the stellation information
				Counter ++;
				if (IsPrimaryCurvature())
					Counter ++;

				if (IsStellated())
				{
					if (IsStellationCurvature())
					{
						pRender->DrawBlob(Array[Counter], BT_UNSELECTED);
						pRender->DrawBlob(Array[Counter+1], BT_UNSELECTED);
						pRender->DrawBlob(Array[Counter+2], BT_UNSELECTED);
						pRender->SetLineColour(COLOUR_BEZIERLINE);
						pRender->DrawLine(Array[Counter], Array[Counter+1]);
						pRender->DrawLine(Array[Counter+1], Array[Counter+2]);
						pRender->SetLineColour(COLOUR_NONE);
						Counter += 3;
					}			
					else
						pRender->DrawBlob(Array[Counter++], BT_UNSELECTED);
				}

				if (IsPrimaryCurvature())
				{
					pRender->DrawBlob(Array[Counter], BT_UNSELECTED);
					pRender->DrawBlob(Array[Counter+1], BT_UNSELECTED);
					pRender->DrawBlob(Array[Counter+2], BT_UNSELECTED);
					pRender->SetLineColour(COLOUR_BEZIERLINE);
					pRender->DrawLine(Array[Counter], Array[Counter+1]);
					pRender->DrawLine(Array[Counter+1], Array[Counter+2]);
					pRender->SetLineColour(COLOUR_NONE);
					Counter ++;
				}
				else
					pRender->DrawBlob(Array[Counter], BT_UNSELECTED);
			}
		}
		if (Array != NULL)
			delete [] Array;
	}
	else
	{
		pRender->DrawBlob(GetMinorAxes(), BT_UNSELECTED);
		pRender->DrawBlob(GetMajorAxes(), BT_UNSELECTED);
		pRender->DrawBlob(GetCentrePoint() - (GetMajorAxes() - GetCentrePoint()), BT_UNSELECTED);
		pRender->DrawBlob(GetCentrePoint() - (GetMinorAxes() - GetCentrePoint()), BT_UNSELECTED);
	}
}



/********************************************************************************************

>	void NodeRegularShape::RenderTinyBlobs(RenderRegion* pRender)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		pRender - the region to render the blobs to
	Purpose:	Renders the Tiny blobs for a NodeRegularShape
	SeeAlso:	BlobManager

********************************************************************************************/

void NodeRegularShape::RenderTinyBlobs(RenderRegion* pRender)
{
	// Set the line colours etc as we need them
	pRender->SetLineColour(COLOUR_NONE);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	Path* RenderPath = NULL;
	if (BuildShapePath(&RenderPath))
	{
	 	// Find out about the path that the shape is made from
		DocCoord* Coords = RenderPath->GetCoordArray();

		// Render a blob on the first moveto
		pRender->DrawBlob(Coords[0], BT_UNSELECTED);
	}
}



/********************************************************************************************

>	DocRect NodeRegularShape::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		DontUseAttrs - TRUE if we should ignore the nodes attributes.
				Defaults to FALSE
	Returns:	The nodes bounding rect
	Purpose:	if the bounding rect is valid it is returned, if not, it is recalculated
				and then returned.
	SeeAlso:	NodeRegularShape::GetBlobBoundingRect

********************************************************************************************/
DocRect NodeRegularShape::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	// Something to put the new bounding rectangle in
	DocRect NewBoundingRect(GetCentrePoint(),GetCentrePoint());
	
	if (!IsBoundingRectValid || DontUseAttrs)
	{
		Path* RenderPath = NULL;
		BuildShapePath(&RenderPath);

		// Find out what the paths bounding rect is now
		if (!CalculatePathBoundingRect(*RenderPath, DontUseAttrs, &NewBoundingRect))
		{
			// GDraw failed to find out how big the bounding rect
			// we will have to make do with the bounding rect of the coords
			NewBoundingRect = RenderPath->GetBoundingRect();
		}

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
				CCRuntimeClass *pType;
				void *pVal;

				// iterating all (key, value) pairs
				for( CCAttrMap::iterator Pos = AttribMap.GetStartPosition(); Pos != AttribMap.GetEndPosition(); )
				{
					// Get attr at position Pos
					AttribMap.GetNextAssoc(Pos,pType,pVal);

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

>	DocRect NodeRegularShape::GetBlobBoundingRect()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Returns:	DocRect - Returns the bounding rect of the shape and its blobs
	Purpose:	This calculates the bounding box of the shape's path and adds in the
				influence of the selection blobs. It does not consider if the blobs are
				visible or not, it just gives the bounding box that they would occupy if
				they were visible

********************************************************************************************/
DocRect NodeRegularShape::GetBlobBoundingRect()
{
	// Find the Shapes bounding rectangle
	DocRect Rect = GetBoundingRect();

	Path* RenderPath = NULL;
	BuildShapePath(&RenderPath);

	// Add on the sizes of the blobs
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr!= NULL)
	{
		DocRect BlobSize;
		// There is a tiny blob on the MoveTo point
		DocCoord* Coords = RenderPath->GetCoordArray();
		pBlobMgr->GetBlobRect(Coords[0], &BlobSize);
		Rect = Rect.Union(BlobSize);

		// And a blob on the centre point
		DocCoord Point = GetCentrePoint();
		pBlobMgr->GetBlobRect(Point, &BlobSize);
		Rect = Rect.Union(BlobSize);
	}

	// Now include the blobs at the primary and stellation points, and their curvature points.
	if (!IsCircular())
	{
		DocCoord*	Array;
		INT32 Points = 0;
		BOOL Success = BuildPolygonPoints(&Array, &Points);
		UINT32 Counter = 0;
		if (Success)
		{
			TransformMatrix.transform((Coord*)Array, Points);
			DocRect BlobSize;
			for (UINT32 loop = 0; loop < NumSides; loop++)
			{
				if (IsStellated())
				{
					if (IsStellationCurvature())
					{
						pBlobMgr->GetBlobRect(Array[Counter], &BlobSize);
						Rect = Rect.Union(BlobSize);
						pBlobMgr->GetBlobRect(Array[Counter+1], &BlobSize);
						Rect = Rect.Union(BlobSize);
						pBlobMgr->GetBlobRect(Array[Counter+2], &BlobSize);
						Rect = Rect.Union(BlobSize);
						Counter += 3;
					}			
					else
					{
						pBlobMgr->GetBlobRect(Array[Counter++], &BlobSize);
						Rect = Rect.Union(BlobSize);
					}
				}

				if (IsPrimaryCurvature())
				{
					pBlobMgr->GetBlobRect(Array[Counter], &BlobSize);
					Rect = Rect.Union(BlobSize);
					pBlobMgr->GetBlobRect(Array[Counter+1], &BlobSize);
					Rect = Rect.Union(BlobSize);
					pBlobMgr->GetBlobRect(Array[Counter+2], &BlobSize);
					Rect = Rect.Union(BlobSize);
					Counter += 3;
				}
				else
				{
					pBlobMgr->GetBlobRect(Array[Counter++], &BlobSize);
					Rect = Rect.Union(BlobSize);
				}
			}
		}
		if (Array != NULL)
			delete [] Array;
	}
	else
	{
		DocRect BlobSize;
		DocCoord Point = GetMajorAxes();
		pBlobMgr->GetBlobRect(Point, &BlobSize);
		Rect = Rect.Union(BlobSize);
		DocCoord Opposite = GetCentrePoint() - (GetMajorAxes() - GetCentrePoint());
		pBlobMgr->GetBlobRect(Opposite, &BlobSize);
		Rect = Rect.Union(BlobSize);
		Point = GetMinorAxes();
		pBlobMgr->GetBlobRect(Point, &BlobSize);
		Rect = Rect.Union(BlobSize);
		Opposite = GetCentrePoint() - (GetMinorAxes() - GetCentrePoint());
		pBlobMgr->GetBlobRect(Opposite, &BlobSize);
		Rect = Rect.Union(BlobSize);
	}

	// Make sure we include the Bounds of our children
	IncludeChildrensBoundingRects(&Rect);

	// return the rectangle with the blobs included
	return Rect;
}



/********************************************************************************************

>	virtual UINT32 NodeRegularShape::GetNodeSize() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Returns:	The size of the node in bytes 
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/
UINT32 NodeRegularShape::GetNodeSize() const 
{     
	return (sizeof(NodeRegularShape)); 
}  



/********************************************************************************************

>	BOOL NodeRegularShape::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
												Spread* pSpread)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		PointerPos - The Location of the mouse pointer at the time of the click
				Click - The type of click received (single, double, drag etc)
				ClickMods - The modifiers to the click (eg shift, control etc )
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the QuickShape to respond to clicks by selecting its blobs or starting
				drags etc.

********************************************************************************************/

BOOL NodeRegularShape::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
														Spread* pSpread)
{
#ifndef STANDALONE

	// we only handle the click if we can confirm that object blobs are being displayed.
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr == NULL)
		return FALSE;
	if (!pBlobMgr->GetCurrentInterest().Object)
		return FALSE;

	DocCoord Start;
	DocCoord End;
	INT32 DragPointNumber;
	ShapeClickEffect HitTest = DetermineClickEffect(&PointerPos, pSpread, &DragPointNumber, &Start, &End);
	switch (HitTest)
	{
		case SHAPECLICK_CENTRE:
		{
			return ClickCentre(PointerPos, Click, ClickMods, pSpread);
			break;
		}
		case SHAPECLICK_ELLIPSE:
		{
			return ClickEllipse(PointerPos, Click, ClickMods, pSpread, FALSE);
			break;
		}
		case SHAPECLICK_PRIMARY:
		{
			return ClickPrimary(PointerPos, Click, ClickMods, pSpread, TRUE);
			break;
		}
		case SHAPECLICK_RECTANGLE:
		{
			return ClickPrimary(PointerPos, Click, ClickMods, pSpread, FALSE);
			break;
		}
		case SHAPECLICK_STELLATION:
		{
			return ClickStellation(PointerPos, Click, ClickMods, pSpread);
			break;
		}
		case SHAPECLICK_PRIMARYCURVE:
		{
			return ClickPCurve(PointerPos, Click, ClickMods, pSpread, Start, End);
			break;
		}
		case SHAPECLICK_STELLATIONCURVE:
		{
			return ClickSCurve(PointerPos, Click, ClickMods, pSpread, Start, End);
			break;
		}
		default: break;
	}

#endif

	// did not use the click
	return FALSE;
}



/********************************************************************************************

>	BOOL NodeRegularShape::OnToolClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
												Spread* pSpread)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95
	Inputs:		PointerPos - The Location of the mouse pointer at the time of the click
				Click - The type of click received (single, double, drag etc)
				ClickMods - The modifiers to the click (eg shift, control etc )
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the QuickShape to respond to clicks by selecting its blobs or starting
				drags etc.  This function is the same as OnClick execpt it is called from the
				QuickShape tool.  It does QuickShape dragging on rectangles and ellipses
				(instead of the specialised rect/ellipse drag).  It also allows edge reforming

********************************************************************************************/

BOOL NodeRegularShape::OnToolClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
														Spread* pSpread)
{
#ifndef STANDALONE

	// Call the default handler for all tools
	BOOL ClickUsed = OnClick(PointerPos, Click, ClickMods, pSpread);

	// Now test for special operations only supplied by shape editing tools
	if (!ClickUsed)
	{
		DocCoord Start;
		DocCoord End;
		INT32 DragPointNumber;
		ShapeClickEffect HitTest = DetermineClickEffect(&PointerPos, pSpread, &DragPointNumber, &Start, &End);

		if (HitTest==SHAPECLICK_EDGE1 || HitTest==SHAPECLICK_EDGE2)
			return ClickEdge(PointerPos, Click, ClickMods, pSpread, (HitTest==SHAPECLICK_EDGE1) );
	}

#endif

	// did not use the click
	return FALSE;
}



/********************************************************************************************

>	virtual BOOL NodeRegularShape::CanBecomeA(BecomeA* pBecomeA)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
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

BOOL NodeRegularShape::CanBecomeA(BecomeA* pBecomeA)
{
	// The NodeRegularShape can become a NodePath
	if (pBecomeA->BAPath())
	{
		pBecomeA->AddCount(1);

		return TRUE;
	}

	return FALSE;
}


		   
/********************************************************************************************

>	virtual BOOL NodeRegularShape::DoBecomeA(BecomeA* pBecomeA) 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/94
	Inputs:		pBecomeA = 	ptr to a class that contains all the info needed to become a new
							type of node.
	Outputs:	-
	Returns:	TRUE if the object has been transformed, FALSE if we run out of memory
	Purpose:	Transforms the object into another type of object. 
	SeeAlso:	NodeRegularShape::CanBecomeA

********************************************************************************************/

BOOL NodeRegularShape::DoBecomeA(BecomeA* pBecomeA)
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

		// Get the shapes render path into the node path.
		Path* ShapePath = NULL;
		if (Success)
			Success = BuildShapePath(&ShapePath);
		pNewNodePath->InkPath.Initialise(ShapePath->GetNumCoords(),1);
	
		if (Success) CALL_WITH_FAIL(pNewNodePath->InkPath.CopyPathDataFrom(ShapePath), pBecomeA->GetUndoOp(), Success);
	

		// If Success is TRUE, then we now have a new NodePath object that contains this shape's path
		if (Success)
		{
		 	switch (pBecomeA->GetReason())
			{
		 		case BECOMEA_REPLACE :
				{
					// It's a BECOMEA_REPLACE, so replace this node with the new NodePath
					UndoableOperation* pOp = pBecomeA->GetUndoOp();

					// Firstly, hide this node
					NodeHidden* pNodeHidden = NULL; 
					if (pOp != NULL)
						Success = pBecomeA->GetUndoOp()->DoHideNode(this, TRUE, &pNodeHidden);

					if (Success)
					{
						// Insert the new NodePath into the tree, next to the hidden node
						if (pOp != NULL)
							pNewNodePath->AttachNode(pNodeHidden,NEXT);
						else
							pNewNodePath->AttachNode(this,NEXT);

						// Copy the node's attributes
						CALL_WITH_FAIL(CopyChildrenTo(pNewNodePath), pBecomeA->GetUndoOp(), Success); 

						if (Success)
						{
							// Set the bounds  
							pNewNodePath->InvalidateBoundingRect();
							pNewNodePath->SetSelected(IsSelected());

							// Create a hide node action to hide the node when we undo 
							if (pOp != NULL)
							{
								HideNodeAction* UndoHideNodeAction;     
								Success = (HideNodeAction::Init(pBecomeA->GetUndoOp(),
												  		 pBecomeA->GetUndoOp()->GetUndoActionList(),
										 				 pNewNodePath, 
										 				 TRUE, 		 // Include subtree size 
								  		 				 ( Action**)(&UndoHideNodeAction))
								  		  				 != AC_FAIL);
							}
							else
							{
								UnlinkNodeFromTree();
								CascadeDelete();	// Delete all of the children
								delete this;		// SCARY!
							 	return TRUE; 
							}
						}
					}

					if (Success)
						pBecomeA->PassBack(pNewNodePath,this);
				}
				break;

				case BECOMEA_PASSBACK :
					Success = pBecomeA->PassBack(pNewNodePath,this);
				break;

				default: break;
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
>  	virtual NodePath* NodeRegularShape::GetVariableWidthStrokePath()
    
	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    15-5-2000
	Inputs:     -  
    Outputs:    -
    Returns:    a nodepath containing the path that is generated by any variable width strokes that
				are applied to us, or NULL if there aren't any
    Purpose:    If we have a variable width stroke applied to us then this will get the path generated
				by that stroke.  This base class version returns NULL, overridden versions must supply
				their own outline path.

				In this version we provide our own path by using BuildShapePath

	See also:   NodePath::GetVariableWidthStrokePath, NodeRegularShape::GetVariableWidthStrokePath
				
	
***********************************************************************************************/

NodePath* NodeRegularShape::GetVariableWidthStrokePath()
{
	// first find out if we actually have a variable width applied to us, if not then we don't do anything
	AttrVariableWidth* pVarWidth = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrVariableWidth), (NodeAttribute**)&pVarWidth);
	if (pVarWidth == NULL || ((VariableWidthAttrValue*)pVarWidth->GetAttributeValue())->GetWidthFunction() == NULL)
		return NULL;

	// next find the stroke attribute that actually does the work
	AttrStrokeType* pStroke = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeType), (NodeAttribute**)&pStroke);
	
	NodePath* pRetNode = NULL;
	if (pStroke && pStroke->HasPathProcessor())
	{
		PathProcessorStroke* pPPS = pStroke->GetPathProcessor();
			
		// Get the shapes render path 
		Path* pShapePath = NULL;
		if (BuildShapePath(&pShapePath))
			pRetNode = pPPS->GetProcessedPath(pShapePath, this);
				
		// now we have a choice, pass in the stroking flag to produce a path that works with bevels
		Path CopyPath;
		CopyPath.Initialise();
		CopyPath.CloneFrom(pRetNode->InkPath);

		// Mark Howitt 31/10/00
		// I`ve removed the ClipPathToPathWithAutoFlatness function and replaced it with
		// the function that returns you a flatness value to use with the ClipPath function
		double ClippingFlatness = CopyPath.CalculateFlatnessValueFromPath(2.0, 375.0, 500.0);
		double SourceFlatness = pShapePath->CalculateFlatnessValueFromPath(2.0, 375.0, 500.0);

		CopyPath.ClipPathToPath(*pShapePath, &(pRetNode->InkPath), 7, 1, ClippingFlatness, SourceFlatness);

		pRetNode->InkPath.InitialiseFlags();
		// lets have a go at smoothing it
		Spread *pSpread = Document::GetSelectedSpread();

		//Smoothness value is important - too little smoothness and you'll still have an over-complicated
		//path - too much and you'll lose detail...
		double smoothness = 15.0;
			
		//Use the retro smoother on all points without rendering to screen the EOR'd version...
		RetroSmooth rSmoother;
		rSmoother.Initialise();
		rSmoother.SetRenderFlag(false);

		// smooth the new path
		pRetNode->InkPath.SetAllSubSelection();
		rSmoother.Changing(pRetNode, pSpread, smoothness);
		rSmoother.FinishedNoUndo(pRetNode);
		pRetNode->InkPath.ClearSubSelection();
		
	}
		
	return pRetNode;	
}



/***********************************************************************************************
>  	virtual NodePath* NodeRegularShape::GetSmoothVariableWidthStrokePath()
    
	Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
    Created:    15-5-2000
	Inputs:     -  
    Outputs:    -
    Returns:    a nodepath containing the path that is generated by any variable width strokes that
				are applied to us, or NULL if there aren't any
    Purpose:    If we have a variable width stroke applied to us then this will get the path generated
				by that stroke.  This base class version returns NULL, overridden versions must supply
				their own outline path.

				In this version we provide our own path by using BuildShapePath

	See also:   NodePath::GetVariableWidthStrokePath, NodeRegularShape::GetVariableWidthStrokePath
				
	
***********************************************************************************************/

NodePath* NodeRegularShape::GetSmoothVariableWidthStrokePath()
{
	// first find out if we actually have a variable width applied to us, if not then we don't do anything
	AttrVariableWidth* pVarWidth = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrVariableWidth), (NodeAttribute**)&pVarWidth);
	if (pVarWidth == NULL || ((VariableWidthAttrValue*)pVarWidth->GetAttributeValue())->GetWidthFunction() == NULL)
		return NULL;

	// next find the stroke attribute that actually does the work
	AttrStrokeType* pStroke = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeType), (NodeAttribute**)&pStroke);
	
	NodePath* pRetNode = NULL;
	if (pStroke && pStroke->HasPathProcessor())
	{
		PathProcessorStroke* pPPS = pStroke->GetPathProcessor();
			
		// Get the shapes render path 
		Path* pShapePath = NULL;
		if (BuildShapePath(&pShapePath))
			pRetNode = pPPS->GetSmoothProcessedPath(pShapePath, this);
				
		// now we have a choice, pass in the stroking flag to produce a path that works with bevels
		Path CopyPath;
		CopyPath.Initialise();
		CopyPath.CloneFrom(pRetNode->InkPath);

		// Mark Howitt 31/10/00
		// I`ve removed the ClipPathToPathWithAutoFlatness function and replaced it with
		// the function that returns you a flatness value to use with the ClipPath function
		double ClippingFlatness = CopyPath.CalculateFlatnessValueFromPath(2.0, 375.0, 500.0);
		double SourceFlatness = pShapePath->CalculateFlatnessValueFromPath(2.0, 375.0, 500.0);

		CopyPath.ClipPathToPath(*pShapePath, &(pRetNode->InkPath), 7, 1, ClippingFlatness, SourceFlatness);

		pRetNode->InkPath.InitialiseFlags();
	}
		
	return pRetNode;	
}





/***********************************************************************************************

> BOOL NodeRegularShape::Snap(DocCoord* pDocCoord)

    Author:     Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
    Created:    21/12/94
	Inputs:     pDocCoord	= a coord in Spread coords
    Outputs:    If the point is snapped then pDocCoord will contain the point of attraction.
    Returns:    TRUE 	- the DocCoord has been snapped to the shapes path.
    			FALSE	- the DocCoord has not been processed.
    			                                  		 
    Purpose:    Snaps to given coord to the nearest point on the shapes render path.  If it is
    			not appropriate to snap the coord to the shape (at the moment this means the
    			coord is too far awawy), then FALSE is returned.
    Errors:     -   
	Scope:      public
	       
**********************************************************************************************/

BOOL NodeRegularShape::Snap(DocCoord* pDocCoord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	Path* RenderPath = NULL;
	BOOL Snapped = FALSE;
	if (BuildShapePath(&RenderPath))
		Snapped = CSnap::SnapCoordToPath(pDocCoord, RenderPath);

	return Snapped;
#else
	return FALSE;
#endif
}


/***********************************************************************************************

> BOOL NodeRegularShape::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)

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

BOOL NodeRegularShape::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
	return FALSE;
}



/********************************************************************************************

>	BOOL NodeRegularShape::SnapToCoords(DocCoord* pDocCoord)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/95
	Inputs:		pDocCoord - the coord to try and snap to
	Outputs:	pDocCoord - The snapped coord, if it was close enough to any of the magnetic
				points for a rectangle.
	Returns:	TRUE if it was snapped, FALSE if not
	Purpose:	Snaps the point magnetically to the the significant points of the shape

********************************************************************************************/

BOOL NodeRegularShape::SnapToCoords(DocCoord* pDocCoord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	BOOL IsSnapped = FALSE;
	
	// We can try snapping to the objects centre point
	DocCoord Centre=GetCentrePoint();
	if (IsMagneticallyClose(&Centre, pDocCoord))
		IsSnapped = TRUE;

	// Try the significant points
	if (!IsSnapped)
	{
		if (IsCircular())
		{
			DocCoord Points[4];
			Points[0]=GetMinorAxes();
			Points[1]=GetMajorAxes();
			Points[2]=GetCentrePoint() - (GetMajorAxes() - GetCentrePoint());
			Points[3]=GetCentrePoint() - (GetMinorAxes() - GetCentrePoint());

			for (INT32 loop = 0; (!IsSnapped && (loop<4)); loop++)
				IsSnapped = IsMagneticallyClose(&(Points[loop]), pDocCoord);
		}
		else
		{
			DocCoord* Array = NULL;
			INT32 Points = 0;
			if (BuildPolygonPoints(&Array, &Points))
			{
				TransformMatrix.transform((Coord*)Array, Points);
				for (INT32 loop = 0; ((loop < Points) && !IsSnapped); loop++)
				{
					if (IsMagneticallyClose(&(Array[loop]), pDocCoord))
						IsSnapped = TRUE;
				}

				// Rectangle special case - snap half way between each vertex
				if (IsARectangle() && !IsReformed())
				{
					for (INT32 loop = 1; ((loop < Points) && !IsSnapped); loop++)
					{
						DocCoord MidPoint( (Array[loop-1].x+Array[loop].x)/2, (Array[loop-1].y+Array[loop].y)/2 );
						if (IsMagneticallyClose(&MidPoint, pDocCoord))
							IsSnapped = TRUE;
					}
				}
			}
			if (Array != NULL)
				delete [] Array;
		}
	}
	else
	{
	}

	return IsSnapped;
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	virtual String NodeRegularShape::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		Plural: Flag indicating if the string description should be plural or
				singular. 
	Returns:	Description of the object                                    
	Purpose:	To return a description of the NodeRegularShape object in either the singular
				or the plural. This method is called by the DescribeRange method.
				The description will always begin with a lower case letter.   
	Errors:		A resource exception will be thrown if a problem occurs when loading the 
				string resource. 

********************************************************************************************/
              
String NodeRegularShape::Describe(BOOL Plural, BOOL Verbose) 
{     
	// Are we a circle?
	if (IsCircular())
	{
		if (Plural)
			return(String(_R(IDS_ELLIPSE_DESCRP)));  
		else
			return(String(_R(IDS_ELLIPSE_DESCRS))); 
	}

	// Are we a rectangle?
	if (IsARectangle())
	{
		if (Plural)
			return(String(_R(IDS_RECTANGLE_DESCRP)));  
		else
			return(String(_R(IDS_RECTANGLE_DESCRS))); 
	}
	
	// OK, we are a boring old QuickShape
	if (Plural)
		return(String(_R(IDS_REGSHAPE_PLURAL)));  
	else
		return(String(_R(IDS_REGSHAPE_SINGULAR))); 
}; 



/********************************************************************************************

>	void NodeRegularShape::PositionPointFromRatio(DocCoord* PosPoint, DocCoord* EndPoint, double Ratio)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		PosPoint - pointer to a DocCoord to change.
				EndPoint - pointer to a DocCoord at the end of a line from the CentrePoint
				Ratio - the distance along the line to set PosPoint.  0 sets it at CentrePoint,
				1 sets it at EndPoint.
	Outputs:	Stores the location of the point in PosPoint.
	Returns:	-                                   
	Purpose:	To obtain the position of a point at any position along any line from the
				(Untransformed) CentrePoint of the Regular shape.
	Errors:		-

********************************************************************************************/
void NodeRegularShape::PositionPointFromRatio(DocCoord* PosPoint, const DocCoord* EndPoint, const double Ratio)
{
	PosPoint->x = UTCentrePoint.x + (INT32)((EndPoint->x - UTCentrePoint.x)*Ratio) ;
	PosPoint->y = UTCentrePoint.y + (INT32)((EndPoint->y - UTCentrePoint.y)*Ratio) ;
} 



/********************************************************************************************

>	BOOL NodeRegularShape::MakeRegular(DocCoord RegularPoint)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		RegularPoint - the DocCoord defining the regularness ofthe shape
	Outputs:	-
	Returns:	FALSE if error occured 
	Purpose:	To make a shape regular, 1:1 aspect ratio
	Errors:		-

********************************************************************************************/
BOOL NodeRegularShape::MakeRegular(DocCoord RegularPoint)
{
#ifndef STANDALONE

	ERROR3IF(NumSides < REGULARSHAPE_MINSIDES, "Shape not inited!");

	OpShapeMakeRegular* Apple = new OpShapeMakeRegular;
	if (Apple != NULL)
	{
		Apple->DoMakeRegular(this, RegularPoint);
		return TRUE;
	}
	else
		return FALSE;
#else
	return FALSE;
#endif
} 



/********************************************************************************************

>	BOOL NodeRegularShape::MakeStraight(BOOL StraightenOne)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/95
	Inputs:		StraightenOne TRUE if EdgePath1 should be made a line, FALSE to straighten EdgePath2.
	Outputs:	Either EdgePath1 or EdgePath2 will have been made a straight line.
	Returns:	FALSE if error occured (SetError will have been called)                                   
	Purpose:	Call this function to reset a shapes edge path back to a straight line.  An
				OpEditRegularShape is created to carry out the change.  
	Errors:		Will complain if it runs out of memory

********************************************************************************************/
BOOL NodeRegularShape::MakeStraight(BOOL StraightenOne)
{
#ifndef STANDALONE

	ERROR3IF(NumSides < REGULARSHAPE_MINSIDES, "Shape not inited!");

	EditRegularShapeParam ChangeData(this);
	Path NewEdge;

	// Create the straight path 
	if (NewEdge.Initialise(4, 1))
	{
		DocCoord Fred1(0,0);
		DocCoord Fred2(72000,0);
		NewEdge.SetPathPosition(0);
		NewEdge.AddMoveTo(Fred1);
		NewEdge.AddCurveTo(Fred2);
	}
	else
	{
		Error::SetError(_R(IDS_OUT_OF_MEMORY));
		return FALSE;
	}

	if (StraightenOne)
		ChangeData.NewEdgePath1 = &NewEdge;
	else
		ChangeData.NewEdgePath2 = &NewEdge;

	// Now do the operation
	OpDescriptor* Apple = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpEditRegularShape));
	if (Apple != NULL)	
	{
		Apple->Invoke(&ChangeData);
		return TRUE;
	}
	else
	{
		Error::SetError(_R(IDS_OUT_OF_MEMORY));
		return FALSE;
	}
#else
	return TRUE;
#endif
} 



/*******************************************************************************************

> ShapeClickEffect NodeRegularShape::DetermineClickEffect(	DocCoord* pPos,
															Spread* pSpread,
															INT32* pPointNum, 
															DocCoord* pStart = NULL,
															DocCoord* pEnd = NULL)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pPos is the position to test
				pSpread is a pointer to the spread containing the position
				pPointNum, pStart and pEnd are return pointers
	Outputs:	pPos is adjusted to the point on the shape that was clicked on eg, a point
				near the centre point returns SHAPECLICK_CENTRE and pPos will be set to the
				centre point of the shape.
				pPointNum is the significant point number if the hit point, -1 if it isn't 
				numbered (eg centre point)
				pStart points to the start of the edge that contains the hit object
				pEnd points to the end of the edge that contains the hit object
				NOTE: These two are only set if the hit object is a curvature blob!
	Returns:	The effect of clicking - one of NewShape, OnCentre, OnMajorPoint, etc...
	Purpose:	Used when the cursor moves and when single clicking. This routine determines
				what effect a click will have by looking at the object under the pointer.

********************************************************************************************/

ShapeClickEffect NodeRegularShape::DetermineClickEffect(DocCoord* pPos, Spread* pSpread,
												INT32* pPointNum, DocCoord* pStart, DocCoord* pEnd)
{
#ifndef STANDALONE

	// Only get excited if the object is on the same spread as the pointer.
	if (FindParentSpread() != pSpread)
		return SHAPECLICK_NONE;

	DocRect BlobBounds;
	DocCoord Interest;
	*pPointNum = -1;

 	// Are we over any of the polygon object blobs?
	// Now draw blobs at the primary and stellation points, and their curvature points.
	if (!IsCircular())
	{
		DocCoord*	Array;
		INT32 Points = 0;
		BOOL Success = BuildPolygonPoints(&Array, &Points);
		UINT32 Counter = 0;
		ShapeClickEffect TempEffect = SHAPECLICK_NONE;
		if (Success)
		{
			TransformMatrix.transform((Coord*)Array,Points);
			for (UINT32 loop = 0; loop < NumSides; loop++)
			{
				UINT32 EndOffset = 1;
				DocCoord Start = Array[Counter];
				DocCoord StellEnd(0,0);
				DocCoord PrimeEnd;
				if (IsPrimaryCurvature())
					EndOffset++ ;
				if (IsStellated())
				{
					if (IsStellationCurvature())
						EndOffset++;
					StellEnd = Array[Counter + EndOffset];
					EndOffset++;
					if (IsStellationCurvature())
						EndOffset++;
				}
				if (IsPrimaryCurvature())
					EndOffset++ ;
				PrimeEnd = Array[Counter + EndOffset];
			
				GetApplication()->GetBlobManager()->GetBlobRect(Array[Counter++],&BlobBounds);
				if (BlobBounds.ContainsCoord(*pPos))
				{
					TempEffect = SHAPECLICK_PRIMARY;
					*pPos = Array[Counter-1];
					*pPointNum = Counter-1;
					goto FoundBlob;
				}

				if (IsPrimaryCurvature())
				{
					GetApplication()->GetBlobManager()->GetBlobRect(Array[Counter++],&BlobBounds);
					if (BlobBounds.ContainsCoord(*pPos))
					{
						*pPos = Array[Counter-1];
						*pPointNum = Counter-1;
						if (pStart != NULL)
							*pStart = Start;
						if (pEnd != NULL)
						{
							if (IsStellated())
								*pEnd = StellEnd;
							else
								*pEnd = PrimeEnd;
						}
						TempEffect = SHAPECLICK_PRIMARYCURVE;
						goto FoundBlob;
					}
				}  

				if (IsStellated())
				{
					// Test for the blob before the stellation point
					if (IsStellationCurvature())
					{
						GetApplication()->GetBlobManager()->GetBlobRect(Array[Counter++],&BlobBounds);
						if (BlobBounds.ContainsCoord(*pPos))
						{
							*pPos = Array[Counter-1];
							*pPointNum = Counter-1;
							if (pStart != NULL)
								*pStart = StellEnd;
							if (pEnd != NULL)
								*pEnd = Start;
							TempEffect = SHAPECLICK_STELLATIONCURVE;
							goto FoundBlob;
						}
					}
					
					// Test the stellation point itself
					GetApplication()->GetBlobManager()->GetBlobRect(Array[Counter++],&BlobBounds);
					if (BlobBounds.ContainsCoord(*pPos))
					{
						*pPos = Array[Counter-1];
						*pPointNum = Counter-1;
						TempEffect = SHAPECLICK_STELLATION;
						goto FoundBlob;
					}

					// Test for the blob after the stellation point
					if (IsStellationCurvature())
					{
						GetApplication()->GetBlobManager()->GetBlobRect(Array[Counter++],&BlobBounds);
						if (BlobBounds.ContainsCoord(*pPos))	
						{
							*pPos = Array[Counter-1];
							*pPointNum = Counter-1;
							if (pStart != NULL)
								*pStart = StellEnd;
							if (pEnd != NULL)
								*pEnd = PrimeEnd;
							TempEffect = SHAPECLICK_STELLATIONCURVE;
							goto FoundBlob;
						}
					} 
				}

				if (IsPrimaryCurvature())
				{
					GetApplication()->GetBlobManager()->GetBlobRect(Array[Counter++],&BlobBounds);
					if (BlobBounds.ContainsCoord(*pPos))
					{
						*pPos = Array[Counter-1];
						*pPointNum = Counter-1;
						if (pEnd != NULL)
						{
							if (IsStellated())
								*pEnd = StellEnd;
							else
								*pEnd = Start;
						}
						if (pStart != NULL)
							*pStart = PrimeEnd;
						TempEffect = SHAPECLICK_PRIMARYCURVE;
						goto FoundBlob;						
					}
				}  
			}
		}
		FoundBlob:
		if (Array != NULL)
			delete [] Array;
		if ((TempEffect == SHAPECLICK_PRIMARY) && IsARectangle())
			TempEffect = SHAPECLICK_RECTANGLE;
		if (TempEffect != SHAPECLICK_NONE)
			return TempEffect;
	}
	else
	{
		// Ellipses have four blobs on major, minor, and the two opposite points
		// Is the pointer is over the major axes or its opposite?
		DocCoord Opposite = GetCentrePoint() - (GetMajorAxes() - GetCentrePoint());
		DocRect OppBounds;
		GetApplication()->GetBlobManager()->GetBlobRect(Opposite,&OppBounds);
		DocCoord Point = GetMajorAxes();
		GetApplication()->GetBlobManager()->GetBlobRect(Point,&BlobBounds);
		if (BlobBounds.ContainsCoord(*pPos))
		{
			*pPos = Point;
			return SHAPECLICK_ELLIPSE;
		}
		if (OppBounds.ContainsCoord(*pPos))
		{
			*pPos = Opposite;
			return SHAPECLICK_ELLIPSE;
		}

		// Or the minor axis point and its opposite?
		Opposite = GetCentrePoint() - (GetMinorAxes() - GetCentrePoint());
		GetApplication()->GetBlobManager()->GetBlobRect(Opposite,&OppBounds);
		Point = GetMinorAxes();
		GetApplication()->GetBlobManager()->GetBlobRect(Point,&BlobBounds);
		if (BlobBounds.ContainsCoord(*pPos))
		{
			*pPos = Point;
			return SHAPECLICK_ELLIPSE;
		}
		if (OppBounds.ContainsCoord(*pPos))
		{
			*pPos = Opposite;
			return SHAPECLICK_ELLIPSE;
		}
	}

	// Is the pointer is over the centre point?
	Interest = GetCentrePoint();
	GetApplication()->GetBlobManager()->GetBlobRect(Interest,&BlobBounds);
	if (BlobBounds.ContainsCoord(*pPos))
	{
		*pPos = Interest;		
		return SHAPECLICK_CENTRE;
	}

	// Is the pointer near enough to do edge distortion?
	// Ellipses cannot have their edges distorted
	if (!IsCircular())
	{
		Path* RenderPath = NULL;
		INT32 tempPos;
		BuildShapePath(&RenderPath);
		if (RenderPath->PointCloseToLine(*pPos, &tempPos))
		{
			// Now work out wether we are going to distort edge path one or two
			const UINT32 EdgePath1Count = EdgePath1.GetNumCoords()-1;
			const UINT32 EdgePath2Count = EdgePath2.GetNumCoords()-1;
			INT32 Current = EdgePath1Count;
			if (Current >= tempPos)
				return SHAPECLICK_EDGE1;
			while (Current <= RenderPath->GetNumCoords()-1)
			{
				if (Current >= tempPos)
					return SHAPECLICK_EDGE1;

				// Allow for stellation curvature
				if (IsStellated() && IsStellationCurvature())
				{
					Current += 3;
					if (Current >= tempPos)
						return SHAPECLICK_NONE;
				}

				// We could now be sitting on the stellation path
				if (IsStellated())
				{
					Current += EdgePath2Count;
					if (Current >= tempPos)
						return SHAPECLICK_EDGE2;
				}

				// Now account for primary curvature
				if (IsPrimaryCurvature())
				{
					Current += 3;
					if (Current >= tempPos)
						return SHAPECLICK_NONE;
				}

				// Point to end of next primary path
				Current += EdgePath1Count;
			}
			ERROR3("Didn't find Current in RenderPath!");
			return SHAPECLICK_NONE;
		}
	}

#endif

	// To get here the pointer didn't excite anything, so return empty
	return SHAPECLICK_NONE;
}




/*******************************************************************************************

>	BOOL NodeRegularShape::InsertShapeEdge(Path* RenderPath, Path* Edge, DocCoord NewEnd)


	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Inputs:		RenderPath points to the path to insert the edge onto the end of
				Edge points to the edge path
				NewEnd is the point the new edge should end on.
	Outputs:	Appends a new edge onto the path pointed to by RenderPath
	Returns:	TRUE or FALSE depending on sucess 
	Purpose:	Adds another edge onto the current shape.  The edge runs from the current end
				of RenderPath to the point defined by NewEnd
				NOTE: Currently this routine assumes that the edge path consists of one Bezier
				curve.  There is an ERROR3 to check this!
	SeeAlso:	NodeRegularShape::BuildShapePath

********************************************************************************************/
BOOL NodeRegularShape::InsertShapeEdge(Path* RenderPath, Path* Edge, DocCoord NewEnd)
{
	// We can cope with a single line or a single curve
	if (Edge->GetNumCoords() == 2)
	{
		return RenderPath->AddLineTo(NewEnd);
	}
	else
	{
		ERROR3IF(Edge->GetNumCoords() != 4,"The edge path did not consist of a single Bezier curve");

		DocCoord* EdgeCoords = Edge->GetCoordArray();
		DocCoord* RenderCoords = RenderPath->GetCoordArray();

		// Get the start and ends of the paths to normalise.
		DocCoord InsertStart = RenderCoords[RenderPath->GetNumCoords()-1];
		DocCoord EdgeStart = EdgeCoords[0];
		DocCoord EdgeEnd = EdgeCoords[3];

		// To avoid mathematical problems detect if the start point == then end point
		if (InsertStart == NewEnd)
		{
			return RenderPath->AddCurveTo(InsertStart, NewEnd, NewEnd);
		}

	   	// We have to position the two control points along the line from InsertStart to InsertEnd
		DocCoord ControlPoints[2];
		ControlPoints[0] = EdgeCoords[1] - EdgeCoords[0];
		ControlPoints[1] = EdgeCoords[2] - EdgeCoords[0];
		double ScaleFactor = InsertStart.Distance(NewEnd)/EdgeStart.Distance(EdgeEnd);

		double EdgeRotation = atan2( (double)(EdgeEnd.y-EdgeStart.y) , (double)(EdgeEnd.x-EdgeStart.x) );
		double InsertRotation = atan2( (double)(NewEnd.y-InsertStart.y) , (double)(NewEnd.x-InsertStart.x) );
		double theta = InsertRotation - EdgeRotation;

		double a = ScaleFactor * cos(theta);
		double b = ScaleFactor * sin(theta);
		double c = -ScaleFactor * sin(theta);
		double d = ScaleFactor * cos(theta);
		INT32 e = InsertStart.x;
		INT32 f = InsertStart.y;									   

		Trans2DMatrix	Trans(Matrix(a,b,c,d,e,f));
		Trans.Transform( ControlPoints, 2);

		return RenderPath->AddCurveTo(ControlPoints[0], ControlPoints[1], NewEnd);
	}
}



/*******************************************************************************************

>	BOOL NodeRegularShape::BuildEllipsePath(Path* RenderPath)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95
	Inputs:		RenderPath points to an uninitialised path object
	Outputs:	RenderPath will contain the ellipse path
	Returns:	TRUE or FALSE depending on whether the path was succesfully created
	Purpose:	This function is called to build a ellipitical render path.  It can build
				the path directly around the CentrePoint.
	Errors:		Validity checks on the shape (is it circular?)
	SeeAlso:	NodeRegularShape::BuildShapePath

********************************************************************************************/
BOOL NodeRegularShape::BuildEllipsePath(Path* RenderPath)
{
	ERROR3IF(!IsCircular(),"BuildEllipsePath called for a non-elliptical shape");

	// Initialise the path
	if (!RenderPath->Initialise(12,4) )
	{
		return FALSE;
	}

	RenderPath->SetPathPosition(0);		   

	// Get the major and minor axes, and the points opposite them.
	const DocCoord w( UTMinorAxes);
	const DocCoord x( UTCentrePoint + (UTCentrePoint - UTMajorAxes) );
	const DocCoord y( UTCentrePoint + (UTCentrePoint - UTMinorAxes) );
	const DocCoord z( UTMajorAxes );

	// Now get the points at the vertices of the parallogram.
	const DocCoord a( UTMinorAxes + (UTMajorAxes - UTCentrePoint) );
	const DocCoord b( UTMinorAxes - (UTMajorAxes - UTCentrePoint) );
	const DocCoord c( y - (UTMajorAxes - UTCentrePoint) );
	const DocCoord d( y + (UTMajorAxes - UTCentrePoint) );

	// Insert the path data
	BOOL Success = TRUE;
//	const INT32 MidX = UTCentrePoint.x;
//	const INT32 MidY = UTCentrePoint.y;
	DocCoord CP1;
	DocCoord CP2;

 	// Add the moveto at the start of the path
 	Success = RenderPath->AddMoveTo(z);

	// Add the first curve
	if (Success)
	{
		CP1 = DocCoord::PositionPointFromRatio(z, a, CurveFactor);
		CP2 = DocCoord::PositionPointFromRatio(w, a, CurveFactor);
		Success = RenderPath->AddCurveTo(CP1, CP2, w);
	}

	// and the second
	if (Success)
	{
		CP1 = DocCoord::PositionPointFromRatio(w, b, CurveFactor);
		CP2 = DocCoord::PositionPointFromRatio(x, b, CurveFactor);
		Success = RenderPath->AddCurveTo(CP1, CP2, x);
	}

	// and the third
	if (Success)
	{
		CP1 = DocCoord::PositionPointFromRatio(x, c, CurveFactor);
		CP2 = DocCoord::PositionPointFromRatio(y, c, CurveFactor);
		Success = RenderPath->AddCurveTo(CP1, CP2, y);
	}

	// and finally the fourth
	if (Success)
	{
		CP1 = DocCoord::PositionPointFromRatio(y, d, CurveFactor);
		CP2 = DocCoord::PositionPointFromRatio(z, d, CurveFactor);
		Success = RenderPath->AddCurveTo(CP1, CP2, z);
	}

	// Finish off the path by setting the close flags and filling it
	if (Success)
	{
		RenderPath->CloseSubPath();
		RenderPath->IsFilled = TRUE;
	}

	return Success;
}



class FloatCoord
{
public:
	FloatCoord() {};
	FloatCoord(double initX, double initY) {X = initX; Y = initY;};
	double X;
	double Y;
};



/*******************************************************************************************

>	BOOL NodeRegularShape::BuildPolygonPoints(DocCoord** pArray, INT32* NumPoints = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95
	Inputs:		pArray points to a pointer to a DocCoord
				NumPoints points to an INT32
	Outputs:	pArray will point to an array of DocCoords.  The array contains the
				significant points of the shape 
				NumPoints will contain the number of points in pArray
	Returns:	TRUE or FALSE for success/failure
	Purpose:	Use this function to build a array of significant locations on a polygonal shape
				These are the primary and stellation points, also the primary and stellation
				curvature points.  The significant points are only present in the array if the
				shape has that feature enabled
				NOTE: The first two points in the array are duplicated at the end of the array
				to make BuildPolygonPath's life easier
	SeeAlso:	NodeRegularShape::BuildShapePath, NodeRegularShape::BuildPolygonPath
	Errors:		ERROR2 Validity checks on shape 

********************************************************************************************/
BOOL NodeRegularShape::BuildPolygonPoints(DocCoord** pArray, INT32* NumPoints)
{
	ERROR3IF(IsCircular(),"Can't build a points array for a circular shape!");
	ERROR2IF(NumSides == 0, FALSE, "Zero number of sides!");
	ERROR3IF(IsPrimaryCurvature() && !IsStellationCurvature(), "Shape has primary curvature but not stellation curvature");
	ERROR3IF(!IsPrimaryCurvature() && IsStellationCurvature(), "Shape has stellation curvature but not primary curvature");

	FloatCoord* pFloatArray = NULL;
	DocCoord* pDocArray = *pArray;

	// Claim the space for the two arrays.
	UINT32 FloatElements = NumSides;
	UINT32 DocElements = NumSides;
	if (IsStellated())
	{
		FloatElements += NumSides;
		DocElements += NumSides;
	}
	if (IsPrimaryCurvature())
		DocElements += NumSides*2;
	if (IsStellated() && IsStellationCurvature())
		DocElements += NumSides*2;
	DocElements += 3;
	FloatElements += 2;

	pFloatArray = new FloatCoord[FloatElements];
	pDocArray = new DocCoord[DocElements];
	*pArray = pDocArray;

	// Deal with lack of memory
	if ( (pDocArray == NULL) || (pFloatArray == NULL) )
	{
		if (pDocArray != NULL)
			delete [] pDocArray;
		if (pFloatArray != NULL)
			delete [] pFloatArray;
		return FALSE;
	}
	
	// Fill in the FloatArray with normalised coordinates
	double CurrentAngle = (float)(PI/NumSides);
	const double AngleInc = (float)((2*PI)/NumSides);
	const double StellationInc = (float)((AngleInc/2) + (StellOffsetRatio*AngleInc));
	const double MaxRadius = 0.5;
	double Radius = MaxRadius;
	double StellationRadius = Stellated ? (Radius * StellRadiusToPrimary) : Radius;
	if (StellationRadius > MaxRadius)
	{
		const double ratio = StellationRadius/Radius;
		Radius = Radius / ratio;
		StellationRadius = StellationRadius / ratio;
	}
	const FloatCoord InitialPrimaryPoint(0.0, Radius);
	const FloatCoord InitialStellationPoint(0.0, StellationRadius);
	UINT32 FloatCounter = 0;
	FloatCoord TempCoord;

	UINT32				CurrentSide;
	for( CurrentSide = 0; CurrentSide < NumSides; CurrentSide++)
	{
		// Calculate the position of the normalised points by rotation
		TempCoord.X = (float)(InitialPrimaryPoint.X*cos(CurrentAngle)
												- InitialPrimaryPoint.Y*sin(CurrentAngle) + MaxRadius);
		TempCoord.Y = (float)(InitialPrimaryPoint.X*sin(CurrentAngle)
												+ InitialPrimaryPoint.Y*cos(CurrentAngle) + MaxRadius);
		pFloatArray[FloatCounter++] = TempCoord;

		// Now do the stellation point if the shape is stellated
		if (Stellated)
		{
			TempCoord.X = (float)(InitialStellationPoint.X*cos(CurrentAngle+StellationInc) 
								- InitialStellationPoint.Y*sin(CurrentAngle+StellationInc) + MaxRadius);
			TempCoord.Y = (float)(InitialStellationPoint.X*sin(CurrentAngle+StellationInc)
								+ InitialStellationPoint.Y*cos(CurrentAngle+StellationInc) + MaxRadius);
			pFloatArray[FloatCounter++] = TempCoord;
		}

		CurrentAngle += AngleInc;
	}
	
	pFloatArray[FloatCounter] = pFloatArray[0];
	ERROR3IF(FloatCounter >= FloatElements, "Ran over end of FloatCoord array");

/*UINT32 Temp = 0;
PATRACE( _T("\n\nFloat Array"));
for (CurrentSide = 0; CurrentSide < NumSides; CurrentSide++)
{
	PATRACE( _T("Primary: %f,%f\n"), pFloatArray[Temp].X, pFloatArray[Temp++].Y);
	if (IsStellated())
		PATRACE( _T("Stellation: %f,%f\n"), pFloatArray[Temp].X, pFloatArray[Temp++].Y);
}*/

	// Now copy the normalised coordinates into the DocCoord array, transforming them at the same time
	// Also insert the curvature points as required...
	DocCoord OuterMajor =  UTMajorAxes;
	DocCoord OuterMinor = UTMinorAxes;
	
	// if the stellations points are further from the centre than the primary points
	// then adjust the positioning length
	if (IsStellated() && (GetStellRadiusToPrimary() > 1.0))
	{
		PositionPointFromRatio(&OuterMajor, &UTMajorAxes, GetStellRadiusToPrimary());
		PositionPointFromRatio(&OuterMinor, &UTMinorAxes, GetStellRadiusToPrimary());
	}

	// Get the origin and two adjacent coordinates of the parallogram that we are trying to fit the
	// normalised coordinates into.
	const DocCoord ParallelOrigin(UTCentrePoint.x*3 - (OuterMajor.x + OuterMinor.x), 
											UTCentrePoint.y*3 - (OuterMajor.y + OuterMinor.y));
	const DocCoord ParallelA((OuterMajor.x - (OuterMinor.x-UTCentrePoint.x)) - ParallelOrigin.x,
								(OuterMajor.y - (OuterMinor.y-UTCentrePoint.y)) - ParallelOrigin.y);
	const DocCoord ParallelB((OuterMinor.x - (OuterMajor.x-UTCentrePoint.x)) - ParallelOrigin.x,
								(OuterMinor.y - (OuterMajor.y-UTCentrePoint.y)) - ParallelOrigin.y);

	UINT32 DocCounter = 0;
	FloatCounter = 0;
	DocCoord PrimaryPoint;
	DocCoord StellationPoint(0,0);
	DocCoord NextPrimary;
	
	// initialise NextPrimary for the first loop iteration
	FloatCoord InitPrimeFloat = pFloatArray[FloatCounter++];
	NextPrimary.x = (INT32)(InitPrimeFloat.X*ParallelB.x + InitPrimeFloat.Y*ParallelA.x) + ParallelOrigin.x;
	NextPrimary.y = (INT32)(InitPrimeFloat.X*ParallelB.y + InitPrimeFloat.Y*ParallelA.y) + ParallelOrigin.y;
//	TransformMatrix.Transform(&NextPrimary,1);

	// process each FloatCoord into an actual DocCoord.  Leave room in the array to go back
	// and insert the curvature points, AFTER the primary&stellation points have been treansformed
	// to the actual document coordinates of the shape.
	for (CurrentSide = 0; CurrentSide < NumSides; CurrentSide++)
	{
		// Store the current primary point in the DocCoord array
		PrimaryPoint = NextPrimary;
		pDocArray[DocCounter++]	= PrimaryPoint;

		// Get the stellation point
		if (IsStellated())
		{
			const FloatCoord StellFloat = pFloatArray[FloatCounter++];
	  		StellationPoint.x = (INT32)(StellFloat.X*ParallelB.x + StellFloat.Y*ParallelA.x) + ParallelOrigin.x;
			StellationPoint.y = (INT32)(StellFloat.X*ParallelB.y + StellFloat.Y*ParallelA.y) + ParallelOrigin.y;
//			TransformMatrix.Transform(&StellationPoint,1);
		}

		// Get the next primary point
		const FloatCoord PrimeFloat = pFloatArray[FloatCounter++];
  		NextPrimary.x = (INT32)(PrimeFloat.X*ParallelB.x + PrimeFloat.Y*ParallelA.x) + ParallelOrigin.x;
		NextPrimary.y = (INT32)(PrimeFloat.X*ParallelB.y + PrimeFloat.Y*ParallelA.y) + ParallelOrigin.y;
//		TransformMatrix.Transform(&NextPrimary,1);

		if (IsPrimaryCurvature())
			DocCounter++;

		if (IsStellated())
		{
			if (IsStellationCurvature())
				DocCounter ++;

			pDocArray[DocCounter++] = StellationPoint;

			if (IsStellationCurvature())
				DocCounter ++;
		}

		if (IsPrimaryCurvature())
			DocCounter++;
	}
	pDocArray[DocCounter] = pDocArray[0];

	// Go through the DocCoord array, filling in the gaps left for the curvature points
	if (IsPrimaryCurvature())  // && IsStellationCurvature()
	{
		// Get the length of the major axis for fitting curvature lengths along
		double temp = UTCentrePoint.Distance(OuterMajor) ;
		const double OuterMajorLength = (temp > 1.0) ? (temp) : (1.0);
		const double PrimaryCurvatureLength = OuterMajorLength * GetPrimaryCurveToPrimary();
		const double StellationCurvatureLength = OuterMajorLength * GetStellCurveToStell();

		DocCounter = 0;
		DocCoord TempCoord;

		for (CurrentSide = 0; CurrentSide < NumSides; CurrentSide++)
		{
			DocCoord PrimaryPoint = pDocArray[DocCounter];
			DocCoord StellationPoint = pDocArray[DocCounter+3];
			DocCoord NextPrimary = IsStellated() ? pDocArray[DocCounter+6] : pDocArray[DocCounter+3];

			// if the shape has primary curvature then insert the primary curvature point between
			// either the next primary point or the setllation point, depending on stellationness
			if (IsStellated())
			{
				// Fit the primary curvature point between the primary point and the stellation point
				double DistanceToNext = PrimaryPoint.Distance(StellationPoint);
				double FitRatio;
				if ((DistanceToNext < 1.0) || ((PrimaryCurvatureLength + StellationCurvatureLength) < 1.0))
					TempCoord = PrimaryPoint;
				else
				{
					FitRatio = PrimaryCurvatureLength / DistanceToNext;
					if ((PrimaryCurvatureLength + StellationCurvatureLength) > DistanceToNext)
					{	
						double Scaler = DistanceToNext / (PrimaryCurvatureLength + StellationCurvatureLength);
						FitRatio = (PrimaryCurvatureLength * Scaler) / DistanceToNext;
					}
					TempCoord = DocCoord::PositionPointFromRatio(PrimaryPoint, StellationPoint, FitRatio); 
				}
			}
			else
			{
				// Fit the primary curvature point between the primary point and the next
				double DistanceToNext = PrimaryPoint.Distance(NextPrimary);
				double FitRatio = 0.5;
				if (DistanceToNext < 1.0)
					TempCoord = PrimaryPoint;
				else
				{
					if ((PrimaryCurvatureLength + PrimaryCurvatureLength) < DistanceToNext)
						FitRatio = PrimaryCurvatureLength / DistanceToNext;
			
					TempCoord = DocCoord::PositionPointFromRatio(PrimaryPoint, NextPrimary, FitRatio);  
				}
			}
			pDocArray[DocCounter+1]	= TempCoord;

			if (IsStellated())
			{
				// Fit the stellation curvature point between the primary point and the stellation point
				double DistanceToNext = PrimaryPoint.Distance(StellationPoint);
				if ((DistanceToNext < 1.0) || ((PrimaryCurvatureLength + StellationCurvatureLength) < 1.0))
					TempCoord = StellationPoint;
				else
				{
					double FitRatio = StellationCurvatureLength / DistanceToNext;
					if ((PrimaryCurvatureLength + StellationCurvatureLength) > DistanceToNext)
					{
						double Scaler = DistanceToNext / (PrimaryCurvatureLength + StellationCurvatureLength);
						FitRatio = (StellationCurvatureLength * Scaler) / DistanceToNext;
					}
					TempCoord = DocCoord::PositionPointFromRatio(StellationPoint, PrimaryPoint, FitRatio); 
				}
				pDocArray[DocCounter+2] = TempCoord;

				// Fit the stellation curvature point between the stellation point and the next primary point
				DistanceToNext = StellationPoint.Distance(NextPrimary);
				if ((DistanceToNext < 1.0) || ((PrimaryCurvatureLength + StellationCurvatureLength) < 1.0))
					TempCoord = StellationPoint;
				else
				{
					double FitRatio = StellationCurvatureLength / DistanceToNext;
					if ((PrimaryCurvatureLength + StellationCurvatureLength) > DistanceToNext)
					{
						double Scaler = DistanceToNext / (PrimaryCurvatureLength + StellationCurvatureLength);
						FitRatio = (StellationCurvatureLength * Scaler) / DistanceToNext;
					}
					TempCoord = DocCoord::PositionPointFromRatio(StellationPoint, NextPrimary, FitRatio); 
				}
				pDocArray[DocCounter+4]	= TempCoord;
			}

			// Fit the second primary curvature point either between the two primary points or
			// the stellation point and the next primary point
			if (IsStellated())
			{
				// Fit the primary curvature point between the stellation point and the next primary point
				double DistanceToNext = StellationPoint.Distance(NextPrimary);
				if ((DistanceToNext < 1.0) || ((PrimaryCurvatureLength + StellationCurvatureLength) < 1.0))
					TempCoord = StellationPoint;
				else
				{
					double FitRatio = PrimaryCurvatureLength / DistanceToNext;
					if ((PrimaryCurvatureLength + StellationCurvatureLength) > DistanceToNext)
					{
						double Scaler = DistanceToNext / (PrimaryCurvatureLength + StellationCurvatureLength);
						FitRatio = (PrimaryCurvatureLength * Scaler) / DistanceToNext;
					}
					TempCoord = DocCoord::PositionPointFromRatio(NextPrimary, StellationPoint, FitRatio); 
				}
			
				pDocArray[DocCounter+5]	= TempCoord;
			}
			else
			{
				// Fit the primary curvature point between the primary point and the next
				double DistanceToNext = PrimaryPoint.Distance(NextPrimary);
				if (DistanceToNext < 1.0)
					TempCoord = PrimaryPoint;
				else
				{
					double FitRatio = 0.5;
					if ((PrimaryCurvatureLength + PrimaryCurvatureLength) < DistanceToNext)
						FitRatio = PrimaryCurvatureLength / DistanceToNext;

					TempCoord = DocCoord::PositionPointFromRatio(NextPrimary, PrimaryPoint, FitRatio);  
				}
				pDocArray[DocCounter+2]	= TempCoord;
			}

			// Now increment DocCounter to point at the next primary point
			DocCounter+=3;
			if (IsStellated())
				DocCounter+=3;
		}
	}

	pDocArray[DocCounter++] = pDocArray[0];
	pDocArray[DocCounter] = pDocArray[1];

/*// Will dump the doccoord array
Temp = 0;
PATRACE( _T("\n\nDocCoord Array"));
for (CurrentSide = 0; CurrentSide < NumSides; CurrentSide++)
{
	PATRACE( _T("Primary: %d,%d\n"), pDocArray[Temp].x, pDocArray[Temp++].y);
	if (IsPrimaryCurvature())
		PATRACE( _T("Primary Curvature: %d,%d\n"), pDocArray[Temp].x, pDocArray[Temp++].y);
	if (IsStellationCurvature())
		PATRACE( _T("Stellation Curvature: %d,%d\n"), pDocArray[Temp].x, pDocArray[Temp++].y);
	if (IsStellated())
		PATRACE( _T("Stellation: %d,%d\n"), pDocArray[Temp].x, pDocArray[Temp++].y);
	if (IsStellationCurvature())
		PATRACE( _T("Stellation Curvature: %d,%d\n"), pDocArray[Temp].x, pDocArray[Temp++].y);
	if (IsPrimaryCurvature())
		PATRACE( _T("Primary Curvature: %d,%d\n"), pDocArray[Temp].x, pDocArray[Temp++].y);
}*/

	delete [] pFloatArray;

	ERROR3IF(DocCounter >= DocElements, "Ran over end of DocCoord array");

	if (NumPoints != NULL)
		*NumPoints = DocCounter+1;
	return TRUE;
}	



/*******************************************************************************************

>	BOOL NodeRegularShape::BuildPolygonPath(Path* RenderPath, DocCoord* pArray)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95
	Inputs:		RenderPath points to the path to build the path into 
				pArray points to the significant points array.
	Outputs:	RenderPath will contain a path describing the polygon.
	Returns:	TRUE or FALSE if the path build succedes/fails 
	Purpose:	Use this function to build a polygonal path.  The path forms a closed and
				filled shape
	Errors:		Validation checks on the shape
	SeeAlso:	NodeRegularShape::BuildShapePath, NodeRegularShape::BuildPolygonPoints

********************************************************************************************/
BOOL NodeRegularShape::BuildPolygonPath(Path* RenderPath, DocCoord* pArray)
{
	ERROR2IF(NumSides == 0, FALSE, "This shape has zero sides!");
	ERROR3IF(IsCircular(), "Building a polygon path for an ellipse!");

	// Calculate the number of elements the path will need
	INT32 Slots = NumSides*(EdgePath1.GetNumCoords()-1);
	if (IsStellated())
		Slots += NumSides*(EdgePath2.GetNumCoords()-1);
	if (IsPrimaryCurvature())
		Slots += NumSides*4;
	if (IsStellationCurvature())
		Slots += (NumSides-1)*4;

	if (!RenderPath->Initialise(Slots,4) )
	{
		return FALSE;
	}
	RenderPath->SetPathPosition(0);

	UINT32 Counter = 0;
	BOOL Success = TRUE;

	// Insert the MoveTo at the start of the path
	if (IsPrimaryCurvature())
		Success = RenderPath->AddMoveTo(pArray[1]);
	else
		Success = RenderPath->AddMoveTo(pArray[0]);

	UINT32				CurrentSide;
	for( CurrentSide = 0; (CurrentSide < NumSides) && Success; CurrentSide++)
	{
		// Skip over the initial Primry point and the curvature point
		Counter ++;
		if (IsPrimaryCurvature())
			Counter++;

		// Insert edge path one so that it ends on the current coordinate.
		Success = InsertShapeEdge(RenderPath, &EdgePath1, pArray[Counter]);

		if (IsStellated())
		{
	 		// Insert the curve around the stellation point
	 		if (IsStellationCurvature() && Success)
			{
				DocCoord CP1;
				DocCoord CP2;
				CP1 = DocCoord::PositionPointFromRatio(pArray[Counter], pArray[Counter+1], CurveFactor);
				CP2 = DocCoord::PositionPointFromRatio(pArray[Counter+2], pArray[Counter+1], CurveFactor);
				Success = RenderPath->AddCurveTo(CP1, CP2, pArray[Counter+2]);
				Counter += 2;
			}

			if (Success)
			{
				Success = InsertShapeEdge(RenderPath, &EdgePath2, pArray[Counter+1]);
				Counter ++;
			}
		}

		// Now insert the curve around the primary point if required
		if (IsPrimaryCurvature() && Success)
		{
			DocCoord CP1;
			DocCoord CP2;
			CP1 = DocCoord::PositionPointFromRatio(pArray[Counter], pArray[Counter+1], CurveFactor);
			CP2 = DocCoord::PositionPointFromRatio(pArray[Counter+2], pArray[Counter+1], CurveFactor);
			Success = RenderPath->AddCurveTo(CP1, CP2, pArray[Counter+2]);
			Counter ++;
		}
	}

	if (!Success)
	{
		return FALSE;
	}

	// Close the path properly
	RenderPath->CloseSubPath();
	RenderPath->IsFilled = TRUE;

	// Now we should run around the path and smooth the joins between curvatures and edges.
	if (IsPrimaryCurvature() || IsStellationCurvature())
	{
		const UINT32 SizePath1 = EdgePath1.GetNumCoords()-1;
		const UINT32 SizePath2 = EdgePath2.GetNumCoords()-1;
		DocCoord* PathCoords = RenderPath->GetCoordArray();
		INT32 Current = 0;

		for (CurrentSide = 0; CurrentSide < NumSides; CurrentSide++)
		{
			// Skip over edge path 1
			Current += SizePath1;

			if (IsStellated())
			{
				// do the stellation curvature
				if (IsStellationCurvature())
				{
					SmoothCurvatureJoin(RenderPath, PathCoords, Current, FALSE);
					Current += 3;
					SmoothCurvatureJoin(RenderPath, PathCoords, Current, TRUE);
				}
				Current += SizePath2;
			}

			// Do the primary curvature
			if (IsPrimaryCurvature())
			{
				SmoothCurvatureJoin(RenderPath, PathCoords, Current, FALSE);
				Current += 3;
				SmoothCurvatureJoin(RenderPath, PathCoords, Current, TRUE);
			}
		}
	}

	return TRUE;
}



/*******************************************************************************************

>	BOOL NodeRegularShape::SmoothCurvatureJoin(Path* RenderPath, DocCoord* Coords, INT32 Index, BOOL MoveLeft)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/95
	Inputs:		RenderPath - points to the path to smooth the joins in
				Coords - points to the path coordinate array
				Index points to the endpoint to smooth
				MoveLeft is TRUE if the left hand control point should move, FALSE if the
				right hand one should move.
	Outputs:	The control point in the Coords array will have moved
	Returns:	FALSE if an error occured
	Purpose:	This function is used to smooth the joins between edges paths and the 
				curvature segments
	Errors:		Can error if the opposite control point cannot be found.
	SeeAlso:	OpEditRegularShape::Do

********************************************************************************************/
BOOL NodeRegularShape::SmoothCurvatureJoin(Path* RenderPath, DocCoord* Coords, INT32 Index, BOOL MoveLeft)
{
	// Dont do any smoothing if the edge is straight
	if ((RenderPath->GetVerbArray()[Index] & ~PT_CLOSEFIGURE) == PT_LINETO)
		return TRUE;

	ERROR3IF(Index == 0, "Index was 0.  Must be one or greater");
	const INT32 Oppo = RenderPath->FindOppositeControlPoint(Index-1);
	if (Oppo == -1)
		return TRUE;

	DocCoord CPLeft = Coords[Index-1];
	DocCoord Centre = Coords[Index];
	DocCoord CPRight = Coords[Oppo];
	DocCoord MoveCP;

	double LeftDist = Centre.Distance(CPLeft);
	double RightDist = Centre.Distance(CPRight);

	if ((LeftDist == 0.0) || (RightDist == 0.0))
		return TRUE;

	if (MoveLeft)
	{
		MoveCP = DocCoord::PositionPointFromRatio(Centre, CPRight, LeftDist/RightDist);
		Coords[Index-1] = Centre - (MoveCP - Centre);
	}
	else
	{
		MoveCP = DocCoord::PositionPointFromRatio(Centre, CPLeft, RightDist/LeftDist);
		Coords[Oppo] = Centre - (MoveCP - Centre);
	}
	return TRUE;
}



/*******************************************************************************************

>	BOOL NodeRegularShape::ToggleCurvature()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		-
	Outputs:	The shape will have has its Primary and Setllation Curvature state toggled
	Returns:	TRUE or FALSE if the path toggle succedes/fails 
	Purpose:	This function will toggle the curvature state of the shape in an undoable
				way by firing off an OpEditRegularShape
	Errors:		-
	SeeAlso:	OpEditRegularShape::Do

********************************************************************************************/
BOOL NodeRegularShape::ToggleCurvature()
{
#ifndef STANDALONE

	EditRegularShapeParam ChangeData(this);
	if (IsPrimaryCurvature() || IsStellationCurvature())
	{
		ChangeData.NewPrimaryCurvature = EditRegularShapeParam::CHANGE_SETFALSE;
		ChangeData.NewStellationCurvature = EditRegularShapeParam::CHANGE_SETFALSE;
	}
	else
	{
		ChangeData.NewPrimaryCurvature = EditRegularShapeParam::CHANGE_SETTRUE;
		ChangeData.NewStellationCurvature = EditRegularShapeParam::CHANGE_SETTRUE;
	}
	OpDescriptor* Apple = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpEditRegularShape));
	if (Apple != NULL)
	{
		Apple->Invoke(&ChangeData);
		return TRUE;
	}
	else
		return FALSE;
#else
	return FALSE;
#endif
}


#ifndef STANDALONE

/*******************************************************************************************

>	BOOL NodeRegularShape::ClickEllipse(DocCoord PointerPos, ClickType Click,
									ClickModifiers ClickMods, Spread* pSpread, BOOL DragCentre))

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/04/95
	Inputs:		PointerPos - the location of the click
				Click - the type of the click
				ClickMods - the current click modifiers
				pSpread - pointer to the spread containing the shape
	Outputs:	-
	Returns:	TRUE if the click is used / FALSE if it isn't
	Purpose:	This function will handle clicks on an ellipse
				Single - unused
				Double - Change ellipse so it is a circle
				Drag - Rotate and resize ellipse
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL NodeRegularShape::ClickEllipse(DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
																	 Spread* pSpread, BOOL DragCentre)
{
	if (IsCircular())
	{
		switch (Click)
		{
			case CLICKTYPE_DOUBLE:
			{
				// A double click on a point is used to make the shape regular
				if (!MakeRegular(PointerPos))
					InformError();
				return TRUE;
				break;
			}
			case CLICKTYPE_DRAG:
			{
				// A drag on the major axes is used to resize and rotate the shape
				OpDragRegularShape* pOp = new (OpDragRegularShape);
				if (pOp != NULL)
				{
					pOp->DoStartDragEdit( this, PointerPos, pSpread, OpDragRegularShape::DRAG_ELLIPSE, DragCentre);
				}
				return TRUE;
				break;
			}
			default:
				return FALSE;
		}
	}
	else
		return FALSE;
}



/*******************************************************************************************

>	BOOL NodeRegularShape::ClickCentre(DocCoord PointerPos, ClickType Click,
													ClickModifiers ClickMods, Spread* pSpread)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		PointerPos - the location of the click
				Click - the type of the click
				ClickMods - the current click modifiers
				pSpread - pointer to the spread containing the shape
	Outputs:	-
	Returns:	TRUE if the click is used / FALSE if it isn't
	Purpose:	This function will handle clicks on a shapes centre axes point
				Single - unused
				Double - Toggle polygon/ellipse
				Drag - Translate shape
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL NodeRegularShape::ClickCentre(DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread* pSpread)
{
	switch (Click)
	{
		case CLICKTYPE_DOUBLE:
		{
			// A double click on the centre point is used to toggle the shapes Ellipse/Polygon state
			EditRegularShapeParam ChangeData(this);
			if (IsCircular())
				ChangeData.NewCircular = EditRegularShapeParam::CHANGE_SETFALSE;
			else
				ChangeData.NewCircular = EditRegularShapeParam::CHANGE_SETTRUE;
			OpDescriptor* Apple = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpEditRegularShape));
			if (Apple != NULL)
				Apple->Invoke(&ChangeData);	
			return TRUE;
			break;
		}
		case CLICKTYPE_DRAG:
		{
			// A drag on the centre point is used to move the shape around
			OpDragRegularShape* pOp = new (OpDragRegularShape);
			if (pOp != NULL)
			{
				pOp->DoStartDragEdit( this, PointerPos, pSpread, OpDragRegularShape::DRAG_CENTRE, TRUE);
			}
			return TRUE;
			break;
		}
		default:
			return FALSE;
	}
	return FALSE; // shouldn't get here but suppresses a warning
}



/*******************************************************************************************

>	BOOL NodeRegularShape::ClickPrimary(DocCoord PointerPos, ClickType Click,
									ClickModifiers ClickMods, Spread* pSpread, BOOL DragCentre)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		PointerPos - the location of the click
				Click - the type of the click
				ClickMods - the current click modifiers
				pSpread - pointer to the spread containing the shape
	Outputs:	-
	Returns:	TRUE if the click is used / FALSE if it isn't
	Purpose:	This function will handle clicks on any primary point on a shape
				Single - unused
				Double - Toggle curvature
				Drag - Rotate and resize shape
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL NodeRegularShape::ClickPrimary(DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread* pSpread, BOOL DragCentre)
{
	switch (Click)
	{
		case CLICKTYPE_DOUBLE:
		{
			// A double click on a stellation point is used to toggle the shapes curvature state
			BOOL Result = ToggleCurvature();
			ERROR2IF(!Result, FALSE, "Toggle curvature failed - OpEditRegularShape is ill");
			return TRUE;
			break;
		}
		case CLICKTYPE_DRAG:
		{
			OpDragRegularShape* pOp = new (OpDragRegularShape);
			if (pOp != NULL)
			{
				pOp->DoStartDragEdit( this, PointerPos, pSpread, OpDragRegularShape::DRAG_PRIMARY, DragCentre);
			}
			return TRUE;
			break;
		}
		default:
			return FALSE;
	}
}



/*******************************************************************************************

>	BOOL NodeRegularShape::ClickStellation(DocCoord PointerPos, ClickType Click,
													ClickModifiers ClickMods, Spread* pSpread)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		PointerPos - the location of the click
				Click - the type of the click
				ClickMods - the current click modifiers
				pSpread - pointer to the spread containing the shape
	Outputs:	-
	Returns:	TRUE if the click is used / FALSE if it isn't
	Purpose:	This function will handle clicks on any stellation point on a shape
				Single - unused
				Double - Toggle curvature
				Drag - Change stellation offset and radius
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL NodeRegularShape::ClickStellation(DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread* pSpread)
{
	switch (Click)
	{
		case CLICKTYPE_DOUBLE:
		{
			// A double click on a stellation point is used to toggle the shapes curvature state
			BOOL Result = ToggleCurvature();
			ERROR2IF(!Result, FALSE, "Toggle curvature failed - OpEditRegularShape is ill");
			return TRUE;
			break;
		}
		case CLICKTYPE_DRAG:
		{
			OpDragRegularShape* pOp = new (OpDragRegularShape);
			if (pOp != NULL)
			{
				pOp->DoStartDragEdit( this, PointerPos, pSpread, OpDragRegularShape::DRAG_STELLATION, TRUE);
			}
			return TRUE;
			break;
		}
		default:
			return FALSE;
	}
}



/*******************************************************************************************

>	BOOL NodeRegularShape::ClickPCurve(DocCoord PointerPos, ClickType Click,
					ClickModifiers ClickMods, Spread* pSpread, DocCoord Start, DocCoord End)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		PointerPos - the location of the click
				Click - the type of the click
				ClickMods - the current click modifiers
				pSpread - pointer to the spread containing the shape
				Start - the start of the edge line to drag along
				End - the end of the edge line to drag along
	Outputs:	-
	Returns:	TRUE if the click is used / FALSE if it isn't
	Purpose:	This function will handle clicks on any primay curvature blob on a shape
				Single - unused
				Double - Toggle curvature
				Drag - Change curvature ratio
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL NodeRegularShape::ClickPCurve(DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread* pSpread, DocCoord Start, DocCoord End)
{
	switch (Click)
	{
		case CLICKTYPE_DOUBLE:
		{
			BOOL Result = ToggleCurvature();
			ERROR2IF(!Result, FALSE, "Toggle curvature failed - OpEditRegularShape is ill");
			return TRUE;
			break;
		}
		case CLICKTYPE_DRAG:
		{
			// A drag is used to change the primary curvature ratio
			OpDragRegularShape* pOp = new (OpDragRegularShape);
			if (pOp != NULL)
			{
				pOp->DoStartDragEditCurve( this, PointerPos, pSpread,
														OpDragRegularShape::DRAG_PRIMARYCURVE, Start, End);
			}	
			return TRUE;
			break;
		}
		default:
			return FALSE;
	}
}



/*******************************************************************************************

>	BOOL NodeRegularShape::ClickSCurve(DocCoord PointerPos, ClickType Click,
					ClickModifiers ClickMods, Spread* pSpread, DocCoord Start, DocCoord End)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		PointerPos - the location of the click
				Click - the type of the click
				ClickMods - the current click modifiers
				pSpread - pointer to the spread containing the shape
				Start - the start of the edge line to drag along
				End - the end of the edge line to drag along
	Outputs:	-
	Returns:	TRUE if the click is used / FALSE if it isn't
	Purpose:	This function will handle clicks on any stellation curvature blob on a shape
				Single - unused
				Double - Toggle curvature
				Drag - Change curvature ratio
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL NodeRegularShape::ClickSCurve(DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread* pSpread, DocCoord Start, DocCoord End)
{
	switch (Click)
	{
		case CLICKTYPE_DOUBLE:
		{
			BOOL Result = ToggleCurvature();
			ERROR2IF(!Result, FALSE, "Toggle curvature failed - OpEditRegularShape is ill");
			return TRUE;
			break;
		}
		case CLICKTYPE_DRAG:
		{
			// A drag is used to change the stellation curvature ratio
			OpDragRegularShape* pOp = new (OpDragRegularShape);
			if (pOp != NULL)
			{
				pOp->DoStartDragEditCurve( this, PointerPos, pSpread,
													OpDragRegularShape::DRAG_STELLATIONCURVE, Start, End);
			}
			return TRUE;
			break;
		}
		default:
			return FALSE;
	}
}



/*******************************************************************************************

>	BOOL NodeRegularShape::ClickEdge(DocCoord PointerPos, ClickType Click,
										ClickModifiers ClickMods, Spread* pSpread, BOOL EdgeOne)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		PointerPos - the location of the click
				Click - the type of the click
				ClickMods - the current click modifiers
				pSpread - pointer to the spread containing the shape
				EdgeOne - TRUE if EdgePath1 is being dragged, FALSE if EdgePath2 is being dragged.
	Outputs:	-
	Returns:	TRUE if the click is used / FALSE if it isn't
	Purpose:	This function will handle clicks on the edge of a shape.
				SingleClick - unused
				DoubleClick - If constrained then reset the path edit to a straight line
							  In unconstrained then toggle stellation state
				Drag - Reform the edge path 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL NodeRegularShape::ClickEdge(DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread* pSpread, BOOL EdgeOne)
{
	switch (Click)
	{
		case CLICKTYPE_DOUBLE:
		{
			if (ClickMods.Constrain)
			{
				if (!MakeStraight(EdgeOne))
					InformError();
			}
			else
			{
				// A double click on an edge is used to toggle the shapes stellation state
				EditRegularShapeParam ChangeData(this);
				if (IsStellated())
					ChangeData.NewStellated = EditRegularShapeParam::CHANGE_SETFALSE;
				else
					ChangeData.NewStellated = EditRegularShapeParam::CHANGE_SETTRUE;
				OpDescriptor* Apple = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpEditRegularShape));
				if (Apple != NULL)
					Apple->Invoke(&ChangeData);
			}
			return TRUE;
			break;
		}
		case CLICKTYPE_DRAG:
		{
			// A drag on edges is used to reform the edge path
			if (!IsCircular())
			{
				double pdist;
				INT32 tempel;
				Path* RenderPath = NULL;
				BuildShapePath(&RenderPath);
				RenderPath->SqrDistanceToPoint(PointerPos, &tempel, &pdist);

				OpReformShapeEdge* pOpReshape = new OpReformShapeEdge;
				if (pOpReshape == NULL)
					InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
				else
				{
					pOpReshape->DoStartDragEdit(this, EdgeOne, PointerPos, pSpread, tempel);
				}
			}
			return TRUE;
			break;
		}
		default:
			return FALSE;
	}
}

#endif

/*******************************************************************************************

>	BOOL NodeRegularShape::IsReformed()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if either of the shapes edges have been reformed / FALSE if they havent
	Purpose:	This function is used to see if a shape has been reformed or not.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL NodeRegularShape::IsReformed()
{
	INT32 loop;
	DocCoord* Coords1 = EdgePath1.GetCoordArray();
	INT32 NumCoords1 = EdgePath1.GetNumCoords()-1;
	DocCoord* Coords2 = EdgePath2.GetCoordArray();
	INT32 NumCoords2 = EdgePath2.GetNumCoords()-1;

	// If it's a line then it can't have been reformed
	if ((NumCoords1 == 2) && (NumCoords2 ==2))
		return FALSE;

	for (loop = 1; loop < NumCoords1 ; loop++)
	{
		if (DocCoord::DistanceFromLine(Coords1[0], Coords1[NumCoords1], Coords1[loop]) > 1.0)
			return TRUE;
	}

	for (loop = 1; loop < NumCoords2 ; loop++)
	{
		if (DocCoord::DistanceFromLine(Coords2[0], Coords2[NumCoords2], Coords2[loop]) > 1.0)
			return TRUE;
	}

	return FALSE;
}



/*******************************************************************************************

>	double NodeRegularShape::GetRotationAngle()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Inputs:		-
	Outputs:	-
	Returns:	The angle of rotation of this shape
	Purpose:	Call this function to get the current angle of rotation of this shape.
				This can be ascertained from the centre point and the major axes point.
	Errors:		-
	SeeAlso:	NodeRenderableBounded::GetRotationAngle

********************************************************************************************/

double NodeRegularShape::GetRotationAngle()
{
	DocCoord Offset = GetMajorAxes() - GetCentrePoint();
	double Angle = atan2((double)Offset.y, (double)Offset.x);
	if (Angle == HUGE_VAL)
		Angle = 0.0;

	return Angle;
}



/*******************************************************************************************

>	UINT32 NodeRegularShape::GetNumSides() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		-
	Outputs:	-
	Returns:	The number of sides the object has (circular objects return their 'hidden'
				number of sides)
	Purpose:	Call this function to get the number of sides in the shape
	Errors:		-
	SeeAlso:	NodeRegularShape::SetNumSides

********************************************************************************************/
UINT32 NodeRegularShape::GetNumSides() const
{
	return NumSides;
}



/*******************************************************************************************

>	void NodeRegularShape::SetNumSides(INT32 NewNumSides)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		NewNumSides - The new number of sides
	Outputs:	-
	Returns:	-
	Purpose:	Call this function to set the number of side in the current object 
	Errors:		-
	SeeAlso:	NodeRegularShape::GetNumSides

********************************************************************************************/
void NodeRegularShape::SetNumSides(UINT32 NewNumSides)
{
	if (NewNumSides >= REGULARSHAPE_MINSIDES)
	{	NumSides = NewNumSides; 
		InvalidateCache();
	}
	else
	{
		ERROR3_PF(("Attempted to set %d number of sides",NewNumSides));
	}
}		       



/*******************************************************************************************

>	BOOL NodeRegularShape::IsCircular() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the shape is a circle, FALSE if it is a polygon
	Purpose:	Call this function to test the circular state of the shape.
	Errors:		-
	SeeAlso:	NodeRegularShape::SetCircular

********************************************************************************************/
BOOL NodeRegularShape::IsCircular() const
{
	// Note the bitfield has values 0 or -1 so we change it to a real BOOL
	return (Circular != 0);
}



/*******************************************************************************************

>	void NodeRegularShape::SetCircular(BOOL Value)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		TRUE to make this shape circular - FALSE to make it a polygon
	Outputs:	-
	Returns:	-
	Purpose:	Call this function to set the circular state of the shape
	Errors:		-
	SeeAlso:	NodeRegularShape::IsCircular

********************************************************************************************/
void NodeRegularShape::SetCircular(BOOL Value)
{
	Circular = Value;
	InvalidateCache();
}



/*******************************************************************************************

>	BOOL NodeRegularShape::IsStellated() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this shape is stellated - FALSE if it isn't
	Purpose:	Call this function to test the stallatioin state of a shape
	Errors:		-
	SeeAlso:	NodeRegularShape::SetStellated

********************************************************************************************/
BOOL NodeRegularShape::IsStellated() const
{
	// Note the bitfield has values 0 or -1 so we change it to a real BOOL
	return (Stellated != 0);
}



/*******************************************************************************************

>	void NodeRegularShape::SetStellated(BOOL Value)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		Value - TRUE to make the shape stellated, FALSE to make it non-stellated
	Outputs:	-
	Returns:	-
	Purpose:	Call this function to set the stellation state of the shape
	Errors:		-
	SeeAlso:	NodeRegularShape::IsStellated

********************************************************************************************/
void NodeRegularShape::SetStellated(BOOL Value)
{
	Stellated = Value;
	InvalidateCache();
}			   



/*******************************************************************************************

>	BOOL NodeRegularShape::IsPrimaryCurvature() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the shape has primary curvature, FALSE if it dosent
	Purpose:	Call this function to test the primary curvature state of a shape
	Errors:		-
	SeeAlso:	NodeRegularShape::SetPrimaryCurvature

********************************************************************************************/
BOOL NodeRegularShape::IsPrimaryCurvature() const
{
	// Note the bitfield has values 0 or -1 so we change it to a real BOOL
	return (PrimaryCurvature != 0);
}



/*******************************************************************************************

>	void NodeRegularShape::SetPrimaryCurvature(BOOL Value)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		Valus - TRUE to give the shape primary curvature - FALSE to remove it
	Outputs:	-
	Returns:	-
	Purpose:	Call this function to set the primary curvature state of the shape
	Errors:		-
	SeeAlso:	NodeRegularShape::IsPrimaryCurvature

********************************************************************************************/
void NodeRegularShape::SetPrimaryCurvature(BOOL Value)
{
	PrimaryCurvature = Value;
	InvalidateCache();
}



/*******************************************************************************************

>	BOOL NodeRegularShape::IsStellationCurvature() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the shape has stellation curvature - FALSE if it hasn't
	Purpose:	Call this function to test the state of the shapes stellation curvature
	Errors:		-
	SeeAlso:	NodeRegularShape::SetStellationCurvature

********************************************************************************************/
BOOL NodeRegularShape::IsStellationCurvature() const
{
	// Note the bitfield has values 0 or -1 so we change it to a real BOOL
	return (StellationCurvature != 0);
}



/*******************************************************************************************

>	void NodeRegularShape::SetStellationCurvature(BOOL Value)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		Value - TRUE to give the shape stellation curvature - FALSE to clear it
	Outputs:	-
	Returns:	-
	Purpose:	Call this function to set the stellation curvature state of the shape
	Errors:		-
	SeeAlso:	NodeRegularShape::IsStellationCurvature

********************************************************************************************/
void NodeRegularShape::SetStellationCurvature(BOOL Value)
{
	StellationCurvature = Value;
	InvalidateCache();
}



/*******************************************************************************************

>	double NodeRegularShape::GetStellRadiusToPrimary() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		-
	Outputs:	-
	Returns:	The ratio of the stellation radius to the primary radius.
	Purpose:	Call this function to get the stellation radius ratio
	Errors:		-
	SeeAlso:	NodeRegularShape::SetStellRadiusToPrimary

********************************************************************************************/
double NodeRegularShape::GetStellRadiusToPrimary() const
{
	return StellRadiusToPrimary;
}



/*******************************************************************************************

>	void NodeRegularShape::SetStellRadiusToPrimary(double Value)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		Value - the new stellation radius ratio
	Outputs:	-
	Returns:	-
	Purpose:	Call this function to set the stellation radius ratio
	Errors:		-
	SeeAlso:	NodeRegularShape::GetStellRadiusToPrimary

********************************************************************************************/
void NodeRegularShape::SetStellRadiusToPrimary(double Value)
{
	StellRadiusToPrimary = Value;
	InvalidateCache();
}			       



/*******************************************************************************************

>	double NodeRegularShape::GetPrimaryCurveToPrimary() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		-
	Outputs:	-
	Returns:	The primary curvature ratio (it's a ratio of the distance along the line
				connecting a primary point to the next primary/stellation point
	Purpose:	Call this function to get the primary curvature ratio
	Errors:		-
	SeeAlso:	NodeRegularShape::SetPrimaryCurveToPrimary

********************************************************************************************/
double NodeRegularShape::GetPrimaryCurveToPrimary() const
{
	return PrimaryCurveToPrimary;
}



/*******************************************************************************************

>	void NodeRegularShape::SetPrimaryCurveToPrimary(double Value)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		Value - the new value of the primary curvature ratio
	Outputs:	-
	Returns:	-
	Purpose:	Call this function to set the value of the primary curvature ratio 
	Errors:		-
	SeeAlso:	NodeRegularShape::GetPrimaryCurveToPrimary

********************************************************************************************/
void NodeRegularShape::SetPrimaryCurveToPrimary(double Value)
{
	PrimaryCurveToPrimary = Value;
	InvalidateCache();
}



/*******************************************************************************************

>	double NodeRegularShape::GetStellCurveToStell() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		-
	Outputs:	-
	Returns:	The stellation curvature ratio (it's a ratio of the distance along the line
				connecting a stellation point to the next primary point)
	Purpose:	Call this function to get the stellation curvature ratio
	Errors:		-
	SeeAlso:	NodeRegularShape::SetStellCurveToStell

********************************************************************************************/
double NodeRegularShape::GetStellCurveToStell() const
{
	return StellCurveToStell;
}



/*******************************************************************************************

>	void NodeRegularShape::GetRotationAngle(double Value)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		Value - the new stellation curvature ratio
	Outputs:	-
	Returns:	-
	Purpose:	Call this function to set the stellation curvature ratio
	Errors:		-
	SeeAlso:	NodeRegularShape::GetStellCurveToStell

********************************************************************************************/
void NodeRegularShape::SetStellCurveToStell(double Value)
{
	StellCurveToStell = Value;
	InvalidateCache();
}			       



/*******************************************************************************************

>	double NodeRegularShape::GetStellationRatio() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		-
	Outputs:	-
	Returns:	The stellation offset ratio.  This is a ratio of something to something else
	Purpose:	Call this function to get the stellation offset ratio
	Errors:		-
	SeeAlso:	NodeRegularShape::SetStellationRatio

********************************************************************************************/
double NodeRegularShape::GetStellationRatio() const
{
	return StellOffsetRatio;
}



/*******************************************************************************************

>	void NodeRegularShape::SetStellationRatio(double Value)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		Value - the new value of the stellation offset
	Outputs:	-
	Returns:	-
	Purpose:	Call this function to set the stellation offset 
	Errors:		-
	SeeAlso:	NodeRegularShape::GetStellationRatio

********************************************************************************************/
void NodeRegularShape::SetStellationRatio(double Value)
{
	StellOffsetRatio = Value;
	InvalidateCache();
}



/*******************************************************************************************

>	DocCoord NodeRegularShape::GetCentrePoint()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		-
	Outputs:	-
	Returns:	The location of the transfomed centre point
	Purpose:	Call this function to get the current centre point of the shape
	Errors:		-
	SeeAlso:	NodeRegularShape::SetCentrePoint

********************************************************************************************/
DocCoord NodeRegularShape::GetCentrePoint()
{
	DocCoord Result = UTCentrePoint;
	TransformMatrix.transform(&Result);
	return Result;
}



/*******************************************************************************************

>	DocCoord NodeRegularShape::GetMajorAxes()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		-
	Outputs:	-
	Returns:	The transformed major point of the shape
	Purpose:	Call this function to get the shapes current major point
	Errors:		-
	SeeAlso:	NodeRegularShape::SetMajorAxes

********************************************************************************************/
DocCoord NodeRegularShape::GetMajorAxes()
{
	DocCoord Result = UTMajorAxes;
	TransformMatrix.transform(&Result);
	return Result;
}


/*******************************************************************************************

>	DocCoord NodeRegularShape::GetMinorAxes()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		-
	Outputs:	-
	Returns:	The transformed minor point of the shape
	Purpose:	Call this function to get the current minor point of the shape
	Errors:		-
	SeeAlso:	NodeRegularShape::SetMinorAxes

********************************************************************************************/
DocCoord NodeRegularShape::GetMinorAxes()
{
	DocCoord Result = UTMinorAxes;
	TransformMatrix.transform(&Result);
	return Result;
}



/*******************************************************************************************

>	void NodeRegularShape::SetCentrePoint(DocCoord Value)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		Value - the new centre point of the shape
	Outputs:	-
	Returns:	-
	Purpose:	Call this function to set the untransformed centre point of the shape
				DO NOT use this function to move shapes around - apply transforms to it instead
	Errors:		-
	SeeAlso:	NodeRegularShape::GetCentrePoint

********************************************************************************************/
void NodeRegularShape::SetCentrePoint(DocCoord Value)
{
	UTCentrePoint = Value;
	InvalidateCache();
}			       



/*******************************************************************************************

>	void NodeRegularShape::SetMinorAxes(DocCoord Value)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		Value - the new location of the minor point
	Outputs:	-
	Returns:	-
	Purpose:	Call this function to set the location of the minor axes point
				DO NOT use this function to move shapes around - apply transforms to it instead
	Errors:		-
	SeeAlso:	NodeRegularShape::GetMinorAxes

********************************************************************************************/
void NodeRegularShape::SetMinorAxes(DocCoord Value)
{
	UTMinorAxes = Value;
	InvalidateCache();
}			       



/*******************************************************************************************

>	void NodeRegularShape::SetMajorAxes(DocCoord Value)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Inputs:		Value - the new location of the major point
	Outputs:	-
	Returns:	-
	Purpose:	Call this function to set the location of the major axes point 
				DO NOT use this function to move shapes around - apply transforms to it instead
	Errors:		-
	SeeAlso:	NodeRegularShape::GetMajorAxes

********************************************************************************************/
void NodeRegularShape::SetMajorAxes(DocCoord Value)
{
	UTMajorAxes = Value;
	InvalidateCache();
}		



/*******************************************************************************************

>	void NodeRegularShape::GetTransformMatrix(Matrix* store) const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Inputs:		store - points to a Matrix object
	Outputs:	store will contain the shapes current transform matrix
	Returns:	-
	Purpose:	Call this function to get the shapes current transformation matrix
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void NodeRegularShape::GetTransformMatrix(Matrix* store) const
{
	*store = TransformMatrix;
}



/*******************************************************************************************

>	void NodeRegularShape::SetTransformMatrix(const Matrix* newmatrix)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Inputs:		newmatrix - points to a Trans2DMatrix object
	Outputs:	-
	Returns:	-
	Purpose:	Call this function to set the shapes current transformation matrix
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void NodeRegularShape::SetTransformMatrix(const Matrix* newmatrix)
{
	TransformMatrix = *newmatrix;
}



/*******************************************************************************************

>	void NodeRegularShape::PreExportRender(RenderRegion* pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Inputs:		pRegion - points to the export render region
	Outputs:	-
	Returns:	-
	Purpose:	This function is called just before the shape is exported.  It outputs the 
				shapes start token
	Errors:		-
	SeeAlso:	NodeRegularShape::ExportRender

********************************************************************************************/
void NodeRegularShape::PreExportRender(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(NativeRenderRegion)))
	{
		// Output "start regular shape" token
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
		pDC->OutputToken("csrs");
		pDC->OutputNewLine();
	}
#endif
}



/*******************************************************************************************

>	void NodeRegularShape::ExportRender(RenderRegion* pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Inputs:		pRegion - points to the export render region
	Outputs:	-
	Returns:	-
	Purpose:	This function is called to export a regular shape as EPS
	Errors:		-
	SeeAlso:	NodeRegularShape::PreExportRender

********************************************************************************************/
BOOL NodeRegularShape::ExportRender(RenderRegion* pRegion) 
{
#ifdef DO_EXPORT
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(NativeRenderRegion)))
	{
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();

		// Work out if the shape is filled or stroked.
		INT32 IsFilled = TRUE;
		StrokeColourAttribute *pStrokeColour =
			(StrokeColourAttribute *) pRegion->GetCurrentAttribute(ATTR_STROKECOLOUR);
		INT32 IsStroked = !pStrokeColour->Colour.IsTransparent();

		ColourFillAttribute *pFillAttr = 
			(ColourFillAttribute *) pRegion->GetCurrentAttribute(ATTR_FILLGEOMETRY);
	
		if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(FlatFillAttribute)) &&
			pFillAttr->Colour.IsTransparent())
		{
			// Flat fill with transparent colour => no fill
			IsFilled = FALSE;
		}

		// Output regular shape parameters
		pDC->OutputValue((INT32)NumSides);
		pDC->OutputValue((INT32)Circular);
		pDC->OutputValue((INT32)Stellated);
		pDC->OutputValue((INT32)PrimaryCurvature);
		pDC->OutputValue((INT32)StellationCurvature);
		pDC->OutputReal(StellRadiusToPrimary);
		pDC->OutputReal(PrimaryCurveToPrimary);
		pDC->OutputReal(StellCurveToStell);
		pDC->OutputReal(StellOffsetRatio);
		pDC->OutputCoord(UTCentrePoint);
		pDC->OutputCoord(UTMajorAxes);
		pDC->OutputCoord(UTMinorAxes);
		pDC->OutputValue(IsFilled);
		pDC->OutputValue(IsStroked);
		pDC->OutputToken("crsp");
		pDC->OutputNewLine();

		// Output the transformation matrix
		fixed16 billy[4];
		INT32 bobby[2];
		TransformMatrix.GetComponents(billy, bobby);
		pDC->OutputReal(billy[0].MakeDouble());
		pDC->OutputReal(billy[1].MakeDouble());
		pDC->OutputReal(billy[2].MakeDouble());
		pDC->OutputReal(billy[3].MakeDouble());
		pDC->OutputUserSpaceValue(bobby[0]);
		pDC->OutputUserSpaceValue(bobby[1]);
		pDC->OutputToken("crstm");
		pDC->OutputNewLine();

		// Output the paths for the shape's two edges
		((EPSRenderRegion*)pRegion)->ExportPath(&EdgePath1, TRUE);
		pDC->OutputToken("crsp1");
		pDC->OutputNewLine();
		((EPSRenderRegion*)pRegion)->ExportPath(&EdgePath2, TRUE);
		pDC->OutputToken("crsp2");
		pDC->OutputNewLine();

		// End the regular shape object
		pDC->OutputToken("cers");
		pDC->OutputNewLine();
				
		// Tell caller we rendered ourselves ok
		return TRUE;
	}

	// This is necessary, because this ExportRender function hides the custom export code
	// used by the Flash render region in its base class.
	else if ( pRegion->IsKindOf ( CC_RUNTIME_CLASS ( FlashRenderRegion ) ) )
	{
		// Call the function to render a stroked shape.
		return static_cast<FlashRenderRegion*> ( pRegion )->ExportRenderableNode ( this );
	}

#endif
	// Render the node in the normal way
	return FALSE;
}

/*******************************************************************************************

>	void NodeRegularShape::EmergencyFixShape()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/03/95
	Inputs:		-
	Outputs:	UTMajorPoint,UTMinorPoint and UTCentrePoint may be moved.  If they are then
				the transform matrix is reset.
	Returns:	-
	Purpose:	This function checks to see if either the transformed major or minor axis
				points are sitting on the transformed centre point.  If they are then the
				shape is bodged to fix up the situation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void NodeRegularShape::EmergencyFixShape()
{
	DocCoord CurrentCentre = GetCentrePoint();
	DocCoord CurrentMajor = GetMajorAxes();
	DocCoord CurrentMinor = GetMinorAxes();

	if ( (CurrentCentre.Distance(CurrentMajor) == 0.0) ||
		 (CurrentCentre.Distance(CurrentMinor) == 0.0) )
	{
		SetCentrePoint(CurrentCentre);
		SetMajorAxes(DocCoord(CurrentCentre.x+100, CurrentCentre.y));
		SetMinorAxes(DocCoord(CurrentCentre.x, CurrentCentre.y+100));

		Matrix fred( FIXED16(1), FIXED16(0), FIXED16(0), FIXED16(1), 0, 0 );
		SetTransformMatrix(&fred);
	}
}



/*******************************************************************************************

>	BOOL NodeRegularShape::IsARectangle()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this shape is a rectangle
				FALSE if it isn't
	Purpose:	Offical, approved, way of seeing if a RegularShape is a rectangluar QuickShape
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL NodeRegularShape::IsARectangle() const
{
	return (!IsCircular() && (GetNumSides() == 4) && !IsStellated());
}


/******************************************************************************************

>	INT32 NodeRegularShape::GetRectWidth()
	
	  Author:	Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	  Created:	08/08/96
	  Inputs: -
	  Outputs: The rectangle's width if it's a rectangle
				-1 if it's not
	  Purpose: Use this function to get the width  

******************************************************************************************/
INT32 NodeRegularShape::GetRectWidth() 
{
	if(!IsARectangle())
		return -1;

	double minorRadius = GetCentrePoint().Distance(GetMinorAxes());
	return (INT32)(minorRadius * cos(PI/4));
}

/******************************************************************************************

>	INT32 NodeRegularShape::GetRectHeight()
	
	  Author:	Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	  Created:	08/08/96
	  Inputs: -
	  Outputs: The rectangle's height if it's a rectangle
				-1 if it's not
	  Purpose: Use this function to get the height 

******************************************************************************************/
INT32 NodeRegularShape::GetRectHeight()
{
	if(!IsARectangle())
		return -1;

	double MajorRadius = GetCentrePoint().Distance(GetMajorAxes());
	return (INT32)(MajorRadius * cos(PI/4));
}

/********************************************************************************************

>	virtual BOOL NodeRegularShape::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/04/95
	Inputs:		pSpread		The spread in which things are happening
				PointerPos	The Location of the mouse pointer at the time of the click
				pMenu		The menu to which items should be added
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the QuickShape to respond to pop up menu clicks on itself.

********************************************************************************************/

BOOL NodeRegularShape::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	BOOL ok = TRUE;
	
PORTNOTE("other","Removed ContextMenu usage")
#ifndef EXCLUDE_FROM_XARALX
	ok = ok && pMenu->BuildCommand(TOOL_OPTOKEN_REGSHAPE, TRUE);

	ok = ok && pMenu->BuildCommand(OPTOKEN_TOGGLEELIPPOLY, TRUE);
	ok = ok && pMenu->BuildCommand(OPTOKEN_TOGGLESTELLATION);
	ok = ok && pMenu->BuildCommand(OPTOKEN_TOGGLECURVATURE, TRUE);

	ok = ok && pMenu->BuildCommand(OPTOKEN_QUICKSHAPE_NUMBERSIDES,TRUE);
	MenuItem* pNumberRoot = pMenu->GetLastItem();
	ok = ok && pMenu->BuildCommand(OPTOKEN_QUICKSHAPE_NUMBERSIDES3, FALSE, pNumberRoot);
	ok = ok && pMenu->BuildCommand(OPTOKEN_QUICKSHAPE_NUMBERSIDES4, FALSE, pNumberRoot);
	ok = ok && pMenu->BuildCommand(OPTOKEN_QUICKSHAPE_NUMBERSIDES5, FALSE, pNumberRoot);
	ok = ok && pMenu->BuildCommand(OPTOKEN_QUICKSHAPE_NUMBERSIDES6, FALSE, pNumberRoot);
	ok = ok && pMenu->BuildCommand(OPTOKEN_QUICKSHAPE_NUMBERSIDES7, FALSE, pNumberRoot);
	ok = ok && pMenu->BuildCommand(OPTOKEN_QUICKSHAPE_NUMBERSIDES8, FALSE, pNumberRoot);
	ok = ok && pMenu->BuildCommand(OPTOKEN_QUICKSHAPE_NUMBERSIDES9, FALSE, pNumberRoot);
	ok = ok && pMenu->BuildCommand(OPTOKEN_QUICKSHAPE_NUMBERSIDES10, FALSE, pNumberRoot);
#endif

	return ok;
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	BOOL NodeRegularShape::IsARegularShape()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/95
	Returns:	TRUE => This node is an instance of NodeRegularShape
				FALSE => otherwise.
	Purpose:	Determine if a node is a QuickShape object.

********************************************************************************************/

BOOL NodeRegularShape::IsARegularShape() const
{
	return TRUE;
}



/********************************************************************************************

>	double NodeRegularShape::GetMajorRadiusSize()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/05/95
	Inputs:		-
	Outputs:	-
	Returns:	The length of the radius of the shape, ie the distance from the centre of the
				shape to a primary point, or a stellation point, whichever is the longer.
	Purpose:	Gets the length of the longest radius of the shape.  

********************************************************************************************/
double NodeRegularShape::GetMajorRadiusSize()
{
	DocCoord OuterMajor = UTMajorAxes;
	if (IsStellated() && (GetStellRadiusToPrimary() > 1.0))
		PositionPointFromRatio(&OuterMajor, &UTMajorAxes, GetStellRadiusToPrimary());

	return UTCentrePoint.Distance(OuterMajor);
}





/********************************************************************************************
>	BOOL NodeRegularShape::MakeRectangle(const INT32 Width, const INT32 Height, const INT32 CurvatureRadius)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/95
	Inputs:		Width - the width of the rectangle
				Height - the height of the rectangle
				CurvatureRadius - the length of the curvature radius.  0 for no curvature
	Outputs:	Affects member variables of the shape
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called when importing rectangles.  Sets up a pre-created QuickShape to have
				all the required features.  Note that it is centered at the origin.  Apply
				furthur transforms as required
********************************************************************************************/
BOOL NodeRegularShape::MakeRectangle(const INT32 Width, const INT32 Height, const INT32 CurvatureRadius)
{
	ERROR3IF(CurvatureRadius < 0, "CurvatureRadius was -ve");

	// Set the flags
	SetNumSides(4);
	SetCircular(FALSE);
	SetStellated(FALSE);

	// To make life easy lets pretend we have a square
	const double SqrHalfHeight = ((double)Height/2)*((double)Height/2);
	const INT32 MajorLength = (INT32)(sqrt( SqrHalfHeight + SqrHalfHeight ));

	SetCentrePoint(DocCoord(0,0));
	SetMajorAxes(DocCoord(0, MajorLength));
	SetMinorAxes(DocCoord(MajorLength, 0));

	// Set the curvature
	if (CurvatureRadius < 1)
	{
		PrimaryCurvature = FALSE;	
		StellationCurvature = FALSE;
	}
	else
	{
		PrimaryCurvature = TRUE;	
		StellationCurvature = TRUE;
		PrimaryCurveToPrimary = ((double)CurvatureRadius) / ((double)MajorLength);
		StellCurveToStell = PrimaryCurveToPrimary;
	}

	// Reset the matrix
	Matrix Unity;					// Default constructor gives unity
	SetTransformMatrix(&Unity);

	DocCoord NewMinor = DocCoord::PositionPointFromRatio(UTCentrePoint, UTMinorAxes, (double)Width/Height);
	SetMinorAxes(NewMinor);

	EmergencyFixShape();			// Bodge the shape if width/height are zero

	return TRUE;
}



/********************************************************************************************
>	BOOL NodeRegularShape::MakeEllipse(const INT32 Width, const INT32 Height)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/95
	Inputs:		Width - the width of the ellipse
				Height - the height of the ellipse
	Outputs:	Affects member variables of the shape
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called when importing ellipse.  Sets up a pre-created QuickShape to have
				all the required features.  Note that it is centered at the origin.  Apply
				furthur transforms as required
********************************************************************************************/
BOOL NodeRegularShape::MakeEllipse(const INT32 Width, const INT32 Height)
{
	// Set the flags
	SetNumSides(6);
	SetCircular(TRUE);
	SetStellated(FALSE);

	SetCentrePoint(DocCoord(0,0));
	SetMajorAxes(DocCoord(0, Height));
	SetMinorAxes(DocCoord(Width, 0));

	// Reset the matrix
	Matrix Unity;					// Default constructor gives unity
	SetTransformMatrix(&Unity);

	EmergencyFixShape();			// Bodge the shape if width/height are zero

	return TRUE;
}

/********************************************************************************************

>	BOOL NodeRegularShape::WritePreChildrenNative(BaseCamelotFilter * pFilter)
	BOOL NodeRegularShape::WritePreChildrenWeb(BaseCamelotFilter * pFilter)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/96
	Inputs:		pFilter - filter to use
	Returns:	Boolean value indicating success
	Purpose:	Writes this regular shape to the filter

********************************************************************************************/

BOOL NodeRegularShape::WritePreChildrenNative(BaseCamelotFilter * pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(this == NULL, FALSE, "this == NULL");
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	TRACEUSER( "Diccon", _T("Exporting Regular Shape\n"));
	String_256 Details = String_256((TCHAR*)(this->GetRuntimeClass()->m_lpszClassName));
	TRACEUSER( "Diccon", _T("(%s - 0x%x)\n"),(TCHAR*)Details,this);
	return CXaraFileRegularShape::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL NodeRegularShape::WritePreChildrenWeb(BaseCamelotFilter * pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(this == NULL, FALSE, "this == NULL");
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");

	return CXaraFileRegularShape::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

  

/********************************************************************************************
>	void NodeRegularShape::DeleteCachedPath()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/96
	Inputs:		-
	Returns:	-
	Purpose:	Deletes the shapes cached render path
********************************************************************************************/
void NodeRegularShape::DeleteCachedPath()
{
	if (CachedRenderPath != NULL)
	{
		delete CachedRenderPath;
		CachedRenderPath = NULL;
	}

	// if we have an applied brush attribute then flush the cache
	AttrBrushType* pBrush = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), (NodeAttribute**)&pBrush);
	if (pBrush && pBrush->GetBrushHandle() != BrushHandle_NoBrush)
		pBrush->FlushCache();


	PathCacheInvalid = TRUE;
}

/********************************************************************************************
>	PathShape NodeRegularShape::GetPathShape()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Inputs:		-
	Returns:	An ENUM to say which shape the path is. Possible return
				values are:
				PATHSHAPE_RECTANGLE
				PATHSHAPE_SQUARE
				PATHSHAPE_ELLIPSE
				PATHSHAPE_CIRCLE
				PATHSHAPE_PATH	(the default)

	Purpose:	Gets a value to say which shape the path is. This value
				is passed to RenderRegion::DrawPath.

  See Also:		RenderRegion::DrawPath, ImagemapRenderRegion::DrawPathToOutputDevice
********************************************************************************************/
PathShape NodeRegularShape::GetPathShape()
{
	//First test to see if the QuickShape is a rectangle
	if (IsARectangle() && !IsReformed())
	{
		//Yes it is

		//Now, is it a square?
		if (AxesAreEqual() && AxesArePerpendicular())
		{
			//Yes it is.

			//Has it been rotated?
			if (IsRotated())
				return PATHSHAPE_SQUARE_ROTATED;
			else
				return PATHSHAPE_SQUARE;
		}
		else
		{
			//No, it's not a square, so it's a normal rectangle.

			//Has it been rotated?
			if (IsRotated())
				return PATHSHAPE_RECTANGLE_ROTATED;
			else
				return PATHSHAPE_RECTANGLE;
		}
	}

	//Now let's find if the QuickShape is an ellipse
	if (IsCircular())
	{
		//Yes, it's an ellipse

		//Now, is it a circle?
		if (AxesAreEqual() && AxesArePerpendicular())
		{
			//Yes it is.

			return PATHSHAPE_CIRCLE;
		}
		else
		{
			//No, it's not a circle, just a normal ellipse

			//Has it been rotated?
			if (IsRotated())
				return PATHSHAPE_ELLIPSE_ROTATED;
			else
				return PATHSHAPE_ELLIPSE;
		}
	}

	//It's neither an ellipse or a rectangle.
	//So return the generic "Path" shape
	
	return PATHSHAPE_PATH;
}

/********************************************************************************************
>	BOOL NodeRegularShape::IsRotated()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Inputs:		-
	Returns:	TRUE if the shape has been rotated
				FALSE otherwise

	Purpose:	Finds if the shape has been rotated

				The shape is defined as not being rotated if
				a. It has been rotated by 0 degrees
				b. It has been rotated by any multiple of (2PI/N) where
					N is the number of sides of the shape.

				The reasoning behind B is that, if a regular shape with
				N sides is rotated by 2PI/N, it will appear to be an
				unrotated regular shape.

				For example, say you start with a rectangle, whose sides
				are exactly parallel to the X and Y axes - that is, an unrotated
				rectangle.

				Then you rotated it by any multiple of 2PI/4 - that is, you rotate
				it by 90, 180, 270 or 360 degrees.

				In any case, the rectangle you end up with still has its sides
				parallel to the X and Y axes - that is, it is still an unrotated
				rectangle.

				This is important in imagemaps: where clickable regions
				can be defined for an unrotated rectangle (one with its sides
				parallel to the X and Y axes) but not for a rotated rectangle.

  See Also:		NodeRegularShape::GetPathShape(), 
				ImagemapRenderRegion::DrawPathToOutputDevice
********************************************************************************************/
BOOL NodeRegularShape::IsRotated()
{
	//First get the absolute angle by which this shape is rotated
	double dAngle=fabs(GetRotationAngle());

	//Now get the angle by which the shape is allowed to be rotated
	//and still defined as unrotated
	double dAllowedAngle=(double) ((2*PI)/GetNumSides());

	//Now check if the angle by which the shape is rotated is
	//equal to any multiple of dAllowedAngle
	for (double dCheckAngle=0; dCheckAngle<PI; dCheckAngle+=dAllowedAngle)
	{
		//If dCheckAngle==dAngle to within a given tolerance,
		//return FALSE
		double dDifference=fabs(dAngle-dCheckAngle);

		if (dDifference<QUICKSHAPE_ANGLE_TOLERANCE)
			return FALSE;
	}

	//No, the shape has not been rotated
	return TRUE;
	
}

/********************************************************************************************
>	BOOL NodeRegularShape::AxesAreEqual()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Inputs:		-
	Returns:	TRUE if the major and minor axes are the same length
				FALSE otherwise

	Purpose:	Finds if the major and minor axes are the same length

  See Also:		NodeRegularShape::GetPathShape()
********************************************************************************************/
BOOL NodeRegularShape::AxesAreEqual()
{
	//Get the centre point and major and minor axis points
	DocCoord dcCentre=GetCentrePoint();
	DocCoord dcMajor=GetMajorAxes();
	DocCoord dcMinor=GetMinorAxes();

	//Get the lengths of the major and minor axes
	double dMajorLength=dcCentre.Distance(dcMajor);
	double dMinorLength=dcCentre.Distance(dcMinor);

	//And return TRUE if the major and minor axes
	//are equal to within a set tolerance
	double dDifference=fabs(dMajorLength-dMinorLength);

	return (dDifference<QUICKSHAPE_RADIUS_TOLERANCE);

}

/********************************************************************************************
>	BOOL NodeRegularShape::AxesArePerpendicular()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Inputs:		-
	Returns:	TRUE if the major and minor axes are perpendicular
				FALSE otherwise

	Purpose:	Finds if the major and minor axes are perpendicular

  See Also:		NodeRegularShape::GetPathShape()
********************************************************************************************/
BOOL NodeRegularShape::AxesArePerpendicular()
{
	//Get the centre point and major and minor axis points
	DocCoord dcCentre=GetCentrePoint();
	DocCoord dcMajor=GetMajorAxes();
	DocCoord dcMinor=GetMinorAxes();

	//Now get a vector between dcCentre and dcMajor
	//For convenience, I won't actually create a vector class...I'll
	//just store the elements of the vector in longs
	INT32 lMajorVectorX=dcMajor.x-dcCentre.x;
	INT32 lMajorVectorY=dcMajor.y-dcCentre.y;

	//And get a vector between dcCentre and dcMinor
	INT32 lMinorVectorX=dcMinor.x-dcCentre.x;
	INT32 lMinorVectorY=dcMinor.y-dcCentre.y;

	//And now get the scalar product between them
	INT32 lScalarProduct=lMajorVectorX*lMinorVectorX+lMajorVectorY*lMinorVectorY;
							 
	//And return TRUE if the scalar product is zero to a set tolerance
	INT32 lDifference=labs(lScalarProduct);

	return (lDifference<QUICKSHAPE_SCALARPRODUCT_TOLERANCE);
}



/********************************************************************************************

>	DocRect NodeRegularShape::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/1999
	Inputs:		ExtParams		parameters describing the extension.
	Outputs:	
	Returns:	TRUE if this regular shape can be validly extended,
				FALSE otherwise.
	Purpose:	Tests to see whether this shape's extend-centre is positioned so as to make
				an extend operation irreversible.
	Errors:		
	See also:	

********************************************************************************************/
DocRect NodeRegularShape::ValidateExtend(const ExtendParams& ExtParams)
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

>	void NodeRegularShape::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/1999
	Inputs:		ExtParams		parameters describing the extension.
	Outputs:	this NodeRegularShape will be extended in accordance with ExtParams.
	Returns:	
	Purpose:	Perform an Extend operation on this shape. Behaviour is as follows:

				* the shape extends separately along each axis.
				* if the shape is asked to stretch, it scales along the corresponding axes.
				* if the shape is asked to extend, it is translated as a whole, as described
																				by ExtParams.
	Errors:		
	See also:	class Extender

********************************************************************************************/
void NodeRegularShape::Extend(const ExtendParams& ExtParams)
{
	// do the extension operations on ourself.
	TransformStretchObject(ExtParams);
	TransformTranslateObject(ExtParams);

	// do the base-class implementation to extend our children.
	Node::Extend(ExtParams);
}



/********************************************************************************************

>	virtual INT32 NodeRegularShape::EstimateNodeComplexity (OpParam* details)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/09/2000

	Inputs:		details		any data that should be used for the calculation

	Outputs:	-

	Returns:	an estimate of the nodes complexity

	Purpose:	This function estimates a complexity value for the node.  The complexity
				value is based upon the total length of all paths in the node.

	See Also:	OpBlendNodes::DeterminBlendObjectsProcessorHit ()

********************************************************************************************/

INT32 NodeRegularShape::EstimateNodeComplexity (OpParam* details)
{
	if (CachedRenderPath)
	{
		return (CachedRenderPath->GetUsedSlots ());
	}
	else
	{
		return ((EdgePath1.GetUsedSlots ()) + (EdgePath2.GetUsedSlots ()));
	}
}
