// $Id: bevtrap.h 662 2006-03-14 21:31:49Z alex $
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
#ifndef _BEVTRAP_H
#define _BEVTRAP_H

#include "pathproc.h"
#include "pathtrap.h"


class CCreateBevelTrapezoids : public CCObject
{
	CC_DECLARE_DYNCREATE(CCreateBevelTrapezoids);

public:
	// creates the bevel trap list
	BOOL ProcessTrapsForBevelling(Path *pPath, TrapsList *RetnTraps, INT32 indent, JointType jType,
							BOOL bOuter);

	// gives the inner point from a trap edge and puts it in dc
	static void CalcInnerPoint(TrapEdge * pEdge, INT32 indent, DocCoord * dc, BOOL bOuter);
	
#ifdef _DEBUG
	static void DumpList(TrapsList * pList);
#endif

	static BOOL CalculateIntersection(const DocCoord * start1, const NormCoord * dir1,
												   const DocCoord * start2, const NormCoord * dir2,
												   DocCoord * point, double * p, double * q);

	static void GetPathFromTraps(TrapsList * pTraps, const MILLIPOINT Width, Path * pPath);

	// elimintates the multiple points in the path
	static BOOL EliminateMultpilePoints(Path * pSrc, Path * pDest);	

	// calculates the intersection between two lines
private:

	// is this an outer turn on the path ?
	BOOL IsOuterTurn(NormCoord * nc1, NormCoord * nc2);

	// adds a given edge to the list
	BOOL AddEdgeToList(TrapEdgeList * pList, DocCoord * Centre, NormCoord * Normal, double Position,
							NormCoord * pNormal2=NULL);

	// smooths off the corners (otherwise they come out as being jaggy
	BOOL SmoothCorners(TrapsList * pList, TrapsList * RetnList, double Indent, BOOL bOuterBevel);

	// offsets index by 'offset', with maximum value being max-1
	// used to circle around the paths
	INT32 AlterIndex(INT32 index, INT32 max, INT32 offset);

	// creates the traps from the path
	BOOL ProcessPath(Path * pPath, TrapsList *RetnTraps, INT32 indent, BOOL bOuter, JointType jType);

	// creates a round join (recursive)
	void CreateRoundJoin(TrapEdgeList * pList, DocCoord * pCentre, 
											 NormCoord * pStartNorm, NormCoord * pEndNorm, 
											 INT32 depth = 0);

	// makes sure all inner points are at a consistant distance from the original path
	// based on where the outer points are
	void FlattenInnerPoints(TrapEdgeList * pList, INT32 indent, BOOL bOuter);

	// adds edges between centre 1 and centre 2 with the same inner point
	void RecursivelyAddEdges(TrapEdgeList * pList, DocCoord * pCentre1, DocCoord * pCentre2,
							 DocCoord * pInnerPoint, double MOneDivIndent, INT32 depth = 0);

} ;

#endif
