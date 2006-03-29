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
// Structures for access to CDR data formats

/*
*/


// this is in the winoil to allow me to use the pragma pack thingy - compiler dependant

#ifndef INC_CDRFORM
#define INC_CDRFORM

#include "cdrdata.h"

#pragma pack(push, 1)	// set the compiler to pack to byte alignment - most handy!
						// this must be matched by a pop at the end of the file to avoid
						// things going deeply wrong.


// IMPORTANT: all data in this file should be accessed through the macros in
// intel\cdrdata.h, except for bytes.

// Naming convention: Version 4 structures have suffix V4, version 3 suffix V3, version
// 6 suffix V6. Unless a structure of the same name with the correct prefix exists for
// a particular version, use the unsuffixed version 5 structure.

//******************************************************************  CDR chunk types

#define cdrT_CDR  0x20524443
#define cdrT_CDR5 0x35524443
#define cdrT_CDR4 0x34524443
#define cdrT_doc  0x20636f64
#define cdrT_mcfg 0x6766636d
#define cdrT_page 0x65676170
#define cdrT_layr 0x7279616c
#define cdrT_lgob 0x626f676c
#define cdrT_loda 0x61646f6c
#define cdrT_obj  0x206a626f
#define cdrT_grp  0x20707267
#define cdrT_trfl 0x6c667274
#define cdrT_trfd 0x64667274
#define cdrT_filt 0x746c6966
#define cdrT_fill 0x6c6c6966
#define cdrT_lnkg 0x676b6e6c
#define cdrT_otlt 0x746c746f
#define cdrT_outl 0x6c74756f
#define cdrT_arrt 0x74727261
#define cdrT_arrw 0x77727261
#define cdrT_lobj 0x6a626f6c
#define cdrT_bpft 0x74667062
#define cdrT_bmpf 0x66706d62
#define cdrT_bmpt 0x74706d62
#define cdrT_bmp  0x20706d62
#define cdrT_stlt 0x746c7473
#define cdrT_styl 0x6c797473
#define cdrT_styd 0x64797473
#define cdrT_vect 0x74636576
#define cdrT_spnd 0x646e7073
#define cdrT_bbox 0x786f6262
#define cdrT_flgs 0x73676c66
#define cdrT_fntt 0x74746e66
#define cdrT_font 0x746e6f66
#define cdrT_txsm 0x6d737874
#define cdrT_tspc 0x63707374
#define cdrT_ftbl 0x6c627466
#define cdrT_lnkt 0x746b6e6c
#define cdrT_btxt 0x74787462
#define cdrT_strl 0x6c727473
#define cdrT_btid 0x64697462
#define cdrT_parl 0x6c726170
#define cdrT_para 0x61726170
#define cdrT_bnch 0x68636e62
#define cdrT_bsch 0x68637362
#define cdrT_envd 0x64766e65
#define cdrT_CDST 0x54534443
#define cdrT_CDT5 0x35544443

#define DEFFOURCC(n, v) n = (((v&0xff)<<24)|((v&0xff00)<<8)|((v&0xff0000)>>8)|((v&0xff000000)>>24))

enum {
	DEFFOURCC(cdrT_CDR6, 'CDR6'),
	DEFFOURCC(cdrT_CDT6, 'CDT6'),
	DEFFOURCC(cdrT_cmx1, 'cmx1')
};

//******************************************************************  Document global

#define CDRCOORDS_TO_MILLIPOINTS	72

typedef struct {
	WORD PageX;				// size of page
	WORD PageY;
	WORD unknown1;
	WORD Orientation;		// orientation of page - doesn't affect pagex and pagey
	BYTE unknown2[48];
} cdrfPageInfo;				// in a doc.mcfg chunk

typedef struct {
	WORD unknown[2];
	WORD PageY;				// size of page divided by two
	WORD PageX;
} cdrfPageInfoV3;			// version 3, in page.mcfg

#define cdrf_ORIENTATION_LANDSCAPE	1
#define cdrf_ORIENTATION_PORTRAIT	0

typedef struct {
	WORD Size;
	WORD unknown[15];
	BYTE Name[32];
} cdrfMasterLayerInfo;		// only gets the name on a master page

#define cdrfLAYERFLAGS_INVISIBLE	0x0100
#define cdrfLAYERFLAGS_LOCKED		0x0010

typedef struct {
	WORD Size;
	BYTE unknown[23];
	BYTE Name[32];
} cdrfMasterLayerInfoV3;

typedef struct {
	SWORD X, Y;
} cdrfCoord;

typedef struct {
	SWORD x0, y0, x1, y1;
} cdrfBBox;

//******************************************************************  General offset header

typedef struct {
	WORD Size;
	WORD NOffsets;			// number of offsets 
	WORD OffsetsOffset;		// offset to list of offsets
	WORD TypesOffset;		// offset to list of types (reverse order)
	WORD ObjectType;
} cdrfOffsetHeader;

	// offset types, for objects
#define cdrfOBJOFFSETTYPE_COORDS        0x001e
#define cdrfOBJOFFSETTYPE_FILL          0x0014
#define cdrfOBJOFFSETTYPE_LINE          0x000a
#define cdrfOBJOFFSETTYPE_TRANSFORM_V3	0x0064
#define cdrfOBJOFFSETTYPE_STYLE			0x00c8
#define cdrfOBJOFFSETTYPE_LENS			0x1f40			// only in version 5

#define cdrfOBJOFFSETTYPE_PATHTEXT1		0x0bb8
#define cdrfOBJOFFSETTYPE_PATHTEXT2		0x0bc2
#define cdrfOBJOFFSETTYPE_POWERCLIP		0x1f45

//******************************************************************  Transformations

typedef struct {
	WORD Size;
	WORD NTransforms;			// number of transformations
	WORD unknown[3];
	WORD TransformOffsets[4];	// offsets to the transformations
} cdrfTransform;				// trfd chunk

typedef struct {
	WORD Type;					// type of transformation
} cdrfTransformHeader;

#define cdrfTRANSFORMTYPE_ENVELOPE		1
#define cdrfTRANSFORMTYPE_PERSPECTIVE	2
#define cdrfTRANSFORMTYPE_EXTRUDE		4
#define cdrfTRANSFORMTYPE_MATRIX		8

typedef struct {
	WORD Type;
	BYTE Transform[8*6];		// elements, stored as 8 byte IEEE floats, but last 4
								// bytes first
} cdrfTransformMatrix;

typedef struct {
	WORD unknown[2];
	SWORD Ox0, Oy1, Ox1, Oy0;
	cdrfCoord NewCorners[4];
} cdrfPerspectiveTransform;

typedef struct {
	WORD Type;
	cdrfPerspectiveTransform Trans;
} cdrfPerspectiveEntry;

typedef struct {
	WORD Size;
	WORD NTransforms;
	WORD OffsetsStart;
} cdrfTransformV4;

typedef struct {
	WORD Type;
	WORD Transform[2*6];		// stored as fixed point 16.16 numbers for a, b, d and e
								// and doc coords for c and f.
} cdrfTransformMatrixV4;

typedef struct {
	WORD unknown;
	WORD NTransforms;
} cdrfTransformV3;		// followed by transform entries

typedef struct {
	WORD Type;
	WORD unknown[2];
	WORD Offset;
	WORD unknown2;
} cdrfTransformEntryV3;

typedef struct {
	WORD Transform[2*6];		// as for V4
} cdrfTransformMatrixV3;

typedef struct {
	WORD unknown[2];
	SWORD Ox0, Oy0, Ox1, Oy1;
	WORD unknown2[4];
	WORD NCoords;				// number of coordinates following
	cdrfCoord Coords[25];
} cdrfTransformEnvelopeV3;

#define cdrfTRANSFORMTYPE_MATRIX_V3			0x8c
#define cdrfTRANSFORMTYPE_EXTRUDE_V3		0x3c
#define cdrfTRANSFORMTYPE_PERSPECTIVE_V3	0x28
#define cdrfTRANSFORMTYPE_ENVELOPE_V3		0x14

// perspective in 4 and 5 files...

typedef struct {
	WORD Type;
	WORD unknown[2];
	WORD Ox0, Oy0, Ox1, Oy1;
	WORD Corners[4];				// coords numbers of the corners of the mould
} cdrfTransformEnvelope;

// and the number of coords is in the object type field
#define cdrfENVELOPEOFFSETTYPE_COORDS	0x001e
#define cdrfENVELOPEOFFSETTYPE_TYPES	0x0028

typedef struct
{
	WORD	cf11a;
	SWORD	cf11b;
	WORD	cf21a;
	SWORD	cf21b;
	WORD	cf31a;
	SWORD	cf31b;
	FLOAT4	f41;
	WORD	cf12a;
	SWORD	cf12b;
	WORD	cf22a;
	SWORD	cf22b;
	WORD	cf32a;
	SWORD	cf32b;
	FLOAT4	f42;
	WORD	cf13a;
	SWORD	cf13b;
	WORD	cf23a;
	SWORD	cf23b;
	WORD	cf33a;
	SWORD	cf33b;
	FLOAT4	f43;

	SWORD	nXRotation;
	SWORD	nYRotation;
	SWORD	nZRotation;
	SWORD	Cx,Cy;
	SDWORD	Cz;			// camera positions
} cdrfTransformExtrudeV3;
 
typedef struct			// versions 4,5,6
{
	WORD	Type;					// from it's header
	WORD	cf11a;
	SWORD	cf11b;
	WORD	cf21a;
	SWORD	cf21b;
	WORD	cf31a;
	SWORD	cf31b;
	SDWORD	f41;
	WORD	cf12a;
	SWORD	cf12b;
	WORD	cf22a;
	SWORD	cf22b;
	WORD	cf32a;
	SWORD	cf32b;
	SDWORD	f42;
	WORD	cf13a;
	SWORD	cf13b;
	WORD	cf23a;
	SWORD	cf23b;
	WORD	cf33a;
	SWORD	cf33b;
	SDWORD	f43;

	SWORD	nXRotation;
	SWORD	nYRotation;
	SWORD	nZRotation;
	SWORD	Cx,Cy;
	SDWORD	Cz;			// camera positions
} cdrfTransformExtrude;

//******************************************************************  Object types

#define cdrfLINKTYPE_EXTRUDE	0x8
#define cdrfLINKTYPE_CONTOUR	0xf
#define cdrfLINKTYPE_POWERLINE	0xd
#define cdrfLINKTYPE_BLEND		0x7

//******************************************************************  Object types

// version 3 numbers are one less than in other versions!
#define cdrfOBJTYPE_V3ADD		1

#define cdrfOBJTYPE_RECTANGLE	1
#define cdrfOBJTYPE_ELLIPSE		2
#define cdrfOBJTYPE_PATH		3
#define cdrfOBJTYPE_TEXT		4
#define cdrfOBJTYPE_BITMAP		5
#define cdrfOBJTYPE_TEXTSTORY	6

//******************************************************************  Colour definitions

#define cdrfCOLOURTYPE_PANTONE_SPOT     0x0001
#define cdrfCOLOURTYPE_CMYK             0x0002
#define cdrfCOLOURTYPE_CMYK255          0x0003
#define cdrfCOLOURTYPE_CMY              0x0004
#define cdrfCOLOURTYPE_RGB              0x0005
#define cdrfCOLOURTYPE_HSV              0x0006
#define cdrfCOLOURTYPE_GREYSCALE        0x0009
#define cdrfCOLOURTYPE_LAB              0x000C

#define cdrfCOLOURSUBTYPE_FOCOLTONE     0x0008
#define cdrfCOLOURSUBTYPE_PANTONE_PROC  0x0002
#define cdrfCOLOURSUBTYPE_TRUMATCH      0x0001

#define cdrfCOLOURTYPEV4_PANTONE_SPOT   0x0000
#define cdrfCOLOURTYPEV4_CMYK           0x0001

#define cdrfCOLOURTYPEV3_PANTONE		0x0000
#define cdrfCOLOURTYPEV3_CMYK			0x0001

typedef struct {
	BYTE Type;
	union {
		struct {
			BYTE C, M, Y, K;
		} CMYK;
		struct {
			WORD ColourNumber;
			WORD Tint;
		} Pantone;
	} Data;
} cdrfColourV3;

typedef struct {
	union {
		struct {
			WORD  type;
			WORD  subtype;
			WORD  unknown[2];
			union {
				struct {
			    	BYTE c, m, y, k;  /* range 0 - 100 */
				} cmyk;
				struct {
					BYTE b, g, r;     /* range 0 - 255 */
				} rgb;
				struct {
					WORD h;
					BYTE s, v;        /* range 0 - 255 */
				} hsv;
				struct {
					BYTE i;           /* range 0 - 255 */
				} greyscale;
				struct {
					WORD colournumber;
					WORD tint;        /* range 0 - 100 */
				} pantone_spot;
			} data;
		} v5;
		struct {
			WORD type;
			union {
				struct {
					WORD c, m, y, k;     /* range 0 - 100 */
				} cmyk;
				struct {
					WORD colournumber;
					WORD tint;
				} pantone_spot;
			} data;
		} v4;
		cdrfColourV3 v3;
	} data;
} cdrfColour;


//******************************************************************  Fills

typedef struct {
	DWORD FillReference;
	WORD  FillType;
	WORD  unknown;
} cdrfFillHeader;

#define cdrfFILLTYPE_NONE       0x0000
#define cdrfFILLTYPE_FLAT       0x0001
#define cdrfFILLTYPE_GRADUATED  0x0002
#define cdrfFILLTYPE_PATTERN	0x0007
#define cdrfFILLTYPE_VECTOR		0x000a
#define cdrfFILLTYPE_TEXTURE	0x000b

typedef struct {
	cdrfFillHeader Header;
	cdrfColour Colour;
	WORD unknown2[5];
} cdrfFillFlat;

typedef struct {
	cdrfFillHeader Header;
	WORD GradFillType;
	WORD unknown[5];
	SWORD Pad;						// percentage
	SWORD Angle;						// if applicable, * 10 of actual value */
	SWORD HorizontalCentreOffset;	// percentage */
	SWORD VerticalCentreOffset;
	WORD unknown2;
	WORD ColourType;
	WORD NColours;					// number of colours following */
} cdrfFillGraduated;

#define cdrfGRADFILLTYPE_LINEAR         1
#define cdrfGRADFILLTYPE_RADIAL         2
#define cdrfGRADFILLTYPE_CONICAL        3
#define cdrfGRADFILLTYPE_SQUARE         4

#define cdrfGRADFILLCOLOURTYPE_DIRECT                   0
#define cdrfGRADFILLCOLOURTYPE_RAINBOW_CLOCKWISE        1
#define cdrfGRADFILLCOLOURTYPE_RAINBOW_ANTICLOCKWISE    2
#define cdrfGRADFILLCOLOURTYPE_CUSTOM                   3

typedef struct {
	cdrfColour Colour;				// alignment problems
	WORD Percentage;				// percentage along line of fill this colour starts
} cdrfFillGradColour;

#define cdrfFILLTYPEV3_NONE		0x00
#define cdrfFILLTYPEV3_FLAT		0x01
#define cdrfFILLTYPEV3_LINEAR	0x02
#define cdrfFILLTYPEV3_RADIAL	0x04
#define cdrfFILLTYPEV3_PATTERN	0x07
#define cdrfFILLTYPEV3_VECTOR	0x0a

typedef struct {
	BYTE Type;
	union {
		struct {
			cdrfColourV3 Colour;
		} Flat;
		struct {
			SWORD Angle;
			cdrfColourV3 Colour1;
			cdrfColourV3 Colour2;
			BYTE unknown[7];
			SWORD Pad;
			SWORD HorizontalCentreOffset;
			SWORD VerticalCentreOffset;
		} Grad;
		struct {
			DWORD Reference;
			SWORD SizeX;
			SWORD SizeY;
			SWORD OffsetX;
			SWORD OffsetY;
			BYTE unknown[3];
			cdrfColourV3 Colour1;
			cdrfColourV3 Colour2;	
		} Pattern;
		struct {
			WORD Reference;
			SWORD SizeX;
			SWORD SizeY;
			SWORD OffsetX;
			SWORD OffsetY;
		} Vector;
	} Data;
} cdrfFillV3;

typedef struct {
	cdrfFillHeader Header;
	DWORD Reference;			// reference of the pattern
	WORD SizeX, SizeY;			// size of tile (CDR units)
	SWORD OffsetX, OffsetY;		// percent size of tile offsets
	SWORD TileOffsets;			// percent offset in tiles (sort of staggers them)
	WORD TileOffsetType;		// row or column
	cdrfColour Colour1;
	cdrfColour Colour2;
} cdrfFillPattern;

#define cdrfTILEOFFSETTYPE_ROW		0
#define cdrfTILEOFFSETTYPE_COLUMN	1

typedef struct {
	cdrfFillHeader Header;
	WORD VectorReference;		// reference of the vectors we need to fill the
								// thing with
								// names an parameters of the texture follow	
} cdrfFillTexture;

typedef struct {
	cdrfFillHeader Header;
	WORD VectorReference;		// reference to the nice (?) vector list
	SWORD SizeX;
	SWORD SizeY;				// size of tile
	SWORD OffsetX;
	SWORD OffsetY;				// offset of first tile
	SWORD TileOffsets;			// stagger type thing
} cdrfFillVector;

//******************************************************************  Lens

typedef struct {
	BYTE Type;
	BYTE unknown[4];
	BYTE SubType;
	WORD Percent;
} cdrfLensDefn;

#define cdrfLENSTYPE_TRANSPARENT	0x01

#define cdrfTRANSPARENTTYPE_MIX		0
#define cdrfTRANSPARENTTYPE_STAINED	1
#define cdrfTRANSPARENTTYPE_BLEACH	2
#define cdrfTRANSPARENTTYPE_CONTRAST	13
#define cdrfTRANSPARENTTYPE_SATURATE	16
#define cdrfTRANSPARENTTYPE_DARKEN		19
#define cdrfTRANSPARENTTYPE_LIGHTEN		22
#define cdrfTRANSPARENTTYPE_BRIGHTNESS	25
#define cdrfTRANSPARENTTYPE_LUMINOSITY	28

//******************************************************************  Outlines

typedef struct {
	DWORD OutlineReference;
	WORD  Flags;
	WORD  EndStyle;
	WORD  JoinStyle;
	WORD  LineThickness;			// in 1000ths of an inch
	WORD  NibStrech;				// percentage
	WORD  NibAngle;					// angle * 10
	cdrfColour Colour;
	WORD  unknown[5];
	WORD  NDashSegments;
	WORD  DashSegments[10];
	DWORD StartArrowReference;		// or zero for no arrow
	DWORD EndArrowReference;
} cdrfOutline;

#define cdrfOUTLINE_NO_ARROWHEAD        0

#define cdrfOUTLINE_DEFAULT_WIDTH (3*72)

#define cdrfOUTLINEFLAGS_NOOUTLINE      0x01
#define cdrfOUTLINEFLAGS_NODASH         0x02
#define cdrfOUTLINEFLAGS_BEHINDFILL     0x10
#define cdrfOUTLINEFLAGS_SCALEWITHOBJ   0x20

#define cdrfENDSTYLE_BUTT       0
#define cdrfENDSTYLE_ROUNDED    1
#define cdrfENDSTYLE_SQUARE     2

#define cdrfJOINSTYLE_SQUARE    0
#define cdrfJOINSTYLE_ROUNDED   1
#define cdrfJOINSTYLE_BEVEL     2

#define cdrfOUTLINEFLAGSV3_STROKED			0x01
#define cdrfOUTLINEFLAGSV3_SCALEWITHOBJECT	0x80

#define cdrfMAX_DASH_ELEMENTS	10

typedef struct {
	BYTE Flags;
	SWORD LineThickness;
	SWORD NibStrech;
	SWORD NibAngle;			// * 10
	cdrfColourV3 Colour;
	BYTE unknown[7];
	BYTE NDashSegments;		// zero for no dashes
	BYTE DashSegments[10];
	WORD JoinStyle;
	BYTE EndStyle;
	BYTE EndStyle2;			// seperate style for start and end? They always have the same value
	DWORD StartArrowReference;
	DWORD EndArrowReference;
} cdrfOutlineV3;

/*#define cdrfOBJFLAGSV3_NODASH	0x0800*/	// set in the objects flgs chunk if the dash pattern should be ignored

// sometimes in CDR3 files (eg snowbarn) you get outline attributes with > 0 dash segements,
// but the lines aren't dashed. There doesn't seem to be anything reliable marking this.

//******************************************************************  Arrowheads

typedef struct {
	DWORD ArrowheadReference;
	WORD Distance;				// offset in the x direction of the origin from the end of the path
	WORD NNodes;
	WORD CoordsOffset;			// - 6 for some reason
	WORD unknown;
	BYTE NodeTypes[4];			// types of node, as for paths...
} cdrfArrowhead;

	// coordinates are in WORDs, and 500 is the width of the line...
	// they're not necessairly word aligned, though...
#define cdrfARROWHEAD_COORDOFF_CORRECT	6

#define cdrfARROWHEAD_LINEWIDTH			500

//******************************************************************  Path objects

typedef struct {
	WORD NCoords;
	WORD unknown2;
} cdrfPathCoordHeader;		// followed by coordinate pairs then coord types

#define cdrfPATHCOORDTYPE_MASK				0xc0

#define cdrfPATHCOORDTYPE_MOVE				0x00
#define cdrfPATHCOORDTYPE_LINETO			0x40
#define cdrfPATHCOORDTYPE_CURVE				0x80
#define cdrfPATHCOORDTYPE_CONTROL			0xc0

#define cdrfPATHCOORDATTR_SMOOTH			0x10
#define cdrfPATHCOORDATTR_CLOSE				0x08

//******************************************************************  Rectangles

typedef struct {
	SWORD Width;
	SWORD Height;	// both can be negative, in which case rectangle is below or left of (0,0)
	SWORD CornerRadius;
} cdrfRectangleData;

//******************************************************************  Ellipse

typedef struct {
	SWORD Width;
	SWORD Height;			// can be negative...
} cdrfEllipseData;

//******************************************************************  Bitmap objects

typedef struct {
	SWORD SizeX;
	SWORD SizeY;			// size on the page (CDR coordinates)
	WORD unknown[2];
	WORD PixelsX;
	WORD PixelsY;			// size in pixels
	WORD BitmapReference;	// reference of bitmap
	WORD unknown2[10];
	SWORD Left;
	SWORD Bottom;
	SWORD Right;
	SWORD Top;				// coords of the cropped edges
} cdrfBitmapData;
							// the top left of the bitmap is at (0,0) untransformed

typedef struct {
	SWORD SizeX;
	SWORD SizeY;			// size on the page (CDR coordinates)
	WORD unknown[2];
	WORD PixelsX;
	WORD PixelsY;			// size in pixels
	WORD unknown2;
	WORD BitmapReference;	// the bitmap reference
	WORD unknown3[6];
	SWORD Left;
	SWORD Bottom;
	SWORD Right;
	SWORD Top;				// coords of the cropped edges
} cdrfBitmapData3;

//******************************************************************  Bitmaps

// different format bitmaps for patterns (2 colour) and other bitmap type
// fills and bitmaps!

typedef struct {
	DWORD Reference;
	DWORD DataOffset;		// offset to palette entrys then image data
	DWORD SizeX;
	DWORD SizeY;
	WORD unknown[4];
	DWORD DataSize;
} cdrfPatternBitmapHeader;

typedef struct {
	WORD Reference;
	WORD unknown[5];
	WORD ImageOffset;		// roughly the offset... correct it
	WORD unknown2;
	WORD ImageType;
	WORD unknown3[3];
	DWORD SizeX;
	DWORD SizeY;
	WORD unknown4[2];
	DWORD Depth;
	WORD unknown5[21];
	WORD NPaletteEntries;
} cdrfBitmapHeader;
	// palette entries follow (3 byte RGB)

#define cdrfBitmapHeaderImageOffsetCorrect	82		// add this to ImageOffset

#define cdrfBitmapGreyscaleImageStart		0x50

#define cdrfBITMAPTYPE_COLOUR		0x0008
#define cdrfBITMAPTYPE_GREYSCALE	0x0005

typedef struct {
	BYTE Blue, Green, Red;
} cdrfBitmapPaletteEntry;

typedef struct {
	WORD Reference;
	WORD unknown[5];
	WORD ImageOffset;		// roughly the offset... correct it
	WORD unknown2[3];
	DWORD SizeX;
	DWORD SizeY;
	WORD unknown4[1];
	WORD Depth;
	WORD unknown5[8];
	WORD NPaletteEntries;
	WORD unknown6[3];
} cdrfBitmapHeader4;
	// palette entries follow (3 byte RGB)

typedef struct {
	BYTE Blue, Green, Red, unused;
} cdrfBitmapPaletteEntry4;

//******************************************************************  Styles

typedef struct {
	WORD Reference;
} cdrfStyleReference;

typedef struct {
	WORD Reference;
	cdrfOffsetHeader Header;
} cdrfStyle;

#define cdrfSTYLEOFFSETTYPE_FILL	0xcd
#define cdrfSTYLEOFFSETTYPE_OUTLINE	0xd2

//******************************************************************  Text

typedef struct {
	WORD unknown[8];
	WORD Position;
	WORD unknown2[2];
	WORD Alignment;
} cdrfTextOnPathInfo;

#define cdrfTEXTONPATHPOS_BOTTOM	1
#define cdrfTEXTONPATHPOS_TOP		2
#define cdrfTEXTONPATHPOS_CENTRE	3
#define cdrfTEXTONPATHPOS_BASELINE	4

#define cdrfTEXTONPATHALIGN_LEFT	1
#define cdrfTEXTONPATHALIGN_RIGHT	2
#define cdrfTEXTONPATHALIGN_CENTRE	3

//******************************************************************  Text (version 5)

// what you get in a txsm chunk:
// cdrfTextInfoHdr
// then paragraphs... * Hdr.NParagraphs
// 		cdrfTextInfoParaHdr
//		cdrfTextInfoFontDefn * ParaHdr.FontDefns
//		cdrfTextInfoParaInfo
//		cdrfTextInfoChar * ParaInfo.NChars

typedef struct {
	WORD Type;
	WORD unknown[3];
	WORD NParagraphs;
} cdrfTextInfoHdr;

typedef struct {
	WORD Style;				// style for this para
	WORD NFontDefns;		// number of font definitions following
} cdrfTextInfoParaHdr;

typedef struct {
	WORD Changes;			// what things need to change
	WORD FontRef;
	WORD FontType;
	WORD unknown2;
	struct {
		WORD Underline	: 3;	// use cdrfLINETYPE_* for these three
		WORD Overline	: 3;
		WORD StrikeOut	: 3;
		WORD Placement	: 3;	// super, sub, or what
	} FontAttr;
	WORD FontSize;			// 1000ths inch
	WORD Angle;				// angle of characters * 10
	WORD unknown3[4];
	WORD HorizontalShift;
	WORD VerticalShift;		// percentages
	WORD unknown4[5];
} cdrfTextInfoFontDefn;

#define cdrfLINETYPE_NONE				0
#define cdrfLINETYPE_SINGLE_THIN		1
#define cdrfLINETYPE_SINGLE_THIN_WORD	2
#define cdrfLINETYPE_SINGLE_THICK		3
#define cdrfLINETYPE_SINGLE_THICK_WORD	4
#define cdrfLINETYPE_DOUBLE_THIN		5
#define cdrfLINETYPE_DOUBLE_THIN_WORD	6

#define cdrfPLACEMENTTYPE_NONE			0
#define cdrfPLACEMENTTYPE_SUPERSCRIPT	1
#define cdrfPLACEMENTTYPE_SUBSCRIPT		2

#define cdrfTEXTINFODEFNCHANGES_FONT	0x0001
#define cdrfTEXTINFODEFNCHANGES_SIZE	0x0004
#define cdrfTEXTINFODEFNCHANGES_ATTR	0x0002

typedef struct {
	WORD NChars;			// number of characters following
} cdrfTextInfoParaInfo;

typedef struct {
	SWORD XSize;
	WORD unknown;
	WORD Code;
	WORD Info;
} cdrfTextInfoChar;

#define cdrfTEXTINFOCHAR_INFO_DEFNMASK		0x0ff0
#define cdrfTEXTINFOCHAR_INFO_DEFNMASKSBY	4		// number of bits to shift right

typedef struct {
	WORD unknown;
	WORD XSize;
	WORD YSize;
	// lots of other stuff
} cdrfTextStoryCoordData;

#define cdrfTEXTSTYLE_OFFSETTYPE_FONT		0xdc
#define cdrfTEXTSTYLE_OFFSETTYPE_SPACING	0xeb
#define cdrfTEXTSTYLE_OFFSETTYPE_ALIGNMENT	0xe1

typedef struct {
	WORD Reference;
	cdrfOffsetHeader Hdr;
} cdrfTextStyleHdr;

typedef struct {
	WORD FontRef;
	WORD FontSize;				// 1000ths inch
	WORD FontType;
} cdrfTextStyleFont;

typedef struct {
	SWORD CharacterSpacing;		// percentage
	SWORD WordSpacing;			// percentage
	SWORD LineSpacing;			// percentage
	WORD unknown;
	WORD SpacingSpecified;		// 0 = % char, 1 = pt, 2 = % pt (doesn't affect value)
} cdrfTextStyleSpacing;

typedef struct {
	WORD Alignment;
} cdrfTextStyleAlignment;

#define cdrfALIGNMENT_LEFT		1
#define cdrfALIGNMENT_CENTRE	2
#define cdrfALIGNMENT_RIGHT		3
#define cdrfALIGNMENT_FULL		4

typedef struct {
	WORD Reference;
	WORD unknown[8];
	TCHAR Name[4];				// zero terminated, as long as it takes
} cdrfFontDefn;

#define cdrfFONTTYPE_NORMAL		0x0040
#define cdrfFONTTYPE_ITALIC		0x0080
#define cdrfFONTTYPE_BOLD		0x1000
#define cdrfFONTTYPE_BOLDITALIC	0x2000

#define cdrfTEXT_NEWLINE		13


//******************************************************************  Link table

// link table - used for other things than just text

typedef struct {
	WORD Size;
	WORD Entries;		// number of entries
	WORD OffsetsOffset;	// offset to offsets
} cdrfLinkTableHdr;

	// offsets are WORDs. Choose the closest above - an accurate offset isn't stored.

#define cdrfLINKTABLEENTRYV4_TEXT	0x0061
#define cdrfLINKTABLEENTRYV4_BLEND	0x0010

#define cdrfLINKTABLEENTRY_TEXTONPATH	0x0040

typedef struct {
	WORD Type;
	WORD Unknown;
	WORD TextSerialNumber;
	WORD PathSerialNumber;
} cdrfLinkTableEntryTextOnPath;

// version for CDR3 files...

#define cdrfLINKTABLEENTRYV3_TEXTONPATH	0x8a68

typedef struct {
	WORD TextSerialNumber;
	WORD Type;
	WORD PathSerialNumber;
	WORD TextSerialNumber2;			// seems to be a copy
} cdrfLinkTableEntryTextOnPathV3;

//******************************************************************  Text (version 4)

typedef struct {
	WORD Type;
	WORD unknown[6];
	WORD TextID;		// the ID of the text
	WORD ObjectSerial;	// the serial number of the object we're linked to
} cdrfLinkTableEntryTextV4;

typedef struct {
	WORD Reference;
	TCHAR Name[4];				// zero terminated, as long as it takes
} cdrfFontDefnV4;

// in coords bit for paragraph text...

typedef struct {
	WORD unknown;
	WORD FrameX;
	WORD FrameY;
} cdrfParaTextHeaderV4;


// in the coords bit of the object chunk...

typedef struct {
	WORD unknown;
	WORD NCharacters;
	BYTE unknown2;
} cdrfTextHeaderV4;

// followed by text characters

typedef struct {
	BYTE unknown;
	BYTE Info;
	BYTE Code;
} cdrfTextCharV4;

typedef struct {
	BYTE unknown;
	BYTE Info;
	BYTE Code;
	BYTE unknown2;
	WORD Changes;			// what's changed...
	WORD FontRef;
	WORD FontSize;
	WORD FontType;
	BYTE unknown3[13];
} cdrfTextCharStyledV4;

#define cdrfSTYLECHANGEV4_FONTNAME	0x0008
#define cdrfSTYLECHANGEV4_FONTSIZE	0x0010
#define cdrfSTYLECHANGEV4_WEIGHT	0x0020

#define cdrfFONTTYPEV4_NORMAL		0x1
#define cdrfFONTTYPEV4_ITALIC		0x4
#define cdrfFONTTYPEV4_BOLD			0x2
#define cdrfFONTTYPEV4_BOLDITALIC	0x8

// in the btxt list...
typedef struct {
	WORD Info;
	WORD Char;
} cdrfTextListCharV4;

typedef struct {
	WORD Info;
	WORD Char;
	WORD FontRef;
	WORD FontSize;
	struct {
		WORD Weight		: 4;
		WORD Placement	: 4;
	};
	WORD unknown[7];
} cdrfTextListCharStyledV4;

#define cdrfPLACEMENTV4_NONE	0x0
#define cdrfPLACEMENTV4_SUPER	0x1
#define cdrfPLACEMENTV4_SUB		0x2

typedef struct {
	WORD Style;
	WORD unknown[2];
} cdrfParaDefnV4;

//******************************************************************  Text (version 3)

// entries in the 'ftbl' chunk

typedef struct {
	WORD Reference;
	BYTE Name[34];		// zero terminated, all lower case
} cdrfFontTableEntryV3;

// the 'tspc' chunk - gives font style info

typedef struct {
	WORD Size;
	WORD NOffsets;
	WORD unknown;
	WORD Offsets[4];	// as many as there are,,,
} cdrfFontStyleTableHeaderV3;

typedef struct {
	WORD unknown;
	WORD FontRef;
	WORD FontSize;
	WORD FontStyle;
	WORD unknown2[2];
} cdrfFontStyleTableEntryV3;

#define cdrfFONTSTYLEV3_WEIGHT_MASK			0x0f
#define cdrfFONTSTYLEV3_WEIGHT_NORMAL		0x01
#define cdrfFONTSTYLEV3_WEIGHT_BOLD			0x02
#define cdrfFONTSTYLEV3_WEIGHT_ITALIC		0x04
#define cdrfFONTSTYLEV3_WEIGHT_BOLDITALIC	0x08
#define cdrfFONTSTYLEV3_FLAG_SUPERSCRIPT	0x10
#define cdrfFONTSTYLEV3_FLAG_SUBSCRIPT		0x20

// data in an object

typedef struct {
	BYTE unknown[3];
	WORD FrameX;
	WORD FrameY;
	BYTE unknown2[4];
	WORD NCharacters;
	WORD unknown3;
	WORD BaseFontStyle;
	WORD unknown4[4];
	BYTE Justification;
	BYTE unknown5[105];
} cdrfTextHeaderV3;

#define cdrfJUSTIFICATIONV3_LEFT	1
#define cdrfJUSTIFICATIONV3_CENTRE	2
#define cdrfJUSTIFICATIONV3_RIGHT	3

typedef struct {
	BYTE unknown;
	BYTE Info;			// non zero if this has a style applied
	BYTE Code;
} cdrfTextCharV3;

typedef struct {
	BYTE unknown;
	BYTE Info;
	BYTE Code;
	WORD unknown2;
	WORD Style;
	BYTE unknown3[9];
} cdrfTextCharStyledV3;

//******************************************************************  End


// This must be the last thing in the file to avoid things going nasty on us
#pragma pack(pop)

#endif  // INC_CDRFORM


