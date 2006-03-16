// $Id: fuzzclip.cpp 662 2006-03-14 21:31:49Z alex $
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
// This is a Gavin File that contains the code to clip paths before rendering them to allow
// much higher levels of zoom.

/*
*/

#include "camtypes.h"
#include "fuzzclip.h"
//#include <limits.h>
//#include <afxwin.h>
//#include <math.h>

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

struct GRECT {
	INT32 lx ;
	INT32 ly ;
	INT32 hx ;
	INT32 hy ;
} ;

/////////////////////////////////////////////////////////////////////////////////////////////////
/*
	bl - bottom left
	br - bottom right
	tl - top left
	tr - top right
	10 - bl+br
	11 - bl+tl
	12 - br+tr
	13 - br+bl
	14 - tl+tr
	15 - tl+bl
	16 - tr+br
	17 - tr+tl
	oo - ignore line
	xx - split line
	pp - plot start and end
	ee - plot end only
*/

#define oo 00
#define bl 01
#define br 02
#define tl 03
#define tr 04
#define pp 05
#define ee 06
#define xx 07

static CONST BYTE CaseTable[25][25] = {
//	 BL    BC    BR                  CL    CC    CR                  TL    TC    TR
    {oo,bl,bl,bl,bl, bl,oo,bl,bl,10, bl,bl,xx,xx,xx, bl,bl,xx,xx,xx, bl,11,xx,xx,xx},	// BL
	{oo,oo,oo,oo,oo, bl,oo,oo,oo,br, bl,bl,xx,xx,xx, bl,bl,xx,xx,xx, bl,11,xx,xx,xx},
	{oo,oo,oo,oo,oo, bl,oo,oo,oo,br, bl,xx,xx,xx,br, xx,xx,xx,xx,xx, xx,xx,xx,xx,xx},	// BC
	{oo,oo,oo,oo,oo, bl,oo,oo,oo,br, xx,xx,xx,br,br, xx,xx,xx,br,br, xx,xx,xx,12,br},
	{br,br,br,br,oo, 13,br,br,oo,br, xx,xx,xx,br,br, xx,xx,xx,br,br, xx,xx,xx,12,br},	// BR

	{oo,bl,bl,bl,bl, oo,oo,bl,bl,10, oo,oo,xx,xx,xx, oo,oo,xx,xx,xx, oo,tl,xx,xx,xx},
	{oo,bl,bl,bl,bl, bl,oo,bl,bl,10, bl,bl,pp,pp,xx, bl,bl,pp,pp,xx, bl,11,xx,xx,xx},
	{oo,oo,oo,oo,oo, bl,oo,oo,oo,br, xx,pp,pp,pp,xx, xx,pp,pp,pp,xx, xx,xx,xx,xx,xx},
	{br,br,br,br,oo, 13,br,br,oo,br, xx,pp,pp,br,br, xx,pp,pp,br,br, xx,xx,xx,12,br},
	{br,br,br,br,oo, 13,br,br,oo,oo, xx,xx,xx,oo,oo, xx,xx,xx,oo,oo, xx,xx,xx,tr,oo},

	{oo,bl,bl,xx,xx, oo,oo,xx,xx,xx, oo,oo,xx,xx,xx, oo,oo,xx,xx,xx, oo,tl,tl,xx,xx},	// CL
	{oo,bl,xx,xx,xx, oo,oo,pp,pp,xx, oo,oo,pp,pp,xx, oo,oo,pp,pp,xx, oo,tl,xx,xx,xx},
	{xx,xx,xx,xx,xx, xx,ee,ee,ee,xx, xx,ee,ee,ee,xx, xx,ee,ee,ee,xx, xx,xx,xx,xx,xx},	// CC
	{xx,xx,xx,br,oo, xx,pp,pp,oo,oo, xx,pp,pp,oo,oo, xx,pp,pp,oo,oo, xx,xx,xx,tr,oo},
	{xx,xx,br,br,oo, xx,xx,xx,oo,oo, xx,xx,xx,oo,oo, xx,xx,xx,oo,oo, xx,xx,tr,tr,oo},	// CR

	{oo,bl,xx,xx,xx, oo,oo,xx,xx,xx, oo,oo,xx,xx,xx, oo,oo,tl,tl,14, oo,tl,tl,tl,tl},
	{tl,15,xx,xx,xx, tl,tl,pp,pp,xx, tl,tl,pp,pp,xx, tl,oo,tl,tl,14, oo,tl,tl,tl,tl},
	{xx,xx,xx,xx,xx, xx,pp,pp,pp,xx, xx,pp,pp,pp,xx, tl,oo,oo,oo,tr, oo,oo,oo,oo,oo},
	{xx,xx,xx,16,tr, xx,pp,pp,tr,tr, xx,pp,pp,tr,tr, 17,tr,tr,oo,tr, tr,tr,tr,tr,oo},
	{xx,xx,xx,br,oo, xx,xx,xx,oo,oo, xx,xx,xx,oo,oo, 17,tr,tr,oo,oo, tr,tr,tr,tr,oo},

	{tl,15,xx,xx,xx, tl,tl,xx,xx,xx, tl,tl,xx,xx,xx, tl,oo,tl,tl,14, oo,tl,tl,tl,tl},	// TL
	{tl,15,xx,xx,xx, tl,tl,xx,xx,xx, tl,tl,xx,xx,xx, tl,oo,oo,oo,tr, oo,oo,oo,oo,oo},
	{xx,xx,xx,xx,xx, xx,xx,xx,xx,xx, tl,xx,xx,xx,tr, tl,oo,oo,oo,tr, oo,oo,oo,oo,oo},	// TC
	{xx,xx,xx,16,tr, xx,xx,xx,tr,tr, xx,xx,xx,tr,tr, tl,oo,oo,oo,tr, oo,oo,oo,oo,oo},
	{xx,xx,xx,16,tr, xx,xx,xx,tr,tr, xx,xx,xx,tr,tr, 17,tr,tr,oo,tr, tr,tr,tr,tr,oo}	// TR
} ;

//
// Define regions :
//
// *****  *++++  ++++*  +++++  +++++  +++++  +++++  +++++  +++++  +++++
// +++++  *++++  ++++*  +++++  +***+  +*+++  +++*+  +++++  ++*++  +++++
// +++++  *++++  ++++*  +++++  +++++  +*+++  +++*+  +++++  ++*++  +***+
// +++++  *++++  ++++*  +++++  +++++  +*+++  +++*+  +***+  ++*++  +++++
// +++++  *++++  ++++*  *****  +++++  +++++  +++++  +++++  +++++  +++++
//
static CONST DWORD Region[25] = {
	0x003,0x001,0x001,0x001,0x005,
	0x002,0x030,0x110,0x050,0x004,
	0x002,0x220,0x300,0x240,0x004,
	0x002,0x0a0,0x180,0x0c0,0x004,
	0x00a,0x008,0x008,0x008,0x00c
} ;

static	INT32	LastSector ;

static	GRECT	*IRect ;
static	GRECT	*ORect ;

static	POINT	LastPoint ;

static	PPOINT	OPoints ;
static	PBYTE	OTypes ;
static	size_t	OMaxLength ;
static	size_t	OLength ;
static	BOOL	OFirstPoint ;

static	UINT32	Index ;				// Position within source path.
static	UINT32	LastIndex ;			// Position within source path for last output point.
static	BOOL	SameLine ;			// Last two output points are for the same path.

/////////////////////////////////////////////////////////////////////////////////////////////////

inline BOOL IsMove( CONST BYTE Type )
{
	return (Type & PT_MOVETO)==PT_MOVETO ;
}

inline BOOL IsntMove( CONST BYTE Type )
{
	return (Type & PT_MOVETO)!=PT_MOVETO ;
}

inline BOOL IsLine( CONST BYTE Type )
{
	return (Type & PT_MOVETO)==PT_LINETO ;
}

inline BOOL IsCurve( CONST BYTE Type )
{
	return (Type & PT_MOVETO)==PT_BEZIERTO ;
}

inline BOOL IsntCurve( CONST BYTE Type )
{
	return (Type & PT_MOVETO)!=PT_BEZIERTO ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// SameLine indicates that two points of the current line have been plotted. A third point can
// overwrite the second.
//
void GenLine( INT32 X, INT32 Y, BOOL Merge=TRUE )
{
	if ( OFirstPoint )
	{
		OFirstPoint = FALSE ;
		if ( ++OLength > OMaxLength )
			throw 1 ;
		*OTypes++ = PT_MOVETO ;
		OPoints->x = X ;
		OPoints->y = Y ;
		OPoints++ ;
		SameLine = FALSE ;
		LastIndex = Merge?Index:INT32_MAX ;
	}
	else if ( Merge && SameLine && LastIndex==Index )
	{
		(OPoints-1)->x = X ;
		(OPoints-1)->y = Y ;
	}
	else if ( (OPoints-1)->x != X ||
			  (OPoints-1)->y != Y )
	{
		if ( ++OLength > OMaxLength )
			throw 1 ;
		*OTypes++ = PT_LINETO ;
		OPoints->x = X ;
		OPoints->y = Y ;
		OPoints++ ;
		SameLine = Merge && (LastIndex==Index) ;
		LastIndex = Merge?Index:INT32_MAX ;
	}
}


void GenCurve( POINT &P1, POINT &P2, POINT &P3 )
{
	if ( (OLength+=3) > OMaxLength )
		throw 1 ;
	*OTypes++ = PT_BEZIERTO ; *OPoints++ = P1 ;
	*OTypes++ = PT_BEZIERTO ; *OPoints++ = P2 ;
	*OTypes++ = PT_BEZIERTO ; *OPoints++ = P3 ;
	SameLine = FALSE ;
	LastIndex = INT32_MAX ;
}


inline void GenLine( POINT &P, BOOL F=TRUE ) { GenLine( P.x,P.y,F ) ; } ;


inline void GenLineToBelowLeft () { GenLine( IRect->lx,IRect->ly,FALSE ) ; } ;
inline void GenLineToBelowRight() { GenLine( IRect->hx,IRect->ly,FALSE ) ; } ;
inline void GenLineToAboveLeft () { GenLine( IRect->lx,IRect->hy,FALSE ) ; } ;
inline void GenLineToAboveRight() { GenLine( IRect->hx,IRect->hy,FALSE ) ; } ;

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// GetSector(P) returns the sector containing the point P according to the following :
//
//
//	 ORect.lx   IRect.hx
//		 |         |
//		 |IRect.lx | ORect.hx
//		 |    |    |    |
//	     |    |    |    |
//	  20 | 21 | 22 | 23 | 24
//	-----+----+----+----+-----	ORect.hy
//	  15 | 16 | 17 | 18 | 19
//	-----+----+----+----+-----  IRect.hy
//	  10 | 11 | 12 | 13 | 14
//	-----+----+----+----+-----	IRect.ly
//	  05 | 06 | 07 | 08 | 09
//	-----+----+----+----+-----	ORect.ly
//	  00 | 01 | 02 | 03 | 04
//	     |    |    |    |
//
//
INT32 GetSector( POINT &P )
{
	INT32 Sector = 12 ;
	if ( P.x < IRect->lx )
		if ( P.x < ORect->lx )
			Sector -= 2 ;
		else
			Sector-- ;
	else if ( P.x > IRect->hx )
		if ( P.x > ORect->hx )
			Sector += 2 ;
		else
			Sector++ ;
	if ( P.y < IRect->ly )
		if ( P.y < ORect->ly )
			Sector -= 2*5 ;
		else
			Sector -= 5 ;
	else if ( P.y > IRect->hy )
		if ( P.y > ORect->hy )
			Sector += 2*5 ;
		else
			Sector += 5 ;
	return Sector ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void ClipLine( POINT &Start, POINT &End )
{
	INT32 ThisSector = GetSector( End ) ;
	switch ( CaseTable[(INT32)LastSector][(INT32)ThisSector] )
	{
	case 01 : GenLineToBelowLeft()	; break ;
	case 02 : GenLineToBelowRight()	; break ;
	case 03 : GenLineToAboveLeft()	; break ;
	case 04 : GenLineToAboveRight()	; break ;
	case 05 : GenLine(Start)		;
			  GenLine(End)			; break ;
	case 06 : if ( OFirstPoint )
				  GenLine(Start) ;
			  GenLine(End)			; break ;
	case 07 :
		POINT Middle ;
		Middle.x = (Start.x+End.x) >> 1 ;
		Middle.y = (Start.y+End.y) >> 1 ;
		ClipLine( Start,Middle ) ;
		ClipLine( Middle,End ) ;
		return ;
	case 10 : GenLineToBelowLeft()	; GenLineToBelowRight()	; break ;
	case 11 : GenLineToBelowLeft()	; GenLineToAboveLeft()	; break ;
	case 12 : GenLineToBelowRight()	; GenLineToAboveRight()	; break ;
	case 13 : GenLineToBelowRight()	; GenLineToBelowLeft()	; break ;
	case 14 : GenLineToAboveLeft()	; GenLineToAboveRight()	; break ;
	case 15 : GenLineToAboveLeft()	; GenLineToBelowLeft()	; break ;
	case 16 : GenLineToAboveRight()	; GenLineToBelowRight()	; break ;
	case 17 : GenLineToAboveRight()	; GenLineToAboveLeft()	; break ;
	}
	LastSector = ThisSector ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void SplitCurve( POINT &P0, POINT &P1, POINT &P2, POINT &P3 ) ;


void ClipCurve( POINT &C0, POINT &C1, POINT &C2, POINT &C3 )
{
	INT32 C1Sector	= GetSector( C1 ) ;
	INT32 C2Sector	= GetSector( C2 ) ;
	INT32 ThisSector	= GetSector( C3 ) ;
//	if ( ( ( C1Sector==LastSector || C1Sector==ThisSector ) &&
//		   ( C2Sector==LastSector || C2Sector==ThisSector ) ) ||
//		 ( Region[LastSector] & Region[C1Sector] & Region[C2Sector] & Region[ThisSector] ) )
	INT32 RegionUnion = Region[LastSector] & Region[C1Sector] & Region[C2Sector] & Region[ThisSector];
	if ( ( RegionUnion & 0x0ff ) ||
		 ( (RegionUnion & 0x300) && (LastSector!=ThisSector || LastSector==12) ) ||
		 ( (C1Sector==LastSector || C1Sector==ThisSector) && (C2Sector==LastSector || C2Sector==ThisSector) ) )
	{
		switch ( CaseTable[(INT32)LastSector][(INT32)ThisSector] )
		{
		case 01 : GenLineToBelowLeft()	; break ;
		case 02 : GenLineToBelowRight()	; break ;
		case 03 : GenLineToAboveLeft()	; break ;
		case 04 : GenLineToAboveRight()	; break ;
		case 05 : GenLine(C0,FALSE) ;
				  GenCurve(C1,C2,C3)	; break ;
		case 06 : if ( OFirstPoint )
					  GenLine(C0,FALSE) ;
				  GenCurve(C1,C2,C3)	; break ;
		case 07 : SplitCurve(C0,C1,C2,C3) ; return ;
		case 10 : GenLineToBelowLeft()	; GenLineToBelowRight()	; break ;
		case 11 : GenLineToBelowLeft()	; GenLineToAboveLeft()	; break ;
		case 12 : GenLineToBelowRight()	; GenLineToAboveRight()	; break ;
		case 13 : GenLineToBelowRight()	; GenLineToBelowLeft()	; break ;
		case 14 : GenLineToAboveLeft()	; GenLineToAboveRight()	; break ;
		case 15 : GenLineToAboveLeft()	; GenLineToBelowLeft()	; break ;
		case 16 : GenLineToAboveRight()	; GenLineToBelowRight()	; break ;
		case 17 : GenLineToAboveRight()	; GenLineToAboveLeft()	; break ;
		}
		LastSector = ThisSector ;
	}
	else
		SplitCurve(C0,C1,C2,C3) ;
}


void SplitCurve( POINT &P0, POINT &P1, POINT &P2, POINT &P3 )
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
    ClipCurve( P0,L1,L2, M ) ;
    ClipCurve(  M,R1,R2,P3 ) ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

size_t FuzzyClip(
		PPOINT	IPoints,
		PBYTE	ITypes,
		size_t	ILength,
		BOOL	IsClosed,
		RECT	*InnerRect,
		RECT	*OuterRect,
		PPOINT	pOPoints,
		PBYTE	pOTypes,
		size_t	pOMaxLength
	)
{
	IRect		= (GRECT*) InnerRect ;
	ORect		= (GRECT*) OuterRect ;
	OPoints		= pOPoints ;
	OTypes		= pOTypes ;
	OMaxLength	= pOMaxLength ;
	OLength		= 0 ;

	if ( ILength <= 0 )
		return 0 ;

	try {

		Index = 0 ;
		while ( Index<ILength )
		{
			OFirstPoint = TRUE ;
			POINT StartPoint = IPoints[Index++] ;
			LastSector = GetSector( StartPoint ) ;
			LastPoint = StartPoint ;
			while ( Index<ILength && (IsLine(ITypes[Index]) || IsCurve(ITypes[Index])) )
			{
				if ( IsLine(ITypes[Index]) )
				{
					ClipLine( LastPoint,IPoints[Index] ) ;
					LastPoint = IPoints[Index++] ;
				}
				else
				{
					ClipCurve( LastPoint,IPoints[Index],IPoints[Index+1],IPoints[Index+2] ) ;
					LastPoint = IPoints[Index+2] ;
					Index += 3 ;
				}
			}
			if ( IsClosed || (ITypes[Index-1] & PT_CLOSEFIGURE) )
			{
				ClipLine( LastPoint,StartPoint ) ;
				if ( !OFirstPoint )
					*(OTypes-1) |= PT_CLOSEFIGURE ;
			}
			if ( !OFirstPoint && IsMove(*(OTypes-1)) )
			{
				OTypes-- ;
				OPoints-- ;
				OLength-- ;
			}
		}

	} catch ( INT32 ) {
		OLength = (size_t)-1 ;
	}

	return OLength ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
