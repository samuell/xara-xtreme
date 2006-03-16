// $Id: gdraw.h 662 2006-03-14 21:31:49Z alex $
/////////////////////////////////////////////////////////////////////////////////////////////////
//
// GDraw.h
//
/////////////////////////////////////////////////////////////////////////////////////////////////
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

#ifndef INC_NEW_GDRAW
#define	INC_NEW_GDRAW

#include "gconsts.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/*
class Vector3D ;
class Colour ;
class FVector2D ;
class FVector3D ;
*/
/////////////////////////////////////////////////////////////////////////////////////////////////

#define TypeDefC2(name1,name2)					\
	typedef		  name2*		  p##name1 ;	\
	typedef const name2*		 pc##name1 ;

#define TypeDefC(name) TypeDefC2(name,name) ;

TypeDefC ( BITMAP				)
TypeDefC ( BITMAPINFOHEADER		)
TypeDefC ( BGR					)
TypeDefC ( BGRT					)
TypeDefC ( BYTE					)
TypeDefC2( CHAR,char			)
TypeDefC ( COLORREF				)
//TypeDefC ( Colour				)
TypeDefC ( DashType				)
//TypeDefC ( FVector2D			)
//TypeDefC ( FVector3D			)
TypeDefC ( GCONTEXT				)
TypeDefC ( GMATRIX				)
TypeDefC ( GraduationTable		)
TypeDefC ( GraduationTable32	)
TypeDefC ( LOGPALETTE			)
TypeDefC ( POINT				)
TypeDefC ( RECT					)
TypeDefC ( REGION				)
TypeDefC ( STATISTICS			)
TypeDefC ( TransparentGradTable	)
TypeDefC2( VOID,void			)

#undef TypeDefC
#undef TypeDefC2

/////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	extern DWORD GDraw_GetVersion() ;
#if !defined(__WXMSW__)
	extern pcCHAR GDraw_GetSvnVersion() ;
#endif

	extern INT32 GDraw_ClearLastError(pGCONTEXT pContext) ;
	extern eError GDraw_GetLastError(pcGCONTEXT pContext) ;
	extern pcCHAR GDraw_GetLastErrorText(pcGCONTEXT pContext) ;

	extern INT32 GDraw_Initialise(pGCONTEXT pContext,pcGCONTEXT pOldContext=NULL) ;
	extern INT32 GDraw_CopyContext(pcGCONTEXT pContext,pGCONTEXT pNewContext) ;

	extern INT32 GColour_GetGraduationTableSize(pcGCONTEXT pContext) ;
	extern INT32 GColour_GetGraduationTableLength() ;
	extern INT32 GColour_GetLongGraduationTableSize(pcGCONTEXT pContext) ;
	extern INT32 GColour_GetLongGraduationTableLength() ;
	extern INT32 GColour_GetTransparentGraduationTableSize() ;
	extern INT32 GColour_GetTransparentGraduationTableLength() ;
	extern INT32 GColour_GetLongTransparentGraduationTableSize() ;
	extern INT32 GColour_GetLongTransparentGraduationTableLength() ;

	extern INT32 GDraw_SetMemoryHandlers(
		pGCONTEXT pContext,
		pBYTE (*MemoryAlloc)( UINT32 Size ),
		void  (*MemoryFree) ( pBYTE Address )
	) ;
	extern INT32 GDraw_SetStackLimit( pGCONTEXT pContext,pcVOID StackLimit ) ;
	extern INT32 GDraw_SetStackSize( pGCONTEXT pContext,UINT32 StackSize ) ;
	extern INT32 GDraw_Terminate(pGCONTEXT pContext) ;
	extern INT32 GDraw_Clean(pGCONTEXT pContext) ;

	extern INT32 GDraw_ContextLength() ;
	extern INT32 GDraw_ComparePalettes( pcGCONTEXT pContext,pcLOGPALETTE Palette,bool Flag ) ;
//	extern INT32 GDraw_SaveContext( GCONTEXT *Context ) ;
//	extern INT32 GDraw_RestoreContext( CONST GCONTEXT *Context ) ;
//	extern INT32 GDraw_SwapContext( GCONTEXT *Context ) ;



	extern pcLOGPALETTE GColour_SelectPalette( pGCONTEXT pContext,bool Flag ) ;
//	extern INT32 GColour_Initialise( HDC dc ) ;
	extern INT32 GColour_InitialiseWithPalette( pGCONTEXT pContext,pcLOGPALETTE Palette ) ;
	extern INT32 GColour_SetDitherStyle( pGCONTEXT pContext,DitherStyle Style=DITHER_GREY_ORDERED ) ;
	extern INT32 GColour_SetTileSmoothingFlag( pGCONTEXT pContext,bool Flag ) ;
	extern INT32 GColour_SetTileFilteringFlag( pGCONTEXT pContext,bool Flag ) ;
	extern INT32 GColour_Sharpen( pGCONTEXT pContext,INT32 Sharpen ) ;
	extern INT32 GColour_Blur   ( pGCONTEXT pContext,INT32 Blur    ) ;
	extern INT32 GColour_SetHalftoneOrigin( pGCONTEXT pContext, INT32 x,INT32 y ) ;
//	extern INT32 GColour_SetSupersampleRate( pGCONTEXT pContext,UINT32 Rate ) ;
	extern INT32 GColour_SetColour( pGCONTEXT pContext,COLORREF Colour ) ;
	extern INT32 GColour_SetSolidColour( pGCONTEXT pContext,COLORREF Colour,UINT32 BPP=0,UINT32 Format16BPP=FORMAT16BPP_555 ) ;
	extern INT32 GColour_SetWordColour( pGCONTEXT pContext,DWORD Colour ) ;
	extern INT32 GColour_SetInvert( pGCONTEXT pContext,DWORD Colour ) ;
	extern INT32 GColour_SetTransparency( pGCONTEXT pContext,COLORREF Colour,DWORD Style ) ;
	extern INT32 GColour_SetTransparencyLookupTable( pGCONTEXT pContext,pcBYTE Table ) ;
	extern pcBYTE GColour_ReturnBrush(pcGCONTEXT pContext) ;
	extern pcBYTE GColour_ReturnBrushRGB(pcGCONTEXT pContext) ;
	extern INT32 GColour_AddToGraduationTable(
		pGCONTEXT pContext,
		COLORREF Colour,
		DWORD HSVFlag,
		pGraduationTable Table,
		UINT32 Index
	) ;
	extern INT32 GColour_BuildGraduationTable(
		pGCONTEXT pContext,
		COLORREF StartColour,
		COLORREF EndColour,
		DWORD HSVFlag,
		pGraduationTable Table
	) ;
	extern INT32 GColour_BuildGraduationTable32(
		pGCONTEXT pContext,
		COLORREF StartColour,
		COLORREF EndColour,
		DWORD HSVFlag,
		pGraduationTable32 Table
	) ;
	extern INT32 GColour_BuildTransparencyTable(
		pGCONTEXT pContext,
		BYTE StartColour,
		BYTE EndColour,
		pTransparentGradTable Table
	) ;
	extern INT32 GColour_SetGraduation(
		pGCONTEXT pContext,
		DWORD Style,
		pcGraduationTable Table,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC
	) ;
	extern INT32 GColour_SetGraduation4(
		pGCONTEXT pContext,
		DWORD Style,
		pcGraduationTable Table,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC,
		pcPOINT PointD
	) ;
//	extern INT32 GColour_SetMultiRadial(
//		pGCONTEXT pContext,
//		DWORD Style,
//		pcGraduationTable32 Table,
//		pcBYTE BlobShapeTable,
//		INT32 BlobShapeOffset,
//		UINT32 NumberOfBlobs,
//		pcGBLOB Blobs
//	) ;
	extern INT32 GColour_Set3WayGraduation(
		pGCONTEXT pContext,
		DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
		pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointD
	) ;
	extern INT32 GColour_Set3WayGraduation4(
		pGCONTEXT pContext,
		DWORD Style,
		COLORREF ColourA, COLORREF ColourB,					  COLORREF ColourD,
		pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointC, pcPOINT   PointD
	) ;
	extern INT32 GColour_Set4WayGraduation(
		pGCONTEXT pContext,
		DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
		pcPOINT   PointA, pcPOINT   PointB,					  pcPOINT   PointD
	) ;
	extern INT32 GColour_Set4WayGraduation4(
		pGCONTEXT pContext,
		DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
		pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointC, pcPOINT   PointD
	) ;
	extern INT32 GColour_SetGourand(
		pGCONTEXT pContext,
		DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
		pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointD
	) ;
	extern INT32 GColour_SetTransparentGraduation(
		pGCONTEXT pContext,
		DWORD Style,
		pcTransparentGradTable Table,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC
	) ;
	extern INT32 GColour_SetTransparentGraduation4(
		pGCONTEXT pContext,
		DWORD Style,
		pcTransparentGradTable Table,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC,
		pcPOINT PointD
	) ;
	extern INT32 GColour_SetTransparent3WayGraduation(
		pGCONTEXT pContext,
		DWORD Style,
		BYTE	ValueA, BYTE	ValueB, BYTE	ValueD,
		pcPOINT PointA, pcPOINT PointB, pcPOINT PointD
	) ;
	extern INT32 GColour_SetTransparent3WayGraduation4(
		pGCONTEXT pContext,
		DWORD Style,
		BYTE	ValueA, BYTE	ValueB,				    BYTE	ValueD,
		pcPOINT PointA, pcPOINT PointB, pcPOINT PointC, pcPOINT PointD
	) ;
	extern INT32 GColour_SetTransparent4WayGraduation(
		pGCONTEXT pContext,
		DWORD Style,
		BYTE	ValueA, BYTE	ValueB, BYTE	ValueC, BYTE	ValueD,
		pcPOINT PointA, pcPOINT PointB,					pcPOINT PointD
	) ;
	extern INT32 GColour_SetTransparent4WayGraduation4(
		pGCONTEXT pContext,
		DWORD Style,
		BYTE	ValueA, BYTE	ValueB, BYTE	ValueC, BYTE	ValueD,
		pcPOINT PointA, pcPOINT PointB, pcPOINT PointC, pcPOINT PointD
	) ;
	extern INT32 GColour_SetTilePattern(
		pGCONTEXT pContext,
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
	) ;
	extern INT32 GColour_SetTilePattern4(
		pGCONTEXT pContext,
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
	) ;
	extern INT32 GColour_SetTransparentTilePattern(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER BitmapInfo,
		pcBYTE Bitmap,
		DWORD Style,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC,
		BYTE DefaultColour = 0,
		pcBYTE TransparencyTranslationTable = 0,
		INT32 TileOffset = 0
	) ;
	extern INT32 GColour_SetTransparentTilePattern4(
		pGCONTEXT pContext,
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
	) ;
	extern INT32 GColour_ConvertBitmap(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER SBitmapInfo, pcBYTE SBitmap,
		pcBITMAPINFOHEADER DBitmapInfo,  pBYTE DBitmap,
		DWORD Dither
	) ;
	extern INT32 GColour_SetConversionPalette( pGCONTEXT pContext,pcLOGPALETTE pPalette ) ;
	extern INT32 GColour_SetMaxDiffusionError( pGCONTEXT pContext,UINT32 MaxError ) ;
	extern INT32 GColour_ScaleBitmap(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER SBitmapInfo, pcBYTE SBitmap,
		pcBITMAPINFOHEADER DBitmapInfo,  pBYTE DBitmap,
		UINT32 Channels
	) ;

	extern INT32 GColour_SetGreyConversionValues( pGCONTEXT pContext,UINT32 Red,UINT32 Green,UINT32 Blue ) ;

	extern COLORREF GColour_ConvertHSVtoRGB( COLORREF hsv )	;
	extern COLORREF GColour_ConvertRGBtoHSV( COLORREF rgb )	;

	extern INT32 GColour_SetSeparationTables(
		pGCONTEXT pContext,
		pcBGR    CyanSeparationTable,
		pcBGR MagentaSeparationTable,
		pcBGR  YellowSeparationTable,
		pcBGR   BlackSeparationTable,
		pcBYTE UnderColourRemovalTable,
		pcBYTE BlackGenerationTable
	) ;
	extern INT32 GColour_SetBitmapConversionTable(
		pGCONTEXT pContext,
		pcBGR BitmapConversionTable
	) ;

	extern INT32 GDraw_SetDIBitmap(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER BitmapInfo,
		pBYTE Bitmap,
		UINT32 Format16BPP = FORMAT16BPP_555
	) ;
	extern INT32 GDraw_SetInvertedDIBitmap(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER BitmapInfo,
		pBYTE Bitmap,
		UINT32 Format16BPP = FORMAT16BPP_555
	) ;
	extern INT32 GDraw_SetMatrix( pGCONTEXT pContext,pcGMATRIX Matrix ) ;
	extern INT32 GDraw_MaxScale ( pGCONTEXT pContext,pcGMATRIX Matrix,pcRECT BBox ) ;
	extern INT32 GDraw_GetMaxBitmapWidth() ;
	extern INT32 GDraw_GetMaxBitmapDepth() ;
	extern INT32 GDraw_SetAntialiasFlag( pGCONTEXT pContext,bool Flag ) ;
	extern INT32 GDraw_SetAntialiasQualityFlag( pGCONTEXT pContext,bool Flag ) ;
	extern INT32 GDraw_SetDashAdjustmentFlag( pGCONTEXT pContext,bool Flag ) ;
	extern INT32 GDraw_SetHintingFlag( pGCONTEXT pContext,bool Flag ) ;
	extern INT32 GDraw_SetFlatness( pGCONTEXT pContext,UINT32 Flatness ) ;
	extern INT32 GDraw_SetMiterLimit( pGCONTEXT pContext,UINT32 MiterLimit ) ;
	extern INT32 GDraw_FillRectangle( pGCONTEXT pContext,pcRECT Rectangle ) ;
	extern INT32 GDraw_FillPoint( pGCONTEXT pContext,pcPOINT Point ) ;
	extern INT32 GDraw_FillPath(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		UINT32 Winding
	) ;
/*	extern INT32 GDraw_InitialiseFillPath( pGCONTEXT pContext,UINT32 Winding,bool DontPlot ) ;
	extern INT32 GDraw_FillPathLine( pGCONTEXT pContext,pcFVector2D Points, bool Reverse ) ;
	extern INT32 GDraw_DoFillPath( pGCONTEXT pContext ) ;
	extern INT32 GDraw_FillTriangle(
		pGCONTEXT pContext,
		pcPOINT PointA,
		pcPOINT PointB,
		pcPOINT PointC,
		DWORD Flags
	) ;
*/	extern INT32 GDraw_HintPath(
		pGCONTEXT pContext,
		pPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		bool Close,
		UINT32 LineWidth
	) ;
	extern INT32 GDraw_StrokePath(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		bool Close,
		UINT32 LineWidth,
		DWORD LineCaps,
		DWORD LineJoin,
		pcDashType Dash
	) ;
	extern INT32 GDraw_StrokePathToPath(
		pGCONTEXT pContext,
		pcPOINT IPoints,
		pcBYTE  ITypes,
		UINT32 ILength,
		pPOINT OPoints,
		pBYTE  OTypes,
		UINT32 OLength,
		bool Close,
		UINT32 LineWidth,
		DWORD LineCaps,
		DWORD LineJoin,
		pcDashType Dash
	) ;
	extern INT32 GDraw_CalcStrokeBBox(
		pGCONTEXT pContext,
		pcPOINT IPoints,
		pcBYTE  ITypes,
		UINT32 ILength,
		pRECT Rect,
		bool Close,
		UINT32 LineWidth,
		DWORD LineCaps,
		DWORD LineJoin,
		pcDashType Dash
	) ;
	extern INT32 GDraw_IsOverlap(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		UINT32 Winding
	) ;
	extern INT32 GDraw_IsStrokeOverlap(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		bool Close,
		UINT32 LineWidth,
		DWORD LineCaps,
		DWORD LineJoin,
		pcDashType Dash
	) ;
	extern INT32 GDraw_GetStatistics(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		UINT32 Winding,
		pSTATISTICS Stats
	) ;
	extern INT32 GDraw_MakeRegion(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		UINT32 Winding,
		pREGION ORegion,
		UINT32 OLength
	) ;
	extern INT32 GDraw_MakeUnclippedRegion(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		UINT32 Winding,
		pREGION ORegion,
		UINT32 OLength
	) ;
	extern INT32 GDraw_FillRegion(
		pGCONTEXT pContext,
		pcREGION Region,
		pcPOINT Offset
	) ;
	extern INT32 GDraw_ClipRectangle( pGCONTEXT pContext,pcRECT Rectangle ) ;
	extern INT32 GDraw_DeviceClipRectangle( pGCONTEXT pContext,pcRECT Rectangle ) ;
	extern INT32 GDraw_GetDeviceClipRectangle( pcGCONTEXT pContext,pRECT Rectangle ) ;
	extern INT32 GDraw_ClipRegion( pGCONTEXT pContext,pcREGION Region ) ;
	extern pcREGION GDraw_GetClipRegion(pGCONTEXT pContext) ;
	extern INT32 GDraw_CalcSimpleBBox(
		pcPOINT Points,
		UINT32 Length,
		pRECT Rect
	) ;
	extern INT32 GDraw_CalcBBox(
		pGCONTEXT pContext,
		pcPOINT Points,
		pcBYTE  Types,
		UINT32 Length,
		pRECT Rect,
		bool Flatten
	) ;
	extern INT32 GDraw_TransformPath(
		pGCONTEXT pContext,
		pcPOINT IPoints,
		 pPOINT OPoints,
		UINT32 Length,
		pcGMATRIX Matrix
	) ;
	extern INT32 GDraw_ScrollBitmap( pGCONTEXT pContext, INT32 x,INT32 y ) ;

	extern INT32 GDraw_ClearChangedBBox(pGCONTEXT pContext) ;
	extern INT32 GDraw_GetChangedBBox( pcGCONTEXT pContext, pRECT Rectangle ) ;
	extern INT32 GDraw_SetChangedBBox(  pGCONTEXT pContext,pcRECT Rectangle ) ;

	extern INT32 GDraw_SetBevelContrast ( pGCONTEXT pContext,UINT32 Contrast ) ;
	extern INT32 GDraw_SetBevelLightness( pGCONTEXT pContext,UINT32 Contrast ) ;
	extern INT32 GDraw_SetBevelDarkness ( pGCONTEXT pContext,UINT32 Contrast ) ;
	extern INT32 GDraw_TranslateBevelValue( pcGCONTEXT pContext, BYTE Index, BYTE Colour ) ;



	extern INT32 GSprite_PlotTile(
		pGCONTEXT pContext,
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
	) ;
	extern INT32 GSprite_PlotTile4(
		pGCONTEXT pContext,
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
	) ;



/*
	extern INT32 G3D_SetTruePerspectiveFlag( pGCONTEXT pContext,bool Flag ) ;
	extern INT32 G3D_DefineView(
		pGCONTEXT pContext,
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
		bool bTransparent = FALSE,
		bool bAlpha = FALSE,
		bool bTransparentTexture = FALSE,
		UINT32 uFade = 0
	) ;
	extern INT32 G3D_DefineTexture(
		pGCONTEXT pContext,
		pcBITMAP pBitmap,
		bool bScaleDown,
		bool bRepeating
	) ;
	extern INT32 G3D_Define2ndTexture(
		pGCONTEXT pContext,
		pcBITMAPINFOHEADER pBitmapInfo,
		pcBYTE pBitmap,
		pcBGRT pRGBIndex,
		bool bTint,
		COLORREF TintColour
	) ;
	extern INT32 G3D_ConvertColourFormat( pcColour pSColour, pColour pDColour ) ;
	extern INT32 G3D_AddTriangleToView(
		pGCONTEXT pContext,
		pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
		pcColour   pColourA, pcColour   pColourB, pcColour   pColourC,
		bool bForward
	) ;
	extern INT32 G3D_AddFlatTriangleToView(
		pGCONTEXT pContext,
		pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
		pcColour pFlatColour,
		bool bForward
	) ;
	extern INT32 G3D_AddTextureToView(
		pGCONTEXT pContext,
		pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
		pcColour   pColourA, pcColour   pColourB, pcColour   pColourC,
		pcColour   pColourA2,pcColour   pColourB2,pcColour   pColourC2,
		pcFVector2D pPointA2,pcFVector2D pPointB2,pcFVector2D pPointC2,
		bool bForward
	) ;
	extern INT32 G3D_AddFlatTextureToView(
		pGCONTEXT pContext,
		pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
		pcColour pFlatColour,
		pcColour pFlatColour2,
		pcFVector2D pPointA2,pcFVector2D pPointB2,pcFVector2D pPointC2,
		bool bForward
	) ;
	extern INT32 G3D_AddTexturesToView(
		pGCONTEXT pContext,
		pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
		pcColour   pColourA, pcColour   pColourB, pcColour   pColourC,
		pcColour   pColourA2,pcColour   pColourB2,pcColour   pColourC2,
		pcFVector2D pPointA2,pcFVector2D pPointB2,pcFVector2D pPointC2,
		pcFVector2D pPointA3,pcFVector2D pPointB3,pcFVector2D pPointC3,
		bool bForward
	) ;
	extern INT32 G3D_AddFlatTexturesToView(
		pGCONTEXT pContext,
		pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
		pcColour pFlatColour,
		pcColour pFlatColour2,
		pcFVector2D pPointA2,pcFVector2D pPointB2,pcFVector2D pPointC2,
		pcFVector2D pPointA3,pcFVector2D pPointB3,pcFVector2D pPointC3,
		bool bForward
	) ;
	extern INT32 G3D_SetSideFaceFlag( pGCONTEXT pContext,bool bFlag ) ;
	extern INT32 G3D_GetViewBBox( pcGCONTEXT pContext,pRECT pBBox ) ;
	extern INT32 G3D_PlotView(pGCONTEXT pContext) ;
*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
