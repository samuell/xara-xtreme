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
// Definition of Path Stroking classes (used in stroke providing)

#ifndef INC_PATHSTRK
#define INC_PATHSTRK

#include "pathtrap.h"
//#include "attr.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "strkcomp.h"

class DocCoord;
class ValueFunction;
class Node;


///////////////////////////////////////////////////////////////////////////////////////////
// Base class
///////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************************************

>	class PathStroker : public CC_CLASS_MEMDUMP

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Purpose:	Abstract Base class
				Converts a Trapezoid (See ProcessPathToTrapezoids, pathtrap.h) stroke
				definition into an output path.

				The base class strokes a path with variable-width

******************************************************************************************/

class PathStroker : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(PathStroker);

public:			// Construction/Destruction
	PathStroker();
	PathStroker(ValueFunction *WidthFunction, INT32 LineWidth, LineCapType CapType);
	virtual ~PathStroker() { }

public:			// Public interface
	virtual BOOL StrokePath(TrapsList *pTraps, Path *pOutput);
			// Go forth and stroke thine path

	// Smoothed version of above...
	virtual BOOL StrokeSmoothPath(TrapsList *pTraps, Path *pOutput);


protected:		// Internal helper functions
	BOOL MapLineForwards(TrapEdgeList *Traps, Path *pOutput);
	void RecursiveMapLineForwards(	TrapEdge *pEdge1, DocCoord &Point1,
									TrapEdge *pEdge2, DocCoord &Point2,
									Path *pOutput);

	BOOL MapLineBackwards(TrapEdgeList *Traps, Path *pOutput);
	void RecursiveMapLineBackwards(	TrapEdge *pEdge1, DocCoord &Point1,
									TrapEdge *pEdge2, DocCoord &Point2,
									Path *pOutput);

	BOOL MapLineCap(LineCapType CapType, TrapEdge *pEdge, BOOL IsStartCap,
									Path *pOutput);

protected:
	ValueFunction *pWidthFunction;					// ValueFunction providing width information
	INT32 MaxWidth;									// Maximum width (in millipoints) of the stroke
	LineCapType CapStyle;							// Line cap style (butt, round, or square)
	INT32 RecursionDepth;							// Recursion depth counter to avoid stack overflows
};





///////////////////////////////////////////////////////////////////////////////////////////
// Derived classes
///////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************************************

>	class PathStrokerVector : public PathStroker

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/97

	Purpose:	Path stroker
				Converts a Trapezoid (See ProcessPathToTrapezoids, pathtrap.h) stroke
				definition into an output path.

				This "moulds" all the elements (beziers/lines) of a given path to lie
				within a stroke envelope defined by the provided trapezoid list

******************************************************************************************/

class PathStrokerVector : public PathStroker
{
	CC_DECLARE_MEMDUMP(PathStrokerVector);

public:
	PathStrokerVector(ValueFunction *WidthFunction, INT32 LineWidth,
						LineCapType CapType, DocRect *pSourceBounds);

public:			// Public interface
	void PrepareToStroke(TrapEdgeList *pTraps);
			// Prepare for stroking. Must be called before StrokePath/MapCoord

	virtual BOOL StrokePath(TrapsList *pTraps, Path *pOutput);	// DO NOT USE this version!
	virtual BOOL StrokePath(Path *pSourceVector, Path *pOutput);
			// Go forth and stroke thine path

	DocCoord MapCoord(DocCoord *pCoord);
			// Maps a coordinate across from the source space to the output space
			// Called internally, and also from FillGeometryAttribute::MouldIntoStroke

	double GetScaleFactor(void);
			// Returns a scaling factor indicating how much larger/smaller the mapped
			// brush is than it was in the original brush definition. This is used
			// to scale line widths etc to keep aspect ratios approximately correct.

protected:		// Internal helper functions
	void FindEdgeForCoord(DocCoord *pCoord, UINT32 StartIndex, INT32 Direction,
							/* TO */ TrapEdge *pOutput);
			// Computes a TrapEdge structure for the exact Position of a source Coord

	void MapMove(DocCoord *pCoord);
	void MapLine(DocCoord *pCoord1, DocCoord *pCoord2);
	void MapBezier(DocCoord *pCoords);
			// Map a Move/Line/BezierTo into the output path

	void RecursiveMapLine(TrapEdge *pEdge1, DocCoord &Point1, double Width1,
						  TrapEdge *pEdge2, DocCoord &Point2, double Width2,
						  INT32 Direction, Path *pOutput);
			// Recursively maps points from a straight line onto the stroke

	void RecursiveMapBezier(DocCoord *pCoords,	DocCoord *p1, double t1,
												DocCoord *p2, double t2);
			// Recursively maps points from a bezier onto the stroke

//////////////////////////////////////////////////////////////////////////////
public:
	static StrokeHandle GetCurrentBrush(void);
			// Returns the current brush. If there isn't one, it builds one

	static void BodgeRipSelection(BOOL Repeating = FALSE);
			// ****!!!! BODGE - will be moved elsewhere later. Rips the selection
			// out of the document and uses it as the new stroke brush
	static StrokeHandle CurrentStroke;
//////////////////////////////////////////////////////////////////////////////


private:		// Member globals to save passing loads of parameters
	Path		*pPath;								// Current source path from the brush
	DocRect		*pBounds;							// Bounds of the brush
	double		BoundsWidth;						// Cached width of the bounds
	double		BoundsHalfHeight;					// Cached half-height of the bounds
	double		BoundsYCentre;						// Cached centre (Y) of the bounds

	Path		*pOutput;							// Path to recieve mapped output data

	TrapEdgeList*pCurrentEdgeList;					// Currently active trapezoid edge list
	UINT32 FirstEdge;									// First trapedge containing the current path
	UINT32 LastEdge;									// Last trapedge containing the current path
};


#endif

