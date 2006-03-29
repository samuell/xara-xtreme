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

#ifndef INC_XaDraw_h
#define INC_XaDraw_h

// XaDraw is an abstraction layer for GDraw V3. The abstraction layer
// compiles away leaving the program calling GDraw directly if CONFIG_DEBUG_XADRAW
// is not defined. If CONFIG_DEBUG_XADRAW is defined all calls to GDraw go via debug
// wrapper functions. The wrapper functions currently display the name of each
// function as it is called and displays the value of the pointer to the context.
// The context pointer and function name can be used to determine which call failed
// and what the current stack pointer is if a call to GDraw crashes.

// To use the abstraction layer simply replace all calls to GDraw with the
// appropriate Xa name. See conversion table below:
//
// GDraw V3 interface.
//
// GDraw_      -> XaDraw_
// GColour_    -> XaColour_
// GSprite_    -> XaSprite_
// G3D_        -> Xa3D_

// Include the Oil specific version of GDraw
//#include "xadrwoil.h"
#include "xagdraw.h"

// If CONFIG_DEBUG_XADRAW is defined then declare a structure that contains pointers to
// all the GDraw functions. These are used to implemented the debugging functionality
// by inserting an additional layer between camelot and GDraw. See GDraw for more
// information about the individual GDraw functions.

#if defined(CONFIG_DEBUG_XADRAW)

typedef struct tagXaDrawFns
{
	DWORD (*pfnXaDraw_GetVersion)() ;


	INT32 (*pfnXaDraw_ClearLastError)(pGCONTEXT pContext) ;
	eError (*pfnXaDraw_GetLastError)(pcGCONTEXT pContext) ;
	pcCHAR (*pfnXaDraw_GetLastErrorText)(pcGCONTEXT pContext) ;

	INT32 (*pfnXaDraw_Initialise)(pGCONTEXT pContext,pcGCONTEXT pOldContext) ;
	INT32 (*pfnXaDraw_CopyContext)(pcGCONTEXT pContext,pGCONTEXT pNewContext) ;

	INT32 (*pfnXaColour_GetGraduationTableSize)(pcGCONTEXT pContext) ;
	INT32 (*pfnXaColour_GetGraduationTableLength)() ;
	INT32 (*pfnXaColour_GetLongGraduationTableSize)(pcGCONTEXT pContext) ;
	INT32 (*pfnXaColour_GetLongGraduationTableLength)() ;
	INT32 (*pfnXaColour_GetTransparentGraduationTableSize)() ;
	INT32 (*pfnXaColour_GetTransparentGraduationTableLength)() ;
	INT32 (*pfnXaColour_GetLongTransparentGraduationTableSize)() ;
	INT32 (*pfnXaColour_GetLongTransparentGraduationTableLength)() ;

	INT32 (*pfnXaDraw_SetMemoryHandlers)(
		pGCONTEXT pContext,
		pBYTE (*MemoryAlloc)( UINT32 Size ),
		void  (*MemoryFree) ( pBYTE Address )
	) ;
	INT32 (*pfnXaDraw_SetStackLimit)( pGCONTEXT pContext,pcVOID StackLimit ) ;
	INT32 (*pfnXaDraw_SetStackSize)( pGCONTEXT pContext,UINT32 StackSize ) ;
	INT32 (*pfnXaDraw_Terminate)(pGCONTEXT pContext) ;
	INT32 (*pfnXaDraw_Clean)(pGCONTEXT pContext) ;

	INT32 (*pfnXaDraw_ContextLength)() ;
	INT32 (*pfnXaDraw_ComparePalettes)( pcGCONTEXT pContext,pcLOGPALETTE Palette,BOOL Flag ) ;

	pcLOGPALETTE (*pfnXaColour_SelectPalette)( pGCONTEXT pContext,BOOL Flag ) ;
	INT32 (*pfnXaColour_InitialiseWithPalette)( pGCONTEXT pContext,pcLOGPALETTE Palette ) ;
	INT32 (*pfnXaColour_SetDitherStyle)( pGCONTEXT pContext,DitherStyle Style ) ;
	INT32 (*pfnXaColour_SetTileSmoothingFlag)( pGCONTEXT pContext,BOOL Flag ) ;
	INT32 (*pfnXaColour_SetTileFilteringFlag)( pGCONTEXT pContext,BOOL Flag ) ;
	INT32 (*pfnXaColour_Sharpen)( pGCONTEXT pContext,INT32 Sharpen ) ;
	INT32 (*pfnXaColour_Blur)   ( pGCONTEXT pContext,INT32 Blur    ) ;
	INT32 (*pfnXaColour_SetHalftoneOrigin)( pGCONTEXT pContext, INT32 x,INT32 y ) ;
	INT32 (*pfnXaColour_SetColour)( pGCONTEXT pContext,COLORREF Colour ) ;
	INT32 (*pfnXaColour_SetSolidColour)( pGCONTEXT pContext,COLORREF Colour,UINT32 BPP,UINT32 Format16BPP ) ;
	INT32 (*pfnXaColour_SetWordColour)( pGCONTEXT pContext,DWORD Colour ) ;
	INT32 (*pfnXaColour_SetInvert)( pGCONTEXT pContext,DWORD Colour ) ;
	INT32 (*pfnXaColour_SetTransparency)( pGCONTEXT pContext,COLORREF Colour,DWORD Style ) ;
	INT32 (*pfnXaColour_SetTransparencyLookupTable)( pGCONTEXT pContext,pcBYTE Table ) ;
	pcBYTE (*pfnXaColour_ReturnBrush)(pcGCONTEXT pContext) ;
	pcBYTE (*pfnXaColour_ReturnBrushRGB)(pcGCONTEXT pContext) ;
	INT32 (*pfnXaColour_AddToGraduationTable)(
		pGCONTEXT pContext,
		COLORREF Colour,
		BOOL HSVFlag,
		pGraduationTable Table,
		UINT32 Index
	) ;
	INT32 (*pfnXaColour_BuildGraduationTable)(
		pGCONTEXT pContext,
		COLORREF StartColour,
		COLORREF EndColour,
		BOOL HSVFlag,
		pGraduationTable Table
	) ;

	INT32 (*pfnXaColour_BuildGraduationTable32)(
		pGCONTEXT pContext,
		COLORREF StartColour,
		COLORREF EndColour,
		BOOL HSVFlag,
		pGraduationTable32 Table
	) ;
	INT32 (*pfnXaColour_BuildTransparencyTable)(
		pGCONTEXT pContext,
		BYTE StartColour,
		BYTE EndColour,
		pTransparentGradTable Table
	) ;
	INT32 (*pfnXaColour_SetGraduation)(
		pGCONTEXT pContext,
		DWORD Style,
		pcGraduationTable Table,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC
	) ;
	INT32 (*pfnXaColour_SetGraduation4)(
		pGCONTEXT pContext,
		DWORD Style,
		pcGraduationTable Table,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC,
		pcPOINT PointD
	) ;
	INT32 (*pfnXaColour_Set3WayGraduation)(
		pGCONTEXT pContext,
		DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
		pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointD
	) ;
	INT32 (*pfnXaColour_Set3WayGraduation4)(
		pGCONTEXT pContext,
		DWORD Style,
		COLORREF ColourA, COLORREF ColourB,					  COLORREF ColourD,
		pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointC, pcPOINT   PointD
	) ;
	INT32 (*pfnXaColour_Set4WayGraduation)(
		pGCONTEXT pContext,
		DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
		pcPOINT   PointA, pcPOINT   PointB,					  pcPOINT   PointD
	) ;
	INT32 (*pfnXaColour_Set4WayGraduation4)(
		pGCONTEXT pContext,
		DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
		pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointC, pcPOINT   PointD
	) ;
	INT32 (*pfnXaColour_SetGourand)(
		pGCONTEXT pContext,
		DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
		pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointD
	) ;
	INT32 (*pfnXaColour_SetTransparentGraduation)(
		pGCONTEXT pContext,
		DWORD Style,
		pcTransparentGradTable Table,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC
	) ;
	INT32 (*pfnXaColour_SetTransparentGraduation4)(
		pGCONTEXT pContext,
		DWORD Style,
		pcTransparentGradTable Table,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC,
		pcPOINT PointD
	) ;
	INT32 (*pfnXaColour_SetTransparent3WayGraduation)(
		pGCONTEXT pContext,
		DWORD Style,
		BYTE	ValueA, BYTE	ValueB, BYTE	ValueD,
		pcPOINT PointA, pcPOINT PointB, pcPOINT PointD
	) ;
	INT32 (*pfnXaColour_SetTransparent3WayGraduation4)(
		pGCONTEXT pContext,
		DWORD Style,
		BYTE	ValueA, BYTE	ValueB,				    BYTE	ValueD,
		pcPOINT PointA, pcPOINT PointB, pcPOINT PointC, pcPOINT PointD
	) ;
	INT32 (*pfnXaColour_SetTransparent4WayGraduation)(
		pGCONTEXT pContext,
		DWORD Style,
		BYTE	ValueA, BYTE	ValueB, BYTE	ValueC, BYTE	ValueD,
		pcPOINT PointA, pcPOINT PointB,					pcPOINT PointD
	) ;
	INT32 (*pfnXaColour_SetTransparent4WayGraduation4)(
		pGCONTEXT pContext,
		DWORD Style,
		BYTE	ValueA, BYTE	ValueB, BYTE	ValueC, BYTE	ValueD,
		pcPOINT PointA, pcPOINT PointB, pcPOINT PointC, pcPOINT PointD
	) ;
	INT32 (*pfnXaBitmap_SetBias)(pGCONTEXT pContext,double fBias) ;
	INT32 (*pfnXaBitmap_SetGain)(pGCONTEXT pContext,double fGain) ;
	INT32 (*pfnXaBitmap_SetContone)( pGCONTEXT pContext,UINT32 uContoneStyle,COLORREF rgbStart,COLORREF rgbEnd ) ;
	INT32 (*pfnXaBitmap_SetTransparencyRamp)( pGCONTEXT pContext,BYTE uStart,BYTE uEnd ) ;
	INT32 (*pfnXaColour_SetTilePattern)(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER BitmapInfo,
		pcBYTE Bitmap,
		DWORD Style,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC,
		COLORREF DefaultColour,
		pcBGRT TranslationTable,
		pcBYTE   RedTranslationTable,
		pcBYTE GreenTranslationTable,
		pcBYTE  BlueTranslationTable,
		pcBYTE TransparencyTranslationTable,
		INT32 TileOffset
	) ;
	INT32 (*pfnXaColour_SetTilePattern4)(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER BitmapInfo,
		pcBYTE Bitmap,
		DWORD Style,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC,
		pcPOINT PointD,
		COLORREF DefaultColour,
		pcBGRT TranslationTable,
		pcBYTE   RedTranslationTable,
		pcBYTE GreenTranslationTable,
		pcBYTE  BlueTranslationTable,
		pcBYTE TransparencyTranslationTable,
		INT32 TileOffset
	) ;
	INT32 (*pfnXaColour_SetTransparentTilePattern)(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER BitmapInfo,
		pcBYTE Bitmap,
		DWORD Style,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC,
		BYTE DefaultColour,
		pcBYTE TransparencyTranslationTable,
		INT32 TileOffset
	) ;
	INT32 (*pfnXaColour_SetTransparentTilePattern4)(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER BitmapInfo,
		pcBYTE Bitmap,
		DWORD Style,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC,
		pcPOINT PointD,
		BYTE DefaultColour,
		pcBYTE TransparencyTranslationTable,
		INT32 TileOffset
	) ;
	INT32 (*pfnXaColour_ConvertBitmap)(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER SBitmapInfo, pcBYTE SBitmap,
		pcBITMAPINFOHEADER DBitmapInfo,  pBYTE DBitmap,
		DWORD Dither
	) ;
	INT32 (*pfnXaColour_SetConversionPalette)( pGCONTEXT pContext,pcLOGPALETTE pPalette ) ;
	INT32 (*pfnXaColour_SetMaxDiffusionError)( pGCONTEXT pContext,UINT32 MaxError ) ;
	INT32 (*pfnXaColour_ScaleBitmap)(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER SBitmapInfo, pcBYTE SBitmap,
		pcBITMAPINFOHEADER DBitmapInfo,  pBYTE DBitmap,
		UINT32 Channels
	) ;

	INT32 (*pfnXaColour_SetGreyConversionValues)( pGCONTEXT pContext,UINT32 Red,UINT32 Green,UINT32 Blue ) ;

	COLORREF (*pfnXaColour_ConvertHSVtoRGB)( COLORREF hsv )	;
	COLORREF (*pfnXaColour_ConvertRGBtoHSV)( COLORREF rgb )	;

//
	INT32 (*pfnXaColour_SetSeparationTables)(
		pGCONTEXT pContext,
		pcBGR    CyanSeparationTable,
		pcBGR MagentaSeparationTable,
		pcBGR  YellowSeparationTable,
		pcBGR   BlackSeparationTable,
		pcBYTE UnderColourRemovalTable,
		pcBYTE BlackGenerationTable
	) ;
	INT32 (*pfnXaColour_SetBitmapConversionTable)(
		pGCONTEXT pContext,
		pcBGR BitmapConversionTable
	) ;

	INT32 (*pfnXaDraw_SetDIBitmap)(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER BitmapInfo,
		pBYTE Bitmap,
		UINT32 Format16BPP
	) ;
	INT32 (*pfnXaDraw_SetInvertedDIBitmap)(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER BitmapInfo,
		pBYTE Bitmap,
		UINT32 Format16BPP
	) ;
	INT32 (*pfnXaDraw_SetMatrix)( pGCONTEXT pContext,pcGMATRIX Matrix ) ;
	INT32 (*pfnXaDraw_MaxScale) ( pGCONTEXT pContext,pcGMATRIX Matrix,pcRECT BBox ) ;
	INT32 (*pfnXaDraw_GetMaxBitmapWidth)() ;
	INT32 (*pfnXaDraw_GetMaxBitmapDepth)() ;
	INT32 (*pfnXaDraw_SetAntialiasFlag)( pGCONTEXT pContext,BOOL Flag ) ;
	INT32 (*pfnXaDraw_SetDashAdjustmentFlag)( pGCONTEXT pContext,BOOL Flag ) ;
	INT32 (*pfnXaDraw_SetHintingFlag)( pGCONTEXT pContext,BOOL Flag ) ;
	INT32 (*pfnXaDraw_SetFlatness)( pGCONTEXT pContext,UINT32 Flatness ) ;
	INT32 (*pfnXaDraw_SetMiterLimit)( pGCONTEXT pContext,UINT32 MiterLimit ) ;
	INT32 (*pfnXaDraw_FillRectangle)( pGCONTEXT pContext,pcRECT Rectangle ) ;
	INT32 (*pfnXaDraw_FillPoint)( pGCONTEXT pContext,pcPOINT Point ) ;
	INT32 (*pfnXaDraw_FillPath)(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		UINT32 Winding
	) ;
	INT32 (*pfnXaDraw_InitialiseFillPath)( pGCONTEXT pContext,UINT32 Winding,BOOL DontPlot ) ;
	INT32 (*pfnXaDraw_FillPathLine)( pGCONTEXT pContext,pcFVector2D Points, BOOL Reverse ) ;
	INT32 (*pfnXaDraw_DoFillPath)( pGCONTEXT pContext ) ;
	INT32 (*pfnXaDraw_FillTriangle)(
		pGCONTEXT pContext,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC,
		DWORD Flags
	) ;
	INT32 (*pfnXaDraw_HintPath)(
		pGCONTEXT pContext,
		pPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		BOOL Close,
		UINT32 LineWidth
	) ;
	INT32 (*pfnXaDraw_StrokePath)(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		BOOL Close,
		UINT32 LineWidth,
		DWORD LineCaps,
		DWORD LineJoin,
		pcDashType Dash
	) ;
	INT32 (*pfnXaDraw_StrokePathToPath)(
		pGCONTEXT pContext,
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
	) ;
	INT32 (*pfnXaDraw_CalcStrokeBBox)(
		pGCONTEXT pContext,
		pcPOINT IPoints,
		pcBYTE  ITypes,
		UINT32 ILength,
		pRECT Rect,
		BOOL Close,
		UINT32 LineWidth,
		DWORD LineCaps,
		DWORD LineJoin,
		pcDashType Dash
	) ;
	INT32 (*pfnXaDraw_IsOverlap)(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		UINT32 Winding
	) ;
	INT32 (*pfnXaDraw_IsStrokeOverlap)(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		BOOL Close,
		UINT32 LineWidth,
		DWORD LineCaps,
		DWORD LineJoin,
		pcDashType Dash
	) ;
	INT32 (*pfnXaDraw_GetStatistics)(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		UINT32 Winding,
		pSTATISTICS Stats
	) ;
	INT32 (*pfnXaDraw_MakeRegion)(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		UINT32 Winding,
		pREGION ORegion,
		UINT32 OLength
	) ;
	INT32 (*pfnXaDraw_MakeUnclippedRegion)(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		UINT32 Winding,
		pREGION ORegion,
		UINT32 OLength
	) ;
	INT32 (*pfnXaDraw_FillRegion)(
		pGCONTEXT pContext,
		pcREGION Region,
		pcPOINT Offset
	) ;
	INT32 (*pfnXaDraw_ClipRectangle)( pGCONTEXT pContext,pcRECT Rectangle ) ;
	INT32 (*pfnXaDraw_DeviceClipRectangle)( pGCONTEXT pContext,pcRECT Rectangle ) ;
	INT32 (*pfnXaDraw_GetDeviceClipRectangle)( pcGCONTEXT pContext,pRECT Rectangle ) ;
	INT32 (*pfnXaDraw_ClipRegion)( pGCONTEXT pContext,pcREGION Region ) ;
	pcREGION (*pfnXaDraw_GetClipRegion)(pGCONTEXT pContext) ;
	INT32 (*pfnXaDraw_CalcSimpleBBox)(
		pcPOINT Points,
		UINT32 Length,
		pRECT Rect
	) ;
	INT32 (*pfnXaDraw_CalcBBox)(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		pRECT Rect,
		BOOL Flatten
	) ;
	INT32 (*pfnXaDraw_TransformPath)(
		pGCONTEXT pContext,
		pcPOINT IPoints,
		 pPOINT OPoints,
		UINT32 Length,
		pcGMATRIX Matrix
	) ;
	INT32 (*pfnXaDraw_ScrollBitmap)( pGCONTEXT pContext, INT32 x,INT32 y ) ;

	INT32 (*pfnXaDraw_ClearChangedBBox)(pGCONTEXT pContext) ;
	INT32 (*pfnXaDraw_GetChangedBBox)( pcGCONTEXT pContext, pRECT Rectangle ) ;
	INT32 (*pfnXaDraw_SetChangedBBox)(  pGCONTEXT pContext,pcRECT Rectangle ) ;

	INT32 (*pfnXaDraw_SetBevelContrast) ( pGCONTEXT pContext,UINT32 Contrast ) ;
	INT32 (*pfnXaDraw_SetBevelLightness)( pGCONTEXT pContext,UINT32 Contrast ) ;
	INT32 (*pfnXaDraw_SetBevelDarkness) ( pGCONTEXT pContext,UINT32 Contrast ) ;
	INT32 (*pfnXaDraw_TranslateBevelValue)( pcGCONTEXT pContext, BYTE Index, BYTE Colour ) ;



	INT32 (*pfnXaSprite_PlotTile)(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER BitmapInfo,
		pcBYTE Bitmap,
		DWORD Style,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC,
		pcBGRT TranslationTable,
		pcBYTE   RedTranslationTable,
		pcBYTE GreenTranslationTable,
		pcBYTE  BlueTranslationTable,
		pcBYTE TransparencyTranslationTable
	) ;
	INT32 (*pfnXaSprite_PlotTile4)(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER BitmapInfo,
		pcBYTE Bitmap,
		DWORD Style,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC,
		pcPOINT PointD,
		pcBGRT TranslationTable,
		pcBYTE   RedTranslationTable,
		pcBYTE GreenTranslationTable,
		pcBYTE  BlueTranslationTable,
		pcBYTE TransparencyTranslationTable
	) ;


	INT32 (*pfnXa3D_SetTruePerspectiveFlag)( pGCONTEXT pContext,BOOL Flag ) ;
	INT32 (*pfnXa3D_DefineView)(
		pGCONTEXT pContext,
		INT32 nObserverDistance,
		COLORREF Background,
		pcBITMAPINFOHEADER pTextureInfo ,
		pcBYTE pTextureBitmap ,
		pcBGRT pTextureColours ,
		UINT32 uTextureXOffset,
		UINT32 uTextureYOffset,
		pcBYTE pOverlayBitmap ,
		pcBITMAPINFOHEADER pShadowInfo ,
		pcBYTE pShadowBitmap ,
		COLORREF ShadowColour,
		BOOL bTransparent ,
		BOOL bAlpha ,
		BOOL bTransparentTexture ,
		UINT32 uFade
	) ;
	INT32 (*pfnXa3D_DefineTexture)(
		pGCONTEXT pContext,
		pcBITMAP pBitmap,
		BOOL bScaleDown,
		BOOL bRepeating
	) ;
	INT32 (*pfnXa3D_Define2ndTexture)(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER pBitmapInfo,
		pcBYTE pBitmap,
		pcBGRT pRGBIndex,
		BOOL bTint,
		COLORREF TintColour
	) ;
	INT32 (*pfnXa3D_ConvertColourFormat)( pcColour pSColour, pColour pDColour ) ;
	INT32 (*pfnXa3D_AddTriangleToView)(
		pGCONTEXT pContext,
		pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
		pcColour   pColourA, pcColour   pColourB, pcColour   pColourC,
		BOOL bForward
	) ;
	INT32 (*pfnXa3D_AddFlatTriangleToView)(
		pGCONTEXT pContext,
		pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
		pcColour pFlatColour,
		BOOL bForward
	) ;
	INT32 (*pfnXa3D_AddTextureToView)(
		pGCONTEXT pContext,
		pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
		pcColour   pColourA, pcColour   pColourB, pcColour   pColourC,
		pcColour   pColourA2,pcColour   pColourB2,pcColour   pColourC2,
		pcFVector2D pPointA2,pcFVector2D pPointB2,pcFVector2D pPointC2,
		BOOL bForward
	) ;
	INT32 (*pfnXa3D_AddFlatTextureToView)(
		pGCONTEXT pContext,
		pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
		pcColour pFlatColour,
		pcColour pFlatColour2,
		pcFVector2D pPointA2,pcFVector2D pPointB2,pcFVector2D pPointC2,
		BOOL bForward
	) ;
	INT32 (*pfnXa3D_AddTexturesToView)(
		pGCONTEXT pContext,
		pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
		pcColour   pColourA, pcColour   pColourB, pcColour   pColourC,
		pcColour   pColourA2,pcColour   pColourB2,pcColour   pColourC2,
		pcFVector2D pPointA2,pcFVector2D pPointB2,pcFVector2D pPointC2,
		pcFVector2D pPointA3,pcFVector2D pPointB3,pcFVector2D pPointC3,
		BOOL bForward
	) ;
	INT32 (*pfnXa3D_AddFlatTexturesToView)(
		pGCONTEXT pContext,
		pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
		pcColour pFlatColour,
		pcColour pFlatColour2,
		pcFVector2D pPointA2,pcFVector2D pPointB2,pcFVector2D pPointC2,
		pcFVector2D pPointA3,pcFVector2D pPointB3,pcFVector2D pPointC3,
		BOOL bForward
	) ;
	INT32 (*pfnXa3D_SetSideFaceFlag)( pGCONTEXT pContext,BOOL bFlag ) ;
	INT32 (*pfnXa3D_GetViewBBox)( pcGCONTEXT pContext,pRECT pBBox ) ;
	INT32 (*pfnXa3D_PlotView)(pGCONTEXT pContext) ;
	INT32 (*pfnXaDraw_SetAntialiasQualityFlag)( pGCONTEXT pContext,BOOL Flag ) ;
}
XA_DRAW_FNS, *PXA_DRAW_FNS;

// Defines that map Xa*_ calls to the debug versions of the functions implemented
// in XaDraw.cpp

#define XaDraw_GetVersion		(g_XaFns.pfnXaDraw_GetVersion)
#define XaDraw_ClearLastError	(g_XaFns.pfnXaDraw_ClearLastError)
#define XaDraw_GetLastError		(g_XaFns.pfnXaDraw_GetLastError)
#define XaDraw_GetLastErrorText	(g_XaFns.pfnXaDraw_GetLastErrorText)

#define XaDraw_Initialise		(g_XaFns.pfnXaDraw_Initialise)
#define XaDraw_CopyContext		(g_XaFns.pfnXaDraw_CopyContext)

#define XaColour_GetGraduationTableSize	\
								(g_XaFns.pfnXaColour_GetGraduationTableSize)
#define XaColour_GetGraduationTableLength \
								(g_XaFns.pfnXaColour_GetGraduationTableLength)
#define XaColour_GetLongGraduationTableSize \
								(g_XaFns.pfnXaColour_GetLongGraduationTableSize)
#define XaColour_GetLongGraduationTableLength \
								(g_XaFns.pfnXaColour_GetLongGraduationTableLength)
#define XaColour_GetTransparentGraduationTableSize \
								(g_XaFns.pfnXaColour_GetTransparentGraduationTableSize)
#define XaColour_GetTransparentGraduationTableLength \
								(g_XaFns.pfnXaColour_GetTransparentGraduationTableLength)
#define XaColour_GetLongTransparentGraduationTableSize \
								(g_XaFns.pfnXaColour_GetLongTransparentGraduationTableSize)
#define XaColour_GetLongTransparentGraduationTableLength \
								(g_XaFns.pfnXaColour_GetLongTransparentGraduationTableLength)


#define XaDraw_SetMemoryHandlers	(g_XaFns.pfnXaDraw_SetMemoryHandlers)
#define XaDraw_SetStackLimit		(g_XaFns.pfnXaDraw_SetStackLimit)
#define XaDraw_SetStackSize			(g_XaFns.pfnXaDraw_SetStackSize)
#define XaDraw_Terminate			(g_XaFns.pfnXaDraw_Terminate)
#define XaDraw_Clean				(g_XaFns.pfnXaDraw_Clean)


#define XaDraw_ContextLength		(g_XaFns.pfnXaDraw_ContextLength)
#define XaDraw_ComparePalettes		(g_XaFns.pfnXaDraw_ComparePalettes)


#define XaColour_SelectPalette		(g_XaFns.pfnXaColour_SelectPalette)
#define XaColour_InitialiseWithPalette	\
									(g_XaFns.pfnXaColour_InitialiseWithPalette)
#define XaColour_SetDitherStyle		(g_XaFns.pfnXaColour_SetDitherStyle)
#define XaColour_SetTileSmoothingFlag \
									(g_XaFns.pfnXaColour_SetTileSmoothingFlag)
#define XaColour_SetTileFilteringFlag \
									(g_XaFns.pfnXaColour_SetTileFilteringFlag)
#define XaColour_Sharpen			(g_XaFns.pfnXaColour_Sharpen)
#define XaColour_Blur				(g_XaFns.pfnXaColour_Blur)
#define XaColour_SetHalftoneOrigin	(g_XaFns.pfnXaColour_SetHalftoneOrigin)
#define XaColour_SetColour			(g_XaFns.pfnXaColour_SetColour)
#define XaColour_SetSolidColour		(g_XaFns.pfnXaColour_SetSolidColour)
#define XaColour_SetWordColour		(g_XaFns.pfnXaColour_SetWordColour)
#define XaColour_SetInvert			(g_XaFns.pfnXaColour_SetInvert)
#define XaColour_SetTransparency	(g_XaFns.pfnXaColour_SetTransparency)
#define XaColour_SetTransparencyLookupTable \
									(g_XaFns.pfnXaColour_SetTransparencyLookupTable)
#define XaColour_ReturnBrush		(g_XaFns.pfnXaColour_ReturnBrush)
#define XaColour_ReturnBrushRGB		(g_XaFns.pfnXaColour_ReturnBrushRGB)
#define XaColour_AddToGraduationTable \
									(g_XaFns.pfnXaColour_AddToGraduationTable)
#define XaColour_BuildGraduationTable \
									(g_XaFns.pfnXaColour_BuildGraduationTable)
#define XaColour_BuildGraduationTable32 \
									(g_XaFns.pfnXaColour_BuildGraduationTable32)
#define XaColour_BuildTransparencyTable \
									(g_XaFns.pfnXaColour_BuildTransparencyTable)
#define XaColour_SetGraduation		(g_XaFns.pfnXaColour_SetGraduation)
#define XaColour_SetGraduation4		(g_XaFns.pfnXaColour_SetGraduation4)
#define XaColour_Set3WayGraduation	(g_XaFns.pfnXaColour_Set3WayGraduation)
#define XaColour_Set3WayGraduation4	(g_XaFns.pfnXaColour_Set3WayGraduation4)
#define XaColour_Set4WayGraduation	(g_XaFns.pfnXaColour_Set4WayGraduation)
#define XaColour_Set4WayGraduation4	(g_XaFns.pfnXaColour_Set4WayGraduation4)
#define XaColour_SetGourand			(g_XaFns.pfnXaColour_SetGourand)
#define XaColour_SetTransparentGraduation \
									(g_XaFns.pfnXaColour_SetTransparentGraduation)
#define XaColour_SetTransparentGraduation4 \
									(g_XaFns.pfnXaColour_SetTransparentGraduation4)
#define XaColour_SetTransparent3WayGraduation \
									(g_XaFns.pfnXaColour_SetTransparent3WayGraduation)
#define XaColour_SetTransparent3WayGraduation4 \
									(g_XaFns.pfnXaColour_SetTransparent3WayGraduation4)
#define XaColour_SetTransparent4WayGraduation \
									(g_XaFns.pfnXaColour_SetTransparent4WayGraduation)
#define XaColour_SetTransparent4WayGraduation4 \
									(g_XaFns.pfnXaColour_SetTransparent4WayGraduation4)
#define XaBitmap_SetBias			(g_XaFns.pfnXaBitmap_SetBias)
#define XaBitmap_SetGain			(g_XaFns.pfnXaBitmap_SetGain)
#define XaBitmap_SetContone			(g_XaFns.pfnXaBitmap_SetContone)
#define XaBitmap_SetTransparencyRamp \
									(g_XaFns.pfnXaBitmap_SetTransparencyRamp)
#define XaColour_SetTilePattern		(g_XaFns.pfnXaColour_SetTilePattern)
#define XaColour_SetTilePattern4	(g_XaFns.pfnXaColour_SetTilePattern4)
#define XaColour_SetTransparentTilePattern \
									(g_XaFns.pfnXaColour_SetTransparentTilePattern)
#define XaColour_SetTransparentTilePattern4 \
									(g_XaFns.pfnXaColour_SetTransparentTilePattern4)
#define XaColour_ConvertBitmap		(g_XaFns.pfnXaColour_ConvertBitmap)
#define XaColour_SetConversionPalette \
									(g_XaFns.pfnXaColour_SetConversionPalette)
#define XaColour_SetMaxDiffusionError \
									(g_XaFns.pfnXaColour_SetMaxDiffusionError)
#define XaColour_ScaleBitmap		(g_XaFns.pfnXaColour_ScaleBitmap)


#define XaColour_SetGreyConversionValues \
									(g_XaFns.pfnXaColour_SetGreyConversionValues)


#define XaColour_ConvertHSVtoRGB	(g_XaFns.pfnXaColour_ConvertHSVtoRGB)
#define XaColour_ConvertRGBtoHSV	(g_XaFns.pfnXaColour_ConvertRGBtoHSV)


#define XaColour_SetSeparationTables \
									(g_XaFns.pfnXaColour_SetSeparationTables)
#define XaColour_SetBitmapConversionTable \
									(g_XaFns.pfnXaColour_SetBitmapConversionTable)


#define XaDraw_SetDIBitmap			(g_XaFns.pfnXaDraw_SetDIBitmap)
#define XaDraw_SetInvertedDIBitmap	(g_XaFns.pfnXaDraw_SetInvertedDIBitmap)
#define XaDraw_SetMatrix			(g_XaFns.pfnXaDraw_SetMatrix)
#define XaDraw_MaxScale				(g_XaFns.pfnXaDraw_MaxScale)
#define XaDraw_GetMaxBitmapWidth	(g_XaFns.pfnXaDraw_GetMaxBitmapWidth)
#define XaDraw_GetMaxBitmapDepth	(g_XaFns.pfnXaDraw_GetMaxBitmapDepth)
#define XaDraw_SetAntialiasFlag		(g_XaFns.pfnXaDraw_SetAntialiasFlag)
#define XaDraw_SetDashAdjustmentFlag \
									(g_XaFns.pfnXaDraw_SetDashAdjustmentFlag)
#define XaDraw_SetHintingFlag		(g_XaFns.pfnXaDraw_SetHintingFlag)
#define XaDraw_SetFlatness			(g_XaFns.pfnXaDraw_SetFlatness)
#define XaDraw_SetMiterLimit		(g_XaFns.pfnXaDraw_SetMiterLimit)
#define XaDraw_FillRectangle		(g_XaFns.pfnXaDraw_FillRectangle)
#define XaDraw_FillPoint			(g_XaFns.pfnXaDraw_FillPoint)
#define XaDraw_FillPath				(g_XaFns.pfnXaDraw_FillPath)
#define XaDraw_InitialiseFillPath	(g_XaFns.pfnXaDraw_InitialiseFillPath)
#define XaDraw_FillPathLine			(g_XaFns.pfnXaDraw_FillPathLine)
#define XaDraw_DoFillPath			(g_XaFns.pfnXaDraw_DoFillPath)
#define XaDraw_FillTriangle			(g_XaFns.pfnXaDraw_FillTriangle)
#define XaDraw_HintPath				(g_XaFns.pfnXaDraw_HintPath)
#define XaDraw_StrokePath			(g_XaFns.pfnXaDraw_StrokePath)
#define XaDraw_StrokePathToPath		(g_XaFns.pfnXaDraw_StrokePathToPath)
#define XaDraw_CalcStrokeBBox		(g_XaFns.pfnXaDraw_CalcStrokeBBox)
#define XaDraw_IsOverlap			(g_XaFns.pfnXaDraw_IsOverlap)
#define XaDraw_IsStrokeOverlap		(g_XaFns.pfnXaDraw_IsStrokeOverlap)
#define XaDraw_GetStatistics		(g_XaFns.pfnXaDraw_GetStatistics)
#define XaDraw_MakeRegion			(g_XaFns.pfnXaDraw_MakeRegion)
#define XaDraw_MakeUnclippedRegion	(g_XaFns.pfnXaDraw_MakeUnclippedRegion)
#define XaDraw_FillRegion			(g_XaFns.pfnXaDraw_FillRegion)
#define XaDraw_ClipRectangle		(g_XaFns.pfnXaDraw_ClipRectangle)
#define XaDraw_DeviceClipRectangle	(g_XaFns.pfnXaDraw_DeviceClipRectangle)
#define XaDraw_GetDeviceClipRectangle \
									(g_XaFns.pfnXaDraw_GetDeviceClipRectangle)
#define XaDraw_ClipRegion			(g_XaFns.pfnXaDraw_ClipRegion)
#define XaDraw_GetClipRegion		(g_XaFns.pfnXaDraw_GetClipRegion)
#define XaDraw_CalcSimpleBBox		(g_XaFns.pfnXaDraw_CalcSimpleBBox)
#define XaDraw_CalcBBox				(g_XaFns.pfnXaDraw_CalcBBox)
#define XaDraw_TransformPath		(g_XaFns.pfnXaDraw_TransformPath)
#define XaDraw_ScrollBitmap			(g_XaFns.pfnXaDraw_ScrollBitmap)


#define XaDraw_ClearChangedBBox		(g_XaFns.pfnXaDraw_ClearChangedBBox)
#define XaDraw_GetChangedBBox		(g_XaFns.pfnXaDraw_GetChangedBBox)
#define XaDraw_SetChangedBBox		(g_XaFns.pfnXaDraw_SetChangedBBox)


#define XaDraw_SetBevelContrast		(g_XaFns.pfnXaDraw_SetBevelContrast)
#define XaDraw_SetBevelLightness	(g_XaFns.pfnXaDraw_SetBevelLightness)
#define XaDraw_SetBevelDarkness		(g_XaFns.pfnXaDraw_SetBevelDarkness)
#define XaDraw_TranslateBevelValue	(g_XaFns.pfnXaDraw_TranslateBevelValue)


#define XaSprite_PlotTile			(g_XaFns.pfnXaSprite_PlotTile)
#define XaSprite_PlotTile4			(g_XaFns.pfnXaSprite_PlotTile4)

#define Xa3D_SetTruePerspectiveFlag	(g_XaFns.pfnXa3D_SetTruePerspectiveFlag)
#define Xa3D_DefineView				(g_XaFns.pfnXa3D_DefineView)
#define Xa3D_DefineTexture			(g_XaFns.pfnXa3D_DefineTexture)
#define Xa3D_Define2ndTexture		(g_XaFns.pfnXa3D_Define2ndTexture)
#define Xa3D_ConvertColourFormat	(g_XaFns.pfnXa3D_ConvertColourFormat)
#define Xa3D_AddTriangleToView		(g_XaFns.pfnXa3D_AddTriangleToView)
#define Xa3D_AddFlatTriangleToView	(g_XaFns.pfnXa3D_AddFlatTriangleToView)
#define Xa3D_AddTextureToView		(g_XaFns.pfnXa3D_AddTextureToView)
#define Xa3D_AddFlatTextureToView	(g_XaFns.pfnXa3D_AddFlatTextureToView)
#define Xa3D_AddTexturesToView		(g_XaFns.pfnXa3D_AddTexturesToView)
#define Xa3D_AddFlatTexturesToView	(g_XaFns.pfnXa3D_AddFlatTexturesToView)
#define Xa3D_SetSideFaceFlag		(g_XaFns.pfnXa3D_SetSideFaceFlag)
#define Xa3D_GetViewBBox			(g_XaFns.pfnXa3D_GetViewBBox)
#define Xa3D_PlotView				(g_XaFns.pfnXa3D_PlotView)
#define XaDraw_SetAntialiasQualityFlag		(g_XaFns.pfnXaDraw_SetAntialiasQualityFlag)

// Declare the prototypes for the two tables of pointers to functions.
// g_XaFns points to the debug versions of all the GDraw functions.
// g_XaFnsOriginal points to the actual GDraw functions.

extern XA_DRAW_FNS g_XaFns;
extern XA_DRAW_FNS g_XaFnsOriginal;
extern BOOL g_bAddXaTimes;
extern LARGE_INTEGER g_Frequency;
extern LARGE_INTEGER g_XaTimes[127];
extern DWORD g_XaCalls[127];
extern TCHAR* g_XaNames[127];

#endif // CONFIG_DEBUG_XADRAW

#endif // INC_XaDraw_h
