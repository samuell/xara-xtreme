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

#ifndef INC_XaGDraw_h
#define INC_XaGDraw_h

/********************************************************************************************

This file provides as direct mapping from Xa* to G*_
The direct mapping is only used if CONFIG_DEBUG_XADRAW is not defined.

NOTE: Do not include this file directly. Include XaDraw.h or XaDrwOld.h (V3 and V2 style
      interface respectively).

********************************************************************************************/

#include "gdraw.h"

#ifndef CONFIG_DEBUG_XADRAW

#define XaDraw_GetVersion			(GDraw_GetVersion)
#define XaDraw_ClearLastError		(GDraw_ClearLastError)
#define XaDraw_GetLastError			(GDraw_GetLastError)
#define XaDraw_GetLastErrorText		(GDraw_GetLastErrorText)

#define XaDraw_Initialise			(GDraw_Initialise)
#define XaDraw_CopyContext			(GDraw_CopyContext)

#define XaColour_GetGraduationTableSize	\
									(GColour_GetGraduationTableSize)
#define XaColour_GetGraduationTableLength \
									(GColour_GetGraduationTableLength)
#define XaColour_GetLongGraduationTableSize \
									(GColour_GetLongGraduationTableSize)
#define XaColour_GetLongGraduationTableLength \
									(GColour_GetLongGraduationTableLength)
#define XaColour_GetTransparentGraduationTableSize \
									(GColour_GetTransparentGraduationTableSize)
#define XaColour_GetTransparentGraduationTableLength \
									(GColour_GetTransparentGraduationTableLength)
#define XaColour_GetLongTransparentGraduationTableSize \
									(GColour_GetLongTransparentGraduationTableSize)
#define XaColour_GetLongTransparentGraduationTableLength \
									(GColour_GetLongTransparentGraduationTableLength)


#define XaDraw_SetMemoryHandlers	(GDraw_SetMemoryHandlers)
#define XaDraw_SetStackLimit		(GDraw_SetStackLimit)
#define XaDraw_SetStackSize			(GDraw_SetStackSize)
#define XaDraw_Terminate			(GDraw_Terminate)
#define XaDraw_Clean				(GDraw_Clean)


#define XaDraw_ContextLength		(GDraw_ContextLength)
#define XaDraw_ComparePalettes		(GDraw_ComparePalettes)


#define XaColour_SelectPalette		(GColour_SelectPalette)
#define XaColour_InitialiseWithPalette	\
									(GColour_InitialiseWithPalette)
#define XaColour_SetDitherStyle		(GColour_SetDitherStyle)
#define XaColour_SetTileSmoothingFlag \
									(GColour_SetTileSmoothingFlag)
#define XaColour_SetTileFilteringFlag \
									(GColour_SetTileFilteringFlag)
#define XaBitmap_SetMaxFilterSize	(GBitmap_SetMaxFilterSize)
#define XaBitmap_Sharpen			(GBitmap_Sharpen)
#define XaBitmap_Blur				(GBitmap_Blur)
#define XaColour_SetHalftoneOrigin	(GColour_SetHalftoneOrigin)
#define XaColour_SetColour			(GColour_SetColour)
#define XaColour_SetSolidColour		(GColour_SetSolidColour)
#define XaColour_SetWordColour		(GColour_SetWordColour)
#define XaColour_SetInvert			(GColour_SetInvert)
#define XaColour_SetTransparency	(GColour_SetTransparency)
#define XaColour_SetTransparencyLookupTable \
									(GColour_SetTransparencyLookupTable)
#define XaColour_ReturnBrush		(GColour_ReturnBrush)
#define XaColour_ReturnBrushRGB		(GColour_ReturnBrushRGB)
#define XaColour_AddToGraduationTable \
									(GColour_AddToGraduationTable)
#define XaColour_BuildGraduationTable \
									(GColour_BuildGraduationTable)
#define XaColour_BuildGraduationTable32 \
									(GColour_BuildGraduationTable32)
#define XaColour_BuildTransparencyTable \
									(GColour_BuildTransparencyTable)
#define XaColour_SetGraduation		(GColour_SetGraduation)
#define XaColour_SetGraduation4		(GColour_SetGraduation4)
#define XaColour_Set3WayGraduation	(GColour_Set3WayGraduation)
#define XaColour_Set3WayGraduation4	(GColour_Set3WayGraduation4)
#define XaColour_Set4WayGraduation	(GColour_Set4WayGraduation)
#define XaColour_Set4WayGraduation4	(GColour_Set4WayGraduation4)
#define XaColour_SetGourand			(GColour_SetGourand)
#define XaColour_SetTransparentGraduation \
									(GColour_SetTransparentGraduation)
#define XaColour_SetTransparentGraduation4 \
									(GColour_SetTransparentGraduation4)
#define XaColour_SetTransparent3WayGraduation \
									(GColour_SetTransparent3WayGraduation)
#define XaColour_SetTransparent3WayGraduation4 \
									(GColour_SetTransparent3WayGraduation4)
#define XaColour_SetTransparent4WayGraduation \
									(GColour_SetTransparent4WayGraduation)
#define XaColour_SetTransparent4WayGraduation4 \
									(GColour_SetTransparent4WayGraduation4)
#define XaBitmap_SetBias			(GBitmap_SetBias)
#define XaBitmap_SetGain			(GBitmap_SetGain)
#define XaBitmap_SetBrightness		(GBitmap_SetBrightness)
#define XaBitmap_SetContrast		(GBitmap_SetContrast)
#define XaBitmap_SetGamma			(GBitmap_SetGamma)
#define XaBitmap_SetPostGamma		(GBitmap_SetPostGamma)
#define XaBitmap_SetSaturation		(GBitmap_SetSaturation)
#define XaBitmap_SetContone			(GBitmap_SetContone)
#define XaBitmap_SetInputRange		(GBitmap_SetInputRange)
#define XaBitmap_SetOutputRange		(GBitmap_SetOutputRange)
#define XaColour_SetTilePattern		(GColour_SetTilePattern)
#define XaColour_SetTilePattern4	(GColour_SetTilePattern4)
#define XaColour_SetTransparentTilePattern \
									(GColour_SetTransparentTilePattern)
#define XaColour_SetTransparentTilePattern4 \
									(GColour_SetTransparentTilePattern4)
#define XaColour_ConvertBitmap		(GColour_ConvertBitmap)
#define XaColour_SetConversionPalette \
									(GColour_SetConversionPalette)
#define XaColour_SetMaxDiffusionError \
									(GColour_SetMaxDiffusionError)
#define XaColour_ScaleBitmap		(GColour_ScaleBitmap)


#define XaColour_SetGreyConversionValues \
									(GColour_SetGreyConversionValues)


#define XaColour_ConvertHSVtoRGB	(GColour_ConvertHSVtoRGB)
#define XaColour_ConvertRGBtoHSV	(GColour_ConvertRGBtoHSV)


#define XaColour_SetSeparationTables \
									(GColour_SetSeparationTables)
#define XaColour_SetBitmapConversionTable \
									(GColour_SetBitmapConversionTable)


#define XaDraw_SetDIBitmap			(GDraw_SetDIBitmap)
#define XaDraw_SetInvertedDIBitmap	(GDraw_SetInvertedDIBitmap)
#define XaDraw_SetMatrix			(GDraw_SetMatrix)
#define XaDraw_MaxScale				(GDraw_MaxScale)
#define XaDraw_GetMaxBitmapWidth	(GDraw_GetMaxBitmapWidth)
#define XaDraw_GetMaxBitmapDepth	(GDraw_GetMaxBitmapDepth)
#define XaDraw_SetAntialiasFlag		(GDraw_SetAntialiasFlag)
#define XaDraw_SetDashAdjustmentFlag \
									(GDraw_SetDashAdjustmentFlag)
#define XaDraw_SetHintingFlag		(GDraw_SetHintingFlag)
#define XaDraw_SetFlatness			(GDraw_SetFlatness)
#define XaDraw_SetMiterLimit		(GDraw_SetMiterLimit)
#define XaDraw_FillRectangle		(GDraw_FillRectangle)
#define XaDraw_FillPoint			(GDraw_FillPoint)
#define XaDraw_FillPath				(GDraw_FillPath)
#define XaDraw_InitialiseFillPath	(GDraw_InitialiseFillPath)
#define XaDraw_FillPathLine			(GDraw_FillPathLine)
#define XaDraw_DoFillPath			(GDraw_DoFillPath)
#define XaDraw_FillTriangle			(GDraw_FillTriangle)
#define XaDraw_HintPath				(GDraw_HintPath)
#define XaDraw_StrokePath			(GDraw_StrokePath)
#define XaDraw_StrokePathToPath		(GDraw_StrokePathToPath)
#define XaDraw_CalcStrokeBBox		(GDraw_CalcStrokeBBox)
#define XaDraw_IsOverlap			(GDraw_IsOverlap)
#define XaDraw_IsStrokeOverlap		(GDraw_IsStrokeOverlap)
#define XaDraw_GetStatistics		(GDraw_GetStatistics)
#define XaDraw_MakeRegion			(GDraw_MakeRegion)
#define XaDraw_MakeUnclippedRegion	(GDraw_MakeUnclippedRegion)
#define XaDraw_FillRegion			(GDraw_FillRegion)
#define XaDraw_ClipRectangle		(GDraw_ClipRectangle)
#define XaDraw_DeviceClipRectangle	(GDraw_DeviceClipRectangle)
#define XaDraw_GetDeviceClipRectangle \
									(GDraw_GetDeviceClipRectangle)
#define XaDraw_ClipRegion			(GDraw_ClipRegion)
#define XaDraw_GetClipRegion		(GDraw_GetClipRegion)
#define XaDraw_CalcSimpleBBox		(GDraw_CalcSimpleBBox)
#define XaDraw_CalcBBox				(GDraw_CalcBBox)
#define XaDraw_TransformPath		(GDraw_TransformPath)
#define XaDraw_ScrollBitmap			(GDraw_ScrollBitmap)


#define XaDraw_ClearChangedBBox		(GDraw_ClearChangedBBox)
#define XaDraw_GetChangedBBox		(GDraw_GetChangedBBox)
#define XaDraw_SetChangedBBox		(GDraw_SetChangedBBox)


#define XaDraw_SetBevelContrast		(GDraw_SetBevelContrast)
#define XaDraw_SetBevelLightness	(GDraw_SetBevelLightness)
#define XaDraw_SetBevelDarkness		(GDraw_SetBevelDarkness)
#define XaDraw_TranslateBevelValue	(GDraw_TranslateBevelValue)


#define XaBitmap_PlotTile			(GBitmap_PlotTile)
#define XaBitmap_PlotTile4			(GBitmap_PlotTile4)

#define Xa3D_SetTruePerspectiveFlag	(G3D_SetTruePerspectiveFlag)
#define Xa3D_DefineView				(G3D_DefineView)
#define Xa3D_DefineTexture			(G3D_DefineTexture)
#define Xa3D_Define2ndTexture		(G3D_Define2ndTexture)
#define Xa3D_ConvertColourFormat	(G3D_ConvertColourFormat)
#define Xa3D_AddTriangleToView		(G3D_AddTriangleToView)
#define Xa3D_AddFlatTriangleToView	(G3D_AddFlatTriangleToView)
#define Xa3D_AddTextureToView		(G3D_AddTextureToView)
#define Xa3D_AddFlatTextureToView	(G3D_AddFlatTextureToView)
#define Xa3D_AddTexturesToView		(G3D_AddTexturesToView)
#define Xa3D_AddFlatTexturesToView	(G3D_AddFlatTexturesToView)
#define Xa3D_SetSideFaceFlag		(G3D_SetSideFaceFlag)
#define Xa3D_GetViewBBox			(G3D_GetViewBBox)
#define Xa3D_PlotView				(G3D_PlotView)
#define XaDraw_SetAntialiasQualityFlag		(GDraw_SetAntialiasQualityFlag)

#endif // !CONFIG_DEBUG_XADRAW

#endif // INC_XaGDraw_h
