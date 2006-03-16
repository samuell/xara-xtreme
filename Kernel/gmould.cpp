// $Id: gmould.cpp 662 2006-03-14 21:31:49Z alex $
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
#include "moldshap.h"
#include "gmould.h"

CC_IMPLEMENT_DYNAMIC(GMould, CCObject)

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// GMould.cpp
//
/////////////////////////////////////////////////////////////////////////////////////////////////

GMould::GMould()
{
	SrcBBox.left   = 0;
	SrcBBox.top    = 1;
	SrcBBox.right  = 1;
	SrcBBox.bottom = 0;

	MinX = MinY = 0;
	MaxX = MaxY = 1;
	Width = Depth = 1;

	Tolerance = 0.0;	
}


BOOL GMould::Define( POINT *P, RECT *pBBox, DWORD pTolerance )
{
	SrcBBox.left	= pBBox->left ;
	SrcBBox.top		= pBBox->top ;
	SrcBBox.right	= pBBox->right ;
	SrcBBox.bottom	= pBBox->bottom ;

	if (!Redefine( P ))
		return FALSE;

	Tolerance = CalcTolerance( pTolerance ) ;

	return TRUE;
}

DocRect GMould::GetBoundingRect()
{
	DocRect Rect;
	Rect.lo.x = MinX;
	Rect.lo.y = MinY;
	Rect.hi.x = MaxX;
	Rect.hi.y = MaxY;
	return Rect;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

double GMould::CalcTolerance( DWORD pTolerance )
{
//	INT32 x, y ;
//	x = (INT32)( ((float)Tolerance*0x10000000)/Width  ) ;
//	y = (INT32)( ((float)Tolerance*0x10000000)/Depth ) ;
	if ( pTolerance==0 )
		return 0.0 ;

	double x, y ;

	x = (double) pTolerance/Width ;
	y = (double) pTolerance/Depth ;

	return 16*(x+y+2*max(x,y)) ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void GMould::FitPoint( POINT a, POINT& b )
{
	DPOINT t ;
	ScaleSrc    ( a, t ) ;
	Transform   ( t, t ) ;
	InverseScale( t, b ) ;
}

void GMould::FitPoint( DPOINT a, POINT& b )
{
	DPOINT t ;
	Transform   ( a, t ) ;
	InverseScale( t, b ) ;
}



BOOL GMould::GenCurve( DPOINT C2, DPOINT C4, 
					   DPOINT P0, DPOINT P2, DPOINT P4, DPOINT P6,
					   DPOINT T0, DPOINT T2, DPOINT T4, DPOINT T6 )
{
	if ( Tolerance > 0 )
	{
		double T ;
		DPOINT D ;
		DPOINT P1, P3, P5 ;
		DPOINT T1, T3, T5 ;
		DPOINT N1, N3, N5 ;
		P1.x = (125*P0.x+75*C2.x+15*C4.x +    P6.x)/216 ;
		P1.y = (125*P0.y+75*C2.y+15*C4.y +    P6.y)/216 ;
		P3.x = (    P0.x+3*(C2.x+   C4.x)+    P6.x)/8 ;
		P3.y = (    P0.y+3*(C2.y+   C4.y)+    P6.y)/8 ;
		P5.x = (    P0.x+15*C2.x+75*C4.x +125*P6.x)/216 ;
		P5.y = (    P0.y+15*C2.y+75*C4.y +125*P6.y)/216 ;
		Transform( P1, T1 ) ;
		Transform( P3, T3 ) ;
		Transform( P5, T5 ) ;
		N3.x = -T0.x+9*(T2.x+T4.x)-T6.x ;
		N3.y = -T0.y+9*(T2.y+T4.y)-T6.y ;
		D.x = fabs(16*T3.x-N3.x) ;
		D.y = fabs(16*T3.y-N3.y) ;
		T = D.x+D.y+2*max(D.x,D.y) ;
		if ( T<=Tolerance )
		{
			N1.x = 5*(T0.x-T4.x)+15*T2.x+T6.x ;
			N1.y = 5*(T0.y-T4.y)+15*T2.y+T6.y ;
			D.x = fabs(16*T1.x-N1.x) ;
			D.y = fabs(16*T1.y-N1.y) ;
			T = D.x+D.y+2*max(D.x,D.y) ;
			if ( T<=Tolerance )
			{
				N5.x = T0.x+15*T4.x+5*(T6.x-T2.x) ;
				N5.y = T0.y+15*T4.y+5*(T6.y-T2.y) ;
				D.x = fabs(16*T5.x-N5.x) ;
				D.y = fabs(16*T5.y-N5.y) ;
				T = D.x+D.y+2*max(D.x,D.y) ;
			}
		}
		if ( T>Tolerance )
		{
			DPOINT CL1, CL2, CR1, CR2 ;
			CL1.x = (P0.x+C2.x)/2 ;
			CL1.y = (P0.y+C2.y)/2 ;
			CL2.x = (P0.x+2*C2.x+C4.x)/4 ;
			CL2.y = (P0.y+2*C2.y+C4.y)/4 ;
			CR1.x = (C2.x+2*C4.x+P6.x)/4 ;
			CR1.y = (C2.y+2*C4.y+P6.y)/4 ;
			CR2.x = (C4.x+P6.x)/2 ;
			CR2.y = (C4.y+P6.y)/2 ;
			return GenCurve( CL1, CL2, P0, P1, P2, P3, T0, T1, T2, T3 ) &&
				   GenCurve( CR1, CR2, P3, P4, P5, P6, T3, T4, T5, T6 ) ;
		}			
	}
	if ( OLength <= 3 ) return FALSE ;
	OLength -= 3 ;
	DPOINT N2, N4 ;
	N2.x = (-5*T0.x+18*T2.x- 9*T4.x+2*T6.x)/6 ;
	N2.y = (-5*T0.y+18*T2.y- 9*T4.y+2*T6.y)/6 ;
	N4.x = ( 2*T0.x- 9*T2.x+18*T4.x-5*T6.x)/6 ;
	N4.y = ( 2*T0.y- 9*T2.y+18*T4.y-5*T6.y)/6 ;
	InverseScale( N2, *OPoints++ ) ; *OTypes++ = PT_BEZIERTO ;
	InverseScale( N4, *OPoints++ ) ; *OTypes++ = PT_BEZIERTO ;
	InverseScale( T6, *OPoints++ ) ; *OTypes++ = PT_BEZIERTO ;
	return TRUE ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void GMould::ScaleSrc( POINT a, DPOINT& b )
{
	b.x = (double)(a.x-SrcBBox.left  )/(SrcBBox.right-SrcBBox.left  ) ;
	b.y = (double)(a.y-SrcBBox.bottom)/(SrcBBox.top  -SrcBBox.bottom) ;
}

void GMould::Scale( POINT a, DPOINT& b )
{
	b.x = (double)(a.x-MinX)/Width ;
	b.y = (double)(a.y-MinY)/Depth ;
}

void GMould::InverseScale( DPOINT a, POINT& b )
{
	b.x = MinX+(INT32)(Width*a.x) ;
	b.y = MinY+(INT32)(Depth*a.y) ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

/*
void GMould::PlotTile(
					void* pData,
					void PlotTriangle(
						void* pData,
						CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC,
						CONST POINT *Point0, CONST POINT *Point1, CONST POINT *Point2
					),
					CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC,
					CONST INT32 Tolerance
				)
{
	POINT PointX ;
	PointX.x = PointB->x+PointC->x-PointA->x ;
	PointX.y = PointB->y+PointC->y-PointA->y ;
	POINT q00,q01,q10,q11 ;
	FitPoint( *PointA, q00 ) ;
	FitPoint( *PointB, q10 ) ;
	FitPoint(  PointX, q11 ) ;
	FitPoint( *PointC, q01 ) ;
	PlotTile(
		pData, PlotTriangle,
		Tolerance,
		0,0,0,
		*PointA, *PointB, PointX, *PointC,
		q00, q10, q11, q01,
		FALSE, FALSE, FALSE, FALSE
	) ;
}



void GMould::PlotTile(
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
				)
{
	POINT a,b ;
	if ( s!=6 )
	{
		if ( !f00_10 )
		{
			a.x = (2*p00.x+p10.x)/3 ; a.y = (2*p00.y+p10.y)/3 ; FitPoint( a, a ) ;
			b.x = (2*q00.x+q10.x)/3 ; b.y = (2*q00.y+q10.y)/3 ;
			if ( abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance )
			{
				a.x = (p00.x+2*p10.x)/3 ; a.y = (p00.y+2*p10.y)/3 ; FitPoint( a, a ) ;
				b.x = (q00.x+2*q10.x)/3 ; b.y = (q00.y+2*q10.y)/3 ;
				f00_10 = abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance ;
			}
		}
		if ( !f10_11 )
		{
			a.x = (2*p10.x+p11.x)/3 ; a.y = (2*p10.y+p11.y)/3 ; FitPoint( a, a ) ;
			b.x = (2*q10.x+q11.x)/3 ; b.y = (2*q10.y+q11.y)/3 ;
			if ( abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance )
			{
				a.x = (p10.x+2*p11.x)/3 ; a.y = (p10.y+2*p11.y)/3 ; FitPoint( a, a ) ;
				b.x = (q10.x+2*q11.x)/3 ; b.y = (q10.y+2*q11.y)/3 ;
				f10_11 = abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance ;
			}
		}
		if ( !f01_11 )
		{
			a.x = (2*p01.x+p11.x)/3 ; a.y = (2*p01.y+p11.y)/3 ; FitPoint( a, a ) ;
			b.x = (2*q01.x+q11.x)/3 ; b.y = (2*q01.y+q11.y)/3 ;
			if ( abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance )
			{
				a.x = (p01.x+2*p11.x)/3 ; a.y = (p01.y+2*p11.y)/3 ; FitPoint( a, a ) ;
				b.x = (q01.x+2*q11.x)/3 ; b.y = (q01.y+2*q11.y)/3 ;
				f01_11 = abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance ;
			}
		}
		if ( !f00_01 )
		{
			a.x = (2*p00.x+p01.x)/3 ; a.y = (2*p00.y+p01.y)/3 ; FitPoint( a, a ) ;
			b.x = (2*q00.x+q01.x)/3 ; b.y = (2*q00.y+q01.y)/3 ;
			if ( abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance )
			{
				a.x = (p00.x+2*p01.x)/3 ; a.y = (p00.y+2*p01.y)/3 ; FitPoint( a, a ) ;
				b.x = (q00.x+2*q01.x)/3 ; b.y = (q00.y+2*q01.y)/3 ;
				f00_01 = abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance ;
			}
		}
	}
	if ( s==6 || (f00_10 && f10_11 && f01_11 && f00_01) )
		PlotTriangles( pData, PlotTriangle, i,j,s, q00, q10, q11, q01 ) ;
	else
	{
		POINT p00_10, p10_11, p01_11, p00_01, p ;
		POINT q00_10, q10_11, q01_11, q00_01, q ;
		p00_10.x = (p00.x+p10.x)/2 ; p00_10.y = (p00.y+p10.y)/2 ;
		p10_11.x = (p10.x+p11.x)/2 ; p10_11.y = (p10.y+p11.y)/2 ;
		p01_11.x = (p01.x+p11.x)/2 ; p01_11.y = (p01.y+p11.y)/2 ;
		p00_01.x = (p00.x+p01.x)/2 ; p00_01.y = (p00.y+p01.y)/2 ;
		p.x		 = (p00.x+p11.x)/2 ; p.y	  = (p00.y+p11.y)/2 ;
		if ( f00_10 ) { q00_10.x = (q00.x+q10.x)/2 ; q00_10.y = (q00.y+q10.y)/2 ; } else FitPoint( p00_10, q00_10 ) ;
		if ( f10_11 ) {	q10_11.x = (q10.x+q11.x)/2 ; q10_11.y = (q10.y+q11.y)/2 ; }	else FitPoint( p10_11, q10_11 ) ;
		if ( f01_11 ) {	q01_11.x = (q01.x+q11.x)/2 ; q01_11.y = (q01.y+q11.y)/2 ; }	else FitPoint( p01_11, q01_11 ) ;
		if ( f00_01 ) {	q00_01.x = (q00.x+q01.x)/2 ; q00_01.y = (q00.y+q01.y)/2 ; }	else FitPoint( p00_01, q00_01 ) ;
		FitPoint( p, q ) ;
		i <<= 1 ;
		j <<= 1 ;
		s++ ;
		PlotTile(
			pData, PlotTriangle, Tolerance, i,j,s,
			p00, p00_10, p, p00_01,
			q00, q00_10, q, q00_01,
			f00_10, FALSE, FALSE, f00_01
		) ;
		PlotTile(
			pData, PlotTriangle, Tolerance, i+1,j,s,
			p00_10, p10, p10_11, p,
			q00_10, q10, q10_11, q,
			f00_10, f10_11, FALSE, FALSE
		) ;
		PlotTile(
			pData, PlotTriangle, Tolerance, i,j+1,s,
			p00_01, p, p01_11, p01,
			q00_01, q, q01_11, q01,
			FALSE, FALSE, f01_11, f00_01
		) ;
		PlotTile(
			pData, PlotTriangle, Tolerance, i+1,j+1,s,
			p, p10_11, p11, p01_11,
			q, q10_11, q11, q01_11,
			FALSE, f10_11, f01_11, FALSE
		) ;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void GMould::PlotTile4(
					void* pData,
					void PlotTriangle(
						void* pData,
						CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC,
						CONST POINT *Point0, CONST POINT *Point1, CONST POINT *Point2
					),
					CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD,
					CONST INT32 Tolerance
				)
{
	POINT PerspectivePoints[4] ;
	PerspectivePoints[0] = *PointA ;
	PerspectivePoints[1] = *PointB ;
	PerspectivePoints[2] = *PointC ;
	PerspectivePoints[3] = *PointD ;
	RECT BBox = { 0,0,1,1 } ;
	GPerspective Per ;
	Per.Define( PerspectivePoints, &BBox, 0 ) ;

	POINT p00,p01,p10,p11 ;
	POINT q00,q01,q10,q11 ;
	DPOINT a ;
	a.x = 0.0 ; a.y = 0.0 ; Per.FitPoint( a, p00 ) ; FitPoint( p00, q00 ) ;
	a.x = 1.0 ; a.y = 0.0 ; Per.FitPoint( a, p10 ) ; FitPoint( p10, q10 ) ;
	a.x = 1.0 ; a.y = 1.0 ; Per.FitPoint( a, p11 ) ; FitPoint( p11, q11 ) ;
	a.x = 0.0 ; a.y = 1.0 ; Per.FitPoint( a, p01 ) ; FitPoint( p01, q01 ) ;

	PlotTile4(
		pData, PlotTriangle,
		Tolerance,
		&Per,
		0,0,0,
		p00, p10, p11, p01,
		q00, q10, q11, q01,
		FALSE, FALSE, FALSE, FALSE
	) ;
}



void GMould::PlotTile4(
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
				)
{
	DPOINT A ;
	POINT a,b ;
	double S = 1.0/(1<<s) ;
	double I = S*i ;
	double J = S*j ;
	double S12 = S/2.0 ;
	double S13 = S/3.0 ;
	double S23 = S13*2.0 ;
	if ( s!=6 )
	{
		if ( !f00_10 )
		{
			A.x = I+S13 ; A.y = J ; Per->FitPoint( A,a ) ; FitPoint( a,a ) ;
			b.x = (2*q00.x+q10.x)/3 ; b.y = (2*q00.y+q10.y)/3 ;
			if ( abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance )
			{
				A.x = I+S23 ; A.y = J ; Per->FitPoint( A,a ) ; FitPoint( a,a ) ;
				b.x = (q00.x+2*q10.x)/3 ; b.y = (q00.y+2*q10.y)/3 ;
				f00_10 = abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance ;
			}
		}
		if ( !f10_11 )
		{
			A.x = I+S ; A.y = J+S13 ; Per->FitPoint( A,a ) ; FitPoint( a,a ) ;
			b.x = (2*q10.x+q11.x)/3 ; b.y = (2*q10.y+q11.y)/3 ;
			if ( abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance )
			{
				A.x = I+S ; A.y = J+S23 ; Per->FitPoint( A,a ) ; FitPoint( a,a ) ;
				b.x = (q10.x+2*q11.x)/3 ; b.y = (q10.y+2*q11.y)/3 ;
				f10_11 = abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance ;
			}
		}
		if ( !f01_11 )
		{
			A.x = I+S13 ; A.y = J+S ; Per->FitPoint( A,a ) ; FitPoint( a,a ) ;
			b.x = (2*q01.x+q11.x)/3 ; b.y = (2*q01.y+q11.y)/3 ;
			if ( abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance )
			{
				A.x = I+S23 ; A.y = J+S ; Per->FitPoint( A,a ) ; FitPoint( a,a ) ;
				b.x = (q01.x+2*q11.x)/3 ; b.y = (q01.y+2*q11.y)/3 ;
				f01_11 = abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance ;
			}
		}
		if ( !f00_01 )
		{
			A.x = I ; A.y = J+S13 ; Per->FitPoint( A,a ) ; FitPoint( a,a ) ;
			b.x = (2*q00.x+q01.x)/3 ; b.y = (2*q00.y+q01.y)/3 ;
			if ( abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance )
			{
				A.x = I ; A.y = J+S23 ; Per->FitPoint( A,a ) ; FitPoint( a,a ) ;
				b.x = (q00.x+2*q01.x)/3 ; b.y = (q00.y+2*q01.y)/3 ;
				f00_01 = abs(a.x-b.x)<=Tolerance && abs(a.y-b.y)<=Tolerance ;
			}
		}
	}
	if ( s==6 || (f00_10 && f10_11 && f01_11 && f00_01) )
		PlotTriangles( pData, PlotTriangle, i,j,s, q00, q10, q11, q01 ) ;
	else
	{
		POINT p00_10, p10_11, p01_11, p00_01, p ;
		POINT q00_10, q10_11, q01_11, q00_01, q ;
		A.x = I+S12 ; A.y = J     ; Per->FitPoint( A,p00_10 ) ;
		A.x = I+S   ; A.y = J+S12 ; Per->FitPoint( A,p10_11 ) ;
		A.x = I+S12 ; A.y = J+S   ; Per->FitPoint( A,p01_11 ) ;
		A.x = I     ; A.y = J+S12 ; Per->FitPoint( A,p00_01 ) ;
		A.x = I+S12 ; A.y = J+S12 ; Per->FitPoint( A,p ) ;
		if ( f00_10 ) { q00_10.x = (q00.x+q10.x)/2 ; q00_10.y = (q00.y+q10.y)/2 ; } else FitPoint( p00_10, q00_10 ) ;
		if ( f10_11 ) {	q10_11.x = (q10.x+q11.x)/2 ; q10_11.y = (q10.y+q11.y)/2 ; }	else FitPoint( p10_11, q10_11 ) ;
		if ( f01_11 ) {	q01_11.x = (q01.x+q11.x)/2 ; q01_11.y = (q01.y+q11.y)/2 ; }	else FitPoint( p01_11, q01_11 ) ;
		if ( f00_01 ) {	q00_01.x = (q00.x+q01.x)/2 ; q00_01.y = (q00.y+q01.y)/2 ; }	else FitPoint( p00_01, q00_01 ) ;
		FitPoint( p, q ) ;
		i <<= 1 ;
		j <<= 1 ;
		s++ ;
		PlotTile4(
			pData, PlotTriangle, Tolerance, Per, i,j,s,
			p00, p00_10, p, p00_01,
			q00, q00_10, q, q00_01,
			f00_10, FALSE, FALSE, f00_01
		) ;
		PlotTile4(
			pData, PlotTriangle, Tolerance, Per, i+1,j,s,
			p00_10, p10, p10_11, p,
			q00_10, q10, q10_11, q,
			f00_10, f10_11, FALSE, FALSE
		) ;
		PlotTile4(
			pData, PlotTriangle, Tolerance, Per, i,j+1,s,
			p00_01, p, p01_11, p01,
			q00_01, q, q01_11, q01,
			FALSE, FALSE, f01_11, f00_01
		) ;
		PlotTile4(
			pData, PlotTriangle, Tolerance, Per, i+1,j+1,s,
			p, p10_11, p11, p01_11,
			q, q10_11, q11, q01_11,
			FALSE, f10_11, f01_11, FALSE
		) ;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void GMould::PlotTriangles(
					void* pData,
					void PlotTriangle(
						void* pData,
						CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC,
						CONST POINT *Point0, CONST POINT *Point1, CONST POINT *Point2
					),
					INT32 i, INT32 j, INT32 s,
					POINT q00, POINT q10, POINT q11, POINT q01
				)
{
	POINT a,b,c ;
	if ( (i^j) & 1 )
	{
		a.x = q00.x-i*(q10.x-q00.x)-j*(q01.x-q00.x) ;
		a.y = q00.y-i*(q10.y-q00.y)-j*(q01.y-q00.y) ;
		b.x = a.x+(q10.x-q00.x << s) ; b.y = a.y+(q10.y-q00.y << s) ;
		c.x = a.x+(q01.x-q00.x << s) ; c.y = a.y+(q01.y-q00.y << s) ;
		PlotTriangle( pData, &a,&b,&c, &q00,&q01,&q10 ) ;
		a.x = q11.x-(i+1)*(q11.x-q01.x)-(j+1)*(q11.x-q10.x) ;
		a.y = q11.y-(i+1)*(q11.y-q01.y)-(j+1)*(q11.y-q10.y) ;
		b.x = a.x+(q11.x-q01.x << s) ; b.y = a.y+(q11.y-q01.y << s) ;
		c.x = a.x+(q11.x-q10.x << s) ; c.y = a.y+(q11.y-q10.y << s) ;
		PlotTriangle( pData, &a,&b,&c, &q11,&q01,&q10 ) ;
	}
	else
	{
		a.x = q00.x-i*(q11.x-q01.x)-j*(q01.x-q00.x) ;
		a.y = q00.y-i*(q11.y-q01.y)-j*(q01.y-q00.y) ;
		b.x = a.x+(q11.x-q01.x << s) ; b.y = a.y+(q11.y-q01.y << s) ;
		c.x = a.x+(q01.x-q00.x << s) ; c.y = a.y+(q01.y-q00.y << s) ;
		PlotTriangle( pData, &a,&b,&c, &q00,&q01,&q11 ) ;
		a.x = q00.x-i*(q10.x-q00.x)-j*(q11.x-q10.x) ;
		a.y = q00.y-i*(q10.y-q00.y)-j*(q11.y-q10.y) ;
		b.x = a.x+(q10.x-q00.x << s) ; b.y = a.y+(q10.y-q00.y << s) ;
		c.x = a.x+(q11.x-q10.x << s) ; c.y = a.y+(q11.y-q10.y << s) ;
		PlotTriangle( pData, &a,&b,&c, &q00,&q10,&q11 ) ;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
*/
