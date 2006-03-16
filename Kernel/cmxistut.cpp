// $Id: cmxistut.cpp 662 2006-03-14 21:31:49Z alex $
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
// data reading stuff for CMX import

#include "camtypes.h"

// flag for parsing the header file
#define COMPILING_CMXISTUT_CPP

#include "ccfile.h"
#include "fixmem.h"
#include "basestr.h"

#include "cmxistut.h"
#include "cmxifltr.h"
#include "cmxibits.h"
#include "cmxform.h"


#define new CAM_DEBUG_NEW


//-0-0---------------------------------------------------------------------------------------

// descriptions for various bits

START_CMXDESC(FileHeader)
	CI_READSTRINGF(32),		// id
	CI_SKIPIN_BYTES(16),	// don't bother about the OS
	CI_READINT32,			// byteorder
	CI_READINT16,			// coord size
	CI_READINT32,			// major version
	CI_READINT32,			// minor version
	CI_READINT16,			// unit
	CI_READDOUBLE,			// Factor
	CI_READINT32, CI_READINT32, CI_READINT32,		// unused stuff
	CI_READINT32,			// Index section
	CI_READINT32,			// info section
	CI_READINT32,			// thumbnail
	CI_READINT32, CI_READINT32, CI_READINT32, CI_READINT32,	// bbox
	CI_READINT32			// tally
END_CMXDESC

START_CMXDESC(MasterIndexHeader)
	CI_READINT16,
	CI_READINT16,
	CI_READINT16
END_CMXDESC

START_CMXDESC(MasterIndexEntry)
	CI_READINT16,
	CI_READOFFSET
END_CMXDESC

START_CMXDESC(SimpleIndexHeader)
	CI_READINT16
END_CMXDESC

START_CMXDESC(PageIndexEntry)
	CI_DO16,
	CI_SKIPOUT(INT32),
	CI_DO32,
	CI_STORENEXT,
	CI_READINT16,
	CI_SETINMARKER,
	CI_DOALL,
	CI_READOFFSET,
	CI_READOFFSET,
	CI_READOFFSET,
	CI_READOFFSET,
	CI_DO32,
	CI_ENSUREINLOC
END_CMXDESC

START_CMXDESC(CommandHeader)
	CI_READUINT16,
	CI_READINT16
END_CMXDESC

START_CMXDESC(BeginPage)
	CI_TAG(cmxTAG_BeginPage_PageSpecification),
	CI_READINT16,	// page number
	CI_READUINT32,	// flags
	CI_READBBOX,
	CI_READOFFSET,	// end page offset
	CI_READINT16,	// group count
	CI_READINT32,	// instruction count
	CI_ENDTAG,
	CI_TAG(cmxTAG_BeginPage_Matrix),
	CI_READMATRIX,
	CI_ENDTAG,
	// there is a mapping mode tag here, but this doesn't matter as we'll skip it
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC(BeginLayer)
	CI_TAG(cmxTAG_BeginLayer_LayerSpecification),
	CI_READINT16,	// page number
	CI_READINT16,	// layer number
	CI_READUINT32,	// flags
	CI_READINT32,	// instruction count
	CI_READSTRING,	// layer name
	CI_ENDTAG,
	CI_TAG(cmxTAG_BeginLayer_Matrix),
	CI_READMATRIX,
	CI_ENDTAG,
	// mapping mode again...
	CI_MINENDTAG
END_CMXDESC

// scary rendering attributes thingy!
START_CMXDESC(RenderAttr)
	CI_STORENEXT,
	CI_READBYTE,
	// ----------------------------- Fill spec
	CI_SETOUTMARKER,
	CI_IFSTOREDIS(CI_IFTYPE_BITSET, cmxRENDATTRMASK_FILL),
		CI_TAG(cmxTAG_RenderAttr_FillSpec),
		CI_STORENEXT,
		CI_READINT16,
		// ------------------------- Uniform fill
		CI_IFSTOREDIS(CI_IFTYPE_VALUE, cmxFILLID_UNIFORM),
			CI_TAG(cmxTAG_RenderAttr_FillSpec_Uniform),
			CI_READREFERENCE,	// colour reference
			CI_READREFERENCE,	// screen reference
			CI_ENDTAG,
			CI_MINENDTAG,
		// ------------------------- Fountain fill
		CI_ELSEIF(CI_IFTYPE_VALUE, cmxFILLID_FOUNTAIN),
			CI_TAG(cmxTAG_RenderAttr_FillSpec_Fountain_Base),
			CI_READINT16,		// type
			CI_READREFERENCE,	// screen
			CI_READINT16,		// padding
			CI_READANGLE,		// angle
			CI_DO16,
			CI_SKIPIN(WORD),	// seems to be added in there in 16 bit files
			CI_DOALL,
			CI_READCOORD,		// x,y offsets
			CI_READINT16,		// step count
			CI_READINT16,		// fill mode
			CI_DO32,
			CI_READINT16,		// rate method
			CI_READINT16,		// rate value
			CI_DO16,
			CI_SKIPOUT_BYTES(sizeof(INT32) * 2),
			CI_DOALL,
			CI_ENDTAG,
			CI_TAG(cmxTAG_RenderAttr_FillSpec_Fountain_Color),
			CI_READCOLOURLIST,	// gets the list of colours
			CI_ENDTAG,
			CI_MINENDTAG,
		// ------------------------- Postscript fill
		CI_ELSEIF(CI_IFTYPE_VALUE, cmxFILLID_PS),
			CI_TAG(cmxTAG_RenderAttr_FillSpec_Postscript_Base),
			CI_READREFERENCE,
			CI_STORENEXT,
			CI_READINT16,
			CI_SKIPINTIMES(WORD),
			CI_ENDTAG,
			CI_TAG(cmxTAG_RenderAttr_FillSpec_Postscript_UserFunc),
			CI_READSTRING,
			CI_ENDTAG,
			CI_MINENDTAG,
		// ------------------------- Two colour bitmap
		CI_ELSEIF(CI_IFTYPE_VALUE, cmxFILLID_COLPATTERN),
			CI_TAG(cmxTAG_RenderAttr_FillSpec_MonoBM),
			CI_READREFERENCE,
			CI_READTILING,
			CI_READREFERENCE,
			CI_READREFERENCE,
			CI_READREFERENCE,
			CI_ENDTAG,
			CI_MINENDTAG,
		// ------------------------- Bitmap fill
		CI_ELSEIF(CI_IFTYPE_VALUE, cmxFILLID_BITMAPFILL),
			CI_TAG(cmxTAG_RenderAttr_FillSpec_ColorBM),
			CI_READREFERENCE,
			CI_READTILING,
			CI_READBBOX,
			CI_ENDTAG,
			CI_MINENDTAG,
		// ------------------------- Bitmap fill (16 bit) / Vector (32 bit)
		CI_ELSEIF(CI_IFTYPE_VALUE, cmxFILLID_BITMAPFILL16),
			CI_TAG(cmxTAG_RenderAttr_FillSpec_ColorBM),
			CI_READREFERENCE,
			CI_READTILING,
			CI_READBBOX,
			CI_ENDTAG,
			CI_MINENDTAG,
		// ------------------------- Texture fill
		CI_ELSEIF(CI_IFTYPE_VALUE, cmxFILLID_TEXTURE),
			// 32 bit version
			CI_TAG(cmxTAG_RenderAttr_FillSpec_Texture),
			CI_TAG(cmxTAG_RenderAttr_FillSpec_ColorBM),
			CI_READREFERENCE,
			CI_READTILING,
			CI_READBBOX,
			CI_ENDTAG,
			CI_MINENDTAG,
			// right then, some random stuff
			CI_SKIPIN_BYTES(7),
			CI_STORENEXT,
			CI_READINT16,		// size of string
			CI_SKIPINTIMES(BYTE),
			CI_STORENEXT,
			CI_READINT16,		// size of another string
			CI_SKIPINTIMES(BYTE),
			CI_STORENEXT,
			CI_READINT16,		// size of yet another string
			CI_SKIPINTIMES(BYTE),
			CI_STORENEXT,
			CI_READINT16,
			CI_SKIPINTIMES_BYTES(8),
			CI_ENDTAG,
			CI_MINENDTAG,
		// ------------------------- <>
		CI_ENDIF,
		CI_ENDTAG,
		CI_MINENDTAG,
	CI_ENDIF,
	CI_GOTOOUTLOC(cmxiFillSpec),		// jump to after the end of the fill spec bit

	// ----------------------------- outline spec
	CI_SETOUTMARKER,
	CI_IFSTOREDIS(CI_IFTYPE_BITSET, cmxRENDATTRMASK_OUTLINE),
		CI_TAG(cmxTAG_RenderAttr_OutlineSpec),
		CI_READREFERENCE,
		CI_ENDTAG,
		CI_MINENDTAG,
	CI_ENDIF,
	CI_GOTOOUTLOC(INT32),

	// ----------------------------- lens spec
	CI_SETOUTMARKER,
	CI_IFSTOREDIS(CI_IFTYPE_BITSET, cmxRENDATTRMASK_LENS),
		CI_TAG(cmxTAG_RenderAttr_LensSpec_Base),
		CI_STORENEXT,
		CI_READBYTE,	// type
		CI_IFSTOREDIS(CI_IFTYPE_VALUE, cmxLENSTYPE_GLASS),
			CI_READBYTE,	// tint method
			CI_READINT16,	// uniform rate
			CI_READREFERENCE,	// colour
			CI_READREFERENCE,	// range procedure reference
		CI_ELSEIF(CI_IFTYPE_VALUE, cmxLENSTYPE_MAGNIFYING),
			CI_READINT16,
			CI_READREFERENCE,
		CI_ELSEIF(CI_IFTYPE_VALUE, cmxLENSTYPE_FISHEYE),
			CI_READINT16,
			CI_READREFERENCE,
		CI_ELSEIF(CI_IFTYPE_VALUE, cmxLENSTYPE_FISHEYE2),
			CI_READINT16,
			CI_READREFERENCE,
		CI_ELSEIF(CI_IFTYPE_VALUE, cmxLENSTYPE_WIREFRAME),
			CI_READBYTE,
			CI_READREFERENCE,
			CI_READBYTE,
			CI_READREFERENCE,
			CI_READREFERENCE,
		CI_ELSEIF(CI_IFTYPE_VALUE, cmxLENSTYPE_WIREFRAME2 ),
			CI_READBYTE,
			CI_READREFERENCE,
			CI_READBYTE,
			CI_READREFERENCE,
			CI_READREFERENCE,
		//CI_ELSEIF(CI_IFTYPE_VALUE, cmxLENSTYPE_TRANSAPRENCY),
		//	CI_TAG(cmxTAG_RenderAttr_NewTransparency),
		//	CI_TRANSPARENCY,
		//	CI_ENDTAG,
		CI_ENDIF,
		// in cmx32 we might get another tag here
//		CI_DO32,
		CI_IF_TAG(cmxTAG_RenderAttr_LensSpec_FrozViewp),
		//CI_SKIPIN_BYTES(18),		// some stuff. Don't like it. Won't load it.
		//CI_ENDTAG,
		CI_DOALL,
		CI_ENDTAG,
		CI_MINENDTAG,
	CI_ENDIF,
	CI_GOTOOUTLOC(cmxiLensSpec),

	// ----------------------------- container spec
	// don't need to bother with out markers as this is the last thing in the struct
	CI_IFSTOREDIS(CI_IFTYPE_BITSET, cmxRENDATTRMASK_CONTAIN),
		CI_TAG(cmxTAG_RenderAttr_ContainerSpec),
			CI_READREFERENCE,		// container ref
			CI_READBOOLEAN,			// auto transform flag
		CI_ENDTAG,
		CI_MINENDTAG,
	CI_ENDIF
END_CMXDESC

START_CMXDESC(PolyCurve)
	CI_TAG(cmxTAG_PolyCurve_RenderingAttr),
	CI_READRENDATTR,
	CI_ENDTAG,
	CI_TAG(cmxTAG_PolyCurve_PointList),
	CI_READPOINTLIST,
	CI_ENDTAG,
	CI_TAG(cmxTAG_PolyCurve_BoundingBox),
	CI_READBBOX,
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC ( Rectangle )						// Graeme (1-2-00)
	CI_TAG ( cmxTAG_Rectangle_RenderingAttr ),
	CI_READRENDATTR,							// Rendering attributes.
	CI_ENDTAG,									
	CI_TAG ( cmxTAG_Rectangle_RectangleSpecification ),
	CI_READCOORD,								// Center of the rectangle.
	CI_READLENGTH,								// Dimensions - Width.
	CI_READLENGTH,								// Dimensions - Height.
	CI_READANGLE,								// Corner radius.
	CI_READANGLE,								// Rotation.
	CI_READBBOX,								// Bounding box.
	CI_ENDTAG,
	CI_MINENDTAG								// End of shape tag.
END_CMXDESC

START_CMXDESC(AddGlobalTransform)
	CI_TAG(cmxTAG_AddGlobalTransform_Matrix),
	CI_READMATRIX,
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC(SetGlobalTransform)
	CI_TAG(cmxTAG_SetGlobalTransfo_Matrix),
	CI_READMATRIX,
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC(Colour)
	CI_TAG(cmxTAG_DescrSection_Color_Base),
	CI_STORENEXT,
	CI_READBYTE,		// colour model
	CI_READBYTE,		// palette type
	CI_ENDTAG,
	CI_TAG(cmxTAG_DescrSection_Color_ColorDescr),
	CI_IFSTOREDIS(CI_IFTYPE_VALUE, cmxCOLMODEL_PANTONE),
		CI_READINT16,	// ID
		CI_READINT16,	// density
	CI_ELSEIF(CI_IFTYPE_VALUE, cmxCOLMODEL_CMYK),
		CI_READBYTE,	// cyan
		CI_READBYTE,	// magenta
		CI_READBYTE,	// yellow
		CI_READBYTE,	// key
	CI_ELSEIF(CI_IFTYPE_VALUE, cmxCOLMODEL_CMYK255),
		CI_READBYTE,	// cyan
		CI_READBYTE,	// magenta
		CI_READBYTE,	// yellow
		CI_READBYTE,	// key
	CI_ELSEIF(CI_IFTYPE_VALUE, cmxCOLMODEL_CMY),
		CI_READBYTE,	// cyan
		CI_READBYTE,	// magenta
		CI_READBYTE,	// yellow
		CI_READBYTE,	// pad
	CI_ELSEIF(CI_IFTYPE_VALUE, cmxCOLMODEL_RGB),
		CI_READBYTE,	// red
		CI_READBYTE,	// green
		CI_READBYTE,	// blue
	CI_ELSEIF(CI_IFTYPE_VALUE, cmxCOLMODEL_HSB),
		CI_READINT16,	// hue
		CI_READBYTE,	// saturation
		CI_READBYTE,	// value
	CI_ELSEIF(CI_IFTYPE_VALUE, cmxCOLMODEL_HLS),
		CI_READINT16,	// bue
		CI_READBYTE,	// lightness
		CI_READBYTE,	// saturation
	CI_ELSEIF(CI_IFTYPE_VALUE, cmxCOLMODEL_BW),
		CI_READBYTE,	// colour
	CI_ELSEIF(CI_IFTYPE_VALUE, cmxCOLMODEL_LAB),
		CI_READBYTE,	// luminocity
		CI_READBYTE,	// Green2Magenta
		CI_READBYTE,	// Blue2Yellow
		CI_READBYTE,	// pad
	CI_ELSEIF(CI_IFTYPE_VALUE, cmxCOLMODEL_GREY),
		CI_READBYTE,	// grey
	CI_ELSEIF(CI_IFTYPE_VALUE, cmxCOLMODEL_YIG255),
		CI_READBYTE,	// y
		CI_READBYTE,	// i
		CI_READBYTE,	// q
		CI_READBYTE,	// pad
	CI_ENDIF,

	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC(Outline)
	CI_TAG(cmxTAG_DescrSection_Outline),
	CI_READREFERENCE,
	CI_READREFERENCE,
	CI_READREFERENCE,
	CI_READREFERENCE,
	CI_READREFERENCE,
	CI_READREFERENCE,
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC(Pen)
	CI_TAG(cmxTAG_DescrSection_Pen),
	CI_READLENGTH,
	CI_READINT16,
	CI_READANGLE,
	CI_DO16,
	CI_SKIPIN(WORD),		// some randomness in 16 bit files
	CI_DOALL,
	CI_READMATRIX,
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC(LineStyle)
	CI_TAG(cmxTAG_DescrSection_LineStyle),
	CI_READBYTE,
	CI_READBYTE,
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC(DotDash)
	CI_TAG(cmxTAG_DescrSection_Dash),
	CI_READINT16ARRAY,
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC


START_CMXDESC(DrawImage)
	CI_TAG(cmxTAG_DrawImage_RenderingAttr),
	CI_READRENDATTR,
	CI_ENDTAG,
	CI_TAG(cmxTAG_DrawImage_DrawImageSpecification),
	CI_READBBOX,		// image extent
	CI_READBBOX,		// cropping rectangle
	CI_READMATRIX,		// transformations
	CI_READINT16,		// image type
	CI_READREFERENCE,	// image reference 1
	CI_READREFERENCE,	// image reference 2
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC(EmbeddedFileIndexEntry)
	CI_DO16,
	CI_SKIPOUT(INT32),
	CI_DO32,
	CI_STORENEXT,
	CI_READINT16,		// size in file, 32 bit only
	CI_SETINMARKER,
	CI_DOALL,
	CI_READOFFSET,		// embedded file offset
	CI_READINT16,		// type
	CI_DO32,
	CI_ENSUREINLOC
END_CMXDESC

START_CMXDESC(ProcedureIndexEntry)
	CI_DO16,
	CI_SKIPOUT(INT32),
	CI_DO32,
	CI_STORENEXT,
	CI_READINT16,		// size in file, 32 bit only
	CI_SETINMARKER,
	CI_DOALL,
	CI_READOFFSET,		// reference list offset
	CI_READOFFSET,		// offset in file
	CI_DO32,
	CI_ENSUREINLOC
END_CMXDESC

START_CMXDESC(JumpAbsolute)
	CI_TAG(cmxTAG_JumpAbsolute_Offset),
	CI_READOFFSET,
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC(ImageInfo)
	CI_TAG(cmxTAG_DescrSection_Image_ImageInfo),
	CI_READINT16,
	CI_READINT16,
	CI_READINT32,
	CI_READINT32,
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC(ImageContents)
	CI_DO32,
	CI_SKIPIN_BYTES(5),
	CI_DOALL,
	CI_READUINT16,
	CI_READUINT32,
	CI_READUINT16,
	CI_READUINT16,
	CI_READUINT32
END_CMXDESC

START_CMXDESC(ImageContentsDIB)
	CI_READUINT16,
	CI_READUINT32,
	CI_READUINT16,
	CI_READUINT16,
	CI_READUINT32
END_CMXDESC

START_CMXDESC(RImageHeader)
	CI_READUINT32,
	CI_READUINT32,
	CI_READUINT32,
	CI_READUINT32,
	CI_READUINT32,
	CI_READUINT32,
	CI_READUINT32,
	CI_READUINT32,
	CI_READUINT32,
	CI_READUINT32,
	CI_READUINT32,
	CI_READINT32,
	CI_READINT32,
	CI_READINT32,
	CI_READINT32,
	CI_READINT32
END_CMXDESC

START_CMXDESC(RImagePalette)
	CI_READINT16,
	CI_READINT16
END_CMXDESC

START_CMXDESC(RImagePaletteEntry)
	CI_READBYTE,
	CI_READBYTE,
	CI_READBYTE
END_CMXDESC

START_CMXDESC(Tiling)
	CI_TAG(cmxTAG_Tiling),
	CI_READLENGTH,
	CI_READLENGTH,
	CI_READINT16,
	CI_READINT16,
	CI_READINT16,
	CI_READINT16,
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC


START_CMXDESC(AddClippingRegion)
	CI_TAG(cmxTAG_AddClippingRegion_RegionSpecification),
	CI_STORENEXT,
	CI_READINT16,			// type...
	CI_SETOUTMARKER,

	CI_IFSTOREDIS(CI_IFTYPE_VALUE, 1),
		CI_READLENGTH,
		CI_READLENGTH,
	CI_ELSEIF(CI_IFTYPE_VALUE, 2),
		CI_READLENGTH,
		CI_READLENGTH,
	CI_ELSEIF(CI_IFTYPE_VALUE, 3),
		CI_READBBOX,
	CI_ELSEIF(CI_IFTYPE_VALUE, 4),
		CI_READPOINTLIST,
	CI_ENDIF,

	CI_GOTOOUTLOC(cmxiRegionSpec),
	CI_ENDTAG,
	CI_TAG(cmxTAG_AddClippingRegion_ClipModeRecoverySpecification),
	CI_READINT16,
	CI_READINT16,
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC(ArrowShapeIndexHeader)
	CI_READINT16,
	CI_DO32,
	CI_READINT16,
	CI_DO16,
	CI_SKIPOUT(INT32),
	CI_DOALL,
	CI_READINT16
END_CMXDESC

START_CMXDESC(ArrowShapeIndexEntry)
	CI_READOFFSET
END_CMXDESC

START_CMXDESC(ArrowShape)
	CI_TAG(cmxTAG_DescrSection_Arrow),
	CI_READPOINTLIST,
	CI_READLENGTH,
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC(Arrowheads)
	CI_READREFERENCE,
	CI_READREFERENCE
END_CMXDESC

START_CMXDESC(BitmapIndexHeader)
	CI_READINT16,
	CI_DO32,
	CI_READINT16,
	CI_DO16,
	CI_SKIPOUT(INT32),
	CI_DOALL,
	CI_READINT16
END_CMXDESC

START_CMXDESC(BitmapIndexEntry)
	CI_READOFFSET
END_CMXDESC


START_CMXDESC(BeginTextStream)
	CI_TAG(cmxTAG_BeginTextStream_TextStreamSpecification),
	CI_READBOOLEAN,			//	Artistic Text
	CI_READBYTE,			//	Pad
	CI_READBBOX,			//	Bounding Box
	CI_READINT16,			//	Frames
	CI_READINT16,			//	Number of paragraph
	CI_READINT32,			//	Number of instruction in stream
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC(TextFrame)
	CI_TAG(cmxTAG_TextFrame_ColumnSpecification),
	//	Skip the column spec for we don't support them anyway
/*	CI_STORENEXT,
	CI_READINT16,
	CI_DO16,
	CI_SKIPINTIMES_BYTE(sizeof(WORD)+sizeof(WORD)),
	CI_DO32,
	CI_SKIPINTIMES_BYTE(sizeof(DWORD)+sizeof(DWORD)),
	CI_DOALL,*/
	CI_READCOLUMN,
	CI_ENDTAG,
	CI_TAG(cmxTAG_TextFrame_HeightSkewMatrix),
	CI_READLENGTH,	// height
	CI_READLENGTH,	// skew
	CI_READMATRIX,	//	Transformation
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC


START_CMXDESC(BeginParagraph)
	CI_TAG(cmxTAG_BeginParagraph_RenderingAttr),
	CI_READRENDATTR,	//	Rendering Attributes
	CI_ENDTAG,
	CI_TAG(cmxTAG_BeginParagraph_FontSpecification),
	CI_READREFERENCE,	//	Default Font
			//	Font Size
	CI_READLENGTH,
	CI_ENDTAG,
	//	Tag_BeginParagraph_UseFontCombo should be here
	//	but no clue what's that for the moment so...
	//	It doesn't appear in Corel's files.
	CI_TAG(cmxTAG_BeginParagraph_KerningSpecification),
	CI_READLENGTH,		//	Vertical shift
	CI_READLENGTH,		//	Horizontal shift
	CI_READANGLE,		//	Kerning angle
	CI_ENDTAG,
	CI_TAG(cmxTAG_BeginParagraph_Justification),
	CI_READBYTE,		//	Justification
	CI_ENDTAG,
	CI_TAG(cmxTAG_BeginParagraph_SpacingSpecification),
	//	Spacing Specification
	CI_READLENGTH,		// Inter-Char Spacing
	CI_READLENGTH,		//	Inter-Word Spacing
	CI_READLENGTH,		//	Inter-Line Spacing
	CI_READLENGTH,		//	Before Paragraph Spacing
	CI_READLENGTH,		//	After Paragraph Spacing
	CI_READLENGTH,		//	MaxInterChar
	CI_READLENGTH,		//	MinInterWord
	CI_READLENGTH,		//	MaxInterWord
	CI_READBYTE,		//	Spacing Mode
	CI_DO16,	//	No underline in v5
	CI_SKIPOUT_BYTES(sizeof(INT32)*33),
	CI_DO32,	//	Not supported in v5.
	//	Underline Specification
		//	Underline
	CI_READBYTE,		//	Flag Absolute
	CI_READINT32,		//	Distance from Baseline
	CI_READINT32,		//	Stroke thickness
		//	Thick Underline
	CI_READBYTE,		//	Flag Absolute
	CI_READINT32,		//	Distance from Baseline
	CI_READINT32,		//	Stroke thickness
		//	Double Underline
	CI_READBYTE,		//	Flag Absolute
	CI_READINT32,		//	Distance from Baseline
	CI_READINT32,		//	Stroke thickness
		//	Double Underline1
	CI_READBYTE,		//	Flag Absolute
	CI_READINT32,		//	Distance from Baseline
	CI_READINT32,		//	Stroke thickness
		//	Double Underline2
	CI_READBYTE,		//	Flag Absolute
	CI_READINT32,		//	Distance from Baseline
	CI_READINT32,		//	Stroke thickness
		//	Thick Overline
	CI_READBYTE,		//	Flag Absolute
	CI_READINT32,		//	Distance from Baseline
	CI_READINT32,		//	Stroke thickness
		//	Double Overline1
	CI_READBYTE,		//	Flag Absolute
	CI_READINT32,		//	Distance from Baseline
	CI_READINT32,		//	Stroke thickness
		//	Double Overline2
	CI_READBYTE,		//	Flag Absolute
	CI_READINT32,		//	Distance from Baseline
	CI_READINT32,		//	Stroke thickness
		//	Thick Strikeout
	CI_READBYTE,		//	Flag Absolute
	CI_READINT32,		//	Distance from Baseline
	CI_READINT32,		//	Stroke thickness
		//	Double Strikeout1
	CI_READBYTE,		//	Flag Absolute
	CI_READINT32,		//	Distance from Baseline
	CI_READINT32,		//	Stroke thickness
		//	Double Strikeout2
	CI_READBYTE,		//	Flag Absolute
	CI_READINT32,		//	Distance from Baseline
	CI_READINT32,		//	Stroke thickness
	CI_DOALL,
	CI_ENDTAG,
	CI_TAG(cmxTAG_BeginParagraph_TabSpecification),
	//	Skip the tab for we don't support them anyway
	CI_STORENEXT,
	CI_READINT16,
	CI_DO16,
	CI_SKIPINTIMES_BYTE(sizeof(WORD)+sizeof(BYTE)),
	CI_DO32,
	CI_SKIPINTIMES_BYTE(sizeof(DWORD)+sizeof(BYTE)),
	CI_DOALL,
	CI_ENDTAG,
	CI_TAG(cmxTAG_BeginParagraph_BulletSpecification),
	CI_DO16,
	CI_SKIPOUT_BYTES(sizeof(INT32)*3),
	CI_READUINT16,		//	Bullet Character
	CI_READLENGTH,		//	Vertical Shift
	CI_READLENGTH,		//	Horizontal Shift
	CI_READANGLE,		//	Kerning angle
	CI_READRENDATTR,	//	Rendering Attributes
	CI_DO32,
	CI_STORENEXT,
	CI_READBOOLEAN,						//	BulletOn?
	CI_IFSTOREDIS(CI_IFTYPE_BOOL, TRUE),	//	Yes
		CI_READBYTE,		//	Flag MultiColour
		CI_READREFERENCE,	//	Default Font
		CI_READINT32,		//	Font Size
		CI_READUINT16,		//	Bullet Character
		CI_READLENGTH,		//	Vertical Shift
		CI_READLENGTH,		//	Horizontal Shift
		CI_READANGLE,		//	Kerning angle
		CI_READRENDATTR,	//	Rendering Attributes
	CI_ELSEIF(CI_IFTYPE_BOOL, FALSE),
		CI_SKIPOUT_BYTES(sizeof(INT32)*6),
		CI_SKIPOUT_BYTES(sizeof(double)),
		CI_SKIPOUT_BYTES(sizeof(cmxiRenderAttr)),
	CI_ENDIF,
	CI_DOALL,
	CI_ENDTAG,
	CI_TAG(cmxTAG_BeginParagraph_Indentation),
	CI_READLENGTH,		//	Left Margin
	CI_READLENGTH,		//	Right Margin
	CI_READLENGTH,		//	First Line
	CI_READLENGTH,		//	Remaining Lines
	CI_ENDTAG,
	CI_TAG(cmxTAG_BeginParagraph_Hyphenation),
	CI_READBOOLEAN,		//	Auto Hyphenation
	CI_DO32,
	CI_READBOOLEAN,		//	Break Capitalised Words
	CI_READINT32,		//	MinWordLength
	CI_READINT32,		//	MinCharsBefore
	CI_READINT32,		//	MinCharsAfter
	CI_READINT32,		//	HotZone
	CI_DO16,
	CI_SKIPOUT_BYTES(sizeof(BOOL)),
	CI_SKIPOUT_BYTES(sizeof(INT32)*3),
	CI_READINT16,		//	HotZone
	CI_ENDTAG,
	CI_TAG(cmxTAG_BeginParagraph_DropCap),
	CI_DO32,
//	CI_STORENEXT,
	CI_READBOOLEAN,		//	DropCapOn ?
//	All this chunk of info doesn't seem to be there at all
/*	CI_IFSTOREDIS(CI_IFTYPE_BOOL, TRUE),	//	Yes
		CI_READREFERENCE,	//	Default Font
		CI_READINT32,		//	Font Size
		CI_READINT32,		//	Vertical shift
		CI_READINT32,		//	Horizontal shift
		CI_READANGLE,		//	Kerning angle
	CI_ELSEIF(CI_IFTYPE_BOOL, TRUE),		//	No, then skip
		CI_SKIPOUT_BYTES(sizeof(INT32)*4),
		CI_SKIPOUT_BYTES(sizeof(double)),		
	CI_ENDIF,*/
	CI_DO16,
	CI_SKIPOUT_BYTES(sizeof(BOOL)),
	/*CI_SKIPOUT_BYTES(sizeof(INT32)*4),
	CI_SKIPOUT_BYTES(sizeof(double)),*/
	CI_DOALL,
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC


START_CMXDESC(Characters)
	CI_TAG(cmxTAG_Characters_CountIndex),
	CI_READCHARLIST,
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

START_CMXDESC(CharInfo)
	CI_TAG(cmxTAG_CharInfo_CharInfo),
	CI_STORENEXT,
		//	Usage Mask
	CI_DO16,	
	CI_READINT16,
	CI_DO32,
	CI_READUINT16,
	CI_DOALL,
	CI_SETOUTMARKER,
	CI_IFSTOREDIS(CI_IFTYPE_BITSET, (char)(cmxUSAGEMASK_UseTypeNum | cmxUSAGEMASK_UseTypeStyle)),
		CI_READREFERENCE,	//	Font Reference
	CI_ENDIF,	
	CI_GOTOOUTLOC(INT32),
	CI_SETOUTMARKER,
	CI_IFSTOREDIS(CI_IFTYPE_BITSET, cmxUSAGEMASK_UseTypeSize),
		CI_READLENGTH,	//	Type Size, different 16/32bit (not documented!!!
	CI_ENDIF,	
	CI_GOTOOUTLOC(INT32),
	CI_SETOUTMARKER,
	CI_IFSTOREDIS(CI_IFTYPE_BITSET, cmxUSAGEMASK_UseCharAngle),
		CI_READANGLE,	//	Character Angle
	CI_ENDIF,	
	CI_GOTOOUTLOC(double),
	CI_SETOUTMARKER,
	CI_IFSTOREDIS(CI_IFTYPE_BITSET, (char)(cmxUSAGEMASK_UseCharFill | cmxUSAGEMASK_UseCharOutline)),
		CI_READRENDATTR,	//	Rendering Attributes
	CI_ENDIF,
	CI_GOTOOUTLOC(cmxiRenderAttr),
	CI_SETOUTMARKER,
	CI_IFSTOREDIS(CI_IFTYPE_BITSET, cmxUSAGEMASK_UseCharHShift),
			//	Horizontal Shift
		CI_READLENGTH,
	CI_ENDIF,	
	CI_GOTOOUTLOC(INT32),
	CI_SETOUTMARKER,
	CI_IFSTOREDIS(CI_IFTYPE_BITSET, (char)cmxUSAGEMASK_UseCharVShift),
			//	Vertical Shift
		CI_READLENGTH,
	CI_ENDIF,	
	CI_GOTOOUTLOC(INT32),
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC



//	This is not documented in the Corel docs 
START_CMXDESC(FontIndexHeader)
	CI_READINT16,
	CI_DO32,
	CI_READINT16,
	CI_DO16,
	CI_SKIPOUT(INT32),
	CI_DOALL,
	CI_READINT16
END_CMXDESC

START_CMXDESC(FontIndexEntry)
	CI_READOFFSET
END_CMXDESC


START_CMXDESC(FontRecord)
	CI_TAG(cmxTAG_DescrSection_Font_FontInfo),
	CI_READSTRING,	//	Font String
	CI_READUINT32,	//	Font Style
	CI_ENDTAG,
	CI_TAG(cmxTAG_DescrSection_Font_Panose),
	CI_READBYTE,	//	bFamilyType
	CI_READBYTE,	//	bSerifStyle
	CI_READBYTE,	//	bWeight
	CI_READBYTE,	//	bProporion
	CI_READBYTE,	//	bContrast
	CI_READBYTE,	//	bStrokeVariation
	CI_READBYTE,	//	bArmStyle
	CI_READBYTE,	//	bLetterForm
	CI_READBYTE,	//	bMidline
	CI_READBYTE,	//	bXHeight
	CI_ENDTAG,
	CI_MINENDTAG
END_CMXDESC

/********************************************************************************************

>	BOOL CMXImportFilter::ReadData(void *Out, char *Instructions)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/08/96
	Inputs:		pointer to the structure to fill, instructions on what to fill it with
	Returns:	success
	Purpose:	fills the structure pointed to by Out according to the instructions

********************************************************************************************/

// some macros to make things easier to write
#define RDIN(type, name)	type name; pFile->read(&name, sizeof(name));
#define RDOUT(type, name)	{*((type *)pOut) = name; pOut += sizeof(type);}
#define RDSTORE(name)		if(StoreNext) {StoreStack[StackLevel] = (INT32)name; \
							/*TRACEUSER( "Ben", _T("store %d = 0x%x\n"), (INT32)name, (INT32)name);*/ \
							StoreNext = FALSE;}
#define SIMPLEREAD(instr, intype, outtype)	case instr:			\
							{RDIN(intype, data) RDOUT(outtype, data) \
							/*TRACEUSER( "Ben", _T("simpleread value = %d = 0x%x\n"), data, data);*/ \
							RDSTORE(data) } break;
#define READLENGTH(name)	INT32 name; if(Is32Bit) {RDIN(SDWORD, data) name = data;} \
							else {RDIN(SWORD, data) name = data;}
#define MAXSTACKLEVEL		16
#define MAXTAGLEVEL			16
#define STACKLEVELUP		{ERROR2IF((StackLevel + 1) >= MAXSTACKLEVEL, FALSE, "stack level exceeded in CMXImportFilter::ReadData"); \
							StackLevel++; StoreStack[StackLevel] = StoreStack[StackLevel - 1]; \
							OutputMarkerStack[StackLevel] = OutputMarkerStack[StackLevel - 1]; \
							IfExecStack[StackLevel] = IfExecStack[StackLevel - 1]; \
							InputMarkerStack[StackLevel] = InputMarkerStack[StackLevel - 1];}
#define STACKLEVELDOWN		{ERROR2IF(StackLevel < 1, FALSE, "unmatched stack levels in CMXImportFilter::ReadData"); \
							StackLevel--;}
#define STOREDVALUE			(StoreStack[StackLevel])
#define PARAM(n)			(Instructions[n+1])
#define OUTPUTMARKER		(OutputMarkerStack[StackLevel])
#define INPUTMARKER			(InputMarkerStack[StackLevel])

BOOL CMXImportFilter::ReadData(void *Out, char *Instructions)
{
	TRACEUSER( "Graeme", _T("readdata\n"));
	BYTE *pOut = (BYTE *)Out;

	INT32 StackLevel = 0;
	INT32 StoreStack[MAXSTACKLEVEL];
	BYTE *OutputMarkerStack[MAXSTACKLEVEL];
	INT32 InputMarkerStack[MAXSTACKLEVEL];
	BOOL IfExecStack[MAXSTACKLEVEL];	// stores the value of ExecEnabled before the IF statement
	BOOL StoreNext = FALSE;
	BOOL ExecutionEnabled = TRUE;
	BOOL IfLevel = 0;
	INT32 TagEnds[MAXTAGLEVEL];
	INT32 TagLevel = 0;
	IfExecStack[0] = TRUE;
	INT32 Count = 0;

	// right then, loop around until we've
	while(TRUE)
	{
		TRACEUSER( "Graeme", _T("LOOP %d.\n"), Count++);
		TRACEUSER( "Claude", _T("Instruction %d\n"), *Instructions);
		TRACEUSER( "Graeme", _T("Instruction %d\n"), *Instructions);
		ERROR2IF(CMXIinfo::CommandLengths[*Instructions] == 0, FALSE, "Instruction code not defined in CMXImportFilter::ReadData()");

		// first switch to handle structural stuff which controls execution of
		// other instructions
		switch(*Instructions)
		{
		case CI_STOP:
		TRACEUSER( "Claude", _T("end readdata\n"));
		TRACEUSER( "Graeme", _T("end readdata\n"));
			ERROR3IF(StackLevel != 0, "unmatched stack levels");
			ERROR3IF(IfLevel != 0, "unmatched if levels");
			return TRUE;		// done everything
			break;

		case CI_IFSTOREDIS_VAL:
			{
				// set the exec stack
				IfExecStack[StackLevel] = ExecutionEnabled;

				BOOL Test = FALSE;
				if(PARAM(0) == CI_IFTYPE_VALUE)
				{
					if(STOREDVALUE == PARAM(1))
						Test = TRUE;
				}
				else if(PARAM(0) == CI_IFTYPE_BITSET)
				{
					if((STOREDVALUE & PARAM(1)) != 0)
						Test = TRUE;
				}
				else if(PARAM(0) == CI_IFTYPE_BOOL)
				{
					BOOL Val = (STOREDVALUE == 0)?FALSE:TRUE;
					if(Val == PARAM(1))
						Test = TRUE;
				}
				if(!ExecutionEnabled)
					Test = FALSE;

				ExecutionEnabled = Test;

				IfLevel++;
				STACKLEVELUP
			}
			break;

		case CI_ELSEIF_VAL:
			{
				ERROR2IF(IfLevel <= 0, FALSE, "Haven't got an if being active right now");
				STACKLEVELDOWN

				BOOL Test = FALSE;
				if(PARAM(0) == CI_IFTYPE_VALUE)
				{
					if(STOREDVALUE == PARAM(1))
						Test = TRUE;
				}
				else if(PARAM(0) == CI_IFTYPE_BITSET)
				{
					if((STOREDVALUE & PARAM(1)) != 0)
						Test = TRUE;
				}
				else if(PARAM(0) == CI_IFTYPE_BOOL)
				{
					BOOL Val = (STOREDVALUE == 0)?FALSE:TRUE;
					if(Val == PARAM(1))
						Test = TRUE;
				}
				if(!IfExecStack[StackLevel])
					Test = FALSE;

				ExecutionEnabled = Test;

				STACKLEVELUP
			}
			break;

		case CI_ENDIF:
			ERROR2IF(IfLevel <= 0, FALSE, "Can't end an if which didn't start");
			IfLevel--;

			STACKLEVELDOWN

			ExecutionEnabled = IfExecStack[StackLevel];
			break;

		default:
			break;				// not a problem here
		}

		// second switch to handle data instructions
		if(ExecutionEnabled)
		{
			switch(*Instructions)
			{
			// --------------- data reading codes ------------------
			SIMPLEREAD(CI_READBYTE, BYTE, INT32)
			SIMPLEREAD(CI_READINT16, SWORD, INT32)
			SIMPLEREAD(CI_READINT32, SDWORD, INT32)
			SIMPLEREAD(CI_READUINT16, WORD, UINT32)
			SIMPLEREAD(CI_READUINT32, DWORD, UINT32)
			SIMPLEREAD(CI_READREFERENCE, WORD, INT32)
			SIMPLEREAD(CI_READOFFSET, DWORD, UINT32)
			SIMPLEREAD(CI_READDOUBLE, double, double)

			case CI_READLENGTH:
				{
					READLENGTH(len)
					RDOUT(INT32, len)
					RDSTORE(len)
				}
				break;

			case CI_READCOORD:
				{
					READLENGTH(x)
					READLENGTH(y)
					DocCoord coord(x, y);
					RDOUT(DocCoord, coord)
					RDSTORE(x)
				}
				break;

			case CI_READBBOX:
				{
					READLENGTH(x0)
					READLENGTH(y1)
					READLENGTH(x1)
					READLENGTH(y0)
					DocRect rect;
					if(x0 > x1)
					{
						INT32 t = x0;
						x0 = x1;
						x1 = t;
					}
					if(y0 > y1)
					{
						INT32 t = y0;
						y0 = y1;
						y1 = t;
					}
					rect.lo.x = x0; rect.lo.y = y0;
					rect.hi.x = x1; rect.hi.y = y1;
					RDOUT(DocRect, rect)
					RDSTORE(x0)
				}
				break;

			case CI_READNATURALWORD:
				{
					UINT32 word;
					if(Is32Bit)
					{
						RDIN(DWORD, data)
						word = data;
					}
					else
					{
						RDIN(WORD, data)
						word = data;
					}
					RDOUT(UINT32, word)
					RDSTORE(word)
				}
				break;

			case CI_READBOOLEAN:
				{
					RDIN(BYTE, data)
					BOOL flag = (data == 0)?FALSE:TRUE;
					RDOUT(BOOL, flag)
					RDSTORE(flag)
				}
				break;

			case CI_READANGLE:
				{
					double Angle;
					if(Is32Bit)
					{
						RDIN(SDWORD, an)
						Angle = (an * 2 * PI) / 360000000.0;
					}
					else
					{
						RDIN(SWORD, an)
						Angle = (an * 2 * PI) / 3600.0;
					}
					// check the range of the thingy
					if(Angle < 0)
					{
						while(Angle < 0) Angle += (2*PI);
					}
					else if(Angle > (2*PI))
					{
						while(Angle > (2*PI)) Angle -= (2*PI);
					}
					RDOUT(double, Angle)
					RDSTORE(Angle)
				}
				break;

			case CI_READRENDATTR:
				// we simply call this function with the appropriate structure defn -- easy!
				// it may seem a little silly to do it like this, but it's so much easier than
				// putting this ever so common function in every single object defn
//TRACEUSER( "Ben", _T("**************** rend attr"));
				if(!ReadData(pOut, cmxiRenderAttrDesc))
					return FALSE;
				pOut += sizeof(cmxiRenderAttr);
				break;

			case CI_READTILING:
				// see comments about about why we have a seperate thingy for this
				if(!ReadData(pOut, cmxiTilingDesc))
					return FALSE;
				pOut += sizeof(cmxiTiling);
				break;

			case CI_READPOINTLIST:
				// calls a nice function which reads a point list from the file
				Path *pPath;
				if(!ReadPointList(&pPath))
					return FALSE;
				RDOUT(Path *, pPath)
				break;

			case CI_READCOLOURLIST:
				{
					// call our nice function to do all the hard work for me
					// but of course, I've written it too, so it's not as if
					// I escape writing the thing. Which is a pity really.
					// There should be lots of parsnips given out free in the
					// streets, because parsnips are very good things. They're
					// really tasty, and make an excellent accompanyment for
					// other parsnips. What's especially good, though, is
					// parsnip bake. What you do is chop up the lovely parsnips
					// (apologise to them first for spoiling their good looks)
					// into nice slices, and bung them into a pot type thing.
					// What you want is layers of pasnip slices alternating
					// with yogurt and cheese -- the layers shouldn't be very
					// thick of either, but you don't want too much yogurt
					// and cheese. On the top, you pop some more yogurt down
					// and cover with a nice layer of cheese. Bung in the oven
					// at about 200 degrees, and cook until it's lovely and
					// moist, and completely yummy.
					// what we need, apart from more parsnips, is to have some
					// funny little green men to write all our code for us,
					// so we don't have to do it ourselves. But you know, that
					// could possibly put us out of work, so maybe it isn't such
					// a good plan. I suppose I ought to go and write that little
					// function, but it's important to think about parsnips,
					// because they are very good.
					CMXImportColourList *pColList = ReadColourList();
					if(pColList == NULL)
						return FALSE;

					RDOUT(CMXImportColourList *, pColList)
				}
				break;

			case CI_READSTRING:
				{
					// we have a WORD giving count of bytes in the string,
					// followed by the bytes of the string
					RDIN(WORD, count)

					TCHAR *text = (TCHAR *)CCMalloc(count + 4);
					if(text == NULL)
						return FALSE;

					// read the characeters from the file
					pFile->read(text, count);

					text[count] = '\0';		// terminate the string

					// create a string object
					StringBase *pString = new StringBase;
					if(!pString->Alloc(count + 4))
						return FALSE;
					(*pString) = text;
					if(pString == NULL)
						return FALSE;

					// output it
					RDOUT(StringBase *, pString)

					// free the temporary buffer
					CCFree(text);
				}
				break;

	
			case CI_READCHARLIST:
				{
					RDIN(DWORD, count)
					
					characterList	c;

					c.count			=	count;
					c.characters	=	new WCHAR[c.count];
					c.indices		=	new WORD[c.count];

					if((c.characters == NULL)||(c.indices ==NULL))
						return FALSE;
					
					for(UINT32 i =0 ; i < count ; i++)
					{
						RDIN(WORD, character)
						c.characters[i] = character;
					}

					for(i = 0 ; i < count ; i++)
					{
						RDIN(WORD, index)
						if(index == cmxMaxValueWORD)
							CorelDraw7 =TRUE;
						c.indices[i] = index;
					}

					RDOUT(characterList, c)
					
				}
				break;

			case CI_READCOLUMN:
				{	
					RDIN(SWORD, count)
					cmxiColumnList	columns;
					columns.count	=	count;
					columns.columns	=	new	INT32[columns.count];
					columns.gutters	=	new INT32[columns.count];

					if((columns.columns == NULL)||(columns.gutters == NULL))
						return FALSE;
					
					for(INT32 i =0 ; i < count ; i++)
					{
						READLENGTH(col)
						columns.columns[i] = col;
						READLENGTH(gut)
						columns.gutters[i] = gut;
					}


					RDOUT(cmxiColumnList, columns)


				}
				break;

	/*		case CI_READ_FONTINDEX:
				{
					RDIN(SWORD, count)
					RDIN(SWORD, size)
					RDIN(SWORD, type)
					fontList	fonts;
					fonts.count =	count;
					fonts.size	=	size;
					fonts.type	=	type;
					fonts.offsets	=	new UINT32[count];
					for(INT32 i = 0 ; i < fonts.count ; i++)
					{
						RDIN(DWORD, offset)
						fonts.offsets[i]	=	offset;
					}

					RDOUT(fontList, fonts) 
				}
				break;*/

			case CI_READMATRIX:
				{
					RDIN(WORD, Type)
					Matrix Mat;		// identity matrix
					if(Type == cmxMATRIXTYPE_GENERAL)
					{
						// OK, so it's not a identity matrix, but a proper one
						// better load it then, I suppose.
						struct cmxmatdo {
							double a,b,c,d,e,f;
						};
						RDIN(struct cmxmatdo, in)
						Mat = Matrix(in.a, in.b, in.c, in.d, (INT32)in.e, (INT32)in.f);
					}
					RDOUT(Matrix, Mat)
				}
				break;

			case CI_READSTRINGF_VAL:
				// read a fixed length string from the file... quite easy this one really
				pFile->read(pOut, PARAM(0));
				pOut += PARAM(0);
				break;

			case CI_READINT16ARRAY:
				{
					RDIN(WORD, Count)
					cmxiIntArray ar;
					ar.NumElements = Count;
					if(Count != 0)
					{
						ar.pElements = new INT32[Count];
						if(ar.pElements == NULL)
							return FALSE;		// couldn't allocate array
						for(INT32 l = 0; l < Count; l++)
						{
							RDIN(WORD, El)
							ar.pElements[l] = El;
						}
					}
					else
					{
						ar.pElements = NULL;
					}
					RDOUT(cmxiIntArray, ar)
				}
				break;

			case CI_TRANSPARENCY:
				{	//	cannot deal with that yet so we're going to tell them

				TRACEUSER( "Claude", _T("Transparency stuffs\n"));
				TRACEUSER( "Graeme", _T("Transparency stuffs\n"));
					
				}
				break;

			// --------------- structural bits ------------------
			case CI_SKIPIN_VAL:
				{
					char dummy;
					for(INT32 l = 0; l < PARAM(0); l++)
						pFile->get(dummy);
				}
				break;

			case CI_SKIPINTIMES_VAL:
				{
					char dummy;
					for(INT32 l = 0; l < (PARAM(0) * STOREDVALUE); l++)
						pFile->get(dummy);
				}
				break;

			case CI_SKIPOUT_VAL:
				{
					BYTE nul = 0;
					for(INT32 l = 0; l < PARAM(0); l++)
						RDOUT(BYTE, nul)
				}
				break;

			case CI_STORENEXT:
				StoreNext = TRUE;
				break;
			
			// do16 and d32 are implemented here to avoid clashes with the
			// if - elseif - endif system
			case CI_DO16:
				{
					if(Is32Bit)
					{
						// scan forward to find CI_DO32 or CI_DOALL
						while(TRUE)
						{
							if((*Instructions) == CI_DO32 || (*Instructions) == CI_DOALL)
								break;

							if((*Instructions) == CI_STOP)
								return TRUE;			// all done

							// next thingy
							Instructions += CMXIinfo::CommandLengths[*Instructions];
						}
					}
				}
				break;

			case CI_DO32:
				{
					if(!Is32Bit)
					{
						// scan forward to find CI_DO16 or CI_DOALL
						while(TRUE)
						{
							if((*Instructions) == CI_DO16 || (*Instructions) == CI_DOALL)
								break;

							if((*Instructions) == CI_STOP)
								return TRUE;			// all done

							// next thingy
							Instructions += CMXIinfo::CommandLengths[*Instructions];
						}
					}
				}
				break;

			case CI_DOALL:
				// don't worry, chaps, we are
				break;

			case CI_TAG_VAL:
				if(Is32Bit)			// tags are ignored in 16 bit files
				{
					ERROR2IF((TagLevel + 1) >= MAXTAGLEVEL, FALSE, "too many nested tags");
					// read data structure, trying to find the right tag
					TagLevel++;
					TRACEUSER( "Claude", _T("tag = %d, level = %d\n"), PARAM(0), TagLevel);
					TRACEUSER( "Graeme", _T("tag = %d, level = %d\n"), PARAM(0), TagLevel);
					while(TRUE)
					{
						INT32 start = Tell();
						RDIN(BYTE, type)
						if(type == cmxTAG_EndTag)
							CMXFORMATERROR(FALSE)		// the tag wasn't there

						// get the size of the tag
						RDIN(WORD, Size)
						if(type == PARAM(0))
						{
							// found the right one
							TagEnds[TagLevel] = start + Size;
							break;
						}
						// otherwise skip to the next one
						TRACEUSER( "Ben", _T("WARNING: tag skipped unknown tag, type = %d\n"), type);
						TRACEUSER( "Claude", _T("WARNING: tag skipped unknown tag, type = %d\n"), type);
						TRACEUSER( "Graeme", _T("WARNING: tag skipped unknown tag, type = %d\n"), type);
						Seek(start + Size);
//						RDIN(BYTE, test)
				//	TRACEUSER( "Claude", _T("test = %d\n"), test);	
//						break;
					}
				}
				break;

			case CI_TAG_IF_THERE:
				if(Is32Bit)
				{
					ERROR2IF((TagLevel + 1) >= MAXTAGLEVEL, FALSE, "too many nested tags");
					
					TagLevel++;
					TRACEUSER( "Claude", _T("tag = %d, level = %d\n"), PARAM(0), TagLevel);
					TRACEUSER( "Graeme", _T("tag = %d, level = %d\n"), PARAM(0), TagLevel);
					
					INT32 start = Tell();
					RDIN(BYTE, type)
					if(type == cmxTAG_EndTag)
					{
						Seek(start);
						break;
					}
				
					// get the size of the tag
					RDIN(WORD, Size)
					if(type == PARAM(0))
					{
						// found the right one
						TagEnds[TagLevel] = start + Size;
						Seek(start + Size);	//	WARNING! This actually skip the info 
											//	for the Frozen viewpoint thingy, as it did
											//	before but it's less obvious now.
						TagLevel--;
						break;
					}
					
					
					// otherwise skip to the next one
					TRACEUSER( "Claude", _T("WARNING: tag skipped unknown tag, type = %d\n"), type);
					TRACEUSER( "Graeme", _T("WARNING: tag skipped unknown tag, type = %d\n"), type);
					Seek(start + Size);
				}
				break;

			case CI_ENDTAG:
				if(Is32Bit)			// tags are ignored in 16 bit files
				{
//TRACEUSER( "Ben", _T("endtag, level = %d\n"), TagLevel);
					// make sure the file is in the correct position for the next thing
					ERROR2IF(TagLevel < 1, FALSE, "no tag when we tried to end");
					if(Tell() != TagEnds[TagLevel])
					{
						TRACEUSER( "Ben", _T("WARNING: end tag skipped %d bytes of data\n"), TagEnds[TagLevel] - Tell());
						TRACEUSER( "Graeme", _T("WARNING: end tag skipped %d bytes of data\n"), TagEnds[TagLevel] - Tell());
						Seek(TagEnds[TagLevel]);
					}
					TagLevel--;
				}
				break;

			case CI_MINENDTAG:
				if(Is32Bit)
				{
//TRACEUSER( "Ben", _T("minendtag\n"));
					// see if we can find the end tag -- there may be tags to skip
					while(TRUE)
					{
						RDIN(BYTE, Thingy)
						if(Thingy == cmxTAG_EndTag)
						{
							break;
						}
						else
						{
							TRACEUSER( "Ben", _T("WARNING: min end tag skipped unknown tag, type = %d\n"), Thingy);
							TRACEUSER( "Graeme", _T("WARNING: min end tag skipped unknown tag, type = %d\n"), Thingy);
							RDIN(WORD, size)
							Seek(Tell() + size - sizeof(BYTE) - sizeof(WORD));
						}
					}
				}
				break;

			case CI_SETOUTMARKER:
				OutputMarkerStack[StackLevel] = pOut;
				break;

			case CI_GOTOOUTLOC_VAL:
				// we need to make the output location n bytes after the
				// last marker set
				pOut = OUTPUTMARKER + PARAM(0);
				break;

			case CI_SETINMARKER:
				InputMarkerStack[StackLevel] = Tell();
				break;

			case CI_ENSUREINLOC:
				// we have a SizeInFile value in the stored value
				// we need to ensure that the correct file location is
				// the input marker plus that value
				{
					INT32 TargetPos = INPUTMARKER + STOREDVALUE;
					if(Tell() != TargetPos)
						Seek(TargetPos);
				}
				break;

			// --------------- debug ------------------

#ifdef _DEBUG
			case CI_TRACE_VAL:
				TRACEUSER( "Ben", _T("rd TRACE: %d, out = %x, in = %x, stored = %d = 0x%x, iflevel = %d, stacklevel = %d\n"), PARAM(0), pOut, Tell(), STOREDVALUE, STOREDVALUE, IfLevel, StackLevel);
				TRACEUSER( "Claude", _T("rd TRACE: %d, out = %x, in = %x, stored = %d = 0x, iflevel = %d, stacklevel = %d\n"), PARAM(0), pOut, Tell(), STOREDVALUE, STOREDVALUE, IfLevel, StackLevel);
				TRACEUSER( "Graeme", _T("rd TRACE: %d, out = %x, in = %x, stored = %d = 0x, iflevel = %d, stacklevel = %d\n"), PARAM(0), pOut, Tell(), STOREDVALUE, STOREDVALUE, IfLevel, StackLevel);
				break;
#endif

			// --------------- catch all ------------------
			default:
				ERROR3("Defined instruction code not implemented");
				break;

			// things which are implemented in the first switch
			case CI_STOP:
			case CI_IFSTOREDIS_VAL:
			case CI_ELSEIF_VAL:
			case CI_ENDIF:
				break;
			}
		}
		// get ready to do the next thingy
		Instructions += CMXIinfo::CommandLengths[*Instructions];
	}

	return FALSE;
}


/********************************************************************************************

>	BOOL CMXImportFilter::ReadPointList(Path **ppPath)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/08/96
	Inputs:		pointer to a pointer to return the new path in
	Returns:	success
	Purpose:	converts a point list from the file into one of our paths
				it isn't transformed or anything

********************************************************************************************/

BOOL CMXImportFilter::ReadPointList(Path **ppPath)
{
	(*ppPath) = 0;
	// what we have in the file...
	// WORD Count
	// appropriate number of coords
	// appropriate number of verbs

	RDIN(WORD, Count)

	INT32 NumCoords = Count;
	DocCoord *pCoords = new DocCoord[NumCoords];
	if(pCoords == NULL)
		return FALSE;

	// read in and convert the coordinates
	if(Is32Bit)
	{
		cmxPoint32 PointyThing;

		for(INT32 l = 0; l < NumCoords; l++)
		{
			pFile->read(&PointyThing, sizeof(PointyThing));
			pCoords[l].x = PointyThing.x;
			pCoords[l].y = PointyThing.y;
		}
	}
	else
	{
		cmxPoint16 PointyThing;

		for(INT32 l = 0; l < NumCoords; l++)
		{
			pFile->read(&PointyThing, sizeof(PointyThing));
			pCoords[l].x = PointyThing.x;
			pCoords[l].y = PointyThing.y;
		}
	}

	// now create the path
	Path *pPath = new Path;
	if(pPath == 0 || !pPath->Initialise())
		return FALSE;

	pPath->IsFilled = FALSE;
	pPath->IsStroked = TRUE;

	// position the new elements at the beginning of the path
	pPath->FindStartOfPath();

	// run through inserting the elements

	INT32 c;					// coord counter
	PathFlags Flags;
	BOOL NeedMoveTo = TRUE;

	// go though converting all the coords
	for(c = 0; c < NumCoords; c++)
	{
		RDIN(BYTE, Verb)

		INT32 CoordType = Verb & CMXNODETYPE_TYPE_MASK;

		// add in a move to if necessary
		if(NeedMoveTo && CoordType != CMXNODETYPE_TYPE_MOVE)
		{
			if(!pPath->InsertMoveTo(pCoords[c]))
				return FALSE;
		}

		NeedMoveTo = FALSE;
		
		// process the coordinate		
		switch(CoordType)
		{
			case CMXNODETYPE_TYPE_MOVE:
				// add a move to this path
				if(!pPath->InsertMoveTo(pCoords[c]))
					return FALSE;
				break;
			
			case CMXNODETYPE_TYPE_LINE:
				// add a line to this coord to the path
				if(!pPath->InsertLineTo(pCoords[c]))
					return FALSE;
				break;
			
			case CMXNODETYPE_TYPE_CURVE:
				// check we have some control points for this curve
				// a control point cannot be the first coord, so it's OK to check against 0
				if(c < 2)
				{
					TRACEUSER( "Ben", _T("No control points for curve element\n"));
					break;
				}

				// create the curve
				Flags.IsSelected = FALSE;
				Flags.IsSmooth = Flags.IsRotate = ((Verb & CMXNODETYPE_CONTINUITYMASK) != 0)?TRUE:FALSE;
				Flags.IsEndPoint = FALSE;
				
				// insert it into the path
				if(!pPath->InsertCurveTo(pCoords[c-2], pCoords[c-1], pCoords[c], &Flags))
					return FALSE;
				break;

			case CMXNODETYPE_TYPE_ARC:
				// control points for a node
				break;
			
			default:
				// can't get here as mask won't allow any other value than the ones above.
				break;
		}

		// is this a close subpath situtation?
		if((Verb & CMXNODETYPE_CLOSED) != 0 && CoordType != CMXNODETYPE_TYPE_ARC)
		{
			// close the sub path
			if(CoordType != CMXNODETYPE_TYPE_MOVE)
			{
				if(!pPath->CloseSubPath())
					return FALSE;

				// ensure that the next coord is a moveto
				NeedMoveTo = TRUE;

				// mark the path as possibly filled
				pPath->IsFilled = TRUE;
			}
		}
	}

	// should have done it.

	// delete the coordinate array
	delete [] pCoords;

	// check the path
	if(!pPath->EnsureValid())
	{
		// let's just knock up a quick valid one to keep everything happy.
		// it's a bodge, so it'll feel nicely at home in Camelot.
		pPath->ClearPath();
		pPath->InsertMoveTo(DocCoord(0,0));
		pPath->InsertLineTo(DocCoord(72,72));
	}

	// return the path thingy
	(*ppPath) = pPath;

	return TRUE;
}


/********************************************************************************************

>	CMXImportColourList *CMXImportFilter::ReadColourList(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Inputs:		none
	Returns:	colour list or 0 for error
	Purpose:	reads a nice colour list thing from the CMX file

********************************************************************************************/

CMXImportColourList *CMXImportFilter::ReadColourList(void)

{
	// get a new colour list object to play with
	CMXImportColourList *pColList = new CMXImportColourList;
	if(pColList == NULL)
		return NULL;

	// get the count of stuff
	RDIN(WORD, Count);
//	TRACEUSER( "Ben", _T("Colour list has %d entries\n"), Count);

	for(INT32 l = 0; l < Count; l++)
	{
		// load the thing in
		RDIN(cmxGradFillColour, col);

		// add it to the list
		if(!pColList->AddColour(col.ColRef, col.ColPos))
			return NULL;
	}

	return pColList;
}

/********************************************************************************************

>	void CMXIinfo::SetUpInfo(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/08/96
	Inputs:		none
	Returns:	none
	Purpose:	fixes up the array of command lengths

********************************************************************************************/

void CMXIinfo::SetUpInfo(void)
{
	char SingleBytes[] = {CI_READBYTE, CI_READINT16, CI_READINT32, CI_READUINT16,
			CI_READUINT32, CI_READREFERENCE, CI_READLENGTH, CI_READCOORD, CI_READBBOX,
			CI_READBBOX, CI_READNATURALWORD, CI_READOFFSET,
			CI_READBOOLEAN, CI_READANGLE, CI_READDOUBLE, CI_STOP,
			CI_STORENEXT, CI_ENDIF, CI_DO16, CI_DO32, CI_DOALL,
			CI_ENDTAG, CI_MINENDTAG, CI_SETOUTMARKER, CI_READRENDATTR,
			CI_READPOINTLIST, CI_READCOLOURLIST, CI_READSTRING, CI_READMATRIX,
			CI_SETINMARKER, CI_ENSUREINLOC, CI_READTILING, CI_READINT16ARRAY,
			CI_SETINMARKER, CI_ENSUREINLOC, CI_READTILING,CI_READPOINTLIST, CI_READCOLOURLIST, 
			CI_READSTRING, CI_READCHARLIST,	CI_READMATRIX, CI_SETINMARKER, CI_ENSUREINLOC, 
			CI_READCOLUMN, CI_TRANSPARENCY};
	char OneParam[] = {CI_SKIPIN_VAL, CI_SKIPOUT_VAL, CI_TAG_VAL, CI_GOTOOUTLOC_VAL, CI_TAG_IF_THERE,
#ifdef _DEBUG
			CI_TRACE_VAL,
#endif
			CI_SKIPINTIMES_VAL, CI_READSTRINGF_VAL};
	char TwoParam[] = {CI_IFSTOREDIS_VAL, CI_ELSEIF_VAL};

	memset(CommandLengths, 0, sizeof(CommandLengths));
	SetLengths(SingleBytes, sizeof(SingleBytes), 1);
	SetLengths(OneParam, sizeof(OneParam), 2);
	SetLengths(TwoParam, sizeof(TwoParam), 3);
	CommandLengths[0] = 1;

/*#ifdef _DEBUG
	for(INT32 l = 0; l < 256; l++)
	{
		TRACEUSER( "Ben", _T("en %d = %d\n"), l, CommandLengths[l]);
	}
#endif*/
}


/********************************************************************************************

>	static void CMXIinfo::SetLengths(char *instrs, INT32 num, INT32 val)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/08/96
	Inputs:		none
	Returns:	none
 	Purpose:	inserts lengths into the length array

********************************************************************************************/

void CMXIinfo::SetLengths(char *instrs, INT32 num, INT32 val)
{
	for(INT32 l = 0; l < num; l++)
	{
		CommandLengths[instrs[l]] = val;
	}
}

char CMXIinfo::CommandLengths[256];



