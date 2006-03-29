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
// Gavin's path winding code

/*
*/

#include "camtypes.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// GWinding.cpp
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "gwinding.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

INT32 GWSgnMulSubMul( INT32 A, INT32 B, INT32 C, INT32 D )
{
#if defined(__WXMSW__)
	UINT32 temp ;
	__asm {
		mov		eax,A
		imul	B
		mov		ebx,eax
		mov		ecx,edx
		mov		eax,C
		imul	D
		sub		ebx,eax
		sbb		ecx,edx
		jne		returnecx
		or		ecx,ebx
		jns		returnecx
			not		ecx
returnecx:
		mov		temp,ecx
	}
	return temp;
#else
	XLONG X = (XLONG)A*B-(XLONG)C*D;
	return X>0 ? 1 : X<0 ? -1 : 0 ;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////

PPOINT Points ;
PPOINT StartPoint ;

POINT BottomLeft ;
POINT PrevPoint ;
POINT LastPoint ;
POINT OldPoint ;

POINT Prev_Point ;
POINT Next_Point ;

INT32 Flags ;

UINT32 Flatness ;
UINT32 BottomLeftOffset ;

/////////////////////////////////////////////////////////////////////////////////////////////////

void DoLine( POINT P )
{
	if ( (Flags & 04) && ( OldPoint.x!=LastPoint.x || OldPoint.y!=LastPoint.y ) )
	{
		PrevPoint = LastPoint ;
		Flags &= ~04 ;
	}
	if (  P.x+P.y >  BottomLeft.x+BottomLeft.y ||
		 (P.x+P.y == BottomLeft.x+BottomLeft.y &&
		  	  P.y >=			  BottomLeft.y) )
	{
		if ( (Flags & 02) && ( P.x!=BottomLeft.x ||
							   P.y!=BottomLeft.y ) )
		{
			Next_Point = P ;
			Flags &= ~02 ;
		}
		Flags |= 04 ;
	}
	else
	{
		Prev_Point = PrevPoint ;
		BottomLeft = P ;
		BottomLeftOffset = Points-StartPoint ; /* Save curve offset */
		Flags = 02 ;
		PrevPoint = P ;
	}
	OldPoint = LastPoint ;
	LastPoint = P ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void FlattenSplit( POINT P0, POINT P1, POINT P2, POINT P3 ) ;

void FlattenCurve( POINT P0, POINT P1, POINT P2, POINT P3 )
{
	UINT32 dx,dy ;
	dx = abs(P1.x*3 - P0.x*2 - P3.x) ;
	dy = abs(P1.y*3 - P0.y*2 - P3.y) ;
	if ( (dx>=dy ? 3*dx+dy : dx+3*dy) > Flatness )
    	FlattenSplit(P0,P1,P2,P3) ;
	else
	{
		dx = abs(P2.x*3 - P0.x - P3.x*2) ;
		dy = abs(P2.y*3 - P0.y - P3.y*2) ;
		if ( (dx>=dy ? 3*dx+dy : dx+3*dy) > Flatness )
    		FlattenSplit(P0,P1,P2,P3) ;
		else
			DoLine(P3) ;
	}
}


void FlattenSplit( POINT P0, POINT P1, POINT P2, POINT P3 )
{
	POINT L1, L2, M, R1, R2 ;
	L1.x = (P0.x + P1.x)/2;
	L1.y = (P0.y + P1.y)/2;
	L2.x = (P0.x + 2*P1.x + P2.x)/4;
	L2.y = (P0.y + 2*P1.y + P2.y)/4;
	 M.x = (P0.x + 3*P1.x + 3*P2.x + P3.x)/8;
	 M.y = (P0.y + 3*P1.y + 3*P2.y + P3.y)/8;
	R1.x = (P1.x + 2*P2.x + P3.x)/4;
	R1.y = (P1.y + 2*P2.y + P3.y)/4;
	R2.x = (P2.x + P3.x)/2;
	R2.y = (P2.y + P3.y)/2;
    FlattenCurve(P0, L1, L2, M) ;
    FlattenCurve(M, R1, R2, P3) ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

BOOL GWinding(
		PPOINT	pPoints,
		PBYTE	Types,
		INT32	Length,
		UINT32	pFlatness,
		UINT32	&pBottomLeftOffset,
		POINT	&pBottomLeft
	)
{
	Flatness = pFlatness*27/2 ;
	StartPoint = Points = pPoints ;
	BottomLeft = PrevPoint = LastPoint = *Points++ ; Types++ ; --Length ;
	BottomLeftOffset = 0 ;
	Flags = 03 ;
	while ( Length>0 )
	{
		if ( (*Types & PT_MOVETO)==PT_LINETO )
		{
			DoLine( Points[0] ) ;
			Points++ ; Types++ ; --Length ;
		}
		else /* Curve */
		{
			FlattenCurve( Points[-1],Points[0],Points[1],Points[2] ) ;
			Points+=3 ; Types+=3 ; Length-=3 ;
		}
	}
	if ( BottomLeft.x!=LastPoint.x || BottomLeft.y!=LastPoint.y )
		PrevPoint = LastPoint ;
	if ( Flags & 01 )
		Prev_Point = PrevPoint ;
	else if ( Flags & 02 )
		Next_Point = *StartPoint ;
	pBottomLeftOffset = BottomLeftOffset ;
	pBottomLeft = BottomLeft ;
	INT32 Sgn = GWSgnMulSubMul( Next_Point.x-BottomLeft.x, Prev_Point.y-BottomLeft.y,
							 Prev_Point.x-BottomLeft.x, Next_Point.y-BottomLeft.y ) ;
	if ( Sgn )
		return Sgn<0 ;
	Sgn = Next_Point.x-Prev_Point.x ;
	if ( Sgn )
		return Sgn<0 ;
	Sgn = Prev_Point.y-Next_Point.y ;
	return Sgn<0 ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
