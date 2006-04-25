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

#ifndef INC_XaDrwOld_h
#define INC_XaDrwOld_h

// XaDrawOld is a translation layer that provides a GDraw V2 interface to
// GDraw V3. The translation is performed by maintaining a static global
// context that is passed to GDraw for all calls to Xa*Old_ functions.

// XaDrawOld is built on top of XaDraw.
#include "XaDraw.h"

// To use the translation layer simply replace all calls of GDraw using the
// appropriate Xa*Old name. See the conversion table below:
//
// GDraw V2 interface.
//
// GDraw_      -> XaDrawOld_
// GColour_    -> XaColourOld_
// GBitmap_    -> XaBitmapOld_
// G3D_        -> Xa3DOld_

// GDraw V2 function prototypes.

DWORD XaDrawOld_GetVersion();

const GCONTEXT* XaDrawOld_GetContext();

INT32 XaDrawOld_ClearLastError();
eError XaDrawOld_GetLastError();

INT32 XaColourOld_GetGraduationTableSize();
INT32 XaColourOld_GetGraduationTableLength();
INT32 XaColourOld_GetLongGraduationTableSize();
INT32 XaColourOld_GetLongGraduationTableLength();
INT32 XaColourOld_GetTransparentGraduationTableSize();
INT32 XaColourOld_GetTransparentGraduationTableLength();
INT32 XaColourOld_GetLongTransparentGraduationTableSize();
INT32 XaColourOld_GetLongTransparentGraduationTableLength();

INT32 XaDrawOld_IsFPU(BOOL Flag);

INT32 XaDrawOld_SetMemoryHandlers(
	pBYTE (*MemoryAlloc)( UINT32 Size ),
	void  (*MemoryFree) ( pBYTE Address )
);

INT32 XaDrawOld_SetStackLimit( pcVOID StackLimit );
INT32 XaDrawOld_SetStackSize( UINT32 StackSize );
INT32 XaDrawOld_Terminate();

INT32 XaDrawOld_ContextLength();
INT32 XaDrawOld_ComparePalettes(pcGCONTEXT pContext, pcLOGPALETTE Palette, BOOL Flag );

INT32 XaDrawOld_SaveContext(pGCONTEXT pContext);
INT32 XaDrawOld_RestoreContext(pcGCONTEXT pContext);

pcLOGPALETTE XaColourOld_SelectPalette( BOOL Flag );
INT32 XaColourOld_InitialiseWithPalette( pcLOGPALETTE Palette );
INT32 XaColourOld_SetDitherStyle( DitherStyle Style = DITHER_GREY_ORDERED );
INT32 XaColourOld_SetTileSmoothingFlag( BOOL Flag );
INT32 XaColourOld_SetTileFilteringFlag( BOOL Flag );
INT32 XaColourOld_SetHalftoneOrigin( INT32 x,INT32 y );
INT32 XaColourOld_SetColour( COLORREF Colour );
INT32 XaColourOld_SetSolidColour(	COLORREF Colour,
									UINT32 BPP,UINT32 Format16BPP);
INT32 XaColourOld_SetWordColour( DWORD Colour );
INT32 XaColourOld_SetInvert( DWORD Colour );
INT32 XaColourOld_SetTransparency( COLORREF Colour,DWORD Style );
INT32 XaColourOld_SetTransparencyLookupTable( pcBYTE Table );
pcBYTE XaColourOld_ReturnBrush();
pcBYTE XaColourOld_ReturnBrushRGB();
INT32 XaColourOld_AddToGraduationTable(
	COLORREF Colour,
	BOOL HSVFlag,
	pGraduationTable Table,
	UINT32 Index
);
INT32 XaColourOld_BuildGraduationTable(
	COLORREF StartColour,
	COLORREF EndColour,
	BOOL HSVFlag,
	pGraduationTable Table
);
INT32 XaColourOld_BuildGraduationTable32(
	COLORREF StartColour,
	COLORREF EndColour,
	BOOL HSVFlag,
	pGraduationTable32 Table
);
INT32 XaColourOld_BuildTransparencyTable(
	BYTE StartColour,
	BYTE EndColour,
	pTransparentGradTable Table
);
INT32 XaColourOld_SetGraduation(
	DWORD Style,
	pcGraduationTable Table,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC
);
INT32 XaColourOld_SetGraduation4(
	DWORD Style,
	pcGraduationTable Table,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD
);
INT32 XaColourOld_SetMultiRadial(
	DWORD Style,
	pcGraduationTable Table,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD
);
INT32 XaColourOld_Set3WayGraduation(
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointD
);
INT32 XaColourOld_Set3WayGraduation4(
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB,					  COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointC, pcPOINT   PointD
);
INT32 XaColourOld_Set4WayGraduation(
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB,					  pcPOINT   PointD
);
INT32 XaColourOld_Set4WayGraduation4(
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointC, pcPOINT   PointD
);
INT32 XaColourOld_SetGourand(
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointD
);
INT32 XaColourOld_SetTransparentGraduation(
	DWORD Style,
	pcTransparentGradTable Table,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC
);
INT32 XaColourOld_SetTransparentGraduation4(
	DWORD Style,
	pcTransparentGradTable Table,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD
);
INT32 XaColourOld_SetTransparent3WayGraduation(
	DWORD Style,
	BYTE	ValueA, BYTE	ValueB, BYTE	ValueD,
	pcPOINT PointA, pcPOINT PointB, pcPOINT PointD
);
INT32 XaColourOld_SetTransparent3WayGraduation4(
	DWORD Style,
	BYTE	ValueA, BYTE	ValueB,				    BYTE	ValueD,
	pcPOINT PointA, pcPOINT PointB, pcPOINT PointC, pcPOINT PointD
);
INT32 XaColourOld_SetTransparent4WayGraduation(
	DWORD Style,
	BYTE	ValueA, BYTE	ValueB, BYTE	ValueC, BYTE	ValueD,
	pcPOINT PointA, pcPOINT PointB,					pcPOINT PointD
);
INT32 XaColourOld_SetTransparent4WayGraduation4(
	DWORD Style,
	BYTE	ValueA, BYTE	ValueB, BYTE	ValueC, BYTE	ValueD,
	pcPOINT PointA, pcPOINT PointB, pcPOINT PointC, pcPOINT PointD
);
INT32 XaBitmapOld_SetBias(double);
INT32 XaBitmapOld_SetGain(double);
INT32 XaBitmapOld_SetBrightness(double);
INT32 XaBitmapOld_SetContrast(double);
INT32 XaBitmapOld_SetGamma(double);
INT32 XaBitmapOld_SetPostGamma(double);
INT32 XaBitmapOld_SetSaturation(double);
INT32 XaBitmapOld_SetContone( UINT32 uContoneStyle, COLORREF rgbStart=0x000000, COLORREF rgbEnd=0xFFFFFF );
INT32 XaBitmapOld_SetInputRange( BYTE uStart=0x00, BYTE uEnd=0xFF );
INT32 XaBitmapOld_SetOutputRange( BYTE uStart=0x00, BYTE uEnd=0xFF );
INT32 XaColourOld_SetTilePattern(
	pcBITMAPINFOHEADER BitmapInfo,
	pcBYTE Bitmap,
	DWORD Style,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	COLORREF DefaultColour = 0,
	pcBGRT TranslationTable = 0,
	pcBYTE   RedTranslationTable = 0,
	pcBYTE GreenTranslationTable = 0,
	pcBYTE  BlueTranslationTable = 0,
	pcBYTE TransparencyTranslationTable = 0,
	INT32 TileOffset = 0
);
INT32 XaColourOld_SetTilePattern4(
	pcBITMAPINFOHEADER BitmapInfo,
	pcBYTE Bitmap,
	DWORD Style,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD,
	COLORREF DefaultColour = 0,
	pcBGRT TranslationTable = 0,
	pcBYTE   RedTranslationTable = 0,
	pcBYTE GreenTranslationTable = 0,
	pcBYTE  BlueTranslationTable = 0,
	pcBYTE TransparencyTranslationTable = 0,
	INT32 TileOffset = 0
);
INT32 XaColourOld_SetTransparentTilePattern(
	pcBITMAPINFOHEADER BitmapInfo,
	pcBYTE Bitmap,
	DWORD Style,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	BYTE DefaultColour = 0,
	pcBYTE TransparencyTranslationTable = 0,
	INT32 TileOffset = 0
);
INT32 XaColourOld_SetTransparentTilePattern4(
	pcBITMAPINFOHEADER BitmapInfo,
	pcBYTE Bitmap,
	DWORD Style,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD,
	BYTE DefaultColour = 0,
	pcBYTE TransparencyTranslationTable = 0,
	INT32 TileOffset = 0
);
INT32 XaColourOld_ConvertBitmap(
	pcBITMAPINFOHEADER SBitmapInfo, pcBYTE SBitmap,
	pcBITMAPINFOHEADER DBitmapInfo,  pBYTE DBitmap,
	DWORD Dither
);
INT32 XaColourOld_SetConversionPalette( pcLOGPALETTE pPalette );
INT32 XaColourOld_SetMaxDiffusionError( UINT32 MaxError );
INT32 XaColourOld_ScaleBitmap(
	pcBITMAPINFOHEADER SBitmapInfo, pcBYTE SBitmap,
	pcBITMAPINFOHEADER DBitmapInfo,  pBYTE DBitmap,
	UINT32 Channels
);

INT32 XaColourOld_SetGreyConversionValues( UINT32 Red,UINT32 Green,UINT32 Blue );

COLORREF XaColourOld_ConvertHSVtoRGB( COLORREF hsv );
COLORREF XaColourOld_ConvertRGBtoHSV( COLORREF rgb );

INT32 XaColourOld_SetSeparationTables(
	pcBGR    CyanSeparationTable,
	pcBGR MagentaSeparationTable,
	pcBGR  YellowSeparationTable,
	pcBGR   BlackSeparationTable,
	pcBYTE UnderColourRemovalTable,
	pcBYTE BlackGenerationTable
);
INT32 XaColourOld_SetBitmapConversionTable(
	pcBGR BitmapConversionTable
);
INT32 XaDrawOld_SetDIBitmap(
	pcBITMAPINFOHEADER BitmapInfo,
	pBYTE Bitmap,
	UINT32 Format16BPP = FORMAT16BPP_555
);
INT32 XaDrawOld_SetInvertedDIBitmap(
	pcBITMAPINFOHEADER BitmapInfo,
	pBYTE Bitmap,
	UINT32 Format16BPP = FORMAT16BPP_555
);
INT32 XaDrawOld_SetMatrix( pcGMATRIX Matrix );
INT32 XaDrawOld_MaxScale ( pcGMATRIX Matrix,pcRECT BBox );
INT32 XaDrawOld_GetMaxBitmapWidth();
INT32 XaDrawOld_GetMaxBitmapDepth();
INT32 XaDrawOld_SetAntialiasFlag( BOOL Flag );
INT32 XaDrawOld_SetDashAdjustmentFlag( BOOL Flag );
INT32 XaDrawOld_SetHintingFlag( BOOL Flag );
INT32 XaDrawOld_SetFlatness( UINT32 Flatness );
INT32 XaDrawOld_SetMiterLimit( UINT32 MiterLimit );
INT32 XaDrawOld_FillRectangle( pcRECT Rectangle );
INT32 XaDrawOld_FillPoint( pcPOINT Point );
INT32 XaDrawOld_FillPath(
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding
);
INT32 XaDrawOld_FillTriangle(
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	DWORD Flags
);
INT32 XaDrawOld_HintPath(
	pPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	BOOL Close,
	UINT32 LineWidth
);
INT32 XaDrawOld_StrokePath(
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	BOOL Close,
	UINT32 LineWidth,
	DWORD LineCaps,
	DWORD LineJoin,
	pcDashType Dash
);
INT32 XaDrawOld_StrokePathToPath(
	pcPOINT IPoints,
	pcBYTE  ITypes,
	UINT32 ILength,
	pPOINT OPoints,
	pBYTE  OTypes,
	UINT32 OLength,
	BOOL Close,
	UINT32 LineWidth,
	DWORD LineCaps,
	DWORD LineJoin,
	pcDashType Dash
);
INT32 XaDrawOld_CalcStrokeBBox(
	pcPOINT IPoints,
	pcBYTE  ITypes,
	UINT32 ILength,
	pRECT Rect,
	BOOL Close,
	UINT32 LineWidth,
	DWORD LineCaps,
	DWORD LineJoin,
	pcDashType Dash
);
INT32 XaDrawOld_IsOverlap(
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding
);
INT32 XaDrawOld_IsStrokeOverlap(
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	BOOL Close,
	UINT32 LineWidth,
	DWORD LineCaps,
	DWORD LineJoin,
	pcDashType Dash
);
INT32 XaDrawOld_GetStatistics(
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding,
	pSTATISTICS Stats
);
INT32 XaDrawOld_MakeRegion(
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding,
	pREGION ORegion,
	UINT32 OLength
);
INT32 XaDrawOld_MakeUnclippedRegion(
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding,
	pREGION ORegion,
	UINT32 OLength
);
INT32 XaDrawOld_FillRegion(
	pcREGION Region,
	pcPOINT Offset
);
INT32 XaDrawOld_ClipRectangle( pcRECT Rectangle );
INT32 XaDrawOld_DeviceClipRectangle( pcRECT Rectangle );
INT32 XaDrawOld_GetDeviceClipRectangle( pRECT Rectangle );
INT32 XaDrawOld_ClipRegion( pcREGION Region );
pcREGION XaDrawOld_GetClipRegion();
INT32 XaDrawOld_CalcBBox(
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	pRECT Rect,
	BOOL Flatten
);
INT32 XaDrawOld_TransformPath(
	pcPOINT IPoints,
	 pPOINT OPoints,
	UINT32 Length,
	pcGMATRIX Matrix
);

INT32 XaDrawOld_ScrollBitmap( INT32 x,INT32 y );

INT32 XaDrawOld_ClearChangedBBox();
INT32 XaDrawOld_GetChangedBBox( pRECT Rectangle );
INT32 XaDrawOld_SetChangedBBox( pcRECT Rectangle );

INT32 XaDrawOld_SetBevelContrast ( UINT32 Contrast );
INT32 XaDrawOld_SetBevelLightness( UINT32 Contrast );
INT32 XaDrawOld_SetBevelDarkness ( UINT32 Contrast );
INT32 XaDrawOld_TranslateBevelValue( BYTE Index, BYTE Colour );
/*
INT32 XaSpriteOld_PlotTile(
	pcBITMAPINFOHEADER BitmapInfo,
	pcBYTE Bitmap,
	DWORD Style,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcBGRT TranslationTable = 0,
	pcBYTE   RedTranslationTable = 0,
	pcBYTE GreenTranslationTable = 0,
	pcBYTE  BlueTranslationTable = 0,
	pcBYTE TransparencyTranslationTable = 0
);
INT32 XaSpriteOld_PlotTile4(
	pcBITMAPINFOHEADER BitmapInfo,
	pcBYTE Bitmap,
	DWORD Style,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD,
	pcBGRT TranslationTable = 0,
	pcBYTE   RedTranslationTable = 0,
	pcBYTE GreenTranslationTable = 0,
	pcBYTE  BlueTranslationTable = 0,
	pcBYTE TransparencyTranslationTable = 0
);

INT32 Xa3DOld_DefineView(
	INT32 nObserverDistance,
	COLORREF Background = 0,
	pcBITMAPINFOHEADER pTextureInfo = NULL,
	pcBYTE pTextureBitmap = NULL,
	pcBGRT pTextureColours = NULL,
	UINT32 uTextureXOffset = 0,
	UINT32 uTextureYOffset = 0,
	pcBYTE pOverlayBitmap = NULL,
	pcBITMAPINFOHEADER pShadowInfo = NULL,
	pcBYTE pShadowBitmap = NULL,
	COLORREF ShadowColour = 0,
	BOOL bTransparent = NULL,
	BOOL bAlpha = NULL,
	BOOL bTransparentTexture = NULL,
	UINT32 uFade = 0
);
INT32 Xa3DOld_AddTriangleToView(
	pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
	pcColour   pColourA, pcColour   pColourB, pcColour   pColourC,
	BOOL bForward
);
INT32 Xa3DOld_AddFlatTriangleToView(
	pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
	pcColour pFlatColour,
	BOOL bForward
);
INT32 Xa3DOld_GetViewBBox( pRECT pBBox );
INT32 Xa3DOld_PlotView();
*/
#endif // INC_XaDrwOld_h
