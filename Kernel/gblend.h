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
// Gavin's blend code

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// GBlend.h
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************************************

void Define(
		PPOINT	Src0Points,			First source path
		PBYTE	Src0Types,
		UINT32	Src0Length,
		PPOINT	Src1Points,			Second source path
		PBYTE	Src1Types,
		UINT32	Src1Length,
		BOOL	Flag,				Set for 1 to 1 blending
		UINT32	pFlatness,			Flatness
		PUINT32	Buffer,				Workspace
		UINT32	BufferLength		Workspace length
	) ;

This call should be made once in order to set up a blend. It will blend one subpath to one other
subpath. Each subpath is treated as though it is unclosed.

Paths are expected to be in the form of a single subpath. ClosePath options are ignored.
Workspace required is 4 bytes per path element per path.

Note that currently (04/11/94) there is a lack of error handling. Paths should be in the correct
format.

/////////////////////////////////////////////////////////////////////////////////////////////////

UINT32 Blend(
		double	pRatio,				Blend ratio of 1st path to 2nd path
		PPOINT	DstPoints,			Destination path
		PBYTE	DstTypes,
		UINT32	DstLength			Maximum length of destination path.
	) ;

This call actually performs a blend. It should be called once for each required value of pRatio.

************************************************************************************************/

class GBlend
{
public:
	void Define(
		PPOINT	Src0Points,
		PBYTE	Src0Types,
		UINT32	Src0Length,
		PPOINT	Src1Points,
		PBYTE	Src1Types,
		UINT32	Src1Length,
		BOOL	Flag,
		UINT32	pFlatness,
		PUINT32	Buffer,
		UINT32	BufferLength
	) ;
	UINT32 Blend(
		double	pRatio,
		PPOINT	DstPoints,
		PBYTE	DstTypes,
		INT32	DstLength
	) ;
private:
	BOOL	Is1to1 ;
	UINT32	Flatness ;
	INT32	Ratio ;
	PPOINT	S0Points ; PBYTE S0Types ; UINT32 S0Length ; PUINT32 LengthPtr0 ; PUINT32 EndLengthPtr0 ;
	PPOINT	S1Points ; PBYTE S1Types ; UINT32 S1Length ; PUINT32 LengthPtr1 ; PUINT32 EndLengthPtr1 ;
	PPOINT	 OPoints ; PBYTE  OTypes ; size_t OLength ;
	PPOINT	IPoints0 ; PBYTE ITypes0 ; UINT32 ILength0 ;
	PPOINT	IPoints1 ; PBYTE ITypes1 ; UINT32 ILength1 ;
	PUINT32	LPtr0 ; BYTE Type0 ; UINT32 Length0 ; INT32 Match0 ; UINT32 Total0 ;
	PUINT32	LPtr1 ; BYTE Type1 ; UINT32 Length1 ; INT32 Match1 ; UINT32 Total1 ;
	POINT P00,P01,P02,P03 ;
	POINT P10,P11,P12,P13 ;
	POINT L1,L2,M,R1,R2 ;

 	BOOL CalcPathLengths(
		PPOINT	Points,
		PBYTE	Types,
		UINT32	Length,
		PUINT32	Buffer,
		UINT32	BufferLength,
		PUINT32 &BufferEnd
	) ;
	BOOL BlendNtoN() ;
	BOOL BlendMtoN() ;
	BOOL Blend1to1() ;
	BOOL BlendPoint( BYTE Type, POINT P0, POINT P1 ) ;
	BOOL BlendCurve( POINT P01,POINT P02,POINT P03, POINT P11,POINT P12,POINT P13 ) ;
	void Match    ( PUINT32 Ptr, PUINT32 EndPtr, UINT32 Length, INT32 &Match, UINT32 &Total ) ;
	void MatchLast( PUINT32 Ptr, PUINT32 EndPtr, UINT32 Length, INT32 &Match, UINT32 &Total ) ;
	void ReadPath0() ; void ReadPathStart0() ;
	void ReadPath1() ; void ReadPathStart1() ;
	void Split( POINT P0,POINT P1,POINT P2,POINT P3, INT32 N,UINT32 D ) ;
	INT32 BezierLength( POINT P0, POINT P1, POINT P2, POINT P3 )	;
	INT32 FlattenSplit( POINT P0, POINT P1, POINT P2, POINT P3 ) ;
} ;

/////////////////////////////////////////////////////////////////////////////////////////////////
