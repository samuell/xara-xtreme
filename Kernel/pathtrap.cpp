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
// Implementation of Path Trapezoid classes (used in stroke providing)

#include "camtypes.h"

#include "pathtrap.h"

//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]


CC_IMPLEMENT_DYNAMIC(NormCoord, CCObject)
CC_IMPLEMENT_DYNAMIC(TrapEdgeList, CCObject)
CC_IMPLEMENT_DYNAMIC(TrapsList, CCObject)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



///////////////////////////////////////////////////////////////////////////////////////////
// TrapEdgeList class
///////////////////////////////////////////////////////////////////////////////////////////



/******************************************************************************************

>	TrapEdgeList::TrapEdgeList(TrapsList *pParent = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/12/97

	Inputs:		pParent	- The parent TrapsList in which this TrapEdgeList resides.
				Must be non-NULL

	Purpose:	Constructor

******************************************************************************************/

TrapEdgeList::TrapEdgeList(TrapsList *pParent)
{
	ERROR3IF(pParent == NULL, "Illegal NULL param");

	Used			= 0;
	CurrentSize		= 0;
	pEdges			= NULL;
	PathLength		= 1.0;
	pParentList		= pParent;
}



/******************************************************************************************

>	TrapEdgeList::~TrapEdgeList()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Purpose:	Destructor

******************************************************************************************/

TrapEdgeList::~TrapEdgeList()
{
	if (pEdges != NULL)
		CCFree(pEdges);
}



/******************************************************************************************

>	UINT32 TrapEdgeList::FindTrapEdge(double Position, UINT32 LoIndex = 0, UINT32 HiIndex = 0)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/97

	Inputs:		Position	- The position to search for
				LoIndex		}
				HiIndex		} Bounds in the list within which to start the search.
							  The search will go outside these bounds if necessary,
							  but good start bounds can significantly increase the
							  speed of the search. If you don't know specific bounds,
							  then you can pass a 0 in either/both variables.

	Returns:	The index of the found trapezoid edge (0 if error such as empty list)

	Purpose:	Searches the trapedge list for the trapezoid "containing" the
				given position value. It starts its search from the given index
				and searches upwards until it finds the last edge which has a position
				less than or equal to the given position.

	Notes:		It may return the last edge, in which case the position value
				lies outside the range of positions recorded in the list.

******************************************************************************************/

UINT32 TrapEdgeList::FindTrapEdge(double Position, UINT32 LoIndex, UINT32 HiIndex)
{
	if (Used < 2)
		return(0);

	// Special cases - if the point is outside the array bounds, then return the end immediately.
	// These boost speed greatly when clipping part of a vector stroke off the end, etc
	if (Position <= pEdges[0].Position)
		return(0);

	if (Position >= pEdges[Used-1].Position)
		return(Used-1);

	// Make sure HiIndex is greater than LoIndex - if not, set it to the last array entry
	if (HiIndex < LoIndex)
		HiIndex = Used-1;

	ERROR3IF(LoIndex >= Used || HiIndex >= Used || LoIndex > HiIndex, "Illegal start index(es)");

	// --- Check if the position is out of the original search bounds, or if it
	// is silly, and expand the bounds as necessary to make sure we will search all
	// relevant TrapEdges
	if (LoIndex >= HiIndex || LoIndex >= Used || Position <= pEdges[LoIndex].Position)
		LoIndex = 0;

	if (HiIndex <= LoIndex || HiIndex >= Used || Position >= pEdges[HiIndex].Position)
		HiIndex = Used-1;

	// --- Search the TrapEdges. This is done using a binary chop - we find the
	// midpoint between LoIndex & HiIndex, and move either Lo or Hi to that point,
	// so that the range reduces toward a single edge. This is far more efficient
	// than a linear search, especially on the large lists we typically deal with.
	while (HiIndex > LoIndex+1)
	{
		const UINT32 MidIndex = (LoIndex + HiIndex) / 2;
		if (pEdges[MidIndex].Position > Position)
			HiIndex = MidIndex;
		else
			LoIndex = MidIndex;
	}

	ERROR3IF(pEdges[LoIndex].Position > Position || pEdges[HiIndex].Position < Position,
				"I think the binary chop has screwed up");

	return(LoIndex);
}



/******************************************************************************************

>	void TrapEdgeList::AddEdge(DocCoord *pPoint, TrapJoinType JoinType = TrapJoin_None)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		pPoint		- The coordinate of the new centreline point
				JoinType	- Indicates if this edge completes a trapezoid which is part
							  of a join (and that join's type). This can be used by the
							  path stroker to correctly handle mitred and bevelled joins,
							  (and also to determine if line caps are needed, by looking
							  to see if the last TrapEdge is a join or a cap)

	Purpose:	Add a new TrapEdge structure to the list

				If this is a repeating stroke, this may create new TrapEdgeList(s)
				and start filling them in. The caller should always append edges
				to the last TrapEdgeList in their TrapsList to ensure it works OK.

				The added TrapEdge structure will be filled in with:
						Centre		 (will be pPoint)
						Position	 (will be the distance of the point, in millipoints,
									 from the start of the stroke)
						PrevTrapJoin (will be filled in with JoinType)

				NOTE that the Normal will NOT be initialised - see ProcessEdgeNormals()

******************************************************************************************/

BOOL TrapEdgeList::AddEdge(DocCoord *pPoint, TrapJoinType JoinType)
{
	// --- Make sure we have room to add the new point to our array
	DocCoord Temp;					// Temporary safe storage for the point
	if (Used >= CurrentSize)
	{
		// I pass in a pointer to the coord for efficiency, to save copying points until
		// absolutely necessary. This is safe under normal circumstances, except when
		// we realloc the array and the input pointer points INTO that array! Most of
		// the time, it works fine, but every now and then NT maps out the memory page
		// and we get a nasty access violation! Rather than copying the Coord every
		// time, I prefer therefore to copy it into a temporary point on those few
		// occasions where we actually have to realloc the array.
		Temp = *pPoint;				// Copy the point into safe, temporary storage
		pPoint = &Temp;				// And point the input value pointer at the safe copy

		if (!ExpandArray())			// Try to allocate more
			return FALSE;					// And return if we failed
	}

	// --- Find the new entry and allocate it
	TrapEdge *pEdge = &pEdges[Used];

	// --- Record the point & join type
	pEdge->Centre = *pPoint;
	pEdge->PrevTrapJoin = JoinType;

	// --- Calculate position, and check for repeating strokes
	if (Used > 0)
	{
		TrapEdge *pPrevEdge = &pEdges[Used-1];
		const double dx = (double) (pPrevEdge->Centre.x - pPoint->x);
		const double dy = (double) (pPrevEdge->Centre.y - pPoint->y);
		double Travel = sqrt(dx*dx + dy*dy);
		if (Travel < 1.0)		// Make sure all positions increment slightly to keep everyone happy
			Travel = 1.0;
		pEdge->Position = pPrevEdge->Position + Travel;

		// Now check for (and handle) repeating strokes
		const INT32 RepeatLength = (pParentList == NULL) ? 0 : pParentList->GetRepeatLength();
		if (RepeatLength > 0 && JoinType == TrapJoin_None)
		{
			// If it's a repeating stroke, and we're not in the middle of a join, then
			// we compare the current path travel to the repeat distance to see if we've gone
			// past the end of this repeat. If we have, then we calculate the point where the
			// (first) repeat in this trapezoid should happen, and break this trapezoid there,
			// ending the current TrapEdgeList, and starting a new TrapEdgeList.
			// Note: We recursively call AddEdge on each new TrapEdgeList so that if the
			// trapezoid contains many repeats, we will subdivide it further.

			if (pEdge->Position >= (double)RepeatLength)
			{
				// We've gone too far. Time to subdivide.
				ERROR3IF(pEdge->Position - pPrevEdge->Position <= 0.0, "Position calculation is screwed up!");

				const double Split = (((double)RepeatLength) - pPrevEdge->Position) / (pEdge->Position - pPrevEdge->Position);

				// Calculate the split point & position value into our last Edge
				pEdge->Centre.x = (INT32) (((1.0 - Split) * (double)pPrevEdge->Centre.x) + (Split * (double)pPoint->x));
				pEdge->Centre.y = (INT32) (((1.0 - Split) * (double)pPrevEdge->Centre.y) + (Split * (double)pPoint->y));
				pEdge->Position = (double) RepeatLength;

				// And create a new TrapEdgelist, and initialise it to go from the split point to the
				// edge which we were orignally passed - NOTE that this may RECURSE.
				TrapEdgeList *pNewEdgeList = pParentList->AddEdgeList();
				if (pNewEdgeList != NULL)
				{
					pNewEdgeList->AddEdge(&pEdge->Centre, TrapJoin_None);	// Add intersection point
					pNewEdgeList->AddEdge(pPoint, TrapJoin_None);			// then add the original end Edge
				}
			}
		}
	}
	else
		pEdge->Position = 0.0;

	// Finally, increment Used to move on to the next TrapEdge entry in our array
	Used++;

	return TRUE;
}



/******************************************************************************************

>	BOOL TrapEdgeList::ExpandArray(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Outputs:	On succesful exit, the member array of TrapEdge structures will be bigger
	Returns:	FALSE if it failed to allocate memory
				
	Purpose:	(Internal method)
				Expands the storage structure of the list to allow more entries to be
				used. Called automatically by AddEdge if it needs more edges.

	Notes:		Internal storage is an array of TrapEdge structures

				NOTE that the array is not initialised, as it is expected that each
				entry will be initialised on its first use. Initialisation would just
				double the overhead of creating an entry, and we do a lot of that!

******************************************************************************************/

BOOL TrapEdgeList::ExpandArray(void)
{
	// Work out how many entries to allocate. Note that I use a fairly large number
	// so that we don't have to reallocate the array very often. Each structure is
	// small, so the memory usage is fairly low, although it must be noted that
	// a dashed line could potentially generate quite a pile of these lists!
	INT32 AllocSize = CurrentSize + 512;
	if (pParentList != NULL && pParentList->GetRepeatLength() > 0)
	{
		// If it's a repeating stroke, then we must be far more careful about memory usage!
		// (i.e. if there are 1000 repeats along a stroke, each taking 15kB, we'll eat 15MB!)
		// We assume that repeats are small and hence will not need very many entries, and
		// then if (and only if) we find we have to realloc to make the array bigger, we jump
		// in bigger steps.
		if (CurrentSize == 0)
			AllocSize = CurrentSize + 4;
		else
			AllocSize = CurrentSize + 16;
	}

	if (pEdges == NULL)
	{
		// We have no array yet, so allocate one
		pEdges = (TrapEdge *) CCMalloc(AllocSize * sizeof(TrapEdge));
		if (pEdges == NULL)
			return(FALSE);
	}
	else
	{
		// We have an array - we must make it larger
		TrapEdge *pNewBuf = (TrapEdge *) CCRealloc(pEdges, AllocSize * sizeof(TrapEdge));
		if (pNewBuf == NULL)
			return(FALSE);

		pEdges = pNewBuf;
	}

	// Success. Update the current size value
	CurrentSize = AllocSize;
	return(TRUE);
}



/******************************************************************************************

>	BOOL TrapEdgeList::ProcessEdgeNormals(ProcessPathToTrapezoids *pProcessor)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		pProcessor	- The object which is calling us

	Outputs:	On succesful exit, the member array of TrapEdge structures will
				contain correct Normal values.

	Returns:	FALSE if it failed (Note: does not set an error at this level)

	Purpose:	Scans the recorded list of edges (added by AddEdge) and fills in the
				edge Normal information.

				MUST be called on the list before trying to use the edge structures!

				Call this before calling ProcessEdgePositions if you want positions as well.

******************************************************************************************/

BOOL TrapEdgeList::ProcessEdgeNormals(ProcessPathToTrapezoids *pProcessor)
{
	if (Used < 2)							// Sanity check
	{
		ERROR3("Not enough coordinates in trapezoid list!");
		return(FALSE);
	}

	TrapEdge *pPrevEdge	= NULL;
	TrapEdge *pEdge		= NULL;
	NormCoord PrevNormal(1.0, 0.0);
	BOOL CompletingAMitre = FALSE;

	// --- First, scan the trapezoid list, and calculate the normal of each edge
	for (UINT32 Index = 0; Index < Used; Index++)
	{
		// Start by finding a pointer to the edge record and previous record (if any)
		pPrevEdge = pEdge;
		pEdge = &(pEdges[Index]);

		// Calculate the path normal for this trapezoid edge.
		if (pPrevEdge == NULL)
		{
			// It's the very first point. The normal is just the normal to the first segment
			TrapEdge *pNextEdge = &(pEdges[Index+1]);
			pEdge->Normal.SetNormalToLine(pEdge->Centre, pNextEdge->Centre);

			PrevNormal = pEdge->Normal;		// Remember this edge's normal for the next pass
		}
		else if (pEdge->Centre == pPrevEdge->Centre)
		{
			// Two points are coincident. That means a joint (or a degenerate source path element).
			//
			// The following cases can apply:
			//	if (the next point is ALSO coincident)
			//		* We have a mitred join. The normal is the average of the previous and next normals
			//		  with some jiggery-pokery to make the normal also have a length which will get the
			//		  outline out to the mitre intersection point (rather than being normalised)
			//	else
			//		* We have the last 2 points of the 3-point mitred join, or
			//		* We have a simple bevelled or rounded join
			//			(both of which are treated in the same manner)

			// Find the "next" edge. Care must be taken for the last join to make sure
			// that the first edge is treated as "next". Note that we don't use point 0
			// in this case, as that is coincident! We use point 1 which is the point
			// at the end of that first line.
			TrapEdge *pNextEdge = &pEdges[1];
			if (Index < Used-1)
				pNextEdge = &pEdges[Index+1];

			if (pNextEdge->Centre == pEdge->Centre)
			{
				// We have found 3 coincident points, so we've hit a mitred join.
				// The middle Edge of the mitred join has a normal which is in the direction
				// of the average of the preceeding/next edge normals (i.e. points towards the
				// mitre intersection), but it has a non-unit-length, so as to stretch the
				// outline out to the mitre point.

				// In this case, we don't bother to do anything yet, as we'll calculate
				// the next normal on the next pass, so we just flag the case and let
				// the next pass come back and fix up our normal once it has all the facts.
				CompletingAMitre = TRUE;

				// NOTE that we leave PrevNormal containing the previous normal - we'll need it
				// on the next pass.
			}
			else
			{
				// The completing trap of any join simply uses the normal of the "next" edge,
				// so we simply calculate this for all cases.
				if (Index >= Used-1)
				{
					// At the end of the curve - we can just copy the normal from the first point
					pEdge->Normal = pEdges[0].Normal;
				}
				else
				{
					// Inside the path - just use the next edge to generate a normal
					pEdge->Normal.SetNormalToLine(pEdge->Centre, pNextEdge->Centre);
				}

				// However, now we must check if we've just completed a mitred join, in
				// which case we have to go back to fill in the previous edge normal
				if (CompletingAMitre)
				{
					// Find the vector pointing towards the mitre point
					pPrevEdge->Normal.Average(PrevNormal, pEdge->Normal);

					// We now know the direction the mitre intersection (pointy bit) lies in.
					// Now, we will "stretch" that normal by the ratio of the distance to
					// the intersection over the stroke width. 
					// We pass in the point before the join, the join centre, and the point
					// after the join. (Note that the join centre point occupies 3 edge entries)
					double MitreChordRatio = 1.0;
					pProcessor->CalculateMitreIntersection(	&pEdges[Index-3].Centre,
															&pEdge->Centre,
															&pNextEdge->Centre,
													/*TO*/	&MitreChordRatio);

					pPrevEdge->Normal.x *= MitreChordRatio;
					pPrevEdge->Normal.y *= MitreChordRatio;
				}

				CompletingAMitre = FALSE;		// Clear the mitre flag
				PrevNormal = pEdge->Normal;		// Remember this edge's normal for the next pass
			}
		}
		else
		{
			// This isn't the first point. It also is not "inside" a joint (although it could be
			// the last edge preceeding a join)
			if (Index >= Used-1)
			{
				// Last point - the normal is merely at right-angles to last line, so = (lineY,-lineX)
				// NOTE that this is not true if this is a closed path, but we will fix that after
				// the loop if it needs fixing
				pEdge->Normal = PrevNormal;
			}
			else
			{
				// Find normal to next line
				NormCoord NextNormal(pEdge->Centre.y - pEdges[Index+1].Centre.y, -(pEdge->Centre.x - pEdges[Index+1].Centre.x));

				if (NextNormal.x == 0.0 && NextNormal.y == 0.0)
				{
					// This point and the next point are coincident, so we must have hit a cusp join
					// (or rampant discontinuity) The normal is simply perpendicular to the last line
					pEdge->Normal = PrevNormal;
				}
				else
				{
					// This point lies between 2 flattened source path segments, so we simply average
					// their normals to get the path normal at the point.
					NextNormal.Normalise();
					pEdge->Normal.Average(PrevNormal, NextNormal);

					PrevNormal = NextNormal;		// Remember this normal for the next pass
				}
			}
		}
	}

	return(TRUE);
}



/******************************************************************************************

>	BOOL TrapEdgeList::ProcessEdgePositions(TrapTravelType TravelType)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		TravelType	- Describes how to record "positions" along the path:
								TrapTravel_None			Don't record travel (FASTEST)
								TrapTravel_Parametric	0.0 to 1.0 parametric range
								TrapTravel_Millipoint	Absolute millipoints distance

	Outputs:	On succesful exit, the member array of TrapEdge structures will contain
				properly calculated Position values.

	Returns:	FALSE if it failed (Note: does not set an error at this level)

	Purpose:	Scans the recorded list of edges (added by AddEdge) and fills in the
				position information according to TravelType. If you don't need Position
				values, then don't call this function (which will leave Postions totally
				uninitialised)

	Notes:		TrapTravel_None will leave edge "Position" values TOTALLY UNINITIALISED

				Call this AFTER calling ProcessEdgeNormals, as it relies on the normals
				calculated in that function to generate position values.

******************************************************************************************/

BOOL TrapEdgeList::ProcessEdgePositions(TrapTravelType TravelType)
{
	// We don't need to do anything unless the user wants parametric positions
	if (TravelType != TrapTravel_Parametric || Used < 1)
		return(TRUE);

	// --- Record the maximum position value as the millipoint length of the path
	// Note: When repeating, we record the length of the repeat rather than the
	// actual path length, as this makes the final repeat partial rather than squashed
	// when it doesn't all fit in.
	if (pParentList != NULL && pParentList->GetRepeatLength() > 0)
		PathLength = (double) pParentList->GetRepeatLength();
	else
		PathLength = pEdges[Used-1].Position;
	ERROR2IF(PathLength <= 0.0, FALSE, "No 'travel' in Stroke");

	// --- Normalise the Travel values to generate Position values in range 0.0 to 1.0
	// This is done by simply dividing them all by the position of the final edge
	if (PathLength > 0.0)
	{
		double R = 1.0/PathLength;
		for (UINT32 Index = 0; Index < Used; Index++)
			pEdges[Index].Position *= R;
	}

	return(TRUE);
}









///////////////////////////////////////////////////////////////////////////////////////////
// TrapsList class
///////////////////////////////////////////////////////////////////////////////////////////



/******************************************************************************************

>	TrapsList::TrapsList(INT32 Repeat = 0)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		Repeat	- 0 to generate a single stroke (TrapEdgeList) per subpath, which
						  will cause the stroke to be stretched along the entire path, or...

						  a millipoint repeat distance (which will cause many TrapEdgelists
						  to be generated), such that the stroke repeats along the path.

	Purpose:	Constructor

******************************************************************************************/

TrapsList::TrapsList(INT32 Repeat)
{
	Used		 = 0;
	CurrentSize  = 0;
	pTraps		 = NULL;
	RepeatLength = Repeat;
}



/******************************************************************************************

>	TrapsList::~TrapsList()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Purpose:	Destructor

******************************************************************************************/

TrapsList::~TrapsList()
{
	if (pTraps != NULL)
	{
		for (UINT32 Index = 0; Index < Used; Index++)
			delete pTraps[Index];

		CCFree(pTraps);
	}
}



/******************************************************************************************

>	TrapEdgeList *TrapsList::AddEdgeList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Returns:	NULL if it failed to allocate memory, else
				a pointer to a new TrapEdgeList object for you to use
				
	Purpose:	Add a new TrapEdgeList object to this list

******************************************************************************************/

TrapEdgeList *TrapsList::AddEdgeList(void)
{
	if (Used >= CurrentSize)		// If we've run out of room in the array
	{
		if (!ExpandArray())			// Try to allocate more
			return(NULL);			// And return NULl if we failed
	}

	TrapEdgeList *pObject = new TrapEdgeList(this);
	if (pObject == NULL)
		return(NULL);

	pTraps[Used] = pObject;
	Used++;

	return(pObject);
}



/******************************************************************************************

>	BOOL TrapsList::ExpandArray(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Outputs:	On succesful exit, the member array of TrapEdgeList objects will be bigger
	Returns:	FALSE if it failed to allocate memory
				
	Purpose:	(Internal method)
				Expands the storage structure of the list to allow more entries to be
				used. Called automatically by AddEdgeList if it needs more edges.

	Notes:		Internal storage is an array of TrapEdgeList _pointers_
				To use an item, you must therefore 

******************************************************************************************/

BOOL TrapsList::ExpandArray(void)
{
	// Work out how many entries to allocate.
	// Each item is small, so we allocate a reasonably large number
	// Note that potential implementation of dashed lines could create a lot of 
	// TrapEdgeLists (one per dash), so we need lots of spare capacity to cope
	// with this situation. I've therefore erred on the generous side.
	const INT32 AllocSize = CurrentSize + 64;

	if (pTraps == NULL)
	{
		// We have no array yet, so allocate one
		pTraps = (TrapEdgeList **) CCMalloc(AllocSize * sizeof(TrapEdgeList *));
		if (pTraps == NULL)
			return(FALSE);
	}
	else
	{
		// We have an array - we must make it larger
		TrapEdgeList **pNewBuf = (TrapEdgeList **) CCRealloc(pTraps, AllocSize * sizeof(TrapEdgeList *));
		if (pNewBuf == NULL)
			return(FALSE);

		pTraps = pNewBuf;
	}

	// Success. Update the current size value
	CurrentSize = AllocSize;

	// Initialise the pointers to safe values
	for (UINT32 Index = Used; Index < CurrentSize; Index++)
		pTraps[Index] = NULL;

	return(TRUE);
}



/******************************************************************************************

>	BOOL TrapsList::PostProcessLists(ProcessPathToTrapezoids *pProcessor,
									TrapTravelType TravelType)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		pProcessor	- The object which is calling us
				TravelType	- Describes how to record "positions" along the path:
								TrapTravel_None			Don't record travel (FASTEST)
								TrapTravel_Parametric	0.0 to 1.0 parametric range
								TrapTravel_Millipoint	Absolute millipoints distance


	Outputs:	On succesful exit, the member TrapEdgeList objects will be complete

	Returns:	FALSE if it failed to complete the operation
				(Note: No error is set at this level)

	Purpose:	Scans the recorded list of TrapEdgeLists and fills in the
				remaining information (normals and position values) for all
				trapezoids therein.

				MUST be called on the list before trying to use the edge structures!

	Notes:		TrapTravel_None will leave edge "Position" values TOTALLY UNINITIALISED

******************************************************************************************/

BOOL TrapsList::PostProcessLists(ProcessPathToTrapezoids *pProcessor,
									TrapTravelType TravelType)
{
// There is a problem with fixed repeating vector brushes sometimes doing a bit
// more than they should, in other words, a 6 repeat brush starting on the 7th
// repeat, or at least adding 1 trap to the list for it...
// The commented out stuff below was the last ever bit of code written by Jason.
// It was intended as a fix, but I (Richard) never got round to getting it compiling
// let alone testing it. Since the stroking job is now no more, and also since this
// comment is still here, I suspect the problem still remains. To see it in action
// you need to draw a wibbly filled freehand shape and pop the gallery up. The
// symptoms are a bizarre line to the right of the preview. It's tricky to get a
// repeating case...
// Jason, if you decided to come back, good on you mate... If not, may I wish
// whoever is reading this all the best with the stroking system - blimmin
// fabby, innit ?
/*	BOOL DiscardPartialRepeats = TRUE;

	if (RepeatDist > 0 && DiscardPartialRepeats && Used > 1)
	{
		TrapEdge *pEdge = pTraps[Used-1]->GetLastTrapEdge();
		if (pEdge != NULL && pEdge->Position < RepeatDist/2)
		{
			delete pTraps[Used-1];
			pTraps[Used-1] = NULL;
			Used--;
		}
	}*/

	for (UINT32 Index = 0; Index < Used; Index++)
	{
		if (pTraps[Index] != NULL)
		{
			if (!pTraps[Index]->ProcessEdgeNormals(pProcessor))
				return(FALSE);

			if (!pTraps[Index]->ProcessEdgePositions(TravelType))
				return(FALSE);
		}
	}

	return(TRUE);
}













///////////////////////////////////////////////////////////////////////////////////////////
// ProcessPathToTrapezoids  class
///////////////////////////////////////////////////////////////////////////////////////////



/******************************************************************************************

>	ProcessPathToTrapezoids::ProcessPathToTrapezoids(const double flat, TrapsList *pOutputList)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		flat		- Flatness value to use (see PathProcessor class)

	Purpose:	The One True Constructor. Don't call any other graven constructors.

******************************************************************************************/

ProcessPathToTrapezoids::ProcessPathToTrapezoids(const double flat)
						: ProcessPath(flat)
{
	pTraps			 = NULL;
	PointFollowsJoin = FALSE;
	JoinType		 = RoundJoin;
}



/******************************************************************************************

>	virtual BOOL ProcessPathToTrapezoids::Init(Path* pSource, TrapsList *pOutputList)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		pSource		- The path for which you want to build trapezoid lists

				pOutputList	- A pointer to the trapezoid list to be filled in when you
							  invoke the Process() function.
							  Note that all generated trapezoid edge lists are APPENDED
							  to this TrapsList, rather than overwriting it.
							  
				JoinStyle	- RoundJoin, MitredJoin, or BevelledJoin

	Purpose:	Initialises the PathProcessor in preparation for processing

	Notes:		Call this version of Init, not the base class one!

******************************************************************************************/

BOOL ProcessPathToTrapezoids::Init(Path* pSource, TrapsList *pOutputList)
{
	ERROR3IF(pOutputList == NULL, "Illegal NULL param");
	pTraps = pOutputList;

	// And init the base class
	return(ProcessPath::Init(pSource));
}



/******************************************************************************************

>	virtual BOOL ProcessPathToTrapezoids::Process(const ProcessFlags &PFlags,
													TrapTravelType TravelType,
													JointType JoinStyle = RoundJoin)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		PFlags		- the ProcessPath flags indicating how to process the path
								(Note that some flags should not be used with this process)

				TravelType	- Describes how to record "positions" along the path:
								TrapTravel_None			Don't record travel (FASTEST)
								TrapTravel_Parametric	0.0 to 1.0 parametric range
								TrapTravel_Millipoint	Absolute millipoints distance

				JoinStyle	- The type of joins in this path.
							  (RoundJoin, MitredJoin, or BevelledJoin)

	Purpose:	Processes the path given in Init() to produce a trapezoid list in the
				TrapsList given to Init().

	Notes:		* Call this version of Process, not the base class one!

				* To process a path, you should write code like this:

					ProcessPathToTrapezoids GenerateTraps(64);
					TrapsList OutputTraps;
					if (GenerateTraps.Init(pPath, &OutputTraps))
					{
						// Flags are: Flatten, QuantiseLines, QuantiseAll
						ProcessFlags PFlags(TRUE, FALSE, FALSE);
						if (!GenerateTraps.Process(PFlags, TrapTravel_Parametric, JoinStyle))
							return;
					}

				* TrapTravel_None will leave edge "Position" values TOTALLY UNINITIALISED
				  because to initialise them will waste time if you're not going to use 'em

******************************************************************************************/

BOOL ProcessPathToTrapezoids::Process(const ProcessFlags &PFlags,
										TrapTravelType TravelType, JointType JoinStyle)
{
	ERROR2IF(pTraps == NULL, FALSE, "Call Init to initialise the ProcessPathToTrapezoids object first!");

	JoinType		 = JoinStyle;
	PointFollowsJoin = FALSE;
	LastPoint		 = DocCoord(0,0);

	BOOL ok = ProcessPath::Process(PFlags);

	if (ok)
		ok = pTraps->PostProcessLists(this, TravelType);

	return(ok);
}



/******************************************************************************************

>	virtual BOOL ProcessPathToTrapezoids::CloseElement(BOOL done, PathVerb Verb, INT32 Index)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/97
	Inputs:		done	= true if the NewPoint function procesed all new points in the
						  open element correctly,
						  false if it did not.
				Verb	= verb of closing element.
				index	= index of closing element.
	Outputs:
	Returns:	FALSE to continue processing the next element
				TRUE to stop processing and return done

	Purpose:	Processes the close of LINETO and BEZIERTO elements to allow us
				to correctly handle joins in the path.

******************************************************************************************/

BOOL ProcessPathToTrapezoids::CloseElement(BOOL done, PathVerb Verb, INT32 Index)
{
	// If it's the end of a line/bezier, then it's a proper join. In this case,
	// we set a flag so we will know that the next point added follows a join.
	// For some join types we need to know which direction the path takes after
	// the join in order to determine what to output.
	if (Verb == PT_LINETO || Verb == PT_BEZIERTO)
		PointFollowsJoin = TRUE;

	return(FALSE);		// continue processing - all is well
}



/******************************************************************************************

>	virtual void ProcessPathToTrapezoids::CloseFigure(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/97

	Purpose:	Derived-class interface.
				Called after closing a LINETO or BEZIERTO element which constitutes
				the end of this figure (subpath) (as indicated by this element having
				the PT_CLOSEFIGURE flag set).

				Used by the path stroking ProcessPathToTrapezoids class to allow it
				to correctly handle joining the start & end of a closed figure.

******************************************************************************************/

void ProcessPathToTrapezoids::CloseFigure(void)
{
	// Find this subpath's edge list
	TrapEdgeList *pEdgeList = pTraps->GetLastTrapEdgeList();
	if (pEdgeList == NULL)
		return;

	// Find the first and last TrapEdges for this subpath. If either is NULL
	// or if they are the same, then we don't have enough trapezoids to bake
	// a cake, so we throw in the towel.
	TrapEdge *pFirstEdge = pEdgeList->GetTrapEdge(0);
	TrapEdge *pLastEdge	 = pEdgeList->GetLastTrapEdge();
	if (pFirstEdge == NULL || pLastEdge == NULL || pFirstEdge == pLastEdge)
		return;

	// Now, compare the edge points. If they are coincident, we must add a join
	// to complete the shape, but if they are not, we leave the path unclosed
	if (pFirstEdge->Centre == pLastEdge->Centre)
	{
		PointFollowsJoin = TRUE;
		NewPoint(PT_LINETO, NULL);
	}
}



/******************************************************************************************

>	BOOL ProcessPathToTrapezoids::NewPoint(PathVerb Verb, DocCoord *pCoord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		Verb		- Descriptor indicating the type of point to add
				pCoord		- The coordinate of the point (if NULL, this will
							  add a closing join to the trapezoid list)

	Outputs:	The new point will have been appended to the member trapezoid list
				in an appropriate manner. If preceeded by a bezier knot (join), 
				appropriate trapezoidal elements will have been added to represent
				the join.

	Returns:	TRUE if it wishes to continue processing the path
				(FALSE if it's run out of memory)

	Purpose:	Constructor

******************************************************************************************/

BOOL ProcessPathToTrapezoids::NewPoint(PathVerb Verb, DocCoord *pCoord)
{
	// Process any join we've just gone past. We have to delay processing of joins
	// until we know which way the curve headed off after the join, which is why
	// we are processing it now. If the new point is a MOVETO, then there is no join
	if (PointFollowsJoin && Verb == PT_LINETO)
	{
		// Find the last edge in the current edge list. If we can't find one, there is
		// nothing to "join to"
		TrapEdgeList *pEdgeList = pTraps->GetLastTrapEdgeList();
		TrapEdge *pEdge = NULL;
		if (pEdgeList != NULL)
			pEdge = pEdgeList->GetLastTrapEdge();

		if (pEdge != NULL)
		{
			switch(JoinType)
			{
				case MitreJoin:
					// A mitred join involves extending the previous and next segments of the outline
					// to their intersection. If there is no intersection, or if the intersection
					// is beyond the "MitreLimit", then we revert to a simple Bevelled join.
					// Otherwise, we need to add 2 line segments (3 trapezoid edges) joining the
					// end of the last segment to the intersection, and then from the intersection
					// to the start of this new segment.
					// These new segments are marked as being part of a join so that they will be
					// stroked as straight line segments rather than interpolating normals to make
					// a smoothed/curved join.

					{
						ERROR3IF(pEdgeList->GetNumEdges() < 2, "Not enough traps for mitred join");
						TrapEdge *pPrevEdge = pEdgeList->GetTrapEdge(pEdgeList->GetNumEdges() - 2);

						BOOL Mitred;
						if (pCoord != NULL)
						{
							Mitred = CalculateMitreIntersection(&pPrevEdge->Centre, &pEdge->Centre, pCoord);
						}
						else
						{
							// No pCoord passed in, so this is the join at the end. Use the 1st point
							// in the subpath as the "next point". (Well, the 2nd in the array, because point
							// 1 is coincident! We use point 2 which is the end of the 1st line)
							DocCoord NextCoord = pEdgeList->GetTrapEdge(1)->Centre;
							Mitred = CalculateMitreIntersection(&pPrevEdge->Centre, &pEdge->Centre, &NextCoord);
						}

						//BLOCK
						{
							// Nasty bodge - Because AddEdge can re-alloc the array, we CANNOT keep pointers
							// to array entries around over calls to AddEdge. We thus copy the edge point
							// into a temporary variable which we can safely use over the 2 calls the AddEdge
							DocCoord Temp = pEdge->Centre;

							// Add a single point for this join - by default this gives a Bevelled join
							pEdgeList->AddEdge(&Temp, TrapJoin_MitredOrBevelled);

							// And if it's Mitred, then add another point for this join
							if (Mitred)
								pEdgeList->AddEdge(&Temp, TrapJoin_MitredOrBevelled);
						}
					}
					break;

				case RoundJoin:
					// To make a rounded join, you might think we need to output a number of trapezoids,
					// but in fact the recursive flattened-mapping algorithm employed by the path
					// stroker will "smooth" a single trapezoid into a proper round join!
					// Thus, we simply insert another trapezoid on the join point (but do NOT mark
					// it as "part of a join") so that it will be mapped as a round join.
					pEdgeList->AddEdge(&pEdge->Centre, TrapJoin_Round);
					break;

				case BevelledJoin:
					// To make a bevelled join, we simply add another TrapEdge on the join point for
					// the start of the next segment, which will be joined with a straight line. 
					// However, the stroking mechanism will actually output that "line" as a round
					// join (due to its recursive flattened-mapping algorithm), so we have to
					// mark this point as "part of a join" so that it simply plonks down the
					// straight segment we want! (Hence the TRUE)
					pEdgeList->AddEdge(&pEdge->Centre, TrapJoin_MitredOrBevelled);
					break;

				default:
					ERROR3("Unsupported join type in ProcessPathToTrapezoids");
					break;
			}			
		}
	}

	// Clear the join flag, as any pending join has been processed
	PointFollowsJoin = FALSE;

	// If the provided coordinate was NULL, then they only wanted to add the join
	// information, so we exit now
	if (pCoord == NULL)
		return(TRUE);

	// Add the new point to the current trapezoid list
	switch(Verb)
	{
		case PT_MOVETO:
			{
				// A MoveTo signifies the start of a new (sub)path, so we start a new TrapList
				TrapEdgeList *pEdgeList = pTraps->AddEdgeList();
				if (pEdgeList == NULL)
					return(FALSE);

				pEdgeList->AddEdge(pCoord);
				LastPoint = *pCoord;
			}
			break;


		case PT_LINETO:
			{
				// Append each new point as a new trapezoid edge in the current TrapList
				// Find the last TrapEdgeList
				TrapEdgeList *pEdgeList = pTraps->GetLastTrapEdgeList();

				if (pEdgeList == NULL)
				{
					// We have started a path with a LineTo!
					// Create a new Trapezoid List (imply that this point is really a MOVETO)
					ERROR3("LINETO added to traplist with no preceding MOVETO");

					LastPoint = DocCoord(0,0);
					pEdgeList = pTraps->AddEdgeList();
					if (pEdgeList == NULL)
						return(FALSE);
				}

				// Append the new point to the current trap list. Check first to eliminate
				// coincident points (places where the source path has coincident points),
				// as we only want coincident points to occur in the special case of joins.
				if (pEdgeList->GetNumEdges() < 1 || LastPoint != *pCoord)
				{
					pEdgeList->AddEdge(pCoord);
					LastPoint = *pCoord;
				}
			}
			break;

		default:
			ERROR3("ProcessPathToTrapezoids only handles MOVETO and LINETO!");
			break;
	}

	return(TRUE);
}




/******************************************************************************************

>	BOOL ProcessPathToTrapezoids::CalculateMitreIntersection(DocCoord *p1, DocCoord *p2, DocCoord *p3,
															 double *pMitreRatio = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/97

	Inputs:		p1	- Centreline point prior to the join (see the diagram below)
				p2	- Centreline point where the join occurs
				p3	- Centreline point subsequent to the join

	Outputs:	pMitreRatio	- if non-NULL, and if the return value is TRUE, this will
							  be returned with the distance-ratio in it (see below)

	Returns:	TRUE	- if the join should be mitred, and the pIntersectionDist output contains
						  the mitre chord ratio.
				FALSE	- if the join should be bevelled. The pIntersectionDist param will be
						  untouched in this case.

	Purpose:	Mitred joins revert to bevelled joins when are so tight that the mitre
				exceeds the "mitre limit". This function determines and returns whether
				a given join should be rendered mitred or bevelled. In the former case,
				it also provides an output, which is based upon the distance from the
				center of the join (point 2, below) to the mitre outline intersection
				("*" below). The distance is not an absolute length, but rather, the
				ratio of that length to the line width.

				i.e. if you take point 2, and add it's "normal" (the vector toward the mitre
				point) and multiply it by Width*pIntersectionRatio, you'll arrive at the
				mitre intersection point. This may sound like an odd number to return,
				but in fact it is precisely the number I need.

						+--------:-----*
									  /			1,2,3 are the inputs p1, p2, p3
						1--------2   /
								/   :			':' are the points where stroke outline meets join
						---	   /   /
						 /	  /   /				* is the pIntersection output
						/	 3   +

******************************************************************************************/

BOOL ProcessPathToTrapezoids::CalculateMitreIntersection(DocCoord *p1, DocCoord *p2, DocCoord *p3,
															double *pMitreRatio)
{
	// Camelot never seems to actually set the MitreLimit in any render regions, so I have
	// taken on a default value which is the same as GDraw uses (GDraw uses 10.0).
	// Anyway, Gavin's MitreLimit is sensibly based on the angle/width ratio, whereas
	// our attributes are a random value in millipoints, which seems daft to me.
	const double MitreLimit = 1.0 / 10.0;	// (10.0 plus a bit of precalculation)

	// Work out the normalised direction vectors for the incoming and outgoing lines
	NormCoord v1(p1->x - p2->x, p1->y - p2->y);
	v1.Normalise();

	NormCoord v2(p3->x - p2->x, p3->y - p2->y);
	v2.Normalise();

	// From these vectors, we can now calculate cos(theta) using a dot product 
	// (where theta is the angle between the vectors), and taking the arc-cosine
	// of that gives us a huge surprise as a theta drops out.
	const double Theta = acos(v1.DotProduct(v2));
	const double SinHalfTheta = sin(Theta / 2.0);

	// If the angle is too tight, then we must bevel this (this also stops a divide by zero)
	if (fabs(SinHalfTheta) < MitreLimit)
		return(FALSE);

	if (pMitreRatio != NULL)
		*pMitreRatio = 1.0 / SinHalfTheta;

	return(TRUE);
}

