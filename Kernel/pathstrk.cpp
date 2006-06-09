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
// Implementation of Path Stroking classes (used in stroke providing)

#include "camtypes.h"

#include "pathstrk.h"

//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "pathtrap.h"
#include "valfunc.h"


//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "colormgr.h"
#include "dbugtree.h"
//#include "fillattr.h"		// For debug/test code  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "layer.h"
#include "nodepath.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]


CC_IMPLEMENT_MEMDUMP(PathStroker, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(PathStrokerVector, PathStroker)


// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


///////////////////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////////////////

const double Flatness = 350.0;		// Flatness criteria in millipoints. (Could be based on zoom level later)
const double Flatness2 = Flatness*Flatness;

const INT32 MaxRecursionDepth = 20;	// Recursion depth at which mapping of complex curves is stopped.
									// This stops stack overflows when the width function contains a discontinuity



///////////////////////////////////////////////////////////////////////////////////////////
// PathStroker class
///////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************************************

>	PathStroker::PathStroker()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Purpose:	Default Constructor - DO NOT USE THIS FORM!

******************************************************************************************/

PathStroker::PathStroker()
{
	ERROR3("Please don't use this PathStroker constructor again");
}


/******************************************************************************************

>	PathStroker::PathStroker(ValueFunction *WidthFunction, INT32 LineWidth, LineCapType CapType)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		WidthFunction - NULL, or a pointer to a width function for the stroke
				LineWidth	  - The maximum width, in millipoints, of the stroke
				CapType		  - The cap style (LineCapButt, LineCapRound, or LineCapSquare)

	Purpose:	Constructor

******************************************************************************************/

PathStroker::PathStroker(ValueFunction *WidthFunction, INT32 LineWidth, LineCapType CapType)
{
	ERROR3IF(WidthFunction == NULL, "Illegal NULL param");
	pWidthFunction = WidthFunction;

	MaxWidth = LineWidth / 2;	// MaxWidth is the RADIUS of the "brush", not diameter

	CapStyle = CapType;

	RecursionDepth = 0;			// Reset recursion depth counter to 0
}



/******************************************************************************************

>	virtual BOOL PathStroker::StrokePath(LineCapType CapType, TrapsList *pTraps, Path *pOutput)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		pTraps		- A trapezoidal description of the path to be stroked
							  (May not be NULL)

	Outputs:	pOutput		- This path is filled in with a "filled outline" of the stroke
							  It's IsFilled flag will be TRUE, IsStroked will be FALSE.

	Returns:	FALSE if it fails. (No error will be set at this level)

	Purpose:	"Strokes" a path according to the "style" set in construction.
				
				This is the second stage in the stroking process (the first stage being
				breaking a path down into a trapezoidal description).

				Stroking is achieved by mapping the intersections of trapezoid edges
				with the width function, and then recursively flattening the mapped
				curve between those points until required "flatness" is achieved.

				The stroke outline is built up of 5 main components:
											A MOVETO point 1
					1+---------------+2		A mapped outline between 1 & 2 ("FORWARDS")
					 |				 |		A line cap between 2 & 3
					4+---------------+3		A mapped outline between 3 & 4 ("BACKWARDS")
											A line cap between 4 & 1

				The distance between points on the forwards & backwards outlines
				(the stroke width) is governed by the LineWidth and WidthFunction
				passed to the constructor.

	SeeAlso:	PathStroker::MapLineForwards;
				PathStroker::MapLineBackwards;
				PathStroker::MapLineCap

******************************************************************************************/

BOOL PathStroker::StrokePath(TrapsList *pTraps, Path *pOutput)
{
	ERROR3IF(pTraps == NULL || pOutput == NULL, "Illegal NULL params");

	// For each trapezoid list generated, add a subpath to the pOutput path
	for (UINT32 Index = 0; Index < pTraps->GetNumTraps(); Index++)
	{
		TrapEdgeList *pTrapEdges = pTraps->GetTrapEdgeList(Index);
		if (pTrapEdges->GetNumEdges() < 2)
		{
			ERROR3("No map traps when stroking! Subpath stripped\n");
			continue;
		}

#if FALSE
		// Useful bit of code to dump out the TrapEdges
		{
			for (UINT32 i = 0; i < pTrapEdges->GetNumEdges(); i++)
			{
				TrapEdge *pEdge = pTrapEdges->GetTrapEdge(i);
				TRACE(_T("Edge % 5d  Position = %g  Type = %s  Centre = (%d, %d)  Normal = (%g, %g)\n"),
					i, pEdge->Position, 
					(pEdge->PrevTrapJoin == TrapJoin_None ? _T("N") : 
						pEdge->PrevTrapJoin == TrapJoin_Round ? _T("R") :
						pEdge->PrevTrapJoin == TrapJoin_MitredOrBevelled ? _T("M") : _T("U")),
					pEdge->Centre.x, pEdge->Centre.y,
					pEdge->Normal.x, pEdge->Normal.y);
			}
		}
#endif

		// Check if this subpath is closed (ends in a join). If so, we must remove the line caps!
		LineCapType CapType = CapStyle;
		TrapEdge *pEdge = pTrapEdges->GetLastTrapEdge();
		if (pEdge->PrevTrapJoin != TrapJoin_None)
			CapType = LineCapButt;

		// Start a new sub-path with a MoveTo to the first point
		pEdge = pTrapEdges->GetTrapEdge(0);
		INT32 Width = (INT32) (MaxWidth * pWidthFunction->GetValue(pEdge->Position));
		INT32 dx	   = (INT32) (pEdge->Normal.x * Width);
		INT32 dy	   = (INT32) (pEdge->Normal.y * Width);
		DocCoord StartPoint(pEdge->Centre.x + dx, pEdge->Centre.y + dy);
		pOutput->AddMoveTo(StartPoint);

		// Map the "forwards" outline
		MapLineForwards(pTrapEdges, pOutput);

		// Add the line cap at the end of the line
		MapLineCap(CapType, pTrapEdges->GetLastTrapEdge(), FALSE, pOutput);

		// Map the "backwards" outline
		MapLineBackwards(pTrapEdges, pOutput);

		// Add the line cap at the end of the line, and we should meet back at the start!
		MapLineCap(CapType, pTrapEdges->GetTrapEdge(0), TRUE, pOutput);

		// And set the PT_CLOSEFIGURE flag on the last point, just to be safe
		INT32			indCoord = pOutput->GetNumCoords() - 1;
		PathVerb	   *pVerbs = pOutput->GetVerbArray();
		pVerbs[indCoord] |= PT_CLOSEFIGURE;
	}

	pOutput->IsFilled	= TRUE;
	pOutput->IsStroked	= FALSE;

#ifdef _DEBUG
	if(pTraps && pTraps->GetNumTraps() > 0 && pOutput)
	{
		TRACEUSER( "Matt", _T("Path stroked: %ld subpaths, (1st is %ld Traps) -> %ld Coords\n"),
								pTraps->GetNumTraps(), pTraps->GetTrapEdgeList(0)->GetNumEdges(),
								pOutput->GetNumCoords());
	}
#endif

	return(TRUE);
}



/******************************************************************************************

>	BOOL PathStroker::MapLineForwards(TrapEdgeList *pTraps, Path *pOutput)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		pTraps	- A pointer to a trapezoid-descriptiuon of the path being stroked
							(May not be NULL)

	Outputs:	pOutput	- A pointer to a path in which the output will be generated
							(May not be NULL)

	Returns:	FALSE if it fails. (No error will be set at this level)

	Purpose:	Maps points along the trapezoids, generating an outline path for the
				"forwards" side of the stroke.

	Notes:		Expects the first point on this edge to have been written (as a MOVETO)
				to the output path before this function was called.

******************************************************************************************/

BOOL PathStroker::MapLineForwards(TrapEdgeList *pTraps, Path *pOutput)
{
	TrapEdge *pEdge		= NULL;
	TrapEdge *pLastEdge	= NULL;
	INT32	 Width;
	DocCoord LastPoint;
	DocCoord NewPoint;

	for (UINT32 Index = 0; Index < pTraps->GetNumEdges(); Index++)
	{
		pLastEdge = pEdge;
		pEdge = pTraps->GetTrapEdge(Index);

		Width		= (INT32) (MaxWidth * pWidthFunction->GetValue(pEdge->Position));
		NewPoint.x	= pEdge->Centre.x + (INT32) (pEdge->Normal.x * Width);
		NewPoint.y	= pEdge->Centre.y + (INT32) (pEdge->Normal.y * Width);

		// The first point has already been added by the caller. The first iteration
		// of this loop is just to calculate the first "LastPoint" value.
		if (pLastEdge != NULL)
		{
			RecursionDepth = 0;						// Reset recursion depth counter to 0
			RecursiveMapLineForwards(pLastEdge, LastPoint, pEdge, NewPoint, pOutput);
		}

		LastPoint = NewPoint;
	}

	return(TRUE);
}



/******************************************************************************************

>	void PathStroker::RecursiveMapLineForwards(	TrapEdge *pEdge1, DocCoord &Point1,
												TrapEdge *pEdge2, DocCoord &Point2,
												Path *pOutput)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/12/96

	Inputs:		pEdge1, Point1	- The source TrapEdge and coordinate of the first point
								  (which must have already been added to the output path)

				pEdge2, Point2	- The source TrapEdge and coordinate of the second point
								  (which must NOT have been added to the output path)

	Outputs:	pOutput	- A pointer to a path in which the output will be generated
							(May not be NULL)

	Purpose:	Recursive method which maps points within a given trapezoid by
				recursively "flattening" the curve between the two trapezoid edges.

				It inserts extra mapped points between the two original points
				passed in to it, until the flatness criteria is met.

******************************************************************************************/

void PathStroker::RecursiveMapLineForwards(	TrapEdge *pEdge1, DocCoord &Point1, TrapEdge *pEdge2, DocCoord &Point2, Path *pOutput)
{
	ERROR3IF(pEdge1 == NULL || pEdge2 == NULL || pOutput == NULL, "Illegal NULL params");

	// --- Calculate the midpoint of the curve we are mapping
	// Calculate the midpoint "position" value
	TrapEdge MidEdge;
	MidEdge.Position = (pEdge1->Position + pEdge2->Position) / 2.0;

	// Calculate the midpoint normal vector
	MidEdge.Normal.x = (pEdge1->Normal.x + pEdge2->Normal.x) / 2.0;
	MidEdge.Normal.y = (pEdge1->Normal.y + pEdge2->Normal.y) / 2.0;

	MidEdge.PrevTrapJoin = pEdge2->PrevTrapJoin;
	if (MidEdge.PrevTrapJoin == TrapJoin_MitredOrBevelled)
	{
		// Work out if this on the inside or outside of the join
		double Val = (pEdge1->Normal.x * pEdge2->Normal.y) - (pEdge1->Normal.y * pEdge2->Normal.x);
		if (Val > 0.0)
		{
			// We are doing the inside of a join

			// If this is the special mitre join edge then just skip it
			// This removes the artifacts from the inside of mitre joins
			if (fabs(pEdge2->Normal.GetLength() - 1.0) > 0.0001)
				return;

			MidEdge.Normal.Normalise();
		}
	}
	else
	{
		MidEdge.Normal.Normalise();
	}

	// Calculate the (approximate) centreline midpoint
	MidEdge.Centre.x = (pEdge1->Centre.x / 2) + (pEdge2->Centre.x / 2);
	MidEdge.Centre.y = (pEdge1->Centre.y / 2) + (pEdge2->Centre.y / 2);

	// Now, calculate the mapped midpoint
	const INT32 Width = (INT32) (MaxWidth * pWidthFunction->GetValue(MidEdge.Position));
	DocCoord MidPoint(MidEdge.Centre);
	MidPoint.x += (INT32) (MidEdge.Normal.x * Width);
	MidPoint.y += (INT32) (MidEdge.Normal.y * Width);

	// --- Calculate the midpoint of the straight line segment
	DocCoord ApproximateMidPoint(Point1.x/2 + Point2.x/2, Point1.y/2 + Point2.y/2);

	// --- Calculate the error in the midpoint position
	const INT32 dx = MidPoint.x - ApproximateMidPoint.x;
	const INT32 dy = MidPoint.y - ApproximateMidPoint.y;
	const double Dist = (double)((dx * dx) + (dy * dy));

	RecursionDepth++;

	// --- If the straight-line approximation is not close enough to the curve, recurse,
	// Don't bother to recurse for mitred or bevelled joins as they should be straight lines
	if ((MidEdge.PrevTrapJoin != TrapJoin_MitredOrBevelled) && ((Dist > Flatness) || (RecursionDepth <= pWidthFunction->GetMinimumRecursionDepth())) && (RecursionDepth <= MaxRecursionDepth))
	{
		// Recurse on the left and right sides of the new midpoint
		RecursiveMapLineForwards(pEdge1,   Point1,	 &MidEdge, MidPoint, pOutput);
		RecursiveMapLineForwards(&MidEdge, MidPoint, pEdge2,   Point2,	 pOutput);
	}
	else
	{
		pOutput->AddLineTo(Point2);
	}

	RecursionDepth--;
}



/******************************************************************************************

>	BOOL PathStroker::MapLineBackwards(TrapEdgeList *pTraps, Path *pOutput)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		pTraps	- A pointer to a trapezoid-descriptiuon of the path being stroked
							(May not be NULL)

	Outputs:	pOutput	- A pointer to a path in which the output will be generated
							(May not be NULL)

	Returns:	FALSE if it fails. (No error will be set at this level)

	Purpose:	Maps points along the trapezoids, generating an outline path for the
				"Backwards" side of the stroke.

	Notes:		Expects the first point on this edge to have been written (as a LINETO)
				to the output path before this function was called.

******************************************************************************************/

BOOL PathStroker::MapLineBackwards(TrapEdgeList *pTraps, Path *pOutput)
{
	TrapEdge *pEdge		= NULL;
	TrapEdge *pLastEdge	= NULL;
	INT32	 Width;
	DocCoord LastPoint;
	DocCoord NewPoint;

	for (INT32 Index = pTraps->GetNumEdges() - 1; Index >= 0; Index--)
	{
		pLastEdge = pEdge;
		pEdge = pTraps->GetTrapEdge(Index);

		Width		= (INT32) (MaxWidth * pWidthFunction->GetValue(pEdge->Position));
		NewPoint.x	= pEdge->Centre.x - (INT32) (pEdge->Normal.x * Width);
		NewPoint.y	= pEdge->Centre.y - (INT32) (pEdge->Normal.y * Width);

		// The first point has already been added by the caller. The first iteration
		// of this loop is just to calculate the first "LastPoint" value.
		if (pLastEdge != NULL)
		{
			RecursionDepth = 0;						// Reset recursion depth counter to 0
			RecursiveMapLineBackwards(pLastEdge, LastPoint, pEdge, NewPoint, pOutput);
		}

		LastPoint = NewPoint;
	}

	return(TRUE);
}



/******************************************************************************************

>	void PathStroker::RecursiveMapLineBackwards(TrapEdge *pEdge1, DocCoord &Point1,
												TrapEdge *pEdge2, DocCoord &Point2,
												Path *pOutput)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/12/96

	Inputs:		pEdge1, Point1	- The source TrapEdge and coordinate of the first point
								  (which must have already been added to the output path)

				pEdge2, Point2	- The source TrapEdge and coordinate of the second point
								  (which must NOT have been added to the output path)

	Outputs:	pOutput	- A pointer to a path in which the output will be generated
							(May not be NULL)

	Purpose:	Recursive method which maps points within a given trapezoid by
				recursively "flattening" the curve between the two trapezoid edges.

				It inserts extra mapped points between the two original points
				passed in to it, until the flatness criteria is met.

******************************************************************************************/

void PathStroker::RecursiveMapLineBackwards(TrapEdge *pEdge1, DocCoord &Point1, TrapEdge *pEdge2, DocCoord &Point2, Path *pOutput)
{
	ERROR3IF(pEdge1 == NULL || pEdge2 == NULL || pOutput == NULL, "Illegal NULL params");

	// --- Calculate the midpoint of the curve we are mapping
	// Calculate the midpoint "position" value
	TrapEdge MidEdge;
	MidEdge.Position = (pEdge1->Position + pEdge2->Position) / 2.0;

	// Calculate the midpoint normal vector
	MidEdge.Normal.x = (pEdge1->Normal.x + pEdge2->Normal.x) / 2.0;
	MidEdge.Normal.y = (pEdge1->Normal.y + pEdge2->Normal.y) / 2.0;

	// (Note in the 'backwards' case the "PrevTrap" flag is in pEdge1)
	MidEdge.PrevTrapJoin = pEdge1->PrevTrapJoin;
	if (MidEdge.PrevTrapJoin == TrapJoin_MitredOrBevelled)
	{
		// Work out if this on the inside or outside of the join
		double Val = (pEdge1->Normal.x * pEdge2->Normal.y) - (pEdge1->Normal.y * pEdge2->Normal.x);
		if (Val > 0.0)
		{
			// We are doing the inside of a join

			// If this is the special mitre join edge then just skip it
			// This removes the artifacts from the inside of mitre joins
			if (fabs(pEdge2->Normal.GetLength() - 1.0) > 0.0001)
				return;

			MidEdge.Normal.Normalise();
		}
	}
	else
	{
		MidEdge.Normal.Normalise();
	}

	// Calculate the (approximate) centreline midpoint
	MidEdge.Centre.x = (pEdge1->Centre.x / 2) + (pEdge2->Centre.x / 2);
	MidEdge.Centre.y = (pEdge1->Centre.y / 2) + (pEdge2->Centre.y / 2);

	// Now, calculate the mapped midpoint
	const INT32 Width = (INT32) (MaxWidth * pWidthFunction->GetValue(MidEdge.Position));
	DocCoord MidPoint(MidEdge.Centre);
	MidPoint.x -= (INT32) (MidEdge.Normal.x * Width);
	MidPoint.y -= (INT32) (MidEdge.Normal.y * Width);

	// --- Calculate the midpoint of the straight line segment
	DocCoord ApproximateMidPoint(Point1.x/2 + Point2.x/2, Point1.y/2 + Point2.y/2);

	// --- Calculate the error in the midpoint position
	const INT32 dx = MidPoint.x - ApproximateMidPoint.x;
	const INT32 dy = MidPoint.y - ApproximateMidPoint.y;
	const double Dist = (double)((dx * dx) + (dy * dy));

	RecursionDepth++;

	// --- If the straight-line approximation is not close enough to the curve, recurse,
	// Don't bother to recurse for mitred or bevelled joins as they should be straight lines
	if ((MidEdge.PrevTrapJoin != TrapJoin_MitredOrBevelled) && ((Dist > Flatness) || (RecursionDepth <= pWidthFunction->GetMinimumRecursionDepth())) && (RecursionDepth <= MaxRecursionDepth))
	{

		// Recurse on the left and right sides of the new midpoint
		RecursiveMapLineBackwards(pEdge1,   Point1,	 &MidEdge, MidPoint, pOutput);
		RecursiveMapLineBackwards(&MidEdge, MidPoint, pEdge2,   Point2,	 pOutput);
	}
	else
	{
		pOutput->AddLineTo(Point2);
	}

	RecursionDepth--;
}



/******************************************************************************************

>	BOOL PathStroker::MapLineCap(LineCapType CapType, TrapEdge *pEdge, BOOL IsStartCap,
									Path *pOutput)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/97

	Inputs:		CapType		- The cap style (LineCapButt, LineCapRound, or LineCapSquare)
				pEdge		- A pointer to the end trapezoid for this end of the stroke
							  (May not be NULL)
				IsStartCap	- TRUE if this is the start cap, FALSE if it's the end cap.
							 This lets us get the cap orientation right

	Outputs:	pOutput		- A pointer to a path in which the output will be generated
							  (May not be NULL)

	Returns:	FALSE if it fails. (No error will be set at this level)

	Purpose:	Maps a line cap onto the path by appending appropriate extra 
				path elements to the end of it.

	Notes:		This maps points from the end of the backward outline to the start
				of the forward outline. It is expected that the StartPoint of the cap
				(the end point of the backward outline) has been written to the
				output immediately beforehand.

				e.g. for the "leftmost" round cap, we have a situation like this:
							 EndPoint  +----> forward outline
									 /
									|   
						(MidPoint) +   + ----centreline (pEdge->Centre)
									|
									 \
							StartPoint +<---- backward outline

******************************************************************************************/

BOOL PathStroker::MapLineCap(LineCapType CapType, TrapEdge *pEdge, BOOL IsStartCap, Path *pOutput)
{
	ERROR3IF(pEdge == NULL || pOutput == NULL, "Illegal NULL params");

	// Work out dx,dy. These are tangential (dx) and perpendicular (dy) to the centreline
	// Orientation of the End cap is in the opposite direction to that of the start cap
	const INT32 Width		= (INT32) (MaxWidth * pWidthFunction->GetValue(pEdge->Position));
	const INT32 Orientation	= (IsStartCap) ? 1 : -1;
	const INT32 dx			= Orientation * (INT32) (pEdge->Normal.x * Width);
	const INT32 dy			= Orientation * (INT32) (pEdge->Normal.y * Width);

	// Calculate the point at the "end" of the cap outline
	DocCoord EndPoint(pEdge->Centre.x + dx, pEdge->Centre.y + dy);					// Last point on cap

	switch (CapType)
	{
		case LineCapButt:
			// Simply join the 2 outlines with a straight line
			pOutput->AddLineTo(EndPoint);
			break;


		case LineCapRound:
			{
				// We are adding a semicircular arc onto the end of the stroke. This is done
				// as two quarter circles: from the end of the outline to the extended centreline,
				// then from the centreline to the start of the opposite outline.
				DocCoord StartPoint(pEdge->Centre.x - dx, pEdge->Centre.y - dy);	// 1st point on cap
				DocCoord MidPoint(pEdge->Centre.x - dy, pEdge->Centre.y + dx);		// midpoint on the arc

				// Determine the distance the control points should be away from the endpoints
				// to get a reasonable circular arc out of the bezier curve.
				const INT32 CPdx	= (INT32) ( ((double)dx) * 0.552 );
				const INT32 CPdy	= (INT32) ( ((double)dy) * 0.552 );

				// Draw an arc from the backward outline of the stroke to the centreline
				DocCoord Control1(StartPoint.x - CPdy, StartPoint.y + CPdx);
				DocCoord Control2(MidPoint.x   - CPdx, MidPoint.y   - CPdy);
				pOutput->AddCurveTo(Control1, Control2, MidPoint);

				// Draw an arc from the centreline to the start of the forward stroke
				DocCoord Control3(MidPoint.x + CPdx, MidPoint.y + CPdy);
				DocCoord Control4(EndPoint.x - CPdy, EndPoint.y + CPdx);
				pOutput->AddCurveTo(Control3, Control4, EndPoint);
			}
			break;


		case LineCapSquare:
			{
				// Add 3 lines making up a half square centred on the centreline endpoint
				DocCoord StartPoint(pEdge->Centre.x - dx, pEdge->Centre.y - dy);	// 1st point on cap

				pOutput->AddLineTo(DocCoord(StartPoint.x - dy, StartPoint.y + dx));
				pOutput->AddLineTo(DocCoord(EndPoint.x - dy, EndPoint.y + dx));
				pOutput->AddLineTo(EndPoint);
			}
			break;

		default:
			ERROR3("Unknown line cap style");
			break;
	}

	return(TRUE);
}



/******************************************************************************************

>	virtual BOOL PathStroker::StrokeSmoothPath(LineCapType CapType, TrapsList *pTraps, Path *pOutput)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> - totally ripped from Jason above
	Created:	18/11/2000

	Inputs:		pTraps		- A trapezoidal description of the path to be stroked (May not be NULL)

	Outputs:	pOutput		- This path is filled in with a "filled outline" of the stroke
							  It's IsFilled flag will be TRUE, IsStroked will be FALSE.

	Returns:	FALSE if it fails. (No error will be set at this level)

	Purpose:	"Strokes" a path as above, but SMOOTHS its forwards and backwards paths to reduce points

*******************************************************************************************/

BOOL PathStroker::StrokeSmoothPath(TrapsList *pTraps, Path *pOutput)
{
	ERROR3IF(pTraps == NULL || pOutput == NULL, "Illegal NULL params");

	// For each trapezoid list generated, add a subpath to the pOutput path
	for (UINT32 Index = 0; Index < pTraps->GetNumTraps(); Index++)
	{
		TrapEdgeList *pTrapEdges = pTraps->GetTrapEdgeList(Index);
		if (pTrapEdges->GetNumEdges() < 2)
		{
			ERROR3("No map traps when stroking! Subpath stripped\n");
			continue;
		}

		// Check if this subpath is closed (ends in a join). If so, we must remove the line caps!
		LineCapType CapType = CapStyle;
		TrapEdge *pEdge = pTrapEdges->GetLastTrapEdge();
		if (pEdge->PrevTrapJoin != TrapJoin_None)
			CapType = LineCapButt;

		// Start a new sub-path with a MoveTo to the first point
		pEdge = pTrapEdges->GetTrapEdge(0);
		INT32 Width = (INT32) (MaxWidth * pWidthFunction->GetValue(pEdge->Position));
		INT32 dx	   = (INT32) (pEdge->Normal.x * Width);
		INT32 dy	   = (INT32) (pEdge->Normal.y * Width);
		DocCoord StartPoint(pEdge->Centre.x + dx, pEdge->Centre.y + dy);
		pOutput->AddMoveTo(StartPoint);

		INT32 first = 0;
		INT32 last = -1;

		// Map the "forwards" outline
		MapLineForwards(pTrapEdges, pOutput);
		pOutput->SmoothSection(first, &last, 1.25 * Flatness/*2500*/, 0);

		// Add the line cap at the end of the line (if the height at this point of the value function is greater
		// than some threshold above zero).
		double height = pWidthFunction->GetValue(1);
		if (height > 0.05)
		{
			MapLineCap(CapType, pTrapEdges->GetLastTrapEdge(), FALSE, pOutput);
		}

		// Map the "backwards" outline
		first = pOutput->GetNumCoords() - 1;
		MapLineBackwards(pTrapEdges, pOutput);
		last = -1l;
		pOutput->SmoothSection(first, &last, 1.25 * Flatness/*2500*/, 0);
		

		INT32			indCoord;
		PathVerb	   *pVerbs = pOutput->GetVerbArray();

		// Check all but the last verb to see if the path has been closed - if so, UNclose it!!!
		for( indCoord = 0; indCoord < pOutput->GetNumCoords() - 2; indCoord++ )
		{
			pVerbs[indCoord] &= ~PT_CLOSEFIGURE;
		}

		indCoord = pOutput->GetNumCoords() - 1;
		
		// Check to see if the last verb in the array has a CLOSEFIGURE type - if so then don't close it again!!!
		if (!(pVerbs[indCoord] & PT_CLOSEFIGURE))
		{
			// Add the line cap at the end of the line, and we should meet back at the start!
			MapLineCap(CapType, pTrapEdges->GetTrapEdge(0), TRUE, pOutput);

			// And set the PT_CLOSEFIGURE flag on the last point, just to be safe
			indCoord = pOutput->GetNumCoords() - 1;
			PathVerb *pVerbs = pOutput->GetVerbArray();
			pVerbs[indCoord] |= PT_CLOSEFIGURE;
		}
	}

	pOutput->IsFilled	= TRUE;
	pOutput->IsStroked	= FALSE;

TRACEUSER( "Matt", _T("Path SMOOTHLY stroked: %ld subpaths, (1st is %ld Traps) -> %ld Coords\n"),
							pTraps->GetNumTraps(), pTraps->GetTrapEdgeList(0)->GetNumEdges(),
							pOutput->GetNumCoords());

	return(TRUE);
}






///////////////////////////////////////////////////////////////////////////////////////////
// PathStrokerVector class
///////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************************************

>	PathStrokerVector::PathStrokerVector(ValueFunction *WidthFunction, INT32 LineWidth,
										 LineCapType CapType, DocRect *pSourceBounds)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/97

	Inputs:		WidthFunction - NULL, or a pointer to a width function for the stroke
				LineWidth	  - The maximum width, in millipoints, of the stroke
				CapType		  - IGNORED
				pSourceBounds - The bounding rectangle of the source brush. A brush may consist
								of many paths, and this is the bounding rectangle of the entire brush.
								(May not be NULL)
	Purpose:	Constructor

******************************************************************************************/

PathStrokerVector::PathStrokerVector(ValueFunction *WidthFunction, INT32 LineWidth,
									 LineCapType CapType, DocRect *pSourceBounds)
				  : PathStroker(WidthFunction, LineWidth, CapType)
	
{
	ERROR3IF(pSourceBounds == NULL , "Illegal NULL params");

	pPath			 = NULL;
	pBounds			 = pSourceBounds;
	BoundsYCentre	 = (double) (pSourceBounds->lo.y/2 + pSourceBounds->hi.y/2);
	BoundsWidth		 = (double) pSourceBounds->Width();
	BoundsHalfHeight = (double) pSourceBounds->Height() / 2.0;
	pOutput			 = NULL;
	pCurrentEdgeList = NULL;
	FirstEdge		 = 0;
	LastEdge		 = 0;
}



/******************************************************************************************

>	void PathStrokerVector::PrepareToStroke(TrapEdgeList *pTraps);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/97

	Inputs:		pTraps		- A trapezoidal description of the path envelope into which
							  the source path must be "moulded".
							  (May not be NULL)

	Purpose:	MUST be called before any calls to StrokePath or MapCoord.

				This sets the current stroke TrapsList which will be used for all
				future mappings. It is separate from StrokePath so that you can set
				this up for moulding of attributes which affect the paths which you
				will mould, and this needs to be done on a per-stroke basis.

	SeeAlso:	PathStrokerVector::StrokePath; PathStrokerVector::MapCoord

******************************************************************************************/

void PathStrokerVector::PrepareToStroke(TrapEdgeList *pTraps)
{
	// Remember the new TrapsList
	ERROR3IF(pTraps == NULL, "Illegal NULL param");
	pCurrentEdgeList = pTraps;

	// And reset our edge markers to make sure they're in range and safe to use
	FirstEdge = LastEdge = 0;
}



/******************************************************************************************

>	virtual BOOL PathStrokerVector::StrokePath(TrapsList *pTraps, Path *pOutput)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/97

	Purpose:	DO NOT CALL this variant of StrokePath!

******************************************************************************************/

BOOL PathStrokerVector::StrokePath(TrapsList *pTraps, Path *pOutput)
{
	ERROR2(FALSE, "Do not call this variant of Strokepath()");
}



/******************************************************************************************

>	virtual BOOL PathStrokerVector::StrokePath(Path *pSourceVector, Path *pOutput)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/97

	Inputs:		pSourceVector
						- A source path to be moulded (the "vector brush" path). This will
						  be moulded to line in the stroke envelope, and output into pOutput
						  (May not be NULL)

	Outputs:	pOutput	- This path is filled in with a "filled outline" of the stroke
						  It's IsFilled flag will be TRUE, IsStroked will be FALSE.
						  NOTE - the new path elements are APPENDED to this path, so
						  you must clear it beforehand if you feel this would be handy.

	Returns:	FALSE if it fails. (No error will be set at this level)

	Purpose:	** Call PrepareToStroke before calling this function **
				"Strokes" a path according to the "style" set in construction.

				This is the second stage in the stroking process (the first stage being
				breaking a path down into a trapezoidal description).

				Stroking is achieved by mapping the intersections of trapezoid edges
				with the width function, and then recursively flattening the mapped
				curve between those points until required "flatness" is achieved.

				Vector stroking is achieved by moulding path(s) to lie inside the stroke
				envelope, in order to produce media (brush, pen, crayon, etc) simulations.

	Notes:		This function must be called individually for each source path of
				the brush, for each stroke in the trapezoid list. (This is to allow
				attributes to be mapped alongside, as the mappings are different
				for different subpaths of the path being stroked)

	SeeAlso:	PathStrokerVector::PrepareToStroke

******************************************************************************************/

BOOL PathStrokerVector::StrokePath(Path *pSourceVector, Path *pOutputPath)
{
	ERROR3IF(pCurrentEdgeList == NULL || pBounds == NULL,  "Call PrepareToStroke first!");
	ERROR3IF(pSourceVector == NULL || pOutputPath == NULL, "Illegal NULL params");

	if (BoundsWidth <= 0.0 || BoundsHalfHeight <= 0.0)
		return(FALSE);

	pPath	= pSourceVector;
	pOutput = pOutputPath;

	// Precalculate the position values for the extents of the path within the brush
	DocRect PathBounds = pPath->GetBoundingRect();
	const double MinPathPos = ((double)(PathBounds.lo.x - pBounds->lo.x)) / BoundsWidth;
	const double MaxPathPos = ((double)(PathBounds.hi.x - pBounds->lo.x)) / BoundsWidth;

	// When doing repeating brushes, the last repeat can be clipped at the end of the path,
	// so we check if this entire path should be clipped off
	// BLOCK
	{
		TrapEdge *pLastEdge = pCurrentEdgeList->GetLastTrapEdge();
		if (pLastEdge != NULL && MinPathPos > pLastEdge->Position)
			return(TRUE);
	}

	// For each curve segment in the source path, we output a mapped curve, which
	// consists of a number of flattened straight line segments.
    DocCoord *pCoords	 = pPath->GetCoordArray();
	PathVerb *pVerbs	 = pPath->GetVerbArray();
	const INT32 NumCoords = pPath->GetNumCoords();

	// Determine the range of trapezoids this path will map into in the current list
	// This makes searching for appropriate trapezoids much more efficient.
	FirstEdge = pCurrentEdgeList->FindTrapEdge(MinPathPos, 0, 0);
	LastEdge  = pCurrentEdgeList->FindTrapEdge(MaxPathPos, FirstEdge, pCurrentEdgeList->GetNumEdges() - 1);

	// And map the path into this stroke
	INT32 Element = 0;
	while (Element < NumCoords)
	{
		switch (pVerbs[Element] & ~PT_CLOSEFIGURE)
		{
			case PT_MOVETO:
				MapMove(&pCoords[Element]);
				break;

			case PT_LINETO:
				{
					ERROR3IF(Element < 1, "Path has LINETO as first element?!");

					RecursionDepth = 0;						// Reset recursion depth counter to 0
					MapLine(&pCoords[Element-1], &pCoords[Element]);

					if ((pVerbs[Element] & PT_CLOSEFIGURE) != 0)
					{
						// If this was a CLOSEFIGURE verb, then set the last output point to be CloseFigure
						PathVerb *pOutVerbs = pOutput->GetVerbArray();
						ERROR3IF(pOutVerbs == NULL || pOutput->GetNumCoords() < 1, "Oh dear. That shouldn't be like that");
						pOutVerbs[pOutput->GetNumCoords() - 1] |= PT_CLOSEFIGURE;
					}
				}
				break;

			case PT_BEZIERTO:
				{
					ERROR3IF(Element < 1, "Path has BEZIERTO as first element?!");

					RecursionDepth = 0;						// Reset recursion depth counter to 0
					MapBezier(&pCoords[Element-1]);

					if ((pVerbs[Element+2] & PT_CLOSEFIGURE) != 0)
					{
						// If this was a CLOSEFIGURE verb, then set the last output point to be CloseFigure
						PathVerb *pOutVerbs = pOutput->GetVerbArray();
						ERROR3IF(pOutVerbs == NULL || pOutput->GetNumCoords() < 1, "Oh dear. That shouldn't be like that");
						pOutVerbs[pOutput->GetNumCoords() - 1] |= PT_CLOSEFIGURE;
					}

					Element += 2;		// Skip the 2 bezier control coordinates
				}
				break;

			default:
				ERROR3("PathStrokerVector::StrokePath() - unknown path verb!");
				break;
    	}

		Element++;
	}

	pOutput->IsFilled	= pSourceVector->IsFilled;
	pOutput->IsStroked	= pSourceVector->IsStroked;

	return(TRUE);
}



/******************************************************************************************

>	void PathStrokerVector::FindEdgeForCoord(DocCoord *pCoord, UINT32 StartIndex,
											 INT32 Direction, TrapEdge *pOutput)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/97

	Inputs:		pCoord		- The coordinate in the source path to be mapped
				StartIndex	- The index in the TrapsList from which to start searching.
							  The closer this is to the right place, the faster it goes
				Direction	- +1 if this edge is being used on a forward outline, or
							  -1 if it is being used on a reverse outline
							  (needed to duplicate join style flags correctly)

	Outputs:	pOutput		- A trapezoid edge structure, which will be filled in with
							  details of an edge to use for mapping this point. (The edge
							  will lie exactly on the Position the point maps to in the path)

	Purpose:	** Call PrepareToStroke before calling this function **
				Determines a TrapEdge Centre point and Normal to be used when mapping the
				given point into the destination path.

	Notes:		Note that only "floating" endpoints need to be mapped in this computationally
				expensive manner - midpoints are mapped elsewhere (by intersecting the
				source path with TrapEdges, and mapping on those exact intersections,
				thus eliminating the need for so much interpolation)

	SeeAlso:	PathStrokerVector::PrepareToStroke

******************************************************************************************/

void PathStrokerVector::FindEdgeForCoord(DocCoord *pCoord, UINT32 StartIndex, INT32 Direction,
											TrapEdge *pOutput)
{
	ERROR3IF(pCurrentEdgeList == NULL || pBounds == NULL,  "Call PrepareToStroke first!");
	ERROR3IF(pCoord == NULL || pOutput == NULL, "Illegal NULL params");

	// Find the parametric position of the coordinate in the source brush
	pOutput->Position = ((double)(pCoord->x - pBounds->lo.x)) / BoundsWidth;

	// --- If this is a grad fill point, or somebody screwed up the brush bounds, then
	// the point could lie outside the "legal" range - but we must try to map it sensibly,
	// or else things like fills will screw up big time!
	if (pOutput->Position < 0.0 || pOutput->Position > 1.0)
	{
		// Find the nearest TrapEdge to the point, and adjust Position to be the "distance" from that TrapEdge
		TrapEdge *pEdge1;
		if (pOutput->Position > 1.0)
		{
			pEdge1 = pCurrentEdgeList->GetLastTrapEdge();
			pOutput->Position -= 1.0;
		}
		else
			pEdge1 = pCurrentEdgeList->GetTrapEdge(0);

		// Calculate a new centre position
		// (pretend that the path continues as an infinite straight line in its last known direction)
		const double PathLength = pCurrentEdgeList->GetPathLength();
		pOutput->Centre.x = pEdge1->Centre.x - (INT32) (pOutput->Position * PathLength * (-pEdge1->Normal.y));
		pOutput->Centre.y = pEdge1->Centre.y - (INT32) (pOutput->Position * PathLength * pEdge1->Normal.x);

		// All the other variables are the same as the end TrapEdge
		pOutput->Normal = pEdge1->Normal;
		pOutput->PrevTrapJoin = pEdge1->PrevTrapJoin;

		// Now clip the position value so the caller thinks we're within the stroke bounds
		// (Necessary so that they use the correct Position with Width ValueFunctions, etc)
		if (pOutput->Position < 0.0)
			pOutput->Position = 0.0;
		else
			pOutput->Position = 1.0;
		return;
	}


	// --- Find the trapezoid edges on either side of that position. Start from FirstEdge,
	// as that is where the path starts, so vastly reduces needless searching for
	// points lying within the path. (Note that fill points can lie outside the
	// path... see the if statement below)
	UINT32 TrapIndex = pCurrentEdgeList->FindTrapEdge(pOutput->Position, StartIndex, 0);
	TrapEdge *pEdge1 = pCurrentEdgeList->GetTrapEdge(TrapIndex);

	// --- Generate the new TrapEdge information
	if (pOutput->Position <= pEdge1->Position || (TrapIndex >= pCurrentEdgeList->GetNumEdges() - 1))
	{
		// The point lies exactly on the pEdge position, so return the exact edge
		// (The if statement checks for it lying outside the trapezoid, but this should never happen,
		// due to the position checks at the top of this function, so it's safe for us to use
		// a catch-all at this point.
		pOutput->Normal = pEdge1->Normal;
		pOutput->Centre = pEdge1->Centre;
		if (Direction > 0 && TrapIndex < pCurrentEdgeList->GetNumEdges() - 1)
		{
			TrapEdge *pEdge2 = pCurrentEdgeList->GetTrapEdge(TrapIndex+1);
			pOutput->PrevTrapJoin = pEdge2->PrevTrapJoin;
		}
		else
			pOutput->PrevTrapJoin = pEdge1->PrevTrapJoin;
	}
	else
	{
		// The point lies between 2 TrapEdges, so we interpolate the trapezoid centre and
		// normal to approximate a trapedge for this exact Position on the path
		TrapEdge *pEdge2 = pCurrentEdgeList->GetTrapEdge(TrapIndex+1);
		ERROR3IF(pEdge2->Position <= pEdge1->Position, "Non-ascending trapezoid positions?!");

		// Determine how far between the 2 positions the point lies
		const double Fraction = (pOutput->Position - pEdge1->Position) / (pEdge2->Position - pEdge1->Position);
		const double InvFraction = 1.0 - Fraction;

		// Compute an interpolated normal
		pOutput->Normal.x = (InvFraction * pEdge1->Normal.x) + (Fraction * pEdge2->Normal.x);
		pOutput->Normal.y = (InvFraction * pEdge1->Normal.y) + (Fraction * pEdge2->Normal.y);
		pOutput->Normal.Normalise();

		// Compute an interpolated centreline point
		pOutput->Centre.x = (INT32) ((InvFraction * (double)pEdge1->Centre.x) + (Fraction * (double)pEdge2->Centre.x));
		pOutput->Centre.y = (INT32) ((InvFraction * (double)pEdge1->Centre.y) + (Fraction * (double)pEdge2->Centre.y));
	
		// Duplicate any join flags. We must take care to replicate in the correct
		// direction or joins will be stroked incorrectly
		if (Direction > 0)
			pOutput->PrevTrapJoin = pEdge2->PrevTrapJoin;
		else
			pOutput->PrevTrapJoin = pEdge1->PrevTrapJoin;
	}
}



/******************************************************************************************

>	DocCoord PathStrokerVector::MapCoord(DocCoord *pCoord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/97

	Inputs:		pCoord - The coordinate in the source path to be mapped

	Returns:	The mapped coordinate

	Purpose:	** Call PrepareToStroke before calling this function **
				Maps the given point into the output stroke path, and returns the
				new coordinate to be output at your discretion.

	Notes:		Mapping is achieved by the following algorithm:
				* Find the trapezoid in which the point lies. This is done by calculating
				  the point's "Position" (X coord) in the source brush, and searching to
				  find the TrapEdge structures on either side of that position.
				* Linearly interpolate within the trapezoid to get a centreline point and
				  normal for this exact position
				* Determine how far the point is from the brush centreline (Y coord) and
				  map that distance onto the new TrapEdge, scaling by the Variable Width
				  function.

				Note that only "floating" endpoints need to be mapped in this computationally
				expensive manner - midpoints are mapped elsewhere (by intersecting the
				source path with TrapEdges, and mapping on those exact intersections,
				thus eliminating the need for so much interpolation)

	SeeAlso:	PathStrokerVector::PrepareToStroke;
				FillGeometryAttribute::MouldIntoStroke

******************************************************************************************/

DocCoord PathStrokerVector::MapCoord(DocCoord *pCoord)
{
	ERROR3IF(pCurrentEdgeList == NULL || pBounds == NULL,  "Call PrepareToStroke first!");

	// Get a trapezoid edge description which lies exactly on this point's mapping Position
	TrapEdge MapEdge;
	FindEdgeForCoord(pCoord, FirstEdge, +1, &MapEdge);
	ERROR3IF(MapEdge.Position < 0.0 || MapEdge.Position > 1.0, "Position value was not properly clipped");

	// Calculate how far the point is from the Y centre of the brush, and scale
	// this distance by the variable width value for its Position
	const double SourceDist = (((double)pCoord->y) - BoundsYCentre) / BoundsHalfHeight;
	const double MappedDist = ((double)MaxWidth) * SourceDist * pWidthFunction->GetValue(MapEdge.Position);

	// Map that width onto the destination path
	return(DocCoord(MapEdge.Centre.x + (INT32)(MapEdge.Normal.x * MappedDist),
					MapEdge.Centre.y + (INT32)(MapEdge.Normal.y * MappedDist) ));
}



/******************************************************************************************

>	double PathStrokerVector::GetScaleFactor(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/97

	Returns:	A scale factor indicating by how much a brush (or element thereof)
				will grow or shrink when mapped into the current stroke. Multiplying
				a size value by this factor will generate an approximate new size to use.

	Purpose:	** Call PrepareToStroke before calling this function **
				Determines a rough scaling factor by which the stroke is changing size
				during the mapping process. Used to map things like LineWidth attributes
				so that their size remains roughly proportional to the new size of the
				stroke. The scale factor is based on the source and destination line
				widths, which may not always be brilliant, but is the best we can do.

	SeeAlso:	PathStrokerVector::PrepareToStroke;
				LineWidthAttribute::MouldIntoStroke

******************************************************************************************/

double PathStrokerVector::GetScaleFactor(void)
{
	if (BoundsHalfHeight < 1.0)		// Avoid that nasty div by zero!
		return(1.0);

	return( ((double) MaxWidth) / BoundsHalfHeight );
}



/******************************************************************************************

>	void PathStrokerVector::MapMove(DocCoord *pCoord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/97

	Inputs:		pCoord - The coordinate in the SOURCE path to be mapped

	Purpose:	Maps the given point into the output stroke path, writing a MOVETO element

******************************************************************************************/

void PathStrokerVector::MapMove(DocCoord *pCoord)
{
	pOutput->AddMoveTo(MapCoord(pCoord));
}



/******************************************************************************************

>	void PathStrokerVector::MapLine(DocCoord *pCoord1, DocCoord *pCoord2)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/97

	Inputs:		pCoord1		- } 2 coordinates representing the line to map
				pCoord2		- } (these are coords in the SOURCE brush space)

	Purpose:	Maps the given line into the output stroke path
				Assumes that the first point (pCoord1) has already been output to
				the destination path (i.e. it appends the line to pOutput)

******************************************************************************************/

void PathStrokerVector::MapLine(DocCoord *pCoord1, DocCoord *pCoord2)
{
	// Find the parametric positions of the coordinates in the source brush
	double Position1 = ((double)(pCoord1->x - pBounds->lo.x)) / BoundsWidth;
	double Position2 = ((double)(pCoord2->x - pBounds->lo.x)) / BoundsWidth;

	// Work out the range of positions this line covers. If this is non-zero then we will map
	// a point at each trapezoid edge intersected by the line - otherwise, the line is vertical
	// and can just be output directly, so we don't bother interpolating
	const double PosRange = Position2 - Position1;
	if (PosRange == 0.0)
	{
		pOutput->AddLineTo(MapCoord(pCoord2));
		return;
	}

	// Find the trapezoid edge relating to the first position
	UINT32 TrapIndex = pCurrentEdgeList->FindTrapEdge(Position1, FirstEdge, LastEdge);
	ERROR3IF(TrapIndex < FirstEdge || TrapIndex > LastEdge, "TrapIndex outside expected range");

	UINT32 EndIndex = pCurrentEdgeList->FindTrapEdge(Position2, FirstEdge, LastEdge);
	ERROR3IF(EndIndex < FirstEdge || EndIndex > LastEdge, "Trap EndIndex outside expected range");

	// Calculate TrapEdge data for the first & last points
	TrapEdge FirstEdgeData;
	TrapEdge LastEdgeData;
	const INT32 Direction = (PosRange > 0.0) ? +1 : -1;
	FindEdgeForCoord(pCoord1, TrapIndex, Direction, &FirstEdgeData);
	FindEdgeForCoord(pCoord2, EndIndex,  Direction, &LastEdgeData);

	// Calculate the values for the 1st point
	TrapEdge *pLastEdge = &FirstEdgeData;
	double LastWidth = ((double)MaxWidth) *
						(((double)pCoord1->y - BoundsYCentre) / BoundsHalfHeight);
	double BaseWidth = LastWidth * pWidthFunction->GetValue(pLastEdge->Position);
	DocCoord LastPoint(	(INT32) (pLastEdge->Centre.x + pLastEdge->Normal.x * BaseWidth),
						(INT32) (pLastEdge->Centre.y + pLastEdge->Normal.y * BaseWidth) );

	TrapEdge *pEdge = NULL;

	// Now, map the line through each trapezoid in turn. There are 2 variants of this
	// loop - 1 for the "forwards" outline, and one for the "reverse" outline.
	if (PosRange > 0.0)
	{
		// Because TrapIndex and EndIndex point at the entry edge of the relevant trapezoids,
		// we want to increment them both to refer to the exit edge instead.
		TrapIndex++;

		while (TrapIndex <= EndIndex)
		{
			pEdge = pCurrentEdgeList->GetTrapEdge(TrapIndex);

			// Interpolate the line between its intersections with the sides of this trapezoid
			// The LastXXXX variables hold the intersection values for the entry-intersection,
			// so now we find the values for the exit-intersection
			const double Fraction = (pEdge->Position - Position1) / PosRange;
			const double Y = ((1.0 - Fraction) * (double)pCoord1->y) + (Fraction * (double)pCoord2->y);
			const double CentreDist = (Y - BoundsYCentre) / BoundsHalfHeight;
			BaseWidth = ((double)MaxWidth) * CentreDist;
			const double Width = BaseWidth * pWidthFunction->GetValue(pEdge->Position);

			DocCoord NewPoint(pEdge->Centre.x + (INT32) (pEdge->Normal.x * Width),
							  pEdge->Centre.y + (INT32) (pEdge->Normal.y * Width) );

			RecursiveMapLine(pLastEdge, LastPoint, LastWidth,
							 pEdge, NewPoint, BaseWidth,
							 +1, pOutput);

			pLastEdge = pEdge;
			LastWidth = BaseWidth;
			LastPoint = NewPoint;

			TrapIndex++;
		}
	}
	else
	{
		// Now, map the line through each trapezoid in turn
		// This condition is really nasty; Translation: while (TrapIndex has not passed EndIndex)
		while (TrapIndex > EndIndex)
		{
			pEdge = pCurrentEdgeList->GetTrapEdge(TrapIndex);

			// Interpolate the line between its intersections with the sides of this trapezoid
			// The LastXXXX variables hold the intersection values for the entry-intersection,
			// so now we find the values for the exit-intersection
			const double Fraction = (pEdge->Position - Position1) / PosRange;
			const double Y = ((1.0 - Fraction) * (double)pCoord1->y) + (Fraction * (double)pCoord2->y);
			const double CentreDist = (Y - BoundsYCentre) / BoundsHalfHeight;
			BaseWidth = ((double)MaxWidth) * CentreDist;
			const double Width = BaseWidth * pWidthFunction->GetValue(pEdge->Position);

			DocCoord NewPoint(pEdge->Centre.x + (INT32) (pEdge->Normal.x * Width),
							  pEdge->Centre.y + (INT32) (pEdge->Normal.y * Width) );

			RecursiveMapLine(pLastEdge, LastPoint, LastWidth,
							 pEdge, NewPoint, BaseWidth,
							 -1, pOutput);

			pLastEdge = pEdge;
			LastWidth = BaseWidth;
			LastPoint = NewPoint;

			TrapIndex--;
		}
	}

	// And then map the last trapezoid
	pEdge = &LastEdgeData;

//	const double Fraction = (pEdge->Position - Position1) / PosRange;
	const double CentreDist = (pCoord2->y - BoundsYCentre) / BoundsHalfHeight;
	BaseWidth = ((double)MaxWidth) * CentreDist;
	const double Width = BaseWidth * pWidthFunction->GetValue(pEdge->Position);

	DocCoord NewPoint(pEdge->Centre.x + (INT32) (pEdge->Normal.x * Width),
					  pEdge->Centre.y + (INT32) (pEdge->Normal.y * Width) );

	RecursiveMapLine(pLastEdge, LastPoint, LastWidth,
					 pEdge, NewPoint, BaseWidth,
					 (PosRange > 0) ? +1 : -1, pOutput);
}



/******************************************************************************************

>	void PathStrokerVector::RecursiveMapLine(TrapEdge *pEdge1, DocCoord &Point1, double Width1,
											 TrapEdge *pEdge2, DocCoord &Point2, double Width2,
											 INT32 Direction, Path *pOutput)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/97

	Inputs:		pEdge1, Point1	- The source TrapEdge and coordinate of the first point
								  (which must have already been added to the output path)
				Width1			- The width (NOT including the effect of pWidthFunction)
								  of the stroke at Point1

				pEdge2, Point2	- The source TrapEdge and coordinate of the second point
								  (which must NOT have been added to the output path)
				Width2			- The width (NOT including the effect of pWidthFunction)
								  of the stroke at Point2

				Direction		- +1 when traversing the path in a forwards direction
								  -1 when traversing the path in a backwards direction

	Outputs:	pOutput	- A pointer to a path in which the output will be generated
							(May not be NULL)

	Purpose:	Recursive method which maps points within a given trapezoid by
				recursively "flattening" the curve between the two trapezoid edges.

				It inserts extra mapped points between the two original points
				passed in to it, until the flatness criteria is met.

******************************************************************************************/

void PathStrokerVector::RecursiveMapLine(TrapEdge *pEdge1, DocCoord &Point1, double Width1,
										 TrapEdge *pEdge2, DocCoord &Point2, double Width2,
										 INT32 Direction, Path *pOutput)
{
	ERROR3IF(pEdge1 == NULL || pEdge2 == NULL || pOutput == NULL, "Illegal NULL params");

	// --- Watch out for infinite recursion. This can come about when there is a discontinuity
	// in the width function, where no amount of flattening will reduce the distance between
	// Point1 and Point2. Note that we are flattening into a very small (already flattened)
	// region of space, so we don't need very great recursion depth to get a decent approximation
	// to the actual curve required, hence I stop the recursion after 20 iterations.
	RecursionDepth++;
	if (RecursionDepth > MaxRecursionDepth)
	{
		TRACEUSER( "Jason", _T(">> Recursion limit hit when stroking\n"));
		pOutput->AddLineTo(Point2);
		return;
	}

	// --- Calculate the midpoint of the curve we are mapping
	// Calculate the midpoint "position" value
	TrapEdge MidEdge;
	MidEdge.Position = (pEdge1->Position + pEdge2->Position) / 2.0;

	// Calculate the midpoint normal vector
	MidEdge.Normal.x = (pEdge1->Normal.x + pEdge2->Normal.x) / 2.0;
	MidEdge.Normal.y = (pEdge1->Normal.y + pEdge2->Normal.y) / 2.0;

	// We should normalise the normal vector, but in the case of mitred/bevelled joins
	// we want a straight edge, which means leaving the normal vector with an interpolated
	// length as well as direction. This is indicated by the end-edge of the trapezoid 
	// having the PrevTrapIsJoin flag set. We also have to duplicate the flag on all
	// intermediate points so that they are also handled correctly.
	// (Note in the 'forwards' case the relevant "PrevTrap" flag is in pEdge2)
	if (Direction > 0)
		MidEdge.PrevTrapJoin = pEdge2->PrevTrapJoin;
	else
		MidEdge.PrevTrapJoin = pEdge1->PrevTrapJoin;
	if (MidEdge.PrevTrapJoin != TrapJoin_MitredOrBevelled)
		MidEdge.Normal.Normalise();

	// Calculate the (approximate) centreline midpoint
	// (approximate because we're using the flattened line rather than the source curve,
	// but this is still close enough to give acceptable quality).
	// We take care with the average to avoid overflowing our INT32s
	MidEdge.Centre.x = (pEdge1->Centre.x / 2) + (pEdge2->Centre.x / 2);
	MidEdge.Centre.y = (pEdge1->Centre.y / 2) + (pEdge2->Centre.y / 2);

	// Now, calculate the mapped midpoint
	const INT32 MidWidth = (INT32) ((Width1 + Width2) / 2.0);
	const INT32 MidWidthAll = (INT32) ((double)MidWidth * pWidthFunction->GetValue(MidEdge.Position));
 	DocCoord MidPoint(MidEdge.Centre);
	MidPoint.x += (INT32) (MidEdge.Normal.x * MidWidthAll);
	MidPoint.y += (INT32) (MidEdge.Normal.y * MidWidthAll);

	// --- Calculate the midpoint of the straight line segment
	DocCoord ApproximateMidPoint(Point1.x/2 + Point2.x/2, Point1.y/2 + Point2.y/2);

	// --- Calculate the error in the midpoint position
	const INT32 dx = MidPoint.x - ApproximateMidPoint.x;
	const INT32 dy = MidPoint.y - ApproximateMidPoint.y;
	const double Dist = (double)((dx * dx) + (dy * dy));

	// --- If the straight-line approximation is not close enough to the curve, recurse,
	// else (recursion-tail) output the end-point that was passed in. We also recurse
	// if the Position distance between the endpoints is large, as otherwise it can
	// fail to flatten width functions properly.
	if (Dist > Flatness2 || fabs(pEdge1->Position - pEdge2->Position) > 0.10)
	{
		// Recurse on the left and right sides of the new midpoint
		RecursiveMapLine(pEdge1,   Point1,	 Width1,    &MidEdge, MidPoint, MidWidth,  Direction, pOutput);
		RecursiveMapLine(&MidEdge, MidPoint, MidWidth,  pEdge2,   Point2,   Width2,    Direction, pOutput);
	}
	else
		pOutput->AddLineTo(Point2);

	// And decrement the recursion depth counter
	RecursionDepth--;
}



/******************************************************************************************

>	void PathStrokerVector::MapBezier(DocCoord *pCoords)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/97

	Inputs:		pCoords		- An array of 4 coordinates representing the bezier to map,
							  in the order [knot1] [control1] [control2] [knot2]

	Purpose:	Maps the given bezier curve into the output stroke path

******************************************************************************************/

void PathStrokerVector::MapBezier(DocCoord *pCoords)
{
	RecursiveMapBezier(pCoords, &pCoords[0], 0.0, &pCoords[3], 1.0);
}



/******************************************************************************************

>	void PathStrokerVector::RecursiveMapBezier(DocCoord *pCoords,
												DocCoord *p1, double t1, DocCoord *p2, double t2)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/3/97

	Inputs:		pCoords		- An array of 4 coordinates representing the bezier to map,
							  in the order [knot1] [control1] [control2] [knot2]

				p1			- The first point on the SOURCE curve
				t1			- The parametric position (between 0.0 & 1.0 inclusive) of p1

				p2			- The second point on the SOURCE curve
				t2			- The parametric position of p2

	Purpose:	Maps the given bezier curve into the output stroke path, recursing
				as necessary to produce a nice smooth result.

	Notes:		Always recurses to subdivide the curve into at least 2 segments, to guarantee
				that no features of the bezier are missed.

******************************************************************************************/

void PathStrokerVector::RecursiveMapBezier(DocCoord *pCoords, DocCoord *p1, double t1, DocCoord *p2, double t2)
{
	// --- Watch out for infinite recursion. This can come about when there is a discontinuity
	// in the width function, where no amount of flattening will reduce the distance between p1 & p2
	RecursionDepth++;
	if (RecursionDepth > MaxRecursionDepth)
	{
		TRACEUSER( "Jason", _T(">> Recursion limit hit when stroking\n"));
		pOutput->AddLineTo(MapCoord(p2));
		return;
	}

#define x0 (pCoords[0].x)
#define y0 (pCoords[0].y)
#define x1 (pCoords[1].x)
#define y1 (pCoords[1].y)
#define x2 (pCoords[2].x)
#define y2 (pCoords[2].y)
#define x3 (pCoords[3].x)
#define y3 (pCoords[3].y)

	// Calculate the point at the middle t value
	const double t = (t1 + t2) / 2.0;

	const double tx = x1+t*(x2-x1);
	const double ty = y1+t*(y2-y1);

	const double Lx1 = x0  + t*(x1-x0);
	const double Ly1 = y0  + t*(y1-y0);
	const double Rx2 = x2  + t*(x3-x2);
	const double Ry2 = y2  + t*(y3-y2);
	const double Lx2 = Lx1 + t*(tx-Lx1);
	const double Ly2 = Ly1 + t*(ty-Ly1);
	const double Rx1 = tx  + t*(Rx2-tx);
	const double Ry1 = ty  + t*(Ry2-ty);
	const double Rx0 = Lx2 + t*(Rx1-Lx2);
	const double Ry0 = Ly2 + t*(Ry1-Ly2);

	DocCoord MidPoint((INT32)(Rx0+0.5), (INT32)(Ry0+0.5));

#undef x0
#undef y0
#undef x1
#undef y1
#undef x2
#undef y2
#undef x3
#undef y3

	// If the distance between the actual point and the current approximation is too great,
	// we will recursively flatten. However, we make sure that we divide the bezier into at
	// least 2 sections, so if the difference in t parameters is too great, we set Dist
	// large enough to cause a recursive flattening.
	double Dist = Flatness2;

	if (t2 - t1 < 0.45)
	{
		// calculate the mid point of the straight-line approximation
		DocCoord ApproxMidPoint(p1->x/2 + p2->x/2, p1->y/2 + p2->y/2);

		// Now map both mid-points into the stroke envelope
		DocCoord MappedMid	  = MapCoord(&MidPoint);
		DocCoord MappedApprox = MapCoord(&ApproxMidPoint);

		// And see how far the approximation is from the ideal position
		const double dx = MappedMid.x - MappedApprox.x;
		const double dy = MappedMid.y - MappedApprox.y;
		Dist = dx*dx + dy*dy;
	}

	// If we're too far away, then flatten further, else map the flattened line segment
	if (Dist >= Flatness2)
	{
		RecursiveMapBezier(pCoords, p1, t1, &MidPoint, t);
		RecursiveMapBezier(pCoords, &MidPoint, t, p2, t2);
	}
	else
		MapLine(p1, p2);

	// And decrement the recursion depth counter
	RecursionDepth--;
}





//////////////////////////////////////////////////////////////////////////////
// !!!!**** Debug stuff
//////////////////////////////////////////////////////////////////////////////

StrokeHandle PathStrokerVector::CurrentStroke = StrokeHandle_NoStroke;

void PathStrokerVector::BodgeRipSelection(BOOL Repeating)
{
	SelRange *pSel = GetApplication()->FindSelection();

	Node *pNode = pSel->FindFirst();
	if (pNode == NULL || !IS_A(pNode, NodeGroup))
	{
		ERROR3("Brush can only be made from a selected GROUP");
		return;
	}

	NodeGroup* pNewGroup = new NodeGroup;
	if (pNewGroup == NULL)
	{
		ERROR3("Couldn't create new brush (1)");
		return;
	}

	NodeGroup* pNewGroup1 = new NodeGroup;
	if (pNewGroup1 == NULL)
	{
		ERROR3("Couldn't create new brush (2)");
		delete pNewGroup;
		return;
	}
	
	// Attach this new group to the tree
	pNewGroup->AttachNode(pNode, NEXT);
	pNewGroup1->AttachNode(pNewGroup, FIRSTCHILD);

	// Copy the children across to this new group, and
	// make sure we have all the attributes we need (We must ensure the default attrs
	// are copied too, else bits of the clipart relying on the default attrs will
	// use current attrs when rendered, giving unpredictanble results - to override
	// things like fill colours, we have to provide a better interface...)
	if (!pNode->CopyChildrenTo(pNewGroup1) ||
		!pNewGroup->MakeAttributeComplete(NULL, TRUE, NULL, TRUE))
	{
		// CascadeDelete unlinks the node from the tree so don't panic.
		ERROR3("Couldn't create new brush (3)");
		pNewGroup->CascadeDelete();
		delete pNewGroup;
		return;
	}

	// --- Now, convert all IndexedColours (which are document-dependant) into standalone DocColours
	// BLOCK
	{
		Node *pCurNode = pNewGroup->FindFirstDepthFirst();
		Node *pNextNode;

		while (pCurNode !=NULL)
		{
			// We may be about to chop this node out of the tree, so get the next node now
			pNextNode = pCurNode->FindNextDepthFirst(pNewGroup);

			// Use to scan the colour fields of the attribute.
			if (pCurNode->IsAnAttribute())
			{
				UINT32 Context = 0;
				NodeAttribute *pNodeAttr = (NodeAttribute *) pCurNode;

				// Get the next colour field from the attribute
				DocColour *pColour = pNodeAttr->EnumerateColourFields(Context++);

				while (pColour != NULL)
				{
					// For each colour field, make sure the colour is a local DocColour so that
					// the sub-tree is entirely stand-alone
					if (pColour->FindParentIndexedColour() != NULL)
					{
						ColourGeneric ColDef;
						ColourContext *cc = ColourManager::GetColourContext(pColour->GetColourModel());
						ERROR3IF(cc == NULL, "Can't find colour context?!");

						// Get the IndexedColour definition as a standalone colour definition
						cc->ConvertColour(pColour->FindParentIndexedColour(), &ColDef);

						// Make the DocColour into a simple standalone "lookalike" of the parent colour
						*pColour = DocColour(pColour->GetColourModel(), &ColDef);
					}

					pColour = pNodeAttr->EnumerateColourFields(Context++);
				}
			}
			pCurNode = pNextNode;
		}
	}

	// --- Rip the node out of the tree and stick it in our cache.
	pNewGroup->UnlinkNodeFromTree();

	// Create a new spread & layer to stick the group into

	Spread *pBrush = new Spread;
	if (pBrush == NULL)
	{
		ERROR3("Couldn't create new brush (4)");
		pNewGroup->CascadeDelete();
		delete pNewGroup;
		return;
	}

	Layer *pBrushLayer = new Layer(pBrush, FIRSTCHILD, String_256(TEXT("Jason did this")));
	if (pBrushLayer == NULL)
	{
		ERROR3("Couldn't create new brush (5)");
		delete pBrush;
		pNewGroup->CascadeDelete();
		delete pNewGroup;
		return;
	}

	// And attach the clipart tree to our new layer
	pNewGroup->AttachNode(pBrushLayer, FIRSTCHILD, FALSE, TRUE);

	StrokeDefinition *pStrokeDef = new StrokeDefinition(pBrush, Repeating);
	if (pStrokeDef == NULL)
	{
		ERROR3("Couldn't create new brush (6)");
		pBrush->CascadeDelete();
		delete pBrush;
		return;
	}

	CurrentStroke = StrokeComponent::AddNewStroke(pStrokeDef);

//	DebugTreeDlg::DumpSubTree(pBodgeBrush);
}

StrokeHandle PathStrokerVector::GetCurrentBrush(void)
{
	return(CurrentStroke);
}

