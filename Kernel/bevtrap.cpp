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
#include "camtypes.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "pathproc.h"
#include "pathtrap.h"
#include "bevtrap.h"
#include "beveler.h"
#include "ppbevel.h"

CC_IMPLEMENT_DYNCREATE(CCreateBevelTrapezoids, CCObject)

/********************************************************************************************
>	BOOL CCreateBevelTrapezoids::ProcessPath(Path * pPath, TrapsList *RetnTraps, INT32 indent);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/98
	Purpose:	Turns the path into trapezoids which can be used for the bevelling
	Inputs:		The source path and the trapezoid list (already initialised) to use
	Returns:	-
	Coments:	Only works on single paths i.e. only ones with PT_MOVETO as coord (0) and
				no-where else. In other words, no sub-paths are allowed.
	SeeAlso:	-

********************************************************************************************/
#if 0 // It is not clear what was commented out here and when and why - there are 3 ProcessPath functions! - AB20060106
/*
BOOL CCreateBevelTrapezoids::ProcessPath(Path * pPath, TrapsList *RetnTraps, INT32 indent,
										 BOOL bOuterBevel, JointType jType)
{
	if (!pPath)
		return TRUE;

	if (pPath->GetNumCoords() < 3)
		return TRUE;

	BevelHelpers::EliminateMultiplePoints(pPath);
	
	// calculate all the normals
	TrapEdgeList * pEdgeList = RetnTraps->AddEdgeList();
	TrapEdge	 * pEdge	 = NULL;
	TrapEdge	 * pEdgeBefore	 = NULL;
	TrapEdge	 * pEdgeAfter	 = NULL;

	NormCoord nctmp1;
	NormCoord nctmp2;
	NormCoord nc1;
	NormCoord nc2;
	NormCoord nc3;

	double len1 = 0.0;
	double len2 = 0.0;
	double dot = 0.0;
	double len = 0.0;

	for (INT32 i = 0; i < pPath->GetNumCoords()-1; i++)
	{
		nc1.x = -(pPath->GetCoordArray()[AlterIndex(i, pPath->GetNumCoords()-1, -1)].x - pPath->GetCoordArray()[i].x);
		nc1.y = -(pPath->GetCoordArray()[AlterIndex(i, pPath->GetNumCoords()-1, -1)].y - pPath->GetCoordArray()[i].y);
		nc2.x = +(pPath->GetCoordArray()[AlterIndex(i, pPath->GetNumCoords()-1, +1)].x - pPath->GetCoordArray()[i].x);
		nc2.y = +(pPath->GetCoordArray()[AlterIndex(i, pPath->GetNumCoords()-1, +1)].y - pPath->GetCoordArray()[i].y);
		len1 = nc1.GetLength();
		len2 = nc2.GetLength();

		if (len1 > 0.0 &&
			len2 > 0.0 &&
			(nc1.x != 0 || nc1.y != 0) &&
			(nc2.x != 0 || nc2.y != 0))
		{
			nc1.x /= len1;
			nc1.y /= len1;
			nc2.x /= len2;
			nc2.y /= len2;

			NormCoord nc4( -nc1.y,+nc1.x ) ;
			NormCoord nc5( -nc2.y,+nc2.x ) ;

			dot = nc1.x*nc2.x+nc1.y*nc2.y;

			if ( bOuterBevel != nc2.y*nc1.x>nc2.x*nc1.y )
				if ( jType==RoundJoin )
				{
					AddEdgeToList  (pEdgeList, &(pPath->GetCoordArray()[i]), &nc4,       0);
					CreateRoundJoin(pEdgeList, &(pPath->GetCoordArray()[i]), &nc4, &nc5   );
					AddEdgeToList  (pEdgeList, &(pPath->GetCoordArray()[i]),       &nc5, 0);
					
				}
				else if ( jType==BevelledJoin )
//				else if ( (jType==BevelledJoin || jType==MitreJoin && dot<-0.707) )
				{
					AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc4, 0);
					AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc5, 0);
				}
				else
				{
					if ( dot+1.0>0.0 )
					{
						len = 1.0/(dot+1.0) ;
						nc3.x = (nc4.x+nc5.x)*len;
						nc3.y = (nc4.y+nc5.y)*len;
						AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc3, 0);
					}
				}
			else
			{
				// add a single edge to the list to be coped with later
				// marking it with position 1
				AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc4, 0);
				AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc5, 0);
			}
		}
	}

	// add first edge as the last edge in the list
	if (pEdgeList && pEdgeList->GetNumEdges() > 0)
	{
		pEdge = pEdgeList->GetTrapEdge(0);
		if (pEdge)
			AddEdgeToList(pEdgeList, &(pEdge->Centre), &(pEdge->Normal), pEdge->Position);
	}

	return TRUE;
}
*/
/*
BOOL CCreateBevelTrapezoids::ProcessPath(Path * pPath, TrapsList *RetnTraps, INT32 indent,
										 BOOL bOuterBevel, JointType jType)
{
	if (!pPath)
		return TRUE;

	if (pPath->GetNumCoords() < 3)
		return TRUE;
	
	// calculate all the normals
	TrapEdgeList * pEdgeList = RetnTraps->AddEdgeList();
	TrapEdge	 * pEdge	 = NULL;
	TrapEdge	 * pEdge2	 = NULL;

	for (INT32 i = 0; i < pPath->GetNumCoords()-1; i++)
	{
		NormCoord nc1;
		NormCoord nc2;
		nc1.x = -(pPath->GetCoordArray()[AlterIndex(i, pPath->GetNumCoords()-1, -1)].x - pPath->GetCoordArray()[i].x);
		nc1.y = -(pPath->GetCoordArray()[AlterIndex(i, pPath->GetNumCoords()-1, -1)].y - pPath->GetCoordArray()[i].y);
		nc2.x = +(pPath->GetCoordArray()[AlterIndex(i, pPath->GetNumCoords()-1, +1)].x - pPath->GetCoordArray()[i].x);
		nc2.y = +(pPath->GetCoordArray()[AlterIndex(i, pPath->GetNumCoords()-1, +1)].y - pPath->GetCoordArray()[i].y);
		double len1 = nc1.GetLength();
		double len2 = nc2.GetLength();

		if (len1 > 0.0 &&
			len2 > 0.0 &&
			(nc1.x != 0 || nc1.y != 0) &&
			(nc2.x != 0 || nc2.y != 0))
		{
			nc1.x /= len1;
			nc1.y /= len1;
			nc2.x /= len2;
			nc2.y /= len2;

			NormCoord nc4( -nc1.y,+nc1.x ) ;
			NormCoord nc5( -nc2.y,+nc2.x ) ;

			bool bOuter = bOuterBevel != nc2.y*nc1.x>nc2.x*nc1.y ;

			if ( bOuter && jType==RoundJoin )
			{
				AddEdgeToList  (pEdgeList, &(pPath->GetCoordArray()[i]), &nc4,       0);
				CreateRoundJoin(pEdgeList, &(pPath->GetCoordArray()[i]), &nc4, &nc5   );
				AddEdgeToList  (pEdgeList, &(pPath->GetCoordArray()[i]),       &nc5, 0);
			}
			else if ( bOuter && jType==BevelledJoin )
//			else if ( bOuter && (jType==BevelledJoin || jType==MitreJoin && dot<-0.707) )
			{
				AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc4, 0);
				AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc5, 0);
			}
			else
			{
				double dot = nc1.x*nc2.x+nc1.y*nc2.y;
				if ( dot!=-1.0 )
				{
					double len = 1.0/(dot+1.0) ;
					NormCoord nc3;
					nc3.x = (nc4.x+nc5.x)*len;
					nc3.y = (nc4.y+nc5.y)*len;
					AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc3, 0);
	//				AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc3, !bOuter);
				}
			}
		}
	}

//	FlattenInnerPoints(pEdgeList, indent, bOuterBevel);

	// add first edge as the last edge in the list
	if (pEdgeList && pEdgeList->GetNumEdges() > 0)
	{
		pEdge = pEdgeList->GetTrapEdge(0);
		if (pEdge)
		{
			// MUST do it this way since the original edge memory allocation may
			// change with a change in the list
			DocCoord Centre2 = pEdge->Centre;
			NormCoord Normal2 = pEdge->Normal;

			AddEdgeToList(pEdgeList, &Centre2, &Normal2, 0);
		}
	}

	return TRUE;
}
*/
/*
BOOL CCreateBevelTrapezoids::ProcessPath(Path * pPath, TrapsList *RetnTraps, INT32 indent,
										 BOOL bOuterBevel, JointType jType)
{
	if (!pPath)
		return TRUE;

	if (pPath->GetNumCoords() < 3)
		return TRUE;
	
	// calculate all the normals
	TrapEdgeList * pEdgeList = RetnTraps->AddEdgeList();
	TrapEdge	 * pEdge	 = NULL;
	TrapEdge	 * pEdge2	 = NULL;

	NormCoord nc1;
	NormCoord nc2;
	NormCoord nc3;

	NormCoord nc4;
	NormCoord nc5;

	double dot = 0;
	double len1 = 0;
	double len2 = 0;
	double len3 = 0;
	double len  = 0;

	BOOL bOuter = FALSE;

	double dTmp = 0;

	for (INT32 i = 0; i < pPath->GetNumCoords()-1; i++)
	{
		nc1.x = pPath->GetCoordArray()[AlterIndex(i, pPath->GetNumCoords()-1, -1)].x - pPath->GetCoordArray()[i].x;
		nc1.y = pPath->GetCoordArray()[AlterIndex(i, pPath->GetNumCoords()-1, -1)].y - pPath->GetCoordArray()[i].y;
		nc1.x = -nc1.x;
		nc1.y = -nc1.y;

		len1 = nc1.GetLength();

		nc2.x = pPath->GetCoordArray()[AlterIndex(i, pPath->GetNumCoords()-1, +1)].x - pPath->GetCoordArray()[i].x;
		nc2.y = pPath->GetCoordArray()[AlterIndex(i, pPath->GetNumCoords()-1, +1)].y - pPath->GetCoordArray()[i].y;
		len2 = nc2.GetLength();

		if (len1 > 0.0 &&
			len2 > 0.0 &&
			(nc1.x != 0 || nc1.y != 0) &&
			(nc2.x != 0 || nc2.y != 0))
		{
			nc1.x /= len1;
			nc1.y /= len1;
			nc2.x /= len2;
			nc2.y /= len2;

			dot = nc1.x * nc2.x + nc1.y * nc2.y;

			bOuter = bOuterBevel^IsOuterTurn(&nc1, &nc2);
			
			nc3.x = (-nc1.x) + nc2.x;
			nc3.y = (-nc1.y) + nc2.y;

			if (dot >= 0.05)
			{
				// flat cap, for large outer turn
				// rotate both normals in & out by 90 degrees
				
				nc4.x = -nc1.y;
				nc4.y = nc1.x;
				
				nc5.x = -nc2.y;
				nc5.y = nc2.x;

				if (bOuter)
				{
					
					nc3.x = (nc4.x + nc5.x) / 2.0;
					nc3.y = (nc4.y + nc5.y) / 2.0;
					
					len = nc3.GetLength();

					if (len > 0)
					{
						nc3.x /= len;
						nc3.y /= len;
					}

					if (len > 0)
					{
						if (jType == RoundJoin)
						{
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc4, 0);
							CreateRoundJoin(pEdgeList, &(pPath->GetCoordArray()[i]), &nc4, &nc5, &nc3);
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc5, 0);
						}
						else if (jType == MitreJoin)
						{
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc3, 0);
						}
						else if (jType == BevelledJoin)
						{
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc4, 0);
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc5, 0);
						}							
					}
				}
				else
				{
					
					nc3.x = (nc4.x + nc5.x);
					nc3.y = (nc4.y + nc5.y);

					len = nc3.GetLength();
					if (len > 0)
					{
						if (!bOuterBevel || dot > 0.707)
						{
							nc3.x /= len;
							nc3.y /= len;					
						}
						AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc3, 1);
					}
				}
			}
			else
			{
				
				nc4.x = -nc1.y;
				nc4.y = nc1.x;
				
				nc5.x = -nc2.y;
				nc5.y = nc2.x;
				
				// calculate the centre point
				nc3.x = nc2.x - nc1.x;
				nc3.y = nc2.y - nc1.y;
				
				if (!bOuterBevel)
				{
					if (bOuter)
					{
						len3 = nc3.GetLength();						
						if (len3 > 0)
						{
							nc3.x /= len3;
							nc3.y /= len3;
							
						}

						if (jType == RoundJoin)
						{
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc4, 0);
							CreateRoundJoin(pEdgeList, &(pPath->GetCoordArray()[i]), &nc4, &nc5, &nc3);
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc5, 0);
						}
						else if (jType == MitreJoin)
						{
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc3, 0);
						}
						else if (jType == BevelledJoin)
						{
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc4, 0);
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc5, 0);
						}							
					}
					else
					{
						nc3.x = -nc3.x;
						nc3.y = -nc3.y;

						len = nc3.GetLength();

						AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc3, 1);
					}
				}
				else
				{
					// rotate these by 90 degrees
					nc4.x = nc1.x;
					nc4.y = nc1.y;
						
					dTmp = nc4.x;
					nc4.x = -nc4.y;
					nc4.y = dTmp;
						
					nc5.x = -nc2.x;
					nc5.y = -nc2.y;
						
					nc3.x = -nc3.x;
					nc3.y = -nc3.y;
						
					dTmp = nc5.x;
					
					nc5.x = nc5.y;
					nc5.y = -dTmp;
					
					if (bOuter)
					{
						len3 = nc3.GetLength();
						if (len3 > 0)
						{
							nc3.x /= len3;
							nc3.y /= len3;
						}

						if (jType == RoundJoin)
						{
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc4, 0);
							CreateRoundJoin(pEdgeList, &(pPath->GetCoordArray()[i]), &nc4, &nc5, &nc3);
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc5, 0);
						}
						else if (jType == MitreJoin)
						{
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc3, 0);
						}
						else if (jType == BevelledJoin)
						{
							nc4.x = -nc1.y;
							nc4.y = nc1.x;

							nc5.x = -nc2.y;
							nc5.y = nc2.x;
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc4, 0);
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc5, 0);
						}											
					}
					else
					{	
						nc3.x = -nc3.x;
						nc3.y = -nc3.y;

						len = nc3.GetLength();

						if (len > 0)
						{
							if (!bOuterBevel || dot < -0.707)
							{
								nc3.x /= len;
								nc3.y /= len;
							}
						}

						if (len > 0)
						{
							AddEdgeToList(pEdgeList, &(pPath->GetCoordArray()[i]), &nc3, 1);
						}
						
					}
				}
			}		
		}
	}

	FlattenInnerPoints(pEdgeList, indent, bOuterBevel);

	// add first edge as the last edge in the list
	if (pEdgeList)
	{
		if (pEdgeList->GetNumEdges() > 0)
		{
			pEdge = pEdgeList->GetTrapEdge(0);

			if (pEdge)
			{
				AddEdgeToList(pEdgeList, &(pEdge->Centre), &(pEdge->Normal), pEdge->Position);
			}
		}
	}

	return TRUE;
}
*/

/********************************************************************************************
>	void CCreateBevelTrapezoids::ProcessTrapsForBevelling(Path * pPath, 
						TrapsList *RetnTraps, INT32 indent)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/98
	Purpose:	Takes a path and gives out the trapezoid list to use
	Inputs:		The traps list to use as source, the traps list to enter the data into
				and the indent to use (if negative, then this denotes an outer bevel).
	Returns:	-
	Coments:	'Rounds' corners according to line cap.
				Inner turns are rounded so that highlights occur
				Outer turns are capped
	SeeAlso:	-

********************************************************************************************/

BOOL CCreateBevelTrapezoids::ProcessTrapsForBevelling(Path *pPath, TrapsList *RetnTraps, 
													  INT32 indent, JointType jType,
													  BOOL bOuter)
{
	if (!pPath || !RetnTraps || indent == 0)
		return FALSE;

	BOOL bSmooth = FALSE;

	if (bOuter || CBeveler::m_bQuick)
		bSmooth = FALSE;
	
	// temporary traps list
	TrapsList * pTmpTraps = new TrapsList;

	// first, flatten the path
	Path FlatPath;
	FlatPath.Initialise();

	ProcessFlatten pf(BEVEL_FLATNESS);
	ProcessFlags pfFlags(TRUE, FALSE, FALSE);
	pf.FlattenPath(pfFlags, pPath, &FlatPath);

	Path SubPath;
	SubPath.Initialise();

	Path EliminatedPath;
	EliminatedPath.Initialise();

	EliminateMultpilePoints(&FlatPath, &EliminatedPath);
	pPath = &EliminatedPath;

	DocCoord LastPoint(0,0);
	DocCoord ThisPoint;
	DocCoord dc1;
	DocCoord dc2;

	TrapsList * pDestTraps = NULL;

	// don't do smoothing when told not to
	if (!bSmooth)
	{
		pDestTraps = RetnTraps;
	}
	else
	{
		pDestTraps = pTmpTraps;
	}
	
	// need to seperate subpaths out, and then use ProcessPath to turn them into trapezoid lists
	for (INT32 i = 0 ; i < FlatPath.GetNumCoords(); i++)
	{
		// new sub-path ?
		if (FlatPath.GetVerbArray()[i] == PT_MOVETO || (i == FlatPath.GetNumCoords() - 1))
		{
			// make sure we don't miss the last point
			if (i == FlatPath.GetNumCoords() - 1)
			{
				// since path has been flattened, then just add the points in as line-tos
				ThisPoint = FlatPath.GetCoordArray()[i];

				if (ThisPoint.x != LastPoint.x ||
					ThisPoint.y != LastPoint.y)
				{
					SubPath.AddLineTo(ThisPoint);
				}

				LastPoint = ThisPoint;
			}
			
			// process the path into trapezoids
			if (SubPath.GetNumCoords() > 0 && i > 0)
			{
				// call process path
				// but check that the first point & last points in the paths are
				// the same (i.e. it is closed !)
				dc1 = SubPath.GetCoordArray()[0];
				dc2 = SubPath.GetCoordArray()[SubPath.GetNumCoords() - 1];

				if (dc1.x != dc2.x ||
					dc1.y != dc2.y)
				{
					SubPath.AddLineTo(dc1);
				}
	
				if (!ProcessPath(&SubPath, pDestTraps, indent, bOuter, jType))
				{
					delete pTmpTraps;
					pTmpTraps = NULL;
					return FALSE;
				}
			}
			
			// clear the path and add the new point
			SubPath.ClearPath(FALSE);
			SubPath.AddMoveTo(FlatPath.GetCoordArray()[i]);
		}
		else
		{
			// since path has been flattened, then just add the points in as line-tos
			ThisPoint = FlatPath.GetCoordArray()[i];

			if (ThisPoint.x != LastPoint.x ||
				ThisPoint.y != LastPoint.y)
			{
				SubPath.AddLineTo(ThisPoint);
			}

			LastPoint = ThisPoint;
		}
	}

	if (bSmooth)
	{
		if (!SmoothCorners(pDestTraps, RetnTraps, (double)indent, bOuter))
		{
			delete pTmpTraps;
			pTmpTraps = NULL;
			return FALSE;
		}
	}
		
	delete pTmpTraps;
	pTmpTraps = NULL;
	
	return TRUE;
}
#endif

/********************************************************************************************
>	void CCreateBevelTrapezoids::CalculateIntersection(DocCoord * start1, NormCoord * dir1,
												   DocCoord * start2, NormCoord * dir2,
												   DocCoord * point, double * p, double * q)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/9/98
	Purpose:	Calculates the intersection between 2 lines
	Inputs:		Start point and direction of each of the lines
	Returns:	The point in *point and the distance along dir1 from start1 of this point.
				Also returns FALSE if the lines are in the same direction (i.e. no intersection)
	SeeAlso:	-

********************************************************************************************/

BOOL CCreateBevelTrapezoids::CalculateIntersection(const DocCoord * start1, const NormCoord * dir1,
												   const DocCoord * start2, const NormCoord * dir2,
												   DocCoord * point, double * p, double * q)
{
	// just turning the start points into doubles
	NormCoord nStart1;
	NormCoord nStart2;
		
	nStart1.x = start1->x;
	nStart1.y = start1->y;

	nStart2.x = start2->x;
	nStart2.y = start2->y;

	double div = (dir2->y * dir1->x - dir2->x * dir1->y);
	if (div == 0)
		return FALSE;
	
	// find the parametric value for the first line defining the point
	double rdiv = 1.0/div;

	double a = dir2->y * (nStart2.x - nStart1.x) + dir2->x * (nStart1.y - nStart2.y);
	a *= rdiv;

	point->x = (INT32)(nStart1.x + dir1->x * a);
	point->y = (INT32)(nStart1.y + dir1->y * a);

	*p = a;

	a = dir1->y * (nStart2.x - nStart1.x) + dir1->x * (nStart1.y - nStart2.y);
	a *= rdiv;

	*q = a;

	return TRUE;
}
#if 0
/********************************************************************************************
>	BOOL CCreateBevelTrapezoids::IsOuterTurn(NormCoord * nc1, NormCoord * nc2);
				
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/98
	Purpose:	Given the two vectors, from the trapezoid list, returns whether a right
				or left turn exists (right turns are outer)
	Inputs:		Two direction vectors, in & out of a single point
	Returns:	TRUE for outer & FALSE for inner turns
	SeeAlso:	-

********************************************************************************************/

BOOL CCreateBevelTrapezoids::IsOuterTurn(NormCoord * nc1, NormCoord * nc2)
{
	return nc2->y*nc1->x > nc2->x*nc1->y ;
}

/********************************************************************************************
>	BOOL CCreateBevelTrapezoids::AddEdgeToList(TrapEdgeList * pList, DocCoord * Centre, 
										   NormCoord * Normal, double Position,
										   NormCoord * pNormal2=NULL)

				
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/98
	Purpose:	Adds an edge to the given list
	Inputs:		Traps list to add this to, the centre point, normal and position of the edge
				Can also use a secondary normal - used when marking points
	Returns:	TRUE for success
	SeeAlso:	-

********************************************************************************************/

BOOL CCreateBevelTrapezoids::AddEdgeToList(TrapEdgeList * pList, DocCoord * Centre, 
										   NormCoord * Normal, double Position, NormCoord * pNormal2)
{
	if (!Normal || !pList || !Centre)
		return TRUE;

	if (Normal->x == 0 && Normal->y == 0)
	{
		return TRUE;
	}

	if (!pList->AddEdge(Centre, TrapJoin_MitredOrBevelled))
	{
		ERROR3("Add edge failed");
		return FALSE;
	}

	TrapEdge * pNewEdge = pList->GetTrapEdge(pList->GetNumEdges()-1);

	if (pNewEdge)
	{
		pNewEdge->Normal.x = Normal->x;
		pNewEdge->Normal.y = Normal->y;
		pNewEdge->Position = Position;

		if (pNormal2)
		{
			pNewEdge->Normal2.x = pNormal2->x;
			pNewEdge->Normal2.y = pNormal2->y;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

/********************************************************************************************
>	BOOL CCreateBevelTrapezoids::SmoothCorners(TrapsList * pList, TrapsList * RetnList, 
				double Indent, BOOL bOuterBevel)

				
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/98
	Purpose:	Creates a traps list with the corners smoothed out
	Inputs:		Input traps list and output traps list
	Returns:	TRUE for success
	SeeAlso:	-

********************************************************************************************/

BOOL CCreateBevelTrapezoids::SmoothCorners(TrapsList * pList, TrapsList * RetnList, double Indent,
										   BOOL bOuterBevel)
{
	TrapEdgeList * pEdgeList	= NULL;
	TrapEdge     * pEdge		= NULL;
	TrapEdge     * pEdgeBefore	= NULL;
	TrapEdge     * pEdgeAfter	= NULL;
	TrapEdge	 * pTmpEdge		= NULL;

	TrapEdgeList * pNewEdgeList = NULL;

	NormCoord ncBefore;
	NormCoord ncAfter;

	// perpendiculars to the above
	NormCoord ncPerpBefore;
	NormCoord ncPerpAfter;

	NormCoord ncNormal;

	DocCoord dcInnerPoint;

	DocCoord dcIntersection;

	double MOneDivIndent = -1/Indent;

	// parametric variables
	double p = 0;
	double q = 0;

	double dot = 0;
	UINT32 j = 0;

	BOOL bOuter = FALSE;

	for (UINT32 i = 0; i < pList->GetNumTraps(); i++)
	{
		pEdgeList = pList->GetTrapEdgeList(i);

		pNewEdgeList = RetnList->AddEdgeList();

		for (j = 0; j < pEdgeList->GetNumEdges()-1; j++)
		{
			pEdge = pEdgeList->GetTrapEdge(j);
			
			if (pEdge)
			{
				
				// set up the before and after edges (start & end edges are the same so
				// need treating differently)
				if (j == 0)
				{
					pEdgeBefore = pEdgeList->GetTrapEdge(pEdgeList->GetNumEdges() - 2);
					pEdgeAfter  = pEdgeList->GetTrapEdge(1);
				}
				else
				{
					pEdgeAfter = pEdgeList->GetTrapEdge(j + 1);
					pEdgeBefore = pEdgeList->GetTrapEdge(j - 1);
				}
				
				// ok, now we work out the vectors before and after the point on the path
				if (pEdgeBefore)
				{
					ncBefore.x = pEdgeBefore->Centre.x - pEdge->Centre.x;
					ncBefore.y = pEdgeBefore->Centre.y - pEdge->Centre.y;
				}
				else
				{
					ncBefore.x = 0;
					ncBefore.y = 0;
				}
				
				if (pEdgeAfter)
				{
					ncAfter.x = pEdgeAfter->Centre.x - pEdge->Centre.x ;
					ncAfter.y = pEdgeAfter->Centre.y - pEdge->Centre.y ;
				}
				else
				{
					ncAfter.x = 0;
					ncAfter.y = 0;
				}
				
				// calculate the inner point according to the edge normal
				dcInnerPoint.x = (INT32)(((double)(pEdge->Centre.x)) - (pEdge->Normal.x * Indent));
				dcInnerPoint.y = (INT32)(((double)(pEdge->Centre.y)) - (pEdge->Normal.y * Indent));
				
				// now, calculate the perpendicular intersections with the before & after lines
				ncPerpBefore.x = ncBefore.y;
				ncPerpBefore.y = -ncBefore.x;
				
				ncPerpAfter.x = -ncAfter.y;
				ncPerpAfter.y = ncAfter.x;
				
				// is this an inner or outer turn ?
				bOuter = bOuterBevel^IsOuterTurn(&ncPerpBefore, &ncPerpAfter);
				
				// get the dot product - only do on near to 90 degree angles
				dot = ncBefore.x * ncAfter.x + ncBefore.y * ncAfter.y;
				
				// calculate the intersection before
				if (CalculateIntersection(&dcInnerPoint, &ncPerpBefore,
					&(pEdge->Centre), &ncBefore,
					&dcIntersection, &p, &q) && !bOuter && dot > -0.1 && dot < 0.1)
				{
					// ok, intersection successful so add the point in
					// only add the point up to half way along the outside path, otherwise we
					// generate overlaps
					if (q > 0.5)
					{
						// recalculate intersection
						dcIntersection.x = (INT32)( (ncBefore.x * 0.5) + pEdge->Centre.x );
						dcIntersection.y = (INT32)( (ncBefore.y * 0.5) + pEdge->Centre.y );
					}
					
					// start half way along
					// dcIntersection.x = (dcIntersection.x + pEdge->Centre.x) / 2;
					// dcIntersection.y = (dcIntersection.y + pEdge->Centre.y) / 2;
					
					RecursivelyAddEdges(pNewEdgeList, &dcIntersection, &(pEdge->Centre), 
						&dcInnerPoint, MOneDivIndent, 0);
				}
				
				// insert the original edge
				AddEdgeToList(pNewEdgeList, &(pEdge->Centre), &(pEdge->Normal), pEdge->Position);
				
				// calculate the intersection after
				if (CalculateIntersection(&dcInnerPoint, &ncPerpAfter,
					&(pEdge->Centre), &ncAfter,
					&dcIntersection, &p, &q) && !bOuter && dot > -0.1 && dot < 0.1)
				{
					// ok, intersection successful so add the point in
					// only add the point up to half way along the outside path, otherwise we
					// generate overlaps
					
					if (q > 0.5)
					{
						// recalculate intersection
						dcIntersection.x = (INT32)( (ncAfter.x * 0.5) + pEdge->Centre.x );
						dcIntersection.y = (INT32)( (ncAfter.y * 0.5) + pEdge->Centre.y );
					}
					
					// start half way along
					// dcIntersection.x = (dcIntersection.x + pEdge->Centre.x) / 2;
					// dcIntersection.y = (dcIntersection.y + pEdge->Centre.y) / 2;
					
					RecursivelyAddEdges(pNewEdgeList, &(pEdge->Centre), &dcIntersection,
						&dcInnerPoint, MOneDivIndent, 0);
				}
			}

			// all done !
		}

		// add the first edge in for the last edge
		pEdge = pNewEdgeList->GetTrapEdge(0);

		if (pEdge)
		{
			AddEdgeToList(pNewEdgeList, &(pEdge->Centre), &(pEdge->Normal), pEdge->Position);
		}
	}

	return TRUE;
}

#ifdef _DEBUG
void CCreateBevelTrapezoids::DumpList(TrapsList * pList)
{
	// dump the traps list
	TrapEdgeList * pEdgeList = NULL;
	TrapEdge	 * pEdge = NULL;
	for (UINT32 i = 0 ; i < pList->GetNumTraps(); i++)
	{
		pEdgeList = pList->GetTrapEdgeList(i);
		TRACE( _T("//////////////////////\nEdge list %d\n"), i);

		for (UINT32 j = 0 ; j < pEdgeList->GetNumEdges(); j++)
		{
			pEdge = pEdgeList->GetTrapEdge(j);
			
			TRACE( _T("Edge : %d : %d %d, %f %f\n"),
				j, pEdge->Centre.x, pEdge->Centre.y, pEdge->Normal.x, pEdge->Normal.y);
		}
	}
}

#endif

/********************************************************************************************
>	void CCreateBevelTrapezoids::CalcInnerPoint(TrapEdge * pEdge, INT32 indent, DocCoord *dc)
										   

				
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/98
	Purpose:	Calculates the inner point of an edge and puts it indc
	Inputs:		
	Returns:	
	SeeAlso:	-

********************************************************************************************/

void CCreateBevelTrapezoids::CalcInnerPoint(TrapEdge * pEdge, INT32 indent, DocCoord *dc,
											BOOL bOuter)
{
	if (!bOuter)
	{
		dc->x = (INT32)((double)pEdge->Centre.x - pEdge->Normal.x * indent);
		dc->y = (INT32)((double)pEdge->Centre.y - pEdge->Normal.y * indent);
	}
	else
	{
		dc->x = (INT32)((double)pEdge->Centre.x + pEdge->Normal.x * indent);
		dc->y = (INT32)((double)pEdge->Centre.y + pEdge->Normal.y * indent);
	}

}

/********************************************************************************************
>	void CCreateBevelTrapezoids::RecursivelyAddEdges(TrapEdgeList * pList, 
							 DocCoord * pCentre1, DocCoord * pCentre2,
							 DocCoord * pInnerPoint, double MOneDivIndent, INT32 depth = 0);
										   

				
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/98
	Purpose:	Recursively adds edges between centre1 and centre 2 with the same inner point
	Inputs:		
	Returns:	
	SeeAlso:	-

********************************************************************************************/

void CCreateBevelTrapezoids::RecursivelyAddEdges(TrapEdgeList * pList, DocCoord * pCentre1, DocCoord * pCentre2,
							 DocCoord * pInnerPoint, double MOneDivIndent, INT32 depth)
{
	// calculate centre point
	DocCoord dcIntersection;
	dcIntersection.x = (INT32)((pCentre1->x + pCentre2->x) * 0.5);
	dcIntersection.y = (INT32)((pCentre1->y + pCentre2->y) * 0.5);

	NormCoord ncNormal;
	ncNormal.x = pInnerPoint->x - dcIntersection.x;
	ncNormal.y = pInnerPoint->y - dcIntersection.y;

	ncNormal.x *= MOneDivIndent;
	ncNormal.y *= MOneDivIndent;

	if (depth >= 2)
	{
		AddEdgeToList(pList, &dcIntersection, &ncNormal, 1);
		return;
	}

	RecursivelyAddEdges(pList, pCentre1, &dcIntersection, pInnerPoint, MOneDivIndent, depth + 1);
	AddEdgeToList(pList, &dcIntersection, &ncNormal, 1);
	RecursivelyAddEdges(pList, &dcIntersection, pCentre2, pInnerPoint, MOneDivIndent, depth + 1);
}



INT32 CCreateBevelTrapezoids::AlterIndex(INT32 index, INT32 max, INT32 offset)
{
	index += offset;

	while (index >= max)
	{
		index -= max;
	}

	while (index < 0)
	{
		index += max;
	}

	return index;
}

/********************************************************************************************
>	BOOL CCreateBevelTrapezoids::EliminateMultpilePoints(Path * pSrc, Path * pDest);
										   

				
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/98
	Purpose:	Eliminates any multiple points in the given path
	Inputs:		Source path to eliminate the multiple points and the destination path
				to put the new path into : pDest must be cleared & initialised
	Returns:	
	SeeAlso:	-

********************************************************************************************/

BOOL CCreateBevelTrapezoids::EliminateMultpilePoints(Path * pSrc, Path * pDest)
{
	if (!pSrc || !pDest)
	{
		return FALSE;
	}

	pDest->ClearPath(FALSE);

	DocCoord dcLastCoord = pSrc->GetCoordArray()[0];

	// add first point into the new path
	pDest->AddMoveTo(dcLastCoord);

	for (INT32 i = 1 ; i < pSrc->GetNumCoords(); i++)
	{
		if (pSrc->GetVerbArray()[i] == PT_MOVETO)
		{
			dcLastCoord = pSrc->GetCoordArray()[i];
			pDest->AddMoveTo(dcLastCoord);
		}
		else
		{
			// ok, is this point the same as the last ?
			if (dcLastCoord.x != pSrc->GetCoordArray()[i].x ||
				dcLastCoord.y != pSrc->GetCoordArray()[i].y)
			{
				dcLastCoord = pSrc->GetCoordArray()[i];
				pDest->AddLineTo(dcLastCoord);
			}
			else
			{
			}
		}
	}

	return TRUE;
}

/********************************************************************************************
>	void CCreateBevelTrapezoids::CreateRoundJoin(TrapEdgeList * pList, DocCoord * pCentre, 
		DocCoord * pStartNorm,
		DocCoord * pEndNorm, INT32 depth)
									   

				
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/98
	Purpose:	Creates a round join at end of the given list
	Inputs:		pList		-	The trap edge list to add the join to
				pCentre		-	The centre point of the join
				pStartNorm	-	The starting normal
				pEndNorm	-	The end normal
				depth		-	The recursion depth

	Returns:	
	SeeAlso:	-

********************************************************************************************/

void CCreateBevelTrapezoids::CreateRoundJoin(TrapEdgeList * pList, DocCoord * pCentre, 
											 NormCoord * pStartNorm, NormCoord * pEndNorm, 
											 INT32 depth)
{
	double dot = pStartNorm->x*pEndNorm->x+pStartNorm->y*pEndNorm->y;

	if ( dot<0.9915 && depth<6 )		// Rely on angle (arccos dot) not depth as
	{									// limit of recursion. Angle is about 7.5 degrees.

		// Work out the centre normal coordinate
		NormCoord nc;
		nc.x = pStartNorm->x+pEndNorm->x ;
		nc.y = pStartNorm->y+pEndNorm->y ;

		double len = nc.GetLength();
		if ( len==0.0 )
			return ;
		len = 1.0/len ;
		nc.x *= len;
		nc.y *= len;
		// Recurse downwards, add in the centre point and recurse rightwards
		CreateRoundJoin(pList, pCentre, pStartNorm, &nc,		   depth + 1);
		AddEdgeToList  (pList, pCentre,				&nc,		   0		);	
		CreateRoundJoin(pList, pCentre,				&nc, pEndNorm, depth + 1);
	}
}

/********************************************************************************************
>	void CCreateBevelTrapezoids::FlattenInnerPoints(TrapEdgeList * pList, INT32 indent)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/98
	Purpose:	Takes a trap edge list, and flattens all inner points so that they are 
				consistant with the distances of the outer points
	Inputs:		The trap edge list to flatten, and the indent we're using
	Returns:	-
	Coments:	All inner points must be defined with their normals
	SeeAlso:	

********************************************************************************************/

void CCreateBevelTrapezoids::FlattenInnerPoints(TrapEdgeList * pList, INT32 indent, BOOL bOuter)
{
	if (!pList || indent == 0)
		return;

	// find the first outer edge
	UINT32 StartIndex = 0;
	BOOL bFound = FALSE;

	TrapEdge * pEdge = NULL;
	TrapEdge * pNextEdge = NULL;
	TrapEdge * pNextEdge2 = NULL;

	for (UINT32 i = 0 ; i < pList->GetNumEdges()-1; i++)
	{
		pEdge = pList->GetTrapEdge(i);

		if (pEdge)
		{
			if (pEdge->Position == 0)
			{
				// ok, we've found an outer point
				StartIndex = i;
				i = pList->GetNumEdges();
				bFound = TRUE;
			}
		}
	}

	if (!bFound)
	{
		// no outer points found, so exit
		return;
	}

	DocCoord dc1;
	
	NormCoord nc1;
	NormCoord nc2;

	DocCoord intersection;
	double p = 0;
	double q = 0;
	double nc1RLen = 0;
	double len = 0;
	double OldLen = 0;
	double dot = 0;
	DocRect BevBounds;

	pEdge = pList->GetTrapEdge(0);
	CalcInnerPoint(pEdge, indent, &dc1, bOuter);
	BevBounds.lo.x = dc1.x;
	BevBounds.lo.y = dc1.y;
	BevBounds.hi.x = dc1.x + 1;
	BevBounds.hi.y = dc1.y + 1;

	// get the bounding rect for the traps list
	for (i = 1 ; i < pList->GetNumEdges(); i++)
	{
		pEdge = pList->GetTrapEdge(i);
		CalcInnerPoint(pEdge, indent, &dc1, bOuter);
		BevBounds.IncludePoint(dc1);
	}

	for (i = 0; i < pList->GetNumEdges()-1; i++)
	{
		// get the trap edge and the next trap edge
		pEdge     = pList->GetTrapEdge(AlterIndex(i, pList->GetNumEdges(), StartIndex    ));
		pNextEdge = pList->GetTrapEdge(AlterIndex(i, pList->GetNumEdges(), StartIndex + 1));
		if (pEdge && pNextEdge)
		{
			// indicates an inner point
			if (pNextEdge->Position == 1)
			{
				CalcInnerPoint(pEdge, indent, &dc1, bOuter);
				
				// calculate the vector between the two centres
				nc1.x = pNextEdge->Centre.x - pEdge->Centre.x;
				nc1.y = pNextEdge->Centre.y - pEdge->Centre.y;
				nc1RLen = 1/nc1.GetLength();
				
				// now, do the intersection
				if (nc1Len > 0)
				{
					nc1.x *= nc1Len;
					nc1.y *= nc1Len;

					pNextEdge->Normal.Normalise();

					// check for the dot product being zero
					if (CalculateIntersection(&dc1, &nc1,
						&(pNextEdge->Centre), &(pNextEdge->Normal),
						&intersection, &p, &q))
					{
						// check that the intersection point is inside the bounds of
						// the original list - if not it's probably a glitch
						if (BevBounds.lo.x <= intersection.x &&
							BevBounds.lo.y <= intersection.y &&
							BevBounds.hi.x >= intersection.x &&
							BevBounds.hi.y >= intersection.y)
						{		
							// alter the normal to the intersection point
							nc2.x = pNextEdge->Centre.x - intersection.x;
							nc2.y = pNextEdge->Centre.y - intersection.y;
							
							double R = 1/(double)indent;
							nc2.x *= R;
							nc2.y *= R;
							
							if (bOuter)
							{
								nc2.x = -nc2.x;
								nc2.y = -nc2.y;
							}
							
							pNextEdge->Normal = nc2;
						}
					}
				}
				
			}
		}
	}
}

/********************************************************************************************
>	void CCreateBevelTrapezoids::GetPathFromTraps(const TrapsList * pTraps, 
											  const MILLIPOINT Width, 
											  Path * pPath)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/98
	Purpose:	Builds a path from 
	Inputs:		The trap list to use, the width to generate (can be negative) and
				the path to deliver the result into (must have been initialised)
	Returns:	-
	Coments:	
	SeeAlso:	

********************************************************************************************/

void CCreateBevelTrapezoids::GetPathFromTraps(TrapsList * pTraps, const MILLIPOINT Width, 
											  Path * pPath)
{
	pPath->ClearPath();

	UINT32 i = 0;
	INT32 j = 0;
	
	// builds a path from all the given trapezoid list
	UINT32 NumTraps = pTraps->GetNumTraps();

	INT32 NumEdges = 0;

	TrapEdgeList* pEdgeList = NULL;
	TrapEdge*	  pEdge		= 0;

	double dX = 0;
	double dY = 0;

	DocCoord dc;
	DocCoord Start;

	// first, find out how many faces are required
	for (i = 0 ; i < NumTraps; i++)
	{
		pEdgeList = pTraps->GetTrapEdgeList(i);
		NumEdges = static_cast<INT32> ( pEdgeList->GetNumEdges() );

		pEdge = pEdgeList->GetTrapEdge(0);

		/*
		pPath->AddMoveTo(pEdge->Centre);

		for (j = 1; j < NumEdges; j++)
		{
			// do the start points first
			pEdge = pEdgeList->GetTrapEdge(j);

			pPath->AddLineTo(pEdge->Centre);
		}

		pEdge = pEdgeList->GetTrapEdge(0);

		pPath->AddLineTo(pEdge->Centre);
		*/

		// now, add in the outer path
		for (j = NumEdges - 1; j >= 0; j--)
		{
			pEdge = pEdgeList->GetTrapEdge(j);

			dX = pEdge->Centre.x;
			dX += pEdge->Normal.x * Width;
			dY = pEdge->Centre.y;
			dY += pEdge->Normal.y * Width;

			dc.x = (MILLIPOINT)dX;
			dc.y = (MILLIPOINT)dY;

			if (j == NumEdges - 1)
			{
				pPath->AddMoveTo(dc);
				Start = dc;
			}
			else
				pPath->AddLineTo(dc);
		}
	}
}

#endif
