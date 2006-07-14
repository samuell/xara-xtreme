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
// Constants and types for the v2 file format

#ifndef INC_CXFDEFS
#define INC_CXFDEFS


/////////////////////////////////////////////////////////////////////////////
// Camelot version 1.5 tags
/////////////////////////////////////////////////////////////////////////////


#define CXF_IDWORD1			0x41524158
#define CXF_IDWORD2			0x0a0da3a3
#define CXF_UNKNOWN_SIZE	(-1)

//-------------------------------------------------------------------------------------
// Data Types
//-------------------------------------------------------------------------------------
typedef unsigned short	UINT16;
typedef short			INT16;

//-------------------------------------------------------------------------------------
// Record Data Constants
//-------------------------------------------------------------------------------------
#define CXF_TRANSPARANCY_TYPE_NONE			0
#define CXF_TRANSPARANCY_TYPE_MIX			1
#define CXF_TRANSPARANCY_TYPE_STAINEDGLASS	2
#define CXF_TRANSPARANCY_TYPE_BLEACH		3
#define CXF_TRANSPARENCY_TYPE_CONTRAST		13
#define CXF_TRANSPARENCY_TYPE_SATURATION	16
#define CXF_TRANSPARENCY_TYPE_DARKEN		19
#define CXF_TRANSPARENCY_TYPE_LIGHTEN		22
#define CXF_TRANSPARENCY_TYPE_BRIGHTNESS	25
#define CXF_TRANSPARENCY_TYPE_LUMINOSITY	28

#define CXF_CAP_STYLE_BUTT		1
#define CXF_CAP_STYLE_ROUND		2
#define CXF_CAP_STYLE_SQUARE	3

#define CXF_JOIN_STYLE_MITRE	1
#define CXF_JOIN_STYLE_ROUND	2
#define CXF_JOIN_STYLE_BEVELLED	3

#define CXF_WINDING_RULE_NONZERO	1
#define CXF_WINDING_RULE_NEGATIVE	2
#define CXF_WINDING_RULE_EVENODD	3
#define CXF_WINDING_RULE_POSITIVE	4

//-------------------------------------------------------------------------------------
// References
//-------------------------------------------------------------------------------------

// Default bitmap definitions for the v2 file format
// These are the built in bitmaps that can be referred to by just their reference number
// as they are assumed to be built in.
// As with all other default references, all their numbers go from -1 downwards i.e. more negative
// and hence occupy the negative range of a INT32 record number.
// Unlike colours, where this is defined in a separate file, cfxcols.h, the bitmaps are
// liklely to be the one default reference which is the default bitmap.

#define REF_DEFAULTBITMAP_BUILTIN	(-1)

//-------------------------------------------------------------------------------------
// Record sizes
//-------------------------------------------------------------------------------------

//-----------------
// Size of Compression records

#define TAG_STARTCOMPRESSION_SIZE	4
#define TAG_ENDCOMPRESSION_SIZE		8

//-----------------
// Document records
#define TAG_CHAPTER_SIZE			0
#define TAG_SPREAD_SIZE				0
#define TAG_PAGE_SIZE				16
#define TAG_SPREADINFORMATION_SIZE	17

#define TAG_GRIDRULERSETTINGS_SIZE	17
#define TAG_GRIDRULERORIGIN_SIZE	8

//----------------
// Tag management
#define TAG_ATOMICTAGS_SIZE			CXF_UNKNOWN_SIZE
#define TAG_ESSENTIALTAGS_SIZE		CXF_UNKNOWN_SIZE
#define TAG_TAGDESCRIPTION_SIZE		CXF_UNKNOWN_SIZE

//-----------------
// Render-related tags
#define TAG_NONRENDERSECTION_START_SIZE	0
#define TAG_NONRENDERSECTION_END_SIZE	0
#define TAG_RENDERING_PAUSE_SIZE		0
#define TAG_RENDERING_RESUME_SIZE		0

//----------------
// Layers
#define TAG_LAYER_SIZE				0
#define TAG_LAYERDETAILS_SIZE		CXF_UNKNOWN_SIZE
#define TAG_GUIDELAYERDETAILS_SIZE	CXF_UNKNOWN_SIZE
#define TAG_LAYER_FLAG_VISIBLE		0x1
#define TAG_LAYER_FLAG_LOCKED		0x2
#define TAG_LAYER_FLAG_PRINTABLE	0x4
#define TAG_LAYER_FLAG_ACTIVE		0x8
#define TAG_LAYER_FLAG_PAGEBACKGROUND	0x10
#define TAG_LAYER_FLAG_BACKGROUND	0x20

// New Frame Layer tags in new flags byte.
#define TAG_LAYER_FLAG_SOLID		0x1 //
#define TAG_LAYER_FLAG_OVERLAY		0x2 //
#define TAG_LAYER_FLAG_HIDDEN		0x4 //

#define TAG_LAYER_FRAMEPROPS_SIZE	5
#define TAG_SPREAD_ANIMPROPS_SIZE	28



#define TAG_SPREADSCALING_ACTIVE_SIZE	24
#define TAG_SPREADSCALING_INACTIVE_SIZE	24

//-----------------
// Size of Colour reference records

#define TAG_DEFINERGBCOLOUR_SIZE		3
#define TAG_DEFINECOMPLEXCOLOUR_SIZE	29

//-----------------
// View records
#define TAG_VIEWPORT_SIZE		16
#define TAG_VIEWQUALITY_SIZE	1

#define TAG_DOCUMENTVIEW_SIZE	24

//-----------------
// Document unit tags

#define TAG_DEFINE_PREFIXUSERUNIT_SIZE		28
#define TAG_DEFINE_SUFFIXUSERUNIT_SIZE		28
#define TAG_DEFINE_DEFAULTUNITS_SIZE		8

//-----------------
// Document info tags

#define TAG_DOCUMENTDATES_SIZE				8
#define TAG_DOCUMENTUNDOSIZE_SIZE			4
#define TAG_DOCUMENTFLAGS_SIZE				4

//-----------------
// document nudge size tag
#define TAG_DOCUMENTNUDGESIZE				4

//-----------------
// Attribute record sizes
#define TAG_LINECOLOUR_SIZE							4
#define TAG_FLATFILL_SIZE							4
#define TAG_LINEWIDTH_SIZE							4
#define TAG_LINEARFILL_SIZE							40		// was 24, BUT + 16 for profile == 40
#define TAG_LINEARFILL3POINT_SIZE					48
#define TAG_CIRCULARFILL_SIZE						40		// was 24, BUT + 16 for profile == 40
#define TAG_ELLIPTICALFILL_SIZE						48		// was 32, BUT + 16 for profile == 48
#define TAG_CONICALFILL_SIZE						40		// was 24, BUT + 16 for profile == 40
#define TAG_SQUAREFILL_SIZE							48		// was 32, BUT + 16 for profile == 48
#define TAG_THREECOLFILL_SIZE						36
#define TAG_FOURCOLFILL_SIZE						40
#define TAG_BITMAPFILL_SIZE							44		// was 28, BUT + 16 for profile == 44
#define TAG_CONTONEBITMAPFILL_SIZE					52		// was 36, BUT + 16 for profile == 52
#define TAG_FRACTALFILL_SIZE						69		// was 53, BUT + 16 for profile == 69
#define TAG_NOISEFILL_SIZE							61		// was 45, BUT + 16 for profile == 61
#define TAG_FILLEFFECT_FADE_SIZE					0
#define TAG_FILLEFFECT_RAINBOW_SIZE					0
#define TAG_FILLEFFECT_ALTRAINBOW_SIZE				0
#define TAG_FILL_REPEATING_SIZE						0
#define TAG_FILL_NONREPEATING_SIZE					0
#define TAG_FILL_REPEATINGINVERTED_SIZE				0
//Mark Howitt, 8/10/97. define new repeating grad fill tag size.
#define TAG_FILL_REPEATING_EXTRA_SIZE				0
//Chris Snook, 8/12/99. We also need one for the transparencies!
#define TAG_TRANSPARENTFILL_REPEATING_EXTRA_SIZE	0

#define TAG_FLATTRANSPARENTFILL_SIZE				2
#define TAG_LINEARTRANSPARENTFILL_SIZE				35		// was 19, BUT + 16 for profile == 35
#define TAG_LINEARTRANSPARENTFILL3POINT_SIZE		43
#define TAG_CIRCULARTRANSPARENTFILL_SIZE			35		// was 19, BUT + 16 for profile == 35
#define TAG_ELLIPTICALTRANSPARENTFILL_SIZE			43		// was 27, BUT + 16 for profile == 43
#define TAG_CONICALTRANSPARENTFILL_SIZE				35		// was 19, BUT + 16 for profile == 35
#define TAG_SQUARETRANSPARENTFILL_SIZE				43		// was 27, BUT + 16 for profile == 43
#define TAG_THREECOLTRANSPARENTFILL_SIZE			28
#define TAG_FOURCOLTRANSPARENTFILL_SIZE				29
#define TAG_BITMAPTRANSPARENTFILL_SIZE				47		// was 31, BUT + 16 for profile == 47
#define TAG_FRACTALTRANSPARENTFILL_SIZE				64		// was 48, BUT + 16 for profile == 64
#define TAG_NOISETRANSPARENTFILL_SIZE				56		// was 40, BUT + 16 for profile == 56
#define TAG_LINETRANSPARENCY_SIZE					2
#define TAG_STARTCAP_SIZE							1
#define TAG_ENDCAP_SIZE								1
#define TAG_JOINSTYLE_SIZE							1
#define TAG_MITRELIMIT_SIZE							4
#define TAG_WINDINGRULE_SIZE						1
#define TAG_QUALITY_SIZE							4
#define TAG_TRANSPARENTFILL_REPEATING_SIZE			0
#define TAG_TRANSPARENTFILL_NONREPEATING_SIZE		0
#define TAG_TRANSPARENTFILL_REPEATINGINVERTED_SIZE	0
#define TAG_USERVALUEATTRSIZE						CXF_UNKNOWN_SIZE
#define TAG_WIZOP_STYLEREF_SIZE						4

#define TAG_FLATFILL_NONE_SIZE						0
#define TAG_FLATFILL_BLACK_SIZE						0
#define TAG_FLATFILL_WHITE_SIZE						0
#define TAG_LINECOLOUR_NONE_SIZE					0
#define TAG_LINECOLOUR_BLACK_SIZE					0
#define TAG_LINECOLOUR_WHITE_SIZE					0

//-----------------
// Path records
#define TAG_PATH_SIZE				CXF_UNKNOWN_SIZE
#define TAG_PATH_FLAGS_SIZE			CXF_UNKNOWN_SIZE
#define TAG_PATH_FLAGS_SMOOTH		0x1
#define TAG_PATH_FLAGS_ROTATE		0x2
#define TAG_PATH_FLAGS_ENDPOINT		0x4
#define TAG_PATH_RELATIVE_SIZE		CXF_UNKNOWN_SIZE
#define TAG_PATHREF_INDENTICAL_SIZE	4
#define TAG_PATHREF_TRANSFORM_SIZE	28

//-----------------
// Arrows and dash patterns
#define TAG_DASHSTYLE_SIZE							4
#define TAG_ARROWHEAD_SIZE							12
#define TAG_ARROWTAIL_SIZE							12

//-----------------
// Blends
#define TAG_BLEND_SIZE				3
#define TAG_BLENDER_SIZE			8
#define TAG_BLEND_FLAG_ONETOONE		(1 << 0)	// Bit 0 is the one to one flag
#define TAG_BLEND_FLAG_ANTIALIAS	(1 << 1)	// Bit 1 is the antialias flag
#define TAG_BLEND_FLAG_TANGENTIAL	(1 << 2)	// Bit 2 is the tangential flag
#define TAG_BLEND_COLEFFECT_MASK	0xf0		// Top 4 bits of the flags byte contains the col blend type
#define TAG_BLEND_COLEFFECT_SHIFT	4			//		The col blend type can be either, fade, rainbow or alt rainbow

//-----------------
// Moulds
#define TAG_MOULD_ENVELOPE_SIZE		4
#define TAG_MOULD_PERSPECTIVE_SIZE	4
#define TAG_MOULD_GROUP_SIZE		0
#define TAG_MOULD_PATH_SIZE			CXF_UNKNOWN_SIZE

// ----------------
// node bitmap
#define TAG_NODE_BITMAP_SIZE				36
#define TAG_NODE_CONTONEDBITMAP_SIZE		44

// bitmap properties - stores the settings in the b.p. dialogue for each bitmap
#define TAG_BITMAP_PROPERTIES_SIZE			(sizeof(INT32)+8)
#define TAG_DOCUMENTBITMAPSMOOTHINGSIZE		5


//-----------------
// Misc objects
#define TAG_GUIDELINE_SIZE			5

// ----------------
// Printing and Imagesetting data
// Imagesetting attributes - these are all boolean tags with no actual record data
const INT32 TAG_OVERPRINTLINEON_SIZE			= 0;
const INT32 TAG_OVERPRINTLINEOFF_SIZE		= 0;
const INT32 TAG_OVERPRINTFILLON_SIZE			= 0;
const INT32 TAG_OVERPRINTFILLOFF_SIZE		= 0;
const INT32 TAG_PRINTONALLPLATESON_SIZE		= 0;
const INT32 TAG_PRINTONALLPLATESOFF_SIZE		= 0;
// Main printing and Imagesetting data records
const INT32 TAG_PRINTERSETTINGS_SIZE			= 45;
const INT32 TAG_IMAGESETTING_SIZE			= 15;
const INT32 TAG_COLOURPLATE_SIZE				= 22;
// Printers mark records
const INT32 TAG_PRINTMARKDEFAULT_SIZE		= 1;
const INT32 TAG_PRINTMARKCUSTOM_SIZE			= CXF_UNKNOWN_SIZE;

//-----------------
// Size of transform attribute records

#define TAG_TRANSFORM_IDENTITY_SIZE			0		
#define TAG_TRANSFORM_MATRIX_SIZE			16	
#define TAG_TRANSFORM_TRANSLATION_SIZE		8
#define TAG_TRANSFORM_GENERAL_SIZE			24
#define TAG_TRANSFORM_ROTATION_SIZE			4
#define TAG_TRANSFORM_ROTATION_CENTRE_SIZE	4
#define TAG_TRANSFORM_ROTATION_POINT_SIZE	12
#define TAG_TRANSFORM_SHEAR_X_SIZE			4
#define TAG_TRANSFORM_SHEAR_Y_SIZE			4
#define TAG_TRANSFORM_SCALE_SIZE			4
#define TAG_TRANSFORM_SCALE_X_SIZE			4
#define TAG_TRANSFORM_SCALE_Y_SIZE			4
#define TAG_TRANSFORM_REFLECTION_X_SIZE		0
#define TAG_TRANSFORM_REFLECTION_Y_SIZE		0

//-----------------
// Size of regular shape records
// Not all of these are defined in the spec, those that are are marked as such.
// Note - any object with reformed edges automatically has an unknown size.

// rectangle			: size += 0
// ellipse				: size += 0
// polygon				: size += 2  (number of sides)
// simple				: size += 16 (centre.x, centre.y, width, height)
// complex				: size += 24 (centre.x, centre.y, major.x, major.y, minor.x, minor.y)
// roundedness			: size += 8  (roundedness)
// stellation			: size += 16
// rounded + stellated	: size += 8
// reformed				: size = CXF_UNKNOWN_SIZE

// Ellipses
#define TAG_ELLIPSE_SIMPLE_SIZE									16					// defined
#define TAG_ELLIPSE_COMPLEX_SIZE								24					// defined

// Rectangles
#define TAG_RECTANGLE_SIMPLE_SIZE								16					// defined
#define TAG_RECTANGLE_SIMPLE_REFORMED_SIZE						CXF_UNKNOWN_SIZE
#define TAG_RECTANGLE_SIMPLE_STELLATED_SIZE						32
#define TAG_RECTANGLE_SIMPLE_STELLATED_REFORMED_SIZE			CXF_UNKNOWN_SIZE
#define TAG_RECTANGLE_SIMPLE_ROUNDED_SIZE						24					// defined
#define TAG_RECTANGLE_SIMPLE_ROUNDED_REFORMED_SIZE				CXF_UNKNOWN_SIZE
#define TAG_RECTANGLE_SIMPLE_ROUNDED_STELLATED_SIZE				48
#define TAG_RECTANGLE_SIMPLE_ROUNDED_STELLATED_REFORMED_SIZE	CXF_UNKNOWN_SIZE
#define TAG_RECTANGLE_COMPLEX_SIZE								24					// defined
#define TAG_RECTANGLE_COMPLEX_REFORMED_SIZE						CXF_UNKNOWN_SIZE
#define TAG_RECTANGLE_COMPLEX_STELLATED_SIZE					40
#define TAG_RECTANGLE_COMPLEX_STELLATED_REFORMED_SIZE			CXF_UNKNOWN_SIZE
#define TAG_RECTANGLE_COMPLEX_ROUNDED_SIZE						32					// defined
#define TAG_RECTANGLE_COMPLEX_ROUNDED_REFORMED_SIZE				CXF_UNKNOWN_SIZE
#define TAG_RECTANGLE_COMPLEX_ROUNDED_STELLATED_SIZE			56
#define TAG_RECTANGLE_COMPLEX_ROUNDED_STELLATED_REFORMED_SIZE	CXF_UNKNOWN_SIZE

// Polygons
#define TAG_POLYGON_COMPLEX_SIZE								26					// defined
#define TAG_POLYGON_COMPLEX_REFORMED_SIZE						CXF_UNKNOWN_SIZE	// defined
#define TAG_POLYGON_COMPLEX_STELLATED_SIZE						42					// defined
#define TAG_POLYGON_COMPLEX_STELLATED_REFORMED_SIZE				CXF_UNKNOWN_SIZE	// defined
#define TAG_POLYGON_COMPLEX_ROUNDED_SIZE						34					// defined
#define TAG_POLYGON_COMPLEX_ROUNDED_REFORMED_SIZE				CXF_UNKNOWN_SIZE	// defined
#define TAG_POLYGON_COMPLEX_ROUNDED_STELLATED_SIZE				58					// defined
#define TAG_POLYGON_COMPLEX_ROUNDED_STELLATED_REFORMED_SIZE		CXF_UNKNOWN_SIZE

// General regular shapes
// (You need to read the prose above the corresponding definitions in kernel/cxftags.h
// if you wish to know the differences between these two - ach)
#define TAG_REGULAR_SHAPE_PHASE_1_SIZE				CXF_UNKNOWN_SIZE
#define TAG_REGULAR_SHAPE_PHASE_2_SIZE				CXF_UNKNOWN_SIZE

// Text definitions

#define TAG_FONT_DEF_TRUETYPE_SIZE					CXF_UNKNOWN_SIZE
#define TAG_FONT_DEF_ATM_SIZE						CXF_UNKNOWN_SIZE

// Text objects

#define TAG_TEXT_STORY_SIMPLE_SIZE					12		// was 8, but + 4 for new autokern flag == 12
#define TAG_TEXT_STORY_COMPLEX_SIZE					28		// was 24, but + 4 for new autokern flag == 28

#define TAG_TEXT_STORY_SIMPLE_START_LEFT_SIZE		12		// was 8, but + 4 for new autokern flag == 12
#define TAG_TEXT_STORY_SIMPLE_START_RIGHT_SIZE		12		// was 8, but + 4 for new autokern flag == 12
#define TAG_TEXT_STORY_SIMPLE_END_LEFT_SIZE			12		// was 8, but + 4 for new autokern flag == 12
#define TAG_TEXT_STORY_SIMPLE_END_RIGHT_SIZE		12		// was 8, but + 4 for new autokern flag == 12
#define TAG_TEXT_STORY_COMPLEX_START_LEFT_SIZE		36		// was 32, but + 4 for new autokern flag == 36
#define TAG_TEXT_STORY_COMPLEX_START_RIGHT_SIZE		36		// was 32, but + 4 for new autokern flag == 36
#define TAG_TEXT_STORY_COMPLEX_END_LEFT_SIZE		36		// was 32, but + 4 for new autokern flag == 36
#define TAG_TEXT_STORY_COMPLEX_END_RIGHT_SIZE		36		// was 32, but + 4 for new autokern flag == 36

#define TAG_TEXT_STORY_WORD_WRAP_INFO_SIZE			5
#define TAG_TEXT_STORY_INDENT_INFO_SIZE				8

#define TAG_TEXT_LINE_SIZE							0
#define TAG_TEXT_STRING_SIZE						CXF_UNKNOWN_SIZE
#define TAG_TEXT_CHAR_SIZE							2
#define TAG_TEXT_EOL_SIZE							0
#define TAG_TEXT_KERN_SIZE							8
#define TAG_TEXT_CARET_SIZE							0
#define TAG_TEXT_LINE_INFO_SIZE						12

// Text attributes

#define TAG_TEXT_LINESPACE_RATIO_SIZE				4
#define TAG_TEXT_LINESPACE_ABSOLUTE_SIZE			4
#define TAG_TEXT_JUSTIFICATION_LEFT_SIZE			0
#define TAG_TEXT_JUSTIFICATION_CENTRE_SIZE			0
#define TAG_TEXT_JUSTIFICATION_RIGHT_SIZE			0
#define TAG_TEXT_JUSTIFICATION_FULL_SIZE			0
#define TAG_TEXT_FONT_TYPEFACE_SIZE					4
#define TAG_TEXT_FONT_SIZE_SIZE						4				
#define TAG_TEXT_BOLD_ON_SIZE						0
#define TAG_TEXT_BOLD_OFF_SIZE						0
#define TAG_TEXT_ITALIC_ON_SIZE						0
#define TAG_TEXT_ITALIC_OFF_SIZE					0
#define TAG_TEXT_UNDERLINE_ON_SIZE						0
#define TAG_TEXT_UNDERLINE_OFF_SIZE					0
#define TAG_TEXT_SCRIPT_ON_SIZE						8	// explicit offset, size
#define TAG_TEXT_SCRIPT_OFF_SIZE					0
#define TAG_TEXT_SUPERSCRIPT_ON_SIZE				0	// implicit offset, size
#define TAG_TEXT_SUBSCRIPT_ON_SIZE					0	// implicit offset, size
#define TAG_TEXT_TRACKING_SIZE						4
#define TAG_TEXT_ASPECT_RATIO_SIZE					4
#define TAG_TEXT_BASELINE_SIZE						4


/////////////////////////////////////////////////////////////////////////////
// Camelot version 2.0 - additional tags
/////////////////////////////////////////////////////////////////////////////

// ----------------
// New-style strokes
const INT32 TAG_VARIABLEWIDTHFUNC_SIZE		= 4;
const INT32 TAG_VARIABLEWIDTHTABLE_SIZE		= CXF_UNKNOWN_SIZE;
const INT32 TAG_STROKETYPE_SIZE				= 4;
const INT32 TAG_STROKEDEFINITION_SIZE		= CXF_UNKNOWN_SIZE;
const INT32 TAG_STROKEAIRBRUSH_SIZE			= CXF_UNKNOWN_SIZE;

// Mould source object bounds
const INT32 TAG_MOULD_BOUNDS_SIZE			= 16;

// Web Address records
const INT32 TAG_WEBADDRESS_SIZE				= CXF_UNKNOWN_SIZE;

// Template Argument/Ops / WizOp / Webster Properties records
const INT32 TAG_WIZOP_SIZE					= CXF_UNKNOWN_SIZE;

/////////////////////////////////////////////////////////////////////////////
/// Camelot version 2.1 tags i.e. next version after 2.0
/////////////////////////////////////////////////////////////////////////////

//-----------------
// Shadows
const INT32 TAG_SHADOWCONTROLLER_SIZE		= 29;
const INT32 TAG_SHADOW_SIZE					= 24;	// was 16, BUT + 8 for Darkness = 24.

//-----------------
// bevel attributes
#define TAG_BEVEL_INDENT_SIZE				4	// INT32
#define TAG_BEVEL_DPI_SIZE					4
#define TAG_BEVEL_TYPE_SIZE					4
#define TAG_BEVEL_LIGHTANGLE_SIZE			8	// double

//-----------------
// Blend on a curve tags
const INT32 TAG_BLENDER_CURVEPROP_SIZE		= (sizeof(double)*2);
const INT32 TAG_BLEND_PATH_SIZE				= CXF_UNKNOWN_SIZE;
const INT32 TAG_BLENDER_CURVEANGLES_SIZE		= (sizeof(double)*2);
const INT32 TAG_BLENDPROFILES_SIZE			= (sizeof(double)*6);
const INT32 TAG_BLENDERADDITIONAL_SIZE		= 17;	// was 16, BUT + 1 for 1-BYTE bitfield storage = 17.
const INT32 TAG_NODEBLENDPATH_FILLED_SIZE	= 4;

//-----------------
// Contour tags
const INT32 TAG_CONTOUR_CONTROLLER_SIZE		= 4 + 4 + 1 + 8 + 8 + 8 + 8;
const INT32 TAG_CONTOUR_SIZE					= 0;

//-----------------
#define TAG_LINEARFILLMULTISTAGE_SIZE		CXF_UNKNOWN_SIZE
#define TAG_LINEARFILLMULTISTAGE3POINT_SIZE	CXF_UNKNOWN_SIZE
#define TAG_CIRCULARFILLMULTISTAGE_SIZE		CXF_UNKNOWN_SIZE
#define TAG_CONICALFILLMULTISTAGE_SIZE		CXF_UNKNOWN_SIZE
#define TAG_ELLIPTICALFILLMULTISTAGE_SIZE	CXF_UNKNOWN_SIZE
#define TAG_SQUAREFILLMULTISTAGE_SIZE		CXF_UNKNOWN_SIZE

//-----------------
// Sets and their properties.
const INT32 TAG_SETSENTINEL_SIZE				= 0;
const INT32 TAG_SETPROPERTY_SIZE				= CXF_UNKNOWN_SIZE;
const INT32 TAG_BARPROPERTY_SIZE				= CXF_UNKNOWN_SIZE;

//-----------------
// painting mode size tags
//const INT32 TAG_PAINTINGMODE_SIZE			= 1;

// brush attribute tags
const UINT32 TAG_BRUSHATTR_SIZE				= (sizeof(INT32) * 4) + (sizeof(double) *2) +1;											 
const UINT32 TAG_BRUSHDEFINITION_SIZE		= (sizeof(INT32)); //CXF_UNKNOWN_SIZE;
#define		TAG_BRUSHTILE_FLAG				0x1;
#define     TAG_BRUSHROTATE_FLAG			0x2;
const UINT32 TAG_BRUSHDATA_SIZE              = UINT32(CXF_UNKNOWN_SIZE); //TAG_BRUSHATTR_SIZE;
const UINT32 TAG_MOREBRUSHDATA_SIZE			= (sizeof(INT32) * 11) + (sizeof(double) *3);
const UINT32 TAG_MOREBRUSHATTR_SIZE			= TAG_MOREBRUSHDATA_SIZE + (sizeof(INT32));
const UINT32 TAG_EVENMOREBRUSHDATA_SIZE      = (sizeof(INT32) * 2);
const UINT32 TAG_EVENMOREBRUSHATTR_SIZE		= TAG_EVENMOREBRUSHDATA_SIZE + 1;
const UINT32 TAG_TIMESTAMPBRUSHDATA_SIZE     = UINT32(CXF_UNKNOWN_SIZE);
const UINT32 TAG_BRUSHPRESSUREINFO_SIZE		= sizeof(INT32) * 7;
const UINT32 TAG_BRUSHATTRPRESSUREINFO_SIZE	= sizeof(INT32) * 7;
const UINT32 TAG_BRUSHPRESSUREDATA_SIZE		= UINT32(CXF_UNKNOWN_SIZE);
const UINT32 TAG_BRUSHSAMPLEDATA_SIZE		= UINT32(CXF_UNKNOWN_SIZE);
const UINT32 TAG_BRUSHATTRFILLFLAGS_SIZE     = 1;
const UINT32 TAG_BRUSHTRANSPINFO_SIZE		= (sizeof(INT32)*6) + (sizeof(double)*2);
const UINT32 TAG_BRUSHATTRTRANSPINFO_SIZE    = (sizeof(INT32)*6) + (sizeof(double)*2);

#define		BRUSHFILLFLAG_LOCALFILL			0x1;
#define		BRUSHFILLFLAG_LOCALTRANSP		0x2;
#define		BRUSHFILLFLAG_NAMEDCOL			0x4;
#define TAG_SCALETOWIDTH_FLAG               0x1;

//-----------------
// ClipView tags
const INT32 TAG_CLIPVIEW_SIZE				= 0;
const INT32 TAG_CLIPVIEW_CONTROLLER_SIZE		= 0;

//-----------------
// Feather attribute
const UINT32 TAG_FEATHER_SIZE				= 20;	// Millipoint value. was 4, BUT + 16 for profile == 20

//-----------------
// Current Attribute Group
const UINT32 TAG_CURRENTATTRIBUTES_SIZE		= 1;
	const UINT32 ATTRIBUTEGROUP_INK			= 1;
	const UINT32 ATTRIBUTEGROUP_TEXT			= 2;
const UINT32 TAG_CURRENTATTRIBUTEBOUNDS_SIZE = sizeof(INT32) * 4;

//-----------------
// Duplication Offset record
const UINT32 TAG_DUPLICATIONOFFSET_SIZE		= sizeof(INT32) * 2;

//-----------------
// CompoundRender records size
const UINT32 TAG_COMPOUNDRENDER_SIZE			= 4 + 16;	// Flags + 2 * Coord

//-----------------
// ObjectBounds records size
const UINT32 TAG_OBJECTBOUNDS_SIZE			= 16;		// 2 * Coord

/////////////////////////////////////////////////////////////////////////////
// XaraLX 0.6
/////////////////////////////////////////////////////////////////////////////

// ------------------------------------------------------------
// New text tags for tabbing and paragraph-based indent support

// Text objects
const UINT32 TAG_TEXT_TAB_SIZE				= 0;

// Text attributes
const UINT32 TAG_TEXT_LEFT_INDENT_SIZE		= 4;
const UINT32 TAG_TEXT_FIRST_INDENT_SIZE		= 4;
const UINT32 TAG_TEXT_RIGHT_INDENT_SIZE		= 4;
const UINT32 TAG_TEXT_RULER_SIZE			= UINT32(CXF_UNKNOWN_SIZE);

#endif
