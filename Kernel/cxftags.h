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
// This file defines all the allocated tags in the v2 CorelXARA file format

#ifndef INC_CXFTAGS
#define INC_CXFTAGS

	//////////////////////////////////////////////////////////////////////////
	// IMPORTANT															//
	// To keep things tidy & maintainable, tags from previous releases of	//
	// camelot are now FROZEN. You MUST NOT add tags with values in "old"	//
	// ranges: See the bottom of this file for the current tag range to use	//
	//																		//
	// Allocated ranges are:												//
	// Range		Comments												//
	// ---------    --------												//
	// 0-3999		Initial release (Camelot v1.5, 1st file format release) //
	// 4000-4999	Second release (Camelot v2.0)							//
	//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Camelot version 1.5 tags - range 0..3999 - FROZEN!
/////////////////////////////////////////////////////////////////////////////
//some harmless comments
//-----------------
// General tags

#define TAG_UNDEFINED (0xffffffff)		// Special tag that is never used.  Used to detect errors

#define TAG_UP					0
#define TAG_DOWN				1
#define TAG_FILEHEADER			2
#define TAG_ENDOFFILE			3

//-----------------
// Tag management
#define TAG_ATOMICTAGS				10
#define TAG_ESSENTIALTAGS			11
#define TAG_TAGDESCRIPTION			12

//-----------------
// Render-related tags
#define TAG_NONRENDERSECTION_START	20
#define TAG_NONRENDERSECTION_END	21
#define TAG_RENDERING_PAUSE			22
#define TAG_RENDERING_RESUME		23

//-----------------
// Compression & precompression tags 

#define TAG_STARTCOMPRESSION	30
#define TAG_ENDCOMPRESSION		31

//-----------------
// Document tags

#define TAG_DOCUMENT			40
#define TAG_CHAPTER				41
#define TAG_SPREAD				42
#define TAG_LAYER				43
#define TAG_PAGE				44
#define TAG_SPREADINFORMATION	45

#define TAG_GRIDRULERSETTINGS	46
#define TAG_GRIDRULERORIGIN		47
#define TAG_LAYERDETAILS		48
#define TAG_GUIDELAYERDETAILS	49

#define TAG_SPREADSCALING_ACTIVE	52
#define TAG_SPREADSCALING_INACTIVE	53

//-----------------
// Colour reference tags

#define TAG_DEFINERGBCOLOUR		50
#define TAG_DEFINECOMPLEXCOLOUR	51

//-----------------
// Bitmap reference tags

#define TAG_PREVIEWBITMAP_BMP		60
#define TAG_PREVIEWBITMAP_GIF		61
#define TAG_PREVIEWBITMAP_JPEG		62
#define TAG_PREVIEWBITMAP_PNG		63
#define TAG_PREVIEWBITMAP_TIFFLZW	64

#define TAG_DEFINEBITMAP_BMP		65
#define TAG_DEFINEBITMAP_GIF		66
#define TAG_DEFINEBITMAP_JPEG		67
#define TAG_DEFINEBITMAP_PNG		68
#define TAG_DEFINEBITMAP_BMPZIP		69
#define TAG_DEFINESOUND_WAV			70
#define TAG_DEFINEBITMAP_JPEG8BPP	71

//-----------------
// View tags

#define TAG_VIEWPORT				80
#define TAG_VIEWQUALITY				81
#define TAG_DOCUMENTVIEW			82

//-----------------
// Document unit tags

#define TAG_DEFINE_PREFIXUSERUNIT		85
#define TAG_DEFINE_SUFFIXUSERUNIT		86
#define TAG_DEFINE_DEFAULTUNITS			87

//-----------------
// Document info tags

#define TAG_DOCUMENTCOMMENT				90
#define TAG_DOCUMENTDATES				91
#define TAG_DOCUMENTUNDOSIZE			92
//#define TAG_DOCUMENTINDEXING			xx
#define TAG_DOCUMENTFLAGS				93

//-----------------
// Name Gallery tags

#define TAG_NAMEGAL_DOCCOMP				95

//-----------------
// Object tags

#define TAG_PATH							100
#define TAG_PATH_FILLED						101
#define TAG_PATH_STROKED					102
#define TAG_PATH_FILLED_STROKED				103
#define TAG_GROUP							104
#define TAG_BLEND							105
#define TAG_BLENDER							106
#define TAG_MOULD_ENVELOPE					107
#define TAG_MOULD_PERSPECTIVE				108
#define TAG_MOULD_GROUP						109
#define TAG_MOULD_PATH						110
#define TAG_PATH_FLAGS						111
#define TAG_GUIDELINE						112
#define TAG_PATH_RELATIVE					113
#define TAG_PATH_RELATIVE_FILLED			114
#define TAG_PATH_RELATIVE_STROKED			115
#define TAG_PATH_RELATIVE_FILLED_STROKED	116
//#define TAG_PATHREF_INDENTICAL				117		// Not used by version 1.5
#define TAG_PATHREF_TRANSFORM				118

//-----------------
// Attribute tags

#define TAG_FLATFILL								150
#define TAG_LINECOLOUR								151
#define TAG_LINEWIDTH								152
#define TAG_LINEARFILL								153
#define TAG_CIRCULARFILL							154
#define TAG_ELLIPTICALFILL							155
#define TAG_CONICALFILL								156
#define TAG_BITMAPFILL								157
#define TAG_CONTONEBITMAPFILL						158
#define TAG_FRACTALFILL								159
#define TAG_FILLEFFECT_FADE							160
#define TAG_FILLEFFECT_RAINBOW						161
#define TAG_FILLEFFECT_ALTRAINBOW					162
#define TAG_FILL_REPEATING							163
#define TAG_FILL_NONREPEATING						164
#define TAG_FILL_REPEATINGINVERTED					165
#define TAG_FLATTRANSPARENTFILL						166
#define TAG_LINEARTRANSPARENTFILL					167
#define TAG_CIRCULARTRANSPARENTFILL					168
#define TAG_ELLIPTICALTRANSPARENTFILL				169
#define TAG_CONICALTRANSPARENTFILL					170
#define TAG_BITMAPTRANSPARENTFILL					171
#define TAG_FRACTALTRANSPARENTFILL					172
#define TAG_LINETRANSPARENCY						173
#define TAG_STARTCAP								174
#define TAG_ENDCAP									175
#define TAG_JOINSTYLE								176
#define TAG_MITRELIMIT								177
#define TAG_WINDINGRULE								178
#define TAG_QUALITY									179
#define TAG_TRANSPARENTFILL_REPEATING				180
#define TAG_TRANSPARENTFILL_NONREPEATING			181
#define TAG_TRANSPARENTFILL_REPEATINGINVERTED		182

// Arrows and dash patterns
#define TAG_DASHSTYLE								183
#define TAG_DEFINEDASH								184
#define TAG_ARROWHEAD								185
#define TAG_ARROWTAIL								186
#define TAG_DEFINEARROW								187
#define TAG_DEFINEDASH_SCALED						188

// User Attributes
#define TAG_USERVALUE								189

// special colour fills
#define TAG_FLATFILL_NONE							190
#define TAG_FLATFILL_BLACK							191
#define TAG_FLATFILL_WHITE							192
#define TAG_LINECOLOUR_NONE							193
#define TAG_LINECOLOUR_BLACK						194
#define TAG_LINECOLOUR_WHITE						195

// Bitmaps
// general node bitmap - don't know how long this will stay like this
#define TAG_NODE_BITMAP								198
#define TAG_NODE_CONTONEDBITMAP						199


// New fill type records
//
// Tags range 200 - 299

#define TAG_SQUAREFILL								200
#define TAG_SQUARETRANSPARENTFILL					201
#define TAG_THREECOLFILL							202
#define TAG_THREECOLTRANSPARENTFILL					203
#define TAG_FOURCOLFILL								204
#define TAG_FOURCOLTRANSPARENTFILL					205
//Mark Howitt, 8/10/97. Include new repeating grad fill tag
#define TAG_FILL_REPEATING_EXTRA					206
//Chris Snook, 8/12/99. We also need one for the transparencies!
#define TAG_TRANSPARENTFILL_REPEATING_EXTRA			207


//-----------------
// Regular shapes
// Similarly, tags in the range 1000 - 1999 are reserved for regular shapes.
// Not all are defined in the spec, those that are are marked. However all these
// get used somewhere (I think).

// Ellipses
#define TAG_ELLIPSE_SIMPLE									1000	// defined
#define TAG_ELLIPSE_COMPLEX									1001	// defined

// Rectangles
#define TAG_RECTANGLE_SIMPLE								1100	// defined
#define TAG_RECTANGLE_SIMPLE_REFORMED						1101
#define TAG_RECTANGLE_SIMPLE_STELLATED						1102
#define TAG_RECTANGLE_SIMPLE_STELLATED_REFORMED				1103
#define TAG_RECTANGLE_SIMPLE_ROUNDED						1104	// defined
#define TAG_RECTANGLE_SIMPLE_ROUNDED_REFORMED				1105
#define TAG_RECTANGLE_SIMPLE_ROUNDED_STELLATED				1106
#define TAG_RECTANGLE_SIMPLE_ROUNDED_STELLATED_REFORMED		1107
#define TAG_RECTANGLE_COMPLEX								1108	// defined
#define TAG_RECTANGLE_COMPLEX_REFORMED						1109
#define TAG_RECTANGLE_COMPLEX_STELLATED						1110
#define TAG_RECTANGLE_COMPLEX_STELLATED_REFORMED			1111
#define TAG_RECTANGLE_COMPLEX_ROUNDED						1112	// defined
#define TAG_RECTANGLE_COMPLEX_ROUNDED_REFORMED				1113
#define TAG_RECTANGLE_COMPLEX_ROUNDED_STELLATED				1114
#define TAG_RECTANGLE_COMPLEX_ROUNDED_STELLATED_REFORMED	1115

// Polygons
#define TAG_POLYGON_COMPLEX									1200	// defined
#define TAG_POLYGON_COMPLEX_REFORMED						1201	// defined
#define TAG_POLYGON_COMPLEX_STELLATED						1212	// defined
#define TAG_POLYGON_COMPLEX_STELLATED_REFORMED				1213	// defined
#define TAG_POLYGON_COMPLEX_ROUNDED							1214	// defined
#define TAG_POLYGON_COMPLEX_ROUNDED_REFORMED				1215	// defined
#define TAG_POLYGON_COMPLEX_ROUNDED_STELLATED				1216	// defined
#define TAG_POLYGON_COMPLEX_ROUNDED_STELLATED_REFORMED		1217

// General regular shapes
//
// (The first, TAG_REGULAR_SHAPE_PHASE was used briefly during development of version
// 2 of Camelot. It contains _all_ information about regular shapes, including the
// UTCentrePoint. The UTCentrePoint is not guaranteed to be DocCoord(0,0) for all
// regular shapes, but once this was not the case and a few files (on the CD!)
// are old enough to have regular shapes with UTCentrePoint != DocCoord(0,0).
// We still need to have this tag since some files may have been created which
// use it; some people get annoyed if files suddenly stop loading for no apparent reason.
// The exporting code has been removed, the importing code is in kernel/rechrshp.cpp
// The second tag below is now used for all regular shapes, any old regualar shapes
// get fixed to place UTCentrePoint at DocCoord(0,0), UTMajorAxis, UTMinorAxis and
// the TransformMatrix get altered accordingly, before saving. The code for fixing
// regular shapes is in kernel/cxfrgshp.cpp.
// Understand? - good, I'm not going to repeat myself.
//
// Ach

#define TAG_REGULAR_SHAPE_PHASE_1					1900
#define TAG_REGULAR_SHAPE_PHASE_2					1901

// Text related records
//
// Tags range 2000 - 2999 are reserved for text related records; this may change in the
// future but it will do for now.

// Text objects

// Text definitions
#define TAG_FONT_DEF_TRUETYPE						2000
#define TAG_FONT_DEF_ATM							2001

// vanilla text story objects
#define TAG_TEXT_STORY_SIMPLE						2100
#define TAG_TEXT_STORY_COMPLEX						2101

// text story objects on a path
#define TAG_TEXT_STORY_SIMPLE_START_LEFT			2110
#define TAG_TEXT_STORY_SIMPLE_START_RIGHT			2111
#define TAG_TEXT_STORY_SIMPLE_END_LEFT				2112
#define TAG_TEXT_STORY_SIMPLE_END_RIGHT				2113
#define TAG_TEXT_STORY_COMPLEX_START_LEFT			2114
#define TAG_TEXT_STORY_COMPLEX_START_RIGHT			2115
#define TAG_TEXT_STORY_COMPLEX_END_LEFT				2116
#define TAG_TEXT_STORY_COMPLEX_END_RIGHT			2117

// Text story information records
#define TAG_TEXT_STORY_WORD_WRAP_INFO				2150
#define TAG_TEXT_STORY_INDENT_INFO					2151

// other text story related objects
#define TAG_TEXT_LINE								2200
#define TAG_TEXT_STRING								2201
#define TAG_TEXT_CHAR								2202
#define TAG_TEXT_EOL								2203
#define TAG_TEXT_KERN								2204
#define TAG_TEXT_CARET								2205
#define TAG_TEXT_LINE_INFO							2206

// Text attributes
#define TAG_TEXT_LINESPACE_RATIO					2900
#define TAG_TEXT_LINESPACE_ABSOLUTE					2901
#define TAG_TEXT_JUSTIFICATION_LEFT					2902
#define TAG_TEXT_JUSTIFICATION_CENTRE				2903
#define TAG_TEXT_JUSTIFICATION_RIGHT				2904
#define TAG_TEXT_JUSTIFICATION_FULL					2905
#define TAG_TEXT_FONT_SIZE							2906
#define TAG_TEXT_FONT_TYPEFACE						2907
#define TAG_TEXT_BOLD_ON							2908
#define TAG_TEXT_BOLD_OFF							2909
#define TAG_TEXT_ITALIC_ON							2910
#define TAG_TEXT_ITALIC_OFF							2911
#define TAG_TEXT_UNDERLINE_ON						2912
#define TAG_TEXT_UNDERLINE_OFF						2913
#define TAG_TEXT_SCRIPT_ON							2914	// explicit offset, size
#define TAG_TEXT_SCRIPT_OFF							2915
#define TAG_TEXT_SUPERSCRIPT_ON						2916	// implicit offset, size
#define TAG_TEXT_SUBSCRIPT_ON						2917	// implicit offset, size
#define TAG_TEXT_TRACKING							2918
#define TAG_TEXT_ASPECT_RATIO						2919
#define TAG_TEXT_BASELINE							2920

											// Next	2921


//-----------------
// Imagesetting and other print-related tags - reserved range 3500 - 3599
// Imagesetting attributes
const UINT32 TAG_OVERPRINTLINEON						= 3500;
const UINT32 TAG_OVERPRINTLINEOFF					= 3501;
const UINT32 TAG_OVERPRINTFILLON						= 3502;
const UINT32 TAG_OVERPRINTFILLOFF					= 3503;
const UINT32 TAG_PRINTONALLPLATESON					= 3504;
const UINT32 TAG_PRINTONALLPLATESOFF					= 3505;
													// Full up
// Document Print/Imagesetting options
const UINT32 TAG_PRINTERSETTINGS						= 3506;
const UINT32 TAG_IMAGESETTING						= 3507;
const UINT32 TAG_COLOURPLATE							= 3508;
													// Full up
// Registration mark records
const UINT32 TAG_PRINTMARKDEFAULT					= 3509;
const UINT32 TAG_PRINTMARKCUSTOM						= 3510;
													// Next 3511
													// Last 3599


/********************************************
These have not been used in v1 of the file format, so therefore have been removed
//-----------------
// Transform attributes

// Misc transformations
#define TAG_TRANSFORM_IDENTITY			
#define TAG_TRANSFORM_MATRIX			
#define TAG_TRANSFORM_TRANSLATION		
#define TAG_TRANSFORM_GENERAL			

// Rotations
#define TAG_TRANSFORM_ROTATION			
#define TAG_TRANSFORM_ROTATION_CENTRE	
#define TAG_TRANSFORM_ROTATION_POINT	

// Shears
#define TAG_TRANSFORM_SHEAR_X			
#define TAG_TRANSFORM_SHEAR_Y			

// Scalings
#define TAG_TRANSFORM_SCALE				
#define TAG_TRANSFORM_SCALE_X			
#define TAG_TRANSFORM_SCALE_Y			

// Reflections
#define TAG_TRANSFORM_REFLECTION_X		
#define TAG_TRANSFORM_REFLECTION_Y		
**********************************************/



/////////////////////////////////////////////////////////////////////////////
// Camelot version 2.0 tags - range 4000-4999
/////////////////////////////////////////////////////////////////////////////

//-----------------
// Stroke Provider tags
const UINT32 TAG_VARIABLEWIDTHFUNC		= 4000;
const UINT32 TAG_VARIABLEWIDTHTABLE		= 4001;
const UINT32 TAG_STROKETYPE				= 4002;
const UINT32 TAG_STROKEDEFINITION		= 4003;
const UINT32 TAG_STROKEAIRBRUSH			= 4004;
											// Next 4005

// Next Free Tag = 4010 (please leave me some space for future expansion)

const UINT32 TAG_NOISEFILL				= 4010;
const UINT32 TAG_NOISETRANSPARENTFILL	= 4011;
const UINT32 TAG_MOULD_BOUNDS			= 4012;

// One moved and one new pathref tags
const UINT32 TAG_PATHREF_IDENTICAL		= 4013;
const UINT32 TAG_PATHREF_TRANSLATE		= 4014;
const UINT32 TAG_EXPORT_HINT				= 4015;
											// Next 4016

//-----------------
// New Web Address tags
const UINT32 TAG_WEBADDRESS				= 4020;
const UINT32 TAG_WEBADDRESS_BOUNDINGBOX	= 4021;
											// Next 4022

//-------------------
// Frmae Layer  tags
const UINT32 TAG_LAYER_FRAMEPROPS		= 4030;
const UINT32 TAG_SPREAD_ANIMPROPS		= 4031;

//-----------------
// Template Argument/Ops / WizOp / Webster Properties tags
const UINT32 TAG_WIZOP					= 4040;
const UINT32 TAG_WIZOP_STYLE				= 4041;
const UINT32 TAG_WIZOP_STYLEREF			= 4042;
											

/////////////////////////////////////////////////////////////////////////////
/// Camelot version 2.1 tags i.e. next version after 2.0
/////////////////////////////////////////////////////////////////////////////

//-----------------
// Shadows
const UINT32 TAG_SHADOWCONTROLLER		= 4050;
const UINT32 TAG_SHADOW					= 4051;

//-----------------
// Bevels
const UINT32 TAG_BEVEL					= 4052;

// variable tags
const UINT32 TAG_BEVATTR_INDENT			= 4053;
const UINT32 TAG_BEVATTR_LIGHTANGLE		= 4054;
const UINT32 TAG_BEVATTR_CONTRAST		= 4055;
const UINT32 TAG_BEVATTR_TYPE			= 4056;

// the bevel inking node tag
const UINT32 TAG_BEVELINK				= 4057;

//-----------------
// Blend on a curve tags
const UINT32 TAG_BLENDER_CURVEPROP		= 4060;
const UINT32 TAG_BLEND_PATH				= 4061;
const UINT32 TAG_BLENDER_CURVEANGLES		= 4062;

// Group Transparency Groups
const UINT32 TAG_GROUPTRANSP				= 4063;
const UINT32 TAG_CACHEBMP				= 4064;
const UINT32 TAG_CACHEDNODESGROUP		= 4065;

//-----------------
// Contouring tags
const UINT32 TAG_CONTOURCONTROLLER		= 4066;
const UINT32 TAG_CONTOUR					= 4067;

//-----------------
// Sets and their properties
const UINT32 TAG_SETSENTINEL				= 4070;
const UINT32 TAG_SETPROPERTY				= 4071;

// more Blended on a curve tags
const UINT32 TAG_BLENDPROFILES			= 4072;
const UINT32 TAG_BLENDERADDITIONAL		= 4073;
const UINT32 TAG_NODEBLENDPATH_FILLED	= 4074;

//-----------------
// Multi stage fill tags
const UINT32 TAG_LINEARFILLMULTISTAGE		= 4075;
const UINT32 TAG_CIRCULARFILLMULTISTAGE		= 4076;
const UINT32 TAG_ELLIPTICALFILLMULTISTAGE	= 4077;
const UINT32 TAG_CONICALFILLMULTISTAGE		= 4078;

//harmless test comment
//-----------------
// painting mode attribute tags
//const UINT32 TAG_PAINTINGMODE			= 4078;

// brush attribute tags
const UINT32 TAG_BRUSHATTR				= 4079;
const UINT32 TAG_BRUSHDEFINITION			= 4080;
const UINT32 TAG_BRUSHDATA				= 4081;
const UINT32 TAG_MOREBRUSHDATA			= 4082;
const UINT32 TAG_MOREBRUSHATTR			= 4083;


//-----------------
// ClipView tags
const UINT32 TAG_CLIPVIEWCONTROLLER		= 4084;
const UINT32 TAG_CLIPVIEW				= 4085;

//-----------------
// Feathering tags
const UINT32 TAG_FEATHER					= 4086;

//-----------------
// Bar properties
const UINT32 TAG_BARPROPERTY				= 4087;

//-----------------
// Other multi stage fill tags
const UINT32 TAG_SQUAREFILLMULTISTAGE	= 4088;

//-----------------
// Tags for saving current attributes. Currently unused.
// const UINT32 TAG_STARTCURRENTATTR		= 4100;
// const UINT32 TAG_ENDCURRENTATTR		= 4101;

// more brush tags
const UINT32 TAG_EVENMOREBRUSHDATA		= 4102;
const UINT32 TAG_EVENMOREBRUSHATTR		= 4103;
const UINT32	TAG_TIMESTAMPBRUSHDATA		= 4104;
const UINT32 TAG_BRUSHPRESSUREINFO		= 4105;
const UINT32 TAG_BRUSHPRESSUREDATA		= 4106;
const UINT32 TAG_BRUSHATTRPRESSUREINFO	= 4107;
const UINT32 TAG_BRUSHCOLOURDATA			= 4108;
const UINT32 TAG_BRUSHPRESSURESAMPLEDATA	= 4109;
const UINT32 TAG_BRUSHTIMESAMPLEDATA		= 4110;
const UINT32 TAG_BRUSHATTRFILLFLAGS		= 4111;
const UINT32 TAG_BRUSHTRANSPINFO			= 4112;
const UINT32 TAG_BRUSHATTRTRANSPINFO		= 4113;

// the nudge size is now a part of the document
const UINT32 TAG_DOCUMENTNUDGE			= 4114;

// bitmap properties - stores the settings in the b.p. dialogue for each bitmap
const UINT32 TAG_BITMAP_PROPERTIES		= 4115;
const UINT32 TAG_DOCUMENTBITMAPSMOOTHING	= 4116;

// Extended bitmap processing data generated by XPE
const UINT32 TAG_XPE_BITMAP_PROPERTIES	= 4117;

// Bitmap file format placeholder when bitmap data is generated on the fly from master and XPE info
const UINT32 TAG_DEFINEBITMAP_XPE		= 4118;

// Current Attributes container and geometry bounds
const UINT32 TAG_CURRENTATTRIBUTES		= 4119;
const UINT32 TAG_CURRENTATTRIBUTEBOUNDS	= 4120;

// New 3-point linear fill records
const UINT32 TAG_LINEARFILL3POINT			= 4121;
const UINT32 TAG_LINEARFILLMULTISTAGE3POINT	= 4122;
const UINT32 TAG_LINEARTRANSPARENTFILL3POINT	= 4123;

// New Duplication distance records
const UINT32 TAG_DUPLICATIONOFFSET		= 4124;

// Live Effect node tags
const UINT32 TAG_LIVE_EFFECT				= 4125;
const UINT32 TAG_LOCKED_EFFECT			= 4126;
const UINT32 TAG_FEATHER_EFFECT			= 4127;

// Compound group rendering stream warning (e.g. transparent groups, live effects, etc.)
const UINT32 TAG_COMPOUNDRENDER			= 4128;

const UINT32 TAG_OBJECTBOUNDS			= 4129;

/////////////////////////////////////////////////////////////////////////////
// TAG RANGES
// Add new tags just before this comment
// The current range for tags is 4000-4999. DO NOT define tags with values
// outside this range. Ask Mark Neves if you need to extend the range or
// do anything else unusual.
/////////////////////////////////////////////////////////////////////////////

#endif
