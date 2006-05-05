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
// Curve Fitting Functions

/*
*/


#include "camtypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "paths.h"
#include "fitcurve.h"
#include "errors.h"
#include "pathtrap.h"


// Set things up so that the tool will be listed in the Dialog box
DECLARE_SOURCE("$Revision$");


#define STEP_SIZE	6
#define ERROR_STEP	6

// Give my name in memory dumps
CC_IMPLEMENT_MEMDUMP(CurveFitObject, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(FitPoint, CC_CLASS_MEMDUMP)

#define new CAM_DEBUG_NEW


/********************************************************************************************

>	FitPoint FitPoint::operator - ()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/03/94
	Returns:	A FitPoint that is the negative of this
	Purpose:	Unary Minus for a FitPoint (Vector)

********************************************************************************************/

FitPoint FitPoint::operator - ()
{
	FitPoint Result;

	// negate the vector
	Result.x = -x;
	Result.y = -y;

	// and return it
	return Result;
}



/********************************************************************************************

>	FitPoint FitPoint::operator * (double Factor)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/03/94
	Inputs:		Factor - the amount to scale the vector by
	Returns:	FitPoint - this vector multiplied by the Factor
	Purpose:	FitPoint Multiply function. This will multiply the vector by a constant
				Factor. The result is returned

********************************************************************************************/

FitPoint FitPoint::operator * (double Factor)
{
	FitPoint Result;

	// Scale the vector by the factor
	Result.x = x*Factor;
	Result.y = y*Factor;

	// and return it
	return Result;
}


/********************************************************************************************

>	FitPoint FitPoint::SetLength( double NewLen )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/03/94
	Inputs:		NewLen - The length you want the vector to be
	Returns:	FitPoint - the new vector that points in the same direction as this vector,
				only of magnitude NewLen
	Purpose:	Scales the FitPoint vector to by the specified length

********************************************************************************************/

FitPoint FitPoint::SetLength( double NewLen )
{
	FitPoint Result(x, y);

	double Len = Length();
	if (Len != 0.0)
	{
		Len = NewLen/Len ;
		Result.x *= Len;
		Result.y *= Len;
	}

	return Result;
}


/********************************************************************************************

>	FitPoint operator + (const FitPoint& Point1, const FitPoint& Point2)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/03/94
	Inputs:		Point1 - The first FitPoint Vector
				Point2 - The Second FitPoint vector
	Returns:	FitPoint - the FitPoint vector that is a combination of Point1 and Point2
	Purpose:	Adds two FitPoint vectors together. This function is a Friend of the FitPoint
				class

********************************************************************************************/

FitPoint operator + (const FitPoint& Point1, const FitPoint& Point2)
{
	FitPoint Result;

	// Add the two vector together
	Result.x = Point1.x + Point2.x;
	Result.y = Point1.y + Point2.y;

	// return the result
	return Result;
}


/********************************************************************************************

>	FitPoint operator - (const FitPoint& Point1, const FitPoint& Point2)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/03/94
	Inputs:		Point1 - The first FitPoint Vector
				Point2 - The Second FitPoint vector
	Returns:	FitPoint - the FitPoint vector that is the first vector minus the second
				vector
	Purpose:	Subtracts the two FitPoint vectors. This function is a Friend of the FitPoint
				class

********************************************************************************************/

FitPoint operator - (const FitPoint& Point1, const FitPoint& Point2)
{
	FitPoint Result;

	// Subtract the two vector from each other
	Result.x = Point1.x - Point2.x;
	Result.y = Point1.y - Point2.y;

	// return the result
	return Result;
}



/********************************************************************************************

>	void FitPoint::Dump()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/03/94
	Purpose:	Dumps the values of the FitPoint class to the TRACE output

********************************************************************************************/

void FitPoint::Dump()
{
	TRACEALL( _T("FitPoint Object (%7.2f, %7.2f)\n"), x, y );
}












/********************************************************************************************

>	CurveFitObject::CurveFitObject( Path* ThePath, double MaxError )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Inputs:		ThePath - A Pointer to a path object that we are to simplify
				MaxError - The Square of the distance that we are prepared to let the fitted
				path stray from the original data
	Purpose:	Constructs a Curve Fitting Object and gives it some data that it will need
				during the fit
	SeeAlso:	CurveFitObject::Initialise

********************************************************************************************/

CurveFitObject::CurveFitObject( Path* ThePath, double MaxError )
{
	// Make a note of info we have got
	LongPath = ThePath;
	Error = MaxError;

	// And set defaults to the rest
	Distances = NULL;
	PathArray = NULL;
	LineArray = NULL;

	TotalStraightLines = 0;
	TotalCoords = 0;
}


/********************************************************************************************

>	CurveFitObject::~CurveFitObject()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Purpose:	Destroys the curve fitter when it is done, freeing any memory that was
				allocated in the Initialise() function
	SeeAlso:	CurveFitObject::Initialise

********************************************************************************************/

CurveFitObject::~CurveFitObject()
{
	if (Distances!=NULL)
		delete Distances;

	if (PathArray!=NULL)
		delete PathArray;

	if (LineArray!=NULL)
		delete LineArray;
}



/********************************************************************************************

>	BOOL CurveFitObject::Initialise(Path* CopyPath, INT32 NumPoints)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Inputs:		CopyPath - The path to copy the MoveTo Data from
				NumPoints - the number of points in the path object
	Returns:	TRUE if the CurveFitter was properly initialised or FALSE if it failed to
				get the memory that it needs
	Purpose:	Gets memory to store all the coords in and starts going through the path
				extracting the relavent points. It also calculates the distance along the path
				of each of the points it stores, which are needed extensivly when checking
				the accuracy of the fit. This function also empties the path object ready for
				the fitted version to be added.

********************************************************************************************/

BOOL CurveFitObject::Initialise(Path* CopyPath, INT32 NumPoints)
{
	// Here we must try and get some memory for the path array
	PathArray = new DocCoord[NumPoints];
	if (PathArray==NULL)
		return FALSE;

	// copy the data out of the path and into the array. Only copy points of interest
	CopyPath->FindStartOfPath();
	DocCoord* Coords = CopyPath->GetCoordArray();

	// Deal with the flags - We have to look for Spare1 being true, meaning that the previous
	// Lineto was meant to stay as a straight line
	PathFlags* Flags = CopyPath->GetFlagArray();
	TotalStraightLines = 0;
	for (INT32 i=0; i<NumPoints; i++)
	{
		if (Flags[i].Spare1==TRUE)
			TotalStraightLines++;
	}

	// Get some memory to store the positions of the line breaks in
	if (TotalStraightLines>0)
	{
		LineArray = new INT32[TotalStraightLines];
		if (LineArray==NULL)
		{
			delete PathArray;
			PathArray = NULL;
			return FALSE;
		}
	}

	// copy the MoveTo out of the path and into our array
	PathArray[0].x = Coords[0].x;
	PathArray[0].y = Coords[0].y;

	INT32 IncludePoint = 1;
	INT32 StraightLinePos = 0;
	for (INT32 i=1; i<NumPoints; i++)
	{
		// If this is one of the straight line bits, then make a note of its position
		if (Flags[i].Spare1==TRUE)
		{
			LineArray[StraightLinePos] = IncludePoint;
			StraightLinePos++;
		}

		// Check to see if this coordinate is really needed (last point is always needed)
		if ((Coords[i].x != PathArray[IncludePoint-1].x) || (Coords[i].y != PathArray[IncludePoint-1].y) &&
			(i!=NumPoints-1))
		{
			// This point is not the same as the one before, so add it into the array
			PathArray[IncludePoint].x = Coords[i].x;
			PathArray[IncludePoint].y = Coords[i].y;

			IncludePoint++;
		}
	}

	// Add the last point in the track data to the path array if it is not already there
	if ((PathArray[IncludePoint-1].x != Coords[NumPoints-1].x) ||
		(PathArray[IncludePoint-1].y != Coords[NumPoints-1].y))
	{
		PathArray[IncludePoint].x = Coords[NumPoints-1].x;
		PathArray[IncludePoint].y = Coords[NumPoints-1].y;
		IncludePoint++;
	}

	// increment the Include count (as we have just added a point to the array) and then
	// set the true number of points properly.
	NumPoints = IncludePoint;
	if (NumPoints<2)
	{
		delete PathArray;
		delete LineArray;
		PathArray = NULL;
		LineArray = NULL;
		return FALSE;
	}

	// Build an array of the distances along the path
	Distances = new INT32[NumPoints];
	if (Distances==NULL)
	{
		delete PathArray;
		delete LineArray;
		PathArray = NULL;
		LineArray = NULL;
		return FALSE;
	}

	Distances[0] = 0;
	INT32 dx, dy, min;
	for (INT32 i=1; i<NumPoints; i++)
	{
		// This is doing an approximation to a Square Root
		// It is about 250 times faster on a machine without FPU

		// find the difference between the last 2 points
		dx = abs(PathArray[i].x - PathArray[i-1].x);
		dy = abs(PathArray[i].y - PathArray[i-1].y);

		// Find out half the smallest of dx and dy
		if (dx>dy)
			min = dy>>1;
		else
			min = dx>>1;

		Distances[i] = Distances[i-1] + dx + dy - min;
	}

	// Now we can delete the Path Data in the path we are to put the smoothed path in
	LongPath->ClearPath();
	LongPath->FindStartOfPath();
	LongPath->InsertMoveTo(PathArray[0]);

	// Store the total number of coords in the array for future reference
	TotalCoords = NumPoints;

	return TRUE;
}


/********************************************************************************************

>	void CurveFitObject::FitCurve()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Purpose:	Actually fits the curve and places the results back in the path object
				declared in the constructor. This function walks through all the data points
				trying to find cusps in the path and fits curves between them by calling the
				FitCubic member of the CurveFitObject
	SeeAlso:	CurveFitObject::FitCubic

********************************************************************************************/

void CurveFitObject::FitCurve()
{
	FitPoint Tangent1, Tangent2;
	double Angle1, Angle2;
	INT32 Start = 0;
	INT32 StraightLinePos = 0;

	for (INT32 i=1; i<TotalCoords-1; i++)
	{
		if ((TotalStraightLines>0) && (i==LineArray[StraightLinePos]))
		{
			// if we have a section to the path that needs fitting before we do this then do it
			if (Start != (i-1))
			{
				// calculate the tangents off the end of the path
				Tangent1 = LeftTangent(Start);
				Tangent2 = RightTangent(i-1);

				// Fit a curve up to the straight line section
				FitCubic(Start, i-1, Tangent1, Tangent2);
			}
			
			// And draw in the straight line section
			InsertStraightLine(PathArray[i]);
			StraightLinePos++;
			Start = i;
		}
		else
		{
			// Go find the angle between a group of points
			Angle1 = atan2((double)PathArray[i].y-PathArray[i-1].y, (double)PathArray[i].x-PathArray[i-1].x);
			Angle2 = atan2((double)PathArray[i+1].y-PathArray[i].y, (double)PathArray[i+1].x-PathArray[i].x);
		
			// Get them in a sensible range
			if (Angle1 < -PI)	Angle1 += 2*PI;
			if (Angle1 > PI)	Angle1 -= 2*PI;
			if (Angle2 < -PI)	Angle2 += 2*PI;
			if (Angle2 > PI)	Angle2 -= 2*PI;
		
			// See if this point is a cusp in the curve
			if ((fabs(Angle2-Angle1) > (PI/2)) && (fabs(Angle2-Angle1) <= PI))
			{
				// calculate the tangents off the end of the path
				Tangent1 = LeftTangent(Start);
				Tangent2 = RightTangent(i);

			    // and do a load of maths that will hopefully fit a nice curve on it
				FitCubic(Start, i, Tangent1, Tangent2);
				Start = i;
			}
		}
	}


	INT32 End = TotalCoords-1;
	if ((TotalStraightLines>0) && (End==LineArray[StraightLinePos]))
	{
		// if we have a section to the path that needs fitting before we do this then do it
		if (Start != (End-1))
		{
			// calculate the tangents off the end of the path
			Tangent1 = LeftTangent(Start);
			Tangent2 = RightTangent(End-1);

			// Fit a curve up to the straight line section
			FitCubic(Start, End-1, Tangent1, Tangent2);
		}
		
		// And draw in the straight line section
		InsertStraightLine(PathArray[End]);
	}
	else
	{
		// Just have to fit a curve from the last cusp to the end of the path
		Tangent1 = LeftTangent(Start);
		Tangent2 = RightTangent(End);
    
	    // and do a load of maths that will hopefully fit a nice curve on it
		FitCubic(Start, End, Tangent1, Tangent2);
	}
}



/********************************************************************************************

>	void CurveFitObject::FitCubic( INT32 FirstPoint, INT32 LastPoint,
								   FitPoint Tangent1, FitPoint Tangent2,
								   BOOL IsStartCusp = TRUE, BOOL IsEndCusp = TRUE);

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Inputs:		FirstPoint - the index of the coordinate in the path to start fitting from
				LastPoint - the index of the coordinate in the path to stop fitting at
				Tangent1 - The tangent of the curve at the first point
				Tangent2 - the tangent of the curve at the last point
				IsStartCusp - TRUE if FirstPoint is next to a cusp
				IsEndCusp - TRUE of EndPoint is next to a cusp
	Purpose:	This function is recursive. It tries to fit a cubic function to the
				coordinates in the path between FirstPoint and LastPoint. It then compares
				this function with the actual coordinates to determine how good a fit it
				has found. If the fit is good it is added to the path object. If the fit
				is bad then it is split at the point where the fit is at its worst and
				FitCubic is called again for the left and right halves.
	Scope:		private
	SeeAlso:	CurveFitObject::GenerateBezier; CurveFitObject::CalcMaxError

********************************************************************************************/

void CurveFitObject::FitCubic(INT32 FirstPoint, INT32 LastPoint, FitPoint Tangent1, FitPoint Tangent2,
							  BOOL IsStartCusp, BOOL IsEndCusp)
{
	// Will need space for a Bezier curve
	FitPoint Bezier[4];
	INT32 NumPoints = LastPoint - FirstPoint + 1;

	// if this segment only has 2 points in it then do the special case
	if ( NumPoints == 2 )
	{
		InsertLine(PathArray[FirstPoint], PathArray[LastPoint], Tangent1, Tangent2, IsStartCusp, IsEndCusp);
		return;
	}
	
	// Due to a bug in the algorithm we also have to consider 3 points as a special case
	if ( NumPoints == 3 )
	{
		INT32 Distance = (Distances[LastPoint] - Distances[FirstPoint]) / 3;
		
		// store the end points
		Bezier[0] = PathArray[FirstPoint];
		Bezier[3] = PathArray[LastPoint];
		
		// calc the control points
		Bezier[1] = Bezier[0] + Tangent1.SetLength(Distance);
		Bezier[2] = Bezier[3] + Tangent2.SetLength(Distance);

		// add it to the path
		InsertBezier(Bezier, IsStartCusp, IsEndCusp);
		return;
	}

	// Create a Bezier curve from the segemnt and see if it is a good fit
	INT32 SplitPoint;
	GenerateBezier(FirstPoint, LastPoint, Tangent1, Tangent2, Bezier);
	double MaxError = CalcMaxError(FirstPoint, LastPoint, Bezier, &SplitPoint);
	
	if (MaxError < Error)
	{
		// The mapping was good, so output the curve segment
		InsertBezier(Bezier, IsStartCusp, IsEndCusp);
		return;
	}
	
	// fitting failed -- split at max error point and fit recursively
	FitPoint CentTangent = CentreTangent(SplitPoint);
	FitCubic(FirstPoint, SplitPoint, Tangent1, CentTangent, IsStartCusp, FALSE);

	CentTangent = -CentTangent;
	FitCubic(SplitPoint, LastPoint, CentTangent, Tangent2, FALSE, IsEndCusp);
}




/********************************************************************************************

>	void CurveFitObject::GenerateBezier(INT32 FirstPoint, INT32 LastPoint, FitPoint Tangent1, 
										FitPoint Tangent2, FitPoint* Bezier)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Inputs:		FirstPoint - the index of the coordinate in the path to start fitting from
				LastPoint - the index of the coordinate in the path to stop fitting at
				Tangent1 - The tangent of the curve at the first point
				Tangent2 - the tangent of the curve at the last point
	Outputs:	Bezier - A pointer to a Bezier Curve
	Purpose:	This function supplies the maths to try and fit a cubic function to a 
				set of points. It spends its time trying to come up with good lengths for
				the two tangents passed in to maximise the closeness of the fit. If it
				fails to come up with realistic results it simply sets the tangent lengths
				to be 1/3 of the distance between the start point and the end point.
	Scope:		private

********************************************************************************************/

void CurveFitObject::GenerateBezier(INT32 FirstPoint, INT32 LastPoint, FitPoint Tangent1, 
									FitPoint Tangent2, FitPoint* Bezier)
{
	INT32 NumPoints = LastPoint - FirstPoint + 1;
	
	// Build a matrix type of thing that contains the tangents scaled by the offsets
	FitPoint A[STEP_SIZE+1][2];			//	Vector2 (*A)[2] = new Vector2[NumPoints+1][2];
	double   Offsets[STEP_SIZE+1];

	INT32 step = (NumPoints+STEP_SIZE) / STEP_SIZE;
	INT32 i, pos = 0;

	// Chord length parameterisation
	const INT32 DistToEnd = Distances[LastPoint] - Distances[FirstPoint];
	for (i=FirstPoint; i<LastPoint+1; i+=step)
	{
		Offsets[pos] = Distances[i] - Distances[FirstPoint];
		Offsets[pos] /= DistToEnd;

		// Fill the matrix A
		A[pos][0] = Tangent1.SetLength( Bezier1(Offsets[pos]) );
		A[pos][1] = Tangent2.SetLength( Bezier2(Offsets[pos]) );

		// Move to the next element in the path
		pos++;
	}

	// For a detailed description of the maths used here, please see Graphics Gems I
	// I have made some changes to the basic algorithm used there - the main one is
	// that this block of maths is only performed on a small selection of the points
	// in the data set, where-as the one in the book uses all the points
	double  C[2][2];
	double  X[2];
	
	C[0][0] = 0.0;
	C[0][1] = 0.0;
	C[1][0] = 0.0;
	C[1][1] = 0.0;
	X[0]    = 0.0;
	X[1]    = 0.0;
	
	FitPoint FirstCoord = PathArray[FirstPoint];
	FitPoint LastCoord  = PathArray[LastPoint];
	FitPoint ThisCoord, Combo;

	pos = 0;
	for (i=0; i<NumPoints; i+=step)
	{
		C[0][0] += A[pos][0].SquaredLength();
		C[0][1] += A[pos][0].Dot(A[pos][1]);
		// Point C[1][0] is the same as C[0][1] and is set outside the loop below
		C[1][1] += A[pos][1].SquaredLength();
		
		// Go ahead and build a vector based on the bezier functions
		ThisCoord = PathArray[FirstPoint+i];
		Combo = ThisCoord - ((FirstCoord * Bezier0(Offsets[pos]))
							+ (FirstCoord * Bezier1(Offsets[pos]))
							+ (LastCoord  * Bezier2(Offsets[pos]))
							+ (LastCoord  * Bezier3(Offsets[pos])));

		// Combine it with the other points
		X[0] += A[pos][0].Dot( Combo );
		X[1] += A[pos][1].Dot( Combo );

		pos++;
	}

	// This point in the matrix is the same, so we do not need to do it in the loop
	C[1][0] = C[0][1];
	
	// calc the determinants of C and X
	double det_C0_C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1];
	double det_C0_X  = C[0][0] * X[1]    - C[0][1] * X[0];
	double det_X_C1  = X[0]    * C[1][1] - X[1]    * C[0][1];
	
	// finally, derive the length of the ideal tangents
	if (det_C0_C1 == 0.0)
		det_C0_C1 = (C[0][0] * C[1][1]) * 10e-12;	// oh err, whats it up to here then!
	
	double AlphaLeft  = det_X_C1 / det_C0_C1;
	double AlphaRight = det_C0_X / det_C0_C1;
	
	Bezier[0] = PathArray[FirstPoint];
	Bezier[3] = PathArray[LastPoint];

	// if alpha negative, the set the tangent length to a third of the distance between
	// the start and the end points of the curve segment	
	if ( AlphaLeft < 0.0 || AlphaRight < 0.0)
	{
		INT32 Distance = (Distances[LastPoint] - Distances[FirstPoint]) / 3;
		
		Bezier[1] = Bezier[0] + Tangent1.SetLength(Distance);
		Bezier[2] = Bezier[3] + Tangent2.SetLength(Distance);
	}
	else
	{	
		Bezier[1] = Bezier[0] + Tangent1.SetLength(AlphaLeft);
		Bezier[2] = Bezier[3] + Tangent2.SetLength(AlphaRight);
	}
}


/********************************************************************************************

>	FitPoint CurveFitObject::BezierPoint( FitPoint* Bez, double u)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Inputs:		Bez - The control points of a bezier curve
				u - the normalised distance along the bezier that we are interested in
	Returns:	The coord of the point that is a distance u into the bezier. for example,
				if u = 0.5 then the coord of the point half way along the bezier will be
				returned
	Purpose:	This function simply evaluates the bezier function for a given position and
				is used to help when determining how good a fit we have obtained
	Scope:		private
	SeeAlso:	CurveFitObject::CalcMaxError

********************************************************************************************/

FitPoint CurveFitObject::BezierPoint( FitPoint* Bez, double u)
{
	double OneMinus = 1.0-u;
	double uSquared = u*u;
	double OneMinusSquared = OneMinus*OneMinus;

	FitPoint Coord;
	Coord = Bez[0]*(OneMinusSquared*OneMinus);
	Coord = Coord + Bez[1]*(3.0*u*OneMinusSquared);
	Coord = Coord + Bez[2]*(3.0*uSquared*OneMinus);
	Coord = Coord + Bez[3]*(uSquared*u);

	return Coord;
}



/********************************************************************************************

>	double CurveFitObject::CalcMaxError(INT32 FirstPoint, INT32 LastPoint,
										FitPoint* Bezier, INT32* SplitPoint)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Inputs:		FirstPoint, LastPoint - The index at the start and end of the curve section
				we have just fit
				Bezier - the control points of the bezier we have fitted to the points
	Outputs:	SplitPoint - the point to split the curve at if the error is too great
	Returns:	The maximum distance between the bezier curve and the original data points
	Purpose:	Finds out how good a fit the bezier curve we have created is when compared
				with the data points
	Scope:		private

********************************************************************************************/

double CurveFitObject::CalcMaxError(INT32 FirstPoint, INT32 LastPoint, FitPoint* Bezier, INT32* SplitPoint)
{
	double		Distance;
	double		MaxDist = 0.0;
	double 		RTotalLength = 1.0/(Distances[LastPoint] - Distances[FirstPoint]);
	FitPoint	Point;

	// Start out by putting the split point in the middle of the curve segment
	INT32 NumPoints = LastPoint - FirstPoint + 1;
	*SplitPoint = NumPoints / 2;
	INT32 step = (NumPoints+ERROR_STEP) / ERROR_STEP;
	
	// Loop though the points, visiting a fixed number of them
	for (INT32 i=FirstPoint+1; i<LastPoint; i+=step)
	{
		// Calculate the offset at this point
		double Offset = Distances[i] - Distances[FirstPoint];
		Offset *= RTotalLength;

		// Calculate where the curve actually is and find the distance from where we want it
		FitPoint Coord = PathArray[i];
		Point = BezierPoint(Bezier, Offset);
		Distance = (Point - Coord).SquaredLength();
		if ( Distance >= MaxDist)
		{
			MaxDist = Distance;
			*SplitPoint = i;
		}
	}
	
	return MaxDist;
}


/********************************************************************************************

>	FitPoint CurveFitObject::LeftTangent(INT32 Start)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Inputs:		Start - the index of the point at the start of the segment to fit
	Returns:	The tangent at the point Start
	Purpose:	Finds the Left tangent at the given point in the path
	Scope:		private

********************************************************************************************/

FitPoint CurveFitObject::LeftTangent(INT32 Start)
{
	FitPoint Tangent;

	// check for empty point array
	if (TotalCoords == 0)
	{
		Tangent.x = 1;
		Tangent.y = 0;

		return Tangent;
	}
	
	// check for start outside the array
	if ((Start >= TotalCoords) || (Start < 0))
		Start = TotalCoords / 2;

	// Find out which point to look to
	INT32 Forward = Start+2;
	if (Forward > TotalCoords-1)
		Forward = TotalCoords-1;

	// Calc the tangent from the left of the curve segment
	Tangent.x = PathArray[Forward].x - PathArray[Start].x;
	Tangent.y = PathArray[Forward].y - PathArray[Start].y;

	// Make sure that is not of zero length
	if ((Tangent.x==0) && (Tangent.y==0))
	{
		TRACEALL( _T("Tangent was a zero length vector in the curve fitter (left)\n"));
		Tangent.x = 1;
	}
	
	return Tangent;
}


/********************************************************************************************

>	FitPoint CurveFitObject::RightTangent(INT32 End)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Inputs:		End - the index of the point at the end of the segment to fit
	Returns:	The tangent at the point End
	Purpose:	Finds the Right tangent at the given point in the path
	Scope:		private

********************************************************************************************/

FitPoint CurveFitObject::RightTangent(INT32 End)
{
	FitPoint Tangent;

	// Find out which point to look to
	INT32 Backward = End-2;
	if (Backward<0)
		Backward = 0;

	Tangent.x = PathArray[Backward].x - PathArray[End].x;
	Tangent.y = PathArray[Backward].y - PathArray[End].y;

	// Make sure that is not of zero length
	if ((Tangent.x==0) && (Tangent.y==0))
	{
		TRACEALL( _T("Tangent was a zero length vector in the curve fitter (Right)\n"));
		Tangent.x = -1;
	}

	return Tangent;
}



/********************************************************************************************

>	FitPoint CurveFitObject::CentreTangent(INT32 Centre)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Inputs:		Centre - the index of the split point in the path
	Returns:	The tangent at the point Centre
	Purpose:	Finds the tangent at the centre of the path
	Scope:		private

********************************************************************************************/

FitPoint CurveFitObject::CentreTangent(INT32 Centre)
{
	DocCoord Left, Right;
	FitPoint CentreTang;

	// check for empty point array
	if (TotalCoords == 0)
	{
		CentreTang.x = 1;
		CentreTang.y = 0;

		return CentreTang;
	}

	// check for centre outside the array
	if ((Centre >= TotalCoords) || (Centre < 0))
		Centre = TotalCoords / 2;

	// Find out which point to look to
	INT32 Forward = Centre+2;
	if (Forward > TotalCoords-1)
		Forward = TotalCoords-1;

	// Find out which point to look to
	INT32 Backward = Centre-2;
	if (Backward < 0)
		Backward = 0;

	// Calc right tangent
	Left.x = PathArray[Backward].x - PathArray[Centre].x;
	Left.y = PathArray[Backward].y - PathArray[Centre].y;

	// Calc left tangent
	Right.x = PathArray[Centre].x - PathArray[Forward].x;
	Right.y = PathArray[Centre].y - PathArray[Forward].y;

	// Average to get the centre tangent
	CentreTang.x = (Left.x + Right.x) / 2.0;
	CentreTang.y = (Left.y + Right.y) / 2.0;

	// Make sure that is not of zero length
	if ((CentreTang.x==0) && (CentreTang.y==0))
	{
		TRACEALL( _T("Tangent was a zero length vector in the curve fitter (Cent)\n"));
		CentreTang.x = 1;
	}

	// return it
	return CentreTang;
}



/********************************************************************************************

>	void CurveFitObject::InsertBezier(FitPoint* Bezier, BOOL IsStartCusp, BOOL IsEndCusp)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Inputs:		Bezier - the control points of the bezier curve to add to the path
				IsStartCusp - TRUE if the start of this bezier is at a cusp
				IsEndCusp - TRUE if the End of this bezier is at a cusp
	Purpose:	Adds the bezier curve to the path. If it is that last curve in the fitting
				(ie Depth is 0) then the rotate flag will not be set
	Scope:		private

********************************************************************************************/

void CurveFitObject::InsertBezier(FitPoint* Bezier, BOOL IsStartCusp, BOOL IsEndCusp)
{
	// Prepare some flags
	PathFlags Flags;
	Flags.IsSelected = FALSE;
	Flags.IsSmooth = FALSE;
	Flags.IsRotate = TRUE;

	// Add this Bezier curve into the path
	LongPath->InsertCurveTo( DocCoord( (INT32)Bezier[1].x, (INT32)Bezier[1].y),
							 DocCoord( (INT32)Bezier[2].x, (INT32)Bezier[2].y),
							 DocCoord( (INT32)Bezier[3].x, (INT32)Bezier[3].y), &Flags);

	// Deal with cusps
	if (IsStartCusp || IsEndCusp)
	{
		// Go find out about the path
		PathFlags* AllFlags = LongPath->GetFlagArray();
		INT32 NumCoords = LongPath->GetNumCoords();

		if (IsStartCusp)
		{
			// Patch up the flags of the bits near that start
			AllFlags[NumCoords-3].IsRotate = FALSE;
		}
	
		if (IsEndCusp)
		{
			// Patch up the flags of the bits near that end
			AllFlags[NumCoords-2].IsRotate = FALSE;
			AllFlags[NumCoords-1].IsRotate = FALSE;
		}
	}
}



/********************************************************************************************

>	void CurveFitObject::InsertLine(const DocCoord& Start, const DocCoord& End, 
									FitPoint Tangent1, FitPoint Tangent2, BOOL IsStartCusp, BOOL IsEndCusp)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Inputs:		Start - the coord of the start point of the line
				End - the coord of the end point of the line
				Tangent1, Tangent2 - The tangents to the curve at the start and end points
				IsStartCusp - TRUE if the start of this bezier is at a cusp
				IsEndCusp - TRUE if the End of this bezier is at a cusp
	Purpose:	Inserts a straight curve into the path. It keeps it as a curve so that the
				path will continue to look smooth after it is edited. If this is the last
				Path element in the fit, (ie Depth is zero) then the rotate flag will not
				be set.
	Scope:		private

********************************************************************************************/

void CurveFitObject::InsertLine(const DocCoord& Start, const DocCoord& End,
								FitPoint Tangent1, FitPoint Tangent2, BOOL IsStartCusp, BOOL IsEndCusp)
{
	// Prepare some flags
	PathFlags Flags;
	Flags.IsSelected = FALSE;
	Flags.IsSmooth = FALSE;
	Flags.IsRotate = TRUE;

	// Find out a third of the distance between the two points
	FitPoint StartPos(Start);
	FitPoint EndPos(End);
	FitPoint DistanceVect = EndPos - StartPos;
	INT32 Length = (INT32)DistanceVect.Length() / 3;

	// Make the tangents the right length
	Tangent1 = Tangent1.SetLength(Length);
	Tangent2 = Tangent2.SetLength(Length);

	// Work out the position of the control points
    StartPos = StartPos + Tangent1;
	EndPos = EndPos + Tangent2;

	// Add the line to the curve
	LongPath->InsertCurveTo( DocCoord( (INT32)StartPos.x, (INT32)StartPos.y ), 
							 DocCoord( (INT32)EndPos.x, (INT32)EndPos.y ),
							 End, &Flags);	

	// Deal with cusps
	if (IsStartCusp || IsEndCusp)
	{
		// Go find out about the path
		PathFlags* AllFlags = LongPath->GetFlagArray();
		INT32 NumCoords = LongPath->GetNumCoords();

		if (IsStartCusp)
		{
			// Patch up the flags of the bits near that start
			AllFlags[NumCoords-3].IsRotate = FALSE;
		}
	
		if (IsEndCusp)
		{
			// Patch up the flags of the bits near that end
			AllFlags[NumCoords-2].IsRotate = FALSE;
			AllFlags[NumCoords-1].IsRotate = FALSE;
		}
	}
}




/********************************************************************************************

>	void CurveFitObject::InsertStraightLine(const DocCoord& End)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/94
	Inputs:		End - the coord of the end point of the line
	Purpose:	Inserts a straight Line into the path. This is used in cases where the
				Line segment must be kept as a straight line (ie by using Straight Line
				Mode of the FreeHand tool)
	Scope:		private

********************************************************************************************/

void CurveFitObject::InsertStraightLine(const DocCoord& End)
{
	// Prepare some flags
	PathFlags Flags;
	Flags.IsSelected = FALSE;
	Flags.IsRotate = FALSE;
	Flags.IsSmooth = FALSE;

	// Insert the line
	LongPath->InsertLineTo(End, &Flags);	
}

//////////////////////////////////////////////////////////////////////////////////////
// FitCurveNoChangeGeometry implementation
/********************************************************************************************

>	void FitCurveNoChangeGeometry::SmoothPath(Path * pPath, double Error)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/2000
	Inputs:		pPath	-	The path to smooth
				Error	-	The error to pass into Path::Smooth()
	Purpose:	Smooths the path, eliminating all unnecessary points
	Notes :		For example of use, see NodeCntr.cpp, GenerateContour() function

********************************************************************************************/
void FitCurveNoChangeGeometry::SmoothPath(Path * pPath, double Error)
{
	Path SubPath;
	SubPath.Initialise();

	Path QuantPath;
	QuantPath.Initialise();

	Path RetnPath;
	RetnPath.Initialise();
	
	// split the path into sub-paths
	INT32 NumSubPaths = pPath->GetNumSubpaths();
	for (INT32 i = 0; i < NumSubPaths ; i++)
	{
		// seperate out all sub-paths and smooth them individually
		SubPath.ClearPath();
		pPath->MakePathFromSubPath(i, &SubPath);

		// quantise this path
		QuantPath.ClearPath();
		SubPath.Quantise(20, &QuantPath);

		SmoothPathNoChangeGeometry(&QuantPath, Error);
		EliminateColinearPointsFromPath(&QuantPath);
		
		RetnPath.MergeTwoPaths(QuantPath);
	}

	pPath->ClearPath();
	pPath->CloneFrom(RetnPath);
}

/********************************************************************************************

>	void FitCurveNoChangeGeometry::SmoothPathNoChangeGeometry(Path * pPath, double Error)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/2000
	Inputs:		pPath	-	The path to smooth (must have NO sub-paths)
				Error	-	The error to pass into Path::Smooth()
	Purpose:	Smooths the path, eliminating all unnecessary points but maintaining
				geometry as much as possible - doesn't eliminate colinear points
	Notes:		Assumes the path is flattened

********************************************************************************************/
void FitCurveNoChangeGeometry::SmoothPathNoChangeGeometry(Path * pPath, double Error)
{
	INT32 StartIndex = 0;
	INT32 EndIndex = 0;

	NormCoord Vec1;
	NormCoord Vec2;

	double dot = 0;

	// threshold for 30 degrees for the dot product
	const double Thres = cos(30.0 * 3.142 / 180.0);
	
	for (INT32 i = 0 ; i < pPath->GetNumCoords(); i++)
	{
		if (i < pPath->GetNumCoords() - 2)
		{
			Vec1.x = pPath->GetCoordArray()[i+1].x - pPath->GetCoordArray()[i].x;
			Vec1.y = pPath->GetCoordArray()[i+1].y - pPath->GetCoordArray()[i].y;

			Vec2.x = pPath->GetCoordArray()[i+2].x - pPath->GetCoordArray()[i+1].x;
			Vec2.y = pPath->GetCoordArray()[i+2].y - pPath->GetCoordArray()[i+1].y;

			Vec1.Normalise();
			Vec2.Normalise();

			dot = (Vec1.x * Vec2.x) + (Vec1.y * Vec2.y);

			if (dot < Thres)
			{
				// don't smooth this !
			}
			else
			{
				StartIndex = i;
				
				// find the region to smooth
				while (dot > Thres && i < (pPath->GetNumCoords() - 2))
				{
					i++;
					Vec1 = Vec2;
					Vec2.x = pPath->GetCoordArray()[i+2].x - pPath->GetCoordArray()[i+1].x;
					Vec2.y = pPath->GetCoordArray()[i+2].y - pPath->GetCoordArray()[i+1].y;
					Vec2.Normalise();

					dot = (Vec1.x * Vec2.x) + (Vec1.y * Vec2.y);
				}
				 
				i--;

				EndIndex = i+1;

				// ok, we've found the region to smooth so do it !
				if (EndIndex > StartIndex && EndIndex < pPath->GetNumCoords())
				{
					pPath->SmoothSection(StartIndex, &EndIndex, Error, 0);
				}

				i = EndIndex+1;
			}
		}
	}
}

/********************************************************************************************

>	void FitCurveNoChangeGeometry::EliminateColinearPointsFromPath(Path * pPath)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/2000
	Inputs:		pPath	-	The path to smooth (must have NO sub-paths)
	Purpose:	Eliminates
	Notes:		Do after SmoothPathNoChangeGeometry

********************************************************************************************/
void FitCurveNoChangeGeometry::EliminateColinearPointsFromPath(Path * pPath)
{
	// make sure the flags are initialised on the path
	pPath->InitialiseFlags();
	
	NormCoord Vec1, Vec2;
	double dot = 0;
//	double OriginalDot = 0;

	BOOL bClose = FALSE;

	if ((pPath->GetVerbArray()[pPath->GetNumCoords() - 1] & PT_CLOSEFIGURE) != 0)
	{
		pPath->GetVerbArray()[pPath->GetNumCoords() - 1] -= PT_CLOSEFIGURE;
		bClose = TRUE;
	}

	INT32 StartIndex = 0;
	INT32 MidIndex = 0;
	INT32 EndIndex = 0;

	// threshold is one degree
	const double Thres = cos (1.0 * 3.142 / 180.0);

	BOOL bDelete = FALSE;

	for (INT32 i = 0 ; i < pPath->GetNumCoords() - 2; 
				i ++)
	{
		// set up the start, mid & end points of the section to test
		StartIndex = -1;
		MidIndex = -1;
		EndIndex = -1;

		if (pPath->GetVerbArray()[i] == PT_MOVETO ||
			pPath->GetVerbArray()[i] == PT_LINETO)
		{
			StartIndex = i;
			MidIndex = i+1;
		}
		else if (pPath->GetVerbArray()[i] == PT_BEZIERTO)
		{
			if (pPath->GetFlagArray()[i].IsEndPoint)
			{
				StartIndex = i;
				MidIndex = i + 1;
			}
			else if (i < pPath->GetNumCoords()-1 && pPath->GetFlagArray()[i+1].IsEndPoint)
			{
				StartIndex = i + 1;
				MidIndex = i + 2;
			}
			else if (i < pPath->GetNumCoords()-2 && pPath->GetFlagArray()[i+2].IsEndPoint)
			{
				StartIndex = i + 2;
				MidIndex = i + 3;
			}
			else
			{
				ERROR3("Path Flags not initialised");
			}
		}
		else
		{
			ERROR3("Unrecognised path element");
		}

		// so we now have the start & mid points, let's get the end point the same way
		if (StartIndex >= 0 && MidIndex >= 0)
		{
			if (pPath->GetVerbArray()[MidIndex] == PT_LINETO ||
				pPath->GetVerbArray()[MidIndex] == PT_MOVETO)
			{
				EndIndex = MidIndex + 1;
			}
			else if (pPath->GetVerbArray()[MidIndex] == PT_BEZIERTO)
			{
				EndIndex = MidIndex + 3;
			}
			else
			{
				ERROR3("Unrecognised path element");
			}
		}

		// if we have valid points, check all vectors imbetween
		if (StartIndex >= 0 && MidIndex >= 0 && EndIndex >= 0)
		{
			bDelete = TRUE;

			if (EndIndex < StartIndex + 2)
				bDelete = FALSE;

			for (INT32 j = StartIndex; j <= EndIndex-2; j++)
			{
				if (j == StartIndex)
				{
					Vec1.x = pPath->GetCoordArray()[j+1].x - pPath->GetCoordArray()[j].x;
					Vec1.y = pPath->GetCoordArray()[j+1].y - pPath->GetCoordArray()[j].y;
					Vec1.Normalise();
				}
				else
				{
					Vec1 = Vec2;
				}

				Vec2.x = pPath->GetCoordArray()[j+2].x - pPath->GetCoordArray()[j+1].x;
				Vec2.y = pPath->GetCoordArray()[j+2].y - pPath->GetCoordArray()[j+1].y;

				Vec2.Normalise();

				dot = (Vec1.x * Vec2.x) + (Vec1.y * Vec2.y);

				if (dot < Thres)
				{
					// angle is greater than the threshold, so don't delete this section
					bDelete = FALSE;
					break;
				}
				else
				{
					bDelete = TRUE;
				}
			}

			if (bDelete)
			{
				if (pPath->GetVerbArray()[MidIndex] == PT_LINETO)
				{
					// delete this point
					pPath->DeleteSection(MidIndex, 1);
				}
				else if (pPath->GetVerbArray()[MidIndex] == PT_BEZIERTO)
				{
					pPath->DeleteSection(MidIndex, 3);
				}

				// start again at this point
				i --;
			}
		}
	}	

	if (bClose)
	{
		pPath->GetVerbArray()[pPath->GetNumCoords() - 1] |= PT_CLOSEFIGURE;
	}
}
