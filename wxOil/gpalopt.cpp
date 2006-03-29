// $Id$
/////////////////////////////////////////////////////////////////////////////
//
// PalOpt.cpp
//
/////////////////////////////////////////////////////////////////////////////
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

#include "gpalopt.h"

#pragma warning ( disable : 4554 )

/***************************************************************************/

inline PALETTEENTRY peRGB( cBYTE r,cBYTE g,cBYTE b, cBYTE f=0 )
{
	PALETTEENTRY q = {r,g,b, f} ;
	return q ;
}

/***************************************************************************/

cBYTE PaletteOptimiser::aSnap[0x132] = {
									   0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x33,0x33,0x33,0x33,0x33,0x33,
	0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33, 0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,
	0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33, 0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,
	0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33, 0x33,0x33,0x33,0x33,0x33,0x66,0x66,0x66,
	0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66, 0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,
	0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66, 0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,
	0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66, 0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99, 0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99, 0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,
	0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99, 0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,
	0x99,0x99,0x99,0xCC,0xCC,0xCC,0xCC,0xCC, 0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,
	0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC, 0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,
	0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC, 0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,
	0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,

	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, 0xFF
} ;

#define S(n) 0x##n*0x##n

const UINT32 PaletteOptimiser::aBDist[0x100] = {
	S(00),S(01),S(02),S(03),S(04),S(05),S(06),S(07), S(08),S(09),S(0a),S(0b),S(0c),S(0d),S(0e),S(0f),
	S(10),S(11),S(12),S(13),S(14),S(15),S(16),S(17), S(18),S(19),S(19),S(18),S(17),S(16),S(15),S(14),
	S(13),S(12),S(11),S(10),S(0f),S(0e),S(0d),S(0c), S(0b),S(0a),S(09),S(08),S(07),S(06),S(05),S(04),
	S(03),S(02),S(01),S(00),S(01),S(02),S(03),S(04), S(05),S(06),S(07),S(08),S(09),S(0a),S(0b),S(0c),
	S(0d),S(0e),S(0f),S(10),S(11),S(12),S(13),S(14), S(15),S(16),S(17),S(18),S(19),S(19),S(18),S(17),
	S(16),S(15),S(14),S(13),S(12),S(11),S(10),S(0f), S(0e),S(0d),S(0c),S(0b),S(0a),S(09),S(08),S(07),
	S(06),S(05),S(04),S(03),S(02),S(01),S(00),S(01), S(02),S(03),S(04),S(05),S(06),S(07),S(08),S(09),
	S(0a),S(0b),S(0c),S(0d),S(0e),S(0f),S(10),S(11), S(12),S(13),S(14),S(15),S(16),S(17),S(18),S(19),
	S(19),S(18),S(17),S(16),S(15),S(14),S(13),S(12), S(11),S(10),S(0f),S(0e),S(0d),S(0c),S(0b),S(0a),
	S(09),S(08),S(07),S(06),S(05),S(04),S(03),S(02), S(01),S(00),S(01),S(02),S(03),S(04),S(05),S(06),
	S(07),S(08),S(09),S(0a),S(0b),S(0c),S(0d),S(0e), S(0f),S(10),S(11),S(12),S(13),S(14),S(15),S(16),
	S(17),S(18),S(19),S(19),S(18),S(17),S(16),S(15), S(14),S(13),S(12),S(11),S(10),S(0f),S(0e),S(0d),
	S(0c),S(0b),S(0a),S(09),S(08),S(07),S(06),S(05), S(04),S(03),S(02),S(01),S(00),S(01),S(02),S(03),
	S(04),S(05),S(06),S(07),S(08),S(09),S(0a),S(0b), S(0c),S(0d),S(0e),S(0f),S(10),S(11),S(12),S(13),
	S(14),S(15),S(16),S(17),S(18),S(19),S(19),S(18), S(17),S(16),S(15),S(14),S(13),S(12),S(11),S(10),
	S(0f),S(0e),S(0d),S(0c),S(0b),S(0a),S(09),S(08), S(07),S(06),S(05),S(04),S(03),S(02),S(01),S(00)
} ;

const UINT32 PaletteOptimiser::aPDist[0x100] = {
	S(00),S(01),S(02),S(03),S(04),S(05),S(06),S(07), S(08),S(09),S(0a),S(0b),S(0c),S(0d),S(0e),S(0f),
	S(10),S(11),S(12),S(13),S(14),S(15),S(16),S(17), S(18),S(19),S(1a),S(1b),S(1c),S(1d),S(1e),S(1f),
	S(20),S(21),S(22),S(23),S(24),S(25),S(26),S(27), S(28),S(29),S(2a),S(2b),S(2c),S(2d),S(2e),S(2f),
	S(30),S(31),S(32),S(33),S(34),S(35),S(36),S(37), S(38),S(39),S(3a),S(3b),S(3c),S(3d),S(3e),S(3f),
	S(40),S(41),S(42),S(43),S(44),S(45),S(46),S(47), S(48),S(49),S(4a),S(4b),S(4c),S(4d),S(4e),S(4f),
	S(50),S(51),S(52),S(53),S(54),S(55),S(56),S(57), S(58),S(59),S(5a),S(5b),S(5c),S(5d),S(5e),S(5f),
	S(60),S(61),S(62),S(63),S(64),S(65),S(66),S(67), S(68),S(69),S(6a),S(6b),S(6c),S(6d),S(6e),S(6f),
	S(70),S(71),S(72),S(73),S(74),S(75),S(76),S(77), S(78),S(79),S(7a),S(7b),S(7c),S(7d),S(7e),S(7f),
	S(7f),S(7e),S(7d),S(7c),S(7b),S(7a),S(79),S(78), S(77),S(76),S(75),S(74),S(73),S(72),S(71),S(70),
	S(6f),S(6e),S(6d),S(6c),S(6b),S(6a),S(69),S(68), S(67),S(66),S(65),S(64),S(63),S(62),S(61),S(60),
	S(5f),S(5e),S(5d),S(5c),S(5b),S(5a),S(59),S(58), S(57),S(56),S(55),S(54),S(53),S(52),S(51),S(50),
	S(4f),S(4e),S(4d),S(4c),S(4b),S(4a),S(49),S(48), S(47),S(46),S(45),S(44),S(43),S(42),S(41),S(40),
	S(3f),S(3e),S(3d),S(3c),S(3b),S(3a),S(39),S(38), S(37),S(36),S(35),S(34),S(33),S(32),S(31),S(30),
	S(2f),S(2e),S(2d),S(2c),S(2b),S(2a),S(29),S(28), S(27),S(26),S(25),S(24),S(23),S(22),S(21),S(20),
	S(1f),S(1e),S(1d),S(1c),S(1b),S(1a),S(19),S(18), S(17),S(16),S(15),S(14),S(13),S(12),S(11),S(10),
	S(0f),S(0e),S(0d),S(0c),S(0b),S(0a),S(09),S(08), S(07),S(06),S(05),S(04),S(03),S(02),S(01),S(00)
} ;

const UINT32 PaletteOptimiser::aSqrs[0x100] = {
	S(00),S(01),S(02),S(03),S(04),S(05),S(06),S(07), S(08),S(09),S(0a),S(0b),S(0c),S(0d),S(0e),S(0f),
	S(10),S(11),S(12),S(13),S(14),S(15),S(16),S(17), S(18),S(19),S(1a),S(1b),S(1c),S(1d),S(1e),S(1f),
	S(20),S(21),S(22),S(23),S(24),S(25),S(26),S(27), S(28),S(29),S(2a),S(2b),S(2c),S(2d),S(2e),S(2f),
	S(30),S(31),S(32),S(33),S(34),S(35),S(36),S(37), S(38),S(39),S(3a),S(3b),S(3c),S(3d),S(3e),S(3f),
	S(40),S(41),S(42),S(43),S(44),S(45),S(46),S(47), S(48),S(49),S(4a),S(4b),S(4c),S(4d),S(4e),S(4f),
	S(50),S(51),S(52),S(53),S(54),S(55),S(56),S(57), S(58),S(59),S(5a),S(5b),S(5c),S(5d),S(5e),S(5f),
	S(60),S(61),S(62),S(63),S(64),S(65),S(66),S(67), S(68),S(69),S(6a),S(6b),S(6c),S(6d),S(6e),S(6f),
	S(70),S(71),S(72),S(73),S(74),S(75),S(76),S(77), S(78),S(79),S(7a),S(7b),S(7c),S(7d),S(7e),S(7f),
	S(80),S(81),S(82),S(83),S(84),S(85),S(86),S(87), S(88),S(89),S(8a),S(8b),S(8c),S(8d),S(8e),S(8f),
	S(90),S(91),S(92),S(93),S(94),S(95),S(96),S(97), S(98),S(99),S(9a),S(9b),S(9c),S(9d),S(9e),S(9f),
	S(a0),S(a1),S(a2),S(a3),S(a4),S(a5),S(a6),S(a7), S(a8),S(a9),S(aa),S(ab),S(ac),S(ad),S(ae),S(af),
	S(b0),S(b1),S(b2),S(b3),S(b4),S(b5),S(b6),S(b7), S(b8),S(b9),S(ba),S(bb),S(bc),S(bd),S(be),S(bf),
	S(c0),S(c1),S(c2),S(c3),S(c4),S(c5),S(c6),S(c7), S(c8),S(c9),S(ca),S(cb),S(cc),S(cd),S(ce),S(cf),
	S(d0),S(d1),S(d2),S(d3),S(d4),S(d5),S(d6),S(d7), S(d8),S(d9),S(da),S(db),S(dc),S(dd),S(de),S(df),
	S(e0),S(e1),S(e2),S(e3),S(e4),S(e5),S(e6),S(e7), S(e8),S(e9),S(ea),S(eb),S(ec),S(ed),S(ee),S(ef),
	S(f0),S(f1),S(f2),S(f3),S(f4),S(f5),S(f6),S(f7), S(f8),S(f9),S(fa),S(fb),S(fc),S(fd),S(fe),S(ff)
} ;

#undef S

/////////////////////////////////////////////////////////////////////////////
//
// Call this function prior to calling AddStats the first time.
// It initialises the statistics tables.
//
void PaletteOptimiser::Initialise()
{
	for ( UINT32 i=0 ; i<AXIS3 ; i++ )
		m_aStats[i].W =
		m_aStats[i].R =
		m_aStats[i].G =
		m_aStats[i].B =
		m_aStats[i].S = 0.0 ;

	m_bMoments = false ;
	m_uTotalColours	  =
	m_uPaletteEntries = 0 ;
}

/////////////////////////////////////////////////////////////////////////////
//
// This function processes the pixels of a bitmap and adds variaus statistics
// to the internal tables. This function called be called several times, once
// for each strip of a bitmap. Call Initialise prior to the first call to
// this function to initialise the statistics tables.
//
//		pBitmap - pointer to bitmap
//		nSize	- size of bitmap
//
void PaletteOptimiser::AddStats( cpcRGBQUAD pBitmap, cUINT32 uSize )
{
	ASSERT(pBitmap) ;
	ASSERT(uSize) ;
	//
	// Determine the number of colours used within the bitmap
	//
	UINT32 i ;
	if ( m_uTotalColours<=MAXCOLOURS )
		for ( i=0 ; i<uSize ; i++ )
			if ( (DWORD&)pBitmap[i]<0xff000000u )
			{
				cDWORD d = (DWORD&)pBitmap[i] & 0x00ffffff ;
				for( UINT32 j=0 ; j<m_uTotalColours ; j++ )
					if ( d==(DWORD&)m_aColours[j] )
					{
						m_aCount[j]++ ;
						goto found ;
					}
				m_aColours[m_uTotalColours++] = (RGBQUAD&)d ;
				if ( m_uTotalColours>MAXCOLOURS )
					break ;
		found:	;
			}
	//
	// Compile image statistics.
	//
	for ( i=0 ; i<uSize ; i++ )
		if ( pBitmap[i].rgbReserved!=0xff )
		{
			static cBYTE aIndex[0x100] = {
				0x01,0x01,0x01,0x01,0x01,0x02,0x02,0x02, 0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,
				0x03,0x03,0x03,0x03,0x03,0x04,0x04,0x04, 0x04,0x04,0x04,0x04,0x04,0x05,0x05,0x05,
				0x05,0x05,0x05,0x05,0x05,0x05,0x06,0x06, 0x06,0x06,0x06,0x06,0x06,0x06,0x07,0x07,
				0x07,0x07,0x07,0x07,0x07,0x07,0x08,0x08, 0x08,0x08,0x08,0x08,0x08,0x08,0x09,0x09,
				0x09,0x09,0x09,0x09,0x09,0x09,0x0A,0x0A, 0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0B,
				0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,0x0C, 0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0D,
				0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0D,0x0E, 0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,
				0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F, 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
				0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11, 0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,
				0x13,0x13,0x13,0x13,0x13,0x13,0x13,0x13, 0x13,0x14,0x14,0x14,0x14,0x14,0x14,0x14,
				0x14,0x15,0x15,0x15,0x15,0x15,0x15,0x15, 0x15,0x16,0x16,0x16,0x16,0x16,0x16,0x16,
				0x16,0x17,0x17,0x17,0x17,0x17,0x17,0x17, 0x17,0x17,0x18,0x18,0x18,0x18,0x18,0x18,
				0x18,0x18,0x19,0x19,0x19,0x19,0x19,0x19, 0x19,0x19,0x1A,0x1A,0x1A,0x1A,0x1A,0x1A,
				0x1A,0x1A,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B, 0x1B,0x1B,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,
				0x1C,0x1C,0x1C,0x1D,0x1D,0x1D,0x1D,0x1D, 0x1D,0x1D,0x1D,0x1E,0x1E,0x1E,0x1E,0x1E,
				0x1E,0x1E,0x1E,0x1F,0x1F,0x1F,0x1F,0x1F, 0x1F,0x1F,0x1F,0x20,0x20,0x20,0x20,0x20
			} ;
			cUINT32 r = pBitmap[i].rgbRed ;
			cUINT32 g = pBitmap[i].rgbGreen ;
			cUINT32 b = pBitmap[i].rgbBlue ;
			cpStats p = m_aStats+(aIndex[r]*AXIS+aIndex[g])*AXIS+aIndex[b] ;
			p->W++ ;
			p->R += r ;
			p->G += g ;
			p->B += b ;
			p->S += aSqrs[r]+aSqrs[g]+aSqrs[b] ;
		}
}

/////////////////////////////////////////////////////////////////////////////
//
// Call this function to add a locked colour. Locked colours are always added
// first to the optimal palette, but the order of the locked colours is not
// relevant.
//
void PaletteOptimiser::AddLockedColour( cBYTE red,cBYTE green,cBYTE blue )
{
	ASSERT(m_uLockedColours<0x100) ;
	m_aLockedColours[m_uLockedColours].peRed   = red   ;
	m_aLockedColours[m_uLockedColours].peGreen = green ;
	m_aLockedColours[m_uLockedColours].peBlue  = blue  ;
	m_uLockedColours++ ;
}

/////////////////////////////////////////////////////////////////////////////

void PaletteOptimiser::Moments3D()
{
	double lineW,areaW[AXIS] ;
	double lineR,areaR[AXIS] ;
	double lineG,areaG[AXIS] ;
	double lineB,areaB[AXIS] ;
	double lineS,areaS[AXIS] ;

    for ( UINT32 r=1 ; r<AXIS ; ++r )
    {
		for ( UINT32 i=0 ; i<AXIS ; ++i ) 
		    areaW[i] = areaR[i] = areaG[i] = areaB[i] = areaS[i] = 0.0 ;
		for ( UINT32 g=1 ; g<AXIS ; ++g )
		{
		    lineW = lineR = lineG = lineB = lineS = 0.0 ;

			pStats p = m_aStats+(r*AXIS+g)*AXIS ;	 // [r][g][b]
		    for ( UINT32 b=1 ; b<AXIS ; ++b )
		    {
				p++ ;
				p->W = p[-AXIS2].W+(areaW[b]+=lineW+=p->W) ;	// ind-AXIS2 = [r-1][g][b]
				p->R = p[-AXIS2].R+(areaR[b]+=lineR+=p->R) ;
				p->G = p[-AXIS2].G+(areaG[b]+=lineG+=p->G) ;
				p->B = p[-AXIS2].B+(areaB[b]+=lineB+=p->B) ;
				p->S = p[-AXIS2].S+(areaS[b]+=lineS+=p->S) ;
		    }
		}
    }
}

/////////////////////////////////////////////////////////////////////////////

double PaletteOptimiser::Snap( cpBox pCube, cDOUBLE halfW,cDOUBLE halfR,cDOUBLE halfG,cDOUBLE halfB )
{
	static cUINT32 aIndex[0x21] = {
		0x00,0x05,0x0D,0x15, 0x1D,0x26,0x2E,0x36,
		0x3E,0x46,0x4F,0x57, 0x5F,0x67,0x70,0x78,
		0x80,0x88,0x90,0x99, 0xA1,0xA9,0xB1,0xBA,
		0xC2,0xCA,0xD2,0xDA, 0xE3,0xEB,0xF3,0xFB, 0x100
	} ;

	if ( m_uAllLockedColours )
	{
		double max = -1e100 ;
		for ( UINT32 i=0 ; i<m_uAllLockedColours ; i++ )
		{
			Clr C = {
				m_aLockedColours[i].peRed,
				m_aLockedColours[i].peGreen,
				m_aLockedColours[i].peBlue
			} ;
			if ( aIndex[pCube->r0]<=C.r && aIndex[pCube->r1]>C.r &&
				 aIndex[pCube->g0]<=C.g && aIndex[pCube->g1]>C.g &&
				 aIndex[pCube->b0]<=C.b && aIndex[pCube->b1]>C.b )
			{
				cDOUBLE temp = 2*(halfR*C.r+halfG*C.g+halfB*C.b)-(C.r*C.r+C.g*C.g+C.b*C.b)*halfW ;
				if ( max<temp )
				{
					max = temp ;
					pCube->clr = C ;
				}
			}
		}
		if ( max>-1e50 )
			return max ;
	}

	if ( halfW==0 )
		return -1e100 ;
	cDOUBLE invW = 1/halfW ;
	pCube->clr.r = (UINT32)(halfR*invW+0.5) ;
	pCube->clr.g = (UINT32)(halfG*invW+0.5) ;
	pCube->clr.b = (UINT32)(halfB*invW+0.5) ;

	if ( m_bUseBrowserPalette )
	{
		double max = -1e100 ;
		Clr C ;
		for (         C.r=aSnap[pCube->clr.r] ; C.r<=aSnap[pCube->clr.r+0x32] ; C.r+=0x33 )
			for (     C.g=aSnap[pCube->clr.g] ; C.g<=aSnap[pCube->clr.g+0x32] ; C.g+=0x33 )
				for ( C.b=aSnap[pCube->clr.b] ; C.b<=aSnap[pCube->clr.b+0x32] ; C.b+=0x33 )
					if ( aIndex[pCube->r0]<=C.r && aIndex[pCube->r1]>C.r &&
						 aIndex[pCube->g0]<=C.g && aIndex[pCube->g1]>C.g &&
						 aIndex[pCube->b0]<=C.b && aIndex[pCube->b1]>C.b )
					{
						cDOUBLE temp = 2*(halfR*C.r+halfG*C.g+halfB*C.b)-(C.r*C.r+C.g*C.g+C.b*C.b)*halfW ;
						if ( max<temp )
						{
							max = temp ;
							pCube->clr = C ;
						}
					}
		return max ;
	}
	if ( m_uSnapToBrowserPalette )
	{
		if ( aBDist[pCube->clr.r]+aBDist[pCube->clr.g]+aBDist[pCube->clr.b] <= m_uSnapToBrowserPalette2 )
		{
			Clr C = {
				aSnap[pCube->clr.r+0x19],
				aSnap[pCube->clr.g+0x19],
				aSnap[pCube->clr.b+0x19]
			} ;
			if ( aIndex[pCube->r0]<=C.r && aIndex[pCube->r1]>C.r &&
				 aIndex[pCube->g0]<=C.g && aIndex[pCube->g1]>C.g &&
				 aIndex[pCube->b0]<=C.b && aIndex[pCube->b1]>C.b )
			{
				pCube->clr = C ;
				return 2*(halfR*C.r+halfG*C.g+halfB*C.b)-(C.r*C.r+C.g*C.g+C.b*C.b)*halfW ;
			}
		}
	}
	if ( m_uSnapToPrimaries )
	{
		if ( aPDist[pCube->clr.r]+aPDist[pCube->clr.g]+aPDist[pCube->clr.b] <= m_uSnapToPrimaries2 )
		{
			Clr C = {
				pCube->clr.r<0x80 ? 0x00 : 0xFF,
				pCube->clr.g<0x80 ? 0x00 : 0xFF,
				pCube->clr.b<0x80 ? 0x00 : 0xFF,
			} ;
			if ( aIndex[pCube->r0]<=C.r && aIndex[pCube->r1]>C.r &&
				 aIndex[pCube->g0]<=C.g && aIndex[pCube->g1]>C.g &&
				 aIndex[pCube->b0]<=C.b && aIndex[pCube->b1]>C.b )
			{
				pCube->clr = C ;
				return 2*(halfR*C.r+halfG*C.g+halfB*C.b)-(C.r*C.r+C.g*C.g+C.b*C.b)*halfW ;
			}
		}
	}
	return (halfR*halfR+halfG*halfG+halfB*halfB)*invW ;
}

/////////////////////////////////////////////////////////////////////////////

double PaletteOptimiser::Vol( cpcBox pCube, double Stats::* p ) const
{
	return +LCube(p,pCube->r1,pCube->g1,pCube->b1)
		   -LCube(p,pCube->r1,pCube->g1,pCube->b0)
		   -LCube(p,pCube->r1,pCube->g0,pCube->b1)
		   +LCube(p,pCube->r1,pCube->g0,pCube->b0)
		   -LCube(p,pCube->r0,pCube->g1,pCube->b1)
		   +LCube(p,pCube->r0,pCube->g1,pCube->b0)
		   +LCube(p,pCube->r0,pCube->g0,pCube->b1)
		   -LCube(p,pCube->r0,pCube->g0,pCube->b0) ;
}

double PaletteOptimiser::Bottom( cpcBox pCube, cUINT32 dir, double Stats::* p ) const
{
	switch (dir)
	{
	case RED :	 return +LCube(p,pCube->r0,pCube->g1,pCube->b0)
						+LCube(p,pCube->r0,pCube->g0,pCube->b1)
						-LCube(p,pCube->r0,pCube->g1,pCube->b1)
						-LCube(p,pCube->r0,pCube->g0,pCube->b0) ;
	case GREEN : return +LCube(p,pCube->r1,pCube->g0,pCube->b0)
						+LCube(p,pCube->r0,pCube->g0,pCube->b1)
						-LCube(p,pCube->r1,pCube->g0,pCube->b1)
						-LCube(p,pCube->r0,pCube->g0,pCube->b0) ;
	case BLUE :	 return +LCube(p,pCube->r1,pCube->g0,pCube->b0)
						+LCube(p,pCube->r0,pCube->g1,pCube->b0)
						-LCube(p,pCube->r1,pCube->g1,pCube->b0)
						-LCube(p,pCube->r0,pCube->g0,pCube->b0) ;
	}
	ASSERT(false) ;
	return 0 ;
}

double PaletteOptimiser::Top( cpcBox pCube, cUINT32 dir, cUINT32 pos, double Stats::* p ) const
{
	switch (dir)
	{
	case RED :	 return +LCube(p,pos,pCube->g1,pCube->b1)
			   			+LCube(p,pos,pCube->g0,pCube->b0) 
			   			-LCube(p,pos,pCube->g1,pCube->b0)
			   			-LCube(p,pos,pCube->g0,pCube->b1) ;
	case GREEN : return +LCube(p,pCube->r1,pos,pCube->b1)
			   			+LCube(p,pCube->r0,pos,pCube->b0) 
			   			-LCube(p,pCube->r1,pos,pCube->b0)
			   			-LCube(p,pCube->r0,pos,pCube->b1) ;
	case BLUE :  return +LCube(p,pCube->r1,pCube->g1,pos)
			   			+LCube(p,pCube->r0,pCube->g0,pos)
			   			-LCube(p,pCube->r1,pCube->g0,pos)
			   			-LCube(p,pCube->r0,pCube->g1,pos) ;
	}
	ASSERT(false) ;
	return 0 ;
}

double PaletteOptimiser::Var( cpcBox pCube ) const
{
	cDOUBLE ww = Vol(pCube,&Stats::W) ;
	cDOUBLE dr = Vol(pCube,&Stats::R) ;
	cDOUBLE dg = Vol(pCube,&Stats::G) ;
	cDOUBLE db = Vol(pCube,&Stats::B) ;
	cDOUBLE xx = Vol(pCube,&Stats::S) ;
	return xx+(pCube->clr.r*pCube->clr.r+pCube->clr.g*pCube->clr.g+pCube->clr.b*pCube->clr.b)*ww
		   -2*(pCube->clr.r*dr          +pCube->clr.g*dg		  +pCube->clr.b*db) ;
}

/////////////////////////////////////////////////////////////////////////////

double PaletteOptimiser::Maximize(
							cpBox pCube, 
							cUINT32 dir, 
							cUINT32 first,
							cUINT32 last,
							cpBox pCube1,
							cpBox pCube2,
							cDOUBLE wholeW,
							cDOUBLE wholeR,
							cDOUBLE wholeG,
							cDOUBLE wholeB
						)
{
	cDOUBLE baseW = Bottom(pCube,dir,&Stats::W) ;
	cDOUBLE baseR = Bottom(pCube,dir,&Stats::R) ;
	cDOUBLE baseG = Bottom(pCube,dir,&Stats::G) ;
	cDOUBLE baseB = Bottom(pCube,dir,&Stats::B) ;

	Box cube1,cube2 ;
	cube1 = cube2 = *pCube ;

	double max = -1e100 ;

	//  Did the bottom of the pCube above.
	for ( UINT32 i=first+1 ; i<last ; ++i )
	{
		switch (dir)
		{
		case RED   : cube1.r1 = cube2.r0 = i ; break ;
		case GREEN : cube1.g1 = cube2.g0 = i ; break ;
		case BLUE  : cube1.b1 = cube2.b0 = i ; break ;
		}
        // Now half_x is sum over lower half of box, if split at i.
		double halfW = baseW+Top(pCube,dir,i,&Stats::W) ;
		double halfR = baseR+Top(pCube,dir,i,&Stats::R) ;
		double halfG = baseG+Top(pCube,dir,i,&Stats::G) ;
		double halfB = baseB+Top(pCube,dir,i,&Stats::B) ;
		cDOUBLE temp1 = Snap(&cube1,halfW,halfR,halfG,halfB) ;
		if ( temp1<-1e50 )
			continue ;
		halfW = wholeW-halfW ;
		halfR = wholeR-halfR ;
		halfG = wholeG-halfG ;
		halfB = wholeB-halfB ;
		cDOUBLE temp2 = Snap(&cube2,halfW,halfR,halfG,halfB) ;
		if ( temp2<-1e50 )
			continue ;
	    if ( max<temp1+temp2 )
    	{
    		max = temp1+temp2 ;
			*pCube1 = cube1 ;
			*pCube2 = cube2 ;
    	}
    }
    return max ;
}

bool PaletteOptimiser::Cut( cpBox pSet1,cpBox pSet2, cpDOUBLE pMax )
{
	cDOUBLE wholeW = Vol(pSet1,&Stats::W) ;
	cDOUBLE wholeR = Vol(pSet1,&Stats::R) ;
	cDOUBLE wholeG = Vol(pSet1,&Stats::G) ;
	cDOUBLE wholeB = Vol(pSet1,&Stats::B) ;

	Box cubeR1,cubeR2, cubeG1,cubeG2, cubeB1,cubeB2 ;
	cDOUBLE maxR = Maximize( pSet1, RED  , pSet1->r0,pSet1->r1, &cubeR1,&cubeR2, wholeW,wholeR,wholeG,wholeB ) ;
	cDOUBLE maxG = Maximize( pSet1, GREEN, pSet1->g0,pSet1->g1, &cubeG1,&cubeG2, wholeW,wholeR,wholeG,wholeB ) ;
	cDOUBLE maxB = Maximize( pSet1, BLUE , pSet1->b0,pSet1->b1, &cubeB1,&cubeB2, wholeW,wholeR,wholeG,wholeB ) ;

	*pSet2 = *pSet1 ;
	if ( maxR>=maxG && maxR>=maxB )
	{
		if ( maxR<-1e50 )
			return false ; // can't split the box
		*pMax = maxR ;
		*pSet1 = cubeR1 ;
		*pSet2 = cubeR2 ;
	}
	else if ( maxG>=maxR && maxG>=maxB ) 
	{
		*pMax = maxG ;
		*pSet1 = cubeG1 ;
		*pSet2 = cubeG2 ;
	}
	else
	{
		*pMax = maxB ;
		*pSet1 = cubeB1 ;
		*pSet2 = cubeB2 ;
	}
	pSet1->vol = (pSet1->r1-pSet1->r0)*(pSet1->g1-pSet1->g0)*(pSet1->b1-pSet1->b0) ;
	pSet2->vol = (pSet2->r1-pSet2->r0)*(pSet2->g1-pSet2->g0)*(pSet2->b1-pSet2->b0) ;
	return true ;
}

double PaletteOptimiser::Maximize(
							cpBox pCube,
							cUINT32 dir, 
							cUINT32 first,
							cUINT32 last,
							cpBox pCube1,
							cpBox pCube2,
							cpBox pCube3,
							cDOUBLE wholeW,
							cDOUBLE wholeR,
							cDOUBLE wholeG,
							cDOUBLE wholeB
						)
{
	cDOUBLE baseW = Bottom(pCube,dir,&Stats::W) ;
	cDOUBLE baseR = Bottom(pCube,dir,&Stats::R) ;
	cDOUBLE baseG = Bottom(pCube,dir,&Stats::G) ;
	cDOUBLE baseB = Bottom(pCube,dir,&Stats::B) ;

	Box cube1,cube2,cube3,cube4 ;
	cube1 = cube2 = *pCube ;

	double temp ;
	double max = -1e100 ;

	//  Did the bottom of the cube above.
	for ( UINT32 i=first+1 ; i<last ; ++i )
	{
		switch (dir)
		{
		case RED   : cube1.r1 = cube2.r0 = i ; break ;
		case GREEN : cube1.g1 = cube2.g0 = i ; break ;
		case BLUE  : cube1.b1 = cube2.b0 = i ; break ;
		}

		// half_x is sum over lower half of box, if split at i.
		double halfW = baseW+Top(pCube,dir,i,&Stats::W) ;
		double halfR = baseR+Top(pCube,dir,i,&Stats::R) ;
		double halfG = baseG+Top(pCube,dir,i,&Stats::G) ;
		double halfB = baseB+Top(pCube,dir,i,&Stats::B) ;
		cube3 = cube2 ;
		if ( Cut(&cube3,&cube4,&temp) )
		{
			temp += Snap(&cube1,halfW,halfR,halfG,halfB) ;
			if ( temp>-1e50 && max<temp )
			{
				max = temp ;
				*pCube1 = cube1 ;
				*pCube2 = cube3 ;
				*pCube3 = cube4 ;
			}
		}

		halfW = wholeW-halfW ;
		halfR = wholeR-halfR ;
		halfG = wholeG-halfG ;
		halfB = wholeB-halfB ;
		cube3 = cube1 ;
		if ( Cut(&cube3,&cube4,&temp) )
		{
			temp += Snap(&cube2,halfW,halfR,halfG,halfB) ;
			if ( temp>-1e50 && max<temp )
			{
				max = temp ;
				*pCube1 = cube2 ;
				*pCube2 = cube3 ;
				*pCube3 = cube4 ;
			}
		}
	}
	return max ;
}

bool PaletteOptimiser::Cut( cpBox pSet1,cpBox pSet2,cpBox pSet3 )
{
	Box set12 ;
	set12.r0 = min(pSet1->r0,pSet2->r0) ;
	set12.r1 = max(pSet1->r1,pSet2->r1) ;
	set12.g0 = min(pSet1->g0,pSet2->g0) ;
	set12.g1 = max(pSet1->g1,pSet2->g1) ;
	set12.b0 = min(pSet1->b0,pSet2->b0) ;
	set12.b1 = max(pSet1->b1,pSet2->b1) ;

	cDOUBLE wholeW = Vol(&set12,&Stats::W) ;
	cDOUBLE wholeR = Vol(&set12,&Stats::R) ;
	cDOUBLE wholeG = Vol(&set12,&Stats::G) ;
	cDOUBLE wholeB = Vol(&set12,&Stats::B) ;

	Box cubeR1,cubeR2,cubeR3 ;
	Box cubeG1,cubeG2,cubeG3 ;
	Box cubeB1,cubeB2,cubeB3 ;
	cDOUBLE maxR = Maximize( &set12, RED  , set12.r0,set12.r1, &cubeR1,&cubeR2,&cubeR3, wholeW,wholeR,wholeG,wholeB ) ;
	cDOUBLE maxG = Maximize( &set12, GREEN, set12.g0,set12.g1, &cubeG1,&cubeG2,&cubeG3, wholeW,wholeR,wholeG,wholeB ) ;
	cDOUBLE maxB = Maximize( &set12, BLUE , set12.b0,set12.b1, &cubeB1,&cubeB2,&cubeB3, wholeW,wholeR,wholeG,wholeB ) ;

	if ( maxR>=maxG && maxR>=maxB )
	{
		if ( maxR<-1e50 )
			return false ; // can't split the box
		*pSet1 = cubeR1 ;
		*pSet2 = cubeR2 ;
		*pSet3 = cubeR3 ;
	}
	else if ( maxG>=maxR && maxG>=maxB ) 
	{
		*pSet1 = cubeG1 ;
		*pSet2 = cubeG2 ;
		*pSet3 = cubeG3 ;
	}
	else
	{
		*pSet1 = cubeB1 ;
		*pSet2 = cubeB2 ;
		*pSet3 = cubeB3 ;
	}
	pSet1->vol = (pSet1->r1-pSet1->r0)*(pSet1->g1-pSet1->g0)*(pSet1->b1-pSet1->b0) ;
	pSet2->vol = (pSet2->r1-pSet2->r0)*(pSet2->g1-pSet2->g0)*(pSet2->b1-pSet2->b0) ;
	pSet3->vol = (pSet3->r1-pSet3->r0)*(pSet3->g1-pSet3->g0)*(pSet3->b1-pSet3->b0) ;
	return true ;
}

/////////////////////////////////////////////////////////////////////////////
//
// This function generates the colours of an optimised palette. This function
// is usually called once after calling AddStats. GetPalette can be called
// several times to then obtain the actual palette of a particular size.
//
void PaletteOptimiser::GenPalette( cUINT32 uMaxColours )
{
	ASSERT( uMaxColours<=0x100 ) ;
	//
	// Calculate the 3d moments of inertia within our field
	//
	if ( !m_bMoments )
	{
		Moments3D() ;
		m_bMoments = true ;
	}

	m_uPaletteEntries = 0 ;

	if ( m_bAddSystemColours )
	{
		AddLockedColour(0x00,0x00,0x00) ;
		AddLockedColour(0x00,0x00,0x80) ;
		AddLockedColour(0x00,0x80,0x00) ;
		AddLockedColour(0x00,0x80,0x80) ;
		AddLockedColour(0x80,0x00,0x00) ;
		AddLockedColour(0x80,0x00,0x80) ;
		AddLockedColour(0x80,0x80,0x00) ;
		AddLockedColour(0x80,0x80,0x80) ;
		AddLockedColour(0x00,0x00,0xFF) ;
		AddLockedColour(0x00,0xFF,0x00) ;
		AddLockedColour(0x00,0xFF,0xFF) ;
		AddLockedColour(0xFF,0x00,0x00) ;
		AddLockedColour(0xFF,0x00,0xFF) ;
		AddLockedColour(0xFF,0xFF,0x00) ;
		AddLockedColour(0xFF,0xFF,0xFF) ;
		AddLockedColour(0xC0,0xC0,0xC0) ;
		m_uAllLockedColours = m_uLockedColours ;
		m_uLockedColours -= 16 ;
	}
	else
		m_uAllLockedColours = m_uLockedColours ;
	m_uSnapToPrimaries2		 = m_uSnapToPrimaries     *m_uSnapToPrimaries ;
	m_uSnapToBrowserPalette2 = m_uSnapToBrowserPalette*m_uSnapToBrowserPalette ;
	//
	// Initialise our first cube to encompass the entire volume
	//
	Box	cube[MAXCOLOURS] ;
	cube[0].r0 = cube[0].g0 = cube[0].b0 = 0 ;
	cube[0].r1 = cube[0].g1 = cube[0].b1 = AXIS-1 ;
	cube[0].vol = (AXIS-1)*(AXIS-1)*(AXIS-1) ;
	cube[0].var = 0 ;
	cube[0].pPair = NULL ;
	cDOUBLE ww = Vol(cube,&Stats::W) ;
	if ( ww==0 )
		return ;
	cube[0].clr.r = (UINT32)(Vol(cube,&Stats::R)/ww+0.5) ;
	cube[0].clr.g = (UINT32)(Vol(cube,&Stats::G)/ww+0.5) ;
	cube[0].clr.b = (UINT32)(Vol(cube,&Stats::B)/ww+0.5) ;

	m_uPaletteEntries = 0 ;
	Store(0,cube) ;

	pBox pCubeO = cube ;
	for ( UINT32 i=1 ; i<uMaxColours ; )
	{
		double temp ;
		cpBox pCubeN = cube+i ;
		cpBox pCubeP = pCubeO->pPair ;
		if ( !m_bFast && pCubeP )
			if ( Cut(pCubeP,pCubeO,pCubeN) )
			{
				pCubeP->pPair = NULL ;
				pCubeO->pPair = pCubeN ;
				pCubeN->pPair = pCubeO ;
				Store(pCubeP-cube,pCubeP) ;
				Store(pCubeO-cube,pCubeO) ;
				Store(pCubeN-cube,pCubeN) ;
				i++ ;
			}
			else
			{
				pCubeP->pPair = NULL ;
				pCubeO->pPair = NULL ;
				pCubeO->vol = 0 ;
			}
		else
			if ( Cut(pCubeO,pCubeN,&temp) )
			{
				pCubeN->pPair = pCubeO ;
				pCubeO->pPair = pCubeN ;
				Store(pCubeO-cube,pCubeO) ;
				Store(pCubeN-cube,pCubeN) ;
				i++ ;
			}
			else
				pCubeO->vol = 0 ;
		//
		// Find box with maximum error.
		//
		pCubeO = cube ;
		temp = -1 ;
		for ( UINT32 k=0 ; k<i ; ++k )
			if ( temp<cube[k].var && cube[k].vol>1 )
			{
				pCubeO = cube+k ;
				temp = pCubeO->var ;
			}
		if ( temp<0 )
			break ;
	}
}



void PaletteOptimiser::Store( cUINT32 i, cpBox pCube )
{
	pCube->var = Var(pCube) ;
	m_aPalette[m_uPaletteEntries  ].C = peRGB( pCube->clr.r,pCube->clr.g,pCube->clr.b, i ) ;
	m_aPalette[m_uPaletteEntries++].W = Vol(pCube,&Stats::W) ;
}

/////////////////////////////////////////////////////////////////////////////
//
// Call this function to obtain the optimised palette of a particalur size.
//
//		pPalette	- points to a logical palette. The palNumEntries entry
//					  should previously have been initialised.
//		nMaxColours - required number of optimised colours.
//
bool PaletteOptimiser::GetPalette( cpLOGPALETTE pPalette, cUINT32 uMaxColours )
{
	ASSERT(pPalette) ;
	ASSERT(pPalette->palNumEntries>0 && pPalette->palNumEntries<=0x100) ;
	ASSERT(uMaxColours>0 && uMaxColours<=0x100) ;
	//
	// Check for zero colour case. This can occur if all colours in the image
	// were transparent.
	//
	if ( m_uPaletteEntries==0 )
	{
		pPalette->palNumEntries = 0 ;
		return true ;
	}
	//
	// n is current size of generated palette
	//
	PalEntry T[0x100] ;
	UINT32 i,j,n ;
	bool bFixed = false ;
	if ( !m_bUseBrowserPalette && m_bUseBitmapColours && m_uTotalColours<=MAXCOLOURS &&
		 (m_uTotalColours<uMaxColours || m_uSnapToBrowserPalette || m_uSnapToPrimaries) )
	{
		bFixed = true ;
		for ( i=0,n=0 ; i<m_uTotalColours ; i++ )
		{
			Clr clr = {
				m_aColours[i].rgbRed,
				m_aColours[i].rgbGreen,
				m_aColours[i].rgbBlue
			} ;
			for ( j=0 ; j<m_uAllLockedColours ; j++ )
				if ( m_aLockedColours[j].peRed  ==clr.r &&
					 m_aLockedColours[j].peGreen==clr.g &&
					 m_aLockedColours[j].peBlue ==clr.b )
					goto nosnap ;
			//
			// Shouldn't snap colour if it is a locked colour.
			//
			if ( m_uSnapToBrowserPalette &&
				 aBDist[clr.r]+aBDist[clr.g]+aBDist[clr.b]<=m_uSnapToBrowserPalette2 )
			{
				clr.r = aSnap[clr.r] ;
				clr.g = aSnap[clr.g] ;
				clr.b = aSnap[clr.b] ;
			}
			if ( m_uSnapToPrimaries &&
				 aPDist[clr.r]+aPDist[clr.g]+aPDist[clr.b]<=m_uSnapToPrimaries2 )
			{
				clr.r = clr.r<0x80 ? 0x00 : 0xFF ;
				clr.g = clr.g<0x80 ? 0x00 : 0xFF ;
				clr.b = clr.b<0x80 ? 0x00 : 0xFF ;
			}
	nosnap:	//
			// Only add colour to palette if not already there.
			//
			for ( j=0 ; j<n ; j++ )
				if ( T[j].C.peRed  ==clr.r &&
					 T[j].C.peGreen==clr.g &&
					 T[j].C.peBlue ==clr.b )
					goto ignore ;
			if ( n==uMaxColours )
			{
				bFixed = false ;
				break ;
			}
			T[n].C.peRed   = clr.r ;
			T[n].C.peGreen = clr.g ;
			T[n].C.peBlue  = clr.b ;
			T[n].W = m_aCount[i] ;
			n++ ;
	ignore:	;
		}
	}

	if ( !bFixed )
		for ( i=0,n=0 ; i<m_uPaletteEntries && n<uMaxColours ; i++ )
		{
			j = m_aPalette[i].C.peFlags ;
			T[j] = m_aPalette[i] ;
			if ( j>=n )
				n++ ;
		}

	qsort(T,n,sizeof PalEntry,SortFn) ;
	for ( i=0 ; i<n ; i++ )
	{
		pPalette->palPalEntry[i] = T[i].C ;
		pPalette->palPalEntry[i].peFlags = 0 ;
	}
	pPalette->palNumEntries = n ;

#ifdef DEBUG
//	TRACE( _T("N : %i\n"),n) ;
//	for ( i=0 ; i<n ; i++ )
//		TRACE( _T("%2i : %02x,%02x,%02x\n"),i,pPalette->palPalEntry[i].peRed,pPalette->palPalEntry[i].peGreen,pPalette->palPalEntry[i].peBlue) ;
#endif
	//
	// Fill in any remaining unused required colours with non-renderable black
	//
//	for ( ; n<nMaxColours ; )
//		pPalette->palPalEntry[n++] = peRGB(0,0,0, 0xFF) ;

	return bFixed ;
}



INT32 __cdecl PaletteOptimiser::SortFn( pcVOID elem1, pcVOID elem2 )
{
	return (INT32)( ((cpcPalEntry)elem2)->W - ((cpcPalEntry)elem1)->W ) ;
}

/***************************************************************************/
