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
// Implementation file for functions which operate on sub path elements
// ie lines and curves

/*
*/

#include "camtypes.h"
#include "paths.h"
#include "pathutil.h"
#include <math.h>
#include "fixmem.h"
#include "macros.h"

#define	EPSILON	(ldexp(1.0,-MAXDEPTH-1))	// Flatness control value
#define	DEGREE 3							// Cubic Bezier curve
#define	W_DEGREE 5							// Degree of eqn to find roots of

const INT32 MAXDEPTH = 64;					// Maximum depth for recursion



/********************************************************************************************

>	DocCoord RampItem::PointOnSemiCircle(const DocCoord& centre, const DocCoord& radialp, double t)

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/03/97
	Inputs:		centre  = the centre point of the circle
				radialp = another point (anywhere, but assumed to be somewhere on the radius of
						  the circle.
				t		= a parameter (0..1) defining the parametric locus about the centre from
						  0 to pi radians.
	Outputs:	-
	Returns:	A new doc coord which is a point on the circles perimeter which corresponds to
				the parameter t. 
				  At t==1 the function will evaluate to radialp
				  At t==0 the function will evaluate to centre-(radialp-centre)
	Purpose:	Find a point on the circumference of a semicircle. 
	            The semicircle is specified by two points, it's	centre and a point on the 
				circumference. If c=0,0 and p=(1,0) then the semicircle exists in the y positive
				half of the plane from sweeping from (-1,0) at t==0 to (1.0) at t==1.0
	Errors:		None.

********************************************************************************************/

DocCoord PathUtil::PointOnSemiCircle(const DocCoord& centre, const DocCoord& radialp, double t)
{
	double X = (double)(radialp.x - centre.x);
	double Y = (double)(radialp.y - centre.y);
	double p = t * XS_PI;

	// spin clockwise.
	double s =  sin(p);
	double c = -cos(p);

	DocCoord r;

	// circular rounding maybe?
	r.x = (INT32)(X*c - Y*s) + centre.x;
	r.y = (INT32)(X*s + Y*c) + centre.y;

	return r;
}



/********************************************************************************************

>	double PathUtil::SqrDistanceToSemiCircle(const DocCoord* plist,
											 const DocCoord& p1, 
											 double* param )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		plist	= a pointer to two doc coordinates describing a half circle
				p1		= the point to find the squared distance to
	Outputs:	param	= the parameter at which the closest point exists
	Returns:	a double, the distance a given point is away from a line element
	Purpose:	Calculates the distance p1 is away from a semi circle segment. 
				plist[0] describes the centre of the semi circle
				plist[1] describes a point on the circumference of the circle.
				For instance say c the centre is (0,0) and cp the circumference point is
				(1,0) then the semicircle exists in the positive y half of the place and
				sweeps from (-1,0) to (1,0). Parameter space is 
				  param==0   at (-1,0)
				  param==1   at ( 1,0)
				  param==0.5 at ( 0,1)
					
********************************************************************************************/

double PathUtil::SqrDistanceToSemiCircle(const DocCoord* plist,
										 const DocCoord& p,
										 double *param)
{
	double ex,ey,px,py,Px,Py,l,t;

	DocCoord c = plist[0];		// the centre point
	DocCoord e = plist[1];		// the end point
	DocCoord q;
	
	// translate to origin
	ex = e.x-c.x;
	ey = e.y-c.y;
	px = p.x-c.x;
	py = p.y-c.y;
	
	// build a (ex,ey) based transform
	l = sqrt(ex*ex+ey*ey);
	if (l!=0)
	{
		ex=ex/l;
		ey=ex/l;
	}
	
	// transform the click point to local canonical
	Px = px*ex+py*ey;
	Py = py*ex-px*ey;
	
	// are we below the half circle
	if (Py>0)
	{
		// no then calculate the dot product angle
		l = sqrt(Px*Px+Py*Py);
		if (l!=0)
			Px=Px/l;
		t = (XS_PI - acos(Px))/XS_PI;
		q = PathUtil::PointOnSemiCircle(c,p,t);
	}
	else
	{
		// yes then decide start point or endpoint
		if (Px>0)
		{
			t=1.0;
			q=e;
		}
		else
		{
			t=0.0;
			q=e-(c-e);
		}
	}
	
	// set the output parameter
	(*param)=t;
	// calculate the squared distance
	return PathUtil::SqrDistance(p,q);
}




/********************************************************************************************

> double PathUtil::SqrDistance(const Coord& Coord)

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/94
	Inputs:		two coordinates
	Outputs:	None
	Returns:	the squared distance between the two coordinates
	Purpose:	Accurate squared distance function.
	Errors:		None.

********************************************************************************************/

double PathUtil::SqrDistance(const DocCoord& p1, const DocCoord& p2)
{
	double dx = (double) (p2.x - p1.x);
	double dy = (double) (p2.y - p1.y);

	return ((dx*dx)+(dy*dy));

}


/********************************************************************************************

>	BOOL PathUtil::SplitLine(const double t,
				   			 const DocCoord* plist,
							 UINT32* NumElements,
							 PathVerb* OutVerbs,
							 DocCoord* OutCoords)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		t			= parameter at which to split the line
				plist		= pointer to 2 doc coords describing the line 
	Outputs:	NumElements	= number of elements generated after split
				OutVerbs	= pointer to verbs list to output data to
				OutCoords	= pointer to coords list to output data to
	Returns:	True if the line can be split, False if not.
	Purpose:	Splits a line element into two lines, returning the lists of new
				coord points and verbs.

********************************************************************************************/
/*
Technical notes:

	Two lineto coordinates will be returned as follows

		Verbs	Coords
		Lineto	OA
		Lineto	OB = IA
		
	and can be inserted as follows
	
			Input				Output		  
		Verbs	Coords		Verbs	Coords
		....	IA			....	IA
		Lineto	IB			Lineto	OA
		...		XX			Lineto	OB
							...		XX

	OB is returned in the output list in order to allow removal of the split element and
	complete replacement with the generated elements 
	 
********************************************************************************************/

BOOL PathUtil::SplitLine(const double t, 
						 const DocCoord* plist,
						 UINT32* NumElements,
						 PathVerb* Verbs,
						 DocCoord* Coords)
{
	// check t is in range for this split.

	if (t < SPLIT_EPSILON) return FALSE;
	if (t > (1.0 - SPLIT_EPSILON)) return FALSE;

	// read the lines start and end points
	
	INT32 x0,y0,x1,y1;

	x0 = plist[0].x;
	y0 = plist[0].y;
 	x1 = plist[1].x;
 	y1 = plist[1].y;  

	// fill in the output block details

	Verbs[0] = PT_LINETO;
	Coords[0].x = x0 + (INT32)(t*(x1-x0)+0.5);
	Coords[0].y = y0 + (INT32)(t*(y1-y0)+0.5);

	Verbs[1] = PT_LINETO;
	Coords[1].x = x1;
	Coords[1].y = y1;

	*NumElements = 2;

	return TRUE;

}


/********************************************************************************************

>	BOOL PathUtil::SplitCurve(const double t,
							  const DocCoord* plist,
							  UINT32* NumElements,
							  PathVerb* OutVerbs,
							  DocCoord* OutCoords)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		t			= parameter at which to split the curve
				plist		= pointer to 4 curve control points to split
	Outputs:	NumElements	= number of elements generated
				OutVerbs	= output verbs list
				OutCoords	= output coords list
	Returns:	True if the curve could be split, False if not.
	Purpose:	Splits a curve element into two curves, returning the lists of new
				control points and verbs.

********************************************************************************************/
/*				

Technical notes:

	Six bezier control points will be returned as follows:

				Input	  Output
				IA		  OA
				IB		  OB
				IC		  OC
				ID		  OD
						  OE
						  OF = ID

				The resultant curve control lists should be encorperated by the caller
				as follows:

					Input				Output		  
				Verbs	Coords		Verbs	Coords
				....	IA			....	IA
				Bez		IB			Bez		OA
				Bez		IC			Bez		OB
				Bez		ID			Bez		OC
				...		XX			Bez		OD
				...		XX			Bez		OE
									Bez		OF
									...		XX
									...		XX

********************************************************************************************/


BOOL PathUtil::SplitCurve(const double t,
						  const DocCoord* plist,
						  UINT32* NumElements,
						  PathVerb* OutVerbs,
						  DocCoord* OutCoords)
{
	// check t is in range for this split.

	if (t < SPLIT_EPSILON) return FALSE;
	if (t > (1.0 - SPLIT_EPSILON)) return FALSE;

	// fill in the path verb array, these are all curveto's

	for (INT32 i=0; i<6; i++)
		OutVerbs[i] = PT_BEZIERTO;

	// read the four curve control points

	INT32 x0,y0,x1,y1,x2,y2,x3,y3;

	x0 = plist[0].x;						// note this should be move,line,curve
	y0 = plist[0].y;
 	x1 = plist[1].x;						// these should all be curveto's
	y1 = plist[1].y;
	x2 = plist[2].x;
	y2 = plist[2].y;
	x3 = plist[3].x;
	y3 = plist[3].y;

	// now calculate six new coordinates from the given 4  

	double tx,ty;

	tx = x1+t*(x2-x1);
	ty = y1+t*(y2-y1);

	double Lx1,Ly1,Lx2,Ly2,Rx0,Ry0,Rx1,Ry1,Rx2,Ry2;

	Lx1 = x0+t*(x1-x0);
	Ly1 = y0+t*(y1-y0);
	Rx2 = x2+t*(x3-x2);
	Ry2 = y2+t*(y3-y2);
	Lx2 = Lx1+t*(tx-Lx1);
	Ly2 = Ly1+t*(ty-Ly1);
	Rx1 = tx+t*(Rx2-tx);
	Ry1 = ty+t*(Ry2-ty);
	Rx0 = Lx2+t*(Rx1-Lx2);
	Ry0 = Ly2+t*(Ry1-Ly2);

	// set the return values

	OutCoords[0].x = (INT32)(Lx1+0.5);
	OutCoords[0].y = (INT32)(Ly1+0.5);
	OutCoords[1].x = (INT32)(Lx2+0.5);
	OutCoords[1].y = (INT32)(Ly2+0.5);
	OutCoords[2].x = (INT32)(Rx0+0.5);
	OutCoords[2].y = (INT32)(Ry0+0.5);
	OutCoords[3].x = (INT32)(Rx1+0.5);
	OutCoords[3].y = (INT32)(Ry1+0.5);
	OutCoords[4].x = (INT32)(Rx2+0.5);
	OutCoords[4].y = (INT32)(Ry2+0.5);
	OutCoords[5].x = x3;
	OutCoords[5].y = y3;

	// Set the output number of elements.

	*NumElements = 6;

	return TRUE;
}



/********************************************************************************************

>	void PathUtil::GetCurveCoefs(const DocCoord* coords, PtCoefs* coefs )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		coords	= a pointer to 4 control point doccoords defining a curve
	Outputs:	coefs	= a set of curve coefficients.
	Returns:	None
	Purpose:	Converts a curve from Bezier form to Canonical form

********************************************************************************************/

void PathUtil::GetCurveCoefs(const DocCoord* coords, PtCoefs* coefs )
{
	// Read the curve coordinates.

	INT32 X0,Y0,X1,Y1,X2,Y2,X3,Y3;

	X0 = coords->x;
	Y0 = coords->y;
	coords++;
	X1 = coords->x;
	Y1 = coords->y;
	coords++;
	X2 = coords->x;
	Y2 = coords->y;
	coords++;
	X3 = coords->x;
	Y3 = coords->y;

	// Calculate the curve coefficients.

	coefs->ax = X3-X0+3*(X1-X2);
	coefs->ay = Y3-Y0+3*(Y1-Y2);
	coefs->bx = 3*(X2-2*X1+X0);
	coefs->by = 3*(Y2-2*Y1+Y0);
	coefs->cx = 3*(X1-X0);
	coefs->cy = 3*(Y1-Y0);

}


/********************************************************************************************

>	double PathUtil::SqrDistanceToLine(const DocCoord* plist,
									   const DocCoord& p1, 
									   double* t )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		plist	= a pointer to two doc coordinates describing a line
				p1		= the point to find the squared distance to
	Outputs:	t		= the parameter at which the closest point exists
	Returns:	a double, the distance a given point is away from a line element
	Purpose:	Calculates the distance p1 is away from a line segment. The perpendicular
				distance is returned only when p1 is within the volume created by sweeping
				the line in the orthoganal direction. Otherwise the distance to the
				closest end point is returned.
	
********************************************************************************************/

double PathUtil::SqrDistanceToLine(const DocCoord* plist,
								   const DocCoord& p1, 
								   double* t )

{

	// get hold of the lines end point coordinates

	INT32 x0,y0,x1,y1;

	x0 = plist[0].x;
	y0 = plist[0].y;
	x1 = plist[1].x;
	y1 = plist[1].y;

	INT32 pdx = (x1-x0);
	INT32 pdy = (y1-y0);

	// calculate the parameter at which the closest point exists
	// on an infinite line passing through (x0,y0),(x1,y1)
	// t=numdot/dendot.

	double numdot,dendot;

	numdot = (double)(p1.x - plist[0].x)*pdx + (double)(p1.y-plist[0].y)*pdy;
	dendot = (double)pdx*pdx + (double)pdy*pdy;

	// if t is less then zero then the closest point lies behind
	// the start point of the vector, hence return the squared
	// distance to the startpoint
 	
	if (numdot<=0 || dendot<=0)
	{
		*t = 0.0;
		return (SqrDistance(p1, plist[0]));
	}

	// if t is greater than or equal to 1 then the closest point
	// lies beyond the line segment, ie on the projected line
	// beyond (x1,y1). So return the squared distance to that end
	// point.
	
	if (dendot<=numdot)
	{
		*t = 1.0;
		return (SqrDistance(p1,plist[1]));
	}

	// ok the closest point lies somewhere inside the line segment
	// (x0,y0),(x1,y1) so return its parameter and the squared
	// distance to it.
	  
	*t = numdot/dendot;

	double c = (double)pdy*p1.x - (double)pdx*p1.y;
	double d = (double)y1*x0 - (double)y0*x1;
	double e = (c-d)*(c-d);

	return fabs(e/dendot);

}



/********************************************************************************************

>	DocCoord PathUtil::PointOnLine(const double t, const DocCoord* startpt)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		t		= parameter at which to evaluate point
				ptlist	= pointer to two doc coords describing start and end of the line
	Outputs:
	Returns:	DocCoord, an evaluation of a point on the specified line
	Purpose:	Given a parameter t go and evaluate the point on the described line segment
				If t is out of range, ie t>1 or t<0 the end points of the line will be
				returned. ie the routine will not evaluate unbounded points.
		
********************************************************************************************/

DocCoord PathUtil::PointOnLine(const double t, const DocCoord* ptlist)
{
	INT32 x0,y0,x1,y1;

	x0 = ptlist[0].x;
	y0 = ptlist[0].y;
	x1 = ptlist[1].x;
	y1 = ptlist[1].y;

	DocCoord d;

	if (t<=0.0)
	{
		d.x = x0;
		d.y = y0;
		return d;
	}

	if (t>=1.0)
	{
		d.x = x1;
		d.y = y1;
		return d;
	}

	d.x = x0 + (INT32)(t*(x1-x0)+0.5);
	d.y = y0 + (INT32)(t*(y1-y0)+0.5);

	return d;
}



/********************************************************************************************

>	double PathUtil::SqrDistanceToCurve(const UINT32 step, 
										const DocCoord* plist, 
										const DocCoord& p1, 
										double* t)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		step 	=	value between 2 and 256 dictates the corseness of stepping
							internally across the curve.
				plist 	=	pointer to 4 doccoord control points describing the curve
				p1		=	coordinate reference used to find closest coordinate to.
	Outputs:	t holds the parameter value of the closest point on the curve, 0<=t<=1  
	Returns:	a double being the distance of the closest point on the curve.
	Purpose:	This function returns the distance to the closest point on the curve and
				the parameter of this point


********************************************************************************************/
/*

Technical notes:

	If   Qx(t) = X0*(1-t)^3 + 3*X1*t*(1-t)^2 + 3*X2*t^2*(1-t) + X3*t^3
	and  Qy(t) is similar

	Then Qx(t) = (X0 - 3*X0*t + 3*X0*t^2 - X0*t^3) +
	             (3*X1*t - 6*X1*t^2 + 3*X1*t^3 ) +
	             (3*X2*t^2 - 3*X2*t^3) +
	             X3*t^3

	Collecting terms in t

	     Qx(t) = t^3*(X3-X0+3*(X1-X2)) +
	             t^2*(3*X0-6*X1+3*X2) +
	             t  *(3*X1-3*X0) +
	             X0

	For forward differencing

	Let  Ax = X3-X0+3*(X1-X2)
	     Bx = 3*(X0-2*X1+X2)
	     Cx = 3*(X1-X0)
		 Dx = X0

	Then Q(t) = A*t^3 + B*t^2 + C*t + D
	          = t*(C+t*(B+t*A)) + D
	
	Ok, having got this calculate the terms involved in

		D(t) = Q(t+delta)-Q(t)

	The terms involved in D(t) will give forward differences which can be
	used to walk the curve Q(t)

********************************************************************************************/

/* double PathUtil::SqrDistanceToCurve(const UINT32 step, 
									const DocCoord* plist, 
									const DocCoord& p1, 
									double* t )

{


	ENSURE(step>1 && step<257,"curve step is out of range");

	// Calculate the curve coefficients for this curve element

	PtCoefs CurveC;
	PathUtil::GetCurveCoefs(plist, &CurveC);

	double t0;
	double t1;
	double dist = PathUtil::CurveClosestRange(step, &CurveC, p1, plist[0].x, plist[0].y, t, &t0, &t1);

// TRACE( _T("T0 = %f\n"),t0);
// TRACE( _T("T  = %f\n"),*t);
// TRACE( _T("T1 = %f\n"),t1);

	if (t0 >= t1) return dist;
	if ((*t==t0) || (*t==t1)) return dist;
	if ((*t==0.0) || (*t==1.0)) return dist;

	INT32 x0,y0,x1,y1,x2,y2,x3,y3;

	x0 = plist[0].x;						// note this should be move,line,curve
	y0 = plist[0].y;
 	x1 = plist[1].x;						// these should all be curveto's
	y1 = plist[1].y;
	x2 = plist[2].x;
	y2 = plist[2].y;
	x3 = plist[3].x;
	y3 = plist[3].y;

	// now calculate the coordinates of a bezier curve
	// lying between t0 and t1
	
	double tx,ty;

	tx = x1+t0*(x2-x1);
	ty = y1+t0*(y2-y1);

	double Qx0,Qy0,Qx1,Qy1;
	double Rx1,Ry1,Rx2,Ry2,Lx1,Ly1,Lx2,Ly2;

	Lx1 = x0+t0*(x1-x0);
	Ly1 = y0+t0*(y1-y0);
	Rx2 = x2+t0*(x3-x2);
	Ry2 = y2+t0*(y3-y2);
	Lx2 = Lx1+t0*(tx-Lx1);
	Ly2 = Ly1+t0*(ty-Ly1);
	Qx1 = tx+t0*(Rx2-tx);
	Qy1 = ty+t0*(Ry2-ty);
	Qx0 = Lx2+t0*(Qx1-Lx2);
	Qy0 = Ly2+t0*(Qy1-Ly2);

	tx = x1+t1*(x2-x1);
	ty = y1+t1*(y2-y1);

	double Qx2,Qy2,Qx3,Qy3;

	Lx1 = x0+t1*(x1-x0);
	Ly1 = y0+t1*(y1-y0);
	Rx1 = x2+t1*(x3-x2);
	Ry1 = y2+t1*(y3-y2);
	Rx2 = tx+t1*(Rx1-tx);
	Ry2 = ty+t1*(Ry1-ty);
	Qx2 = Lx1+t1*(tx-Lx1);
	Qy2 = Ly1+t1*(ty-Ly1);
	Qx3 = Qx2+t1*(Rx2-Qx2);
	Qy3 = Qy2+t1*(Ry2-Qy2);

	// now home in a little bit closer
	
	CurveC.ax = Qx3-Qx0+3*(Qx1-Qx2);
	CurveC.ay = Qy3-Qy0+3*(Qy1-Qy2);
	CurveC.bx = 3*(Qx2-2*Qx1+Qx0);
	CurveC.by = 3*(Qy2-2*Qy1+Qy0);
	CurveC.cx = 3*(Qx1-Qx0);
	CurveC.cy = 3*(Qy1-Qy0);

	double t2;
	double t3;
	dist = PathUtil::CurveClosestRange(step, &CurveC, p1, Qx0, Qy0, t, &t2, &t3);

	*t = (1.0-(*t))*t0 + (*t)*t1;

// TRACE( _T("T' = %f\n\n"),*t);

	return dist;
}


double PathUtil::CurveClosestRange(const UINT32 step,
								   PtCoefs* CurveC,
								   const DocCoord& p1,
								   const double dx,
								   const double dy,
								   double* t,
								   double* t0,
								   double* t1)

{

	// Calculate the step rate across the curve given the users
	// step value

	double Delta=(double) 1/step;

	// Calculate the forward difference factors for curve stepping

	double X0,Y0,X1,Y1,X2,Y2,X3,Y3;

	X3 = 6*Delta*Delta*Delta*CurveC->ax;
	Y3 = 6*Delta*Delta*Delta*CurveC->ay;
	X2 = 2*Delta*Delta*CurveC->bx+X3;
	Y2 = 2*Delta*Delta*CurveC->by+Y3;

	// Evaluate the first differenced point on the curve

	X1 = Delta*(CurveC->cx+Delta*(CurveC->bx+Delta*CurveC->ax));
	Y1 = Delta*(CurveC->cy+Delta*(CurveC->by+Delta*CurveC->ay));
	X0 = dx;
	Y0 = dy;

	// find the distance from the first point on the curve

	double Cdist=(p1.x - dx)*(p1.x - dx) + (p1.y - dy)*(p1.y - dy);
	double Fdist;
	INT32 Cpt=0;

	for (UINT32 i=1; i<=step; i++)
	{
	  	X0+=X1;
	  	Y0+=Y1;
	  	X1+=X2;
	  	Y1+=Y2;
	  	X2+=X3;
	  	Y2+=Y3;
	  	Fdist = (X0-p1.x)*(X0-p1.x);		// Is this point closer?
	  	if (Cdist>Fdist) 
	  	{
	  		Fdist += (Y0-p1.y)*(Y0-p1.y);
	  		if (Cdist>Fdist)
	  		{
	  			Cdist=Fdist;
	  			Cpt=i;
			}
		}
	}

	INT32  tl = Cpt-2;
	UINT32 tr = Cpt+2;
	if (tl<0) tl=0;
	if (tr>step) tr=step;

	*t1 = (double) tr/step;
	*t0 = (double) tl/step;
	*t  = (double) Cpt/step;				// set the point parameter

	return Cdist;							// return the point distance
}

*/


/********************************************************************************************

>	DocCoord PathUtil::PointOnCurve(double t, const DocCoord* plist)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		t		= parameter to evaluate curve at
				plist	= pointer to list of 4 curve control points.
	Outputs:
	Returns:	A DocCoord describing the point on the curve	
	Purpose:	Evaluate a bezier curve, given a pointer to a set of control points and
				a parameter value. 

********************************************************************************************/

DocCoord PathUtil::PointOnCurve(double t, const DocCoord* plist)
{
	PtCoefs p;
	DocCoord c;

	PathUtil::GetCurveCoefs(plist, &p);					// calculate coefs block

	if (t<0) t=0;										// clamp range of parameter
	if (t>1) t=1;

	// Eval bezier at specified parameter

 	double dx = t*(p.cx+t*(p.bx+t*p.ax));
	double dy =	t*(p.cy+t*(p.by+t*p.ay));

	c.x = plist->x + (INT32)(dx+0.5);
	c.y = plist->y + (INT32)(dy+0.5);
	
	return c;
}


/********************************************************************************************
	Quick local vector functions to calculate various properties of vectors
********************************************************************************************/

Point2 V2ScaleII( Point2 *v, double s)
{
    Point2 result;

    result.x = v->x * s; 
    result.y = v->y * s;

    return (result);
}


double V2SquaredLength(Point2* a)
{
	return( a->x*a->x + a->y*a->y );
}


Point2 *V2Sub(Point2* a, Point2* b, Point2* c)
{
  c->x = a->x - b->x;
  c->y = a->y - b->y;
  return(c);
}


double V2Dot(Point2* a, Point2* b)
{
  return (a->x*b->x + a->y*b->y);
}



/********************************************************************************************

	Point2 Bezier( Point2 *V, INT32 degree, double t, Point2 *Left, Point2 *Right)	

  	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		V		= Control points of cubic Bezier
				degree	= The degree of the polynomial
				t		= Parameter value	
	Outputs:	Left	= RETURN left half ctl pts (if NULL	return none)
 				Right	= RETURN right half ctl pts	(if NULL return none)
	Returns:	Q(t), a point on the curve
	Purpose:	Evaluate a Bezier curve at a particular parameter value
				Fill in control points for resulting sub-curves if "Left" and
 				"Right" are non-null.

********************************************************************************************/

Point2 Bezier( Point2 *V, INT32 degree, double t,	Point2 *Left, Point2 *Right)	
{
    INT32 	i, j;		// Index variables
    Point2 	Vtemp[W_DEGREE+1][W_DEGREE+1];

    // Copy control points

    for (j =0; j <= degree; j++) 
    {
		Vtemp[0][j] = V[j];
    }

    // Triangle computation

    for (i = 1; i <= degree; i++) 
    {	
		for (j =0 ; j <= degree - i; j++) 
		{
	    	Vtemp[i][j].x =	(1.0 - t) * Vtemp[i-1][j].x + t * Vtemp[i-1][j+1].x;
	    	Vtemp[i][j].y =	(1.0 - t) * Vtemp[i-1][j].y + t * Vtemp[i-1][j+1].y;
		}
    }
    
    if (Left != NULL) 
    {
		for (j = 0; j <= degree; j++) 
		{
	    	Left[j]  = Vtemp[j][0];
		}
    }
    if (Right != NULL) 
    {
		for (j = 0; j <= degree; j++) 
		{
	    	Right[j] = Vtemp[degree-j][j];
		}
    }

    return (Vtemp[degree][0]);
}



/********************************************************************************************

	double ComputeXIntercept(Point2 *V, INT32 degree)
 
 	Author:		
	Created:	22/08/94
	Inputs:		V		= Control points of cubic Bezier
				degree	= The degree of the polynomial
	Outputs:
	Returns:	
	Purpose:	Compute intersection of chord from first control point to last
 				with 0-axis.

********************************************************************************************/

double ComputeXIntercept(Point2 *V, INT32 degree)
{
    double	XLK, YLK, XNM, YNM, XMK, YMK;
    double	det, detInv;
    double	S, T;
    double	X, Y;

    XLK = 1.0 - 0.0;
    YLK = 0.0 - 0.0;
    XNM = V[degree].x - V[0].x;
    YNM = V[degree].y - V[0].y;
    XMK = V[0].x - 0.0;
    YMK = V[0].y - 0.0;

    det = XNM*YLK - YNM*XLK;
    detInv = 1.0/det;

    S = (XNM*YMK - YNM*XMK) * detInv;
    T = (XLK*YMK - YLK*XMK) * detInv;
    
    X = 0.0 + XLK * S;
    Y = 0.0 + YLK * S;

    return X;
}





/********************************************************************************************

	INT32 CrossingCount( Point2 *V, INT32 degree)	
 
 	Author:		
	Created:	22/08/94
	Inputs:		V		= Control points of cubic Bezier
				degree	= The degree of the polynomial
	Outputs:
	Returns:	number of crossings
	Purpose:	Count the number of times a Bezier control polygon 
 				crosses the 0-axis. This number is >= the number of roots.


*********************************************************************************************/


INT32 CrossingCount( Point2 *V, INT32 degree)	
{
    INT32 i;	
    INT32 n_crossings = 0;	//  Number of zero-crossings
    INT32	sign, old_sign;		//  Sign of coefficients

    sign = old_sign = SGN(V[0].y);
    for (i = 1; i <= degree; i++) 
    {
		sign = SGN(V[i].y);
		if (sign != old_sign) n_crossings++;
		old_sign = sign;
    }
    return n_crossings;
}




/********************************************************************************************

	INT32 ControlPolygonFlatEnough(Point2 *V, INT32 degree)	
 
 	Author:		
	Created:	22/08/94
	Inputs:		V		= Control points of cubic Bezier
				degree	= The degree of the polynomial
	Outputs:
	Returns:	
	Purpose:	Check if the control polygon of a Bezier curve is flat enough
				for recursive subdivision to bottom out.

********************************************************************************************/


INT32 ControlPolygonFlatEnough(Point2 *V, INT32 degree)	
{
    INT32 	i;						// Index variable
    double 	*distance;				// Distances from pts to line
    double 	max_distance_above;		// maximum of these
    double 	max_distance_below;
    double 	error;					// Precision of root
    double 	intercept_1,
    	   	intercept_2,
	   		left_intercept,
		   	right_intercept;
    double 	a, b, c;				// Coefficients of implicit
    								// eqn for line from V[0]-V[deg]

    /* Find the  perpendicular distance
       from each interior control point to
       line connecting V[0] and V[degree]	*/

    distance = (double *)CCMalloc((unsigned)(degree + 1) * sizeof(double));
    {
	double	abSquared;

	// Derive the implicit equation for line connecting first
    // and last control points

	a = V[0].y - V[degree].y;
	b = V[degree].x - V[0].x;
	c = V[0].x * V[degree].y - V[degree].x * V[0].y;

	abSquared = (a * a) + (b * b);

        for (i = 1; i < degree; i++)
        {
	    // Compute distance from each of the points to that line
			distance[i] = a * V[i].x + b * V[i].y + c;
	    	if (distance[i] > 0.0) 
	    	{
				distance[i] = (distance[i] * distance[i]) / abSquared;
	    	}
	    	if (distance[i] < 0.0) 
	    	{
				distance[i] = -((distance[i] * distance[i]) / abSquared);
	    	}
		}
    }


    // Find the largest distance

    max_distance_above = 0.0;
    max_distance_below = 0.0;
    for (i = 1; i < degree; i++) 
    {
		if (distance[i] < 0.0) 
		{
	    	max_distance_below = MIN(max_distance_below, distance[i]);
		};
		if (distance[i] > 0.0) 
		{
	    	max_distance_above = MAX(max_distance_above, distance[i]);
		}
    }
    CCFree((char *)distance);

    {
	double	det, dInv;
	double	a1, b1, c1, a2, b2, c2;

	//  Implicit equation for zero line

	a1 = 0.0;
	b1 = 1.0;
	c1 = 0.0;

	//  Implicit equation for "above" line

	a2 = a;
	b2 = b;
	c2 = c + max_distance_above;

	det = a1 * b2 - a2 * b1;
	dInv = 1.0/det;
	
	intercept_1 = (b1 * c2 - b2 * c1) * dInv;

	//  Implicit equation for "below" line

	a2 = a;
	b2 = b;
	c2 = c + max_distance_below;
	
	det = a1 * b2 - a2 * b1;
	dInv = 1.0/det;
	
	intercept_2 = (b1 * c2 - b2 * c1) * dInv;
    }

    // Compute intercepts of bounding box

    left_intercept = MIN(intercept_1, intercept_2);
    right_intercept = MAX(intercept_1, intercept_2);

    error = 0.5 * (right_intercept-left_intercept);    
    if (error < EPSILON) 
    {
		return 1;
    }
    else 
    {
		return 0;
    }
}



/********************************************************************************************

	INT32 FindRoots( Point2* w, INT32 degree, double* t, INT32 depth)
 
 	Author:		
	Created:	22/08/94
	Inputs:		w		= Control points of cubic Bezier
				degree	= The degree of the polynomial
				depth	= The depth of the recursion
	Outputs:	t		= a list of candidate t-values
	Returns:
	Purpose:	Given a 5th-degree equation in Bernstein-Bezier form, find
 				all of the roots in the interval [0, 1].  Return the number
 				of roots found.
 
 ********************************************************************************************/

INT32 FindRoots( Point2* w, INT32 degree, double* t, INT32 depth) 
{  
    INT32 	i;
    Point2 	Left[W_DEGREE+1],		// New left and right
    	  	Right[W_DEGREE+1];		// control polygons
    INT32 	left_count,				// Solution count from children
			right_count;
    double 	left_t[W_DEGREE+1],		// Solutions from kids
	   		right_t[W_DEGREE+1];

    switch (CrossingCount(w, degree)) 
    {
       	case 0 : 
       	{	// No solutions here
	     return 0;	
	     break;
		}
		case 1 : 
		{	// Unique solution
		    // Stop recursion when the tree is deep enough
		    // if deep enough, return 1 solution at midpoint
		    if (depth >= MAXDEPTH) 
		    {
				t[0] = (w[0].x + w[W_DEGREE].x) / 2.0;
				return 1;
		    }
		    if (ControlPolygonFlatEnough(w, degree)) 
		    {
				t[0] = ComputeXIntercept(w, degree);
				return 1;
		    }
		    break;
		}
	}

    // Otherwise, solve recursively after
    // subdividing control polygon

    Bezier(w, degree, 0.5, Left, Right);
    left_count  = FindRoots(Left,  degree, left_t, depth+1);
    right_count = FindRoots(Right, degree, right_t, depth+1);


    /* Gather solutions together	*/
    for (i = 0; i < left_count; i++) 
    {
        t[i] = left_t[i];
    }
    for (i = 0; i < right_count; i++) 
    {
 		t[i+left_count] = right_t[i];
    }

    /* Send back total number of solutions	*/
    return (left_count+right_count);
}




/********************************************************************************************

	Point2 *ConvertToBezierForm( Point2 P, Point2* V )

	Author:		
	Created:	22/08/94
	Inputs:		P	=  The point to find t for
				V	=  Control points of cubic Bezier
	Outputs:
	Returns:
	Purpose:	Given a point and a Bezier curve, generate a 5th-degree
 				Bezier-format equation whose solution finds the point on the
 				curve nearest the user-defined point.

 *******************************************************************************************/

Point2 *ConvertToBezierForm( Point2 P, Point2* V )
{
    INT32 i, j, k, m, n, ub, lb;	
    INT32 row, column;			// Table indices
    Vector2 c[DEGREE+1];		// V(i)'s - P
    Vector2 d[DEGREE];			// V(i+1) - V(i)
    Point2 *w;					// Ctrl pts of 5th-degree curve
    double cdTable[3][4];		// Dot product of c, d
    static double z[3][4] = {	// Precomputed "z" for cubics
		{1.0, 0.6, 0.3, 0.1},
		{0.4, 0.6, 0.6, 0.4},
		{0.1, 0.3, 0.6, 1.0},
    };


    // Determine the c's -- these are vectors created by subtracting
    // point P from each of the control points

    for (i = 0; i <= DEGREE; i++) 
    {
		V2Sub(&V[i], &P, &c[i]);
    }
    
    // Determine the d's -- these are vectors created by subtracting
    // each control point from the next

    for (i = 0; i <= DEGREE - 1; i++) 
    { 
		d[i] = V2ScaleII(V2Sub(&V[i+1], &V[i], &d[i]), 3.0);
    }

    // Create the c,d table -- this is a table of dot products of the
    // c's and d's
    
    for (row = 0; row <= DEGREE - 1; row++) 
    {
		for (column = 0; column <= DEGREE; column++) 
		{
	    	cdTable[row][column] = V2Dot(&d[row], &c[column]);
		}
    }

    // Now, apply the z's to the dot products, on the skew diagonal
    // Also, set up the x-values, making these "points"

    w = (Point2 *)CCMalloc((unsigned)(W_DEGREE+1) * sizeof(Point2));
    for (i = 0; i <= W_DEGREE; i++) 
    {
		w[i].y = 0.0;
		w[i].x = (double)(i) / W_DEGREE;
    }

    n = DEGREE;
    m = DEGREE-1;
    for (k = 0; k <= n + m; k++) 
    {
		lb = MAX(0, k - m);
		ub = MIN(k, n);
		for (i = lb; i <= ub; i++) 
		{
	    	j = k - i;
	    	w[i+j].y += cdTable[j][i] * z[j][i];
		}
    }

    return (w);
}


/*******************************************************************************************

>	double PathUtil::SqrDistanceToCurve(DocCoord* V, DocCoord& P, double* mu )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/94
	Inputs:		split =	 max level at which to split the curve (usually 64)
				P	  =  The user-supplied point
				V	  =  Control points of cubic Bezier
	Outputs:	mu holds the parameter value of the closest point on the curve, 0<=mu<=1	
	Returns:	The squared distance to the curve
	Purpose:  	Compute the parameter value of the point on a Bezier
 				curve segment closest to some arbtitrary, user-input point.
 				Return the squared distance to the curve.
 
 *******************************************************************************************/

double PathUtil::SqrDistanceToCurve(const UINT32 split,
									const DocCoord* V,
									const DocCoord& P,
									double* mu )
{
    Point2	*w;								// Ctrl pts for 5th-degree equ
	Point2  lcurve[DEGREE+1];				// Local double version of control polygon
    double 	t_candidate[W_DEGREE];			// Possible roots     
    INT32 	n_solutions;					// Number of roots found
    double	t;								// Parameter value of closest pt
	double  dist;							// Closest squared distance to curve

	// Convert control polygon to double type

	for (INT32 i=0; i < DEGREE+1; i++)
	{
		lcurve[i].x = (double) V[i].x;
		lcurve[i].y = (double) V[i].y;
	}

	Point2 p;
	p.x = (double) P.x;
	p.y = (double) P.y;

   	//  Convert problem to 5th-degree Bezier form

	w = ConvertToBezierForm(p, lcurve);

    // Find all possible roots of 5th-degree equation

    n_solutions = FindRoots(w, W_DEGREE, t_candidate, 0);
    CCFree((char *)w);

    // Compare distances of P to all candidates, and to t=0, and t=1

    {	double 	new_dist;
		Point2 	q;
		Vector2 v;

	// Check distance to beginning of curve, where t = 0

		dist = V2SquaredLength(V2Sub(&p, lcurve, &v));
        t = 0.0;

	// Find distances for candidate points

        for (INT32 i = 0; i < n_solutions; i++) 
        {
	    	q = Bezier(lcurve, DEGREE, t_candidate[i], NULL, NULL);
	    	new_dist = V2SquaredLength(V2Sub(&p, &q, &v));
	    	if (new_dist < dist)
	    	{
                	dist = new_dist;
	        		t = t_candidate[i];
    	    }
        }

	// Finally, look at distance to end point, where t = 1

		new_dist = V2SquaredLength(V2Sub(&p, &(lcurve[DEGREE]), &v));
        if (new_dist < dist)
        {
           	dist = new_dist;
	    	t = 1.0;
        }
    }

   *mu = t;
   return dist;

}
