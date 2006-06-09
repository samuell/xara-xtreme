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
// ppbevel - Definition of bevel BevelPathProcessor class

#include "camtypes.h"

#include "ppbevel.h"

//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "pathtrap.h"
#include "bevtrap.h"
//#include "view.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "gclips.h"
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "attrbev.h"

CC_IMPLEMENT_DYNAMIC(BevelPathProcessor, PathProcessor);
CC_IMPLEMENT_DYNAMIC(BevelAttributePathProcessor, PathProcessor);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/////////////////////////////////////////////////////////////////////
// Clipping & flattening consts
const MILLIPOINT BevelPathProcessor_Flatness = 200;
const MILLIPOINT BevelPathProcessor_Tolerance = 50;

#define SHRINKSCALE 0.5

/********************************************************************************************

>	BevelPathProcessor::BevelPathProcessor()


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	12/11/98

	Purpose:	Constructor

********************************************************************************************/

INT32 num = 0;

BevelPathProcessor::BevelPathProcessor(BOOL bShrinkPath)
{
	m_bShrinkPath = bShrinkPath;
	m_bIsPathClosed = TRUE;
}

/********************************************************************************************

>	virtual BevelPathProcessor::~BevelPathProcessor()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	12/11/98

	Purpose:	Destructor
	
	Errors:		If this object still has a non-NULL Next pointer, it will ERROR3 in
				an effort to get you to clean up properly before destruction.

********************************************************************************************/

BevelPathProcessor::~BevelPathProcessor()
{
}

/********************************************************************************************

>	void BevelPathProcessor::ProcessPath(Path *pPath,
										 RenderRegion *pRender,
										 PathShape ShapePath = PATHSHAPE_PATH)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	12/11/98

	Purpose:	Adds the given path to the path in me for a bevel node to use

********************************************************************************************/
void BevelPathProcessor::ProcessPath(Path *pPath,
									 RenderRegion *pRender,
									 PathShape ShapePath)
{
	PORTNOTETRACE("other","BevelPathProcessor::PreOpProcessing - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	pRender->SaveContext();

	BOOL bPathIsClosed = pPath->IsClosed();

	if (m_bIsPathClosed && !bPathIsClosed)
	{
		m_bIsPathClosed = FALSE;
	}

	Path ShrunkPath;
	ShrunkPath.Initialise();

	// get the line width from the render region
	StrokeColourAttribute * pStrokeColour = (StrokeColourAttribute *)pRender->GetCurrentAttribute(ATTR_STROKECOLOUR);

	BOOL bShrinkPath = m_bShrinkPath;
	
	if (pStrokeColour)
	{
		if (pStrokeColour->GetStartColour())
		{
			if (!(pStrokeColour->GetStartColour()->IsTransparent()))
			{
				bShrinkPath = FALSE;
			}
		}
	}

	if (bShrinkPath && bPathIsClosed)
	{
		// flatten the path first
		ProcessFlatten Flatten(200);

		Path FlatPath;
		FlatPath.Initialise();

		ProcessFlags pf(TRUE, FALSE, FALSE);
		Flatten.FlattenPath(pf, pPath, &FlatPath);

		// ensure that the path is correct
		Path ClippedPath;
		ClippedPath.Initialise();

		Path CopyPath;
		CopyPath.Initialise();
		CopyPath.CloneFrom(FlatPath);

		FlatPath.ClipPathToPath(CopyPath, &ClippedPath, 3, 50, 200, 200);

		BevelHelpers::ShrinkPath(&ClippedPath, pRender->GetScaledPixelWidth(),
			pRender->GetScaledPixelWidth(), &ShrunkPath);

		pRender->DrawPath(&ShrunkPath, this, ShapePath);
	}
	else
	{
		pRender->DrawPath(pPath, this, ShapePath);
	}

	pRender->RestoreContext();
#endif
}

/********************************************************************************************

>	static void BevelPathProcessor::ShrinkPath(Path * pSrcPath, 
									double amountX,
									double amountY, 
									Path * pRetnPath)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/98
	Purpose:	Shrinks the path very slightly so that antialiasing doesn't bleed out
				of the bevel
	SeeAlso:	-

********************************************************************************************/
void BevelHelpers::ShrinkPath(Path * pSrcPath, double amountX, double amountY, 
									Path * pRetnPath, BOOL bTest)
{
	if (!pSrcPath || !pRetnPath)
		return;

	if (pSrcPath->GetNumCoords() < 3)
	{
		pRetnPath->CloneFrom(*pSrcPath);
		return;
	}
	
	if (amountX == 0 || amountY == 0)
		return;

	pRetnPath->ClearPath();
	pRetnPath->CloneFrom(*pSrcPath);

	DocCoord dcBefore;
	DocCoord dcThis;
	DocCoord dcAfter;
	DocCoord dcTest;

	DocRect testRect;
	DocRect thisRect;

	NormCoord ncBefore;
	NormCoord ncAfter;
	NormCoord ncAverage;

	double dt = 0;
//	double dot = 0;

	INT32 IStart = 0;
	INT32 IEnd = 0;

	double BeforeLen = 0;
	double AfterLen  = 0;
	double LenAverage = 0;

	BOOL bOK = FALSE;

	// first, calculate the normal list
	NormCoord *pNormList = new NormCoord[pSrcPath->GetNumCoords()];
	
	for (INT32 i = 0 ; i < pSrcPath->GetNumCoords(); i++)
	{
		// get the points before & after
		IStart = i - 1;
		IEnd = i + 1;
		
		if (i == pSrcPath->GetNumCoords() - 1)
		{
			IEnd = i;
			pSrcPath->FindStartOfSubPath(&IEnd);
			IEnd ++;
		}
		else if (pSrcPath->GetVerbArray()[i] == PT_MOVETO)
		{
			// find the end of the sub-path
			IStart = i;
			pSrcPath->FindEndElOfSubPath(&IStart);
			IStart -= 1;
		}
		else if (pSrcPath->GetVerbArray()[i+1] == PT_MOVETO)
		{
			IEnd = i;
			pSrcPath->FindStartOfSubPath(&IEnd);
			IEnd ++;
		}
		
		dcBefore = pSrcPath->GetCoordArray()[IStart];
		dcThis = pSrcPath->GetCoordArray()[i];
		dcAfter = pSrcPath->GetCoordArray()[IEnd];

		ncBefore.x = dcThis.x - dcBefore.x;
		ncBefore.y = dcThis.y - dcBefore.y;
		BeforeLen = 1/ncBefore.GetLength();

		if (BeforeLen > 0)
		{
			ncBefore.x *= BeforeLen;
			ncBefore.y *= BeforeLen;
		}

		ncAfter.x = dcAfter.x - dcThis.x;
		ncAfter.y = dcAfter.y - dcThis.y;
		AfterLen = 1/ncAfter.GetLength();

		if (AfterLen > 0)
		{
			ncAfter.x *= AfterLen;
			ncAfter.y *= AfterLen;
		}
		
		ncAverage.x = ncBefore.x + ncAfter.x;
		ncAverage.y = ncBefore.y + ncAfter.y;
		
		LenAverage = 1/ncAverage.GetLength();

		// do the normal
		if (LenAverage > 0)
		{
			ncAverage.x *= LenAverage;
			ncAverage.y *= LenAverage;
		
			// rotate by 90 degrees
			dt = ncAverage.x;
			ncAverage.x = -ncAverage.y;
			ncAverage.y = dt;

			pNormList[i] = ncAverage;
		}
	}					   

	INT32 AbsAmountX = abs((INT32)(amountX));
	INT32 AbsAmountY = abs((INT32)(amountY));
	INT32 NextIndex = 0;

	DocCoord intersection;
	double p = 0;
	double q = 0;
	
	BOOL bDoTest = FALSE;

	// now, run through the list again calculating bounding rects
	for (INT32 i = 0 ; i < pSrcPath->GetNumCoords(); i++)
	{
		dcThis = pSrcPath->GetCoordArray()[i];
		
		// only do this on single sub-paths (quicker !)
		/*
		if (pSrcPath->GetVerbArray()[i] == PT_MOVETO)
		{
			IStart = i;
			IEnd = i;
			pSrcPath->FindEndElOfSubPath(&IEnd);
		}
		*/

		IStart = 0;
		IEnd = pSrcPath->GetNumCoords();
		
		// is sub path in list ?
		bOK = TRUE;

		IStart = 0;
		IEnd = pSrcPath->GetNumCoords();

		thisRect.lo.x = dcThis.x - AbsAmountX;
		thisRect.lo.y = dcThis.y - AbsAmountY;
		thisRect.hi.x = dcThis.x + AbsAmountX;
		thisRect.hi.y = dcThis.y + AbsAmountY;
		
		for (INT32 j = IStart; j <= IEnd && bOK; j++)
		{
			// get the line segment to test			
			dcTest = pSrcPath->GetCoordArray()[j];
			
			bDoTest = TRUE;

			if (i == IStart)
			{
				if (j == IEnd || j == i || j == i+1)
				{
					bDoTest = FALSE;
				}
			}
			else if (i == IEnd)
			{
				if (j == IEnd - 1 || j == i || j == IStart)
				{
					bDoTest = FALSE;
				}
			}
			else if (j == i - 1 || j == i || j == i+1)
			{
				bDoTest = FALSE;
			}
			
			if (bDoTest)
			{
				NextIndex = j + 1;
				
				if (NextIndex == IEnd)
				{
					NextIndex = IStart;
				}
				
				dcAfter = pSrcPath->GetCoordArray()[NextIndex];
				
				// do the bounding rect
				if (dcTest.x < dcAfter.x)
				{
					testRect.lo.x = dcTest.x;
					testRect.hi.x = dcAfter.x;
				}
				else
				{
					testRect.lo.x = dcAfter.x;
					testRect.hi.x = dcTest.x;
				}
				
				if (dcTest.y < dcAfter.y)
				{
					testRect.lo.y = dcTest.y;
					testRect.hi.y = dcAfter.y;
				}
				else
				{
					testRect.lo.y = dcAfter.y;
					testRect.hi.y = dcTest.y;
				}
				
				if (testRect.IsIntersectedWith(thisRect))
				{
					// check if when we move the point the point changes sides from
					// one side of the line segment to the other
					ncAverage.x = dcTest.x - dcAfter.x;
					ncAverage.y = dcTest.y - dcAfter.y;
					
					if (CCreateBevelTrapezoids::CalculateIntersection(&dcThis, &(pNormList[i]),
						&dcAfter, &ncAverage, 
						&intersection, &p, &q))
					{
						if (intersection.x > testRect.lo.x && intersection.x < testRect.hi.x &&
							intersection.y > testRect.lo.y && intersection.y < testRect.hi.y)
						{
							// get the length of the line
							ncAverage.x = dcThis.x - intersection.x;
							ncAverage.y = dcThis.y - intersection.y;

							q = ncAverage.GetLength();

							if (q > 0)
							{
								if (q < fabs(amountX*2))
								{
									bOK = FALSE;
								}
							}

						}
					}
				}			
			}
		}

		if (bOK)
		{
			dcThis.x = (INT32)((((double)dcThis.x) - (pNormList[i].x * amountX)));
			dcThis.y = (INT32)((((double)dcThis.y) - (pNormList[i].y * amountY)));
					
			pRetnPath->GetCoordArray()[i] = dcThis;
		}		
	}

	delete [] pNormList;
	pNormList = NULL;

	/*
	Path StrokedPath;
	StrokedPath.Initialise();

	pSrcPath->StrokePathToPath((INT32)(amountX*2),
							  LineCapButt,
							  MitreJoin,
							  NULL,
							  &StrokedPath,
							  BEVEL_FLATNESS/5,
							  TRUE);
	
	// need to flatten this
	ProcessFlatten Flatten(BEVEL_FLATNESS/5);

	Path FlatPath;
	FlatPath.Initialise();

	pRetnPath->ClearPath(FALSE);
	pRetnPath->Initialise();
	
	ProcessFlags pf(TRUE, FALSE, FALSE);
	Flatten.FlattenPath(pf, &StrokedPath, &FlatPath);

	if (!bAdd)
	{
		FlatPath.ClipPathToPath(pSrcPath, 
			pRetnPath,
			1 | CLIPPING_SOURCE_WINDING,
			0, BEVEL_FLATNESS/5, BEVEL_FLATNESS/5);
	}
	else
	{
		FlatPath.ClipPathToPath(pSrcPath, 
			pRetnPath,
			7 | CLIPPING_SOURCE_WINDING,
			0, BEVEL_FLATNESS/5, BEVEL_FLATNESS/5);
	}
	*/


	pRetnPath->IsFilled = TRUE;
	pRetnPath->IsStroked = FALSE;

}

/********************************************************************************************

>	static BOOL BevelHelpers::IsPathToBeReversed(Path * pPath)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/98
	Purpose:	Indicates that the path is around the wrong way for bevelling i.e.
				the normals will be facing outwards, not inwards
	Notes:		The path must have no sub-paths
	SeeAlso:	-

********************************************************************************************/

BOOL BevelHelpers::IsPathToBeReversed(Path * pPath)
{
	NormCoord ncBefore;
	NormCoord ncAfter;
	NormCoord ncBefore90;

	double TotalDot = 0;
	double dot1 = 0;
	double dot2 = 0;

	DocRect Bounds = pPath->GetBoundingRect();
	DocCoord Centre ;
	Centre.x = Bounds.lo.x + (Bounds.Width() / 2);
	Centre.y = Bounds.lo.y + (Bounds.Height() / 2);

	UINT32 Count = 0;
	
	for (INT32 i = 0; i < pPath->GetNumCoords()-1; i++)
	{
		if (i > 0 && pPath->GetVerbArray()[i] == PT_MOVETO)
			break;

		Count ++;
		
		// get the points before & after
		ncBefore.x = pPath->GetCoordArray()[i].x - Centre.x;
		ncBefore.y = pPath->GetCoordArray()[i].y - Centre.y;
		
		ncAfter.x = pPath->GetCoordArray()[i+1].x - Centre.x;
		ncAfter.y = pPath->GetCoordArray()[i+1].y - Centre.y;

		ncBefore.Normalise();
		ncAfter.Normalise();

		ncBefore90.y = -ncBefore.x;
		ncBefore90.x =  ncBefore.y;

		dot1 = ncBefore90.x * ncAfter.x + ncBefore90.y * ncAfter.y;
		dot2 = ncBefore.x * ncAfter.x + ncBefore.y * ncAfter.y;

		TotalDot += dot1;
	}

	TotalDot /= (double)Count;

	TRACEUSER( "DavidM", _T("Total %f\n"), TotalDot);

	return (TotalDot < -0.05);
}

/********************************************************************************************

>	void BevelHelpers::EliminateMultiplePoints(Path * pPath)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/99
	Purpose:	Eliminates multiple points in the path
				Path must be flat !
	SeeAlso:	-

********************************************************************************************/
void BevelHelpers::EliminateMultiplePoints(Path * pPath)
{	
	Path NewPath;
	NewPath.Initialise();

	// flatten the path
	pPath->Flatten(200, &NewPath);
	pPath->CloneFrom(NewPath);
	pPath->IsFilled = TRUE;
	pPath->IsStroked = FALSE;

	NewPath.ClearPath();
	NewPath.IsFilled = TRUE;
	NewPath.IsStroked = FALSE;

	DocCoord dc1(0,0);
	DocCoord dc2;

	INT32 StartIndex = 0;

	for (INT32 i = 0 ; i < pPath->GetNumCoords(); i++)
	{
		dc2 = pPath->GetCoordArray()[i];

		// ensure that dc1 is different to dc2 on first index
		if (i == 0)
		{
			dc1.x = dc2.x - 1000;
		}
		
		if (dc2.x != dc1.x ||
			dc2.y != dc1.y ||
			pPath->GetVerbArray()[i] == PT_MOVETO)
		{
			if (pPath->GetVerbArray()[i] == PT_MOVETO)
			{
				/*
				if (i > 0)
				{
					// make sure we close the path
					if (NewPath.GetCoordArray()[i - 1].x != NewPath.GetCoordArray()[StartIndex].x ||
						NewPath.GetCoordArray()[i - 1].y != NewPath.GetCoordArray()[StartIndex].y)
					{
						NewPath.AddLineTo(NewPath.GetCoordArray()[StartIndex]);
					}
				}
				*/

				NewPath.AddMoveTo(dc2);
				StartIndex = i;
			}
			else
			{
				NewPath.AddLineTo(dc2);
			}
		}

		dc1 = dc2;
	}

	// make sure we close the path
	/*
	if (NewPath.GetCoordArray()[i - 1].x != NewPath.GetCoordArray()[StartIndex].x ||
		NewPath.GetCoordArray()[i - 1].y != NewPath.GetCoordArray()[StartIndex].y)
	{
		NewPath.AddLineTo(NewPath.GetCoordArray()[StartIndex]);
	}
	*/

	pPath->ClearPath();
	pPath->CloneFrom(NewPath);
}



////////////////////////////////////////////////////////////
// BevelAttributePathProcessor implementation
/********************************************************************************************

>	BevelAttributePathProcessor::BevelAttributePathProcessor()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	12/11/98

	Purpose:	Constructor

********************************************************************************************/
BevelAttributePathProcessor::BevelAttributePathProcessor()
{
	m_SummedPath.Initialise();
	m_StrokedPath.Initialise();
	m_Indent = 0;
}


/********************************************************************************************

>	BevelAttributePathProcessor::~BevelAttributePathProcessor()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	12/11/98

	Purpose:	Destructor

********************************************************************************************/
BevelAttributePathProcessor::~BevelAttributePathProcessor()
{

}


/********************************************************************************************

>	void BevelAttributePathProcessor::ProcessPath(Path *pPath,
												  RenderRegion *pRender,
												  PathShape ShapePath = PATHSHAPE_PATH)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	12/11/98

	Purpose:	Adds the given path to the path in me for a bevel node to use

********************************************************************************************/
void BevelAttributePathProcessor::ProcessPath(Path *pPath,
											  RenderRegion *pRender,
											  PathShape ShapePath)
{
//	PORTNOTETRACE("other","BevelAttributePathProcessor::PreOpProcessing - do nothing");
//#ifndef EXCLUDE_FROM_XARALX
	// pRender->DrawPath(pPath, this);

	// clip the path to itself
	Path BlankPath;
	BlankPath.Initialise();

	Path SafePath;
	SafePath.Initialise();

	pPath->ClipPathToPath(BlankPath, &SafePath, 6, BevelPathProcessor_Tolerance, 
		BevelPathProcessor_Flatness,
		BevelPathProcessor_Flatness);

	// now, sum the path
	m_SummedPath.MergeTwoPaths(SafePath);

	Path StrokedPath;
	StrokedPath.Initialise();

	SafePath.StrokePathToPath(abs(m_Indent)*2,
							LineCapButt,
							RoundJoin,
							NULL,
							&StrokedPath,
							BevelPathProcessor_Flatness,
							FALSE);

	// now, clip this path against the original (i.e. the passed in) path to
	// produce the final clipped path
	Path ClippedPath;
	ClippedPath.Initialise();

	// shrink the clipped path
	Path ShrunkPath;
	ShrunkPath.Initialise();

	if (m_Indent < 0)
	{
		pPath->ClipPathToPath(StrokedPath, &ClippedPath, 7 | CLIPPING_SOURCE_WINDING,
			BevelPathProcessor_Tolerance, BevelPathProcessor_Flatness,
			BevelPathProcessor_Flatness);

		BevelHelpers::ShrinkPath(&ClippedPath, pRender->GetScaledPixelWidth(), pRender->GetScaledPixelWidth(),
							&ShrunkPath, FALSE);

		ClippedPath.CloneFrom(ShrunkPath);

		// merge the original path (reversed) into this path to produce a hole for 
		// outer bevels
		Path ReversedPath;
		ReversedPath.Initialise();
		ReversedPath.CloneFrom(SafePath);
		ReversedPath.Reverse();
		ClippedPath.MergeTwoPaths(ReversedPath);
	}
	else
	{
		pPath->ClipPathToPath(StrokedPath, &ClippedPath, 2 | CLIPPING_SOURCE_WINDING,
			BevelPathProcessor_Tolerance, BevelPathProcessor_Flatness,
			BevelPathProcessor_Flatness);

		BevelHelpers::ShrinkPath(&ClippedPath, pRender->GetScaledPixelWidth(), pRender->GetScaledPixelWidth(),
							&ShrunkPath, FALSE);

		ClippedPath.CloneFrom(ShrunkPath);		
	}

	// merge this path with the stroked path member variable - as the attribute needs
	// it too
	m_StrokedPath.MergeTwoPaths(ClippedPath);

	// render the path with the same parameters
	ClippedPath.IsFilled = TRUE;
	ClippedPath.IsStroked = FALSE;

	pRender->SaveContext();
	pRender->SetLineColour(COLOUR_NONE);

	pRender->DrawPath(&ClippedPath, this, ShapePath);
	pRender->RestoreContext();
//#endif
}
