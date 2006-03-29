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
// Header for the base class mould shapes implementation

#ifndef INC_GMOULD
#define INC_GMOULD

/********************************************************************************************

>	class GMould : public CCObject

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com> / Peter
	Created:	21/01/97
	Purpose:	common functions used GEnvelope and GPerspective are held in this base class.

********************************************************************************************/

/*

void Define( POINT *P, RECT *pBBox, DWORD pTolerance = 0 ) ;

	Define the envelope with twelve control points P. The source path has bounding box pBBox.
	pTolerance defines the accuracy with which curves are moulded. If pTolerance is zero then
	curves are never split.

void FitPoint(  POINT a, POINT& b ) ;

	This will mould the point a returning the result in b. Note that a may be outside the
	original bounding box.

void FitPoint( DPOINT a, POINT& b ) ;

	This will mould the point a specified as two values in the range 0 to 1. The result is
	returned in b. This call can be used for drawing grid points.

void PlotTile(
	void* pData,
	void PlotTriangle(
		void* pData,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC,
		CONST POINT *Point0, CONST POINT *Point1, CONST POINT *Point2
	),
	CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC,
	CONST INT32 Steps
) ;

	This call can be used to plot a bitmap by splitting the bitmap into triangles. The bitmap
	area is defined by the three points PointA, PointB and PointC. Steps controls the fineness of
	the approximation. The PlotTriangle routine is called to plot each triangle in turn. It is
	passed pData and the new bitmap control points PointA, PointB, PointC and the triangle
	boundary points Point0, Point1, Point2. pData can be used as a pointer to a data block
	containing such information as the bitmap size.

void PlotTile4(
	void* pData,
	void PlotTriangle(
		void* pData,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC,
		CONST POINT *Point0, CONST POINT *Point1, CONST POINT *Point2
	),
	CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD,
	CONST INT32 Steps
) ;

	This call can be used to plot a bitmap by splitting the bitmap into triangles. The bitmap
	area is defined by the three points PointA, PointB, PointC and PointD. Other parameters are
	as for PlotTile.

*/


class GPerspective ;

class GMould : public CCObject
{
	CC_DECLARE_DYNAMIC(GMould)
	
	public:
		GMould();

		BOOL Define( POINT *P, RECT *pBBox, DWORD pTolerance = 0 );
		
		virtual void CopyShape( POINT *P ) = 0;
		virtual BOOL Redefine( POINT *P ) = 0 ;
		virtual BOOL FitPath(CONST POINT *IPoints,	
							 CONST BYTE  *ITypes, 
							 DWORD ILength,
							 LPPOINT OPoints, 
							 LPBYTE  OTypes, 
							 DWORD OLength,	
							 BOOL Close = FALSE) = 0;

		void	FitPoint( POINT a, POINT& b );
		void	FitPoint( DPOINT a, POINT& b );
		DocRect GetBoundingRect();
		RECT    GetSourceBBox() const { return SrcBBox; }

	/*
		void PlotTile(
			void* pData,
			void PlotTriangle(
				void* pData,
				CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC,
				CONST POINT *Point0, CONST POINT *Point1, CONST POINT *Point2
			),
			CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC,
			CONST INT32 Steps
		) ;
		void PlotTile4(
			void* pData,
			void PlotTriangle(
				void* pData,
				CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC,
				CONST POINT *Point0, CONST POINT *Point1, CONST POINT *Point2
			),
			CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD,
			CONST INT32 Steps
		) ;
	*/


	protected:
		double CalcTolerance( DWORD pTolerance );
		
		inline BOOL	GenOp( BYTE Op, DPOINT T );
		inline BOOL	GenMove( DPOINT T );
		inline BOOL	GenLine( DPOINT T );
		
		void        ScaleSrc(  POINT a, DPOINT& b ) ;
		void           Scale(  POINT a, DPOINT& b ) ;
		void	InverseScale( DPOINT a,  POINT& b ) ;

		BOOL	GenCurve( DPOINT C2, DPOINT C4, 
						  DPOINT P0, DPOINT P2, DPOINT P4, DPOINT P6,
						  DPOINT T0, DPOINT T2, DPOINT T4, DPOINT T6 );

		virtual void ScaleView() = 0;
		virtual void CalcViewBBox() = 0;
		virtual void CalcViewConsts() = 0;
		virtual void Transform( DPOINT a, DPOINT& b ) = 0;

	protected:
		RECT    SrcBBox ;
		INT32    MinX, MaxX, MinY, MaxY;
		INT32	Width, Depth ;

		double  Tolerance ;
		LPPOINT OPoints ; 
		LPBYTE  OTypes ; 
		DWORD   OLength ;


	/*
		void PlotTile(
			void* pData,
			void PlotTriangle(
				void* pData,
				CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC,
				CONST POINT *Point0, CONST POINT *Point1, CONST POINT *Point2
			),
			CONST INT32 Tolerance,
			INT32 i, INT32 j, INT32 s,
			POINT p00, POINT p10, POINT p11, POINT p01,
			POINT q00, POINT q10, POINT q11, POINT q01,
			BOOL f00_10, BOOL f10_11, BOOL f01_11, BOOL f00_01
		) ;
		void PlotTile4(
			void* pData,
			void PlotTriangle(
				void* pData,
				CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC,
				CONST POINT *Point0, CONST POINT *Point1, CONST POINT *Point2
			),
			CONST INT32 Tolerance,
			GPerspective* Per,
			INT32 i, INT32 j, INT32 s,
			POINT p00, POINT p10, POINT p11, POINT p01,
			POINT q00, POINT q10, POINT q11, POINT q01,
			BOOL f00_10, BOOL f10_11, BOOL f01_11, BOOL f00_01
		) ;
		void PlotTriangles(
			void* pData,
			void PlotTriangle(
				void* pData,
				CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC,
				CONST POINT *Point0, CONST POINT *Point1, CONST POINT *Point2
			),
			INT32 i, INT32 j, INT32 s,
			POINT q00, POINT q10, POINT q11, POINT q01
		) ;
	*/

};


inline BOOL GMould::GenOp( BYTE Op, DPOINT T )
{
	if ( OLength-- < 1 ) return FALSE ;
	InverseScale( T, *OPoints++ ) ; *OTypes++ = Op ;
	return TRUE ;
}

inline BOOL GMould::GenMove( DPOINT T )
{
	return GenOp( PT_MOVETO, T );
}

inline BOOL GMould::GenLine( DPOINT T )
{
	return GenOp( PT_LINETO, T );
}


#endif
