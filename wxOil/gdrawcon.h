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

//

#ifndef INC_GDRAWCON
#define	INC_GDRAWCON

#include "gconsts.h"
class TranspGradTable;

/*******************************************************************************************

>	class GDrawContext : public SimpleCCObject

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Purpose:	To put a C++ wrapper around the GDraw DLL. Also allows non-Gavin originated
				GDraws (e.g. XDraw) to be dropped in as required. This class is almost a pure
				base class - inherited classes have to have their own functions for almost
				everything.

				The caller should only ever need one instance of one of these (really its
				inherited brothers) and keep it around for the duration of the program.
				The keeper of the pointer to this is GRenderRegion.

				With the exception of the ConvertBitmap function, this class could be put
				in the kernel. Maybe it should. Perhaps. Ish.

********************************************************************************************/ 


class GDrawContext : public SimpleCCObject
{
public:

	GDrawContext();
	virtual ~GDrawContext();

	virtual BOOL Init() = 0;

	virtual eError GetLastError() = 0;

	virtual INT32 GetGraduationTableSize() = 0;
	virtual INT32 GetGraduationTableLength() = 0;
	virtual INT32 GetLongGraduationTableSize() = 0;
	virtual INT32 GetLongGraduationTableLength() = 0;
	virtual INT32 GetTransparentGraduationTableSize() = 0;
	virtual INT32 GetTransparentGraduationTableLength() = 0;
	virtual INT32 GetLongTransparentGraduationTableSize() = 0;
	virtual INT32 GetLongTransparentGraduationTableLength() = 0;

	virtual void SetStackLimit( LPVOID ) = 0;
	virtual void SetStackSize( DWORD ) = 0;

	virtual BOOL SetupBitmap( INT32 Width, INT32 Height, INT32 Depth, LPBYTE Bits, DWORD Format = 2, BOOL DoBiCompression = FALSE ) = 0;
	virtual BOOL SetBitmap( CONST LPBITMAPINFOHEADER BitmapInfo, CONST LPBYTE Bitmap, DWORD BitmapFormat = 2) = 0;

	virtual BOOL SetMatrix( GMATRIX* ) = 0;
	virtual void SetAntialiasFlag( BOOL ) = 0;
	virtual void SetAntialiasQualityFlag( BOOL ) = 0;
	virtual void SetTileSmoothingFlag( BOOL ) = 0;
	virtual void SetTileFilteringFlag( BOOL ) = 0;
	virtual void SetMaxFilterSize( UINT32 ) = 0;
	virtual BOOL Sharpen( INT32 ) = 0;
	virtual BOOL Blur( INT32 ) = 0;
	virtual void SetHintingFlag( BOOL ) = 0;
	virtual void SetFlatness( DWORD ) = 0;

	virtual BOOL SetMiterLimit( DWORD ) = 0;
	virtual BOOL SetMatrixIntFlag( BOOL ) = 0;
	virtual BOOL SetDashAdjustmentFlag( BOOL ) = 0;

	virtual void TransformPath( LPPOINT, LPPOINT, DWORD, GMATRIX* ) = 0;

	virtual INT32 MaxScale( CONST GMATRIX *Matrix, CONST RECT *BBox ) = 0;

	// colour
	virtual void SetHalftoneOrigin( DWORD, DWORD ) = 0;
	virtual BOOL SetDitherStyle(DitherStyle Style = DITHER_GREY_ORDERED) = 0;
	virtual BOOL AddToGraduationTable( COLORREF, BOOL, GraduationTable*, DWORD ) = 0;
	virtual BOOL BuildGraduationTable( COLORREF, COLORREF, DWORD, GraduationTable* ) = 0;
	virtual BOOL BuildTransparencyTable( DWORD, DWORD, TransparentGradTable* ) = 0;
	virtual BOOL SetColour( COLORREF ) = 0;
	virtual BOOL SetSolidColour( COLORREF, DWORD BPP = 0, DWORD Format = 2 ) = 0;
	virtual BOOL SetWordColour( DWORD ) = 0;
	virtual BOOL SetTransparency( COLORREF, TransparencyEnum ) = 0;
	virtual BOOL ConvertBitmap( LPBITMAPINFOHEADER, LPBYTE, LPBITMAPINFOHEADER, LPBYTE, INT32 ) = 0;
	virtual const LOGPALETTE* SelectPalette( INT32 ) = 0;

	virtual BOOL SetInvert( DWORD ) = 0;
	virtual BOOL InitialiseWithPalette( const LOGPALETTE* ) = 0;
	virtual CONST BYTE* ReturnBrush() = 0;
	virtual CONST BYTE* ReturnBrushRGB() = 0;

	virtual BOOL SetSeparationTables(const BGR *CyanSepTable = NULL,
									 const BGR *MagentaSepTable = NULL,
									 const BGR *YellowSepTable = NULL,
									 const BGR *BlackSepTable = NULL,
									 const BYTE *UnderColourRemovalTable = NULL,
									 const BYTE *BlackGenerationTable = NULL) = 0;

	virtual BOOL SetBitmapConversionTable( const BGR* BitmapConversionTable = NULL ) = 0;

	virtual BOOL SetGraduation(
		DWORD Style,
		CONST GraduationTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC
	)  = 0;
	virtual BOOL SetTransparentGraduation(
		DWORD Style,
		CONST TranspGradTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC
	)  = 0;

	virtual BOOL SetGraduation4(
		DWORD Style,
		CONST GraduationTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC,
		CONST POINT *PointD
	)  = 0;
	virtual BOOL SetTransparentGraduation4(
		DWORD Style,
		CONST TranspGradTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC,
		CONST POINT *PointD
	)  = 0;

	virtual BOOL SetBias(UINT32,double) = 0;
	virtual BOOL SetGain(UINT32,double) = 0;
	virtual BOOL SetBrightness(double) = 0;
	virtual BOOL SetContrast(double) = 0;
	virtual BOOL SetGamma(double) = 0;
	virtual BOOL SetPostGamma(double) = 0;
	virtual BOOL SetSaturation(double) = 0;
	virtual BOOL SetContone( UINT32, COLORREF=0x000000, COLORREF=0xFFFFFF ) = 0;
	virtual BOOL SetInputRange( UINT32,BYTE=0x00,BYTE=0xFF ) = 0;
	virtual BOOL SetOutputRange( UINT32,BYTE=0x00,BYTE=0xFF ) = 0;

	virtual BOOL SetBitmapFill( LPBITMAPINFOHEADER, LPBYTE, DWORD, LPPOINT, COLORREF, LPRGBQUAD,
									LPBYTE, LPBYTE, LPBYTE, LPBYTE, DWORD = 0) = 0;

	virtual BOOL SetTransparentBitmapFill( LPBITMAPINFOHEADER, LPBYTE, DWORD, LPPOINT, COLORREF, BYTE* = NULL, DWORD = 0) = 0;

	virtual BOOL SetPerspectiveBitmapFill( LPBITMAPINFOHEADER, LPBYTE, DWORD, LPPOINT, COLORREF, LPRGBQUAD,
											LPBYTE, LPBYTE, LPBYTE, LPBYTE, DWORD ) = 0;

	virtual BOOL SetPerspectiveTransparentBitmapFill( LPBITMAPINFOHEADER, LPBYTE, DWORD, LPPOINT, COLORREF, BYTE*, DWORD) = 0;

	virtual BOOL SetTransparencyLookupTable(CONST BYTE * Table)=0;
	
	// rendering
	virtual BOOL PlotBitmap( LPBITMAPINFOHEADER, LPBYTE, DWORD, LPPOINT, LPRGBQUAD,
									LPBYTE, LPBYTE, LPBYTE, LPBYTE) = 0;

	virtual BOOL PlotPerspectiveBitmap( LPBITMAPINFOHEADER, LPBYTE, DWORD, LPPOINT, LPRGBQUAD,
											LPBYTE, LPBYTE, LPBYTE, LPBYTE) = 0;

	virtual BOOL StrokePath( LPPOINT, const BYTE *, UINT32, BOOL, DWORD, CapStyles, JoinStyles, const DashType* ) = 0;
	virtual BOOL FillPath( LPPOINT, const BYTE *, UINT32, DWORD ) = 0;
	virtual BOOL FillRectangle( LPRECT ) = 0;
	virtual BOOL FillPoint( LPPOINT ) = 0;
	virtual INT32 StrokePathToPath(
		CONST POINT *IPoints,
		CONST BYTE  *ITypes,
		DWORD ILength,
		POINT *OPoints,
		BYTE  *OTypes,
		DWORD OLength,
		BOOL Close,
		DWORD LineWidth,
		CapStyles LineCaps,
		JoinStyles LineJoin,
		CONST DashType *Dash) = 0;

	virtual INT32 CalcStrokeBBox(
		CONST POINT *IPoints,
		CONST BYTE  *ITypes,
		DWORD ILength,
		LPRECT Rect,
		BOOL Close,
		DWORD LineWidth,
		CapStyles LineCaps,
		JoinStyles LineJoin,
		CONST DashType *Dash) = 0;

	virtual BOOL HintPath( 
		CONST POINT *Points,
		CONST BYTE  *Types,
		UINT32 Length,
		BOOL Close,
		DWORD LineWidth) = 0;

	virtual INT32 MakeUnclippedRegion( 
		CONST POINT *Points,
		CONST BYTE  *Types,
		UINT32 Length,
		DWORD Winding,
		REGION *ORegion,
		UINT32 OLength) = 0;

	virtual BOOL FillRegion( CONST REGION *Region, CONST POINT *Offset ) = 0;

	virtual INT32 SetBevelContrast(UINT32 nContrast) = 0;
	virtual INT32 SetBevelLightness(UINT32 nContrast) = 0;
	virtual INT32 SetBevelDarkness(UINT32 nContrast) = 0;
	virtual INT32 TranslateBevelValue(BYTE nIndex, BYTE Colour) = 0;

	// misc
	virtual BOOL IsOverlap( LPPOINT, const BYTE *, UINT32, DWORD) = 0;
	virtual BOOL GetStatistics( LPPOINT, const BYTE *, UINT32, DWORD, STATISTICS *) = 0;
	virtual BOOL CalcBBox( LPPOINT, const BYTE *, DWORD, LPRECT, BOOL) = 0;

	virtual INT32 MakeRegion( LPPOINT, const BYTE *, UINT32, DWORD, REGION *, UINT32) = 0;

	virtual BOOL ClipRectangle( LPRECT ) = 0;
	virtual BOOL DeviceClipRectangle( LPRECT ) = 0;
	virtual BOOL GetDeviceClipRectangle( LPRECT ) = 0;
	virtual BOOL ClipRegion( const REGION* ) = 0;
	virtual const REGION* GetClipRegion() = 0;

	virtual void ClearChangedBBox() = 0;
	virtual BOOL GetChangedBBox( LPRECT ) = 0;
	virtual BOOL SetChangedBBox( LPRECT ) = 0;

	virtual BOOL SetConversionPalette( PLOGPALETTE ) = 0;

	virtual COLORREF ConvertHSVtoRGB(COLORREF) = 0;
	virtual COLORREF ConvertRGBtoHSV(COLORREF) = 0;

	virtual DWORD GetMaxBitmapWidth(void) = 0;
	virtual DWORD GetMaxBitmapDepth(void) = 0;

	// New GDraw fill types
	virtual BOOL Set3WayGraduation(DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointD) = 0;

	virtual BOOL Set3WayGraduation4(DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD) = 0;

	virtual BOOL Set4WayGraduation(DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointD) = 0;

	virtual BOOL Set4WayGraduation4(DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD) = 0;

	virtual BOOL SetTransparent3WayGraduation(DWORD Style,
		BYTE ValueA, BYTE ValueB, BYTE ValueD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointD) = 0;

	virtual BOOL SetTransparent3WayGraduation4(DWORD Style,
		BYTE ValueA, BYTE ValueB, BYTE ValueD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD) = 0;

	virtual BOOL SetTransparent4WayGraduation(DWORD Style,
		BYTE ValueA, BYTE ValueB, BYTE ValueC, BYTE ValueD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointD) = 0;

	virtual BOOL SetTransparent4WayGraduation4(DWORD Style,
		BYTE ValueA, BYTE ValueB, BYTE ValueC, BYTE ValueD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD) = 0;

	virtual BOOL SaveContext(GCONTEXT* Context) = 0;
	virtual BOOL RestoreContext(CONST GCONTEXT* Context) = 0;
};


#endif
