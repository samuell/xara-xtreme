// $Id: tracergn.h 662 2006-03-14 21:31:49Z alex $
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
// This is the header file for the bitmap effect atom class

#ifndef INC_TRACERGN
#define INC_TRACERGN

#include "coord.h"
#include "doccoord.h"

class UndoableOperation;
class Path;
class KernelBitmap;
class DocCoord;
class BfxPixelOp;

/********************************************************************************************

>	class TraceBoundaryPoint : public Coord

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/1994
	Purpose:	Implements the TraceRegion class.

This is an exceedingly simple class used to store the coordinates of boundary points on the
trace

********************************************************************************************/


class TraceBoundaryPoint : public /*Coord*/ SimpleCCObject
{
	friend class TraceRegion;
	
	inline void translate( const TraceBoundaryPoint & that);

	inline TraceBoundaryPoint(double x1, double y1) { x=x1; y=y1; };
	inline TraceBoundaryPoint() {};
	inline TraceBoundaryPoint(const TraceBoundaryPoint & that) {x=that.x; y=that.y;};
	inline TraceBoundaryPoint& operator=(const TraceBoundaryPoint & that) {x=that.x; y=that.y; return *this;};

	inline void operator = (const DocCoord& Other) { x=Other.x; y=Other.y; };
	inline BOOL operator==(const TraceBoundaryPoint& input) const { return ((input.x == x) && (input.y == y)); };
	inline BOOL operator!=(const TraceBoundaryPoint& input) const { return ((input.x != x) || (input.y != y));};

	void RotateDirection(const TraceBoundaryPoint & rotation);

	void Init() {x=0; y=0;}
	void Init(INT32 x1, INT32 y1) { x=x1; y=y1; };
	inline double SquaredLength()	{ return (x*x + y*y); }
	inline double Length()			{ return sqrt(x*x + y*y); }
	inline double Dot(const TraceBoundaryPoint& Other)	{ return (Other.x*x + Other.y*y); }

	TraceBoundaryPoint SetLength( double NewLen );

	// Addition/Subtraction operators
	inline friend TraceBoundaryPoint operator + (const TraceBoundaryPoint& Point1, const TraceBoundaryPoint& Point2);
	inline friend TraceBoundaryPoint operator - (const TraceBoundaryPoint& Point1, const TraceBoundaryPoint& Point2);

	// Unary minus
	inline TraceBoundaryPoint operator - ();

	// Scale
	inline TraceBoundaryPoint operator * ( double Factor );
	inline TraceBoundaryPoint operator / ( double Factor );

	double x;
	double y;

	private:
	
};

/********************************************************************************************

>	TraceBoundaryPoint TraceBoundaryPoint::operator - ()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & Alex
	Created:	02/03/94
	Returns:	A TraceBoundaryPoint that is the negative of this
	Purpose:	Unary Minus for a TraceBoundaryPoint (Vector)

********************************************************************************************/

TraceBoundaryPoint TraceBoundaryPoint::operator - ()
{
	TraceBoundaryPoint Result;

	// negate the vector
	Result.x = -x;
	Result.y = -y;

	// and return it
	return Result;
}



/********************************************************************************************

>	TraceBoundaryPoint TraceBoundaryPoint::operator * (double Factor)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & Alex
	Created:	02/03/94
	Inputs:		Factor - the amount to scale the vector by
	Returns:	TraceBoundaryPoint - this vector multiplied by the Factor
	Purpose:	TraceBoundaryPoint Multiply function. This will multiply the vector by a constant
				Factor. The result is returned

********************************************************************************************/

TraceBoundaryPoint TraceBoundaryPoint::operator * (double Factor)
{
	TraceBoundaryPoint Result;

	// Scale the vector by the factor
	Result.x = (x*Factor);
	Result.y = (y*Factor);

	// and return it
	return Result;
}


/********************************************************************************************

>	TraceBoundaryPoint TraceBoundaryPoint::operator / (double Factor)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & Alex
	Created:	02/03/94
	Inputs:		Factor - the amount to scale the vector by
	Returns:	TraceBoundaryPoint - this vector divided by the Factor
	Purpose:	TraceBoundaryPoint Multiply function. This will divide the vector by a constant
				Factor. The result is returned

********************************************************************************************/

TraceBoundaryPoint TraceBoundaryPoint::operator / (double Factor)
{
	TraceBoundaryPoint Result;

	// Scale the vector by the factor
	Result.x = (x/Factor);
	Result.y = (y/Factor);

	// and return it
	return Result;
}


/********************************************************************************************

>	TraceBoundaryPoint operator + (const TraceBoundaryPoint& Point1, const TraceBoundaryPoint& Point2)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & Alex
	Created:	02/03/94
	Inputs:		Point1 - The first TraceBoundaryPoint Vector
				Point2 - The Second TraceBoundaryPoint vector
	Returns:	TraceBoundaryPoint - the TraceBoundaryPoint vector that is a combination of Point1 and Point2
	Purpose:	Adds two TraceBoundaryPoint vectors together. This function is a Friend of the TraceBoundaryPoint
				class

********************************************************************************************/

TraceBoundaryPoint operator + (const TraceBoundaryPoint& Point1, const TraceBoundaryPoint& Point2)
{
	TraceBoundaryPoint Result;

	// Add the two vector together
	Result.x = Point1.x + Point2.x;
	Result.y = Point1.y + Point2.y;

	// return the result
	return Result;
}


/********************************************************************************************

>	TraceBoundaryPoint operator - (const TraceBoundaryPoint& Point1, const TraceBoundaryPoint& Point2)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> & Alex
	Created:	02/03/94
	Inputs:		Point1 - The first TraceBoundaryPoint Vector
				Point2 - The Second TraceBoundaryPoint vector
	Returns:	TraceBoundaryPoint - the TraceBoundaryPoint vector that is the first vector minus the second
				vector
	Purpose:	Subtracts the two TraceBoundaryPoint vectors. This function is a Friend of the TraceBoundaryPoint
				class

********************************************************************************************/

TraceBoundaryPoint operator - (const TraceBoundaryPoint& Point1, const TraceBoundaryPoint& Point2)
{
	TraceBoundaryPoint Result;

	// Subtract the two vector from each other
	Result.x = Point1.x - Point2.x;
	Result.y = Point1.y - Point2.y;

	// return the result
	return Result;
}

inline void TraceBoundaryPoint::translate(const TraceBoundaryPoint & that) { x+=that.x; y+=that.y;}

/********************************************************************************************

>	class TraceRegion : public CCObject

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/1994
	Purpose:	Implements the TraceRegion class.

This class takes a given point on a bitmap and produces a boundary trace for it. The boundary
criterion currently imposed is 'not white'.

********************************************************************************************/

#define TR_NUMDIRECTIONMASK (0x7)
#define FIT_STEPS	10

class TraceRegion : public CCObject
{
	CC_DECLARE_DYNCREATE(TraceRegion);

	//TraceRegion(); constructors and destructors are simple
	//~TraceRegion();

	public:

	// Constructor & Destructor
	TraceRegion();
	~TraceRegion();

	// External object interface
	BOOL UseBitmap(KernelBitmap * pBitmap);
	BOOL UsePath(Path * pPath);
	BOOL Trace(INT32 InitialX, INT32 InitialY, BfxPixelOp * thepBfxPixelOp);

	BOOL SetParams(double * pPixelError = NULL);
	BOOL GetParams(double * pPixelError = NULL);

	// For testing porpoises
	static void Test(UndoableOperation * Op);

	protected:

	// For testing porpoises
	BOOL TraceBoundary(DocCoord Origin,DocCoord Point1,DocCoord Point2);

	// Trace buffer management	
	BOOL FillBoundaryBuffer(BOOL * End);
	BOOL ProcessBoundaryBuffer(BOOL Done = FALSE);
	BOOL FindInitialPoint(BOOL * End);
	
	// Curve smoothing	
	void FitCubic(INT32 FirstPoint, INT32 LastPoint, TraceBoundaryPoint Tangent1, TraceBoundaryPoint Tangent2,
				  BOOL IsStartCusp, BOOL IsEndCusp);
	void GenerateBezier(INT32 FirstPoint, INT32 LastPoint, TraceBoundaryPoint Tangent1, 
						TraceBoundaryPoint Tangent2, TraceBoundaryPoint* Bezier);
	
	TraceBoundaryPoint BezierPoint( TraceBoundaryPoint* Bez, double u);
	BOOL CalcMaxError(INT32 LeftPoint, INT32 RightPoint, TraceBoundaryPoint* Bezier,
								 INT32* SplitPoint, double * MaxDist, INT32 Level);
	void Parameterize(INT32 FirstPoint, INT32 LastPoint);

	TraceBoundaryPoint LeftTangent(INT32 Start, INT32 Points);
	TraceBoundaryPoint RightTangent(INT32 End, INT32 Points);
	TraceBoundaryPoint CentreTangent(INT32 Centre, INT32 Points);

	// Path management	
 	void InsertBezier(TraceBoundaryPoint* Bezier, BOOL IsStartCusp, BOOL IsEndCusp);
	void InsertLine(const DocCoord& Start, const DocCoord& End,BOOL IsStartCusp, BOOL IsEndCusp);
	void InsertStraightLine(const DocCoord& End);

	// Functions to evaluate various components of the bezier function
	double Bezier0(double u) { double t=1.0-u; return (t*t*t); }
	double Bezier1(double u) { double t=1.0-u; return (3*u*t*t); }
	double Bezier2(double u) { double t=1.0-u; return (3*u*u*t); }
	double Bezier3(double u) { return (u*u*u); }


	// Miscellaneous
	inline BOOL IsCuspDirectionChange(INT32 dir1, INT32 dir2);


	TraceBoundaryPoint * BoundaryBuffer;
	BfxPixelOp * pBfxPixelOp;

	KernelBitmap * Bitmap;
	INT32 xsize;
	INT32 ysize;

	TraceBoundaryPoint InitialPoint;
	TraceBoundaryPoint HeadPoint;
	INT32	InitialDirection;
	INT32 TailDirection;
	INT32 HeadDirection;

	TraceBoundaryPoint FirstBufferPoint;

	INT32 BoundaryTail;
	INT32 BoundaryHead;
	INT32 BoundaryRingMask;
	INT32 BoundaryRingSize;

	double Error;	  		// Squared error
	double PixelError;		// user i/f to error above

	BOOL VirginBuffer;
	BOOL VirginPath;
	BOOL AtCusp;
	BOOL TailCusp;

	Path * ThePath;

	TraceBoundaryPoint Directions[TR_NUMDIRECTIONMASK+1];	
	TraceBoundaryPoint DirectionPoint[TR_NUMDIRECTIONMASK+1][TR_NUMDIRECTIONMASK+1][4];

	double TValues[FIT_STEPS+1];
	INT32 TPoints[FIT_STEPS+1];
	TraceBoundaryPoint Q1[3], Q2[2]; 	// Q' & Q''
	INT32	NumTPoints;

};

inline BOOL TraceRegion::IsCuspDirectionChange(INT32 dir1, INT32 dir2)
		{ return (((dir1-dir2+TR_NUMDIRECTIONMASK+2) & TR_NUMDIRECTIONMASK)>2); }
		// Returns TRUE iff (dir1 - dir2) MOD numberofdirections is neither -1, 0, nor 1, i.e. 90 degree or greater
		// path change


#endif  // INC_TRACERGN


