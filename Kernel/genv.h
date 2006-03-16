// $Id: genv.h 662 2006-03-14 21:31:49Z alex $
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
// Header for the envelope mould shapes implementation

#ifndef INC_GENVELOPE
#define INC_GENVELOPE

#include "moldshap.h"

/********************************************************************************************

>	class GEnvelope

	Author:		Gavin_Theobald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/94
	Purpose:	The low level enveloper class.

********************************************************************************************/

/*

void Define( POINT *P, RECT *pBBox, DWORD pTolerance = 0 ) ;

	Define the envelope with twelve control points P. The source path has bounding box pBBox.
	pTolerance defines the accuracy with which curves are moulded. If pTolerance is zero then
	curves are never split.

void Redefine( POINT *P ) ;

	Redefines the mould after dragging the control points P.


void FitPoint(  POINT a, POINT& b ) ;

	This will mould the point a returning the result in b. Note that a may be outside the
	original bounding box.

void FitPoint( DPOINT a, POINT& b ) ;

	This will mould the point a specified as two values in the range 0 to 1. The result is
	returned in b. This call can be used for drawing grid points.

BOOL FitPath (
	CONST POINT *IPoints,
	CONST BYTE  *ITypes,
	DWORD ILength,
	LPPOINT OPoints,
	LPBYTE  OTypes,
	INT32 OLength,
	BOOL Close = FALSE
) ;

	This will mould a source path IPoints,ITypes,ILength returning the result in the destination
	path OPoints,OTypes,OLength. Curves may be split into several curves for greater accuracy if
	pTolerance was non-zero. If Close is non-zero then open paths are closed.


*/

class GEnvelope : public GMould
{
	CC_DECLARE_DYNAMIC(GEnvelope)

	public:
		GEnvelope();

		virtual BOOL Redefine( POINT *P ) ;
		virtual BOOL FitPath (
			CONST POINT *IPoints,
			CONST BYTE  *ITypes,
			DWORD ILength,
			LPPOINT OPoints,
			LPBYTE  OTypes,
			DWORD OLength,
			BOOL Close = FALSE
		) ;
		virtual void CopyShape( POINT *P ) ;

	private:
		virtual void ScaleView();
		virtual void CalcViewBBox();
		virtual void CalcViewConsts();
		virtual void Transform( DPOINT a, DPOINT& b ) ;
		
	private:
		POINT L0, L1, L2 ;
		POINT T0, T1, T2 ;
		POINT R0, R1, R2 ;
		POINT B0, B1, B2 ;
		DPOINT VL0, VL1, VL2 ;
		DPOINT VT0, VT1, VT2 ;
		DPOINT VR0, VR1, VR2 ;
		DPOINT VB0, VB1, VB2 ;
		DPOINT A, B, C, D, E, F, G, H, J, K, L, M ;
} ;


/********************************************************************************************

>	class GEnvelope2x2

	Author:		Gavin_Theobald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/94
	Purpose:	The low level enveloper class.

********************************************************************************************/

/*

void Define( POINT *P, RECT *pBBox, DWORD pTolerance = 0 ) ;

	Define the envelope with twenty-four control points P. The source path has bounding box pBBox.
	pTolerance defines the accuracy with which curves are moulded. If pTolerance is zero then
	curves are never split.

void Redefine( POINT *P ) ;

	Redefines the mould after dragging the control points P.


void FitPoint(  POINT a, POINT& b ) ;

	This will mould the point a returning the result in b. Note that a may be outside the
	original bounding box.

void FitPoint( DPOINT a, POINT& b ) ;

	This will mould the point a specified as two values in the range 0 to 1. The result is
	returned in b. This call can be used for drawing grid points.

BOOL FitPath (
	CONST POINT *IPoints,
	CONST BYTE  *ITypes,
	DWORD ILength,
	LPPOINT OPoints,
	LPBYTE  OTypes,
	INT32 OLength,
	BOOL Close = FALSE
) ;

	This will mould a source path IPoints,ITypes,ILength returning the result in the destination
	path OPoints,OTypes,OLength. Curves may be split into several curves for greater accuracy if
	pTolerance was non-zero. If Close is non-zero then open paths are closed.

*/
/////////////////////////////////////////////////////////////////////////////////////////////////

class GEnvelope2x2 : public GMould
{
	CC_DECLARE_DYNAMIC(GEnvelope2x2)

	public:
		GEnvelope2x2();

		virtual BOOL Redefine( POINT *P ) ;
		virtual BOOL FitPath (
			CONST POINT *IPoints,
			CONST BYTE  *ITypes,
			DWORD ILength,
			LPPOINT OPoints,
			LPBYTE  OTypes,
			DWORD OLength,
			BOOL Close = FALSE
		) ;
		virtual void CopyShape( POINT *P ) ;
	
	private:
		virtual void ScaleView() ;
		virtual void CalcViewBBox() ;
		virtual void CalcViewConsts() ;
		virtual void Transform( DPOINT a, DPOINT& b ) ;

		BOOL CalcConsts() ;
		BOOL GenLine( DPOINT P0, DPOINT P6, DPOINT T0, DPOINT T6 ) ;

	private:
		POINT L0, L1, L2, L3, L4, L5 ;
		POINT T0, T1, T2, T3, T4, T5 ;
		POINT R0, R1, R2, R3, R4, R5 ;
		POINT B0, B1, B2, B3, B4, B5 ;
		POINT H1, H2, H4, H5 ;
		POINT V1, V2, V4, V5 ;
		POINT MM ;
		DPOINT VL0, VL1, VL2, VL3, VL4, VL5 ;
		DPOINT VT0, VT1, VT2, VT3, VT4, VT5 ;
		DPOINT VR0, VR1, VR2, VR3, VR4, VR5 ;
		DPOINT VB0, VB1, VB2, VB3, VB4, VB5 ;
		DPOINT VH1, VH2, VH4, VH5 ;
		DPOINT VV1, VV2, VV4, VV5 ;
		DPOINT VMM ;
		BOOL SmoothX, SmoothY ;
		DPOINT A00, B00, C00, D00, E00, F00, G00, H00, J00, K00, L00, M00 ;
		DPOINT A01, B01, C01, D01, E01, F01, G01, H01, J01, K01, L01, M01 ;
		DPOINT A10, B10, C10, D10, E10, F10, G10, H10, J10, K10, L10, M10 ;
		DPOINT A11, B11, C11, D11, E11, F11, G11, H11, J11, K11, L11, M11 ;
} ;

#endif
