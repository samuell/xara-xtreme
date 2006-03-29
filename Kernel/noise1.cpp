// $Id$
// noise1.cpp
//
// Author  : Mike
// Purpose : Implemetation of low level noise function calls.
//			 It uses a quadratic B-Spline instead of the cubic Hermite interpolant 
//			 implied by Perlin. Also note that DNoise is just the x, y, and z 
//			 derivatives of Noise (which are also B-Splines).
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

// Version : 1.0
// Started : 20/01/96

#include "camtypes.h"
#include "macros.h"
#include "vector3d.h"
#include "noisebas.h"
#include "noise1.h"

#define TABLEWIDTH 50						// we can reduce this at the expense
#define MAXPTS	   (TABLEWIDTH*TABLEWIDTH)	// of randomness.

/**************************************************************************************
>	NoiseGen1::NoiseGen1()
***************************************************************************************/

NoiseGen1::NoiseGen1()
{
	RTable=NULL;
}

/**************************************************************************************
>	NoiseGen1::~NoiseGen1()
***************************************************************************************/

NoiseGen1::~NoiseGen1()
{
	if (RTable!=NULL)
	{
		free((void*)RTable);
		RTable=NULL;
	}
}

/**************************************************************************************

>	virtual BOOL NoiseGen1::Initialise()

	Aurthor:	Mike
	Created:	07/07/96
	Purpose:	Initialise all the data tables we need to get this noise generator
				going. This particular table holds random float values in the range
				[0..1]

***************************************************************************************/

BOOL NoiseGen1::Initialise()
{
	// allocate some space big enough
	RTable = (float*)malloc(MAXPTS * sizeof(float));
	if (RTable==NULL)
		return FALSE;

	// create the table
	SeedTable(0);
	return TRUE;
}


/**************************************************************************************

>	void NoiseGen1::SeedTable(UINT32 seed)

	Aurthor:	Mike
	Created:	07/07/96
	Inputs:		seed = the seed to use as an initial value in our random number table
	Purpose:	Seed the random number table we use.

***************************************************************************************/

void NoiseGen1::SeedTable(UINT32 seed)
{
	if (RTable!=NULL)
	{
		// seed the random number generator for consitency
		srand(seed);
		// now create those random impulses!
		// ok I'm assuming rand() will be compiled in and not a opsys level call.
		for (INT32 i=0; i<MAXPTS; i++)
			RTable[i] = (float)rand() / ((float)RAND_MAX);
	}
}


/**************************************************************************************

>	double NoiseGen1::VNoise1D(const Vector3D &p, double scale)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/97
	Inputs:		p = a three dimensional vector
				scale = a scaling factor
	Returns:	A double f, 0<=f<=1.0

	Notes:		Uses 2d quadratic B-Spline smoothing

				1d quadratic bsplines are defined as...

					C0 = 1 - 2u +  u^2
					C1 = 1 + 2u - 2u^2
					C2 =           u^2

					v = (C0*p0 + C1*p1 + C2*p2)/2		0<=u<=1

				Note, this spline is a none-interpolating spline, i.e. it tends towards
				the points but never passes through any! This doesn't matter to us as 
				we're interpolating random impulses. 
				To make our tile cyclic, wrap coordinates at the edges.
				Also note, give points { p0,p1,p2 } the curve lives in the region [p0,p1]
				i.e. it is does not cover all p0..p2. To get segment [p1,p2] you'll
				need { p1,p2,p3 }
				Calculating the cyclic curve through p0,p1,p2,p3
				fit...
				  [p0,p1] = CF{ p0,p1,p2 }
				  [p1,p2] = CF{ p1,p2,p3 }
				  [p2,p3] = CF{ p2,p3,p0 }
				  [P3,P0] = CF{ p3,p0,p1 }
				i.e. 4 input points = 4 curves = 4 intervals 

***************************************************************************************/

double NoiseGen1::VNoise1D(const Vector3D &p, double scale)
{
	float  x2, x1, x0, y2, y1, y0, xf, yf;
	float  p00, p01, p02;
	INT32    ax, bx, cx;
	INT32    ay, by, cy;
	INT32    samplerange;

	// multiplying by TABLEWIDTH rather than (TABLEWIDTH-1)
	// forces a wrapping as our range is 0..TABLEWIDTH-1
	samplerange = (INT32)(scale); // *TABLEWIDTH);

	if (samplerange<1)
		return 0.0;
	
	// essentially ignore p.z (the calling routine sets it to 0)
	xf = ((float)p.x) * samplerange;
	yf = ((float)p.y) * samplerange;
	
	// index the 9 random impulses we'll used for (xf,yf)
	ax = (INT32)floor(xf);
	ay = (INT32)floor(yf);

	// calculate the 2d bspline blending coefs
	xf = xf - ax;				// u		(i.e. fract(p.x) 0..1)
	yf = yf - ay;				// v		(i.e. fract(p.y) 0..1)
	x1 = xf * xf;				// u^2
	y1 = yf * yf;				// v^2
	x2 = 0.5f * x1;				// u^2/2
	y2 = 0.5f * y1;				// v^2/2
	x1 = 0.5f + xf - x1;		// 0.5 + u - u^2
	y1 = 0.5f + yf - y1;		// 0.5 + v - v^2
	x0 = 0.5f - xf + x2;		// 0.5 - u + u^2/2
	y0 = 0.5f - yf + y2;		// 0.5 - v + v^2/2

	// get the next neighbours
	bx = ax + 1;
	by = ay + 1;
	cx = bx + 1;
	cy = by + 1;

	// do a potential double modulus as follows...
	//
	// 0 1 2 3 4 5 6 7 8 9  a b c d e f		  index into impulse table
	// 0 1 2 3 4 5 6 7 0 1  2 3 4 5 6 7  ...  samplerange == 8
	// 0 1 2 3 4 0 1 2 0 1  2 3 4 0 1 2  ...   tablewidth == 5
	// ^			   ^
	// notice the important symmetry of the values at either end of sample range
	// this gives tileable noise.

	ax = ax % samplerange;		// all 0 if samplerange == 1
	ay = ay % samplerange;
	bx = bx % samplerange;
	by = by % samplerange;
	cx = cx % samplerange;
	cy = cy % samplerange;

	if (samplerange==1)			// do something at zero if poss
		cx=cy=1;		

	if (TABLEWIDTH<samplerange)
	{
		// if our random impulse table is smaller than samplerange
		// then we need to mod into it.
		ax = ax % TABLEWIDTH;
		ay = ay % TABLEWIDTH;
		bx = bx % TABLEWIDTH;
		by = by % TABLEWIDTH;
		cx = cx % TABLEWIDTH;
		cy = cy % TABLEWIDTH;
	}

	ay*=TABLEWIDTH;
	by*=TABLEWIDTH;
	cy*=TABLEWIDTH;

	// read and interpolate the impulse values
	p00 = RTable[ax+ay];
	p01 = RTable[bx+ay];
	p02 = RTable[cx+ay];

	xf  = y0 * (x0*p00 + x1*p01 + x2*p02);

	p00 = RTable[ax+by];
	p01 = RTable[bx+by];
	p02 = RTable[cx+by];

	xf += y1 * (x0*p00 + x1*p01 + x2*p02);

	p00 = RTable[ax+cy];
	p01 = RTable[bx+cy];
	p02 = RTable[cx+cy];

	xf += y2 * (x0*p00 + x1*p01 + x2*p02);
	
	return (double)xf;
}


/*
double NoiseGen1::VNoise1D(const Vector3D &p, double scale)
{
	float  x2, x1, x0, y2, y1, y0, xf, yf;
	float  p00, p01, p02;
	INT32    ax, bx, cx;
	INT32    ay, by, cy;
	INT32    width;

	width = (INT32)(scale);
	
	// essentially ignore p.z (the calling routine sets it to 0)
	xf = ((float)p.x) * width;
	yf = ((float)p.y) * width;
	
	// index the 9 random impulses we'll used for (xf,yf)
	ax = (INT32)floor(xf);
	ay = (INT32)floor(yf);
	bx = ax+1;
	by = ay+1;
	cx = bx+1;
	cy = by+1;

	// modulo the width we're using
	if (cx>=width)
	{
		cx-=width;
		if (bx>=width)
		{
			bx-=width;
			if (ax>=width)
				ax-=width;
		}
	}

	if (cy>=width)
	{
		cy-=width;
		if (by>=width)
		{
			by-=width;
			if (ay>=width)
				ay-=width;
		}
	}

	// calculate the 2d bspline blending coefs

	xf = xf - ax;				// u		(i.e. fract(p.x) 0..1)
	yf = yf - ay;				// v		(i.e. fract(p.y) 0..1)
	x1 = xf * xf;				// u^2
	y1 = yf * yf;				// v^2
	x2 = 0.5f * x1;				// u^2/2
	y2 = 0.5f * y1;				// v^2/2
	x1 = 0.5f + xf - x1;		// 0.5 + u - u^2
	y1 = 0.5f + yf - y1;		// 0.5 + v - v^2
	x0 = 0.5f - xf + x2;		// 0.5 - u + u^2/2
	y0 = 0.5f - yf + y2;		// 0.5 - v + v^2/2

	ay*=TABLEWIDTH;
	by*=TABLEWIDTH;
	cy*=TABLEWIDTH;

	// read and interpolate the impulse values
	p00 = RTable[ax+ay];
	p01 = RTable[bx+ay];
	p02 = RTable[cx+ay];

	xf  = y0 * (x0*p00 + x1*p01 + x2*p02);

	p00 = RTable[ax+by];
	p01 = RTable[bx+by];
	p02 = RTable[cx+by];

	xf += y1 * (x0*p00 + x1*p01 + x2*p02);

	p00 = RTable[ax+cy];
	p01 = RTable[bx+cy];
	p02 = RTable[cx+cy];

	xf += y2 * (x0*p00 + x1*p01 + x2*p02);
	
	return (double)xf;
}
*/

/**************************************************************************************

>	Vector3D NoiseGen1::VNoise3D(const Vector3D &v)

	Purpose:	Vector-valued version of "Noise"

***************************************************************************************/

Vector3D NoiseGen1::VNoise3D(const Vector3D &p)
{
	return Vector3D(0,0,0);
}
