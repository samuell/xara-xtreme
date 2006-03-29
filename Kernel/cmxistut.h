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
// data reading definitions for CMX import

#ifndef INC_CMXISTUT
#define INC_CMXISTUT

#include "cmxform.h"
#include "cmxibits.h"

//-0-0-----------------------------------------------------------------------------------

// naming conventions:
//    cmxi<Name>			-- structure
//	  cmxi<Name>Desc		-- instructions for loading this structure
//
// but remember, boys and girls, this should be done for you if you use my
// lovely CMX structure defining stuff. In fact, you probably won't even
// notice them as the CI_READDATA hides this from you as well.

//-0-0-----------------------------------------------------------------------------------

// first of all, I'll define the instruction codes I'm going to use

// data types
//---------	name -------------- code ----- target data type
#define		CI_READBYTE			1		// INT32
#define		CI_READINT16		2		// INT32
#define		CI_READINT32		3		// INT32
#define		CI_READUINT16		4		// UINT32
#define		CI_READUINT32		5		// UINT32
#define		CI_READREFERENCE	6		// INT32
#define		CI_READLENGTH		7		// INT32
#define		CI_READCOORD		8		// DocCoord
#define		CI_READBBOX			9		// DocRect
#define		CI_READNATURALWORD	10		// UINT32 (reads 16/32 bit word)
#define		CI_READOFFSET		11		// UINT32 (file offset)
#define		CI_READBOOLEAN		12		// BOOL
#define		CI_READANGLE		13		// double (angle in radians)
#define		CI_READDOUBLE		14		// double
#define		CI_READRENDATTR		15		// cmxiRenderAttr (rendering attributes)
#define		CI_READPOINTLIST	16		// Path * (remember to delete it)
#define		CI_READCOLOURLIST	17		// CMXImportColourList * (remember to delete it)
#define		CI_READSTRING		18		// StringBase * (remember to delete it)
#define		CI_READMATRIX		19		// Matrix
#define		CI_READSTRINGF_VAL	20		// char[n]
#define		CI_READTILING		21		// cmxiTiling (tiling description)
#define		CI_READINT16ARRAY	22		// cmxiIntArray (delete array afterwards with delete [])
#define		CI_READCHARLIST		23		// characterList for 16bit (remember to delete the WCHAR *)
#define		CI_READCOLUMN		24		// the column specification (something to delete there!)
#define		CI_TRANSPARENCY		25		//	In case the new transparency is used we need to stop 
										//	and tell why.


// structural bits
//--------- name -------------- code ----- what it does
#define		CI_STOP				0		// stop everything
#define		CI_SKIPIN_VAL		64		// skip n bytes from input
#define		CI_SKIPOUT_VAL		65		// skip n bytes in output structure
#define		CI_STORENEXT		66		// stores next value for use in if's
#define		CI_IFSTOREDIS_VAL	67		// do next bits if stored has certain value
#define		CI_ELSEIF_VAL		68		// else statement
#define		CI_ENDIF			69		// end if bit
#define		CI_DO16				70		// do next bits for 16 bit only
#define		CI_DO32				71		// do next bits for 32 bit only
#define		CI_DOALL			72		// do next bits for everything (default)
#define		CI_TAG_VAL			73		// expect a tag
#define		CI_ENDTAG			74		// end of tag -- fix up file location
#define		CI_MINENDTAG		75		// expect one of those end of tags codes
										// use where you see <Tag_EndTag> in CMX documentation
#define		CI_SETOUTMARKER		76		// set an output marker
#define		CI_GOTOOUTLOC_VAL	77		// goto a set distance after the output marker in the output structure
#define		CI_SKIPINTIMES_VAL	78		// skip input data depending on stored value
#define		CI_SETINMARKER		79		// sets a marker for the input file location
#define		CI_ENSUREINLOC		80		// ensures that the file location is in the correct position
										// SizeInFile value currently stored
#define		CI_TAG_IF_THERE		81		//	Trick to cope with transparency

// macros for things with parameters
#define		CI_SKIPIN(type)			(CI_SKIPIN_VAL), sizeof(type)
#define		CI_SKIPOUT(type)		(CI_SKIPOUT_VAL), sizeof(type)
#define		CI_SKIPIN_BYTES(n)		(CI_SKIPIN_VAL), (n)
#define		CI_SKIPOUT_BYTES(n)		(CI_SKIPOUT_VAL), (n)
#define		CI_IFSTOREDIS(type, n)	(CI_IFSTOREDIS_VAL), (type), (n)
#define		CI_ELSEIF(type, n)		(CI_ELSEIF_VAL), (type), (n)
#define		CI_TAG(n)				(CI_TAG_VAL), (n)
#define		CI_GOTOOUTLOC(type)		(CI_GOTOOUTLOC_VAL), sizeof(type)
#define		CI_SKIPINTIMES(type)	(CI_SKIPINTIMES_VAL), sizeof(type)
#define		CI_SKIPINTIMES_BYTES(n)	(CI_SKIPINTIMES_VAL), (n)
#define		CI_READSTRINGF(n)		(CI_READSTRINGF_VAL), (n)
#define		CI_SKIPINTIMES_BYTE(n)	(CI_SKIPINTIMES_VAL), (n)

#define		CI_IF_TAG(n)			(CI_TAG_IF_THERE), (n)

// parameters
#define		CI_IFTYPE_VALUE		0		// if value is equal
#define		CI_IFTYPE_BITSET	1		// if bit is set
#define		CI_IFTYPE_BOOL		2		// for handling Corel's BOOLs (use TRUE/FALSE in the statements)

#ifdef _DEBUG
#define		CI_TRACE_VAL	101
#define		CI_TRACE(n)		(CI_TRACE_VAL), (n),
#else
#define		CI_TRACE(n)		/**/
#endif


// notes about if's in the 'language' 'defined' above:
//   if's can be nested
//   each level of if's has it's own stored value so you can do nested if's
//   without worrying about corrupting the stored value of the if above you
//   output markers are stacked as well

// a thing so we know some information about our stuff
class CMXIinfo
{
public:
	static void SetUpInfo(void);

	static char CommandLengths[256];

protected:
	static void SetLengths(char *instrs, INT32 num, INT32 val);
};

// handy thing to read structures from the file
#define CI_READDATA(type, name)		cmxi##type name; if(!ReadData(&name, cmxi##type##Desc)) return FALSE;

typedef struct {
	INT32 NumElements;
	INT32 *pElements;
} cmxiIntArray;


#pragma pack(push, 1)

//-0-0-----------------------------------------------------------------------------------

// now for some macros to help setting things up
#define START_CMXSTRUCT		typedef struct {
#ifndef COMPILING_CMXISTUT_CPP
#define END_CMXSTRUCT(name)	} cmxi##name; extern char cmxi##name##Desc[];
#else
#define END_CMXSTRUCT(name)	} cmxi##name;
#endif

#define START_CMXDESC(name)	char cmxi##name##Desc[] = {
#define END_CMXDESC			,CI_STOP};

//-0-0-----------------------------------------------------------------------------------

typedef struct {
	INT32	count;
	WCHAR*	characters;
	WORD*	indices;
} characterList;

typedef struct {
	INT32	count;
	INT32*	columns;
	INT32*	gutters;
} cmxiColumnList;


// structure to allow us to bung DocCoords in unions
typedef struct {
	INT32 x,y;
} fudgedDocCoord;

typedef struct {
	INT32 x0,y0,x1,y1;
} fudgedDocRect;

// definitions of structures we will encounter in the file. Remember that
// each has an associted description in cmxistut.cpp which you'll have to
// update as well as these

START_CMXSTRUCT
	char ID[32];
	INT32 ByteOrder;
	INT32 CoordSize;
	INT32 Major;
	INT32 Minor;
	INT32 Unit;
	double Factor;
	INT32 IOption, IForeignKey, ICapability, IndexSection, InfoSection, Thumbnail;
	struct {
		INT32 x0, y1, x1, y0;
	} bbox;
	INT32 Tally;
END_CMXSTRUCT(FileHeader)

START_CMXSTRUCT
	INT32 MasterID;
	INT32 Size;
	INT32 RecordCount;
END_CMXSTRUCT(MasterIndexHeader)


START_CMXSTRUCT
	INT32 IndexRecordID;
	INT32 Offset;
END_CMXSTRUCT(MasterIndexEntry)

START_CMXSTRUCT
	INT32 RecordCount;
END_CMXSTRUCT(SimpleIndexHeader)

START_CMXSTRUCT
	INT32 SizeInFile;
	INT32 PageOffset;
	INT32 LayerTableOffset;
	INT32 ThumbnailOffset;
	INT32 RefListOffset;
	// unknown extra bonus data
END_CMXSTRUCT(PageIndexEntry)

START_CMXSTRUCT
	INT32 Size;
	INT32 Code;
END_CMXSTRUCT(CommandHeader)

START_CMXSTRUCT
	INT32 PageNumber;
	UINT32 PageFlags;
	DocRect BBox;
	INT32 EndPageOffset;
	INT32 GroupCount;
	INT32 InstructionCount;
	Matrix PageMatrix;		// just a concantation of matrices so far -- probably just ignore it
END_CMXSTRUCT(BeginPage)

START_CMXSTRUCT
	INT32 PageNumber;
	INT32 LayerNumber;
	INT32 LayerFlags;
	INT32 InstructionCount;
	StringBase *pLayerName;
	Matrix LayerMatrix;
END_CMXSTRUCT(BeginLayer)

START_CMXSTRUCT
	INT32 ColourModel;
	INT32 PaletteType;
	union {
		struct {
			INT32 ID, Density;
		} Pantone;
		struct {
			INT32 Cyan, Magenta, Yellow, Key;
		} CMYK;
		struct {
			INT32 Red, Green, Blue;
		} RGB;
		struct {
			INT32 Hue, Saturation, Value;
		} HSV;
		struct {
			INT32 Grey;
		} Grey;
		struct {
			INT32 Value1, Value2, Value3, Value4;
		} Misc;
	} Spec;
END_CMXSTRUCT(Colour)

START_CMXSTRUCT
	INT32 Width;
	INT32 Height;
	INT32 XOffset;
	INT32 YOffset;
	INT32 InterTileOffset;
	INT32 TilingFlags;
END_CMXSTRUCT(Tiling)

typedef struct {
	INT32 FillID;
	union {
		struct {
			INT32 ColourReference;
			INT32 ScreenReference;
		} Uniform;
		struct {
			INT32 Type;
			INT32 Screen;
			INT32 Padding;
			double Angle;
			fudgedDocCoord Offset;
			INT32 StepCount;
			INT32 FillMode;
			INT32 RateMethod;
			INT32 RateValue;
			CMXImportColourList *pColours;
		} Fountain;
		struct {
			INT32 Reference;
			INT32 ParamCount;
			StringBase *UserFunction;
		} Postscript;
		struct {
			INT32 VectorFillReference;
			cmxiTiling Tile;
			fudgedDocRect BBox;
		} Bitmap;		// and vector fills
		struct {
			INT32 BitmapReference;
			cmxiTiling Tile;
			INT32 ForeColourReference;
			INT32 BackColourReference;
			INT32 ScreenReference;
		} TwoColourBitmap;
		struct {
			INT32 VectorFillReference;
			cmxiTiling Tile;
			fudgedDocRect BBox;
			INT32 StringLen1, StringLen2, StringLen3, ParamCount;
		} Texture;
	} Spec;
} cmxiFillSpec;

typedef union {
	struct {
		INT32 TintMethod;
		INT32 UniformRate;
		INT32 ColourRef;
		INT32 RangeProcRef;
	} Glass;
	struct {
		INT32 UniformRate;
		INT32 RangeProcRef;
	} Magnify;
	struct {
		INT32 UniformRate;
		INT32 RangeProcRef;
	} Fisheye;
	struct {
		INT32 OutlineMethod;
		INT32 OutlineColour;
		INT32 FillMethod;
		INT32 FillColour;
		INT32 RangeProcRef;
	} Wireframe;
} cmxiLensSpecSpec;

typedef struct {
	INT32 Type;
	union {
		struct {
			INT32 TintMethod;
			INT32 UniformRate;
			INT32 ColourRef;
			INT32 RangeProcRef;
		} Glass;
		struct {
			INT32 UniformRate;
			INT32 RangeProcRef;
		} Magnify;
		struct {
			INT32 UniformRate;
			INT32 RangeProcRef;
		} Fisheye;
		struct {
			INT32 OutlineMethod;
			INT32 OutlineColour;
			INT32 FillMethod;
			INT32 FillColour;
			INT32 RangeProcRef;
		} Wireframe;
	};
} cmxiLensSpec;

typedef struct {
	INT32 ContainerReference;
	BOOL AutoTransform;
} cmxiContainerSpec;

START_CMXSTRUCT
	INT32 Mask;
	cmxiFillSpec FillSpec;
	INT32 OutlineReference;
	cmxiLensSpec LensSpec;
	cmxiContainerSpec ContainerSpec;
END_CMXSTRUCT(RenderAttr)

START_CMXSTRUCT
	cmxiRenderAttr RenderAttributes;
	Path *PointList;
	DocRect BBox;
END_CMXSTRUCT(PolyCurve)

START_CMXSTRUCT							// Graeme (3-2-00)
	cmxiRenderAttr RenderAttributes;
	DocCoord Centre;
	INT32 Width;
	INT32 Height;
	double CornerRadius;
	double Rotation;
	DocRect BBox;
END_CMXSTRUCT ( Rectangle )

START_CMXSTRUCT
	Matrix TransformMatrix;
END_CMXSTRUCT(AddGlobalTransform)

START_CMXSTRUCT
	Matrix TransformMatrix;
END_CMXSTRUCT(SetGlobalTransform)

START_CMXSTRUCT
	INT32 LineStyleRef;
	INT32 ScreenRef;
	INT32 ColourRef;
	INT32 ArrowheadsRef;
	INT32 PenRef;
	INT32 DotDashRef;
END_CMXSTRUCT(Outline)

START_CMXSTRUCT
	INT32 Width;
	INT32 Aspect;
	double Angle;
	Matrix Mat;
END_CMXSTRUCT(Pen)

START_CMXSTRUCT
	INT32 Spec;
	INT32 CapAndJoin;
END_CMXSTRUCT(LineStyle)

START_CMXSTRUCT
	cmxiIntArray Dots;
END_CMXSTRUCT(DotDash)

START_CMXSTRUCT
	cmxiRenderAttr RenderAttributes;
	DocRect ImageExtent;
	DocRect CroppingRectangle;
	Matrix Transformation;
	INT32 ImageType;
	INT32 ImageFileReference1;
	INT32 ImageFileReference2;
END_CMXSTRUCT(DrawImage)

START_CMXSTRUCT
	INT32 SizeInFile;
	INT32 Offset;
	INT32 Type;
	// unknown extra bonus data
END_CMXSTRUCT(EmbeddedFileIndexEntry)

START_CMXSTRUCT
	INT32 SizeInFile;
	INT32 ReferenceList;
	INT32 Position;
	// unknown extra bonus data
END_CMXSTRUCT(ProcedureIndexEntry)

START_CMXSTRUCT
	INT32 Offset;
END_CMXSTRUCT(JumpAbsolute)

START_CMXSTRUCT
	INT32 Type;
	INT32 Compression;
	INT32 ISize;
	INT32 ICompressedSize;
END_CMXSTRUCT(ImageInfo)

START_CMXSTRUCT
	UINT32 Type;
	UINT32 Size;
	INT32 Reserved1, Reserved2;
	INT32 OffsetToBits;
END_CMXSTRUCT(ImageContents)

START_CMXSTRUCT
	UINT32 Type;
	UINT32 Size;
	INT32 Reserved1, Reserved2;
	INT32 OffsetToBits;
END_CMXSTRUCT(ImageContentsDIB)

START_CMXSTRUCT
	UINT32 Type;
	UINT32 CompressionType;
	UINT32 Width;
	UINT32 Height;
	UINT32 Planes;
	UINT32 BitsPerPlane;
	UINT32 BytesPerPlane;
	UINT32 BufferSize;
	UINT32 CompressedBufferSize;
	UINT32 XPelsPerKM;
	UINT32 YPelsPerKM;
	INT32 IPaletteOffset;
	INT32 IDataOffset;
	INT32 IStartBufOffset;
	INT32 IAlphaOffset;
	INT32 IMaskOffset;
END_CMXSTRUCT(RImageHeader)

START_CMXSTRUCT
	INT32 Type;
	INT32 NumEntries;
END_CMXSTRUCT(RImagePalette)

START_CMXSTRUCT
	INT32 Blue, Green, Red;
END_CMXSTRUCT(RImagePaletteEntry)

typedef union {
	struct {
		INT32 x0, x1;
	} Vertical;
	struct {
		INT32 y0, y1;
	} Horizontal;
	fudgedDocRect Rectangular;
	Path *pPolygon;
} cmxiRegionSpec;

START_CMXSTRUCT
	INT32 RegionType;
	cmxiRegionSpec Spec;
	INT32 ClipMode;
	INT32 RecoverySpecification;
END_CMXSTRUCT(AddClippingRegion)

START_CMXSTRUCT
	INT32 RecordCount;
	INT32 SizeInFile;
	INT32 Type;
END_CMXSTRUCT(ArrowShapeIndexHeader)

START_CMXSTRUCT
	INT32 Offset;
END_CMXSTRUCT(ArrowShapeIndexEntry)

START_CMXSTRUCT
	Path *pShape;
	INT32 LineOffset;
END_CMXSTRUCT(ArrowShape)

START_CMXSTRUCT
	INT32 StartRef;
	INT32 EndRef;
END_CMXSTRUCT(Arrowheads)

START_CMXSTRUCT
	INT32 RecordCount;
	INT32 SizeInFile;
	INT32 Type;
END_CMXSTRUCT(BitmapIndexHeader)

START_CMXSTRUCT
	INT32 Offset;
END_CMXSTRUCT(BitmapIndexEntry)

START_CMXSTRUCT
	INT32 RecordCount;
	INT32 SizeInFile;
	INT32 Type;
END_CMXSTRUCT(FontIndexHeader)

START_CMXSTRUCT
	INT32 Offset;
END_CMXSTRUCT(FontIndexEntry)


START_CMXSTRUCT
	BOOL	Artistic;
	INT32	Pad;
	DocRect	BBox;
	INT32	Frames;
	INT32	Num_Paragraph;
	INT32	Num_Instruction;
END_CMXSTRUCT(BeginTextStream)

START_CMXSTRUCT
	cmxiColumnList	columns;
	INT32		Height;
	INT32		Skew;
	Matrix		Mat;
END_CMXSTRUCT(TextFrame)

START_CMXSTRUCT
	cmxiRenderAttr	RenderAttributes;
	INT32			FontRef;
	INT32			FontSize;
	INT32			KerningVertical;
	INT32			KerningHorizontal;
	double			KerningAngle;
	INT32			Justification;
	INT32			InterCharSpacing;
	INT32			InterWordSpacing;
	INT32			InterLineSpacing;
	INT32			BeforeParagraphSpacing;
	INT32			AfterParagraphSpacing;
	INT32			MaxInterChar;
	INT32			MinInterWord;
	INT32			MaxInterWord;
	INT32			SpacingMode;
	INT32			UnderlineFlag;
	INT32			UnderlineDistanceFromBaseline;
	INT32			UnderlineStrokeThickness;
	INT32			ThickUnderlineFlag;
	INT32			ThickUnderlineDistanceFromBaseline;
	INT32			ThickUnderlineStrokeThickness;
	INT32			DoubleUnderlineFlag;
	INT32			DoubleUnderlineDistanceFromBaseline;
	INT32			DoubleUnderlineStrokeThickness;
	INT32			DoubleUnderline1Flag;
	INT32			DoubleUnderline1DistanceFromBaseline;
	INT32			DoubleUnderline1StrokeThickness;
	INT32			DoubleUnderline2Flag;
	INT32			DoubleUnderline2DistanceFromBaseline;
	INT32			DoubleUnderline2StrokeThickness;
	INT32			ThickOverlineFlag;
	INT32			ThickOverlineDistanceFromBaseline;
	INT32			ThickOverlineStrokeThickness;
	INT32			ThickOverline1Flag;
	INT32			ThickOverline1DistanceFromBaseline;
	INT32			ThickOverline1StrokeThickness;
	INT32			ThickOverline2Flag;
	INT32			ThickOverline2DistanceFromBaseline;
	INT32			ThickOverline2StrokeThickness;
	INT32			ThickStrikeoutFlag;
	INT32			ThickStrikeoutDistanceFromBaseline;
	INT32			ThickStrikeoutStrokeThickness;
	INT32			DoubleStrikeout1Flag;
	INT32			DoubleStrikeout1DistanceFromBaseline;
	INT32			DoubleStrikeout1StrokeThickness;
	INT32			DoubleStrikeout2Flag;
	INT32			DoubleStrikeout2DistanceFromBaseline;
	INT32			DoubleStrikeout2StrokeThickness;
	INT32			TabCount;
	BOOL			BulletOn;
	INT32			BulletFlagMulticolour;
	INT32			BulletDefaultFont;			
	INT32			BulletFontSize;
	INT32			BulletCharacter;			
	INT32			BulletVerticalShift;
	INT32			BulletHorizontalShift;
	double			BulletKerningAngle;
	cmxiRenderAttr	BulletRenderAttributes;
	INT32			LeftMargin;	
	INT32			RightMargin;
	INT32			FirstLine;
	INT32			RemainingLines;
	BOOL			AutoHyphenation;
	BOOL			BreakCapitalisedWords;
	INT32			MinWordLength;			
	INT32			MinCharsBefore;
	INT32			MinCharsAfter;
	INT32			HotZone;
	BOOL			DropCapOn;
	//INT32			DropCapDefaultFont;			
	//INT32			DropCapFontSize;
	//INT32			DropCapVerticalShift;
	//INT32			DropCapHorizontalShift;	
	//double			DropCapKerningAngle;			
END_CMXSTRUCT(BeginParagraph)


START_CMXSTRUCT
	characterList	ch;
END_CMXSTRUCT(Characters)

START_CMXSTRUCT
	INT32			UsageMask;
	INT32			FontRef;
	INT32			TypeSize;
	double			CharAngle;
	cmxiRenderAttr	RenderAttributes;	
	INT32			HShift;
	INT32			VShift;
END_CMXSTRUCT(CharInfo)

START_CMXSTRUCT
	INT32	count;
	INT32	SizeInFile;
	INT32	Type;
	INT32	*Offsets;
END_CMXSTRUCT(FontIndex)


START_CMXSTRUCT
		//	Font Info
	StringBase *	FontName;
	INT32			FontStyle;
		//	Panose Stuffs
	INT32	bFamily;
	INT32	bSerifStyle;
	INT32	bWeight;
	INT32	bProportion;
	INT32	bContrast;
	INT32	bStrokeVaration;
	INT32	bArmStyle;
	INT32	bLetterForm;
	INT32	bMidline;
	INT32	bXHeight;
END_CMXSTRUCT(FontRecord)

	

//-0-0-----------------------------------------------------------------------------------

#pragma pack(pop)
#endif	// INC_CMXISTUT

