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
// Envelope shape implementation

/*
$Log: /wxCamelot/Kernel/genv.cpp $
 * 
 * 3     20/07/05 15:39 Luke
 * Intermediate
 * 
 * 2     29/01/97 20:41 Mike
 * New files to hold the envelope class out of moldenv.h / .cpp.
 * 
 * 1     23/01/97 15:56 Mike
 * GEnvelope code taken out of moldenv.cpp

*/

#include "camtypes.h"

#include <math.h>
#include "moldshap.h"
#include "gmould.h"
#include "genv.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(GEnvelope, GMould)
CC_IMPLEMENT_DYNAMIC(GEnvelope2x2, GMould)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// GEnvelop.cpp
//
/////////////////////////////////////////////////////////////////////////////////////////////////

GEnvelope::GEnvelope()
{
	SrcBBox.left   = 0;
	SrcBBox.top    = 3;
	SrcBBox.right  = 3;
	SrcBBox.bottom = 0;

	L0.x = 0 ; L0.y = 0 ;
	L1.x = 0 ; L1.y = 1 ;
	L2.x = 0 ; L2.y = 2 ;
	T0.x = 0 ; T0.y = 3 ;
	T1.x = 1 ; T1.y = 3 ;
	T2.x = 2 ; T2.y = 3 ;
	R0.x = 3 ; R0.y = 3 ;
	R1.x = 3 ; R1.y = 2 ;
	R2.x = 3 ; R2.y = 1 ;
	B0.x = 3 ; B0.y = 0 ;
	B1.x = 2 ; B1.y = 0 ;
	B2.x = 1 ; B2.y = 0 ;

	MinX = 0; MinY = 0;
	MaxX = 3; MaxY = 3;
	Width = 3; Depth = 3;

	ScaleView();
	CalcViewConsts();
}
	

BOOL GEnvelope::Redefine( POINT *P )
{
	L0.x = P[ 0].x ; L0.y = P[ 0].y ;
	L1.x = P[ 1].x ; L1.y = P[ 1].y ;
	L2.x = P[ 2].x ; L2.y = P[ 2].y ;
	T0.x = P[ 3].x ; T0.y = P[ 3].y ;
	T1.x = P[ 4].x ; T1.y = P[ 4].y ;
	T2.x = P[ 5].x ; T2.y = P[ 5].y ;
	R0.x = P[ 6].x ; R0.y = P[ 6].y ;
	R1.x = P[ 7].x ; R1.y = P[ 7].y ;
	R2.x = P[ 8].x ; R2.y = P[ 8].y ;
	B0.x = P[ 9].x ; B0.y = P[ 9].y ;
	B1.x = P[10].x ; B1.y = P[10].y ;
	B2.x = P[11].x ; B2.y = P[11].y ;

	CalcViewBBox();
	if (Width==0 || Depth==0) 
		return FALSE;

	ScaleView();
	CalcViewConsts();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void GEnvelope::CalcViewBBox()
{
	MinX = MaxX = L0.x ;
	MinY = MaxY = L0.y ;
	MinX=min(MinX, INT32(L1.x)) ; MaxX=max(MaxX, INT32(L1.x)) ; MinY=min(MinY, INT32(L1.y)) ; MaxY=max(MaxY, INT32(L1.y)) ;
	MinX=min(MinX, INT32(L2.x)) ; MaxX=max(MaxX, INT32(L2.x)) ; MinY=min(MinY, INT32(L2.y)) ; MaxY=max(MaxY, INT32(L2.y)) ;
	MinX=min(MinX, INT32(T0.x)) ; MaxX=max(MaxX, INT32(T0.x)) ; MinY=min(MinY, INT32(T0.y)) ; MaxY=max(MaxY, INT32(T0.y)) ;
	MinX=min(MinX, INT32(T1.x)) ; MaxX=max(MaxX, INT32(T1.x)) ; MinY=min(MinY, INT32(T1.y)) ; MaxY=max(MaxY, INT32(T1.y)) ;
	MinX=min(MinX, INT32(T2.x)) ; MaxX=max(MaxX, INT32(T2.x)) ; MinY=min(MinY, INT32(T2.y)) ; MaxY=max(MaxY, INT32(T2.y)) ;
	MinX=min(MinX, INT32(R0.x)) ; MaxX=max(MaxX, INT32(R0.x)) ; MinY=min(MinY, INT32(R0.y)) ; MaxY=max(MaxY, INT32(R0.y)) ;
	MinX=min(MinX, INT32(R1.x)) ; MaxX=max(MaxX, INT32(R1.x)) ; MinY=min(MinY, INT32(R1.y)) ; MaxY=max(MaxY, INT32(R1.y)) ;
	MinX=min(MinX, INT32(R2.x)) ; MaxX=max(MaxX, INT32(R2.x)) ; MinY=min(MinY, INT32(R2.y)) ; MaxY=max(MaxY, INT32(R2.y)) ;
	MinX=min(MinX, INT32(B0.x)) ; MaxX=max(MaxX, INT32(B0.x)) ; MinY=min(MinY, INT32(B0.y)) ; MaxY=max(MaxY, INT32(B0.y)) ;
	MinX=min(MinX, INT32(B1.x)) ; MaxX=max(MaxX, INT32(B1.x)) ; MinY=min(MinY, INT32(B1.y)) ; MaxY=max(MaxY, INT32(B1.y)) ;
	MinX=min(MinX, INT32(B2.x)) ; MaxX=max(MaxX, INT32(B2.x)) ; MinY=min(MinY, INT32(B2.y)) ; MaxY=max(MaxY, INT32(B2.y)) ;
	Width = MaxX-MinX ;
	Depth = MaxY-MinY ;
}


void GEnvelope::CopyShape(POINT* P)
{
	P[ 0].x = L0.x ; P[ 0].y = L0.y ; 
	P[ 1].x = L1.x ; P[ 1].y = L1.y ;
	P[ 2].x = L2.x ; P[ 2].y = L2.y ;
	P[ 3].x = T0.x ; P[ 3].y = T0.y ;
	P[ 4].x = T1.x ; P[ 4].y = T1.y ;
	P[ 5].x = T2.x ; P[ 5].y = T2.y ;
	P[ 6].x = R0.x ; P[ 6].y = R0.y ;
	P[ 7].x = R1.x ; P[ 7].y = R1.y ;
	P[ 8].x = R2.x ; P[ 8].y = R2.y ;
	P[ 9].x = B0.x ; P[ 9].y = B0.y ;
	P[10].x = B1.x ; P[10].y = B1.y ;
	P[11].x = B2.x ; P[11].y = B2.y ;
}
	

void GEnvelope::ScaleView()
{
	VL0.x = (double)(L0.x-MinX)/Width ;	VL0.y = (double)(L0.y-MinY)/Depth ;
	VL1.x = (double)(L1.x-MinX)/Width ;	VL1.y = (double)(L1.y-MinY)/Depth ;
	VL2.x = (double)(L2.x-MinX)/Width ;	VL2.y = (double)(L2.y-MinY)/Depth ;
	VT0.x = (double)(T0.x-MinX)/Width ;	VT0.y = (double)(T0.y-MinY)/Depth ;
	VT1.x = (double)(T1.x-MinX)/Width ;	VT1.y = (double)(T1.y-MinY)/Depth ;
	VT2.x = (double)(T2.x-MinX)/Width ;	VT2.y = (double)(T2.y-MinY)/Depth ;
	VR0.x = (double)(R0.x-MinX)/Width ;	VR0.y = (double)(R0.y-MinY)/Depth ;
	VR1.x = (double)(R1.x-MinX)/Width ;	VR1.y = (double)(R1.y-MinY)/Depth ;
	VR2.x = (double)(R2.x-MinX)/Width ;	VR2.y = (double)(R2.y-MinY)/Depth ;
	VB0.x = (double)(B0.x-MinX)/Width ;	VB0.y = (double)(B0.y-MinY)/Depth ;
	VB1.x = (double)(B1.x-MinX)/Width ;	VB1.y = (double)(B1.y-MinY)/Depth ;
	VB2.x = (double)(B2.x-MinX)/Width ;	VB2.y = (double)(B2.y-MinY)/Depth ;
}

void GEnvelope::CalcViewConsts()
{
	A.x = VL0.x ;
	A.y = VL0.y ;
	B.x = 3*(VB2.x-VL0.x) ;
	B.y = 3*(VB2.y-VL0.y) ;
	C.x = 3*(VL1.x-VL0.x) ;
	C.y = 3*(VL1.y-VL0.y) ;
	D.x = 3*(VT1.x+VR2.x)-2*(VT0.x+VB0.x)-VL0.x-VR0.x-B.x-C.x ;
	D.y = 3*(VT1.y+VR2.y)-2*(VT0.y+VB0.y)-VL0.y-VR0.y-B.y-C.y ;
	E.x = 3*(VL0.x-2*VB2.x+VB1.x) ;
	E.y = 3*(VL0.y-2*VB2.y+VB1.y) ;
	F.x = 3*(VL2.x-2*VL1.x+VL0.x) ;
	F.y = 3*(VL2.y-2*VL1.y+VL0.y) ;
	G.x = 3*(VT2.x-2*VT1.x+VT0.x)-E.x ;
	G.y = 3*(VT2.y-2*VT1.y+VT0.y)-E.y ;
	H.x = 3*(VB0.x-2*VR2.x+VR1.x)-F.x ;
	H.y = 3*(VB0.y-2*VR2.y+VR1.y)-F.y ;
	J.x = VB0.x-3*(VB1.x-VB2.x)-VL0.x ;
	J.y = VB0.y-3*(VB1.y-VB2.y)-VL0.y ;
	K.x = VT0.x-3*(VL2.x-VL1.x)-VL0.x ;
	K.y = VT0.y-3*(VL2.y-VL1.y)-VL0.y ;
	L.x = VR0.x-3*(VT2.x-VT1.x)-VT0.x-J.x ;
	L.y = VR0.y-3*(VT2.y-VT1.y)-VT0.y-J.y ;
	M.x = VR0.x-3*(VR1.x-VR2.x)-VB0.x-K.x ;
	M.y = VR0.y-3*(VR1.y-VR2.y)-VB0.y-K.y ;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

BOOL GEnvelope::FitPath (
						CONST POINT* IPoints,
						CONST BYTE*  ITypes,
						DWORD ILength,
						LPPOINT pOPoints,
						LPBYTE  pOTypes,
						DWORD pOLength,
						BOOL Close
					)
{
	OPoints = pOPoints ;
	OTypes  = pOTypes  ;
	OLength = pOLength ;

	DPOINT     C2, C4 ;
	DPOINT P0, P2, P4, P6, SP ;
	DPOINT T0, T2, T4, T6, ST ;
	ST.x = ST.y = SP.x = SP.y =0;
	while ( ILength )
	{
		switch ( *ITypes & PT_MOVETO )
		{
		case PT_MOVETO :
			--ILength ;
			ScaleSrc ( *IPoints++, P0 ) ; SP.x = P0.x ; SP.y = P0.y ;
			Transform( P0, T0 ) ;		  ST.x = T0.x ; ST.y = T0.y ;
			if ( !GenOp( *ITypes++, T0 ) ) return -1 ;
			break ;
		case PT_LINETO :
			--ILength ;	++ITypes ;
			ScaleSrc( *IPoints++, P6 ) ;
			P2.x = (2*P0.x+P6.x)/3 ;
			P2.y = (2*P0.y+P6.y)/3 ;
			P4.x = (P0.x+2*P6.x)/3 ;
			P4.y = (P0.y+2*P6.y)/3 ;
			Transform( P2, T2 ) ;
			Transform( P4, T4 ) ;
			Transform( P6, T6 ) ;
			if ( !GenCurve( P2, P4, P0, P2, P4, P6, T0, T2, T4, T6 ) ) return -1 ;
			P0.x = P6.x ; P0.y = P6.y ;
			T0.x = T6.x ; T0.y = T6.y ;
			break ;
		case PT_BEZIERTO :
			ILength -= 3 ; ITypes += 3 ;
			ScaleSrc( *IPoints++, C2 ) ;
			ScaleSrc( *IPoints++, C4 ) ;
			ScaleSrc( *IPoints++, P6 ) ;
			P2.x = (8*P0.x+12*C2.x+ 6*C4.x+  P6.x)/27 ;
			P2.y = (8*P0.y+12*C2.y+ 6*C4.y+  P6.y)/27 ;
			P4.x = (  P0.x+ 6*C2.x+12*C4.x+8*P6.x)/27 ;
			P4.y = (  P0.y+ 6*C2.y+12*C4.y+8*P6.y)/27 ;
			Transform( P2, T2 ) ;
			Transform( P4, T4 ) ;
			Transform( P6, T6 ) ;
			if ( !GenCurve( C2, C4, P0, P2, P4, P6, T0, T2, T4, T6 ) ) return -1 ;
			P0.x = P6.x ; P0.y = P6.y ;
			T0.x = T6.x ; T0.y = T6.y ;
			break ;
		default :
			return -1 ;
		}

		if ( *(ITypes-1) & PT_CLOSEFIGURE ||
				 			( Close && ( ILength==0 || (*ITypes & PT_MOVETO) == PT_MOVETO ) ) )
		{
			P2.x = (2*P0.x+SP.x)/3 ;
			P2.y = (2*P0.y+SP.y)/3 ;
			P4.x = (P0.x+2*SP.x)/3 ;
			P4.y = (P0.y+2*SP.y)/3 ;
			Transform( P2, T2 ) ;
			Transform( P4, T4 ) ;
			if ( !GenCurve( P2, P4, P0, P2, P4, SP, T0, T2, T4, ST ) ) return -1 ;
			*(OTypes-1) |= *(ITypes-1) & PT_CLOSEFIGURE ;
		}
	}

	if ( ILength )
		return -1 ;

	return pOLength-OLength ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void GEnvelope::Transform( DPOINT a, DPOINT& b )
{
	b.x = A.x+a.x*(B.x+a.x*(E.x+a.x*J.x))
			 +a.y*(C.x+a.y*(F.x+a.y*K.x)
			 +a.x*(D.x+a.x*(G.x+a.x*L.x)
			  		  +a.y*(H.x+a.y*M.x))) ;
	b.y = A.y+a.x*(B.y+a.x*(E.y+a.x*J.y))
			 +a.y*(C.y+a.y*(F.y+a.y*K.y)
			 +a.x*(D.y+a.x*(G.y+a.x*L.y)
			  		  +a.y*(H.y+a.y*M.y))) ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	T0 T1 T2 T3 T4 T5 R0
//	L5	     V5       R1
//	L4	(10) V4 (11)  R2
//	L3 H1 H2 MM H4 H5 R3
//	L2	(00) V3 (01)  R4
//	L1	     V4       R5
//	L0 B5 B4 B3 B2 B1 B0
//


GEnvelope2x2::GEnvelope2x2()
{
	L0.x = 0 ; L0.y = 0 ;
	L1.x = 0 ; L1.y = 1 ;
	L2.x = 0 ; L2.y = 2 ;
	L3.x = 0 ; L3.y = 3 ;
	L4.x = 0 ; L4.y = 4 ;
	L5.x = 0 ; L5.y = 5 ;

	T0.x = 0 ; T0.y = 6 ;
	T1.x = 1 ; T1.y = 6 ;
	T2.x = 2 ; T2.y = 6 ;
	T3.x = 3 ; T3.y = 6 ;
	T4.x = 4 ; T4.y = 6 ;
	T5.x = 5 ; T5.y = 6 ;

	R0.x = 6 ; R0.y = 6 ;
	R1.x = 6 ; R1.y = 5 ;
	R2.x = 6 ; R2.y = 4 ;
	R3.x = 6 ; R3.y = 3 ;
	R4.x = 6 ; R4.y = 2 ;
	R5.x = 6 ; R5.y = 1 ;

	B0.x = 6 ; B0.y = 0 ;
	B1.x = 5 ; B1.y = 0 ;
	B2.x = 4 ; B2.y = 0 ;
	B3.x = 3 ; B3.y = 0 ;
	B4.x = 2 ; B4.y = 0 ;
	B5.x = 1 ; B5.y = 0 ;

	CalcConsts();
}


BOOL GEnvelope2x2::Redefine( POINT *P )
{
	L0.x = P[ 0].x ; L0.y = P[ 0].y ;
	L1.x = P[ 1].x ; L1.y = P[ 1].y ;
	L2.x = P[ 2].x ; L2.y = P[ 2].y ;
	L3.x = P[ 3].x ; L3.y = P[ 3].y ;
	L4.x = P[ 4].x ; L4.y = P[ 4].y ;
	L5.x = P[ 5].x ; L5.y = P[ 5].y ;
	T0.x = P[ 6].x ; T0.y = P[ 6].y ;
	T1.x = P[ 7].x ; T1.y = P[ 7].y ;
	T2.x = P[ 8].x ; T2.y = P[ 8].y ;
	T3.x = P[ 9].x ; T3.y = P[ 9].y ;
	T4.x = P[10].x ; T4.y = P[10].y ;
	T5.x = P[11].x ; T5.y = P[11].y ;
	R0.x = P[12].x ; R0.y = P[12].y ;
	R1.x = P[13].x ; R1.y = P[13].y ;
	R2.x = P[14].x ; R2.y = P[14].y ;
	R3.x = P[15].x ; R3.y = P[15].y ;
	R4.x = P[16].x ; R4.y = P[16].y ;
	R5.x = P[17].x ; R5.y = P[17].y ;
	B0.x = P[18].x ; B0.y = P[18].y ;
	B1.x = P[19].x ; B1.y = P[19].y ;
	B2.x = P[20].x ; B2.y = P[20].y ;
	B3.x = P[21].x ; B3.y = P[21].y ;
	B4.x = P[22].x ; B4.y = P[22].y ;
	B5.x = P[23].x ; B5.y = P[23].y ;

	return CalcConsts();
}


BOOL GEnvelope2x2::CalcConsts()
{
	MM.x = (-  B0.x +  2*B3.x -   L0.x +  2*L3.x -   R0.x +  2*R3.x -   T0.x +  2*T3.x)/ 4 ;
	MM.y = (-  B0.y +  2*B3.y -   L0.y +  2*L3.y -   R0.y +  2*R3.y -   T0.y +  2*T3.y)/ 4 ;
	H1.x = (-  B0.x +  6*B5.x - 5*L0.x + 10*L3.x -   R0.x +  2*R3.x - 5*T0.x +  6*T1.x)/12 ;
	H1.y = (-  B0.y +  6*B5.y - 5*L0.y + 10*L3.y -   R0.y +  2*R3.y - 5*T0.y +  6*T1.y)/12 ;
	H2.x = (-  B0.x +  3*B4.x - 2*L0.x +  4*L3.x -   R0.x +  2*R3.x - 2*T0.x +  3*T2.x)/ 6 ;
	H2.y = (-  B0.y +  3*B4.y - 2*L0.y +  4*L3.y -   R0.y +  2*R3.y - 2*T0.y +  3*T2.y)/ 6 ;
	H4.x = (-2*B0.x +  3*B2.x -   L0.x +  2*L3.x - 2*R0.x +  4*R3.x -   T0.x +  3*T4.x)/ 6 ;
	H4.y = (-2*B0.y +  3*B2.y -   L0.y +  2*L3.y - 2*R0.y +  4*R3.y -   T0.y +  3*T4.y)/ 6 ;
	H5.x = (-5*B0.x +  6*B1.x -   L0.x +  2*L3.x - 5*R0.x + 10*R3.x -   T0.x +  6*T5.x)/12 ;
	H5.y = (-5*B0.y +  6*B1.y -   L0.y +  2*L3.y - 5*R0.y + 10*R3.y -   T0.y +  6*T5.y)/12 ;
	V1.x = (-5*B0.x + 10*B3.x - 5*L0.x +  6*L1.x -   R0.x +  6*R5.x -   T0.x +  2*T3.x)/12 ;
	V1.y = (-5*B0.y + 10*B3.y - 5*L0.y +  6*L1.y -   R0.y +  6*R5.y -   T0.y +  2*T3.y)/12 ;
	V2.x = (-2*B0.x +  4*B3.x - 2*L0.x +  3*L2.x -   R0.x +  3*R4.x -   T0.x +  2*T3.x)/ 6 ;
	V2.y = (-2*B0.y +  4*B3.y - 2*L0.y +  3*L2.y -   R0.y +  3*R4.y -   T0.y +  2*T3.y)/ 6 ;
	V4.x = (-  B0.x +  2*B3.x -   L0.x +  3*L4.x - 2*R0.x +  3*R2.x - 2*T0.x +  4*T3.x)/ 6 ;
	V4.y = (-  B0.y +  2*B3.y -   L0.y +  3*L4.y - 2*R0.y +  3*R2.y - 2*T0.y +  4*T3.y)/ 6 ;
	V5.x = (-  B0.x +  2*B3.x -   L0.x +  6*L5.x - 5*R0.x +  6*R1.x - 5*T0.x + 10*T3.x)/12 ;
	V5.y = (-  B0.y +  2*B3.y -   L0.y +  6*L5.y - 5*R0.y +  6*R1.y - 5*T0.y + 10*T3.y)/12 ;
	SmoothX = T4.x-T3.x == T3.x-T2.x && T4.y-T3.y == T3.y-T2.y &&
			  B4.x-B3.x == B3.x-B2.x && B4.y-B3.y == B3.y-B2.y ;
	SmoothY = L4.x-L3.x == L3.x-L2.x && L4.y-L3.y == L3.y-L2.y &&
			  R4.x-R3.x == R3.x-R2.x && R4.y-R3.y == R3.y-R2.y ;

	CalcViewBBox() ;
	if (Width==0 || Depth==0)
		return FALSE;
	
	ScaleView() ;
	CalcViewConsts() ;

	return TRUE;
}



void GEnvelope2x2::CopyShape(POINT* P)
{
	P[ 0].x = L0.x ; P[ 0].y = L0.y ; 
	P[ 1].x = L1.x ; P[ 1].y = L1.y ;
	P[ 2].x = L2.x ; P[ 2].y = L2.y ;
	P[ 3].x = L3.x ; P[ 3].y = L3.y ; 
	P[ 4].x = L4.x ; P[ 4].y = L4.y ;
	P[ 5].x = L5.x ; P[ 5].y = L5.y ;
	P[ 6].x = T0.x ; P[ 6].y = T0.y ; 
	P[ 7].x = T1.x ; P[ 7].y = T1.y ;
	P[ 8].x = T2.x ; P[ 8].y = T2.y ;
	P[ 9].x = T3.x ; P[ 9].y = T3.y ; 
	P[10].x = T4.x ; P[10].y = T4.y ;
	P[11].x = T5.x ; P[11].y = T5.y ;
	P[12].x = R0.x ; P[12].y = R0.y ; 
	P[13].x = R1.x ; P[13].y = R1.y ;
	P[14].x = R2.x ; P[14].y = R2.y ;
	P[15].x = R3.x ; P[15].y = R3.y ; 
	P[16].x = R4.x ; P[16].y = R4.y ;
	P[17].x = R5.x ; P[17].y = R5.y ;
	P[18].x = B0.x ; P[18].y = B0.y ; 
	P[19].x = B1.x ; P[19].y = B1.y ;
	P[20].x = B2.x ; P[20].y = B2.y ;
	P[21].x = B3.x ; P[21].y = B3.y ; 
	P[22].x = B4.x ; P[22].y = B4.y ;
	P[23].x = B5.x ; P[23].y = B5.y ;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

void GEnvelope2x2::CalcViewBBox()
{
	MinX = MaxX = L0.x ;
	MinY = MaxY = L0.y ;
	MinX=min(MinX, INT32(L1.x)) ; MaxX=max(MaxX, INT32(L1.x)) ; MinY=min(MinY, INT32(L1.y)) ; MaxY=max(MaxY, INT32(L1.y)) ;
	MinX=min(MinX, INT32(L2.x)) ; MaxX=max(MaxX, INT32(L2.x)) ; MinY=min(MinY, INT32(L2.y)) ; MaxY=max(MaxY, INT32(L2.y)) ;
	MinX=min(MinX, INT32(L3.x)) ; MaxX=max(MaxX, INT32(L3.x)) ; MinY=min(MinY, INT32(L3.y)) ; MaxY=max(MaxY, INT32(L3.y)) ;
	MinX=min(MinX, INT32(L4.x)) ; MaxX=max(MaxX, INT32(L4.x)) ; MinY=min(MinY, INT32(L4.y)) ; MaxY=max(MaxY, INT32(L4.y)) ;
	MinX=min(MinX, INT32(L5.x)) ; MaxX=max(MaxX, INT32(L5.x)) ; MinY=min(MinY, INT32(L5.y)) ; MaxY=max(MaxY, INT32(L5.y)) ;
	MinX=min(MinX, INT32(T0.x)) ; MaxX=max(MaxX, INT32(T0.x)) ; MinY=min(MinY, INT32(T0.y)) ; MaxY=max(MaxY, INT32(T0.y)) ;
	MinX=min(MinX, INT32(T1.x)) ; MaxX=max(MaxX, INT32(T1.x)) ; MinY=min(MinY, INT32(T1.y)) ; MaxY=max(MaxY, INT32(T1.y)) ;
	MinX=min(MinX, INT32(T2.x)) ; MaxX=max(MaxX, INT32(T2.x)) ; MinY=min(MinY, INT32(T2.y)) ; MaxY=max(MaxY, INT32(T2.y)) ;
	MinX=min(MinX, INT32(T3.x)) ; MaxX=max(MaxX, INT32(T3.x)) ; MinY=min(MinY, INT32(T3.y)) ; MaxY=max(MaxY, INT32(T3.y)) ;
	MinX=min(MinX, INT32(T4.x)) ; MaxX=max(MaxX, INT32(T4.x)) ; MinY=min(MinY, INT32(T4.y)) ; MaxY=max(MaxY, INT32(T4.y)) ;
	MinX=min(MinX, INT32(T5.x)) ; MaxX=max(MaxX, INT32(T5.x)) ; MinY=min(MinY, INT32(T5.y)) ; MaxY=max(MaxY, INT32(T5.y)) ;
	MinX=min(MinX, INT32(R0.x)) ; MaxX=max(MaxX, INT32(R0.x)) ; MinY=min(MinY, INT32(R0.y)) ; MaxY=max(MaxY, INT32(R0.y)) ;
	MinX=min(MinX, INT32(R1.x)) ; MaxX=max(MaxX, INT32(R1.x)) ; MinY=min(MinY, INT32(R1.y)) ; MaxY=max(MaxY, INT32(R1.y)) ;
	MinX=min(MinX, INT32(R2.x)) ; MaxX=max(MaxX, INT32(R2.x)) ; MinY=min(MinY, INT32(R2.y)) ; MaxY=max(MaxY, INT32(R2.y)) ;
	MinX=min(MinX, INT32(R3.x)) ; MaxX=max(MaxX, INT32(R3.x)) ; MinY=min(MinY, INT32(R3.y)) ; MaxY=max(MaxY, INT32(R3.y)) ;
	MinX=min(MinX, INT32(R4.x)) ; MaxX=max(MaxX, INT32(R4.x)) ; MinY=min(MinY, INT32(R4.y)) ; MaxY=max(MaxY, INT32(R4.y)) ;
	MinX=min(MinX, INT32(R5.x)) ; MaxX=max(MaxX, INT32(R5.x)) ; MinY=min(MinY, INT32(R5.y)) ; MaxY=max(MaxY, INT32(R5.y)) ;
	MinX=min(MinX, INT32(B0.x)) ; MaxX=max(MaxX, INT32(B0.x)) ; MinY=min(MinY, INT32(B0.y)) ; MaxY=max(MaxY, INT32(B0.y)) ;
	MinX=min(MinX, INT32(B1.x)) ; MaxX=max(MaxX, INT32(B1.x)) ; MinY=min(MinY, INT32(B1.y)) ; MaxY=max(MaxY, INT32(B1.y)) ;
	MinX=min(MinX, INT32(B2.x)) ; MaxX=max(MaxX, INT32(B2.x)) ; MinY=min(MinY, INT32(B2.y)) ; MaxY=max(MaxY, INT32(B2.y)) ;
	MinX=min(MinX, INT32(B3.x)) ; MaxX=max(MaxX, INT32(B3.x)) ; MinY=min(MinY, INT32(B3.y)) ; MaxY=max(MaxY, INT32(B3.y)) ;
	MinX=min(MinX, INT32(B4.x)) ; MaxX=max(MaxX, INT32(B4.x)) ; MinY=min(MinY, INT32(B4.y)) ; MaxY=max(MaxY, INT32(B4.y)) ;
	MinX=min(MinX, INT32(B5.x)) ; MaxX=max(MaxX, INT32(B5.x)) ; MinY=min(MinY, INT32(B5.y)) ; MaxY=max(MaxY, INT32(B5.y)) ;
	Width = MaxX-MinX ;
	Depth = MaxY-MinY ;
}



void GEnvelope2x2::ScaleView()
{
	VL0.x = (double)(L0.x-MinX)/Width ;	VL0.y = (double)(L0.y-MinY)/Depth ;
	VL1.x = (double)(L1.x-MinX)/Width ;	VL1.y = (double)(L1.y-MinY)/Depth ;
	VL2.x = (double)(L2.x-MinX)/Width ;	VL2.y = (double)(L2.y-MinY)/Depth ;
	VL3.x = (double)(L3.x-MinX)/Width ;	VL3.y = (double)(L3.y-MinY)/Depth ;
	VL4.x = (double)(L4.x-MinX)/Width ;	VL4.y = (double)(L4.y-MinY)/Depth ;
	VL5.x = (double)(L5.x-MinX)/Width ;	VL5.y = (double)(L5.y-MinY)/Depth ;
	VT0.x = (double)(T0.x-MinX)/Width ;	VT0.y = (double)(T0.y-MinY)/Depth ;
	VT1.x = (double)(T1.x-MinX)/Width ;	VT1.y = (double)(T1.y-MinY)/Depth ;
	VT2.x = (double)(T2.x-MinX)/Width ;	VT2.y = (double)(T2.y-MinY)/Depth ;
	VT3.x = (double)(T3.x-MinX)/Width ;	VT3.y = (double)(T3.y-MinY)/Depth ;
	VT4.x = (double)(T4.x-MinX)/Width ;	VT4.y = (double)(T4.y-MinY)/Depth ;
	VT5.x = (double)(T5.x-MinX)/Width ;	VT5.y = (double)(T5.y-MinY)/Depth ;
	VR0.x = (double)(R0.x-MinX)/Width ;	VR0.y = (double)(R0.y-MinY)/Depth ;
	VR1.x = (double)(R1.x-MinX)/Width ;	VR1.y = (double)(R1.y-MinY)/Depth ;
	VR2.x = (double)(R2.x-MinX)/Width ;	VR2.y = (double)(R2.y-MinY)/Depth ;
	VR3.x = (double)(R3.x-MinX)/Width ;	VR3.y = (double)(R3.y-MinY)/Depth ;
	VR4.x = (double)(R4.x-MinX)/Width ;	VR4.y = (double)(R4.y-MinY)/Depth ;
	VR5.x = (double)(R5.x-MinX)/Width ;	VR5.y = (double)(R5.y-MinY)/Depth ;
	VB0.x = (double)(B0.x-MinX)/Width ;	VB0.y = (double)(B0.y-MinY)/Depth ;
	VB1.x = (double)(B1.x-MinX)/Width ;	VB1.y = (double)(B1.y-MinY)/Depth ;
	VB2.x = (double)(B2.x-MinX)/Width ;	VB2.y = (double)(B2.y-MinY)/Depth ;
	VB3.x = (double)(B3.x-MinX)/Width ;	VB3.y = (double)(B3.y-MinY)/Depth ;
	VB4.x = (double)(B4.x-MinX)/Width ;	VB4.y = (double)(B4.y-MinY)/Depth ;
	VB5.x = (double)(B5.x-MinX)/Width ;	VB5.y = (double)(B5.y-MinY)/Depth ;
	VMM.x = (double)(MM.x-MinX)/Width ;	VMM.y = (double)(MM.y-MinY)/Depth ;
	VH1.x = (double)(H1.x-MinX)/Width ;	VH1.y = (double)(H1.y-MinY)/Depth ;
	VH2.x = (double)(H2.x-MinX)/Width ;	VH2.y = (double)(H2.y-MinY)/Depth ;
	VH4.x = (double)(H4.x-MinX)/Width ;	VH4.y = (double)(H4.y-MinY)/Depth ;
	VH5.x = (double)(H5.x-MinX)/Width ;	VH5.y = (double)(H5.y-MinY)/Depth ;
	VV1.x = (double)(V1.x-MinX)/Width ;	VV1.y = (double)(V1.y-MinY)/Depth ;
	VV2.x = (double)(V2.x-MinX)/Width ;	VV2.y = (double)(V2.y-MinY)/Depth ;
	VV4.x = (double)(V4.x-MinX)/Width ;	VV4.y = (double)(V4.y-MinY)/Depth ;
	VV5.x = (double)(V5.x-MinX)/Width ;	VV5.y = (double)(V5.y-MinY)/Depth ;
}



void GEnvelope2x2::CalcViewConsts()
{
	A00.x = VL0.x ;
	A00.y = VL0.y ;
	B00.x = 3*(VB5.x-VL0.x) ;
	B00.y = 3*(VB5.y-VL0.y) ;
	C00.x = 3*(VL1.x-VL0.x) ;
	C00.y = 3*(VL1.y-VL0.y) ;
	D00.x = 3*(VH1.x+VV1.x)-2*(VL3.x+VB3.x)-VL0.x-VMM.x-B00.x-C00.x ;
	D00.y = 3*(VH1.y+VV1.y)-2*(VL3.y+VB3.y)-VL0.y-VMM.y-B00.y-C00.y ;
	E00.x = 3*(VL0.x-2*VB5.x+VB4.x) ;
	E00.y = 3*(VL0.y-2*VB5.y+VB4.y) ;
	F00.x = 3*(VL2.x-2*VL1.x+VL0.x) ;
	F00.y = 3*(VL2.y-2*VL1.y+VL0.y) ;
	G00.x = 3*(VH2.x-2*VH1.x+VL3.x)-E00.x ;
	G00.y = 3*(VH2.y-2*VH1.y+VL3.y)-E00.y ;
	H00.x = 3*(VB3.x-2*VV1.x+VV2.x)-F00.x ;
	H00.y = 3*(VB3.y-2*VV1.y+VV2.y)-F00.y ;
	J00.x = VB3.x-3*(VB4.x-VB5.x)-VL0.x ;
	J00.y = VB3.y-3*(VB4.y-VB5.y)-VL0.y ;
	K00.x = VL3.x-3*(VL2.x-VL1.x)-VL0.x ;
	K00.y = VL3.y-3*(VL2.y-VL1.y)-VL0.y ;
	L00.x = VMM.x-3*(VH2.x-VH1.x)-VL3.x-J00.x ;
	L00.y = VMM.y-3*(VH2.y-VH1.y)-VL3.y-J00.y ;
	M00.x = VMM.x-3*(VV2.x-VV1.x)-VB3.x-K00.x ;
	M00.y = VMM.y-3*(VV2.y-VV1.y)-VB3.y-K00.y ;

	A01.x = VB3.x ;
	A01.y = VB3.y ;
	B01.x = 3*(VB2.x-VB3.x) ;
	B01.y = 3*(VB2.y-VB3.y) ;
	C01.x = 3*(VV1.x-VB3.x) ;
	C01.y = 3*(VV1.y-VB3.y) ;
	D01.x = 3*(VH4.x+VR5.x)-2*(VMM.x+VB0.x)-VB3.x-VR3.x-B01.x-C01.x ;
	D01.y = 3*(VH4.y+VR5.y)-2*(VMM.y+VB0.y)-VB3.y-VR3.y-B01.y-C01.y ;
	E01.x = 3*(VB3.x-2*VB2.x+VB1.x) ;
	E01.y = 3*(VB3.y-2*VB2.y+VB1.y) ;
	F01.x = 3*(VV2.x-2*VV1.x+VB3.x) ;
	F01.y = 3*(VV2.y-2*VV1.y+VB3.y) ;
	G01.x = 3*(VH5.x-2*VH4.x+VMM.x)-E01.x ;
	G01.y = 3*(VH5.y-2*VH4.y+VMM.y)-E01.y ;
	H01.x = 3*(VB0.x-2*VR5.x+VR4.x)-F01.x ;
	H01.y = 3*(VB0.y-2*VR5.y+VR4.y)-F01.y ;
	J01.x = VB0.x-3*(VB1.x-VB2.x)-VB3.x ;
	J01.y = VB0.y-3*(VB1.y-VB2.y)-VB3.y ;
	K01.x = VMM.x-3*(VV2.x-VV1.x)-VB3.x ;
	K01.y = VMM.y-3*(VV2.y-VV1.y)-VB3.y ;
	L01.x = VR3.x-3*(VH5.x-VH4.x)-VMM.x-J01.x ;
	L01.y = VR3.y-3*(VH5.y-VH4.y)-VMM.y-J01.y ;
	M01.x = VR3.x-3*(VR4.x-VR5.x)-VB0.x-K01.x ;
	M01.y = VR3.y-3*(VR4.y-VR5.y)-VB0.y-K01.y ;

	A10.x = VL3.x ;
	A10.y = VL3.y ;
	B10.x = 3*(VH1.x-VL3.x) ;
	B10.y = 3*(VH1.y-VL3.y) ;
	C10.x = 3*(VL4.x-VL3.x) ;
	C10.y = 3*(VL4.y-VL3.y) ;
	D10.x = 3*(VT1.x+VV4.x)-2*(VT0.x+VMM.x)-VL3.x-VT3.x-B10.x-C10.x ;
	D10.y = 3*(VT1.y+VV4.y)-2*(VT0.y+VMM.y)-VL3.y-VT3.y-B10.y-C10.y ;
	E10.x = 3*(VL3.x-2*VH1.x+VH2.x) ;
	E10.y = 3*(VL3.y-2*VH1.y+VH2.y) ;
	F10.x = 3*(VL5.x-2*VL4.x+VL3.x) ;
	F10.y = 3*(VL5.y-2*VL4.y+VL3.y) ;
	G10.x = 3*(VT2.x-2*VT1.x+VT0.x)-E10.x ;
	G10.y = 3*(VT2.y-2*VT1.y+VT0.y)-E10.y ;
	H10.x = 3*(VMM.x-2*VV4.x+VV5.x)-F10.x ;
	H10.y = 3*(VMM.y-2*VV4.y+VV5.y)-F10.y ;
	J10.x = VMM.x-3*(VH2.x-VH1.x)-VL3.x ;
	J10.y = VMM.y-3*(VH2.y-VH1.y)-VL3.y ;
	K10.x = VT0.x-3*(VL5.x-VL4.x)-VL3.x ;
	K10.y = VT0.y-3*(VL5.y-VL4.y)-VL3.y ;
	L10.x = VT3.x-3*(VT2.x-VT1.x)-VT0.x-J10.x ;
	L10.y = VT3.y-3*(VT2.y-VT1.y)-VT0.y-J10.y ;
	M10.x = VT3.x-3*(VV5.x-VV4.x)-VMM.x-K10.x ;
	M10.y = VT3.y-3*(VV5.y-VV4.y)-VMM.y-K10.y ;

	A11.x = VMM.x ;
	A11.y = VMM.y ;
	B11.x = 3*(VH4.x-VMM.x) ;
	B11.y = 3*(VH4.y-VMM.y) ;
	C11.x = 3*(VV4.x-VMM.x) ;
	C11.y = 3*(VV4.y-VMM.y) ;
	D11.x = 3*(VT4.x+VR2.x)-2*(VT3.x+VR3.x)-VMM.x-VR0.x-B11.x-C11.x ;
	D11.y = 3*(VT4.y+VR2.y)-2*(VT3.y+VR3.y)-VMM.y-VR0.y-B11.y-C11.y ;
	E11.x = 3*(VMM.x-2*VH4.x+VH5.x) ;
	E11.y = 3*(VMM.y-2*VH4.y+VH5.y) ;
	F11.x = 3*(VV5.x-2*VV4.x+VMM.x) ;
	F11.y = 3*(VV5.y-2*VV4.y+VMM.y) ;
	G11.x = 3*(VT5.x-2*VT4.x+VT3.x)-E11.x ;
	G11.y = 3*(VT5.y-2*VT4.y+VT3.y)-E11.y ;
	H11.x = 3*(VR3.x-2*VR2.x+VR1.x)-F11.x ;
	H11.y = 3*(VR3.y-2*VR2.y+VR1.y)-F11.y ;
	J11.x = VR3.x-3*(VH5.x-VH4.x)-VMM.x ;
	J11.y = VR3.y-3*(VH5.y-VH4.y)-VMM.y ;
	K11.x = VT3.x-3*(VV5.x-VV4.x)-VMM.x ;
	K11.y = VT3.y-3*(VV5.y-VV4.y)-VMM.y ;
	L11.x = VR0.x-3*(VT5.x-VT4.x)-VT3.x-J11.x ;
	L11.y = VR0.y-3*(VT5.y-VT4.y)-VT3.y-J11.y ;
	M11.x = VR0.x-3*(VR1.x-VR2.x)-VR3.x-K11.x ;
	M11.y = VR0.y-3*(VR1.y-VR2.y)-VR3.y-K11.y ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

BOOL GEnvelope2x2::FitPath (
						CONST POINT* IPoints, 
						CONST BYTE* ITypes, 
						DWORD ILength,
						LPPOINT pOPoints, 
						LPBYTE pOTypes, 
						DWORD pOLength,
						BOOL Close
					)
{
	OPoints = pOPoints ;
	OTypes  = pOTypes  ;
	OLength = pOLength ;

	DPOINT     C2, C4 ;
	DPOINT P0, P2, P4, P6, SP;
	DPOINT T0, T2, T4, T6, ST;
	SP.x=SP.y=ST.x=ST.y=0; // silence compiler whinging
	while ( ILength )
	{
		switch ( *ITypes & PT_MOVETO )
		{
		case PT_MOVETO :
			--ILength ;	++ITypes ;
			ScaleSrc ( *IPoints++, P0 ) ; SP.x = P0.x ; SP.y = P0.y ;
			Transform( P0, T0 ) ;		  ST.x = T0.x ; ST.y = T0.y ;
			if ( !GenMove( T0 ) ) return -1 ;
			break ;
		case PT_LINETO :
			--ILength ;	++ITypes ;
			ScaleSrc( *IPoints++, P6 ) ;
			Transform( P6, T6 ) ;
			if ( !GenLine( P0,P6,T0,T6 ) ) return -1 ;
			P0.x = P6.x ; P0.y = P6.y ;
			T0.x = T6.x ; T0.y = T6.y ;
			break ;
		case PT_BEZIERTO :
			ILength -= 3 ; ITypes += 3 ;
			ScaleSrc( *IPoints++, C2 ) ;
			ScaleSrc( *IPoints++, C4 ) ;
			ScaleSrc( *IPoints++, P6 ) ;
			P2.x = (8*P0.x+12*C2.x+ 6*C4.x+  P6.x)/27 ;
			P2.y = (8*P0.y+12*C2.y+ 6*C4.y+  P6.y)/27 ;
			P4.x = (  P0.x+ 6*C2.x+12*C4.x+8*P6.x)/27 ;
			P4.y = (  P0.y+ 6*C2.y+12*C4.y+8*P6.y)/27 ;
			Transform( P2, T2 ) ;
			Transform( P4, T4 ) ;
			Transform( P6, T6 ) ;
			if ( !GenCurve( C2, C4, P0, P2, P4, P6, T0, T2, T4, T6 ) ) return -1 ;
			P0.x = P6.x ; P0.y = P6.y ;
			T0.x = T6.x ; T0.y = T6.y ;
			break ;
		default :
			return -1 ;
		}
		if ( *(ITypes-1) & PT_CLOSEFIGURE ||
				 			( Close && ( ILength==0 || (*ITypes & PT_MOVETO) == PT_MOVETO ) ) )
		{
			if ( !GenLine( P0,SP,T0,ST ) ) return -1 ;
			*(OTypes-1) |= *(ITypes-1) & PT_CLOSEFIGURE ;
		}
	}
	if ( ILength )
		return -1 ;
	return pOLength-OLength ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

BOOL GEnvelope2x2::GenLine( DPOINT P0, DPOINT P6, DPOINT T0, DPOINT T6 )
{
	BOOL F0 = !SmoothX && ((P0.x<0.5 && P6.x>0.5) || (P0.x>0.5 && P6.x<0.5)) ;
	BOOL F1 = !SmoothY && ((P0.y<0.5 && P6.y>0.5) || (P0.y>0.5 && P6.y<0.5)) ;
	if ( F0 || F1 )
	{
		DPOINT P1,P2,P3,P4,P5 ;
		DPOINT T1,T2,T3,T4,T5 ;
		P3.x=P3.y=0;
		if ( F0 )
		{
			P3.x = 0.5 ;
			P3.y = P0.y+(0.5-P0.x)*(P6.y-P0.y)/(P6.x-P0.x) ;
		}
		else if ( F1 )
		{
			P3.x = P0.x+(0.5-P0.y)*(P6.x-P0.x)/(P6.y-P0.y) ;
			P3.y = 0.5 ;
		}
		P1.x = (2*P0.x+P3.x)/3 ;
		P1.y = (2*P0.y+P3.y)/3 ;
		P2.x = (P0.x+2*P3.x)/3 ;
		P2.y = (P0.y+2*P3.y)/3 ;
		P4.x = (2*P3.x+P6.x)/3 ;
		P4.y = (2*P3.y+P6.y)/3 ;
		P5.x = (P3.x+2*P6.x)/3 ;
		P5.y = (P3.y+2*P6.y)/3 ;
		Transform( P1, T1 ) ;
		Transform( P2, T2 ) ;
		Transform( P3, T3 ) ;
		Transform( P4, T4 ) ;
		Transform( P5, T5 ) ;
		return GenCurve( P1, P2, P0, P1, P2, P3, T0, T1, T2, T3 ) &&
			   GenCurve( P4, P5, P3, P4, P5, P6, T3, T4, T5, T6 ) ;
	}
	else
	{
		DPOINT P2,P4 ;
		DPOINT T2,T4 ;
		P2.x = (2*P0.x+P6.x)/3 ;
		P2.y = (2*P0.y+P6.y)/3 ;
		P4.x = (P0.x+2*P6.x)/3 ;
		P4.y = (P0.y+2*P6.y)/3 ;
		Transform( P2, T2 ) ;
		Transform( P4, T4 ) ;
		return GenCurve( P2, P4, P0, P2, P4, P6, T0, T2, T4, T6 ) ;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void GEnvelope2x2::Transform( DPOINT a, DPOINT& b )
{
	a.x *= 2.0 ;
	a.y *= 2.0 ;
	if ( a.y<1.0 )
		if ( a.x<1.0 )
		{
			b.x = A00.x+a.x*(B00.x+a.x*(E00.x+a.x*J00.x))
					   +a.y*(C00.x+a.y*(F00.x+a.y*K00.x)
					   +a.x*(D00.x+a.x*(G00.x+a.x*L00.x)
					  			  +a.y*(H00.x+a.y*M00.x))) ;
			b.y = A00.y+a.x*(B00.y+a.x*(E00.y+a.x*J00.y))
					   +a.y*(C00.y+a.y*(F00.y+a.y*K00.y)
					   +a.x*(D00.y+a.x*(G00.y+a.x*L00.y)
					  			  +a.y*(H00.y+a.y*M00.y))) ;
		}
		else
		{
			a.x-- ;
			b.x = A01.x+a.x*(B01.x+a.x*(E01.x+a.x*J01.x))
					   +a.y*(C01.x+a.y*(F01.x+a.y*K01.x)
					   +a.x*(D01.x+a.x*(G01.x+a.x*L01.x)
					  			  +a.y*(H01.x+a.y*M01.x))) ;
			b.y = A01.y+a.x*(B01.y+a.x*(E01.y+a.x*J01.y))
					   +a.y*(C01.y+a.y*(F01.y+a.y*K01.y)
					   +a.x*(D01.y+a.x*(G01.y+a.x*L01.y)
					  			  +a.y*(H01.y+a.y*M01.y))) ;
		}
	else
	{
		a.y-- ;
		if ( a.x<1.0 )
		{
			b.x = A10.x+a.x*(B10.x+a.x*(E10.x+a.x*J10.x))
					   +a.y*(C10.x+a.y*(F10.x+a.y*K10.x)
					   +a.x*(D10.x+a.x*(G10.x+a.x*L10.x)
					  			  +a.y*(H10.x+a.y*M10.x))) ;
			b.y = A10.y+a.x*(B10.y+a.x*(E10.y+a.x*J10.y))
					   +a.y*(C10.y+a.y*(F10.y+a.y*K10.y)
					   +a.x*(D10.y+a.x*(G10.y+a.x*L10.y)
					  			  +a.y*(H10.y+a.y*M10.y))) ;
		}
		else
		{
			a.x-- ;
			b.x = A11.x+a.x*(B11.x+a.x*(E11.x+a.x*J11.x))
					   +a.y*(C11.x+a.y*(F11.x+a.y*K11.x)
					   +a.x*(D11.x+a.x*(G11.x+a.x*L11.x)
					  			  +a.y*(H11.x+a.y*M11.x))) ;
			b.y = A11.y+a.x*(B11.y+a.x*(E11.y+a.x*J11.y))
					   +a.y*(C11.y+a.y*(F11.y+a.y*K11.y)
					   +a.x*(D11.y+a.x*(G11.y+a.x*L11.y)
					  			  +a.y*(H11.y+a.y*M11.y))) ;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
