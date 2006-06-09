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
// The Scanning Render Region class

/*
*/

#include "camtypes.h"
#include "scanrr.h"
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodebmp.h"
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "lineattr.h"
#include "nodebldr.h"

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNAMIC(ScanningRenderRegion, RenderRegion);
CC_IMPLEMENT_DYNAMIC(NodeListItemWithComplexity, NodeListItem)

// Proper memory dumps
#define new CAM_DEBUG_NEW




/********************************************************************************************

>	ScanningRenderRegion::ScanningRenderRegion(BOOL Printing)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	The Scanning Render Regions constructor. This sets all the member vars of
				the class to default values (usually 0 or NULL).

********************************************************************************************/

ScanningRenderRegion::ScanningRenderRegion(BOOL Printing)
{
	// The complex nodes if there are any. Default to none
	pFirstComplex = NULL;
	pLastComplex = NULL;

	// Counts of objects, default to none
	NumComplexShapes = 0;

	// The bounds of the complex objects
	ComplexBounds.MakeEmpty();

	// Flag to say if we have finished rendering or not
	IsFinishedRendering = FALSE;
	
	// Assume the host render region can do nothing till we get told otherwise
	HostCaps.CanDoNothing();

	// Start by assuming that the first node is simple for now
	IsFirstComplex = FALSE;
	InComplexRun = FALSE;
	RunLength = 0;
	
	// The list info
	NodeRuns.DeleteAll();
	pCurrentListItem = NULL;
	IsThisTheFirstNode = FALSE;

	bIsComplex = FALSE;
	bIsSimple = FALSE;
	pLastClipAttr = NULL;

	// Render flags
	RenderFlags.Printing = Printing;
}



/********************************************************************************************

>	ScanningRenderRegion::~ScanningRenderRegion()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Destructor - It will do something if it needs to but at present this function
				is empty

********************************************************************************************/

ScanningRenderRegion::~ScanningRenderRegion()
{
	// Make sure the list is cleared out.
	NodeRuns.DeleteAll();
}


/********************************************************************************************

>	virtual BOOL ScanningRenderRegion::StartRender()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Returns:	TRUE if everything went OK, FALSE if there was a problem
	Purpose:	Called to start the rendering process. Allows the render region to get
				ready to start rendering
	SeeAlso:	ScanningRenderRegion::StopRender

********************************************************************************************/

BOOL ScanningRenderRegion::StartRender()
{
	// Call base class first
	if (!RenderRegion::StartRender())
		return FALSE;

	// Say that we have not rendered any nodes yet
	IsThisTheFirstNode = TRUE;

	// All worked
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL ScanningRenderRegion::StopRender()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Called to stop rendering. Since Scanning render regions are not used to
				background render, this marks the end of all rendering. This function
				marks the render region as completed, so all the get functions that return
				info about the objects found in the region start to function.

********************************************************************************************/

BOOL ScanningRenderRegion::StopRender()
{
	SetRenderState(NULL);
	// Update the RunLength of the last span
	NodeListItemWithComplexity* pLast = (NodeListItemWithComplexity*)NodeRuns.GetTail();
	if (pLast)
	{
//		TRACEUSER( "Gerry", _T("Just finished a run of %ld %s shapes\n", RunLength, pLast->wasClassifiedSimple ? "simple" : "complex"));
		pLast->RunLength = RunLength;
		pLast->BoundsRect = SpanBounds;
	}
	IsFinishedRendering = TRUE;
	return TRUE;
}



/********************************************************************************************

>	virtual void ScanningRenderRegion::DrawPathToOutputDevice(Path* pPathToRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		pPathToRender - The path that we want to try and render
	Purpose:	This function will examine the attributes etc to see if this represents a
				simple or compex shape and update the counters that this class keeps.
				There is no actual rendering, as this class does not actually render anything
				to a device.

********************************************************************************************/

void ScanningRenderRegion::DrawPathToOutputDevice(Path* pPathToRender, PathShape)
{
	// see if this is a complex shape
	if (TestForComplexShape(&HostCaps))
		ComplexShapeFound();
	else
		SimpleShapeFound();
}


/********************************************************************************************

>	virtual void ScanningRenderRegion::DrawRect(DocRect* pRectToRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		pRectToRender - The Rectangle that we need to render
	Purpose:	This function will examine the attributes etc to see if this represents a
				simple or compex shape and update the counters that this class keeps.
				There is no actual rendering, as this class does not actually render anything
				to a device.

********************************************************************************************/

void ScanningRenderRegion::DrawRect(DocRect* pRectToRender)
{
	// see if this is a complex shape
	if (TestForComplexShape(&HostCaps))
		ComplexShapeFound();
	else
		SimpleShapeFound();
}


/********************************************************************************************

>	virtual void ScanningRenderRegion::DrawLine(const DocCoord &StartPoint,
												const DocCoord &EndPoint)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		StartPoint, EndPoint - the coords to draw a line between
	Purpose:	This function will examine the attributes etc to see if this represents a
				simple or compex shape and update the counters that this class keeps.
				There is no actual rendering, as this class does not actually render anything
				to a device.

********************************************************************************************/

void ScanningRenderRegion::DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint)
{
	// see if this is a complex shape
	if (TestForComplexShape(&HostCaps))
		ComplexShapeFound();
	else
		SimpleShapeFound();
}


/********************************************************************************************

>	virtual void ScanningRenderRegion::DrawPixel(const DocCoord &Point)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - the coord to draw the pixel in
	Purpose:	This function will examine the attributes etc to see if this represents a
				simple or compex shape and update the counters that this class keeps.
				There is no actual rendering, as this class does not actually render anything
				to a device.

********************************************************************************************/

void ScanningRenderRegion::DrawPixel(const DocCoord &Point)
{
	// see if this is a complex shape
	if (TestForComplexShape(&HostCaps))
		ComplexShapeFound();
	else
		SimpleShapeFound();
}


/********************************************************************************************

>	virtual void ScanningRenderRegion::DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - The position to draw the bitmap at
				pBitmap - the bitmap that needs drawing
	Purpose:	This function will examine the attributes etc to see if this represents a
				simple or compex shape and update the counters that this class keeps.
				There is no actual rendering, as this class does not actually render anything
				to a device.

********************************************************************************************/

void ScanningRenderRegion::DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap)
{
	// see if this is a complex shape
	if (TestForComplexShape(&HostCaps))
		ComplexShapeFound();
	else
		SimpleShapeFound();
}


/********************************************************************************************

>	virtual void ScanningRenderRegion::DrawBitmap(const DocCoord &Point, UINT32 BitmapID,
												UINT32 ToolID)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - the position to draw the bitmap at
				BitmapID - The ID of the bitmap to render
				ToolID - the ID of the tool that the bitmap belongs to
	Purpose:	This function will examine the attributes etc to see if this represents a
				simple or compex shape and update the counters that this class keeps.
				There is no actual rendering, as this class does not actually render anything
				to a device.

********************************************************************************************/

void ScanningRenderRegion::DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID)
{
	// see if this is a complex shape
	if (TestForComplexShape(&HostCaps))
		ComplexShapeFound();
	else
		SimpleShapeFound();
}


/********************************************************************************************

>	BOOL ScanningRenderRegion::DrawTransformedBitmap(NodeBitmap *pNodeBitmap)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/95
	Inputs:		pNodeBitmap - The bitmap to render
	Returns:	TRUE if it worked, FLASE if not
	Purpose:	Works out if it can draw a Transformed bitmap.

********************************************************************************************/

BOOL ScanningRenderRegion::DrawTransformedBitmap(NodeBitmap *pNodeBitmap)
{
	BOOL IsShapeComplex = FALSE;

	// see if this is a complex shape (e.g. transparency)
	if (TestForComplexShape(&HostCaps) || pNodeBitmap->NeedsTransparency())
	{
		ComplexShapeFound();
		return TRUE;
	}

	// See what kind of bitmaps this render region can do
	if (HostCaps.ArbitraryBitmaps)
	{
		// We can do anything...
		if (!IsShapeComplex)
			SimpleShapeFound();

		return TRUE;
	}

	if (HostCaps.SimpleBitmaps)
	{
		// Check that the bitmap area is rectangular, and is perpendicular to
		// the x and y axes.
		if (pNodeBitmap->HasSimpleOrientation(this))
		{
			SimpleShapeFound();
			return TRUE;
		}
	}

	// If we get here, then we can't do the bitmap directly
	ComplexShapeFound();

	return TRUE;
}


/********************************************************************************************

>	virtual void ScanningRenderRegion::DrawDragRect(DocRect *RectToRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		RectToRender - The Rectangle
	Purpose:	None - I just had to overide this function as it is pure virtual. Does nothing

********************************************************************************************/

void ScanningRenderRegion::DrawDragRect(DocRect *RectToRender)
{
}


/********************************************************************************************

>	virtual void ScanningRenderRegion::DrawBlob(DocCoord p, BlobType type)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		p - A coord
				type - The type of blob
	Purpose:	None - I just had to overide this function as it is pure virtual. Does nothing

********************************************************************************************/

void ScanningRenderRegion::DrawBlob(DocCoord p, BlobType type)
{
}


/********************************************************************************************

>	virtual void ScanningRenderRegion::DrawCross(const DocCoord &Point, const UINT32 Size)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - a Coord
				Size - the size of the cross
	Purpose:	None - I just had to overide this function as it is pure virtual. Does nothing

********************************************************************************************/

void ScanningRenderRegion::DrawCross(const DocCoord &Point, const UINT32 Size)
{
}


/********************************************************************************************

>	virtual void ScanningRenderRegion::DrawBitmapBlob(const DocCoord &Point,
													KernelBitmap* BlobShape)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - A Coord
				BlobShape - the bitmap to use
	Purpose:	None - I just had to overide this function as it is pure virtual. Does nothing

********************************************************************************************/

void ScanningRenderRegion::DrawBitmapBlob(const DocCoord &Point, KernelBitmap* BlobShape)
{
}


/********************************************************************************************

>	virtual void ScanningRenderRegion::DrawBitmapBlob(const DocCoord &Point, char TCHAR* pszBitmap)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		Point - A coord
				BitmapID - the ID of a bitmap
				ToolID - the Id of the tool the bitmap resource is in
	Purpose:	None - I just had to overide this function as it is pure virtual. Does nothing

********************************************************************************************/

void ScanningRenderRegion::DrawBitmapBlob(const DocCoord &Point, ResourceID resID )
{
}



/********************************************************************************************

>	virtual void ScanningRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Outputs:	pCaps - The details about what types of thing this render region can render
	Purpose:	This function allows render regions to admit to what they can and can not
				render. This allows other areas of the program to come in and help render
				regions out in some situations, if they are unable to render everything.
				eg. an OSRenderRegion can not render transparancy.

********************************************************************************************/

void ScanningRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)
{
	pCaps->CanDoAll();
}


/********************************************************************************************

>	void ScanningRenderRegion::SetHostRRCaps(const RRCaps& Caps)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		Caps - The Device Caps for the render region we are scanning on behalf of.
	Purpose:	This function makes a note of what things the render region we are scanning
				on behalf of can render and what it can not. We use the info take a copy
				if pCaps and use it to make the decisions as to what is a complex object
				and what is not.

********************************************************************************************/

void ScanningRenderRegion::SetHostRRCaps(const RRCaps& Caps)
{
	// make a copy of the RRCaps field
	// Host caps is used in TestForComplexShape();
	HostCaps = Caps;
}




/********************************************************************************************

>	Node* ScanningRenderRegion::GetFirstComplexObject()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Returns:	A pointer to a Node
	Purpose:	This function will return a pointer to the first complex (ie one that can
				not be rendered without GDraw - such as a transparant shape) node that the
				render region would need to render. This function will return NULL if there
				were no complex shapes found.
				If the Scanning render region has not yet finished rendering, then this
				function will always return NULL
	SeeAlso:	ScanningRenderRegion::GetLastComplexObject;
				ScanningRenderRegion::IsFinished

********************************************************************************************/

Node* ScanningRenderRegion::GetFirstComplexObject()
{
	// If we have not yet finished rendering then this function returns NULL
	if (!IsFinishedRendering)
		return NULL;

	// Its finished so we these values are accurate
	return pFirstComplex;
}


/********************************************************************************************

>	Node* ScanningRenderRegion::GetLastComplexObject()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Returns:	A pointer to a Node
	Purpose:	This function will return a pointer to the last complex (ie one that can
				not be rendered without GDraw - such as a transparant shape) node that the
				render region would need to render. This function will return NULL if there
				were no complex shapes found.
				If the Scanning render region has not yet finished rendering, then this
				function will always return NULL
	SeeAlso:	ScanningRenderRegion::GetLastComplexObject;
				ScanningRenderRegion::IsFinished

********************************************************************************************/

Node* ScanningRenderRegion::GetLastComplexObject()
{
	// If we have not yet finished rendering then this function returns NULL
	if (!IsFinishedRendering)
		return NULL;

	// We have finished, so return the true value
	return pLastComplex;
}




/********************************************************************************************

>	INT32 ScanningRenderRegion::GetNumComplex()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Returns:	The number of complex nodes that need to be rendered
	Purpose:	This function returns the number of complex shapes that have been found in
				the scanning render region. It only counts the complex shapes in the region.
				It will return 0 if there were no complex shapes in the region.
				If the Scanning render region has not yet finished rendering, then this
				function will always return 0

********************************************************************************************/

INT32 ScanningRenderRegion::GetNumComplex()
{
	// If we have not yet finished rendering then this function returns 0
	if (!IsFinishedRendering)
		return 0;

	return NumComplexShapes;
}




/********************************************************************************************

>	DocRect ScanningRenderRegion::GetComplexBounds()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Returns:	DocRect - the smallest rect that covers all the complex shapes
				in this render region
	Purpose:	Used to find out the bounding rect of all the complex shapes in the completed
				render region. If it is called before the region has finished rendering, then
				it will return the empty rectangle. If there are no complex shapes in the
				region then it will also return the empty rectangle.

********************************************************************************************/

DocRect ScanningRenderRegion::GetComplexBounds()
{
	// If we have not yet finished rendering then this function returns an empty rectangle
	if (!IsFinishedRendering)
	{
		DocRect EmptyOne(0,0,0,0);
		return EmptyOne;
	}

	return ComplexBounds;
}


/********************************************************************************************

>	void ScanningRenderRegion::SetRenderState(Node* pNode)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/05/2004
	Inputs:		pNode - The Node about to be rendered
	Purpose:	Sets the next Node to be rendered

********************************************************************************************/

void ScanningRenderRegion::SetRenderState(Node* pNode)
{
	// If we have been rendering a Node then stick it's details where needed
//	Node* pLastNode = GetRenderState();
//	if (pLastNode)
//	{
		if (bIsComplex || (bIsSimple && GetCaptureDepth() > 1))
			ComplexShapeFoundHelper();			// go ahead and record the node
		else if (bIsSimple)
			SimpleShapeFoundHelper();			// go ahead and record the node
//	}

	RenderRegion::SetRenderState(pNode);
	bIsComplex = FALSE;
	bIsSimple = FALSE;
}


/********************************************************************************************

>	void ScanningRenderRegion::ComplexShapeFoundWrapper ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/2000
	Purpose:	This function is a public wrapper for ComplexShapeFound().  You should only
				call this function if you know what you are doing!

				e.g.  if you have geometry that does not regenerate certain parts of itself
				when inside blends (hence missing the complex classification), you should
				detect this situation and call this function directly.  NodeBevel::Render()
				uses this method at present.

********************************************************************************************/

void ScanningRenderRegion::ComplexShapeFoundWrapper()
{
	ComplexShapeFound();
}


/********************************************************************************************

>	void ScanningRenderRegion::ComplexShapeFoundHelper()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/2000
	Purpose:	This function is called every time the rendering functions find that the
				node they are dealing with is complex. It updates all the info the scanning
				render region tracks about complex shapes, such as the first and last complex
				nodes and the bounding rectangle of all the complex shapes.

********************************************************************************************/
void ScanningRenderRegion::ComplexShapeFoundHelper()
{
//	TRACE( _T("ComplexShapeFound\n"));

	Node* pThisNode = GetRenderState();
	if (pThisNode == NULL)
	{
		TRACE( _T("NULL RenderState in ComplexShapeFoundHelper\n"));
	}

	// Update the count of complex shapes
	NumComplexShapes++;

	// update the first complex shape pointers as needed
	if (pFirstComplex==NULL)
		pFirstComplex = pThisNode;

	// This is the last complex shape we know about so far...
	pLastComplex = pThisNode;

	// Now deal with the runs of Nodes system, in case they want to use that.
	if (IsThisTheFirstNode)
	{
		// This node represents the first renderable node in the region
		// so make sure that we note the fact that the first node is complex
		IsFirstComplex = TRUE;
		IsThisTheFirstNode = FALSE;
		InComplexRun = TRUE;

		NodeListItemWithComplexity* pItem = new NodeListItemWithComplexity(pThisNode, FALSE);
		if (pItem!=NULL)
			NodeRuns.AddTail(pItem);
	}

	// Karim 12/12/2000
	// Clipping changes - basically, all objects in a run of complex shapes
	// must share the same clip-attr.
	if (pThisNode)
	{
		// if we were in the middle of a Simple Nodes Run, then record this node
		// as the first node in the Complex run and mark that we are now in a complex run
		if (!InComplexRun)
		{
//			TRACEUSER( "Gerry", _T("Just finished a run of %ld simple shapes - Add Next Complex shape\n"), RunLength);
			NodeListItemWithComplexity* pLast = (NodeListItemWithComplexity*)NodeRuns.GetTail();
			if (pLast)
				pLast->RunLength = RunLength;

			SpanBounds.MakeEmpty();

			// Add this node to the list
			RunLength = 0;
			NodeListItemWithComplexity* pItem = new NodeListItemWithComplexity(pThisNode, FALSE);
			if (pItem!=NULL)
				NodeRuns.AddTail(pItem);
		}

		// Update the bounds to date of complex shapes
		if (pThisNode->IsBounded())
		{
			DocRect NodeBounds = ((NodeRenderableBounded*)pThisNode)->GetBoundingRect();
			ComplexBounds = NodeBounds.Union(ComplexBounds);
			SpanBounds = NodeBounds.Union(SpanBounds);
//			TRACE( _T("BoundsRect = (%d, %d) - (%d, %d)\n"), SpanBounds.lox, SpanBounds.loy, SpanBounds.hix, SpanBounds.hiy);
		}
	}

	// Make sure we know we are rendering complex shapes now
	InComplexRun = TRUE;
	RunLength++;
}


/********************************************************************************************

>	void ScanningRenderRegion::SimpleShapeFoundHelper ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/2000
	Purpose:	This function should be called whenever a simple node is encountered. This
				allows the list of runs of simple / complex nodes to be maintained.

********************************************************************************************/
void ScanningRenderRegion::SimpleShapeFoundHelper ()
{
//	TRACE( _T("SimpleShapeFound\n"));

	Node* pThisNode = GetRenderState();
	if (pThisNode == NULL)
	{
		TRACE( _T("NULL RenderState in SimpleShapeFoundHelper\n"));
	}

	if (IsThisTheFirstNode)
	{
		// This node represents the first renderable node in the region
		// so make sure that we note the fact that the first node is simple
		IsThisTheFirstNode = FALSE;

		NodeListItemWithComplexity* pItem = new NodeListItemWithComplexity(pThisNode, TRUE);
		if (pItem!=NULL)
			NodeRuns.AddTail(pItem);
	}

	// If we are in the middle of a Complex run then change over to a simple run
	if (InComplexRun)
	{
		// Add this node to the list as the first node in a simple run
//		TRACEUSER( "Gerry", _T("Just finished a run of %ld complex shapes - Add Next Simple shape\n"), RunLength);

		NodeListItemWithComplexity* pLast = (NodeListItemWithComplexity*)NodeRuns.GetTail();
		if (pLast)
		{
			pLast->RunLength = RunLength;
			pLast->BoundsRect = SpanBounds;
		}
		
		SpanBounds.MakeEmpty();
		RunLength = 0;
		NodeListItemWithComplexity* pItem = NULL;
		if (pThisNode)
		{
			// If this is the first non-attribute sibling then make the new simple
			// run start with our parent and continue up the tree
			while (TRUE)
			{
				// Get the previous sibling
				Node* pTestNode = pThisNode->FindPrevious();
				// If we don't have one or it is an attribute then get our parent
				// and try again otherwise use this node
				if (!pTestNode || pTestNode->IsAnAttribute())
					pThisNode = pThisNode->FindParent();
				else
					break;
			}
			
			pItem = new NodeListItemWithComplexity(pThisNode, TRUE);
		}

		if (pItem != NULL)
			NodeRuns.AddTail(pItem);
	}

	// We are no longer in a complex run
	InComplexRun = FALSE;
	RunLength++;
}



/********************************************************************************************

>	BOOL ScanningRenderRegion::IsFinished()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Returns:	TRUE if the scanning render region has finished scanning, FALSE if it still
				has more nodes to look at
	Purpose:	Lets you find out if the render region has finished rendering yet. If it
				has not finished then all the information that this class has to offer about
				the number of complex shapes etc is not valid. All the info functions will
				return 0 or NULL until the rendering is finished to stop you looking at
				incomplete data.

********************************************************************************************/

BOOL ScanningRenderRegion::IsFinished()
{
	return IsFinishedRendering;
}






/********************************************************************************************

>	BOOL ScanningRenderRegion::IsFirstNodeComplex()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/95
	Returns:	TRUE if the First node to be rendered is Complex, FALSE if not
	Purpose:	

********************************************************************************************/

BOOL ScanningRenderRegion::IsFirstNodeComplex()
{
	return IsFirstComplex;
}



/********************************************************************************************

>	Node* ScanningRenderRegion::FindFirstFromList()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/95
	Returns:	The first node in the list or NULL.
	Purpose:	The nodes in the list represent the first node in a run of simple / complex
				nodes. You can determine if the first node to be rendered is simple or
				complex by calling IsFirstNodeComplex(). eg. If IsFirstNodeComplex() returned
				FALSE, then all the nodes up to the node returned by FindFirstFromList() will
				be simple. All the nodes from this one to the node returned by FindNextFromList
				will be complex. This toggle carries on all the way through the tree until
				there are no more nodes.
				If this function returns NULL, you can assume that there are no runs of nodes
				in the tree and all the nodes are of the same start as the first one (ie the
				same as what IsFirstNodeComplex() says).

********************************************************************************************/

Node* ScanningRenderRegion::FindFirstFromList()
{
	// Reset our list pointer
	pCurrentListItem = NULL;

	// If we have not finished yet, then return NULL
	if (!IsFinishedRendering)
	{
		return NULL;
	}

	// return the first item in the list
	pCurrentListItem = (NodeListItemWithComplexity*) NodeRuns.GetHead();
	if (pCurrentListItem!=NULL)
		return pCurrentListItem->pNode;

	// the list was empty
	return NULL;
}




/********************************************************************************************

>	Node* ScanningRenderRegion::FindNextFromList()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/95
	Returns:	The Next Node in the list of runs of nodes
	Purpose:	Finds the first node in the next run of nodes.
	SeeAlso:	ScanningRenderRegion::FirstFirstFromList

********************************************************************************************/

Node* ScanningRenderRegion::FindNextFromList()
{
	// If we have not finished rendering, or we are not looking at a list already, return null
	if ((!IsFinishedRendering) || (pCurrentListItem==NULL))
	{
		return NULL;
	}

	// return the next item in the list
	pCurrentListItem = (NodeListItemWithComplexity*) NodeRuns.GetNext(pCurrentListItem);
	if (pCurrentListItem!=NULL)
		return pCurrentListItem->pNode;

	// the list was empty
	return NULL;
}

/********************************************************************************************

>	BOOL ScanningRenderRegion::IsThisNodeComplex()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/01/05
	Returns:	TRUE if the currentlist node is Complex, FALSE if not
	Purpose:	

********************************************************************************************/

BOOL ScanningRenderRegion::IsThisNodeComplex()
{
	if ((!IsFinishedRendering) || (pCurrentListItem==NULL))
	{
		return FALSE;
	}

	return(!(pCurrentListItem->wasClassifiedSimple));
}


/********************************************************************************************

>	DocRect ScanningRenderRegion::GetSpanBounds()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/01/05
	Returns:	DocRect - the smallest rect that covers all the shapes in this span
				in this render region
	Purpose:	Used to find out the bounding rect of all the shapes in the span. This 
				rectanlge is only calculated for complex spans.  If it is called before 
				the region has finished rendering, then it will return the empty rectangle.
				If there are no complex shapes in the region then it will also return the 
				empty rectangle.

********************************************************************************************/

DocRect ScanningRenderRegion::GetSpanBounds()
{
	// If we have not yet finished rendering then this function returns an empty rectangle
	if (!IsFinishedRendering || !pCurrentListItem)
	{
		DocRect EmptyOne(0,0,0,0);
		return EmptyOne;
	}

	return pCurrentListItem->BoundsRect;
}




/********************************************************************************************

>	virtual void ScanningRenderRegion::InitClipping()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Sets up the clipping - does nothing in this class

********************************************************************************************/

void ScanningRenderRegion::InitClipping()
{
}


/********************************************************************************************

>	virtual void ScanningRenderRegion::InitAttributes()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Sets up the attributes. Does nothing in this class

********************************************************************************************/

void ScanningRenderRegion::InitAttributes()
{
}


/********************************************************************************************

>	virtual void ScanningRenderRegion::SetLineAttributes()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Does nothing in this class

********************************************************************************************/

void ScanningRenderRegion::SetLineAttributes()
{
}


/********************************************************************************************

>	virtual void ScanningRenderRegion::SetOSDrawingMode()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Does nothing in this class

********************************************************************************************/

void ScanningRenderRegion::SetOSDrawingMode()
{
}


/********************************************************************************************

>	virtual void ScanningRenderRegion::SetQualityLevel()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Does nothing in this class

********************************************************************************************/

void ScanningRenderRegion::SetQualityLevel()
{
}


/********************************************************************************************

>	virtual MILLIPOINT ScanningRenderRegion::CalcPixelWidth()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Does nothing in this class

********************************************************************************************/

MILLIPOINT ScanningRenderRegion::CalcPixelWidth()
{
	return 0;
}


/********************************************************************************************

>	virtual MILLIPOINT ScanningRenderRegion::CalcScaledPixelWidth()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	Does nothing in this class

********************************************************************************************/

MILLIPOINT ScanningRenderRegion::CalcScaledPixelWidth()
{

	return 0;
}

#ifdef _DEBUG
void ScanningRenderRegion::DumpNodeRuns()
{
	NodeListItemWithComplexity* pItem = (NodeListItemWithComplexity*) NodeRuns.GetHead();
	while (pItem)
	{
		if (pItem->wasClassifiedSimple)
		{
			TRACE( _T("NormalSpan of %d nodes  0x%08x - %s\n"), pItem->RunLength, pItem->pNode,
				pItem->pNode->GetRuntimeClass()->GetClassName());
		}
		else
		{
			TRACE( _T("ComplexSpan of %d nodes 0x%08x - %s\n"), pItem->RunLength, pItem->pNode,
				pItem->pNode->GetRuntimeClass()->GetClassName());
		}
		TRACE( _T("BoundsRect = (%d, %d) - (%d, %d)\n"), pItem->BoundsRect.lo.x, pItem->BoundsRect.lo.y,
													pItem->BoundsRect.hi.x, pItem->BoundsRect.hi.y);
		pItem = (NodeListItemWithComplexity*) NodeRuns.GetNext(pItem);
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of NodeListItemWithComplexity
/////////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	NodeListItemWithComplexity::NodeListItemWithComplexity()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/2000
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	default constructor. Initialises pNode = NULL
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

NodeListItemWithComplexity::NodeListItemWithComplexity()
{
	pNode = NULL;
	wasClassifiedSimple = TRUE;
	RunLength = 0;
}

/********************************************************************************************

>	NodeListItemWithComplexity::NodeListItemWithComplexity(Node* WhichNode, BOOL val)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for NodeListItem which sets the pNode variable to WhichNode
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

NodeListItemWithComplexity::NodeListItemWithComplexity(Node* WhichNode, BOOL val)
{
	pNode = WhichNode;
	wasClassifiedSimple = val;
	RunLength = 0;
}
